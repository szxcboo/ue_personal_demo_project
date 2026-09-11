#include "Player/MyPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "Player/MyPlayerState.h"
#include "Input/MyInputComponent.h"
#include "Inventory/MyInventoryComponent.h"
#include "UI/MyInventoryWidget.h"

AMyPlayerController::AMyPlayerController()
{
	bReplicates = true;

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

// Called before BeginPlay
void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputMappingContext) return;

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem)) return;

	InputSubsystem->AddMappingContext(InputMappingContext, 0);

	// Needs to set up the Input Component in Project Settings
	UMyInputComponent* MyInputComponent = CastChecked<UMyInputComponent>(InputComponent);
	if (!IsValid(MyInputComponent)) return;

	MyInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

	// Bind ability actions
	MyInputComponent->BindAbilityActions(
		InputConfig,
		this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld
	);
}

void AMyPlayerController::BeginPlay()
{
	// Create widgets in blueprint
	Super::BeginPlay();

	// On server the PlayerState is valid, try initialize the widget
	if (IsLocalController() && IsValid(PlayerState) && IsValid(NativeInventoryUI)) // Also check is local controller?
	{
		AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>();
		UMyInventoryComponent* Inventory = MyPlayerState->GetInventory();
		if (Inventory)
		{
			NativeInventoryUI->InitializeInventory(Inventory);
			UE_LOG(LogPlayerController, Log, TEXT("Inventory initialized in PlayerController::BeginPlay"));
		}

		// Test add item
		// FMyItemDefinition ItemDef = FMyItemDefinition(1, FName("Portion1"));
		// FMyItemSpec TestItem = FMyItemSpec(ItemDef, 1);
		// GetInventory()->TryAddItem(TestItem);
	}
}

// After BeginPlay on server
// Before OnRep_PlayerState, OnPossess and BeginPlay on client
void AMyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

// Before pawn's BeginPlay
// Server only, ensures Pawn is valid
// Before beginPlay on listen server, but after it on dedicated server
void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

// Client only, ensures PlayerState is valid.
// It is uncertain whether it is called after BeginPlay!
void AMyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	BP_OnPlayerStateReplicated();

	// On clients tell widget that PlayerState is replicated
	if (IsLocalController() && IsValid(PlayerState) && IsValid(NativeInventoryUI)) // Also check is local controller?
	{
		AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>();
		UMyInventoryComponent* Inventory = MyPlayerState->GetInventory();
		if (Inventory && NativeInventoryUI->GetInventory() == nullptr) // Preventing initialize again
		{
			NativeInventoryUI->InitializeInventory(Inventory);
			UE_LOG(LogPlayerController, Log, TEXT("Inventory initialized in PlayerController::OnRep_PlayerState"));
		}

		// Test add item
		// FMyItemDefinition ItemDef = FMyItemDefinition(2, FName("Portion2"));
		// FMyItemSpec TestItem = FMyItemSpec(ItemDef, 1);
		// GetInventory()->TryAddItem(TestItem);
	}
}

void AMyPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
}

UMyInventoryComponent* AMyPlayerController::GetInventory() const
{
	if (AMyPlayerState* MyPlayerState = GetPlayerState<AMyPlayerState>())
	{
		return MyPlayerState->GetInventory();
	}
	return nullptr;
}

void AMyPlayerController::Move(const FInputActionValue& Value)
{
	if (!IsValid(GetPawn()))
	{
		return;
	}
	// if (!CanMove()) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.X);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.Y);

	// GetPawn()->AddControllerYawInput()
}

UMyAbilitySystemComponent* AMyPlayerController::GetMyASC()
{
	// if (AuraAbilitySystemComponent == nullptr)
	// {
	// 	AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	// }
	// return AuraAbilitySystemComponent;
	return Cast<UMyAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState));
}

void AMyPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// Block inputs
	// if ( && GetASC()->HasMatchingGameplayTag(FAuraMyGameplayTags::Get().Player_Block_InputPressed))
	// {
	// 	return;
	// }
	// Set targeting status
	// if (InputTag.MatchesTagExact())
	// {
	// }

	// ASC handle input
	if (UMyAbilitySystemComponent* MyASC = GetMyASC())
	{
		MyASC->AbilityInputTagPressed(InputTag);
	}
}

void AMyPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UMyAbilitySystemComponent* MyASC = GetMyASC())
	{
		MyASC->AbilityInputTagReleased(InputTag);
	}
}

void AMyPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
}
