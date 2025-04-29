#include "MagicPushBackAttribute.h"
#include "Movement/StateMachines/BaseCharacterMovementStateMachine.h"

MagicPushbackAttribute::MagicPushbackAttribute(ABaseMagic *owner, EMagicAttributeTarget target, float pushBackAmount, int priority)
	: BaseMagicAttribute(owner, target, false, priority, 50)
{
	impulseMultiplier = pushBackAmount;
}

void MagicPushbackAttribute::DoTrigger(BaseCharacter *characterToAffect)
{
	if (characterToAffect->GetPawnSelf()->IsValidLowLevelFast())
		characterToAffect->CharacterMovementSM->AddImpulse(characterToAffect->GetPawnSelf()->GetActorForwardVector() * impulseMultiplier * -1.0f, true);
}
