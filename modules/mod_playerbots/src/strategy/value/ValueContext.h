#ifndef _PLAYERBOT_VALUECONTEXT_H
#define _PLAYERBOT_VALUECONTEXT_H

#include "ActiveSpellValue.h"
#include "AttackersValue.h"
#include "AttackerCountValues.h"
#include "AttackerWithoutAuraTargetValue.h"
#include "CurrentTargetValue.h"
#include "DistanceValue.h"
#include "DpsTargetValue.h"
#include "EnemyPlayerValue.h"
#include "EstimatedLifetimeValue.h"
#include "Formations.h"
#include "GroupValues.h"
#include "GrindTargetValue.h"
#include "HasTotemValue.h"
#include "InvalidTargetValue.h"
#include "IsBehindValue.h"
#include "IsMovingValue.h"
#include "LastMovementValue.h"
#include "LastSpellCastValue.h"
#include "LastSpellCastTimeValue.h"
#include "MaintenanceValues.h"
#include "ManaSaveLevelValue.h"
#include "MasterTargetValue.h"
#include "NearestUnitsValue.h"
#include "NearestFriendlyPlayersValue.h"
#include "PartyMemberWithoutAuraValue.h"
#include "PartyMemberToDispelValue.h"
#include "PartyMemberToHealValue.h"
#include "PartyMemberToResurectValue.h"
#include "PetTargetValue.h"
#include "PositionValue.h"
#include "StatsValues.h"
#include "SelfTargetValue.h"
#include "SpellCastUsefulValue.h"
#include "SpellIdValue.h"
#include "TankTargetValue.h"
#include "ThreatValues.h"
#include "PossibleTargetsValue.h"
#include "RandomBotUpdateValue.h"
#include "RangeValues.h"
#include "RtiValue.h"
#include "RtiTargetValue.h"

class PlayerbotAI;
class ValueContext : public NamedObjectContext<UntypedValue>
{
public:
    ValueContext()
    {
        creators["health"] = &ValueContext::health;
        creators["rage"] = &ValueContext::rage;
        creators["energy"] = &ValueContext::energy;
        creators["mana"] = &ValueContext::mana;
        creators["mana save level"] = &ValueContext::mana_save_level;
        creators["combo"] = &ValueContext::combo;
        creators["dead"] = &ValueContext::dead;
        creators["pet dead"] = &ValueContext::pet_dead;
        creators["has mana"] = &ValueContext::has_mana;
        creators["mounted"] = &ValueContext::mounted;
        creators["last movement"] = &ValueContext::last_movement;
        creators["stay time"] = &ValueContext::stay_time;
        creators["last taxi"] = &ValueContext::last_movement;
        creators["last area trigger"] = &ValueContext::last_movement;
        creators["combat"] = &ValueContext::combat;
        creators["bag space"] = &ValueContext::bag_space;
        creators["durability"] = &ValueContext::durability;
        creators["speed"] = &ValueContext::speed;
        creators["death count"] = &ValueContext::death_count;
        creators["experience"] = &ValueContext::experience;
        creators["disperse distance"] = &ValueContext::disperse_distance;
        creators["random bot update"] = &ValueContext::random_bot_update_value;
        creators["has totem"] = &ValueContext::has_totem;

        creators["distance"] = &ValueContext::distance;
        creators["moving"] = &ValueContext::moving;
        creators["swimming"] = &ValueContext::swimming;
        creators["behind"] = &ValueContext::behind;
        creators["facing"] = &ValueContext::facing;
        creators["balance"] = &ValueContext::balance;
        creators["attackers"] = &ValueContext::attackers;
        creators["attacker count"] = &ValueContext::attacker_count;
        creators["my attacker count"] = &ValueContext::my_attacker_count;
        creators["possible adds"] = &ValueContext::possible_adds;

        creators["rti target"] = &ValueContext::rti_target;
        creators["rti cc target"] = &ValueContext::rti_cc_target;
        creators["rti cc"] = &ValueContext::rti_cc;
        creators["rti"] = &ValueContext::rti;

        creators["prioritized targets"] = &ValueContext::prioritized_targets;
        creators["possible triggers"] = &ValueContext::possible_triggers;
        creators["possible targets"] = &ValueContext::possible_targets;
        creators["possible targets no los"] = &ValueContext::possible_targets_no_los;

        creators["position"] = &ValueContext::position;
        creators["current position"] = &ValueContext::current_position;
        creators["can move around"] = &ValueContext::can_move_around;

        creators["master target"] = &ValueContext::master;
        creators["all targets"] = &ValueContext::all_targets;
        creators["self target"] = &ValueContext::self_target;
        creators["pet target"] = &ValueContext::pet_target;
        creators["current target"] = &ValueContext::current_target;
        creators["old target"] = &ValueContext::old_target;
        creators["grind target"] = &ValueContext::grind_target;
        creators["pull target"] = &ValueContext::pull_target;
        creators["invalid target"] = &ValueContext::invalid_target;
        creators["nearest friendly players"] = &ValueContext::nearest_friendly_players;
        creators["closest friendly players"] = &ValueContext::closest_friendly_players;
        creators["nearest enemy players"] = &ValueContext::nearest_enemy_players;
        creators["enemy player target"] = &ValueContext::enemy_player_target;
        creators["dps target"] = &ValueContext::dps_target;
        creators["dps aoe target"] = &ValueContext::dps_aoe_target;

        creators["can move around"] = &ValueContext::can_move_around;
        creators["should home bind"] = &ValueContext::should_home_bind;
        creators["should repair"] = &ValueContext::should_repair;
        creators["can repair"] = &ValueContext::can_repair;
        creators["should sell"] = &ValueContext::should_sell;
        creators["can sell"] = &ValueContext::can_sell;
        creators["can fight equal"] = &ValueContext::can_fight_equal;
        creators["can fight elite"] = &ValueContext::can_fight_elite;
        creators["can fight boss"] = &ValueContext::can_fight_boss;

        creators["formation"] = &ValueContext::formation;
        creators["range"] = &ValueContext::range;
        creators["group"] = &ValueContext::group;
        creators["group members"] = &ValueContext::group_members;
        creators["following party"] = &ValueContext::following_party;
        creators["near leader"] = &ValueContext::near_leader;
        creators["and"] = &ValueContext::and_value;
        creators["group count"] = &ValueContext::group_count;
        creators["group and"] = &ValueContext::group_and;
        creators["group or"] = &ValueContext::group_or;
        creators["group ready"] = &ValueContext::group_ready;

        creators["active spell"] = &ValueContext::active_spell;
        creators["spell id"] = &ValueContext::spell_id;
        creators["vehicle spell id"] = &ValueContext::vehicle_spell_id;
        //creators["item for spell"] = &ValueContext::item_for_spell;
        creators["spell cast useful"] = &ValueContext::spell_cast_useful;
        creators["last spell cast"] = &ValueContext::last_spell_cast;
        creators["last spell cast time"] = &ValueContext::last_spell_cast_time;

        creators["estimated lifetime"] = &ValueContext::expected_lifetime;
        creators["estimated group dps"] = &ValueContext::expected_group_dps;

        creators["attacker without aura"] = &ValueContext::attacker_without_aura;
        creators["melee attacker without aura"] = &ValueContext::melee_attacker_without_aura;
        creators["party member without aura"] = &ValueContext::party_member_without_aura;
        creators["party member to heal"] = &ValueContext::party_member_to_heal;
        creators["party member to resurrect"] = &ValueContext::party_member_to_resurrect;
        creators["party member to dispel"] = &ValueContext::party_member_to_dispel;
        creators["party member to protect"] = &ValueContext::party_member_to_protect;

        creators["main tank"] = &ValueContext::main_tank;
        creators["threat"] = &ValueContext::threat;
        creators["has aggro"] = &ValueContext::has_aggro;
        creators["tank target"] = &ValueContext::tank_target;

        creators["last flee angle"] = &ValueContext::last_flee_angle;
        creators["last flee timestamp"] = &ValueContext::last_flee_timestamp;
        creators["recently flee info"] = &ValueContext::recently_flee_info;
    }

private:
    static UntypedValue* last_movement(PlayerbotAI* botAI) { return new LastMovementValue(botAI); }
    static UntypedValue* stay_time(PlayerbotAI* botAI) { return new StayTimeValue(botAI); }
    static UntypedValue* mounted(PlayerbotAI* botAI) { return new IsMountedValue(botAI); }
    static UntypedValue* health(PlayerbotAI* botAI) { return new HealthValue(botAI); }
    static UntypedValue* rage(PlayerbotAI* botAI) { return new RageValue(botAI); }
    static UntypedValue* energy(PlayerbotAI* botAI) { return new EnergyValue(botAI); }
    static UntypedValue* mana(PlayerbotAI* botAI) { return new ManaValue(botAI); }
    static UntypedValue* mana_save_level(PlayerbotAI* botAI) { return new ManaSaveLevelValue(botAI); }
    static UntypedValue* combo(PlayerbotAI* botAI) { return new ComboPointsValue(botAI); }
    static UntypedValue* dead(PlayerbotAI* botAI) { return new IsDeadValue(botAI); }
    static UntypedValue* pet_dead(PlayerbotAI* botAI) { return new PetIsDeadValue(botAI); }
    static UntypedValue* has_mana(PlayerbotAI* botAI) { return new HasManaValue(botAI); }   
    static UntypedValue* combat(PlayerbotAI* botAI) { return new IsInCombatValue(botAI); }
    static UntypedValue* bag_space(PlayerbotAI* botAI) { return new BagSpaceValue(botAI); }
    static UntypedValue* durability(PlayerbotAI* botAI) { return new DurabilityValue(botAI); }
    static UntypedValue* speed(PlayerbotAI* botAI) { return new SpeedValue(botAI); }
    static UntypedValue* death_count(PlayerbotAI* botAI) { return new DeathCountValue(botAI); }
    static UntypedValue* experience(PlayerbotAI* botAI) { return new ExperienceValue(botAI); }
    static UntypedValue* disperse_distance(PlayerbotAI* ai) { return new DisperseDistanceValue(ai); }
    static UntypedValue* random_bot_update_value(PlayerbotAI* botAI) { return new RandomBotUpdateValue(botAI); }
    static UntypedValue* has_totem(PlayerbotAI* botAI) { return new HasTotemValue(botAI); }

    static UntypedValue* position(PlayerbotAI* botAI) { return new PositionValue(botAI); }
    static UntypedValue* current_position(PlayerbotAI* botAI) { return new CurrentPositionValue(botAI); }
    static UntypedValue* behind(PlayerbotAI* botAI) { return new IsBehindValue(botAI); }
    static UntypedValue* facing(PlayerbotAI* botAI) { return new IsFacingValue(botAI); }
    static UntypedValue* moving(PlayerbotAI* botAI) { return new IsMovingValue(botAI); }
    static UntypedValue* swimming(PlayerbotAI* botAI) { return new IsSwimmingValue(botAI); }
    static UntypedValue* distance(PlayerbotAI* botAI) { return new DistanceValue(botAI); }
    static UntypedValue* rti_target(PlayerbotAI* botAI) { return new RtiTargetValue(botAI); }
    static UntypedValue* rti_cc_target(PlayerbotAI* botAI) { return new RtiCcTargetValue(botAI); }
    static UntypedValue* rti(PlayerbotAI* botAI) { return new RtiValue(botAI); }
    static UntypedValue* rti_cc(PlayerbotAI* botAI) { return new RtiCcValue(botAI); }

    static UntypedValue* balance(PlayerbotAI* botAI) { return new BalancePercentValue(botAI); }
    static UntypedValue* attackers(PlayerbotAI* botAI) { return new AttackersValue(botAI); }
    static UntypedValue* attacker_count(PlayerbotAI* botAI) { return new AttackerCountValue(botAI); }
    static UntypedValue* my_attacker_count(PlayerbotAI* botAI) { return new MyAttackerCountValue(botAI); }

    static UntypedValue* possible_adds(PlayerbotAI* botAI) { return new PossibleAddsValue(botAI); }
    static UntypedValue* prioritized_targets(PlayerbotAI* botAI) { return new PrioritizedTargetsValue(botAI); }

    static UntypedValue* master(PlayerbotAI* botAI) { return new MasterTargetValue(botAI); }
    static UntypedValue* all_targets(PlayerbotAI* botAI) { return new AllTargetsValue(botAI); }
    static UntypedValue* self_target(PlayerbotAI* botAI) { return new SelfTargetValue(botAI); }
    static UntypedValue* pet_target(PlayerbotAI* botAI) { return new PetTargetValue(botAI); }
    static UntypedValue* current_target(PlayerbotAI* botAI) { return new CurrentTargetValue(botAI); }
    static UntypedValue* possible_triggers(PlayerbotAI* botAI) { return new PossibleTriggersValue(botAI); }
    static UntypedValue* possible_targets(PlayerbotAI* botAI) { return new PossibleTargetsValue(botAI); }
    static UntypedValue* possible_targets_no_los(PlayerbotAI* botAI) { return new PossibleTargetsValue(botAI, "possible targets", sPlayerbotAIConfig->sightDistance, true); }
    static UntypedValue* old_target(PlayerbotAI* botAI) { return new CurrentTargetValue(botAI); }
    static UntypedValue* grind_target(PlayerbotAI* botAI) { return new GrindTargetValue(botAI); }
    static UntypedValue* pull_target(PlayerbotAI* botAI) { return new PullTargetValue(botAI); }
    static UntypedValue* invalid_target(PlayerbotAI* botAI) { return new InvalidTargetValue(botAI); }
    static UntypedValue* nearest_friendly_players(PlayerbotAI* botAI) { return new NearestFriendlyPlayersValue(botAI); }
    static UntypedValue* closest_friendly_players(PlayerbotAI* botAI) { return new NearestFriendlyPlayersValue(botAI, INTERACTION_DISTANCE); }
    static UntypedValue* nearest_enemy_players(PlayerbotAI* botAI) { return new NearestEnemyPlayersValue(botAI); }
    static UntypedValue* enemy_player_target(PlayerbotAI* botAI) { return new EnemyPlayerValue(botAI); }
    static UntypedValue* dps_target(PlayerbotAI* botAI) { return new DpsTargetValue(botAI); }
    static UntypedValue* dps_aoe_target(PlayerbotAI* botAI) { return new DpsAoeTargetValue(botAI); }

    static UntypedValue* can_move_around(PlayerbotAI* botAI) { return new CanMoveAroundValue(botAI); }
    static UntypedValue* should_home_bind(PlayerbotAI* botAI) { return new ShouldHomeBindValue(botAI); }
    static UntypedValue* should_repair(PlayerbotAI* botAI) { return new ShouldRepairValue(botAI); }
    static UntypedValue* can_repair(PlayerbotAI* botAI) { return new CanRepairValue(botAI); }
    static UntypedValue* should_sell(PlayerbotAI* botAI) { return new ShouldSellValue(botAI); }
    static UntypedValue* can_sell(PlayerbotAI* botAI) { return new CanSellValue(botAI); }
    static UntypedValue* can_fight_equal(PlayerbotAI* botAI) { return new CanFightEqualValue(botAI); }
    static UntypedValue* can_fight_elite(PlayerbotAI* botAI) { return new CanFightEliteValue(botAI); }
    static UntypedValue* can_fight_boss(PlayerbotAI* botAI) { return new CanFightBossValue(botAI); }

    static UntypedValue* formation(PlayerbotAI* botAI) { return new FormationValue(botAI); }
    static UntypedValue* range(PlayerbotAI* botAI) { return new RangeValue(botAI); }
    static UntypedValue* group_members(PlayerbotAI* botAI) { return new GroupMembersValue(botAI); }
    static UntypedValue* following_party(PlayerbotAI* botAI) { return new IsFollowingPartyValue(botAI); }
    static UntypedValue* near_leader(PlayerbotAI* botAI) { return new IsNearLeaderValue(botAI); }
    static UntypedValue* group(PlayerbotAI* botAI) { return new IsInGroupValue(botAI); }
    static UntypedValue* and_value(PlayerbotAI* botAI) { return new BoolANDValue(botAI); }
    static UntypedValue* group_count(PlayerbotAI* botAI) { return new GroupBoolCountValue(botAI); }
    static UntypedValue* group_and(PlayerbotAI* botAI) { return new GroupBoolANDValue(botAI); }
    static UntypedValue* group_or(PlayerbotAI* botAI) { return new GroupBoolORValue(botAI); }
    static UntypedValue* group_ready(PlayerbotAI* botAI) { return new GroupReadyValue(botAI); }

    static UntypedValue* active_spell(PlayerbotAI* botAI) { return new ActiveSpellValue(botAI); }
    static UntypedValue* spell_id(PlayerbotAI* botAI) { return new SpellIdValue(botAI); }
    static UntypedValue* vehicle_spell_id(PlayerbotAI* botAI) { return new VehicleSpellIdValue(botAI); }
    //static UntypedValue* inventory_item(PlayerbotAI* botAI) { return new InventoryItemValue(botAI); }
    static UntypedValue* last_spell_cast(PlayerbotAI* botAI) { return new LastSpellCastValue(botAI); }
    static UntypedValue* last_spell_cast_time(PlayerbotAI* botAI) { return new LastSpellCastTimeValue(botAI); }
    static UntypedValue* spell_cast_useful(PlayerbotAI* botAI) { return new SpellCastUsefulValue(botAI); }

    static UntypedValue* expected_lifetime(PlayerbotAI* ai) { return new EstimatedLifetimeValue(ai); }
    static UntypedValue* expected_group_dps(PlayerbotAI* ai) { return new EstimatedGroupDpsValue(ai); }

    static UntypedValue* attacker_without_aura(PlayerbotAI* botAI) { return new AttackerWithoutAuraTargetValue(botAI); }
    static UntypedValue* melee_attacker_without_aura(PlayerbotAI* botAI) { return new MeleeAttackerWithoutAuraTargetValue(botAI); }
    static UntypedValue* party_member_without_aura(PlayerbotAI* botAI) { return new PartyMemberWithoutAuraValue(botAI); }
    static UntypedValue* party_member_to_heal(PlayerbotAI* botAI) { return new PartyMemberToHeal(botAI); }
    static UntypedValue* party_member_to_resurrect(PlayerbotAI* botAI) { return new PartyMemberToResurrect(botAI); }
    static UntypedValue* party_member_to_dispel(PlayerbotAI* botAI) { return new PartyMemberToDispel(botAI); }
    static UntypedValue* party_member_to_protect(PlayerbotAI* botAI) { return new PartyMemberToProtect(botAI); }

    static UntypedValue* main_tank(PlayerbotAI* ai) { return new PartyMemberMainTankValue(ai); }
    static UntypedValue* tank_target(PlayerbotAI* botAI) { return new TankTargetValue(botAI); }
    static UntypedValue* threat(PlayerbotAI* botAI) { return new ThreatValue(botAI); }
    static UntypedValue* has_aggro(PlayerbotAI* botAI) { return new HasAggroValue(botAI); }

    static UntypedValue* last_flee_angle(PlayerbotAI* ai) { return new LastFleeAngleValue(ai); }
    static UntypedValue* last_flee_timestamp(PlayerbotAI* ai) { return new LastFleeTimestampValue(ai); }
    static UntypedValue* recently_flee_info(PlayerbotAI* ai) { return new RecentlyFleeInfo(ai); }
};

#endif
