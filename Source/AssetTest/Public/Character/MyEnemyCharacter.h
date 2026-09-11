#pragma once

#include "CoreMinimal.h"
#include "MyCharacterBase.h"
#include "MyEnemyCharacter.generated.h"

class UMyAbilitySet;
class UBehaviorTree;
class AMyAIController;
class UWidgetComponent;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttributesInitialized);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS()
class ASSETTEST_API AMyEnemyCharacter : public AMyCharacterBase
{
	GENERATED_BODY()

public:
	AMyEnemyCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributesInitialized OnAttributesInitialized;

	/* Bind the OnAttributeChanged delegates in health bar widget blueprint */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<AMyAIController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

private:
	void HandleHealthChanged(const FOnAttributeChangeData& AttributeChangeData);
	void HandleDeath();

	UPROPERTY(Replicated)
	bool bAlive = true;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TObjectPtr<UMyAbilitySet> AbilitySet;

public:
	FORCEINLINE bool IsAlive() const { return bAlive; }
};
