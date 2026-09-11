#pragma once

#include "CoreMinimal.h"
#include "MyAbilityBase.h"
#include "MyGameplayAbility_Damage.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyGameplayAbility_Damage : public UMyAbilityBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void SendHitReactEventToActors(const TArray<AActor*>& ActorsHit);
	
protected:
	UFUNCTION(BlueprintCallable)
	void HitBoxOverlap(FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable)
	void ApplyDamageEffect(AActor* Target);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Abilities)
	float HitBoxRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Abilities)
	float HitBoxForwardOffset = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Abilities)
	float HitBoxElevationOffset = 20.0f;

	UPROPERTY(EditDefaultsOnly)
	float WarpingRadius = 200.f;
};
