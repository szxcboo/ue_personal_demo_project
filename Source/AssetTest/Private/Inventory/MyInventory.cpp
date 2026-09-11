#include "Inventory/MyInventory.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/MyItemDefinition.h"
#include "Inventory/MyItemInstance.h"

void UMyInventory::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwnerComponent);
	DOREPLIFETIME(ThisClass, Size);
	DOREPLIFETIME(ThisClass, InventoryData);
}

bool UMyInventory::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = false;
	if (Channel)
	{
		for (UMyItemInstance* Item : Items)
		{
			WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

#if 0
void UMyInventory::ClientRegisterReplicatedItem(UMyItem* Item)
{
	if (IsValid(Item) && !ItemMap.Contains(Item->GetGuid()))
	{
		Items.Add(Item);
		ItemMap.Add(Item->GetGuid(), Item);
		// MarkItemChange();
		OnItemChanged.Broadcast();
	}
}

void UMyInventory::ClientUnregisterReplicatedItem(UMyItem* Item)
{
	if (IsValid(Item))
	{
		Items.Remove(Item);
		ItemMap.Remove(Item->GetGuid());
		// MarkItemChange();
		OnItemChanged.Broadcast();
	}
}
#endif

int32 UMyInventory::GetItemCount(const FName& ItemName)
{
	int32 Count = 0;
	for (UMyItemInstance* Item : Items)
	{
		if (Item->GetItemName() == ItemName)
		{
			Count += Item->GetCount();
		}
	}
	return Count;
}

UMyItemInstance* UMyInventory::GetItem(const FGuid& Guid)
{
	if (auto Result = ItemMap.Find(Guid))
	{
		return *Result;
	}
	return nullptr;
}

TArray<UMyItemInstance*> UMyInventory::GetAll()
{
	return Items;
}

bool UMyInventory::AddItem(const FName& ItemName, int32 Count)
{
	bool Result = false;
	// 1. Stack the items
	for (UMyItemInstance* Item : Items)
	{
		if (Item->GetItemName() == ItemName)
		{
			if (FMyItemDefinition* Row = GetTableRow(Item))
			{
				int32 AddCount = FMath::Min(Row->MaxCount - Item->GetCount(), Count);
				Item->SetCount(Item->GetCount() + AddCount);
				Count -= AddCount;

				if (Count <= 0)
				{
					Result = true;
					break;
				}
			}
		}
	}
	// 2. Create new items
	if (!Result)
	{
		// Ensure not over the size
		if (Size >= Items.Num() + 1)
		{
			if (auto Row = GetTableRow(ItemName))
			{
				while (Count > 0) // handle the item count
				{
					UMyItemInstance* Item = AddItem_Internal(); // New an item and add it to the inventory
					Item->SetItemName(ItemName);
					Item->SetCount(FMath::Min(Row->MaxCount, Count));
					Count -= Row->MaxCount;
				}
				Result = true;
			}
		}
	}
	OnItemChanged.Broadcast(); // MarkItemChange();
	return Result;
}

bool UMyInventory::RemoveItem(const FName& ItemName, int32 RemoveCount)
{
	bool Result = false;
	// Find the item and remove till the count to remove is <= 0
	TArray<UMyItemInstance*> CopyItems = Items;
	for (UMyItemInstance* Item : CopyItems)	// Or use iterator to avoid copy
	{
		if (Item->GetItemName() == ItemName)
		{
			int32 MinusCount = FMath::Min(Item->GetCount(), RemoveCount);
			Item->SetCount(Item->GetCount() - MinusCount);
			if (Item->GetCount() <= 0)
			{
				RemoveItem_Internal(Item);
			}
			RemoveCount -= MinusCount;
			if (RemoveCount <= 0)
			{
				Result = true;
				break;
			}
		}
	}
	OnItemChanged.Broadcast(); // MarkItemChange();
	return Result;
}

bool UMyInventory::AddItems(const TArray<FMyInventoryItemInfo>& ItemInfos)
{
	// TODO: CheckFirst

	// Make a map of items to add
	TMap<FName, int32> CountMap;
	for (const FMyInventoryItemInfo& Info : ItemInfos)
	{
		FName InfoName = Info.ItemName;
		int32 InfoCount = Info.Count;
		CountMap.FindOrAdd(InfoName, 0) += InfoCount;
	}
	// 1. Stack the items
	for (UMyItemInstance* Item : Items)
	{
		if (auto Result = CountMap.Find(Item->GetItemName()))
		{
			int32& Count = *Result;
			// if (auto Row = Item->GetTableRow())
			if (auto Row = GetTableRow(Item))
			{
				int32 AddCount = FMath::Min(Row->MaxCount - Item->GetCount(), Count);
				// if (CheckMethod != EItemCheckMethod::CheckOnly)
				// {
				// 	Item->SetCount(Item->GetCount() + AddCount);
				// }
				Item->SetCount(Item->GetCount() + AddCount);
				Count -= AddCount;

				if (Count <= 0)
				{
					CountMap.Remove(Item->GetItemName());
				}
			}
		}
	}
	// 2. Create new items
	if (CountMap.Num() > 0)
	{
		// Over the size
		if (CountMap.Num() > Size - Items.Num()) return false;

		for (auto Iter = CountMap.CreateIterator(); Iter; ++Iter)
		{
			if (auto Row = GetTableRow(Iter->Key)) // GetTableRow(ItemName)
			{
				while (Iter->Value > 0) // Count > 0, handle the item count
				{
					// if (CheckMethod != EItemCheckMethod::CheckOnly)
					// {
					// 	auto Item = AddItem_Internal();
					// 	Item->SetItemName(Iter->Key);
					// 	Item->SetCount(FMath::Min(Row->MaxCount, Iter->Value));
					// }
					UMyItemInstance* Item = AddItem_Internal(); // New an item and add it to the inventory
					Item->SetItemName(Iter->Key);
					Item->SetCount(FMath::Min(Row->MaxCount, Iter->Value));
					Iter->Value -= Row->MaxCount;
				}
				Iter.RemoveCurrent();
			}
		}
	}
	// MarkItemChange();
	OnItemChanged.Broadcast();
	return CountMap.Num() <= 0;
}

bool UMyInventory::RemoveItems(const TArray<FMyInventoryItemInfo>& ItemInfos)
{
	// TODO: CheckFirst

	TMap<FName, int32> CountMap;
	for (auto& Info : ItemInfos)
	{
		CountMap.FindOrAdd(Info.ItemName, 0) += Info.Count;
	}

	TArray<UMyItemInstance*> CopyItems = Items;
	for (UMyItemInstance* Item : CopyItems)
	{
		FName ItemName = Item->GetItemName();
		if (auto Result = CountMap.Find(ItemName))
		{
			int32& Count = *Result;
			int32 MinusCount = FMath::Min(Item->GetCount(), Count);
			Item->SetCount(Item->GetCount() - MinusCount);
			if (Item->GetCount() <= 0)
			{
				RemoveItem_Internal(Item);
			}
			Count -= MinusCount;
			if (Count <= 0)
			{
				CountMap.Remove(ItemName);
			}
		}
	}
	// MarkItemChange();
	OnItemChanged.Broadcast();
	return CountMap.Num() <= 0;
}

FMyItemDefinition* UMyInventory::GetTableRow(const FName& ItemName) const
{
	check(InventoryData.IsValid());
	if (FMyItemDefinition* ItemDef = InventoryData->FindRow<FMyItemDefinition>(ItemName, FString()))
	{
		return ItemDef;
	}
	UE_LOG(LogTemp, Error, TEXT("Can not find the item."));
	return nullptr;
}

FMyItemDefinition* UMyInventory::GetTableRow(UMyItemInstance* Item) const
{
	check(InventoryData.IsValid());
	return InventoryData->FindRow<FMyItemDefinition>(Item->GetItemName(), FString());
}

UMyItemInstance* UMyInventory::AddItem_Internal()
{
	UMyItemInstance* NewItem = NewObject<UMyItemInstance>(GetOuter());
	NewItem->SetGuid(FGuid::NewGuid());
	NewItem->SetOwnerInventory(this);
	Items.Add(NewItem);
	ItemMap.Add(NewItem->GetGuid(), NewItem);
	return NewItem;
}

void UMyInventory::RemoveItem_Internal(UMyItemInstance* Item)
{
	Items.RemoveSwap(Item);
	ItemMap.Remove(Item->GetGuid());
	Item->MarkAsGarbage();
}
