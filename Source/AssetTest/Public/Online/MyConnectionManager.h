#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "MyConnectionManager.generated.h"

#define TEAM_NONE -1

class UMyReplicationGraphNode_AlwaysRelevant_ForConnection;
class UMyReplicationGraphNode_AlwaysRelevant_ForTeam;

/**
 * Stores team and pawn additionally
 */
UCLASS()
class ASSETTEST_API UMyConnectionManager : public UNetReplicationGraphConnection
{
	GENERATED_BODY()

public:
	// UReplicationGraphNode_AlwaysRelevant_ForConnection is a node type provided by Epic for actors always relevant to a connection
	UPROPERTY()
	TObjectPtr<UMyReplicationGraphNode_AlwaysRelevant_ForConnection> AlwaysRelevantForConnectionNode;

	UPROPERTY()
	TObjectPtr<UMyReplicationGraphNode_AlwaysRelevant_ForTeam> AlwaysRelevantForTeamNode;

	uint8 Team = TEAM_NONE;
	
	TWeakObjectPtr<APawn> ControlledPawn;
};
