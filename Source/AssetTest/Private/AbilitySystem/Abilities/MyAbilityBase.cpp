#include "AbilitySystem/Abilities/MyAbilityBase.h"

UMyAbilityBase::UMyAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMyAbilityBase::EndAbilitySimple()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
