#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/MyInventoryComponent.h"
#include "Messages/MyVerbMessage.h"
#include "MyInventoryWidget.generated.h"

class UMyItemInstance;
class UMyInventory;

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override; // Can be called multiple times

	UFUNCTION()
	void InitializeInventory(UMyInventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable)
	UMyInventoryComponent* GetInventory();

	UFUNCTION(BlueprintCallable)
	void SetInventory(UMyInventoryComponent* InInventory);

	UFUNCTION(BlueprintCallable)
	TArray<UMyItemInstance*> GetAllInventoryItems();

	UFUNCTION(BlueprintCallable)
	void AddInventoryItem(const FMyItemDefinition& ItemDef, int32 Count = 1);

	UFUNCTION(BlueprintCallable)
	void RemoveInventoryItem(const FMyItemDefinition& ItemDef, int32 Count = 1);

	UFUNCTION(BlueprintCallable)
	void RefreshItems();

	void OnInventoryMessage(FGameplayTag Channel, const FMyInventoryChangeMessage& Payload);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInventoryItemAdded")
	void BP_OnInventoryItemAdded(UMyItemInstance* Instance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInventoryItemChanged")
	void BP_OnInventoryItemChanged(UMyItemInstance* Instance);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnInventoryItemRemoved")
	void BP_OnInventoryItemRemoved(UMyItemInstance* Instance);

private:
	// UPROPERTY()
	// TObjectPtr<UMyInventory> OwningInventory;

	UPROPERTY()
	TObjectPtr<UMyInventoryComponent> OwningInventory;
};
