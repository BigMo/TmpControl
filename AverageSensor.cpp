#include "AverageSensor.h"

AverageSensor::AverageSensor(char* p_szName) : Sensor(0, p_szName)
{
	
}

void AverageSensor::Update(DallasTemperature* sensors)
{
	float temp = 0;
	for(int i = 0; i < this->m_iNumSensors; i++)
	{
		temp += this->m_ppSensors[i]->GetCurrentTemperature();
	}
	this->m_fTempCurrent = temp / m_iNumSensors;
	
	if (this->m_fTempCurrent > this->m_fTempMax)
		this->m_fTempMax = this->m_fTempCurrent;
}