#include "WindTypeMagic.h"

AWindTypeMagic::AWindTypeMagic()
{
	MagicType = EMagicType::WIND;
}

bool AWindTypeMagic::OnInteract_Implementation(EMagicType magicType, ABaseMagic* interactingMagic)
{
	switch (magicType)
	{
	case EMagicType::FIRE:
		Destroy();
		return true;
		break;
	}

	return false;
}

bool AWindTypeMagic::WillHaveReaction_Implementation(EMagicType magicType, ABaseMagic* interactingMagic)
{
	switch (magicType)
	{
	case EMagicType::FIRE:
		return true;
	}

	return false;
}
