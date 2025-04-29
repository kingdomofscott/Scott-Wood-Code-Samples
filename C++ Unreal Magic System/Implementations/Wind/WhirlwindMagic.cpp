#include "WhirlwindMagic.h"
#include "Magic/MagicStateMachine/BaseMagicStateMachine.h"
#include "Magic/MagicAttributes/Exploration/MagicJumpBoostAttribute.h"
#include "Magic/MagicAttributes/Exploration/MagicMoveActorAlongPathAttribute.h"
#include "Magic/MagicAttributes/Combat/MagicDealDamageOverTimeAttribute.h"
#include "Components/SplineComponent.h"

AWhirlwindMagic::AWhirlwindMagic()
{
	StateMachineProperties.spawnTime = 0.25f;
	StateMachineProperties.lifetime = 5.0f;
	StateMachineProperties.deathTime = 0.25f;

	twirlSpline = CreateDefaultSubobject<USplineComponent>(FName("Twirl Path"));
	twirlSpline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	magicAttributes.Add((BaseMagicAttribute *)new MagicJumpBoostAttribute(this, 1250.0f));
	magicAttributes.Add((BaseMagicAttribute *)new MagicMoveActorAlongPathAttribute(this, EMagicAttributeTarget::Enemy, twirlSpline, true, 750.0f, true));
	magicAttributes.Add((BaseMagicAttribute *)new MagicDealDamageOverTimeAttribute(this, twirlSpline->Duration, (twirlSpline->Duration - 0.1) / 3.0f));
}

void AWhirlwindMagic::BeginPlay()
{
	ABaseMagic::BeginPlay();

	magicStateMachine = new PrePostMagicStateMachine(StateMachineProperties.spawnTime, StateMachineProperties.lifetime, StateMachineProperties.deathTime);
}
