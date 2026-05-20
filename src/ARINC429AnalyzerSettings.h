#ifndef ARINC429_ANALYZER_SETTINGS
#define ARINC429_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class ARINC429AnalyzerSettings : public AnalyzerSettings
{
public:
	ARINC429AnalyzerSettings();
	virtual ~ARINC429AnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mA429InputPositive;
	Channel mA429InputNegative;
	U32 mA429DataRate;

protected:
	AnalyzerSettingInterfaceChannel	mA429InputPositiveInterface;
	AnalyzerSettingInterfaceChannel	mA429InputNegativeInterface;
    AnalyzerSettingInterfaceNumberList mA429DataRateInterface;
};

#endif //ARINC429_ANALYZER_SETTINGS
