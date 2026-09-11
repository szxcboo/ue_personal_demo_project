#include "AbilitySystem/Abilities/MyGameplayAbility_NormalAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "MyBlueprintLibrary.h"
#include "MyGameplayTags.h"


UMyGameplayAbility_NormalAttack::UMyGameplayAbility_NormalAttack()
{
	AbilityTags.AddTag(MyGameplayTags::Abilities::Player::Normal);
}

void UMyGameplayAbility_NormalAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Apply block hit react GE
	// FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(BlockHitReactEffect, 1.f, GetAbilitySystemComponentFromActorInfo()->MakeEffectContext());
	// GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// UAnimMontage* MontageToPlay = Cast<AC_CharacterBase>(GetAvatarActorFromActorInfo())->GetAttackMontage();
	if (IsValid(MontageToPlay))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), MontageToPlay);

		PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::EndAbilitySimple);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this, &ThisClass::EndAbilitySimple);
		PlayMontageAndWait->OnCancelled.AddDynamic(this, &ThisClass::EndAbilitySimple);

		PlayMontageAndWait->Activate();
	}

	// Execute hit box overlap test and apply damage to all hit actors
	UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MyGameplayTags::Events::Player::Normal);
	WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::HitBoxOverlap);
	WaitGameplayEvent->Activate();
}

// void UMyGameplayAbility_NormalAttack::HitBoxOverlap(FGameplayEventData Payload)
// {
// 	if (!IsValid(DamageEffectClass)) return;
//
// 	TArray<AActor*> ActorsHit = UMyBlueprintLibrary::HitBoxOverlapTest(GetAvatarActorFromActorInfo(), HitBoxRadius, HitBoxForwardOffset, HitBoxElevationOffset, bDrawDebugs);
// 	for (AActor* HitActor : ActorsHit)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Hit " + HitActor->GetName());
// 		ApplyDamageEffect(HitActor);
// 	}
// 	SendHitReactEventToActors(ActorsHit);
// }
//
// void UMyGameplayAbility_NormalAttack::ApplyDamageEffect(AActor* Target)
// {
// 	if (!IsValid(Target) || !IsValid(DamageEffectClass)) return;
//
// 	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();;
// 	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
//
// 	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
//
// 	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
// }
