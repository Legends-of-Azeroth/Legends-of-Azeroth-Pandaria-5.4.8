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

#include "ScriptPCH.h"
#include "ahnkahet.h"

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_SLAY                                      = 1,
    SAY_DEATH                                     = 2,
    SAY_EGG_SAC                                   = 3,
    EMOTE_HATCHES                                 = 4,
};

enum Spells
{
    SPELL_BROOD_PLAGUE                            = 56130,
    SPELL_BROOD_PLAGUE_H                          = 59467,
    SPELL_BROOD_RAGE_H                            = 59465,
    SPELL_FEAR                                    = 30584,
    SPELL_ENRAGE                                  = 26662, // Enraged if too far away from home
    SPELL_SUMMON_SWARMERS                         = 56119, // 2x 30178  -- 2x every 10secs
    SPELL_SUMMON_SWARM_GUARD                      = 56120, // 1x 30176  -- every 25secs
};

enum Npcs
{
    NPC_AHNKAHAR_SWARMER                          = 30178,
    NPC_AHNKAHAR_GUARDIAN                         = 30176
};

enum Misc
{
    WORLD_STATE_RESPECT_YOUR_ELDERS               = 6328,
};

class boss_elder_nadox : public CreatureScript
{
    public:
        boss_elder_nadox() : CreatureScript("boss_elder_nadox") { }

        struct boss_elder_nadoxAI : public ScriptedAI
        {
            boss_elder_nadoxAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            uint32 uiPlagueTimer;
            uint32 uiRagueTimer;

            uint32 uiSwarmerSpawnTimer;
            uint32 uiGuardSpawnTimer;
            uint32 uiEnragueTimer;
            uint32 Fear_Timer;

            bool bGuardSpawned;

            InstanceScript* instance;

            void Reset() override
            {
                uiPlagueTimer = 13 * IN_MILLISECONDS;
                uiRagueTimer = 20 * IN_MILLISECONDS;

                uiSwarmerSpawnTimer = 10 * IN_MILLISECONDS;
                uiGuardSpawnTimer = 25 * IN_MILLISECONDS;

                uiEnragueTimer = 5 * IN_MILLISECONDS;
                Fear_Timer = 8 * IN_MILLISECONDS;

                bGuardSpawned = false;

                if (instance)
                    instance->SetData(DATA_ELDER_NADOX_EVENT, NOT_STARTED);

                me->GetMap()->SetWorldState(WORLD_STATE_RESPECT_YOUR_ELDERS, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_ELDER_NADOX_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_ELDER_NADOX_EVENT, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiPlagueTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(target, DUNGEON_MODE(SPELL_BROOD_PLAGUE, SPELL_BROOD_PLAGUE_H));
                    uiPlagueTimer = 15 * IN_MILLISECONDS;

                    uiPlagueTimer = 15 * IN_MILLISECONDS;
                } else uiPlagueTimer -= diff;

                if (IsHeroic())
                {
                    if (uiRagueTimer <= diff)
                    {
                        if (Creature* pSwarmer = me->FindNearestCreature(NPC_AHNKAHAR_SWARMER, 35))
                        {
                            DoCast(pSwarmer, SPELL_BROOD_RAGE_H, true);
                            uiRagueTimer = 15 * IN_MILLISECONDS;
                        }
                    } else uiRagueTimer -= diff;
                }

                if (uiSwarmerSpawnTimer <= diff)
                {
                    DoCast(me, SPELL_SUMMON_SWARMERS, true);
                    DoCast(me, SPELL_SUMMON_SWARMERS);
                    if (urand(1, 3) == 3) // 33% chance of dialog
                        Talk(SAY_EGG_SAC);

                    uiSwarmerSpawnTimer = 10 * IN_MILLISECONDS;
                } else uiSwarmerSpawnTimer -= diff;

                if (!bGuardSpawned && uiGuardSpawnTimer <= diff)
                {
                    Talk(EMOTE_HATCHES);
                    DoCast(me, SPELL_SUMMON_SWARM_GUARD);
                    bGuardSpawned = true;
                } else uiGuardSpawnTimer -= diff;

                if (Fear_Timer <= diff)
                {
                    DoCast(me, SPELL_FEAR);
                    Fear_Timer = 15000 + rand() % 20000;
                } else Fear_Timer -= diff;

                if (uiEnragueTimer <= diff)
                {
                    if (me->HasAura(SPELL_ENRAGE,0))
                        return;

                    float x, y, z, o;
                    me->GetHomePosition(x, y, z, o);
                    if (z < 24)
                        if (!me->IsNonMeleeSpellCasted(false))
                            DoCast(me, SPELL_ENRAGE, true);

                    uiEnragueTimer = 5 * IN_MILLISECONDS;
                } else uiEnragueTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_elder_nadoxAI(creature);
        }
};

enum AddSpells
{
    SPELL_SPRINT                                  = 56354,
    SPELL_GUARDIAN_AURA                           = 56151
};

class npc_ahnkahar_nerubian : public CreatureScript
{
    public:
        npc_ahnkahar_nerubian() : CreatureScript("npc_ahnkahar_nerubian") { }

        struct npc_ahnkahar_nerubianAI : public ScriptedAI
        {
            npc_ahnkahar_nerubianAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 uiSprintTimer;

            void Reset() override
            {
                if (me->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                    DoCast(me, SPELL_GUARDIAN_AURA, true);
                uiSprintTimer = 10 * IN_MILLISECONDS;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                    me->GetMap()->SetWorldState(WORLD_STATE_RESPECT_YOUR_ELDERS, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                    me->RemoveAurasDueToSpell(SPELL_GUARDIAN_AURA);

                if (instance)
                    if (instance->GetData(DATA_ELDER_NADOX_EVENT) != IN_PROGRESS)
                        me->DisappearAndDie();

                if (!UpdateVictim())
                    return;

                if (uiSprintTimer <= diff)
                {
                    DoCast(me, SPELL_SPRINT);
                    uiSprintTimer = 25 * IN_MILLISECONDS;
                } else uiSprintTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ahnkahar_nerubianAI(creature);
        }
};

// HACK: No, AI. Replace with proper db content?
class npc_nadox_eggs : public CreatureScript
{
    public:
        npc_nadox_eggs() : CreatureScript("npc_nadox_eggs") { }

        struct npc_nadox_eggsAI : public ScriptedAI
        {
            npc_nadox_eggsAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                creature->UpdateAllStats();
            }

            void Reset() override { }

            void JustEngagedWith(Unit* /*who*/) override { }

            void AttackStart(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_nadox_eggsAI(creature);
        }
};

void AddSC_boss_elder_nadox()
{
    new boss_elder_nadox;
    new npc_ahnkahar_nerubian;
    new npc_nadox_eggs;
}
