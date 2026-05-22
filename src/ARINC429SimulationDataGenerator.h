#ifndef ARINC429_SIMULATION_DATA_GENERATOR
#define ARINC429_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class ARINC429AnalyzerSettings;

class ARINC429SimulationDataGenerator
{
public:
	ARINC429SimulationDataGenerator();
	~ARINC429SimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, ARINC429AnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	ARINC429AnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;
    U32 mA429DataWord; /* start with some arbitrary number so it looks nice :) */

protected:
	void CreateA429SimulationData(void);
	SimulationChannelDescriptor *mA429SimulationDataPositive;
	SimulationChannelDescriptor *mA429SimulationDataNegative;
    SimulationChannelDescriptorGroup mA429SimulationDataGroup;

};
#endif //ARINC429_SIMULATION_DATA_GENERATOR