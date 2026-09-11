#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MyAttributeChangeTask.generated.h"

struct FOnAttributeChangeData;

// The additional output execution pins are the results of broadcasting delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAttributeChangeTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UMyAttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	void EndTask(); // For cleaning up this task and make sure any resources that allocated are freed

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent; // Doesn't need a UPROPERTY as it is a weak pointer and it is not going to effect the lifetime of ASC
	FGameplayAttribute AttributeToListenFor;

	void AttributeChanged(const FOnAttributeChangeData& AttributeChangeData);
};
