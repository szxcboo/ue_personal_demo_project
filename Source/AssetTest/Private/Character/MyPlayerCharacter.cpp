#include "Character/MyPlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemComponent.h"
#include "Game/MyGameModeBase.h"
#include "Player/MyPlayerState.h"
#include "AbilitySystem/MyAbilitySet.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->AddLocalRotation(FRotator(0.f, -30.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false; // Fixing the camera
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraLag = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Set variables which needs to be replicated
void AMyPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Server/Standalone only, called after BeginPlay
void AMyPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Initialize ASC
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	ASC->InitAbilityActorInfo(GetPlayerState(), this);

	// Give abilities using AbilitySet, and they will be replicated down to clients
	if (UWorld* World = GetWorld())
	{
		AMyGameModeBase* GameMode = World->GetAuthGameMode<AMyGameModeBase>();

		TArray<FGameplayAbilitySpecHandle> GrantedHandles;
		GameMode->PlayerAbilitySet->GiveToAbilitySystem(ASC, GrantedHandles);
	}

	OnAbilitySystemComponentInitialized.Broadcast(GetAbilitySystemComponent());

	InitializeAttributes();
}

// Client only, called after BeginPlay
void AMyPlayerCharacter::OnRep_PlayerState()
{
	// Reset replicated Player State
	Super::OnRep_PlayerState();

	// Initialize ASC
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	ASC->InitAbilityActorInfo(GetPlayerState(), this);

	OnAbilitySystemComponentInitialized.Broadcast(GetAbilitySystemComponent());

	// UCC_AttributeSet* CC_AttributeSet = Cast<UCC_AttributeSet>(GetAttributeSet());
	// if (IsValid(CC_AttributeSet))
	// {
	// 	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(CC_AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	// }
}

void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Player"));
}

UAbilitySystemComponent* AMyPlayerCharacter::GetAbilitySystemComponent() const
{
	AMyPlayerState* MyPS = GetPlayerState<AMyPlayerState>();
	if (IsValid(MyPS))
	{
		return MyPS->GetAbilitySystemComponent();
	}
	return nullptr;
}

UAttributeSet* AMyPlayerCharacter::GetAttributeSet() const
{
	AMyPlayerState* MyPS = GetPlayerState<AMyPlayerState>();
	if (IsValid(MyPS))
	{
		return MyPS->GetAttributeSet();
	}
	return nullptr;
}
