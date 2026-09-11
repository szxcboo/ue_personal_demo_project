#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyCustomMovement.generated.h"

class UCharacterMovementComponent;
class UMyCustomMovement;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMyCustomMovementSpec
{
	GENERATED_BODY()

	FMyCustomMovementSpec()
		: MovementClass(nullptr),
		  StartLocation(FVector()),
		  EndLocation(FVector()),
		  Duration(0.f)
	{
	}

	FMyCustomMovementSpec(const TSubclassOf<UMyCustomMovement>& InMovement, const FVector& InStartLocation, const FVector& InEndLocation, float InDuration)
		: MovementClass(InMovement),
		  StartLocation(InStartLocation),
		  EndLocation(InEndLocation),
		  Duration(InDuration)
	{
	}

	/** Movement of the spec (Always the CDO) */
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UMyCustomMovement> MovementClass;

	UPROPERTY(BlueprintReadWrite)
	FVector StartLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector EndLocation;

	UPROPERTY(BlueprintReadWrite)
	float Duration;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementCompleted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementCanceled);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementInterrupted, FHitResult, Hit);

/**
 * 
 */
UCLASS(Blueprintable)
class ASSETTEST_API UMyCustomMovement : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsSupportedForNetworking() const override { return true; }

	/** Initialize this movement before it starts */
	UFUNCTION(BlueprintCallable)
	virtual void InitializeMovement(const FVector& InStartLocation, const FVector& InEndLocation, float InDuration);

	/** Called when the movement starts */
	virtual void StartMovement(UCharacterMovementComponent* MoveComp);

	/** Calculate the velocity for delta time */
	virtual FVector TickDeltaMovement(UCharacterMovementComponent* MoveComp, float DeltaTime);

	/** Called when the movement ends */
	virtual void EndMovement(UCharacterMovementComponent* MoveComp);

	// /** Blueprint implementable event for movement starting */
	// UFUNCTION(BlueprintImplementableEvent)
	// void BP_StartMovement(UCharacterMovementComponent* MoveComp);
	//
	// /** Blueprint implementable function for calculating the velocity every frame */
	// UFUNCTION(BlueprintImplementableEvent)
	// FVector BP_TickDeltaMovement(UCharacterMovementComponent* MoveComp, float DeltaTime);
	//
	// /** Blueprint implementable event for movement ending */
	// UFUNCTION(BlueprintImplementableEvent)
	// void BP_EndMovement(UCharacterMovementComponent* MoveComp);

	UPROPERTY(BlueprintAssignable)
	FOnMovementStart OnMovementStart;

	UPROPERTY(BlueprintAssignable)
	FOnMovementEnd OnMovementEnd;

	UPROPERTY(BlueprintAssignable)
	FOnMovementCompleted OnMovementCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnMovementCanceled OnMovementCanceled;

	UPROPERTY(BlueprintAssignable)
	FOnMovementInterrupted OnMovementInterrupted;

	/** Whether this movement is finished */
	bool IsFinished() const;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugs = false;

protected:
	TObjectPtr<ACharacter> OwnerCharacter;

	FVector StartLocation;
	FVector EndLocation;
	float Duration;

	float ElapsedTime;
	FVector LastFrameVelocity;
	FVector LastFrameLocation;
};
