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
SDName: Boss_Midnight
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "ScriptPCH.h"

enum Midnight
{
    SAY_MIDNIGHT_KILL           = 0,
    SAY_APPEAR                  = 1,
    SAY_MOUNT                   = 2,

    SAY_KILL                    = 0,
    SAY_DISARMED                = 1,
    SAY_DEATH                   = 2,
    SAY_RANDOM                  = 3,

    SPELL_SHADOWCLEAVE          = 29832,
    SPELL_INTANGIBLE_PRESENCE   = 29833,
    SPELL_BERSERKER_CHARGE      = 26561, // Only when mounted

    MOUNTED_DISPLAYID           = 16040,

    // Attumen (@todo Use the summoning spell instead of Creature id. It works, but is not convenient for us)
    SUMMON_ATTUMEN              = 15550,
};

class boss_attumen : public CreatureScript
{
    public:
        boss_attumen() : CreatureScript("boss_attumen") { }

        struct boss_attumenAI : public ScriptedAI
        {
            boss_attumenAI(Creature* creature) : ScriptedAI(creature)
            {
                Phase = 1;

                CleaveTimer = urand(10000,15000);
                CurseTimer = 30000;
                RandomYellTimer = urand(30000,60000);              //Occasionally yell
                ChargeTimer = 20000;
            }

            ObjectGuid midnightGUID;
            uint8 Phase;
            uint32 CleaveTimer;
            uint32 CurseTimer;
            uint32 RandomYellTimer;
            uint32 ChargeTimer; // only when mounted
            uint32 ResetTimer;

            void Reset() override
            {
                ResetTimer = 0;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                ResetTimer = 2000;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                if (Unit* midnight = Unit::GetUnit(*me, midnightGUID))
                    midnight->Kill(midnight);
            }

            void UpdateAI(uint32 diff) override;

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Mechanic == MECHANIC_DISARM)
                    Talk(SAY_DISARMED);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_attumenAI(creature);
        }
};

class boss_midnight : public CreatureScript
{
    public:
        boss_midnight() : CreatureScript("boss_midnight") { }

        struct boss_midnightAI : public ScriptedAI
        {
            boss_midnightAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid attumenGUID;
            uint8 Phase;
            uint32 Mount_Timer;

            void Reset() override
            {
                Phase = 1;
                attumenGUID = ObjectGuid::Empty;
                Mount_Timer = 0;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetVisible(true);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void KilledUnit(Unit* /*victim*/) override
            {
                if (Phase == 2)
                {
                    if (Unit* unit = Unit::GetUnit(*me, attumenGUID))
                        Talk(SAY_MIDNIGHT_KILL, unit);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (Phase == 1 && HealthBelowPct(95))
                {
                    Phase = 2;
                    if (Creature* attumen = me->SummonCreature(SUMMON_ATTUMEN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000ms))
                    {
                        attumenGUID = attumen->GetGUID();
                        attumen->AI()->AttackStart(me->GetVictim());
                        SetMidnight(attumen, me->GetGUID());
                        Talk(SAY_APPEAR, attumen);
                    }
                }
                else if (Phase == 2 && HealthBelowPct(25))
                {
                    if (Unit* attumen = Unit::GetUnit(*me, attumenGUID))
                        Mount(attumen);
                }
                else if (Phase == 3)
                {
                    if (Mount_Timer)
                    {
                        if (Mount_Timer <= diff)
                        {
                            Mount_Timer = 0;
                            me->SetVisible(false);
                            me->GetMotionMaster()->MoveIdle();
                            if (Unit* attumen = Unit::GetUnit(*me, attumenGUID))
                            {
                                attumen->SetDisplayId(MOUNTED_DISPLAYID);
                                attumen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                if (attumen->GetVictim())
                                {
                                    attumen->GetMotionMaster()->MoveChase(attumen->GetVictim());
                                    attumen->SetTarget(attumen->GetVictim()->GetGUID());
                                }
                                attumen->SetObjectScale(1);
                            }
                        } else Mount_Timer -= diff;
                    }
                }

                if (Phase != 3)
                    DoMeleeAttackIfReady();
            }

            void Mount(Unit* attumen)
            {
                Talk(SAY_MOUNT, attumen);
                Phase = 3;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                attumen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                float angle = me->GetAngle(attumen);
                float distance = me->GetDistance2d(attumen);
                float newX = me->GetPositionX() + cos(angle) * (distance / 2);
                float newY = me->GetPositionY() + sin(angle) * (distance / 2);
                float newZ = 50;
                //me->Relocate(newX, newY, newZ, angle);
                //me->SendMonsterMove(newX, newY, newZ, 0, true, 1000);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, newX, newY, newZ);
                distance += 10;
                newX = me->GetPositionX() + cos(angle)*(distance / 2);
                newY = me->GetPositionY() + sin(angle)*(distance / 2);
                attumen->GetMotionMaster()->Clear();
                attumen->GetMotionMaster()->MovePoint(0, newX, newY, newZ);
                //attumen->Relocate(newX, newY, newZ, -angle);
                //attumen->SendMonsterMove(newX, newY, newZ, 0, true, 1000);
                Mount_Timer = 1000;
            }

            void SetMidnight(Creature* attumen, ObjectGuid value)
            {
                CAST_AI(boss_attumen::boss_attumenAI, attumen->AI())->midnightGUID = value;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_midnightAI(creature);
        }
};

void boss_attumen::boss_attumenAI::UpdateAI(uint32 diff)
{
    if (ResetTimer)
    {
        if (ResetTimer <= diff)
        {
            ResetTimer = 0;
            if (Unit* midnight = Unit::GetUnit(*me, midnightGUID))
            {
                midnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                midnight->SetVisible(true);
            }
            midnightGUID = ObjectGuid::Empty;
            me->SetVisible(false);
            me->Kill(me);
        }  else ResetTimer -= diff;
    }

    // Return since we have no target
    if (!UpdateVictim())
        return;

    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
        return;

    if (CleaveTimer <= diff)
    {
        DoCast(me->GetVictim(), SPELL_SHADOWCLEAVE);
        CleaveTimer = urand(10000,15000);
    } else CleaveTimer -= diff;

    if (CurseTimer <= diff)
    {
        DoCast(me->GetVictim(), SPELL_INTANGIBLE_PRESENCE);
        CurseTimer = 30000;
    } else CurseTimer -= diff;

    if (me->GetUInt32Value(UNIT_FIELD_DISPLAY_ID) == MOUNTED_DISPLAYID)
    {
        if (ChargeTimer <= diff)
        {
            Unit* target = nullptr;
            std::vector<Unit *> target_list;
            for (auto&& itr : me->GetThreatManager().getThreatList())
            {
                target = Unit::GetUnit(*me, itr->getUnitGuid());
                if (target && !target->IsWithinDist(me, ATTACK_DISTANCE, false))
                    target_list.push_back(target);
                target = nullptr;
            }
            if (target_list.size())
                target = *(target_list.begin() + rand() % target_list.size());

            DoCast(target, SPELL_BERSERKER_CHARGE);
            ChargeTimer = 20000;
        } else ChargeTimer -= diff;
    }
    else
    {
        if (HealthBelowPct(25))
        {
            Creature* midnight = Unit::GetCreature(*me, midnightGUID);
            if (midnight && midnight->GetTypeId() == TYPEID_UNIT)
            {
                CAST_AI(boss_midnight::boss_midnightAI, (midnight->AI()))->Mount(me);
                me->SetHealth(midnight->GetHealth());
                DoResetThreat();
            }
        }
    }

    DoMeleeAttackIfReady();
}

void AddSC_boss_attumen()
{
    new boss_attumen();
    new boss_midnight();
}
