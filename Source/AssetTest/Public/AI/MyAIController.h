#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "MyAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class ASSETTEST_API AMyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyAIController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
