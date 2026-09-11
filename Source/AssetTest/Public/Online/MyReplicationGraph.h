#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "MyReplicationGraph.generated.h"

/**
 * Function Usages
 * 
 * - InitGlobalActorClassSettings / InitGlobalGraphNodes
 * UReplicationGraph::InitForNetDriver
 * UNetDriver::SetReplicationDriver
 * UNetDriver::InitConnect (Initialize the net driver in client mode) / InitListen (Initialize the network driver in server mode (listener))
 * Impl by UIPNetDriver
 * UPendingNetGame::InitNetDriver / UWorld::Listen
 * UGameInstance::StartGameInstance / UGameInstance::EnableListenServer / ...
 *
 * - InitConnectionGraphNodes
 * UReplicationGraph::CreateClientConnectionManagerInternal
 * UReplicationGraph::AddClientConnection
 * UReplicationGraph::InitForNetDriver
 *
 * - RouteAddNetworkActorToNodes / RouteRemoveNetworkActorToNodes
 * UReplicationGraph::AddNetworkActor
 * UNetDriver::AddNetworkActor
 * UWorld::AddNetworkActor
 *
 * - RemoveClientConnection
 * UNetDriver::RemoveClientConnection
 * UNetConnection::CleanUp
 *
 * - ResetGameWorldState
 * UNetDriver::ResetGameWorldState
 */

class UMyConnectionManager;
class UMyReplicationGraphNode_AlwaysRelevant_WithPending;
class UMyReplicationGraphNode_AlwaysRelevant_ForTeam;

// This is the main enum we use to route actors to the right replication node. Each class maps to one enum.
UENUM()
enum class EClassRepNodeMapping : uint8
{
	NotRouted,
	RelevantAllConnections,
	Spatialize_Static,
	Spatialize_Dynamic,
	Spatialize_Dormancy,
	RelevantForConnection,
	RelevantForTeam
};

struct FTeamConnectionListMap : TMap<uint8, TArray<UMyConnectionManager*>>
{
	TArray<UMyConnectionManager*>* GetConnectionArrayForTeam(uint8 Team);
	void AddConnectionToTeam(uint8 Team, UMyConnectionManager* ConnectionManager);
	void RemoveConnectionFromTeam(uint8 Team, UMyConnectionManager* ConnectionManager);
	TArray<UMyConnectionManager*> GetVisibleConnectionArrayForNonTeam(APawn* Pawn, uint8 Team);
};

/**
 * 
 */
UCLASS()
class ASSETTEST_API UMyReplicationGraph : public UReplicationGraph
{
	GENERATED_BODY()

public:
	UMyReplicationGraph();

	/** Initialize the per-class data for replication */
	virtual void InitGlobalActorClassSettings() override;

	/** Init/configure your project's Global Graph */
	virtual void InitGlobalGraphNodes() override;

	/** Init/configure graph for a specific connection. Note they do not all have to be unique: connections can share nodes (e.g, 2 nodes for 2 teams) */
	virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager) override;

	/** Route actor spawning/despawning to the right node. (Or your nodes can gather the actors themselves) */
	virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo) override;
	virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;

	virtual void RemoveClientConnection(UNetConnection* NetConnection) override;
	virtual void ResetGameWorldState() override;

	void SetTeamForPlayerController(APlayerController* PlayerController, int32 Team);
	void HandlePendingActorsAndTeamRequests();

	UPROPERTY()
	TObjectPtr<UMyReplicationGraphNode_AlwaysRelevant_WithPending> AlwaysRelevantNode;

	UPROPERTY()
	TObjectPtr<UReplicationGraphNode_GridSpatialization2D> GridNode;

	FTeamConnectionListMap TeamConnectionListMap;

	// TMap<int32, TArray<UNetReplicationGraphConnection*>> TeamConnectionsMap;

protected:
	UMyConnectionManager* GetConnectionManagerFromActor(const AActor* Actor);
	EClassRepNodeMapping GetMappingPolicy(UClass* Class);

	TMap<APlayerController*, int32> PendingTeamRequests;
	TArray<AActor*> PendingConnectionActors;

	TClassMap<EClassRepNodeMapping> ClassRepNodePolicies;
};

UCLASS()
class UMyReplicationGraphNode_AlwaysRelevant_WithPending : public UReplicationGraphNode_ActorList // AlwaysRelevant node has a UReplicationGraphNode_ActorList as child node but we need to override RouteAdd/RemoveNotify... for it
{
	GENERATED_BODY()

public:
	UMyReplicationGraphNode_AlwaysRelevant_WithPending();
	
	/** Called once per frame prior to replication ONLY on root nodes */
	virtual void PrepareForReplication() override;
	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;

	// Explicit list of classes that are always relevant.
	TArray<UClass*> AlwaysRelevantClasses;

	uint32 HandlePendingRequestPeriodFrame = 8;

protected:
	bool bShouldHandlePendingRequest = false;
};

UCLASS()
class UMyReplicationGraphNode_AlwaysRelevant_ForConnection : public UReplicationGraphNode_AlwaysRelevant_ForConnection
{
	GENERATED_BODY()

public:
	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;
};

UCLASS()
class UMyReplicationGraphNode_AlwaysRelevant_ForTeam : public UReplicationGraphNode_ActorList
{
	GENERATED_BODY()

public:
	/**
	 * Will be called every tick to provide a list of nodes with the same team index as itself and will therefore be relevant
	 * Called by UReplicationGraph::ServerReplicateActors
	 */
	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;
	void GatherActorListsForConnectionDefault(const FConnectionGatherActorListParameters& Params);
};
