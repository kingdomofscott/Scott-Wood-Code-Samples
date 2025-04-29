#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"

class UNiagaraSystem;

/*This is a basic attribute that is put onto any magic attacks that deal damage once*/
class LITTLETALKS_API MagicDealDamageAttribute : BaseMagicAttribute
{
public:
	MagicDealDamageAttribute(ABaseMagic *owner, bool affectActorsOnce = false,
		FString hitEffect = "", int priority = -1);
	virtual void DoTrigger(BaseCharacter *characterToAffect) override;
	virtual void DoTrigger(AActor* actor) override {};

private:
	UNiagaraSystem *assetToSpawn;
};
