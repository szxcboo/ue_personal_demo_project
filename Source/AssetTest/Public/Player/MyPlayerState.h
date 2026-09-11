#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MyPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UMyInventoryComponent;
class UMyInventory;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /*StatValue*/, bool /*bLevelUp*/)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, uint8, NewTeam);

UCLASS()
class ASSETTEST_API AMyPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMyPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

	FOnPlayerStatChanged OnXPChangedDelegate;
	FOnLevelChanged OnLevelChangedDelegate;

	UFUNCTION(BlueprintCallable)
	UMyInventoryComponent* GetInventory() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable)
	void SetTeam(uint8 NewTeam);

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Team, Category="Team")
	uint8 Team = INDEX_NONE;

	void SetTeamForConnection(uint8 NewTeam);

private:
	UPROPERTY(VisibleAnywhere, Category="Ability System")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category="Ability System")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TObjectPtr<UMyInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(int32 OldXP);

	UFUNCTION()
	void OnRep_Team();
};
