#include "Inventory/MyItemInstance.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Inventory/MyInventory.h"

// Tell the engine what properties need to be replicated
void UMyItemInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true; // Once this is set to true, we need to manually mark in Setter when the replicated properties change

	DOREPLIFETIME_WITH_PARAMS_FAST(UMyItemInstance, Guid, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UMyItemInstance, ItemName, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UMyItemInstance, OwnerInventory, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(UMyItemInstance, Count, SharedParams);

	DOREPLIFETIME(UMyItemInstance, OwnerComponent);
	DOREPLIFETIME(UMyItemInstance, Definition);
}

void UMyItemInstance::SetOwnerInventory(UMyInventory* InOwner)
{
	OwnerInventory = InOwner;
	MARK_PROPERTY_DIRTY_FROM_NAME(UMyItemInstance, OwnerInventory, this);
}

void UMyItemInstance::SetGuid(const FGuid& InGuid)
{
	Guid = InGuid;
	MARK_PROPERTY_DIRTY_FROM_NAME(UMyItemInstance, Guid, this);
}

void UMyItemInstance::SetItemName(const FName& InItemName)
{
	ItemName = InItemName;
	MARK_PROPERTY_DIRTY_FROM_NAME(UMyItemInstance, ItemName, this);
}

void UMyItemInstance::SetCount(int32 InCount)
{
	Count = InCount;
	MARK_PROPERTY_DIRTY_FROM_NAME(UMyItemInstance, Count, this);
}

void UMyItemInstance::SetDefinition(FMyItemDefinition InDefinition)
{
	Definition = InDefinition;
}

// void UMyItem::PreDestroyFromReplication()
// {
// 	if (IsValid(OwnerInventory))
// 	{
// 		OwnerInventory->ClientUnregisterReplicatedItem(this);
// 	}
// }
//
// void UMyItem::OnRep_OwnerInventory()
// {
// 	if (IsValid(OwnerInventory))
// 	{
// 		OwnerInventory->ClientRegisterReplicatedItem(this);
// 	}
// }
