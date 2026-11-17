/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "MageAiObjectContext.h"

#include "ArcaneMageStrategy.h"
#include "FireMageStrategy.h"
#include "FrostMageStrategy.h"
#include "GenericMageNonCombatStrategy.h"
#include "MageActions.h"
#include "MageTriggers.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"

/*
* TODO
* aoe => flamestrike / arcane explo / blizzard etc
*/

class MageStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MageStrategyFactoryInternal()
    {
        creators["nc"] = &MageStrategyFactoryInternal::nc;
        //creators["pull"] = &MageStrategyFactoryInternal::pull;
        creators["fire aoe"] = &MageStrategyFactoryInternal::fire_aoe;
        creators["frost aoe"] = &MageStrategyFactoryInternal::frost_aoe;
        creators["arcane aoe"] = &MageStrategyFactoryInternal::arcane_aoe;
        //creators["cure"] = &MageStrategyFactoryInternal::cure;
        creators["buff"] = &MageStrategyFactoryInternal::buff;
        //creators["boost"] = &MageStrategyFactoryInternal::boost;
        //creators["cc"] = &MageStrategyFactoryInternal::cc;
    }

private:
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericMageNonCombatStrategy(botAI); }
    //static Strategy* pull(PlayerbotAI* botAI) { return new PullStrategy(botAI, "shoot"); }
    static Strategy* fire_aoe(PlayerbotAI* botAI) { return new FireMageAoeStrategy(botAI); }
    static Strategy* frost_aoe(PlayerbotAI* botAI) { return new FrostMageAoeStrategy(botAI); }
    static Strategy* arcane_aoe(PlayerbotAI* botAI) { return new ArcaneMageAoeStrategy(botAI); }
    //static Strategy* cure(PlayerbotAI* botAI) { return new MageCureStrategy(botAI); }
    static Strategy* buff(PlayerbotAI* botAI) { return new MageBuffStrategy(botAI); }
    //static Strategy* boost(PlayerbotAI* botAI) { return new MageBoostStrategy(botAI); }
    //static Strategy* cc(PlayerbotAI* botAI) { return new MageCcStrategy(botAI); }
};

class MageCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MageCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["frost"] = &MageCombatStrategyFactoryInternal::frost;
        creators["fire"] = &MageCombatStrategyFactoryInternal::fire;
        creators["arcane"] = &MageCombatStrategyFactoryInternal::arcane;
    }

private:
    static Strategy* frost(PlayerbotAI* botAI) { return new FrostMageStrategy(botAI); }
    static Strategy* fire(PlayerbotAI* botAI) { return new FireMageStrategy(botAI); }
    static Strategy* arcane(PlayerbotAI* botAI) { return new ArcaneMageStrategy(botAI); }
};

class MageBuffStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    MageBuffStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["bmana"] = &MageBuffStrategyFactoryInternal::bmana;
        creators["bdps"] = &MageBuffStrategyFactoryInternal::bdps;
    }

private:
    static Strategy* bmana(PlayerbotAI* botAI) { return new MageBuffManaStrategy(botAI); }
    static Strategy* bdps(PlayerbotAI* botAI) { return new MageBuffDpsStrategy(botAI); }
};

class MageTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    MageTriggerFactoryInternal()
    {
        creators["arcane brilliance"] = &MageTriggerFactoryInternal::arcane_brilliance;
        creators["rune of power"] = &MageTriggerFactoryInternal::rune_of_power;
        creators["mage armor"] = &MageTriggerFactoryInternal::mage_armor;
        creators["frost armor"] = &MageTriggerFactoryInternal::frost_armor;
        creators["molten armor"] = &MageTriggerFactoryInternal::molten_armor;
        creators["living bomb"] = &MageTriggerFactoryInternal::living_bomb;
        creators["missile barrage"] = &MageTriggerFactoryInternal::missile_barrage;
        creators["presence of mind"] = &MageTriggerFactoryInternal::presence_of_mind;
        creators["frost nova on target"] = &MageTriggerFactoryInternal::frost_nova_on_target;
        creators["counterspell"] = &MageTriggerFactoryInternal::counterspell;
        creators["spellsteal"] = &MageTriggerFactoryInternal::spellsteal;

        // -- fire
        creators["heating up"] = &MageTriggerFactoryInternal::heating_up;
        creators["pyroblast!"] = &MageTriggerFactoryInternal::pyroblast;

        // -- forst
        creators["icy veins"] = &MageTriggerFactoryInternal::icy_veins;
        creators["fingers of frost single"] = &MageTriggerFactoryInternal::fingers_of_frost_single;
        creators["fingers of frost double"] = &MageTriggerFactoryInternal::fingers_of_frost_double;
        creators["brain freeze"] = &MageTriggerFactoryInternal::brain_freeze;
        creators["forst bomb"] = &MageTriggerFactoryInternal::frost_bomb;

        // -- arcane
        creators["arcane blast"] = &MageTriggerFactoryInternal::arcane_blast;
        creators["arcane power"] = &MageTriggerFactoryInternal::arcane_power;
        creators["arcane missiles"] = &MageTriggerFactoryInternal::arcane_missiles_aura;
        creators["arcane charge"] = &MageTriggerFactoryInternal::arcane_charge_aura;
        creators["slow not on target"] = &MageTriggerFactoryInternal::slow_not_on_target;
        creators["nether tempest"] = &MageTriggerFactoryInternal::nether_tempest;
    }

private:
    static Trigger* presence_of_mind(PlayerbotAI* botAI) { return new PresenceOfMindTrigger(botAI); }
    static Trigger* arcane_brilliance(PlayerbotAI* botAI) { return new ArcaneBrillianceTrigger(botAI); }
    static Trigger* rune_of_power(PlayerbotAI* botAI) { return new RuneOfPowerTrigger(botAI); }
    static Trigger* mage_armor(PlayerbotAI* botAI) { return new MageArmorTrigger(botAI); }
    static Trigger* frost_armor(PlayerbotAI* botAI) { return new FrostArmorTrigger(botAI); }
    static Trigger* molten_armor(PlayerbotAI* botAI) { return new MoltenArmorTrigger(botAI); }
    static Trigger* living_bomb(PlayerbotAI* botAI) { return new LivingBombTrigger(botAI); }
    static Trigger* frost_nova_on_target(PlayerbotAI* botAI) { return new FrostNovaOnTargetTrigger(botAI); }
    static Trigger* counterspell(PlayerbotAI* botAI) { return new CounterspellInterruptSpellTrigger(botAI); }
    static Trigger* spellsteal(PlayerbotAI* botAI) { return new SpellstealTrigger(botAI); }

    // -- fire
    static Trigger* heating_up(PlayerbotAI* botAI) { return new HeatingUpTrigger(botAI); }
    static Trigger* pyroblast(PlayerbotAI* botAI) { return new PyroblastTrigger(botAI); }

    // -- frost
    static Trigger* icy_veins(PlayerbotAI* botAI) { return new IcyVeinsTrigger(botAI); }
    static Trigger* fingers_of_frost_single(PlayerbotAI* botAI) { return new FingersOfFrostSingleTrigger(botAI); }
    static Trigger* fingers_of_frost_double(PlayerbotAI* botAI) { return new FingersOfFrostDoubleTrigger(botAI); }
    static Trigger* brain_freeze(PlayerbotAI* botAI) { return new BrainFreezeTrigger(botAI); }
    static Trigger* frost_bomb(PlayerbotAI* botAI) { return new FrostBombTrigger(botAI); }

    // -- arcane
    static Trigger* arcane_power(PlayerbotAI* botAI) { return new ArcanePowerTrigger(botAI); }
    static Trigger* missile_barrage(PlayerbotAI* botAI) { return new MissileBarrageTrigger(botAI); }
    static Trigger* arcane_blast(PlayerbotAI* botAI) { return new ArcaneBlastTrigger(botAI); }
    static Trigger* arcane_missiles_aura(PlayerbotAI* botAI) { return new ArcaneMissilesTrigger(botAI); }
    static Trigger* arcane_charge_aura(PlayerbotAI* botAI) { return new ArcaneChargeTrigger(botAI); }
    static Trigger* slow_not_on_target(PlayerbotAI* botAI) { return new SlowNotOnTargetTrigger(botAI); }
    static Trigger* nether_tempest(PlayerbotAI* botAI) { return new NetherTempestTrigger(botAI); }
};

class MageAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    MageAiObjectContextInternal()
    {
        // -- generic needs actions
        creators["molten armor"] = &MageAiObjectContextInternal::molten_armor;
        creators["mage armor"] = &MageAiObjectContextInternal::mage_armor;
        creators["frost armor"] = &MageAiObjectContextInternal::frost_armor;
        creators["arcane brilliance"] = &MageAiObjectContextInternal::arcane_brilliance;
        creators["rune of power"] = &MageAiObjectContextInternal::rune_of_power;

        creators["frostfire bolt"] = &MageAiObjectContextInternal::frostfirebolt;
        creators["frost nova"] = &MageAiObjectContextInternal::frostnova;
        creators["fire blast"] = &MageAiObjectContextInternal::fireblast;
        creators["counterspell"] = &MageAiObjectContextInternal::counterspell;
        creators["counterspell on enemy healer"] = &MageAiObjectContextInternal::counterspell_on_enemy_healer;
        creators["spellsteal"] = &MageAiObjectContextInternal::spellsteal;
        creators["arcane explosion"] = &MageAiObjectContextInternal::arcaneexplosion;
        creators["blizzard"] = &MageAiObjectContextInternal::blizzard;
        creators["ice lance"] = &MageAiObjectContextInternal::icelance;
        creators["arcane missiles"] = &MageAiObjectContextInternal::arcanemissiles;
        creators["cone of cold"] = &MageAiObjectContextInternal::coneofcold;
        creators["remove curse"] = &MageAiObjectContextInternal::removecurse;
        creators["remove curse on party"] = &MageAiObjectContextInternal::removecurseonparty;
        creators["evocation"] = &MageAiObjectContextInternal::evocation;
        creators["deep freeze"] = &MageAiObjectContextInternal::deepfreeze;
        creators["ice block"] = &MageAiObjectContextInternal::ice_block;
        creators["blink"] = &MageAiObjectContextInternal::blink;
        creators["frostjaw"] = &MageAiObjectContextInternal::frostjaw;

        // --

        // -- fire specific talent or spec
        creators["fireball"] = &MageAiObjectContextInternal::fireball;
        creators["dragon's breath"] = &MageAiObjectContextInternal::dragon_breath;
        creators["inferno blast"] = &MageAiObjectContextInternal::inferno_blast;
        creators["pyroblast"] = &MageAiObjectContextInternal::pyroblast;
        creators["scorch"] = &MageAiObjectContextInternal::scorch;
        creators["living bomb"] = &MageAiObjectContextInternal::living_bomb;
        creators["flamestrike"] = &MageAiObjectContextInternal::flamestrike;
        

        // -- frost specific talent or spec
        creators["frostbolt"] = &MageAiObjectContextInternal::frostbolt;
        creators["summon water elemental"] = &summon_water_elemental;
        creators["fingers of frost"] = &MageAiObjectContextInternal::fingers_of_frost;
        creators["icy veins"] = &MageAiObjectContextInternal::icy_veins;
        creators["frozen orb"] = &MageAiObjectContextInternal::frozen_orb;
        creators["cold snap"] = &MageAiObjectContextInternal::cold_snap;
        creators["ice barrier"] = &MageAiObjectContextInternal::ice_barrier;
        creators["forst bomb"] = &MageAiObjectContextInternal::frost_bomb;

        // -- arcane
        creators["arcane power"] = &MageAiObjectContextInternal::arcane_power;
        creators["arcane barrage"] = &MageAiObjectContextInternal::arcane_barrage;
        creators["arcane blast"] = &MageAiObjectContextInternal::arcane_blast;
        creators["slow"] = &MageAiObjectContextInternal::slow;
        creators["nether tempest"] = &MageAiObjectContextInternal::nether_tempest;
    }

private:
    static Action* molten_armor(PlayerbotAI* botAI) { return new CastMoltenArmorAction(botAI); }
    static Action* mage_armor(PlayerbotAI* botAI) { return new CastMageArmorAction(botAI); }
    static Action* frost_armor(PlayerbotAI* botAI) { return new CastFrostArmorAction(botAI); }
    static Action* arcane_brilliance(PlayerbotAI* botAI) { return new CastArcaneBrillanceAction(botAI); }
    static Action* rune_of_power(PlayerbotAI* botAI) { return new CastRuneOfPowerAction(botAI); }
    
    static Action* frostfirebolt(PlayerbotAI* botAI) { return new CastFrostfireBoltAction(botAI); }
    static Action* frostbolt(PlayerbotAI* botAI) { return new CastFrostboltAction(botAI); }
    static Action* frostnova(PlayerbotAI* botAI) { return new CastFrostNovaAction(botAI); }
    static Action* fireblast(PlayerbotAI* botAI) { return new CastFireBlastAction(botAI); }
    static Action* counterspell(PlayerbotAI* botAI) { return new CastCounterspellAction(botAI); }
    static Action* counterspell_on_enemy_healer(PlayerbotAI* botAI) { return new CastCounterspellOnEnemyHealerAction(botAI); }
    static Action* spellsteal(PlayerbotAI* botAI) { return new CastSpellstealAction(botAI); }
    static Action* arcaneexplosion(PlayerbotAI* botAI) { return new CastArcaneExplosionAction(botAI); }
    static Action* blizzard(PlayerbotAI* botAI) { return new CastBlizzardAction(botAI); }
    static Action* icelance(PlayerbotAI* botAI) { return new CastIceLanceAction(botAI); }
    static Action* arcanemissiles(PlayerbotAI* botAI) { return new CastArcaneMissilesAction(botAI); }
    static Action* coneofcold(PlayerbotAI* botAI) { return new CastConeOfColdAction(botAI); }
    static Action* removecurse(PlayerbotAI* botAI) { return new CastRemoveCurseAction(botAI); }
    static Action* removecurseonparty(PlayerbotAI* botAI) { return new CastRemoveCurseOnPartyAction(botAI); }
    static Action* evocation(PlayerbotAI* botAI) { return new CastEvocationAction(botAI); }
    static Action* deepfreeze(PlayerbotAI* botAI) { return new CastDeepFreezeAction(botAI); }
    static Action* ice_block(PlayerbotAI* botAI) { return new CastIceBlockAction(botAI); }
    static Action* blink(PlayerbotAI* botAI) { return new CastBlinkAction(botAI); }
    static Action* fingers_of_frost(PlayerbotAI* botAI) { return new CastFingersOfFrostAction(botAI); }
    static Action* icy_veins(PlayerbotAI* botAI) { return new CastIcyVeinsAction(botAI); }
    static Action* frozen_orb(PlayerbotAI* botAI) { return new CastFrozenOrbAction(botAI); }
    static Action* summon_water_elemental(PlayerbotAI* botAI) { return new CastSummonWaterElementalAction(botAI); }
    static Action* cold_snap(PlayerbotAI* botAI) { return new CastColdSnapAction(botAI); }
    static Action* ice_barrier(PlayerbotAI* botAI) { return new CastIceBarrierAction(botAI); }
    static Action* frostjaw(PlayerbotAI* botAI) { return new CastFrostjawAction(botAI); }

    // -- fire
    static Action* fireball(PlayerbotAI* botAI) { return new CastFireballAction(botAI); }
    static Action* dragon_breath(PlayerbotAI* botAI) { return new CastDragonsBreathAction(botAI); }
    static Action* inferno_blast(PlayerbotAI* botAI) { return new CastInfernoBlastAction(botAI); }
    static Action* pyroblast(PlayerbotAI* botAI) { return new CastPyroblastAction(botAI); }
    static Action* scorch(PlayerbotAI* botAI) { return new CastScorchAction(botAI); }
    static Action* living_bomb(PlayerbotAI* botAI) { return new CastLivingBombAction(botAI); }
    static Action* flamestrike(PlayerbotAI* botAI) { return new CastFlamestrikeAction(botAI); }


    // -- forst
    static Action* frost_bomb(PlayerbotAI* botAI) { return new CastFrostBombAction(botAI); }

    // -- arcane
    static Action* arcane_power(PlayerbotAI* botAI) { return new CastArcanePowerAction(botAI); }
    static Action* arcane_blast(PlayerbotAI* botAI) { return new CastArcaneBlastAction(botAI); }
    static Action* arcane_barrage(PlayerbotAI* botAI) { return new CastArcaneBarrageAction(botAI); }
    static Action* slow(PlayerbotAI* botAI) { return new CastSlowAction(botAI); }
    static Action* nether_tempest(PlayerbotAI* botAI) { return new CastNetherTempestAction(botAI); }

    static Action* focus_magic_on_party(PlayerbotAI* botAI) { return new CastFocusMagicOnPartyAction(botAI); }
};

MageAiObjectContext::MageAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new MageStrategyFactoryInternal());
    strategyContexts.Add(new MageCombatStrategyFactoryInternal());
    strategyContexts.Add(new MageBuffStrategyFactoryInternal());
    actionContexts.Add(new MageAiObjectContextInternal());
    triggerContexts.Add(new MageTriggerFactoryInternal());
}
