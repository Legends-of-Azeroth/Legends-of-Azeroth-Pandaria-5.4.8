/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "mogu_shan_vault.h"
#include "ScriptMgr.h"
#include "CreatureTextMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "SpellScript.h"

enum eSpells
{
    SPELL_BANISHMENT                                = 116272,
    SPELL_SOUL_CUT_SUICIDE                          = 116278,
    SPELL_SOUL_CUT_DAMAGE                           = 117337,
                                                    
    SPELL_TAP_THE_SPIRIT_WORLD                      = 117002, // Only cosmetic
    SPELL_CHANNEL_SPIRIT_TOTEM                      = 116833, // Only cosmetic
    SPELL_STRONG_MOJO                               = 115964, // Only cosmetic
    SPELL_VOODOO_DOLL_VISUAL                        = 122151,
    SPELL_VOODOO_DOLL_SHARE                         = 116000,
    SPELL_VOODOO_DOLL_FIXATE                        = 115999,
    SPELL_SUMMON_SPIRIT_TOTEM                       = 116174,
    SPELL_COSMETIC_SUMMON                           = 118086, // Only cosmetic - Tiki mask
    SPELL_SOUL_CUTTER_COSMETIC                      = 120677, // Only cosmetic - Tiki mask too
    SPELL_SUMMON_MINION                             = 118087,
                                                    
    // attaques ombreuses                           
    SPELL_RIGHT_CROSS                               = 117215,
    SPELL_LEFT_HOOK                                 = 117218,
    SPELL_HAMMER_FIST                               = 117219,
    SPELL_SWEEPING_KICK                             = 117222,
                                                    
    SPELL_FRENESIE                                  = 117752,
                                                    
    // Shadowy Minion                               
    SPELL_SHADOW_BOLT                               = 122118,
    SPELL_SPIRITUAL_GRASP                           = 118421,
    SPELL_SPIRITUAL_GRASP_EFF                       = 115982,
                                                    
    // Misc                                         
    SPELL_CLONE                                     = 119051,
    SPELL_CLONE_VISUAL                              = 119053,
    SPELL_LIFE_FRAGILE_THREAD                       = 116227,
    SPELL_CROSSED_OVER                              = 116161, // Todo : virer le summon
    SPELL_REVITALIZED_SPIRIT                        = 120717,
    SPELL_COSMETIC_RED_AURA                         = 118265,
                                                    
    SPELL_FRAIL_SOUL                                = 117723, // Heroic
    SPELL_SOUL_EXPLOSION                            = 120639,
    SPELL_ROOT_FOR_EVER                             = 31366,
                                                    
    // Death Event                                  
    SPELL_RELEASE_SPIRIT                            = 118580, // Summon Gara'jal Ghost
    SPELL_SPIRIT_HOVER                              = 75333,
    SPELL_GHOST_VISUAL                              = 91218,
    SPELL_SPIRITUAL_PORTAL                          = 118581, // Summon Spiritual Portal
    SPELL_VISUAL_PORTAL                             = 118661, // Portal Visual
                                                    
    // Enrage                                       
    SPELL_FINAL_DESTINATION                         = 118469,
    SPELL_INSTANTLY_DEATH                           = 118443,

    // Player spirit link (buff at overheal in crossed over)
    SPELL_SPIRITUAL_INNERVATION_DPS                 = 117549,
    SPELL_SPIRITUAL_INNERVATION_HEALER              = 117543,
    SPELL_SPIRITUAL_INNERVATION_GUARDIAN_DROOD      = 117544, // also warrior
    SPELL_SPIRITUAL_INNERVATION_BALANCE_DROOD       = 121069,
    SPELL_SPIRITUAL_INNERVATION_ENCH_SHAMY          = 117687,
    SPELL_SPIRITUAL_INNERVATION_PALADIN             = 117688,
    SPELL_SPIRITUAL_INNERVATION_MONK                = 117548,
    SPELL_SPIRITUAL_INNERVATION_ELEMENTAL_SHAMY     = 121070,
    SPELL_SPIRITUAL_INNERVATION_SHADOW_PRIEST       = 121072,
    SPELL_SPIRITUAL_INNERVATION_DEMO_WARLOCK        = 121071,
    SPELL_SPIRITUAL_INNERVATION_DESTRUCTION_WARLOCK = 121089,
    SPELL_SPIRITUAL_INNERVATION_AFFLI_WARLOCK       = 121091,
    SPELL_SPIRITUAL_INNERVATION_HUNTER              = 117547,
    SPELL_SPIRITUAL_INNERVATION_DEATH_KNIGHT        = 117546,
    SPELL_SPIRITUAL_INNERVATION_FIRE_MAGE           = 121111,
    SPELL_SPIRITUAL_INNERVATION_FROST_MAGE          = 121112,
    SPELL_SPIRITUAL_INNERVATION_ENERGY              = 117545, // same... [possible brewmaster or...]
    SPELL_SPIRITUAL_INNERVATION_DISC_PRIEST         = 121068, // also holly

    SPELL_MOGU_RUNES_HORIZONTAL                     = 126038,
    SPELL_ACTIVATE_ARTIFACT                         = 126022,
    SPELL_SORRY_WERE_YOU_LOOKING_FOR_THIS_CRITERIA  = 125986,
};

enum eEvents
{
    EVENT_SECONDARY_ATTACK                          = 1,
    EVENT_SUMMON_TOTEM                              = 2,
    EVENT_SUMMON_SHADOWY_MINION                     = 3,
    EVENT_VOODOO_DOLL                               = 4,
    EVENT_BANISHMENT                                = 5,
                                                    
    // Shadowy Minion                               
    EVENT_SHADOW_BOLT                               = 6,
    EVENT_SPIRITUAL_GRASP                           = 7,
                                                    
    // Gara'Jal Ghost                               
    EVENT_GROWTH                                    = 8,
    EVENT_TALK_DEATH                                = 9,
    EVENT_SUMMON_PORTAL                             = 10,
    EVENT_DISAPPEAR                                 = 11,
                                                    
    // Enrage                                       
    EVENT_FINAL_DESTINATION                         = 12,
    EVENT_SOUL_EXPLOSION                            = 13,
                                                    
    // Wyvern                                       
    EVENT_ESCORT                                    = 14,

    // Misc
    EVENT_SUMMON_SHADOWY_MINION_2                   = 15,
};

enum GarajalTalk
{
    TALK_AGGRO                                      = 0,
    TALK_SLAY,
    TALK_FRENZY,
    TALK_FRENZY_ANN,
    TALK_CROSS_OVER_ANN,
    TALK_VOO_DOO_DOLLS_ANN,
    TALK_VOO_DOO_DOLLS_DMG_ANN,
};

enum GarajalGhostTalk
{
    TALK_DEATH                                      = 0
};

enum GarajalActions
{
    ACTION_FLY_IN_CENTER                            = 0,
    ACTION_GARAJAL_MOVE_DOWN                        = 1,
};

class VoodooTargetSelector
{
    public:
        VoodooTargetSelector() {}

        bool operator() (WorldObject* const& object)
        {
            return object && object->ToPlayer() && (object->ToPlayer()->HasAura(SPELL_VOODOO_DOLL_VISUAL) || object->ToPlayer()->GetRoleForGroup(object->ToPlayer()->GetTalentSpecialization()) == ROLES_TANK || object->ToPlayer()->IsGameMaster());
        }
};

Position const soulCutterPos[3] = 
{
    {4253.181152f, 1341.662354f, 454.153503f, 6.265248f},
    {4294.578613f, 1357.740356f, 454.153656f, 4.777716f},
    {4294.578613f, 1324.024536f, 454.153656f, 1.608639f},
};

// 60143 - Gara'jal the Spiritbinder
class boss_garajal : public CreatureScript
{
    public:
        boss_garajal() : CreatureScript("boss_garajal") { }

        struct boss_garajalAI : public BossAI
        {
            boss_garajalAI(Creature* creature) : BossAI(creature, DATA_GARAJAL) { }

            std::vector<ObjectGuid> voodooTargets;
            std::list<uint32> shadowySpells;
            bool intro;
            uint32 banishmentCount;

            void InitializeAI() override
            {
                switch (instance->GetData(DATA_GARAJAL))
                {
                    case DONE:
                        intro = true;
                        break;
                    case SPECIAL:
                        intro = true;
                        me->SetHomePosition(GarajalHomePos.GetPositionX(), GarajalHomePos.GetPositionY(), GarajalHomePos.GetPositionZ(), GarajalHomePos.GetOrientation());
                        me->GetMotionMaster()->MoveTargetedHome();
                        break;
                    default:
                        me->setActive(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_PASSIVE);
                        intro = false;
                        me->SetHomePosition(GarajalHomePos.GetPositionX(), GarajalHomePos.GetPositionY(), GarajalHomePos.GetPositionZ(), GarajalHomePos.GetOrientation());
                        break; 
                }

                InitializeShadowySpells();

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 117.8m in 10 normal
                    me->SetCreateHealth(117800000);
                    me->SetMaxHealth(117800000);
                    me->SetHealth(117800000);
                    me->ResetPlayerDamageReq();
                }

                Reset();
            }

            void Reset() override
            {
                _Reset();

                if (instance->GetBossState(DATA_GARAJAL) != DONE)
                    instance->SetBossState(DATA_GARAJAL, NOT_STARTED);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_FIXATE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLONE_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CROSSED_OVER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIFE_FRAGILE_THREAD);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_CUT_DAMAGE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_CUT_SUICIDE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BANISHMENT);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);

                me->AddAura(SPELL_STRONG_MOJO, me);
                me->CastSpell(me, SPELL_TAP_THE_SPIRIT_WORLD, true);

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 117.8m in 10 normal
                    me->SetCreateHealth(117800000);
                    me->SetMaxHealth(117800000);
                    me->SetHealth(117800000);
                    me->ResetPlayerDamageReq();
                }

                banishmentCount = 0;
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->SetBossState(DATA_GARAJAL, DONE);
                _JustDied();
                me->CastSpell(me, SPELL_RELEASE_SPIRIT, false);

                events.Reset();
                instance->SetBossState(DATA_GARAJAL, DONE);
                instance->SaveToDB();

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_FIXATE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLONE_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CROSSED_OVER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIFE_FRAGILE_THREAD);

                std::list<Creature*> choList;
                GetCreatureListWithEntryInGrid(choList, me, NPC_LOREWALKER_CHO, 200.0f);
                choList.remove_if([=](Creature* target) { return target->GetPositionZ() < 445.37f; });

                for (auto&& itr : choList)
                    itr->DespawnOrUnsummon();

                instance->SetData(DATA_GARAJAL, DONE);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_GARAJAL, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    who->NearTeleportTo(EncountersEntrance[0].GetPositionX(), EncountersEntrance[0].GetPositionY(), EncountersEntrance[0].GetPositionZ(), EncountersEntrance[0].GetOrientation());
                    return;
                }

                instance->SetBossState(DATA_GARAJAL, IN_PROGRESS);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                Talk(TALK_AGGRO);

                me->RemoveAura(SPELL_STRONG_MOJO);
                me->RemoveAura(SPELL_TAP_THE_SPIRIT_WORLD);

                me->InterruptNonMeleeSpells(false);

                events.ScheduleEvent(EVENT_SECONDARY_ATTACK, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SUMMON_TOTEM, 36 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SUMMON_SHADOWY_MINION, urand(3000, 8000));

                if (Is25ManRaid())
                    events.ScheduleEvent(EVENT_SUMMON_SHADOWY_MINION_2, urand(3000, 8000));

                events.ScheduleEvent(EVENT_BANISHMENT, 90000);
                events.ScheduleEvent(EVENT_VOODOO_DOLL, 2500);
                events.ScheduleEvent(EVENT_FINAL_DESTINATION, 361000); // 6 min & 10s
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_BANISHMENT)
                    return banishmentCount;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_BANISHMENT)
                    banishmentCount = data;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_GARAJAL_MOVE_DOWN)
                {
                    me->GetMotionMaster()->MoveJump(GarajalHomePos, 15.0f, 20.0f, EVENT_JUMP);
                    instance->SetData(DATA_GARAJAL, SPECIAL);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == EFFECT_MOTION_TYPE && pointId == EVENT_JUMP)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);    

                    // Remove All bombs in area if they not were explosive
                    std::list<Creature*> ExplosiveBombs;
                    GetCreatureListWithEntryInGrid(ExplosiveBombs, me, NPC_TROLL_EXPLOSIVES, VISIBLE_RANGE);

                    for (auto&& itr : ExplosiveBombs)
                        itr->DespawnOrUnsummon();
                }
            }
     
            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_FIXATE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLONE_VISUAL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CROSSED_OVER);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIFE_FRAGILE_THREAD);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_CUT_DAMAGE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_CUT_SUICIDE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BANISHMENT);
                instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                BossAI::EnterEvadeMode();
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                //_DespawnAtEvade();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!instance)
                    return;

                if (!me->HasAura(SPELL_FRENESIE))
                {
                    if (me->HealthBelowPctDamaged(20, damage))
                    {
                        me->CastSpell(me, SPELL_FRENESIE, true);
                        Talk(TALK_FRENZY_ANN);
                        events.CancelEvent(EVENT_SUMMON_TOTEM);
                        events.CancelEvent(EVENT_BANISHMENT);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!HasAnyZandalariInArea() && !intro)
                {
                    intro = true;

                    if (Creature* Wyvern = GetClosestCreatureWithEntry(me, NPC_ZANDALARI_WAR_WYVERN, 50.0f, true))
                    {
                        me->EnterVehicle(Wyvern, 0);
                        Wyvern->AI()->DoAction(ACTION_FLY_IN_CENTER);
                    }
                        
                }
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SECONDARY_ATTACK:
                        {
                            if (shadowySpells.empty())
                                InitializeShadowySpells();

                            uint32 spellId = GetRandomShadowySpells();
                            shadowySpells.erase(std::find(shadowySpells.begin(), shadowySpells.end(), spellId));
                            me->CastSpell((Unit*)NULL, spellId, true);
                            events.ScheduleEvent(EVENT_SECONDARY_ATTACK, 8 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SUMMON_TOTEM:
                        {
                            Talk(TALK_CROSS_OVER_ANN);
                            float x = 0.0f, y = 0.0f;
                            GetRandPosFromCenterInDist(4277.08f, 1341.35f, frand(0.0f, 30.0f), x, y);
                            me->SummonCreature(NPC_SPIRIT_TOTEM, x, y, 454.55f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                            events.ScheduleEvent(EVENT_SUMMON_TOTEM, 36 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_SUMMON_SHADOWY_MINION:
                        case EVENT_SUMMON_SHADOWY_MINION_2:
                        {
                            float x = 0.0f, y = 0.0f;
                            GetRandPosFromCenterInDist(4277.08f, 1341.35f, frand(0.0f, 30.0f), x, y);
                            me->SummonCreature(NPC_SHADOWY_MINION_REAL, x, y, 454.55f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                            events.ScheduleEvent(eventId, urand(9000, 12000));
                            break;
                        }
                        case EVENT_VOODOO_DOLL:
                        {

                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                            voodooTargets.clear();
                            Talk(TALK_VOO_DOO_DOLLS_ANN);

                            // select current tank as first
                            if (Unit* vict = me->GetVictim())
                            {
                                voodooTargets.push_back(vict->GetGUID());
                                Talk(TALK_VOO_DOO_DOLLS_DMG_ANN, vict);
                                vict->AddAura(SPELL_VOODOO_DOLL_VISUAL, vict);
                                DoCast(vict, SPELL_VOODOO_DOLL_FIXATE);
                            }

                            int32 mobCount = Is25ManRaid() ? 3 : 2;

                            // select another 2/3 targets
                            std::list<Player*> PlayersIn;
                            GetPlayerListInGrid(PlayersIn, me, 150.0f);

                            PlayersIn.remove_if(VoodooTargetSelector());
                            Trinity::Containers::RandomResizeList(PlayersIn, mobCount);

                            for (auto&& pItr : PlayersIn)
                            {
                                voodooTargets.push_back(pItr->GetGUID());
                                Talk(TALK_VOO_DOO_DOLLS_DMG_ANN, pItr);
                                pItr->AddAura(SPELL_VOODOO_DOLL_VISUAL, pItr);
                            }

                            if (Unit* tank = ObjectAccessor::GetPlayer(*me, voodooTargets[0]))
                                me->CastSpell(tank, SPELL_VOODOO_DOLL_SHARE, true);

                            break;
                        }
                        case EVENT_BANISHMENT:
                        {
                            banishmentCount = 0;
                            // Remove taunt immune till reset vodoo
                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);

                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                me->AddAura(SPELL_BANISHMENT,       target);
                                me->AddAura(SPELL_SOUL_CUT_SUICIDE, target);
                                me->AddAura(SPELL_SOUL_CUT_DAMAGE,  target);
                                target->AddAura(SPELL_SPIRITUAL_INNERVATION_DPS, target);

                                uint8 mobCount   = IsHeroic() ? 3: 1;

                                for (uint8 i = 0; i < mobCount; ++i)
                                {
                                    if (Creature* soulCutter = me->SummonCreature(NPC_SOUL_CUTTER, soulCutterPos[i], TEMPSUMMON_TIMED_DESPAWN, 30000ms))
                                    {
                                        if (IsHeroic() && i == 0)
                                            soulCutter->CastSpell(soulCutter, SPELL_COSMETIC_RED_AURA, true);

                                        soulCutter->SetPhaseMask(i == 0 ? 4 : 6, true);
                                        soulCutter->AI()->AttackStart(target);
                                        soulCutter->SetInCombatWith(target);
                                        soulCutter->GetThreatManager().addThreat(target, 10000.0f);
                                    }
                                }

                                me->GetThreatManager().addThreat(target, 0.0f);
                            }

                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_FIXATE);

                            events.ScheduleEvent(EVENT_VOODOO_DOLL, 5000);
                            events.ScheduleEvent(EVENT_BANISHMENT, 90000);
                            break;
                        }
                        case EVENT_FINAL_DESTINATION:
                        {
                            me->CastSpell(me, SPELL_FINAL_DESTINATION, false);
                            Talk(TALK_FRENZY);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 45.0f);
            }
            private:  
                void InitializeShadowySpells()
                {
                    shadowySpells.push_back(SPELL_RIGHT_CROSS);
                    shadowySpells.push_back(SPELL_LEFT_HOOK);
                    shadowySpells.push_back(SPELL_HAMMER_FIST);
                    shadowySpells.push_back(SPELL_SWEEPING_KICK);
                }

                uint32 GetRandomShadowySpells()
                {
                    return Trinity::Containers::SelectRandomContainerElement(shadowySpells);
                }

                bool HasAnyZandalariInArea()
                {
                    Creature* Skullcharger = GetClosestCreatureWithEntry(me, NPC_ZANDALARI_SKULLCHARGER, 150.0f, true);
                    Creature* FireDancer = GetClosestCreatureWithEntry(me, NPC_ZANDALARI_FIRE_DANCER, 150.0f, true);
                    Creature* Infiltrator = GetClosestCreatureWithEntry(me, NPC_ZANDALARI_INFILTRATOR, 150.0f, true);

                    if (!Skullcharger && !FireDancer && !Infiltrator)
                        return false;

                    return true;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_garajalAI(creature);
        }
};

// 61140 - Gara'jal the Spiritbinder
class npc_garajal_ghost : public CreatureScript
{
    public:
        npc_garajal_ghost() : CreatureScript("npc_garajal_ghost") { }

        struct npc_garajal_ghostAI : public ScriptedAI
        {
            npc_garajal_ghostAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            EventMap events;
            InstanceScript* instance;
            float ghostScale;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->AddAura(SPELL_GHOST_VISUAL, me);
                me->AddAura(SPELL_STRONG_MOJO, me);
                me->AddAura(SPELL_SPIRIT_HOVER, me);
                ghostScale = me->GetFloatValue(OBJECT_FIELD_SCALE);
                me->SetObjectScale(ghostScale / 10.0f);

                events.ScheduleEvent(EVENT_GROWTH,          200);
                events.ScheduleEvent(EVENT_TALK_DEATH,      4000);
                events.ScheduleEvent(EVENT_SUMMON_PORTAL,   8000);
                events.ScheduleEvent(EVENT_DISAPPEAR,       13000);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_SPIRITUAL_PORTAL)
                {
                    summon->CastSpell(summon, SPELL_VISUAL_PORTAL, true);
                    summon->SetReactState(REACT_PASSIVE);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GARAJAL) != DONE)
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GROWTH:
                            if (me->GetFloatValue(OBJECT_FIELD_SCALE) < ghostScale)
                            {
                                me->SetObjectScale(me->GetFloatValue(OBJECT_FIELD_SCALE) + (ghostScale / 10.0f));
                                events.ScheduleEvent(EVENT_GROWTH, 200);
                            }
                            break;
                        case EVENT_TALK_DEATH:
                            Talk(TALK_DEATH);
                            break;
                        case EVENT_SUMMON_PORTAL:
                            me->CastSpell(me, SPELL_SPIRITUAL_PORTAL, true);
                            break;
                        case EVENT_DISAPPEAR:
                            if (Creature* spiritualPortal = me->FindNearestCreature(NPC_SPIRITUAL_PORTAL, 50.0f))
                                spiritualPortal->DespawnOrUnsummon(5000);
                            me->DespawnOrUnsummon(500);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garajal_ghostAI(creature);
        }
};

// 60240 - Spirit Totem
class npc_spirit_totem : public CreatureScript
{
    public:
        npc_spirit_totem() : CreatureScript("npc_spirit_totem") { }

        struct npc_spirit_totemAI : public ScriptedAI
        {
            npc_spirit_totemAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset() override
            {
                me->AddAura(116827, me);
                me->AddAura(SPELL_ROOT_FOR_EVER, me);
                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 6.0f);

                uint8 count = 0;
                for (auto&& player: playerList)
                {
                    // Interrupt process if garajal already got frenzie
                    if (instance)
                        if (Creature* garajal = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_GARAJAL)))
                            if (garajal->HasAura(SPELL_FRENESIE))
                                break;

                    // If player has frail soul then just kill him
                    if (player->HasAura(SPELL_FRAIL_SOUL))
                    {
                        me->Kill(player);
                        continue;
                    }

                    // if player has vodoo then just ignore
                    if (player->HasAura(SPELL_VOODOO_DOLL_VISUAL))
                        continue;

                    if (++count > (Is25ManRaid() ? 5 : 3))
                        break;

                    if (Creature* clone = me->SummonCreature(56405, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation()))
                    {
                        clone->SetDisplayId(11686);

                        if (player->GetHealthPct() >= 10.0f)
                            player->SetHealth(player->GetMaxHealth() / 10);

                        player->CastSpell(player, SPELL_CLONE_VISUAL, true);
                        player->CastSpell(player, SPELL_CROSSED_OVER, true);

                        player->CastSpell(clone,  SPELL_CLONE, true);

                        clone->CastSpell(clone, SPELL_LIFE_FRAGILE_THREAD, true);
                        Position pos = { clone->GetPositionX(), clone->GetPositionY(), clone->GetPositionZ() + 10.0f };
                        clone->GetMotionMaster()->MoveTakeoff(1, pos);

                        player->CastSpell(player, SPELL_LIFE_FRAGILE_THREAD, true);

                        if (player->GetRoleForGroup(player->GetSpecialization()) == ROLES_HEALER)
                            player->AddAura(SPELL_SPIRITUAL_INNERVATION_HEALER, player);
                        else
                            player->AddAura(SPELL_SPIRITUAL_INNERVATION_DPS, player);
                    }
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance)
                    if (instance->GetBossState(DATA_GARAJAL) != IN_PROGRESS)
                        me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spirit_totemAI(creature);
        }
};

// 60184 - Shadowy Minion
// 60940 - Shadowy Minion
class npc_shadowy_minion : public CreatureScript
{
    public:
        npc_shadowy_minion() : CreatureScript("npc_shadowy_minion") { }

        struct npc_shadowy_minionAI : public ScriptedAI
        {
            npc_shadowy_minionAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            ObjectGuid spiritGuid;
            EventMap events;

            void Reset() override
            {
                events.Reset();
                spiritGuid = ObjectGuid::Empty;

                if (me->GetEntry() == NPC_SHADOWY_MINION_REAL)
                {
                    me->SetDisplayId(11686);

                    if (Creature* spirit = me->SummonCreature(NPC_SHADOWY_MINION_SPIRIT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN))
                    {
                        spiritGuid = spirit->GetGUID();
                        spirit->SetPhaseMask(2, true);
                    }

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                    events.ScheduleEvent(EVENT_SPIRITUAL_GRASP, urand(2000, 5000));
                }
                else
                    events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2000, 5000));

                me->AddAura(SPELL_COSMETIC_SUMMON, me);

                DoZoneInCombat();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_SHADOWY_MINION_SPIRIT)
                    me->DespawnOrUnsummon();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetEntry() == NPC_SHADOWY_MINION_SPIRIT)
                    if (me->ToTempSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (summoner->ToCreature())
                                summoner->ToCreature()->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance)
                    if (instance->GetBossState(DATA_GARAJAL) != IN_PROGRESS)
                        me->DespawnOrUnsummon();

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Spirit World
                        case EVENT_SHADOW_BOLT:
                        {
                            std::list<Player*> playerList;
                            GetPlayerListInGrid(playerList, me, 150.0f);

                            playerList.remove_if([=](Player* target) { return target->GetPhaseMask() != 2; });

                            if (playerList.empty())
                                return;

                            if (Unit* target = Trinity::Containers::SelectRandomContainerElement(playerList))
                                me->CastSpell(target, SPELL_SHADOW_BOLT, false);

                            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2000, 3000));
                            break;
                        }
                        // Real World
                        case EVENT_SPIRITUAL_GRASP:
                        {
                            std::list<Player*> playerList;
                            GetPlayerListInGrid(playerList, me, 150.0f);

                            playerList.remove_if([=](Player* target) { return target->GetPhaseMask() != 1; });

                            if (playerList.empty())
                                return;

                            if (Unit* target = Trinity::Containers::SelectRandomContainerElement(playerList))
                                me->CastSpell(target, SPELL_SPIRITUAL_GRASP, false);

                            events.ScheduleEvent(EVENT_SPIRITUAL_GRASP, urand(5000, 8000));
                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadowy_minionAI(creature);
        }
};

// 62003 - Severer of Souls
class npc_soul_cutter : public CreatureScript
{
    public:
        npc_soul_cutter() : CreatureScript("npc_soul_cutter") { }

        struct npc_soul_cutterAI : public ScriptedAI
        {
            npc_soul_cutterAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SOUL_EXPLOSION, 30000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    if (Creature* Garajal = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_GARAJAL)))
                    {
                        uint32 sCount = IsHeroic() ? 3 : 1;
                        if (Garajal->AI()->GetData(TYPE_BANISHMENT) + 1 < sCount)
                        {
                            Garajal->AI()->SetData(TYPE_BANISHMENT, Garajal->AI()->GetData(TYPE_BANISHMENT) + 1);
                            return;
                        }
                    }
                }

                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 300.0f);

                for (auto&& itr : playerList)
                {
                    itr->RemoveAurasDueToSpell(SPELL_BANISHMENT);
                    itr->RemoveAurasDueToSpell(SPELL_SOUL_CUT_SUICIDE);
                    itr->RemoveAurasDueToSpell(SPELL_SOUL_CUT_DAMAGE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance)
                    if (instance->GetBossState(DATA_GARAJAL) != IN_PROGRESS)
                        me->DespawnOrUnsummon();

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SOUL_EXPLOSION:
                        {
                            std::list<Player*> playerList;
                            GetPlayerListInGrid(playerList, me, 300.0f);
                            if (!playerList.empty())
                            {
                                Player* target = Trinity::Containers::SelectRandomContainerElement(playerList);
                                me->CastSpell(target, SPELL_SOUL_EXPLOSION, false);
                            }

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SOUL_EXPLOSION, 30000);

                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soul_cutterAI(creature);
        }
};

// 60512 - Spirit Totem
// 60513 - Spirit Totem
class npc_spirit_totem_intro : public CreatureScript
{
    public:
        npc_spirit_totem_intro() : CreatureScript("npc_spirit_totem_intro") { }

        struct npc_spirit_totem_introAI : public ScriptedAI
        {
            npc_spirit_totem_introAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                creature->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
            }

            InstanceScript* instance;

            void Reset() override
            {
                if (me->GetEntry() == NPC_SPIRIT_TOTEM_SKULL_INTRO)
                    me->SetDisplayId(11686);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (Unit* skull = me->FindNearestCreature(NPC_SPIRIT_TOTEM_SKULL_INTRO, 15))
                    if (!skull->HasAura(SPELL_CHANNEL_SPIRIT_TOTEM))
                        me->CastSpell(skull, SPELL_CHANNEL_SPIRIT_TOTEM, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spirit_totem_introAI(creature);
        }
};

// Garajal war wyvern 60385
class npc_garajal_war_wyvern : public CreatureScript
{
    public:
        npc_garajal_war_wyvern() : CreatureScript("npc_garajal_war_wyvern") { }

        struct npc_garajal_war_wyvernAI : public ScriptedAI
        {
            npc_garajal_war_wyvernAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events, nonCombatEvents;
            uint32 wp;

            void IsSummonedBy(Unit* summoner) override { }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                wp = 0;
            }

            void JustDied(Unit* /*killer*/) override { }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_FLY_IN_CENTER)
                {
                    nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(1000, 2000));
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 3:
                        if (Vehicle* Passengers = me->GetVehicleKit())
                            Passengers->RemoveAllPassengers(/*true*/); // TODO: do something with vehicles
                        if (Creature* Garajal = GetClosestCreatureWithEntry(me, NPC_GARAJAL, 10.0f, true))
                            Garajal->AI()->DoAction(ACTION_GARAJAL_MOVE_DOWN);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(1000, 2000));
                        break;
                    case 4:
                        me->DespawnOrUnsummon();
                        break;
                    default:
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, urand(100, 200));
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override { }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                nonCombatEvents.Update(diff);

                if (uint32 eventId = nonCombatEvents.ExecuteEvent())
                    if (eventId == EVENT_ESCORT)
                    {
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MovePoint(wp, GarajalTaxiWp[wp]);
                    }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_garajal_war_wyvernAI(creature);
        }
};

// Soul Back - 120715
class spell_soul_back : public SpellScriptLoader
{
    public:
        spell_soul_back() : SpellScriptLoader("spell_soul_back") { }

        class spell_soul_back_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_soul_back_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    // SPELL_LIFE_FRAGILE_THREAD removed by default effect
                    target->RemoveAurasDueToSpell(SPELL_CLONE_VISUAL);
                    target->RemoveAurasDueToSpell(SPELL_CROSSED_OVER);
                    if (target->GetMap()->IsHeroic())
                        target->AddAura(SPELL_FRAIL_SOUL, target);

                    // Todo : Jump le joueur lяПН ou яПНtait son corps
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_soul_back_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_REMOVE_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_soul_back_SpellScript();
        }
};

// Final Destination - 118469
class spell_final_destination : public SpellScriptLoader
{
    public:
        spell_final_destination() : SpellScriptLoader("spell_final_destination") { }

        class spell_final_destination_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_final_destination_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* Garajal = GetCaster())
                {
                    if (InstanceScript* instance = Garajal->GetInstanceScript())
                    {
                        Map::PlayerList const &playerList = instance->instance->GetPlayers();
                        for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                            if (Player* player = i->GetSource())
                                if (player->IsAlive())
                                    Garajal->CastSpell(player, SPELL_INSTANTLY_DEATH, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_final_destination_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_final_destination_AuraScript();
        }
};

// Voodoo Doll Fixate 115999
class spell_garajal_voodoo_doll_fixate : public SpellScriptLoader
{
    public:
        spell_garajal_voodoo_doll_fixate() : SpellScriptLoader("spell_garajal_voodoo_doll_fixate") { }

        class spell_garajal_voodoo_doll_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_garajal_voodoo_doll_fixate_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster() && GetCaster()->ToCreature() && GetOwner() && GetOwner()->ToUnit())
                {
                    // Apply taunt immune each time when choise new targets for vodoo
                    GetCaster()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    GetCaster()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                    GetCaster()->GetThreatManager().addThreat(GetOwner()->ToUnit(), 10000.0f);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_garajal_voodoo_doll_fixate_AuraScript::OnApply, EFFECT_0, SPELL_AURA_FIXATE_TARGET, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_garajal_voodoo_doll_fixate_AuraScript();
        }
};

// Spiritual Grasp 115982 [make handle aoe, no data or blizzard fail]
class spell_spiritual_grasp : public SpellScript
{
    PrepareSpellScript(spell_spiritual_grasp);

    void HandleHitEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->ToPlayer())
                return;

            if (Unit* target = GetHitUnit())
            {
                std::list<Player*> pList;
                GetPlayerListInGrid(pList, target, 3.0f);

                for (auto&& itr : pList)
                    itr->CastSpell(itr, SPELL_SPIRITUAL_GRASP_EFF, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spiritual_grasp::HandleHitEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 126022 - Activate Artifact
class spell_activate_artifact : public AuraScript
{
    PrepareAuraScript(spell_activate_artifact);

    bool Load() override
    {
        return GetOwner()->GetTypeId() == TYPEID_UNIT;
    }

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (uint32 count = GetUnitOwner()->GetAuraCount(GetSpellInfo()->Id))
        {
            GetAura()->SetMaxDuration(GetMaxDuration() / count);
            GetAura()->RefreshDuration(false);
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
        {
            if (auto instance = GetUnitOwner()->GetInstanceScript())
            {
                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_SORRY_WERE_YOU_LOOKING_FOR_THIS_CRITERIA, 0, GetUnitOwner());
                GetUnitOwner()->ToCreature()->DespawnOrUnsummon();
            }
        }
    }

    void Register() override
    {
        OnEffectApply  += AuraEffectApplyFn (spell_activate_artifact::HandleApply,  EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_activate_artifact::HandleRemove, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_garajal()
{
    new boss_garajal();
    new npc_garajal_ghost();
    new npc_spirit_totem();
    new npc_shadowy_minion();
    new npc_soul_cutter();
    new npc_spirit_totem_intro();
    new npc_garajal_war_wyvern();
    new spell_soul_back();
    new spell_final_destination();
    new spell_garajal_voodoo_doll_fixate();
    new spell_script<spell_spiritual_grasp>("spell_spiritual_grasp");
    new aura_script<spell_activate_artifact>("spell_activate_artifact");
}
