#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyGameplayAbility_Damage.h"
#include "MyGameplayAbility_Combo.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInputReceived, FGameplayEventData, Payload);

class UAbilityTask_PlayMontageAndWait;

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyGameplayAbility_Combo : public UMyGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	/** Input Cache */

	UFUNCTION()
	void OnInputCacheStart(FGameplayEventData Payload);

	UFUNCTION()
	void OnInputCacheEnd(FGameplayEventData Payload);

	FOnInputReceived OnInputReceivedDelegate;

	UFUNCTION()
	void OnInputReceived(FGameplayEventData Payload);

	/** Transition Window */

	UFUNCTION()
	void OnTransitionWindowOpen(FGameplayEventData Payload);

	UFUNCTION()
	void OnTransitionWindowClose(FGameplayEventData Payload);

	/** Jump Section */

	UFUNCTION(BlueprintCallable)
	FName JumpToNextSection(FGameplayEventData Payload);

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FName> JumpSectionMap;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayMontageAndWait;

	bool bCanReceiveInput = false;
	bool bCanJumpSection = false;
};
