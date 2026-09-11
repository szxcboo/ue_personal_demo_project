#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyItemDefinition.h"
#include "MyItemInstance.generated.h"

class UMyInputComponent;
class UMyInventory;

/**
 * 
 */
UCLASS(BlueprintType)
class ASSETTEST_API UMyItemInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; } // Tell the engine that this Object supports network

	UFUNCTION(BlueprintCallable)
	FMyItemDefinition GetDefinition() const { return Definition; }

	FORCEINLINE UMyInventory* GetOwnerInventory() const { return OwnerInventory; }
	FORCEINLINE FGuid GetGuid() const { return Guid; }
	FORCEINLINE FName GetItemName() const { return ItemName; }
	FORCEINLINE int32 GetCount() const { return Count; }

	void SetOwnerInventory(UMyInventory* InOwner);
	void SetGuid(const FGuid& InGuid);
	void SetItemName(const FName& InItemName);
	void SetCount(int32 InCount);

	// Must be called while the new instance created on server!
	void SetDefinition(FMyItemDefinition InDefinition);

	// virtual void PreDestroyFromReplication() override;

private:
	// UPROPERTY(ReplicatedUsing = OnRep_OwnerInventory)
	UPROPERTY(Replicated)
	TObjectPtr<UMyInventory> OwnerInventory;

	UPROPERTY(Replicated)
	TObjectPtr<UMyInputComponent> OwnerComponent;

	UPROPERTY(Replicated)
	FMyItemDefinition Definition;

	UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FGuid Guid;

	/** Matches row name of table */
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FName ItemName;

	// UPROPERTY(ReplicatedUsing = OnRep_Count)
	UPROPERTY(Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int32 Count;

	// UFUNCTION()
	// void OnRep_OwnerInventory();
	// void OnRep_Count();
};
