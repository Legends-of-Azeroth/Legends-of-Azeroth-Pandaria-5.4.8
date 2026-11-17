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
 SDName: boss_Marwyn
 SD%Complete:
 SDCategory: Halls of Reflection
 EndScriptData */

#include "ScriptPCH.h"
#include "halls_of_reflection.h"

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_SLAY                                      = 1,
    SAY_DEATH                                     = 2,
    SAY_GAUNTLET_WIPE                             = 3,
    SAY_INTRO_1                                   = 4,
    SAY_CORRUPTED_FLESH                           = 5,
    SAY_GAUNTLET_RESUME_AFTER_WIPE                = 6,
    SAY_GAUNTLET_RESUME                           = 7,
};

enum Spells
{
    SPELL_OBLITERATE                              = 72360,
    SPELL_WELL_OF_CORRUPTION                      = 72362,
    SPELL_CORRUPTED_FLESH                         = 72363,
    SPELL_SHARED_SUFFERING                        = 72368,
};

enum Events
{
    EVENT_NONE,
    EVENT_OBLITERATE,
    EVENT_WELL_OF_CORRUPTION,
    EVENT_CORRUPTED_FLESH,
    EVENT_SHARED_SUFFERING,
};

class boss_marwyn : public CreatureScript
{
    public:
        boss_marwyn() : CreatureScript("boss_marwyn") { }

        struct boss_marwynAI : public boss_horAI
        {
            boss_marwynAI(Creature* creature) : boss_horAI(creature) { }

            void Reset() override
            {
                boss_horAI::Reset();

                if (instance)
                    instance->SetData(DATA_MARWYN_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetData(DATA_MARWYN_EVENT, IN_PROGRESS);

                events.ScheduleEvent(EVENT_OBLITERATE, 30000);          // TODO Check timer
                events.ScheduleEvent(EVENT_WELL_OF_CORRUPTION, 13000);
                events.ScheduleEvent(EVENT_CORRUPTED_FLESH, 20000);
                events.ScheduleEvent(EVENT_SHARED_SUFFERING, 20000);    // TODO Check timer
            }

            void JustDied(Unit* killer) override
            {
                Talk(SAY_DEATH, killer);

                if (instance)
                    instance->SetData(DATA_MARWYN_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                Talk(SAY_SLAY, victim);
            }

            void DoAction(int32 action) override
            {
                ObjectGuid playerGuid = ObjectGuid::Empty;
                if (Player* player = me->FindNearestPlayer(100.0f))
                    playerGuid = player->GetGUID();

                switch (action)
                {
                    case ACTION_ENTER_COMBAT:  // called by InstanceScript when boss shall enter in combat.
                        // Just in case. Should have been done by InstanceScript
                        me->SetVisible(true);

                        Talk(SAY_AGGRO, ObjectAccessor::GetPlayer(*me, playerGuid));
                        me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);

                        me->m_Events.Schedule(2000, [this]()
                        {
                            me->HandleEmoteStateCommand(0);

                            // Reset flags
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Unit* pUnit = me->SelectNearestTarget())
                                AttackStart(pUnit);

                            DoZoneInCombat();

                            if (!me->IsInCombat())
                                EnterEvadeMode();
                        });
                        break;
                    case ACTION_SAY_INTRO_1:
                        Talk(SAY_INTRO_1, ObjectAccessor::GetPlayer(*me, playerGuid));
                        break;
                    case ACTION_SAY_GAUNTLET_WIPE:
                        Talk(SAY_GAUNTLET_WIPE, ObjectAccessor::GetPlayer(*me, playerGuid));
                        break;
                    case ACTION_SAY_GAUNTLET_RESUME_AFTER_WIPE:
                        Talk(SAY_GAUNTLET_RESUME_AFTER_WIPE, ObjectAccessor::GetPlayer(*me, playerGuid));
                        break;
                    case ACTION_SAY_GAUNTLET_RESUME:
                        Talk(SAY_GAUNTLET_RESUME, ObjectAccessor::GetPlayer(*me, playerGuid));
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_OBLITERATE:
                        DoCast(SPELL_OBLITERATE);
                        events.ScheduleEvent(EVENT_OBLITERATE, 30000);
                        break;
                    case EVENT_WELL_OF_CORRUPTION:
                        DoCast(SPELL_WELL_OF_CORRUPTION);
                        events.ScheduleEvent(EVENT_WELL_OF_CORRUPTION, 13000);
                        break;
                    case EVENT_CORRUPTED_FLESH:
                        Talk(SAY_CORRUPTED_FLESH);
                        DoCast(SPELL_CORRUPTED_FLESH);
                        events.ScheduleEvent(EVENT_CORRUPTED_FLESH, 20000);
                        break;
                    case EVENT_SHARED_SUFFERING:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(target, SPELL_SHARED_SUFFERING);
                        events.ScheduleEvent(EVENT_SHARED_SUFFERING, 20000);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_marwynAI(creature);
        }
};

void AddSC_boss_marwyn()
{
    new boss_marwyn();
}
