#include "BaseMagicStateMachine.h"

BaseMagicState::BaseMagicState(float totalLife, bool applyAttributes)
{
	totalLifetime = totalLife;
	currentLifetime = 0.0f;
	shouldApplyAttributes = applyAttributes;
}

BaseMagicStateMachine::BaseMagicStateMachine()
{
	CurrentState = 0;
	IsStateMachineFinished = false;
}

BaseMagicStateMachine::~BaseMagicStateMachine() {}

void BaseMagicStateMachine::Update(float deltaTime)
{
	if (states.Num() > CurrentState)
	{
		states[CurrentState].currentLifetime += deltaTime;

		if (states[CurrentState].currentLifetime >= states[CurrentState].totalLifetime)
			TransitionState();
	}
}

void BaseMagicStateMachine::TransitionState()
{
}

BaseMagicState BaseMagicStateMachine::GetCurrentState()
{
	return states[CurrentState];
}
