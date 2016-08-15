//-------------------------------------------------------------------
#ifndef __accessory_H__
#define __accessory_H__
//-------------------------------------------------------------------

#ifdef VISUALSTUDIO
#include "../../DIO2/VStudio/DIO2.h"
#else
#include "arduino.h"
#include "DIO2.h"
#endif

//-------------------------------------------------------------------

#ifndef UNDEFINED_ID
#define UNDEFINED_ID	((unsigned long)-1)
#endif

// The two next enums are identical to the enums in Commanders library.
enum ACCESSORIES_MOVE_TYPE
{
	ACCESSORIES_MOVE_MORE = +1,
	ACCESSORIES_MOVE_LESS = -1,
	ACCESSORIES_MOVE_STOP = 0,
	ACCESSORIES_MOVE_LEFT = -2,
	ACCESSORIES_MOVE_RIGHT = -3,
	ACCESSORIES_MOVE_CENTER = -4,
	ACCESSORIES_MOVE_TOP = -5,
	ACCESSORIES_MOVE_BOTTOM = -6,
	ACCESSORIES_MOVE_STRAIGHT = -7,
	ACCESSORIES_MOVE_DIVERGE = -8,
	ACCESSORIES_MOVE_ON = -9,
	ACCESSORIES_MOVE_OFF = -10
};

enum ACCESSORIES_EVENT_TYPE
{
	ACCESSORIES_EVENT_NONE = 0,			// Should never appear
	ACCESSORIES_EVENT_TOGGLE = 1,		// If a push button or similar is pressed, invert the state/position
	ACCESSORIES_EVENT_MOVE = 2,			// If a push 'left' button or similar is pressed, data is a COMMANDERS_MOVE_TYPE
	ACCESSORIES_EVENT_MOVEPOSITION = 3,	// If a potentiometer or similar is moved, data is the position to reach
	ACCESSORIES_EVENT_CONFIG = 4,		// Data is the configuration address and value

	// Part of events not common with Commanders library events.
	ACCESSORIES_EVENT_DIRECTION = 20,	// Move in positive or negative direction.
	ACCESSORIES_EVENT_SETSTARTPOSITION = 21,	// Change the starting position of the motor
	ACCESSORIES_EVENT_SETSPEED = 22,		// Change the speed for a motor or the intensity for a light
};

#define ACCESSORIESSCONFIG(address, value)	( ((int)address<<8) | value )
#define ACCESSORIESCONFIGADDRESS(data)		highByte((int)data)
#define ACCESSORIESCONFIGVALUE(data)		lowByte((int)data)

enum ACCESSORYTYPE { ACCESSORYUNDEFINED, ACCESSORYMOTOR, ACCESSORYLIGHT, ACCESSORYSERVO, ACCESSORYSTEPPER };
enum ACC_STATE { STATE_NONE = 0, STATE_FIRST = 10000, STATE_SECOND = 20000};

struct MovingPosition
{
	unsigned long Id;
	int Position;
};

class Accessory
{
private:
	Accessory *pNextAccessory;
	
	int movingPositionsSize;
	int movingPositionsAddCounter;
	MovingPosition *pMovingPositions;

	unsigned long duration;
	unsigned long startingMillis;
	MovingPosition lastMovingPosition;
	unsigned int debounceDelay;
	unsigned long lastMoveTime;

protected:
	//Driver *pDriver;
	//byte driverPort;
	DriverPort *pPort;

	ACC_STATE state;
	ACC_STATE prevState;
	bool useStateNone;

	ACCESSORYTYPE type;

public:
	Accessory();

	inline void begin(ACC_STATE inStartingState = STATE_NONE) { this->state = inStartingState; }

	inline ACCESSORYTYPE GetAccessoryType() const { return this->type; }

	//inline Driver *GetDriver() { return this->pDriver; }
	//inline byte GetDriverPort() { return this->driverPort; }
	inline DriverPort *GetDriverPort() { return this->pPort; }

	inline ACC_STATE GetState() const { return this->state; }
	inline ACC_STATE GetPreviousState() const { return this->prevState; }

	inline bool IsNone() const { return this->state == STATE_NONE; }
	inline bool IsFirst() const { return this->state == STATE_FIRST; }
	inline bool IsSecond() const { return this->state == STATE_SECOND; }
	inline bool UseStateNone() const { return this->useStateNone; }

	unsigned char AddMovingPosition(unsigned long inId, int inPosition);

	inline unsigned int GetDebounceDelay() const { return this->debounceDelay; }
	inline unsigned long GetLastMoveTime() const { return this->lastMoveTime; }
	inline unsigned long GetDuration() const { return this->duration; }

	inline bool loop() { return this->ActionEnded(); }
	inline virtual void Event(unsigned long inId, ACCESSORIES_EVENT_TYPE inEvent = ACCESSORIES_EVENT_TOGGLE, int inData = 0) {}

public:	//but should be protected !
	int IndexOfMovingPosition(unsigned long inId);
	inline const MovingPosition &GetLastMovingPosition() const { return this->lastMovingPosition; }
	inline const int GetMovingPositionSize() const { return this->movingPositionsSize; }
	inline void SetLastMovingPosition(unsigned long inId) { this->lastMovingPosition.Id = inId; }
	inline void SetLastMovingPosition(unsigned long inId, int inPosition) { this->lastMovingPosition.Id = inId; this->lastMovingPosition.Position = inPosition; }

	inline void SetDebounceDelay(unsigned int inDebounceDelay) { this->debounceDelay = inDebounceDelay; }
	inline void SetLastMoveTime() { this->lastMoveTime = millis(); }
	inline void SetDuration(unsigned long inDuration) { this->duration = inDuration; }

	inline void SetNextAccessory(Accessory *inAccessory) { this->pNextAccessory = inAccessory; }
	inline Accessory *GetNextAccessory() const { return this->pNextAccessory; }

	inline unsigned long GetActionStartingMillis() const { return this->startingMillis; }
	virtual void StartAction();
	virtual void StartAction(ACC_STATE inState);
#ifdef DEBUG_VERBOSE_MODE
	virtual void ResetAction();
#else
	inline virtual void ResetAction() { this->startingMillis = 0; }
#endif

	virtual bool ActionEnded();
	inline bool IsActionPending() const { return this->startingMillis > 0; }
	inline virtual bool IsGroupActionPending() const { return this->IsActionPending(); }

	virtual void Move(unsigned long inMovingPositionId) = 0;
	virtual ACC_STATE Toggle() = 0;

	inline virtual bool CanBePositionnal() const { return false; }
	inline virtual void MovePosition(int inPosition) {}
	inline bool IsEmpty() const { return this->pMovingPositions == 0; }
	inline const MovingPosition &GetMovingPosition(int inIndex) { return this->pMovingPositions[inIndex]; }

	inline virtual void SetState(ACC_STATE inNewState) { this->state = inNewState; }

	void AdjustMovingPositionsSize(int inNewSize);
	inline void SetStartingMillis() { this->startingMillis = millis(); }
	inline void ResetStartingMillis() { this->startingMillis = 0; }
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------