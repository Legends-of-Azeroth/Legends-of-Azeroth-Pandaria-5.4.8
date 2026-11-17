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

enum GalvangarSpellData
{
    SPELL_CLEAVE                            = 15284,
    SPELL_FRIGHTENING_SHOUT                 = 19134,
    SPELL_WHIRLWIND                         = 15589,
    SPELL_WHIRLWIND_2                       = 13736,
    SPELL_MORTAL_STRIKE                     = 16856
};

enum GalvangarTexts
{
    YELL_AGGRO                              = 0,
    YELL_EVADE                              = 1,
    YELL_HALF_HP                            = 2
};

enum GalvangarEvents
{
    EVENT_CAST_SPELL_CLEAVE                 = 1,
    EVENT_CAST_SPELL_FRIGHTENING_SHOUT      = 2,
    EVENT_CAST_SPELL_WHIRLWIND              = 3,
    EVENT_CAST_SPELL_WHIRLWIND_2            = 4,
    EVENT_CAST_SPELL_MORTAL_STRIKE          = 5,
    EVENT_HEALTH_CHECK                      = 6,
    EVENT_GALVANGAR_RESET                   = 7
};

struct boss_galvangar : public ScriptedAI
{
    boss_galvangar(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(YELL_AGGRO);
        _events.ScheduleEvent(EVENT_CAST_SPELL_CLEAVE, randtime(1s, 9s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_FRIGHTENING_SHOUT, randtime(2s, 19s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND, randtime(1s, 13s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND_2, randtime(5s, 20s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_MORTAL_STRIKE, randtime(5s, 20s));
        _events.ScheduleEvent(EVENT_HEALTH_CHECK, 1s);
        _events.ScheduleEvent(EVENT_GALVANGAR_RESET, 5s);
    }

    void JustAppeared() override
    {
        Reset();
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
                case EVENT_CAST_SPELL_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_CLEAVE, randtime(10s, 16s));
                    break;
                case EVENT_CAST_SPELL_FRIGHTENING_SHOUT:
                    DoCastVictim(SPELL_FRIGHTENING_SHOUT);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_FRIGHTENING_SHOUT, randtime(10s, 15s));
                    break;
                case EVENT_CAST_SPELL_WHIRLWIND:
                    DoCastVictim(SPELL_WHIRLWIND);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND, randtime(6s, 10s));
                    break;
                case EVENT_CAST_SPELL_WHIRLWIND_2:
                    DoCastVictim(SPELL_WHIRLWIND_2);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_WHIRLWIND_2, randtime(10s, 25s));
                    break;
                case EVENT_CAST_SPELL_MORTAL_STRIKE:
                    DoCastVictim(SPELL_MORTAL_STRIKE);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_MORTAL_STRIKE, randtime(10s, 30s));
                    break;
                case EVENT_HEALTH_CHECK:
                    if (me->GetHealthPct() < 50.0f)
                    {
                        Talk(YELL_HALF_HP);
                        _events.PopEvent();
                        break;
                    }
                    _events.RepeatEvent(1s);
                    break;
                case EVENT_GALVANGAR_RESET:
                    // Check if creature is not outside of building
                    if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
                    {
                        EnterEvadeMode();
                        Talk(YELL_EVADE);
                    }
                    _events.ScheduleEvent(EVENT_GALVANGAR_RESET, 5s);
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

void AddSC_boss_galvangar()
{
    new creature_script<boss_galvangar>("boss_galvangar");
}
