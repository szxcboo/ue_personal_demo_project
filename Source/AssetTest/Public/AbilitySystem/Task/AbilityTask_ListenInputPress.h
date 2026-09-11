#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ListenInputPress.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputPressDelegate);

/**
 * 
 */
UCLASS()
class ASSETTEST_API UAbilityTask_ListenInputPress : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FInputPressDelegate OnPress;

	UFUNCTION()
	void OnPressCallback();

	/** Listen for the user presses the input button for this ability's activation. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ListenInputPress* ListenInputPress(UGameplayAbility* OwningAbility, bool bTestAlreadyPressed = false);

	virtual void Activate() override;

	bool bTestInitialState = false;
	FDelegateHandle DelegateHandle;
};
