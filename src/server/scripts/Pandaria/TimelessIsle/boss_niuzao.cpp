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

#define BOSS_NIUZAO 0

const float _chargePos[5][3] = 
{
    { -740.79f, -5019.06f, -6.277f },
    { -740.41f, -4962.37f, -6.277f },
    { -563.80f, -4963.35f, -6.277f },
    { -563.88f, -5069.82f, -6.277f },
    { -737.17f, -5072.31f, -6.277f }
};

enum eSpells
{
    SPELL_NIUZAO_CHARGE                = 144608,
    SPELL_NIUZAO_CHARGE_EFF            = 144609,
    SPELL_NIUZAO_HEADBUTT              = 144610,
    SPELL_NIUZAO_MASSIVE_QUAKE         = 144611,
    SPELL_NIUZAO_OXEN_FORTITUDE        = 144606,
    SPELL_NIUZAO_OXEN_FORTITUDE_ACTIVE = 144607,
};

enum eEvents
{
    EVENT_NIUZAO_OXEN_FORTITUDE = 1,
    EVENT_NIUZAO_HEADBUTT,
    EVENT_NIUZAO_MASSIVE_QUAKE,
    EVENT_NIUZAO_DEFEATED,
    EVENT_NIUZAO_DEATH,
    EVENT_NIUZAO_CHARGE,
    EVENT_NIUZAO_CHARGE_PREPARE,
    EVENT_NIUZAO_SHAO_DO_INTRO,
    EVENT_NIUZAO_SHAO_DO_INTRO_ATTACKABLE,
    EVENT_NIUZAO_SHAO_DO_OUTRO,
    EVENT_TIMER_HEALTH_POOL,
};

enum eSays
{
    SAY_AGGRO,
    SAY_INTRO,
    SAY_DEATH,
    SAY_KILL,
    SAY_SPELL_1,
    SAY_SPELL_2,
    SAY_SPELL_3,
};

enum eActions
{
    ACTION_NIUZAO_CHARGE_66 = 1,
    ACTION_NIUZAO_CHARGE_33,
    ACTION_NIUZAO_CHARGE
};

enum eMovement
{
    MOVEMENT_NIUZAO_CHARGE = 2
};

class boss_niuzao : public CreatureScript
{
    public:
        boss_niuzao() : CreatureScript("boss_niuzao") { }

        struct boss_niuzaoAI : public ScriptedAI
        {
            boss_niuzaoAI(Creature* creature) : ScriptedAI(creature), summons(me) 
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // prisma
            }

            bool _charge66, _charge33 = false;
            bool death = false;
            uint8 _charges;
            float nextCharge;
            SummonList summons;
            EventMap events, nonCombatEvents;

            void Reset() override
            {
                events.Reset();

                me->SetCombatDistance(500.0f);

                if (me->GetFaction() == FACTION_HOSTILE_NEUTRAL)
                    me->SetFacingTo(MIDDLE_FACING_ANGLE);

                me->RemoveAllAuras();
                summons.DespawnAll();
                _charges = 0;
                nextCharge = 66.0f;
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
                events.ScheduleEvent(EVENT_NIUZAO_OXEN_FORTITUDE, 25 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_NIUZAO_MASSIVE_QUAKE, 30 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_NIUZAO_HEADBUTT, 12 * IN_MILLISECONDS);
                HandleDoor(me, GO_CELESTIAL_DOOR, false);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_NIUZAO_CHARGE)
                    me->GetMotionMaster()->MoveCharge(_chargePos[_charges][0], _chargePos[_charges][1], _chargePos[_charges][2], 15.0f, MOVEMENT_NIUZAO_CHARGE);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(nextCharge) && nextCharge > 0.0f)
                {
                    nextCharge -= 33.0f;
                    events.ScheduleEvent(EVENT_NIUZAO_CHARGE_PREPARE, 0.5 * IN_MILLISECONDS);
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
                    nonCombatEvents.ScheduleEvent(EVENT_NIUZAO_SHAO_DO_OUTRO, 10000);
                    nonCombatEvents.ScheduleEvent(EVENT_NIUZAO_DEATH, 13000);
                    death = true;

                    HandleDoor(me, GO_CELESTIAL_DOOR, true);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
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

                switch (pointId)
                {
                    case 1:
                    {
                        nonCombatEvents.ScheduleEvent(EVENT_NIUZAO_SHAO_DO_INTRO, CELESTIAL_COURT_BOSS_INTRO_TIMER_1);
                        me->SetFaction(FACTION_HOSTILE_NEUTRAL);
                        me->SetFacingTo(MIDDLE_FACING_ANGLE);
                        me->SetHomePosition(_timelessIsleMiddle);
                        break;
                    }
                    case MOVEMENT_NIUZAO_CHARGE:
                    {
                        if (_charges >= 4)
                        {
                            _charges = 0;
                            me->RemoveAura(SPELL_NIUZAO_CHARGE);

                            events.Reset();
                            events.ScheduleEvent(EVENT_NIUZAO_OXEN_FORTITUDE, 12 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_NIUZAO_MASSIVE_QUAKE, 15 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_NIUZAO_HEADBUTT, 6 * IN_MILLISECONDS);
                            return;
                        }

                        events.ScheduleEvent(EVENT_NIUZAO_CHARGE, 100);
                        break;
                    }
                    default:
                        break;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
                        return;
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_NIUZAO_SHAO_DO_INTRO:
                        {
                            Talk(SAY_INTRO);
                            nonCombatEvents.ScheduleEvent(EVENT_NIUZAO_SHAO_DO_INTRO_ATTACKABLE, CELESTIAL_COURT_BOSS_INTRO_TIMER_2);
                            break;
                        }
                        case EVENT_NIUZAO_SHAO_DO_INTRO_ATTACKABLE:
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetHealth(391000000/*me->GetMaxHealth()*/);
                            break;
                        }
                        case EVENT_NIUZAO_SHAO_DO_OUTRO:
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); // prisma

                            if (Creature* shao = me->FindNearestCreature(NPC_EMPEROR_SHAOHAO_TI, 300.0f, true))
                                shao->AI()->Talk(EMPEROR_TALK_OUTRO_NIUZAO);
                            break;
                        }
                        case EVENT_NIUZAO_DEATH:
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
                        case EVENT_NIUZAO_CHARGE:
                        {
                            ++_charges;
                            DoAction(ACTION_NIUZAO_CHARGE);
                            break;
                        }
                        case EVENT_NIUZAO_MASSIVE_QUAKE:
                        {
                            if (me->HasAura(SPELL_NIUZAO_CHARGE))
                                return;

                            DoCast(SPELL_NIUZAO_MASSIVE_QUAKE);
                            Talk(SAY_SPELL_3);
                            events.ScheduleEvent(EVENT_NIUZAO_MASSIVE_QUAKE, 35 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_NIUZAO_HEADBUTT:
                        {
                            if (me->HasAura(SPELL_NIUZAO_CHARGE))
                                return;

                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_NIUZAO_HEADBUTT);

                            events.ScheduleEvent(EVENT_NIUZAO_HEADBUTT, 20.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_NIUZAO_OXEN_FORTITUDE:
                        {
                            if (me->HasAura(SPELL_NIUZAO_CHARGE))
                                return;

                            DoCast(SPELL_NIUZAO_OXEN_FORTITUDE);
                            events.ScheduleEvent(EVENT_NIUZAO_OXEN_FORTITUDE, 30 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_NIUZAO_CHARGE_PREPARE:
                            DoCast(SPELL_NIUZAO_CHARGE);
                            Talk(SAY_SPELL_1);
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
            return new boss_niuzaoAI(creature);
        }
};

// Headbutt - 144610
class spell_niuzao_headbutt : public SpellScript
{
    PrepareSpellScript(spell_niuzao_headbutt)

    void RemoveThreat(SpellEffIndex /*eff*/)
    {
        if (!GetHitUnit())
            return;

        if (Unit* target = GetHitUnit())
            GetCaster()->SetRedirectThreat(target->GetGUID(), 1000);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_niuzao_headbutt::RemoveThreat, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
    }
};

// Charge - 144608
class spell_niuzao_charge : public AuraScript
{
    PrepareAuraScript(spell_niuzao_charge)

    void HandleEffectApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* pOwner = GetOwner()->ToCreature())
            pOwner->AI()->DoAction(ACTION_NIUZAO_CHARGE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_niuzao_charge::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_niuzao()
{
    new boss_niuzao();

    new spell_script<spell_niuzao_headbutt>("spell_niuzao_headbutt");
    new aura_script<spell_niuzao_charge>("spell_niuzao_charge");
}
