#include "MagicDieOnImpactAttribute.h"
#include "Magic/BaseMagic.h"

MagicDieOnImpactAttribute::MagicDieOnImpactAttribute(ABaseMagic *owner, int priority) :
	BaseMagicAttribute(owner, 
		(EMagicAttributeTarget)((uint8)EMagicAttributeTarget::Enemy | (uint8)EMagicAttributeTarget::Objects), 
		true, priority)
{
}

void MagicDieOnImpactAttribute::DoTrigger(BaseCharacter *characterToAffect)
{
	owningMagic->Destroy();
}

void MagicDieOnImpactAttribute::DoTrigger(AActor *actor)
{
	owningMagic->Destroy();
}
