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
#include "pit_of_saron.h"
#include "Vehicle.h"

enum Spells
{
    // Fallen Warrior
    SPELL_ARCING_SLICE          = 69579,
    SPELL_DEMORALIZING_SHOUT    = 61044,
    SPELL_SHIELD_BLOCK          = 69580,

    // Plagueborn Horror
    SPELL_BLIGHT_BOMB           = 69582,
    SPELL_PUSTULANT_FLESH       = 69581,
    SPELL_TOXIC_WASTE           = 70274,

    // Wrathbone Laborer
    SPELL_BLINDING_DIRT         = 70302,
    SPELL_PUNCTURE_WOUND        = 70278,
    SPELL_SHOVELLED             = 69572,

    // Ymirjar Skycaller
    SPELL_FROSTBLADE            = 70291,
    SPELL_GLACIAL_STRIKE        = 70292,

    // Ymirjar Flamebearer
    SPELL_FIREBALL              = 69583,
    SPELL_HELLFIRE              = 69586,
    SPELL_TACTICAL_BLINK        = 69584,

    // Iceborn Proto-Drake
    SPELL_FROST_BREATH          = 69527,

    // Wrathbone Laborer
    //SPELL_BLINDING_DIRT         = 70302,
    //SPELL_PUNCTURE_WOUND        = 70278,
    //SPELL_SHOVELLED             = 69572,

    // Geist Ambusher
    SPELL_LEAPING_FACE_MAUL     = 69504,
};

enum eEvents
{
    // Ymirjar Flamebearer
    EVENT_FIREBALL              = 1,
    EVENT_TACTICAL_BLINK        = 2,

    // Wrathbone Laborer
    EVENT_BLINDING_DIRT         = 3,
    EVENT_PUNCTURE_WOUND        = 4,
    EVENT_SHOVELLED             = 5,

    // Fallen Warrior
    EVENT_ARCING_SLICE          = 6,
    EVENT_DEMORALIZING_SHOUT    = 7,
    EVENT_SHIELD_BLOCK          = 8,

    // Plagueborn Horror
    EVENT_BLIGHT_BOMB           = 8,
    EVENT_PUSTULANT_FLESH       = 9,
    EVENT_TOXIC_WASTE           = 10,

    // Ymirjar Skycaller
    EVENT_FROSTBLADE            = 11,
    EVENT_GLACIAL_STRIKE        = 12,
};

class npc_ymirjar_flamebearer : public CreatureScript
{
    public:
        npc_ymirjar_flamebearer() : CreatureScript("npc_ymirjar_flamebearer") { }

        struct npc_ymirjar_flamebearerAI: public ScriptedAI
        {
            npc_ymirjar_flamebearerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _events.ScheduleEvent(EVENT_FIREBALL, 4000);
                _events.ScheduleEvent(EVENT_TACTICAL_BLINK, 15000);
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
                        case EVENT_FIREBALL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FIREBALL);
                            _events.RescheduleEvent(EVENT_FIREBALL, 5000);
                            break;
                        case EVENT_TACTICAL_BLINK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_TACTICAL_BLINK);
                            DoCast(me, SPELL_HELLFIRE);
                            _events.RescheduleEvent(EVENT_TACTICAL_BLINK, 12000);
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

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ymirjar_flamebearerAI(creature);
        }
};

class npc_iceborn_protodrake : public CreatureScript
{
    public:
        npc_iceborn_protodrake() : CreatureScript("npc_iceborn_protodrake") { }

        struct npc_iceborn_protodrakeAI: public ScriptedAI
        {
            npc_iceborn_protodrakeAI(Creature* creature) : ScriptedAI(creature), _vehicle(creature->GetVehicleKit())
            {
                ASSERT(_vehicle);
            }

            void Reset() override
            {
                _frostBreathCooldown = 5000;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _vehicle->RemoveAllPassengers();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (_frostBreathCooldown < diff)
                {
                    DoCastVictim(SPELL_FROST_BREATH);
                    _frostBreathCooldown = 10000;
                }
                else
                    _frostBreathCooldown -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            Vehicle* _vehicle;
            uint32 _frostBreathCooldown;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_iceborn_protodrakeAI(creature);
        }
};

class npc_wrathbone_laborer : public CreatureScript
{
    public:
        npc_wrathbone_laborer() : CreatureScript("npc_wrathbone_laborer") { }

        struct npc_wrathbone_laborerAI: public ScriptedAI
        {
            npc_wrathbone_laborerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _events.ScheduleEvent(EVENT_BLINDING_DIRT, 3000);
                _events.ScheduleEvent(EVENT_PUNCTURE_WOUND, urand(3000, 5000));
                _events.ScheduleEvent(EVENT_SHOVELLED, 5000);
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
                        case EVENT_BLINDING_DIRT:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                            //if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f, true))
                                DoCast(target, SPELL_BLINDING_DIRT);
                            _events.RescheduleEvent(EVENT_BLINDING_DIRT, urand(5000, 10000));
                            break;
                        case EVENT_PUNCTURE_WOUND:
                            DoCastVictim(SPELL_PUNCTURE_WOUND);
                            _events.RescheduleEvent(EVENT_PUNCTURE_WOUND, urand(8000, 12000));
                            break;
                        case EVENT_SHOVELLED:
                        if (Unit* target = me->SelectNearbyTarget())
                            //if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, -5.0f))
                                DoCast(target, SPELL_SHOVELLED);
                            _events.RescheduleEvent(EVENT_SHOVELLED, urand(5000, 8000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wrathbone_laborerAI(creature);
        }
};

class npc_geist_ambusher : public CreatureScript
{
    public:
        npc_geist_ambusher() : CreatureScript("npc_geist_ambusher") { }

        struct npc_geist_ambusherAI: public ScriptedAI
        {
            npc_geist_ambusherAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _leapingFaceMaulCooldown = urand(0, 2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (_leapingFaceMaulCooldown < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, -5.0f, true))
                        DoCast(target, SPELL_LEAPING_FACE_MAUL);
                    _leapingFaceMaulCooldown = urand(9000, 14000);
                }
                else
                    _leapingFaceMaulCooldown -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 _leapingFaceMaulCooldown;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_geist_ambusherAI(creature);
        }
};

class spell_trash_npc_glacial_strike : public SpellScriptLoader
{
    public:
        spell_trash_npc_glacial_strike() : SpellScriptLoader("spell_trash_npc_glacial_strike") { }

        class spell_trash_npc_glacial_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trash_npc_glacial_strike_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (GetTarget()->IsFullHealth())
                {
                    Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                    PreventDefaultAction();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_trash_npc_glacial_strike_AuraScript::PeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_trash_npc_glacial_strike_AuraScript();
        }
};

class npc_fallen_warrior : public CreatureScript
{
    public:
        npc_fallen_warrior() : CreatureScript("npc_fallen_warrior") { }

        struct npc_fallen_warriorAI: public ScriptedAI
        {
            npc_fallen_warriorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARCING_SLICE, 8000);
                events.ScheduleEvent(EVENT_DEMORALIZING_SHOUT, 20000);
                events.ScheduleEvent(EVENT_SHIELD_BLOCK, 8000);
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
                        case EVENT_ARCING_SLICE:
                            DoCast(me->GetVictim(), SPELL_ARCING_SLICE);
                            events.RescheduleEvent(EVENT_ARCING_SLICE, 10000);
                            return;
                        case EVENT_DEMORALIZING_SHOUT:
                            DoCast(me, SPELL_DEMORALIZING_SHOUT);
                            events.RescheduleEvent(EVENT_DEMORALIZING_SHOUT, 20000);
                            return;
                        case EVENT_SHIELD_BLOCK:
                            DoCast(me->GetVictim(), SPELL_SHIELD_BLOCK);
                            events.RescheduleEvent(EVENT_SHIELD_BLOCK, 8000);
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fallen_warriorAI(creature);
        }
};

class npc_plagueborn_horror : public CreatureScript
{
    public:
        npc_plagueborn_horror() : CreatureScript("npc_plagueborn_horror") { }

        struct npc_plagueborn_horrorAI: public ScriptedAI
        {
            npc_plagueborn_horrorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BLIGHT_BOMB, 5000);
                events.ScheduleEvent(EVENT_PUSTULANT_FLESH, urand(1000, 5000));
                events.ScheduleEvent(EVENT_TOXIC_WASTE, 100);
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
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);

                    switch (eventId)
                    {
                        case EVENT_BLIGHT_BOMB:
                            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);

                            if (HealthBelowPct(15))
                            {
                                DoCast(me, SPELL_BLIGHT_BOMB);
                                events.CancelEvent(EVENT_BLIGHT_BOMB);
                            }
                            else
                                events.RescheduleEvent(EVENT_BLIGHT_BOMB, 1000);
                            break;
                        case EVENT_PUSTULANT_FLESH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_PUSTULANT_FLESH);
                            events.RescheduleEvent(EVENT_PUSTULANT_FLESH, urand(5000, 10000));
                            break;
                        case EVENT_TOXIC_WASTE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_TOXIC_WASTE);
                            events.RescheduleEvent(EVENT_TOXIC_WASTE, urand(5000, 10000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_plagueborn_horrorAI(creature);
        }
};

class npc_ymirjar_skycaller : public CreatureScript
{
    public:
        npc_ymirjar_skycaller() : CreatureScript("npc_ymirjar_skycaller") { }

        struct npc_ymirjar_skyCallerAI: public ScriptedAI
        {
            npc_ymirjar_skyCallerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FROSTBLADE, 8000);
                events.ScheduleEvent(EVENT_GLACIAL_STRIKE, 5000);
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
                        case EVENT_GLACIAL_STRIKE:
                            if (!me->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISARMED))
                            {
                                DoCastVictim(SPELL_GLACIAL_STRIKE);
                                events.RescheduleEvent(EVENT_GLACIAL_STRIKE, 22000);
                            }
                            else
                                events.RescheduleEvent(EVENT_GLACIAL_STRIKE, 2000);
                            break;
                        case EVENT_FROSTBLADE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FROSTBLADE, true);
                            events.RescheduleEvent(EVENT_FROSTBLADE, 25000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ymirjar_skyCallerAI(creature);
        }
};

void AddSC_pit_of_saron()
{
    new npc_ymirjar_flamebearer();
    new npc_wrathbone_laborer();
    new npc_iceborn_protodrake();
    new npc_geist_ambusher();
    new spell_trash_npc_glacial_strike();
    new npc_fallen_warrior();
    new npc_plagueborn_horror();
    new npc_ymirjar_skycaller();
}
