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

/* ScriptData
SDName: Boss_Skarvald_Dalronn
SD%Complete: 95
SDComment: Needs adjustments to blizzlike timers, Yell Text + Sound to DB
SDCategory: Utgarde Keep
EndScriptData */

#include "ScriptPCH.h"
#include "utgarde_keep.h"

enum ScriptTexts
{
    // signed for 24200, but used by 24200, 27390
    SAY_SKARVALD_AGGRO                          = 0,
    SAY_SKARVALD_DAL_DIED                       = 1,
    SAY_SKARVALD_SKA_DIEDFIRST                  = 2,
    SAY_SKARVALD_KILL                           = 3,
    SAY_SKARVALD_DAL_DIEDFIRST                  = 4,

    // signed for 24201, but used by 24201, 27389
    SAY_DALRONN_AGGRO                           = 0,
    SAY_DALRONN_SKA_DIED                        = 1,
    SAY_DALRONN_DAL_DIEDFIRST                   = 2,
    SAY_DALRONN_KILL                            = 3,
    SAY_DALRONN_SKA_DIEDFIRST                   = 4,
};

enum Spells
{
    // Spells of Skarvald and his Ghost
    SPELL_CHARGE                                = 43651,
    SPELL_STONE_STRIKE                          = 48583,
    SPELL_SUMMON_SKARVALD_GHOST                 = 48613,
    SPELL_ENRAGE                                = 48193,
    // Spells of Dalronn and his Ghost
    SPELL_SHADOW_BOLT                           = 43649,
    SPELL_SHADOW_BOLT_H                         = 59575,
    SPELL_SUMMON_SKELETONS_H                    = 52611,
    SPELL_DEBILITATE                            = 43650,
    SPELL_SUMMON_DALRONN_GHOST                  = 48612,
};

enum Npcs
{
    NPC_SKARVALD_THE_CONSTRUCTOR                = 24200,
    NPC_SKARVALD_GHOST                          = 27390,
    NPC_DALRONN_THE_CONTROLLER                  = 24201,
    NPC_DALRONN_GHOST                           = 27389
};

class SkarvaldChargePredicate
{
   public:
      SkarvaldChargePredicate(Unit* unit) : me(unit) { }

    bool operator() (WorldObject* object) const
    {
        return object->GetDistance2d(me) >= 5.0f && object->GetDistance2d(me) <= 30.0f;
    }

    private:
        Unit* me;
};

class boss_skarvald_the_constructor : public CreatureScript
{
    public:
        boss_skarvald_the_constructor() : CreatureScript("boss_skarvald_the_constructor") { }

        struct boss_skarvald_the_constructorAI : public ScriptedAI
        {
            boss_skarvald_the_constructorAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            SummonList summons { me };

            bool ghost;
            uint32 Charge_Timer;
            uint32 StoneStrike_Timer;
            uint32 Response_Timer;
            uint32 Check_Timer;
            bool Dalronn_isDead;
            bool Enraged;

            void Reset() override
            {
                Charge_Timer = 5000;
                StoneStrike_Timer = 10000;
                Dalronn_isDead = false;
                Check_Timer = 5000;
                Enraged = false;
                me->RemoveLootMode(LOOT_MODE_DEFAULT);
                summons.DespawnAll();

                ghost = (me->GetEntry() == NPC_SKARVALD_GHOST);
                if (!ghost && instance)
                {
                    if (Creature* dalronn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DALRONN)))
                        if (dalronn->isDead())
                            dalronn->Respawn();

                    instance->SetData(DATA_SKARVALD_DALRONN_EVENT, NOT_STARTED);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!ghost && instance)
                {
                    Talk(SAY_SKARVALD_AGGRO);

                    if (Creature* dalronn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DALRONN)))
                        if (dalronn->IsAlive() && !dalronn->GetVictim())
                            dalronn->GetThreatManager().AddThreat(who, 0.0f);

                    instance->SetData(DATA_SKARVALD_DALRONN_EVENT, IN_PROGRESS);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!Enraged && !ghost && me->HealthBelowPctDamaged(15, damage))
                {
                    Enraged = true;
                    DoCast(me, SPELL_ENRAGE);
                }
            }

            void JustDied(Unit* killer) override
            {
                if (!ghost && instance)
                {
                    if (Creature* dalronn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DALRONN)))
                    {
                        if (dalronn->isDead())
                        {
                            Talk(SAY_SKARVALD_DAL_DIED);

                            instance->SetData(DATA_SKARVALD_DALRONN_EVENT, DONE);
                        }
                        else
                        {
                            Talk(SAY_SKARVALD_SKA_DIEDFIRST);

                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                            dalronn->AddLootMode(LOOT_MODE_DEFAULT);
                            DoCast(me, SPELL_SUMMON_SKARVALD_GHOST, true);
                        }
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                if (!ghost)
                {
                    Talk(SAY_SKARVALD_KILL);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_SKARVALD_GHOST)
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                DoZoneInCombat(summon, 100.0f);
                summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (ghost)
                {
                    if (instance && instance->GetData(DATA_SKARVALD_DALRONN_EVENT) != IN_PROGRESS)
                        me->DespawnOrUnsummon();
                }

                if (!UpdateVictim())
                    return;

                if (!ghost)
                {
                    if (Check_Timer)
                    {
                        if (Check_Timer <= diff)
                        {
                            Check_Timer = 5000;
                            Creature* dalronn = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_DALRONN) : ObjectGuid::Empty);
                            if (dalronn && dalronn->isDead())
                            {
                                Dalronn_isDead = true;
                                Response_Timer = 2000;
                                Check_Timer = 0;
                            }
                        } else Check_Timer -= diff;
                    }
                    if (Response_Timer && Dalronn_isDead)
                    {
                        if (Response_Timer <= diff)
                        {
                            Talk(SAY_SKARVALD_DAL_DIEDFIRST);

                            Response_Timer = 0;
                        } else Response_Timer -= diff;
                    }
                }

                if (Charge_Timer <= diff)
                {
                    DoCast(SelectTarget(SELECT_TARGET_RANDOM, 1, SkarvaldChargePredicate(me)), SPELL_CHARGE);
                    Charge_Timer = 5000 + rand() % 5000;
                } else Charge_Timer -= diff;

                if (StoneStrike_Timer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_STONE_STRIKE);
                    StoneStrike_Timer = 5000 + rand() % 5000;
                } else StoneStrike_Timer -= diff;

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_skarvald_the_constructorAI(creature);
        }
};

class boss_dalronn_the_controller : public CreatureScript
{
    public:
        boss_dalronn_the_controller() : CreatureScript("boss_dalronn_the_controller") { }

        struct boss_dalronn_the_controllerAI : public ScriptedAI
        {
            boss_dalronn_the_controllerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            SummonList summons { me };

            bool ghost;
            uint32 ShadowBolt_Timer;
            uint32 Debilitate_Timer;
            uint32 Summon_Timer;

            uint32 Response_Timer;
            uint32 Check_Timer;
            uint32 AggroYell_Timer;
            bool Skarvald_isDead;

            void Reset() override
            {
                ShadowBolt_Timer = 1000;
                Debilitate_Timer = 5000;
                Summon_Timer = 10000;
                Check_Timer = 5000;
                Skarvald_isDead = false;
                AggroYell_Timer = 0;
                me->RemoveLootMode(LOOT_MODE_DEFAULT);
                summons.DespawnAll();

                ghost = me->GetEntry() == NPC_DALRONN_GHOST;
                if (!ghost && instance)
                {
                    if (Creature* skarvald = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SKARVALD)))
                        if (skarvald->isDead())
                            skarvald->Respawn();

                    instance->SetData(DATA_SKARVALD_DALRONN_EVENT, NOT_STARTED);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!ghost && instance)
                {
                    if (Creature* skarvald = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SKARVALD)))
                        if (skarvald->IsAlive() && !skarvald->GetVictim())
                            skarvald->GetThreatManager().AddThreat(who,0.0f);

                    AggroYell_Timer = 5000;

                    instance->SetData(DATA_SKARVALD_DALRONN_EVENT, IN_PROGRESS);
                }
            }

            void JustDied(Unit* killer) override
            {
                if (!ghost && instance)
                {
                    if (Creature* skarvald = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SKARVALD)))
                    {
                        if (skarvald->isDead())
                        {
                            Talk(SAY_DALRONN_SKA_DIED);

                            instance->SetData(DATA_SKARVALD_DALRONN_EVENT, DONE);
                        }
                        else
                        {
                            Talk(SAY_DALRONN_DAL_DIEDFIRST);

                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                            skarvald->AddLootMode(LOOT_MODE_DEFAULT);
                            DoCast(me, SPELL_SUMMON_DALRONN_GHOST, true);
                        }
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                if (!ghost)
                {
                    Talk(SAY_DALRONN_KILL);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_DALRONN_GHOST)
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                DoZoneInCombat(summon, 100.0f);
                summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (ghost)
                    if (instance && instance->GetData(DATA_SKARVALD_DALRONN_EVENT) != IN_PROGRESS)
                        me->DespawnOrUnsummon();

                if (!UpdateVictim())
                    return;

                if (AggroYell_Timer)
                {
                    if (AggroYell_Timer <= diff)
                    {
                        Talk(SAY_DALRONN_AGGRO);

                        AggroYell_Timer = 0;
                    } else AggroYell_Timer -= diff;
                }

                if (!ghost)
                {
                    if (Check_Timer)
                    {
                        if (Check_Timer <= diff)
                        {
                            Check_Timer = 5000;
                            Creature* skarvald = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SKARVALD) : ObjectGuid::Empty);
                            if (skarvald && skarvald->isDead())
                            {
                                Skarvald_isDead = true;
                                Response_Timer = 2000;
                                Check_Timer = 0;
                            }
                        } else Check_Timer -= diff;
                    }

                    if (Response_Timer && Skarvald_isDead)
                    {
                        if (Response_Timer <= diff)
                        {
                            Talk(SAY_DALRONN_SKA_DIEDFIRST);
                            Response_Timer = 0;
                        } else Response_Timer -= diff;
                    }
                }

                if (ShadowBolt_Timer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_SHADOW_BOLT);
                        ShadowBolt_Timer = 2100;//give a 100ms pause to try cast other spells
                    }
                } else ShadowBolt_Timer -= diff;

                if (Debilitate_Timer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_DEBILITATE);
                        Debilitate_Timer = 5000 + rand() % 5000;
                    }
                } else Debilitate_Timer -= diff;

                if (IsHeroic())
                {
                    if (Summon_Timer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCast(me, SPELL_SUMMON_SKELETONS_H);
                            Summon_Timer = (rand() % 10000) + 20000;
                        }
                    } else Summon_Timer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_dalronn_the_controllerAI(creature);
        }
};

void AddSC_boss_skarvald_dalronn()
{
    new boss_skarvald_the_constructor();
    new boss_dalronn_the_controller();
}
