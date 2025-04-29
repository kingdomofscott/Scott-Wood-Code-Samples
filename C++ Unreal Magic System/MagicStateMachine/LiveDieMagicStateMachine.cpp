#include "LiveDieMagicStateMachine.h"

LiveDieMagicStateMachine::LiveDieMagicStateMachine(float life, float postLife)
{
	states.Add(BaseMagicState(life, true));
	states.Add(BaseMagicState(postLife, false));
}

void LiveDieMagicStateMachine::TransitionState()
{
	switch ((ELiveDieMagicStates)CurrentState)
	{
	case ELiveDieMagicStates::ALIVE:
		CurrentState = (uint8)ELiveDieMagicStates::DYING;
		break;
	case ELiveDieMagicStates::DYING:
		IsStateMachineFinished = true;
		break;
	}
}