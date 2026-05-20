#include "ARINC429AnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "ARINC429Analyzer.h"
#include "ARINC429AnalyzerSettings.h"
#include <iostream>
#include <fstream>

ARINC429AnalyzerResults::ARINC429AnalyzerResults( ARINC429Analyzer* analyzer, ARINC429AnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

ARINC429AnalyzerResults::~ARINC429AnalyzerResults()
{
}

void ARINC429AnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

	switch( frame.mFlags )
    {
    case ARINC429_MFLAGS_LABEL:
		AddResultString( "Label: ", number_str );
		break;
    case ARINC429_MFLAGS_SDI:
		AddResultString( "SDI: ", number_str );
		break;
    case ARINC429_MFLAGS_DATA:
		AddResultString( "DATA: ", number_str );
		break;
    case ARINC429_MFLAGS_SSM:
		AddResultString( "SSM: ", number_str );
		break;
    case ARINC429_MFLAGS_PARITY:
		AddResultString( "PARITY: ", number_str );
		break;
	}
}

void ARINC429AnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void ARINC429AnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void ARINC429AnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void ARINC429AnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}