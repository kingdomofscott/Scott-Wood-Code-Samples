#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"

struct CharacterToDamage
{
public:
	BaseCharacter *CharacterToDamage;
	float Progress;
	float WhenToHit;
};

class UNiagaraSystem;

/*This is a basic attribute that is put onto any magic attacks that deal damage more than once over time*/
class LITTLETALKS_API MagicDealDamageOverTimeAttribute : BaseMagicAttribute
{
public:
	MagicDealDamageOverTimeAttribute(ABaseMagic *owner, float howLong, float howOftenToDamage, bool affectOncePerCharacter = true, 
		FString hitEffect = "", int priority = -1);
	virtual void Update(float dTime) override;
	virtual void Finish() override;

	virtual void DoTrigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(AActor* actor) override {};

	void RemoveCharacter(BaseCharacter *characterToRemove);

	void SetTimeToAffect(float affectTime) { timeToAffect = affectTime; }

private:
	void TryApplyDamage(BaseCharacter *characterToAffect);

private:
	float timeToAffect;
	float hitRate;
	TArray<CharacterToDamage> charactersToDamage;
	UNiagaraSystem *assetToSpawn;
};
