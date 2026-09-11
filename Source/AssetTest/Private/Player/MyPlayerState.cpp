#include "Player/MyPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "AbilitySystem/MyAbilitySystemComponent.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "Inventory/MyInventoryComponent.h"
#include "Online/MyReplicationGraph.h"
#include "Online/MyConnectionManager.h"

AMyPlayerState::AMyPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UMyAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // Mixed for player

	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>("AttributeSet");

	InventoryComponent = CreateDefaultSubobject<UMyInventoryComponent>("InventoryComponent");

	SetNetUpdateFrequency(100.f);
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, Level);
	DOREPLIFETIME(AMyPlayerState, XP);
}

// Called before BeginPlay
// void AMyPlayerState::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
//
// 	// @Lyra: Init ASC and Give startup abilities
// 	// But at this point the PrivatePawn has not been set...
// 	check(AbilitySystemComponent);
// 	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
//
// 	if (AMyGameModeBase* MyGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(this)))
// 	{
// 		TArray<FGameplayAbilitySpecHandle> GrantedHandles;
// 		MyGameMode->PlayerAbilitySet->GiveToAbilitySystem(AbilitySystemComponent, GrantedHandles);
// 	}
// }

UAbilitySystemComponent* AMyPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AMyPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// Outside of the connection graph we need to call into SetTeamForPlayerController whenever the team index changes.
	// As this is a simple example we just do it once in the BeginPlay of the PlayerState.
	if (HasAuthority())
	{
		Team = FMath::RandRange(1, 3);
		SetTeamForConnection(Team);
	}
}

UMyInventoryComponent* AMyPlayerState::GetInventory() const
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory Comp is null"));
	}
	return InventoryComponent;
}

void AMyPlayerState::SetTeamForConnection(uint8 NewTeam)
{
	// Notify connection manager when we change team
	if (UWorld* World = GetWorld())
	{
		if (UNetDriver* NetworkDriver = World->GetNetDriver()) // GetNetConnection()->GetDriver()?
		{
			if (UMyReplicationGraph* RepGraph = NetworkDriver->GetReplicationDriver<UMyReplicationGraph>())
			{
				RepGraph->SetTeamForPlayerController(GetPlayerController(), NewTeam);
			}
		}
	}
}

void AMyPlayerState::SetTeam(uint8 NewTeam)
{
	if (NewTeam == Team)
	{
		return;
	}

	Team = NewTeam;

	// Change connection's team
	// At this point NetConnection may has not been prepared, make sure we don't call GetNetConnection()
	SetTeamForConnection(NewTeam);

	OnTeamChanged.Broadcast(NewTeam);
}

void AMyPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level, true);
}

void AMyPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AMyPlayerState::OnRep_Team()
{
	OnTeamChanged.Broadcast(Team);
}
