#include "WarlockAiObjectContext.h"

#include "AffliWarlockStrategy.h"
#include "DemonoWarlockStrategy.h"
#include "DestruWarlockStrategy.h"
#include "GenericTriggers.h"
#include "GenericWarlockNonCombatStrategy.h"
#include "NamedObjectContext.h"
#include "Playerbots.h"
#include "Strategy.h"
#include "WarlockActions.h"
#include "WarlockTriggers.h"

class WarlockStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    WarlockStrategyFactoryInternal()
    {
        creators["nc"] = &WarlockStrategyFactoryInternal::nc;
        //creators["pull"] = &WarlockStrategyFactoryInternal::pull;
        creators["aoe"] = &WarlockStrategyFactoryInternal::aoe;
        creators["boost"] = &WarlockStrategyFactoryInternal::boost;
        creators["cc"] = &WarlockStrategyFactoryInternal::cc;
        creators["pet"] = &WarlockStrategyFactoryInternal::pet;
    }

private:
    static Strategy* pet(PlayerbotAI* botAI) { return new WarlockPetStrategy(botAI); }
    static Strategy* nc(PlayerbotAI* botAI) { return new GenericWarlockNonCombatStrategy(botAI); }
    static Strategy* aoe(PlayerbotAI* botAI) { return new AoeWarlockStrategy(botAI); }
    //static Strategy* pull(PlayerbotAI* botAI) { return new PullStrategy(botAI, "shoot"); }
    static Strategy* boost(PlayerbotAI* botAI) { return new WarlockBoostStrategy(botAI); }
    static Strategy* cc(PlayerbotAI* botAI) { return new WarlockCcStrategy(botAI); }
};

class WarlockCombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    WarlockCombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["affli"] = &WarlockCombatStrategyFactoryInternal::affli;
        creators["demono"] = &WarlockCombatStrategyFactoryInternal::demono;
        creators["destru"] = &WarlockCombatStrategyFactoryInternal::destru;
    }

private:
    static Strategy* affli(PlayerbotAI* botAI) { return new AffliWarlockStrategy(botAI); }
    static Strategy* demono(PlayerbotAI* botAI) { return new DemonoWarlockStrategy(botAI); }
    static Strategy* destru(PlayerbotAI* botAI) { return new DestruWarlockStrategy(botAI); }
};

class NonCombatBuffStrategyFactoryInternal : public NamedObjectContext<Strategy>
{
public:
    NonCombatBuffStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
    {
        creators["bdps"] = &NonCombatBuffStrategyFactoryInternal::felguard;
        creators["bmana"] = &NonCombatBuffStrategyFactoryInternal::felhunter;
        creators["bhealth"] = &NonCombatBuffStrategyFactoryInternal::imp;
    }

private:
    static Strategy* imp(PlayerbotAI* ai) { return new SummonImpStrategy(ai); }
    static Strategy* felhunter(PlayerbotAI* ai) { return new SummonFelhunterStrategy(ai); }
    static Strategy* felguard(PlayerbotAI* ai) { return new SummonFelguardStrategy(ai); }
};

class WarlockTriggerFactoryInternal : public NamedObjectContext<Trigger>
{
public:
    WarlockTriggerFactoryInternal()
    {
        // -- talents
        creators["soul link"] = &WarlockTriggerFactoryInternal::soul_link;

        // -- generic
        creators["corruption"] = &WarlockTriggerFactoryInternal::corruption;
        creators["dark intent"] = &WarlockTriggerFactoryInternal::dark_intent;
        creators["curse of enfeeblement"] = &WarlockTriggerFactoryInternal::curse_enfeeblement;
        creators["curse of the elements"] = &WarlockTriggerFactoryInternal::curse_elements;

        // -- Affliction
        creators["agony"] = &WarlockTriggerFactoryInternal::agony;
        creators["unstable affliction"] = &WarlockTriggerFactoryInternal::unstable_affliction;

        // -- Demonologie
        creators["metamorphosis"] = &WarlockTriggerFactoryInternal::metamorphosis;
        creators["molten core"] = &WarlockTriggerFactoryInternal::molten_core;

        // -- Destru
        creators["immolate"] = &WarlockTriggerFactoryInternal::immolate;
        creators["backdraft"] = &WarlockTriggerFactoryInternal::backdraft;
        creators["fire and brimstone"] = &WarlockTriggerFactoryInternal::fire_and_brimstone;

        // -- Pet
        creators["warlock pet low health"] = &WarlockTriggerFactoryInternal::warlock_pet_low_health;
        creators["warlock pet medium health"] = &WarlockTriggerFactoryInternal::warlock_pet_medium_health;
    }

private:
    // -- talents
    static Trigger* soul_link(PlayerbotAI* botAI) { return new SoulLinkTrigger(botAI); }

    // -- generic
    static Trigger* corruption(PlayerbotAI* botAI) { return new CorruptionTrigger(botAI); }
    static Trigger* dark_intent(PlayerbotAI* botAI) { return new DarkIntentTrigger(botAI); }
    static Trigger* curse_enfeeblement(PlayerbotAI* botAI) { return new CurseOfEnfeeblementTrigger(botAI); }
    static Trigger* curse_elements(PlayerbotAI* botAI) { return new CurseOfElementsTrigger(botAI); }

    // -- Affliction
    static Trigger* agony(PlayerbotAI* botAI) { return new CurseOfAgonyTrigger(botAI); }
    static Trigger* unstable_affliction(PlayerbotAI* botAI) { return new UnstableAfflictionTrigger(botAI); }

    // -- Demonologie
    static Trigger* metamorphosis(PlayerbotAI* botAI) { return new MetamorphosisTrigger(botAI); }
    static Trigger* molten_core(PlayerbotAI* botAI) { return new MoltenCoreTrigger(botAI); }

    // -- Destr
    static Trigger* immolate(PlayerbotAI* botAI) { return new ImmolateTrigger(botAI); }
    static Trigger* backdraft(PlayerbotAI* botAI) { return new BackdraftTrigger(botAI); }
    static Trigger* fire_and_brimstone(PlayerbotAI* botAI) { return new FireAndBrimeStoneTrigger(botAI); }

    // -- Pet
    static Trigger* warlock_pet_low_health(PlayerbotAI* botAI) { return new WarlockPetLowHealthTrigger(botAI); }
    static Trigger* warlock_pet_medium_health(PlayerbotAI* botAI) { return new WarlockPetMediumHealthTrigger(botAI); }
};

class WarlockAiObjectContextInternal : public NamedObjectContext<Action>
{
public:
    WarlockAiObjectContextInternal()
    {
        // -- new
        creators["fel flame"] = &WarlockAiObjectContextInternal::fel_flame;
        creators["dark intent"] = &WarlockAiObjectContextInternal::dark_intent;
            // low threat
        creators["soulshatter"] = &WarlockAiObjectContextInternal::soulshatter;
            // low life cd
        creators["unending resolve"] = &WarlockAiObjectContextInternal::unending_resolve;
        creators["twilight ward"] = &WarlockAiObjectContextInternal::twilight_ward;
        creators["health funnel"] = &WarlockAiObjectContextInternal::health_funnel;
        // -- talents
        creators["dark regeneration"] = &WarlockAiObjectContextInternal::dark_regeneration;
        creators["demonic breath"] = &WarlockAiObjectContextInternal::demonic_breath;
        creators["mortal coil"] = &WarlockAiObjectContextInternal::mortal_coil;
        creators["shadowfury"] = &WarlockAiObjectContextInternal::shadowfury;
        creators["soul link"] = &WarlockAiObjectContextInternal::soul_link;
        creators["sacrificial pact"] = &WarlockAiObjectContextInternal::sacrificial_pact;
        creators["dark bargain"] = &WarlockAiObjectContextInternal::dark_bargain;
        creators["blood horror"] = &WarlockAiObjectContextInternal::blood_horror;
        creators["grimoire of supremacy"] = &WarlockAiObjectContextInternal::grimoire_of_supremacy;
        creators["grimoire of sacrifice"] = &WarlockAiObjectContextInternal::grimoire_of_sacrifice;
        creators["mannoroth's fury"] = &WarlockAiObjectContextInternal::mannoroths_fury;

        creators["create healthstone"] = &WarlockAiObjectContextInternal::create_healthstone;
        
        creators["summon voidwalker"] = &WarlockAiObjectContextInternal::summon_voidwalker;
        creators["summon felguard"] = &WarlockAiObjectContextInternal::summon_felguard;
        creators["summon felhunter"] = &WarlockAiObjectContextInternal::summon_felhunter;
        creators["summon succubus"] = &WarlockAiObjectContextInternal::summon_succubus;
        creators["summon imp"] = &WarlockAiObjectContextInternal::summon_imp;
        creators["summon infernal"] = &WarlockAiObjectContextInternal::summon_infernal;
        
        creators["summon wrathguard"] = &WarlockAiObjectContextInternal::summon_wrathguard;
        creators["summon doomguard"] = &WarlockAiObjectContextInternal::summon_doomguard;
        creators["summon fel imp"] = &WarlockAiObjectContextInternal::summon_fel_imp;
        creators["summon voidlord"] = &WarlockAiObjectContextInternal::summon_voidlord;
        creators["summon shivarra"] = &WarlockAiObjectContextInternal::summon_shivarra;
        creators["summon observer"] = &WarlockAiObjectContextInternal::summon_observer;
        creators["summon abyssal"] = &WarlockAiObjectContextInternal::summon_abyssal;
        creators["summon terrorguard"] = &WarlockAiObjectContextInternal::summon_terrorguard;

        // -- generic
        creators["curse of enfeeblement"] = &WarlockAiObjectContextInternal::curse_enfeeblement;
        creators["curse of the elements"] = &WarlockAiObjectContextInternal::curse_elements;
        creators["corruption"] = &WarlockAiObjectContextInternal::corruption;
        creators["corruption on attacker"] = &WarlockAiObjectContextInternal::corruption_on_attacker;
        creators["siphon life"] = &WarlockAiObjectContextInternal::siphon_life;
        creators["siphon life on attacker"] = &WarlockAiObjectContextInternal::siphon_life_on_attacker;
        creators["drain life"] = &WarlockAiObjectContextInternal::drain_life;
        
        creators["shadow bolt"] = &WarlockAiObjectContextInternal::shadow_bolt;
        creators["drain soul"] = &WarlockAiObjectContextInternal::drain_soul;
        creators["banish"] = &WarlockAiObjectContextInternal::banish;
        creators["banish on cc"] = &WarlockAiObjectContextInternal::banish_on_cc;
        creators["seed of corruption"] = &WarlockAiObjectContextInternal::seed_of_corruption;
        creators["seed of corruption on attacker"] = &WarlockAiObjectContextInternal::seed_of_corruption_on_attacker;
        creators["rain of fire"] = &WarlockAiObjectContextInternal::rain_of_fire;
        creators["life tap"] = &WarlockAiObjectContextInternal::life_tap;
        creators["fear"] = &WarlockAiObjectContextInternal::fear;
        creators["fear on cc"] = &WarlockAiObjectContextInternal::fear_on_cc; 
        creators["demonic empowerment"] = &WarlockAiObjectContextInternal::demonic_empowerment;
        
        // -- Affliction
        creators["soul burn"] = &WarlockAiObjectContextInternal::soul_burn;
        creators["dark soul: misery"] = &WarlockAiObjectContextInternal::dark_soul_misery;
        creators["malefic grasp"] = &WarlockAiObjectContextInternal::malefic_grasp;
        creators["agony"] = &WarlockAiObjectContextInternal::curse_of_agony;
        creators["agony on attacker"] = &WarlockAiObjectContextInternal::curse_of_agony_on_attacker;
        creators["unstable affliction"] = &WarlockAiObjectContextInternal::unstable_affliction;
        creators["unstable affliction on attacker"] = &WarlockAiObjectContextInternal::unstable_affliction_on_attacker;
        creators["haunt"] = &WarlockAiObjectContextInternal::haunt;

        // -- Demonologie
        creators["dark soul: knowledge"] = &WarlockAiObjectContextInternal::dark_soul_knowledge;
        creators["hellfire"] = &WarlockAiObjectContextInternal::hellfire;
        creators["hand of guldan"] = &WarlockAiObjectContextInternal::hand_of_guldan;
        creators["soul fire"] = &WarlockAiObjectContextInternal::soul_fire;
        creators["metamorphosis"] = &WarlockAiObjectContextInternal::metamorphosis;
        creators["chaos wave"] = &WarlockAiObjectContextInternal::metamorphosis_chaos_wave;
        creators["doom"] = &WarlockAiObjectContextInternal::metamorphosis_doom;
        creators["immolation aura"] = &WarlockAiObjectContextInternal::metamorphosis_immolation;
        creators["touch of chaos"] = &WarlockAiObjectContextInternal::metamorphosis_touch_chaos;
        creators["void ray"] = &WarlockAiObjectContextInternal::metamorphosis_void_ray;

        // -- Destru
        creators["dark soul: instability"] = &WarlockAiObjectContextInternal::dark_soul_instability;
        creators["chaos bolt"] = &WarlockAiObjectContextInternal::chaos_bolt;
        creators["conflagrate"] = &WarlockAiObjectContextInternal::conflagrate;
        creators["fire and brimstone"] = &WarlockAiObjectContextInternal::fire_and_brimstone;
        creators["flames of xoroth"] = &WarlockAiObjectContextInternal::flames_of_xoroth;
        creators["immolate"] = &WarlockAiObjectContextInternal::immolate;
        creators["immolate on attacker"] = &WarlockAiObjectContextInternal::immolate_on_attacker;
        creators["incinerate"] = &WarlockAiObjectContextInternal::incinerate;
        creators["shadowburn"] = &WarlockAiObjectContextInternal::shadowburn;
    }

private:
    // -- generic
    static Action* summon_imp(PlayerbotAI* botAI) { return new CastSummonImpAction(botAI); }
    static Action* summon_succubus(PlayerbotAI* botAI) { return new CastSummonSuccubusAction(botAI); }
    static Action* summon_voidwalker(PlayerbotAI* botAI) { return new CastSummonVoidwalkerAction(botAI); }
    static Action* summon_felguard(PlayerbotAI* botAI) { return new CastSummonFelguardAction(botAI); }
    static Action* summon_felhunter(PlayerbotAI* botAI) { return new CastSummonFelhunterAction(botAI); }
    static Action* summon_infernal(PlayerbotAI* botAI) { return new CastSummonInfernalAction(botAI); }
    static Action* summon_wrathguard(PlayerbotAI* botAI) { return new CastSummonWrathGuardGuardAction(botAI); }
    static Action* summon_doomguard(PlayerbotAI* botAI) { return new CastSummonDoomGuardAction(botAI); }
    static Action* summon_fel_imp(PlayerbotAI* botAI) { return new CastSummonFelImpAction(botAI); }
    static Action* summon_voidlord(PlayerbotAI* botAI) { return new CastSummonVoidLordAction(botAI); }
    static Action* summon_shivarra(PlayerbotAI* botAI) { return new CastSummonShivarraAction(botAI); }
    static Action* summon_observer(PlayerbotAI* botAI) { return new CastSummonObserverAction(botAI); }
    static Action* summon_abyssal(PlayerbotAI* botAI) { return new CastSummonAbyssalAction(botAI); }
    static Action* summon_terrorguard(PlayerbotAI* botAI) { return new CastSummonTerrorGuardAction(botAI); }
    static Action* demonic_empowerment(PlayerbotAI* ai) { return new CastDemonicEmpowermentAction(ai); }

    static Action* soulshatter(PlayerbotAI* botAI) { return new CastSoulShatterAction(botAI); }
    static Action* unending_resolve(PlayerbotAI* botAI) { return new CastUnendingResolveAction(botAI); }
    static Action* fel_flame(PlayerbotAI* botAI) { return new CastFelFlameAction(botAI); }
    static Action* dark_intent(PlayerbotAI* botAI) { return new CastDarkIntentAction(botAI); }
    static Action* twilight_ward(PlayerbotAI* botAI) { return new CastTwilightWardAction(botAI); }
    static Action* health_funnel(PlayerbotAI* botAI) { return new CastHealthFunnelAction(botAI); }

    static Action* corruption(PlayerbotAI* botAI) { return new CastCorruptionAction(botAI); }
    static Action* curse_enfeeblement(PlayerbotAI* botAI) { return new CastCurseOfEnfeeblementAction(botAI); }
    static Action* curse_elements(PlayerbotAI* botAI) { return new CastCurseOfElementsAction(botAI); }
    static Action* corruption_on_attacker(PlayerbotAI* botAI) { return new CastCorruptionOnAttackerAction(botAI); }
    static Action* siphon_life(PlayerbotAI* botAI) { return new CastSiphonLifeAction(botAI); }
    static Action* siphon_life_on_attacker(PlayerbotAI* botAI) { return new CastSiphonLifeOnAttackerAction(botAI); }
    static Action* fear_on_cc(PlayerbotAI* botAI) { return new CastFearOnCcAction(botAI); }
    static Action* fear(PlayerbotAI* botAI) { return new CastFearAction(botAI); }
    static Action* banish(PlayerbotAI* botAI) { return new CastBanishAction(botAI); }
    static Action* banish_on_cc(PlayerbotAI* botAI) { return new CastBanishAction(botAI); }
    static Action* shadow_bolt(PlayerbotAI* botAI) { return new CastShadowBoltAction(botAI); }
    static Action* drain_life(PlayerbotAI* botAI) { return new CastDrainLifeAction(botAI); }
    static Action* create_healthstone(PlayerbotAI* botAI) { return new CastCreateHealthstoneAction(botAI); }

    // -- talents
    static Action* dark_regeneration(PlayerbotAI* botAI) { return new CastDarkRegenerationAction(botAI); }
    static Action* demonic_breath(PlayerbotAI* botAI) { return new CastDemonicBreathAction(botAI); }
    static Action* mortal_coil(PlayerbotAI* botAI) { return new CastMortalCoilAction(botAI); }
    static Action* shadowfury(PlayerbotAI* botAI) { return new CastShadowFuryAction(botAI); }
    static Action* soul_link(PlayerbotAI* botAI) { return new CastSoulLinkAction(botAI); }
    static Action* sacrificial_pact(PlayerbotAI* botAI) { return new CastSacrificialPactAction(botAI); }
    static Action* dark_bargain(PlayerbotAI* botAI) { return new CastDarkBarginAction(botAI); }
    static Action* blood_horror(PlayerbotAI* botAI) { return new CastBloodHorrorAction(botAI); }
    static Action* grimoire_of_supremacy(PlayerbotAI* botAI) { return new CastGrmoireOfSupremacyAction(botAI); }
    static Action* grimoire_of_sacrifice(PlayerbotAI* botAI) { return new CastGrimoireOfSacrificeAction(botAI); }
    static Action* mannoroths_fury(PlayerbotAI* botAI) { return new CastMannorothFuryAction(botAI); }

    // -- affli
    static Action* soul_burn(PlayerbotAI* botAI) { return new CastSoulBurnAction(botAI); }
    static Action* dark_soul_misery(PlayerbotAI* botAI) { return new CastDarkSoulMiseryAction(botAI); }
    static Action* malefic_grasp(PlayerbotAI* botAI) { return new CastMaleficGraspAction(botAI); }
    static Action* curse_of_agony(PlayerbotAI* botAI) { return new CastCurseOfAgonyAction(botAI); }
    static Action* curse_of_agony_on_attacker(PlayerbotAI* botAI) { return new CastCurseOfAgonyOnAttackerAction(botAI); }
    static Action* drain_soul(PlayerbotAI* botAI) { return new CastDrainSoulAction(botAI); }
    static Action* seed_of_corruption(PlayerbotAI* botAI) { return new CastSeedOfCorruptionAction(botAI); }
    static Action* seed_of_corruption_on_attacker(PlayerbotAI* botAI) { return new CastSeedOfCorruptionOnAttackerAction(botAI); }
    static Action* unstable_affliction(PlayerbotAI* ai) { return new CastUnstableAfflictionAction(ai); }
    static Action* unstable_affliction_on_attacker(PlayerbotAI* ai) { return new CastUnstableAfflictionOnAttackerAction(ai); }
    static Action* haunt(PlayerbotAI* ai) { return new CastHauntAction(ai); }

    // -- demono
    static Action* soul_fire(PlayerbotAI* ai) { return new CastSoulFireAction(ai); }
    static Action* dark_soul_knowledge(PlayerbotAI* botAI) { return new CastDarkSoulKnowledgeAction(botAI); }
    static Action* hellfire(PlayerbotAI* botAI) { return new CastHellfireAction(botAI); }
    static Action* hand_of_guldan(PlayerbotAI* botAI) { return new CastHandOfGuldanAction(botAI); }

    static Action* metamorphosis(PlayerbotAI* ai) { return new CastMetamorphosisAction(ai); }
    static Action* metamorphosis_chaos_wave(PlayerbotAI* botAI) { return new CastMetaChaosWaveAction(botAI); }
    static Action* metamorphosis_doom(PlayerbotAI* botAI) { return new CastMetaDoomAction(botAI); }
    static Action* metamorphosis_immolation(PlayerbotAI* botAI) { return new CastMetaImmolationAction(botAI); }
    static Action* metamorphosis_touch_chaos(PlayerbotAI* botAI) { return new CastMetaChaosAction(botAI); }
    static Action* metamorphosis_void_ray(PlayerbotAI* botAI) { return new CastMetaVoidRayAction(botAI); }

    // -- destruc
    static Action* dark_soul_instability(PlayerbotAI* botAI) { return new CastDarkSoulInstabilityAction(botAI); }
    static Action* chaos_bolt(PlayerbotAI* botAI) { return new CastChaosBoltAction(botAI); }
    static Action* fire_and_brimstone(PlayerbotAI* botAI) { return new CastFireAndBrimeStoneAction(botAI); }
    static Action* flames_of_xoroth(PlayerbotAI* botAI) { return new CastFlamesOfXorothAction(botAI); }
    static Action* shadowburn(PlayerbotAI* botAI) { return new CastShadowBurnAction(botAI); }
    static Action* conflagrate(PlayerbotAI* botAI) { return new CastConflagrateAction(botAI); }
    static Action* incinerate(PlayerbotAI* botAI) { return new CastIncinerateAction(botAI); }
    static Action* immolate(PlayerbotAI* botAI) { return new CastImmolateAction(botAI); }
    static Action* rain_of_fire(PlayerbotAI* botAI) { return new CastRainOfFireAction(botAI); }
    static Action* life_tap(PlayerbotAI* botAI) { return new CastLifeTapAction(botAI); }
    static Action* immolate_on_attacker(PlayerbotAI* ai) { return new CastImmolateOnAttackerAction(ai); }
};

WarlockAiObjectContext::WarlockAiObjectContext(PlayerbotAI* botAI) : AiObjectContext(botAI)
{
    strategyContexts.Add(new WarlockStrategyFactoryInternal());
    strategyContexts.Add(new WarlockCombatStrategyFactoryInternal());
    strategyContexts.Add(new NonCombatBuffStrategyFactoryInternal());
    actionContexts.Add(new WarlockAiObjectContextInternal());
    triggerContexts.Add(new WarlockTriggerFactoryInternal());
}
