#include "AbilitySystem/Abilities/MyGameplayAbility_ListenForHealthChange.h"

#include "AbilitySystem/Task/MyAttributeChangeTask.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "MyGameplayTags.h"

UMyGameplayAbility_ListenForHealthChange::UMyGameplayAbility_ListenForHealthChange()
{
	AbilityTags.AddTag(MyGameplayTags::Abilities::ActivateOnGiven);
	AbilityTags.AddTag(MyGameplayTags::Abilities::Death);
}

void UMyGameplayAbility_ListenForHealthChange::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ListenForAttributeChange = UMyAttributeChangeTask::ListenForAttributeChange(GetAbilitySystemComponentFromActorInfo(), UMyAttributeSet::GetHealthAttribute());
	ListenForAttributeChange->OnAttributeChanged.AddDynamic(this, &ThisClass::HandleActivateDeath);
	ListenForAttributeChange->Activate();
}

void UMyGameplayAbility_ListenForHealthChange::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ListenForAttributeChange->EndTask();
}

void UMyGameplayAbility_ListenForHealthChange::HandleActivateDeath(FGameplayAttribute Attribute, float NewValue, float OldValue)
{
	FGameplayTag DeathTag = MyGameplayTags::Abilities::Death;
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(DeathTag.GetSingleTagContainer());
}
