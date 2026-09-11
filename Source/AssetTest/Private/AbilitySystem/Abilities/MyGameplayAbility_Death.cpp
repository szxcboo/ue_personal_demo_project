#include "AbilitySystem/Abilities/MyGameplayAbility_Death.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/MyCharacterBase.h"

void UMyGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (MontageToPlay)
	{
		UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), MontageToPlay);
		AbilityTask_PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::HandleDeath);
		AbilityTask_PlayMontageAndWait->OnInterrupted.AddDynamic(this, &ThisClass::HandleDeath);
		AbilityTask_PlayMontageAndWait->OnCancelled.AddDynamic(this, &ThisClass::HandleDeath);
		AbilityTask_PlayMontageAndWait->ReadyForActivation();
	}

	// Apply death effect to self
	// if(!IsValid(DeathEffect)) return;
	//
	// FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();;
	// FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(DeathEffect, 1.f, ContextHandle);
	// GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UMyGameplayAbility_Death::HandleDeath()
{
	Cast<AMyCharacterBase>(GetAvatarActorFromActorInfo())->Ragdoll();
}
