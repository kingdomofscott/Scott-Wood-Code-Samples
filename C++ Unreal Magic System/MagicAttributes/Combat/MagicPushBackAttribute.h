#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"
#include "Combat/Interfaces/Targetable.h"

/*This is a basic attribute that is put onto any magic attacks that pushes back the actor on collision*/
class LITTLETALKS_API MagicPushbackAttribute : BaseMagicAttribute
{
public:
	MagicPushbackAttribute(ABaseMagic *owner, EMagicAttributeTarget target, float pushBackAmount, int priority = -1);

	virtual void DoTrigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(AActor *actor) override {};

private:
	bool AddActor(AActor *actor);

	//Variables
private:
	float impulseMultiplier;
};
