#pragma once

#include "MagicMoveActorAlongPathAttribute.h"

/*This is a basic attribute that is put onto any magic that grabs an actor, waits for a set time, and moves it along a spline*/
class LITTLETALKS_API MagicDelayedLoopMoveActorAlongPathAttribute : MagicMoveActorAlongPathAttribute
{
public:
	MagicDelayedLoopMoveActorAlongPathAttribute(ABaseMagic *owner, EMagicAttributeTarget target, USplineComponent *path,
		float endImpulseAmount, float startDelay = 0.0f, int priority = -1);
	virtual void Update(float dTime) override;

protected:
	virtual void UpdateActorProgress(int index, float dTime) override;
	virtual bool HasActorFinished(int index) override;

	//Variables
private:
	float delayedStartTime;
	float currentTime;

	bool movingForward;
};
