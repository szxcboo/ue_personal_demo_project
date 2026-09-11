#pragma once

#include "MyItemDefinition.generated.h"

USTRUCT(BlueprintType)
struct FMyItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	FMyItemDefinition()
	{
	}

	FMyItemDefinition(int32 InID, FName InDisplayName)
		: ID(InID), DisplayName(InDisplayName)
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ID = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	/** Icon to display */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxCount = 16;

	bool operator==(const FMyItemDefinition& Other) const
	{
		return Other.DisplayName == DisplayName;
	}

	bool IsValid() const
	{
		return ID != 0 && DisplayName.IsValid();
	}
};
