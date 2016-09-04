/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Class for a stepper motor accessory>
*************************************************************/

// A stepper motor moves only in relative move. So the current position is the last MovingPosition called.

#include "Accessories.h"

#ifndef NO_STEPPER

AccessoryStepper::AccessoryStepper()
{
	this->pPort = NULL;
	this->type = ACCESSORYSTEPPER;
}

void AccessoryStepper::begin(DriverPort *inpPort, int inStepsNumber, int inReduction, unsigned int inSpeed, int inMovingPositionsNumber)
{
	this->pPort = inpPort;

	((DriverPortStepper *) this->pPort)->SetSpeed(inSpeed);

	Accessory::begin(STATE_NONE);
	this->prevState = STATE_NONE;
	this->stepsNumber = inStepsNumber;
	this->reduction = inReduction;
	this->angleByStep = 360. / inStepsNumber / inReduction;
	this->AdjustMovingPositionsSize(inMovingPositionsNumber);
}

ACC_STATE AccessoryStepper::MoveToggle()
{
	if (this->IsActionPending())
		return this->GetState();

#ifdef ACCESSORIES_DEBUG_MODE
	Serial.print(F("AccessoryStepper MoveToggle() : "));
#endif

	int pos = this->GetLastMovingPosition() + 1;
	if (this->GetLastMovingPosition() == 255)
		pos = 0;
	if (pos >= this->GetMovingPositionSize())
		pos = 0;

	this->Move(this->GetMovingPositionIdByIndex(pos));

	return this->state;
}

void AccessoryStepper::Move(unsigned long inId)
{
#ifdef ACCESSORIES_DEBUG_MODE
	Serial.println(F("AccessoryStepper Move()"));
#endif

	if (this->GetMovingPositionSize() == 1)
	{
		this->MoveToggle();
		return;
	}

	this->SetLastMovingPosition(this->IndexOfMovingPosition(inId));
	this->MovePosition(this->GetMovingPosition(inId));
}

void AccessoryStepper::MovePosition(int inAbsolutePosition)
{
	if (this->IsActionPending())
		return;

#ifdef ACCESSORIES_DEBUG_MODE
	Serial.print(F(" AccessoryStepper MovePosition() to: "));
	Serial.println(inAbsolutePosition);
#endif

	this->pPort->MovePosition(0, inAbsolutePosition);
}

void AccessoryStepper::MoveRelativePosition(int inRelativePosition)
{
	if (this->IsActionPending())
		return;

#ifdef ACCESSORIES_DEBUG_MODE
	Serial.print(F(" AccessoryStepper MoveRelativePosition() for "));
	Serial.print(inRelativePosition);
	Serial.print(F(" steps."));
#endif

	((DriverPortStepper *)this->pPort)->MoveRelativePosition(0, inRelativePosition);
}

void AccessoryStepper::Event(unsigned long inId, ACCESSORIES_EVENT_TYPE inEvent, int inData)
{
	switch (inEvent)
	{
	case ACCESSORIES_EVENT_TOGGLE:
		this->MoveToggle();
		break;

	case ACCESSORIES_EVENT_MOVE:
		switch (inData)
		{
		case ACCESSORIES_MOVE_STRAIGHT:
		case ACCESSORIES_MOVE_TOP:
		case ACCESSORIES_MOVE_LEFT:
		case ACCESSORIES_MOVE_DIVERGE:
		case ACCESSORIES_MOVE_BOTTOM:
		case ACCESSORIES_MOVE_RIGHT:
		case ACCESSORIES_MOVE_OFF:
		case ACCESSORIES_MOVE_STOP:
			this->pPort->MoveStop();
			this->ResetAction();
			this->state = STATE_NONE;
			break;
		case ACCESSORIES_MOVE_MORE:
		case ACCESSORIES_MOVE_LESS:
			if (this->state == CALIBRATION)
				this->MoveRelativePosition(inData);
			else
			{
				int pos = this->GetLastMovingPosition() + inData;
				if (pos < 0)
					pos = 0;
				if (pos >= this->GetMovingPositionSize())
					pos = this->GetMovingPositionSize() - 1;

				this->Move(this->GetMovingPositionIdByIndex(pos));
			}
			break;
		}
		break;

	case ACCESSORIES_EVENT_MOVEPOSITION:
		this->MovePosition(inData);	// absolute movement
		break;

	case ACCESSORIES_EVENT_MOVEPOSITIONID:
		this->Move(inId);
		break;

	case ACCESSORIES_EVENT_MOVEPOSITIONINDEX:
		this->Move(this->GetMovingPositionIdByIndex(inData));
		break;

	case ACCESSORIES_EVENT_SETSPEED:
		this->pPort->SetSpeed(inData);
		break;

	default:
		break;
	}
}

bool AccessoryStepper::ActionEnded()
{
	DriverPortStepper *pStepper = (DriverPortStepper *) this->pPort;

	if (pStepper->targetPosition() == pStepper->currentPosition())
		return true;

	pStepper->run();

	if (pStepper->targetPosition() == pStepper->currentPosition())
	{
		this->pPort->MoveStop();
		this->state = STATE_NONE;

#ifdef ACCESSORIES_DEBUG_MODE
		Serial.print(F(" AccessoryStepper end of movement. Pos : "));
		Serial.println(((DriverPortStepper *) this->pPort)->currentPosition());
#endif
	}

	return false;
}

#endif