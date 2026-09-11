#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MyGameplayAbility_Damage.h"
#include "MyGameplayAbility_NormalAttack.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyGameplayAbility_NormalAttack : public UMyGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	UMyGameplayAbility_NormalAttack();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// UFUNCTION(BlueprintCallable)
	// void HitBoxOverlap(FGameplayEventData Payload);
	//
	// UFUNCTION(BlueprintCallable)
	// void ApplyDamageEffect(AActor* Target);
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Abilities)
	// float HitBoxRadius = 100.0f;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Abilities)
	// float HitBoxForwardOffset = 100.0f;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Abilities)
	// float HitBoxElevationOffset = 20.0f;
	//
	// UPROPERTY(EditDefaultsOnly)
	// float WarpingRadius = 200.f;

	// UPROPERTY(EditAnywhere)
	// TSubclassOf<UGameplayEffect> DamageEffectClass;

	// UPROPERTY(EditDefaultsOnly, Category = "Damage")
	// FScalableFloat Damage;
};
