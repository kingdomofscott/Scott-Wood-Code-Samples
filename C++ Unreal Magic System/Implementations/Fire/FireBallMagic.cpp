#include "FireBallMagic.h"
#include "Magic/MagicAttributes/Combat/MagicDealDamageAttribute.h"
#include "Magic/MagicAttributes/Misc/SpawnParticleEffectAttribute.h"
#include "Magic/MagicAttributes/Misc/MagicDieOnImpactAttribute.h"

AFireBallMagic::AFireBallMagic()
{
	StateMachineProperties.spawnTime = 0.5f;
	StateMachineProperties.lifetime = 3.0f;
	StateMachineProperties.deathTime = 0.5f;

	magicAttributes.Add((BaseMagicAttribute *)new MagicDealDamageAttribute(this, false, ""));
	magicAttributes.Add((BaseMagicAttribute *)new SpawnParticleEffectAttribute(this, "ParticleSystem'/Game/Add-Ons/StylizedFX_2/ParticleSystems/P_Beam_Hit.P_Beam_Hit'", false));
	magicAttributes.Add((BaseMagicAttribute *)new MagicDieOnImpactAttribute(this));
}

void AFireBallMagic::BeginPlay()
{
	ABaseMagic::BeginPlay();

	magicStateMachine = new PrePostMagicStateMachine(StateMachineProperties.spawnTime, StateMachineProperties.lifetime, StateMachineProperties.deathTime);
}
