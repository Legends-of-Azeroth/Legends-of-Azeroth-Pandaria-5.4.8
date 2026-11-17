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

enum VanndarSpellData
{
    SPELL_AVATAR                            = 19135,
    SPELL_THUNDERCLAP                       = 15588,
    SPELL_STORMBOLT                         = 20685  // not sure
};

enum VanndarTexts
{
    YELL_AGGRO                              = 0,
    YELL_EVADE                              = 1,
    YELL_RESPAWN                            = 2,
    YELL_RANDOM                             = 3,
    YELL_SPELL                              = 4
};

enum VanndarEvents
{
    EVENT_CAST_SPELL_AVATAR                 = 1,
    EVENT_CAST_SPELL_THUNDERCLAP            = 2,
    EVENT_CAST_SPELL_STORMBOLT              = 3,
    EVENT_RANDOM_YELLS                      = 4,
    EVENT_VANNDAR_RESET                     = 5
};

struct boss_vanndar : public ScriptedAI
{
    boss_vanndar(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(YELL_AGGRO);
        _events.ScheduleEvent(EVENT_CAST_SPELL_AVATAR, 3s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_THUNDERCLAP, 4s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_STORMBOLT, 6s);
        _events.ScheduleEvent(EVENT_RANDOM_YELLS, randtime(20s, 30s));
        _events.ScheduleEvent(EVENT_VANNDAR_RESET, 5s);
    }

    void JustAppeared() override
    {
        Reset();
        Talk(YELL_RESPAWN);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CAST_SPELL_AVATAR:
                    DoCastVictim(SPELL_AVATAR);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_AVATAR, randtime(15s, 20s));
                    break;
                case EVENT_CAST_SPELL_THUNDERCLAP:
                    DoCastVictim(SPELL_THUNDERCLAP);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_THUNDERCLAP, randtime(5s, 15s));
                    break;
                case EVENT_CAST_SPELL_STORMBOLT:
                    DoCastVictim(SPELL_STORMBOLT);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_STORMBOLT, randtime(10s, 25s));
                    break;
                case EVENT_RANDOM_YELLS:
                    Talk(YELL_RANDOM);
                    _events.ScheduleEvent(EVENT_RANDOM_YELLS, randtime(20s, 30s));
                    break;
                case EVENT_VANNDAR_RESET:
                    // Check if creature is not outside of building
                    if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
                    {
                        EnterEvadeMode();
                        Talk(YELL_EVADE);
                    }
                    _events.ScheduleEvent(EVENT_VANNDAR_RESET, 5s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

void AddSC_boss_vanndar()
{
    new creature_script<boss_vanndar>("boss_vanndar");
}