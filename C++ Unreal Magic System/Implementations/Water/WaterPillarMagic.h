#pragma once

#include "Magic/MagicTypes/WaterTypeMagic.h"
#include "Magic/MagicStateMachine/PrePostMagicStateMachine.h"
#include "WaterPillarMagic.generated.h"

class USplineComponent;

/*This is a magic attack that starts at a location on the floor and bursts up, carrying any enemies with it and holding them in place*/
UCLASS()
class LITTLETALKS_API AWaterPillarMagic : public AWaterTypeMagic
{
	GENERATED_BODY()

public:
	AWaterPillarMagic();

	virtual void BeginPlay() override;

	//Variables
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		FPrePostMSMProps StateMachineProperties;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		float CollisionHeightScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		float WhenIsMaxScale;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USplineComponent *movementSpline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USplineComponent *bounceSpline;
};
