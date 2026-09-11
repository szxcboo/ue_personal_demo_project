#include "Character/MyEnemyCharacter.h"

#include "AI/MyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "AbilitySystem/MyAbilitySet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/MyAttributeWidget.h"

AMyEnemyCharacter::AMyEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Minimal for AI enemies

	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true; // Set it true to use Rotate to Face AI Task

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	// GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	// GetMesh()->MarkRenderStateDirty();
	// Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	// Weapon->MarkRenderStateDirty();
	//
	// BaseWalkSpeed = 250.f;
	// GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void AMyEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bAlive);
}

void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySystemComponent);
	check(AttributeSet);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this); // When we have ASC directly on class, we can init it on BeginPlay
	}

	// Give startup abilities and initialize attributes on server
	if (HasAuthority())
	{
		if (IsValid(AbilitySystemComponent) && IsValid(AbilitySet))
		{
			TArray<FGameplayAbilitySpecHandle> GrantedHandles;
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, GrantedHandles);
		}

		InitializeAttributes();
	}

	for (FGameplayAbilitySpec Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		UKismetSystemLibrary::PrintString(this, Spec.GetDebugString());
	}

	// Set UI widgets on clients
	if (UMyAttributeWidget* AttributeWidget = Cast<UMyAttributeWidget>(HealthBar->GetUserWidgetObject()))
	{
		AttributeWidget->SetWidgetController(this);
	}

	if (const UMyAttributeSet* AS = Cast<UMyAttributeSet>(AttributeSet))
	{
		// Bind to the delegates on Attribute Set for setting UI
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
				// UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), Data.NewValue));
			});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
				// UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), Data.NewValue));
			});

		// AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
		// 	this,
		// 	&AAuraEnemy::HitReactTagChanged
		// );

		// Broadcast initial values
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
		OnHealthChanged.Broadcast(AS->GetHealth());

		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), AS->GetHealth()));

		// Bind to handle death
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	}
}

void AMyEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AIController = Cast<AMyAIController>(NewController);
	AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AIController->RunBehaviorTree(BehaviorTree);
	// AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	// AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AMyEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

UAbilitySystemComponent* AMyEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AMyEnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void AMyEnemyCharacter::HandleHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.NewValue <= 0.f)
	{
		HandleDeath();
	}
}

void AMyEnemyCharacter::HandleDeath()
{
	bAlive = false;
	AIController->StopMovement();
	Ragdoll();
}
