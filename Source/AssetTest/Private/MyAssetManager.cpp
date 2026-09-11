#include "MyAssetManager.h"

#include "AbilitySystemGlobals.h"

const FPrimaryAssetType	UMyAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType	UMyAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UMyAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UMyAssetManager::WeaponItemType = TEXT("Weapon");

void UMyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// This is required to use Target Data
	UAbilitySystemGlobals::Get().InitGlobalData();
}
