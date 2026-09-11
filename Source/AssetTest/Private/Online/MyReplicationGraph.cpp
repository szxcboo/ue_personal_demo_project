#include "Online/MyReplicationGraph.h"

#include "Online/MyConnectionManager.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Character/MyPlayerCharacter.h"
#include "Player/MyPlayerState.h"

float GridCellSize = 1000.f;
static FAutoConsoleVariableRef CVarRepGraphCellSize(TEXT("RepGraph.CellSize"), GridCellSize,TEXT(""));

// --------------------------------------------------------------------------------
// FTeamConnectionListMap
// --------------------------------------------------------------------------------

TArray<UMyConnectionManager*>* FTeamConnectionListMap::GetConnectionArrayForTeam(uint8 Team)
{
	return Find(Team);
}

TArray<UMyConnectionManager*> FTeamConnectionListMap::GetVisibleConnectionArrayForNonTeam(APawn* Pawn, uint8 Team)
{
	TArray<UMyConnectionManager*> NonTeamConnections;

	if (!IsValid(Pawn))
	{
		return NonTeamConnections;
	}

	// Setup query params and ignore all team members
	TArray<UMyConnectionManager*>* TeamMembers = GetConnectionArrayForTeam(Team);

	FCollisionQueryParams TraceParams;
	TraceParams.bDebugQuery = true;
	if (TeamMembers)
	{
		for (UMyConnectionManager* ConnectionManager : *TeamMembers)
		{
			TraceParams.AddIgnoredActor(ConnectionManager->ControlledPawn.Get());
		}
	}
	else
	{
		TraceParams.AddIgnoredActor(Pawn);
	}

	// Iterate over all teams that do not match the input team
	TArray<uint8> Teams;
	GetKeys(Teams);

	UWorld* World = Pawn->GetWorld();
	FVector TraceOffset = FVector(0.0f, 0.0f, 180.0f);
	FVector TraceStart = Pawn->GetActorLocation() + TraceOffset;
	for (int32 i = 0; i < Teams.Num(); i++)
	{
		uint8 TeamID = Teams[i];
		if (TeamID != Team)
		{
			TArray<UMyConnectionManager*>* OtherTeamMembers = GetConnectionArrayForTeam(TeamID);

			if (!OtherTeamMembers)
			{
				continue;
			}

			for (UMyConnectionManager* ConnectionManager : *OtherTeamMembers)
			{
				if (!ConnectionManager->ControlledPawn.IsValid())
				{
					continue;
				}

				// Raycast between our pawn and the other. If we hit anything then we do not have line of sight
				FHitResult OutHit;
				FVector TraceEnd = ConnectionManager->ControlledPawn.Get()->GetActorLocation() + TraceOffset;
				if (!World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_GameTraceChannel1, TraceParams))
				{
					NonTeamConnections.Add(ConnectionManager);
				}
			}
		}
	}

	return NonTeamConnections;
}

void FTeamConnectionListMap::AddConnectionToTeam(uint8 Team, UMyConnectionManager* ConnectionManager)
{
	TArray<UMyConnectionManager*>& TeamList = FindOrAdd(Team);
	TeamList.Add(ConnectionManager);
}

void FTeamConnectionListMap::RemoveConnectionFromTeam(uint8 Team, UMyConnectionManager* ConnectionManager)
{
	if (TArray<UMyConnectionManager*>* TeamList = Find(Team))
	{
		TeamList->RemoveSwap(ConnectionManager);

		// Remove the team from the map if there are no more connections
		if (TeamList->Num() == 0)
		{
			Remove(Team);
		}
	}
}

// --------------------------------------------------------------------------------
// UMyReplicationGraph
// --------------------------------------------------------------------------------

UMyReplicationGraph::UMyReplicationGraph()
{
	// Specify the connection graph class to use
	ReplicationConnectionManagerClass = UMyConnectionManager::StaticClass();
}

void UMyReplicationGraph::InitGlobalActorClassSettings()
{
	Super::InitGlobalActorClassSettings();

	// Programatically build the rules.
	ClassRepNodePolicies.Set(APlayerState::StaticClass(), EClassRepNodeMapping::RelevantAllConnections); // Special cased via UShooterReplicationGraphNode_PlayerStateFrequencyLimiter
	ClassRepNodePolicies.Set(AReplicationGraphDebugActor::StaticClass(), EClassRepNodeMapping::NotRouted); // Not needed. Replicated special case inside RepGraph
	ClassRepNodePolicies.Set(AInfo::StaticClass(), EClassRepNodeMapping::RelevantAllConnections); // Non spatialized, relevant to all
	ClassRepNodePolicies.Set(AGameStateBase::StaticClass(), EClassRepNodeMapping::RelevantAllConnections);
	ClassRepNodePolicies.Set(APlayerController::StaticClass(), EClassRepNodeMapping::RelevantForConnection);
	ClassRepNodePolicies.Set(AMyPlayerCharacter::StaticClass(), EClassRepNodeMapping::RelevantForTeam); // Replicate player pawn for the same team

	// @Shooter
	TArray<UClass*> AllReplicatedClasses;

	// ReplicationGraph stores internal associative data for actor classes. 
	// We build this data here based on actor CDO values.
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		AActor* ActorCDO = Cast<AActor>(Class->GetDefaultObject());
		if (!ActorCDO || !ActorCDO->GetIsReplicated())
		{
			continue;
		}

		// Skip SKEL and REINST classes.
		if (Class->GetName().StartsWith(TEXT("SKEL_")) || Class->GetName().StartsWith(TEXT("REINST_")))
		{
			continue;
		}

		// FClassReplicationInfo ClassInfo;
		//
		// // Replication Graph is frame based. Convert NetUpdateFrequency to ReplicationPeriodFrame based on Server MaxTickRate.
		// ClassInfo.ReplicationPeriodFrame = GetReplicationPeriodFrameForFrequency(ActorCDO->GetNetUpdateFrequency());
		//
		// if (ActorCDO->bAlwaysRelevant || ActorCDO->bOnlyRelevantToOwner)
		// {
		// 	ClassInfo.SetCullDistanceSquared(0.f);
		// }
		// else
		// {
		// 	ClassInfo.SetCullDistanceSquared(ActorCDO->GetNetCullDistanceSquared());
		// }
		//
		// GlobalActorReplicationInfoMap.SetClassInfo(Class, ClassInfo);

		// @Shooter
		// This is a replicated class. Save this off for the second pass below
		AllReplicatedClasses.Add(Class);

		// Skip if already in the map (added explicitly)
		if (ClassRepNodePolicies.Contains(Class, true)) // Check super classes for blueprints
		{
			continue;
		}

		auto ShouldSpatialize = [](const AActor* CDO)
		{
			return !CDO->bAlwaysRelevant && !CDO->bOnlyRelevantToOwner && !CDO->bNetUseOwnerRelevancy;
		};

		if (ShouldSpatialize(ActorCDO))
		{
			ClassRepNodePolicies.Set(Class, EClassRepNodeMapping::Spatialize_Dynamic); // Add info
		}
		else if (ActorCDO->bAlwaysRelevant && !ActorCDO->bOnlyRelevantToOwner)
		{
			ClassRepNodePolicies.Set(Class, EClassRepNodeMapping::RelevantAllConnections);
		}
		else if (ActorCDO->bOnlyRelevantToOwner && !ActorCDO->bAlwaysRelevant)
		{
			ClassRepNodePolicies.Set(Class, EClassRepNodeMapping::RelevantForConnection);
		}
	}

	// Setup FClassReplicationInfo. This is essentially the per class replication settings. Some we set explicitly, the rest we are setting via looking at the legacy settings on AActor.
	TArray<UClass*> ExplicitlySetClasses;

	// Explicitly set for Pawn
	FClassReplicationInfo PawnRepInfo;
	PawnRepInfo.DistancePriorityScale = 1.f;
	PawnRepInfo.StarvationPriorityScale = 1.f;
	PawnRepInfo.ActorChannelFrameTimeout = 4;
	PawnRepInfo.SetCullDistanceSquared(1500.f * 1500.f);
	GlobalActorReplicationInfoMap.SetClassInfo(APawn::StaticClass(), PawnRepInfo);
	ExplicitlySetClasses.Add(APawn::StaticClass());

	// Explicitly set for PlayerState
	FClassReplicationInfo PlayerStateRepInfo;
	PlayerStateRepInfo.DistancePriorityScale = 0.f;
	PlayerStateRepInfo.ActorChannelFrameTimeout = 0;
	GlobalActorReplicationInfoMap.SetClassInfo(APlayerState::StaticClass(), PlayerStateRepInfo);
	ExplicitlySetClasses.Add(APlayerState::StaticClass());

	auto InitClassReplicationInfo = [&](FClassReplicationInfo& Info, UClass* Class, bool bSpatialize)
	{
		AActor* CDO = Class->GetDefaultObject<AActor>();
		if (bSpatialize)
		{
			Info.SetCullDistanceSquared(CDO->NetCullDistanceSquared);
			UE_LOG(LogReplicationGraph, Log, TEXT("Setting cull distance for %s to %f (%f)"), *Class->GetName(), Info.GetCullDistanceSquared(), Info.GetCullDistance());
		}

		Info.ReplicationPeriodFrame = GetReplicationPeriodFrameForFrequency(CDO->GetNetUpdateFrequency());

		UClass* NativeClass = Class;
		while (!NativeClass->IsNative() && NativeClass->GetSuperClass() && NativeClass->GetSuperClass() != AActor::StaticClass())
		{
			NativeClass = NativeClass->GetSuperClass();
		}

		UE_LOG(LogReplicationGraph, Log, TEXT("Setting replication period for %s (%s) to %d frames (%.2f)"), *Class->GetName(), *NativeClass->GetName(), Info.ReplicationPeriodFrame, CDO->NetUpdateFrequency);
	};

	// Set FClassReplicationInfo based on legacy settings from all replicated classes
	for (UClass* ReplicatedClass : AllReplicatedClasses)
	{
		// Skip all child classes of explicitly set classes
		if (ExplicitlySetClasses.FindByPredicate([&](const UClass* SetClass) { return ReplicatedClass->IsChildOf(SetClass); }))
		{
			continue;
		}

		bool bClassIsSpatialized = ClassRepNodePolicies.GetChecked(ReplicatedClass) >= EClassRepNodeMapping::Spatialize_Static; // Is spatialized

		FClassReplicationInfo ClassInfo;
		InitClassReplicationInfo(ClassInfo, ReplicatedClass, bClassIsSpatialized);
		GlobalActorReplicationInfoMap.SetClassInfo(ReplicatedClass, ClassInfo);
	}
}

void UMyReplicationGraph::InitGlobalGraphNodes()
{
	AlwaysRelevantNode = CreateNewNode<UMyReplicationGraphNode_AlwaysRelevant_WithPending>();
	AddGlobalGraphNode(AlwaysRelevantNode);

	GridNode = CreateNewNode<UReplicationGraphNode_GridSpatialization2D>();
	GridNode->CellSize = GridCellSize;
	// GridNode->SpatialBias
	AddGlobalGraphNode(GridNode);
}

void UMyReplicationGraph::InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager)
{
	Super::InitConnectionGraphNodes(ConnectionManager);

	if (UMyConnectionManager* MyConnectionManager = Cast<UMyConnectionManager>(ConnectionManager))
	{
		MyConnectionManager->AlwaysRelevantForConnectionNode = CreateNewNode<UMyReplicationGraphNode_AlwaysRelevant_ForConnection>();
		AddConnectionGraphNode(MyConnectionManager->AlwaysRelevantForConnectionNode, ConnectionManager);

		MyConnectionManager->AlwaysRelevantForTeamNode = CreateNewNode<UMyReplicationGraphNode_AlwaysRelevant_ForTeam>();
		AddConnectionGraphNode(MyConnectionManager->AlwaysRelevantForTeamNode, ConnectionManager);
	}
}

void UMyReplicationGraph::RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo)
{
	// The base implementation just routes to every global node. Subclasses will want a more direct routing function where possible.

	// // All clients must receive game states and player states
	// if (ActorInfo.Class->IsChildOf(AGameStateBase::StaticClass()) || ActorInfo.Class->IsChildOf(APlayerState::StaticClass())
	// 	// Loop over this list to make classes always relevant
	// 	|| AlwaysRelevantClasses.ContainsByPredicate([ActorInfo](UClass* Class) { return ActorInfo.Class->IsChildOf(Class); }))
	// {
	// 	AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
	// 	UE_LOG(LogReplicationGraph, Log, TEXT("[%s] routed to AlwaysRelevantNode"), *GetNameSafe(ActorInfo.Actor))
	// }
	// // If not we see if it belongs to a connection
	// else if (UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(ActorInfo.Actor))
	// {
	// 	if (ActorInfo.Actor->bOnlyRelevantToOwner)
	// 	{
	// 		ConnectionManager->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(ActorInfo);
	// 		UE_LOG(LogReplicationGraph, Log, TEXT("[%s] routed to AlwaysRelevantForConnectionNode"), *GetNameSafe(ActorInfo.Actor))
	// 	}
	// 	else
	// 	{
	// 		ConnectionManager->AlwaysRelevantForTeamNode->NotifyAddNetworkActor(ActorInfo);
	// 		UE_LOG(LogReplicationGraph, Log, TEXT("[%s] routed to AlwaysRelevantForTeamNode"), *GetNameSafe(ActorInfo.Actor))
	//
	// 		// Cache the connection’s pawn when it is added to the node so that we can access it quickly each time we need to raycast
	// 		if (APawn* Pawn = Cast<APawn>(ActorInfo.Actor))
	// 		{
	// 			ConnectionManager->Pawn = Pawn;
	// 		}
	// 	}
	// }
	// else if (ActorInfo.Actor->GetNetOwner())
	// {
	// 	// Add to PendingConnectionActors if the net connection is not ready yet
	// 	PendingConnectionActors.Add(ActorInfo.Actor);
	// }

	EClassRepNodeMapping Policy = GetMappingPolicy(ActorInfo.Class);

	switch (Policy)
	{
	case EClassRepNodeMapping::NotRouted:
		{
			break;
		}

	case EClassRepNodeMapping::RelevantAllConnections:
		{
			if (ActorInfo.StreamingLevelName == NAME_None)
			{
				AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
			}
			break;
		}

	case EClassRepNodeMapping::Spatialize_Static:
		{
			GridNode->AddActor_Static(ActorInfo, GlobalInfo);
			break;
		}

	case EClassRepNodeMapping::Spatialize_Dynamic:
		{
			GridNode->AddActor_Dynamic(ActorInfo, GlobalInfo);
			break;
		}

	case EClassRepNodeMapping::Spatialize_Dormancy:
		{
			GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);
			break;
		}

	case EClassRepNodeMapping::RelevantForConnection:
		{
			if (UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(ActorInfo.Actor))
			{
				ConnectionManager->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(ActorInfo);
			}
			else if (ActorInfo.Actor->GetNetOwner()) // Not a player controller
			{
				PendingConnectionActors.Add(ActorInfo.Actor);
			}
			break;
		}

	case EClassRepNodeMapping::RelevantForTeam:
		{
			if (UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(ActorInfo.Actor))
			{
				ConnectionManager->AlwaysRelevantForTeamNode->NotifyAddNetworkActor(ActorInfo);

				if (APawn* Pawn = Cast<APawn>(ActorInfo.Actor))
				{
					ConnectionManager->ControlledPawn = Pawn;
				}
			}
			else if (ActorInfo.Actor->GetNetOwner())
			{
				PendingConnectionActors.Add(ActorInfo.Actor);
			}
			break;
		}
	}
}

void UMyReplicationGraph::RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo)
{
	// if (ActorInfo.Class->IsChildOf(AGameStateBase::StaticClass()) || ActorInfo.Class->IsChildOf(APlayerState::StaticClass()))
	// {
	// 	AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
	// }
	// else if (const UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(ActorInfo.GetActor()))
	// {
	// 	if (ActorInfo.Actor->bOnlyRelevantToOwner)
	// 	{
	// 		ConnectionManager->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(ActorInfo);
	// 	}
	// 	else
	// 	{
	// 		ConnectionManager->AlwaysRelevantForTeamNode->NotifyRemoveNetworkActor(ActorInfo);
	// 	}
	// }
	// else if (ActorInfo.Actor->GetNetOwner())
	// {
	// 	PendingConnectionActors.Remove(ActorInfo.Actor);
	// }

	EClassRepNodeMapping Policy = GetMappingPolicy(ActorInfo.Class);

	switch (Policy)
	{
	case EClassRepNodeMapping::NotRouted:
		{
			break;
		}

	case EClassRepNodeMapping::RelevantAllConnections:
		{
			if (ActorInfo.StreamingLevelName == NAME_None)
			{
				AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
			}
			break;
		}

	case EClassRepNodeMapping::Spatialize_Static:
		{
			GridNode->RemoveActor_Static(ActorInfo);
			break;
		}

	case EClassRepNodeMapping::Spatialize_Dynamic:
		{
			GridNode->RemoveActor_Dynamic(ActorInfo);
			break;
		}

	case EClassRepNodeMapping::Spatialize_Dormancy:
		{
			GridNode->RemoveActor_Dormancy(ActorInfo);
			break;
		}
	case EClassRepNodeMapping::RelevantForConnection:
		{
			if (UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(ActorInfo.Actor))
			{
				ConnectionManager->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(ActorInfo);
			}
			else if (ActorInfo.Actor->GetNetOwner())
			{
				PendingConnectionActors.Remove(ActorInfo.Actor);
			}
			break;
		}

	case EClassRepNodeMapping::RelevantForTeam:
		{
			if (UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(ActorInfo.Actor))
			{
				ConnectionManager->AlwaysRelevantForTeamNode->NotifyAddNetworkActor(ActorInfo);
			}
			else if (ActorInfo.Actor->GetNetOwner())
			{
				PendingConnectionActors.Remove(ActorInfo.Actor);
			}
			break;
		}
	}
}

void UMyReplicationGraph::RemoveClientConnection(UNetConnection* NetConnection)
{
	Super::RemoveClientConnection(NetConnection);

	int32 ConnectionId = 0;
	bool bFound = false;

	auto UpdateList = [&](TArray<TObjectPtr<UNetReplicationGraphConnection>>& List)
	{
		for (int32 idx = 0; idx < List.Num(); ++idx)
		{
			UMyConnectionManager* ConnectionManager = Cast<UMyConnectionManager>(Connections[idx]);
			repCheck(ConnectionManager);

			if (ConnectionManager->NetConnection == NetConnection)
			{
				ensure(!bFound);

				// Remove the connection from the team node if the team is valid
				if (ConnectionManager->Team != TEAM_NONE)
				{
					TeamConnectionListMap.RemoveConnectionFromTeam(ConnectionManager->Team, ConnectionManager);
				}

				// Also remove it from the input list
				List.RemoveAtSwap(idx, 1, EAllowShrinking::No);
				bFound = true;
			}
			else
			{
				ConnectionManager->ConnectionOrderNum = ConnectionId;
				ConnectionId++;
			}
		}
	};

	UpdateList(Connections);
	UpdateList(PendingConnections);
}

void UMyReplicationGraph::ResetGameWorldState()
{
	PendingConnectionActors.Reset();
	PendingTeamRequests.Reset();

	auto EmptyConnectionNode = [](TArray<TObjectPtr<UNetReplicationGraphConnection>>& GraphConnections)
	{
		for (UNetReplicationGraphConnection* GraphConnection : GraphConnections)
		{
			if (UMyConnectionManager* ConnectionManager = Cast<UMyConnectionManager>(GraphConnection))
			{
				// Clear out all always relevant actors
				// Seamless travel means that the team connections will still be relevant due to the controllers not being destroyed
				ConnectionManager->AlwaysRelevantForConnectionNode->NotifyResetAllNetworkActors();
			}
		}
	};

	EmptyConnectionNode(PendingConnections);
	EmptyConnectionNode(Connections);
}

void UMyReplicationGraph::SetTeamForPlayerController(APlayerController* PlayerController, int32 Team)
{
	repCheck(PlayerController);

	if (UMyConnectionManager* ConnectionManager = GetConnectionManagerFromActor(PlayerController))
	{
		uint8 CurrentTeam = ConnectionManager->Team;

		if (CurrentTeam != Team)
		{
			// Remove the connection to the old team list
			if (CurrentTeam != TEAM_NONE)
			{
				TeamConnectionListMap.RemoveConnectionFromTeam(CurrentTeam, ConnectionManager);
			}

			// Add the graph to the new team list
			if (Team != TEAM_NONE)
			{
				TeamConnectionListMap.AddConnectionToTeam(Team, ConnectionManager);
			}

			ConnectionManager->Team = Team;
		}
	}
	else
	{
		// Add to PendingTeamRequests if the net connection is not ready yet
		PendingTeamRequests.Emplace(PlayerController, Team);
	}
}

UMyConnectionManager* UMyReplicationGraph::GetConnectionManagerFromActor(const AActor* Actor)
{
	repCheck(Actor)

	if (UNetConnection* NetConnection = Actor->GetNetConnection())
	{
		if (UMyConnectionManager* ConnectionManager = Cast<UMyConnectionManager>(FindOrAddConnectionManager(NetConnection)))
		{
			return ConnectionManager;
		}
	}

	return nullptr;
}

EClassRepNodeMapping UMyReplicationGraph::GetMappingPolicy(UClass* Class)
{
	EClassRepNodeMapping* PolicyPtr = ClassRepNodePolicies.Get(Class);
	EClassRepNodeMapping Policy = PolicyPtr ? *PolicyPtr : EClassRepNodeMapping::NotRouted;
	return Policy;
}

void UMyReplicationGraph::HandlePendingActorsAndTeamRequests()
{
	// Setup all pending team requests
	if (PendingTeamRequests.Num() > 0)
	{
		TMap<APlayerController*, int32> TempRequests = MoveTemp(PendingTeamRequests);

		for (TTuple<APlayerController*, int32>& Request : TempRequests)
		{
			if (IsValid(Request.Key))
			{
				SetTeamForPlayerController(Request.Key, Request.Value);
			}
		}
	}

	// Set up all pending connections
	if (PendingConnectionActors.Num() > 0)
	{
		TArray<AActor*> PendingActors = MoveTemp(PendingConnectionActors);

		for (AActor* Actor : PendingActors)
		{
			if (IsValid(Actor))
			{
				FGlobalActorReplicationInfo& GlobalInfo = GlobalActorReplicationInfoMap.Get(Actor);
				RouteAddNetworkActorToNodes(FNewReplicatedActorInfo(Actor), GlobalInfo);
			}
		}
	}
}

// --------------------------------------------------------------------------------
// UReplicationGraphNodes
// --------------------------------------------------------------------------------

UMyReplicationGraphNode_AlwaysRelevant_WithPending::UMyReplicationGraphNode_AlwaysRelevant_WithPending()
{
	// Call PrepareForReplication before replication once per frame
	bRequiresPrepareForReplicationCall = true;
}

void UMyReplicationGraphNode_AlwaysRelevant_WithPending::PrepareForReplication()
{
	// Limit frequency
	if (bShouldHandlePendingRequest)
	{
		UMyReplicationGraph* ReplicationGraph = Cast<UMyReplicationGraph>(GetOuter());
		ReplicationGraph->HandlePendingActorsAndTeamRequests();

		bShouldHandlePendingRequest = false;

		UE_LOG(LogReplicationGraph,Log,TEXT("AlwaysRelevant_WithPending handled pending requests."))
	}
}

void UMyReplicationGraphNode_AlwaysRelevant_WithPending::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	Super::GatherActorListsForConnection(Params);

	if (Params.ReplicationFrameNum % HandlePendingRequestPeriodFrame == 0)
	{
		bShouldHandlePendingRequest = true; // Will be handled next rep frame
	}
}

void UMyReplicationGraphNode_AlwaysRelevant_ForConnection::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	// Default implementation just adds PlayerController (InViewer) and Pawn (ViewTarget) to Params' RepList
	Super::GatherActorListsForConnection(Params);

	for (const FNetViewer& CurViewer : Params.Viewers)
	{
		if (APlayerController* PC = Cast<APlayerController>(CurViewer.InViewer))
		{
			// // 50% throttling of PlayerStates.
			// const bool bReplicatePS = (Params.ConnectionManager.ConnectionOrderNum % 2) == (Params.ReplicationFrameNum % 2);
			// if (bReplicatePS)
			// {
			// 	// Always return the player state to the owning player. Simulated proxy player states are handled by UShooterReplicationGraphNode_PlayerStateFrequencyLimiter
			// 	if (APlayerState* PS = PC->PlayerState)
			// 	{
			// 		if (!bInitializedPlayerState)
			// 		{
			// 			bInitializedPlayerState = true;
			// 			FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(PS);
			// 			ConnectionActorInfo.ReplicationPeriodFrame = 1;
			// 		}
			//
			// 		ReplicationActorList.ConditionalAdd(PS);
			// 	}
			// }

			// FAlwaysRelevantActorInfo* LastData = PastRelevantActors.FindByKey<UNetConnection*>(CurViewer.Connection);
			FCachedAlwaysRelevantActorInfo* LastData = PastRelevantActorMap.Find(CurViewer.Connection);

			// We've not seen this actor before, go ahead and add them.
			if (LastData == nullptr)
			{
				FCachedAlwaysRelevantActorInfo NewActorInfo;
				LastData = &PastRelevantActorMap.Add(CurViewer.Connection, NewActorInfo);
			}

			check(LastData != nullptr);

			if (ACharacter* Pawn = Cast<ACharacter>(PC->GetPawn()))
			{
				// ResetActorCullDistance
				if (LastData->LastViewer != Pawn)
				{
					LastData->LastViewer = Pawn;

					UE_LOG(LogReplicationGraph, Verbose, TEXT("Setting pawn cull distance to 0. %s"), *Pawn->GetName());
					FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(Pawn);
					ConnectionActorInfo.SetCullDistanceSquared(0.f);
				}

				if (Pawn != CurViewer.ViewTarget)
				{
					ReplicationActorList.ConditionalAdd(Pawn);
				}

				// int32 InventoryCount = Pawn->GetInventoryCount();
				// for (int32 i = 0; i < InventoryCount; ++i)
				// {
				// 	AShooterWeapon* Weapon = Pawn->GetInventoryWeapon(i);
				// 	if (Weapon)
				// 	{
				// 		ReplicationActorList.ConditionalAdd(Weapon);
				// 	}
				// }
			}

			if (ACharacter* ViewTargetPawn = Cast<ACharacter>(CurViewer.ViewTarget))
			{
				// ResetActorCullDistance
				if (LastData->LastViewer != ViewTargetPawn)
				{
					LastData->LastViewer = ViewTargetPawn;

					UE_LOG(LogReplicationGraph, Verbose, TEXT("Setting pawn cull distance to 0. %s"), *ViewTargetPawn->GetName());
					FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(ViewTargetPawn);
					ConnectionActorInfo.SetCullDistanceSquared(0.f);
				}
			}
		}
	}

	// Remove actor if its connection is null
	for (auto It = PastRelevantActorMap.CreateIterator(); It; ++It)
	{
		if (It.Key().ResolveObjectPtr() == nullptr)
		{
			It.RemoveCurrent();
		}
	}

	Params.OutGatheredReplicationLists.AddReplicationActorList(ReplicationActorList);
}

void UMyReplicationGraphNode_AlwaysRelevant_ForTeam::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	// Get all other team members with the same team ID
	UMyReplicationGraph* ReplicationGraph = Cast<UMyReplicationGraph>(GetOuter());
	UMyConnectionManager* ConnectionManager = Cast<UMyConnectionManager>(&Params.ConnectionManager);

	if (ReplicationGraph && ConnectionManager && ConnectionManager->Team != TEAM_NONE)
	{
		// Add all team members to the list
		if (TArray<UMyConnectionManager*>* TeamConnections = ReplicationGraph->TeamConnectionListMap.GetConnectionArrayForTeam(ConnectionManager->Team))
		{
			for (UMyConnectionManager* TeamMember : *TeamConnections)
			{
				if (TeamMember == ConnectionManager)
				{
					continue;
				}

				// TeamMember->AlwaysRelevantForTeamNode->GatherActorListsForConnectionDefault(Params);
				Params.OutGatheredReplicationLists.AddReplicationActorList(TeamMember->AlwaysRelevantForTeamNode->ReplicationActorList);
				// TODO: Handling streaming level actors
			}

			// UE_LOG(LogReplicationGraph, Log, TEXT("GatherActorListsForConnection: [%s] [%d]"), *GetNameSafe(ConnectionManager->ControlledPawn.Get()), ConnectionManager->Team)
		}

		// TODO limit the frequency

		// Add all visible non-team actors to the list
		// TArray<UMyConnectionManager*>& NonTeamConnections = ReplicationGraph->TeamConnectionListMap.GetVisibleConnectionArrayForNonTeam(ConnectionManager->Pawn.Get(), ConnectionManager->Team);
		// for (const UMyConnectionManager* NonTeamMember : NonTeamConnections)
		// {
		// 	NonTeamMember->AlwaysRelevantForTeamNode->GatherActorListsForConnectionDefault(Params);
		// }
	}
	else
	{
		Super::GatherActorListsForConnection(Params);
	}
}

void UMyReplicationGraphNode_AlwaysRelevant_ForTeam::GatherActorListsForConnectionDefault(const FConnectionGatherActorListParameters& Params)
{
	// Handling replication actor list
	Super::GatherActorListsForConnection(Params);
}
