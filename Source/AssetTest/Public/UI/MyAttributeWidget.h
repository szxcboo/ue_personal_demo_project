#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "MyAttributeWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAttributeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	FGameplayAttribute MaxAttribute;

	// void OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, UAttributeSet* AttributeSet);
	// bool MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	//
	// UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Attribute Change"))
	// void BP_OnAttributeChange(float NewValue, float NewMaxValue);

	// Will be called by the controller (Enemy / Widget Component)
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	// Since this is a UObject type, it can be set to any type derived such as Enemy
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	
	// void WidgetControllerSet();

	// Will be implemented to update the progress bar percent, and called by SetWidgetController
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnWidgetControllerSet"))
	void BP_OnWidgetControllerSet();
};
