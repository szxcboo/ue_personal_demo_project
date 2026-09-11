#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "MyAbilitySet.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

/**
* Data used by the ability set to grant gameplay abilities.
*/
USTRUCT(BlueprintType)
struct FMyAbilitySet_Ability
{
	GENERATED_BODY()

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(UAbilitySystemComponent* ASC, TArray<FGameplayAbilitySpecHandle>& OutGrantedHandles) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UGameplayAbility>, int32> GrantedAbilities;

	// UPROPERTY(EditDefaultsOnly)
	// TArray<FMyAbilitySet_Ability> GrantedAbilities;
};
