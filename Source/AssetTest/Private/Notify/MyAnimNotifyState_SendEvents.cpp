#include "Notify/MyAnimNotifyState_SendEvents.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UMyAnimNotifyState_SendEvents::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), BeginEvent, Payload);

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	UE_LOG(LogTemp, Warning,
		TEXT("NotifyBegin Role=%d AnimInstance=%p Montage=%p"),
		(int32)MeshComp->GetOwnerRole(),
		AnimInstance,
		AnimInstance ? AnimInstance->GetCurrentActiveMontage() : nullptr);
}

void UMyAnimNotifyState_SendEvents::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EndEvent, Payload);
}
