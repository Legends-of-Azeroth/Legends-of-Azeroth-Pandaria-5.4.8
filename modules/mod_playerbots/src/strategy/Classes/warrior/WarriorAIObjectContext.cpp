/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "WarriorAIObjectContext.h"

#include "ArmsWarriorStrategy.h"
#include "FuryWarriorStrategy.h"
#include "GenericWarriorNonCombatStrategy.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "TankWarriorStrategy.h"
#include "WarriorActions.h"
#include "WarriorTriggers.h"

class WarriorStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    WarriorStrategyFactoryInternal()
    {
        creators["nc"] = &WarriorStrategyFactoryInternal::nc;
        creators["aoe"] = &WarriorStrategyFactoryInternal::aoe;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericWarriorNonCombatStrategy(botAI); }
    static Strategy* aoe(PlayerbotAI* botAI) { return new WarriorAoeStrategy(botAI); }
};

class WarriorCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    WarriorCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["tank"] = &WarriorCombatStrategyFactoryInternal::tank;
        creators["arms"] = &WarriorCombatStrategyFactoryInternal::arms;
        creators["fury"] = &WarriorCombatStrategyFactoryInternal::fury;
    }
private:
    static Strategy* tank(PlayerbotAI* botAI) { return new TankWarriorStrategy(botAI); }
    static Strategy* arms(PlayerbotAI* botAI) { return new ArmsWarriorStrategy(botAI); }
    static Strategy* fury(PlayerbotAI* botAI) { return new FuryWarriorStrategy(botAI); }
};

class WarriorTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    WarriorTriggerFactoryInternal()
    {
        creators["battle shout"] = &WarriorTriggerFactoryInternal::battle_shout;
        creators["commanding shout"] = &WarriorTriggerFactoryInternal::commanding_shout;
        creators["battle stance"] = &WarriorTriggerFactoryInternal::battle_stance;
        creators["defensive stance"] = &WarriorTriggerFactoryInternal::defensive_stance;
        creators["berserker stance"] = &WarriorTriggerFactoryInternal::berserker_stance;

        creators["victory rush"] = &WarriorTriggerFactoryInternal::victory_rush;
        creators["thunder clap"] = &WarriorTriggerFactoryInternal::thunder_clap;
        creators["sunder armor"] = &WarriorTriggerFactoryInternal::sunder_armor;
        creators["colossus smash"] = &WarriorTriggerFactoryInternal::colossus_smash;
        creators["disarm"] = &WarriorTriggerFactoryInternal::disarm;

        creators["pummel on enemy healer"] = &WarriorTriggerFactoryInternal::pummel_on_enemy_healer;
        creators["pummel"] = &WarriorTriggerFactoryInternal::pummel;

        // -- arms
        creators["overpower"] = &WarriorTriggerFactoryInternal::overpower;
        creators["mortal wounds"] = &WarriorTriggerFactoryInternal::mortal_wounds;
        creators["taste for blood"] = &WarriorTriggerFactoryInternal::taste_for_blood;

        // -- fury
        creators["raging blow"] = &WarriorTriggerFactoryInternal::raging_blow;
        creators["meat cleaver"] = &WarriorTriggerFactoryInternal::meat_cleaver;
        creators["bloodsurge"] = &WarriorTriggerFactoryInternal::bloodsurge;

        // -- prot
        creators["shield barrier"] = &WarriorTriggerFactoryInternal::shield_barrier;
        creators["shield block"] = &WarriorTriggerFactoryInternal::shield_block;
        creators["revenge"] = &WarriorTriggerFactoryInternal::revenge;
        creators["vigilance"] = &WarriorTriggerFactoryInternal::vigilance;
    }
private:
    static Trigger* battle_shout(PlayerbotAI* botAI) { return new BattleShoutTrigger(botAI); }
    static Trigger* commanding_shout(PlayerbotAI* botAI) { return new CommandingShoutTrigger(botAI); }
    static Trigger* defensive_stance(PlayerbotAI* botAI) { return new DefensiveStanceTrigger(botAI); }
    static Trigger* berserker_stance(PlayerbotAI* botAI) { return new BerserkerStanceTrigger(botAI); }
    static Trigger* battle_stance(PlayerbotAI* botAI) { return new BattleStanceTrigger(botAI); }

    static Trigger* victory_rush(PlayerbotAI* botAI) { return new VictoryRushTrigger(botAI); }
    static Trigger* thunder_clap(PlayerbotAI* botAI) { return new ThunderClapTrigger(botAI); }
    static Trigger* sunder_armor(PlayerbotAI* botAI) { return new SunderArmorTrigger(botAI); }
    static Trigger* colossus_smash(PlayerbotAI* botAI) { return new ColossusSmashTrigger(botAI); }
    static Trigger* disarm(PlayerbotAI* botAI) { return new DisarmDebuffTrigger(botAI); }

    static Trigger* pummel(PlayerbotAI* botAI) { return new PummelInterruptSpellTrigger(botAI); }
    static Trigger* pummel_on_enemy_healer(PlayerbotAI* botAI) { return new PummelInterruptEnemyHealerSpellTrigger(botAI); }

    // -- arms
    static Trigger* overpower(PlayerbotAI* botAI) { return new OverpowerAvailableTrigger(botAI); }
    static Trigger* mortal_wounds(PlayerbotAI* botAI) { return new MortalWoundsDebuffTrigger(botAI); }
    static Trigger* taste_for_blood(PlayerbotAI* botAI) { return new TasteForBloodTrigger(botAI); }
    
    // -- fury
    static Trigger* raging_blow(PlayerbotAI* botAI) { return new RagingBlowAvailableTrigger(botAI); }
    static Trigger* meat_cleaver(PlayerbotAI* botAI) { return new MeatCleaverTrigger(botAI); }
    static Trigger* bloodsurge(PlayerbotAI* botAI) { return new BloodSurgeTrigger(botAI); }

    // -- prot
    static Trigger* shield_block(PlayerbotAI* botAI) { return new ShieldBlockTrigger(botAI); }
    static Trigger* shield_barrier(PlayerbotAI* botAI) { return new ShieldBarrierTrigger(botAI); }
    static Trigger* revenge(PlayerbotAI* botAI) { return new RevengeAvailableTrigger(botAI); }
    static Trigger* vigilance(PlayerbotAI* botAI) { return new VigilanceTrigger(botAI); }
};

class WarriorAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    WarriorAiObjectContextInternal()
    {
        // -- buffs
        creators["battle shout"] = &WarriorAiObjectContextInternal::battle_shout;
        creators["commanding shout"] = &WarriorAiObjectContextInternal::commanding_shout;
        creators["battle stance"] = &WarriorAiObjectContextInternal::battle_stance;
        creators["berserker stance"] = &WarriorAiObjectContextInternal::berserker_stance;
        creators["defensive stance"] = &WarriorAiObjectContextInternal::defensive_stance;

        // -- general
        creators["charge"] = &WarriorAiObjectContextInternal::charge;
        creators["heroic strike"] = &WarriorAiObjectContextInternal::heroic_strike;
        creators["heroic leap"] = &WarriorAiObjectContextInternal::heroic_leap;
        creators["victory rush"] = &WarriorAiObjectContextInternal::victory_rush;
        creators["execute"] = &WarriorAiObjectContextInternal::execute;
        creators["sunder armor"] = &WarriorAiObjectContextInternal::sunder_armor;

        creators["bloodbath"] = &WarriorAiObjectContextInternal::bloodbath;
        creators["berserker rage"] = &WarriorAiObjectContextInternal::berserker_rage;
        creators["rallying cry"] = &WarriorAiObjectContextInternal::rallying_cry;
        creators["shield wall"] = &WarriorAiObjectContextInternal::shield_wall;
        creators["recklessness"] = &WarriorAiObjectContextInternal::recklessness;
        creators["colossus smash"] = &WarriorAiObjectContextInternal::colossus_smash;
        creators["disarm"] = &WarriorAiObjectContextInternal::disarm;
        creators["taunt"] = &WarriorAiObjectContextInternal::taunt;

        // -- aoe
        creators["thunder clap"] = &WarriorAiObjectContextInternal::thunder_clap;
        creators["whirlwind"] = &WarriorAiObjectContextInternal::whirlwind;
        creators["cleave"] = &WarriorAiObjectContextInternal::cleave;
        creators["bladestorm"] = &WarriorAiObjectContextInternal::bladestorm;
        creators["dragonroar"] = &WarriorAiObjectContextInternal::dragonroar;
        creators["shockwave"] = &WarriorAiObjectContextInternal::shockwave;

        // -- arms
        creators["mortal strike"] = &WarriorAiObjectContextInternal::mortal_strike;
        creators["slam"] = &WarriorAiObjectContextInternal::slam;
        creators["overpower"] = &WarriorAiObjectContextInternal::overpower;
        creators["die by the sword"] = &WarriorAiObjectContextInternal::die_by_the_sword;
        creators["sweeping strikes"] = &WarriorAiObjectContextInternal::sweeping_strikes;
        creators["demoralizing banner"] = &WarriorAiObjectContextInternal::demoralizing_banner;

        // -- fury
        creators["skull banner"] = &WarriorAiObjectContextInternal::skull_banner;
        creators["raging blow"] = &WarriorAiObjectContextInternal::raging_blow;
        creators["bloodthirst"] = &WarriorAiObjectContextInternal::bloodthirst;
        creators["wild strike"] = &WarriorAiObjectContextInternal::wild_strike;

        // -- prot
        creators["demoralizing shout"] = &WarriorAiObjectContextInternal::demoralizing_shout;
        creators["devastate"] = &WarriorAiObjectContextInternal::devastate;
        creators["last stand"] = &WarriorAiObjectContextInternal::last_stand;
        creators["revenge"] = &WarriorAiObjectContextInternal::revenge;
        creators["shield barrier"] = &WarriorAiObjectContextInternal::shield_barrier;
        creators["shield block"] = &WarriorAiObjectContextInternal::shield_block;
        creators["shield slam"] = &WarriorAiObjectContextInternal::shield_slam;
        creators["enraged regeneration"] = &WarriorAiObjectContextInternal::enraged_regeneration;
        creators["vigilance"] = &WarriorAiObjectContextInternal::vigilance;
        creators["storm bolt"] = &WarriorAiObjectContextInternal::storm_bolt;
        creators["mocking banner"] = &WarriorAiObjectContextInternal::mocking_banner;
    }
private:
    // -- buffs
    static Action* battle_shout(PlayerbotAI* botAI) { return new CastBattleShoutAction(botAI); }
    static Action* commanding_shout(PlayerbotAI* botAI) { return new CastCommandingShoutAction(botAI); }
    static Action* defensive_stance(PlayerbotAI* botAI) { return new CastDefensiveStanceAction(botAI); }
    static Action* battle_stance(PlayerbotAI* botAI) { return new CastBattleStanceAction(botAI); }
    static Action* berserker_stance(PlayerbotAI* botAI) { return new CastBerserkerStanceAction(botAI); }

    // -- generic
    static Action* charge(PlayerbotAI* botAI) { return new CastChargeAction(botAI); }
    static Action* heroic_strike(PlayerbotAI* botAI) { return new CastHeroicStrikeAction(botAI); }
    static Action* heroic_leap(PlayerbotAI* botAI) { return new CastHeroicLeapAction(botAI); }
    static Action* victory_rush(PlayerbotAI* botAI) { return new CastVictoryRushAction(botAI); }
    static Action* execute(PlayerbotAI* botAI) { return new CastExecuteAction(botAI); }
    static Action* sunder_armor(PlayerbotAI* botAI) { return new CastSunderArmorAction(botAI); }
    static Action* bloodbath(PlayerbotAI* botAI) { return new CastBloodbathAction(botAI); }
    static Action* berserker_rage(PlayerbotAI* botAI) { return new CastBerserkerRageAction(botAI); }
    static Action* rallying_cry(PlayerbotAI* botAI) { return new CastRallyingCryAction(botAI); }
    static Action* shield_wall(PlayerbotAI* botAI) { return new CastShieldWallAction(botAI); }
    static Action* recklessness(PlayerbotAI* botAI) { return new CastRecklessnessAction(botAI); }
    static Action* die_by_the_sword(PlayerbotAI* botAI) { return new CastDieByTheSwordAction(botAI); }
    static Action* colossus_smash(PlayerbotAI* botAI) { return new CastColossusSmashAction(botAI); }
    static Action* disarm(PlayerbotAI* botAI) { return new CastDisarmAction(botAI); }
    static Action* taunt(PlayerbotAI* botAI) { return new CastTauntAction(botAI); }

    // -- aoe
    static Action* whirlwind(PlayerbotAI* botAI) { return new CastWhirlWindAction(botAI); }
    static Action* thunder_clap(PlayerbotAI* botAI) { return new CastThunderClapAction(botAI); }
    static Action* cleave(PlayerbotAI* botAI) { return new CastCleaveAction(botAI); }
    static Action* bladestorm(PlayerbotAI* botAI) { return new CastBladestormAction(botAI); }
    static Action* dragonroar(PlayerbotAI* botAI) { return new CastDragonroarAction(botAI); }
    static Action* shockwave(PlayerbotAI* botAI) { return new CastShockwaveAction(botAI); }

    // -- ARMS
    static Action* demoralizing_banner(PlayerbotAI* botAI) { return new CastDemoralizingBannerAction(botAI); }
    static Action* mortal_strike(PlayerbotAI* botAI) { return new CastMortalStrikeAction(botAI); }
    static Action* slam(PlayerbotAI* botAI) { return new CastSlamAction(botAI); }
    static Action* overpower(PlayerbotAI* botAI) { return new CastOverPowerAction(botAI); }    
    static Action* sweeping_strikes(PlayerbotAI* botAI) { return new CastSweepingStrikesAction(botAI); }
    
    // -- FURY
    static Action* skull_banner(PlayerbotAI* botAI) { return new CastSkullBannerAction(botAI); }
    static Action* raging_blow(PlayerbotAI* botAI) { return new CastRagingBlowAction(botAI); }
    static Action* bloodthirst(PlayerbotAI* botAI) { return new CastBloodThirstAction(botAI); }
    static Action* wild_strike(PlayerbotAI* botAI) { return new CastWildStrikeAction(botAI); }

    // -- PROT
    static Action* demoralizing_shout(PlayerbotAI* botAI) { return new CastDemoralizingShoutAction(botAI); }
    static Action* devastate(PlayerbotAI* botAI) { return new CastDevastateAction(botAI); }
    static Action* last_stand(PlayerbotAI* botAI) { return new CastLastStandAction(botAI); }
    static Action* revenge(PlayerbotAI* botAI) { return new CastRevengeAction(botAI); }
    static Action* shield_barrier(PlayerbotAI* botAI) { return new CastShieldBarrierAction(botAI); }
    static Action* shield_block(PlayerbotAI* botAI) { return new CastShieldBlockAction(botAI); }
    static Action* shield_slam(PlayerbotAI* botAI) { return new CastShieldSlamAction(botAI); }
    static Action* enraged_regeneration(PlayerbotAI* botAI) { return new CastEnragedRegenerationAction(botAI); }
    static Action* vigilance(PlayerbotAI* botAI) { return new CastVigilanceAction(botAI); }
    static Action* storm_bolt(PlayerbotAI* botAI) { return new CastStormBoltAction(botAI); }
    static Action* mocking_banner(PlayerbotAI* botAI) { return new CastMockingBannerAction(botAI); }
};

WarriorAiObjectContext::WarriorAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new WarriorStrategyFactoryInternal());
    strategyContexts.Add(new WarriorCombatStrategyFactoryInternal());
    actionContexts.Add(new WarriorAiObjectContextInternal());
    triggerContexts.Add(new WarriorTriggerFactoryInternal());
}
