// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Magic/MagicTypes/FireTypeMagic.h"
#include "Magic/MagicStateMachine/PrePostMagicStateMachine.h"
#include "FireBallMagic.generated.h"

/*This is a magic attack that spawns a fireball, travels in a direction, and on hit can spawn FireExplosionMagic*/
UCLASS()
class LITTLETALKS_API AFireBallMagic : public AFireTypeMagic
{
	GENERATED_BODY()

public:
	AFireBallMagic();

	virtual void BeginPlay() override;

	//Variables
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = StateMachine)
		FPrePostMSMProps StateMachineProperties;
};
