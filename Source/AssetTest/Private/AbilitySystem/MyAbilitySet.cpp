#include "AbilitySystem/MyAbilitySet.h"

#include "AbilitySystem/MyAbilitySystemComponent.h"

void UMyAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* ASC, TArray<FGameplayAbilitySpecHandle>& OutGrantedHandles) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (const TTuple<TSubclassOf<UGameplayAbility>, int32>& AbilityToGrant : GrantedAbilities)
	{
		if (!IsValid(AbilityToGrant.Key))
		{
			// UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}
		
		FGameplayAbilitySpec AbilitySpec(AbilityToGrant.Key, AbilityToGrant.Value);
		// AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		OutGrantedHandles.Add(AbilitySpecHandle);
	}
}
