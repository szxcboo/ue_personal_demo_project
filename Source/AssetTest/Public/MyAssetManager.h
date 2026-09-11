#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	virtual void StartInitialLoading() override;

	/** Static types for items */
	static const FPrimaryAssetType PotionItemType;
	static const FPrimaryAssetType SkillItemType;
	static const FPrimaryAssetType TokenItemType;
	static const FPrimaryAssetType WeaponItemType;

	// /** Returns the current AssetManager object */
	// static UMyAssetManager& Get();
};
