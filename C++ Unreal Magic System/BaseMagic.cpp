#include "BaseMagic.h"
#include "Abilities/CharacterAbilityList.h"
#include "Combat/Attackables/BaseAttackable.h"
#include "Components/BoxComponent.h"
#include "LittleTalks.h"
#include "MagicStateMachine/BaseMagicStateMachine.h"
#include "NiagaraComponent.h"

ABaseMagic::ABaseMagic()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicType = EMagicType::NONE;
	prevCurrentState = -1;
	pendingDestroy = false;
	TriggerOnCollide = true;
	updateAttributes = false;
	UseCollider = true;
	AutoActivateParticles = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	collider = CreateDefaultSubobject<UBoxComponent>(FName("Collider"));
	collider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SetCollision(false);

	MagicParticles = CreateDefaultSubobject<UNiagaraComponent>(FName("Magic Particles"));
	MagicParticles->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ABaseMagic::BeginPlay()
{
	Super::BeginPlay();

	if (UseCollider)
		collider->OnComponentBeginOverlap.AddDynamic(this, &ABaseMagic::OnOverlapBegin);

	if (AutoActivateParticles)
		MagicParticles->Activate();
}

void ABaseMagic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (magicStateMachine && !pendingDestroy)
	{
		magicStateMachine->Update(DeltaTime);

		uint8 currState = GetCurrentState();
		if (prevCurrentState != currState)
		{
			OnStateChanged.Broadcast(currState);
			prevCurrentState = currState;

			BaseMagicState newState = magicStateMachine->GetCurrentState();

			if (updateAttributes != newState.shouldApplyAttributes)
				SetUpdateAttributes(newState.shouldApplyAttributes);
		}
		else if (magicStateMachine->IsStateMachineFinished)
		{
			for (int i = 0; i < magicAttributes.Num(); ++i)
			{
				if (magicAttributes[i])
				{
					magicAttributes[i]->Finish();
					delete magicAttributes[i];
				}
			}

			magicAttributes.Empty();

			Destroy();
			pendingDestroy = true;
		}

		if (!pendingDestroy && updateAttributes)
		{
			for (int i = 0; i < magicAttributes.Num(); ++i)
				magicAttributes[i]->Update(DeltaTime);
		}
	}
}

void ABaseMagic::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (magicStateMachine)
		delete magicStateMachine;

	Super::EndPlay(EndPlayReason);
}

void ABaseMagic::Trigger(AActor *actorToAffect)
{
	if (OwningAttack && OwningAttack->OwningCharacter)
	{
		BaseCharacter *castedAffectChar = nullptr;

		if (Cast<APawn>(actorToAffect))
			castedAffectChar = dynamic_cast<BaseCharacter *>(actorToAffect);

		if (actorToAffect->GetClass()->ImplementsInterface(UMagicInteractable::StaticClass())) //Affect random object
		{
			EMagicType otherType = IMagicInteractable::Execute_GetMagicType(actorToAffect);

			if (IMagicInteractable::Execute_WillHaveReaction(actorToAffect, MagicType, this))
				IMagicInteractable::Execute_OnInteract(actorToAffect, MagicType, this);

			if (IMagicInteractable::Execute_ShouldApplyAttributes(actorToAffect, this))
				TriggerAttributes(actorToAffect, EMagicAttributeTarget::Objects);

			ABaseMagic *possibleMagic = Cast<ABaseMagic>(actorToAffect);

			if (WillHaveReaction(otherType, possibleMagic))
				OnInteract(otherType, possibleMagic);
		}
		else if (castedAffectChar)
		{
			if (castedAffectChar->TargetGroup() != OwningAttack->OwningCharacter->TargetGroup())//Affect enemy group
				TriggerAttributes(castedAffectChar->GetPawnSelf(), EMagicAttributeTarget::Enemy);

			if (castedAffectChar->TargetGroup() == OwningAttack->OwningCharacter->TargetGroup())
				TriggerAttributes(castedAffectChar->GetPawnSelf(), EMagicAttributeTarget::Ally);
		}
	}
}

void ABaseMagic::TriggerAttributes(AActor *actorToAffect, EMagicAttributeTarget target)
{
	magicAttributes.Sort([](BaseAbilityAttribute &A, BaseAbilityAttribute &B) {return A.AttributePriority > B.AttributePriority; });

	for (int i = 0; i < magicAttributes.Num(); ++i)
	{
		if (magicAttributes[i])
		{
			BaseCharacter *castedAffectChar = dynamic_cast<BaseCharacter *>(actorToAffect);

			if ((uint8)magicAttributes[i]->AttributeTarget & (uint8)target)
			{
				if (castedAffectChar)
					magicAttributes[i]->Trigger(castedAffectChar);
				else
					magicAttributes[i]->Trigger(actorToAffect);
			}
		}
		else
		{
			UE_LOG(LTMagic, Error, TEXT("Attempting to trigger attributes for %s with a nullptr"), *GetName());
			checkNoEntry();
		}
	}
}

uint8 ABaseMagic::GetCurrentState()
{
	if (magicStateMachine)
		return magicStateMachine->CurrentState;

	return 0xff;
}

bool ABaseMagic::IsStateMachineFinished()
{
	if (magicStateMachine)
		return magicStateMachine->IsStateMachineFinished;

	return false;
}

EActorCollisionType ABaseMagic::GetCollisionType_Implementation()
{
	return EActorCollisionType::MAGIC;
}

void ABaseMagic::OnOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (TriggerOnCollide)
		Trigger(OtherActor);
}

void ABaseMagic::SpawnNewMagicDulicate(TSubclassOf<ABaseMagic> magic)
{
	ABaseMagic *newMagic = GetWorld()->SpawnActorDeferred<ABaseMagic>(magic, GetActorTransform());

	newMagic->OwningAttack = OwningAttack;
	newMagic->Attack = Attack;

	newMagic->FinishSpawning(GetActorTransform());
}

void ABaseMagic::SetCollision(bool enabled)
{
	ECollisionEnabled::Type collisionType;

	if (enabled)
		collisionType = ECollisionEnabled::Type::QueryOnly;
	else
		collisionType = ECollisionEnabled::Type::NoCollision;

	collider->SetCollisionEnabled(collisionType);
}

void ABaseMagic::SetUpdateAttributes(bool shouldUpdateAttributes)
{
	updateAttributes = shouldUpdateAttributes;

	for (int i = 0; i < magicAttributes.Num(); ++i)
	{
		if (updateAttributes)
			magicAttributes[i]->Start();
		else
			magicAttributes[i]->Finish();
	}

	SetCollision(updateAttributes);
}

bool ABaseMagic::GuardAgainst_Implementation(AActor *guardAgainst)
{
	FMagicAbility attackInfo;

	if (!OwningAttack->OwningCharacter->CharacterAbilities->GetMagicAbilityInfo(Attack, attackInfo))
	{
		UE_LOG(LTCombat, Warning, TEXT("Failed to get attack to check: %s %d"), *OwningAttack->OwningCharacter->GetPawnSelf()->GetName(), Attack);
		return false;
	}
	else
	{
		return attackInfo.CanBeGuarded;
	}
}
