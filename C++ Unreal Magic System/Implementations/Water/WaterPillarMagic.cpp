#include "WaterPillarMagic.h"
#include "Character/BasePlayableCharacter.h"
#include "Combat/Attackables/BaseAttackable.h"
#include "Components/SplineComponent.h"
#include "Magic/MagicAttributes/Combat/MagicDealDamageAttribute.h"
#include "Magic/MagicAttributes/Combat/MagicPushbackAttribute.h"
#include "Magic/MagicAttributes/Exploration/MagicMoveActorAlongPathAttribute.h"
#include "Magic/MagicAttributes/Exploration/MagicDelayedLoopMoveActorAlongPathAttribute.h"
#include "Magic/MagicAttributes/Combat/MagicScaleOverTimeAttribute.h"
#include "Components/BoxComponent.h"

AWaterPillarMagic::AWaterPillarMagic()
{
	StateMachineProperties.spawnTime = 0.5f;
	StateMachineProperties.lifetime = 5.0f;
	StateMachineProperties.deathTime = 0.5f;

	CollisionHeightScale = 10.0f;
	WhenIsMaxScale = 1.0f;

	movementSpline = CreateDefaultSubobject<USplineComponent>(FName("Movement Path"));
	movementSpline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bounceSpline = CreateDefaultSubobject<USplineComponent>(FName("Bounce Path"));
	bounceSpline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	magicAttributes.Add((BaseMagicAttribute *)new MagicDealDamageAttribute(this, true));
}

void AWaterPillarMagic::BeginPlay()
{
	ABaseMagic::BeginPlay();

	magicStateMachine = new PrePostMagicStateMachine(StateMachineProperties.spawnTime, StateMachineProperties.lifetime, StateMachineProperties.deathTime);

	bool forCombat = true;
	ABasePlayableCharacter *playableCharacter = (ABasePlayableCharacter*)(OwningAttack->OwningCharacter);

	if (playableCharacter)
	{
		AActor *playableTarget = playableCharacter->GetCurrentTarget();
		BaseCharacter *enemy = (playableTarget) ? dynamic_cast<BaseCharacter *>(playableTarget) : nullptr;

		if (!enemy)//If we are targetting something that is non enemy then only push up the object actors
			forCombat = false;
	}

	EMagicAttributeTarget raiseTarget = EMagicAttributeTarget::Enemy;
	EMagicAttributeTarget pushTarget = EMagicAttributeTarget::Ally;

	if (!forCombat)
	{
		raiseTarget = EMagicAttributeTarget::Objects;
		pushTarget = (EMagicAttributeTarget)((uint8)pushTarget | (uint8)EMagicAttributeTarget::Enemy);
	}

	magicAttributes.Add((BaseMagicAttribute *)new MagicMoveActorAlongPathAttribute(this, raiseTarget, movementSpline, false, 0.0f));
	magicAttributes.Add((BaseMagicAttribute *)new MagicDelayedLoopMoveActorAlongPathAttribute(this, raiseTarget, bounceSpline, 0.0f, movementSpline->Duration));
	magicAttributes.Add((BaseMagicAttribute *)new MagicPushbackAttribute(this, pushTarget, 1000.0f));
	magicAttributes.Add((BaseMagicAttribute *)new MagicScaleOverTimeAttribute(this, StateMachineProperties.spawnTime, WhenIsMaxScale,
		CollisionHeightScale, collider, FVector(0.0f, 0.0f, 1.0f)));
}
