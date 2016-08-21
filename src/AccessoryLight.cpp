/**********************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Class for a light, flashing or not, with optional fading>
***********************************************************************/

#include "Accessories.h"
#include "AccessoryLight.hpp"

#ifndef NO_LIGHT

AccessoryLight::AccessoryLight()
{
}

void AccessoryLight::begin(DriverPort *inpPort, unsigned long inId, unsigned long inBlinkDuration, int inIntensity)
{ 
	this->pPort = inpPort;
	this->pLight = new AccessoryBaseLight(this);

	this->pLight->SetBlinking(inBlinkDuration);
	this->type = ACCESSORYLIGHT;

	this->pLight->begin(inpPort, inIntensity);
	this->AddMovingPosition(inId, LIGHTON);
}

void AccessoryLight::Move(unsigned long inId)
{
	if (this->GetMovingPositionSize() == 1)
	{
		this->Toggle();
		return;
	}

	int position = this->IndexOfMovingPosition(inId);

	if (position != -1)
		this->SetState((ACC_STATE) position);
}

void AccessoryLight::Event(unsigned long inId, ACCESSORIES_EVENT_TYPE inEvent, int inData)
{
	if (inEvent == ACCESSORIES_EVENT_MOVEPOSITIONINDEX)
	{
		int oldValue = this->pPort->GetSpeed();
		this->pPort->SetSpeed(this->GetMovingPosition(inData));
		this->LightOn();
		this->pPort->SetSpeed(oldValue);
		return;
	}

	this->pLight->Event(inEvent, inData);
}
#endif
