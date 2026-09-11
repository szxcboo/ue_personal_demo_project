#include "Movement/MyCustomMovement_Curved.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UMyCustomMovement_Curved::PostInitProperties()
{
	Super::PostInitProperties();

	// Check if the curve is valid
	FString ContextString = FString();
	CurveValid = Curve.IsValid(ContextString)
		&& FMath::IsNearlyEqual(Curve.Eval(0.f, ContextString), 0.f)
		&& FMath::IsNearlyEqual(Curve.Eval(1.f, ContextString), 1.f);
}

FVector UMyCustomMovement_Curved::TickDeltaMovement(UCharacterMovementComponent* MoveComp, float DeltaTime)
{
	Super::TickDeltaMovement(MoveComp, DeltaTime);

	if (!CurveValid) return FVector::Zero();
	FString ContextString = FString();

	// Calculate the velocity this frame
	// Velocity = (D * f (x / T))' = D / T * f'(x)
	// f'(x) = f(x + delta_x) - f(x - delta_x) / (2 * delta_X)
	float X = (ElapsedTime == 0.f) ? 0.f : (ElapsedTime / Duration); // Safe devide
	float DeltaX = DeltaTime / Duration;
	float Distance = (EndLocation - StartLocation).Length();
	float X1 = FMath::Clamp(X + DeltaX, 0, 1);
	float X2 = FMath::Clamp(X - DeltaX, 0, 1);
	float Velocity = (Distance / Duration) *
		((Curve.Eval(X1, ContextString) - Curve.Eval(X2, ContextString)) / (2 * DeltaX));

	// Draw debugs
	if (bDrawDebugs)
	{
		UKismetSystemLibrary::DrawDebugPoint(this, MoveComp->GetActorLocation(), 5.f, FColor::Green, 5.f);
	}

	return Velocity * (EndLocation - StartLocation).GetUnsafeNormal() * DeltaTime;
}
