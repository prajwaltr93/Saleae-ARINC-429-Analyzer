#include "ARINC429Analyzer.h"
#include "ARINC429AnalyzerSettings.h"
#include <AnalyzerChannelData.h>

ARINC429Analyzer::ARINC429Analyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
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
    U64 first_sample_number = 0U;

	mA429PositiveChannelData = GetAnalyzerChannelData( mSettings.mA429InputPositive );
	mA429NegativeChannelData = GetAnalyzerChannelData( mSettings.mA429InputNegative );

	if( mA429PositiveChannelData->GetBitState() == BIT_LOW )
		mA429PositiveChannelData->AdvanceToNextEdge();

	first_sample_number = mA429PositiveChannelData->GetSampleNumber();

	if( mA429NegativeChannelData->GetBitState() == BIT_LOW )
		mA429NegativeChannelData->AdvanceToNextEdge();

	if( first_sample_number < mA429NegativeChannelData->GetSampleNumber() )
    {
        /* we have moved too far on negative channel, rolling back to first edge on positive channel */
        mA429NegativeChannelData->AdvanceToAbsPosition(first_sample_number);
	}
    else
    {
        /* we have moved too far on positive channel, rolling back to first edge on negative channel */
        mA429PositiveChannelData->AdvanceToAbsPosition(mA429NegativeChannelData->GetSampleNumber());
	}

	U32 samples_per_half_cycle = sample_rate_hz / (mSettings.mA429DataRate * 1000U); // Kilo Hz, / 2 is to arrive at half bit time

	/* first move only half a cycle to get to mid point of first data bit */
	mA429PositiveChannelData->Advance( samples_per_half_cycle / 2U);
	mA429NegativeChannelData->Advance( samples_per_half_cycle / 2U);

	for( ; ; )
	{
		U32 data = 0;
		U8 mask = 1 << 7;

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
		
			mA429PositiveChannelData->Advance( samples_per_half_cycle );
			mA429NegativeChannelData->Advance( samples_per_half_cycle );
		}

		//we have a byte to save. 
		Frame frame;
		frame.mData1 = data;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = first_sample_number;
		frame.mEndingSampleInclusive = mA429PositiveChannelData->GetSampleNumber();

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
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