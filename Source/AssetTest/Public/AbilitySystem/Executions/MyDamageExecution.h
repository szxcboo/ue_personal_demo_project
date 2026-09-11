#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MyDamageExecution.generated.h"

/**
 * Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class ASSETTEST_API UMyDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UMyDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};