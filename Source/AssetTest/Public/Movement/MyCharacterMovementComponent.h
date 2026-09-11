#pragma once

#include "CoreMinimal.h"
#include "MyCustomMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterMovementComponent.generated.h"

class UMyCustomMovement;

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ASSETTEST_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UMyCharacterMovementComponent();
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/** Create a custom movement instance using the specific class, add it to the instance list and as a subobject of the component owner */
	UFUNCTION(BlueprintCallable)
	UMyCustomMovement* AssignCustomMovementInstance(TSubclassOf<UMyCustomMovement> CustomMovementClass);

	/** Start a custom movement using object reference */
	UFUNCTION(BlueprintCallable)
	void StartCustomMovementByRef(UMyCustomMovement* CustomMovement);

	/** Start a custom movement using spec */
	UFUNCTION(BlueprintCallable)
	UMyCustomMovement* StartCustomMovement(const FMyCustomMovementSpec& Spec);

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UMyCustomMovement>> CustomMovements;

	UPROPERTY()
	TArray<TObjectPtr<UMyCustomMovement>> ActiveMovements;
};
