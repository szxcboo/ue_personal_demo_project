#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "MyCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttributeSetInitialized);

UCLASS(Abstract)
class ASSETTEST_API AMyCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMyCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return nullptr; }
	virtual UAttributeSet* GetAttributeSet() const { return nullptr; }

	void Ragdoll();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeSetInitialized OnAttributeSetInitialized;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CharacterTag;

protected:
	void GiveStartupAbilities();
	void InitializeAttributes() const;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

private:
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
};
