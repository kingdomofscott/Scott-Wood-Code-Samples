#pragma once

#include "CoreMinimal.h"

/*This is the base functionality for state machines. This goes from state to state based on a linear lifetime*/
struct BaseMagicState
{
public:
	BaseMagicState(float totalLife, bool applyAttributes);

	float totalLifetime;
	float currentLifetime;
	bool shouldApplyAttributes;
};

class LITTLETALKS_API BaseMagicStateMachine
{
public:
	BaseMagicStateMachine();
	BaseMagicStateMachine(float lifetime);
	virtual ~BaseMagicStateMachine();
	void Update(float deltaTime);
	virtual void TransitionState();
	BaseMagicState GetCurrentState();
	//Variables
public:
	uint8 CurrentState;
	bool IsStateMachineFinished;
protected:
	/*This should be added in override classes in the order corresponing to the override's state enum*/
	TArray<BaseMagicState> states;
};
