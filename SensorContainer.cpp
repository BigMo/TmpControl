#include "SensorContainer.h"
bool SensorContainer::AddSensor(Sensor* sensor)
{
	if (this->m_iNumSensors < SC_MAX_SENSORS - 1)
	{
		this->m_ppSensors[this->m_iNumSensors++] = sensor;
		return true;
	}
	return false;
}

int SensorContainer::GetNumberOfSensors()
{
	return this->m_iNumSensors;
}