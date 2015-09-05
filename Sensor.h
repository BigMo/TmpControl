#ifndef __SENSOR__
#define __SENSOR__

#include <DallasTemperature.h>

class Sensor
{
	public:
		Sensor(byte* p_pAddress, char* p_szName);
		virtual void Update(DallasTemperature* sensors);
		float GetCurrentTemperature();
		float GetMaxTemperature();
		char* GetName();
	private:
		byte m_pAddress[8];
	protected:
		char m_szName[20];
		float m_fTempCurrent;
		float m_fTempMax;
};
#endif