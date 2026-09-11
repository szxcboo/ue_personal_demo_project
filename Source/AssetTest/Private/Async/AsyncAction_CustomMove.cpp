#include "Async/AsyncAction_CustomMove.h"

#include "GameFramework/Character.h"
#include "Movement/MyCharacterMovementComponent.h"
#include "Movement/MyCustomMovement.h"

UAsyncAction_CustomMove* UAsyncAction_CustomMove::PerformCustomMove(ACharacter* CharacterToMove, const FMyCustomMovementSpec& MovementSpec)
{
	check(CharacterToMove);

	UAsyncAction_CustomMove* MyObj = NewObject<UAsyncAction_CustomMove>();

	MyObj->CharacterMovementComponent = CharacterToMove->GetCharacterMovement<UMyCharacterMovementComponent>();
	MyObj->MovementSpec = MovementSpec;

	return MyObj;
}

void UAsyncAction_CustomMove::Activate()
{
	UMyCustomMovement* Movement = CharacterMovementComponent->StartCustomMovement(MovementSpec);

	if (ShouldBroadcastDelegates() && IsValid(Movement))
	{
		Movement->OnMovementEnd.AddDynamic(this, &ThisClass::OnMovementCompleted);
	}
}

void UAsyncAction_CustomMove::Cancel()
{
	if (ShouldBroadcastDelegates())
	{
		OnCancelled.Broadcast();
	}
	EndAction();
}

void UAsyncAction_CustomMove::EndAction()
{
	// Child classes should override this if they need to explicitly unbind delegates that aren't just using weak pointers
	// Clear our CMC so it won't broadcast delegates
	CharacterMovementComponent = nullptr;
	SetReadyToDestroy();
}

bool UAsyncAction_CustomMove::ShouldBroadcastDelegates() const
{
	// By default, broadcast if our CMC is valid
	if (CharacterMovementComponent.IsValid())
	{
		return true;
	}
	return false;
}

void UAsyncAction_CustomMove::OnMovementCompleted()
{
	OnCompleted.Broadcast();
}