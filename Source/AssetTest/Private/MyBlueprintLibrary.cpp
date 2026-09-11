#include "MyBlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/MyCharacterBase.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystem/MyAttributeSet.h"
#include "MyGameplayTags.h"

TArray<AActor*> UMyBlueprintLibrary::HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset, float HitBoxElevationOffset, bool bDrawDebugs)
{
	if (!IsValid(AvatarActor)) return TArray<AActor*>();

	// Ensure that the overlap test ignores the avatar actor
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	// Ignore all channels except for Pawn
	// This is an optimization that can't do in blueprint. In blueprint the overlap test would be set to block all by default
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	// Make a vector for the location
	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector Elevation = FVector(0.f, 0.f, HitBoxElevationOffset);
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + Elevation;

	TArray<FOverlapResult> OverlapResults;

	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return TArray<AActor*>();
	World->OverlapMultiByChannel(
		OverlapResults,
		HitBoxLocation,
		FQuat::Identity, // It doesn't matter when the hit box is sphere
		ECC_Visibility,
		Sphere,
		QueryParams,
		ResponseParams // Optional
	);

	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		AMyCharacterBase* BaseCharacter = Cast<AMyCharacterBase>(Result.GetActor());
		if (!IsValid(BaseCharacter)) continue;
		// if (!BaseCharacter->IsAlive()) continue;

		ActorsHit.AddUnique(Result.GetActor());
	}

	// Send a Gameplay event
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;

		FGameplayEventData Payload;
		Payload.Instigator = AvatarActor;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Result.GetActor(), MyGameplayTags::Events::Enemy::HitReact, Payload);
	}

	// Draw debugs
	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(AvatarActor, OverlapResults, HitBoxLocation, HitBoxRadius);
	}

	return ActorsHit;
}

void UMyBlueprintLibrary::DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults, const FVector& HitBoxLocation, float HitBoxRadius)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return;
	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16, FColor::Red, false, 3.f);

	for (const FOverlapResult& Result : OverlapResults)
	{
		FVector DebugLocation = Result.GetActor()->GetActorLocation();
		DebugLocation.Z += 100.f;
		DrawDebugSphere(World, DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
	}
}