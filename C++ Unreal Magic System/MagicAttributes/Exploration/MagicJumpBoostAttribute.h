#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"

/*This is a basic attribute that is put onto any magic that provides a vertical impulse*/
class LITTLETALKS_API MagicJumpBoostAttribute : BaseMagicAttribute
{
public:
	MagicJumpBoostAttribute(ABaseMagic *owner, float impulseAmount, int priority = -1);
	virtual void DoTrigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(AActor* actor) override {};

	//Variables
private:
	float impulseMultiplier;
};
