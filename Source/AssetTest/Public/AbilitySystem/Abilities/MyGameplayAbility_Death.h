#pragma once

#include "CoreMinimal.h"
#include "MyAbilityBase.h"
#include "MyGameplayAbility_Death.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyGameplayAbility_Death : public UMyAbilityBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void HandleDeath();
};
