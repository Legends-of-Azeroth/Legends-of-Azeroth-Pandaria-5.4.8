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
#include "ScriptMgr.h"
#include "timeless_isle.h"

#define BOSS_YU_LON 0

#define WALLS_MAX 6
#define WALLS_Y_M 145

enum eSpells
{
    SPELL_JADEFIRE_BREATH        = 144530,
    SPELL_JADEFIRE_BUFFET        = 144630,
    SPELL_JADEFIRE_WALL_AT       = 144533,
    SPELL_JADEFIRE_WALL_EFF      = 144539,
    SPELL_JADEFIRE_BOLT_SELECTOR = 144545,
    SPELL_JADEFIRE_BOLT_YULON    = 144532,
    SPELL_JADEFIRE_BOLT_SUMM     = 144541,
    SPELL_JADEFIRE_BLAZE         = 144537,
    SPELL_JADEFIRE_BLAZE_EFF     = 144538,
};

enum eEvents
{
    EVENT_TIMER_JADE_BREATH = 1,
    EVENT_TIMER_JADEFIRE_WALL,
    EVENT_TIMER_JADEFIRE_BOLT,
    EVENT_TIMER_JADEFIRE_BUFFET,
    EVENT_TIMER_SHAO_DO_INTRO,
    EVENT_TIMER_SHAO_DO_INTRO_ATTACKABLE,
    EVENT_TIMER_SHAO_DO_OUTRO,
    EVENT_TIMER_DEATH,
    EVENT_TIMER_HEALTH_POOL,
};

enum eCreatures
{
    NPC_JADEFIRE_WALL   = 72020,
    NPC_JADEFIRE_BLAZE  = 72016,
};

enum eSays
{
    SAY_AGGRO,
    SAY_INTRO,
    SAY_DEATH,
    SAY_KILL,
    SAY_WALL,
    SAY_WALL_2,
    SAY_WALL_3,
    SAY_WALL_ANN,
};

enum eActions
{
    ACTION_JADEFIRE_WALL = 1,
    ACTION_JADEFIRE_BLAZE,
};

enum wallEvent
{
    WALL_EVENT_TIMER_DESPAWN = 1,
    WALL_EVENT_TIMER_GO,
    WALL_EVENT_TIMER_DAMAGE
};

enum wallAction
{
    WALL_ACTION_GO = 1,
    WALL_ACTION_RETURN
};

enum wallMovement
{
    WALL_MOVEMENT_FORWARD = 1
};

Position wallStartPos = { -562.14f, -5086.51f, -6.278f, MIDDLE_FACING_ANGLE };

class boss_yu_lon : public CreatureScript
{
    public:
        boss_yu_lon() : CreatureScript("boss_yu_lon") { }

        struct boss_yu_lonAI : public ScriptedAI
        {
            boss_yu_lonAI(Creature* creature) : ScriptedAI(creature), summons(me) 
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // prisma
            }

            bool death = false;
            EventMap events, nonCombatEvents;
            SummonList summons;
            float x, y;

            void Reset() override
            {
                events.Reset();

                if (me->GetFaction() == FACTION_HOSTILE_NEUTRAL)
                    me->SetFacingTo(MIDDLE_FACING_ANGLE);

                me->SetCombatDistance(500.0f);
                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
                me->SetWalk(true);
                me->setActive(true);

                me->ResetPlayerDamageReq();
                me->SetLootRecipient(nullptr);

                HandleDoor(me, GO_CELESTIAL_DOOR, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                UpdateHealth(me);
                death = false;
                me->SetWalk(false);
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_TIMER_HEALTH_POOL, 1000);
                events.ScheduleEvent(EVENT_TIMER_JADE_BREATH, 6000);
                events.ScheduleEvent(EVENT_TIMER_JADEFIRE_WALL, 40000);
                events.ScheduleEvent(EVENT_TIMER_JADEFIRE_BOLT, 15000);
                events.ScheduleEvent(EVENT_TIMER_JADEFIRE_BUFFET, 30000);
                HandleDoor(me, GO_CELESTIAL_DOOR, false);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (death)
                    damage = 0;

                if (!me->HasLootRecipient())
                    me->SetLootRecipient(attacker);

                if (damage >= me->GetHealth() && !death)
                {
                    death = true;
                    damage = 0;
                    Talk(SAY_DEATH);

                    RewardPlayers(me);

                    me->SetFaction(FACTION_FRIENDLY);

                    me->StopMoving();
                    me->RemoveAllAuras();
                    me->GetMotionMaster()->Clear();
                    me->CombatStop(true);
                    me->SetHealth(391000000/*me->GetMaxHealth()*/);

                    me->SetFacingTo(MIDDLE_FACING_ANGLE);
                    me->DeleteThreatList();

                    summons.DespawnAll();
                    me->RemoveAllAreasTrigger();

                    events.Reset();
                    summons.DespawnAll();
                    nonCombatEvents.ScheduleEvent(EVENT_TIMER_SHAO_DO_OUTRO, 10000);
                    nonCombatEvents.ScheduleEvent(EVENT_TIMER_DEATH, 13000);

                    HandleDoor(me, GO_CELESTIAL_DOOR, true);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    nonCombatEvents.ScheduleEvent(EVENT_TIMER_SHAO_DO_INTRO, CELESTIAL_COURT_BOSS_INTRO_TIMER_1);
                    me->SetFacingTo(MIDDLE_FACING_ANGLE);
                    me->SetFaction(FACTION_HOSTILE_NEUTRAL);
                    me->SetHomePosition(_timelessIsleMiddle);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_JADEFIRE_BLAZE)
                    summon->CastSpell(summon, SPELL_JADEFIRE_BLAZE, true);
            }

            void EnterEvadeMode() override
            {
                me->AddUnitState(UNIT_STATE_EVADE);

                ScriptedAI::EnterEvadeMode();
                summons.DespawnAll();
                events.Reset();
                me->RemoveAllAreasTrigger();
                me->GetMotionMaster()->MoveTargetedHome();
                me->SetLootRecipient(nullptr);
                HandleDoor(me, GO_CELESTIAL_DOOR, true);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
                        return;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_JADEFIRE_WALL)
                {
                    uint8 _random = urand(1, WALLS_MAX - 2); // not select at edge

                    me->SummonCreature(NPC_JADEFIRE_WALL, wallStartPos, TEMPSUMMON_MANUAL_DESPAWN);

                    for (uint8 i = 1; i < WALLS_MAX; i++)
                    {
                        if (i == _random) // skip this
                            continue;

                        x = wallStartPos.GetPositionX() + ((36.0f * i) * cos(Position::NormalizeOrientation(MIDDLE_FACING_ANGLE + M_PI / 2)));
                        y = wallStartPos.GetPositionY() + ((36.0f * i) * sin(Position::NormalizeOrientation(MIDDLE_FACING_ANGLE + M_PI / 2)));
                        me->SummonCreature(NPC_JADEFIRE_WALL, x, y, wallStartPos.GetPositionZ(), MIDDLE_FACING_ANGLE, TEMPSUMMON_MANUAL_DESPAWN);
                    }

                    Talk(urand(SAY_WALL, SAY_WALL_3));
                }
            };

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TIMER_SHAO_DO_INTRO:
                        {
                            Talk(SAY_INTRO);
                            nonCombatEvents.ScheduleEvent(EVENT_TIMER_SHAO_DO_INTRO_ATTACKABLE, CELESTIAL_COURT_BOSS_INTRO_TIMER_2);
                            break;
                        }
                        case EVENT_TIMER_SHAO_DO_INTRO_ATTACKABLE:
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetHealth(391000000/*me->GetMaxHealth()*/);
                            break;
                        }
                        case EVENT_TIMER_SHAO_DO_OUTRO:
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // prisma

                            if (Creature* shao = me->FindNearestCreature(NPC_EMPEROR_SHAOHAO_TI, 300.0f, true))
                                shao->AI()->Talk(EMPEROR_TALK_OUTRO_YULON);
                            break;
                        }
                        case EVENT_TIMER_DEATH:
                        {
                            if (Creature* shao = me->FindNearestCreature(NPC_EMPEROR_SHAOHAO_TI, 500.0f, true))
                                shao->AI()->DoAction(0);

                            uint32 corpseDelay = me->GetCorpseDelay();
                            uint32 respawnDelay = me->GetRespawnDelay();

                            me->SetCorpseDelay(1);
                            me->SetRespawnDelay(4);

                            death = false;
                            me->DespawnOrUnsummon();

                            me->SetCorpseDelay(corpseDelay);
                            me->SetRespawnDelay(respawnDelay);
                            break;
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TIMER_JADEFIRE_BUFFET:
                        {
                            DoCast(me, SPELL_JADEFIRE_BUFFET);
                            events.ScheduleEvent(EVENT_TIMER_JADEFIRE_BUFFET, 30000);
                            break;
                        }
                        case EVENT_TIMER_JADEFIRE_BOLT:
                        {
                            DoCast(me, SPELL_JADEFIRE_BOLT_SELECTOR);
                            events.ScheduleEvent(EVENT_TIMER_JADEFIRE_BOLT, 15000);
                            break;
                        }
                        case EVENT_TIMER_JADEFIRE_WALL:
                        {
                            DoAction(ACTION_JADEFIRE_WALL);
                            events.ScheduleEvent(EVENT_TIMER_JADEFIRE_WALL, 60000);
                            break;
                        }
                        case EVENT_TIMER_JADE_BREATH:
                        {
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_JADEFIRE_BREATH);

                            events.ScheduleEvent(EVENT_TIMER_JADE_BREATH, 10000);
                            break;
                        }
                        case EVENT_TIMER_HEALTH_POOL:
                        {
                            UpdateHealth(me);
                            events.ScheduleEvent(EVENT_TIMER_HEALTH_POOL, 5000);
                            break;
                        }
                        default:
                            break;
                    }
                }

                EnterEvadeIfOutOfCombatArea(diff);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_yu_lonAI(creature);
        }
};

// Jadefire Wall 72020
struct npc_jadefire_wall : public ScriptedAI
{
    npc_jadefire_wall(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;

    void Reset() override
    {
        me->setActive(true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        DoCast(me, SPELL_JADEFIRE_WALL_AT);

        me->SetFacingTo(MIDDLE_FACING_ANGLE);

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            GetPositionWithDistInOrientation(me, 146.0f, me->GetOrientation(), x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), me->GetOrientation());
            me->DespawnOrUnsummon(me->GetSplineDuration());
        });
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Jadefire Bolt Selector 144545
class spell_yulon_jadefire_bolt_selector : public SpellScript
{
    PrepareSpellScript(spell_yulon_jadefire_bolt_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIdx].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        uint8 minCount = 8;

        if (targets.size() < minCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount);

            return;
        }

        if (targets.size() > minCount)
            Trinity::Containers::RandomResizeList(targets, minCount);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_yulon_jadefire_bolt_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yulon_jadefire_bolt_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 1087. Summoned by 144533 - Jadefire Wall
class sat_yulon_jadefire_wall : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && !GetCaster()->isInBack(triggering->ToPlayer()) && (GetCaster()->isInFront(triggering->ToPlayer(), M_PI / 2) && GetCaster()->GetExactDist2d(triggering->ToPlayer()) < 10.0f || GetCaster()->isInFront(triggering->ToPlayer()) && GetCaster()->GetExactDist2d(triggering->ToPlayer()) >= 10.0f);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_JADEFIRE_WALL_EFF, true);
    }
};

void AddSC_boss_yu_lon()
{
    new boss_yu_lon();
    new creature_script<npc_jadefire_wall>("npc_jadefire_wall");

    new spell_script<spell_yulon_jadefire_bolt_selector>("spell_yulon_jadefire_bolt_selector");
    new atrigger_script<sat_yulon_jadefire_wall>("sat_yulon_jadefire_wall");
}
