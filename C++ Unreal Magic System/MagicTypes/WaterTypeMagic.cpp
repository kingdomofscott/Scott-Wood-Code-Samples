#include "WaterTypeMagic.h"

AWaterTypeMagic::AWaterTypeMagic()
{
	MagicType = EMagicType::WATER;
}

bool AWaterTypeMagic::OnInteract_Implementation(EMagicType magicType, ABaseMagic* interactingMagic)
{
	TSubclassOf<ABaseMagic> newMagic = nullptr;

	switch (magicType)
	{
	case EMagicType::ICE:
		return true;
		break;
	}

	return false;
}

bool AWaterTypeMagic::WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic* interactingMagic)
{
	switch (magicType)
	{
	case EMagicType::ICE:
		return true;
		break;
	}

	return false;
}
