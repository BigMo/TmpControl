#include "SensorManager.h"

void SensorManager::Update(DallasTemperature* sensors)
{
	for(int i = 0; i < this->m_iNumSensors; i++)
	{
		this->m_ppSensors[i]->Update(sensors);
	}
}

float SensorManager::GetCurrentTemperature(int p_iIndex)
{
	if (p_iIndex < m_iNumSensors)
		return m_ppSensors[p_iIndex]->GetCurrentTemperature();
	return 0.0f;
}

float SensorManager::GetMaxTemperature(int p_iIndex)
{
	if (p_iIndex < m_iNumSensors)
		return m_ppSensors[p_iIndex]->GetMaxTemperature();
	return 0.0f;
}

char* SensorManager::GetName(int p_iIndex)
{
	if (p_iIndex < m_iNumSensors)
		return m_ppSensors[p_iIndex]->GetName();
	return "NOT FOUND";
}