#include "MagicDelayedLoopMoveActorAlongPathAttribute.h"
#include "Components/SplineComponent.h"

MagicDelayedLoopMoveActorAlongPathAttribute::MagicDelayedLoopMoveActorAlongPathAttribute::MagicDelayedLoopMoveActorAlongPathAttribute(ABaseMagic *owner,
	EMagicAttributeTarget target, USplineComponent *path, float endImpulseAmount, float startDelay, int priority)
	: MagicMoveActorAlongPathAttribute(owner, target, path, false, endImpulseAmount, true, priority)
{
	delayedStartTime = startDelay;
	currentTime = 0.0f;
	movingForward = true;
}

void MagicDelayedLoopMoveActorAlongPathAttribute::Update(float dTime)
{
	currentTime += dTime;

	if (currentTime >= delayedStartTime)
		MagicMoveActorAlongPathAttribute::Update(dTime);
}

void MagicDelayedLoopMoveActorAlongPathAttribute::UpdateActorProgress(int index, float dTime)
{
	if (movingForward)
	{
		controlledActors[index].Progress += dTime;

		if (controlledActors[index].Progress >= pathToMoveOn->Duration)
		{
			controlledActors[index].Progress = pathToMoveOn->Duration;
			movingForward = false;
		}
	}
	else
	{
		controlledActors[index].Progress -= dTime;

		if (controlledActors[index].Progress <= 0.0f)
		{
			controlledActors[index].Progress = 0.0f;
			movingForward = true;
		}
	}
}

bool MagicDelayedLoopMoveActorAlongPathAttribute::HasActorFinished(int index)
{
	return false;
}
