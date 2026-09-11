#include "AbilitySystem/MyAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" // FGameplayEffectModCallbackData

UMyAttributeSet::UMyAttributeSet()
{
}

void UMyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, DefensePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	// Used by the prediction system
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}

void UMyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxHealth, OldMaxHealth);
}

void UMyAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Mana, OldMana);
}

void UMyAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxMana, OldMaxMana);
}

void UMyAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, AttackPower, OldAttackPower);
}

void UMyAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, DefensePower, OldDefensePower);
}

// void UC_AttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
// {
// 	GAMEPLAYATTRIBUTE_REPNOTIFY(UC_AttributeSet, Armor, OldArmor);
// }

void UMyAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
}

void UMyAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// SetEffectProperties(Data, Props);

	// if (Attribute == GetMaxHealthAttribute())
	// {
	// 	SetHealth(GetMaxHealth());
	// }
	// if (Attribute == GetMaxManaAttribute())
	// {
	// 	SetMana(GetMaxMana());
	// }
}

void UMyAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f); // Reset
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		// const bool bFatal = NewHealth <= 0.f;
		// if (bFatal)
		// {
		// 	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
		// 	if (CombatInterface)
		// 	{
		// 		FVector Impulse = UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle);
		// 		CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
		// 	}
		// 	SendXPEvent(Props);
		// }
		// else
		// {
		// 	if (Props.TargetCharacter->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter))
		// 	{
		// 		FGameplayTagContainer TagContainer;
		// 		TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
		// 		Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		// 	}
		// 	
		// 	const FVector& KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
		// 	if (!KnockbackForce.IsNearlyZero(1.f))
		// 	{
		// 		Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
		// 	}
		// }
		//
		// const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
		// const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		// ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		// if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
		// {
		// 	Debuff(Props);
		// }
	}
}
