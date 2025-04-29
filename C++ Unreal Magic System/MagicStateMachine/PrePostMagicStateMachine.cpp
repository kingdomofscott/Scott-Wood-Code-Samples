#include "PrePostMagicStateMachine.h"

PrePostMagicStateMachine::PrePostMagicStateMachine(float preCastLife, float castLife, float postCastLife)
{
	states.Add(BaseMagicState(preCastLife, false));
	states.Add(BaseMagicState(castLife, true));
	states.Add(BaseMagicState(postCastLife, false));
}

void PrePostMagicStateMachine::TransitionState()
{
	switch ((EPrePostMagicStates)CurrentState)
	{
	case EPrePostMagicStates::PRE_CAST:
		CurrentState = (uint8)EPrePostMagicStates::CAST;
		break;
	case EPrePostMagicStates::CAST:
		CurrentState = (uint8)EPrePostMagicStates::POST_CAST;
		break;
	case EPrePostMagicStates::POST_CAST:
		IsStateMachineFinished = true;
		break;
	}
}