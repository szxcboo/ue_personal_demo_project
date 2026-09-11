#include "AbilitySystem/Task/MyAttributeChangeTask.h"

#include "AbilitySystemComponent.h"

UMyAttributeChangeTask* UMyAttributeChangeTask::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UMyAttributeChangeTask* WaitForAttributeChangeTask = NewObject<UMyAttributeChangeTask>();
	WaitForAttributeChangeTask->AbilitySystemComponent = AbilitySystemComponent;
	WaitForAttributeChangeTask->AttributeToListenFor = Attribute;

	if (!IsValid(AbilitySystemComponent))
	{
		WaitForAttributeChangeTask->RemoveFromRoot(); // Remove it from the root set as we just create a new object, so that it can be garbage collected
		return nullptr;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(WaitForAttributeChangeTask, &ThisClass::AttributeChanged);

	return WaitForAttributeChangeTask;
}

void UMyAttributeChangeTask::EndTask()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this); // Removing all callbacks that this class has bound to the delegate
	}

	SetReadyToDestroy(); // From UBlueprintAsyncActionBase
	MarkAsGarbage(); // Will trigger replication and clean up any resources that need to be cleaned up
}

void UMyAttributeChangeTask::AttributeChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.NewValue <= 0.f)
	{
		OnAttributeChanged.Broadcast(AttributeChangeData.Attribute, AttributeChangeData.NewValue, AttributeChangeData.OldValue);
	}
}
