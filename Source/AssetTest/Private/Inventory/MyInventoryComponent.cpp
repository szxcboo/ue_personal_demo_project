#include "Inventory/MyInventoryComponent.h"

#include "MyGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Engine/ActorChannel.h"
#include "Game/MyGameModeBase.h"
#include "Inventory/MyInventory.h"
#include "Inventory/MyItemInstance.h"

// void FMyInventoryEntry::PreReplicatedRemove(const FMyInventoryList& InArraySerializer)
// {
// 	UE_LOG(LogNetFastTArray,Log, TEXT("PreReplicatedRemove"))
// }
//
// void FMyInventoryEntry::PostReplicatedAdd(const FMyInventoryList& InArraySerializer)
// {
// 	UE_LOG(LogNetFastTArray,Log, TEXT("PostReplicatedAdd"))
// }
//
// void FMyInventoryEntry::PostReplicatedChange(const FMyInventoryList& InArraySerializer)
// {
// 	UE_LOG(LogNetFastTArray,Log, TEXT("PostReplicatedChange"))
// }

void FMyInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	check(OwnerComponent);

	UMyInventoryComponent* OwnerInventory = Cast<UMyInventoryComponent>(OwnerComponent);
	check(OwnerInventory);

	for (int32 Index : RemovedIndices)
	{
		FMyInventoryEntry& Stack = Entries[Index];
		// BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.ReplicateCount, /*NewCount=*/ 0);
		OwnerInventory->BroadcastChangeMessage(Stack.Item, Stack.ReplicateCount, 0);
		Stack.LastObservedCount = 0;
	}
}

// Client only!
void FMyInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	check(OwnerComponent);

	UMyInventoryComponent* OwnerInventory = Cast<UMyInventoryComponent>(OwnerComponent);
	check(OwnerInventory);

	for (int32 Index : AddedIndices)
	{
		FMyInventoryEntry& Stack = Entries[Index];
		// BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.ReplicateCount);
		OwnerInventory->BroadcastChangeMessage(Stack.Item, 0, Stack.ReplicateCount);
		Stack.LastObservedCount = Stack.ReplicateCount;
	}
}

void FMyInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	check(OwnerComponent);

	UMyInventoryComponent* OwnerInventory = Cast<UMyInventoryComponent>(OwnerComponent);
	check(OwnerInventory);

	for (int32 Index : ChangedIndices)
	{
		FMyInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		// BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.Count);
		OwnerInventory->BroadcastChangeMessage(Stack.Item, Stack.LastObservedCount, Stack.ReplicateCount);
		Stack.LastObservedCount = Stack.ReplicateCount;
	}
}

TArray<UMyItemInstance*> FMyInventoryList::GetAllItems() const
{
	TArray<UMyItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FMyInventoryEntry& Entry : Entries)
	{
		if (Entry.Item)
		{
			Results.Add(Entry.Item);
		}
	}
	return Results;
}

TArray<UMyItemInstance*> FMyInventoryList::GetAllItemsForDefinition(const FMyItemDefinition& ItemDef) const
{
	TArray<UMyItemInstance*> Results;
	for (const FMyInventoryEntry& Entry : Entries)
	{
		if (Entry.Item->GetDefinition() == ItemDef)
		{
			Results.Add(Entry.Item);
		}
	}
	return Results;
}

UMyItemInstance* FMyInventoryList::AddEntry(const FMyItemDefinition& ItemDef/*, int32 Count*/)
{
	check(OwnerComponent);
	AActor* Owner = OwnerComponent->GetOwner();
	check(Owner->HasAuthority());

	FMyInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = NewObject<UMyItemInstance>(OwnerComponent->GetOuter());
	NewEntry.Item->SetDefinition(ItemDef);
	NewEntry.ReplicateCount = 1;

	MarkItemDirty(NewEntry); // This must be called if you add or change an item in the array
	return NewEntry.Item;
}

UMyItemInstance* FMyInventoryList::AddEntry(UMyItemInstance* Item)
{
	check(OwnerComponent);
	AActor* Owner = OwnerComponent->GetOwner();
	check(Owner->HasAuthority());

	FMyInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;
	NewEntry.ReplicateCount = Item->GetCount();

	MarkItemDirty(NewEntry); // This must be called if you add or change an item in the array
	return NewEntry.Item;
}

void FMyInventoryList::RemoveEntry(UMyItemInstance* Item)
{
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FMyInventoryEntry& Entry = *It;
		if (Entry.Item == Item)
		{
			It.RemoveCurrent();
			MarkArrayDirty(); // This must be called if you just remove something from the array
		}
	}
}

UMyItemInstance* FMyInventoryList::FindFirstItem(FMyItemDefinition ItemDef)
{
	for (const FMyInventoryEntry& Entry : Entries)
	{
		if (Entry.Item->GetDefinition() == ItemDef)
		{
			return Entry.Item;
		}
	}
	return nullptr;
}

void FMyInventoryList::EntryChanged(UMyItemInstance* Item)
{
	for (FMyInventoryEntry& Entry : Entries)
	{
		if (Entry.Item == Item)
		{
			MarkItemDirty(Entry);
		}
	}
}

// void FMyInventoryList::BroadcastChangeMessage(FMyInventoryEntry& Entry, int32 OldCount, int32 NewCount)
// {
// 	FMyInventoryChangeMessage Message;
// 	Message.Instance = Entry.Item;
// 	Message.NewCount = NewCount;
// 	Message.Delta = NewCount - OldCount;
//
// 	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
// 	MessageSystem.BroadcastMessage(MyGameplayTags::None, Message);
// }

void FMyInventoryList::BroadcastChangeMessage(FMyInventoryEntry& Entry, int32 NewCount)
{
	FMyInventoryChangeMessage Message;
	Message.Instance = Entry.Item;
	Message.NewCount = NewCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(MyGameplayTags::Messages::InventoryStackChanged, Message);
}

UMyInventoryComponent::UMyInventoryComponent()
	: InventoryList(this)
{
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	// Enable replication subobject list
	// Override ReadyForReplication to add all existing instance to the list
	bReplicateUsingRegisteredSubObjectList = true;
}

void UMyInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(ThisClass, Inventory);

	DOREPLIFETIME(ThisClass, InventoryList);
}

void UMyInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// if (GetOwner()->HasAuthority())
	// {
	// 	// for (auto& Config : InventoryConfigs)
	// 	// {
	// 	// 	if (Inventories.Contains(Config.Tag))
	// 	// 	{
	// 	// 		continue;
	// 	// 	}
	// 	// 	auto NewInventory = UInventory::CreateInventory(this, Config);
	// 	// 	Inventories.Add(Config.Tag, NewInventory);
	// 	// 	if (Config.bReplicates)
	// 	// 	{
	// 	// 		RepInventories.Add(NewInventory);
	// 	// 	}
	// 	// }
	// 	if (!IsValid(Inventory))
	// 	{
	// 		UMyInventory* NewInventory = NewObject<UMyInventory>(GetOuter());
	// 		NewInventory->InventoryData = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode())->InventoryData;
	// 		NewInventory->SetSize(18);
	// 		Inventory = NewInventory;
	// 	}
	// 	RelevantPlayer = GetOwner();
	// }

	if (AMyGameModeBase* MyGameMode = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		InventoryData = MyGameMode->InventoryData;
	}
}

void UMyInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing instance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FMyInventoryEntry& Entry : InventoryList.Entries)
		{
			UMyItemInstance* Instance = Entry.Item;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

// Register the subobjects manually
// This method is used only when bReplicateUsingRegisteredSubObjectList is false.
// Otherwise this function is not called and only the ReplicatedSubObjects list is used.
bool UMyInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// if (IsValid(Inventory))
	// {
	// 	WroteSomething |= Inventory->ReplicateSubobjects(Channel, Bunch, RepFlags); // Replicate items
	// 	WroteSomething |= Channel->ReplicateSubobject(Inventory, *Bunch, *RepFlags); // Replicate inventory
	// }

	return WroteSomething;
}

// bool UMyInventoryComponent::AddItem(const FName& ItemName, int32 Count)
// {
// 	return Inventory->AddItem(ItemName, Count);
// }

TArray<UMyItemInstance*> UMyInventoryComponent::GetAllItems() const
{
	// return Inventory->GetAll();
	return InventoryList.GetAllItems();
}

void UMyInventoryComponent::TryAddItem(const FMyItemDefinition& ItemDef, int32 Count)
{
	// 1. No room, return directly

	UMyItemInstance* Result = InventoryList.FindFirstItem(ItemDef);
	if (IsValid(Result))
	{
		// 2. Stackable, add stack count
		ServerAddItemStackCount(Result, Count);
	}
	else
	{
		// 3. Add new item
		ServerAddNewItem(ItemDef, Count);
	}
}

void UMyInventoryComponent::ServerAddItemStackCount_Implementation(UMyItemInstance* Instance, int32 Count)
{
	UMyItemInstance* Result = InventoryList.FindFirstItem(Instance->GetDefinition());
	if (IsValid(Result))
	{
		const int32 OldCount = Result->GetCount();
		const int32 NewCount = Result->GetCount() + Count;
		Result->SetCount(NewCount);
		InventoryList.EntryChanged(Result); // Mark item dirty otherwise it won't refresh!

		BroadcastChangeMessage(Result, OldCount, NewCount);
	}
}

void UMyInventoryComponent::ServerAddNewItem_Implementation(const FMyItemDefinition& ItemDef, int32 Count)
{
	// We assume it is a valid item definition
	UMyItemInstance* NewItem = NewObject<UMyItemInstance>(GetOuter());
	NewItem->SetDefinition(ItemDef);
	NewItem->SetCount(Count);

	UMyItemInstance* Result = InventoryList.AddEntry(NewItem);
	if (IsValid(Result))
	{
		Result->SetCount(Count);

		// Add to subobject list
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
		}

		BroadcastChangeMessage(NewItem, 0, NewItem->GetCount());
	}
}

void UMyInventoryComponent::TryRemoveItem(const FMyItemDefinition& ItemDef, int32 Count)
{
	UMyItemInstance* Result = InventoryList.FindFirstItem(ItemDef);
	if (IsValid(Result))
	{
		ServerRemoveItemStackCount(Result, Count);
	}
}

void UMyInventoryComponent::ServerRemoveItemStackCount_Implementation(UMyItemInstance* Instance, int32 Count)
{
	int32 RemoveCount = Count;
	checkf((RemoveCount >= 0), TEXT("Remove count < 0."))

	// Find all instance in this inventory and remove stack count till remove count is <= 0
	TArray<UMyItemInstance*> Results = InventoryList.GetAllItemsForDefinition(Instance->GetDefinition());

	for (UMyItemInstance* ItemToRemove : Results)
	{
		const int32 OldCount = ItemToRemove->GetCount();
		const int32 MinusCount = FMath::Min(ItemToRemove->GetCount(), RemoveCount);
		const int32 NewCount = ItemToRemove->GetCount() - MinusCount;
		if (NewCount <= 0)
		{
			InventoryList.RemoveEntry(ItemToRemove);
			BroadcastChangeMessage(ItemToRemove, OldCount, 0);

			// Remove from subobject list
			if (ItemToRemove && IsUsingRegisteredSubObjectList())
			{
				RemoveReplicatedSubObject(ItemToRemove);
			}
		}
		else
		{
			ItemToRemove->SetCount(NewCount);
			InventoryList.EntryChanged(ItemToRemove);
			BroadcastChangeMessage(ItemToRemove, OldCount, NewCount);
		}

		RemoveCount -= MinusCount;
		if (RemoveCount <= 0)
		{
			return;
		}
	}
}

void UMyInventoryComponent::BroadcastChangeMessage(UMyItemInstance* Instance, int32 OldCount, int32 NewCount)
{
	FMyInventoryChangeMessage Message;
	Message.InventoryOwner = this;
	Message.Instance = Instance;
	Message.OldCount = OldCount;
	Message.NewCount = NewCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(MyGameplayTags::Messages::InventoryStackChanged, Message);
}

// bool UMyInventoryComponent::CheckConnection(UActorChannel* Channel) const
// {
// 	if (Channel && Channel->Connection && Channel->Connection->OwningActor)
// 	{
// 		return RelevantPlayer.Get() == Channel->Connection->OwningActor;
// 	}
// 	return false;
// }

void UMyInventoryComponent::OnRep_Inventory()
{
}
