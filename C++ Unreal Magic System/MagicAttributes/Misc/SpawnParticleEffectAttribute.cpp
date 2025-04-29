#include "SpawnParticleEffectAttribute.h"
#include "Magic/BaseMagic.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

SpawnParticleEffectAttribute::SpawnParticleEffectAttribute(ABaseMagic *owner, FString effectLocation, bool niagaraSystem, int priority) :
	BaseMagicAttribute(owner,
		(EMagicAttributeTarget)((uint8)EMagicAttributeTarget::Enemy | (uint8)EMagicAttributeTarget::Objects),
		true, priority)
{
	useNiagara = niagaraSystem;

	if (useNiagara)
	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem> systemTemplate(*effectLocation);
		assetToSpawn = systemTemplate.Object;
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> systemTemplate(*effectLocation);
		assetToSpawn = systemTemplate.Object;
	}
}

void SpawnParticleEffectAttribute::DoTrigger(BaseCharacter *characterToAffect)
{
	SpawnEffect();
}

void SpawnParticleEffectAttribute::DoTrigger(AActor *actor)
{
	SpawnEffect();
}

void SpawnParticleEffectAttribute::SpawnEffect()
{
	if (assetToSpawn != nullptr)
	{
		if (useNiagara)
		{
			UNiagaraSystem *system = Cast<UNiagaraSystem>(assetToSpawn);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(owningMagic->GetWorld(), system, owningMagic->GetActorLocation());
		}
		else
		{
			UParticleSystem *system = Cast<UParticleSystem>(assetToSpawn);
			UGameplayStatics::SpawnEmitterAtLocation(owningMagic->GetWorld(), system, owningMagic->GetActorTransform());
		}
	}
}
