#include "AbilitySystem/Abilities/MyGameplayAbility_HitReact.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "MyGameplayTags.h"

UMyGameplayAbility_HitReact::UMyGameplayAbility_HitReact()
{
	AbilityTags.AddTag(MyGameplayTags::Abilities::Enemy::HitReact);
	AbilityTags.AddTag(MyGameplayTags::Abilities::ActivateOnGiven); // Auto activated
}

void UMyGameplayAbility_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MyGameplayTags::Events::HitReact);
	WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::HitReact);
	WaitGameplayEvent->ReadyForActivation();
}

void UMyGameplayAbility_HitReact::HitReact(FGameplayEventData Payload)
{
	if (IsValid(MontageToPlay))
	{
		UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), MontageToPlay);
		AbilityTask_PlayMontageAndWait->ReadyForActivation();
	}
}
