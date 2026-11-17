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
#include "sunken_temple.h"

enum ScriptTexts
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,
    SAY_KILL  = 2,
};

enum Events
{
    EVENT_INTRO            = 1,
    EVENT_CAUSE_INSANITY   = 2,
    EVENT_CURSE_OF_TONGUES = 3,
    EVENT_LASH             = 4,
};

enum Spells
{
    SPELL_CAUSE_OF_INSANITY = 12888,
    SPELL_CURSE_OF_TONGUES  = 12889,
    SPELL_LASH              = 59678,
};

class boss_avatar_of_hakkar : public CreatureScript
{
    public:
        boss_avatar_of_hakkar() : CreatureScript("boss_avatar_of_hakkar") { }

        struct boss_avatar_of_hakkarAI : public BossAI
        {
            boss_avatar_of_hakkarAI(Creature* creature) : BossAI(creature, DATA_HAKKAR) { }

            EventMap nonCombatEvents;

            void InitializeAI() override
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
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                nonCombatEvents.Reset();
                HandleRemoveCharmedPlayers(false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_INTRO, 2 * IN_MILLISECONDS);
                DoZoneInCombat();
                instance->SetBossState(DATA_HAKKAR, IN_PROGRESS);

                events.ScheduleEvent(EVENT_CURSE_OF_TONGUES, urand(6 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CAUSE_INSANITY, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_LASH, 8 * IN_MILLISECONDS);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                HandleRemoveCharmedPlayers();
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_HAKKAR_RISE)
                {
                    me->SetVisible(true);
                    nonCombatEvents.ScheduleEvent(EVENT_INTRO, 2 * IN_MILLISECONDS);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SetData(DATA_HAKKAR, DONE);
                HandleRemoveCharmedPlayers(false);
            }

            void HandleRemoveCharmedPlayers(bool death = true)
            {
                std::list<Player*> m_targets;
                GetPlayerListInGrid(m_targets, me, 300.0f);

                if (!m_targets.empty())
                    for (auto itr : m_targets)
                        if (itr->IsCharmed())
                            death ? me->Kill(itr) : itr->RemoveCharmAuras();
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INTRO)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                    break;
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
                        case EVENT_CURSE_OF_TONGUES:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, TonguePredicate(me)))
                                DoCast(target, SPELL_CURSE_OF_TONGUES);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_CURSE_OF_TONGUES);

                            events.ScheduleEvent(EVENT_CURSE_OF_TONGUES, urand(14 * IN_MILLISECONDS, 14.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_LASH:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_LASH);

                            events.ScheduleEvent(EVENT_LASH, 8 * IN_MILLISECONDS);
                            break;
                        case EVENT_CAUSE_INSANITY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_CAUSE_OF_INSANITY);

                            events.ScheduleEvent(EVENT_CAUSE_INSANITY, 20 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct TonguePredicate 
                {
                    public:
                        TonguePredicate(Creature const* me) : _me(me) { }

                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) > 6.0f;
                        }

                    private:
                        Creature const* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_avatar_of_hakkarAI(creature);
        }
};

// Shrine of soulflayer 208321
class go_shrine_of_soulflayer : public GameObjectScript
{
    public:
        go_shrine_of_soulflayer() : GameObjectScript("go_shrine_of_soulflayer") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* m_Hakkar = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_AVATAR_OF_HAKKAR)))
                    m_Hakkar->AI()->DoAction(ACTION_HAKKAR_RISE);

            return true;
        }
};

void AddSC_avatar_of_hakkar()
{
    new boss_avatar_of_hakkar();
    new go_shrine_of_soulflayer();
}