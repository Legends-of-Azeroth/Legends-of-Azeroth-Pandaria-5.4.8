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
#include "nexus.h"

enum Spells
{
    SPELL_CRYSTAL_SPIKES                          = 47958, // Don't work, using walkaround
    SPELL_CRYSTAL_SPIKES_H                        = 57082, // Don't work, using walkaround
    SPELL_CRYSTALL_SPIKE_DAMAGE                   = 47944,
    SPELL_CRYSTALL_SPIKE_DAMAGE_H                 = 57067,
    SPELL_CRYSTAL_SPIKE_PREVISUAL                 = 50442,
    NPC_CRYSTAL_SPIKE                             = 27099,
    SPELL_SPELL_REFLECTION                        = 47981,
    SPELL_TRAMPLE                                 = 48016,
    SPELL_TRAMPLE_H                               = 57066,
    SPELL_FRENZY                                  = 48017,
    SPELL_SUMMON_CRYSTALLINE_TANGLER              = 61564, // summons npc 32665
    SPELL_ROOTS                                   = 28858  // proper spell id is unknown
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_DEATH                                     = 1,
    SAY_REFLECT                                   = 2,
    SAY_CRYSTAL_SPIKES                            = 3,
    SAY_KILL                                      = 4
};

enum Creatures
{
    NPC_CRYSTALLINE_TANGLER                       = 32665
};

#define SPIKE_DISTANCE                            5.0f

class boss_ormorok : public CreatureScript
{
    public:
        boss_ormorok() : CreatureScript("boss_ormorok") { }

        struct boss_ormorokAI : public ScriptedAI
        {
            boss_ormorokAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            bool bFrenzy;
            bool bCrystalSpikes;
            uint8 uiCrystalSpikesCount;
            float fBaseX;
            float fBaseY;
            float fBaseZ;
            float fBaseO;
            float fSpikeXY[4][2];

            uint32 uiCrystalSpikesTimer;
            uint32 uiCrystalSpikesTimer2;
            uint32 uiTrampleTimer;
            uint32 uiFrenzyTimer;
            uint32 uiSpellReflectionTimer;
            uint32 uiSummonCrystallineTanglerTimer;

            void Reset() override
            {
                uiCrystalSpikesTimer = 12 * IN_MILLISECONDS;
                uiTrampleTimer = 10 * IN_MILLISECONDS;
                uiSpellReflectionTimer = 30 * IN_MILLISECONDS;
                uiSummonCrystallineTanglerTimer = 17 * IN_MILLISECONDS;
                bFrenzy = false;
                bCrystalSpikes = false;

                if (instance)
                    instance->SetData(DATA_ORMOROK_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_ORMOROK_EVENT, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_ORMOROK_EVENT, DONE);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (bCrystalSpikes)
                {
                    if (uiCrystalSpikesTimer2 <= diff)
                    {
                        fSpikeXY[0][0] = fBaseX + (SPIKE_DISTANCE * uiCrystalSpikesCount * cos (fBaseO));
                        fSpikeXY[0][1] = fBaseY + (SPIKE_DISTANCE * uiCrystalSpikesCount * sin (fBaseO));
                        fSpikeXY[1][0] = fBaseX - (SPIKE_DISTANCE * uiCrystalSpikesCount * cos (fBaseO));
                        fSpikeXY[1][1] = fBaseY - (SPIKE_DISTANCE * uiCrystalSpikesCount * sin (fBaseO));
                        fSpikeXY[2][0] = fBaseX + (SPIKE_DISTANCE * uiCrystalSpikesCount * cos (fBaseO - (M_PI / 2)));
                        fSpikeXY[2][1] = fBaseY + (SPIKE_DISTANCE * uiCrystalSpikesCount * sin (fBaseO - (M_PI / 2)));
                        fSpikeXY[3][0] = fBaseX - (SPIKE_DISTANCE * uiCrystalSpikesCount * cos (fBaseO - (M_PI / 2)));
                        fSpikeXY[3][1] = fBaseY - (SPIKE_DISTANCE * uiCrystalSpikesCount * sin (fBaseO - (M_PI / 2)));
                        for (uint8 i = 0; i < 4; ++i)
                            me->SummonCreature(NPC_CRYSTAL_SPIKE, fSpikeXY[i][0], fSpikeXY[i][1], fBaseZ, 0, TEMPSUMMON_TIMED_DESPAWN, Milliseconds(7 * IN_MILLISECONDS));
                        if (++uiCrystalSpikesCount >= 13)
                            bCrystalSpikes = false;
                        uiCrystalSpikesTimer2 = 200;
                    } else uiCrystalSpikesTimer2 -= diff;
                }

                if (!bFrenzy && HealthBelowPct(25))
                {
                    DoCast(me, SPELL_FRENZY);
                    bFrenzy = true;
                }

                if (uiTrampleTimer <= diff)
                {
                    DoCast(me, SPELL_TRAMPLE);
                    uiTrampleTimer = 10 * IN_MILLISECONDS;
                } else uiTrampleTimer -= diff;

                if (uiSpellReflectionTimer <= diff)
                {
                    Talk(SAY_REFLECT);
                    DoCast(me, SPELL_SPELL_REFLECTION);
                    uiSpellReflectionTimer = 30 * IN_MILLISECONDS;
                } else uiSpellReflectionTimer -= diff;

                if (uiCrystalSpikesTimer <= diff)
                {
                    Talk(SAY_CRYSTAL_SPIKES);
                    bCrystalSpikes = true;
                    uiCrystalSpikesCount = 1;
                    uiCrystalSpikesTimer2 = 0;
                    fBaseX = me->GetPositionX();
                    fBaseY = me->GetPositionY();
                    fBaseZ = me->GetPositionZ();
                    fBaseO = me->GetOrientation();
                    uiCrystalSpikesTimer = 20 * IN_MILLISECONDS;
                } else uiCrystalSpikesTimer -= diff;

                if (IsHeroic() && (uiSummonCrystallineTanglerTimer <= diff))
                {
                    Creature* Crystalline_Tangler = me->SummonCreature(NPC_CRYSTALLINE_TANGLER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000ms);
                    if (Crystalline_Tangler)
                    {
                        Unit* target = NULL;
                        uint8 Healer = 0;
                        for (uint8 j = 1; j <= 4; j++)
                        {
                            switch (j)
                            {
                                case 1: Healer = CLASS_PRIEST;  break;
                                case 2: Healer = CLASS_PALADIN; break;
                                case 3: Healer = CLASS_DRUID;   break;
                                case 4: Healer = CLASS_SHAMAN;  break;
                            }
                            std::list<HostileReference*>::const_iterator i = me->GetThreatManager().getThreatList().begin();
                            for (; i != me->GetThreatManager().getThreatList().end(); ++i)
                            {
                                Unit* pTemp = Unit::GetUnit(*me,(*i)->getUnitGuid());
                                if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER && pTemp->GetClass() == Healer)
                                {
                                    target = pTemp;
                                    break;
                                }
                            }
                            if (target)
                                break;
                        }
                        if (!target)
                            target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        if (target)
                        {
                            Crystalline_Tangler->AI()->AttackStart(target);
                            Crystalline_Tangler->GetThreatManager().addThreat(target, 1000000000.0f);
                        }
                    }
                    uiSummonCrystallineTanglerTimer = 17 * IN_MILLISECONDS;
                } else uiSummonCrystallineTanglerTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ormorokAI(creature);
        }
};

class npc_crystal_spike : public CreatureScript
{
    public:
        npc_crystal_spike() : CreatureScript("npc_crystal_spike") { }

        struct npc_crystal_spikeAI : public ScriptedAI
        {
            npc_crystal_spikeAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            uint32 SpellCrystalSpikeDamageTimer;
            uint32 SpellCrystalSpikePrevisualTimer;

            void Reset() override
            {
                SpellCrystalSpikeDamageTimer = 3700;
                SpellCrystalSpikePrevisualTimer = 1 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 diff) override
            {
                if (SpellCrystalSpikePrevisualTimer <= diff)
                {
                    DoCast(me, SPELL_CRYSTAL_SPIKE_PREVISUAL);
                    SpellCrystalSpikePrevisualTimer = 10 * IN_MILLISECONDS;
                } else SpellCrystalSpikePrevisualTimer -= diff;

                if (SpellCrystalSpikeDamageTimer <= diff)
                {
                    DoCast(me, SPELL_CRYSTALL_SPIKE_DAMAGE);
                    SpellCrystalSpikeDamageTimer = 10 * IN_MILLISECONDS;
                } else SpellCrystalSpikeDamageTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_crystal_spikeAI(creature);
        }
};

class npc_crystalline_tangler : public CreatureScript
{
    public:
        npc_crystalline_tangler() : CreatureScript("npc_crystalline_tangler") { }

        struct npc_crystalline_tanglerAI : public ScriptedAI
        {
            npc_crystalline_tanglerAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiRootsTimer;

            void Reset() override
            {
                uiRootsTimer = 1 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 diff) override
            {
                if (uiRootsTimer <= diff)
                {
                    if (me->IsWithinDist(me->GetVictim(), 5.0f, false))
                    {
                        DoCast(me->GetVictim(), SPELL_ROOTS);
                        uiRootsTimer = 15 * IN_MILLISECONDS;
                    }
                } else uiRootsTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_crystalline_tanglerAI(creature);
        }
};

void AddSC_boss_ormorok()
{
    new boss_ormorok();
    new npc_crystal_spike();
    new npc_crystalline_tangler();
}
