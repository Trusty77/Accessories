/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Driver port for L293n>
*************************************************************/

#if !defined(__AVR_ATmega32U4__)

#include "Accessories.h"

#ifndef NO_SHIELDL293D

PortShieldL293d::PortShieldL293d() : Port(MOTOR_LIGHT)
{
}

void PortShieldL293d::begin(unsigned char inOutPort, uint8_t inSpeed, uint8_t inFreq)
{
	this->pmotor = new AF_DCMotor(inOutPort, inFreq);
	this->pmotor->pwmfreq = inFreq;
	this->pmotor->setSpeed(inSpeed);
 
	this->pmotor->run(RELEASE);
}

int PortShieldL293d::SetSpeed(uint8_t inSpeed)
{
	int oldspeed = Port::SetSpeed(inSpeed);
	this->pmotor->setSpeed(inSpeed);
	return oldspeed;
}

void PortShieldL293d::MoveLeftDir(unsigned long inDuration)
{
#ifdef ACCESSORIES_DEBUG_MODE
	Serial.print(F(" PortShieldL293d MoveLeftDir() "));
	if (inDuration != 0)
	{
		Serial.print(F("for "));
		Serial.print(inDuration);
		Serial.println(F("ms"));
	}
	else
		Serial.println("");
#endif
	this->pmotor->run(FORWARD);
	if (inDuration != 0)
	{
		delay(inDuration);
		this->pmotor->run(RELEASE);
	}

	this->state = PORT_LEFT;
}

void PortShieldL293d::MoveRightDir(unsigned long inDuration)
{
#ifdef ACCESSORIES_DEBUG_MODE
	//Serial.print(this->pmotor->motornum);
	Serial.print(F(" PortShieldL293d MoveRightDir() "));
	if (inDuration != 0)
	{
		Serial.print(F("for "));
		Serial.print(inDuration);
		Serial.println(F("ms"));
	}
	else
		Serial.println("");
#endif
	this->pmotor->run(BACKWARD);
	if (inDuration != 0)
	{
		delay(inDuration);
		this->pmotor->run(RELEASE);
	}

	this->state = PORT_RIGHT;
}

void PortShieldL293d::MoveStop()
{
	this->pmotor->run(RELEASE);
	this->state = PORT_STOP;
}
#endif
#endif
