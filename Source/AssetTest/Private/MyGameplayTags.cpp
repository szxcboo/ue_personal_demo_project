#include "MyGameplayTags.h"

namespace MyGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(None, "MyGameplayTags.None")

	namespace Messages
	{
		UE_DEFINE_GAMEPLAY_TAG(InventoryStackChanged, "Messages.InventoryStackChanged")
	}

	namespace CharacterType
	{
		UE_DEFINE_GAMEPLAY_TAG(Player, "MyGameplayTags.CharacterType.Player")
		UE_DEFINE_GAMEPLAY_TAG(Enemy, "MyGameplayTags.CharacterType.Enemy")
	}

	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG(Projectile, "MyGameplayTags.SetByCaller.Projectile")
	}

	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG(ActivateOnGiven, "MyGameplayTags.Abilities.ActivateOnGiven")
		UE_DEFINE_GAMEPLAY_TAG(Death, "MyGameplayTags.Abilities.Death")

		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG(Normal, "MyGameplayTags.Abilities.Player.Normal")
			UE_DEFINE_GAMEPLAY_TAG(Primary, "MyGameplayTags.Abilities.Player.Primary")
			UE_DEFINE_GAMEPLAY_TAG(Secondary, "MyGameplayTags.Abilities.Player.Secondary")
			UE_DEFINE_GAMEPLAY_TAG(Tertiary, "MyGameplayTags.Abilities.Player.Tertiary")
		}

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG(Attack, "MyGameplayTags.Abilities.Enemy.Attack")
			UE_DEFINE_GAMEPLAY_TAG(HitReact, "MyGameplayTags.Abilities.Enemy.HitReact")
		}
	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG(KillScored, "MyGameplayTags.Events.KillScored")
		UE_DEFINE_GAMEPLAY_TAG(HitReact, "MyGameplayTags.Events.HitReact")

		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG(Normal, "MyGameplayTags.Events.Player.Normal")
			UE_DEFINE_GAMEPLAY_TAG(Primary, "MyGameplayTags.Events.Player.Primary")
			UE_DEFINE_GAMEPLAY_TAG(HitReact, "MyGameplayTags.Events.Player.HitReact")
			UE_DEFINE_GAMEPLAY_TAG(Death, "MyGameplayTags.Events.Player.Death")
		}

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG(HitReact, "MyGameplayTags.Events.Enemy.HitReact")
			UE_DEFINE_GAMEPLAY_TAG(EndAttack, "MyGameplayTags.Events.Enemy.EndAttack")
			UE_DEFINE_GAMEPLAY_TAG(MeleeTraceHit, "MyGameplayTags.Events.Enemy.TraceHit")
		}

		namespace Combo
		{
			UE_DEFINE_GAMEPLAY_TAG(StartInputCache, "MyGameplayTags.Events.Combo.StartInputCache")
			UE_DEFINE_GAMEPLAY_TAG(EndInputCache, "MyGameplayTags.Events.Combo.EndInputCache")

			UE_DEFINE_GAMEPLAY_TAG(WindowOpen, "MyGameplayTags.Events.Combo.WindowOpen")
			UE_DEFINE_GAMEPLAY_TAG(WindowClose, "MyGameplayTags.Events.Combo.WindowClose")
		}
	}

	namespace Status
	{
		UE_DEFINE_GAMEPLAY_TAG(Dead, "MyGameplayTags.Status.Dead")
	}
}
