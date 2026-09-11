#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace MyGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(None)

	namespace Messages
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(InventoryStackChanged)
	}

	namespace CharacterType
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy)
	}
	
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Projectile)
	}
	
	namespace Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death)

		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Normal)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary)
		}

		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attack)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)
		}
	}

	namespace Events
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(KillScored)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)

		namespace Player
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Normal)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Death)
		}

		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndAttack)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(TraceHit)
		}

		namespace Combo
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(StartInputCache)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(EndInputCache)
			
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(WindowOpen)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(WindowClose)
		}
	}

	namespace Status
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dead)
	}
}
