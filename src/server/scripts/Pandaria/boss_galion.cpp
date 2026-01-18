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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "Vehicle.h"

enum GalionSpellData
{
    SPELL_STOMP                             = 121787,
    SPELL_CANNON_BARRAGE                    = 121577,
    SPELL_FIRE_SHOT                         = 121673,
    SPELL_EMPALLING_PULL                    = 121747,
    SPELL_BERSERK                           = 47008
};

enum GalionEvents
{
    EVENT_STOMP                             = 1,
    EVENT_CANNON                            = 2,
    EVENT_FIRE_SHOT                         = 3,
    EVENT_EMPALLING                         = 4,
    EVENT_SPAWN                             = 6,
    EVENT_BERSERK                           = 7,
    EVENT_INIT_VEHICLE                      = 8,
    EVENT_INIT_COMBAT                       = 9
};

enum GalionActions
{
    ACTION_INIT_COMBAT,
};

enum GalionCreatures
{
    NPC_GALLEON                             = 62346,
    NPC_GALLEON_CANNON                      = 62355,
    NPC_SALYIN_WARMONGER                    = 62351, // temp summon
    NPC_SALYIN_SKRIMISHER                   = 62350,
    NPC_CHIEF_SALYIS                        = 62352
};

enum GalionMisc
{
    DATA_BOSS_GALION,
};

void HandleInitCombat(Creature* me, ObjectGuid ownerGUID)
{
    Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID);

    if (!owner)
        return;

    std::list<Player*> pList;
    GetPlayerListInGrid(pList, owner, 200.0f);

    if (pList.empty())
        return;

    if (pList.size() > 1)
    {
        Trinity::Containers::RandomResizeList(pList, 1);

        Player* target = *pList.begin();
        owner->GetMotionMaster()->MoveChase(target);
        owner->Attack(target, true);
    }
}

class boss_galion : public CreatureScript
{
    public:
        boss_galion() : CreatureScript("boss_galion") { }

        struct boss_galion_AI : public BossAI
        {
            boss_galion_AI(Creature* creature) : BossAI(creature, DATA_BOSS_GALION) { }

            EventMap vEvents;
            int32 m_bp;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                m_bp = 0;

                vEvents.ScheduleEvent(EVENT_INIT_VEHICLE, 500ms);
            }

            void KilledUnit(Unit* /*victim*/) override { }

            void EnterEvadeMode() override
            {
                summons.DespawnAll();
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_STOMP, 50s);
                events.ScheduleEvent(EVENT_CANNON, 25s);
                events.ScheduleEvent(EVENT_SPAWN, 1min);
                events.ScheduleEvent(EVENT_BERSERK, 15min);
                events.ScheduleEvent(EVENT_INIT_COMBAT, 1s + 500ms);
            }

            void UpdateAI(uint32 diff) override
            {
                vEvents.Update(diff);

                while (uint32 eventId = vEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INIT_VEHICLE)
                    {
                        // Init vehicles, only cosmetic or passive
                        if (const SpellInfo* m_spell = sSpellMgr->GetSpellInfo(VEHICLE_SPELL_RIDE_HARDCODED))
                        {
                            for (uint32 i = 1; i < 3; i++)
                            {
                                m_bp = m_spell->Effects[EFFECT_0].BasePoints + (int32)i;

                                if (Creature* cannon = me->SummonCreature(NPC_GALLEON_CANNON, *me, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    cannon->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &m_bp, 0, 0, true);
                                    cannon->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI / 2));
                                }
                            }

                            for (uint32 i = 3; i < 7; i++)
                            {
                                m_bp = m_spell->Effects[EFFECT_0].BasePoints + (int32)i;

                                if (Creature* skirmisher = me->SummonCreature(NPC_SALYIN_SKRIMISHER, *me, TEMPSUMMON_MANUAL_DESPAWN))
                                    skirmisher->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &m_bp, 0, 0, true);
                            }

                            m_bp = m_spell->Effects[EFFECT_0].BasePoints + 7;

                            if (Creature* chief = me->SummonCreature(NPC_CHIEF_SALYIS, *me, TEMPSUMMON_MANUAL_DESPAWN))
                                chief->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &m_bp, 0, 0, true);
                        }
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STOMP:
                        {
                            me->CastSpell(me, SPELL_STOMP, true);
                            events.ScheduleEvent(EVENT_STOMP, 1min);
                            break;
                        }
                        case EVENT_CANNON:
                        {
                            DoCast(me, SPELL_CANNON_BARRAGE);
                            events.ScheduleEvent(EVENT_CANNON, 1min);
                            break;
                        }
                        case EVENT_SPAWN:
                        {
                            for (uint8 i = 0; i < 6; ++i)
                                me->SummonCreature(NPC_SALYIN_WARMONGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

                            events.ScheduleEvent(EVENT_SPAWN, 1min);
                            break;
                        }
                        case EVENT_BERSERK:
                        {
                            me->CastSpell(me, SPELL_BERSERK, false);
                            break;
                        }
                        case EVENT_INIT_COMBAT:
                        {
                            if (Vehicle* veh = me->GetVehicleKit())
                            {
                                for (auto&& seat : veh->Seats)
                                    if (Creature* passenger = ObjectAccessor::GetCreature(*veh->GetBase(), seat.second.Passenger.Guid))
                                        if (passenger->AI())
                                            passenger->AI()->DoAction(ACTION_INIT_COMBAT);
                            }
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 125.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_galion_AI(creature);
        }
};

struct npc_salyin_skrimisher : public ScriptedAI
{
    npc_salyin_skrimisher(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        ownerGUID = summoner->GetGUID();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INIT_COMBAT)
        {
            HandleInitCombat(me, me->GetGUID());
            events.ScheduleEvent(EVENT_FIRE_SHOT, randtime(3s, 16s));
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_FIRE_SHOT)
            {
                DoCast(me, SPELL_FIRE_SHOT);
                events.ScheduleEvent(EVENT_FIRE_SHOT, randtime(3s, 16s));
            }
            break;
        }
    }
};

struct npc_salyin_warmonger : public ScriptedAI
{
    npc_salyin_warmonger(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void IsSummonedBy(Unit* summoner) override
    {
        HandleInitCombat(me, me->GetGUID());
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_EMPALLING, 50s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_EMPALLING:
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        me->CastSpell(target, SPELL_EMPALLING_PULL, true);

                    events.ScheduleEvent(EVENT_EMPALLING, 1min);
                    break;
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

class spell_galleon_fire_shot : public SpellScript
{
    PrepareSpellScript(spell_galleon_fire_shot);

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() > 3)
            Trinity::Containers::RandomResizeList(targets, (int)(targets.size() / 2));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galleon_fire_shot::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

void AddSC_boss_galion()
{
    new boss_galion();
    new creature_script<npc_salyin_skrimisher>("npc_salyin_skrimisher");
    new creature_script<npc_salyin_warmonger>("npc_salyin_warmonger");
    new spell_script<spell_galleon_fire_shot>("spell_galleon_fire_shot");
}
