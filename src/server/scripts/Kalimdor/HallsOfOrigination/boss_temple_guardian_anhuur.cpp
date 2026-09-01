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
#include "Spell.h"
#include "halls_of_origination.h"

enum ScriptTexts
{
    SAY_DEATH       = 0,
    SAY_AGGRO       = 1,
    SAY_EVENT       = 2,
    SAY_KILL        = 3, 
};

enum Spells
{
    SPELL_DIVINE_RECKONING      = 75592,
    SPELL_REVERBERATING_HYMN    = 75322,
    SPELL_SEARING_FLAME         = 75115,
    SPELL_BURNING_LIGHT_SUM     = 75114,
    SPELL_BURNING_LIGHT_DMG     = 75116,
    SPELL_BURNING_LIGHT_FORCE   = 75115,
    SPELL_SHIELD_OF_LIGHT       = 74938,
    SPELL_BEAM_LEFT             = 83697, 
    SPELL_BEAM_RIGHT            = 83698, 
    SPELL_POISON_TIPPED_FANGS   = 74538,
};

enum Events
{
    EVENT_SEARING_FLAME         = 1,
    EVENT_DIVINE_RECKONING      = 2,
    EVENT_POISON_TIPPED_FANGS   = 3,
    EVENT_ACHIEVEMENT           = 4,
    EVENT_SHIELD_OF_LIGHT       = 5,
    EVENT_REVERBERATING_HYMN    = 6,
};

enum Actions
{
    ACTION_ACTIVATE = 1,
};

enum Points
{
    POINT_CENTER = 1,
};

enum Adds
{
    NPC_CAVE_IN         = 40183,
    NPC_PIT_SNAKE       = 39444,
    NPC_SEARING_FLAME   = 40283,

    GO_BEACON_LEFT      = 203133,
    GO_BEACON_RIGHT     = 203136,
};

const Position SpawnPosition[] =
{
    {-654.277f, 361.118f, 52.9508f, 5.86241f},
    {-670.102f, 350.896f, 54.1803f, 2.53073f},
    {-668.896f, 326.048f, 53.2267f, 3.36574f},
    {-618.875f, 344.237f, 52.95f, 0.194356f},
    {-661.667f, 338.78f, 53.0333f, 2.53073f},
    {-607.836f, 348.586f, 53.4939f, 1.0558f},
    {-656.452f, 376.388f, 53.9709f, 1.4525f},
    {-652.762f, 370.634f, 52.9503f, 2.5221f},
    {-682.656f, 343.953f, 53.7329f, 2.53073f},
    {-658.877f, 309.077f, 53.6711f, 0.595064f},
    {-619.399f, 309.049f, 53.4247f, 4.63496f},
    {-612.648f, 318.365f, 53.777f, 3.53434f},
    {-616.398f, 345.109f, 53.0165f, 2.53073f},
    {-681.394f, 342.813f, 53.8955f, 6.24987f},
    {-668.843f, 351.407f, 54.1813f, 5.5293f},
    {-672.797f, 317.175f, 52.9636f, 5.51166f},
    {-631.834f, 375.502f, 55.7079f, 0.738231f},
    {-617.027f, 360.071f, 52.9816f, 2.00725f},
    {-623.891f, 361.178f, 52.9334f, 5.61183f},
    {-614.988f, 331.613f, 52.9533f, 2.91186f},
    {-662.902f, 341.463f, 52.9502f, 2.84307f}
};

const Position bosspos = {-640.527f, 334.855f, 78.345f, 1.54f};

class boss_temple_guardian_anhuur : public CreatureScript
{
    public:
        boss_temple_guardian_anhuur() : CreatureScript("boss_temple_guardian_anhuur") { }

        struct boss_temple_guardian_anhuurAI : public BossAI
        {
            boss_temple_guardian_anhuurAI(Creature* creature) : BossAI(creature, DATA_TEMPLE_GUARDIAN_ANHUUR)
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
            }

            uint8 phase;
            uint8 beacons;
            bool achieve;
            bool casting;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetReactState(REACT_AGGRESSIVE);
                phase = 0;
                beacons = 0;
                achieve = false;
                casting = false;
                summons.DespawnAll();
                me->GetMap()->SetWorldState(WORLDSTATE_I_HATE_THAT_SONG, 1);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_DIVINE_RECKONING, urand(3000, 10000));
                events.ScheduleEvent(EVENT_SEARING_FLAME, urand(2000, 7000));
                Talk(SAY_AGGRO);
                DoZoneInCombat();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
            }

            bool HasAchieved()
            {
                return achieve;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_ACTIVATE)
                {
                    beacons++;
                    if (beacons == 2)
                    {
                        me->RemoveAurasDueToSpell(SPELL_SHIELD_OF_LIGHT);
                        beacons = 0;
                    }
                }
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (me->HasAura(SPELL_SHIELD_OF_LIGHT))
                    return;

                if (spell->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
                    if (me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                        if (me->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_spellInfo->Id == SPELL_REVERBERATING_HYMN)
                        {
                            me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                            casting = false;
                            phase++;
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            events.CancelEvent(EVENT_ACHIEVEMENT);
                            events.ScheduleEvent(EVENT_DIVINE_RECKONING, urand(3000, 10000));
                            events.ScheduleEvent(EVENT_SEARING_FLAME, urand(2000, 7000));
                        }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                switch (summon->GetEntry())
                {
                    case NPC_PIT_SNAKE:
                        if (me->IsInCombat())
                            DoZoneInCombat(summon);
                        break;
                    case NPC_SEARING_LIGHT:
                        summon->CastSpell(summon, SPELL_BURNING_LIGHT_DMG, true);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (casting && !me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                {
                    casting = false;
                    phase++;
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                    events.CancelEvent(EVENT_ACHIEVEMENT);
                    events.ScheduleEvent(EVENT_DIVINE_RECKONING, urand(3000, 10000));
                    events.ScheduleEvent(EVENT_SEARING_FLAME, urand(2000, 7000));
                }

                if (!UpdateVictim())
                    return;

                if ((me->HealthBelowPct(67) && phase == 0) ||
                    (me->HealthBelowPct(34) && phase == 2))
                {
                    events.Reset();
                    me->RemoveAllAuras();
                    beacons = 0;
                    phase++;
                    for (uint32 i = 0; i < 21; ++i)
                        me->SummonCreature(NPC_PIT_SNAKE,SpawnPosition[i],TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000ms);
                    
                    if (GameObject* go = me->FindNearestGameObject(GO_BEACON_LEFT, 100.0f))
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                    if (GameObject* go = me->FindNearestGameObject(GO_BEACON_RIGHT, 100.0f))
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                    Talk(SAY_EVENT);
                    
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    //DoTeleportTo(bosspos.GetPositionX(), bosspos.GetPositionY(), bosspos.GetPositionZ());
                    events.ScheduleEvent(EVENT_SHIELD_OF_LIGHT, 2000);
                    return;
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHIELD_OF_LIGHT:
                            me->SetOrientation(bosspos.GetOrientation());
                            DoCast(me, SPELL_SHIELD_OF_LIGHT, true);
                            events.ScheduleEvent(EVENT_REVERBERATING_HYMN, 1000);
                            break;
                        case EVENT_REVERBERATING_HYMN:
                            if (!achieve)
                            {
                                achieve = true;
                                events.ScheduleEvent(EVENT_ACHIEVEMENT, 15000);
                            }
                            DoCast(me, SPELL_REVERBERATING_HYMN);
                            casting = true;
                            break;
                        case EVENT_ACHIEVEMENT:
                            achieve = false;
                            break;
                        case EVENT_DIVINE_RECKONING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_DIVINE_RECKONING);
                            events.ScheduleEvent(EVENT_DIVINE_RECKONING, urand(15000, 17000));
                            break;
                        case EVENT_SEARING_FLAME:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_BURNING_LIGHT_FORCE);
                            events.ScheduleEvent(EVENT_SEARING_FLAME, urand(8000, 10000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_temple_guardian_anhuurAI>(creature);
        }
};

class npc_pit_snake : public CreatureScript
{
    public:
        npc_pit_snake() : CreatureScript("npc_pit_snake") { }

        struct npc_pit_snakeAI : public ScriptedAI
        {
            npc_pit_snakeAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_POISON_TIPPED_FANGS, urand(2000, 8000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_POISON_TIPPED_FANGS:
                            DoCast(me->GetVictim(), SPELL_POISON_TIPPED_FANGS);
                            events.ScheduleEvent(EVENT_POISON_TIPPED_FANGS, urand(5000, 10000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_pit_snakeAI>(creature);
        }
};

class go_beacon_of_light : public GameObjectScript
{
    public:
        go_beacon_of_light() : GameObjectScript("go_beacon_of_light") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            if (Creature* pAnhuur = go->FindNearestCreature(NPC_TEMPLE_GUARDIAN_ANHUUR, 100.0f))
                pAnhuur->GetAI()->DoAction(ACTION_ACTIVATE);
            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            return false;
        }
};

// Reverberating Hymn 75322
class spell_halls_of_orig_reverberating_hymn : public AuraScript
{
    PrepareAuraScript(spell_halls_of_orig_reverberating_hymn)

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (pAuraEffect->GetBase()->GetDuration() > 165 * IN_MILLISECONDS)
            return;

        if (Unit* owner = GetOwner()->ToUnit())
            owner->GetMap()->SetWorldState(WORLDSTATE_I_HATE_THAT_SONG, 0);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_halls_of_orig_reverberating_hymn::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_temple_guardian_anhuur()
{
    new boss_temple_guardian_anhuur();
    new npc_pit_snake();
    new go_beacon_of_light();
    new aura_script<spell_halls_of_orig_reverberating_hymn>("spell_halls_of_orig_reverberating_hymn");
}
