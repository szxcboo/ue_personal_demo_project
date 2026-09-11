#pragma once

#include "CoreMinimal.h"
#include "MyCustomMovement.h"
#include "MyCustomMovement_Curved.generated.h"

/**
 * Perform a custom movement based on a given curve
 * The curve must be normalize, and has at least two keys (0. 0) and (1, 1), representing the start and end point
 * The the X axis represents the timeline, and the Y axis represents the distance progress of this movement
 * The slope of the curve will be the velocity
 */
UCLASS()
class ASSETTEST_API UMyCustomMovement_Curved : public UMyCustomMovement
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual FVector TickDeltaMovement(UCharacterMovementComponent* MoveComp, float DeltaTime) override;

private:
	/** Normalize Distance Progress - Timeline Curve */
	UPROPERTY(EditDefaultsOnly)
	FCurveTableRowHandle Curve;

	bool CurveValid;
};
