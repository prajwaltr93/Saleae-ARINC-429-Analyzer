#include "ARINC429AnalyzerSettings.h"
#include <AnalyzerHelpers.h>


ARINC429AnalyzerSettings::ARINC429AnalyzerSettings()
:   mA429InputPositive( UNDEFINED_CHANNEL ),
	mA429InputNegative( UNDEFINED_CHANNEL ),
	mA429DataRate( 50 ),
	mA429InputNegativeInterface(),
	mA429InputPositiveInterface(),
	mA429DataRateInterface()
{
	mA429InputPositiveInterface.SetTitleAndTooltip( "ARINC 429 Positive Line", "A429 Differential Pair Positive Line." );
	mA429InputPositiveInterface.SetChannel( mA429InputPositive );

	mA429InputNegativeInterface.SetTitleAndTooltip( "ARINC 429 Postive Line", "A429 Differential Pair Negative Line." );
	mA429InputNegativeInterface.SetChannel( mA429InputPositive );

	mA429DataRateInterface.SetTitleAndTooltip( "ARINC 429 Data Rate",  "Specify the Data Rate of ARINC 429 in KHz." );
    mA429DataRateInterface.AddNumber( 12.5, "12.5 KHz", "12.5 KHz" );
    mA429DataRateInterface.AddNumber( 50, "50 KHz", "50 KHz" );
    mA429DataRateInterface.AddNumber( 100, "100 KHz", "100 KHz" );

	AddInterface( &mA429InputPositiveInterface );
	AddInterface( &mA429InputNegativeInterface );
	AddInterface( &mA429DataRateInterface );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mA429InputPositive, "ARINC 429 Positive Line", false );
	AddChannel( mA429InputPositive, "ARINC 429 Negative Line", false );
}

ARINC429AnalyzerSettings::~ARINC429AnalyzerSettings()
{
}

bool ARINC429AnalyzerSettings::SetSettingsFromInterfaces()
{
	mA429InputPositive = mA429InputPositiveInterface.GetChannel();
	mA429InputNegative = mA429InputNegativeInterface.GetChannel();
	mA429DataRate = mA429DataRateInterface.GetNumber();

	ClearChannels();
	AddChannel( mA429InputPositive, "ARINC 429 Positive Line", true );
	AddChannel( mA429InputPositive, "ARINC 429 Negative Line", true );

	return true;
}

void ARINC429AnalyzerSettings::UpdateInterfacesFromSettings()
{
	mA429InputPositiveInterface.SetChannel( mA429InputPositive );
	mA429InputNegativeInterface.SetChannel( mA429InputNegative );
	mA429DataRateInterface.SetNumber( mA429DataRate );
}

void ARINC429AnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mA429InputPositive;
	text_archive >> mA429InputNegative;
	text_archive >> mA429DataRate;

	ClearChannels();
	AddChannel( mA429InputPositive, "ARINC 429 Positive Line", true );
	AddChannel( mA429InputPositive, "ARINC 429 Negative Line", true );

	UpdateInterfacesFromSettings();
}

const char* ARINC429AnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mA429InputPositive;
	text_archive << mA429InputNegative;
	text_archive << mA429DataRate;

	return SetReturnString( text_archive.GetString() );
}
