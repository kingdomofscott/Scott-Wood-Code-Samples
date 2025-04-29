#include"MagicScaleOverTimeAttribute.h"
#include "Components/SceneComponent.h"
#include "CoreMinimal.h"

MagicScaleOverTimeAttribute::MagicScaleOverTimeAttribute(ABaseMagic* owner, float startDelay, float howLong, float finalScale,
	USceneComponent* componentToScale, FVector valuesToScaleOn, int priority)
	: BaseMagicAttribute(owner, EMagicAttributeTarget::Enemy, false, priority, 50)
{
	scalingComponent = componentToScale;

	delayTime = startDelay;
	howLongToScale = howLong;
	currentTime = 0.0f;
	startScale = scalingComponent->GetComponentScale().X;
	endScale = finalScale;
	scaleRates = valuesToScaleOn;
}

void MagicScaleOverTimeAttribute::Start()
{
	BaseMagicAttribute::Start();

	currentTime = 0.0f;
}

void MagicScaleOverTimeAttribute::Update(float dTime)
{
	currentTime += dTime;

	if (currentTime >= delayTime)
		scalingComponent->SetRelativeScale3D(FVector::OneVector + scaleRates * FMath::Lerp(startScale, endScale, FMath::Clamp((currentTime - delayTime) / howLongToScale, 0.0f, 1.0f)));
}
