#include "AbilitySystem/MyAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/MyAbilityBase.h"
#include "MyGameplayTags.h"

void UMyAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	if (!IsValid(AbilitySpec.Ability)) return;
	HandleAutoActivateAbility(AbilitySpec);
}

void UMyAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	// Lock the scope to prevent trying to access the ability already removed
	FScopedAbilityListLock ActiveScopedLock(*this);
	// Loop over all the activatable abilities and try to activate them if they have the tag
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivateAbility(AbilitySpec);
	}
}

// @ARPG: Abilities should be set to Instance per Execution!
void UMyAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UMyAbilityBase*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances(); // Returns all instances, which can include InstancedPerExecution abilities
		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<UMyAbilityBase>(ActiveAbility));
		}
	}
}

void UMyAbilitySystemComponent::GetActiveAbilitiesWithTag(const FGameplayTag& GameplayTag, TArray<UMyAbilityBase*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> ActivatableAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTag.GetSingleTagContainer(), ActivatableAbilitySpecs, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : ActivatableAbilitySpecs)
	{
		for (UGameplayAbility* Instance : Spec->GetAbilityInstances())
		{
			if (Instance->IsActive())
			{
				ActiveAbilities.Add(Cast<UMyAbilityBase>(Instance));
			}
		}
	}
}

void UMyAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		// AbilitySpec.GetDynamicSpecSourceTags().AddTag();
		GiveAbility(AbilitySpec);
	}
}

void UMyAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		// if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		if (Spec.Ability->AbilityTags.HasTagExact(InputTag))
		{
			// Mark the Ability Spec input pressed
			AbilitySpecInputPressed(Spec);
			if (Spec.IsActive())
			{
				// Invoke input pressed event if the ability is active
				UGameplayAbility* Instance = Spec.GetPrimaryInstance(); // Only valid on InstancedPerActor
				FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
			}
			else
			{
				TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UMyAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		// if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && Spec.IsActive())
		if (Spec.Ability->AbilityTags.HasTagExact(InputTag) && Spec.IsActive())
		{
			AbilitySpecInputReleased(Spec);
			UGameplayAbility* Instance = Spec.GetPrimaryInstance();
			FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
		}
	}
}

void UMyAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	unimplemented();
}

void UMyAbilitySystemComponent::HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!IsValid(AbilitySpec.Ability)) return;

	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTagExact(MyGameplayTags::Abilities::ActivateOnGiven))
		{
			TryActivateAbility(AbilitySpec.Handle);
			return;
		}
	}
}
