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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulgurub.h"

enum Yells
{
    SAY_AGGRO               = 0,
    SAY_DEATH               = 1,
    SAY_DREAMSTATE          = 2,
    SAY_KILL                = 3,
    SAY_DREAMSTATE_ANNOUNCE = 4
};

enum Spells
{
    SPELL_EARTH_SHOCK       = 96650,
    SPELL_WRATH             = 96652,
    SPELL_AWAKEN_NIGHTMARES = 96658,
    SPELL_NIGHTMARE         = 96670,
    SPELL_WAKING_NIGHTMARE  = 96757,
    SPELL_CONSUME_SOUL      = 96758
};

enum Events
{
    EVENT_EARTH_SHOCK = 1,
    EVENT_WRATH       = 2
};

enum Points
{
    POINT_NIGHTMARE_TARGET = 522840
};

typedef std::list<WorldObject*> ObjectList;

class npc_nightmare_illusion : public CreatureScript
{
    public:
        npc_nightmare_illusion() : CreatureScript("npc_nightmare_illusion") { }

        struct npc_nightmare_illusionAI : public ScriptedAI
        {
            npc_nightmare_illusionAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                SetCombatMovement(false);
                me->SetSpeed(MOVE_RUN, 0.25f, true);
                me->SetSpeed(MOVE_WALK, 0.25f, true);
                me->SetSpeed(MOVE_FLIGHT, 0.25f, true);
                //me->SetWalk(true);
                aiSet = false;
                nightmareOwner = ObjectGuid::Empty;
                nightmareTarget = ObjectGuid::Empty;
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!nightmareTarget || !nightmareOwner)
                {
                    me->DespawnOrUnsummon();
                    return;
                }
                if (Player* player = ObjectAccessor::GetPlayer(*me, nightmareTarget))
                    if (!player->IsAlive())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                if (Unit* unit = ObjectAccessor::GetUnit(*me, nightmareTarget))
                    if (!unit->IsAlive())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                if (aiSet)
                    return;
                aiSet = true;
                if (Unit* target = ObjectAccessor::GetUnit(*me, nightmareTarget))
                {
                    me->SetTarget(nightmareTarget);
                    DoCast(target, SPELL_WAKING_NIGHTMARE, true);
                    Position pos = target->GetPosition();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(POINT_NIGHTMARE_TARGET, pos);
                }
            }

            void SetNightmareTarget(ObjectGuid owner, ObjectGuid target)
            {
                nightmareOwner = owner;
                nightmareTarget = target;
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == POINT_NIGHTMARE_TARGET)
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*me, nightmareTarget))
                        if (Unit* owner = ObjectAccessor::GetUnit(*me, nightmareOwner))
                        {
                            me->CastSpell(target, SPELL_CONSUME_SOUL, true);
                            CAST_AI(BossAI, owner->GetAI())->KilledUnit(target);
                            me->DespawnOrUnsummon();
                        }
                }
            }

        protected:
            bool aiSet;
            ObjectGuid nightmareOwner;
            ObjectGuid nightmareTarget;

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_nightmare_illusionAI>(creature);
        }
};

class boss_hazzarah : public CreatureScript
{
    public:
        boss_hazzarah() : CreatureScript("boss_hazzarah") { }

        struct boss_hazzarahAI : public BossAI
        {
            boss_hazzarahAI(Creature* creature) : BossAI(creature, DATA_CACHE_OF_MADNESS_BOSS)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            void Reset() override
            {
                _Reset();
                summonedAt66Pct = false;
                summonedAt33Pct = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(SAY_AGGRO);

                events.ScheduleEvent(EVENT_EARTH_SHOCK, 8000);
                events.ScheduleEvent(EVENT_WRATH, 500);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!summonedAt66Pct && HealthBelowPct(67))
                {
                    summonedAt66Pct = true;
                    NightmarePhase();
                }
                if (!summonedAt33Pct && HealthBelowPct(34))
                {
                    summonedAt33Pct = true;
                    NightmarePhase();
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EARTH_SHOCK:
                            DoCast(me->GetVictim(), SPELL_EARTH_SHOCK);
                            events.ScheduleEvent(EVENT_EARTH_SHOCK, 10000);
                            break;
                        case EVENT_WRATH:
                            DoCast(me->GetVictim(), SPELL_WRATH);
                            events.ScheduleEvent(EVENT_WRATH, urand(3000, 5000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            ObjectList* GetWorldObjectsInDist(WorldObject* obj, float dist)
            {
                ObjectList* targets = new ObjectList();
                if (obj)
                {
                    Trinity::AllWorldObjectsInRange u_check(obj, dist);
                    Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(obj, *targets, u_check);
                    obj->VisitNearbyObject(dist, searcher);
                }
                return targets;
            }

            void NightmarePhase()
            {
                me->InterruptSpell(CURRENT_GENERIC_SPELL);
                me->InterruptSpell(CURRENT_CHANNELED_SPELL);

                // Gather all nearby player GUIDs
                ObjectGuid playerGuids[5];
                uint32 playerCount = 0;
                ObjectList* units = GetWorldObjectsInDist(me, 100);
                for (ObjectList::const_iterator itr = units->begin(); itr != units->end(); ++itr)
                {
                    if (!(*itr) || (*itr)->GetTypeId() != TYPEID_PLAYER)
                        continue;

                    if (Player* nearby = (*itr)->ToPlayer())
                        if (nearby->IsAlive())
                            playerGuids[playerCount++] = nearby->GetGUID();
                }

                // Select random 4 players to be sleeped
                uint32 stunnedPlayerIndexes[4];
                uint32 stunnedPlayerCount = 0;
                for (uint32 i = 0; i < playerCount - 1; ++i)
                {
                    uint32 randomIndex;
                    while (true)
                    {
                        randomIndex = urand(0, playerCount - 1);
                        bool ok = true;
                        for (uint32 j = 0; j < stunnedPlayerCount; ++j)
                            if (stunnedPlayerIndexes[j] == randomIndex)
                            {
                                ok = false;
                                break;
                            }
                        if (ok)
                            break;
                    }
                    stunnedPlayerIndexes[stunnedPlayerCount++] = randomIndex;
                }

                // Spawn nightmare for each player
                for (uint32 i = 0; i < stunnedPlayerCount; ++i)
                {
                    ObjectGuid guid = playerGuids[stunnedPlayerIndexes[i]];
                    playerGuids[stunnedPlayerIndexes[i]] = ObjectGuid::Empty;
                    if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                    {
                        Position pos;
                        do
                        {
                            pos = player->GetNearPosition(30, (float)(rand_norm() * 2 * M_PI));
                        }
                        while (player->GetDistance(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()) < 20);
                        if (TempSummon* summon = me->SummonCreature(52284, pos, TEMPSUMMON_DEAD_DESPAWN))
                            if (npc_nightmare_illusion::npc_nightmare_illusionAI* ai = (npc_nightmare_illusion::npc_nightmare_illusionAI*)summon->GetAI())
                                ai->SetNightmareTarget(me->GetGUID(), player->GetGUID());
                    }
                }

                if (stunnedPlayerCount)
                {
                    // Announce to the only player that is not sleeping
                    for (uint32 i = 0; i < playerCount; ++i)
                    {
                        ObjectGuid guid = playerGuids[i];
                        if (guid)
                        {
                            playerGuids[i] = ObjectGuid::Empty;
                            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                                Talk(SAY_DREAMSTATE_ANNOUNCE, player);
                        }
                    }

                    Talk(SAY_DREAMSTATE);
                    DoCastAOE(SPELL_AWAKEN_NIGHTMARES);
                }
            }

        protected:
            bool summonedAt66Pct;
            bool summonedAt33Pct;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_hazzarahAI>(creature);
        }
};

void AddSC_boss_hazzarah()
{
    new npc_nightmare_illusion();
    new boss_hazzarah();
}
