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

enum DrektharSpellData
{
    SPELL_WHIRLWIND                         = 15589,
    SPELL_WHIRLWIND_2                       = 13736,
    SPELL_KNOCKDOWN                         = 19128,
    SPELL_FRENZY                            = 8269,
    SPELL_SWEEPING_STRIKES                  = 18765, // not sure
    SPELL_CLEAVE                            = 20677, // not sure
    SPELL_WINDFURY                          = 35886, // not sure
    SPELL_STORMPIKE                         = 51876  // not sure
};

enum DrektharTexts
{
    YELL_AGGRO                              = 0,
    YELL_EVADE                              = 1,
    YELL_RESPAWN                            = 2,
    YELL_RANDOM                             = 3
};

enum DrektharEvents
{
    EVENT_CAST_SPELL_WHIRLWIND              = 1,
    EVENT_CAST_SPELL_WHIRLWIND_2            = 2,
    EVENT_CAST_SPELL_KNOCKDOWN              = 3,
    EVENT_CAST_SPELL_FRENZY                 = 4,
    EVENT_RANDOM_YELLS                      = 5,
    EVENT_DREKTHAR_RESET                    = 6
};

struct boss_drekthar : public ScriptedAI
{
    boss_drekthar(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(YELL_AGGRO);
        _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND, randtime(1s, 20s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND_2, randtime(1s, 20s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_KNOCKDOWN, 12s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_FRENZY, 6s);
        _events.ScheduleEvent(EVENT_RANDOM_YELLS, randtime(20s, 30s));
        _events.ScheduleEvent(EVENT_DREKTHAR_RESET, 5s);
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
                case EVENT_CAST_SPELL_WHIRLWIND:
                    DoCastVictim(SPELL_WHIRLWIND);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND, randtime(8s, 18s));
                    break;
                case EVENT_CAST_SPELL_WHIRLWIND_2:
                    DoCastVictim(SPELL_WHIRLWIND_2);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND_2, randtime(7s, 25s));
                    break;
                case EVENT_CAST_SPELL_KNOCKDOWN:
                    DoCastVictim(SPELL_KNOCKDOWN);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_KNOCKDOWN, randtime(10s, 15s));
                    break;
                case EVENT_CAST_SPELL_FRENZY:
                    DoCastVictim(SPELL_FRENZY);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_FRENZY, randtime(20s, 30s));
                    break;
                case EVENT_RANDOM_YELLS:
                    Talk(YELL_RANDOM);
                    _events.ScheduleEvent(EVENT_RANDOM_YELLS, randtime(20s, 30s));
                    break;
                case EVENT_DREKTHAR_RESET:
                    // Check if creature is not outside of building
                    if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
                    {
                        EnterEvadeMode();
                        Talk(YELL_EVADE);
                    }
                    _events.ScheduleEvent(EVENT_DREKTHAR_RESET, 5s);
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

void AddSC_boss_drekthar()
{
    new creature_script<boss_drekthar>("boss_drekthar");
}
