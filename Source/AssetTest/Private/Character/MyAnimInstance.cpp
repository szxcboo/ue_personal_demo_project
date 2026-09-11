#include "Character/MyAnimInstance.h"

#include "Character/MyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"


void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AMyCharacterBase>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovement)
	{
		GroundSpeed = CharacterMovement->Velocity.Size2D();
	}
}
