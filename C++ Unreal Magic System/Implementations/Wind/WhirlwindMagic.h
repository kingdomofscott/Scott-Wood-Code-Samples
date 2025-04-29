#pragma once

#include "Magic/MagicTypes/WindTypeMagic.h"
#include "Magic/MagicStateMachine/PrePostMagicStateMachine.h"
#include "WhirlwindMagic.generated.h"

class USplineComponent;

/*This is a magic attack that spawns a whirlwind that will grab an enemy and fling them along a spiral path. Alternatively, the player can use this for a boosted jump*/
UCLASS()
class LITTLETALKS_API AWhirlwindMagic : public AWindTypeMagic
{
	GENERATED_BODY()

public:
	AWhirlwindMagic();

	virtual void BeginPlay() override;

	//Variables
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		FPrePostMSMProps StateMachineProperties;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USplineComponent *twirlSpline;
};