/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "AccountMgr.h"
#include "AreaTrigger.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlePetMgr.h"
#include "Common.h"
#include "CellImpl.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "DynamicObject.h"
#include "Formulas.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GossipDef.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Language.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvPMgr.h"
#include "PathGenerator.h"
#include "Pet.h"
#include "PetBattle.h"
#include "Player.h"
#include "Random.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "ServiceMgr.h"
#include "SharedDefines.h"
#include "SkillExtraItems.h"
#include "SkillDiscovery.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellPackets.h"
#include "SocialMgr.h"
#include "TemporarySummon.h"
#include "Totem.h"
#include "Unit.h"
#include "UpdateData.h"
#include "UpdateFieldFlags.h"
#include "UpdateMask.h"
#include "Util.h"
#include "Vehicle.h"
#include "VMapFactory.h"
#include "World.h"
#include "WorldPacket.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS]=
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectBind,                                     // 11 SPELL_EFFECT_BIND
    &Spell::EffectNULL,                                     // 12 SPELL_EFFECT_PORTAL
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectUnused,                                   // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectUnused,                                   // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectUnused,                                   // 25 SPELL_EFFECT_WEAPON
    &Spell::EffectUnused,                                   // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummonType,                               // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectLeap,                                     // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAreaAura,                            // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectUnused,                                   // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectUnused,                                   // 39 SPELL_EFFECT_LANGUAGE
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectJump,                                     // 41 SPELL_EFFECT_JUMP
    &Spell::EffectJumpDest,                                 // 42 SPELL_EFFECT_JUMP_DEST
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectPlayMovie,                                // 45 SPELL_EFFECT_PLAY_MOVIE
    &Spell::EffectUnused,                                   // 46 SPELL_EFFECT_SPAWN clientside, unit appears as if it was just spawned
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            unused
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectCreateRandomItem,                         // 59 SPELL_EFFECT_CREATE_RANDOM_ITEM       create item base at spell specific loot
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectApplyAreaAura,                            // 65 SPELL_EFFECT_APPLY_AREA_AURA_RAID
    &Spell::EffectRechargeManaGem,                          // 66 SPELL_EFFECT_CREATE_MANA_GEM          (possibly recharge it, misc - is item ID)
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectUntrainTalents,                           // 73 SPELL_EFFECT_UNTRAIN_TALENTS
    &Spell::EffectApplyGlyph,                               // 74 SPELL_EFFECT_APPLY_GLYPH
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             one spell: Create House (TEST)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectGameObjectDamage,                         // 87 SPELL_EFFECT_GAMEOBJECT_DAMAGE
    &Spell::EffectGameObjectRepair,                         // 88 SPELL_EFFECT_GAMEOBJECT_REPAIR
    &Spell::EffectGameObjectSetDestructionState,            // 89 SPELL_EFFECT_GAMEOBJECT_SET_DESTRUCTION_STATE
    &Spell::EffectKillCreditPersonal,                       // 90 SPELL_EFFECT_KILL_CREDIT              Kill credit but only for single person
    &Spell::EffectUnused,                                   // 91 SPELL_EFFECT_THREAT_ALL               one spell: zzOLDBrainwash
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectForceDeselect,                            // 93 SPELL_EFFECT_FORCE_DESELECT
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectCharge,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectCastButtons,                              // 97 SPELL_EFFECT_CAST_BUTTON (totem bar since 3.2.2a)
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    &Spell::EffectSummonObject,                             //105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    &Spell::EffectSummonRaidMarker,                         //106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    &Spell::EffectSummonObject,                             //107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectResurrectPet,                             //109 SPELL_EFFECT_RESURRECT_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectUnused,                                   //112 SPELL_EFFECT_112
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAreaAura,                            //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectUnused,                                   //122 SPELL_EFFECT_122                      unused
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPullTowards,                              //124 SPELL_EFFECT_PULL_TOWARDS
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectStealBeneficialBuff,                      //126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF    spell steal effect?
    &Spell::EffectProspecting,                              //127 SPELL_EFFECT_PROSPECTING              Prospecting spell
    &Spell::EffectApplyAreaAura,                            //128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    &Spell::EffectApplyAreaAura,                            //129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    &Spell::EffectRedirectThreat,                           //130 SPELL_EFFECT_REDIRECT_THREAT
    &Spell::EffectPlaySound,                                //131 SPELL_EFFECT_PLAY_SOUND               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectPlayMusic,                                //132 SPELL_EFFECT_PLAY_MUSIC               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectUnlearnSpecialization,                    //133 SPELL_EFFECT_UNLEARN_SPECIALIZATION   unlearn profession specialization
    &Spell::EffectKillCredit,                               //134 SPELL_EFFECT_KILL_CREDIT              misc value is creature entry
    &Spell::EffectNULL,                                     //135 SPELL_EFFECT_CALL_PET
    &Spell::EffectHealPct,                                  //136 SPELL_EFFECT_HEAL_PCT
    &Spell::EffectEnergizePct,                              //137 SPELL_EFFECT_ENERGIZE_PCT
    &Spell::EffectLeapBack,                                 //138 SPELL_EFFECT_LEAP_BACK                Leap back
    &Spell::EffectQuestClear,                               //139 SPELL_EFFECT_CLEAR_QUEST              Reset quest status (miscValue - quest ID)
    &Spell::EffectForceCast,                                //140 SPELL_EFFECT_FORCE_CAST
    &Spell::EffectForceCast,                                //141 SPELL_EFFECT_FORCE_CAST_WITH_VALUE
    &Spell::EffectTriggerSpell,                             //142 SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE
    &Spell::EffectApplyAreaAura,                            //143 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    &Spell::EffectKnockBack,                                //144 SPELL_EFFECT_KNOCK_BACK_DEST
    &Spell::EffectPullTowards,                              //145 SPELL_EFFECT_PULL_TOWARDS_DEST                      Black Hole Effect
    &Spell::EffectActivateRune,                             //146 SPELL_EFFECT_ACTIVATE_RUNE
    &Spell::EffectQuestFail,                                //147 SPELL_EFFECT_QUEST_FAIL               quest fail
    &Spell::EffectTriggerMissileSpell,                      //148 SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE
    &Spell::EffectChargeDest,                               //149 SPELL_EFFECT_CHARGE_DEST
    &Spell::EffectQuestStart,                               //150 SPELL_EFFECT_QUEST_START
    &Spell::EffectTriggerRitualOfSummoning,                 //151 SPELL_EFFECT_TRIGGER_SPELL_2
    &Spell::EffectSummonRaFFriend,                          //152 SPELL_EFFECT_SUMMON_RAF_FRIEND        summon Refer-a-Friend
    &Spell::EffectCreateTamedPet,                           //153 SPELL_EFFECT_CREATE_TAMED_PET         misc value is creature entry
    &Spell::EffectDiscoverTaxi,                             //154 SPELL_EFFECT_DISCOVER_TAXI
    &Spell::EffectTitanGrip,                                //155 SPELL_EFFECT_TITAN_GRIP Allows you to equip two-handed axes, maces and swords in one hand, but you attack $49152s1% slower than normal.
    &Spell::EffectEnchantItemPrismatic,                     //156 SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC
    &Spell::EffectCreateItem2,                              //157 SPELL_EFFECT_CREATE_ITEM_2            create item or create item template and replace by some randon spell loot item
    &Spell::EffectMilling,                                  //158 SPELL_EFFECT_MILLING                  milling
    &Spell::EffectRenamePet,                                //159 SPELL_EFFECT_ALLOW_RENAME_PET         allow rename pet once again
    &Spell::EffectForcePlayerInteraction,                   //160 SPELL_EFFECT_FORCE_PLAYER_INTERACTION
    &Spell::EffectSpecCount,                                //161 SPELL_EFFECT_TALENT_SPEC_COUNT        second talent spec (learn/revert)
    &Spell::EffectActivateSpec,                             //162 SPELL_EFFECT_TALENT_SPEC_SELECT       activate primary/secondary spec
    &Spell::EffectUnused,                                   //163 SPELL_EFFECT_163  unused
    &Spell::EffectRemoveAura,                               //164 SPELL_EFFECT_REMOVE_AURA
    &Spell::EffectDamageFromMaxHealthPCT,                   //165 SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT
    &Spell::EffectGiveCurrency,                             //166 SPELL_EFFECT_GIVE_CURRENCY
    &Spell::EffectUpdatePlayerPhase,                        //167 SPELL_EFFECT_UPDATE_PLAYER_PHASE
    &Spell::EffectNULL,                                     //168 SPELL_EFFECT_168
    &Spell::EffectDestroyItem,                              //169 SPELL_EFFECT_DESTROY_ITEM
    &Spell::EffectUpdateZoneAurasAndPhases,                 //170 SPELL_EFFECT_UPDATE_ZONE_AURAS_AND_PHASES
    &Spell::EffectSummonPersonalGameObject,                 //171 SPELL_EFFECT_SUMMON_PERSONAL_GAMEOBJECT
    &Spell::EffectResurrectWithAura,                        //172 SPELL_EFFECT_RESURRECT_WITH_AURA
    &Spell::EffectUnlockGuildVaultTab,                      //173 SPELL_EFFECT_UNLOCK_GUILD_VAULT_TAB
    &Spell::EffectApplyAreaAura,                            //174 SPELL_EFFECT_APPLY_AREA_AURA_ENTRY
    &Spell::EffectUnused,                                   //175 SPELL_EFFECT_175  unused
    &Spell::EffectSanctuary,                                //176 SPELL_EFFECT_SANCTUARY2
    &Spell::EffectNULL,                                     //177 SPELL_EFFECT_177
    &Spell::EffectUnused,                                   //178 SPELL_EFFECT_178 unused
    &Spell::EffectCreateAreaTrigger,                        //179 SPELL_EFFECT_CREATE_AREATRIGGER
    &Spell::EffectUnused,                                   //180 SPELL_EFFECT_180 unused
    &Spell::EffectRemoveTalent,                             //181 SPELL_EFFECT_REMOVE_TALENT
    &Spell::EffectNULL,                                     //182 SPELL_EFFECT_182
    &Spell::EffectNULL,                                     //183 SPELL_EFFECT_183
    &Spell::EffectNULL,                                     //184 SPELL_EFFECT_184
    &Spell::EffectNULL,                                     //185 SPELL_EFFECT_185
    &Spell::EffectNULL,                                     //186 SPELL_EFFECT_186
    &Spell::EffectRandomizeDigsite,                         //187 SPELL_EFFECT_RANDOMIZE_DIGSITES
    &Spell::EffectNULL,                                     //188 SPELL_EFFECT_188
    &Spell::EffectBonusRoll,                                //189 SPELL_EFFECT_BONUS_ROLL
    &Spell::EffectNULL,                                     //190 SPELL_EFFECT_190
    &Spell::EffectNULL,                                     //191 SPELL_EFFECT_TELEPORT_TO_DIGSITE
    &Spell::EffectUncageBattlePet,                          //192 SPELL_EFFECT_UNCAGE_BATTLEPET
    &Spell::EffectNULL,                                     //193 SPELL_EFFECT_193
    &Spell::EffectNULL,                                     //194 SPELL_EFFECT_194
    &Spell::EffectPlayScenePackage,                         //195 SPELL_EFFECT_PLAY_SCENE_PACKAGE
    &Spell::EffectNULL,                                     //196 SPELL_EFFECT_196
    &Spell::EffectNULL,                                     //197 SPELL_EFFECT_197
    &Spell::EffectPlaySceneId,                              //198 SPELL_EFFECT_PLAY_SCENE_ID
    &Spell::EffectNULL,                                     //199 SPELL_EFFECT_199
    &Spell::EffectHealBattlePet,                            //200 SPELL_EFFECT_HEAL_BATTLEPET_PCT
    &Spell::EffectBattlePetsUnlock,                         //201 SPELL_EFFECT_UNLOCK_BATTLE_PETS
    &Spell::EffectNULL,                                     //202 SPELL_EFFECT_202
    &Spell::EffectRemoveAura,                               //203 SPELL_EFFECT_REMOVE_AURA_2
    &Spell::EffectNULL,                                     //204 SPELL_EFFECT_204
    &Spell::EffectPlayerChoice,                             //205 SPELL_EFFECT_PLAYER_CHOICE
    &Spell::EffectNULL,                                     //206 SPELL_EFFECT_206
    &Spell::EffectNULL,                                     //207 SPELL_EFFECT_207
    &Spell::EffectNULL,                                     //208 SPELL_EFFECT_208
    &Spell::EffectNULL,                                     //209 SPELL_EFFECT_209
    &Spell::EffectNULL,                                     //210 SPELL_EFFECT_210
    &Spell::EffectNULL,                                     //211 SPELL_EFFECT_211
    &Spell::EffectNULL,                                     //212 SPELL_EFFECT_212
    &Spell::EffectJumpDest,                                 //213 SPELL_EFFECT_JUMP_DEST_2
};

void Spell::EffectNULL(SpellEffIndex /*effIndex*/)
{
    TC_LOG_DEBUG("spells", "WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(SpellEffIndex /*effIndex*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN TRINITY
}

void Spell::EffectResurrectNew(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->IsAlive())
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!unitTarget->IsInWorld())
        return;

    Player* target = unitTarget->ToPlayer();

    if (target->IsRessurectRequested())       // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = m_spellInfo->Effects[effIndex].MiscValue;
    ExecuteLogEffectResurrect(effIndex, target);
    target->SetResurrectRequestData(m_caster, health, mana, 0);
    SendResurrectRequest(target);
}

void Spell::EffectInstaKill(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive())
        return;

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        if (unitTarget->ToPlayer()->GetCommandStatus(CHEAT_GOD))
            return;

    if (m_caster == unitTarget)                              // prevent interrupt message
        finish();

    ObjectGuid targetguid = unitTarget->GetGUID();
    ObjectGuid casterguid = m_caster->GetGUID();

    WorldPacket data(SMSG_SPELLINSTAKILLLOG, 8 + 8 + 4);

    data.WriteBit(casterguid[6]);
    data.WriteBit(targetguid[0]);
    data.WriteBit(casterguid[7]);
    data.WriteBit(targetguid[2]);
    data.WriteBit(casterguid[3]);
    data.WriteBit(casterguid[1]);
    data.WriteBit(casterguid[2]);
    data.WriteBit(casterguid[0]);
    data.WriteBit(casterguid[4]);
    data.WriteBit(targetguid[4]);
    data.WriteBit(targetguid[7]);
    data.WriteBit(targetguid[1]);
    data.WriteBit(targetguid[6]);
    data.WriteBit(targetguid[5]);
    data.WriteBit(casterguid[5]);
    data.WriteBit(targetguid[3]);

    data.WriteByteSeq(casterguid[0]);
    data.WriteByteSeq(targetguid[1]);
    data.WriteByteSeq(casterguid[3]);
    data.WriteByteSeq(casterguid[4]);
    data.WriteByteSeq(casterguid[5]);
    data.WriteByteSeq(casterguid[7]);
    data.WriteByteSeq(targetguid[0]);
    data.WriteByteSeq(casterguid[6]);
    data.WriteByteSeq(targetguid[2]);
    data.WriteByteSeq(targetguid[4]);
    data.WriteByteSeq(casterguid[1]);
    data << int32(m_spellInfo->Id);
    data.WriteByteSeq(targetguid[3]);
    data.WriteByteSeq(casterguid[2]);
    data.WriteByteSeq(targetguid[7]);
    data.WriteByteSeq(targetguid[6]);
    data.WriteByteSeq(targetguid[5]);

    m_caster->SendMessageToSet(&data, true);

    m_caster->DealDamage(unitTarget, unitTarget->GetHealth(), NULL, NODAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void Spell::EffectEnvironmentalDMG(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive())
        return;

    uint32 absorb = 0;
    uint32 resist = 0;

    m_caster->CalcAbsorbResist(unitTarget, m_spellInfo->GetSchoolMask(), SPELL_DIRECT_DAMAGE, damage, &absorb, &resist, m_spellInfo);

    m_caster->SendSpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage, m_spellInfo->GetSchoolMask(), absorb, resist, false, 0, false);
    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        unitTarget->ToPlayer()->EnvironmentalDamage(DAMAGE_FIRE, damage);
}

void Spell::EffectSchoolDMG(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (unitTarget && unitTarget->IsAlive())
    {
        bool applyDirectBonus = true;
        switch (m_spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                // Meteor like spells (divided damage to targets)
                if (m_spellInfo->AttributesCu & SPELL_ATTR0_CU_SHARE_DAMAGE)
                {
                    uint32 count = 0;
                    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        if (ihit->effectMask & (1<<effIndex))
                            ++count;

                        switch (m_spellInfo->Id)
                        {
                            case 107439: // Twilight Barrage
                            case 109203: // Twilight Barrage
                            case 109204: // Twilight Barrage
                            case 109205: // Twilight Barrage
                            case 106401: // Twilight Onslaught
                            case 108862: // Twilight Onslaught
                            case 109226: // Twilight Onslaught
                            case 109227: // Twilight Onslaught
                                ++count; // + ship
                                break;
                        }

                    damage /= count;                    // divide to all targets
                }

                switch (m_spellInfo->Id)                     // better way to check unknown
                {
                    case 105408: // Burning Blood dmg, Madness of Deathwing, Dragon Soul
                    case 109612:
                    case 109613:
                    case 109614:
                        if (m_triggeredByAuraSpell)
                        {
                            if (Aura* const aur = m_caster->GetAura(m_triggeredByAuraSpell->Id))
                                damage *= aur->GetStackAmount();
                        }
                        break;
                    case 105033: // Searing Blood, Yor'sahj The Unsleeping, Dragon Soul
                    case 108356:
                    case 108357:
                    case 108358:
                    case 108218: // Searing Blood, Crimson Globule, Dragon Soul
                    case 108363:
                    {
                        if (!unitTarget)
                            break;

                        float dist = m_caster->GetDistance(unitTarget);

                        if (dist > 10.0f)
                            damage *= dist / 10.0f;
                        break;
                    }
                    // Resonating Crystal dmg, Morchok, Dragon Soul
                    case 103545:
                    case 108572:
                    case 110041:
                    case 110040:
                        if (!unitTarget)
                            break;

                        if (unitTarget->HasAura(103534))
                            damage *= 1.5f;
                        else if (unitTarget->HasAura(103536))
                            damage *= 0.7f;
                        else if (unitTarget->HasAura(103541))
                            damage *= 0.3f;

                        unitTarget->RemoveAura(103534);
                        unitTarget->RemoveAura(103536);
                        unitTarget->RemoveAura(103541);
                        break;
                    // Stomp, Morchok, Dragon Soul
                    case 103414:
                    case 108571:
                    case 109033:
                    case 109034:
                    {
                        if (!unitTarget)
                            break;

                        if (Creature* pMorchok = m_caster->ToCreature())
                        {
                            if ((pMorchok->GetEntry() == 57773) || pMorchok->AI()->GetData(3))
                                damage /= 2;

                            if ((unitTarget->GetGUID() == pMorchok->AI()->GetGUID(1)) || 
                                (unitTarget->GetGUID() == pMorchok->AI()->GetGUID(2)))
                                damage *= 2;
                        }
                        break;
                    }
                    // Ragnaros (Firelands), Molten Inferno
                    case 98518:
                    case 100252:
                    case 100253:
                    case 100254:
                    {
                        int32 min_dmg = 8000;
                        float distance = m_caster->GetDistance(unitTarget);
                        int32 new_dmg = damage - (int32(distance * 4000));
                        damage = std::max(min_dmg, new_dmg);
                        break;
                    }
                    // Decimation Blade, Baleroc
                    case 99353:
                    {
                        uint32 mindmg = 250000;
                        uint32 maxdmg = CalculatePct(unitTarget->GetMaxHealth(), 90);
                        damage = std::max(mindmg, maxdmg);
                        break;
                    }
                    // Torment, Baleroc
                    case 99256:
                    case 100230:
                    case 100231:
                    case 100232:
                        if (Aura* const aur = unitTarget->GetAura(m_spellInfo->Id, m_caster->GetGUID()))
                            damage *= aur->GetStackAmount();
                        break;
                    // Consumption
                    case 28865:
                        damage = (((InstanceMap*)m_caster->GetMap())->GetDifficulty() == REGULAR_DIFFICULTY ? 2750 : 4250);
                        break;
                    // percent from health with min
                    case 25599:                             // Thundercrash
                    {
                        damage = unitTarget->GetHealth() / 2;
                        if (damage < 200)
                            damage = 200;
                        break;
                    }
                    // arcane charge. must only affect demons (also undead?)
                    case 45072:
                    {
                        if (unitTarget->GetCreatureType() != CREATURE_TYPE_DEMON
                            && unitTarget->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                            return;
                        break;
                    }
                    // Gargoyle Strike
                    case 51963:
                    {
                        // about +4 base spell dmg per level
                        damage = (m_caster->GetLevel() - 60) * 4 + 60;
                        break;
                    }
                    case 148008: // Essence of Yu'lon FIXME
                        applyDirectBonus = false;
                        break;
                }
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                if (m_spellInfo->Id == 46968)
                {
                    int32 pct = m_caster->CalculateSpellDamage(unitTarget, m_spellInfo, 2);
                    if (pct > 0)
                        damage += int32(CalculatePct(m_caster->GetTotalAttackPowerValue(BASE_ATTACK), pct));
                    break;
                }
                break;
            }
            case SPELLFAMILY_PRIEST:
            {
                // Improved Mind Blast (Mind Blast in shadow form bonus)
                if (m_caster->GetShapeshiftForm() == FORM_SHADOW && (m_spellInfo->SpellFamilyFlags[0] & 0x00002000))
                {
                    Unit::AuraEffectList const& ImprMindBlast = m_caster->GetAuraEffectsByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                    for (Unit::AuraEffectList::const_iterator i = ImprMindBlast.begin(); i != ImprMindBlast.end(); ++i)
                    {
                        if ((*i)->GetSpellInfo()->SpellFamilyName == SPELLFAMILY_PRIEST &&
                            ((*i)->GetSpellInfo()->SpellIconID == 95))
                        {
                            int chance = (*i)->GetSpellInfo()->Effects[EFFECT_1].CalcValue(m_caster);
                            if (roll_chance_i(chance))
                                // Mind Trauma
                                m_caster->CastSpell(unitTarget, 48301, true, 0);
                            break;
                        }
                    }
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                // Ferocious Bite
                if (m_caster->GetTypeId() == TYPEID_PLAYER && (m_spellInfo->SpellFamilyFlags[0] & 0x000800000) && m_spellInfo->SpellVisual[0] == 6587)
                {
                    // converts each extra point of energy ( up to 25 energy ) into additional damage
                    int32 energy = -(m_caster->ModifyPower(POWER_ENERGY, -25));
                    // 25 energy = 100% more damage
                    AddPct(damage, energy * 4);
                }
                break;
            }
            case SPELLFAMILY_MAGE:
            {
                // Deep Freeze should deal damage to permanently stun-immune targets.
                if (m_spellInfo->Id == 71757)
                    if (unitTarget->GetTypeId() != TYPEID_UNIT || !(unitTarget->IsImmunedToSpellEffect(sSpellMgr->GetSpellInfo(44572), 0)))
                        return;
                break;
            }
        }

        int32 unmitigated = 0;
        if (m_originalCaster && damage > 0)
        {
            if (applyDirectBonus)
                damage = m_originalCaster->SpellDamageBonusDone(unitTarget, m_spellInfo, effIndex, (uint32)damage, SPELL_DIRECT_DAMAGE);
            unmitigated = damage;
            damage = unitTarget->SpellDamageBonusTaken(m_originalCaster, m_spellInfo, effIndex, (uint32)damage, SPELL_DIRECT_DAMAGE);
        }

        m_damage += damage;
        m_unmitigatedDamage += unmitigated;
    }
}

void Spell::EffectDummy(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget && !gameObjTarget && !itemTarget)
        return;

    // Capture point
    if ((GetSpellInfo()->Id == 97388 || GetSpellInfo()->Id == 97372))
        if (Player* player = GetCaster()->ToPlayer())
            if (Battleground* bg = player->GetBattleground())
                bg->EventPlayerClickedOnFlag(player, unitTarget);

    uint32 spell_id = 0;
    int32 bp = 0;
    bool triggered = true;
    SpellCastTargets targets;

    // selection by spell family
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            switch (m_spellInfo->Id)
            {
                case 54041: // Remove Haunted Memento
                {
                    std::list<Creature*> haunts;
                    Trinity::AllCreaturesOfEntryInRange check(unitTarget, 29238, 100.0f); // Scourge Haunt
                    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(unitTarget, haunts, check);
                    unitTarget->VisitNearbyGridObject(100.0f, searcher);
                    for (auto&& haunt : haunts)
                        if (TempSummon* summon = haunt->ToTempSummon())
                            if (summon->GetSummonerGUID() == unitTarget->GetGUID())
                                summon->DespawnOrUnsummon(1);
                    break;
                }
                case 32312: // Move 1 (Chess event AI short distance move)
                case 37388: // Move 2 (Chess event AI long distance move)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // cast generic move spell
                    m_caster->CastSpell(unitTarget, 30012, true); // Move
                    return;
                }
                case 45260: // Karazhan - Chess - Force Player to Kill Bunny
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, 45259, true); // Karazhan - Chess - Kill Chest Bunny
                    return;
                }
                case 37366: // Karazhan - Chess: CLEAR BOARD
                case 39398: // Karazhan - Chess: CLEAR BOARD more (DND)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->ToCreature()->DespawnOrUnsummon(10000);
                    unitTarget->RemoveAurasDueToSpell(32226); // Karazhan - Chess NPC AI, attack timer
                    return;
                }
            }
            break;
        case SPELLFAMILY_PALADIN:
            switch (m_spellInfo->Id)
            {
                case 31789:                                 // Righteous Defense (step 1)
                {
                    // Clear targets for eff 1
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        ihit->effectMask &= ~(1<<1);

                    // not empty (checked), copy
                    Unit::AttackerSet attackers = unitTarget->getAttackers();

                    // remove invalid attackers
                    for (Unit::AttackerSet::iterator aItr = attackers.begin(); aItr != attackers.end();)
                        if (!(*aItr)->IsValidAttackTarget(m_caster))
                            attackers.erase(aItr++);
                        else
                            ++aItr;

                    // selected from list 3
                    uint32 maxTargets = std::min<uint32>(3, attackers.size());
                    for (uint32 i = 0; i < maxTargets; ++i)
                    {
                        Unit* attacker = Trinity::Containers::SelectRandomContainerElement(attackers);
                        AddUnitTarget(attacker, 1 << 1);
                        attackers.erase(attacker);
                    }

                    // now let next effect cast spell at each target.
                    return;
                }
            }
            break;
        case SPELLFAMILY_DEATHKNIGHT:
            switch (m_spellInfo->Id)
            {
                // Raise dead - take reagents and trigger summon spells
                case 48289:
                    if (m_targets.HasDst())
                        targets.SetDst(*m_targets.GetDstPos());
                    spell_id = CalculateDamage(0, NULL);
                    break;
            }
            break;
        case SPELLFAMILY_WARRIOR:
            switch (m_spellInfo->Id)
            {
                case 30012: // Move
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER || !unitTarget || unitTarget->HasAura(39400))
                        return;

                    unitTarget->CastSpell(m_caster, 30253, true);
                    return;
                }
                case 30284: // Change Facing
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, 30270, true);
                    return;
                }
                case 37144: // Move (Chess event player knight move)
                case 37146: // Move (Chess event player pawn move)
                case 37148: // Move (Chess event player queen move)
                case 37151: // Move (Chess event player rook move)
                case 37152: // Move (Chess event player bishop move)
                case 37153: // Move (Chess event player king move)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // cast generic move spell
                    m_caster->CastSpell(unitTarget, 30012, true);
                    return;
                }
            }
            break;
    }

    //spells triggered by dummy effect should not miss
    if (spell_id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);

        if (!spellInfo)
        {
            TC_LOG_ERROR("spells", "EffectDummy of spell %u: triggering unknown spell id %i\n", m_spellInfo->Id, spell_id);
            return;
        }

        targets.SetUnitTarget(unitTarget);
        Spell* spell = new Spell(m_caster, spellInfo, triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE, m_originalCasterGUID, true);
        if (bp) spell->SetSpellValue(SPELLVALUE_BASE_POINT0, bp);
        spell->prepare(&targets);
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr->GetPetAura(m_spellInfo->Id, effIndex))
    {
        m_caster->AddPetAura(petSpell);
        return;
    }

    // normal DB scripted effect
    TC_LOG_DEBUG("spells", "Spell ScriptStart spellid %u in EffectDummy(%u)", m_spellInfo->Id, effIndex);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, uint32(m_spellInfo->Id | (effIndex << 24)), m_caster, unitTarget);

    // Script based implementation. Must be used only for not good for implementation in core spell effects
    // So called only for not proccessed cases
    if (gameObjTarget)
        sScriptMgr->OnDummyEffect(m_caster, m_spellInfo->Id, effIndex, gameObjTarget);
    else if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnDummyEffect(m_caster, m_spellInfo->Id, effIndex, unitTarget->ToCreature());
    else if (itemTarget)
        sScriptMgr->OnDummyEffect(m_caster, m_spellInfo->Id, effIndex, itemTarget);
}

void Spell::EffectTriggerSpell(SpellEffIndex effIndex)
{
    bool onHit = false;
    switch (GetSpellInfo()->Id)
    {
        case 19647: // Spell Lock
        case 115781:// Optical Blast
        case 30451: // Arcane Blast, applies Arcane Charge after the damage dealt
        case 53563: // Beacon of Light
        case 113724:// Ring of Frost
        case 140384:// Ring of Frost
            onHit = true;
            break;
    }
    if (onHit)
    {
        if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
            return;
    }
    else
    {
        if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET && effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
            return;
    }

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;

    /// @todo move those to spell scripts
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_TRIGGER_SPELL
        && effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
    {
        // special cases
        switch (triggered_spell_id)
        {
            // Venomous Effusion, Venoxis
            case 96680:
                if (m_caster->FindNearestCreature(52288, 2.0f))
                    return;
                break;
            // Demonic Empowerment -- succubus
            case 54437:
            {
                unitTarget->RemoveMovementImpairingAuras();
                unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STALKED);
                unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STUN);

                // Cast Lesser Invisibility
                unitTarget->CastSpell(unitTarget, 7870, true);
                return;
            }
            // Brittle Armor - (need add max stack of 24575 Brittle Armor)
            case 29284:
            {
                // Brittle Armor
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(24575);
                if (!spell)
                    return;

                for (uint32 j = 0; j < spell->StackAmount; ++j)
                    m_caster->CastSpell(unitTarget, spell->Id, true);
                return;
            }
            // Mercurial Shield - (need add max stack of 26464 Mercurial Shield)
            case 29286:
            {
                // Mercurial Shield
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(26464);
                if (!spell)
                    return;

                for (uint32 j = 0; j < spell->StackAmount; ++j)
                    m_caster->CastSpell(unitTarget, spell->Id, true);
                return;
            }
            // Cloak of Shadows
            case 35729:
            {
                uint32 dispelMask = SpellInfo::GetDispelMask(DISPEL_ALL);
                Unit::AuraApplicationMap& Auras = unitTarget->GetAppliedAuras();
                for (Unit::AuraApplicationMap::iterator iter = Auras.begin(); iter != Auras.end();)
                {
                    // remove all harmful spells on you...
                    SpellInfo const* spell = iter->second->GetBase()->GetSpellInfo();
                    if ((spell->DmgClass == SPELL_DAMAGE_CLASS_MAGIC // only affect magic spells
                        || ((spell->GetDispelMask()) & dispelMask))
                        // ignore positive and passive auras
                        && !iter->second->IsPositive() && !iter->second->GetBase()->IsPassive())
                    {
                        m_caster->RemoveAura(iter);
                    }
                    else
                        ++iter;
                }
                return;
            }
        }
    }

    // normal case
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);
    if (!spellInfo)
    {
        TC_LOG_DEBUG("spells", "Spell::EffectTriggerSpell spell %u tried to trigger unknown spell %u", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    SpellCastTargets targets;
    if (effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
    {
        if (!spellInfo->NeedsToBeTriggeredByCaster(m_spellInfo))
            return;
        targets.SetUnitTarget(unitTarget);
    }
    else if (effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH)
    {
        if (spellInfo->NeedsToBeTriggeredByCaster(m_spellInfo) && (m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & TARGET_FLAG_UNIT_MASK))
            return;

        if (spellInfo->GetExplicitTargetMask() & TARGET_FLAG_DEST_LOCATION && m_targets.HasDst())
            targets.SetDst(m_targets);

        targets.SetUnitTarget(m_caster);
    }
    else // SPELL_EFFECT_HANDLE_HIT_TARGET
    {
        if (spellInfo->NeedsToBeTriggeredByCaster(m_spellInfo) && m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & TARGET_FLAG_UNIT_MASK)
        {
            targets.SetUnitTarget(unitTarget);
        }
        else
        {
            if (spellInfo->GetExplicitTargetMask() & TARGET_FLAG_DEST_LOCATION)
                targets.SetDst(m_targets);
            targets.SetUnitTarget(m_caster);
        }
    }

    CustomSpellValues values;
    // set basepoints for trigger with value effect
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE)
    {
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, damage);
    }

    // Remove spell cooldown (not category) if spell triggering spell with cooldown and same category
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->CategoryRecoveryTime && spellInfo->CategoryRecoveryTime
        && m_spellInfo->GetCategory() == spellInfo->GetCategory())
        m_caster->ToPlayer()->RemoveSpellCooldown(spellInfo->Id);

    // original caster guid only for GO cast
    m_caster->CastSpell(targets, spellInfo, &values, TRIGGERED_FULL_MASK, NULL, NULL, m_originalCasterGUID);
}

void Spell::EffectTriggerMissileSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET
        && effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;

    // normal case
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);
    if (!spellInfo)
    {
        TC_LOG_DEBUG("spells", "Spell::EffectTriggerMissileSpell spell %u tried to trigger unknown spell %u", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    SpellCastTargets targets;
    if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        if (!spellInfo->NeedsToBeTriggeredByCaster(m_spellInfo))
            return;
        targets.SetUnitTarget(unitTarget);
    }
    else //if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT)
    {
        if (spellInfo->NeedsToBeTriggeredByCaster(m_spellInfo) && (m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & TARGET_FLAG_UNIT_MASK))
            return;

        if (spellInfo->GetExplicitTargetMask() & TARGET_FLAG_DEST_LOCATION)
            targets.SetDst(m_targets);

        targets.SetUnitTarget(m_caster);
    }

    CustomSpellValues values;
    // set basepoints for trigger with value effect
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_TRIGGER_MISSILE_SPELL_WITH_VALUE)
    {
        // maybe need to set value only when basepoints == 0?
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, damage);
    }

    // Remove spell cooldown (not category) if spell triggering spell with cooldown and same category
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->CategoryRecoveryTime && spellInfo->CategoryRecoveryTime
        && m_spellInfo->GetCategory() == spellInfo->GetCategory())
        m_caster->ToPlayer()->RemoveSpellCooldown(spellInfo->Id);

    // original caster guid only for GO cast
    m_caster->CastSpell(targets, spellInfo, &values, TRIGGERED_FULL_MASK, nullptr, nullptr, m_originalCasterGUID);
}

void Spell::EffectForceCast(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;

    // normal case
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!spellInfo)
    {
        TC_LOG_ERROR("spells", "Spell::EffectForceCast of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_FORCE_CAST && damage)
    {
        switch (m_spellInfo->Id)
        {
            case 52588: // Skeletal Gryphon Escape
            case 48598: // Ride Flamebringer Cue
                unitTarget->RemoveAura(damage);
                break;
            case 52463: // Hide In Mine Car
            case 52349: // Overtake
                unitTarget->CastCustomSpell(unitTarget, spellInfo->Id, &damage, NULL, NULL, true, NULL, NULL, m_originalCasterGUID);
                return;
            case 72299: // Malleable Goo Summon Trigger
                unitTarget->CastSpell(unitTarget, spellInfo->Id, true, NULL, NULL, m_originalCasterGUID);
                return;
        }
    }

    CustomSpellValues values;
    // set basepoints for trigger with value effect
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_FORCE_CAST_WITH_VALUE)
    {
        // maybe need to set value only when basepoints == 0?
        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
        values.AddSpellMod(SPELLVALUE_BASE_POINT2, damage);
    }

    SpellCastTargets targets;
    targets.SetUnitTarget(m_caster);

    unitTarget->CastSpell(targets, spellInfo, &values, TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_IGNORE_POWER_AND_REAGENT_COST));
}

void Spell::EffectTriggerRitualOfSummoning(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 triggered_spell_id = m_spellInfo->Effects[effIndex].TriggerSpell;
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(triggered_spell_id);

    if (!spellInfo)
    {
        TC_LOG_ERROR("spells", "EffectTriggerRitualOfSummoning of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    finish();

    m_caster->CastSpell((Unit*)NULL, spellInfo, false);
}

void Spell::EffectJump(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (m_caster->IsInFlight())
        return;

    if (!unitTarget)
        return;

    float x, y, z;
    unitTarget->GetContactPoint(m_caster, x, y, z, CONTACT_DISTANCE);

    float speedXY, speedZ;
    CalculateJumpSpeeds(effIndex, m_caster->GetExactDist2d(x, y), speedXY, speedZ);
    m_caster->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
}

void Spell::EffectJumpDest(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_caster->IsInFlight())
        return;

    if (!m_targets.HasDst())
        return;

    // Init dest coordinates
    float x, y, z;
    destTarget->GetPosition(x, y, z);

    float speedXY, speedZ;
    CalculateJumpSpeeds(effIndex, m_caster->GetExactDist2d(x, y), speedXY, speedZ);
    m_caster->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);

    if (m_spellInfo->HasPvpKnockbackDiminishing())
        if (Player* player = m_caster->ToPlayer())
            player->UpdateKnockbackTime();
}

void Spell::CalculateJumpSpeeds(uint8 i, float dist, float& speedXY, float& speedZ)
{
    float runSpeed = m_caster->IsControlledByPlayer() ? playerBaseMoveSpeed[MOVE_RUN] : baseMoveSpeed[MOVE_RUN];
    if (Creature* creature = m_caster->ToCreature())
        runSpeed *= creature->IsPet() ? 8.0f / 7.0f : creature->GetCreatureTemplate()->speed_run;

    float speedMultiplier = m_spellInfo->Effects[i].ValueMultiplier;
    if (speedMultiplier <= 0.0f)
        speedMultiplier = 1.0f;

    speedXY = std::min(runSpeed * 3.0f * speedMultiplier, 50.0f);

    float duration = dist / speedXY;
    if (m_spellInfo->Speed)
        m_delayMoment = duration * 1000;
    float durationSqr = duration * duration;
    float minHeight = m_spellInfo->Effects[i].MiscValue ? m_spellInfo->Effects[i].MiscValue / 10.0f : 0.5f; // Lower bound is blizzlike
    float maxHeight = m_spellInfo->Effects[i].MiscValueB ? m_spellInfo->Effects[i].MiscValueB / 10.0f : 1000.0f; // Upper bound is unknown
    float height;
    if (durationSqr < minHeight * 8 / Movement::gravity)
        height = minHeight;
    else if (durationSqr > maxHeight * 8 / Movement::gravity)
        height = maxHeight;
    else
        height = Movement::gravity * durationSqr / 8;

    speedZ = std::sqrt(2 * Movement::gravity * height);
}

void Spell::EffectTeleportUnits(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->IsInFlight())
        return;

    // If not exist data for dest location - return
    if (!m_targets.HasDst())
    {
        TC_LOG_ERROR("spells", "Spell::EffectTeleportUnits - does not have destination for spellId %u.", m_spellInfo->Id);
        return;
    }

    // Init dest coordinates
    uint32 mapid = destTarget->GetMapId();
    if (mapid == MAPID_INVALID)
        mapid = unitTarget->GetMapId();

    uint32 options = 0;
    if (mapid == unitTarget->GetMapId())
        options |= TELE_TO_NOT_LEAVE_COMBAT;

    float x, y, z, orientation;
    destTarget->GetPosition(x, y, z, orientation);
    if (!orientation && m_targets.GetUnitTarget())
        orientation = m_targets.GetUnitTarget()->GetOrientation();
    TC_LOG_DEBUG("spells", "Spell::EffectTeleportUnits - teleport unit to %u %f %f %f %f\n", mapid, x, y, z, orientation);

    if (Player* player = unitTarget->ToPlayer())
    {
        // Custom loading screen
        if (uint32 customLoadingScreenId = m_spellInfo->Effects[effIndex].MiscValue)
        {
            WorldPacket data(SMSG_CUSTOM_LOAD_SCREEN);
            data << uint32(customLoadingScreenId);
            player->SendDirectMessage(&data);
        }

        if (unitTarget == m_caster)
            options |= TELE_TO_SPELL;
        player->TeleportTo(mapid, x, y, z, orientation, options);
    }
    else if (mapid == unitTarget->GetMapId())
        unitTarget->NearTeleportTo(x, y, z, orientation, unitTarget == m_caster);
    else
    {
        TC_LOG_ERROR("spells", "Spell::EffectTeleportUnits - spellId %u attempted to teleport creature to a different map.", m_spellInfo->Id);
        return;
    }

    // post effects for TARGET_DEST_DB
    switch (m_spellInfo->Id)
    {
        // Dimensional Ripper - Everlook
        case 23442:
        {
            int32 r = irand(0, 119);
            if (r >= 70)                                  // 7/12 success
            {
                if (r < 100)                              // 4/12 evil twin
                    m_caster->CastSpell(m_caster, 23445, true);
                else                                        // 1/12 fire
                    m_caster->CastSpell(m_caster, 23449, true);
            }
            return;
        }
        // Ultrasafe Transporter: Toshley's Station
        case 36941:
        {
            if (roll_chance_i(50))                        // 50% success
            {
                int32 rand_eff = urand(1, 7);
                switch (rand_eff)
                {
                    case 1:
                        // soul split - evil
                        m_caster->CastSpell(m_caster, 36900, true);
                        break;
                    case 2:
                        // soul split - good
                        m_caster->CastSpell(m_caster, 36901, true);
                        break;
                    case 3:
                        // Increase the size
                        m_caster->CastSpell(m_caster, 36895, true);
                        break;
                    case 4:
                        // Decrease the size
                        m_caster->CastSpell(m_caster, 36893, true);
                        break;
                    case 5:
                    // Transform
                    {
                        if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                            m_caster->CastSpell(m_caster, 36897, true);
                        else
                            m_caster->CastSpell(m_caster, 36899, true);
                        break;
                    }
                    case 6:
                        // chicken
                        m_caster->CastSpell(m_caster, 36940, true);
                        break;
                    case 7:
                        // evil twin
                        m_caster->CastSpell(m_caster, 23445, true);
                        break;
                }
            }
            return;
        }
        // Dimensional Ripper - Area 52
        case 36890:
        {
            if (roll_chance_i(50))                        // 50% success
            {
                int32 rand_eff = urand(1, 4);
                switch (rand_eff)
                {
                    case 1:
                        // soul split - evil
                        m_caster->CastSpell(m_caster, 36900, true);
                        break;
                    case 2:
                        // soul split - good
                        m_caster->CastSpell(m_caster, 36901, true);
                        break;
                    case 3:
                        // Increase the size
                        m_caster->CastSpell(m_caster, 36895, true);
                        break;
                    case 4:
                        // Transform
                    {
                        if (m_caster->ToPlayer()->GetTeam() == ALLIANCE)
                            m_caster->CastSpell(m_caster, 36897, true);
                        else
                            m_caster->CastSpell(m_caster, 36899, true);
                        break;
                    }
                }
            }
            return;
        }
    }
}

void Spell::EffectApplyAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!m_spellAura || !unitTarget)
        return;
    ASSERT(unitTarget == m_spellAura->GetOwner());
    m_spellAura->_ApplyEffectForTargets(effIndex);
}

void Spell::EffectApplyAreaAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!m_spellAura || !unitTarget)
        return;
    ASSERT (unitTarget == m_spellAura->GetOwner());
    m_spellAura->_ApplyEffectForTargets(effIndex);
}

void Spell::EffectUnlearnSpecialization(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    uint32 spellToUnlearn = m_spellInfo->Effects[effIndex].TriggerSpell;

    player->RemoveSpell(spellToUnlearn);

    TC_LOG_DEBUG("spells", "Spell: Player %u has unlearned spell %u from NpcGUID: %u", player->GetGUID().GetCounter(), spellToUnlearn, m_caster->GetGUID().GetCounter());
}

void Spell::EffectPowerDrain(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers powerType = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    if (!unitTarget || !unitTarget->IsAlive() || unitTarget->GetPowerType() != powerType || damage < 0)
        return;

    // add spell damage bonus
    damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, effIndex, uint32(damage), SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, effIndex, uint32(damage), SPELL_DIRECT_DAMAGE);

    int32 newDamage = -(unitTarget->ModifyPower(powerType, -damage));

    float gainMultiplier = 0.0f;

    // Don't restore from self drain
    if (m_caster != unitTarget)
    {
        gainMultiplier = m_spellInfo->Effects[effIndex].CalcValueMultiplier(m_originalCaster, this);

        int32 gain = int32(newDamage* gainMultiplier);

        m_caster->EnergizeBySpell(m_caster, m_spellInfo->Id, gain, powerType);
    }
    ExecuteLogEffectTakeTargetPower(effIndex, unitTarget, powerType, newDamage, gainMultiplier);
}

void Spell::EffectSendEvent(SpellEffIndex effIndex)
{
    // we do not handle a flag dropping or clicking on flag in battleground by sendevent system
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET
        && effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    WorldObject* target = NULL;

    // call events for object target if present
    if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        if (unitTarget)
            target = unitTarget;
        else if (gameObjTarget)
            target = gameObjTarget;
    }
    else // if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT)
    {
        // let's prevent executing effect handler twice in case when spell effect is capable of targeting an object
        // this check was requested by scripters, but it has some downsides:
        // now it's impossible to script (using sEventScripts) a cast which misses all targets
        // or to have an ability to script the moment spell hits dest (in a case when there are object targets present)
        if (m_spellInfo->Effects[effIndex].GetProvidedTargetMask() & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_GAMEOBJECT_MASK))
            return;
        // some spells have no target entries in dbc and they use focus target
        if (focusObject)
            target = focusObject;
        /// @todo there should be a possibility to pass dest target to event script
    }

    TC_LOG_DEBUG("spells", "Spell ScriptStart %u for spellid %u in EffectSendEvent ", m_spellInfo->Effects[effIndex].MiscValue, m_spellInfo->Id);

    if (ZoneScript* zoneScript = m_caster->GetZoneScript())
        zoneScript->ProcessEvent(target, m_spellInfo->Effects[effIndex].MiscValue);
    else if (InstanceScript* instanceScript = m_caster->GetInstanceScript())    // needed in case Player is the caster
        instanceScript->ProcessEvent(target, m_spellInfo->Effects[effIndex].MiscValue);

    m_caster->GetMap()->ScriptsStart(sEventScripts, m_spellInfo->Effects[effIndex].MiscValue, m_caster, target);
}

void Spell::EffectPowerBurn(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers powerType = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    if (!unitTarget || !unitTarget->IsAlive() || damage < 0)
        return;

    if (unitTarget->GetPowerType() != powerType)
        if (m_spellInfo->Id != 108222 || GetPowerIndexByClass(powerType, unitTarget->GetClass()) == MAX_POWERS) // Mana Void (Cobalt Globule)
            return;

    // burn x% of target's mana, up to maximum of 2x% of caster's mana (Mana Burn)
    if (m_spellInfo->Id == 8129)
    {
        if (unitTarget->GetEntry() == 52498 || // Beth'tilac
            unitTarget->GetEntry() == 52530)   // Alysrazor
            return;

        int32 maxDamage = int32(CalculatePct(m_caster->GetMaxPower(powerType), damage * 2));
        damage = int32(CalculatePct(unitTarget->GetMaxPower(powerType), damage));
        damage = std::min(damage, maxDamage);
    }

    int32 newDamage = -(unitTarget->ModifyPower(powerType, -damage));

    // NO - Not a typo - EffectPowerBurn uses effect value multiplier - not effect damage multiplier
    float dmgMultiplier = m_spellInfo->Effects[effIndex].CalcValueMultiplier(m_originalCaster, this);

    // add log data before multiplication (need power amount, not damage)
    ExecuteLogEffectTakeTargetPower(effIndex, unitTarget, powerType, newDamage, 0.0f);

    newDamage = int32(newDamage* dmgMultiplier);

    m_damage += newDamage;
    m_unmitigatedDamage += newDamage;
}

void Spell::EffectHeal(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (unitTarget && unitTarget->IsAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit* caster = m_originalCasterGUID ? m_originalCaster : m_caster;

        // Skip if m_originalCaster not available
        if (!caster)
            return;

        int32 addhealth = damage;

        if (m_spellInfo->Id == 105996) // Essence of Dreams, Ultraxion, Dragon Soul
        {
            uint32 count = 0;
            for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            if (ihit->effectMask & (1 << effIndex))
                ++count;

            addhealth /= count;
        }
        // Vessel of the Naaru (Vial of the Sunwell trinket)
        else if (m_spellInfo->Id == 45064)
        {
            // Amount of heal - depends from stacked Holy Energy
            int damageAmount = 0;
            if (AuraEffect const* aurEff = m_caster->GetAuraEffect(45062, 0))
            {
                damageAmount+= aurEff->GetAmount();
                m_caster->RemoveAurasDueToSpell(45062);
            }

            addhealth += damageAmount;
        }
        // Runic Healing Injector (heal increased by 25% for engineers - 3.2.0 patch change)
        else if (m_spellInfo->Id == 67489)
        {
            if (Player* player = m_caster->ToPlayer())
                if (player->HasSkill(SKILL_ENGINEERING))
                    AddPct(addhealth, 25);
        }
        // Death Pact - return pct of max health to caster
        else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && m_spellInfo->SpellFamilyFlags[0] & 0x00080000)
            addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, effIndex, int32(caster->CountPctFromMaxHealth(damage)), HEAL);
        else if (m_spellValue->HealingBonusDone)
            addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, effIndex, addhealth, HEAL);

        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, effIndex, addhealth, HEAL);

        // Remove Grievious bite if fully healed
        if (unitTarget->HasAura(48920) && (unitTarget->GetHealth() + addhealth >= unitTarget->GetMaxHealth()))
            unitTarget->RemoveAura(48920);

        // Earth Shield
        if (m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN && !GetCaster()->IsTotem())
            if (!IsTriggered() || m_spellInfo->Id == 73921 || m_spellInfo->Id == 73685) // Healing Rain, Unleash Life
                if (AuraEffect const* earthShield = unitTarget->GetAuraEffect(974, EFFECT_1, GetCaster()->GetGUID()))
                    AddPct(addhealth, earthShield->GetAmount());

        Player* _player = unitTarget->ToPlayer();
        // Mogu'Shan Vault
        if (caster && _player && (caster->HasAura(116161) || _player->HasAura(116161))) // SPELL_CROSSED_OVER
        {
            enum Innervation
            {
                SPELL_SPIRIT_INNERVATION_HEALER = 117543,
                SPELL_SPIRIT_INNERVATION_DPS    = 117549
            };

            Roles playerRole = _player->GetRoleForGroup();
            uint32 innervationId = playerRole == ROLES_DPS ? SPELL_SPIRIT_INNERVATION_DPS : SPELL_SPIRIT_INNERVATION_HEALER;

            if (Aura* Innervation = _player->GetAura(innervationId))
            {
                int32 amount = ((addhealth / (float)_player->GetMaxHealth()) * 100) / 5; // every 5% healed by spells is 1% of innervation buff
                if (Innervation->GetEffect(EFFECT_1)->GetAmount() < 25)
                {
                    Innervation->GetEffect(EFFECT_0)->ChangeAmount(Innervation->GetEffect(EFFECT_0)->GetAmount() + (amount / 100.0f)* (playerRole == ROLES_HEALER ? _player->GetMaxPower(POWER_MANA) : 0)); // mana restoration
                    Innervation->GetEffect(EFFECT_1)->ChangeAmount(Innervation->GetEffect(EFFECT_1)->GetAmount() + amount); // size scale
                    
                    if (Innervation->GetId() == SPELL_SPIRIT_INNERVATION_DPS)
                    {
                        Innervation->GetEffect(EFFECT_2)->ChangeAmount(Innervation->GetEffect(EFFECT_2)->GetAmount() + amount); // melee
                        Innervation->GetEffect(EFFECT_3)->ChangeAmount(Innervation->GetEffect(EFFECT_3)->GetAmount() + amount); // crit
                        Innervation->GetEffect(EFFECT_4)->ChangeAmount(Innervation->GetEffect(EFFECT_4)->GetAmount() + amount); // damage
                        Innervation->GetEffect(EFFECT_5)->ChangeAmount(Innervation->GetEffect(EFFECT_5)->GetAmount() + amount); // mastery
                    }

                    //everytime we change the values, refresh duration
                    Innervation->RefreshDuration();
                }
            }

            if (_player->GetHealth() + addhealth >= _player->GetMaxHealth())
                _player->CastSpell(_player, 120717, true);  // Revitalized Spirit
        }

        m_damage -= addhealth;
    }
}

void Spell::EffectHealPct(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive() || damage < 0)
        return;

    // Skip if m_originalCaster not available
    if (!m_originalCaster)
        return;

    uint32 heal = unitTarget->CountPctFromMaxHealth(damage);
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        uint32 max = m_caster->CountPctFromMaxHealth(damage * 2); // May be not blizzlike, but to prevent healing some bosses for millions
        heal = std::min(heal, max);
    }

    heal = m_originalCaster->SpellHealingBonusDone(unitTarget, m_spellInfo, effIndex, heal, HEAL);
    heal = unitTarget->SpellHealingBonusTaken(m_originalCaster, m_spellInfo, effIndex, heal, HEAL);

    m_healing += heal;
}

void Spell::EffectHealMechanical(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive() || damage < 0)
        return;

    // Skip if m_originalCaster not available
    if (!m_originalCaster)
        return;

    uint32 heal = m_originalCaster->SpellHealingBonusDone(unitTarget, m_spellInfo, effIndex, uint32(damage), HEAL);

    m_healing += unitTarget->SpellHealingBonusTaken(m_originalCaster, m_spellInfo, effIndex, heal, HEAL);
}

void Spell::EffectHealthLeech(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive() || damage < 0)
        return;

    switch (m_spellInfo->Id)
    {
        // Siphon Vitality, Warmaster Blackhorn, Dragon Soul
        case 110312:
            damage = unitTarget->CountPctFromCurHealth(20);
            break;  
        default:
            break;
    }

    damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, effIndex, uint32(damage), SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, effIndex, uint32(damage), SPELL_DIRECT_DAMAGE);

    TC_LOG_DEBUG("spells", "HealthLeech :%i", damage);

    float healMultiplier = m_spellInfo->Effects[effIndex].CalcValueMultiplier(m_originalCaster, this);

    m_damage += damage;
    m_unmitigatedDamage += damage;
    // get max possible damage, don't count overkill for heal
    uint32 healthGain = uint32(-unitTarget->GetHealthGain(-damage) * healMultiplier);

    if (m_caster->IsAlive())
    {
        healthGain = m_caster->SpellHealingBonusDone(m_caster, m_spellInfo, effIndex, healthGain, HEAL);
        healthGain = m_caster->SpellHealingBonusTaken(m_caster, m_spellInfo, effIndex, healthGain, HEAL);

        m_caster->HealBySpell(m_caster, m_spellInfo, uint32(healthGain));
    }
}

void Spell::DoCreateItem(uint32 /*i*/, uint32 itemtype)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    uint32 newitemid = itemtype;
    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(newitemid);
    if (!pProto)
    {
        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    // bg reward have some special in code work
    uint32 bgType = 0;
    switch (m_spellInfo->Id)
    {
        case SPELL_AV_MARK_WINNER:
        case SPELL_AV_MARK_LOSER:
            bgType = BATTLEGROUND_AV;
            break;
        case SPELL_WS_MARK_WINNER:
        case SPELL_WS_MARK_LOSER:
            bgType = BATTLEGROUND_WS;
            break;
        case SPELL_AB_MARK_WINNER:
        case SPELL_AB_MARK_LOSER:
            bgType = BATTLEGROUND_AB;
            break;
        default:
            break;
    }

    uint32 num_to_add = damage;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->GetMaxStackSize())
        num_to_add = pProto->GetMaxStackSize();

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count=1;
    // the chance to create additional items
    float additionalCreateChance=0.0f;
    // the maximum number of created additional items
    uint8 additionalMaxNum=0;
    // get the chance and maximum number for creating extra items
    if (CanCreateExtraItems(player, m_spellInfo->Id, additionalCreateChance, additionalMaxNum))
    {
        // roll with this chance till we roll not to create or we create the max num
        while (roll_chance_f(additionalCreateChance) && items_count <= additionalMaxNum)
            ++items_count;
    }

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space);
    if (msg != EQUIP_ERR_OK)
    {
        // convert to possible store amount
        if (msg == EQUIP_ERR_INV_FULL || msg == EQUIP_ERR_ITEM_MAX_COUNT)
            num_to_add -= no_space;
        else
        {
            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError(msg, NULL, NULL, newitemid);
            return;
        }
    }

    if (num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem(dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        // was it successful? return error if not
        if (!pItem)
        {
            player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
            return;
        }

        // set the "Crafted by ..." property of the item
        if (pItem->GetTemplate()->Class != ITEM_CLASS_CONSUMABLE && pItem->GetTemplate()->Class != ITEM_CLASS_QUEST && newitemid != 6265 && newitemid != 6948)
            pItem->SetGuidValue(ITEM_FIELD_CREATOR, player->GetGUID());

        // send info to the client
        player->SendNewItem(pItem, num_to_add, true, bgType == 0);

        if (pProto->Quality > ITEM_QUALITY_EPIC || (pProto->Quality == ITEM_QUALITY_EPIC && pProto->ItemLevel >= MinNewsItemLevel[sWorld->getIntConfig(CONFIG_EXPANSION)]))
            if (Guild* guild = player->GetGuild())
                guild->AddGuildNews(GUILD_NEWS_ITEM_CRAFTED, player->GetGUID(), 0, pProto->ItemId);

        if (Guild* guild = player->GetGuild())
        {
            bool cooking = false;
            auto bounds = sSpellMgr->GetSkillLineAbilityMapBounds(m_spellInfo->Id);
            for (auto itr = bounds.first; itr != bounds.second; ++itr)
                if (itr->second->skillId == SKILL_COOKING || (itr->second->skillId >= SKILL_WAY_OF_THE_GRILL && itr->second->skillId <= SKILL_WAY_OF_THE_BREW))
                    cooking = true;

            if (cooking)
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COOK_RECIPES_GUILD, newitemid, num_to_add, 0, nullptr, player);
            else
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, newitemid, num_to_add, 0, nullptr, player);
        }

        // we succeeded in creating at least one item, so a levelup is possible
        if (bgType == 0)
            player->UpdateCraftSkill(m_spellInfo->Id);

        sScriptMgr->OnItemPickup(player, pItem, ItemPickupSourceType::Spell, m_spellInfo->Id);
    }

/*
    // for battleground marks send by mail if not add all expected
    if (no_space > 0 && bgType)
    {
        if (Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BattlegroundTypeId(bgType)))
            bg->SendRewardMarkByMail(player, newitemid, no_space);
    }
*/
}

void Spell::EffectCreateItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    DoCreateItem(effIndex, m_spellInfo->Effects[effIndex].ItemType);
    ExecuteLogEffectCreateItem(effIndex, m_spellInfo->Effects[effIndex].ItemType);
}

void Spell::EffectCreateItem2(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    uint32 item_id = m_spellInfo->Effects[effIndex].ItemType;

    // Gem Perfection
    if (m_spellInfo->SpellIconID == 1768)
    {
        static std::map<uint32, uint32> const gemPerfectionMap =
        {
            // WotLK
            { 39900, 41432 }, // Bold Bloodstone -> Perfect Bold Bloodstone
            { 39905, 41434 }, // Delicate Bloodstone -> Perfect Delicate Bloodstone
            { 39906, 41433 }, // Bright Bloodstone -> Perfect Bright Bloodstone
            { 39907, 41439 }, // Subtle Bloodstone -> Perfect Subtle Bloodstone
            { 39908, 41435 }, // Flashing Bloodstone -> Perfect Flashing Bloodstone
            { 39909, 41436 }, // Fractured Bloodstone -> Perfect Fractured Bloodstone
            { 39910, 41437 }, // Precise Bloodstone -> Perfect Precise Bloodstone
            { 39911, 41438 }, // Runed Bloodstone -> Perfect Runed Bloodstone
            { 39912, 41444 }, // Brilliant Sun Crystal -> Perfect Brilliant Sun Crystal
            { 39914, 41448 }, // Smooth Sun Crystal -> Perfect Smooth Sun Crystal
            { 39915, 41447 }, // Rigid Sun Crystal -> Perfect Rigid Sun Crystal
            { 39916, 41449 }, // Thick Sun Crystal -> Perfect Thick Sun Crystal
            { 39917, 41445 }, // Mystic Sun Crystal -> Perfect Mystic Sun Crystal
            { 39918, 41446 }, // Quick Sun Crystal -> Perfect Quick Sun Crystal
            { 39919, 41441 }, // Solid Chalcedony -> Perfect Solid Chalcedony
            { 39920, 41442 }, // Sparkling Chalcedony -> Perfect Sparkling Chalcedony
            { 39927, 41440 }, // Lustrous Chalcedony -> Perfect Lustrous Chalcedony
            { 39932, 41443 }, // Stormy Chalcedony -> Perfect Stormy Chalcedony
            { 39933, 41456 }, // Puissant Shadow Crystal -> Perfect Puissant Shadow Crystal
            { 39934, 41461 }, // Sovereign Shadow Crystal -> Perfect Sovereign Shadow Crystal
            { 39935, 41460 }, // Shifting Shadow Crystal -> Perfect Shifting Shadow Crystal
            { 39936, 41452 }, // Glowing Shadow Crystal -> Perfect Glowing Shadow Crystal
            { 39937, 41450 }, // Balanced Shadow Crystal -> Perfect Balanced Shadow Crystal
            { 39938, 41458 }, // Regal Shadow Crystal -> Perfect Regal Shadow Crystal
            { 39939, 41451 }, // Defender's Shadow Crystal -> Perfect Defender's Shadow Crystal
            { 39940, 41453 }, // Guardian's Shadow Crystal -> Perfect Guardian's Shadow Crystal
            { 39941, 41457 }, // Purified Shadow Crystal -> Perfect Purified Shadow Crystal
            { 39942, 41462 }, // Tenuous Shadow Crystal -> Perfect Tenuous Shadow Crystal
            { 39943, 41459 }, // Royal Shadow Crystal -> Perfect Royal Shadow Crystal
            { 39944, 41454 }, // Infused Shadow Crystal -> Perfect Infused Shadow Crystal
            { 39945, 41455 }, // Mysterious Shadow Crystal -> Perfect Mysterious Shadow Crystal
            { 39946, 41494 }, // Luminous Huge Citrine -> Perfect Luminous Huge Citrine
            { 39947, 41492 }, // Inscribed Huge Citrine -> Perfect Inscribed Huge Citrine
            { 39948, 41488 }, // Etched Huge Citrine -> Perfect Etched Huge Citrine
            { 39949, 41483 }, // Champion's Huge Citrine -> Perfect Champion's Huge Citrine
            { 39950, 41499 }, // Resplendent Huge Citrine -> Perfect Resplendent Huge Citrine
            { 39951, 41489 }, // Fierce Huge Citrine -> Perfect Fierce Huge Citrine
            { 39952, 41484 }, // Deadly Huge Citrine -> Perfect Deadly Huge Citrine
            { 39953, 41491 }, // Glinting Huge Citrine -> Perfect Glinting Huge Citrine
            { 39954, 41493 }, // Lucent Huge Citrine -> Perfect Lucent Huge Citrine
            { 39955, 41485 }, // Deft Huge Citrine -> Perfect Deft Huge Citrine
            { 39956, 41495 }, // Potent Huge Citrine -> Perfect Potent Huge Citrine
            { 39957, 41502 }, // Veiled Huge Citrine -> Perfect Veiled Huge Citrine
            { 39958, 41486 }, // Durable Huge Citrine -> Perfect Durable Huge Citrine
            { 39959, 41497 }, // Reckless Huge Citrine -> Perfect Reckless Huge Citrine
            { 39960, 41429 }, // Wicked Huge Citrine -> Perfect Wicked Huge Citrine
            { 39961, 41496 }, // Pristine Huge Citrine -> Perfect Pristine Huge Citrine
            { 39962, 41487 }, // Empowered Huge Citrine -> Perfect Empowered Huge Citrine
            { 39963, 41501 }, // Stark Huge Citrine -> Perfect Stark Huge Citrine
            { 39964, 41500 }, // Stalwart Huge Citrine -> Perfect Stalwart Huge Citrine
            { 39965, 41490 }, // Glimmering Huge Citrine -> Perfect Glimmering Huge Citrine
            { 39966, 41482 }, // Accurate Huge Citrine -> Perfect Accurate Huge Citrine
            { 39967, 41498 }, // Resolute Huge Citrine -> Perfect Resolute Huge Citrine
            { 39968, 41479 }, // Timeless Dark Jade -> Perfect Timeless Dark Jade
            { 39974, 41468 }, // Jagged Dark Jade -> Perfect Jagged Dark Jade
            { 39975, 41481 }, // Vivid Dark Jade -> Perfect Vivid Dark Jade
            { 39976, 41464 }, // Enduring Dark Jade -> Perfect Enduring Dark Jade
            { 39977, 41476 }, // Steady Dark Jade -> Perfect Steady Dark Jade
            { 39978, 41466 }, // Forceful Dark Jade -> Perfect Forceful Dark Jade
            { 39979, 41473 }, // Seer's Dark Jade -> Perfect Seer's Dark Jade
            { 39980, 41470 }, // Misty Dark Jade -> Perfect Misty Dark Jade
            { 39981, 41475 }, // Shining Dark Jade -> Perfect Shining Dark Jade
            { 39982, 41480 }, // Turbid Dark Jade -> Perfect Turbid Dark Jade
            { 39983, 41467 }, // Intricate Dark Jade -> Perfect Intricate Dark Jade
            { 39984, 41463 }, // Dazzling Dark Jade -> Perfect Dazzling Dark Jade
            { 39985, 41477 }, // Sundered Dark Jade -> Perfect Sundered Dark Jade
            { 39986, 41469 }, // Lambent Dark Jade -> Perfect Lambent Dark Jade
            { 39988, 41471 }, // Opaque Dark Jade -> Perfect Opaque Dark Jade
            { 39989, 41465 }, // Energized Dark Jade -> Perfect Energized Dark Jade
            { 39990, 41472 }, // Radiant Dark Jade -> Perfect Radiant Dark Jade
            { 39991, 41478 }, // Tense Dark Jade -> Perfect Tense Dark Jade
            { 39992, 41474 }, // Shattered Dark Jade -> Perfect Shattered Dark Jade
            // Cataclysm
            { 52081, 52176 }, // Bold Carnelian         -> Perfect Bold Carnelian
            { 52082, 52175 }, // Delicate Carnelian     -> Perfect Delicate Carnelian
            { 52083, 52174 }, // Flashing Carnelian     -> Perfect Flashing Carnelian
            { 52084, 52173 }, // Brilliant Carnelian    -> Perfect Brilliant Carnelian
            { 52085, 52172 }, // Precise Carnelian      -> Perfect Precise Carnelian
            { 52086, 52171 }, // Solid Zephyrite        -> Perfect Solid Zephyrite
            { 52087, 52170 }, // Sparkling Zephyrite    -> Perfect Sparkling Zephyrite
            { 52088, 52169 }, // Stormy Zephyrite       -> Perfect Stormy Zephyrite
            { 52089, 52168 }, // Rigid Zephyrite        -> Perfect Rigid Zephyrite
            { 52090, 52167 }, // Subtle Alicite         -> Perfect Subtle Alicite
            { 52091, 52166 }, // Smooth Alicite         -> Perfect Smooth Alicite
            { 52092, 52165 }, // Mystic Alicite         -> Perfect Mystic Alicite
            { 52093, 52164 }, // Quick Alicite          -> Perfect Quick Alicite
            { 52094, 52163 }, // Fractured Alicite      -> Perfect Fractured Alicite
            { 52095, 52162 }, // Sovereign Nightstone   -> Perfect Sovereign Nightstone
            { 52096, 52161 }, // Shifting Nightstone    -> Perfect Shifting Nightstone
            { 52097, 52160 }, // Defender's Nightstone  -> Perfect Defender's Nightstone
            { 52098, 52159 }, // Timeless Nightstone    -> Perfect Timeless Nightstone
            { 52099, 52158 }, // Guardian's Nightstone  -> Perfect Guardian's Nightstone
            { 52100, 52157 }, // Purified Nightstone    -> Perfect Purified Nightstone
            { 52101, 52156 }, // Etched Nightstone      -> Perfect Etched Nightstone
            { 52102, 52155 }, // Glinting Nightstone    -> Perfect Glinting Nightstone
            { 52103, 52154 }, // Retaliating Nightstone -> Perfect Retaliating Nightstone
            { 52104, 52153 }, // Veiled Nightstone      -> Perfect Veiled Nightstone
            { 52105, 52152 }, // Accurate Nightstone    -> Perfect Accurate Nightstone
            { 52106, 52151 }, // Polished Hessonite     -> Perfect Polished Hessonite
            { 52107, 52150 }, // Resolute Hessonite     -> Perfect Resolute Hessonite
            { 52108, 52149 }, // Inscribed Hessonite    -> Perfect Inscribed Hessonite
            { 52109, 52148 }, // Deadly Hessonite       -> Perfect Deadly Hessonite
            { 52110, 52147 }, // Potent Hessonite       -> Perfect Potent Hessonite
            { 52111, 52146 }, // Fierce Hessonite       -> Perfect Fierce Hessonite
            { 52112, 52145 }, // Deft Hessonite         -> Perfect Deft Hessonite
            { 52113, 52144 }, // Reckless Hessonite     -> Perfect Reckless Hessonite
            { 52114, 52143 }, // Skillful Hessonite     -> Perfect Skillful Hessonite
            { 52115, 52142 }, // Adept Hessonite        -> Perfect Adept Hessonite
            { 52116, 52141 }, // Fine Hessonite         -> Perfect Fine Hessonite
            { 52117, 52140 }, // Artful Hessonite       -> Perfect Artful Hessonite
            { 52118, 52139 }, // Keen Hessonite         -> Perfect Keen Hessonite
            { 52119, 52138 }, // Regal Jasper           -> Perfect Regal Jasper
            { 52120, 52137 }, // Nimble Jasper          -> Perfect Nimble Jasper
            { 52121, 52136 }, // Jagged Jasper          -> Perfect Jagged Jasper
            { 52122, 52135 }, // Piercing Jasper        -> Perfect Piercing Jasper
            { 52123, 52134 }, // Steady Jasper          -> Perfect Steady Jasper
            { 52124, 52133 }, // Forceful Jasper        -> Perfect Forceful Jasper
            { 52125, 52132 }, // Lightning Jasper       -> Perfect Lightning Jasper
            { 52126, 52131 }, // Puissant Jasper        -> Perfect Puissant Jasper
            { 52127, 52130 }, // Zen Jasper             -> Perfect Zen Jasper
            { 52128, 52129 }, // Sensei's Jasper        -> Perfect Sensei's Jasper
        };

        auto itr = gemPerfectionMap.find(item_id);
        if (itr != gemPerfectionMap.end() && player->HasSpell(55534) && roll_chance_i(20)) // Gem Perfection
            item_id = itr->second;
    }

    if (item_id)
        DoCreateItem(effIndex, item_id);

    // special case: fake item replaced by generate using spell_loot_template
    if (m_spellInfo->IsLootCrafting())
    {
        if (item_id)
        {
            if (!player->HasItemCount(item_id))
                return;

            // remove reagent
            uint32 count = 1;
            player->DestroyItemCount(item_id, count, true);

            // create some random items
            player->AutoStoreLoot(m_spellInfo->Id, LootTemplates_Spell, ItemPickupSourceType::Spell, m_spellInfo->Id);
        }
        else
            player->AutoStoreLoot(m_spellInfo->Id, LootTemplates_Spell, ItemPickupSourceType::Spell, m_spellInfo->Id);    // create some random items
    }
    /// @todo ExecuteLogEffectCreateItem(i, m_spellInfo->Effects[i].ItemType);
}

void Spell::EffectCreateRandomItem(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = unitTarget->ToPlayer();

    // create some random items
    player->AutoStoreLoot(m_spellInfo->Id, LootTemplates_Spell, ItemPickupSourceType::Spell, m_spellInfo->Id);
    /// @todo ExecuteLogEffectCreateItem(i, m_spellInfo->Effects[i].ItemType);
}

void Spell::EffectPersistentAA(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!m_spellAura)
    {
        Unit* caster = m_caster->GetEntry() == WORLD_TRIGGER ? m_originalCaster : m_caster;
        float radius = m_spellInfo->Effects[effIndex].CalcRadius(caster);

        // Caster not in world, might be spell triggered from aura removal
        if (!caster->IsInWorld())
            return;
        DynamicObject* dynObj = new DynamicObject(false);
        if (!dynObj->CreateDynamicObject(caster->GetMap()->GenerateLowGuid<HighGuid::DynamicObject>(), caster, m_spellInfo, *destTarget, radius, DYNAMIC_OBJECT_AREA_SPELL))
        {
            delete dynObj;
            return;
        }

        if (Aura* aura = Aura::TryCreate(m_spellInfo, MAX_EFFECT_MASK, dynObj, caster, &m_spellValue->EffectBasePoints[0]))
        {
            m_spellAura = aura;
            m_spellAura->_RegisterForTargets();
        }
        else
            return;
    }

    ASSERT(m_spellAura->GetDynobjOwner());
    m_spellAura->_ApplyEffectForTargets(effIndex);
}

void Spell::EffectEnergize(SpellEffIndex effIndex)
{
    // Ugly hack to force energize from Cannon Blast to trigger on caster immediately.
    // Proper fix would require splitting delayed immediate processing from m_delayMoment and making a separate timer for it.
    // Otherwise you either handle both projectile hit and energize effect at projectile hit, or immediately on launch (i.e. on caster hit)...
    if (m_spellInfo->Id == 69399 || m_spellInfo->Id == 70172) // Cannon Blast
    {
        if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
            return;
    }
    else if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers power = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    // Some level depends spells
    int level_multiplier = 0;
    int level_diff = 0;
    switch (m_spellInfo->Id)
    {
        case 9512:                                          // Restore Energy
            level_diff = m_caster->GetLevel() - 40;
            level_multiplier = 2;
            break;
        case 24571:                                         // Blood Fury
            level_diff = m_caster->GetLevel() - 60;
            level_multiplier = 10;
            break;
        case 24532:                                         // Burst of Energy
            level_diff = m_caster->GetLevel() - 60;
            level_multiplier = 4;
            break;
        case 31930:                                         // Judgements of the Wise
        case 63375:                                         // Primal Wisdom
        case 68082:                                         // Glyph of Seal of Command
            damage = int32(CalculatePct(unitTarget->GetCreateMana(), damage));
            break;
        case 67490:                                         // Runic Mana Injector (mana gain increased by 25% for engineers - 3.2.0 patch change)
        {
            if (Player* player = m_caster->ToPlayer())
                if (player->HasSkill(SKILL_ENGINEERING))
                    AddPct(damage, 25);
            break;
        }
        default:
            break;
    }

    if (level_diff > 0)
        damage -= level_multiplier * level_diff;

    if (damage < 0 && power != POWER_ECLIPSE)
        return;

    if (unitTarget->GetMaxPower(power) == 0)
        return;

    damage *= m_spellValue->Multiplier;

    if (m_spellInfo->Id == 100780) // Jab
    {
        damage = 1;
        if (Player* player = m_caster->ToPlayer())
            if (AuraEffect* aurEff = player->GetAuraEffect(103985, 3)) // Stance of the Fierce Tiger
                damage += 1;
    }
    
    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, damage, power);

    // Mad Alchemist's Potion
    if (m_spellInfo->Id == 45051)
    {
        // find elixirs on target
        bool guardianFound = false;
        bool battleFound = false;
        Unit::AuraApplicationMap& Auras = unitTarget->GetAppliedAuras();
        for (Unit::AuraApplicationMap::iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
        {
            uint32 spell_id = itr->second->GetBase()->GetId();
            if (!guardianFound)
                if (sSpellMgr->IsSpellMemberOfSpellGroup(spell_id, SPELL_GROUP_ELIXIR_GUARDIAN))
                    guardianFound = true;
            if (!battleFound)
                if (sSpellMgr->IsSpellMemberOfSpellGroup(spell_id, SPELL_GROUP_ELIXIR_BATTLE))
                    battleFound = true;
            if (battleFound && guardianFound)
                break;
        }

        // get all available elixirs by mask and spell level
        std::set<uint32> avalibleElixirs;
        if (!guardianFound)
            sSpellMgr->GetSetOfSpellsInSpellGroup(SPELL_GROUP_ELIXIR_GUARDIAN, avalibleElixirs);
        if (!battleFound)
            sSpellMgr->GetSetOfSpellsInSpellGroup(SPELL_GROUP_ELIXIR_BATTLE, avalibleElixirs);
        for (std::set<uint32>::iterator itr = avalibleElixirs.begin(); itr != avalibleElixirs.end();)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(*itr);
            if (spellInfo->SpellLevel < m_spellInfo->SpellLevel || spellInfo->SpellLevel > unitTarget->GetLevel())
                avalibleElixirs.erase(itr++);
            else if (sSpellMgr->IsSpellMemberOfSpellGroup(*itr, SPELL_GROUP_ELIXIR_SHATTRATH))
                avalibleElixirs.erase(itr++);
            else if (sSpellMgr->IsSpellMemberOfSpellGroup(*itr, SPELL_GROUP_ELIXIR_UNSTABLE))
                avalibleElixirs.erase(itr++);
            else
                ++itr;
        }

        if (!avalibleElixirs.empty())
        {
            // cast random elixir on target
            m_caster->CastSpell(unitTarget, Trinity::Containers::SelectRandomContainerElement(avalibleElixirs), true, m_CastItem);
        }
    }
}

void Spell::EffectEnergizePct(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue < 0 || m_spellInfo->Effects[effIndex].MiscValue >= int8(MAX_POWERS))
        return;

    Powers power = Powers(m_spellInfo->Effects[effIndex].MiscValue);

    uint32 maxPower = unitTarget->GetMaxPower(power);
    if (maxPower == 0)
        return;

    float pct = damage;
    if (GetSpellInfo()->Id == 123051) // Mana Leech, Mindbender version
        pct = 1.75f;

    uint32 gain = CalculatePct(maxPower, pct);
    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, gain, power);
}

void Spell::SendLoot(ObjectGuid guid, LootType loottype)
{
    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (gameObjTarget)
    {
        // Players shouldn't be able to loot gameobjects that are currently despawned
        if (!gameObjTarget->isSpawned() && !player->IsGameMaster())
        {
            TC_LOG_ERROR("spells", "Possible hacking attempt: Player %s [guid: %u] tried to loot a gameobject [entry: %u id: %u] which is on respawn time without being in GM mode!",
                            player->GetName().c_str(), player->GetGUID().GetCounter(), gameObjTarget->GetEntry(), gameObjTarget->GetGUID().GetCounter());
            return;
        }
        // special case, already has GossipHello inside so return and avoid calling twice
        if (gameObjTarget->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        {
            gameObjTarget->Use(m_caster);
            return;
        }

        if (sScriptMgr->OnGossipHello(player, gameObjTarget))
            return;

        if (gameObjTarget->AI()->OnGossipHello(player, true))
            return;

        switch (gameObjTarget->GetGoType())
        {
            case GAMEOBJECT_TYPE_DOOR:
            case GAMEOBJECT_TYPE_BUTTON:
                gameObjTarget->UseDoorOrButton(0, false, player);
                return;

            case GAMEOBJECT_TYPE_QUESTGIVER:
                player->PrepareGossipMenu(gameObjTarget, gameObjTarget->GetGOInfo()->questgiver.gossipID, true);
                player->SendPreparedGossip(gameObjTarget);
                return;

            case GAMEOBJECT_TYPE_SPELL_FOCUS:
                // triggering linked GO
                if (uint32 trapEntry = gameObjTarget->GetGOInfo()->spellFocus.linkedTrapId)
                    gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);
                return;

            case GAMEOBJECT_TYPE_CHEST:
                /// @todo possible must be moved to loot release (in different from linked triggering)
                if (gameObjTarget->GetGOInfo()->chest.eventId)
                {
                    TC_LOG_DEBUG("spells", "Chest ScriptStart id %u for GO %u", gameObjTarget->GetGOInfo()->chest.eventId, gameObjTarget->GetDBTableGUIDLow());
                    player->GetMap()->ScriptsStart(sEventScripts, gameObjTarget->GetGOInfo()->chest.eventId, player, gameObjTarget);
                }

                // triggering linked GO
                if (uint32 trapEntry = gameObjTarget->GetGOInfo()->chest.linkedTrapId)
                    gameObjTarget->TriggeringLinkedGameObject(trapEntry, m_caster);

                // Don't return, let loots been taken
            default:
                break;
        }
    }

    // Send loot
    player->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("spells", "WORLD: Open Lock - No Player Caster!");
        return;
    }

    Player* player = m_caster->ToPlayer();

    uint32 lockId = 0;
    ObjectGuid guid = ObjectGuid::Empty;

    // Get lockId
    if (gameObjTarget)
    {
        GameObjectTemplate const* goInfo = gameObjTarget->GetGOInfo();
        // Arathi Basin banner opening. /// @todo Verify correctness of this check
        if ((goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune) ||
            (goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.losOK))
        {
            //CanUseBattlegroundObject() already called in CheckCast()
            // in battleground check
            if (Battleground* bg = player->GetBattleground())
            {
                bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND || goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND_2)
        {
            //CanUseBattlegroundObject() already called in CheckCast()
            // in battleground check
            if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetTypeID(true) == BATTLEGROUND_EY)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (m_spellInfo->Id == 1842 && gameObjTarget->GetGOInfo()->type == GAMEOBJECT_TYPE_TRAP && gameObjTarget->GetOwner())
        {
            gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);
            return;
        }
        /// @todo Add script for spell 41920 - Filling, becouse server it freze when use this spell
        // handle outdoor pvp object opening, return true if go was registered for handling
        // these objects must have been spawned by outdoorpvp!
        else if (gameObjTarget->GetGOInfo()->type == GAMEOBJECT_TYPE_GOOBER && sOutdoorPvPMgr->HandleOpenGo(player, gameObjTarget->GetGUID()))
            return;
        lockId = goInfo->GetLockId();
        guid = gameObjTarget->GetGUID();
    }
    else if (itemTarget)
    {
        lockId = itemTarget->GetTemplate()->LockID;
        guid = itemTarget->GetGUID();
    }
    else
    {
        TC_LOG_DEBUG("spells", "WORLD: Open Lock - No GameObject/Item Target!");
        return;
    }

    SkillType skillId = SKILL_NONE;
    int32 reqSkillValue = 0;
    int32 skillValue;

    SpellCastResult res = CanOpenLock(effIndex, lockId, skillId, reqSkillValue, skillValue);
    if (res != SPELL_CAST_OK)
    {
        SendCastResult(res);
        return;
    }

    LootType lootType = LOOT_SKINNING;
    if (skillId == SKILL_MINING || skillId == SKILL_HERBALISM)
        lootType = LOOT_MINING_OR_HERBALISM;

    if (gameObjTarget)
    {
        if (gameObjTarget->isSpawned() || player->IsGameMaster())
            if (gameObjTarget->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED))
                gameObjTarget->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED);

        // idk how it must works, hack for Massive Seaforium Charge at Strands of the Ancients
        if (gameObjTarget->GetGOInfo()->type != GAMEOBJECT_TYPE_TRAP)
            SendLoot(guid, lootType);
    }
    else if (itemTarget)
    {
        itemTarget->SetFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_UNLOCKED);
        itemTarget->SetState(ITEM_CHANGED, player);
    }

    // not allow use skill grow at item base open
    if (!m_CastItem && skillId != SKILL_NONE)
    {
        // update skill if really known
        if (uint32 pureSkillValue = player->GetPureSkillValue(skillId))
        {
            if (gameObjTarget)
            {
                // Allow one skill-up until respawned
                if (!gameObjTarget->IsInSkillupList(player->GetGUID().GetCounter()))
                {
                    player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue, 1, gameObjTarget);
                    gameObjTarget->AddToSkillupList(player->GetGUID().GetCounter());
                }
            }
            else if (itemTarget)
            {
                // Do one skill-up
                player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue);
            }
        }
    }
    ExecuteLogEffectOpenLock(effIndex, gameObjTarget ? (Object*)gameObjTarget : (Object*)itemTarget);
}

void Spell::EffectSummonChangeItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    // applied only to using item
    if (!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if (m_CastItem->GetOwnerGUID() != player->GetGUID())
        return;

    uint32 newitemid = m_spellInfo->Effects[effIndex].ItemType;
    if (!newitemid)
        return;

    uint16 pos = m_CastItem->GetPos();

    Item* pNewItem = Item::CreateItem(newitemid, 1, player);
    if (!pNewItem)
        return;

    for (uint8 j = PERM_ENCHANTMENT_SLOT; j <= TEMP_ENCHANTMENT_SLOT; ++j)
        if (m_CastItem->GetEnchantmentId(EnchantmentSlot(j)))
            pNewItem->SetEnchantment(EnchantmentSlot(j), m_CastItem->GetEnchantmentId(EnchantmentSlot(j)), m_CastItem->GetEnchantmentDuration(EnchantmentSlot(j)), m_CastItem->GetEnchantmentCharges(EnchantmentSlot(j)));

    if (m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) < m_CastItem->GetUInt32Value(ITEM_FIELD_MAX_DURABILITY))
    {
        double lossPercent = 1 - m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) / double(m_CastItem->GetUInt32Value(ITEM_FIELD_MAX_DURABILITY));
        player->DurabilityLoss(pNewItem, lossPercent);
    }

    if (player->IsInventoryPos(pos))
    {
        ItemPosCountVec dest;
        InventoryResult msg = player->CanStoreItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.GetItemTarget())
                m_targets.SetItemTarget(NULL);

            m_CastItem = NULL;

            player->StoreItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsBankPos(pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanBankItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.GetItemTarget())
                m_targets.SetItemTarget(NULL);

            m_CastItem = NULL;

            player->BankItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsEquipmentPos(pos))
    {
        uint16 dest;

        player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

        uint8 msg = player->CanEquipItem(m_CastItem->GetSlot(), dest, pNewItem, true);

        if (msg == EQUIP_ERR_OK || msg == EQUIP_ERR_CLIENT_LOCKED_OUT)
        {
            if (msg == EQUIP_ERR_CLIENT_LOCKED_OUT) dest = EQUIPMENT_SLOT_MAINHAND;

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            if (m_CastItem == m_targets.GetItemTarget())
                m_targets.SetItemTarget(NULL);

            m_CastItem = NULL;

            player->EquipItem(dest, pNewItem, true);
            player->AutoUnequipOffhandIfNeed();
            return;
        }
    }

    // fail
    delete pNewItem;
}

void Spell::EffectProficiency(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* p_target = m_caster->ToPlayer();

    uint32 subClassMask = m_spellInfo->EquippedItemSubClassMask;
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON && !(p_target->GetWeaponProficiency() & subClassMask))
    {
        p_target->AddWeaponProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_WEAPON, p_target->GetWeaponProficiency());
    }
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR && !(p_target->GetArmorProficiency() & subClassMask))
    {
        p_target->AddArmorProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_ARMOR, p_target->GetArmorProficiency());
    }
}

void Spell::EffectSummonType(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 entry = m_spellInfo->Effects[effIndex].MiscValue;
    if (!entry)
        return;

    SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(m_spellInfo->Effects[effIndex].MiscValueB);
    if (!properties)
    {
        TC_LOG_ERROR("spells", "EffectSummonType: Unhandled summon type %u", m_spellInfo->Effects[effIndex].MiscValueB);
        return;
    }

    if (!m_originalCaster)
        return;

    // Don't summon Wild Imps and Infernal if player is dead
    if (m_originalCaster->GetTypeId() == TYPEID_PLAYER && !m_originalCaster->IsAlive())
        return;

    int32 duration = m_spellInfo->GetDuration();
    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);
    
    ObjectGuid privateObjectOwner = [&]()
    {
        if (!(properties->Flags & (SUMMON_PROP_FLAG_PERSONAL_SPAWN | SUMMON_PROP_FLAG_PERSONAL_GROUP_SPAWN)))
            return ObjectGuid::Empty;

        if (m_originalCaster->IsPrivateObject())
            return m_originalCaster->GetPrivateObjectOwner();

        if (properties->Flags & SUMMON_PROP_FLAG_PERSONAL_GROUP_SPAWN)
            if (m_originalCaster->IsPlayer() && m_originalCaster->ToPlayer()->GetGroup())
                return m_originalCaster->ToPlayer()->GetGroup()->GetGUID();

        return m_originalCaster->GetGUID();
    }();

    TempSummon* summon = nullptr;

    // determine how many units should be summoned
    uint32 numSummons;

    // some spells need to summon many units, for those spells number of summons is stored in effect value
    // however so far noone found a generic check to find all of those (there's no related data in summonproperties.dbc
    // and in spell attributes, possibly we need to add a table for those)
    // so here's a list of MiscValueB values, which is currently most generic check
    switch (properties->Id)
    {
        case 64:
        case 61:
        case 1101:
        case 66:
        case 648:
        case 2301:
        case 1061:
        case 1261:
        case 629:
        case 181:
        case 715:
        case 1562:
        case 833:
        case 1161:
        case 2929: // Summon Unbound Flamesparks, Flameseer's Staff
        case 3409: // Bad Juju
        case 3245: // Feral Spirit (Symbiosis)
            numSummons = (damage > 0) ? damage : 1;
            break;
        default:
            numSummons = 1;
            break;
    }

    switch (properties->Category)
    {
        case SUMMON_CATEGORY_WILD:
        case SUMMON_CATEGORY_ALLY:
        case SUMMON_CATEGORY_UNK:
            if ((properties->Flags & 512 &&
                properties->Type != SUMMON_TYPE_TOTEM &&
                m_spellInfo->Id != 162979)             || // Nightmarish Hitching Post
                m_spellInfo->Id == 127665              || // Void Tendrils
                m_spellInfo->Id == 124927              || // Call Dog (q31185)
                m_spellInfo->Id == 148684              || // Sky Lantern
                m_spellInfo->Id == 131411              || // Pandaren Dragonling (I think we neeed Flags & 2 here)
                m_spellInfo->Id == 125640              || // Summon Fish (q31314)
                m_spellInfo->Id == 142917              || // Xan'tish's Flute
                m_spellInfo->Id == 147044)                // Tomb Summon
            {
                SummonGuardian(effIndex, entry, properties, numSummons, privateObjectOwner);
                break;
            }
            switch (properties->Type)
            {
                case SUMMON_TYPE_PET:
                case SUMMON_TYPE_GUARDIAN:
                case SUMMON_TYPE_GUARDIAN2:
                case SUMMON_TYPE_MINION:
                case SUMMON_TYPE_WARBANNER:
                    SummonGuardian(effIndex, entry, properties, numSummons, privateObjectOwner);
                    break;
                // Summons a vehicle, but doesn't force anyone to enter it (see SUMMON_CATEGORY_VEHICLE)
                case SUMMON_TYPE_VEHICLE:
                case SUMMON_TYPE_VEHICLE2:
                    summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id, 0, privateObjectOwner);
                    break;
                case SUMMON_TYPE_LIGHTWELL:
                case SUMMON_TYPE_TOTEM:
                case SUMMON_TYPE_STATUE:
                    summon = SummonTotem(entry, properties, duration, privateObjectOwner);
                    break;
                case SUMMON_TYPE_MINIPET:
                {
                    summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id, 0, privateObjectOwner);
                    if (!summon || !summon->HasUnitTypeMask(UNIT_MASK_MINION))
                        return;

                    summon->AI()->EnterEvadeMode();

                    Player* player = m_caster->ToPlayer();
                    if (!player)
                        break;

                    BattlePetMgr& battlePetMgr = player->GetBattlePetMgr();

                    battlePetMgr.SetCurrentSummon(summon->GetGUID());

                    BattlePet* battlePet = battlePetMgr.GetBattlePet(battlePetMgr.GetCurrentSummonId());
                    if (!battlePet)
                        break;

                    player->SetUInt64Value(PLAYER_FIELD_SUMMONED_BATTLE_PET_GUID, battlePet->GetId());
                    player->SetUInt32Value(PLAYER_FIELD_CURRENT_BATTLE_PET_BREED_QUALITY, battlePet->GetQuality());

                    summon->SetCreateHealth(battlePet->GetMaxHealth());
                    summon->SetMaxHealth(battlePet->GetMaxHealth());
                    summon->SetHealth(battlePet->GetCurrentHealth());

                    summon->SetUInt64Value(UNIT_FIELD_BATTLE_PET_COMPANION_GUID, battlePet->GetId());
                    summon->SetUInt32Value(UNIT_FIELD_BATTLE_PET_COMPANION_NAME_TIMESTAMP, battlePet->GetTimestamp());
                    summon->SetUInt64Value(UNIT_FIELD_CREATED_BY, player->GetGUID());
                    summon->SetUInt32Value(UNIT_FIELD_WILD_BATTLE_PET_LEVEL, battlePet->GetLevel());
                    summon->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, summon->GetCreatureTemplate()->npcflag);

                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    break;
                }
                default:
                {
                    if (entry == 60199 || entry == 47649 || entry == 59172) // Rune of Power, Wild Mushrooms (druid), Wild Mushroom: Plague (dk)
                    {
                        summon = SummonTotem(entry, properties, duration, ObjectGuid::Empty);
                        break;
                    }

                    float radius = m_spellInfo->Effects[effIndex].CalcRadius();

                    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

                    for (uint32 count = 0; count < numSummons; ++count)
                    {
                        Position pos;
                        if (count == 0)
                            pos = *destTarget;
                        else
                            // randomize position for multiple summons
                            pos = m_caster->GetRandomPoint(*destTarget, radius);

                        summon = m_originalCaster->SummonCreature(entry, pos, summonType, duration, 0, privateObjectOwner);
                        if (!summon)
                            continue;

                        if (properties->Category == SUMMON_CATEGORY_ALLY)
                        {
                            summon->SetOwnerGUID(m_originalCaster->GetGUID());
                            summon->SetFaction(m_originalCaster->GetFaction());
                            summon->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, m_spellInfo->Id);
                        }
                        ExecuteLogEffectSummonObject(effIndex, summon);
                    }
                    return;
                }
            }//switch
            break;
        case SUMMON_CATEGORY_PET:
            SummonGuardian(effIndex, entry, properties, numSummons, privateObjectOwner);
            break;
        case SUMMON_CATEGORY_PUPPET:
            summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id, 0, privateObjectOwner);
            break;
        case SUMMON_CATEGORY_VEHICLE:
            // Summoning spells (usually triggered by npc_spellclick) that spawn a vehicle and that cause the clicker
            // to cast a ride vehicle spell on the summoned unit.
            summon = m_originalCaster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_caster, m_spellInfo->Id, 0, privateObjectOwner);
            if (!summon || !summon->IsVehicle())
                return;

            // The spell that this effect will trigger. It has SPELL_AURA_CONTROL_VEHICLE
            uint32 spellId = VEHICLE_SPELL_RIDE_HARDCODED;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[effIndex].CalcValue());
            if (spellInfo && spellInfo->HasAura(SPELL_AURA_CONTROL_VEHICLE))
                spellId = spellInfo->Id;

            // Hard coded enter vehicle spell
            m_originalCaster->CastSpell(summon, spellId, true);

            uint32 faction = properties->Faction;
            if (!faction)
                faction = m_originalCaster->GetFaction();

            summon->SetFaction(faction);
            break;
    }

    if (summon)
    {
        summon->SetCreatorGUID(m_originalCaster->GetGUID());
        ExecuteLogEffectSummonObject(effIndex, summon);
    }
}

void Spell::EffectLearnSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (unitTarget->ToPet())
            EffectLearnPetSpell(effIndex);
        return;
    }

    Player* player = unitTarget->ToPlayer();

    uint32 spellToLearn = (m_spellInfo->Id == 483 || m_spellInfo->Id == 55884) ? damage : m_spellInfo->Effects[effIndex].TriggerSpell;
    player->LearnSpell(spellToLearn, false);

    TC_LOG_DEBUG("spells", "Spell: Player %u has learned spell %u from NpcGUID=%u", player->GetGUID().GetCounter(), spellToLearn, m_caster->GetGUID().GetCounter());
}

typedef std::list< std::pair<uint32, uint64> > DispelList;
void Spell::EffectDispel(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    // Create dispel mask by dispel type
    uint32 dispel_type = m_spellInfo->Effects[effIndex].MiscValue;
    uint32 dispelMask  = SpellInfo::GetDispelMask(DispelType(dispel_type));

    DispelChargesList dispelList;
    unitTarget->GetDispellableAuraList(m_caster, dispelMask, dispelList);
    if (dispelList.empty())
        return;

    DispelChargesList::iterator itr = dispelList.begin();
    for (; itr != dispelList.end(); ++itr)
        if (itr->first->GetSpellInfo()->Id == 642 || itr->first->GetSpellInfo()->Id == 45438)
            break;

    bool predefined = itr != dispelList.end();

    // Ok if exist some buffs for dispel try dispel it
    uint32 failCount = 0;
    DispelChargesList successList;

    ObjectGuid caster = m_caster->GetGUID();
    ObjectGuid target = unitTarget->GetGUID();
    bool offensive = !unitTarget->IsFriendlyTo(m_caster);
    bool aoe = GetSpellInfo()->Effects[effIndex].IsTargetingArea();

    std::vector<int32> failedSpells;

    WorldPacket dataFail(SMSG_DISPEL_FAILED, 8+8+4+4+damage*4);
    // dispel N = damage buffs (or while exist buffs for dispel)
    for (int32 count = 0; count < damage && !dispelList.empty();)
    {
        if (!predefined)
        {
            // Random select buff for dispel
            itr = dispelList.begin();
            std::advance(itr, urand(0, dispelList.size() - 1));
        }

        int32 chance = itr->first->CalcDispelChance(unitTarget, offensive);
        // 2.4.3 Patch Notes: "Dispel effects will no longer attempt to remove effects that have 100% dispel resistance."
        if (!chance || (aoe && itr->first->GetSpellInfo()->Id == 144351)) // Mark of Arrogance. This effect is only removed by single target dispels 
        {
            dispelList.erase(itr);
            continue;
        }
        else
        {
            if (roll_chance_i(chance))
            {
                bool alreadyListed = false;
                for (auto&& sitr : successList)
                {
                    if (sitr.first == itr->first)
                    {
                        ++sitr.second;
                        alreadyListed = true;
                    }
                }
                if (!alreadyListed)
                    successList.push_back(std::make_pair(itr->first, 1));
                --itr->second;
                if (itr->second <= 0)
                    dispelList.erase(itr);
            }
            else
                failedSpells.push_back(int32(itr->first->GetId()));

            ++count;
        }
    }

    if (!failedSpells.empty())
    {
        dataFail.WriteBit(caster[5]);
        dataFail.WriteBit(caster[0]);
        dataFail.WriteBit(caster[4]);
        dataFail.WriteBit(target[4]);
        dataFail.WriteBit(caster[7]);
        dataFail.WriteBit(target[5]);
        dataFail.WriteBit(target[0]);
        dataFail.WriteBit(caster[3]);
        dataFail.WriteBit(target[7]);
        dataFail.WriteBit(caster[6]);
        dataFail.WriteBit(target[1]);
        dataFail.WriteBit(caster[1]);
        dataFail.WriteBit(caster[2]);
        dataFail.WriteBit(target[3]);
        dataFail.WriteBits(failedSpells.size(), 22);
        dataFail.WriteBit(target[2]);
        dataFail.WriteBit(target[6]);

        dataFail.FlushBits();

        dataFail.WriteByteSeq(caster[2]);
        dataFail.WriteByteSeq(target[1]);
        dataFail.WriteByteSeq(caster[5]);
        dataFail.WriteByteSeq(target[2]);
        dataFail.WriteByteSeq(target[5]);
        dataFail.WriteByteSeq(target[3]);
        dataFail.WriteByteSeq(target[7]);
        dataFail.WriteByteSeq(caster[4]);
        dataFail.WriteByteSeq(target[4]);
        dataFail.WriteByteSeq(caster[3]);
        dataFail.WriteByteSeq(caster[0]);
        dataFail.WriteByteSeq(caster[1]);
        dataFail.WriteByteSeq(target[6]);
        dataFail.WriteByteSeq(target[0]);
        dataFail.WriteByteSeq(caster[7]);
        dataFail.WriteByteSeq(caster[6]);

        for (auto itr : failedSpells)
            dataFail << int32(itr);

        dataFail << uint32(m_spellInfo->Id);

        m_caster->SendMessageToSet(&dataFail, true);
    }

    if (successList.empty())
        return;

    if (Player* player = m_caster->GetCharmerOrOwnerPlayerOrPlayerItself())
        if (Battleground* bg = player->GetBattleground())
            bg->UpdatePlayerScore(player, SCORE_AURAS_DISPELLED, successList.size());
    
    m_dispelCount += successList.size();

    WorldPacket data(SMSG_SPELLDISPELLOG);

    data.WriteBit(target[2]);
    data.WriteBit(caster[4]);
    data.WriteBit(target[6]);
    data.WriteBit(caster[5]);
    data.WriteBit(false); // is break
    data.WriteBit(false); // is steal
    data.WriteBit(target[5]);
    data.WriteBit(target[7]);
    data.WriteBit(target[4]);
    data.WriteBit(target[0]);
    data.WriteBit(target[1]);
    data.WriteBits(successList.size(), 22);
    data.WriteBit(caster[0]);

    for (DispelChargesList::iterator itr = successList.begin(); itr != successList.end(); ++itr)
    {
        data.WriteBit(false); // unk1
        data.WriteBit(false); // unk2
        data.WriteBit(false); // harmful
    }

    data.WriteBit(caster[3]);
    data.WriteBit(caster[2]);
    data.WriteBit(target[3]);
    data.WriteBit(caster[1]);
    data.WriteBit(caster[7]);
    data.WriteBit(caster[6]);

    for (DispelChargesList::iterator itr = successList.begin(); itr != successList.end(); ++itr)
    {
        // Send dispelled spell info
        //if (unk1)
        //    data << uint32(0);

        data << uint32(itr->first->GetId());              // Spell Id

        //if (unk2)
        //    data << uint32(0);

        unitTarget->RemoveAurasDueToSpellByDispel(itr->first->GetId(), m_spellInfo->Id, itr->first->GetCasterGUID(), m_caster, itr->second);
    }

    data.WriteByteSeq(caster[4]);
    data.WriteByteSeq(target[3]);
    data.WriteByteSeq(caster[6]);
    data.WriteByteSeq(caster[0]);
    data.WriteByteSeq(target[5]);
    data.WriteByteSeq(target[1]);
    data.WriteByteSeq(caster[3]);
    data.WriteByteSeq(caster[2]);
    data.WriteByteSeq(caster[1]);
    data.WriteByteSeq(caster[5]);
    data.WriteByteSeq(target[0]);

    data << uint32(m_spellInfo->Id);                // dispel spell id

    data.WriteByteSeq(target[7]);
    data.WriteByteSeq(target[6]);
    data.WriteByteSeq(target[2]);
    data.WriteByteSeq(caster[7]);
    data.WriteByteSeq(target[4]);

    m_caster->SendMessageToSet(&data, true);

    // On success dispel
    // Devour Magic
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->GetCategory() == SPELLCATEGORY_DEVOUR_MAGIC)
    {
        int32 heal_amount = m_spellInfo->Effects[EFFECT_1].CalcValue(m_caster);
        m_caster->CastCustomSpell(m_caster, 19658, &heal_amount, NULL, NULL, true);
        // Glyph of Felhunter
        if (Unit* owner = m_caster->GetOwner())
            if (owner->GetAura(56249))
                owner->CastCustomSpell(owner, 19658, &heal_amount, NULL, NULL, true);
    }
}

void Spell::EffectDualWield(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    unitTarget->SetCanDualWield(true);
    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->ToCreature()->UpdateDamagePhysical(OFF_ATTACK);
}

void Spell::EffectPull(SpellEffIndex effIndex)
{
    /// @todo create a proper pull towards distract spell center for distract
    EffectNULL(effIndex);
}

void Spell::EffectDistract(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    // Check for possible target
    if (!unitTarget || unitTarget->IsInCombat())
        return;

    // target must be OK to do this
    if (unitTarget->HasUnitState(UNIT_STATE_CONFUSED | UNIT_STATE_STUNNED | UNIT_STATE_FLEEING))
        return;

    unitTarget->SetFacingTo(unitTarget->GetAngle(destTarget));
    unitTarget->ClearUnitState(UNIT_STATE_MOVING);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->GetMotionMaster()->MoveDistract(damage * IN_MILLISECONDS);
}

void Spell::EffectPickPocket(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // victim must be creature and attackable
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || (unitTarget->GetEntry() != 56233 && m_caster->IsFriendlyTo(unitTarget))) // hack, but I dont know how it must work (failure on IsFriendlyTo for npc 56233 and faction 2393)
        return;

    // victim have to be alive and humanoid or undead
    if (unitTarget->IsAlive() && (unitTarget->GetCreatureTypeMask() &CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0)
        m_caster->ToPlayer()->SendLoot(unitTarget->GetGUID(), LOOT_PICKPOCKETING);
}

void Spell::EffectAddFarsight(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    float radius = m_spellInfo->Effects[effIndex].CalcRadius();
    int32 duration = m_spellInfo->GetDuration();
    // Caster not in world, might be spell triggered from aura removal
    if (!m_caster->IsInWorld())
        return;

    DynamicObject* dynObj = new DynamicObject(true);
    if (!dynObj->CreateDynamicObject(m_caster->GetMap()->GenerateLowGuid<HighGuid::DynamicObject>(), m_caster, m_spellInfo, *destTarget, radius, DYNAMIC_OBJECT_FARSIGHT_FOCUS))
    {
        delete dynObj;
        return;
    }

    dynObj->SetDuration(duration);
    dynObj->SetCasterViewpoint();
}

void Spell::EffectUntrainTalents(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || m_caster->GetTypeId() == TYPEID_PLAYER)
        return;

    if (ObjectGuid guid = m_caster->GetGUID()) // the trainer is the caster
        unitTarget->ToPlayer()->SendTalentWipeConfirm(guid, false);
}

void Spell::EffectTeleUnitsFaceCaster(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->IsInFlight())
        return;

    float dis = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);

    //float fx, fy, fz;
    //m_caster->GetClosePoint(fx, fy, fz, unitTarget->GetObjectSize(), dis);
    Position pos = m_caster->GetNearPosition(m_caster->GetObjectSize(), m_caster->GetAngle(unitTarget));

    // Earthen Vortex, Morchok, Dragon Soul
    // Prevent dropping into textures
    switch (m_spellInfo->Id)
    {
        case 103821:
        case 110045:
        case 110046:
        case 110047:
            pos.m_positionX += 8.0f;
            break;
        default:
            break;
    }

    unitTarget->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), -m_caster->GetOrientation(), unitTarget == m_caster);
}

void Spell::EffectLearnSkill(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage <= 0)
        return;

    uint32 skillid = m_spellInfo->Effects[effIndex].MiscValue;

    auto entry = GetSkillRaceClassInfo(skillid, unitTarget->GetRace(), unitTarget->GetClass());
    if (!entry)
    {
        TC_LOG_ERROR("spells", "Spell::EffectLearnSkill skill (%u) not found in SkillRaceClassInfo.dbc", skillid);
        return;
    }
    auto skillTier = sSkillTiersStore.LookupEntry(entry->SkillTierId);
    if (!skillTier)
    {
        TC_LOG_ERROR("spells", "Spell::EffectLearnSkill skill tier (%u) not found for skill %u", entry->SkillTierId, skillid);
        return;
    }

    uint16 skillval = unitTarget->ToPlayer()->GetPureSkillValue(skillid);
    if (!skillval && skillid >= SKILL_WAY_OF_THE_GRILL && skillid <= SKILL_WAY_OF_THE_BREW)
        skillval = 525;

    unitTarget->ToPlayer()->SetSkill(skillid, damage, skillval ? skillval : 1, skillTier->MaxSkillValue[damage - 1]);
}

void Spell::EffectPlayMovie(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 movieId = GetSpellInfo()->Effects[effIndex].MiscValue;
    if (!sMovieStore.LookupEntry(movieId))
        return;

    unitTarget->ToPlayer()->SendMovieStart(movieId);
}

void Spell::EffectTradeSkill(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->Effects[i].MiscValue;
    // uint16 skillmax = unitTarget->ToPlayer()->(skillid);
    // m_caster->ToPlayer()->SetSkill(skillid, skillval?skillval:1, skillmax+75);
}

void Spell::EffectEnchantItemPerm(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!itemTarget)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    // Handle vellums
    if (itemTarget->IsVellum())
    {
        // destroy one vellum from stack
        uint32 count = 1;
        player->DestroyItemCount(itemTarget, count, true);
        unitTarget = player;
        // Item count to create
        damage = 1;
        // and add a scroll
        DoCreateItem(effIndex, m_spellInfo->Effects[effIndex].ItemType);
        itemTarget = NULL;
        m_targets.SetItemTarget(NULL);
    }
    else
    {
        // do not increase skill if vellum used
        if (!(m_CastItem && m_CastItem->GetTemplate()->Flags & ITEM_PROTO_FLAG_TRIGGERED_CAST))
            player->UpdateCraftSkill(m_spellInfo->Id);

        uint32 enchant_id = m_spellInfo->Effects[effIndex].MiscValue;
        if (!enchant_id)
            return;

        SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchant)
            return;

        // item can be in trade slot and have owner diff. from caster
        Player* item_owner = itemTarget->GetOwner();
        if (!item_owner)
            return;

        if (item_owner != player && player->GetSession()->GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
        {
            sLog->outCommand(player->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
                player->GetName().c_str(), player->GetSession()->GetAccountId(),
                itemTarget->GetTemplate()->Name1.c_str(), itemTarget->GetEntry(),
                item_owner->GetName().c_str(), item_owner->GetSession()->GetAccountId());
        }

        EnchantmentSlot slot = PERM_ENCHANTMENT_SLOT;

        if (enchant->RequiredSkill == SKILL_ENGINEERING && enchant->Type[0] == ITEM_ENCHANTMENT_TYPE_USE_SPELL)
            slot = ENGINEERING_ENCHANTMENT_SLOT;

        // remove old enchanting before applying new if equipped
        item_owner->ApplyEnchantment(itemTarget, slot, false);

        itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, enchant->Charges, m_caster->GetGUID());

        // add new enchanting if equipped
        item_owner->ApplyEnchantment(itemTarget, slot, true);

        itemTarget->ClearSoulboundTradeable(item_owner);
    }
}

void Spell::EffectEnchantItemPrismatic(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!itemTarget)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    uint32 enchantId = m_spellInfo->Effects[effIndex].MiscValue;
    if (!enchantId)
        return;

    SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
    if (!enchant)
        return;

    // support only enchantings with add socket in this slot
    {
        bool add_socket = false;
        for (uint8 i = 0; i < MAX_ITEM_ENCHANTMENT_EFFECTS; ++i)
        {
            if (enchant->Type[i] == ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET)
            {
                add_socket = true;
                break;
            }
        }
        if (!add_socket)
        {
            TC_LOG_ERROR("spells", "Spell::EffectEnchantItemPrismatic: attempt apply enchant spell %u with SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC (%u) but without ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET (%u), not suppoted yet.",
                m_spellInfo->Id, SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC, ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET);
            return;
        }
    }

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (item_owner != player && player->GetSession()->GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
    {
        sLog->outCommand(player->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
            player->GetName().c_str(), player->GetSession()->GetAccountId(),
            itemTarget->GetTemplate()->Name1.c_str(), itemTarget->GetEntry(),
            item_owner->GetName().c_str(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, PRISMATIC_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchantId, 0, enchant->Charges, m_caster->GetGUID());

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, PRISMATIC_ENCHANTMENT_SLOT, true);

    itemTarget->ClearSoulboundTradeable(item_owner);
}

void Spell::EffectEnchantItemTmp(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (!itemTarget)
        return;

    uint32 enchant_id = m_spellInfo->Effects[effIndex].MiscValue;

    if (!enchant_id)
    {
        TC_LOG_ERROR("spells", "Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have 0 as enchanting id", m_spellInfo->Id, effIndex);
        return;
    }

    SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!enchant)
    {
        TC_LOG_ERROR("spells", "Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have not existed enchanting id %u ", m_spellInfo->Id, effIndex, enchant_id);
        return;
    }

    // select enchantment duration
    uint32 duration;

    // rogue family enchantments exception by duration
    if (m_spellInfo->Id == 38615)
        duration = 1800;                                    // 30 mins
    // other rogue family enchantments always 1 hour (some have spell damage=0, but some have wrong data in EffBasePoints)
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
        duration = 3600;                                    // 1 hour
    // shaman family enchantments
    else if (m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN)
        duration = 3600;                                    // 1 hour
    // other cases with this SpellVisual already selected
    else if (m_spellInfo->SpellVisual[0] == 215)
        duration = 1800;                                    // 30 mins
    // some fishing pole bonuses except Glow Worm which lasts full hour
    else if (m_spellInfo->SpellVisual[0] == 563 && m_spellInfo->Id != 64401)
        duration = 600;                                     // 10 mins
    else if (m_spellInfo->Id == 29702)
        duration = 300;                                     // 5 mins
    else if (m_spellInfo->Id == 37360)
        duration = 300;                                     // 5 mins
    // default case
    else
        duration = 3600;                                    // 1 hour

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (item_owner != player && player->GetSession()->GetSecurity() > SEC_PLAYER && sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE))
    {
        sLog->outCommand(player->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(temp): %s (Entry: %d) for player: %s (Account: %u)",
            player->GetName().c_str(), player->GetSession()->GetAccountId(),
            itemTarget->GetTemplate()->Name1.c_str(), itemTarget->GetEntry(),
            item_owner->GetName().c_str(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, duration * 1000, enchant->Charges, m_caster->GetGUID());

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, true);
}

void Spell::EffectTameCreature(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetPetGUID())
        return;

    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_UNIT)
        return;

    Creature* creatureTarget = unitTarget->ToCreature();

    if (creatureTarget->IsPet())
        return;

    if (m_caster->GetClass() != CLASS_HUNTER)
        return;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    int8 newPetSlot = player->GetSlotForNewPet();
    if (newPetSlot == -1)
        return;

    Pet* pet = player->CreateTamedPetFrom(creatureTarget, m_spellInfo->Id);
    if (!pet)                                               // in very specific state like near world end/etc.
        return;

    // "kill" original creature
    creatureTarget->DespawnOrUnsummon();

    uint8 level = player->GetLevel();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level - 1);

    // add to world
    pet->GetMap()->AddToMap(pet->ToCreature());

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level);

    // caster have pet now
    m_caster->SetMinion(pet, true);

    pet->InitTalentForLevel();

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    player->AddNewPet(newPetSlot, pet);
    player->SetCurrentPetId(pet->GetCharmInfo()->GetPetNumber(), trans);
    pet->SavePetToDB(trans);
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
    stmt->setUInt8(0, newPetSlot);
    stmt->setUInt32(1, player->GetGUID().GetCounter());
    stmt->setUInt32(2, pet->GetCharmInfo()->GetPetNumber());
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
    player->PetSpellInitialize();
    player->GetSession()->SendPetList(ObjectGuid::Empty, PET_SLOT_ACTIVE_FIRST, PET_SLOT_ACTIVE_LAST);
}

void Spell::EffectSummonPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* owner = NULL;
    if (m_originalCaster)
    {
        owner = m_originalCaster->ToPlayer();
        if (!owner && m_originalCaster->ToCreature()->IsTotem())
            owner = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    }

    uint32 petentry = m_spellInfo->Effects[effIndex].MiscValue;

    if (!owner)
    {
        SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(67);
        if (properties)
            SummonGuardian(effIndex, petentry, properties, 1, ObjectGuid::Empty);
        return;
    }

    Pet* oldSummon = owner->GetPet();

    // if pet requested type already exist
    if (oldSummon)
    {
        if (petentry == 0 || oldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if (oldSummon->isDead())
                return;

            ASSERT(oldSummon->GetMap() == owner->GetMap());

            float px, py, pz;
            owner->GetClosePoint(px, py, pz, oldSummon->GetObjectSize());

            auto& auras = oldSummon->GetAppliedAuras();
            for (auto itr = auras.begin(); itr != auras.end();)
            {
                Aura const* aura = itr->second->GetBase();
                if (!aura->IsPassive())
                    oldSummon->RemoveAura(itr);
                else
                    ++itr;
            }

            oldSummon->CombatStop();
            oldSummon->CastPetAuras(true);
            oldSummon->SetFullHealth();
            oldSummon->m_CreatureCategoryCooldowns.clear();
            oldSummon->m_CreatureSpellCooldowns.clear();
            if (owner->GetTypeId() == TYPEID_PLAYER && oldSummon->isControlled())
                owner->ToPlayer()->PetSpellInitialize();

            return;
        }

        if (owner->GetTypeId() == TYPEID_PLAYER)
            owner->ToPlayer()->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
        else
            return;
    }

    if (petentry == 0)
    {
        uint8 slot = static_cast<uint8>(m_spellInfo->Effects[effIndex].BasePoints);
        Pet* pet = new Pet(owner);
        if (!pet->LoadPetFromDB(PET_LOAD_BY_SLOT, slot))
            delete pet;
        return;
    }

    float x, y, z;
    owner->GetClosePoint(x, y, z, owner->GetObjectSize());
    Pet* pet = owner->SummonPet(petentry, x, y, z, owner->GetOrientation(), 0);
    if (!pet)
        return;

    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if (m_caster->ToCreature()->IsTotem())
            pet->SetReactState(REACT_AGGRESSIVE);
        else
            pet->SetReactState(REACT_DEFENSIVE);
    }

    pet->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, m_spellInfo->Id);

    // generate new name for summon pet
    std::string new_name=sObjectMgr->GeneratePetName(petentry);
    if (!new_name.empty())
        pet->SetName(new_name);

    // Okay, pet was not loaded from DB. It was created at the first time and we must save it to DB.
    pet->SavePetToDB();

    ExecuteLogEffectSummonObject(effIndex, pet);
}

void Spell::EffectLearnPetSpell(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (unitTarget->ToPlayer())
    {
        EffectLearnSpell(effIndex);
        return;
    }
    Pet* pet = unitTarget->ToPet();
    if (!pet)
        return;

    SpellInfo const* learn_spellproto = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[effIndex].TriggerSpell);
    if (!learn_spellproto)
        return;

    pet->LearnSpell(learn_spellproto->Id);
    pet->SavePetToDB();
    pet->GetOwner()->PetSpellInitialize();
}

void Spell::EffectTaunt(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if (!unitTarget || !unitTarget->CanHaveThreatList()
        || unitTarget->GetVictim() == m_caster)
    {
        SendCastResult(SPELL_FAILED_DONT_REPORT);
        return;
    }

    Player* player = m_caster->ToPlayer();
    if (player && player->GetRoleForGroup() == ROLES_TANK)
    {
        if (auto currentVictim = unitTarget->GetThreatManager().getCurrentVictim())
        {
            const uint32 spellVengeance = 132365;

            if (AuraEffect* victimVengeance = currentVictim->getTarget()->GetAuraEffect(spellVengeance, EFFECT_0))
            {
                AuraEffect* myVengeance = m_caster->GetAuraEffect(spellVengeance, EFFECT_0);
                if (!myVengeance || myVengeance->GetAmount() < victimVengeance->GetAmount() / 2)
                {
                    CustomSpellValues values;
                    values.AddSpellMod(SPELLVALUE_BASE_POINT0, victimVengeance->GetAmount() / 2);
                    values.AddSpellMod(SPELLVALUE_BASE_POINT1, victimVengeance->GetAmount() / 2);
                    m_caster->CastCustomSpell(spellVengeance, values, m_caster, TRIGGERED_FULL_MASK);
                }
            }
        }
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if (unitTarget->GetThreatManager().getCurrentVictim())
    {
        float myThreat = unitTarget->GetThreatManager().getThreat(m_caster);
        float itsThreat = unitTarget->GetThreatManager().getCurrentVictim()->getThreat();
        if (itsThreat > myThreat)
            unitTarget->GetThreatManager().addThreat(m_caster, itsThreat - myThreat);
    }

    //Set aggro victim to caster
    if (!unitTarget->GetThreatManager().getOnlineContainer().empty())
        if (HostileReference* forcedVictim = unitTarget->GetThreatManager().getOnlineContainer().getReferenceByTarget(m_caster))
            unitTarget->GetThreatManager().setCurrentVictim(forcedVictim);

    if ((unitTarget->ToCreature()->IsAIEnabled && !unitTarget->ToCreature()->HasReactState(REACT_PASSIVE)) || (unitTarget->IsPetGuardianStuff() && unitTarget->GetCharmerOrOwnerGUID().IsPlayer()))
        unitTarget->ToCreature()->AI()->AttackStart(m_caster);
}

void Spell::EffectWeaponDmg(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (uint32 j = effIndex + 1; j < MAX_SPELL_EFFECTS; ++j)
    {
        switch (m_spellInfo->Effects[j].Effect)
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                return;     // we must calculate only at last weapon effect
            break;
        }
    }

    // some spell specific modifiers
    float totalDamagePercentMod  = 1.0f;                    // applied to final bonus+weapon damage
    int32 fixed_bonus = 0;
    int32 spell_bonus = 0;                                  // bonus specific for spell

    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_SHAMAN:
        {
            // Skyshatter Harness item set bonus
            // Stormstrike
            if (AuraEffect* aurEff = m_caster->IsScriptOverriden(m_spellInfo, 5634))
                m_caster->CastSpell(m_caster, 38430, true, NULL, aurEff);
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Mangle (Cat): CP
            if (m_spellInfo->SpellFamilyFlags[1] & 0x400)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    m_caster->ToPlayer()->AddComboPoints(unitTarget, 1, this);
            }
            // Shred, Maul - Rend and Tear
            else if (m_spellInfo->SpellFamilyFlags[0] & 0x00008800 && unitTarget->HasAuraState(AURA_STATE_BLEEDING))
            {
                if (AuraEffect const* rendAndTear = m_caster->GetDummyAuraEffect(SPELLFAMILY_DRUID, 2859, 0))
                    AddPct(totalDamagePercentMod, rendAndTear->GetAmount());
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Blood Strike
            if (m_spellInfo->SpellFamilyFlags[0] & 0x400000)
            {
                float bonusPct = m_spellInfo->Effects[EFFECT_2].CalcValue(m_caster) * unitTarget->GetDiseasesByCaster(m_caster->GetGUID()) / 2.0f;
                // Death Knight T8 Melee 4P Bonus
                if (AuraEffect const* aurEff = m_caster->GetAuraEffect(64736, EFFECT_0))
                    AddPct(bonusPct, aurEff->GetAmount());
                AddPct(totalDamagePercentMod, bonusPct);
                break;
            }
            // Death Strike
            if (m_spellInfo->SpellFamilyFlags[0] & 0x10)
            {
                // Glyph of Death Strike
                // 2% more damage per 5 runic power, up to a maximum of 40%
                if (AuraEffect const* aurEff = m_caster->GetAuraEffect(59336, EFFECT_0))
                    if (uint32 runic = std::min<uint32>(uint32(m_caster->GetPower(POWER_RUNIC_POWER) / 2.5f), aurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue(m_caster)))
                        AddPct(totalDamagePercentMod, runic);
                break;
            }
            // Obliterate (12.5% more damage per disease)
            if (m_spellInfo->SpellFamilyFlags[1] & 0x20000)
            {
                float bonusPct = m_spellInfo->Effects[EFFECT_2].CalcValue(m_caster) * unitTarget->GetDiseasesByCaster(m_caster->GetGUID(), false) / 2.0f;
                // Death Knight T8 Melee 4P Bonus
                if (AuraEffect const* aurEff = m_caster->GetAuraEffect(64736, EFFECT_0))
                    AddPct(bonusPct, aurEff->GetAmount());
                AddPct(totalDamagePercentMod, bonusPct);
                break;
            }
            // Blood-Caked Strike - Blood-Caked Blade
            if (m_spellInfo->SpellIconID == 1736)
            {
                AddPct(totalDamagePercentMod, unitTarget->GetDiseasesByCaster(m_caster->GetGUID()) * 50.0f);
                break;
            }
            // Heart Strike
            if (m_spellInfo->SpellFamilyFlags[0] & 0x1000000)
            {
                float bonusPct = m_spellInfo->Effects[EFFECT_2].CalcValue(m_caster) * unitTarget->GetDiseasesByCaster(m_caster->GetGUID());
                // Death Knight T8 Melee 4P Bonus
                if (AuraEffect const* aurEff = m_caster->GetAuraEffect(64736, EFFECT_0))
                    AddPct(bonusPct, aurEff->GetAmount());

                AddPct(totalDamagePercentMod, bonusPct);
                break;
            }
            break;
        }
    }

    auto it = std::find_if(m_UniqueTargetInfo.begin(), m_UniqueTargetInfo.end(), [=](TargetInfo const& target)
    {
        return target.targetGUID == unitTarget->GetGUID();
    });

    TargetInfo const* targetInfo = it != m_UniqueTargetInfo.end() ? &*it : nullptr;

    bool normalized = false;
    float weaponDamagePercentMod = 0.0f;
    for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
    {
        if (targetInfo && !(targetInfo->effectMask & (1 << j)))
            continue;

        switch (m_spellInfo->Effects[j].Effect)
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                fixed_bonus += CalculateDamage(j, unitTarget);
                break;
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                fixed_bonus += CalculateDamage(j, unitTarget);
                normalized = true;
                break;
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
            {
                int32 pct = CalculateDamage(j, unitTarget);
                weaponDamagePercentMod += pct;
                ApplyPct(fixed_bonus, pct);
                break;
            }
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if (fixed_bonus || spell_bonus)
    {
        UnitMods unitMod;
        switch (m_attackType)
        {
            default:
            case BASE_ATTACK:   unitMod = UNIT_MOD_DAMAGE_MAINHAND; break;
            case OFF_ATTACK:    unitMod = UNIT_MOD_DAMAGE_OFFHAND;  break;
            case RANGED_ATTACK: unitMod = UNIT_MOD_DAMAGE_RANGED;   break;
        }

        float weapon_total_pct = 1.0f;
        if (m_spellInfo->SchoolMask & SPELL_SCHOOL_MASK_NORMAL)
        {
            weapon_total_pct = m_caster->GetModifierValue(unitMod, TOTAL_PCT);
            weapon_total_pct *= m_caster->GetTotalAuraEffectValue(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            weapon_total_pct *= m_caster->GetScallingDamageMod();
            // Bonus weapon damage should not be subject to 50% off-hand damage penalty
            if (m_attackType == OFF_ATTACK)
                weapon_total_pct *= 2;
        }

        if (fixed_bonus)
            fixed_bonus = int32(fixed_bonus * weapon_total_pct);
        if (spell_bonus)
            spell_bonus = int32(spell_bonus * weapon_total_pct);
    }

    int32 weaponDamage = m_caster->CalculateDamage(m_attackType, normalized, true);

    if (!normalized && m_caster->HasAuraType(SPELL_AURA_MOD_AUTOATTACK_DAMAGE))
    {
        // Already included to fields because of retarded system
        weaponDamage /= m_caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_AUTOATTACK_DAMAGE);
    }

    if (weaponDamagePercentMod)
        ApplyPct(weaponDamage, weaponDamagePercentMod);
    weaponDamage += fixed_bonus;

    if (spell_bonus)
        weaponDamage += spell_bonus;

    if (totalDamagePercentMod != 1.0f)
        weaponDamage = int32(weaponDamage* totalDamagePercentMod);

    // prevent negative damage
    uint32 eff_damage(std::max(weaponDamage, 0));

    // Add melee damage bonuses (also check for negative)
    uint32 damage = m_caster->MeleeDamageBonusDone(unitTarget, eff_damage, m_attackType, m_spellInfo);
    m_unmitigatedDamage += damage;
    m_damage += unitTarget->MeleeDamageBonusTaken(m_caster, damage, m_attackType, m_spellInfo);
}

void Spell::EffectThreat(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive() || !m_caster->IsAlive())
        return;

    if (!unitTarget->CanHaveThreatList())
        return;

    unitTarget->AddThreat(m_caster, float(damage));
}

void Spell::EffectHealMaxHealth(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive())
        return;

    uint32 addhealth = 0;

    // damage == 0 - heal for caster max health
    if (damage == 0)
        addhealth = m_caster->GetMaxHealth();
    else
        addhealth = unitTarget->GetMaxHealth() - unitTarget->GetHealth();

    addhealth *= unitTarget->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_PCT);

    m_caster->ApplyPvPHealMods(unitTarget, addhealth, GetSpellInfo());

    addhealth = std::min(addhealth, uint32(std::numeric_limits<int32>::max()));

    m_healing += int32(addhealth);
}

void Spell::EffectInterruptCast(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive())
        return;

    /// @todo not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    // there is no CURRENT_AUTOREPEAT_SPELL spells that can be interrupted
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_AUTOREPEAT_SPELL; ++i)
    {
        if (Spell* spell = unitTarget->GetCurrentSpell(CurrentSpellTypes(i)))
        {
            SpellInfo const* curSpellInfo = spell->m_spellInfo;
            // check if we can interrupt spell
            if ((spell->getState() == SPELL_STATE_CASTING
                || (spell->getState() == SPELL_STATE_PREPARING && spell->GetCastTime() > 0.0f))
                && (curSpellInfo->PreventionType & SPELL_PREVENTION_TYPE_SILENCE)
                && ((i == CURRENT_GENERIC_SPELL && curSpellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT)
                || (i == CURRENT_CHANNELED_SPELL && curSpellInfo->ChannelInterruptFlags[0] & CHANNEL_INTERRUPT_FLAG_INTERRUPT))) // this is shit
            {
                if (m_originalCaster)
                {
                    int32 duration = m_spellInfo->GetDuration();
                    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
                        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);
                    unitTarget->ProhibitSpellSchool(curSpellInfo->GetSchoolMask(), unitTarget->ModSpellDuration(m_spellInfo, unitTarget, duration, false, 1 << effIndex));
                }
                ExecuteLogEffectInterruptCast(effIndex, unitTarget, curSpellInfo->Id);
                unitTarget->InterruptSpell(CurrentSpellTypes(i), false);

                m_successfulInterrupt = true;

                if (unitTarget->GetTypeId() == TYPEID_PLAYER) // 1766, 15614, 93985
                {
                    ObjectGuid caster = GetCaster()->GetGUID();

                    WorldPacket data(SMSG_ADD_LOSS_OF_CONTROL);
                    data << uint32(m_spellInfo->Id);
                    data << uint32(m_spellInfo->DmgClass);
                    data << uint32(m_spellInfo->DurationEntry->Duration[0] ? m_spellInfo->DurationEntry->Duration[0] : 0);
                    data << uint32(m_spellInfo->DurationEntry->Duration[2] ? m_spellInfo->DurationEntry->Duration[2] : 0);

                    data.WriteBit(caster[5]);
                    data.WriteBits(m_spellInfo->Mechanic, 8);
                    data.WriteBit(caster[7]);
                    data.WriteBit(caster[2]);
                    data.WriteBit(caster[1]);
                    data.WriteBit(caster[6]);
                    data.WriteBit(caster[3]);
                    data.WriteBit(caster[4]);
                    data.WriteBits(CONTROL_TYPE_INTERRUPT, 8);
                    data.WriteBit(caster[0]);

                    data.FlushBits();

                    data.WriteByteSeq(caster[4]);
                    data.WriteByteSeq(caster[2]);
                    data.WriteByteSeq(caster[5]);
                    data.WriteByteSeq(caster[6]);
                    data.WriteByteSeq(caster[0]);
                    data.WriteByteSeq(caster[7]);
                    data.WriteByteSeq(caster[1]);
                    data.WriteByteSeq(caster[3]);

                    unitTarget->ToPlayer()->SendDirectMessage(&data);
                }
            }
        }
    }
}

void Spell::EffectSummonObjectWild(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 gameobject_id = m_spellInfo->Effects[effIndex].MiscValue;

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if (!target)
        target = m_caster;

    float x, y, z;
    if (m_targets.HasDst())
        destTarget->GetPosition(x, y, z);
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_PLAYER_BOUNDING_RADIUS);

    Map* map = target->GetMap();

    if (!pGameObj->Create(map->GenerateLowGuid<HighGuid::GameObject>(), gameobject_id, map,
        m_caster->GetPhaseMask(), x, y, z, target->GetOrientation(), { }, 100, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->AddToTransportIfNeeded(m_caster->GetTransport());

    for (auto phase : m_caster->GetPhases())
        pGameObj->SetPhased(phase, false, true);

    int32 duration = m_spellInfo->GetDuration();

    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    ExecuteLogEffectSummonObject(effIndex, pGameObj);

    // Wild object not have owner and check clickable by players
    map->AddToMap(pGameObj);

    if (pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP)
        if (Player* player = m_caster->ToPlayer())
            if (Battleground* bg = player->GetBattleground())
                bg->SetDroppedFlagGUID(pGameObj->GetGUID(), player->GetTeam() == ALLIANCE ? TEAM_HORDE: TEAM_ALLIANCE);

    if (uint32 linkedEntry = pGameObj->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if (linkedGO->Create(map->GenerateLowGuid<HighGuid::GameObject>(), linkedEntry, map,
            m_caster->GetPhaseMask(), x, y, z, target->GetOrientation(), { }, 100, GO_STATE_READY))
        {
            linkedGO->AddToTransportIfNeeded(m_caster->GetTransport());

            for (auto phase : m_caster->GetPhases())
                linkedGO->SetPhased(phase, false, true);

            linkedGO->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
            linkedGO->SetSpellId(m_spellInfo->Id);

            ExecuteLogEffectSummonObject(effIndex, linkedGO);

            // Wild object not have owner and check clickable by players
            map->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectScriptEffect(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    /// @todo we must implement hunter pet summon at login there (spell 6962)

    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                // Glyph of Backstab
                case 63975:
                {
                    // search our Rupture aura on target
                    if (AuraEffect const* aurEff = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, flag128{ 0x00100000 }, m_caster->GetGUID()))
                    {
                        uint32 countMin = aurEff->GetBase()->GetMaxDuration();
                        uint32 countMax = 12000; // this can be wrong, duration should be based on combo-points
                        countMax += m_caster->HasAura(56801) ? 4000 : 0;

                        if (countMin < countMax)
                        {
                            aurEff->GetBase()->SetDuration(uint32(aurEff->GetBase()->GetDuration() + 3000));
                            aurEff->GetBase()->SetMaxDuration(countMin + 2000);
                        }

                    }
                    return;
                }
                // Glyph of Scourge Strike
                case 69961:
                {
                    Unit::AuraEffectList const &mPeriodic = unitTarget->GetAuraEffectsByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraEffectList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
                    {
                        AuraEffect const* aurEff = *i;
                        SpellInfo const* spellInfo = aurEff->GetSpellInfo();
                        // search our Blood Plague and Frost Fever on target
                        if (spellInfo->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && spellInfo->SpellFamilyFlags[2] & 0x2 &&
                            aurEff->GetCasterGUID() == m_caster->GetGUID())
                        {
                            uint32 countMin = aurEff->GetBase()->GetMaxDuration();
                            uint32 countMax = spellInfo->GetMaxDuration();

                            // this Glyph
                            countMax += 9000;
                            // talent Epidemic
                            if (AuraEffect const* epidemic = m_caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_DEATHKNIGHT, 234, EFFECT_0))
                                countMax += epidemic->GetAmount();

                            if (countMin < countMax)
                            {
                                aurEff->GetBase()->SetDuration(aurEff->GetBase()->GetDuration() + 3000);
                                aurEff->GetBase()->SetMaxDuration(countMin + 3000);
                            }
                        }
                    }
                    return;
                }
                case 45204: // Clone Me!
                    m_caster->CastSpell(unitTarget, damage, true);
                    break;
                case 55693:                                 // Remove Collapsing Cave Aura
                    if (!unitTarget)
                        return;
                    unitTarget->RemoveAurasDueToSpell(m_spellInfo->Effects[effIndex].CalcValue());
                    break;
                // Bending Shinbone
                case 8856:
                {
                    if (!itemTarget && m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(20) ? 8854 : 8855;

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
                // Brittle Armor - need remove one 24575 Brittle Armor aura
                case 24590:
                    unitTarget->RemoveAuraFromStack(24575);
                    return;
                // Mercurial Shield - need remove one 26464 Mercurial Shield aura
                case 26465:
                    unitTarget->RemoveAuraFromStack(26464);
                    return;
                // Shadow Flame (All script effects, not just end ones to prevent player from dodging the last triggered spell)
                case 22539:
                case 22972:
                case 22975:
                case 22976:
                case 22977:
                case 22978:
                case 22979:
                case 22980:
                case 22981:
                case 22982:
                case 22983:
                case 22984:
                case 22985:
                {
                    if (!unitTarget || !unitTarget->IsAlive())
                        return;

                    // Onyxia Scale Cloak
                    if (unitTarget->HasAura(22683))
                        return;

                    // Shadow Flame
                    m_caster->CastSpell(unitTarget, 22682, true);
                    return;
                }
                // Decimate
                case 28374:
                case 54426:
                    if (unitTarget)
                    {
                        int32 damage = int32(unitTarget->GetHealth()) - int32(unitTarget->CountPctFromMaxHealth(5));
                        if (damage > 0)
                            m_caster->CastCustomSpell(28375, SPELLVALUE_BASE_POINT0, damage, unitTarget);
                    }
                    return;
                // Mirren's Drinking Hat
                case 29830:
                {
                    uint32 item = 0;
                    switch (urand(1, 6))
                    {
                        case 1:
                        case 2:
                        case 3:
                            item = 23584; break;            // Loch Modan Lager
                        case 4:
                        case 5:
                            item = 23585; break;            // Stouthammer Lite
                        case 6:
                            item = 23586; break;            // Aerie Peak Pale Ale
                    }
                    if (item)
                        DoCreateItem(effIndex, item);
                    break;
                }
                case 20589: // Escape artist
                case 30918: // Improved Sprint
                {
                    // Removes snares and roots.
                    unitTarget->RemoveMovementImpairingAuras();
                    break;
                }
                // Plant Warmaul Ogre Banner
                case 32307:
                    if (Player* caster = m_caster->ToPlayer())
                    {
                        caster->RewardPlayerAndGroupAtEvent(18388, unitTarget, true);
                        if (Creature* target = unitTarget->ToCreature())
                            target->DespawnOrUnsummon();
                    }
                    break;
                // Plant Kil'sorrow Banner
                case 32314:
                    if (Player* caster = m_caster->ToPlayer())
                    {
                        caster->RewardPlayerAndGroupAtEvent(18393, unitTarget, true);
                        if (Creature* target = unitTarget->ToCreature())
                            target->DespawnOrUnsummon();
                    }
                // Mug Transformation
                case 41931:
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint8 bag = 19;
                    uint8 slot = 0;
                    Item* item = NULL;

                    while (bag) // 256 = 0 due to var type
                    {
                        item = m_caster->ToPlayer()->GetItemByPos(bag, slot);
                        if (item && item->GetEntry() == 38587)
                            break;

                        ++slot;
                        if (slot == 39)
                        {
                            slot = 0;
                            ++bag;
                        }
                    }
                    if (bag)
                    {
                        if (m_caster->ToPlayer()->GetItemByPos(bag, slot)->GetCount() == 1) m_caster->ToPlayer()->RemoveItem(bag, slot, true);
                        else m_caster->ToPlayer()->GetItemByPos(bag, slot)->SetCount(m_caster->ToPlayer()->GetItemByPos(bag, slot)->GetCount()-1);
                        // Spell 42518 (Braufest - Gratisprobe des Braufest herstellen)
                        m_caster->CastSpell(m_caster, 42518, true);
                        return;
                    }
                    break;
                }
                // Brutallus - Burn
                case 45141:
                case 45151:
                {
                    //Workaround for Range ... should be global for every ScriptEffect
                    float radius = m_spellInfo->Effects[effIndex].CalcRadius();
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER && unitTarget->GetDistance(m_caster) >= radius && !unitTarget->HasAura(46394) && unitTarget != m_caster)
                        unitTarget->CastSpell(unitTarget, 46394, true);

                    break;
                }
                // Goblin Weather Machine
                case 46203:
                {
                    if (!unitTarget)
                        return;

                    uint32 spellId = 0;
                    switch (rand() % 4)
                    {
                        case 0: spellId = 46740; break;
                        case 1: spellId = 46739; break;
                        case 2: spellId = 46738; break;
                        case 3: spellId = 46736; break;
                    }
                    unitTarget->CastSpell(unitTarget, spellId, true);
                    break;
                }
                // 5, 000 Gold
                case 46642:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->ToPlayer()->ModifyMoney(5000 * GOLD);

                    break;
                }
                // Roll Dice - Decahedral Dwarven Dice
                case 47770:
                {
                    BroadcastText const* bct = sObjectMgr->GetBroadcastText(26147);//rubs his |Hitem:36863|h|cFFFFFFFF[Decahedral Dwarven Dice]|r|h between his hands and rolls.
                    LocaleConstant loc_idx = m_caster->ToPlayer()->GetSession()->GetSessionDbLocaleIndex();
                    std::string baseText = "";
                    if (bct)
                        baseText = bct->GetText(loc_idx, m_caster->GetGender());
                        m_caster->TextEmote(baseText);
                    break;
                }
                // Roll 'dem Bones - Worn Troll Dice
                case 47776:
                {
                    BroadcastText const* bct = sObjectMgr->GetBroadcastText(26152);//casually tosses his |Hitem:36862|h|cFFFFFFFF[Worn Troll Dice]|r|h.
                    LocaleConstant loc_idx = m_caster->ToPlayer()->GetSession()->GetSessionDbLocaleIndex();
                    std::string baseText = "";
                    if (bct)
                        baseText = bct->GetText(loc_idx, m_caster->GetGender());
                        m_caster->TextEmote(baseText);
                    break;                    
                }
                // Death Knight Initiate Visual
                case 51519:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    uint32 iTmpSpellId = 0;
                    switch (unitTarget->GetDisplayId())
                    {
                        case 25369: iTmpSpellId = 51552; break; // bloodelf female
                        case 25373: iTmpSpellId = 51551; break; // bloodelf male
                        case 25363: iTmpSpellId = 51542; break; // draenei female
                        case 25357: iTmpSpellId = 51541; break; // draenei male
                        case 25361: iTmpSpellId = 51537; break; // dwarf female
                        case 25356: iTmpSpellId = 51538; break; // dwarf male
                        case 25372: iTmpSpellId = 51550; break; // forsaken female
                        case 25367: iTmpSpellId = 51549; break; // forsaken male
                        case 25362: iTmpSpellId = 51540; break; // gnome female
                        case 25359: iTmpSpellId = 51539; break; // gnome male
                        case 25355: iTmpSpellId = 51534; break; // human female
                        case 25354: iTmpSpellId = 51520; break; // human male
                        case 25360: iTmpSpellId = 51536; break; // nightelf female
                        case 25358: iTmpSpellId = 51535; break; // nightelf male
                        case 25368: iTmpSpellId = 51544; break; // orc female
                        case 25364: iTmpSpellId = 51543; break; // orc male
                        case 25371: iTmpSpellId = 51548; break; // tauren female
                        case 25366: iTmpSpellId = 51547; break; // tauren male
                        case 25370: iTmpSpellId = 51545; break; // troll female
                        case 25365: iTmpSpellId = 51546; break; // troll male
                        default: return;
                    }

                    unitTarget->CastSpell(unitTarget, iTmpSpellId, true);
                    Creature* npc = unitTarget->ToCreature();
                    npc->LoadEquipment();
                    return;
                }
                // Deathbolt from Thalgran Blightbringer
                // reflected by Freya's Ward
                // Retribution by Sevenfold Retribution
                case 51854:
                {
                    if (!unitTarget)
                        return;
                    if (unitTarget->HasAura(51845))
                        unitTarget->CastSpell(m_caster, 51856, true);
                    else
                        m_caster->CastSpell(unitTarget, 51855, true);
                    break;
                }
                // Summon Ghouls On Scarlet Crusade
                case 51904:
                {
                    if (!m_targets.HasDst())
                        return;

                    float x, y, z;
                    float radius = m_spellInfo->Effects[effIndex].CalcRadius();
                    for (uint8 i = 0; i < 15; ++i)
                    {
                        m_caster->GetRandomPoint(*destTarget, radius, x, y, z);
                        m_caster->CastSpell(x, y, z, 54522, true);
                    }
                    break;
                }
                case 52173: // Coyote Spirit Despawn
                case 60243: // Blood Parrot Despawn
                    if (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->ToCreature()->IsSummon())
                        unitTarget->ToTempSummon()->UnSummon();
                    return;
                case 52479: // Gift of the Harvester
                    if (unitTarget && m_originalCaster)
                        m_originalCaster->CastSpell(unitTarget, urand(0, 1) ? damage : 52505, true);
                    return;
                case 53110: // Devour Humanoid
                    if (unitTarget)
                        unitTarget->CastSpell(m_caster, damage, true);
                    return;
                case 57347: // Retrieving (Wintergrasp RP-GG pickup spell)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->ToCreature()->DespawnOrUnsummon();

                    return;
                }
                case 57349: // Drop RP-GG (Wintergrasp RP-GG at death drop spell)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Delete item from inventory at death
                    m_caster->ToPlayer()->DestroyItemCount(damage, 5, true);

                    return;
                }
                case 58418:                                 // Portal to Orgrimmar
                case 58420:                                 // Portal to Stormwind
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER || effIndex != 0)
                        return;

                    uint32 spellID = m_spellInfo->Effects[EFFECT_0].CalcValue();
                    uint32 questID = m_spellInfo->Effects[EFFECT_1].CalcValue();

                    if (unitTarget->ToPlayer()->GetQuestStatus(questID) == QUEST_STATUS_COMPLETE)
                        unitTarget->CastSpell(unitTarget, spellID, true);

                    return;
                }
                case 58983: // Big Blizzard Bear
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Prevent stacking of mounts and client crashes upon dismounting
                    unitTarget->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    // Triggered spell id dependent on riding skill
                    if (uint16 skillval = unitTarget->ToPlayer()->GetSkillValue(SKILL_RIDING))
                    {
                        if (skillval >= 150)
                            unitTarget->CastSpell(unitTarget, 58999, true);
                        else
                            unitTarget->CastSpell(unitTarget, 58997, true);
                    }
                    return;
                }
                case 59317:                                 // Teleporting
                {

                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // return from top
                    if (unitTarget->ToPlayer()->GetAreaId() == 4637)
                        unitTarget->CastSpell(unitTarget, 59316, true);
                    // teleport atop
                    else
                        unitTarget->CastSpell(unitTarget, 59314, true);

                    return;
                }
                case 62482: // Grab Crate
                {
                    if (unitTarget)
                    {
                        if (Unit* seat = m_caster->GetVehicleBase())
                        {
                            if (Unit* parent = seat->GetVehicleBase())
                            {
                                /// @todo a hack, range = 11, should after some time cast, otherwise too far
                                m_caster->CastSpell(parent, 62496, true);
                                unitTarget->CastSpell(seat, m_spellInfo->Effects[EFFECT_0].CalcValue());
                            }
                        }
                    }
                    return;
                }
                // Stoneclaw Totem
                case 55328: // Rank 1
                case 55329: // Rank 2
                case 55330: // Rank 3
                case 55332: // Rank 4
                case 55333: // Rank 5
                case 55335: // Rank 6
                case 55278: // Rank 7
                case 58589: // Rank 8
                case 58590: // Rank 9
                case 58591: // Rank 10
                {
                    int32 basepoints0 = damage;
                    // Cast Absorb on totems
                    for (uint8 slot = SUMMON_SLOT_TOTEM; slot < SUMMON_SLOT_MAX_TOTEM; ++slot)
                    {
                        if (!unitTarget->m_SummonSlot[slot])
                            continue;

                        Creature* totem = unitTarget->GetMap()->GetCreature(unitTarget->m_SummonSlot[slot]);
                        if (totem && totem->IsTotem())
                        {
                            m_caster->CastCustomSpell(totem, 55277, &basepoints0, NULL, NULL, true);
                        }
                    }
                    // Glyph of Stoneclaw Totem
                    if (AuraEffect* aur=unitTarget->GetAuraEffect(63298, 0))
                    {
                        basepoints0 *= aur->GetAmount();
                        m_caster->CastCustomSpell(unitTarget, 55277, &basepoints0, NULL, NULL, true);
                    }
                    break;
                }
                case 45668:                                 // Ultra-Advanced Proto-Typical Shortening Blaster
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (roll_chance_i(50))                  // chance unknown, using 50
                        return;

                    static uint32 const spellPlayer[5] =
                    {
                        45674,                            // Bigger!
                        45675,                            // Shrunk
                        45678,                            // Yellow
                        45682,                            // Ghost
                        45684                             // Polymorph
                    };

                    static uint32 const spellTarget[5] =
                    {
                        45673,                            // Bigger!
                        45672,                            // Shrunk
                        45677,                            // Yellow
                        45681,                            // Ghost
                        45683                             // Polymorph
                    };

                    m_caster->CastSpell(m_caster, spellPlayer[urand(0, 4)], true);
                    unitTarget->CastSpell(unitTarget, spellTarget[urand(0, 4)], true);
                    break;
                }
                case 46877: // Teleport to Apex Point
                    if (unitTarget)
                        unitTarget->CastSpell(unitTarget, 46881, true); // eleport to Apex Point.
                    break;
                case 46879: // Teleport to Witch's Sanctum
                    if (unitTarget)
                        unitTarget->CastSpell(unitTarget, 46883, true); // Teleport to Witch's Sanctum
                    break;
                case 46880: // Teleport to Sunwell Plateau
                    if (unitTarget)
                        unitTarget->CastSpell(unitTarget, 46884, true); // Teleport to Sunwell Plateau
                    break;
                case 54042: // Force Player to Remove Haunted Memento
                    if (unitTarget)
                        unitTarget->CastSpell(unitTarget, 54041, true); // Remove Haunted Memento
                    break;
                case 74890: // Instant Statue
                    if (Unit* player = unitTarget)
                    {
                        player->m_Events.Schedule(1, [player]()
                        {
                            if (Creature* pedestal = player->GetVehicleCreatureBase())
                            {
                                player->CastSpell(pedestal, 75058, true);
                                pedestal->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            }
                        });
                        SendSpellGo(); // Entering a vehicle messes up animation
                    }
                    break;
                case 75058: // Instant Statue
                    if (Unit* pedestal = unitTarget)
                        if (Unit* player = m_caster)
                            pedestal->CastSpell(player, 75055, TRIGGERED_IGNORE_AURA_INTERRUPT_FLAGS);
                    break;
                case 37142: // Karazhan - Chess NPC Action: Melee Attack: Conjured Water Elemental
                case 37143: // Karazhan - Chess NPC Action: Melee Attack: Charger
                case 37147: // Karazhan - Chess NPC Action: Melee Attack: Human Cleric
                case 37149: // Karazhan - Chess NPC Action: Melee Attack: Human Conjurer
                case 37150: // Karazhan - Chess NPC Action: Melee Attack: King Llane
                case 37220: // Karazhan - Chess NPC Action: Melee Attack: Summoned Daemon
                case 32227: // Karazhan - Chess NPC Action: Melee Attack: Footman
                case 32228: // Karazhan - Chess NPC Action: Melee Attack: Grunt
                case 37337: // Karazhan - Chess NPC Action: Melee Attack: Orc Necrolyte
                case 37339: // Karazhan - Chess NPC Action: Melee Attack: Orc Wolf
                case 37345: // Karazhan - Chess NPC Action: Melee Attack: Orc Warlock
                case 37348: // Karazhan - Chess NPC Action: Melee Attack: Warchief Blackhand
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 32247, true); // Karazhan - Chess NPC Action: Melee Attack: DAMAGE (Footman)
                    return;
                }
                case 37775: // Karazhan - Chess NPC Action - Poison Cloud
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 37469, true); // Poison Cloud
                    return;
                }
                case 37824: // Karazhan - Chess NPC Action - Shadow Mend
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 37456, true); // Shadow Mend
                    return;
                }
                case 39338: // Karazhan - Chess, Medivh CHEAT: Hand of Medivh, Target Horde
                case 39342: // Karazhan - Chess, Medivh CHEAT: Hand of Medivh, Target Alliance
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 39339, true); // Hand of Medivh
                    return;
                }
                case 39341: // Karazhan - Chess, Medivh CHEAT: Fury of Medivh, Target Horde
                case 39344: // Karazhan - Chess, Medivh CHEAT: Fury of Medivh, Target Alliance
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, damage, true); // Fury of Medivh
                    return;
                }
                case 39395: // Victory Visual Controller
                {
                    if (roll_chance_i(10))
                    {
                        std::list<Creature*> squares;
                        unitTarget->GetCreatureListWithEntryInGrid(squares, 17208, 150.0f);
                        unitTarget->GetCreatureListWithEntryInGrid(squares, 17305, 150.0f);
                        if (!squares.empty())
                        {
                            if (Creature* square = Trinity::Containers::SelectRandomContainerElement(squares))
                            {
                                Position pos = square->GetPosition();
                                pos.RelocateOffset(frand(0.0f, 2 * M_PI), frand(0.0f, 2.0f));
                                unitTarget->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), unitTarget->GetOrientation());
                                unitTarget->CastSpell(unitTarget, 39390, true); // Board Visual
                            }
                        }
                    }
                    return;
                }
            }
            break;
        }
    }

    // normal DB scripted effect
    TC_LOG_DEBUG("spells", "Spell ScriptStart spellid %u in EffectScriptEffect(%u)", m_spellInfo->Id, effIndex);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, uint32(m_spellInfo->Id | (effIndex << 24)), m_caster, unitTarget);
}

void Spell::EffectSanctuary(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    unitTarget->getHostileRefManager().UpdateVisibility();

    Unit::AttackerSet const& attackers = unitTarget->getAttackers();
    for (Unit::AttackerSet::const_iterator itr = attackers.begin(); itr != attackers.end();)
    {
        if (!(*itr)->CanSeeOrDetect(unitTarget))
            (*(itr++))->AttackStop();
        else
            ++itr;
    }

    // Vanish takes damage
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && (m_spellInfo->SpellFamilyFlags[0] & SPELLFAMILYFLAG0_ROGUE_VANISH))
    {
        // While subterfuge is active the rogue is always visible for mobs. Just ensures that threat will be removed.
        unitTarget->getHostileRefManager().deleteReferences();
        return;
    }

    unitTarget->m_lastSanctuaryTime = getMSTime();
}

void Spell::EffectAddComboPoints(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (!unitTarget)
        return;

    if (!m_caster->m_movedPlayer)
        return;

    if (damage <= 0)
        return;

    m_caster->m_movedPlayer->AddComboPoints(unitTarget, damage, this);
}

void Spell::EffectDuel(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* caster = m_caster->ToPlayer();
    Player* target = unitTarget->ToPlayer();

    // caster or target already have requested duel
    if (caster->duel || target->duel || !target->GetSocial() || target->GetSocial()->HasIgnore(caster->GetGUID()))
        return;

    // Players can only fight a duel in zones with this flag
    AreaTableEntry const* casterAreaEntry = sAreaTableStore.LookupEntry(caster->GetAreaId());
    if (casterAreaEntry && !(casterAreaEntry->Flags & AREA_FLAG_ALLOW_DUELS))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* targetAreaEntry = sAreaTableStore.LookupEntry(target->GetAreaId());
    if (targetAreaEntry && !(targetAreaEntry->Flags & AREA_FLAG_ALLOW_DUELS))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* pGameObj = new GameObject;

    uint32 gameobject_id = m_spellInfo->Effects[effIndex].MiscValue;

    Map* map = m_caster->GetMap();
    if (!pGameObj->Create(map->GenerateLowGuid<HighGuid::GameObject>(), gameobject_id,
        map, m_caster->GetPhaseMask(),
        m_caster->GetPositionX()+(unitTarget->GetPositionX()-m_caster->GetPositionX())/2,
        m_caster->GetPositionY()+(unitTarget->GetPositionY()-m_caster->GetPositionY())/2,
        m_caster->GetPositionZ(),
        m_caster->GetOrientation(), { }, 0, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->AddToTransportIfNeeded(m_caster->GetTransport());

    for (auto phase : m_caster->GetPhases())
        pGameObj->SetPhased(phase, false, true);

    pGameObj->SetFaction(m_caster->GetFaction());
    pGameObj->SetUInt32Value(GAMEOBJECT_FIELD_LEVEL, m_caster->GetLevel()+1);
    int32 duration = m_spellInfo->GetDuration();
    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    ExecuteLogEffectSummonObject(effIndex, pGameObj);

    m_caster->AddGameObject(pGameObj);
    map->AddToMap(pGameObj);
    //END

    // Send request
    ObjectGuid arbiterGuid = pGameObj->GetGUID();
    ObjectGuid casterGuid = caster->GetGUID();

    WorldPacket data(SMSG_DUEL_REQUESTED, 9 + 9);
    data.WriteBit(arbiterGuid[5]);
    data.WriteBit(casterGuid[4]);
    data.WriteBit(casterGuid[2]);
    data.WriteBit(casterGuid[7]);
    data.WriteBit(arbiterGuid[0]);
    data.WriteBit(casterGuid[5]);
    data.WriteBit(arbiterGuid[4]);
    data.WriteBit(arbiterGuid[6]);
    data.WriteBit(casterGuid[1]);
    data.WriteBit(casterGuid[3]);
    data.WriteBit(casterGuid[6]);
    data.WriteBit(arbiterGuid[7]);
    data.WriteBit(arbiterGuid[3]);
    data.WriteBit(arbiterGuid[2]);
    data.WriteBit(arbiterGuid[1]);
    data.WriteBit(casterGuid[0]);

    data.WriteByteSeq(arbiterGuid[5]);
    data.WriteByteSeq(arbiterGuid[3]);
    data.WriteByteSeq(casterGuid[7]);
    data.WriteByteSeq(casterGuid[4]);
    data.WriteByteSeq(arbiterGuid[7]);
    data.WriteByteSeq(casterGuid[3]);
    data.WriteByteSeq(casterGuid[6]);
    data.WriteByteSeq(casterGuid[0]);
    data.WriteByteSeq(arbiterGuid[4]);
    data.WriteByteSeq(casterGuid[2]);
    data.WriteByteSeq(casterGuid[1]);
    data.WriteByteSeq(arbiterGuid[0]);
    data.WriteByteSeq(arbiterGuid[2]);
    data.WriteByteSeq(arbiterGuid[6]);
    data.WriteByteSeq(arbiterGuid[1]);
    data.WriteByteSeq(casterGuid[5]);

    caster->GetSession()->SendPacket(&data);
    target->GetSession()->SendPacket(&data);

    // create duel-info
    DuelInfo* duel   = new DuelInfo;
    duel->initiator  = caster;
    duel->opponent   = target;
    duel->startTime  = 0;
    duel->startTimer = 0;
    duel->isMounted  = (GetSpellInfo()->Id == 62875); // Mounted Duel
    caster->duel     = duel;

    DuelInfo* duel2   = new DuelInfo;
    duel2->initiator  = caster;
    duel2->opponent   = caster;
    duel2->startTime  = 0;
    duel2->startTimer = 0;
    duel2->isMounted  = (GetSpellInfo()->Id == 62875); // Mounted Duel
    target->duel      = duel2;

    caster->SetGuidValue(PLAYER_FIELD_DUEL_ARBITER, pGameObj->GetGUID());
    target->SetGuidValue(PLAYER_FIELD_DUEL_ARBITER, pGameObj->GetGUID());

    sScriptMgr->OnPlayerDuelRequest(target, caster);
}

void Spell::EffectStuck(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!sWorld->getBoolConfig(CONFIG_CAST_UNSTUCK))
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    TC_LOG_DEBUG("spells", "Spell Effect: Stuck");
    TC_LOG_INFO("spells", "Player %s (guid %u) used auto-unstuck future at map %u (%f, %f, %f)", player->GetName().c_str(), player->GetGUID().GetCounter(), player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

    if (player->IsInFlight())
        return;

    player->TeleportTo(player->GetStartPosition(), TELE_TO_SPELL);
    // homebind location is loaded always
    // target->TeleportTo(target->m_homebindMapId, target->m_homebindX, target->m_homebindY, target->m_homebindZ, target->GetOrientation(), (m_caster == m_caster ? TELE_TO_SPELL : 0));

    // Stuck spell trigger Hearthstone cooldown
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(8690);
    if (!spellInfo)
        return;
    Spell spell(player, spellInfo, TRIGGERED_FULL_MASK);
    spell.SendSpellCooldown();
}

void Spell::EffectSummonPlayer(SpellEffIndex /*effIndex*/)
{
    // workaround - this effect should not use target map
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Evil Twin (ignore player summon, but hide this for summoner)
    if (unitTarget->HasAura(23445))
        return;

    float x, y, z;
    m_caster->GetPosition(x, y, z);

    unitTarget->ToPlayer()->SetSummonPoint(m_caster->GetMapId(), x, y, z);

    WorldPacket data(SMSG_SUMMON_REQUEST, 8+4+4);
    ObjectGuid SummonerGUID = m_caster->GetGUID();

    data.WriteBit(SummonerGUID[0]);
    data.WriteBit(SummonerGUID[6]);
    data.WriteBit(SummonerGUID[3]);
    data.WriteBit(SummonerGUID[2]);
    data.WriteBit(SummonerGUID[1]);
    data.WriteBit(SummonerGUID[4]);
    data.WriteBit(SummonerGUID[7]);
    data.WriteBit(SummonerGUID[5]);

    data.WriteByteSeq(SummonerGUID[4]);
    data << uint32(m_caster->GetZoneId());                  // summoner zone
    data.WriteByteSeq(SummonerGUID[7]);
    data.WriteByteSeq(SummonerGUID[3]);
    data.WriteByteSeq(SummonerGUID[1]);
    data << uint32(MAX_PLAYER_SUMMON_DELAY*IN_MILLISECONDS); // auto decline after msecs
    data.WriteByteSeq(SummonerGUID[2]);
    data.WriteByteSeq(SummonerGUID[6]);
    data.WriteByteSeq(SummonerGUID[5]);
    data.WriteByteSeq(SummonerGUID[0]);
    unitTarget->ToPlayer()->GetSession()->SendPacket(&data);
}

void Spell::EffectActivateObject(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget)
        return;

    ScriptInfo activateCommand;
    activateCommand.command = SCRIPT_COMMAND_ACTIVATE_OBJECT;

    // int32 unk = m_spellInfo->Effects[effIndex].MiscValue; // This is set for EffectActivateObject spells; needs research

    switch (m_spellInfo->Id)
    {
        case 105847: // Seal Armor Breach (left), Spine of Deathwing, Dragon Soul
        case 105848: // Seal Armor Breach (right), Spine of Deathwing, Dragon Soul
        case 105363: // Breach Armor (left), Spine of Deathwing, Dragon Soul
        case 105385: // Breach Armor (right), Spine of Deathwing, Dragon Soul
        case 105366: // Plate Fly Off (left), Spine of Deathwing, Dragon Soul
        case 105384: // Plate Fly Off (right), Spine of Deathwing, Dragon Soul
            if (gameObjTarget->GetEntry() == 209623 || gameObjTarget->GetEntry() == 209631 || gameObjTarget->GetEntry() == 209632)
            {
                // Send anim kit
                gameObjTarget->SetAnimKitId(m_spellInfo->Effects[effIndex].MiscValueB, false);
                return;
            }
            break;
        case 104012:    // FIXME
            return;
    }

    // FIXME
    if (gameObjTarget->GetBattlegorund())
        return;

    gameObjTarget->GetMap()->ScriptCommandStart(activateCommand, 0, m_caster, gameObjTarget);
}

void Spell::EffectApplyGlyph(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_glyphIndex >= MAX_GLYPH_SLOT_INDEX)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    // glyph sockets level requirement
    uint8 minLevel = 0;
    switch (m_glyphIndex)
    {
        case 0:
        case 1:
            minLevel = 25;
            break;
        case 2:
        case 3:
            minLevel = 50;
            break;
        case 4:
        case 5:
            minLevel = 75;
            break;
    }

    if (minLevel && m_caster->GetLevel() < minLevel)
    {
        SendCastResult(SPELL_FAILED_GLYPH_SOCKET_LOCKED);
        return;
    }

    // apply new one
    if (uint32 newGlyph = m_spellInfo->Effects[effIndex].MiscValue)
    {
        if (GlyphPropertiesEntry const* newGlyphProperties = sGlyphPropertiesStore.LookupEntry(newGlyph))
        {
            if (GlyphSlotEntry const* newGlyphSlot = sGlyphSlotStore.LookupEntry(player->GetGlyphSlot(m_glyphIndex)))
            {
                if (newGlyphProperties->TypeFlags != newGlyphSlot->TypeFlags)
                {
                    SendCastResult(SPELL_FAILED_INVALID_GLYPH);
                    return;                                 // glyph slot mismatch
                }
            }

            player->SetGlyph(m_glyphIndex, newGlyph);
        }
    }
    else if (uint32 oldGlyph = player->GetGlyph(player->GetActiveSpec(), m_glyphIndex)) // Removing the glyph, get the old one
    {
        if (GlyphPropertiesEntry const* oldGlyphProperties = sGlyphPropertiesStore.LookupEntry(oldGlyph))
        {
            player->SetGlyph(m_glyphIndex, 0);
        }
    }
    player->SendTalentsInfoData();
}

void Spell::EffectEnchantHeldItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = unitTarget->ToPlayer();
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if (!item)
        return;

    // must be equipped
    if (!item->IsEquipped())
        return;

    if (m_spellInfo->Effects[effIndex].MiscValue)
    {
        uint32 enchantId = m_spellInfo->Effects[effIndex].MiscValue;
        if (!enchantId)
        {
            TC_LOG_ERROR("spells", "Spell %u Effect %u (SPELL_EFFECT_ENCHANT_HELD_ITEM) have 0 as enchanting id", m_spellInfo->Id, effIndex);
            return;
        }

        SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
        if (!enchant)
        {
            TC_LOG_ERROR("spells", "Spell %u Effect %u (SPELL_EFFECT_ENCHANT_HELD_ITEM) have not existing enchanting id %u", m_spellInfo->Id, effIndex, enchantId);
            return;
        }

        // select enchantment duration
        uint32 duration;

        // Venomhide Poison
        if (enchantId == 1003)
            duration = 300;                                     // 5 mins
        // default case
        else
            duration = 10;                                      // 10 seconds

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if (item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchantId)
            return;

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchantId, duration * IN_MILLISECONDS, enchant->Charges, m_caster->GetGUID());
        item_owner->ApplyEnchantment(item, slot, true);
    }
}

void Spell::EffectDisEnchant(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!itemTarget || !itemTarget->GetTemplate()->DisenchantID)
        return;

    if (Player* caster = m_caster->ToPlayer())
    {
        sServiceMgr->DeletedItemNotify(caster->GetGUID(), itemTarget, DELETING_TYPE_DISENCHANT);

        caster->UpdateCraftSkill(m_spellInfo->Id);
        caster->SendLoot(itemTarget->GetGUID(), LOOT_DISENCHANTING);
    }

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    int32 currentDrunk = int32(player->GetDrunkValue());
    int32 drunkMod = damage;
    if (currentDrunk + drunkMod > 100)
    {
        currentDrunk = 100;
        if (rand_chance() < 25.0f)
            player->CastSpell(player, 67468, false);    // Drunken Vomit
    }
    else if (currentDrunk + drunkMod < 0)
        currentDrunk = 0;
    else
        currentDrunk += drunkMod;

    player->SetDrunkValue(uint8(currentDrunk), m_CastItem ? m_CastItem->GetEntry() : 0);
}

void Spell::EffectFeedPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    Item* foodItem = itemTarget;
    if (!foodItem)
        return;

    Pet* pet = player->GetPet();
    if (!pet)
        return;

    if (!pet->IsAlive())
        return;

    ExecuteLogEffectDestroyItem(effIndex, foodItem->GetEntry());

    uint32 count = 1;
    player->DestroyItemCount(foodItem, count, true);
    /// @todo fix crash when a spell has two effects, both pointed at the same item target

    m_caster->CastSpell(pet, m_spellInfo->Effects[effIndex].TriggerSpell, true);
}

void Spell::EffectDismissPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsPet())
        return;

    Pet* pet = unitTarget->ToPet();

    ExecuteLogEffectUnsummonObject(effIndex, pet);
    pet->GetOwner()->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
}

void Spell::EffectSummonObject(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_SUMMON_OBJECT_SLOT2) // implementation in spell_generic - spell_gen_survey
        return;

    uint32 go_id = m_spellInfo->Effects[effIndex].MiscValue;
    uint8 slot = m_spellInfo->Effects[effIndex].Effect - SPELL_EFFECT_SUMMON_OBJECT_SLOT1;

    if (ObjectGuid guid = m_caster->m_ObjectSlot[slot])
    {
        if (GameObject* obj = m_caster->GetMap()->GetGameObject(guid))
        {
            bool hunterTrap = obj->GetGoType() == GAMEOBJECT_TYPE_TRAP && m_caster->GetClass() == CLASS_HUNTER;
            if (hunterTrap)
            {
                if (m_spellInfo->Id == obj->GetSpellId())
                {
                    obj->SetSpellId(0);
                    m_caster->RemoveGameObject(obj, true);
                }
            }
            else
            {
                // Recast case - null spell id to make auras not be removed on object remove from world
                if (m_spellInfo->Id == obj->GetSpellId())
                    obj->SetSpellId(0);
                m_caster->RemoveGameObject(obj, true);
            }
        }
        m_caster->m_ObjectSlot[slot].Clear();
    }

    GameObject* go = new GameObject();

    float x, y, z;
    // If dest location if present
    if (m_targets.HasDst())
        destTarget->GetPosition(x, y, z);
    // Summon in random point all other units if location present
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_PLAYER_BOUNDING_RADIUS);

    Map* map = m_caster->GetMap();
    if (!go->Create(map->GenerateLowGuid<HighGuid::GameObject>(), go_id, map,
        m_caster->GetPhaseMask(), x, y, z, m_caster->GetOrientation(), { }, 0, GO_STATE_READY))
    {
        delete go;
        return;
    }

    go->AddToTransportIfNeeded(m_caster->GetTransport());

    for (auto phase : m_caster->GetPhases())
        go->SetPhased(phase, false, true);

    //pGameObj->SetUInt32Value(GAMEOBJECT_FIELD_LEVEL, m_caster->GetLevel());
    int32 duration = m_spellInfo->GetDuration();
    go->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
    go->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(go);

    ExecuteLogEffectSummonObject(effIndex, go);

    map->AddToMap(go);

    m_caster->m_ObjectSlot[slot] = go->GetGUID();
}

void Spell::EffectSummonRaidMarker(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    Group* group = player->GetGroup();
    if (!group)
        return;

    if (group->isRaidGroup() && (!group->IsAssistant(player->GetGUID()) && !group->IsLeader(player->GetGUID()) && !(group->GetGroupType() & GROUPTYPE_EVERYONE_IS_ASSISTANT)))
        return;

    Group::RaidMarkerList& markers = group->GetRaidMarkers();
    for (Group::RaidMarkerList::iterator itr = markers.begin(); itr != markers.end();)
    {
        if ((*itr).spellId == m_spellInfo->Id)
        {
            if (DynamicObject* obj = player->GetMap()->GetDynamicObject((*itr).guid))
                obj->Remove();

            itr = markers.erase(itr);
        }
        else itr++;
    }

    // We could, but we do not use the ID or slot directly :).
    // uint32 go_id = m_spellInfo->Effects[effIndex].MiscValue;
    // uint32 slot = m_spellInfo->Effects[effIndex].BasePoints;
    // uint32 slotMask = 1 << m_spellInfo->Effects[effIndex].BasePoints;
    float radius = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);

    DynamicObject* dynObj = new DynamicObject(false);
    if (!dynObj->CreateDynamicObject(player->GetMap()->GenerateLowGuid<HighGuid::DynamicObject>(), m_caster, m_spellInfo, *destTarget, radius, DYNAMIC_OBJECT_RAID_MARKER))
    {
        delete dynObj;
        return;
    }

    dynObj->UpdateObjectVisibility(false);

    int32 duration = m_spellInfo->GetDuration();
    dynObj->SetDuration(duration);

    dynObj->SetFieldNotifyFlag(UF_FLAG_PARTY_MEMBER);

    // Calculate the position.
    float x = 0.0f, y = 0.0f, z = 0.0f;
    if (m_targets.HasDst())
        destTarget->GetPosition(x, y, z);

    group->AddRaidMarker(dynObj->GetGUID(), m_spellInfo->Id, m_caster->GetMapId(), x, y, z);
}

void Spell::EffectResurrect(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (unitTarget->IsAlive() || !unitTarget->IsInWorld())
        return;

    Player* target = unitTarget->ToPlayer();

    if (target->IsRessurectRequested())       // already have one active request
        return;

    uint32 health = target->CountPctFromMaxHealth(damage);
    uint32 mana   = CalculatePct(target->GetMaxPower(POWER_MANA), damage);
    if (GetSpellInfo()->Id == 20484)
        health = target->CountPctFromMaxHealth(CalculateDamage(EFFECT_1, target));

    if (m_spellInfo->AttributesEx8 & SPELL_ATTR8_BATTLE_RESURRECTION)
        if (InstanceScript* instance = target->GetInstanceScript())
            instance->UpdateResurrectionsCount();

    ExecuteLogEffectResurrect(effIndex, target);

    target->SetResurrectRequestData(m_caster, health, mana, 0);
    SendResurrectRequest(target);
}

void Spell::EffectAddExtraAttacks(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || !unitTarget->IsAlive() || !unitTarget->GetVictim())
        return;

    if (unitTarget->m_extraAttacks)
        return;

    unitTarget->m_extraAttacks = damage;

    ExecuteLogEffectExtraAttacks(effIndex, unitTarget->GetVictim(), damage);
}

void Spell::EffectParry(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetCanParry(true);
}

void Spell::EffectBlock(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetCanBlock(true);
}

void Spell::EffectLeap(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->IsInFlight())
        return;

    if (!m_targets.HasDst())
        return;

    Position pos = destTarget->GetPosition();
    pos = unitTarget->GetFirstCollisionPosition(unitTarget->GetDistance(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + 2.0f), 0.0f);
    unitTarget->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), unitTarget == m_caster);
}

void Spell::EffectReputation(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    float repChange = damage;

    uint32 factionId = m_spellInfo->Effects[effIndex].MiscValue;

    FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);
    if (!factionEntry)
        return;

    repChange = player->CalculateReputationGain(REPUTATION_SOURCE_SPELL, 0, repChange, factionId);

    player->GetReputationMgr().ModifyReputation(factionEntry, repChange);
}

void Spell::EffectQuestComplete(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Unknown logic for this quests, rewarding seems do nothing.
    // And there are another spells with actual qeusts which track cooldown for world bosses.
    // e.g.: 148314 - Ordos Bonus Loot from aura, 151559 - Ordos Bonus Loot with normal quest
    // And this is like BONUS loot not the regular personal loot...
    if (m_spellInfo->HasEffect(SPELL_EFFECT_BONUS_ROLL))
        return;

    Player* player = unitTarget->ToPlayer();

    uint32 questId = m_spellInfo->Effects[effIndex].MiscValue;
    if (questId)
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            return;

        uint16 logSlot = player->FindQuestSlot(questId);
        if (logSlot < MAX_QUEST_LOG_SIZE)
            player->AreaExploredOrEventHappens(questId);
        else if (player->CanTakeQuest(quest, false))    // never rewarded before
            player->CompleteQuest(questId);             // quest not in log - for internal use
    }
}

void Spell::EffectForceDeselect(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    // SMSG_BREAK_TAREGT not SMSG_CLEAR_TARGET
    GetCaster()->SendClearTarget();

    WorldPackets::Spells::ClearTarget clearTarget;
    clearTarget.Guid = m_caster->GetGUID();
    m_caster->SendMessageToSet(clearTarget.Write(), true);

    UnitList targets;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(GetCaster(), GetCaster(), GetCaster()->GetMap()->GetVisibilityRange());
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(GetCaster(), targets, u_check);
    GetCaster()->VisitNearbyObject(GetCaster()->GetMap()->GetVisibilityRange(), searcher);
    for (auto&& it : targets)
    {
        if (!it->HasUnitState(UNIT_STATE_CASTING))
            continue;

        for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++)
        {
            if (it->GetCurrentSpell(i)
                && it->GetCurrentSpell(i)->m_targets.GetUnitTargetGUID() == GetCaster()->GetGUID())
            {
                it->InterruptSpell(CurrentSpellTypes(i), false);
            }
        }
    }
}

void Spell::EffectSelfResurrect(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!m_caster || m_caster->IsAlive())
        return;
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!m_caster->IsInWorld())
        return;

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if (damage < 0)
    {
        health = uint32(-damage);
        mana = m_spellInfo->Effects[effIndex].MiscValue;
    }
    // percent case
    else
    {
        health = m_caster->CountPctFromMaxHealth(GetSpellInfo()->Id == 3026 ? 60 : damage);
        if (m_caster->GetMaxPower(POWER_MANA) > 0)
            mana = CalculatePct(m_caster->GetMaxPower(POWER_MANA), damage);
    }

    Player* player = m_caster->ToPlayer();
    player->ResurrectPlayer(0.0f);

    AddPct(health, player->GetResurrectHealthBonus());

    player->SetHealth(health);
    player->SetPower(POWER_MANA, mana);
    player->SetPower(POWER_RAGE, 0);
    player->SetPower(POWER_ENERGY, player->GetMaxPower(POWER_ENERGY));
    player->SetPower(POWER_FOCUS, 0);

    player->RestoreCombatWithInstance();

    player->SpawnCorpseBones();

    if (m_spellInfo->HasAttribute(SPELL_ATTR8_BATTLE_RESURRECTION))
        if (InstanceScript* instance = player->GetInstanceScript())
            instance->UpdateResurrectionsCount();
}

void Spell::EffectSkinning(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget->GetTypeId() != TYPEID_UNIT)
        return;
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Creature* creature = unitTarget->ToCreature();
    int32 targetLevel = creature->GetLevel();

    uint32 skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();

    m_caster->ToPlayer()->SendLoot(creature->GetGUID(), LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    int32 reqValue = targetLevel < 10 ? 0 :
                     targetLevel < 20 ? (targetLevel - 10) * 10 :
                     targetLevel < 74 ? targetLevel * 5 :
                     targetLevel < 80 ? (targetLevel * 2 - 73) * 5 :
                     targetLevel < 85 ? targetLevel * 5 + 35 :
                     targetLevel < 88 ? targetLevel * 5 + 35 + (targetLevel - 84) * 10 :
                                        targetLevel * 5 + 35 + (targetLevel - 85) * 15;

    int32 skillValue = m_caster->ToPlayer()->GetPureSkillValue(skill);

    // Double chances for elites
    m_caster->ToPlayer()->UpdateGatherSkill(skill, skillValue, reqValue, creature->isElite() ? 2 : 1, unitTarget);

}

void Spell::EffectCharge(SpellEffIndex /*effIndex*/)
{
    if (!unitTarget)
        return;

    if (effectHandleMode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
    {
        // Spell is not using explicit target - no generated path
        if (!m_preGeneratedPath || m_preGeneratedPath->GetPathType() == PATHFIND_BLANK)
        {
            //unitTarget->GetContactPoint(m_caster, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
            Position pos = unitTarget->GetFirstCollisionPosition(unitTarget->GetObjectSize(), unitTarget->GetRelativeAngle(m_caster));
            m_caster->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
        }
        else
            m_caster->GetMotionMaster()->MoveCharge(*m_preGeneratedPath);
    }

    if (effectHandleMode == SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        // not all charge effects used in negative spells
        if (!m_spellInfo->IsPositive() && m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->Attack(unitTarget, true);
    }
}

void Spell::EffectChargeDest(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_targets.HasDst())
    {
        Position pos = destTarget->GetPosition();
        float angle = m_caster->GetRelativeAngle(pos.GetPositionX(), pos.GetPositionY());
        float dist = m_caster->GetDistance(pos);

        if (m_spellInfo->Id != 143704) // Hackfix for Flash (Karoz, Paragon of the Klaxxi has collision dumb in climb) TODO
            pos = m_caster->GetFirstCollisionPosition(dist, angle);

        m_caster->GetMotionMaster()->MoveCharge(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    }
}

void Spell::EffectKnockBack(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (Creature* creatureTarget = unitTarget->ToCreature())
        if ((creatureTarget->isWorldBoss() || creatureTarget->IsDungeonBoss()) && creatureTarget->GetEntry() != 73483) // exclude thrall in Siege of Orgrimmar
            return;

    // Spells with SPELL_EFFECT_KNOCK_BACK (like Thunderstorm) can't knockback target if target has ROOT/STUN
    if (unitTarget->HasUnitState(UNIT_STATE_ROOT))
        return;

    // Instantly interrupt non melee spells being casted
    if (unitTarget->IsNonMeleeSpellCasted(true))
        unitTarget->InterruptNonMeleeSpells(true);

    float ratio = 0.1f;
    float speedxy = std::abs(float(m_spellInfo->Effects[effIndex].MiscValue) * ratio);
    float speedz = float(damage) * ratio;
    if (speedxy < 0.1f && speedz < 0.1f)
        return;

    float x, y;
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_KNOCK_BACK_DEST)
    {
        if (m_targets.HasDst())
            destTarget->GetPosition(x, y);
        else
            return;
    }
    else //if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_KNOCK_BACK)
    {
        if (unitTarget == m_caster)
        {
            // If value < 0 the push comes from target's back and target must fly face forward.
            // Otherwise target gets push at face and must fly back
            float offset = m_spellInfo->Effects[effIndex].MiscValue < 0 ? -1 : 1;
            x = m_caster->GetPositionX() + cos(m_caster->GetOrientation()) * offset;
            y = m_caster->GetPositionY() + sin(m_caster->GetOrientation()) * offset;
        }
        else
            m_caster->GetPosition(x, y);
    }

    unitTarget->KnockbackFrom(x, y, speedxy, speedz);

    if (m_spellInfo->HasPvpKnockbackDiminishing())
        if (Player* player = unitTarget->ToPlayer())
            player->UpdateKnockbackTime();
}

void Spell::EffectLeapBack(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        return;

    if (!unitTarget)
        return;

    float speedxy = float(m_spellInfo->Effects[effIndex].MiscValue)/10;
    float speedz = float(damage/10);

    switch (m_spellInfo->Id)
    {
        case 98928: // Lava Wave dmg, Ragnaros, Firelands
        case 100292:
        case 100293:
        case 100294:
        case 103684: // Wave of Virtue, Archbishop Benedictus, Hour of Twilight
        case 103781: // Wave of Twilight, Archbishop Benedictus, Hour of Twilight
            if (!unitTarget)
                break;

            if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                unitTarget->JumpTo(speedxy, speedz, false);
            break;
        case 140814: // Spray Water
            unitTarget->JumpTo(speedxy, speedz);
            break;
        case 781:    // Disengage
        case 102383: // Wild Charge (Moonkin)
        case 140949: // Direhorn Spirit
        case 134535: // Slip
            m_caster->JumpTo(speedxy, speedz, false);
            break;
        default:
            m_caster->JumpTo(speedxy, speedz, true);    // The fuck?
            break;
    }    
}

void Spell::EffectQuestClear(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = unitTarget->ToPlayer();

    uint32 quest_id = m_spellInfo->Effects[effIndex].MiscValue;

    Quest const* quest = sObjectMgr->GetQuestTemplate(quest_id);

    if (!quest)
        return;

    QuestStatus oldStatus = player->GetQuestStatus(quest_id);

    // Player has never done this quest
    if (oldStatus == QUEST_STATUS_NONE)
        return;

    // remove all quest entries for 'entry' from quest log
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = player->GetQuestSlotQuestId(slot);
        if (logQuest == quest_id)
        {
            player->SetQuestSlot(slot, 0);

            // we ignore unequippable quest items in this case, it's still be equipped
            player->TakeQuestSourceItem(logQuest, false);

            if (quest->HasFlag(QUEST_FLAGS_FLAGS_PVP))
            {
                player->pvpInfo.IsHostile = player->pvpInfo.IsInHostileArea || player->HasPvPForcingQuest();
                player->UpdatePvPState();
            }
        }
    }

    player->RemoveActiveQuest(quest_id, false);
    player->RemoveRewardedQuest(quest_id);

    sScriptMgr->OnQuestStatusChange(player, quest, oldStatus, QUEST_STATUS_NONE);
}

void Spell::EffectSendTaxi(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->ActivateTaxiPathTo(m_spellInfo->Effects[effIndex].MiscValue, m_spellInfo->Id);
}

void Spell::EffectPullTowards(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    float speedZ = (float)(m_spellInfo->Effects[effIndex].CalcValue() / 10);
    float speedXY = (float)(m_spellInfo->Effects[effIndex].MiscValue/10);
    Position pos;
    if (m_spellInfo->Effects[effIndex].Effect == SPELL_EFFECT_PULL_TOWARDS_DEST)
    {
        if (m_targets.HasDst())
            pos.Relocate(*destTarget);
        else
            return;
    }
    else //if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_PULL_TOWARDS)
        pos.Relocate(m_caster);

    unitTarget->GetMotionMaster()->MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), speedXY, speedZ);
}

void Spell::EffectDispelMechanic(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    uint32 mechanic = m_spellInfo->Effects[effIndex].MiscValue;

    std::queue < std::pair < uint32, uint64 > > dispel_list;

    Unit::AuraMap const& auras = unitTarget->GetOwnedAuras();
    for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura* aura = itr->second;
        if (!aura->GetApplicationOfTarget(unitTarget->GetGUID()))
            continue;
        if (roll_chance_i(aura->CalcDispelChance(unitTarget, !unitTarget->IsFriendlyTo(m_caster))))
            if ((aura->GetSpellInfo()->GetAllEffectsMechanicMask() & (1 << mechanic)))
                dispel_list.push(std::make_pair(aura->GetId(), aura->GetCasterGUID()));
    }

    m_dispelCount += dispel_list.size();

    for (; dispel_list.size(); dispel_list.pop())
    {
        unitTarget->RemoveAura(dispel_list.front().first, dispel_list.front().second, 0, AURA_REMOVE_BY_ENEMY_SPELL);
    }
}

void Spell::EffectResurrectPet(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    Pet* pet = player->GetPet();
    if (!pet || pet->IsAlive())
        return;

    if (damage < 0)
        return;

    float x, y, z;
    player->GetPosition(x, y, z);
    player->GetMap()->CreatureRelocation(pet, x, y, z, player->GetOrientation());

    pet->SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
    pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->setDeathState(ALIVE);
    pet->ClearUnitState(uint32(UNIT_STATE_ALL_STATE & ~UNIT_STATE_IGNORE_PATHFINDING));
    pet->SetHealth(pet->CountPctFromMaxHealth(damage));

    //pet->AIM_Initialize();
    //player->PetSpellInitialize();
    pet->SavePetToDB();
}

void Spell::EffectDestroyAllTotems(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    int32 mana = 0;
    for (uint8 slot : { SUMMON_SLOT_TOTEM_FIRE, SUMMON_SLOT_TOTEM_EARTH, SUMMON_SLOT_TOTEM_WATER, SUMMON_SLOT_TOTEM_AIR, SUMMON_SLOT_TOTEM_EXTRA})
    {
        if (!m_caster->m_SummonSlot[slot])
            continue;

        Creature* totem = m_caster->GetMap()->GetCreature(m_caster->m_SummonSlot[slot]);
        if (totem && totem->IsTotem())
        {
            uint32 spell_id = totem->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL);
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
            if (spellInfo)
            {
                int32 powerEntryIndex = -1;
                auto powerType = spellInfo->GetPowerType(m_caster, &powerEntryIndex);
                if (powerType == POWER_MANA && powerEntryIndex != -1)
                    mana += spellInfo->CalcPowerCost(m_caster, spellInfo->GetSchoolMask(), powerEntryIndex);
            }
            totem->ToTotem()->UnSummon();
        }
    }

    if (auto spellInfo = sSpellMgr->GetSpellInfo(39104))
        damage = spellInfo->Effects[EFFECT_1].CalcValue(m_caster);

    ApplyPct(mana, damage);
    if (mana)
        m_caster->CastCustomSpell(m_caster, 39104, &mana, NULL, NULL, true);
}

void Spell::EffectDurabilityDamage(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->Effects[effIndex].MiscValue;

    // -1 means all player equipped items and -2 all items
    if (slot < 0)
    {
        unitTarget->ToPlayer()->DurabilityPointsLossAll(damage, (slot < -1));
        ExecuteLogEffectDurabilityDamage(effIndex, unitTarget, -1, -1);
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (Item* item = unitTarget->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
    {
        unitTarget->ToPlayer()->DurabilityPointsLoss(item, damage);
        ExecuteLogEffectDurabilityDamage(effIndex, unitTarget, item->GetEntry(), slot);
    }
}

void Spell::EffectDurabilityDamagePCT(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->Effects[effIndex].MiscValue;

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        unitTarget->ToPlayer()->DurabilityLossAll(float(damage) / 100.0f, (slot < -1));
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (damage <= 0)
        return;

    if (Item* item = unitTarget->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        unitTarget->ToPlayer()->DurabilityLoss(item, float(damage) / 100.0f);
}

void Spell::EffectModifyThreatPercent(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    unitTarget->GetThreatManager().modifyThreatPercent(m_caster, damage);
}

void Spell::EffectTransmitted(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 name_id = m_spellInfo->Effects[effIndex].MiscValue;

    Unit::AuraEffectList const& overrideSummonedGameObjects = m_caster->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_SUMMONED_OBJECT);
    for (AuraEffect const* aurEff : overrideSummonedGameObjects)
    {
        if (uint32(aurEff->GetMiscValue()) == name_id)
        {
            name_id = uint32(aurEff->GetMiscValueB());
            break;
        }
     }

    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(name_id);

    if (!goinfo)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast", name_id, m_spellInfo->Id);
        return;
    }

    float fx, fy, fz;

    if (m_targets.HasDst())
        destTarget->GetPosition(fx, fy, fz);
    //FIXME: this can be better check for most objects but still hack
    else if (m_spellInfo->Effects[effIndex].HasRadius() && m_spellInfo->Speed == 0)
    {
        float dis = m_spellInfo->Effects[effIndex].CalcRadius(m_originalCaster);
        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_PLAYER_BOUNDING_RADIUS, dis);
    }
    else
    {
        //GO is always friendly to it's creator, get range for friends
        float min_dis = m_spellInfo->GetMinRange(true);
        float max_dis = m_spellInfo->GetMaxRange(true);
        float dis = (float)rand_norm() * (max_dis - min_dis) + min_dis;

        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_PLAYER_BOUNDING_RADIUS, dis);
    }

    Map* cMap = m_caster->GetMap();
    // if gameobject is summoning object, it should be spawned right on caster's position
    if (goinfo->type == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
        m_caster->GetPosition(fx, fy, fz);

    GameObject* pGameObj = new GameObject;

    if (!pGameObj->Create(cMap->GenerateLowGuid<HighGuid::GameObject>(), name_id, cMap,
        m_caster->GetPhaseMask(), fx, fy, fz, m_caster->GetOrientation(), { }, 100, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->AddToTransportIfNeeded(m_caster->GetTransport());

    for (auto phase : m_caster->GetPhases())
        pGameObj->SetPhased(phase, false, true);

    int32 duration = m_spellInfo->GetDuration();

    switch (goinfo->type)
    {
        case GAMEOBJECT_TYPE_FISHINGNODE:
        {
            m_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, pGameObj->GetGUID());
            m_caster->AddGameObject(pGameObj);              // will removed at spell cancel

            // end time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
            // start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
            int32 lastSec = 0;
            switch (urand(0, 3))
            {
                case 0: lastSec =  3; break;
                case 1: lastSec =  7; break;
                case 2: lastSec = 13; break;
                case 3: lastSec = 17; break;
            }

            duration = duration - lastSec*IN_MILLISECONDS + FISHING_BOBBER_READY_TIME*IN_MILLISECONDS;
            break;
        }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                pGameObj->AddUniqueUse(m_caster->ToPlayer());
                m_caster->AddGameObject(pGameObj);      // will be removed at spell cancel
            }
            break;
        }
        case GAMEOBJECT_TYPE_DUEL_ARBITER: // 52991
            m_caster->AddGameObject(pGameObj);
            break;
        case GAMEOBJECT_TYPE_FISHINGHOLE:
        case GAMEOBJECT_TYPE_CHEST:
        default:
            break;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);

    pGameObj->SetOwnerGUID(m_caster->GetGUID());

    //pGameObj->SetUInt32Value(GAMEOBJECT_FIELD_LEVEL, m_caster->GetLevel());
    pGameObj->SetSpellId(m_spellInfo->Id);

    ExecuteLogEffectSummonObject(effIndex, pGameObj);

    TC_LOG_DEBUG("spells", "AddObject at SpellEfects.cpp EffectTransmitted");
    //m_caster->AddGameObject(pGameObj);
    //m_ObjToDel.push_back(pGameObj);

    cMap->AddToMap(pGameObj);

    if (uint32 linkedEntry = pGameObj->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedGO = new GameObject;
        if (linkedGO->Create(cMap->GenerateLowGuid<HighGuid::GameObject>(), linkedEntry, cMap,
            m_caster->GetPhaseMask(), fx, fy, fz, m_caster->GetOrientation(), { }, 100, GO_STATE_READY))
        {
            linkedGO->AddToTransportIfNeeded(m_caster->GetTransport());

            for (auto phase : m_caster->GetPhases())
                linkedGO->SetPhased(phase, false, true);

            linkedGO->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);
            //linkedGO->SetUInt32Value(GAMEOBJECT_FIELD_LEVEL, m_caster->GetLevel());
            linkedGO->SetSpellId(m_spellInfo->Id);
            linkedGO->SetOwnerGUID(m_caster->GetGUID());

            ExecuteLogEffectSummonObject(effIndex, linkedGO);

            linkedGO->GetMap()->AddToMap(linkedGO);
        }
        else
        {
            delete linkedGO;
            linkedGO = NULL;
            return;
        }
    }
}

void Spell::EffectProspecting(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (!itemTarget || !(itemTarget->GetTemplate()->Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
        return;

    if (itemTarget->GetCount() < 5)
        return;

    if (sWorld->getBoolConfig(CONFIG_SKILL_PROSPECTING))
    {
        uint32 SkillValue = player->GetPureSkillValue(SKILL_JEWELCRAFTING);
        uint32 reqSkillValue = itemTarget->GetTemplate()->RequiredSkillRank;
        player->UpdateGatherSkill(SKILL_JEWELCRAFTING, SkillValue, reqSkillValue);
    }

    player->SendLoot(itemTarget->GetGUID(), LOOT_PROSPECTING);
}

void Spell::EffectMilling(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (!itemTarget || !(itemTarget->GetTemplate()->Flags & ITEM_PROTO_FLAG_MILLABLE))
        return;

    if (itemTarget->GetCount() < 5)
        return;

    if (sWorld->getBoolConfig(CONFIG_SKILL_MILLING))
    {
        uint32 SkillValue = player->GetPureSkillValue(SKILL_INSCRIPTION);
        uint32 reqSkillValue = itemTarget->GetTemplate()->RequiredSkillRank;
        player->UpdateGatherSkill(SKILL_INSCRIPTION, SkillValue, reqSkillValue);
    }

    player->SendLoot(itemTarget->GetGUID(), LOOT_MILLING);
}

void Spell::EffectSkill(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    TC_LOG_DEBUG("spells", "WORLD: SkillEFFECT");
}

/* There is currently no need for this effect. We handle it in Battleground.cpp
   If we would handle the resurrection here, the spiritguide would instantly disappear as the
   player revives, and so we wouldn't see the spirit heal visual effect on the npc.
   This is why we use a half sec delay between the visual effect and the resurrection itself */
void Spell::EffectSpiritHeal(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    /*
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;

    //m_spellInfo->Effects[i].BasePoints; == 99 (percent?)
    //unitTarget->ToPlayer()->setResurrect(m_caster->GetGUID(), unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), unitTarget->GetMaxHealth(), unitTarget->GetMaxPower(POWER_MANA));
    unitTarget->ToPlayer()->ResurrectPlayer(1.0f);
    unitTarget->ToPlayer()->SpawnCorpseBones();
    */
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    TC_LOG_DEBUG("spells", "Effect: SkinPlayerCorpse");

    Player* player = m_caster->ToPlayer();
    Player* target = unitTarget->ToPlayer();
    if (!player || !target || target->IsAlive())
        return;

    target->RemovedInsignia(player);
}

void Spell::EffectStealBeneficialBuff(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    TC_LOG_DEBUG("spells", "Effect: StealBeneficialBuff");

    if (!unitTarget || unitTarget == m_caster)                 // can't steal from self
        return;

    DispelChargesList steal_list;

    // Create dispel mask by dispel type
    uint32 dispelMask  = SpellInfo::GetDispelMask(DispelType(m_spellInfo->Effects[effIndex].MiscValue));
    Unit::AuraMap const& auras = unitTarget->GetOwnedAuras();
    for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        Aura* aura = itr->second;
        AuraApplication * aurApp = aura->GetApplicationOfTarget(unitTarget->GetGUID());
        if (!aurApp)
            continue;

        if ((aura->GetSpellInfo()->GetDispelMask()) & dispelMask)
        {
            // Need check for passive? this
            if (!aurApp->IsPositive() || aura->IsPassive() || aura->GetSpellInfo()->AttributesEx4 & SPELL_ATTR4_NOT_STEALABLE)
                continue;

            // The charges / stack amounts don't count towards the total number of auras that can be dispelled.
            // Ie: A dispel on a target with 5 stacks of Winters Chill and a Polymorph has 1 / (1 + 1) -> 50% chance to dispell
            // Polymorph instead of 1 / (5 + 1) -> 16%.
            bool dispel_charges = aura->GetSpellInfo()->AttributesEx7 & SPELL_ATTR7_DISPEL_CHARGES;
            uint8 charges = dispel_charges ? aura->GetCharges() : aura->GetStackAmount();
            if (charges > 0)
                steal_list.push_back(std::make_pair(aura, charges));
        }
    }

    if (steal_list.empty())
        return;

    // Ok if exist some buffs for dispel try dispel it
    uint32 failCount = 0;
    DispelList success_list;

    ObjectGuid caster = m_caster->GetGUID();
    ObjectGuid target = unitTarget->GetGUID();
    std::vector<int32> FailedSpells;

    WorldPacket dataFail(SMSG_DISPEL_FAILED, 8+8+4+4+damage*4);
    // dispel N = damage buffs (or while exist buffs for dispel)
    for (int32 count = 0; count < damage && !steal_list.empty();)
    {
        // Random select buff for dispel
        DispelChargesList::iterator itr = steal_list.begin();
        std::advance(itr, urand(0, steal_list.size() - 1));

        int32 chance = itr->first->CalcDispelChance(unitTarget, !unitTarget->IsFriendlyTo(m_caster));
        // 2.4.3 Patch Notes: "Dispel effects will no longer attempt to remove effects that have 100% dispel resistance."
        if (!chance)
        {
            steal_list.erase(itr);
            continue;
        }
        else
        {
            if (roll_chance_i(chance))
            {
                success_list.push_back(std::make_pair(itr->first->GetId(), itr->first->GetCasterGUID()));
                --itr->second;
                if (itr->second <= 0)
                    steal_list.erase(itr);
            }
            else
                FailedSpells.push_back(int32(itr->first->GetId()));

            ++count;
        }
    }

    if (!FailedSpells.empty())
    {
        dataFail.WriteBit(caster[5]);
        dataFail.WriteBit(caster[0]);
        dataFail.WriteBit(caster[4]);
        dataFail.WriteBit(target[4]);
        dataFail.WriteBit(caster[7]);
        dataFail.WriteBit(target[5]);
        dataFail.WriteBit(target[0]);
        dataFail.WriteBit(caster[3]);
        dataFail.WriteBit(target[7]);
        dataFail.WriteBit(caster[6]);
        dataFail.WriteBit(target[1]);
        dataFail.WriteBit(caster[1]);
        dataFail.WriteBit(caster[2]);
        dataFail.WriteBit(target[3]);
        dataFail.WriteBits(FailedSpells.size(), 22);
        dataFail.WriteBit(target[2]);
        dataFail.WriteBit(target[6]);

        dataFail.FlushBits();

        dataFail.WriteByteSeq(caster[2]);
        dataFail.WriteByteSeq(target[1]);
        dataFail.WriteByteSeq(caster[5]);
        dataFail.WriteByteSeq(target[2]);
        dataFail.WriteByteSeq(target[5]);
        dataFail.WriteByteSeq(target[3]);
        dataFail.WriteByteSeq(target[7]);
        dataFail.WriteByteSeq(caster[4]);
        dataFail.WriteByteSeq(target[4]);
        dataFail.WriteByteSeq(caster[3]);
        dataFail.WriteByteSeq(caster[0]);
        dataFail.WriteByteSeq(caster[1]);
        dataFail.WriteByteSeq(target[6]);
        dataFail.WriteByteSeq(target[0]);
        dataFail.WriteByteSeq(caster[7]);
        dataFail.WriteByteSeq(caster[6]);

        for (auto itr : FailedSpells)
            dataFail << int32(itr);

        dataFail << uint32(m_spellInfo->Id);

        m_caster->SendMessageToSet(&dataFail, true);
    }

    if (success_list.empty())
        return;

    m_dispelCount = success_list.size();

    if (Player* player = m_caster->GetCharmerOrOwnerPlayerOrPlayerItself())
        if (Battleground* bg = player->GetBattleground())
            bg->UpdatePlayerScore(player, SCORE_AURAS_STOLEN, success_list.size());

    WorldPacket data(SMSG_SPELLDISPELLOG);

    data.WriteBit(target[2]);
    data.WriteBit(caster[4]);
    data.WriteBit(target[6]);
    data.WriteBit(caster[5]);
    data.WriteBit(false); // is break
    data.WriteBit(true); // is steal
    data.WriteBit(target[5]);
    data.WriteBit(target[7]);
    data.WriteBit(target[4]);
    data.WriteBit(target[0]);
    data.WriteBit(target[1]);
    data.WriteBits(success_list.size(), 22);
    data.WriteBit(caster[0]);

    for (DispelList::iterator itr = success_list.begin(); itr != success_list.end(); ++itr)
    {
        data.WriteBit(false); // unk1
        data.WriteBit(false); // unk2
        data.WriteBit(false); // harmful
    }

    data.WriteBit(caster[3]);
    data.WriteBit(caster[2]);
    data.WriteBit(target[3]);
    data.WriteBit(caster[1]);
    data.WriteBit(caster[7]);
    data.WriteBit(caster[6]);

    for (DispelList::iterator itr = success_list.begin(); itr != success_list.end(); ++itr)
    {
        // Send dispelled spell info
        //if (unk1)
        //    data << uint32(0);

        data << uint32(itr->first);              // Spell Id

        //if (unk2)
        //    data << uint32(0);

        unitTarget->RemoveAurasDueToSpellBySteal(itr->first, itr->second, m_caster);
    }

    data.WriteByteSeq(caster[4]);
    data.WriteByteSeq(target[3]);
    data.WriteByteSeq(caster[6]);
    data.WriteByteSeq(caster[0]);
    data.WriteByteSeq(target[5]);
    data.WriteByteSeq(target[1]);
    data.WriteByteSeq(caster[3]);
    data.WriteByteSeq(caster[2]);
    data.WriteByteSeq(caster[1]);
    data.WriteByteSeq(caster[5]);
    data.WriteByteSeq(target[0]);

    data << uint32(m_spellInfo->Id);                // dispel spell id

    data.WriteByteSeq(target[7]);
    data.WriteByteSeq(target[6]);
    data.WriteByteSeq(target[2]);
    data.WriteByteSeq(caster[7]);
    data.WriteByteSeq(target[4]);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::EffectKillCreditPersonal(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->KilledMonsterCredit(m_spellInfo->Effects[effIndex].MiscValue, ObjectGuid::Empty);
}

void Spell::EffectKillCredit(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 creatureEntry = m_spellInfo->Effects[effIndex].MiscValue;
    if (!creatureEntry)
    {
        if (m_spellInfo->Id == 42793) // Burn Body
            creatureEntry = 24008; // Fallen Combatant
    }

    if (creatureEntry)
        unitTarget->ToPlayer()->RewardPlayerAndGroupAtEvent(creatureEntry, unitTarget);
}

void Spell::EffectQuestFail(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->FailQuest(m_spellInfo->Effects[effIndex].MiscValue);
}

void Spell::EffectQuestStart(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 questId = m_spellInfo->Effects[effIndex].MiscValue;
    Player* player = unitTarget->ToPlayer();

    if (Quest const* qInfo = sObjectMgr->GetQuestTemplate(questId))
    {
        if (player->CanTakeQuest(qInfo, false) && player->CanAddQuest(qInfo, false))
        {
            player->AddQuest(qInfo, NULL);

            if (player->CanCompleteQuest(questId))
                player->CompleteQuest(questId);
        }
    }
}

void Spell::EffectActivateRune(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_LAUNCH)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    if (player->GetClass() != CLASS_DEATH_KNIGHT)
        return;

    // needed later
    m_runesState = m_caster->ToPlayer()->GetRunesState();

    uint32 count = std::max<uint32>(damage, 1);
    for (uint32 j = 0; j < MAX_RUNES && count > 0; ++j)
    {
        if (player->HasRuneCooldown(j) && player->GetCurrentRune(j) == RuneType(m_spellInfo->Effects[effIndex].MiscValue))
        {
            if (m_spellInfo->Id == 45529)
                if (player->GetBaseRune(j) != RuneType(m_spellInfo->Effects[effIndex].MiscValueB))
                    continue;

            player->ActivateRune(j);
            --count;
        }
    }

    player->ResyncRunes(MAX_RUNES);
}

void Spell::EffectCreateTamedPet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetPetGUID() || unitTarget->GetClass() != CLASS_HUNTER)
        return;

    Player* player = unitTarget->ToPlayer();
    if (!player)
        return;

    int8 newPetSlot = player->GetSlotForNewPet();
    if (newPetSlot == -1)
        return;

    uint32 creatureEntry = m_spellInfo->Effects[effIndex].MiscValue;
    Pet* pet = unitTarget->CreateTamedPetFrom(creatureEntry, m_spellInfo->Id);
    if (!pet)
        return;

    // relocate
    float px, py, pz;
    unitTarget->GetClosePoint(px, py, pz, pet->GetObjectSize(), PET_FOLLOW_DIST, pet->GetFollowAngle());
    pet->Relocate(px, py, pz, unitTarget->GetOrientation());

    // add to world
    pet->GetMap()->AddToMap(pet->ToCreature());

    // unitTarget has pet now
    unitTarget->SetMinion(pet, true);

    pet->InitTalentForLevel();

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    player->AddNewPet(newPetSlot, pet);
    player->SetCurrentPetId(pet->GetCharmInfo()->GetPetNumber(), trans);
    pet->SavePetToDB(trans);
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
    stmt->setUInt8(0, newPetSlot);
    stmt->setUInt32(1, player->GetGUID().GetCounter());
    stmt->setUInt32(2, pet->GetCharmInfo()->GetPetNumber());
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);
    player->PetSpellInitialize();

    player->GetSession()->SendPetList(ObjectGuid::Empty, PET_SLOT_ACTIVE_FIRST, PET_SLOT_ACTIVE_LAST);
}

void Spell::EffectDiscoverTaxi(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    uint32 nodeid = m_spellInfo->Effects[effIndex].MiscValue;
    if (sTaxiNodesStore.LookupEntry(nodeid))
        unitTarget->ToPlayer()->GetSession()->SendDiscoverNewTaxiNode(nodeid);
}

void Spell::EffectTitanGrip(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        m_caster->ToPlayer()->SetCanTitanGrip(true);
}

void Spell::EffectRedirectThreat(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (unitTarget)
        m_caster->SetRedirectThreat(unitTarget->GetGUID(), uint32(damage));
}

void Spell::EffectGameObjectDamage(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget)
        return;

    Unit* caster = m_originalCaster;
    if (!caster)
        return;

    FactionTemplateEntry const* casterFaction = caster->GetFactionTemplateEntry();
    FactionTemplateEntry const* targetFaction = sFactionTemplateStore.LookupEntry(gameObjTarget->GetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE));
    // Do not allow to damage GO's of friendly factions (ie: Wintergrasp Walls/Ulduar Storm Beacons)
    if ((casterFaction && targetFaction && !casterFaction->IsFriendlyTo(*targetFaction)) || !targetFaction)
        gameObjTarget->ModifyHealth(-damage, caster, GetSpellInfo()->Id);
}

void Spell::EffectGameObjectRepair(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget)
        return;

    gameObjTarget->ModifyHealth(damage, m_caster);
}

void Spell::EffectGameObjectSetDestructionState(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!gameObjTarget || !m_originalCaster)
        return;

    Player* player = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself();
    gameObjTarget->SetDestructibleState(GameObjectDestructibleState(m_spellInfo->Effects[effIndex].MiscValue), player, true);
}

void Spell::SummonGuardian(uint32 i, uint32 entry, SummonPropertiesEntry const* properties, uint32 numGuardians, ObjectGuid privateObjectOwner)
{
    Unit* caster = m_originalCaster;
    if (!caster)
        return;

    if (caster->IsTotem())
        caster = caster->ToTotem()->GetOwner();

    // in another case summon new
    uint8 level = caster->GetLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if (m_CastItem && caster->GetTypeId() == TYPEID_PLAYER)
        if (ItemTemplate const* proto = m_CastItem->GetTemplate())
            if (proto->RequiredSkill == SKILL_ENGINEERING)
                if (uint16 skill202 = caster->ToPlayer()->GetSkillValue(SKILL_ENGINEERING))
                    level = skill202 / 5;

    float radius = 5.0f;
    int32 duration = m_spellInfo->GetDuration();

    if (Player* modOwner = m_originalCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

    //TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
    Map* map = caster->GetMap();

    for (uint32 count = 0; count < numGuardians; ++count)
    {
        Position pos;
        if (count == 0)
            pos = *destTarget;
        else
            // randomize position for multiple summons
            pos = m_caster->GetRandomPoint(*destTarget, radius);

        TempSummon* summon = map->SummonCreature(entry, pos, properties, duration, caster, m_spellInfo->Id, 0, privateObjectOwner);
        if (!summon)
            return;
        if (summon->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
            ((Guardian*)summon)->InitStatsForLevel(level);

        if (properties && properties->Category == SUMMON_CATEGORY_ALLY)
            summon->SetFaction(caster->GetFaction());

        if (summon->HasUnitTypeMask(UNIT_MASK_MINION) && m_targets.HasDst())
            ((Minion*)summon)->SetFollowAngle(m_caster->GetAngle(summon));

        switch (summon->GetEntry())
        {
            case 27893:
                if (uint32 weapon = m_caster->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + 16))
                {
                    summon->SetDisplayId(11686);
                    summon->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, weapon);
                }
                else
                    summon->SetDisplayId(1126);
                break;
        }

        auto startAttackIfNeed = [=](Unit* target)
        {
            if (!target || summon->IsInCombat())
                return;
            if (summon->IsValidAttackTarget(target) && summon->IsAIEnabled && summon->AI()->CanAIAttack(target))
                summon->AI()->AttackStart(target);
        };

        if (m_spellInfo->GetExplicitTargetMask() & TARGET_FLAG_UNIT_ENEMY)
            startAttackIfNeed(m_targets.GetUnitTarget());

        if (Spell* spell = caster->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            startAttackIfNeed(spell->GetExplTargetUnit());

        if (Spell* spell = caster->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            startAttackIfNeed(spell->GetExplTargetUnit());

        if (caster->IsInCombat())
        {
            startAttackIfNeed(caster->GetVictim());
            if (caster->GetTypeId() == TYPEID_PLAYER)
                startAttackIfNeed(ObjectAccessor::GetUnit(*caster, caster->ToPlayer()->GetTarget()));

            if (!summon->IsInCombat())
                startAttackIfNeed(summon->SelectVictim());
        }

        if (!summon->IsInCombat() && summon->IsAIEnabled)
            summon->AI()->EnterEvadeMode();

        ExecuteLogEffectSummonObject(i, summon);
    }
}

TempSummon* Spell::SummonTotem(uint32 entry, SummonPropertiesEntry const* properties, uint32 duration, ObjectGuid privateObjectOwner)
{
    auto summon = m_caster->GetMap()->SummonCreature(entry, *destTarget, properties, duration, m_originalCaster, m_spellInfo->Id, 0, privateObjectOwner);
    if (!summon || !summon->IsTotem())
        return nullptr;

    // Mana Tide Totem
    if (m_spellInfo->Id == 16190)
        damage = m_caster->CountPctFromMaxHealth(10);
    else if (entry == 59172) // Wild Mushroom: Plague
        damage = 5;
    else if (properties->Type != SUMMON_TYPE_TOTEM)
        damage = m_caster->CountPctFromMaxHealth(50);

    if (damage)                                            // if not spell info, DB values used
    {
        if (AuraEffect const* glyph = m_caster->GetAuraEffect(63298, EFFECT_0)) // Glyph of Totemic Vigor
            damage += m_caster->CountPctFromMaxHealth(glyph->GetAmount());
        summon->SetMaxHealth(damage);
        summon->SetHealth(damage);
    }
    return summon;
}

void Spell::EffectRenamePet(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT ||
        !unitTarget->ToCreature()->IsPet() || ((Pet*)unitTarget)->getPetType() != HUNTER_PET)
        return;

    unitTarget->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);
}

void Spell::EffectForcePlayerInteraction(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Unit* caster = unitTarget;
    if (!caster)
        return;

    // Couple triggers have TARGET_DEST_TARGET_ANY TARGET_UNIT_TARGET_ANY so I don't exactly know
    caster->CastSpell(m_caster, m_spellInfo->Effects[effIndex].TriggerSpell);
}

void Spell::EffectPlayMusic(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 soundid = m_spellInfo->Effects[effIndex].MiscValue;

    if (!sSoundEntriesStore.LookupEntry(soundid))
    {
        TC_LOG_ERROR("spells", "EffectPlayMusic: Sound (Id: %u) not exist in spell %u.", soundid, m_spellInfo->Id);
        return;
    }

    unitTarget->ToPlayer()->SendDirectMessage(WorldPackets::Misc::PlayMusic(soundid).Write());
}

void Spell::EffectSpecCount(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->UpdateSpecCount(damage);
}

void Spell::EffectActivateSpec(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->ActivateSpec(damage-1);  // damage is 1 or 2, spec is 0 or 1
}

void Spell::EffectPlaySound(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    Player* player = unitTarget->ToPlayer();
    if (!player)
        return;

    switch (m_spellInfo->Id)
    {
        case 91604: // Restricted Flight Area
            player->GetSession()->SendNotification(LANG_ZONE_NOFLYZONE);
            break;
        default:
            break;
    }

    uint32 soundId = m_spellInfo->Effects[effIndex].MiscValue;

    if (!sSoundEntriesStore.LookupEntry(soundId))
    {
        TC_LOG_ERROR("spells", "EffectPlaySound: Sound (Id: %u) not exist in spell %u.", soundId, m_spellInfo->Id);
        return;
    }

    player->PlayDirectSound(soundId, player);
}

void Spell::EffectRemoveAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;
    // there may be need of specifying casterguid of removed auras
    unitTarget->RemoveAurasDueToSpell(m_spellInfo->Effects[effIndex].TriggerSpell);
}

void Spell::EffectDamageFromMaxHealthPCT(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    damage = unitTarget->CountPctFromMaxHealth(damage);
    m_damage += damage;
    m_unmitigatedDamage += damage;
}

void Spell::EffectGiveCurrency(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->ModifyCurrency(m_spellInfo->Effects[effIndex].MiscValue, damage);
}

void Spell::EffectDestroyItem(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();
    uint32 itemId = m_spellInfo->Effects[effIndex].ItemType;

    if (Item* item = player->GetItemByEntry(itemId))
        player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
}

void Spell::EffectCastButtons(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = m_caster->ToPlayer();
    uint32 button_id = m_spellInfo->Effects[effIndex].MiscValue + 132;
    uint32 n_buttons = m_spellInfo->Effects[effIndex].MiscValueB;

    for (; n_buttons; --n_buttons, ++button_id)
    {
        ActionButton const* ab = p_caster->GetActionButton(button_id);
        if (!ab || ab->GetType() != ACTION_BUTTON_SPELL)
            continue;

        //! Action button data is unverified when it's set so it can be "hacked"
        //! to contain invalid spells, so filter here.
        uint32 spell_id = ab->GetAction();
        if (!spell_id)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
        if (!spellInfo)
            continue;

        if (!p_caster->HasSpell(spell_id) || p_caster->HasSpellCooldown(spell_id))
            continue;

        if (!(spellInfo->AttributesEx9 & SPELL_ATTR9_SUMMON_PLAYER_TOTEM))
            continue;

        int32 cost = spellInfo->CalcPowerCost(m_caster, spellInfo->GetSchoolMask());
        if (m_caster->GetPower(POWER_MANA) < cost)
            continue;

        TriggerCastFlags triggerFlags = TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_CAST_DIRECTLY);
        m_caster->CastSpell(m_caster, spell_id, triggerFlags);
    }
}

void Spell::EffectRechargeManaGem(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    if (!player)
        return;

    uint32 item_id = m_spellInfo->Effects[EFFECT_0].ItemType;

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(item_id);
    if (!pProto)
    {
        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    if (Item* pItem = player->GetItemByEntry(item_id))
    {
        for (int x = 0; x < MAX_ITEM_PROTO_SPELLS; ++x)
            pItem->SetSpellCharges(x, pProto->Spells[x].SpellCharges);
        pItem->SetState(ITEM_CHANGED, player);
    }
}

void Spell::EffectBind(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = unitTarget->ToPlayer();

    WorldLocation homeLoc;
    uint32 areaId = player->GetAreaId();

    if (m_spellInfo->Effects[effIndex].MiscValue)
        areaId = m_spellInfo->Effects[effIndex].MiscValue;

    if (m_targets.HasDst())
        homeLoc.WorldRelocate(*destTarget);
    else
    {
        homeLoc = player->GetWorldLocation();
        homeLoc.m_mapId = player->GetMapId();
    }

    player->SetHomebind(homeLoc, areaId);
    player->SendBindPointUpdate();

    TC_LOG_DEBUG("spells", "EffectBind: New homebind X: %f, Y: %f, Z: %f, MapId: %u, AreaId: %u",
        homeLoc.GetPositionX(), homeLoc.GetPositionY(), homeLoc.GetPositionZ(), homeLoc.GetMapId(), areaId);

    ObjectGuid guid = m_caster->GetGUID();

    // zone update
    WorldPacket data(SMSG_PLAYERBOUND, 1 + 8 + 4);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);

    data << uint32(areaId);

    player->SendDirectMessage(&data);
}

void Spell::EffectSummonRaFFriend(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER || !unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    m_caster->CastSpell(unitTarget, m_spellInfo->Effects[effIndex].TriggerSpell, true);
}

void Spell::EffectUnlockGuildVaultTab(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    // Safety checks done in Spell::CheckCast
    Player* caster = m_caster->ToPlayer();
    if (Guild* guild = caster->GetGuild())
        guild->HandleBuyBankTab(caster->GetSession(), m_spellInfo->Effects[effIndex].BasePoints - 1); // Bank tabs start at zero internally
}

void Spell::EffectResurrectWithAura(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* target = unitTarget ? unitTarget->ToPlayer() : nullptr;
    if (!unitTarget && corpseTarget)
        target = ObjectAccessor::FindConnectedPlayer(corpseTarget->GetOwnerGUID());

    if (!target)
        return;

    ObjectGuid playerGuid = target->GetGUID();
    ObjectGuid casterGuid = m_caster->GetGUID();
    uint32 effectValue = damage;
    SpellInfo const* spellInfo = m_spellInfo;

    // Shitty workaround but still better than unsafe access
//     TaskMgr::Default()->ScheduleInvocation([playerGuid, casterGuid, effectValue, spellInfo, effIndex]
//     {
//         Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid);
//         Player* caster = ObjectAccessor::FindConnectedPlayer(casterGuid);
//         if (!player || !caster)
//             return;

//         if (player->IsRessurectRequested())       // already have one active request
//             return;

//         uint32 health = player->CountPctFromMaxHealth(effectValue);
//         uint32 mana = CalculatePct(player->GetMaxPower(POWER_MANA), effectValue);

//         uint32 resurrectAura = 0;
//         if (sSpellMgr->GetSpellInfo(spellInfo->Effects[effIndex].TriggerSpell))
//             resurrectAura = spellInfo->Effects[effIndex].TriggerSpell;
//         if (resurrectAura && player->HasAura(resurrectAura))
//             return;

//         player->SetResurrectRequestData(caster, health, mana, resurrectAura);
// #pragma warning(push)
// #pragma warning(disable : 4573)
//         Spell::SendResurrectRequest(caster, player, spellInfo->Id);
// #pragma warning(pop)
//     });

        Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid);
        Player* caster = ObjectAccessor::FindConnectedPlayer(casterGuid);
        if (!player || !caster)
            return;

        if (player->IsRessurectRequested())       // already have one active request
            return;

        uint32 health = player->CountPctFromMaxHealth(effectValue);
        uint32 mana = CalculatePct(player->GetMaxPower(POWER_MANA), effectValue);

        uint32 resurrectAura = 0;
        if (sSpellMgr->GetSpellInfo(spellInfo->Effects[effIndex].TriggerSpell))
            resurrectAura = spellInfo->Effects[effIndex].TriggerSpell;
        if (resurrectAura && player->HasAura(resurrectAura))
            return;

        player->SetResurrectRequestData(caster, health, mana, resurrectAura);
#pragma warning(push)
#pragma warning(disable : 4573)
        Spell::SendResurrectRequest(caster, player, spellInfo->Id);
#pragma warning(pop)

    
    ExecuteLogEffectResurrect(effIndex, target);
}

void Spell::EffectCreateAreaTrigger(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Position pos;
    if (!m_targets.HasDst())
        pos = GetCaster()->GetPosition();
    else
        pos = destTarget->GetPosition();

    // trigger entry/miscvalue relation is currently unknown, for now use MiscValue as trigger entry
    uint32 triggerEntry = GetSpellInfo()->Effects[effIndex].MiscValue;

    AreaTrigger * areaTrigger = new AreaTrigger;
    if (!areaTrigger->CreateAreaTrigger(GetCaster()->GetMap()->GenerateLowGuid<HighGuid::AreaTrigger>(), triggerEntry, GetCaster(), GetSpellInfo(), pos))
        delete areaTrigger;
}

void Spell::EffectRemoveTalent(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = GetCaster()->ToPlayer();

    // Blizz sends talentId as glyphIndex
    if (player)
        player->RemoveTalent(m_glyphIndex);
}

void Spell::EffectHealBattlePet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget)
        return;

    if (sPetBattleSystem->GetPlayerPetBattle(unitTarget->GetGUID()))
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    auto& battlePetMgr = unitTarget->ToPlayer()->GetBattlePetMgr();
    for (auto&& battlePet : battlePetMgr.BattlePets)
    {
        // don't heal pets marked for deletion
        if (battlePet->GetDbState() == BATTLE_PET_DB_STATE_DELETE)
            continue;

        // don't heal pets already on full health
        if (battlePet->GetCurrentHealth() == battlePet->GetMaxHealth())
            continue;

        // heal and notify client
        battlePet->SetCurrentHealth(battlePet->GetMaxHealth());
        battlePetMgr.SendBattlePetUpdate(battlePet, false);
    }
}

void Spell::EffectBattlePetsUnlock(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = GetCaster()->ToPlayer();
    if (!player)
        return;

    BattlePetMgr& battlePetMgr = player->GetBattlePetMgr();

    if (player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_BATTLE_PET_ENABLED) && battlePetMgr.HasLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_1))
        return;

    player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_BATTLE_PET_ENABLED);

    player->LearnSpell(SPELL_BATTLE_PET_TRAINING_PASSIVE, false);
    player->LearnSpell(SPELL_TRACK_PETS, false);
    player->LearnSpell(SPELL_REVIVE_BATTLE_PETS, false);

    if (!battlePetMgr.HasLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_1))
        battlePetMgr.UnlockLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_1);
}

void Spell::EffectUncageBattlePet(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    if (!m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = m_caster->ToPlayer();

    uint32 speciesId = m_CastItem->GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_SPECIES);
    uint16 breed     = m_CastItem->GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_BREED) & 0xFFFFFF;
    uint8 quality    = (m_CastItem->GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_BREED) >> 24) & 0xFF;
    uint16 level     = m_CastItem->GetDynamicUInt32Value(ITEM_DYNAMIC_MODIFIERS, ITEM_MODIFIER_INDEX_BATTLE_PET_LEVEL);

    BattlePetSpeciesEntry const* speciesEntry = sBattlePetSpeciesStore.LookupEntry(speciesId);
    if (!speciesEntry)
        return;

    BattlePetMgr& battlePetMgr = player->GetBattlePetMgr();

    uint8 maxLearnedLevel = 0;
    for (auto&& pet : battlePetMgr.BattlePets)
        maxLearnedLevel = std::max(pet->GetLevel(), maxLearnedLevel);

    // TODO: This means if you put your highest lvl pet into cage, you won't be able to uncage it again which is probably wrong.
    // We will need to store maxLearnedLevel somewhere to avoid this behaviour.
    if (maxLearnedLevel < level)
    {
        player->SendGameError(GameError::ERR_PET_TOO_HIGH_LEVEL_TO_UNCAGE, speciesEntry->NpcId);
        SendCastResult(SPELL_FAILED_CANT_ADD_BATTLE_PET);
        return;
    }

    if (!battlePetMgr.CanStoreBattlePet(speciesId))
    {
        player->SendGameError(GameError::ERR_CANT_HAVE_MORE_PETS_OF_THAT_TYPE, speciesEntry->NpcId);
        SendCastResult(SPELL_FAILED_CANT_ADD_BATTLE_PET);
        return;
    }

    battlePetMgr.Create(speciesId, level, breed, quality);
    // After, don't change
    if (uint32 spell = db2::GetBattlePetSummonSpell(speciesId))
        player->LearnSpell(spell, false);
    player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);
    m_CastItem = nullptr;
}

void Spell::EffectPlaySceneId(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = GetCaster()->ToPlayer();
    if (!player)
        return;

    SceneTemplate const* sceneTemplate = sObjectMgr->GetSceneTemplate(m_spellInfo->Effects[effIndex].MiscValue);
    if (!sceneTemplate)
        return;

    player->GetSceneMgr().PlayScene(sceneTemplate->SceneId);
}

void Spell::EffectPlayScenePackage(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = GetCaster()->ToPlayer();
    if (!player)
        return;

    uint32 packageId = m_spellInfo->Effects[effIndex].MiscValue;
    if (!packageId)
        return;

    player->GetSceneMgr().PlaySceneByPackageId(packageId, SCENEFLAG_UNK1);
}

void Spell::EffectRandomizeDigsite(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = GetCaster()->ToPlayer();
    if (!player)
        return;

    uint32 map = GetSpellInfo()->Effects[effIndex].MiscValue;
    player->RandomizeDigsites(map);
}

void Spell::EffectBonusRoll(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    Player* player = GetCaster()->ToPlayer();
    if (!player)
        return;

    if (m_CastItem)
    {
        BonusLootTemplate lootTemplate;
        lootTemplate.LootIdPersonal = m_CastItem->GetEntry();
        lootTemplate.Source = BonusLootTemplate::Item;
        lootTemplate.Spell = GetSpellInfo()->Id;
        lootTemplate.LootIdHeroic = 0;
        lootTemplate.LootIdNormal = 0;
        lootTemplate.Currency = 0;

        BonusLoot loot{ &lootTemplate, REGULAR_DIFFICULTY };
        if (loot)
            loot.Reward(player);
        else
            TC_LOG_ERROR("shitlog", "Spell::EffectBonusRoll - !loot, %u, " UI64FMTD, GetSpellInfo()->Id, m_caster->GetGUID().GetRawValue());
    }
    else
    {
        AuraEffect* triggeredByAura = nullptr;
        for (auto&& it : GetCaster()->GetAuraEffectsByType(SPELL_AURA_BONUS_ROLL_TRIGGER))
            if (it->GetSpellEffectInfo().TriggerSpell == GetSpellInfo()->Id)
                triggeredByAura = it;

        if (!triggeredByAura)
            return;

        // Blizzlike lootId is in the value of this spell effect (for different difficulty)
        Difficulty difficulty = REGULAR_DIFFICULTY;
        uint32 currency = 0;
        BonusLootTemplate const* bonusLoot = sLootMgr->GetBonusLootForSpell(triggeredByAura->GetId());
        if (bonusLoot)
        {
            if (bonusLoot->Spell == triggeredByAura->GetId())
            {
                if (player->HasCurrency(bonusLoot->Currency, 1))
                {
                    currency = bonusLoot->Currency;
                    difficulty = Difficulty(triggeredByAura->GetAmount());
                }
                else
                    TC_LOG_ERROR("shitlog", "Spell::EffectBonusRoll - !player->HasCurrency(bonusLoot->Currency, 1) %u, " UI64FMTD, GetSpellInfo()->Id, triggeredByAura->GetCasterGUID().GetRawValue());
            }
            else
                TC_LOG_ERROR("shitlog", "Spell::EffectBonusRoll - bonusLoot->Spell != triggeredByAura->GetId() %u, " UI64FMTD, GetSpellInfo()->Id, triggeredByAura->GetCasterGUID().GetRawValue());
        }
        else
            TC_LOG_ERROR("shitlog", "Spell::EffectBonusRoll - !bonusLoot %u, " UI64FMTD, GetSpellInfo()->Id, triggeredByAura->GetCasterGUID().GetRawValue());

        if (currency)
        {
            BonusLoot loot{ bonusLoot, difficulty };
            if (loot)
            {
                loot.Reward(player);
                player->ModifyCurrency(currency, -1);
                player->AddLootLockout(LootLockoutType::BonusLoot, bonusLoot->Entry, difficulty);
            }
            else
                TC_LOG_ERROR("shitlog", "Spell::EffectBonusRoll - !loot %u, " UI64FMTD, GetSpellInfo()->Id, triggeredByAura->GetCasterGUID().GetRawValue());
        }

        player->RemoveAura(triggeredByAura->GetBase());
    }
}

void Spell::EffectPlayerChoice(SpellEffIndex effIndex)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    Player* player = GetCaster()->ToPlayer();
    if (!player)
        return;

    uint32 choiceId = GetSpellInfo()->Effects[effIndex].MiscValue;
    if (!choiceId)
        return;

    player->GetSession()->SendPlayerChoice(choiceId);
}

void Spell::EffectUpdatePlayerPhase(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->UpdateAreaAndZonePhase();
}

void Spell::EffectUpdateZoneAurasAndPhases(SpellEffIndex /*effIndex*/)
{
    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT_TARGET)
        return;

    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    unitTarget->ToPlayer()->UpdateAreaDependentAuras(unitTarget->GetAreaId());
}

void Spell::EffectSummonPersonalGameObject(SpellEffIndex effIndex)
{

    if (effectHandleMode != SPELL_EFFECT_HANDLE_HIT)
        return;

    uint32 goId = m_spellInfo->Effects[effIndex].MiscValue;
    if (!goId)
        return;

    float x, y, z;
    if (m_targets.HasDst())
        destTarget->GetPosition(x, y, z);
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_PLAYER_BOUNDING_RADIUS);

    Map* map = m_caster->GetMap();
    Position pos = Position(x, y, z, m_caster->GetOrientation());
    //QuaternionData rot = QuaternionData::fromEulerAnglesZYX(m_caster->GetOrientation(), 0.f, 0.f);
    GameObject* go = new GameObject();

    if (!go->Create(map->GenerateLowGuid<HighGuid::GameObject>(), goId, map, m_caster->GetPhaseMask(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 
        pos.GetOrientation(), { }/*rot*/, 100, GO_STATE_READY))
    {
        TC_LOG_WARN("spells", "SpellEffect Failed to summon personal gameobject. SpellId %u, effect %u", m_spellInfo->Id, effIndex);
        delete go;
        return;
    }

    go->AddToTransportIfNeeded(m_caster->GetTransport());

    //PhasingHandler::InheritPhaseShift(go, m_caster);
    for (auto phase : m_caster->GetPhases())
        go->SetPhased(phase, false, true);

    int32 duration = m_spellInfo->GetDuration();

    go->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    go->SetSpellId(m_spellInfo->Id);
    go->SetPrivateObjectOwner(m_caster->GetGUID());

    ExecuteLogEffectSummonObject(effIndex, go);

    map->AddToMap(go);

    if (uint32 linkedEntry = go->GetGOInfo()->GetLinkedGameObjectEntry())
    {
        GameObject* linkedTrap = new GameObject;

        if (linkedTrap->Create(map->GenerateLowGuid<HighGuid::GameObject>(), linkedEntry, map, m_caster->GetPhaseMask(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(),
            pos.GetOrientation(), { }/*rot*/, 255, GO_STATE_READY))
        {
            linkedTrap->AddToTransportIfNeeded(m_caster->GetTransport());

            //PhasingHandler::InheritPhaseShift(linkedTrap, m_caster);
            for (auto phase : m_caster->GetPhases())
                linkedTrap->SetPhased(phase, false, true);

            linkedTrap->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
            linkedTrap->SetSpellId(m_spellInfo->Id);

            ExecuteLogEffectSummonObject(effIndex, linkedTrap);

            map->AddToMap(linkedTrap);
        }
        else
        {
            TC_LOG_WARN("spells", "SpellEffect Failed to summon linked gameobject. SpellId %u, effect %u", m_spellInfo->Id, effIndex);
            delete linkedTrap;
            linkedTrap = nullptr;
            return;
        }
    }
}
