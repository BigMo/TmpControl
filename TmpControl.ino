#include <Time.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Button.h"
#include "SensorManager.h"
#include "Sensor.h"
#include "AverageSensor.h"
//---------------------------------------------
// VARIABLES
//---------------------------------------------
//Alignment
const int LEFT = 0, CENTER = 1, RIGHT = 3;
//LCD-"driver"
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//OneWire
OneWire oneWire(10);
//Dallas Temperature Lib
DallasTemperature sensors(&oneWire);
//Show uptime?
bool bShowUptime = false;
//"Previous"-button down?
bool bButtonPrev = false;
//"Next"-button down?
bool bButtonNext = false;
Button btnButtonPrev;
Button btnButtonNext;
//Sensor-names
char *szSensors[6] = { "CPU", "GPU", "PSU", "Case (front)", "Case (back)", "Case (top)" };
byte ppSensorAddresses[11][8] {
	{ 0x28, 0xFF, 0xC7, 0x19, 0x51, 0x15, 0x03, 0xB1 },
	{ 0x28, 0xFF, 0x7A, 0xB0, 0x50, 0x15, 0x02, 0xDD },
	{ 0x28, 0xFF, 0x87, 0x1A, 0x51, 0x15, 0x03, 0xD0 },
	{ 0x28, 0xFF, 0xBB, 0xD9, 0x50, 0x15, 0x03, 0x6F },
	{ 0x28, 0xFF, 0x91, 0x01, 0x21, 0x15, 0x03, 0x9C },
	{ 0x28, 0xFF, 0xD6, 0x39, 0x51, 0x15, 0x03, 0x70 },
	{ 0x28, 0xFF, 0xB7, 0xB3, 0x50, 0x15, 0x02, 0x9B },
	{ 0x28, 0xFF, 0xF7, 0x1D, 0x51, 0x15, 0x03, 0x3B },
	{ 0x28, 0xFF, 0xDC, 0x02, 0x51, 0x15, 0x03, 0x11 },
	{ 0x28, 0xFF, 0xD0, 0x1B, 0x51, 0x15, 0x03, 0x9B },
	{ 0x28, 0xFF, 0x85, 0x38, 0x51, 0x15, 0x03, 0x24 }
};
//Sensors
Sensor
	m_SensorCPU(ppSensorAddresses[0], "CPU"),
	m_SensorGPU(ppSensorAddresses[1], "GPU"),
	m_SensorPSU(ppSensorAddresses[2], "PSU"),
	m_SensorCaseFront(ppSensorAddresses[3], "Case (front)"),
	m_SensorCaseBack(ppSensorAddresses[6], "Case (back)"),
	m_SensorCaseTop(ppSensorAddresses[5], "Case (top)");
AverageSensor m_SensorAverage("Average");
//Sensor-manager
SensorManager m_SensorManager;
//Number of sensors
unsigned short iNumSensors = 6;
//Current sensor displayed
unsigned short iCurrentSensor = 0;
//Display-lines
char szDisplay[5][20];
//Min/max temperatures
float fTemperatues[6][2];
//Keep track of updates (sensor and ui)
unsigned short iLastSec = 0;
//Keep track of last input
unsigned short iLastInputSec = 0;
//Keep track of last sensor-scrolling
unsigned short iLastSensorsScrollSec = 0;
//Update-interval of the sensors
unsigned short iSensorsUpdate = 10;
//Auto-scrolling-interval
unsigned short iSensorsScroll = 30;
//Delay between auto-scrolling and input
unsigned short iSensorsScrollDelay = 10;
//Update UI this tick?
bool bUpdateUI = false;

//---------------------------------------------
// METHODS
//---------------------------------------------
void setup() {
	setTime(0);
	//Set up pins
	btnButtonNext.SetUp(13);
	btnButtonPrev.SetUp(12);
	//Set up LCD
	lcd.begin(20, 4);
	//Set up sensors
	sensors.begin();
	sensors.setResolution(TEMP_12_BIT);
	AddSensor(&m_SensorManager, &m_SensorCPU);
	AddSensor(&m_SensorManager, &m_SensorGPU);
	AddSensor(&m_SensorManager, &m_SensorPSU);
	AddSensor(&m_SensorManager, &m_SensorCaseFront);
	AddSensor(&m_SensorManager, &m_SensorCaseBack);
	AddSensor(&m_SensorManager, &m_SensorCaseTop);
	AddSensor(&m_SensorManager, &m_SensorAverage);
	
	AddSensor(&m_SensorAverage, &m_SensorCPU);
	AddSensor(&m_SensorAverage, &m_SensorGPU);
	AddSensor(&m_SensorAverage, &m_SensorPSU);
	AddSensor(&m_SensorAverage, &m_SensorCaseFront);
	AddSensor(&m_SensorAverage, &m_SensorCaseBack);
	AddSensor(&m_SensorAverage, &m_SensorCaseTop);
	//Set up empty display-line
	sprintf(szDisplay[4], "                    ");
	ShowIntro();
	UpdateSensors();
	lcd.clear();
}

void loop() 
{
	bUpdateUI = false;
	unsigned short sec = millis() / 1000;
	
	//---------------------------------------------
	// Process input
	//---------------------------------------------
	btnButtonNext.Update();
	btnButtonPrev.Update();
	
	if (btnButtonNext.Changed() || btnButtonPrev.Changed())
		iLastInputSec = sec;
	
	//Are both buttons down? (one is down, the other one went down)
	if ((btnButtonNext.IsDown() && btnButtonPrev.WentDown()) || (btnButtonNext.WentDown() && btnButtonPrev.IsDown()))
	{
		//Toggle uptime-display
		bShowUptime = !bShowUptime;
	} 
	else 
	{
		//Navigate
		if (btnButtonNext.WentDown())
		{
			//Forwards
			iCurrentSensor++;
			iCurrentSensor %= m_SensorManager.GetNumberOfSensors();
			bUpdateUI = true;
		}
		if (btnButtonPrev.WentDown())
		{
			//Backwards
			iCurrentSensor--;
			iCurrentSensor += m_SensorManager.GetNumberOfSensors();
			iCurrentSensor %= m_SensorManager.GetNumberOfSensors();
			bUpdateUI = true;
		}
	}
	
	//---------------------------------------------
	// Auto-scroll
	//---------------------------------------------
	if (sec - iLastInputSec > iSensorsScrollDelay && (sec - iLastInputSec) % iSensorsScroll == 0 && sec != iLastSensorsScrollSec)
	{
		iCurrentSensor++;
		iCurrentSensor %= m_SensorManager.GetNumberOfSensors();
		iLastSensorsScrollSec = sec;
		bUpdateUI = true;
	}
	
	//---------------------------------------------
	// Time-based actions
	//---------------------------------------------
	if (sec > iLastSec)
	{
		//Update UI every second
		bUpdateUI = true;
	}
	
	//---------------------------------------------
	// Update
	//---------------------------------------------
	//Update UI if needed
	if (bUpdateUI)
		UpdateUI();
	
	if (sec > iLastSec && sec % iSensorsUpdate == 0)
		UpdateSensors();
	
	iLastSec = sec;
	//Update 20 times a sec
	delay(50);
}

void AddSensor(SensorContainer* container, Sensor *sensor)
{
	if (!container->AddSensor(sensor))
	{
		PrintAligned("Failed to add", CENTER, 0, 0);
		PrintAligned(sensor->GetName(), CENTER, 0, 1);
		delay(3000);
		lcd.clear();
	}
}

void UpdateUI()
{
	//---------------------------------------------
	// Prepare
	//---------------------------------------------
	//Prepare uptime
	if (bShowUptime)
		PrintUptime(szDisplay[0]);

	//Prepare current sensor name
	sprintf(szDisplay[1], "[%s]", m_SensorManager.GetName(iCurrentSensor));

	//Prepare temperatures
	char curr[8], max[8];
	ConvertFromFloat(curr, m_SensorManager.GetCurrentTemperature(iCurrentSensor));
	ConvertFromFloat(max, m_SensorManager.GetMaxTemperature(iCurrentSensor));
	sprintf(szDisplay[2], "%sC (max %sC)", curr, max);

	//Prepare indices
	sprintf(szDisplay[3], "(%i/%i)", iCurrentSensor + 1, m_SensorManager.GetNumberOfSensors());

	//---------------------------------------------
	// Print
	//---------------------------------------------
	//Line #1 - Uptime
	if (bShowUptime)
	{
		PrintAligned(szDisplay[0], LEFT, 0, 0);
	} else {
		PrintAligned(szDisplay[4], LEFT, 0, 0);
	}
	
	//Line #2 - Sensorname
	PrintAligned(szDisplay[1], CENTER, 0, 1);
	
	//Line #3 - Sensordata
	PrintAligned(szDisplay[2], CENTER, 0, 2);

	//Line #4 - Index
	PrintAligned(szDisplay[3], RIGHT, 0, 3);
}

void UpdateSensors()
{
	//Display * to let the user know that input is blocked
	lcd.setCursor(19, 0);
	lcd.write("*");
	
	sensors.requestTemperatures();
	m_SensorManager.Update(&sensors);
	
	//Remove *
	lcd.setCursor(19, 0);
	lcd.write(" ");
}

void PrintUptime(char *buffer)
{
	time_t t = now();

	sprintf(buffer, "<%id %02i:%02i:%02i>", (day(t) - 1), hour(t), minute(t), second(t));
}

void PrintAligned(char *buffer, int alignment, int column, int row)
{
	int stringLength = strlen(buffer);
	int padding = 0;
	
	//Clear current row
	lcd.setCursor(column, row);
	for(int i = 0; i < 20; i++)
		lcd.print(" ");
	lcd.setCursor(column, row);
	
	switch(alignment)
	{
		case LEFT:
			lcd.print(buffer);
			return;
		case CENTER:
			padding = 10 - stringLength / 2;
			break;
		case RIGHT:
			padding = 20 - stringLength;
			break;
	}
	//Print as many spaces as needed to achieve the desired padding-effect
	for(int i = 0; i < padding; i++)
		lcd.print(" ");
	lcd.print(buffer);
}

void ShowIntro()
{
	lcd.setCursor(0, 0);
	lcd.print(" TMP-   ____ ______ ");
	lcd.setCursor(0, 1);
	lcd.print("Control/ . //  ___/ ");
	lcd.setCursor(0, 2);
	lcd.print("      /___//  /__   ");
	lcd.setCursor(0, 3);
	lcd.print(" >_ Bigmo /_____/   ");
	delay(3000);
}

void ConvertFromFloat(char *buffer, float val)
{
	int val1 = (int)(val * 10);
	int val2 = (int)val;
	int val3 = val1 % 10;
	sprintf(buffer, "%i.%i", val2, val3);
}