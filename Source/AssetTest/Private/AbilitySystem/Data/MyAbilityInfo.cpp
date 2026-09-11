#include "AbilitySystem/Data/MyAbilityInfo.h"

FMyAbilityInfoData UMyAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FMyAbilityInfoData& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	// if (bLogNotFound)
	// {
	// 	UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	// }

	return FMyAbilityInfoData();
}
