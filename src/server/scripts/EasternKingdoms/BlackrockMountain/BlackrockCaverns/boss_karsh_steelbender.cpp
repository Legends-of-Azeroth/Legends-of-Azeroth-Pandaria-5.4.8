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
#include "blackrock_caverns.h"

// todo: сделать скалирование дамага от стаков брони

enum ScriptTexts
{
    SAY_AGGRO     = 0,
    SAY_KILL      = 1,
    SAY_BURN      = 2,
    SAY_DEATH     = 3,
};

enum Spells
{
    SPELL_CLEAVE                                = 15284,
    SPELL_QUICKSILVER_ARMOR                     = 75842,
    SPELL_HEAT_WAVE                             = 75851,
    SPELL_SUPERHEATED_QUICKSILVER_ARMOR         = 75846,
    SPELL_SUPERHEATED_QUICKSILVER_ARMOR_0       = 76015,

    SPELL_BURNING                               = 77490,
};

enum Events
{
     EVENT_CLEAVE          = 1,
     EVENT_HEAT_ARMOR      = 2,
};

enum Adds
{
    NPC_BOUND_FLAMES    = 50417,
};

const Position centerkarshPos = { 237.84f, 784.76f, 95.67f, 1.95f };

class boss_karsh_steelbender : public CreatureScript
{
    public:
        boss_karsh_steelbender() : CreatureScript("boss_karsh_steelbender") { }

        struct boss_karsh_steelbenderAI : public BossAI
        {
            boss_karsh_steelbenderAI(Creature* creature) : BossAI(creature, DATA_KARSH)
            {
               me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
               me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
               me->setActive(true);
            }

            void Reset() override
            {
                _Reset();
                DoCast(me, SPELL_QUICKSILVER_ARMOR);
                bHeat = false;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->GetMap()->SetWorldState(WORLDSTATE_TOO_HOT_TO_HANDLE, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 7000));
                events.ScheduleEvent(EVENT_HEAT_ARMOR, 1000);
                Talk(SAY_AGGRO);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }
     
            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }
     
                bHeat = (me->GetDistance(centerkarshPos) <= 6.0f) ? true : false;

                if (!me->HasAura(SPELL_QUICKSILVER_ARMOR) && !me->HasAura(SPELL_SUPERHEATED_QUICKSILVER_ARMOR))
                    DoCast(me, SPELL_QUICKSILVER_ARMOR);
                if (me->HasAura(SPELL_SUPERHEATED_QUICKSILVER_ARMOR))
                    me->RemoveAurasDueToSpell(SPELL_QUICKSILVER_ARMOR);

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CLEAVE:
                            DoCast(me->GetVictim(), SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 7000));
                            break;
                        case EVENT_HEAT_ARMOR:
                            if (bHeat)
                            {
                                DoCast(me, SPELL_HEAT_WAVE);
                                DoCast(me, SPELL_SUPERHEATED_QUICKSILVER_ARMOR);
                            }
                            events.ScheduleEvent(EVENT_HEAT_ARMOR, 1000);
                            break;
                    }
                    DoMeleeAttackIfReady();
                }
            }

            private:
                bool bHeat;        
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_karsh_steelbenderAI>(creature);
        }
};

// Superheated Quicksilver Armor 75846
class spell_superheated_quicksilver_armor : public AuraScript
{
    PrepareAuraScript(spell_superheated_quicksilver_armor);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 14 && GetOwner() && GetOwner()->ToUnit())
            GetOwner()->ToUnit()->GetMap()->SetWorldState(WORLDSTATE_TOO_HOT_TO_HANDLE, 1);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_superheated_quicksilver_armor::HandleAuraEffectApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAPPLY);
    }
};
 
void AddSC_boss_karsh_steelbender()
{
    new boss_karsh_steelbender();
    new aura_script<spell_superheated_quicksilver_armor>("spell_superheated_quicksilver_armor");
}
