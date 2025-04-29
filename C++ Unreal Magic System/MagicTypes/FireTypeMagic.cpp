#include "FireTypeMagic.h"
#include "LittleTalks.h"

AFireTypeMagic::AFireTypeMagic()
{
	MagicType = EMagicType::FIRE;
}

bool AFireTypeMagic::OnInteract_Implementation(EMagicType magicType, ABaseMagic* interactingMagic)
{
	TSubclassOf<ABaseMagic> newMagic = nullptr;

	switch (magicType)
	{
	case EMagicType::WIND:
		newMagic = GetFireExplosionMagic();

		if (newMagic)
			SpawnNewMagicDulicate(newMagic);
		else
			UE_LOG(LTMagic, Warning, TEXT("Failed to get fire explosion magic override: %s"), *GetName());

	case EMagicType::WATER:
	case EMagicType::ICE:
		Destroy();
		return true;
		break;
	}

	return false;
}

bool AFireTypeMagic::WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic* interactingMagic)
{
	switch (magicType)
	{
	case EMagicType::WATER:
	case EMagicType::ICE:
	case EMagicType::WIND:
		return true;
		break;
	}

	return false;
}
