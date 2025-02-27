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
#include "MapManager.h"
#include "timeless_isle.h"

enum Spells
{
    SPELL_INSPIRING_SONG            = 144468,
    SPELL_BEACON_OF_HOPE_SUMM       = 144473,
    SPELL_BEACON_OF_HOPE            = 144474,
    SPELL_BEACON_OF_HOPE_EFF        = 144475,
    SPELL_BLAZING_SONG              = 144471,
    SPELL_BLAZING_SONG_EFF          = 144472,
    SPELL_CRANE_RUSH                = 144470,
    SPELL_CRANE_RUSH_EFF            = 144495,
    SPELL_FIRESTORM                 = 144463,
    SPELL_FIRESTORM_EFF             = 144462,
    SPELL_FIRESTORM_SUMM            = 144461,
    SPELL_BLAZING_NOVA              = 144493,
    SPELL_BLAZING_NOVA_EFF          = 144494,
};

enum Events
{
    EVENT_FIRESTORM = 1,
    EVENT_INSPIRING_SONG,
    EVENT_BEACON_OF_HOPE,
    EVENT_BLAZING_SONG,
    EVENT_CRANE_RUSH,
    EVENT_SHAO_DO_OUTRO,
    EVENT_DEATH,
    EVENT_SHAO_DO_INTRO,
    EVENT_SHAO_DO_INTRO_ATTACKABLE,
    EVENT_TIMER_HEALTH_POOL,
};

enum Says
{
    SAY_AGGRO,
    SAY_INTRO,
    SAY_DEATH,
    SAY_KILL,
    SAY_SPELL,
    SAY_SPELL_2,
    SAY_SPELL_3,
};

enum eCreatures
{
    NPC_CHILD_OF_CHI_JI = 71990,
    NPC_FIRESTORM       = 71971,
    NPC_BEACON_OF_HOPE  = 71978,
};

enum eActions
{
    ACTION_REMOVE_BLAZE,
};

class boss_chi_ji : public CreatureScript
{
    public:
        boss_chi_ji() : CreatureScript("boss_chi_ji") { }

        struct boss_chi_jiAI : public ScriptedAI
        {
            boss_chi_jiAI(Creature* creature) : ScriptedAI(creature), summons(me) 
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // prisma
            }

            SummonList summons;
            EventMap events, nonCombatEvents;
            bool death = false;
            float nextChildrens;

            void Reset() override
            {
                events.Reset();

                if (me->GetFaction() == FACTION_HOSTILE_NEUTRAL)
                    me->SetFacingTo(MIDDLE_FACING_ANGLE);

                me->SetCombatDistance(500.0f);
                nextChildrens = 66.0f;
                summons.DespawnAll();
                me->SetWalk(true);
                me->setActive(true);

                me->ResetPlayerDamageReq();
                me->SetLootRecipient(nullptr);

                HandleDoor(me, GO_CELESTIAL_DOOR, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                UpdateHealth(me);
                me->SetWalk(false);
                death = false;
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_TIMER_HEALTH_POOL, 1000);
                events.ScheduleEvent(EVENT_FIRESTORM, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_INSPIRING_SONG, 19.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BEACON_OF_HOPE, 50000);
                HandleDoor(me, GO_CELESTIAL_DOOR, false);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(nextChildrens) && nextChildrens > 0.0f)
                {
                    nextChildrens -= 33.0f;
                    events.ScheduleEvent(EVENT_CRANE_RUSH, 0.5 * IN_MILLISECONDS);
                }

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

                    events.Reset();
                    summons.DespawnAll();
                    nonCombatEvents.ScheduleEvent(EVENT_SHAO_DO_OUTRO, 10000);
                    nonCombatEvents.ScheduleEvent(EVENT_DEATH, 13000);
                    death = true;

                    HandleDoor(me, GO_CELESTIAL_DOOR, true);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                switch (summon->GetEntry())
                {
                    case NPC_FIRESTORM:
                        summon->CastSpell(summon, SPELL_FIRESTORM, false);
                        break;
                    case NPC_BEACON_OF_HOPE:
                        summon->CastSpell(summon, SPELL_BEACON_OF_HOPE, false);
                        break;
                }
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

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    nonCombatEvents.ScheduleEvent(EVENT_SHAO_DO_INTRO, CELESTIAL_COURT_BOSS_INTRO_TIMER_1);
                    me->SetFacingTo(MIDDLE_FACING_ANGLE);
                    me->SetFaction(FACTION_HOSTILE_NEUTRAL);
                    me->SetHomePosition(_timelessIsleMiddle);
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_REMOVE_BLAZE)
                {
                    events.ScheduleEvent(EVENT_FIRESTORM, 1 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_INSPIRING_SONG, 5 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_BEACON_OF_HOPE, 30 * IN_MILLISECONDS);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                {
                    Talk(SAY_KILL);
                    return;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHAO_DO_INTRO:
                        {
                            Talk(SAY_INTRO);
                            nonCombatEvents.ScheduleEvent(EVENT_SHAO_DO_INTRO_ATTACKABLE, CELESTIAL_COURT_BOSS_INTRO_TIMER_2);
                            break;
                        }
                        case EVENT_SHAO_DO_INTRO_ATTACKABLE:
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetHealth(391000000/*me->GetMaxHealth()*/);
                            break;
                        }
                        case EVENT_SHAO_DO_OUTRO:
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // prisma

                            if (Creature* shao = me->FindNearestCreature(NPC_EMPEROR_SHAOHAO_TI, 300.0f, true))
                                shao->AI()->Talk(EMPEROR_TALK_OUTRO_CHIJI);
                            break;
                        }
                        case EVENT_DEATH:
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
                        case EVENT_BEACON_OF_HOPE:
                        {
                            Talk(SAY_SPELL);

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                DoCast(target, SPELL_BEACON_OF_HOPE_SUMM);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                                DoCast(target, SPELL_BEACON_OF_HOPE_SUMM);

                            events.ScheduleEvent(EVENT_BEACON_OF_HOPE, 50000);
                            events.ScheduleEvent(EVENT_BLAZING_SONG, 2.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_INSPIRING_SONG:
                        {
                            DoCast(me, SPELL_INSPIRING_SONG);
                            events.ScheduleEvent(EVENT_INSPIRING_SONG, 30 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_FIRESTORM:
                        {
                            DoCast(me, SPELL_FIRESTORM_SUMM);
                            events.ScheduleEvent(EVENT_FIRESTORM, 14.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_CRANE_RUSH:
                            events.Reset();
                            Talk(urand(SAY_SPELL_2, SAY_SPELL_3));
                            DoCast(SPELL_CRANE_RUSH);
                            break;
                        case EVENT_BLAZING_SONG:
                            DoCast(me, SPELL_BLAZING_SONG);
                            break;
                        case EVENT_TIMER_HEALTH_POOL:
                        {
                            UpdateHealth(me);
                            events.ScheduleEvent(EVENT_TIMER_HEALTH_POOL, 5000);
                            break;
                        }
                    }
                }

                EnterEvadeIfOutOfCombatArea(diff);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_chi_jiAI(creature);
        }
};

// Child of Chi Ji 71990
struct npc_child_of_chi_ji : public ScriptedAI
{
    npc_child_of_chi_ji(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        me->setActive(true);

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext /*context*/)
        {
            DoCast(me, SPELL_BLAZING_NOVA);

            if (Creature* chiJi = ObjectAccessor::GetCreature(*me, summonerGUID))
            {
                GetPositionWithDistInOrientation(me, 100.0f, chiJi->GetAngle(me), x, y);
                me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), chiJi->GetAngle(me));
                me->DespawnOrUnsummon(me->GetSplineDuration());
            }
        });
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Blazing Of Hope - 144475
class spell_chi_ji_beacon_of_hope : public SpellScript
{
    PrepareSpellScript(spell_chi_ji_beacon_of_hope);

    void DeselectNonPlayer(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_chi_ji_beacon_of_hope::DeselectNonPlayer, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Crane Rush 144470
class spell_chi_ji_crane_rush : public AuraScript
{
    PrepareAuraScript(spell_chi_ji_crane_rush);

    void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_REMOVE_BLAZE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_chi_ji_crane_rush::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_chi_ji()
{
    new boss_chi_ji();
    new creature_script<npc_child_of_chi_ji>("npc_child_of_chi_ji");

    new spell_script<spell_chi_ji_beacon_of_hope>("spell_chi_ji_beacon_of_hope");
    new aura_script<spell_chi_ji_crane_rush>("spell_chi_ji_crane_rush");
}