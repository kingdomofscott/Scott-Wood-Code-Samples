#pragma once

#include "CoreMinimal.h"
#include "BaseMagicStateMachine.h"

enum class ELiveDieMagicStates : uint8
{
	ALIVE,
	DYING
};

/*This is a state machine that has an immedaite start and an end.*/
class LITTLETALKS_API LiveDieMagicStateMachine : public BaseMagicStateMachine
{
public:
	LiveDieMagicStateMachine(float life, float postLife);
	virtual void TransitionState() override;
};
