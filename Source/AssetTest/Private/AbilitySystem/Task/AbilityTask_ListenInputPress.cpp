#include "AbilitySystem/Task/AbilityTask_ListenInputPress.h"

#include "AbilitySystemComponent.h"

void UAbilityTask_ListenInputPress::OnPressCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC) return;

	// Clean
	// ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
	
	// Start a prediction window
	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient()) // Is a locally predicted ability running on a client
	{
		// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	// Keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPress.Broadcast();
	}
}

UAbilityTask_ListenInputPress* UAbilityTask_ListenInputPress::ListenInputPress(UGameplayAbility* OwningAbility, bool bTestAlreadyPressed)
{
	UAbilityTask_ListenInputPress* Task = NewAbilityTask<UAbilityTask_ListenInputPress>(OwningAbility);
	Task->bTestInitialState = bTestAlreadyPressed;
	return Task;
}

void UAbilityTask_ListenInputPress::Activate()
{
	Super::Activate(); // Log only

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		// Test already pressed
		if (bTestInitialState && IsLocallyControlled()) // On the locally controlled client
		{
			FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec();
			if (Spec && Spec->InputPressed)
			{
				OnPressCallback();
			}
		}

		// Bind the Input Pressed Event
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnPressCallback);
		if (IsForRemoteClient()) // On the server for a non locally controlled client
		{
			if (!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey())) // Calls a given Generic Replicated Event delegate if the event has already been sent
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}
