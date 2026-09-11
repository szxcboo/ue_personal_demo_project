#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MyInventory.h"
#include "MyItemInstance.h"
#include "MyInventoryComponent.generated.h"

class UMyItemInstance;
class UMyInventory;
struct FMyItemDefinition;
class UMyInventoryItemInstance;

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FMyInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UMyItemInstance> Item = nullptr;

	UPROPERTY()
	int32 ReplicateCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;

	// void PreReplicatedRemove(const struct FMyInventoryList& InArraySerializer);
	// void PostReplicatedAdd(const struct FMyInventoryList& InArraySerializer);
	// void PostReplicatedChange(const struct FMyInventoryList& InArraySerializer);
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FMyInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FMyInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FMyInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	// Replicated list of items
	UPROPERTY()
	TArray<FMyInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	/** Called before removing elements and after the elements themselves are notified. */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);

	/** Called after adding all new elements and after the elements themselves are notified. */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

	/** Called after updating all existing elements with new data and after the elements themselves are notified. */
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FMyInventoryEntry, FMyInventoryList>(Entries, DeltaParms, *this);
	}

	TArray<UMyItemInstance*> GetAllItems() const;
	TArray<UMyItemInstance*> GetAllItemsForDefinition(const FMyItemDefinition& ItemDef) const;
	UMyItemInstance* AddEntry(const FMyItemDefinition& ItemDef); // Add count 1
	UMyItemInstance* AddEntry(UMyItemInstance* Item);
	void RemoveEntry(UMyItemInstance* Item);

	/** Find the first item for this definition */
	UMyItemInstance* FindFirstItem(FMyItemDefinition ItemDef);

	/** Should be called when an item changed to refresh it */
	void EntryChanged(UMyItemInstance* Item);

private:
	// void BroadcastChangeMessage(FMyInventoryEntry& Entry, int32 OldCount, int32 NewCount);
	void BroadcastChangeMessage(FMyInventoryEntry& Entry, int32 NewCount);
};

template <>
struct TStructOpsTypeTraits<FMyInventoryList> : public TStructOpsTypeTraitsBase2<FMyInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true // struct has a NetDeltaSerialize function for serializing differences in state from a previous NetSerialize operation.
	};
};

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FMyInventoryChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UMyItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 OldCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ASSETTEST_API UMyInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMyInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	// UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	// bool AddItem(const FName& ItemName, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<UMyItemInstance*> GetAllItems() const;

	// UFUNCTION(BlueprintCallable, Category="Inventory")
	// UMyInventory* GetInventory() const { return Inventory; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	void TryAddItem(const FMyItemDefinition& ItemDef, int32 Count);

	UFUNCTION(Server, Reliable)
	void ServerAddItemStackCount(UMyItemInstance* Instance, int32 Count);

	UFUNCTION(Server, Reliable)
	void ServerAddNewItem(const FMyItemDefinition& ItemDef, int32 Count);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	void TryRemoveItem(const FMyItemDefinition& ItemDef, int32 Count);

	UFUNCTION(Server, Reliable)
	void ServerRemoveItemStackCount(UMyItemInstance* Instance, int32 Count);

	void BroadcastChangeMessage(UMyItemInstance* Instance, int32 OldCount, int32 NewCount);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDataTable> InventoryData;

	// UPROPERTY(VisibleAnywhere)
	// TWeakObjectPtr<AActor> RelevantPlayer;

private:
	// bool CheckConnection(UActorChannel* Channel) const;

	// UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Inventory)
	// TObjectPtr<UMyInventory> Inventory;

	UPROPERTY(ReplicatedUsing=OnRep_Inventory)
	FMyInventoryList InventoryList;

	UFUNCTION()
	void OnRep_Inventory();
};
