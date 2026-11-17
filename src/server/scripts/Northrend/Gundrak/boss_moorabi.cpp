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
#include "gundrak.h"

enum eSpells
{
    SPELL_DETERMINED_STAB                         = 55104,
    SPELL_GROUND_TREMOR                           = 55142,
    SPELL_NUMBING_SHOUT                           = 55106,
    SPELL_DETERMINED_GORE                         = 55102,
    SPELL_QUAKE                                   = 55101,
    SPELL_NUMBING_ROAR                            = 55100,
    SPELL_MOJO_FRENZY                             = 55163,
    SPELL_MOJO_FRENZY_SPEED                       = 55096,
    SPELL_SUMMON_PHANTOM                          = 55097,
    SPELL_SUMMON_PHANTOM_OOC                      = 55205,
    SPELL_TRANSFORMATION                          = 55098,
};

enum eSays
{
    EMOTE_TRANSFORM                               = 0,
    EMOTE_TRANSFORMED                             = 1,
    EMOTE_DEATH                                   = 2,
    SAY_AGGRO                                     = 3,
    SAY_TRANSFORM                                 = 4,
    SAY_QUAKE                                     = 5,
    SAY_SLAY                                      = 6,
    SAY_DEATH                                     = 7,

    SOUND_TRANSFORMED_QUAKE                       = 14724,
};

enum eEvents
{
    EVENT_TRANSFORM                               = 1,
    EVENT_QUAKE,
    EVENT_NUMBING,
    EVENT_DETERMINED,
};

#define DATA_LESS_RABI                            1

class boss_moorabi : public CreatureScript
{
    public:
        boss_moorabi() : CreatureScript("boss_moorabi") { }

        struct boss_moorabiAI : public ScriptedAI
        {
            boss_moorabiAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                oocTimer = urand(20000, 30000);
            }

            void Reset() override
            {
                mammoth = false;
                me->GetMap()->SetWorldState(WORLDSTATE_LESS_RABI, 1);

                me->SetCanDualWield(true);
                const CreatureTemplate* cinfo = me->GetCreatureTemplate();
                float basedmg = sObjectMgr->GetCreatureBaseStats(me->GetLevel(), cinfo->unit_class)->GenerateBaseDamage(cinfo);
                me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, basedmg);
                me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, basedmg * 1.5f);
                me->UpdateDamagePhysical(OFF_ATTACK);

                events.Reset();
                events.ScheduleEvent(EVENT_TRANSFORM, 11000);
                events.ScheduleEvent(EVENT_QUAKE, 25000);
                events.ScheduleEvent(EVENT_NUMBING, 7000);
                events.ScheduleEvent(EVENT_DETERMINED, 6000);

                if (instance)
                    instance->SetData(DATA_MOORABI_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(false);

                oocTimer = 0;
                DoCast(me, SPELL_MOJO_FRENZY);
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_MOORABI_EVENT, IN_PROGRESS);
            }

            void JustReachedHome() override
            {
                ScriptedAI::JustReachedHome();
                oocTimer = urand(20000, 30000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                Talk(EMOTE_DEATH);

                if (instance)
                    instance->SetData(DATA_MOORABI_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_TRANSFORMATION)
                {
                    mammoth = true;
                    me->GetMap()->SetWorldState(WORLDSTATE_LESS_RABI, 0);
                    me->RemoveAurasDueToSpell(SPELL_MOJO_FRENZY); // Cast speed aura stays, intended.
                    Talk(EMOTE_TRANSFORMED);
                    events.CancelEvent(EVENT_TRANSFORM);
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_LESS_RABI)
                    return !mammoth;

                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (oocTimer)
                    {
                        if (oocTimer <= diff)
                        {
                            DoCast(me, SPELL_SUMMON_PHANTOM_OOC);
                            oocTimer = urand(20000, 30000);
                        }
                        else
                            oocTimer -= diff;
                    }
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TRANSFORM:
                            Talk(SAY_TRANSFORM);
                            Talk(EMOTE_TRANSFORM);
                            DoCast(me, SPELL_SUMMON_PHANTOM, true);
                            DoCast(me, SPELL_TRANSFORMATION);
                            events.ScheduleEvent(EVENT_TRANSFORM, me->HealthBelowPct(50) ? 6000 : 11000);
                            break;
                        case EVENT_QUAKE:
                            DoCastAOE(mammoth ? SPELL_QUAKE : SPELL_GROUND_TREMOR);
                            if (mammoth)
                                me->PlayDirectSound(SOUND_TRANSFORMED_QUAKE);
                            else
                                Talk(SAY_QUAKE);
                            events.ScheduleEvent(EVENT_QUAKE, urand(15000, 25000));
                            break;
                        case EVENT_NUMBING:
                            DoCastAOE(mammoth ? SPELL_NUMBING_ROAR : SPELL_NUMBING_SHOUT);
                            events.ScheduleEvent(EVENT_NUMBING, urand(8000, 20000));
                            break;
                        case EVENT_DETERMINED:
                            DoCastAOE(mammoth ? SPELL_DETERMINED_GORE : SPELL_DETERMINED_STAB);
                            events.ScheduleEvent(EVENT_DETERMINED, urand(6000, 15000));
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
             }

        private:
            InstanceScript* instance;
            EventMap events;
            bool mammoth = false;
            uint32 oocTimer = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_moorabiAI(creature);
        }
};

enum DrakkariInciterEnums
{
    SPELL_STRIKE                = 12057,
    SPELL_STRIKE_H              = 15580,

    EVENT_STRIKE                = 1,
    EVENT_SPARRING_START,
    EVENT_SPARRING_CHECK,
    EVENT_SPARRING_EMOTE,
    EVENT_SPARRING_CHEER_SOUTH,
    EVENT_SPARRING_CHEER_NORTH,
    EVENT_SPARRING_BOW,
    EVENT_SPARRING_END,
};

class npc_drakkari_inciter : public CreatureScript
{
    public:
        npc_drakkari_inciter() : CreatureScript("npc_drakkari_inciter") { }

        struct npc_drakkari_inciterAI : public ScriptedAI
        {
            npc_drakkari_inciterAI(Creature* creature) : ScriptedAI(creature)
            {
                events.ScheduleEvent(EVENT_SPARRING_START, 1000);
                events.ScheduleEvent(EVENT_SPARRING_EMOTE, urand(2000, 10000));
                events.ScheduleEvent(EVENT_SPARRING_CHEER_SOUTH, urand(18000, 20000));
                events.ScheduleEvent(EVENT_SPARRING_CHEER_NORTH, urand(12000, 14000));

                me->m_Events.Schedule(1, [this]()
                {
                    std::list<Creature*> earthshakers;
                    GetCreatureListWithEntryInGrid(earthshakers, me, NPC_DRAKKARI_EARTHSHAKER, 20.0f);
                    for (auto&& earthshaker : earthshakers)
                        earthshakerGuids[earthshaker->GetPositionY() > me->GetPositionY() ? 1 : 0] = earthshaker->GetGUID();

                    if (Creature* left = GetLeftEarthshaker())
                        left->SetFaction(1692);
                    if (Creature* right = GetRightEarthshaker())
                        right->SetFaction(1693);

                    std::list<Creature*> spectators;
                    GetCreatureListWithEntryInGrid(spectators, me, NPC_DRAKKARI_LANCER, 40.0f);
                    GetCreatureListWithEntryInGrid(spectators, me, NPC_DRAKKARI_GOD_HUNTER, 40.0f);
                    GetCreatureListWithEntryInGrid(spectators, me, NPC_DRAKKARI_FIRE_WEAVER, 40.0f);
                    GetCreatureListWithEntryInGrid(spectators, me, NPC_DRAKKARI_MEDICINE_MAN, 40.0f);
                    for (auto&& spectator : spectators)
                        spectatorGuids[spectator->GetPositionX() > me->GetPositionX() ? 1 : 0].push_back(spectator->GetGUID());
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_STRIKE, 500);
            }

            void UpdateAI(uint32 diff) override
            {
                UpdateVictim();

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STRIKE:
                            if (me->GetVictim() && me->IsWithinCombatRange(me->GetVictim(), 5.0f))
                            {
                                DoCastVictim(DUNGEON_MODE(SPELL_STRIKE, SPELL_STRIKE_H));
                                events.ScheduleEvent(EVENT_STRIKE, urand(5000, 7000));
                            }
                            else
                                events.ScheduleEvent(EVENT_STRIKE, 500);
                            break;
                        case EVENT_SPARRING_START:
                            if (Creature* left = GetLeftEarthshaker())
                            {
                                if (Creature* right = GetRightEarthshaker())
                                {
                                    if (left->isDead() || right->isDead() || left->IsInCombat() || right->IsInCombat())
                                        break;

                                    right->AI()->AttackStart(left);
                                    left->AI()->AttackStart(right);

                                    events.ScheduleEvent(EVENT_SPARRING_CHECK, 500);
                                }
                            }
                            break;
                        case EVENT_SPARRING_CHECK:
                            if (Creature* left = GetLeftEarthshaker())
                            {
                                if (Creature* right = GetRightEarthshaker())
                                {
                                    if (left->HealthAbovePct(30) && right->HealthAbovePct(30) && left->IsInCombat() && right->IsInCombat())
                                    {
                                        events.ScheduleEvent(EVENT_SPARRING_CHECK, 500);
                                        break;
                                    }

                                    left->SetReactState(REACT_PASSIVE);
                                    right->SetReactState(REACT_PASSIVE);
                                    left->SetFacingToObject(right);
                                    right->SetFacingToObject(left);
                                    left->AttackStop();
                                    right->AttackStop();
                                }
                            }
                            events.ScheduleEvent(EVENT_SPARRING_START, 34000);
                            events.ScheduleEvent(EVENT_SPARRING_BOW, 2000);
                            events.ScheduleEvent(EVENT_SPARRING_END, 6000);
                            break;
                        case EVENT_SPARRING_EMOTE:
                            me->HandleEmoteCommand(RAND(EMOTE_ONESHOT_LAUGH, EMOTE_ONESHOT_FLEX, EMOTE_ONESHOT_POINT, EMOTE_ONESHOT_EXCLAMATION, EMOTE_ONESHOT_ROAR));
                            events.ScheduleEvent(EVENT_SPARRING_EMOTE, urand(2000, 10000));
                            break;
                        case EVENT_SPARRING_CHEER_SOUTH:
                            for (auto&& guid : GetSouthSpectators())
                                if (Creature* spectator = ObjectAccessor::GetCreature(*me, guid))
                                    if (spectator->IsAlive() && !spectator->IsInCombat())
                                        spectator->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                            events.ScheduleEvent(EVENT_SPARRING_CHEER_SOUTH, urand(18000, 20000));
                            break;
                        case EVENT_SPARRING_CHEER_NORTH:
                            for (auto&& guid : GetNorthSpectators())
                                if (Creature* spectator = ObjectAccessor::GetCreature(*me, guid))
                                    if (spectator->IsAlive() && !spectator->IsInCombat())
                                        spectator->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                            events.ScheduleEvent(EVENT_SPARRING_CHEER_NORTH, urand(18000, 20000));
                            break;
                        case EVENT_SPARRING_BOW:
                            if (Creature* left = GetLeftEarthshaker())
                                left->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                            if (Creature* right = GetRightEarthshaker())
                                right->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                            break;
                        case EVENT_SPARRING_END:
                            if (Creature* left = GetLeftEarthshaker())
                            {
                                left->InitializeReactState();
                                left->SetFullHealth();
                                left->AI()->EnterEvadeMode();
                            }
                            if (Creature* right = GetRightEarthshaker())
                            {
                                right->InitializeReactState();
                                right->SetFullHealth();
                                right->AI()->EnterEvadeMode();
                            }
                            break;
                        default:
                            break;
                    }
                }

                if (me->IsInCombat())
                    DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            ObjectGuid earthshakerGuids[2];
            std::vector<ObjectGuid> spectatorGuids[2];

            Creature* GetLeftEarthshaker() const { return ObjectAccessor::GetCreature(*me, earthshakerGuids[0]); }
            Creature* GetRightEarthshaker() const { return ObjectAccessor::GetCreature(*me, earthshakerGuids[1]); }
            std::vector<ObjectGuid> const& GetSouthSpectators() const { return spectatorGuids[0]; }
            std::vector<ObjectGuid> const& GetNorthSpectators() const { return spectatorGuids[1]; }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_inciterAI(creature);
        }
};

enum DrakkariEarthshakerEnums
{
    SPELL_HEAD_CRACK                = 16172,
    SPELL_HEAD_CRACK_H              = 58969,
    SPELL_POWERFUL_BLOW             = 55567,
    SPELL_SLAM_GROUND               = 55563,
    SPELL_POWERFUL_BLOW_SPARRING    = 58612,

    EVENT_HEAD_CRACK = 1,
    EVENT_POWERFUL_BLOW,
    EVENT_SLAM_GROUND,
};

class npc_drakkari_earthshaker : public CreatureScript
{
    public:
        npc_drakkari_earthshaker() : CreatureScript("npc_drakkari_earthshaker") { }

        struct npc_drakkari_earthshakerAI : public ScriptedAI
        {
            npc_drakkari_earthshakerAI(Creature* creature) : ScriptedAI(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                sparring = who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_DRAKKARI_EARTHSHAKER;
                if (sparring)
                {
                    me->SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, 3800);
                    me->SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, 4200);
                }
                else
                    StopSparring();

                events.Reset();
                events.ScheduleEvent(EVENT_HEAD_CRACK, 500);
                events.ScheduleEvent(EVENT_POWERFUL_BLOW, urand(5000, 8000));
                events.ScheduleEvent(EVENT_SLAM_GROUND, urand(12000, 15000));

                sparringEvents.Reset();
                sparringEvents.ScheduleEvent(EVENT_POWERFUL_BLOW, 6000);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_DRAKKARI_EARTHSHAKER)
                    return;

                ScriptedAI::MoveInLineOfSight(who);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (sparring && damage >= me->GetHealth())
                    damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (sparring && me->GetThreatManager().GetThreatListSize() > 1)
                {
                    sparring = false;
                    StopSparring();
                }

                GetEventMap().Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = GetEventMap().ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HEAD_CRACK:
                            DoCastVictim(DUNGEON_MODE(SPELL_HEAD_CRACK, SPELL_HEAD_CRACK_H));
                            GetEventMap().ScheduleEvent(EVENT_HEAD_CRACK, urand(9000, 14000));
                            break;
                        case EVENT_POWERFUL_BLOW:
                            if (sparring)
                            {
                                DoCastVictim(SPELL_POWERFUL_BLOW_SPARRING);
                                GetEventMap().ScheduleEvent(EVENT_POWERFUL_BLOW, 6000);
                            }
                            else
                            {
                                DoCastVictim(SPELL_POWERFUL_BLOW);
                                GetEventMap().ScheduleEvent(EVENT_POWERFUL_BLOW, urand(5000, 7000));
                            }
                            break;
                        case EVENT_SLAM_GROUND:
                            DoCastAOE(SPELL_SLAM_GROUND);
                            GetEventMap().ScheduleEvent(EVENT_SLAM_GROUND, urand(12000, 18000));
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            EventMap sparringEvents;
            bool sparring = false;

            EventMap& GetEventMap() { return sparring ? sparringEvents : events; }

            void StopSparring()
            {
                me->SetFaction(me->GetCreatureTemplate()->faction);

                if (me->GetVictim() && me->GetVictim()->GetTypeId() == TYPEID_UNIT && me->GetVictim()->GetEntry() == NPC_DRAKKARI_EARTHSHAKER)
                {
                    me->AttackStop();
                    me->InterruptNonMeleeSpells(false);
                }

                me->UpdateDamagePhysical(BASE_ATTACK);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_earthshakerAI(creature);
        }
};

class spell_mojo_frenzy : public SpellScriptLoader
{
    public:
        spell_mojo_frenzy() : SpellScriptLoader("spell_mojo_frenzy") { }

        class spell_mojo_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mojo_frenzy_AuraScript);

            void HandlePeriodic(AuraEffect const* /*auraEffect*/)
            {
                if (Unit* target = GetTarget())
                {
                    // Best approximate: 16.3 * (1 - pct)^3
                    float missingHealthPct = 1 - (float)target->GetHealth() / target->GetMaxHealth();
                    target->RemoveAurasDueToSpell(SPELL_MOJO_FRENZY_SPEED);
                    target->CastCustomSpell(SPELL_MOJO_FRENZY_SPEED, SPELLVALUE_BASE_POINT0, 1630.0f * missingHealthPct * missingHealthPct * missingHealthPct, target, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mojo_frenzy_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mojo_frenzy_AuraScript();
        }
};

void AddSC_boss_moorabi()
{
    new boss_moorabi();
    new npc_drakkari_inciter();
    new npc_drakkari_earthshaker();
    new spell_mojo_frenzy();
}
