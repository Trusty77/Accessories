/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Class for a generic accessory>
*************************************************************/

#include "Accessories.h"
#include "ActionsStack.hpp"
#ifndef NO_EEPROM
#include "EEPROM.h"
#endif

Accessory *Accessory::pFirstAccessory = NULL;

Accessory::Accessory()
{
	this->pPort = NULL;
	this->movingPositionsSize = 0;
	this->movingPositionsAddCounter = 0;
	this->pMovingPositions = NULL;
	this->lastMovingPosition = 255;
	this->SetLastMoveTime();

	this->duration = 0;
	this->startingMillis = 0;
	Accessory::Add(this);
}

void Accessory::AdjustMovingPositionsSize(uint8_t inNewSize)
{
	if (inNewSize <= this->movingPositionsSize)
		return;

	MovingPosition *pNewList = new MovingPosition[inNewSize];
	int i = 0;
	for (; i < this->movingPositionsSize; i++)
		pNewList[i] = this->pMovingPositions[i];

	for (; i < inNewSize; i++)
		pNewList[i].Id = UNDEFINED_ID;	//empty

	this->movingPositionsSize = inNewSize;
	if (this->pMovingPositions != NULL)
		delete[] this->pMovingPositions;
	this->pMovingPositions = pNewList;
}

// Returns the index of the new added position.
uint8_t Accessory::AddMovingPosition(unsigned long inId, int inPosition)
{
	Accessory::AdjustMovingPositionsSize(this->movingPositionsAddCounter + 1);

	this->pMovingPositions[this->movingPositionsAddCounter].Id = inId;
	this->pMovingPositions[this->movingPositionsAddCounter++].Position = inPosition;

	return this->movingPositionsAddCounter - 1;
}

uint8_t Accessory::IndexOfMovingPosition(unsigned long inId) const
{
	for (int i = 0; i < this->movingPositionsSize; i++)
		if (this->pMovingPositions[i].Id == inId)
			return i;

	return 255;
}

int Accessory::GetMovingPosition(unsigned long inId) const
{
	for (int i = 0; i < this->movingPositionsSize; i++)
		if (this->pMovingPositions[i].Id == inId)
			return this->pMovingPositions[i].Position;

	return UNDEFINED_POS;
}

void Accessory::StartAction()
{
	if (this->duration > 0)
		this->startingMillis = millis();

#ifdef ACCESSORIES_DEBUG_MODE
#ifdef ACCESSORIES_DEBUG_VERBOSE_MODE
	Serial.print(F("Accessory start action "));
	Serial.println(this->startingMillis);
#endif
#endif
}

void Accessory::StartAction(ACC_STATE inState)
{
	if (this->duration > 0)
		this->startingMillis = millis();
	this->SetState(inState);

#ifdef ACCESSORIES_DEBUG_MODE
#ifdef ACCESSORIES_DEBUG_VERBOSE_MODE
	Serial.print(F("Accessory start action at "));
	Serial.print(this->startingMillis);
	Serial.print(F("ms for state "));
	Serial.print(inState);

	if (this->startingMillis == 0)
		Serial.println(" ended");
	else
		Serial.println("");
#endif
#endif
}

#ifdef ACCESSORIES_DEBUG_MODE
void Accessory::CheckPort() const
{
	if (this->GetPort() == NULL)
	{
		Serial.println(F("One accessory have no port !"));
	}
}
#endif

#ifdef ACCESSORIES_DEBUG_VERBOSE_MODE
void Accessory::ResetAction()
{
	Serial.print(F("End (reset) action at "));
	Serial.print(millis() - this->startingMillis);
	Serial.print(F("ms for "));
	Serial.print(this->duration);
	Serial.println(F("ms"));

	this->startingMillis = 0;
}
#endif

bool Accessory::ActionEnded()
{
	if (this->startingMillis <= 0)
		return false;

	if ((unsigned long)(millis() - this->startingMillis) > this->duration)
	{
#ifdef ACCESSORIES_DEBUG_MODE
#ifdef ACCESSORIES_DEBUG_VERBOSE_MODE
		Serial.print(F("End action at "));
		Serial.print(millis() - this->startingMillis);
		Serial.print(F("ms for "));
		Serial.print(this->duration);
		Serial.println(F("ms"));
#endif
#endif
		this->startingMillis = 0;
		return true;
	}

	return false;
}

void Accessory::SetLastMovingPosition(uint8_t inLastPositionIndex)
{
	this->lastMovingPosition = inLastPositionIndex; 
#ifndef NO_EEPROM
	Accessories::EEPROMSave();
#endif
}

void Accessory::SetStateRaw(ACC_STATE inNewState)
{
	if (this->state != inNewState)
	{
		this->state = inNewState;
#ifndef NO_EEPROM
		Accessories::EEPROMSave();
#endif
	}
}

#ifndef NO_EEPROM
int Accessory::EEPROMSave(int inPos, bool inSimulate)
{
	if (!inSimulate)
	{
		EEPROM.write(inPos, this->state);
		EEPROM.write(inPos+1, this->GetLastMovingPosition());
		EEPROM.write(inPos+2, this->pPort->GetSpeed());
	}

	return inPos + 3;
}

int Accessory::EEPROMLoad(int inPos)
{
	this->state = (ACC_STATE)EEPROM.read(inPos++);
	this->SetLastMovingPosition(EEPROM.read(inPos++));
	this->pPort->SetSpeed(EEPROM.read(inPos++));

	return inPos;
}
#endif

#ifdef ACCESSORIES_PRINT_ACCESSORIES
void Accessory::printMovingPositions()
{
	for (int i = 0; i < this->movingPositionsSize; i++)
	{
		Serial.print(i);
		Serial.print(F(": id "));
		Serial.print(this->pMovingPositions[i].Id);
		Serial.print(F(" / pos "));
		Serial.println(this->pMovingPositions[i].Position);
	}
}
#endif

// Accessory list part

void Accessory::Add(Accessory *inpAccessory)
{
	if (Accessory::pFirstAccessory == NULL)
	{
		Accessory::pFirstAccessory = inpAccessory;
		inpAccessory->SetNextAccessory(NULL);
		return;
	}

	Accessory *pCurr = Accessory::pFirstAccessory;

	while (pCurr->GetNextAccessory() != NULL)
		pCurr = pCurr->GetNextAccessory();

	pCurr->SetNextAccessory(inpAccessory);
	inpAccessory->SetNextAccessory(NULL);
}

uint8_t Accessory::GetCount()
{
	uint8_t accCount = 0;
	Accessory *pCurr = Accessory::pFirstAccessory;
	while (pCurr != NULL)
	{
		accCount++;
		pCurr = pCurr->GetNextAccessory();
	}

	return accCount;
}

Accessory *Accessory::GetById(unsigned long inId)
{
	Accessory *pCurr = Accessory::pFirstAccessory;

	while (pCurr != NULL)
	{
		if (pCurr->IndexOfMovingPosition(inId) != (uint8_t)-1)
			return pCurr;
		pCurr = pCurr->GetNextAccessory();
	}

	return NULL;
}

bool Accessory::IsActionPending()
{
	Accessory *pCurr = Accessory::pFirstAccessory;

	while (pCurr != NULL)
	{
		if (pCurr->IsActionDelayPending())
			return true;
		pCurr = pCurr->GetNextAccessory();
	}

	return false;
}

bool Accessory::CanMove(unsigned long inId)
{
	Accessory *acc = GetById(inId);

	if (acc == NULL)
		return false;

	// Move if there is more than one MovingPosition (no toggle id), and moving ids are the same than
	// previous time...
	if (acc->GetMovingPositionSize() > 1)
	{
		bool move = acc->IndexOfMovingPosition(inId) == acc->GetLastMovingPosition();
#ifdef ACCESSORIES_DEBUG_MODE
		if (!move)
			Serial.println(F("Same position : Cant move !"));
#endif
		return move;
	}

	if (millis() - acc->GetLastMoveTime() <= acc->GetDebounceDelay())
	{
#ifdef ACCESSORIES_DEBUG_MODE
		Serial.println(F("Debounce : Cant move !"));
#endif
		return false;
	}

	acc->SetLastMoveTime();
	return true;
}

bool Accessory::Toggle(unsigned long inId)
{
	if (ActionsStack::FillingStack)
	{
#ifdef ACCESSORIES_DEBUG_MODE
		Serial.print(F(" ---- Stack id "));
		Serial.println(inId);
#endif
		ActionsStack::Actions.Add(inId, ACCESSORIES_EVENT_MOVEPOSITIONID);
		return false;
	}

	Accessory *acc = GetById(inId);

	if (acc == NULL)
	{
		return false;
	}

	if (!CanMove(inId))
	{
		return true;
	}

	uint8_t pos = acc->IndexOfMovingPosition(inId);

	if (pos == 255)
	{
		return false;
	}

	acc->SetLastMovingPosition(pos);

#ifdef ACCESSORIES_DEBUG_MODE
	Serial.print(F("Toggle : Accessory id "));
	Serial.println(inId);
#endif

	acc->Move(inId);

	return true;
}

bool Accessory::MovePosition(unsigned long inId)
{
	Accessory *acc = GetById(inId);

	if (acc == NULL)
	{
		return false;
	}

	uint8_t pos = acc->IndexOfMovingPosition(inId);

	if (pos == 255)
	{
		return false;
	}

	if (ActionsStack::FillingStack)
	{
#ifdef ACCESSORIES_DEBUG_MODE
		Serial.print(F(" ---- Stack id "));
		Serial.println(inId);
#endif
		ActionsStack::Actions.Add(inId, ACCESSORIES_EVENT_MOVEPOSITIONINDEX, pos);
		return false;
	}

	acc->SetLastMovingPosition(pos);

#ifdef ACCESSORIES_DEBUG_MODE
	Serial.print(F("MovePosition : Accessory id "));
	Serial.print(inId);
	Serial.print(F(" to position "));
	Serial.println(acc->GetMovingPosition(inId));
#endif

	acc->MovePosition(acc->GetMovingPosition(inId));

	return true;
}

void Accessory::ExecuteEvent(unsigned long inId, ACCESSORIES_EVENT_TYPE inEvent, int inData)
{
	if (!CanMove(inId) || (ActionsStack::FillingStack && inEvent != ACCESSORIES_EVENT_EXTERNALMOVE))
	{
		ActionsStack::Actions.Add(inId, inEvent, inData);
		return;
	}

	Accessory *acc = GetById(inId);

	if (acc != NULL)
	{
		if (inEvent == ACCESSORIES_EVENT_MOVEPOSITIONINDEX && (inData < 0 || inData >= acc->GetMovingPositionSize()))
		{
#ifdef ACCESSORIES_DEBUG_MODE
			Serial.print(F("Accessory id "));
			Serial.print(inId);
			Serial.print(F(" bad MovePositionIndex event "));
			Serial.println(inData);
#endif
			return;
		}

		acc->Event(inId, inEvent, inData);
	}
}
