#include "Sensor.h"

Sensor::Sensor(byte* p_pAddress, char* p_szName)
{
	memcpy(this->m_pAddress, p_pAddress, 8);
	strcpy(this->m_szName, p_szName);
	this->m_fTempCurrent = 0.0f;
	this->m_fTempMax = 0.0f;
}

void Sensor::Update(DallasTemperature* sensors)
{
	if (sensors->validAddress(this->m_pAddress) && sensors->isConnected(this->m_pAddress))
	{
		this->m_fTempCurrent = sensors->getTempC(this->m_pAddress);
		if (this->m_fTempCurrent > this->m_fTempMax)
			this->m_fTempMax = this->m_fTempCurrent;
	}
}

float Sensor::GetCurrentTemperature()
{
	return this->m_fTempCurrent;
}

float Sensor::GetMaxTemperature()
{
	return this->m_fTempMax;
}

char* Sensor::GetName()
{
	return this->m_szName;
}