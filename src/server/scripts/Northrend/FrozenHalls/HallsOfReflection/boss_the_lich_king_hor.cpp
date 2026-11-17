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
 SDName: boss_Lich_king
 SDCategory: Halls of Reflection
 EndScriptData */

#include "ScriptPCH.h"
#include "halls_of_reflection.h"
#include "ScriptedEscortAI.h"
#include "Transport.h"

enum Spells
{
    SPELL_SUMMON_ICE_WALL              = 69768,
    SPELL_SUMMON_RAGING_GHOUL          = 69818,
    SPELL_SUMMON_LUMBERING_ABOMINATION = 69835,
    SPELL_SUMMON_RISEN_WITCH_DOCTOR    = 69836,
    SPELL_HARVEST_SOUL                 = 70070,

    // Raging gnoul
    SPELL_EMERGE_VISUAL                = 50142,
    SPELL_GNOUL_JUMP                   = 70150,

    // Witch Doctor
    SPELL_COURSE_OF_DOOM               = 70144,
    SPELL_SHADOW_BOLT_VOLLEY           = 70145,
    SPELL_SHADOW_BOLT                  = 70080,

    // Lumbering Abomination
    SPELL_ABON_STRIKE                  = 40505,
    SPELL_VOMIT_SPRAY                  = 70176,
};

enum Events
{
    EVENT_SAY_REMORSELESS_WINTER       = 1,
    EVENT_SAY_SUMMON_RAGING_GHOUL,
    EVENT_SAY_SUMMON_MINIONS,
    EVENT_REMORSELESS_WINTER,
    EVENT_SUMMON_RAGING_GHOUL,
    EVENT_SUMMON_LUMBERING_ABOMINATION,
    EVENT_SUMMON_RISEN_WITCH_DOCTOR,
};

class boss_lich_king_hor : public CreatureScript
{
    public:
        boss_lich_king_hor() : CreatureScript("boss_lich_king_hor") { }

        struct boss_lich_king_horAI : public ScriptedAI
        {
            boss_lich_king_horAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
                Reset();
            }

            InstanceScript* instance;
            SummonList summons;
            bool StartEscort;
            bool NonFight;
            bool splineStarted;
            bool WaypointsEnded = false;

            void Reset() override
            {
                if (!instance)
                    return;

                NonFight = false;
                StartEscort = false;
                splineStarted = false;
            }

            void JustDied(Unit* /*killer*/) override { }

            void AttackStart(Unit* who) override
            {
                if (!instance || !who)
                    return;

                if (NonFight)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS || who->GetTypeId() == TYPEID_PLAYER)
                    return;

                ScriptedAI::AttackStart(who);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_ICE_WALL_TARGET)
                    return;

                summon->SetInCombatWithZone();
                summon->setActive(true);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            bool CanAIAttack(Unit const* who) const override
            {
                return who->GetTypeId() == TYPEID_UNIT && (who->GetEntry() == NPC_JAINA_OUTRO || who->GetEntry() == NPC_SYLVANAS_OUTRO);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                    damage = me->GetHealth() - 1;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_LICH_KING_STOP_FOLLOW)
                {
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();

                    Movement::MoveSplineInit init(me);
                    for (auto itr : lichKingPath)
                    {
                        if (itr.GetPositionX() > me->GetPositionX()) // break already reached points
                            continue;

                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));
                    }

                    init.SetUncompressed();
                    init.Launch();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) == NOT_STARTED)
                {
                    if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);

                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
                }
                else if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);

                // Start chase for leader
                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS && !StartEscort)
                {
                    StartEscort = true;
                    if (me->HasAura(SPELL_ICE_PRISON))
                    {
                        DoCast(me, SPELL_STUN_BREAK_JAINA);
                        me->RemoveAurasDueToSpell(SPELL_ICE_PRISON);
                    }
                    else if (me->HasAura(SPELL_DARK_ARROW))
                    {
                        DoCast(me, SPELL_STUN_BREAK_JAINA);
                        me->RemoveAurasDueToSpell(SPELL_DARK_ARROW);
                    }
                    me->setActive(true);
                    NonFight = true;
                    me->AttackStop();
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    DoAttackerAreaInCombat(me, 50000.0f);
                    Follow(false);

                    me->SummonCreature(NPC_ICE_WALL_TARGET, 5547.833f, 2083.701f, 731.4332f, 1.0297440f);
                    me->SummonCreature(NPC_ICE_WALL_TARGET, 5503.213f, 1969.547f, 737.0245f, 1.2740900f);
                    me->SummonCreature(NPC_ICE_WALL_TARGET, 5439.799f, 1878.861f, 752.7194f, 1.0646510f);
                    me->SummonCreature(NPC_ICE_WALL_TARGET, 5318.289f, 1749.184f, 771.9423f, 0.8726646f);
                }

                // Leader caught, wipe
                if (Creature* pLider = ((Creature*)Unit::GetUnit(*me, instance->GetGuidData(DATA_ESCAPE_LIDER))))
                {
                    if (pLider->IsWithinDistInMap(me, 2.0f) && instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS)
                    {
                        me->setActive(false);
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        Talk(SAY_LICH_KING_WIN);
                        me->CastSpell(me, SPELL_FURY_OF_FROSTMOURNE, false);
                        me->Kill(pLider);
                        summons.DespawnAll();
                    }
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SAY_REMORSELESS_WINTER:
                            Talk(SAY_LICH_KING_WINTER);
                            DoCast(me, SPELL_PAIN_AND_SUFFERING, true);
                            break;
                        case EVENT_SAY_SUMMON_RAGING_GHOUL:
                            Talk(SAY_LICH_KING_SUMMON_GHOULS);
                            break;
                        case EVENT_SAY_SUMMON_MINIONS:
                            Talk(SAY_LICH_KING_SUMMON_MINIONS);
                            break;
                        case EVENT_REMORSELESS_WINTER:
                        {
                            DoCast(me, SPELL_REMORSELESS_WINTER);
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            splineStarted = true;

                            Movement::MoveSplineInit init(me);
                            for (auto itr : lichKingPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetWalk(true);
                            init.SetVelocity(1.0f);
                            init.SetUncompressed();
                            init.Launch();

                            events.ScheduleEvent(EVENT_SAY_REMORSELESS_WINTER, 2000);
                            break;
                        }
                        case EVENT_SUMMON_RAGING_GHOUL:
                            DoCast(me, SPELL_SUMMON_RAGING_GHOUL);
                            break;
                        case EVENT_SUMMON_LUMBERING_ABOMINATION:
                            DoCast(me, SPELL_SUMMON_LUMBERING_ABOMINATION);
                            break;
                        case EVENT_SUMMON_RISEN_WITCH_DOCTOR:
                            DoCast(me, SPELL_SUMMON_RISEN_WITCH_DOCTOR);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                if (me->GetPositionX() < 5280.0f && !WaypointsEnded)
                {
                    WaypointsEnded = true;
                    instance->SetData(DATA_LICHKING_EVENT, SPECIAL);
                    Talk(SAY_LICH_KING_ESCAPE_03);
                    if (Creature* leader = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ESCAPE_LIDER)))
                        me->CastSpell(leader, SPELL_HARVEST_SOUL, false);

                    if (Transport* gunship = ObjectAccessor::GetTransport(*me, instance->GetGuidData(DATA_GUNSHIP)))
                        gunship->EnableMovement(true);

                    me->setActive(false);
                }

                if (instance->GetData(DATA_ICE_WALL_1) == IN_PROGRESS)
                {
                    DoCast(me, SPELL_SUMMON_ICE_WALL);
                    Talk(SAY_LICH_KING_WALL_01);

                    Follow();

                    uint32 delay = 0;
                    events.ScheduleEvent(EVENT_REMORSELESS_WINTER,              delay +=  3 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RAGING_GHOUL,             delay +=  4 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  6 * IN_MILLISECONDS);
                    instance->SetData(DATA_SUMMONS, 7);
                    instance->SetData(DATA_ICE_WALL_1, DONE);
                }

                if (instance->GetData(DATA_ICE_WALL_2) == IN_PROGRESS)
                {
                    DoCast(me, SPELL_SUMMON_ICE_WALL);
                    Talk(SAY_LICH_KING_WALL_02);
                    events.ScheduleEvent(EVENT_SAY_SUMMON_RAGING_GHOUL, 5000); // Timer unknown or if even used at all

                    uint32 delay = 0;
                    events.ScheduleEvent(EVENT_SUMMON_RAGING_GHOUL,             delay +=  5 * IN_MILLISECONDS); // From old script
                    events.ScheduleEvent(EVENT_SUMMON_LUMBERING_ABOMINATION,    delay +=  6 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  2 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  2 * IN_MILLISECONDS);
                    instance->SetData(DATA_SUMMONS, 9);
                    instance->SetData(DATA_ICE_WALL_2, DONE);
                }

                if (instance->GetData(DATA_ICE_WALL_3) == IN_PROGRESS)
                {
                    DoCast(me, SPELL_SUMMON_ICE_WALL);
                    Talk(SAY_LICH_KING_WALL_03);
                    events.ScheduleEvent(EVENT_SAY_SUMMON_MINIONS, 11000); // Timer unknown or if even used at all

                    uint32 delay = 0;
                    events.ScheduleEvent(EVENT_SUMMON_RAGING_GHOUL,             delay +=  5 * IN_MILLISECONDS); // From old script
                    events.ScheduleEvent(EVENT_SUMMON_LUMBERING_ABOMINATION,    delay +=  6 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  2 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_LUMBERING_ABOMINATION,    delay +=  2 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay += 20 * IN_MILLISECONDS);
                    instance->SetData(DATA_SUMMONS, 10);
                    instance->SetData(DATA_ICE_WALL_3, DONE);
                }

                if (instance->GetData(DATA_ICE_WALL_4) == IN_PROGRESS)
                {
                    DoCast(me, SPELL_SUMMON_ICE_WALL);
                    Talk(SAY_LICH_KING_WALL_04);
                    events.ScheduleEvent(EVENT_SAY_SUMMON_MINIONS, 11000); // Timer unknown or if even used at all

                    uint32 delay = 0;
                    events.ScheduleEvent(EVENT_SUMMON_RAGING_GHOUL,             delay +=  5 * IN_MILLISECONDS); // From old script
                    events.ScheduleEvent(EVENT_SUMMON_LUMBERING_ABOMINATION,    delay +=  6 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  2 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  2 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_LUMBERING_ABOMINATION,    delay += 20 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_LUMBERING_ABOMINATION,    delay +=  6 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RAGING_GHOUL,             delay += 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  6 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_RISEN_WITCH_DOCTOR,       delay +=  4 * IN_MILLISECONDS);
                    instance->SetData(DATA_SUMMONS, 19);
                    instance->SetData(DATA_ICE_WALL_4, DONE);
                }
            }

        private:
            EventMap events;

            void Follow(bool walk = true)
            {
                // Temporarily remove combat flag in order for MotionMaster::MoveFollow not to remove MOVEMENTFLAG_WALKING
                bool wasInCombat = me->IsInCombat();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

                if (!splineStarted)
                {
                    me->SetWalk(walk);
                    if (Creature* leader = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ESCAPE_LIDER)))
                        me->GetMotionMaster()->MoveFollow(leader, 0.1f, 0.0f);
                }

                // Restore now that the movement generator has been initialized
                if (wasInCombat)
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lich_king_horAI(creature);
        }
};

class npc_raging_gnoul : public CreatureScript
{
    public:
        npc_raging_gnoul() : CreatureScript("npc_raging_gnoul") { }

        struct npc_raging_gnoulAI : public ScriptedAI
        {
            npc_raging_gnoulAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->setActive(true);
                Reset();
            }

            InstanceScript* instance;
            uint32 EmergeTimer;
            bool Emerge;
            bool Jumped;

            void Reset() override
            {
                me->UpdateObjectVisibility();
                DoCast(me, SPELL_EMERGE_VISUAL);
                EmergeTimer = 4000;
                Emerge = false;
                Jumped = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!instance)
                    return;

                instance->SetData(DATA_SUMMONS, 0);
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (Emerge == false)
                    return;

                ScriptedAI::AttackStart(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS)
                {
                    if (Emerge != true)
                    {
                        if (EmergeTimer < diff)
                        {
                            //me->RemoveFlag(SPLINEFLAG_WALKING | MOVEMENTFLAG_WALKING, true);
                            Emerge = true;
                            if (Creature* pLider = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ESCAPE_LIDER)))
                            {
                                DoResetThreat();
                                me->AI()->AttackStart(pLider);
                                me->GetThreatManager().AddThreat(pLider, 100.0f);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(pLider);
                            }
                        }
                        else
                            EmergeTimer -= diff;

                        return;
                    }

                    if (!UpdateVictim())
                        return;

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f))
                    {
                        if (!Jumped && me->IsWithinDistInMap(target, 30.0f) && !me->IsWithinDistInMap(target, 5.0f))
                        {
                            Jumped = true;
                            me->GetThreatManager().AddThreat(target, 200.0f);
                            AttackStart(target);
                            DoCast(target, SPELL_GNOUL_JUMP);
                        }
                    }
                } 
                else if (instance->GetData(DATA_LICHKING_EVENT) == NOT_STARTED)
                    me->DespawnOrUnsummon();

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_raging_gnoulAI(creature);
        }
};

class npc_risen_witch_doctor : public CreatureScript
{
    public:
        npc_risen_witch_doctor() : CreatureScript("npc_risen_witch_doctor") { }

        struct npc_risen_witch_doctorAI : public ScriptedAI
        {
            npc_risen_witch_doctorAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->setActive(true);
                Reset();
            }

            InstanceScript* instance;
            uint32 EmergeTimer;
            bool Emerge;
            uint32 uiBoltTimer;
            uint32 uiBoltVolleyTimer;
            uint32 uiCurseTimer;

            void Reset() override
            {
                me->UpdateObjectVisibility();
                DoCast(me, SPELL_EMERGE_VISUAL);
                EmergeTimer = 5000;
                uiBoltTimer = 6000;
                uiBoltVolleyTimer = 15000;
                uiCurseTimer = 7000;
                Emerge = false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!instance)
                    return;

                instance->SetData(DATA_SUMMONS, 0);
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (Emerge == false)
                    return;

                ScriptedAI::AttackStart(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS)
                {
                    if (Emerge != true)
                    {
                        if (EmergeTimer < diff)
                        {
                            Emerge = true;
                            if (Creature* pLider = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ESCAPE_LIDER)))
                            {
                                DoResetThreat();
                                me->AI()->AttackStart(pLider);
                                me->GetThreatManager().AddThreat(pLider, 100.0f);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(pLider);
                            }
                        }
                        else
                            EmergeTimer -= diff;

                        return;
                    }

                    if (!UpdateVictim())
                        return;

                    if (uiCurseTimer < diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(target, SPELL_COURSE_OF_DOOM);
                        uiCurseTimer = urand(10000, 15000);
                    } else uiCurseTimer -= diff;

                    if (uiBoltTimer < diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            DoCast(target, SPELL_SHADOW_BOLT);
                        uiBoltTimer = urand(2000, 3000);
                    } else uiBoltTimer -= diff;

                    if (uiBoltVolleyTimer < diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
                        uiBoltVolleyTimer = urand(15000, 22000);
                    } else uiBoltVolleyTimer -= diff;

                }
                else if (instance->GetData(DATA_LICHKING_EVENT) == NOT_STARTED)
                    me->DespawnOrUnsummon();

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_risen_witch_doctorAI(creature);
        }
};

class npc_abon : public CreatureScript
{
    public:
        npc_abon() : CreatureScript("npc_abon") { }

        struct npc_abonAI : public ScriptedAI
        {
            npc_abonAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->setActive(true);
                Reset();
            }

            InstanceScript* instance;
            bool Walk;
            uint32 uiStrikeTimer;
            uint32 uiVomitTimer;

            void Reset() override
            {
                Walk = false;
                uiVomitTimer = 15000;
                uiStrikeTimer = 6000;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!instance)
                    return;

                instance->SetData(DATA_SUMMONS, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance) return;

                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS)
                {
                    if (Walk != true)
                    {
                        Walk = true;
                        if (Creature* pLider = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ESCAPE_LIDER)))
                        {
                            DoResetThreat();
                            me->AI()->AttackStart(pLider);
                            me->GetThreatManager().AddThreat(pLider, 100.0f);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(pLider);
                        }
                    }

                    if (!UpdateVictim())
                        return;

                    if (uiStrikeTimer < diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            DoCast(target, SPELL_ABON_STRIKE);
                        uiStrikeTimer = urand(7000, 9000);
                    } else uiStrikeTimer -= diff;

                    if (uiVomitTimer < diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            DoCast(target, SPELL_VOMIT_SPRAY);
                        uiVomitTimer = urand(15000, 20000);
                    } else uiVomitTimer -= diff;
                }
                else if (instance->GetData(DATA_LICHKING_EVENT) == NOT_STARTED)
                    me->DespawnOrUnsummon();

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_abonAI(creature);
        }
};

void AddSC_boss_lich_king_hr()
{
    new boss_lich_king_hor();
    new npc_raging_gnoul();
    new npc_risen_witch_doctor();
    new npc_abon();
}
