#include "AbilitySystem/Task/MyAbilityTask_WaitNotifyStateEvent.h"

#include "AbilitySystemComponent.h"

UMyAbilityTask_WaitNotifyStateEvent* UMyAbilityTask_WaitNotifyStateEvent::WaitNotifyStateEvent(UGameplayAbility* OwningAbility, FGameplayTag BeginEventTag, FGameplayTag EndEventTag, bool OnlyTriggerOnce)
{
	UMyAbilityTask_WaitNotifyStateEvent* MyObj = NewAbilityTask<UMyAbilityTask_WaitNotifyStateEvent>(OwningAbility);
	MyObj->BeginEventTag = BeginEventTag;
	MyObj->EndEventTag = EndEventTag;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->BeginEventTriggered = false;
	MyObj->EndEventTriggered = false;

	return MyObj;
}

void UMyAbilityTask_WaitNotifyStateEvent::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		MyHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(BeginEventTag).AddUObject(this, &UMyAbilityTask_WaitNotifyStateEvent::BeginEventCallback);
		MyHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EndEventTag).AddUObject(this, &UMyAbilityTask_WaitNotifyStateEvent::EndEventCallback);
	}

	Super::Activate();
}

void UMyAbilityTask_WaitNotifyStateEvent::BeginEventCallback(const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		BeginEventReceived.Broadcast();
	}

	BeginEventTriggered = true;
	if (OnlyTriggerOnce && EndEventTriggered)
	{
		EndTask();
	}
}

void UMyAbilityTask_WaitNotifyStateEvent::EndEventCallback(const FGameplayEventData* Payload)
{
	// UE_LOG(LogTemp, Warning, TEXT("EndEvent"));

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		EndEventReceived.Broadcast();
	}

	EndEventTriggered = true;
	if (OnlyTriggerOnce && BeginEventTriggered)
	{
		EndTask();
	}
}

void UMyAbilityTask_WaitNotifyStateEvent::OnDestroy(bool bInOwnerFinished)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && MyHandle.IsValid())
	{
		ASC->GenericGameplayEventCallbacks.FindOrAdd(BeginEventTag).Remove(MyHandle);
		ASC->GenericGameplayEventCallbacks.FindOrAdd(EndEventTag).Remove(MyHandle);
	}

	Super::OnDestroy(bInOwnerFinished);
}
