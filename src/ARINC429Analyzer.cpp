#include "ARINC429Analyzer.h"
#include "ARINC429AnalyzerSettings.h"
#include <AnalyzerChannelData.h>

ARINC429Analyzer::ARINC429Analyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
    UseFrameV2();
}

ARINC429Analyzer::~ARINC429Analyzer()
{
	KillThread();
}

void ARINC429Analyzer::SetupResults()
{
	// SetupResults is called each time the analyzer is run. Because the same instance can be used for multiple runs, we need to clear the results each time.
	mResults.reset(new ARINC429AnalyzerResults( this, &mSettings ));
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings.mA429InputPositive );
	mResults->AddChannelBubblesWillAppearOn( mSettings.mA429InputNegative );
}

void ARINC429Analyzer::WorkerThread()
{
	U32 sample_rate_hz = GetSampleRate();
    U64 starting_sample_number = 0U, ending_sample_number = 0U;

	mA429PositiveChannelData = GetAnalyzerChannelData( mSettings.mA429InputPositive );
	mA429NegativeChannelData = GetAnalyzerChannelData( mSettings.mA429InputNegative );

	starting_sample_number = mA429PositiveChannelData->GetSampleOfNextEdge();
    if( starting_sample_number > mA429NegativeChannelData->GetSampleOfNextEdge() )
    {
		starting_sample_number = mA429NegativeChannelData->GetSampleOfNextEdge();
	}

	mA429NegativeChannelData->AdvanceToAbsPosition(starting_sample_number);
	mA429PositiveChannelData->AdvanceToAbsPosition(starting_sample_number);
    
	U32 samples_per_half_cycle = sample_rate_hz / (mSettings.mA429DataRate * 1000U); // Kilo Hz, / 2 is to arrive at half bit time

	/* first move only half a cycle to get to mid point of first data bit */
	mA429PositiveChannelData->Advance( samples_per_half_cycle / 2U);
	mA429NegativeChannelData->Advance( samples_per_half_cycle / 2U);

	for( ; ; )
	{
		U8 label = 0U, SSM = 0U, SDI = 0U, parity = 0U;
        U32 data = 0U;

		for( U32 i=0U; i<32U; i++ )
		{
			mResults->AddMarker( mA429PositiveChannelData->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mA429InputPositive );
			mResults->AddMarker( mA429NegativeChannelData->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mA429InputNegative );

			if( (mA429PositiveChannelData->GetBitState() == BIT_HIGH)  && (mA429NegativeChannelData->GetBitState() == BIT_LOW))
            {
                data |= ( 1U << ( 31 - i ) );
            }
            else
            {
                /* TODO: This is an ERROR, not a differential PAIR ! */
			}
		
			ending_sample_number = mA429PositiveChannelData->GetSampleNumber();

			/* Label || SDI || Data || SSM || Parity */
			if( i == 7 || i == 9 || i == 28 || i == 30 || i == 31 )
            {
				//we have a byte to save. 
				Frame frame;
                FrameV2 frame_v2;
				frame.mFlags = 0;
				frame.mStartingSampleInclusive = starting_sample_number;
				/* TODO: not very accurate, fix this ! */
				frame.mEndingSampleInclusive = ending_sample_number + (samples_per_half_cycle / 15U); // Both channels have advanced to end position of current ARINC word, can use any one of them.
				starting_sample_number = frame.mEndingSampleInclusive;

				switch( i )
                {
                case 7U:
                    frame.mFlags |= ARINC429_MFLAGS_LABEL;
                    label = ( data & 0xFF000000U ) >> 24;
					frame.mData1 = label;
					frame_v2.AddInteger( "Data", label);
					mResults->AddFrameV2( frame_v2, "Label", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive);
                    break;
                case 9U:
                    frame.mFlags |= ARINC429_MFLAGS_SDI;
                    SDI = ( (data & 0x00C00000U) >> 22);
					frame.mData1 = SDI;
					frame_v2.AddInteger( "Data", SDI );
					mResults->AddFrameV2( frame_v2, "SDI", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive);
                    break;
                case 28U:
                    frame.mFlags |= ARINC429_MFLAGS_DATA;
					frame.mData1 = (U32)((data & 0x003FFFF8U) >> 3);
					frame_v2.AddInteger( "Data", frame.mData1 );
					mResults->AddFrameV2( frame_v2, "Data", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive);
                    break;
                case 30U:
                    frame.mFlags |= ARINC429_MFLAGS_SSM;
                    SSM = ( (data & 0x00000006U) >> 1 );
					frame.mData1 = SSM;
					frame_v2.AddInteger( "Data", SSM );
					mResults->AddFrameV2( frame_v2, "SSM", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive);
                    break;
                case 31U:
                    frame.mFlags |= ARINC429_MFLAGS_PARITY;
                    parity = ( data & 0x00000001U );
					frame.mData1 = parity;
					frame_v2.AddInteger( "Data", parity );
					mResults->AddFrameV2( frame_v2, "Parity", frame.mStartingSampleInclusive, frame.mEndingSampleInclusive);
                    break;
				}


				mResults->AddFrame( frame );
				mResults->CommitResults();
				ReportProgress( frame.mEndingSampleInclusive );
			}

			mA429PositiveChannelData->Advance( samples_per_half_cycle );
			mA429NegativeChannelData->Advance( samples_per_half_cycle );
		}

		/* Reset Data */
		data = 0U;

		/* Scheduling Rate can be anything, although a minimum of 4 bit times is considered valid */
        /* TODO: revisit this idea */
		/* Move to next bit so we can advance to LOW and then find the next EDGE */
		mA429PositiveChannelData->Advance( samples_per_half_cycle );
		mA429NegativeChannelData->Advance( samples_per_half_cycle );

		starting_sample_number = mA429PositiveChannelData->GetSampleOfNextEdge();
		if( starting_sample_number > mA429NegativeChannelData->GetSampleOfNextEdge() )
		{
			starting_sample_number = mA429NegativeChannelData->GetSampleOfNextEdge();
		}

		mA429NegativeChannelData->AdvanceToAbsPosition(starting_sample_number);
		mA429PositiveChannelData->AdvanceToAbsPosition(starting_sample_number);

		/* first move only half a cycle to get to mid point of first data bit */
		mA429PositiveChannelData->Advance( samples_per_half_cycle / 2U);
		mA429NegativeChannelData->Advance( samples_per_half_cycle / 2U);
	}
}

bool ARINC429Analyzer::NeedsRerun()
{
	return false;
}

U32 ARINC429Analyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 ARINC429Analyzer::GetMinimumSampleRateHz()
{
	return mSettings.mA429DataRate * 4;
}

const char* ARINC429Analyzer::GetAnalyzerName() const
{
	return "ARINC 429";
}

const char* GetAnalyzerName()
{
	return "ARINC 429";
}

Analyzer* CreateAnalyzer()
{
	return new ARINC429Analyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}