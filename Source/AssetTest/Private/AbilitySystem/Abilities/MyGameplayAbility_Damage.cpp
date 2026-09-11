#include "AbilitySystem/Abilities/MyGameplayAbility_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MyBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

void UMyGameplayAbility_Damage::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, FGameplayTag()/* DamageType */, ScaledDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

void UMyGameplayAbility_Damage::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)
{
	for (AActor* HitActor : ActorsHit)
	{
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, MyGameplayTags::Events::HitReact, Payload);
	}
}

void UMyGameplayAbility_Damage::HitBoxOverlap(FGameplayEventData Payload)
{
	if (!IsValid(DamageEffectClass)) return;

	TArray<AActor*> ActorsHit = UMyBlueprintLibrary::HitBoxOverlapTest(GetAvatarActorFromActorInfo(), HitBoxRadius, HitBoxForwardOffset, HitBoxElevationOffset, bDrawDebugs);
	for (AActor* HitActor : ActorsHit)
	{
		UKismetSystemLibrary::PrintString(this, "Hit " + HitActor->GetName());
		ApplyDamageEffect(HitActor);
	}
	SendHitReactEventToActors(ActorsHit);
}

void UMyGameplayAbility_Damage::ApplyDamageEffect(AActor* Target)
{
	if (!IsValid(Target) || !IsValid(DamageEffectClass)) return;

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();;
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}
