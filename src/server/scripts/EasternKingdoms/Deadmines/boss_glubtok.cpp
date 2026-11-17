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
#include "deadmines.h"

//todo: реализовать flame wall

enum ScriptTexts
{
    SAY_DEATH        = 0,
    SAY_ARCANE_POWER = 1,
    SAY_AGGRO        = 2,
    SAY_KILL         = 3,
    SAY_FIRE         = 4,
    SAY_HEAD1        = 5,
    SAY_FROST        = 6,
    SAY_HEAD2        = 7,
};
enum Spells
{
    SPELL_ARCANE_POWER         = 88009,
    SPELL_FIST_OF_FLAME        = 87859,
    SPELL_FIST_OF_FLAME_0      = 87896,
    SPELL_FIST_OF_FROST        = 87861,
    SPELL_FIST_OF_FROST_0      = 87901,
    SPELL_FIRE_BLOSSOM         = 88129,
    SPELL_FROST_BLOSSOM        = 88169,
    SPELL_FROST_BLOSSOM_0      = 88177,
    SPELL_BLINK                = 38932,

    // Misc
    SPELL_FIRE_BLOSSOM_VISUAL  = 88164,
    SPELL_FROST_BLOSSOM_VISUAL = 88165, // casted by bunny
    SPELL_FIRE_BEAM_VISUAL     = 45576, 
    SPELL_FROST_BEAM_VISUAL    = 99490,
};

enum Events
{
    EVENT_FIST_OF_FLAME = 1,
    EVENT_FIST_OF_FROST = 2,
    EVENT_BLINK         = 3,
    EVENT_BLOSSOM       = 4,
    EVENT_ARCANE_POWER1 = 5,
    EVENT_ARCANE_POWER2 = 6,
    EVENT_ARCANE_POWER3 = 7,
};

enum Adds
{
    NPC_FIRE_BLOSSOM    = 48957,
    NPC_FROST_BLOSSOM   = 48958,
    NPC_FIREWALL_2A     = 48976,
    NPC_FIREWALL_1A     = 48975,
    NPC_FIREWALL_1B     = 49039,
    NPC_FIREWALL_2B     = 49041,
    NPC_FIREWALL_2C     = 49042,
};

class boss_glubtok : public CreatureScript
{
    public:
        boss_glubtok() : CreatureScript("boss_glubtok") { }

        struct boss_glubtokAI : public BossAI
        {
            boss_glubtokAI(Creature* creature) : BossAI(creature, DATA_GLUBTOK)
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            uint8 stage;
     
            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->RemoveAurasDueToSpell(SPELL_FROST_BLOSSOM_VISUAL);
                me->RemoveAurasDueToSpell(SPELL_FIRE_BEAM_VISUAL);

                stage = 0;
                me->SetReactState(REACT_AGGRESSIVE);
            }

            void JustEngagedWith(Unit* /*who*/) override 
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                stage = 0;
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_FIST_OF_FLAME, 10000);
                DoZoneInCombat();
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                summon->CastSpell(summon, summon->GetEntry() == NPC_FIRE_BLOSSOM ? SPELL_FIRE_BLOSSOM_VISUAL : SPELL_FROST_BLOSSOM_VISUAL, false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->RemoveAurasDueToSpell(SPELL_FROST_BLOSSOM_VISUAL);
                me->RemoveAurasDueToSpell(SPELL_FIRE_BEAM_VISUAL);

                Talk(SAY_DEATH);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetHealthPct()<=50 && stage == 0)
                {
                    stage = 1;
                    events.Reset();
                    me->SetReactState(REACT_PASSIVE);
                    Talk(SAY_HEAD1);
                    events.ScheduleEvent(EVENT_ARCANE_POWER1, 1800);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_POWER1:
                            Talk(SAY_HEAD2);
                            events.ScheduleEvent(EVENT_ARCANE_POWER2, 2200);
                            break;
                        case EVENT_ARCANE_POWER2:
                            me->NearTeleportTo(-193.43f,-437.86f,54.38f,4.88f,true);
                            SetCombatMovement(false);
                            me->AttackStop();
                            me->RemoveAllAuras();
                            InitHandleBeamEvent();
                            events.ScheduleEvent(EVENT_ARCANE_POWER3, 1000);
                            break;
                        case EVENT_ARCANE_POWER3:
                            SetCombatMovement(false);
                            DoCast(me, SPELL_ARCANE_POWER, true);
                            Talk(SAY_ARCANE_POWER);
                            events.ScheduleEvent(EVENT_BLOSSOM, 5000);
                            break;
                        case EVENT_BLOSSOM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                    DoCast(target, urand(0, 1) ? SPELL_FIRE_BLOSSOM : SPELL_FROST_BLOSSOM);
                            events.ScheduleEvent(EVENT_BLOSSOM, 5000);
                            break;
                        case EVENT_FIST_OF_FLAME:
                            DoCast(me, SPELL_FIST_OF_FLAME);
                            Talk(SAY_FIRE);
                            events.ScheduleEvent(EVENT_BLINK, 20000);
                            events.ScheduleEvent(EVENT_FIST_OF_FROST, 20500);
                            break;
                        case EVENT_FIST_OF_FROST:
                            DoCast(me, SPELL_FIST_OF_FROST);
                            Talk(SAY_FROST);
                            events.ScheduleEvent(EVENT_BLINK, 20000);
                            events.ScheduleEvent(EVENT_FIST_OF_FLAME, 20500);
                            break;
                        case EVENT_BLINK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                            {
                                DoCast(target, SPELL_BLINK);
                                if (IsHeroic())
                                    DoResetThreat();
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                void InitHandleBeamEvent()
                {
                    std::list<Creature*> FireList;
                    std::list<Creature*> FrostList;

                    GetCreatureListWithEntryInGrid(FireList, me, NPC_FIRE_BLOSSOM_BUNNY, 100.0f);
                    GetCreatureListWithEntryInGrid(FrostList, me, NPC_FROST_BLOSSOM_BUNNY, 100.0f);

                    if (FireList.size() > 4)
                        Trinity::Containers::RandomResizeList(FireList, 4);

                    if (FrostList.size() > 3)
                        Trinity::Containers::RandomResizeList(FrostList, 3);

                    for (auto &fItr : FireList)
                        fItr->CastSpell(me, SPELL_FIRE_BEAM_VISUAL, false);

                    for (auto &frItr : FrostList)
                        frItr->CastSpell(me, SPELL_FROST_BEAM_VISUAL, false);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_glubtokAI>(creature);
        }
};

void AddSC_boss_glubtok()
{
    new boss_glubtok();
}