#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityAttributes/BaseAbilityAttribute.h"
#include "Character/BaseCharacter.h"

enum class EMagicAttributeTarget : uint8
{
	Ally = 1,
	Enemy = 2,
	Objects = 4
};

class ABaseMagic; 

/*
This is the base of all magic attributes. It inherits off of a base ability attribute (not shown) that is used for things outside of the magic system.
The purpose of this is to genericize common functionality among all the magic attacks and remove the need to rewrite code. 
*/
class LITTLETALKS_API BaseMagicAttribute : public BaseAbilityAttribute
{
public:
	BaseMagicAttribute(ABaseMagic *owner, EMagicAttributeTarget targetGroup, bool oncePerActor = false, int priority = -1, int defaultPriority = 100);
	virtual ~BaseMagicAttribute();
	void Trigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(BaseCharacter *characterToAffect) = 0;
	void Trigger(AActor *actor);
	virtual void DoTrigger(AActor *actor) = 0;
	void Trigger() override {};
	virtual void Start();
	virtual void Finish();

	//Variables
public:
	EMagicAttributeTarget AttributeTarget;

protected:
	ABaseMagic *owningMagic;
	bool canTrigger;

private:
	bool affectOnce;
	TArray<AActor *> prevAffected;
};
