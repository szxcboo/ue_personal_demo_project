#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ComboGraphDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FComboTransition
{
	GENERATED_BODY()

	FGameplayTag NextNodeTag;
	FName NextSectionName;
};

USTRUCT(BlueprintType)
struct FComboNode
{
	GENERATED_BODY()

	FGameplayTag NodeTag;
	TSoftObjectPtr<UAnimMontage> MontageSection;
	FName SectionName;
	TArray<FComboTransition> Transitions;
};

/**
 * 
 */
UCLASS()
class ASSETTEST_API UComboGraphDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FComboNode> Nodes;
};
