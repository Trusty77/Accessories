/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Test for fading light with just one button>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

// DCC codes
#define RED_TOGGLE		DCCINT(20,0)
#define GREEN_TOGGLE	DCCINT(20,1)

// Commanders

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard	push;
#else
ButtonsCommanderPush push;
#endif

SERIAL_COMMANDER(Serial);

// Accessories

AccessoryLight red, green;

// Drivers
	
DriverArduino arduino;

void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData)
{
	Accessories::ReceiveEvent(inId, (ACCESSORIES_EVENT_TYPE)inEventType, inEventData);
}

//////////////////////////////////
//
// Setup
//
void setup()
{
	Commanders::SetEventHandler(ReceiveEvent);
	Commanders::SetStatusLedPin(LED_BUILTIN);

	Serial.begin(115200);

	// One button
    // This button will send commands to Dcc code 20/0 and 20/1, on pin 26
#ifdef VISUALSTUDIO
	push.begin(RED_TOGGLE, '0');
#else
	push.begin(RED_TOGGLE, 26);
#endif
	push.AddEvent(GREEN_TOGGLE);

	// Drivers setups

    // Two ports of the Arduino used by the leds, 10 and 11
	arduino.begin();
	DriverPort *pPort0 = arduino.AddPortMotor(10, ANALOG_INVERTED);
	DriverPort *pPort1 = arduino.AddPortMotor(11, ANALOG_INVERTED);

	// Accessories setups

    // Two lights in the accessory list.

	red.begin(pPort0, RED_TOGGLE, 0, 120);
	green.begin(pPort1, GREEN_TOGGLE, 0, 200);

    // Define fading/dimming effect
	red.SetFading(20, 20);
	green.SetFading(20, 20);

    // Start with light0 on !
	red.LightOn();
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}