#include "Movement/MyCustomMovement.h"

#include "Kismet/KismetSystemLibrary.h"

void UMyCustomMovement::InitializeMovement(const FVector& InStartLocation, const FVector& InEndLocation, float InDuration)
{
	// Reset properties
	ElapsedTime = 0.f;
	LastFrameLocation = InStartLocation;
	LastFrameVelocity = FVector::ZeroVector;

	StartLocation = InStartLocation;
	EndLocation = InEndLocation;
	Duration = InDuration;

	if (bDrawDebugs)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, StartLocation, 20, 12, FColor::White, 3);
		UKismetSystemLibrary::DrawDebugSphere(this, EndLocation, 20, 12, FColor::White, 3);
	}
}

void UMyCustomMovement::StartMovement(UCharacterMovementComponent* MoveComp)
{
	// BP_StartMovement(MoveComp);
	OnMovementStart.Broadcast();
}

FVector UMyCustomMovement::TickDeltaMovement(UCharacterMovementComponent* MoveComp, float DeltaTime)
{
	ElapsedTime += DeltaTime;
	// BP_TickDeltaMovement(MoveComp, DeltaTime);
	return FVector::ZeroVector;
}

void UMyCustomMovement::EndMovement(UCharacterMovementComponent* MoveComp)
{
	// BP_EndMovement(MoveComp);
	OnMovementEnd.Broadcast();
}

bool UMyCustomMovement::IsFinished() const
{
	return ElapsedTime >= Duration;
}
