#include "MagicJumpBoostAttribute.h"
#include "Movement/StateMachines/BaseCharacterMovementStateMachine.h"

MagicJumpBoostAttribute::MagicJumpBoostAttribute(ABaseMagic *owner, float impulseAmount, int priority) :
	BaseMagicAttribute(owner, EMagicAttributeTarget::Ally, false, priority, 50)
{
	impulseMultiplier = impulseAmount;
}

void MagicJumpBoostAttribute::DoTrigger(BaseCharacter *characterToAffect)
{
	characterToAffect->CharacterMovementSM->AddImpulse(FVector::UpVector * impulseMultiplier);
}
