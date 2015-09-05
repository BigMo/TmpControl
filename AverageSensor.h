#ifndef __AVERAGE_SENSOR__
#define __AVERAGE_SENSOR__

#include "Sensor.h"
#include "SensorContainer.h"
class AverageSensor : public Sensor, public SensorContainer
{
	public:
		AverageSensor(char* p_szName);
		void Update(DallasTemperature* sensors);
};
#endif