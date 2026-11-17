/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_MAGEACTIONS_H
#define _PLAYERBOT_MAGEACTIONS_H

#include "GenericSpellActions.h"
#include "SharedDefines.h"

class PlayerbotAI;

BUFF_ACTION(CastFireWardAction, "fire ward");
BUFF_ACTION(CastFrostWardAction, "frost ward");

class CastFireballAction : public CastSpellAction
{
public:
    CastFireballAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "fireball") {}
};

class CastScorchAction : public CastSpellAction
{
public:
    CastScorchAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "scorch") {}
};

class CastFireBlastAction : public CastSpellAction
{
public:
    CastFireBlastAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "fire blast") {}
};

class CastArcaneBlastAction : public CastBuffSpellAction
{
public:
    CastArcaneBlastAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "arcane blast") {}
    std::string const GetTargetName() override { return "current target"; }
};

class CastSlowAction : public CastSpellAction
{
public:
    CastSlowAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "slow") {}
    bool isUseful() override;
};

class CastFrostjawAction : public CastSpellAction
{
public:
    CastFrostjawAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "frostjaw") {}
};

class CastArcaneBarrageAction : public CastSpellAction
{
public:
    CastArcaneBarrageAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane barrage") {}
};

class CastArcaneMissilesAction : public CastSpellAction
{
public:
    CastArcaneMissilesAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane missiles") {}
};

class CastArcaneExplosionAction : public CastSpellAction
{
public:
    CastArcaneExplosionAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "arcane explosion") {}
};

class CastPyroblastAction : public CastSpellAction
{
public:
    CastPyroblastAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "pyroblast") {}
};

class CastFlamestrikeAction : public CastDebuffSpellAction
{
public:
    CastFlamestrikeAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "flamestrike", true, 0.0f) {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

class CastFrostNovaAction : public CastSpellAction
{
public:
    CastFrostNovaAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "frost nova") {}
    bool isUseful() override;
};

class CastBlinkAction : public CastSpellAction
{
public:
    CastBlinkAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "blink") {}
    bool isUseful() override;
};

class CastFrostboltAction : public CastSpellAction
{
public:
    CastFrostboltAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "frostbolt") {}
};

class CastFrostfireBoltAction : public CastSpellAction
{
public:
    CastFrostfireBoltAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "frostfire bolt") {}
};

class CastIceLanceAction : public CastSpellAction
{
public:
    CastIceLanceAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "ice lance") {}
};

class CastDeepFreezeAction : public CastSpellAction
{
public:
    CastDeepFreezeAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "deep freeze") {}
    bool isUseful() override;
};

class CastBlizzardAction : public CastSpellAction
{
public:
    CastBlizzardAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "blizzard") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

class CastConeOfColdAction : public CastSpellAction
{
public:
    CastConeOfColdAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "cone of cold") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

class CastFrozenOrbAction : public CastCureSpellAction
{
public:
    CastFrozenOrbAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "frozen orb") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

class CastArcaneBrillanceAction : public CastBuffSpellAction
{
public:
    CastArcaneBrillanceAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "arcane brilliance") {}
};

class CastRemoveCurseAction : public CastCureSpellAction
{
public:
    CastRemoveCurseAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "remove curse") {}
};

class CastRemoveLesserCurseAction : public CastCureSpellAction
{
public:
    CastRemoveLesserCurseAction(PlayerbotAI* botAI) : CastCureSpellAction(botAI, "remove lesser curse") {}
};

class CastIcyVeinsAction : public CastBuffSpellAction
{
public:
    CastIcyVeinsAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "icy veins") {}
};

class CastFingersOfFrostAction : public CastBuffSpellAction
{
public:
    CastFingersOfFrostAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "fingers of frost") {}
};

class CastColdSnapAction : public CastBuffSpellAction
{
public:
    CastColdSnapAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "cold snap") {}
};

class CastIceBarrierAction : public CastBuffSpellAction
{
public:
    CastIceBarrierAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "ice barrier") {}
};

class CastSummonWaterElementalAction : public CastBuffSpellAction
{
public:
    CastSummonWaterElementalAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "summon water elemental") {}
};

class CastCombustionAction : public CastBuffSpellAction
{
public:
    CastCombustionAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "combustion") {}
};

BEGIN_SPELL_ACTION(CastCounterspellAction, "counterspell")
END_SPELL_ACTION()

class CastRemoveCurseOnPartyAction : public CurePartyMemberAction
{
public:
    CastRemoveCurseOnPartyAction(PlayerbotAI* botAI) : CurePartyMemberAction(botAI, "remove curse", DISPEL_CURSE) {}
};

class CastRemoveLesserCurseOnPartyAction : public CurePartyMemberAction
{
public:
    CastRemoveLesserCurseOnPartyAction(PlayerbotAI* botAI)
        : CurePartyMemberAction(botAI, "remove lesser curse", DISPEL_CURSE)
    {
    }
};

class CastConjureFoodAction : public CastBuffSpellAction
{
public:
    CastConjureFoodAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "conjure food") {}
};

class CastConjureWaterAction : public CastBuffSpellAction
{
public:
    CastConjureWaterAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "conjure water") {}
};

class CastIceBlockAction : public CastBuffSpellAction
{
public:
    CastIceBlockAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "ice block") {}
};

class CastMoltenArmorAction : public CastBuffSpellAction
{
public:
    CastMoltenArmorAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "molten armor") {}
};

class CastMageArmorAction : public CastBuffSpellAction
{
public:
    CastMageArmorAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "mage armor") {}
};

class CastIceArmorAction : public CastBuffSpellAction
{
public:
    CastIceArmorAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "ice armor") {}
};

class CastFrostArmorAction : public CastBuffSpellAction
{
public:
    CastFrostArmorAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "frost armor") {}
};

class CastPolymorphAction : public CastBuffSpellAction
{
public:
    CastPolymorphAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "polymorph") {}
    Value<Unit*>* GetTargetValue() override;
};

class CastSpellstealAction : public CastSpellAction
{
public:
    CastSpellstealAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "spellsteal") {}
};

class CastLivingBombAction : public CastDebuffSpellAction
{
public:
    CastLivingBombAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "living bomb", true) {}
};

class CastFrostBombAction : public CastDebuffSpellAction
{
public:
    CastFrostBombAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "frost bomb", true) {}
};

class CastNetherTempestAction : public CastDebuffSpellAction
{
public:
    CastNetherTempestAction(PlayerbotAI* botAI) : CastDebuffSpellAction(botAI, "nether tempest", true) {}
};

class CastLivingBombOnAttackersAction : public CastDebuffSpellOnAttackerAction
{
public:
    CastLivingBombOnAttackersAction(PlayerbotAI* botAI) : CastDebuffSpellOnAttackerAction(botAI, "living bomb", true) {}
};

class CastDragonsBreathAction : public CastSpellAction
{
public:
    CastDragonsBreathAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "dragon's breath") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

class CastInfernoBlastAction : public CastSpellAction
{
public:
    CastInfernoBlastAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "inferno blast") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Single; }
};

class CastBlastWaveAction : public CastSpellAction
{
public:
    CastBlastWaveAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "blast wave") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool isUseful() override;
};

class CastInvisibilityAction : public CastBuffSpellAction
{
public:
    CastInvisibilityAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "invisibility") {}
};

class CastRuneOfPowerAction : public CastSpellAction
{
public:
    CastRuneOfPowerAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "rune of power") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
    bool Execute(Event event) override;
};

class CastEvocationAction : public CastSpellAction
{
public:
    CastEvocationAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "evocation") {}
    std::string const GetTargetName() override { return "self target"; }
};

class CastCounterspellOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
{
public:
    CastCounterspellOnEnemyHealerAction(PlayerbotAI* botAI) : CastSpellOnEnemyHealerAction(botAI, "counterspell") {}
};

class CastArcanePowerAction : public CastBuffSpellAction
{
public:
    CastArcanePowerAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "arcane power") {}
};

class CastPresenceOfMindAction : public CastBuffSpellAction
{
public:
    CastPresenceOfMindAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "presence of mind") {}
};

class CastMirrorImageAction : public CastBuffSpellAction
{
public:
    CastMirrorImageAction(PlayerbotAI* botAI) : CastBuffSpellAction(botAI, "mirror image") {}
};

class CastFocusMagicOnPartyAction : public CastSpellAction
{
public:
    CastFocusMagicOnPartyAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "focus magic") {}
    Unit* GetTarget() override;
};

#endif
