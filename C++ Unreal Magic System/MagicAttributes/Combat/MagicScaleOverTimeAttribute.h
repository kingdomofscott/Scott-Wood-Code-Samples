#pragma once

#include "Magic/MagicAttributes/BaseMagicAttribute.h"

class USceneComponent;

/*This is a basic attribute that is put onto any magic attacks that change scale over time*/
class LITTLETALKS_API MagicScaleOverTimeAttribute : BaseMagicAttribute
{
public:
	MagicScaleOverTimeAttribute(ABaseMagic* owner, float startDelay, float howLong, float finalScale, 
		USceneComponent* componentToScale, FVector valuesToScaleOn = FVector::OneVector, int priority = -1);
	virtual void Start() override;
	virtual void Update(float dTime) override;

	virtual void DoTrigger(BaseCharacter* characterToAffect) override {};
	virtual void DoTrigger(AActor* actor) override {};

private:
	USceneComponent* scalingComponent;

	float delayTime;
	float howLongToScale;
	float currentTime;
	float startScale;
	float endScale;
	FVector scaleRates;
};
