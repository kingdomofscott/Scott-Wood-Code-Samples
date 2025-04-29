#include "MagicMoveActorAlongPathAttribute.h"
#include "Components/SplineComponent.h"
#include "Framework/Interfaces/PhysicsMovable.h"
#include "Movement/StateMachines/BaseCharacterMovementStateMachine.h"

MagicMoveActorAlongPathAttribute::MagicMoveActorAlongPathAttribute(ABaseMagic *owner, EMagicAttributeTarget target, USplineComponent *path,
	bool releaseOnComplete, float endImpulseAmount, bool affectActorOnce, int priority)
	: BaseMagicAttribute(owner, target, affectActorOnce, priority, 50)
{
	pathToMoveOn = path;
	impulseMultiplier = endImpulseAmount;
	releaseActors = releaseOnComplete;
}

void MagicMoveActorAlongPathAttribute::Update(float dTime)
{
	TArray<int> completedActors;

	for (int i = 0; i < controlledActors.Num(); ++i)
	{
		if (controlledActors[i].UpdatePosition)
		{
			UpdateActorProgress(i, dTime);
			FVector newPositionOnPath = controlledActors[i].StartingPosition;
			newPositionOnPath += pathToMoveOn->GetLocationAtTime(controlledActors[i].Progress, ESplineCoordinateSpace::Local);

			controlledActors[i].ActorMoving->SetActorLocation(newPositionOnPath);

			if (HasActorFinished(i))
			{
				controlledActors[i].UpdatePosition = false;

				if (releaseActors)
				{
					completedActors.Add(i);

					FreeActor(controlledActors[i].ActorMoving, true);
				}
			}
		}
	}

	for (int i = completedActors.Num() - 1; i >= 0; --i)
	{
		controlledActors.RemoveAt(completedActors[i]);
	}
}

void MagicMoveActorAlongPathAttribute::Finish()
{
	BaseMagicAttribute::Finish();

	for (int i = 0; i < controlledActors.Num(); ++i)
		FreeActor(controlledActors[i].ActorMoving, false);

	controlledActors.Empty();
}

void MagicMoveActorAlongPathAttribute::DoTrigger(BaseCharacter *characterToAffect)
{
	if (AddActor(characterToAffect->GetPawnSelf()))
		characterToAffect->SetMovementBlockedExternally(true);
}

void MagicMoveActorAlongPathAttribute::DoTrigger(AActor *actor)
{
	if (AddActor(actor) && actor->GetClass()->ImplementsInterface(UPhysicsMovable::StaticClass()))
		IPhysicsMovable::Execute_SetMovementBlockedExternally(actor, true);
}

void MagicMoveActorAlongPathAttribute::UpdateActorProgress(int index, float dTime)
{
	controlledActors[index].Progress = FMath::Clamp(controlledActors[index].Progress + dTime, 0.0f, pathToMoveOn->Duration);
}

bool MagicMoveActorAlongPathAttribute::HasActorFinished(int index)
{
	return controlledActors[index].Progress >= pathToMoveOn->Duration;
}

bool MagicMoveActorAlongPathAttribute::AddActor(AActor *actor)
{
	for (int i = 0; i < controlledActors.Num(); ++i)
		if (controlledActors[i].ActorMoving == actor)
			return false;

	ActorOnPath newControlledActor;
	newControlledActor.ActorMoving = actor;
	newControlledActor.Progress = 0.0f;
	newControlledActor.StartingPosition = actor->GetActorLocation();
	newControlledActor.UpdatePosition = true;
	controlledActors.Add(newControlledActor);

	return true;
}

void MagicMoveActorAlongPathAttribute::FreeActor(AActor *actor, bool addImpulse)
{
	if (actor->GetClass()->ImplementsInterface(UPhysicsMovable::StaticClass()))
	{
		if (actor->HasActorBegunPlay())
			IPhysicsMovable::Execute_SetMovementBlockedExternally(actor, false);
	}
	else
	{
		BaseCharacter *character = dynamic_cast<BaseCharacter *>(actor);

		if (character && actor->HasActorBegunPlay())
		{
			character->SetMovementBlockedExternally(false);

			if (addImpulse)
				character->CharacterMovementSM->AddImpulse(pathToMoveOn->GetDirectionAtTime(pathToMoveOn->Duration, ESplineCoordinateSpace::Local) * impulseMultiplier);
		}
	}
}
