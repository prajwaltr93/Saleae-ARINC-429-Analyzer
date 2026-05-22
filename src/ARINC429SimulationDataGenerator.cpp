#include "ARINC429SimulationDataGenerator.h"
#include "ARINC429AnalyzerSettings.h"

#include <AnalyzerHelpers.h>

ARINC429SimulationDataGenerator::ARINC429SimulationDataGenerator() : mA429DataWord(0xDEADBEEFU)
{

}

ARINC429SimulationDataGenerator::~ARINC429SimulationDataGenerator()
{
}

void ARINC429SimulationDataGenerator::Initialize( U32 simulation_sample_rate, ARINC429AnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mA429SimulationDataPositive = mA429SimulationDataGroup.Add( mSettings->mA429InputPositive, mSimulationSampleRateHz, BIT_LOW );
	mA429SimulationDataNegative = mA429SimulationDataGroup.Add( mSettings->mA429InputNegative, mSimulationSampleRateHz, BIT_LOW );
}

U32 ARINC429SimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	/* we will be moving both input channels by same steps */
	while( mA429SimulationDataPositive->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		this->CreateA429SimulationData();
	}

	*simulation_channel = mA429SimulationDataGroup.GetArray();
	return mA429SimulationDataGroup.GetCount();
}

void ARINC429SimulationDataGenerator::CreateA429SimulationData()
{
	U32 samples_per_bit = mSimulationSampleRateHz / (mSettings->mA429DataRate * 1000U);
    U8 parity_count = 0U;

	// let's move forward a little, 4 bit times as per the standard that is minimum
	mA429SimulationDataPositive->Advance( samples_per_bit * 4U ); 
	mA429SimulationDataNegative->Advance( samples_per_bit * 4U );

	/* Generate Wave form */
	for( U8 i = 0; i < 31U; i++ )
    {
        /* Rise Time is near instantaneous, negligible considering threshold is 1.2V */

		//mA429SimulationDataPositive->Advance( samples_per_bit * 0.01 * 0.5 ); 
		//mA429SimulationDataNegative->Advance( samples_per_bit * 0.01 * 0.5 );

        /* start from Label, SDI, Data, SSM */
        if( mA429DataWord & ( 1U << ( 31 - i ) ) )
        {
            mA429SimulationDataPositive->Transition();
            parity_count += 1U; 
		}
        else
        {
            mA429SimulationDataNegative->Transition();
		}

		/* High time is larger than low time because falling edge past half bit time takes time */
        /* to register as 0 since threshold is 1.2v in saleae and is not configurable */
		mA429SimulationDataPositive->Advance( samples_per_bit * 0.55); 
		mA429SimulationDataNegative->Advance( samples_per_bit * 0.55);

        if( mA429DataWord & ( 1U << ( 31 - i ) ) )
        {
            mA429SimulationDataPositive->Transition();
		}
        else
        {
            mA429SimulationDataNegative->Transition();
		}

		mA429SimulationDataPositive->Advance( samples_per_bit * 0.45 );
		mA429SimulationDataNegative->Advance( samples_per_bit * 0.45 );
	}

	/* TODO: once parity is settable, figure what parity to set to */
    /* using ODD for now ＼（〇_ｏ）／ */
	/* figure out parity and appropriate waveform */
    if( parity_count & ( 1U << 0 ) )
    {
        /* ODD number */
		mA429SimulationDataPositive->Transition();

		mA429SimulationDataPositive->Advance( samples_per_bit * 0.55); 
		mA429SimulationDataNegative->Advance( samples_per_bit * 0.55);
		mA429SimulationDataPositive->Transition();

		mA429SimulationDataPositive->Advance( samples_per_bit * 0.45 );
		mA429SimulationDataNegative->Advance( samples_per_bit * 0.45 );
	} 
	else
    {
        /* Even Number */
		mA429SimulationDataPositive->Advance( samples_per_bit );
		mA429SimulationDataNegative->Advance( samples_per_bit );
	}

	mA429DataWord += 1U; /* un-signed integer arithmetic says it will roll over once over-flow happens, so we good :^) */
}
