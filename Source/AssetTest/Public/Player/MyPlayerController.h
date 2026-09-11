#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "MyPlayerController.generated.h"

class UMyInventoryWidget;
class UInputMappingContext;
class UInputAction;
class UMyInputConfig;
class UMyInventoryComponent;
class UMyItemInstance;
struct FInputActionValue;
struct FGameplayTag;

/** Delegate called when player state is replicated down to clients and has valid inventory */
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInventoryReplicated, UMyInventoryComponent*, Inventory);

/**
 * 
 */
UCLASS()
class ASSETTEST_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void ReceivedPlayer() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnPlayerStateReplicated();

	// UPROPERTY(BlueprintAssignable)
	FOnInventoryReplicated OnInventoryReplicated;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetupInventoryUI(UMyInventoryComponent* Inventory);

	/** Get InventoryComponent from PlayerState */
	UFUNCTION(BlueprintCallable)
	UMyInventoryComponent* GetInventory() const;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UMyInventoryWidget> InventoryUIClass;

	/** Set up reference in blueprint */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory")
	UMyInventoryWidget* NativeInventoryUI;

private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UMyInputConfig> InputConfig;

	void Move(const FInputActionValue& Value);

	UMyAbilitySystemComponent* GetMyASC();

	/** Data driven input */
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
};
