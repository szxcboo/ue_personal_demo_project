#pragma once

/**
 * Note: The order of the activation of an ability is:
 * ASC: TryActivateAbility -> Client/Server...
 * ASC: InternalTryActivateAbility->
 * GA: CallActivateAbility ->
 * GA: Native ActivateAbility -> Parent ActivateAbility if called Super
 * Ends up the K2_ActivateAbility(Blueprint Event) will be called.
 * So all the parent logic will be executed anyway if we implemented the Blueprint Event.
 */

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMyAbilityBase();
	
	UFUNCTION()
	void EndAbilitySimple();

protected:
	UPROPERTY(EditAnywhere)
	bool bDrawDebugs = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> MontageToPlay;
};
