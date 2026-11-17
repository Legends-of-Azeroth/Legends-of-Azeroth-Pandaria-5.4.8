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

enum BalindaSpellData
{
    SPELL_ARCANE_EXPLOSION                  = 46608,
    SPELL_CONE_OF_COLD                      = 38384,
    SPELL_FIREBALL                          = 46988,
    SPELL_FROSTBOLT                         = 46987
};

enum BalindaTexts
{
    YELL_AGGRO                              = 0,
    YELL_EVADE                              = 1,
    YELL_HALF_HP                            = 2
};

enum BalindaEvents
{
    EVENT_CAST_SPELL_ARCANE_EXPLOSION       = 1,
    EVENT_CAST_SPELL_CONE_OF_COLD           = 2,
    EVENT_CAST_SPELL_FIREBALL               = 3,
    EVENT_CAST_SPELL_FROSTBOLT              = 4,
    EVENT_SUMMON_WATER_ELEMENTAL            = 5,
    EVENT_HEALTH_CHECK                      = 6,
    EVENT_BALINDA_RESET                     = 7
};

enum BalindaCreatures
{
    NPC_WATER_ELEMENTAL                     = 25040
};

enum BalindaWaterElementalSpellData
{
    SPELL_WATERBOLT                         = 46983
};

enum BalindaWaterElementalEvents
{
    EVENT_CAST_SPELL_WATERBOLT              = 1,
    EVENT_WATER_ELEMENTAL_RESET             = 2
};

class npc_water_elemental : public CreatureScript
{
    public:
        npc_water_elemental(const char *ScriptName) : CreatureScript(ScriptName) { }

    struct npc_water_elementalAI : public ScriptedAI
    {
        npc_water_elementalAI(Creature* creature) : ScriptedAI(creature) { }

        ObjectGuid balindaGUID;

        void Reset() override
        {
            _events.Reset();
            balindaGUID = ObjectGuid::Empty;
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            Talk(YELL_AGGRO);
            _events.ScheduleEvent(EVENT_CAST_SPELL_WATERBOLT, 3s);
            _events.ScheduleEvent(EVENT_WATER_ELEMENTAL_RESET, 5s);
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
                    case EVENT_CAST_SPELL_WATERBOLT:
                        DoCastVictim(SPELL_WATERBOLT);
                        _events.ScheduleEvent(EVENT_CAST_SPELL_WATERBOLT, 5s);
                        break;
                    case EVENT_WATER_ELEMENTAL_RESET:
                        // Check if creature is not outside of building
                        if (Creature* pBalinda = Unit::GetCreature(*me, balindaGUID))
                            if (me->GetDistance2d(pBalinda->GetHomePosition().GetPositionX(), pBalinda->GetHomePosition().GetPositionY()) > 50)
                                EnterEvadeMode();

                        _events.ScheduleEvent(EVENT_WATER_ELEMENTAL_RESET, 5s);
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

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_water_elementalAI(creature);
    }
};

struct boss_balinda : public ScriptedAI
{
    boss_balinda(Creature* creature) : ScriptedAI(creature), summons(me)
    {
        Initialize();
    }

    void Initialize()
    {
        summons.DespawnAll();
    }

    void Reset() override
    {
        _events.Reset();
        Initialize();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(YELL_AGGRO);
        _events.ScheduleEvent(EVENT_CAST_SPELL_ARCANE_EXPLOSION, randtime(5s, 15s));
        _events.ScheduleEvent(EVENT_CAST_SPELL_CONE_OF_COLD, 8s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_FIREBALL, 1s);
        _events.ScheduleEvent(EVENT_CAST_SPELL_FROSTBOLT, 4s);
        _events.ScheduleEvent(EVENT_SUMMON_WATER_ELEMENTAL, 0ms);
        _events.ScheduleEvent(EVENT_HEALTH_CHECK, 1s);
        _events.ScheduleEvent(EVENT_BALINDA_RESET, 5s);
    }

    void JustAppeared() override
    {
        Reset();
    }

    void JustSummoned(Creature* summoned) override
    {
        CAST_AI(npc_water_elemental::npc_water_elementalAI, summoned->AI())->balindaGUID = me->GetGUID();
        summoned->AI()->AttackStart(SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true));
        summoned->SetFaction(me->GetFaction());
        summons.Summon(summoned);
    }

    void JustDied(Unit* /*killer*/) override
    {
        summons.DespawnAll();
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
                case EVENT_CAST_SPELL_ARCANE_EXPLOSION:
                    DoCastVictim(SPELL_ARCANE_EXPLOSION);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_ARCANE_EXPLOSION, randtime(5s, 15s));
                    break;
                case EVENT_CAST_SPELL_CONE_OF_COLD:
                    DoCastVictim(SPELL_CONE_OF_COLD);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_CONE_OF_COLD, randtime(10s, 20s));
                    break;
                case EVENT_CAST_SPELL_FIREBALL:
                    DoCastVictim(SPELL_FIREBALL);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_FIREBALL, randtime(5s, 9s));
                    break;
                case EVENT_CAST_SPELL_FROSTBOLT:
                    DoCastVictim(SPELL_FROSTBOLT);
                    _events.ScheduleEvent(EVENT_CAST_SPELL_FROSTBOLT, randtime(4s, 12s));
                    break;
                case EVENT_SUMMON_WATER_ELEMENTAL:
                    if (summons.empty())
                        me->SummonCreature(NPC_WATER_ELEMENTAL, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, Milliseconds(45 * IN_MILLISECONDS));

                    _events.ScheduleEvent(EVENT_SUMMON_WATER_ELEMENTAL, 50s);
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
                case EVENT_BALINDA_RESET:
                    // Check if creature is not outside of building
                    if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
                    {
                        EnterEvadeMode();
                        Talk(YELL_EVADE);
                    }
                    _events.ScheduleEvent(EVENT_BALINDA_RESET, 5s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    SummonList summons;
    EventMap _events;
};

void AddSC_boss_balinda()
{
    new npc_water_elemental("npc_water_elemental");
    new creature_script<boss_balinda>("boss_balinda");
};
