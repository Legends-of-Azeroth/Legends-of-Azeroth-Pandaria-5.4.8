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
#include "drak_tharon_keep.h"

enum eSpells
{
    SPELL_BELLOWING_ROAR                          = 22686, // fears the group, can be resisted/dispelled
    SPELL_GRIEVOUS_BITE                           = 48920,
    SPELL_MANGLING_SLASH                          = 48873, // casted on the current tank, adds debuf
    SPELL_FEARSOME_ROAR                           = 48849,
    SPELL_FEARSOME_ROAR_H                         = 59422, // Not stacking, debuff
    SPELL_PIERCING_SLASH                          = 48878, // debuff -->Armor reduced by 75%
    SPELL_RAPTOR_CALL                             = 59416, // dummy
    SPELL_GUT_RIP                                 = 49710,
    SPELL_REND                                    = 13738
};

enum Creatures
{
    NPC_RAPTOR_1                                  = 26641,
    NPC_RAPTOR_2                                  = 26628
};

enum Misc
{
    ACTION_RAPTOR_KILLED                          = 1,
    DATA_KING_DRED                                = 2,
};

class boss_dred : public CreatureScript
{
    public:
        boss_dred() : CreatureScript("boss_dred") { }

        struct boss_dredAI : public ScriptedAI
        {
            boss_dredAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            uint32 uiBellowingRoarTimer;
            uint32 uiGrievousBiteTimer;
            uint32 uiManglingSlashTimer;
            uint32 uiFearsomeRoarTimer;
            uint32 uiPiercingSlashTimer;
            uint32 uiRaptorCallTimer;
            uint8 raptorsKilled;

            InstanceScript* instance;

            void Reset() override
            {
                if (instance)
                    instance->SetData(DATA_DRED_EVENT, NOT_STARTED);

                uiBellowingRoarTimer = 33 * IN_MILLISECONDS;
                uiGrievousBiteTimer  = 20 * IN_MILLISECONDS;
                uiManglingSlashTimer = 18500;
                uiFearsomeRoarTimer  = urand(10 * IN_MILLISECONDS,20 * IN_MILLISECONDS);
                uiPiercingSlashTimer = 17 * IN_MILLISECONDS;
                uiRaptorCallTimer    = urand(20 * IN_MILLISECONDS,25 * IN_MILLISECONDS);
                raptorsKilled = 0;
                me->GetMap()->SetWorldState(WORLDSTATE_BETTER_OF_DRED, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetData(DATA_DRED_EVENT,IN_PROGRESS);

                raptorsKilled = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiBellowingRoarTimer <= diff)
                {
                    DoCastAOE(SPELL_BELLOWING_ROAR, false);
                    uiBellowingRoarTimer = 40 * IN_MILLISECONDS;
                } else uiBellowingRoarTimer -=diff;

                if (uiGrievousBiteTimer <= diff)
                {
                    DoCastVictim(SPELL_GRIEVOUS_BITE ,false);
                    uiGrievousBiteTimer = 20 * IN_MILLISECONDS;
                } else uiGrievousBiteTimer -=diff;

                if (uiManglingSlashTimer <= diff)
                {
                    DoCastVictim(SPELL_MANGLING_SLASH,false);
                    uiManglingSlashTimer = 20 * IN_MILLISECONDS;
                } else uiManglingSlashTimer -=diff;

                if (uiFearsomeRoarTimer <= diff)
                {
                    DoCastAOE(SPELL_FEARSOME_ROAR,false);
                    uiFearsomeRoarTimer = urand(16 * IN_MILLISECONDS,18 * IN_MILLISECONDS);
                } else uiFearsomeRoarTimer -=diff;

                if (uiPiercingSlashTimer <= diff)
                {
                    DoCastVictim(SPELL_PIERCING_SLASH,false);
                    uiPiercingSlashTimer = 20 * IN_MILLISECONDS;
                } else uiPiercingSlashTimer -=diff;

                if (uiRaptorCallTimer <= diff)
                {
                    DoCastVictim(SPELL_RAPTOR_CALL,false);

                    /*float x, y, z;

                    me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 10.0f);
                    me->SummonCreature(RAND(NPC_RAPTOR_1, NPC_RAPTOR_2), x, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 1 * IN_MILLISECONDS);*/

                    std::list<Creature*> raptors;
                    GetCreatureListWithEntryInGrid(raptors, me, NPC_RAPTOR_1, 100.0f);
                    GetCreatureListWithEntryInGrid(raptors, me, NPC_RAPTOR_2, 100.0f);
                    auto pred = [](Creature* raptor) { return raptor->IsAlive() && !raptor->IsInCombat() && raptor->GetPositionX() < -517.0f; };
                    Trinity::Containers::RandomResizeList(raptors, pred, 1);
                    if (!raptors.empty())
                        DoZoneInCombat(raptors.front(), 200.0f);

                    uiRaptorCallTimer = urand(20 * IN_MILLISECONDS,25 * IN_MILLISECONDS);
                } else uiRaptorCallTimer -=diff;

                DoMeleeAttackIfReady();
            }

                void DoAction(int32 action) override
                {
                    if (action == ACTION_RAPTOR_KILLED)
                    {
                        if (++raptorsKilled >= 6)
                            me->GetMap()->SetWorldState(WORLDSTATE_BETTER_OF_DRED, 1);
                    }
                }

                uint32 GetData(uint32 type) const override
                {
                    if (type == DATA_KING_DRED)
                        return raptorsKilled;

                    return 0;
                }

                void JustDied(Unit* /*killer*/) override
                {
                    if (instance)
                        instance->SetData(DATA_DRED_EVENT, DONE);
                }
            };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_dredAI(creature);
        }
};

class npc_drakkari_gutripper : public CreatureScript
{
    public:
        npc_drakkari_gutripper() : CreatureScript("npc_drakkari_gutripper") { }

        struct npc_drakkari_gutripperAI : public ScriptedAI
        {
            npc_drakkari_gutripperAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 GutRipTimer;

            void Reset() override
            {
                GutRipTimer = urand(10000, 15000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (GutRipTimer <= diff)
                {
                    DoCastVictim(SPELL_GUT_RIP,false);
                    GutRipTimer = urand(10000, 15000);
                } else GutRipTimer -=diff;

                DoMeleeAttackIfReady();
            }

                void JustDied(Unit* /*killer*/) override
                {
                    if (Creature* Dred = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DRED)))
                        Dred->AI()->DoAction(ACTION_RAPTOR_KILLED);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_gutripperAI(creature);
        }
};

class npc_drakkari_scytheclaw : public CreatureScript
{
    public:
        npc_drakkari_scytheclaw() : CreatureScript("npc_drakkari_scytheclaw") { }

        struct npc_drakkari_scytheclawAI : public ScriptedAI
        {
            npc_drakkari_scytheclawAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiRendTimer;

            void Reset() override
            {
                uiRendTimer = urand(10 * IN_MILLISECONDS,15 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiRendTimer <= diff)
                {
                    DoCastVictim(SPELL_REND,false);
                    uiRendTimer = urand(10 * IN_MILLISECONDS,15 * IN_MILLISECONDS);
                } else uiRendTimer -=diff;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* Dred = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DRED)))
                    Dred->AI()->DoAction(ACTION_RAPTOR_KILLED);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_scytheclawAI(creature);
        }
};

void AddSC_boss_king_dred()
{
    new boss_dred;
    new npc_drakkari_gutripper;
    new npc_drakkari_scytheclaw;
}
