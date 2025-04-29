#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"
#include "Combat/Interfaces/Targetable.h"

class USplineComponent;

struct ActorOnPath
{
public:
	AActor *ActorMoving;
	float Progress;
	FVector StartingPosition;
	bool UpdatePosition;
};

/*This is a basic attribute that is put onto any magic that grabs an actor and moves it along a spline*/
class LITTLETALKS_API MagicMoveActorAlongPathAttribute : BaseMagicAttribute
{
public:
	MagicMoveActorAlongPathAttribute(ABaseMagic *owner, EMagicAttributeTarget target, USplineComponent *path,
		bool releaseOnComplete, float endImpulseAmount, bool affectActorOnce = false, int priority = -1);
	virtual void Update(float dTime) override;
	virtual void Finish() override;

	virtual void DoTrigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(AActor *actor) override;

protected:
	virtual void UpdateActorProgress(int index, float dTime);
	virtual bool HasActorFinished(int index);

private:
	bool AddActor(AActor *actor);
	void FreeActor(AActor *actor, bool addImpulse);

	//Variables
protected:
	USplineComponent *pathToMoveOn;
	float impulseMultiplier;
	bool releaseActors;

	TArray<ActorOnPath> controlledActors;
};
