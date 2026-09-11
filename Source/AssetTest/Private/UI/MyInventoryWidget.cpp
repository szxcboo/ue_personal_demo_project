#include "UI/MyInventoryWidget.h"

#include "MyGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/MyInventoryComponent.h"
#include "Inventory/MyItemInstance.h"

bool UMyInventoryWidget::Initialize()
{
	return Super::Initialize();
}

void UMyInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyInventoryWidget::InitializeInventory(UMyInventoryComponent* Inventory)
{
	checkf(IsValid(Inventory), TEXT("Widget: Owning inventory is null."))
	
	OwningInventory = Inventory;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwningInventory->GetWorld());
	FGameplayMessageListenerHandle ListenerHandle = MessageSystem.RegisterListener(MyGameplayTags::Messages::InventoryStackChanged, this, &ThisClass::OnInventoryMessage);
}

UMyInventoryComponent* UMyInventoryWidget::GetInventory()
{
	checkf(IsValid(OwningInventory), TEXT("Widget: Owning inventory is null."))
	return OwningInventory;
}

void UMyInventoryWidget::SetInventory(UMyInventoryComponent* InInventory)
{
	checkf(IsValid(OwningInventory), TEXT("Widget: Owning inventory is null."))
	OwningInventory = InInventory;
}

TArray<UMyItemInstance*> UMyInventoryWidget::GetAllInventoryItems()
{
	checkf(IsValid(OwningInventory), TEXT("Widget: Owning inventory is null."))
	return OwningInventory->GetAllItems();
}

void UMyInventoryWidget::AddInventoryItem(const FMyItemDefinition& ItemDef, int32 Count)
{
	checkf(IsValid(OwningInventory), TEXT("Widget: Owning inventory is null."))

	OwningInventory->TryAddItem(ItemDef, Count);
}

void UMyInventoryWidget::RemoveInventoryItem(const FMyItemDefinition& ItemDef, int32 Count)
{
	checkf(IsValid(OwningInventory), TEXT("Widget: Owning inventory is null."))

	OwningInventory->TryRemoveItem(ItemDef, Count);
}

void UMyInventoryWidget::RefreshItems()
{
}

void UMyInventoryWidget::OnInventoryMessage(FGameplayTag Channel, const FMyInventoryChangeMessage& Payload)
{
	if (Payload.InventoryOwner == OwningInventory)
	{
		UE_LOG(LogGameplayMessageSubsystem, Log, TEXT("%s received message."), *GetNameSafe(OwningInventory->GetOuter()))

		const int32 OldCount = Payload.OldCount;
		const int32 NewCount = Payload.NewCount;

		if (OldCount == 0 && NewCount > 0)
		{
			BP_OnInventoryItemAdded(Payload.Instance);
		}
		else if (OldCount > 0 && NewCount == 0)
		{
			BP_OnInventoryItemRemoved(Payload.Instance);
		}
		else
		{
			BP_OnInventoryItemChanged(Payload.Instance);
		}
	}
}
