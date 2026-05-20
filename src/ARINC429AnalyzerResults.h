#ifndef ARINC429_ANALYZER_RESULTS
#define ARINC429_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class ARINC429Analyzer;
class ARINC429AnalyzerSettings;

class ARINC429AnalyzerResults : public AnalyzerResults
{
public:
	ARINC429AnalyzerResults( ARINC429Analyzer* analyzer, ARINC429AnalyzerSettings* settings );
	virtual ~ARINC429AnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	ARINC429AnalyzerSettings* mSettings;
	ARINC429Analyzer* mAnalyzer;
};

#endif //ARINC429_ANALYZER_RESULTS
