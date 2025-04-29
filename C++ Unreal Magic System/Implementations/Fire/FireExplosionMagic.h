#pragma once

#include "Magic/MagicTypes/FireTypeMagic.h"
#include "Magic/MagicStateMachine/PrePostMagicStateMachine.h"
#include "FireExplosionMagic.generated.h"

class USphereComponent; 

/*This is a magic attack that starts at a point and expands out damaging everything with an AOE*/
UCLASS()
class LITTLETALKS_API AFireExplosionMagic : public AFireTypeMagic
{
	GENERATED_BODY()

public:
	AFireExplosionMagic();

	virtual void BeginPlay() override;

protected:
	virtual void SetCollision(bool enabled);

	//Variables
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USphereComponent* ExplosionRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		FPrePostMSMProps StateMachineProperties;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		float EndScale;
};
