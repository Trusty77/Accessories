/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Demo for servo>
*************************************************************/

#include "Accessories.h"
#include "Commanders.h"

///////////////////////////////////////////////////////////////
// The target is to move a servo, allowing four buttons to change
// driven mode (fast, slow exclusive, memo and very slow).
// And four other buttons to reach four different positions.
// A classic button also toggle between min and max positions.

// DCC codes
#define ACTION		DCCINT(20,0)
#define POS1		DCCINT(21,0)
#define POS2		DCCINT(21,1)
#define POS3		DCCINT(22,0)
#define POS4		DCCINT(22,1)
#define MODE1		DCCINT(23,0)
#define MODE2		DCCINT(23,1)
#define MODE3		DCCINT(24,0)
#define MODE4		DCCINT(24,1)

// Commanders

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard	action;
ButtonsCommanderKeyboard	pos1;
ButtonsCommanderKeyboard	pos2;
ButtonsCommanderKeyboard	pos3;
ButtonsCommanderKeyboard	pos4;
ButtonsCommanderKeyboard	mode1;
ButtonsCommanderKeyboard	mode2;
ButtonsCommanderKeyboard	mode3;
ButtonsCommanderKeyboard	mode4;
#else
ButtonsCommanderPush	action;
ButtonsCommanderPush	pos1;
ButtonsCommanderPush	pos2;
ButtonsCommanderPush	pos3;
ButtonsCommanderPush	pos4;
ButtonsCommanderPush	mode1;
ButtonsCommanderPush	mode2;
ButtonsCommanderPush	mode3;
ButtonsCommanderPush	mode4;
#endif

// Accessories

AccessoryServo servo;

// Drivers

// We could use the arduino itself...	
DriverL293d driver;

// current user define speed.
int speed;

void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData)
{
	switch (inId)
	{
	case MODE1:	// Fast
		servo.SetDuration(0);
		Serial.println("Fast selected");
		break;
	case MODE2:	// Slow exclusive
		servo.SetDuration(5);
		Serial.println("Slow exclusive selected");
		break;
	case MODE3: // Slow with event memo
		servo.SetDuration(10);
		Serial.println("Slow selected");
		break;
	case MODE4: // Classic Slow. 
		servo.SetDuration(30);
		Serial.println("Classic slow selected");
		break;
	}

	Accessories::ReceiveEvent(inId, (ACCESSORIES_EVENT_TYPE)inEventType, inEventData);
}

//////////////////////////////////
//
// Setup
//
void setup()
{
	DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(3));
	Commanders::SetEventHandler(ReceiveEvent);
	Commanders::SetStatusLedPin(LED_BUILTIN);

#ifdef VISUALSTUDIO
	action.begin(ACTION, '0');
	pos1.begin(POS1, '1');
	pos2.begin(POS2, '2');
	pos3.begin(POS3, '3');
	pos4.begin(POS4, '4');

	mode1.begin(MODE1, 'a');
	mode2.begin(MODE2, 'z');
	mode3.begin(MODE3, 'e');
	mode4.begin(MODE4, 'r');
#else
	action.begin(ACTION, 40);
	pos1.begin(POS1, 30);
	pos2.begin(POS2, 31);
	pos3.begin(POS3, 32);
	pos4.begin(POS4, 33);

	mode1.begin(MODE1, 40);
	mode2.begin(MODE2, 41);
	mode3.begin(MODE3, 42);
	mode4.begin(MODE4, 43);
#endif

	driver.begin();

	DriverPort *pPort = driver.beginPortServo(L293D_PORT_SERVO1);

	speed = 19;	// starting speed state : fast.

	// Accessories setups

	servo.begin(pPort, 0, 40, 50, 4);
	servo.AddMovingPosition(POS1, 10);
	servo.AddMovingPosition(POS2, 20);
	servo.AddMovingPosition(POS3, 30);
	servo.AddMovingPosition(POS4, 40);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}
