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
#include "halls_of_lightning.h"

enum Spells
{
    // Titanium Thunderer
    SPELL_COUNTERATTACK        = 52883,
    SPELL_DEADLY_THROW         = 52885,
    SPELL_DEADLY_THROW_H       = 59180,
    SPELL_DEFLECTION           = 52879,
    SPELL_DEFLECTION_H         = 59181,
    SPELL_THROW                = 52904,
    SPELL_THROW_H              = 59179,

    // Titanium Siegebreaker
    SPELL_BLADE_TURNING        = 52891,
    SPELL_BLADE_TURNING_H      = 59173,
    SPELL_FRIGHTENING_SHOUT_10 = 19134, // 10 yards range
    SPELL_FRIGHTENING_SHOUT_15 = 29544, // 15 yards range
    SPELL_PENETRATING_STRIKE   = 52890,
    SPELL_PIERCING_HOWL        = 23600,
    SPELL_SPELL_DAMPING        = 52896,
    SPELL_SPELL_DAMPING_H      = 59177,

    SPELL_FREEZE               = 16245,
    SPELL_AWAKEN               = 52875,

    SPELL_ARC_WELD             = 59097,
};

enum Misc
{
    NPC_TITANIUM_SIEGEBREAKER  = 28961,
    NPC_TITANIUM_THUNDERER     = 28965,

    AT_HALL_OF_THE_WATCHERS_1  = 5082,
    AT_HALL_OF_THE_WATCHERS_2  = 5083,
    AT_HALL_OF_THE_WATCHERS_3  = 5084,
};

enum ThundererEvents
{
    EVENT_COUNTERATTACK = 1,
    EVENT_DEADLY_THROW,
    EVENT_DEFLECTION,
    EVENT_THROW,
};

class npc_titanium_thunderer : public CreatureScript
{
    public:
        npc_titanium_thunderer() : CreatureScript("npc_titanium_thunderer") { }

        struct npc_titanium_thundererAI : public ScriptedAI
        {
            npc_titanium_thundererAI(Creature* creature) : ScriptedAI(creature)
            {
                me->m_CombatDistance = 20.0f;

                if (me->IsAlive())
                    DoCast(me, SPELL_FREEZE, true);
            }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_COUNTERATTACK, urand(16000, 18000));
                events.ScheduleEvent(EVENT_DEADLY_THROW, urand(9000, 12000));
                events.ScheduleEvent(EVENT_DEFLECTION, 15000);
                events.ScheduleEvent(EVENT_THROW, urand(20000, 25000));
            }

            void JustReachedHome() override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_COUNTERATTACK:
                            DoCast(me->GetVictim(), SPELL_COUNTERATTACK);
                            events.ScheduleEvent(EVENT_COUNTERATTACK, urand(16000, 18000));
                            break;
                        case EVENT_DEADLY_THROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                if (me->IsInRange(target, 8.0f, 30.0f, false))
                                    DoCast(target, DUNGEON_MODE(SPELL_DEADLY_THROW, SPELL_DEADLY_THROW_H));

                            events.ScheduleEvent(EVENT_DEADLY_THROW, urand(9000, 12000));
                            break;
                        case EVENT_DEFLECTION:
                            DoCast(me, DUNGEON_MODE(SPELL_DEFLECTION, SPELL_DEFLECTION_H));
                            events.ScheduleEvent(EVENT_DEFLECTION, 15000);
                            break;
                        case EVENT_THROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                if (me->IsInRange(target, 8.0f, 30.0f, false))
                                    DoCast(target, DUNGEON_MODE(SPELL_THROW, SPELL_THROW_H));

                            events.ScheduleEvent(EVENT_THROW, urand(20000, 25000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_titanium_thundererAI(creature);
        }
};

enum SiegebreakerEvents
{
    EVENT_BLADE_TURNING = 1,
    EVENT_PENETRATING_STRIKE,
    EVENT_PIERCING_HOWL,
    EVENT_SPELL_DAMPING,
};

class npc_titanium_siegebreaker : public CreatureScript
{
    public:
        npc_titanium_siegebreaker() : CreatureScript("npc_titanium_siegebreaker") { }

        struct npc_titanium_siegebreakerAI : public ScriptedAI
        {
            npc_titanium_siegebreakerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->m_CombatDistance = 20.0f;
                if (me->IsAlive())
                    DoCast(me, SPELL_FREEZE, true);
            }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_BLADE_TURNING, 12000);
                events.ScheduleEvent(EVENT_PENETRATING_STRIKE, urand(8000, 10000));
                events.ScheduleEvent(EVENT_PIERCING_HOWL, urand(20000, 25000));
                events.ScheduleEvent(EVENT_SPELL_DAMPING, urand(14000, 18000));
            }

            void JustReachedHome() override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* killer) override
            {
                DoCast(killer, DUNGEON_MODE(SPELL_FRIGHTENING_SHOUT_10, SPELL_FRIGHTENING_SHOUT_15));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLADE_TURNING:
                            DoCast(DUNGEON_MODE(SPELL_BLADE_TURNING, SPELL_BLADE_TURNING_H));
                            events.ScheduleEvent(EVENT_BLADE_TURNING, 15000);
                            break;
                        case EVENT_PENETRATING_STRIKE:
                            DoCast(me->GetVictim(), SPELL_PENETRATING_STRIKE);
                            events.ScheduleEvent(EVENT_PENETRATING_STRIKE, urand(10000, 12000));
                            break;
                        case EVENT_PIERCING_HOWL:
                            DoCast(SPELL_PIERCING_HOWL);
                            events.ScheduleEvent(EVENT_PIERCING_HOWL, urand(23000, 27000));
                            break;
                        case EVENT_SPELL_DAMPING:
                            DoCast(DUNGEON_MODE(SPELL_SPELL_DAMPING, SPELL_SPELL_DAMPING_H));
                            events.ScheduleEvent(EVENT_SPELL_DAMPING, urand(17000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_titanium_siegebreakerAI(creature);
        }
};

class at_hall_of_the_watchers : public AreaTriggerScript
{
    public:
        at_hall_of_the_watchers() : AreaTriggerScript("at_hall_of_the_watchers") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            static Position const packs[4] =
            {
                { 1256.571655f, -164.643295f },
                { 1217.440186f, -164.711990f },
                { 1178.418945f, -163.393524f },
                { 1175.898926f, -210.360596f },
            };

            uint32 dataMask;
            uint32 packMask;
            uint32 packCount;

            switch (trigger->ID)
            {
                case AT_HALL_OF_THE_WATCHERS_1:
                    dataMask = 0x1;
                    packMask = 0x3;
                    packCount = 3;
                    break;
                case AT_HALL_OF_THE_WATCHERS_2:
                    dataMask = 0x2;
                    packMask = 0x6;
                    packCount = 3;
                    break;
                case AT_HALL_OF_THE_WATCHERS_3:
                    dataMask = 0x4;
                    packMask = 0xC;
                    packCount = 4;
                    break;
                default:
                    return true;
            }

            // Check if this wave has already been triggered
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(TYPE_HALL_OF_THE_WATCHERS) & dataMask)
                    return true;

                instance->SetData(TYPE_HALL_OF_THE_WATCHERS, dataMask);
            }

            std::list<Creature*> pack;
            GetCreatureListWithEntryInGrid(pack, player, NPC_TITANIUM_SIEGEBREAKER, 200.0f);
            GetCreatureListWithEntryInGrid(pack, player, NPC_TITANIUM_THUNDERER, 200.0f);

            pack.remove_if([packMask](Creature* creature)
            {
                if (!creature->HasAura(SPELL_FREEZE))
                    return true;

                for (uint32 i = 0; i < 4; ++i)
                    if (packMask & (1 << i))
                        if (creature->GetExactDist2d(&packs[i]) <= 25.0f)
                            return false;

                return true;
            });

            Trinity::Containers::RandomResizeList(pack, packCount);

            for (auto&& creature : pack)
            {
                creature->CastSpell(creature, SPELL_AWAKEN, true);
                creature->m_Events.Schedule(2000, [creature]()
                {
                    creature->RemoveAurasDueToSpell(SPELL_FREEZE);
                    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    creature->AI()->DoZoneInCombat(creature, 200.0f);
                });
            }

            return true;
        }
};

class spell_halls_of_lightning_arc_weld : public SpellScriptLoader
{
    public:
        spell_halls_of_lightning_arc_weld() : SpellScriptLoader("spell_halls_of_lightning_arc_weld") { }

        class spell_halls_of_lightning_arc_weld_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_halls_of_lightning_arc_weld_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    if (caster->isMoving())
                        caster->CastSpell(caster, SPELL_ARC_WELD, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_halls_of_lightning_arc_weld_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_halls_of_lightning_arc_weld_SpellScript();
        }
};

void AddSC_halls_of_lightning()
{
    new npc_titanium_thunderer();
    new npc_titanium_siegebreaker();
    new at_hall_of_the_watchers();
    new spell_halls_of_lightning_arc_weld();
}
