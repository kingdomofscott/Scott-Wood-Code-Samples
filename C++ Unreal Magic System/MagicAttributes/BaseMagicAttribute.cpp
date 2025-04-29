#include "BaseMagicAttribute.h"

BaseMagicAttribute::BaseMagicAttribute(ABaseMagic *owner, EMagicAttributeTarget targetGroup, bool oncePerActor, int priority, int defaultPriority) :
	BaseAbilityAttribute(priority, defaultPriority)
{
	owningMagic = owner;
	AttributeTarget = targetGroup;
	affectOnce = oncePerActor;
}

BaseMagicAttribute::~BaseMagicAttribute()
{
}

void BaseMagicAttribute::Trigger(BaseCharacter *characterToAffect)
{
	if (!canTrigger)
		return;

	if (affectOnce)
	{
		if (!prevAffected.Contains(characterToAffect->GetPawnSelf()))
		{
			DoTrigger(characterToAffect);
			prevAffected.Add(characterToAffect->GetPawnSelf());
		}
	}
	else
	{
		DoTrigger(characterToAffect);
	}
}

void BaseMagicAttribute::Trigger(AActor *actor)
{
	if (!canTrigger)
		return;

	if (affectOnce)
	{
		if (!prevAffected.Contains(actor))
		{
			DoTrigger(actor);
			prevAffected.Add(actor);
		}
	}
	else
	{
		DoTrigger(actor);
	}
}

void BaseMagicAttribute::Start()
{
	canTrigger = true;
}

void BaseMagicAttribute::Finish()
{
	canTrigger = false;
}
