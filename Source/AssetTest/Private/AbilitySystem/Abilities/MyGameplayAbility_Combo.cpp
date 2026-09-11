#include "AbilitySystem/Abilities/MyGameplayAbility_Combo.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"

void UMyGameplayAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// // Reset the properties
	// bCanReceiveInput = false;
	// bCanJumpSection = false;
	// OnInputReceivedDelegate.Clear();
	//
	// // Start playing combo Montage
	// PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), MontageToPlay);
	// PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	// PlayMontageAndWait->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	// PlayMontageAndWait->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	// PlayMontageAndWait->ReadyForActivation();
	//
	// // Wait for input cache window open
	// UAbilityTask_WaitGameplayEvent* WaitInputCacheEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GameplayTags::Events::Combo::StartInputCache);
	// WaitInputCacheEvent->EventReceived.AddDynamic(this, &ThisClass::OnInputCacheStart);
	// WaitInputCacheEvent->ReadyForActivation();
	//
	// // Wait for transition window open
	// UAbilityTask_WaitGameplayEvent* WaitWindowOpenEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GameplayTags::Events::Combo::WindowOpen);
	// WaitWindowOpenEvent->EventReceived.AddDynamic(this, &ThisClass::OnTransitionWindowOpen);
	// WaitWindowOpenEvent->ReadyForActivation();
	//
	// UAbilityTask_WaitGameplayEvent* WaitWindowCloseEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GameplayTags::Events::Combo::WindowClose);
	// WaitWindowCloseEvent->EventReceived.AddDynamic(this, &ThisClass::OnTransitionWindowClose);
	// WaitWindowCloseEvent->ReadyForActivation();
}

void UMyGameplayAbility_Combo::OnInputCacheStart(FGameplayEventData Payload)
{
	bCanReceiveInput = true;
	// Wait for input receiving
	// UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MyGameplayTags::Events::Combo::ReceiveInput);
	// WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::OnInputReceived);
	// WaitGameplayEvent->ReadyForActivation();
}

void UMyGameplayAbility_Combo::OnInputCacheEnd(FGameplayEventData Payload)
{
	bCanReceiveInput = false;
}

void UMyGameplayAbility_Combo::OnInputReceived(FGameplayEventData Payload)
{
	if (bCanReceiveInput)
	{
		bCanJumpSection = true;
		OnInputReceivedDelegate.ExecuteIfBound(Payload);
	}
}

void UMyGameplayAbility_Combo::OnTransitionWindowOpen(FGameplayEventData Payload)
{
	if (bCanJumpSection)
	{
		// Jump to next section
		JumpToNextSection(Payload);
	}
	// Didn't receive during the input cache window, continue listening
	// OnInputReceivedDelegate.BindDynamic(this, &ThisClass::JumpToNextSection);
}

void UMyGameplayAbility_Combo::OnTransitionWindowClose(FGameplayEventData Payload)
{
	bCanJumpSection = false;
}

FName UMyGameplayAbility_Combo::JumpToNextSection(FGameplayEventData Payload)
{
	// Issue:
	// Current montage section on dedicated server will be change through the server RPC, which reaches the server earlier than the server validation
	// And then the server validation will call JumpToSection again, which cause client: A -> B, instead server: B -> C
	// Make this only executed on client
	// Or we could just use Montage_JumpToSection() instead of ASC function
	
	if (IsValid(CurrentMontage))
	{
		FName CurrentSection = GetAbilitySystemComponentFromActorInfo()->GetCurrentMontageSectionName();
		FName NextSection;

		// Find the matching next section for current section
		bool bFoundMatchingSection = false;
		for (const TTuple<FName, FName>& Pair : JumpSectionMap)
		{
			if (Pair.Key == CurrentSection)
			{
				NextSection = Pair.Value;
				bFoundMatchingSection = true;
				break;
			}
		}
		if (bFoundMatchingSection)
		{
			// MontageJumpToSection(NextSection);
			// Refer to UAbilitySystemComponent::CurrentMontageJumpToSection
			UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_JumpToSection(NextSection, CurrentMontage);
			}
		}
		// UE_LOG(LogTemp, Warning, TEXT("%s, %s"), *CurrentSection.ToString(), *NextSection.ToString());

		return NextSection;
	}
	bCanJumpSection = false;

	return FName();
}
