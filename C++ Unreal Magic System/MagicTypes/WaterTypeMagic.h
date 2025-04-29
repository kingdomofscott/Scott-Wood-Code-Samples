#pragma once

#include "Magic/BaseMagic.h"
#include "WaterTypeMagic.generated.h"

/*This is the base class for all water type magics*/
UCLASS()
class LITTLETALKS_API AWaterTypeMagic : public ABaseMagic
{
	GENERATED_BODY()

public:
	AWaterTypeMagic();

	//IMagicInteractable
	virtual bool OnInteract_Implementation(EMagicType magicType, ABaseMagic* interactingMagic) override;
	virtual bool WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic* interactingMagic) override;
};
