#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MyAbilityTask_WaitNotifyStateEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitNotifyStateEventDelegate);

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAbilityTask_WaitNotifyStateEvent : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitNotifyStateEventDelegate BeginEventReceived;

	UPROPERTY(BlueprintAssignable)
	FWaitNotifyStateEventDelegate EndEventReceived;

	/**
	 * Wait until the specified gameplay tag event sent by Notify State is triggered. By default this will look at the owner of this ability.
	 * It will keep listening as long as OnlyTriggerOnce = false
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UMyAbilityTask_WaitNotifyStateEvent* WaitNotifyStateEvent(UGameplayAbility* OwningAbility, UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory"))FGameplayTag BeginEventTag, UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory"))FGameplayTag EndEventTag, bool OnlyTriggerOnce = false);

	virtual void Activate() override;

	void BeginEventCallback(const FGameplayEventData* Payload);
	void EndEventCallback(const FGameplayEventData* Payload);

	virtual void OnDestroy(bool bInOwnerFinished) override;

	FGameplayTag BeginEventTag;
	FGameplayTag EndEventTag;

	bool OnlyTriggerOnce;

	bool BeginEventTriggered;
	bool EndEventTriggered;

	FDelegateHandle MyHandle;
};
