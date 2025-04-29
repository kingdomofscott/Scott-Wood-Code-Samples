#pragma once

#include "Magic/BaseMagic.h"
#include "WindTypeMagic.generated.h"

/*This is the base class for all wind type magics*/
UCLASS()
class LITTLETALKS_API AWindTypeMagic : public ABaseMagic
{
	GENERATED_BODY()

public:
	AWindTypeMagic();

	//IMagicInteractable
	virtual bool OnInteract_Implementation(EMagicType magicType, ABaseMagic* interactingMagic) override;
	virtual bool WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic* interactingMagic) override;
};
