#pragma once

#include "Magic/BaseMagic.h"
#include "FireTypeMagic.generated.h"

/*This is the base class for all fire type magics*/
UCLASS()
class LITTLETALKS_API AFireTypeMagic : public ABaseMagic
{
	GENERATED_BODY()

public:
	AFireTypeMagic();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		TSubclassOf<ABaseMagic> GetFireExplosionMagic();
	virtual TSubclassOf<ABaseMagic> GetFireExplosionMagic_Implementation() { return nullptr; };

	//IMagicInteractable
	virtual bool OnInteract_Implementation(EMagicType magicType, ABaseMagic* interactingMagic) override;
	virtual bool WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic* interactingMagic) override;
};
