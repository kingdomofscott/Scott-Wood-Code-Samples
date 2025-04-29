#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Framework/Interfaces/ActorCollisionID.h"
#include "Combat/Interfaces/DamageCauser.h"
#include "Magic/MagicAttributes/BaseMagicAttribute.h"
#include "Framework/Interfaces/MagicInteractable.h"
#include "BaseMagic.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChanged, uint8, newActiveState);

class BaseMagicStateMachine;
class BaseCharacter; 
class ABaseAttackable; 
class UBoxComponent; 
class UNiagaraComponent;

UCLASS()
class LITTLETALKS_API ABaseMagic : public AActor, public IActorCollisionID, public IDamageCauser, public IMagicInteractable
{
	GENERATED_BODY()

public:
	ABaseMagic();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*Apply any of the stored effects onto a target*/
	UFUNCTION(BlueprintCallable)
		void Trigger(AActor *actorToAffect);

	void TriggerAttributes(AActor *actorToAffect, EMagicAttributeTarget target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		uint8 GetCurrentState();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsStateMachineFinished();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsPendingDestroy() { return pendingDestroy; }

	//IActorCollisionID Implementaion
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		EActorCollisionType GetCollisionType();
	EActorCollisionType GetCollisionType_Implementation();

	//IDamageCauser Implementation
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool GuardAgainst(AActor *guardAgainst);
	virtual bool GuardAgainst_Implementation(AActor *guardAgainst) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool ParryAgainst(AActor *parryAgainst);
	virtual bool ParryAgainst_Implementation(AActor *parryAgainst) override { return true; };

	//IMagicInteractable Implementation
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool OnInteract(EMagicType magicType, ABaseMagic *interactingMagic);
	virtual bool OnInteract_Implementation(EMagicType magicType, ABaseMagic *interactingMagic) override { return false; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		EMagicType GetMagicType();
	EMagicType GetMagicType_Implementation() override { return MagicType; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool WillHaveReaction(EMagicType magicType, ABaseMagic *interactingMagic);
	virtual bool WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic *interactingMagic) override { return false; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool ShouldApplyAttributes(ABaseMagic *interactingMagic);
	virtual bool ShouldApplyAttributes_Implementation(ABaseMagic *interactingMagic) override { return false; };

protected:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable)
		void SpawnNewMagicDulicate(TSubclassOf<ABaseMagic> magic);

	UFUNCTION(BlueprintCallable)
		virtual void SetCollision(bool enabled);

private:
	void SetUpdateAttributes(bool shouldUpdateAttributes);

	//Variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Magic)
		ABaseAttackable *OwningAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Magic)
		uint8 Attack;

	UPROPERTY(BlueprintReadOnly, Category = Magic)
		EMagicType MagicType;

	UPROPERTY(BlueprintAssignable)
		FStateChanged OnStateChanged;

	UPROPERTY(EditAnywhere, Category = Trigger)
		bool TriggerOnCollide;

	UPROPERTY(EditDefaultsOnly, Category = Trigger)
		bool UseCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		UNiagaraComponent *MagicParticles;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		bool AutoActivateParticles;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UBoxComponent *collider;

	BaseMagicStateMachine *magicStateMachine;
	TArray<BaseMagicAttribute *> magicAttributes;

private:
	uint8 prevCurrentState;
	bool pendingDestroy;
	bool updateAttributes;
};
