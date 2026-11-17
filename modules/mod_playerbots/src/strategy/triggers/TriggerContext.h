/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_TRIGGERCONTEXT_H
#define _PLAYERBOT_TRIGGERCONTEXT_H


#include "NamedObjectContext.h"
#include "GenericTriggers.h"
#include "HealthTriggers.h"
#include "RangeTriggers.h"

class PlayerbotAI;
class TriggerContext : public NamedObjectContext<Trigger>
{
public:
    TriggerContext()
    {
        creators["timer"] = &TriggerContext::Timer;
        creators["random"] = &TriggerContext::Random;
        creators["seldom"] = &TriggerContext::seldom;
        creators["often"] = &TriggerContext::often;
        creators["always trigger"] = &TriggerContext::always;
        creators["random bot update"] = &TriggerContext::random_bot_update_trigger;

        creators["target critical health"] = &TriggerContext::TargetCriticalHealth;

        creators["critical health"] = &TriggerContext::CriticalHealth;
        creators["low health"] = &TriggerContext::LowHealth;
        creators["medium health"] = &TriggerContext::MediumHealth;
        creators["almost full health"] = &TriggerContext::AlmostFullHealth;

        creators["tank assist"] = &TriggerContext::TankAssist;
        creators["lose aggro"] = &TriggerContext::LoseAggro;
        creators["has aggro"] = &TriggerContext::HasAggro;
        creators["medium threat"] = &TriggerContext::MediumThreat;
        creators["low tank threat"] = &TriggerContext::low_tank_threat;

        creators["low mana"] = &TriggerContext::LowMana;
        creators["medium mana"] = &TriggerContext::MediumMana;
        creators["high mana"] = &TriggerContext::HighMana;
        creators["almost full mana"] = &TriggerContext::AlmostFullMana;
        creators["enough mana"] = &TriggerContext::EnoughMana;

        creators["light rage available"] = &TriggerContext::LightRageAvailable;
        creators["medium rage available"] = &TriggerContext::MediumRageAvailable;
        creators["high rage available"] = &TriggerContext::HighRageAvailable;

        creators["light energy available"] = &TriggerContext::LightEnergyAvailable;
        creators["medium energy available"] = &TriggerContext::MediumEnergyAvailable;
        creators["high energy available"] = &TriggerContext::HighEnergyAvailable;
        creators["almost full energy available"] = &TriggerContext::AlmostFullEnergyAvailable;

        creators["party member critical health"] = &TriggerContext::PartyMemberCriticalHealth;
        creators["party member low health"] = &TriggerContext::PartyMemberLowHealth;
        creators["party member medium health"] = &TriggerContext::PartyMemberMediumHealth;
        creators["party member almost full health"] = &TriggerContext::PartyMemberAlmostFullHealth;

        creators["dead"] = &TriggerContext::Dead;
        creators["corpse near"] = &TriggerContext::corpse_near;

        creators["light aoe"] = &TriggerContext::LightAoe;
        creators["medium aoe"] = &TriggerContext::MediumAoe;
        creators["high aoe"] = &TriggerContext::HighAoe;

        creators["panic"] = &TriggerContext::panic;
        creators["outnumbered"] = &TriggerContext::outnumbered;
        creators["behind target"] = &TriggerContext::behind_target;
        creators["not behind target"] = &TriggerContext::not_behind_target;
        creators["not facing target"] = &TriggerContext::not_facing_target;

        creators["enemy out of melee"] = &TriggerContext::EnemyOutOfMelee;
        creators["enemy out of spell"] = &TriggerContext::EnemyOutOfSpell;
        creators["enemy too close for spell"] = &TriggerContext::enemy_too_close_for_spell;
        creators["enemy too close for shoot"] = &TriggerContext::enemy_too_close_for_shoot;
        creators["enemy too close for auto shot"] = &TriggerContext::enemy_too_close_for_auto_shot;
        creators["enemy too close for melee"] = &TriggerContext::enemy_too_close_for_melee;
        creators["enemy is close"] = &TriggerContext::enemy_is_close;
        creators["enemy within melee"] = &TriggerContext::enemy_within_melee;
        creators["not dps target active"] = &TriggerContext::not_dps_target_active;
        creators["not dps aoe target active"] = &TriggerContext::not_dps_aoe_target_active;

        creators["party member to heal out of spell range"] = &TriggerContext::party_member_to_heal_out_of_spell_range;
        creators["no target"] = &TriggerContext::NoTarget;
        creators["target changed"] = &TriggerContext::target_changed;
        creators["invalid target"] = &TriggerContext::invalid_target;

        creators["no attackers"] = &TriggerContext::NoAttackers;
        creators["has attackers"] = &TriggerContext::has_attackers;
        creators["no possible targets"] = &TriggerContext::no_possible_targets;
        creators["possible adds"] = &TriggerContext::possible_adds;

        creators["no pet"] = &TriggerContext::no_pet;
        creators["has pet"] = &TriggerContext::has_pet;
        creators["pet attack"] = &TriggerContext::pet_attack;

        creators["protect party member"] = &TriggerContext::protect_party_member;
    }

private:
    static Trigger* Timer(PlayerbotAI* botAI) { return new TimerTrigger(botAI); }
    static Trigger* Random(PlayerbotAI* botAI) { return new RandomTrigger(botAI, "random", 20); }
    static Trigger* seldom(PlayerbotAI* botAI) { return new RandomTrigger(botAI, "seldom", 300); }
    static Trigger* often(PlayerbotAI* botAI) { return new RandomTrigger(botAI, "often", 5); }
    static Trigger* always(PlayerbotAI* botAI) { return new AlwaysTrigger(botAI); }
    static Trigger* random_bot_update_trigger(PlayerbotAI* botAI) { return new RandomBotUpdateTrigger(botAI); }

    static Trigger* LowHealth(PlayerbotAI* botAI) { return new LowHealthTrigger(botAI); }
    static Trigger* MediumHealth(PlayerbotAI* botAI) { return new MediumHealthTrigger(botAI); }
    static Trigger* AlmostFullHealth(PlayerbotAI* botAI) { return new AlmostFullHealthTrigger(botAI); }
    static Trigger* CriticalHealth(PlayerbotAI* botAI) { return new CriticalHealthTrigger(botAI); }
    static Trigger* TargetCriticalHealth(PlayerbotAI* botAI) { return new TargetCriticalHealthTrigger(botAI); }
    static Trigger* PartyMemberDead(PlayerbotAI* botAI) { return new PartyMemberDeadTrigger(botAI); }
    static Trigger* CombatPartyMemberDead(PlayerbotAI* botAI) { return new CombatPartyMemberDeadTrigger(botAI); }
    static Trigger* PartyMemberLowHealth(PlayerbotAI* botAI) { return new PartyMemberLowHealthTrigger(botAI); }
    static Trigger* PartyMemberMediumHealth(PlayerbotAI* botAI) { return new PartyMemberMediumHealthTrigger(botAI); }
    static Trigger* PartyMemberAlmostFullHealth(PlayerbotAI* botAI) { return new PartyMemberAlmostFullHealthTrigger(botAI); }
    static Trigger* LightRageAvailable(PlayerbotAI* botAI) { return new LightRageAvailableTrigger(botAI); }
    static Trigger* MediumRageAvailable(PlayerbotAI* botAI) { return new MediumRageAvailableTrigger(botAI); }
    static Trigger* HighRageAvailable(PlayerbotAI* botAI) { return new HighRageAvailableTrigger(botAI); }
    static Trigger* LightEnergyAvailable(PlayerbotAI* botAI) { return new LightEnergyAvailableTrigger(botAI); }
    static Trigger* MediumEnergyAvailable(PlayerbotAI* botAI) { return new MediumEnergyAvailableTrigger(botAI); }
    static Trigger* HighEnergyAvailable(PlayerbotAI* botAI) { return new HighEnergyAvailableTrigger(botAI); }
    static Trigger* AlmostFullEnergyAvailable(PlayerbotAI* botAI) { return new EnergyAvailable(botAI, 90); }
    static Trigger* generic_boost(PlayerbotAI* botAI) { return new GenericBoostTrigger(botAI); }
    static Trigger* PartyMemberCriticalHealth(PlayerbotAI* botAI) { return new PartyMemberCriticalHealthTrigger(botAI); }
    static Trigger* LowMana(PlayerbotAI* botAI) { return new LowManaTrigger(botAI); }
    static Trigger* MediumMana(PlayerbotAI* botAI) { return new MediumManaTrigger(botAI); }
    static Trigger* HighMana(PlayerbotAI* botAI) { return new HighManaTrigger(botAI); }
    static Trigger* AlmostFullMana(PlayerbotAI* botAI) { return new AlmostFullManaTrigger(botAI); }
    static Trigger* EnoughMana(PlayerbotAI* botAI) { return new EnoughManaTrigger(botAI); }

    static Trigger* TankAssist(PlayerbotAI* botAI) { return new TankAssistTrigger(botAI); }
    static Trigger* LoseAggro(PlayerbotAI* botAI) { return new LoseAggroTrigger(botAI); }
    static Trigger* HasAggro(PlayerbotAI* botAI) { return new HasAggroTrigger(botAI); }
    static Trigger* MediumThreat(PlayerbotAI* botAI) { return new MediumThreatTrigger(botAI); }
    static Trigger* low_tank_threat(PlayerbotAI* botAI) { return new LowTankThreatTrigger(botAI); }

    static Trigger* Dead(PlayerbotAI* botAI) { return new DeadTrigger(botAI); }
    static Trigger* corpse_near(PlayerbotAI* botAI) { return new CorpseNearTrigger(botAI); }

    static Trigger* LightAoe(PlayerbotAI* botAI) { return new LightAoeTrigger(botAI); }
    static Trigger* MediumAoe(PlayerbotAI* botAI) { return new MediumAoeTrigger(botAI); }
    static Trigger* HighAoe(PlayerbotAI* botAI) { return new HighAoeTrigger(botAI); }

    static Trigger* panic(PlayerbotAI* botAI) { return new PanicTrigger(botAI); }
    static Trigger* outnumbered(PlayerbotAI* botAI) { return new OutNumberedTrigger(botAI); }

    static Trigger* behind_target(PlayerbotAI* botAI) { return new IsBehindTargetTrigger(botAI); }
    static Trigger* not_behind_target(PlayerbotAI* botAI) { return new IsNotBehindTargetTrigger(botAI); }
    static Trigger* not_facing_target(PlayerbotAI* botAI) { return new IsNotFacingTargetTrigger(botAI); }
    static Trigger* EnemyOutOfMelee(PlayerbotAI* botAI) { return new EnemyOutOfMeleeTrigger(botAI); }
    static Trigger* EnemyOutOfSpell(PlayerbotAI* botAI) { return new EnemyOutOfSpellRangeTrigger(botAI); }
    static Trigger* enemy_too_close_for_spell(PlayerbotAI* botAI) { return new EnemyTooCloseForSpellTrigger(botAI); }
    static Trigger* enemy_too_close_for_auto_shot(PlayerbotAI* botAI) { return new EnemyTooCloseForAutoShotTrigger(botAI); }
    static Trigger* enemy_too_close_for_shoot(PlayerbotAI* botAI) { return new EnemyTooCloseForShootTrigger(botAI); }
    static Trigger* enemy_too_close_for_melee(PlayerbotAI* botAI) { return new EnemyTooCloseForMeleeTrigger(botAI); }
    static Trigger* enemy_is_close(PlayerbotAI* botAI) { return new EnemyIsCloseTrigger(botAI); }
    static Trigger* enemy_within_melee(PlayerbotAI* botAI) { return new EnemyWithinMeleeTrigger(botAI); }
    static Trigger* not_dps_target_active(PlayerbotAI* botAI) { return new NotDpsTargetActiveTrigger(botAI); }
    static Trigger* not_dps_aoe_target_active(PlayerbotAI* botAI) { return new NotDpsAoeTargetActiveTrigger(botAI); }

    static Trigger* party_member_to_heal_out_of_spell_range(PlayerbotAI* botAI) { return new PartyMemberToHealOutOfSpellRangeTrigger(botAI); }
    static Trigger* target_changed(PlayerbotAI* botAI) { return new TargetChangedTrigger(botAI); }
    static Trigger* NoTarget(PlayerbotAI* botAI) { return new NoTargetTrigger(botAI); }
    static Trigger* invalid_target(PlayerbotAI* botAI) { return new InvalidTargetTrigger(botAI); }

    static Trigger* NoAttackers(PlayerbotAI* botAI) { return new NoAttackersTrigger(botAI); }
    static Trigger* has_attackers(PlayerbotAI* botAI) { return new HasAttackersTrigger(botAI); }
    static Trigger* no_possible_targets(PlayerbotAI* botAI) { return new NoPossibleTargetsTrigger(botAI); }
    static Trigger* possible_adds(PlayerbotAI* botAI) { return new PossibleAddsTrigger(botAI); }

    static Trigger* no_pet(PlayerbotAI* botAI) { return new NoPetTrigger(botAI); }
    static Trigger* has_pet(PlayerbotAI* botAI) { return new HasPetTrigger(botAI); }
    static Trigger* pet_attack(PlayerbotAI* botAI) { return new PetAttackTrigger(botAI); }

    static Trigger* protect_party_member(PlayerbotAI* botAI) { return new ProtectPartyMemberTrigger(botAI); }
};

#endif
