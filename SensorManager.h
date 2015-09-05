#ifndef __SENSOR_MANAGER__
#define __SENSOR_MANAGER__

#include <DallasTemperature.h>
#include "Sensor.h"
#include "SensorContainer.h"
class SensorManager : public SensorContainer
{
	public:
		void Update(DallasTemperature* sensors);
		
		float GetCurrentTemperature(int p_iIndex);
		float GetMaxTemperature(int p_iIndex);
		char* GetName(int p_iIndex);
};
#endif