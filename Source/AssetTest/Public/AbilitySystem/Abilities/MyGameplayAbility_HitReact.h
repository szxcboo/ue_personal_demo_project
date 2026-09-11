#pragma once

#include "CoreMinimal.h"
#include "MyAbilityBase.h"
#include "MyGameplayAbility_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyGameplayAbility_HitReact : public UMyAbilityBase
{
	GENERATED_BODY()

public:
	UMyGameplayAbility_HitReact();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void HitReact(FGameplayEventData Payload);
};
