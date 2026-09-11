#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

class UMyAbilitySet;

/**
 * 
 */
UCLASS()
class ASSETTEST_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// CharacterClassInfo

	// AbilityInfo

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMyAbilitySet> PlayerAbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDataTable> InventoryData;
};
