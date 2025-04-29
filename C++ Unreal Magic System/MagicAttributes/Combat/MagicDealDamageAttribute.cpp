#include "MagicDealDamageAttribute.h"
#include "Combat/Attackables/BaseAttackable.h"
#include "LittleTalks.h"
#include "Magic/BaseMagic.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"

MagicDealDamageAttribute::MagicDealDamageAttribute(ABaseMagic *owner, bool affectActorsOnce, FString hitEffect, int priority) :
	BaseMagicAttribute(owner, EMagicAttributeTarget::Enemy, affectActorsOnce, priority, 50)
{
	assetToSpawn = nullptr;

	if (!hitEffect.IsEmpty())
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> systemTemplate(*hitEffect);
		assetToSpawn = systemTemplate.Object;
	}
}

void MagicDealDamageAttribute::DoTrigger(BaseCharacter *characterToAffect)
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
