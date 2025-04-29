#pragma once

#include "CoreMinimal.h"
#include "BaseMagicStateMachine.h"
#include "PrePostMagicStateMachine.generated.h"

enum class EPrePostMagicStates : uint8
{
	PRE_CAST,
	CAST,
	POST_CAST
};

/*
This is a state machine that has a beginning->middle->end. 
Mostly this is for visuals. Time to start up, runtime, and then disappear time
*/
class LITTLETALKS_API PrePostMagicStateMachine : public BaseMagicStateMachine
{
public:
	PrePostMagicStateMachine(float preCastLife, float castLife, float postCastLife);
	virtual void TransitionState() override;
};

USTRUCT(BlueprintType)
struct LITTLETALKS_API FPrePostMSMProps
{
	GENERATED_BODY()

public:
	/*How long does it need to start*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) 
		float spawnTime;
	/*How long does it live*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) 
		float lifetime;
	/*How long does it need to die*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) 
		float deathTime;
};
