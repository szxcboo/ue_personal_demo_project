#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ComboCondition.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UComboCondition : public UObject
{
	GENERATED_BODY()

public:
	virtual bool Evaluate() { return false; };
};