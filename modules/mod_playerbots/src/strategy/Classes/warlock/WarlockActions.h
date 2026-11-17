/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_WARLOCKACTIONS_H
#define _PLAYERBOT_WARLOCKACTIONS_H

#include "GenericSpellActions.h"
#include "Playerbots.h"

class PlayerbotAI;
class Unit;

// -- TALENTS
SPELL_ACTION(CastDarkRegenerationAction, "dark regeneration");
SPELL_ACTION(CastDemonicBreathAction, "demonic breath");
SPELL_ACTION(CastMortalCoilAction, "mortal coil");
BUFF_ACTION(CastSoulLinkAction, "soul link");
SPELL_ACTION(CastSacrificialPactAction, "sacrificial pact");
SPELL_ACTION(CastDarkBarginAction, "dark bargain");
SPELL_ACTION(CastBloodHorrorAction, "blood horror");
SPELL_ACTION(CastGrmoireOfSupremacyAction, "grimoire of supremacy");
SPELL_ACTION(CastGrimoireOfSacrificeAction, "grimoire of sacrifice");
SPELL_ACTION(CastMannorothFuryAction, "mannoroth's fury");
class CastShadowFuryAction : public CastSpellAction
{
public:
    CastShadowFuryAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "shadowfury") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};
// -- GENERIC
SPELL_ACTION(CastHealthFunnelAction, "health funnel");
SPELL_ACTION(CastSoulShatterAction, "soulshatter");
SPELL_ACTION(CastUnendingResolveAction, "unending resolve");
SPELL_ACTION(CastFelFlameAction, "fel flame");
BUFF_ACTION(CastDarkIntentAction, "dark intent");
SPELL_ACTION(CastTwilightWardAction, "twilight ward");

BEGIN_RANGED_SPELL_ACTION(CastShadowBoltAction, "shadow bolt")
END_SPELL_ACTION()

class CastDrainSoulAction : public CastSpellAction
{
public:
    CastDrainSoulAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "drain soul") {}

    bool isUseful() override;
};

class CastDrainLifeAction : public CastSpellAction
{
public:
    CastDrainLifeAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "drain life") {}
};

DEBUFF_ACTION(CastCurseOfWeaknessAction, "curse of weakness");
DEBUFF_ACTION(CastCurseOfElementsAction, "curse of the elements");
DEBUFF_ACTION(CastCurseOfEnfeeblementAction, "curse of enfeeblement");

class CastCorruptionAction : public CastDebuffSpellAction
{
public:
    CastCorruptionAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "corruption", true) {}
};
class CastCorruptionOnAttackerAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastCorruptionOnAttackerAction(PlayerbotAI* botAI) : CastDebuffSpellOnAttackerAction(botAI, "corruption", true) {}
};
class CastCreateHealthstoneAction : public CastBuffSpellAction
{
public:
    CastCreateHealthstoneAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "create healthstone") {}
};

class CastBanishAction : public CastBuffSpellAction
{
public:
    CastBanishAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "banish on cc") {}

    Value<Unit*>* GetTargetValue() override;
    bool Execute(Event event) override;
};
class CastRainOfFireAction : public CastSpellAction
{
public:
    CastRainOfFireAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "rain of fire") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};
class CastFearAction : public CastDebuffSpellAction
{
public:
    CastFearAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "fear") {}
};

class CastFearOnCcAction : public CastBuffSpellAction
{
public:
    CastFearOnCcAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "fear on cc") {}

    Value<Unit*>* GetTargetValue() override;
    bool Execute(Event event) override;
    bool isPossible() override;
    bool isUseful() override;
};

class CastLifeTapAction : public CastSpellAction
{
public:
    CastLifeTapAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "life tap") {}

    std::string const GetTargetName() override { return "self target"; }
    bool isUseful() override;
};

class CastSiphonLifeAction : public CastDebuffSpellAction
{
public:
    CastSiphonLifeAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "siphon life", true) {}
};

class CastSiphonLifeOnAttackerAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastSiphonLifeOnAttackerAction(PlayerbotAI* botAI) : CastDebuffSpellOnAttackerAction(botAI, "siphon life") {}
};
class CastDemonicEmpowermentAction : public CastBuffSpellAction
{
public:
    CastDemonicEmpowermentAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "demonic empowerment") {}
    std::string const GetTargetName() override { return "pet target"; }
};

// -- Affliction
SPELL_ACTION(CastSoulBurnAction, "soul burn");
SPELL_ACTION(CastDarkSoulMiseryAction, "dark soul: misery");
SPELL_ACTION(CastMaleficGraspAction, "malefic grasp");
class CastCurseOfAgonyAction : public CastDebuffSpellAction
{
public:
    CastCurseOfAgonyAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "agony", true) {}
};
class CastUnstableAfflictionAction : public CastDebuffSpellAction
{
public:
    CastUnstableAfflictionAction(PlayerbotAI* ai) : CastDebuffSpellAction(ai, "unstable affliction", true) {}
};
class CastUnstableAfflictionOnAttackerAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastUnstableAfflictionOnAttackerAction(PlayerbotAI* ai)
        : CastDebuffSpellOnAttackerAction(ai, "unstable affliction", true)
    {
    }
};
class CastHauntAction : public CastDebuffSpellAction
{
public:
    CastHauntAction(PlayerbotAI* ai) : CastDebuffSpellAction(ai, "haunt", true, 0) {}
    bool isUseful() override
    {
        return CastDebuffSpellAction::isUseful() && !botAI->HasAura("haunt", GetTarget(), true, false);
    }
};
class CastSeedOfCorruptionAction : public CastDebuffSpellAction
{
public:
    CastSeedOfCorruptionAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "seed of corruption", true, 0) {}
    bool isUseful() override
    {
        return CastDebuffSpellAction::isUseful() && !botAI->HasAura("corruption", GetTarget(), false, true);
    }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

class CastSeedOfCorruptionOnAttackerAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastSeedOfCorruptionOnAttackerAction(PlayerbotAI* botAI)
        : CastDebuffSpellOnAttackerAction(botAI, "seed of corruption", true, 0)
    {
    }
    bool isUseful() override
    {
        return CastDebuffSpellOnAttackerAction::isUseful() && !botAI->HasAura("corruption", GetTarget(), false, true);
    }
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};
class CastCurseOfAgonyOnAttackerAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastCurseOfAgonyOnAttackerAction(PlayerbotAI* botAI)
        : CastDebuffSpellOnAttackerAction(botAI, "agony", true)
    {
    }
};

// -- summons
BUFF_ACTION(CastSummonVoidwalkerAction, "summon voidwalker");
BUFF_ACTION(CastSummonFelguardAction, "summon felguard");
BUFF_ACTION(CastSummonFelhunterAction, "summon felhunter");
BUFF_ACTION(CastSummonImpAction, "summon imp");
BUFF_ACTION(CastSummonSuccubusAction, "summon succubus");
BUFF_ACTION(CastSummonDoomGuardAction, "summon doomguard");
BUFF_ACTION(CastSummonFelImpAction, "summon fel imp");
BUFF_ACTION(CastSummonVoidLordAction, "summon voidlord");
BUFF_ACTION(CastSummonShivarraAction, "summon shivarra");
BUFF_ACTION(CastSummonObserverAction, "summon observer");
BUFF_ACTION(CastSummonTerrorGuardAction, "summon terrorguard");
BUFF_ACTION(CastSummonWrathGuardGuardAction, "summon wrathguard");

class CastSummonInfernalAction : public CastSpellAction
{
public:
    CastSummonInfernalAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "summon infernal") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};
class CastSummonAbyssalAction : public CastSpellAction
{
public:
    CastSummonAbyssalAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "summon abyssal") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

// -- Demono
SPELL_ACTION(CastDarkSoulKnowledgeAction, "dark soul: knowledge");
class CastHellfireAction : public CastMeleeSpellAction
{
public:
    CastHellfireAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "hellfire")
    {
        range = ATTACK_DISTANCE;
    }
};
class CastMetaImmolationAction : public CastMeleeSpellAction
{
public:
    CastMetaImmolationAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "immolation aura")
    {
        range = ATTACK_DISTANCE;
    }
};
class CastHandOfGuldanAction : public CastSpellAction
{
public:
    CastHandOfGuldanAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "hand of guldan") {}
    bool Execute(Event event) override;
};
SPELL_ACTION(CastSoulFireAction, "soul fire");

BUFF_ACTION(CastMetamorphosisAction, "metamorphosis");
SPELL_ACTION(CastMetaChaosWaveAction, "chaos wave");
DEBUFF_ACTION(CastMetaDoomAction, "doom");
SPELL_ACTION(CastMetaChaosAction, "touch of chaos");
SPELL_ACTION(CastMetaVoidRayAction, "void ray");


// -- Destru
SPELL_ACTION(CastDarkSoulInstabilityAction, "dark soul: instability");
SPELL_ACTION(CastFireAndBrimeStoneAction, "fire and brimstone");
SPELL_ACTION(CastFlamesOfXorothAction, "flames of xoroth");
SPELL_ACTION(CastShadowBurnAction, "shadowburn");
class CastChaosBoltAction : public CastSpellAction
{
public:
    CastChaosBoltAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "chaos bolt") {}
    bool isUseful() override;
};
class CastImmolateAction : public CastDebuffSpellAction
{
public:
    CastImmolateAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "immolate", true) {}
    bool Execute(Event event) override;
};
class CastImmolateOnAttackerAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastImmolateOnAttackerAction(PlayerbotAI* botAI) : CastDebuffSpellOnAttackerAction(botAI, "immolate", true) {}
};
class CastConflagrateAction : public CastSpellAction
{
public:
    CastConflagrateAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "conflagrate") {}
    bool Execute(Event event) override;
};
class CastIncinerateAction : public CastSpellAction
{
public:
    CastIncinerateAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "incinerate") {}
    bool Execute(Event event) override;
};
#endif
