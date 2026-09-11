#include "Movement/MyCharacterMovementComponent.h"

#include "Engine/ActorChannel.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Movement/MyCustomMovement.h"

UMyCharacterMovementComponent::UMyCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// bReplicateUsingRegisteredSubObjectList = true; // Use subobject list
}

bool UMyCharacterMovementComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	// Replicate custom movement lists
	for (UMyCustomMovement* Movement : CustomMovements)
	{
		if (Movement)
		{
			WroteSomething |= Channel->ReplicateSubobject(Movement, *Bunch, *RepFlags);
		}
	}
	for (UMyCustomMovement* Movement : ActiveMovements)
	{
		if (Movement)
		{
			WroteSomething |= Channel->ReplicateSubobject(Movement, *Bunch, *RepFlags);
		}
	}
	return WroteSomething;
}

UMyCustomMovement* UMyCharacterMovementComponent::AssignCustomMovementInstance(TSubclassOf<UMyCustomMovement> CustomMovementClass)
{
	AActor* Owner = GetOwner();
	check(Owner);

	UMyCustomMovement* MovementInstance = NewObject<UMyCustomMovement>(Owner, CustomMovementClass.Get());
	check(MovementInstance);

	// Add it to one of our instance lists so that it doesn't GC.
	CustomMovements.Add(MovementInstance);

	return MovementInstance;
}

void UMyCharacterMovementComponent::StartCustomMovementByRef(UMyCustomMovement* CustomMovement)
{
	if (CustomMovement)
	{
		ActiveMovements.Add(CustomMovement);
	}
	// Start custom move
	SetMovementMode(MOVE_Custom);
}

UMyCustomMovement* UMyCharacterMovementComponent::StartCustomMovement(const FMyCustomMovementSpec& Spec)
{
	UMyCustomMovement* Result = nullptr;
	for (UMyCustomMovement* Movement : CustomMovements)
	{
		if (Movement && Movement->GetClass() == Spec.MovementClass.Get())
		{
			ActiveMovements.Add(Movement);
			Movement->InitializeMovement(Spec.StartLocation, Spec.EndLocation, Spec.Duration);
			Result = Movement;
		}
	}
	// Start custom move
	SetMovementMode(MOVE_Custom);

	return Result;
}

void UMyCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	// @CharacterMovementComponent::PhysWalking()
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// Time slice to avoid low frame rate
	float RemainingTime = DeltaTime;

	const EMovementMode StartingMovementMode = MovementMode;
	const uint8 StartingCustomMovementMode = CustomMovementMode;

	// Perform the move
	while (RemainingTime >= MIN_TICK_TIME && Iterations < MaxSimulationIterations)
	{
		Iterations++;
		const float TimeTick = GetSimulationTimeStep(RemainingTime, Iterations);
		RemainingTime -= TimeTick;

		// Save current values
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Accumulate velocity from custom movements
		FVector AccumulatedVelocity = FVector::ZeroVector;
		
		// for (UMyCustomMovement* Movement : ActiveMovements)
		// {
		// 	// Accumulate velocity
		// 	AccumulatedVelocity += Movement->TickDeltaMovement(this, TimeTick);
		//
		// 	if (Movement->IsFinished())
		// 	{
		// 		SetMovementMode(MOVE_Walking);
		// 		break;
		// 	}
		// }

		// Loop over active movement list
		for (auto MovementIt = ActiveMovements.CreateIterator(); MovementIt; ++MovementIt)
		{
			UMyCustomMovement* Movement = *MovementIt;
			
			// Accumulate velocity
			AccumulatedVelocity += Movement->TickDeltaMovement(this, TimeTick);

			// Remove finished movements from list
			if (Movement->IsFinished())
			{
				Movement->OnMovementEnd.Broadcast();
				MovementIt.RemoveCurrent();
			}
		}
		
		// Actually move
		FHitResult Hit;
		SafeMoveUpdatedComponent(AccumulatedVelocity, UpdatedComponent->GetComponentRotation(), true, Hit);

		// Broadcast hit result
		if (Hit.bBlockingHit)
		{
			// Movement->OnMovementInterrupted.Broadcast(Hit);
		}

		// Draw debugs
		UKismetSystemLibrary::DrawDebugPoint(this, CharacterOwner->GetActorLocation(), 5.f, FColor::Blue, 5.f);

		// if (MovementMode != StartingMovementMode || CustomMovementMode != StartingCustomMovementMode)
		// {
		// 	// Root motion could have taken us out of our current mode
		// 	// No movement has taken place this movement tick so we pass on full time/past iteration count
		// 	StartNewPhysics(RemainingTime + TimeTick, Iterations - 1);
		// 	return;
		// }

		// Set movement mode back to walking if movements are all completed
		if (ActiveMovements.IsEmpty())
		{
			SetMovementMode(MOVE_Walking);
			break;
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			RemainingTime = 0.f;
			break;
		}
	}
}

void UMyCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Custom)
	{
		for (UMyCustomMovement* Movement: ActiveMovements)
		{
			Movement->StartMovement(this);
		}
	}
	if (PreviousMovementMode == MOVE_Custom)
	{
		for (UMyCustomMovement* Movement: ActiveMovements)
		{
			Movement->EndMovement(this);
		}
	}
}
