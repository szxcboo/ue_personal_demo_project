#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyInventory.generated.h"

class UMyInventoryComponent;
class UMyItemInstance;
struct FMyItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemChanged);

USTRUCT(BlueprintType)
struct FMyInventoryItemInfo
{
	GENERATED_BODY()

	FName ItemName;

	int32 Count;
};

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyInventory : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; };

	/**
	 * Method that allows an actor to replicate subobjects on its actor channel.
	 * Inventory -> Item, while the reference, Items array, still needs to be replicated.
	 */
	bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

	// void ClientRegisterReplicatedItem(UMyItem* Item);
	// void ClientUnregisterReplicatedItem(UMyItem* Item);

	UPROPERTY(BlueprintAssignable)
	FOnItemChanged OnItemChanged;

	UFUNCTION(BlueprintCallable)
	int32 GetItemCount(const FName& ItemName);

	UFUNCTION(BlueprintCallable)
	UMyItemInstance* GetItem(const FGuid& Guid);

	UFUNCTION(BlueprintCallable)
	TArray<UMyItemInstance*> GetAll();

	UFUNCTION(BlueprintCallable)
	bool AddItem(const FName& ItemName, int32 Count = 1);

	UFUNCTION(Blueprintable)
	bool RemoveItem(const FName& ItemName, int32 RemoveCount = 1);

	UFUNCTION(BlueprintCallable)
	bool AddItems(const TArray<FMyInventoryItemInfo>& ItemInfos);

	UFUNCTION(BlueprintCallable)
	bool RemoveItems(const TArray<FMyInventoryItemInfo>& ItemInfos);

	UFUNCTION(BlueprintCallable)
	void SetSize(int32 InSize) { Size = InSize; };

	UPROPERTY(VisibleAnywhere, Replicated)
	TWeakObjectPtr<UDataTable> InventoryData;

	FMyItemDefinition* GetTableRow(const FName& ItemName) const;
	FMyItemDefinition* GetTableRow(UMyItemInstance* Item) const;

private:
	// UPROPERTY(ReplicatedUsing = OnRep_OwnerComponent)
	UPROPERTY(Replicated)
	TObjectPtr<UMyInventoryComponent> OwnerComponent;

	UMyItemInstance* AddItem_Internal();
	void RemoveItem_Internal(UMyItemInstance* Item);

	UPROPERTY(Replicated)
	int32 Size{18};

	UPROPERTY()	// No needs to mark as Replicated?
	TArray<UMyItemInstance*> Items;

	UPROPERTY()
	TMap<FGuid, UMyItemInstance*> ItemMap;

	// bool ItemChanged;
};
