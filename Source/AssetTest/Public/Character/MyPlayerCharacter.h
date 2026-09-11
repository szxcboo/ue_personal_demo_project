#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "MyCharacterBase.h"
#include "MyPlayerCharacter.generated.h"

class UMyInventoryComponent;
class USpringArmComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemComponentInitialized, UAbilitySystemComponent*, ASC);

UCLASS()
class ASSETTEST_API AMyPlayerCharacter : public AMyCharacterBase
{
	GENERATED_BODY()

public:
	AMyPlayerCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAttributeSet* GetAttributeSet() const override;

	UPROPERTY(BlueprintAssignable)
	FOnAbilitySystemComponentInitialized OnAbilitySystemComponentInitialized;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UStaticMeshComponent> Weapon;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
};
