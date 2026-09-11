#include "Character/MyCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "Movement/MyCharacterMovementComponent.h"

AMyCharacterBase::AMyCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void AMyCharacterBase::Ragdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	Destroy();
}

void AMyCharacterBase::GiveStartupAbilities()
{
	checkf(IsValid(GetAbilitySystemComponent()), TEXT("AbilitySystemComponent not set"));

	if (UMyAbilitySystemComponent* MyASC = Cast<UMyAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		MyASC->AddCharacterAbilities(StartupAbilities);
	}
}

void AMyCharacterBase::InitializeAttributes() const
{
	checkf(IsValid(GetAbilitySystemComponent()), TEXT("AbilitySystemComponent not set"));
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set"));

	// Apply GameplayEffect
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()); // Dereference
}
