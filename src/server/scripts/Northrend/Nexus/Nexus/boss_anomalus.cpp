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

#include "CreatureTextMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "nexus.h"

enum Spells
{
    SPELL_SPARK                                   = 47751,
    SPELL_SPARK_H                                 = 57062,
    SPELL_RIFT_SHIELD                             = 47748,
    SPELL_CHARGE_RIFT                             = 47747, //Works wrong (affect players, not rifts)
    SPELL_CREATE_RIFT                             = 47743, //Don't work, using WA
    SPELL_ARCANE_ATTRACTION                       = 57063, //No idea, when it's used
};

enum Adds
{
    NPC_CRAZED_MANA_WRAITH                        = 26746,
    NPC_CHAOTIC_RIFT                              = 26918
};

enum AnomYells
{
    SAY_AGGRO                                     = 0,
    SAY_DEATH                                     = 1,
    SAY_RIFT                                      = 2,
    SAY_SHIELD                                    = 3,
    SAY_RIFT_EMOTE                                = 4,
    SAY_SHIELD_EMOTE                              = 5  // Needs to be added to script
};

enum RiftSpells
{
    SPELL_CHAOTIC_ENERGY_BURST                    = 47688,
    SPELL_CHARGED_CHAOTIC_ENERGY_BURST            = 47737,
    SPELL_ARCANEFORM                              = 48019, //Chaotic Rift visual
};

enum
{
    WORLD_STATE_CHAOS_THEORY = 6327,
};

Position const RiftLocation[6] =
{
    { 652.64f, -273.70f, -8.75f, 0.0f },
    { 634.45f, -265.94f, -8.44f, 0.0f },
    { 620.73f, -281.17f, -9.02f, 0.0f },
    { 626.10f, -304.67f, -9.44f, 0.0f },
    { 639.87f, -314.11f, -9.49f, 0.0f },
    { 651.72f, -297.44f, -9.37f, 0.0f }
};

#define DATA_CHAOS_THEORY                         1

class boss_anomalus : public CreatureScript
{
    public:
        boss_anomalus() : CreatureScript("boss_anomalus") { }

        struct boss_anomalusAI : public ScriptedAI
        {
            boss_anomalusAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

            uint8 Phase;
            uint32 uiSparkTimer;
            uint32 uiCreateRiftTimer;
            ObjectGuid uiChaoticRiftGUID;
            SummonList lSummons;

            void Reset() override
            {
                Phase = 0;
                uiSparkTimer = 5000;
                uiChaoticRiftGUID = ObjectGuid::Empty;
                lSummons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_ANOMALUS_EVENT, NOT_STARTED);
                me->GetMap()->SetWorldState(WORLD_STATE_CHAOS_THEORY, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_ANOMALUS_EVENT, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                lSummons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_ANOMALUS_EVENT, DONE);
            }

            void JustSummoned(Creature* summon) override
            {
                lSummons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 60.0f)
                {
                    // Not blizzlike, hack to avoid an exploit
                    EnterEvadeMode();
                    return;
                }

                if (me->HasAura(SPELL_RIFT_SHIELD))
                {
                    if (uiChaoticRiftGUID)
                    {
                        Creature* Rift = ObjectAccessor::GetCreature(*me, uiChaoticRiftGUID);
                        if (Rift && Rift->isDead())
                        {
                            me->RemoveAurasDueToSpell(SPELL_RIFT_SHIELD);
                            uiChaoticRiftGUID = ObjectGuid::Empty;
                        }
                        return;
                    }
                }
                else
                    uiChaoticRiftGUID = ObjectGuid::Empty;

                if ((Phase == 0) && HealthBelowPct(50))
                {
                    Phase = 1;
                    Talk(SAY_SHIELD);
                    DoCast(me, SPELL_RIFT_SHIELD);
                    if (Creature* Rift = me->SummonCreature(NPC_CHAOTIC_RIFT, RiftLocation[urand(0, 5)], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms))
                    {
                        sCreatureTextMgr->SendChat(me, SAY_RIFT_EMOTE, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL);

                        //DoCast(Rift, SPELL_CHARGE_RIFT);
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            Rift->AI()->AttackStart(target);
                        uiChaoticRiftGUID = Rift->GetGUID();
                        Talk(SAY_RIFT);
                    }
                }

                if (uiSparkTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SPARK);
                    uiSparkTimer = 5000;
                }
                else
                    uiSparkTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_anomalusAI(creature);
        }
};

class npc_chaotic_rift : public CreatureScript
{
    public:
        npc_chaotic_rift() : CreatureScript("npc_chaotic_rift") { }

        struct npc_chaotic_riftAI : public ScriptedAI
        {
            npc_chaotic_riftAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                instance = me->GetInstanceScript();
                SetCombatMovement(false);
            }

            InstanceScript* instance;

            uint32 uiChaoticEnergyBurstTimer;
            uint32 uiSummonCrazedManaWraithTimer;

            void Reset() override
            {
                uiChaoticEnergyBurstTimer = 1000;
                uiSummonCrazedManaWraithTimer = 5000;
                me->SetDisplayFromModel(1);
                DoCast(me, SPELL_ARCANEFORM, false);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiChaoticEnergyBurstTimer <= diff)
                {
                    Creature* Anomalus = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ANOMALUS));
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        if (Anomalus && Anomalus->HasAura(SPELL_RIFT_SHIELD))
                            DoCast(target, SPELL_CHARGED_CHAOTIC_ENERGY_BURST);
                        else
                            DoCast(target, SPELL_CHAOTIC_ENERGY_BURST);
                    }
                    uiChaoticEnergyBurstTimer = 1000;
                }
                else
                    uiChaoticEnergyBurstTimer -= diff;

                if (uiSummonCrazedManaWraithTimer <= diff)
                {
                    if (Creature* Wraith = me->SummonCreature(NPC_CRAZED_MANA_WRAITH, me->GetPositionX() + 1, me->GetPositionY() + 1, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000ms))
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            Wraith->AI()->AttackStart(target);
                    Creature* Anomalus = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ANOMALUS));
                    if (Anomalus && Anomalus->HasAura(SPELL_RIFT_SHIELD))
                        uiSummonCrazedManaWraithTimer = 5000;
                    else
                        uiSummonCrazedManaWraithTimer = 10000;
                }
                else
                    uiSummonCrazedManaWraithTimer -= diff;
            }

            void JustDied(Unit*) override
            {
                if (auto instance = me->GetInstanceScript())
                    if (instance->GetData(DATA_ANOMALUS_EVENT) == IN_PROGRESS)      // Who will rewrite this shit? nobody?...
                        me->GetMap()->SetWorldState(WORLD_STATE_CHAOS_THEORY, 0);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chaotic_riftAI(creature);
        }
};

void AddSC_boss_anomalus()
{
    new boss_anomalus();
    new npc_chaotic_rift();
}
