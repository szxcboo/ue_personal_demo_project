#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MyAbilitySystemComponent.generated.h"

class UMyAbilityBase;

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;

	/** Returns a list of currently active ability instances that match the tags, works only when the abilities are set to Instance Per Execution */
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UMyAbilityBase*>& ActiveAbilities);

	/** Returns a list of currently active ability instances that match the tags */
	void GetActiveAbilitiesWithTag(const FGameplayTag& GameplayTag, TArray<UMyAbilityBase*>& ActiveAbilities);
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag CharacterTag;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	/** Input */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

private:
	void HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySpec);
};
