#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Movement/MyCustomMovement.h"
#include "AsyncAction_CustomMove.generated.h"

class UMyCharacterMovementComponent;
class UMyCustomMovement;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementSimpleDelegate);

/**
 * 
 */
UCLASS()
class ASSETTEST_API UAsyncAction_CustomMove : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// TODO Bind the delegates to CMC or movement obj
	UPROPERTY(BlueprintAssignable)
	FMovementSimpleDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FMovementSimpleDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FMovementSimpleDelegate OnCancelled;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncAction_CustomMove* PerformCustomMove(ACharacter* CharacterToMove, const FMyCustomMovementSpec& MovementSpec);

	/** Called to trigger the action once the delegates have been bound */
	virtual void Activate() override;

	/** Cancel an asynchronous action, this attempts to cancel any lower level processes and also prevents delegates from being fired */
	UFUNCTION(BlueprintCallable)
	virtual void Cancel();

	/** Explicitly end the action, will disable any callbacks and allow action to be destroyed */
	UFUNCTION(BlueprintCallable)
	virtual void EndAction();

	/** This should be called prior to broadcasting delegates back into the event graph, this ensures the action and CMC are still valid */
	virtual bool ShouldBroadcastDelegates() const;

private:
	UFUNCTION()
	void OnMovementCompleted();
	
	TWeakObjectPtr<UMyCharacterMovementComponent> CharacterMovementComponent;
	
	FMyCustomMovementSpec MovementSpec;
};
