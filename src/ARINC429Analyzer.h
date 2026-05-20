#ifndef ARINC429_ANALYZER_H
#define ARINC429_ANALYZER_H

#include <Analyzer.h>
#include "ARINC429AnalyzerSettings.h"
#include "ARINC429AnalyzerResults.h"
#include "ARINC429SimulationDataGenerator.h"
#include <memory>

#define ARINC429_MFLAGS_LABEL    	(1U << 0)
#define ARINC429_MFLAGS_SDI			(1U << 1)
#define ARINC429_MFLAGS_DATA		(1U << 2)
#define ARINC429_MFLAGS_SSM			(1U << 3)
#define ARINC429_MFLAGS_PARITY      (1U << 4)

class ANALYZER_EXPORT ARINC429Analyzer : public Analyzer2
{
public:
	ARINC429Analyzer();
	virtual ~ARINC429Analyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	ARINC429AnalyzerSettings mSettings;
	std::unique_ptr<ARINC429AnalyzerResults> mResults;
	AnalyzerChannelData* mA429PositiveChannelData;
	AnalyzerChannelData* mA429NegativeChannelData;

	ARINC429SimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //ARINC429_ANALYZER_H
