#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"

/*This is a basic attribute that is put onto any magic that is meant to disappear once it collides*/
class LITTLETALKS_API MagicDieOnImpactAttribute : BaseMagicAttribute
{
public:
	MagicDieOnImpactAttribute(ABaseMagic *owner, int priority = 0);
	virtual void DoTrigger(BaseCharacter* characterToAffect) override;
	virtual void DoTrigger(AActor* actor) override;
};
