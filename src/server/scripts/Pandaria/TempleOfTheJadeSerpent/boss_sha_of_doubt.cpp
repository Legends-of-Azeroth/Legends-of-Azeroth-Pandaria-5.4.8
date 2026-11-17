/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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
#include "MoveSplineInit.h"
#include "temple_of_the_jade_serpent.h"

class boss_sha_of_doubt : public CreatureScript
{
    public:
        boss_sha_of_doubt() : CreatureScript("boss_sha_of_doubt") { }

        enum eSpells
        {
            SPELL_WITHER_WILL            = 106736,
            SPELL_TOUCH_OF_NOTHINGNESS   = 106113,
            SPELL_BOUNDS_OF_REALITY      = 117665,
            SPELL_FIGMENT_OF_DOUBT_2     = 106936,
            SPELL_INVISIBILITY_DETECTION = 126839
        };

        enum eEvents
        {
            EVENT_WITHER_WILL          = 1,
            EVENT_TOUCH_OF_NOTHINGNESS = 2,
            EVENT_BOUNDS_OF_REALITY    = 3,
            EVENT_CLOSE_DOOR           = 4,
            EVENT_OPEN_DOOR            = 5
        };

        enum eActions
        {
            ACTION_FIGMENT_DIE = 0
        };

        enum eTalks
        {
            TALK_AGGRO   = 0,
            TALK_DEATH   = 1,
            TALK_FIGMENT = 2,
            TALK_RESET   = 3,
            TALK_SLAY    = 4
        };

        struct boss_sha_of_doubt_AI : public BossAI
        {
            boss_sha_of_doubt_AI(Creature* creature) : BossAI(creature, DATA_SHA_OF_DOUBT) { }

            uint32 figmentsCount, figmentsDie, delay;
            EventMap nonCombatEvents;

            void InitializeAI() override
            {
                me->CastSpell(me, SPELL_INVISIBILITY_DETECTION, false);
                
                if (instance)
                {
                    instance->SetData(DATA_SHA_OF_DOUBT, NOT_STARTED);
                    if (instance->GetBossState(DATA_LIU) == DONE)
                    {
                        nonCombatEvents.ScheduleEvent(EVENT_OPEN_DOOR, 1 * IN_MILLISECONDS);
                        for (int i = 0; i < 2; i++)
                        {
                            if (Creature* sha = me->SummonCreature(NPC_MINION_OF_DOUBTS, ShaSummonPosition[i]))
                            {
                                float x, y, z;
                                x = sha->GetPositionX();
                                y = sha->GetPositionY();
                                z = sha->GetPositionZ();
                                GetPositionWithDistInOrientation(sha, 18.0f, sha->GetOrientation(), x, y);
                                Movement::MoveSplineInit init(sha);
                                init.MoveTo(x, y, z);
                                init.SetVelocity(9.0f);
                                init.Launch();
                            }
                        }
                    }
                    else
                    {
                        nonCombatEvents.ScheduleEvent(EVENT_CLOSE_DOOR, 1 * IN_MILLISECONDS);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                Reset();
            }

            void Reset() override
            {
                figmentsCount = 0;
                figmentsDie   = 0;
                delay         = 0;
                events.Reset();
                Talk(TALK_RESET);
                me->SetReactState(REACT_DEFENSIVE);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_FIGMENT_DIE)
                {
                    ++figmentsDie;
                    if (figmentsDie == figmentsCount)
                    {
                        figmentsDie = 0;
                        figmentsCount = 0;
                        me->RemoveAura(SPELL_BOUNDS_OF_REALITY);
                    }
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_SHA_OF_DOUBT, DONE);
                }

                std::list<Creature*> figments;
                me->GetCreatureListWithEntryInGrid(figments, NPC_FIGMENT_OF_DOUBT, 100.0f);

                    for (auto&& creature : figments)
                        creature->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_AGGRO);
                events.ScheduleEvent(EVENT_WITHER_WILL, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, urand(8, 12) * IN_MILLISECONDS);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetData(DATA_SHA_OF_DOUBT, IN_PROGRESS);
                }

                delay = 0;
                me->m_Events.Schedule(delay += 3000, 20, [this]()
                {
                    if (me->IsInCombat())
                        _JustEngagedWith();
                });
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_SHA_OF_DOUBT, FAIL);
                    instance->SetBossState(DATA_SHA_OF_DOUBT, FAIL);
                }

                std::list<Creature*> figments;
                me->GetCreatureListWithEntryInGrid(figments, NPC_FIGMENT_OF_DOUBT, 100.0f);

                for (auto&& creature : figments)
                    creature->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CLOSE_DOOR:
                            if (GameObject* door = me->FindNearestGameObject(GAMEOBJECT_DOOR_SHA_OF_DOUBT, 70.0f))
                                door->SetGoState(GO_STATE_READY);
                            break;
                        case EVENT_OPEN_DOOR:
                            if (GameObject* door = me->FindNearestGameObject(GAMEOBJECT_DOOR_SHA_OF_DOUBT, 70.0f))
                                door->SetGoState(GO_STATE_ACTIVE);
                            break;
                    }
                }

                events.Update(diff);
                nonCombatEvents.Update(diff);

                if (!UpdateVictim())
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WITHER_WILL:
                            if (!me->HasAura(SPELL_BOUNDS_OF_REALITY))
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                {
                                    me->CastSpell(target, SPELL_WITHER_WILL, false);
                                    me->CastSpell(target, SPELL_WITHER_WILL, false);
                                }
                            }
                            events.ScheduleEvent(EVENT_WITHER_WILL, urand(4, 7) * IN_MILLISECONDS);
                            break;
                        case EVENT_TOUCH_OF_NOTHINGNESS:
                            if (!me->HasAura(SPELL_BOUNDS_OF_REALITY))
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_TOUCH_OF_NOTHINGNESS, false);
                            events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_BOUNDS_OF_REALITY:
                        {
                            Talk(TALK_FIGMENT);
                            me->CastSpell(me, SPELL_BOUNDS_OF_REALITY, false);

                            Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
                            if (!playerList.isEmpty())
                            {
                               figmentsCount = playerList.getSize();
                               for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                               {
                                   if (Player* plr = i->GetSource())
                                       plr->CastSpell(plr, SPELL_FIGMENT_OF_DOUBT_2, false);
                               }
                            }
                            events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, 60 * IN_MILLISECONDS);
                            break;
                        }
                    }
                }

                if (!me->HasAura(SPELL_BOUNDS_OF_REALITY))
                    DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff, 53.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sha_of_doubt_AI(creature);
        }
};

class npc_figment_of_doubt : public CreatureScript
{
    public:
        npc_figment_of_doubt() : CreatureScript("npc_figment_of_doubt") { }

        enum eEvents
        {
            EVENT_GATHERING_DOUBT      = 1,
            EVENT_SPELL_PHANTOM_STRIKE = 2,
            EVENT_SPELL_ARMOR_BUFF     = 3,
            EVENT_SPELL_FURY           = 4,
            EVENT_SANCTUARY            = 5,
            EVENT_SIPHON_ESSENCE       = 6,
            EVENT_STUN                 = 7,
            EVENT_BLADE_SONG           = 8,
            EVENT_UNTAMED_FURY         = 9,
            EVENT_GLIMPSE_OF_MADNESS   = 10
        };

        enum eActions
        {
            ACTION_FIGMENT_DIE = 0
        };

        enum eSpells
        {
            SPELL_SHADOWFORM        = 41408,
            SPELL_GROW              = 104921,
            SPELL_DROWNED_STATE     = 117509,
            SPELL_DRAW_DOUBT        = 106290,
            SPELL_KNOCK_BACK_SELF   = 117525,
            SPELL_GATHERING_DOUBT   = 117570,
            SPELL_GATHERING_DOUBT_2 = 117571,
            SPELL_WEAKENED_BLOWS    = 115798,
            SPELL_RELEASE_DOUBT     = 106112,
            SPELL_QUIET_SUICIDE     = 115372
        };

        struct npc_figment_of_doubt_AI : public ScriptedAI
        {
            npc_figment_of_doubt_AI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->CastSpell(me, SPELL_SHADOWFORM, false);
                me->CastSpell(me, SPELL_GROW, false);

                if (me->ToTempSummon())
                    me->CastSpell((Unit*)NULL, SPELL_DRAW_DOUBT, false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->CastSpell(me, SPELL_GATHERING_DOUBT, false);
                events.ScheduleEvent(EVENT_GATHERING_DOUBT, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SIPHON_ESSENCE, 8 * IN_MILLISECONDS);

                if (me->ToTempSummon())
                {
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    {
                        if (summoner->GetTypeId() == TYPEID_PLAYER)
                        {
                            switch (summoner->ToPlayer()->GetRoleForGroup(summoner->ToPlayer()->GetTalentSpecialization()))
                            {
                                case ROLES_HEALER:
                                    events.ScheduleEvent(EVENT_SPELL_PHANTOM_STRIKE, 20 * IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_STUN, 7 * IN_MILLISECONDS);
                                    break;
                                case ROLES_DPS:
                                    events.ScheduleEvent(EVENT_SPELL_FURY, 5 * IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_BLADE_SONG, 13 * IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_GLIMPSE_OF_MADNESS, 8 * IN_MILLISECONDS);
                                    break;
                                case ROLES_TANK:
                                    events.ScheduleEvent(EVENT_SPELL_ARMOR_BUFF, 10 * IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_SANCTUARY, urand(8, 12) * IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_STUN, 7 * IN_MILLISECONDS);
                                    break;
                            }
                        }
                    }
                }
            }

            void JustDied(Unit* killer) override
            {
                me->CastSpell(me, SPELL_DROWNED_STATE, false);
                me->RemoveAura(SPELL_GATHERING_DOUBT);
                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);

                if (me->ToTempSummon())
                {
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        summoner->RemoveAura(SPELL_DRAW_DOUBT);
                }

                if (Creature* sha = Unit::GetCreature(*me, instance->GetGuidData(DATA_SHA_OF_DOUBT)))
                {
                    if (sha->IsAIEnabled)
                        sha->AI()->DoAction(ACTION_FIGMENT_DIE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GATHERING_DOUBT:
                            if (me->GetAuraCount(SPELL_GATHERING_DOUBT_2) == 30)
                            {
                                me->RemoveAura(SPELL_GATHERING_DOUBT);
                                me->CastSpell(me, SPELL_RELEASE_DOUBT, false); 
                                me->CastSpell(me, SPELL_QUIET_SUICIDE, false);
                            }
                            else
                                events.ScheduleEvent(EVENT_GATHERING_DOUBT, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPELL_PHANTOM_STRIKE:
                            if (me->GetVictim())
                                me->CastSpell(me->GetVictim(), 9806, false);
                            events.ScheduleEvent(EVENT_SPELL_PHANTOM_STRIKE, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPELL_ARMOR_BUFF:
                            me->CastSpell(me, 34199, false);
                            events.ScheduleEvent(EVENT_SPELL_ARMOR_BUFF, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPELL_FURY:
                            me->CastSpell(me, 15494, false);
                            events.ScheduleEvent(EVENT_SPELL_FURY, 5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SANCTUARY:
                            me->CastSpell(me, 69207, false);
                            events.ScheduleEvent(EVENT_SANCTUARY, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_SIPHON_ESSENCE:
                            me->CastSpell(me, 40291, false);
                            events.ScheduleEvent(EVENT_SIPHON_ESSENCE, 8 * IN_MILLISECONDS);
                            break;
                        case EVENT_STUN:
                            if (me->GetVictim())
                                me->CastSpell(me->GetVictim(), 23454, false);
                            events.ScheduleEvent(EVENT_STUN, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_BLADE_SONG:
                            me->CastSpell(me, 38282, false);
                            events.ScheduleEvent(EVENT_BLADE_SONG, 13 * IN_MILLISECONDS);
                            break;
                        case EVENT_UNTAMED_FURY:
                            me->CastSpell(me, 23719, false);
                            events.ScheduleEvent(EVENT_UNTAMED_FURY, 9 * IN_MILLISECONDS);
                            break;
                        case EVENT_GLIMPSE_OF_MADNESS:
                            if (me->GetVictim())
                                me->CastSpell(me->GetVictim(), 26108, false);
                            events.ScheduleEvent(EVENT_GLIMPSE_OF_MADNESS, 8 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_figment_of_doubt_AI(creature);
        }
};

enum LinDagu
{
    TALK_LIN_AGGRO      = 0,
    TALK_LIN_SLAY       = 1,
    TALK_LIN_DEATH      = 2,
    TALK_LIN_NOODLES    = 3,
    TALK_LIN_BOILING    = 4,

    SPELL_NOODLES       = 145884,
    SPELL_DASH_OF_SPICE = 146918,
    SPELL_BOILING_BROTH = 145887,

    EVENT_NOODLES       = 1,
    EVENT_DASH_OF_SPICE = 2,
    EVENT_BOILING_BROTH = 3,
};

struct npc_ghost_of_lin_dagu : public ScriptedAI
{
    npc_ghost_of_lin_dagu(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_LIN_DEATH);
        events.ScheduleEvent(EVENT_NOODLES, urand(5, 8) * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DASH_OF_SPICE, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BOILING_BROTH, urand(2, 4) * IN_MILLISECONDS);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            Talk(TALK_LIN_SLAY);
    }

    void JustDied(Unit* killer) override
    {
        Talk(TALK_LIN_DEATH);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_NOODLES:
                    Talk(TALK_LIN_NOODLES);
                    me->CastSpell(me, SPELL_NOODLES);
                    events.ScheduleEvent(EVENT_NOODLES, urand(12, 15) * IN_MILLISECONDS);
                    break;
                case EVENT_DASH_OF_SPICE:
                    me->CastSpell(me, SPELL_DASH_OF_SPICE);
                    events.ScheduleEvent(EVENT_DASH_OF_SPICE, 22 * IN_MILLISECONDS);
                    break;
                case EVENT_BOILING_BROTH:
                    Talk(TALK_LIN_BOILING);
                    me->CastSpell(me, SPELL_BOILING_BROTH);
                    events.ScheduleEvent(EVENT_BOILING_BROTH, urand(15, 25) * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

class PlayerTargetSelector
{
    public:
        PlayerTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            if (object->GetTypeId() == TYPEID_PLAYER)
                return false;

            return true;
        }
};

class ShaTargetSelector
{
    public:
        ShaTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            if (Creature* creature = object->ToCreature())
               if (creature->GetEntry() == NPC_SHA_OF_DOUBT)
                  return false;

            return true;
        }
};

class spell_sod_release_doubt : public SpellScriptLoader
{
    public:
        spell_sod_release_doubt() : SpellScriptLoader("spell_sod_release_doubt") { }

        class spell_sod_release_doubt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sod_release_doubt_SpellScript);

            void FilterPlayerTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (PlayerTargetSelector());
            }

            void FilterSha(std::list<WorldObject*>& targets)
            {
                targets.remove_if (ShaTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sod_release_doubt_SpellScript::FilterPlayerTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sod_release_doubt_SpellScript::FilterSha, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sod_release_doubt_SpellScript();
        }
};

enum AchiementSpells
{
    SPELL_SEED_OF_DOUBT_EFF              = 123916,
    SPELL_SEED_OF_DOUBT_ACHIEVEMENT_DONE = 123944,
};

// Seed of Doubt 123916
class spell_tjs_seed_of_doubt : public AuraScript
{
    PrepareAuraScript(spell_tjs_seed_of_doubt)

    void HandleEffectApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (GetStackAmount() > 3 && !owner->HasAura(SPELL_SEED_OF_DOUBT_ACHIEVEMENT_DONE))
                owner->CastSpell(owner, SPELL_SEED_OF_DOUBT_ACHIEVEMENT_DONE, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_tjs_seed_of_doubt::HandleEffectApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK); // Indeed, it should be AfterEffectReapply
    }
};

// Noodles - 145884
class spell_noodles : public SpellScript
{
    PrepareSpellScript(spell_noodles);

    void HandleDummy()
    {
        if (Unit* caster = GetCaster())
        {
            Position pos;
            for (uint8 i = 0; i < 3; i++)
            {
                pos = caster->GetRandomNearPosition(15.0f);
                caster->CastSpell(pos, 147144);
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_noodles::HandleDummy);
    }
};

// Dash of Spice - 146918
class spell_dash_of_spice : public SpellScript
{
    PrepareSpellScript(spell_dash_of_spice);

    void HandleDummy()
    {
        if (Unit* caster = GetCaster())
        {
            Position pos;
            for (uint8 i = 0; i < 3; i++)
            {
                pos = caster->GetRandomNearPosition(25.0f);
                caster->CastSpell(pos, 147146);
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dash_of_spice::HandleDummy);
    }
};

// 1276 Summoned by 147143 - Noodles
class sat_noodles : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, 145883, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(145883);
    }
};

// 1277 Summoned by 147152 - Dsah of Spice
class sat_dsah_of_spice : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, 146919, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(146919);
    }
};

void AddSC_boss_sha_of_doubt()
{
    new boss_sha_of_doubt();
    new npc_figment_of_doubt();
    new creature_script<npc_ghost_of_lin_dagu>("npc_ghost_of_lin_dagu");
    new spell_sod_release_doubt();
    new aura_script<spell_tjs_seed_of_doubt>("spell_tjs_seed_of_doubt");
    new spell_script<spell_noodles>("spell_noodles");
    new spell_script<spell_dash_of_spice>("spell_dash_of_spice");
    new atrigger_script<sat_noodles>("sat_noodles");
    new atrigger_script<sat_dsah_of_spice>("sat_dsah_of_spice");
}
