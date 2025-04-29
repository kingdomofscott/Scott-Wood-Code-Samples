#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"

class UFXSystemAsset;

/*This is a basic attribute that needs to spawn particles at a certain time*/
class LITTLETALKS_API SpawnParticleEffectAttribute : BaseMagicAttribute
{
public:
	SpawnParticleEffectAttribute(ABaseMagic *owner, FString effectLocation, bool niagaraSystem, int priority = -1);
	virtual void DoTrigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(AActor *actor) override;

	void SpawnEffect();

public:
	UFXSystemAsset *assetToSpawn;
	bool useNiagara;
};
