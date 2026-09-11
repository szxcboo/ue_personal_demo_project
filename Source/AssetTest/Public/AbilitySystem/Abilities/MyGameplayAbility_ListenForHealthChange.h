#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyGameplayAbility_ListenForHealthChange.generated.h"

class UMyAttributeChangeTask;

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyGameplayAbility_ListenForHealthChange : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMyGameplayAbility_ListenForHealthChange();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void HandleActivateDeath(FGameplayAttribute Attribute, float NewValue, float OldValue);

	UPROPERTY()
	UMyAttributeChangeTask* ListenForAttributeChange;
};
