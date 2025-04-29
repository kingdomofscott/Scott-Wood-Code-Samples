#include "FireExplosionMagic.h"
#include "Magic/MagicAttributes/Combat/MagicDealDamageAttribute.h"
#include "Magic/MagicAttributes/Combat/MagicScaleOverTimeAttribute.h"
#include "Components/SphereComponent.h"

AFireExplosionMagic::AFireExplosionMagic()
{
	StateMachineProperties.spawnTime = 0.5f;
	StateMachineProperties.lifetime = 3.0f;
	StateMachineProperties.deathTime = 0.5f;

	EndScale = 5.0f;

	ExplosionRadius = CreateDefaultSubobject<USphereComponent>(FName("Explosion Radius"));
	ExplosionRadius->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AFireExplosionMagic::BeginPlay()
{
	ABaseMagic::BeginPlay();

	magicStateMachine = new PrePostMagicStateMachine(StateMachineProperties.spawnTime, StateMachineProperties.lifetime, StateMachineProperties.deathTime);

	magicAttributes.Add((BaseMagicAttribute*)new MagicDealDamageAttribute(this));
	magicAttributes.Add((BaseMagicAttribute*)new MagicScaleOverTimeAttribute(this, StateMachineProperties.spawnTime, StateMachineProperties.lifetime, 
		EndScale, ExplosionRadius));

	ExplosionRadius->OnComponentBeginOverlap.AddDynamic(this, &AFireExplosionMagic::OnOverlapBegin);
}

void AFireExplosionMagic::SetCollision(bool enabled)
{
	ECollisionEnabled::Type collisionType;

	if (enabled)
		collisionType = ECollisionEnabled::Type::QueryOnly;
	else
		collisionType = ECollisionEnabled::Type::NoCollision;

	ExplosionRadius->SetCollisionEnabled(collisionType);
}
