#include "MagicDealDamageOverTimeAttribute.h"
#include "Combat/Attackables/BaseAttackable.h"
#include "LittleTalks.h"
#include "Magic/BaseMagic.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"

MagicDealDamageOverTimeAttribute::MagicDealDamageOverTimeAttribute(ABaseMagic *owner, float howLong, float howOftenToDamage, bool affectOncePerCharacter, FString hitEffect, int priority)
	: BaseMagicAttribute(owner, EMagicAttributeTarget::Enemy, affectOncePerCharacter, priority, 50)
{
	timeToAffect = howLong;
	hitRate = howOftenToDamage;
	assetToSpawn = nullptr;

	if (!hitEffect.IsEmpty())
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> systemTemplate(*hitEffect);
		assetToSpawn = systemTemplate.Object;
	}
}

void MagicDealDamageOverTimeAttribute::Update(float dTime)
{
	TArray<int> completedCharacters;

	for (int i = 0; i < charactersToDamage.Num(); ++i)
	{
		charactersToDamage[i].Progress = FMath::Clamp(charactersToDamage[i].Progress + dTime, 0.0f, timeToAffect);

		if (charactersToDamage[i].Progress >= timeToAffect)
		{
			completedCharacters.Add(i);
		}
		else if (charactersToDamage[i].Progress >= charactersToDamage[i].WhenToHit)
		{
			charactersToDamage[i].WhenToHit = charactersToDamage[i].Progress + hitRate;

			TryApplyDamage(charactersToDamage[i].CharacterToDamage);
		}
	}

	for (int i = completedCharacters.Num() - 1; i >= 0; --i)
	{
		charactersToDamage.RemoveAt(completedCharacters[i]);
	}
}

void MagicDealDamageOverTimeAttribute::Finish()
{
	BaseMagicAttribute::Finish();

	charactersToDamage.Empty();
}

void MagicDealDamageOverTimeAttribute::DoTrigger(BaseCharacter *characterToAffect)
{
	for (int i = 0; i < charactersToDamage.Num(); ++i)
		if (charactersToDamage[i].CharacterToDamage == characterToAffect)
			return;

	CharacterToDamage newCharacterToDamage;
	newCharacterToDamage.CharacterToDamage = characterToAffect;
	newCharacterToDamage.Progress = 0.0f;
	newCharacterToDamage.WhenToHit = 0.0f;

	charactersToDamage.Add(newCharacterToDamage);
}

void MagicDealDamageOverTimeAttribute::TryApplyDamage(BaseCharacter *characterToAffect)
{
	BaseCharacter *owner = owningMagic->OwningAttack->OwningCharacter;

	ETargetGroup ourGroup = owner->TargetGroup();
	ETargetGroup otherGroup = characterToAffect->TargetGroup();

	if (characterToAffect == owner || otherGroup == ourGroup)
		return;

	int damage = owningMagic->OwningAttack->CalculateMagicAttackDamageOutput(characterToAffect, owningMagic->Attack);

	int appliedDamage = 0;

	FTakeDamageParameters damageParams;
	damageParams.Damage = damage;
	damageParams.DamagingCharacter = owner;
	damageParams.DamageCauser = owningMagic;
	damageParams.Attack = attackInfo;
	damageParams.HitLocation = owningMagic->GetActorLocation();

	appliedDamage = characterToAffect->TakeDamage(damageParams);

	if (appliedDamage > 0)
	{
		owner->DamageApplied(appliedDamage);

		UBaseGameInstance *instance = owningMagic->GetGameInstance<UBaseGameInstance>();

		if (assetToSpawn)
			instance->VFXManager->SpawnGivenParticlesAtLocation(assetToSpawn, characterToAffect->GetPawnSelf()->GetActorLocation());
		else
			instance->VFXManager->SpawnCommonParticlesAtLocation(ECommonParticles::HIT, characterToAffect->GetPawnSelf()->GetActorLocation());
	}
}

void MagicDealDamageOverTimeAttribute::RemoveCharacter(BaseCharacter *characterToRemove)
{
	int removeIndex = -1;

	for (int i = 0; i < charactersToDamage.Num(); ++i)
		if (charactersToDamage[i].CharacterToDamage == characterToRemove)
			removeIndex = i;

	if (removeIndex > -1)
		charactersToDamage.RemoveAt(removeIndex);
}
