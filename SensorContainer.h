#ifndef __SENSOR_CONTAINER__
#define __SENSOR_CONTAINER__

#include "Sensor.h"
#define SC_MAX_SENSORS 16
class SensorContainer
{
	public:
		bool AddSensor(Sensor* sensor);
		int GetNumberOfSensors();
	protected:
		int m_iNumSensors;
		Sensor* m_ppSensors[SC_MAX_SENSORS];
};
#endif