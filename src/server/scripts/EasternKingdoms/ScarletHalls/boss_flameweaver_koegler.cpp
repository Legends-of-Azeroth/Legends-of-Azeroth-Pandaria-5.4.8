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
#include "scarlet_halls.h"

enum Spells
{
    SPELL_BOOK_BURNER                = 113364,
    SPELL_BOOK_BURNER_COSMETIC       = 113366,
    SPELL_BURNING_BOOKS              = 113616, // aura on trigger
    SPELL_GREATER_DRAGONS_BREATH     = 113641,
    SPELL_QUICKENED_MIND             = 113682, // instant 3 casts
    SPELL_PYROBLAST                  = 113690,
    SPELL_FIREBALL_VALLEY            = 113691,
    SPELL_TELEPORT                   = 113626, // to center room
    SPELL_COSMETIC_FLAME             = 126642, // at intro
};

enum Events
{
    EVENT_BOOK_BURNER                = 1,
    EVENT_QUICKENED_MIND             = 2,
    EVENT_DRAGON_BREATH              = 3,
    EVENT_FIREBALL_VALLEY            = 4,
    EVENT_GREATER_DRAGONS_BREATH_END = 5,
    EVENT_PYROBLAST                  = 6,
    EVENT_TELEPORT                   = 7,
    EVENT_DRAGON_BREATH_INTRO        = 8,
};

enum Yells
{
    TALK_INTRO                       = 0,
    TALK_AGGRO                       = 1,
    TALK_DRAGON_BREATH               = 2,
    TALK_BOOK_BURNER_ANN             = 3,
    TALK_DRAGON_BREATH_ANN           = 4,
    TALK_DEATH                       = 5
};

// make mercesed spawn
Position CalculateMovement(Creature* me, float m_ori, ObjectGuid casterGUID, float m_dist = 8.0f)
{
    Unit* caster = ObjectAccessor::GetUnit(*me, casterGUID);

    if (!caster)
        return{ 0.0f, 0.0f, 0.0f, 0.0f };

    float x, y;
    GetPositionWithDistInOrientation(caster, m_dist, m_ori, x, y);

    Position pos = { x, y, caster->GetPositionZ(), 0.0f };

    // At spawn spirit should look at Liuyang
    return{ x, y, caster->GetPositionZ(), pos.GetAngle(caster) };
}

// current range to Koegler
float GetFocusDistance(Creature* me, ObjectGuid casterGUID, ObjectGuid LiuyangGUID)
{
    if (Unit* m_caster = ObjectAccessor::GetUnit(*me, casterGUID))
        if (Unit* Liuyang = ObjectAccessor::GetUnit(*me, LiuyangGUID))
            return m_caster->GetExactDist2d(Liuyang);

    return 0.0f;
}

class boss_flameweaver_koegler : public CreatureScript
{
    public:
        boss_flameweaver_koegler() : CreatureScript("boss_flameweaver_koegler") { }

        struct boss_flameweaver_koeglerAI : public BossAI
        {
            boss_flameweaver_koeglerAI(Creature* creature) : BossAI(creature, BOSS_FLAMEWEAVER_KOEGLER) { }

            EventMap nonCombatEvents;
            bool InDragon, Intro;
            ObjectGuid victimGUID;

            void Reset() override
            {
                _Reset();
                events.Reset();
                InDragon = false;
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);
                nonCombatEvents.ScheduleEvent(EVENT_DRAGON_BREATH_INTRO, urand(1000, 2000));

                if (instance)
                    instance->SetData(BOSS_FLAMEWEAVER_KOEGLER, NOT_STARTED);
            }

            void InitializeAI() override
            {
                Intro = false;
                Reset();

                if (instance)
                    if (Creature* Crusader = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HOODED_CRUSADER)))
                        Crusader->SetVisible(false);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!Intro && who && who->GetTypeId() == TYPEID_PLAYER)
                {
                    Intro = true;
                    Talk(TALK_INTRO);
                }

                ScriptedAI::MoveInLineOfSight(who);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->SetData(BOSS_FLAMEWEAVER_KOEGLER, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                if (instance)
                {
                    if (Creature* Crusader = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_HOODED_CRUSADER)))
                    {
                        Crusader->SetVisible(true);
                        Crusader->AI()->DoAction(ACTION_KODEX);
                    }
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _DespawnAtEvade();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(BOSS_FLAMEWEAVER_KOEGLER, FAIL);
                }
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_AGGRO);

                if (instance)
                {
                    instance->SetData(BOSS_FLAMEWEAVER_KOEGLER, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                nonCombatEvents.CancelEvent(EVENT_DRAGON_BREATH_INTRO);
                me->RemoveAllAuras();
                GetEvents();
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);
                events.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_DRAGON_BREATH_INTRO)
                    {
                        me->CastSpell(me, SPELL_COSMETIC_FLAME, false);

                        nonCombatEvents.ScheduleEvent(EVENT_DRAGON_BREATH_INTRO, 4000);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BOOK_BURNER:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            me->InterruptNonMeleeSpells(true);
                            Talk(TALK_BOOK_BURNER_ANN);
                            DoCast(me, SPELL_BOOK_BURNER_COSMETIC);

                            if (Creature* BookCase = SelectedBurningCupboard(me->GetGUID()))
                            {
                                me->SetFacingTo(me->GetAngle(BookCase));
                                me->CastSpell(BookCase, SPELL_BOOK_BURNER, false);
                            }

                            events.ScheduleEvent(EVENT_BOOK_BURNER, 15000);
                            break;
                        case EVENT_QUICKENED_MIND:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            me->CastSpell(me, SPELL_QUICKENED_MIND, false);
                            break;
                        case EVENT_FIREBALL_VALLEY:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            {
                                if (Aura* QuckMind = me->GetAura(SPELL_QUICKENED_MIND))
                                {
                                    // Remove the stacks.
                                    if (QuckMind->GetStackAmount() > 1)
                                        QuckMind->SetStackAmount(QuckMind->GetStackAmount() - 1);
                                    else
                                        me->RemoveAurasDueToSpell(SPELL_QUICKENED_MIND);

                                    me->CastSpell(target, SPELL_FIREBALL_VALLEY, true);
                                }
                                else
                                    me->CastSpell(target, SPELL_FIREBALL_VALLEY, false);
                            }

                            events.ScheduleEvent(EVENT_FIREBALL_VALLEY, 13500);
                            break;
                        case EVENT_PYROBLAST:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = me->GetVictim())
                            {
                                if (Aura* QuckMind = me->GetAura(SPELL_QUICKENED_MIND))
                                {
                                    // Remove the stacks.
                                    if (QuckMind->GetStackAmount() > 1)
                                        QuckMind->SetStackAmount(QuckMind->GetStackAmount() - 1);
                                    else
                                        me->RemoveAurasDueToSpell(SPELL_QUICKENED_MIND);

                                    me->CastSpell(target, SPELL_PYROBLAST, true);
                                }
                                else
                                    me->CastSpell(target, SPELL_PYROBLAST, false);
                            }

                            events.ScheduleEvent(EVENT_PYROBLAST, urand(4000, 8000));
                            break;
                        case EVENT_GREATER_DRAGONS_BREATH_END:
                            events.Reset();
                            InDragon = false;
                            GetEvents();
                            me->RemoveChanneledCast(victimGUID);
                            break;
                        case EVENT_TELEPORT:
                            Talk(TALK_DRAGON_BREATH);
                            Talk(TALK_DRAGON_BREATH_ANN);
                            me->CastSpell(me, SPELL_TELEPORT, false);
                            me->NearTeleportTo(TeleportPoint.GetPositionX(), TeleportPoint.GetPositionY(), TeleportPoint.GetPositionZ(), TeleportPoint.GetOrientation());
                            events.Reset();
                            if (Unit* vict = me->GetVictim())
                            {
                                victimGUID = vict->GetGUID();
                                me->PrepareChanneledCast(TeleportPoint.GetOrientation());
                            }
                            events.ScheduleEvent(EVENT_DRAGON_BREATH, 1500);
                            events.ScheduleEvent(EVENT_GREATER_DRAGONS_BREATH_END, 13500);
                            break;
                        case EVENT_DRAGON_BREATH:
                            DoCast(me, SPELL_GREATER_DRAGONS_BREATH);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 35.0f);
            }

            private:
                void GetEvents()
                {
                    events.ScheduleEvent(EVENT_BOOK_BURNER, 7000);
                    events.ScheduleEvent(EVENT_TELEPORT, 30000);
                    events.ScheduleEvent(EVENT_QUICKENED_MIND, 10000);
                    events.ScheduleEvent(EVENT_FIREBALL_VALLEY, 13500);
                    events.ScheduleEvent(EVENT_PYROBLAST, urand(4000, 8000));
                }

                Creature* SelectedBurningCupboard(ObjectGuid ownerGUID)
                {
                    Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID);

                    if (!owner)
                        return NULL;

                    std::list<Creature*> BurningCupboards;
                    GetCreatureListWithEntryInGrid(BurningCupboards, owner, NPC_BOOK_CASE, 200.0f);

                    BurningCupboards.remove_if([](WorldObject* target) -> bool
                    {
                        if (Creature* creature = target->ToCreature())
                            return !creature->HasAura(SPELL_BOOK_BURNER_COSMETIC);

                        return true;
                    });

                    if (!BurningCupboards.empty())
                        return Trinity::Containers::SelectRandomContainerElement(BurningCupboards);

                    return NULL;
                }

                bool HandleRescheduleEventsIfCastAny(uint32 eventId)
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        events.RescheduleEvent(eventId, urand(0.2 * IN_MILLISECONDS, 0.45 * IN_MILLISECONDS));
                        return true;
                    }

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_flameweaver_koeglerAI(creature);
        }
};

// Book burner 113364
class spell_koegler_book_burner : public SpellScriptLoader
{
    public:
        spell_koegler_book_burner() : SpellScriptLoader("spell_koegler_book_burner") { }

        class spell_koegler_book_burner_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_koegler_book_burner_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* target = GetHitUnit();
                Unit* caster = GetCaster();

                if (!target || !caster)
                    return;

                if (target->GetTypeId() == TYPEID_PLAYER)
                    return;

                if (!target->HasAura(SPELL_BURNING_BOOKS))
                    target->CastSpell(target, SPELL_BURNING_BOOKS, true);

                SetHitDamage(0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_koegler_book_burner_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_koegler_book_burner_SpellScript();
        }
};

// Dragon Breath Targeting 59198
class npc_dragon_breath_targeting : public CreatureScript
{
    public:
        npc_dragon_breath_targeting() : CreatureScript("npc_dragon_breath_targeting") { }

        enum eEvents : uint32
        {
            EVENT_NONE,
            EVENT_MOVE
        };

        struct npc_dragon_breath_targetingAI : public ScriptedAI
        {
            npc_dragon_breath_targetingAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                events.ScheduleEvent(EVENT_MOVE, 200);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    events.ScheduleEvent(EVENT_MOVE, 200);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_MOVE)
                    {
                        if (instance)
                        {
                            if (Unit* Koegler = ObjectAccessor::GetUnit(*me, instance->GetGuidData(BOSS_FLAMEWEAVER_KOEGLER)))
                            {
                                me->GetMotionMaster()->MovePoint(0, CalculateMovement(me, Position::NormalizeOrientation(Koegler->GetAngle(me) + M_PI / 6), Koegler->GetGUID(), GetFocusDistance(me, me->GetGUID(), Koegler->GetGUID())).GetPositionX(), CalculateMovement(me, Position::NormalizeOrientation(Koegler->GetAngle(me) + M_PI / 6), Koegler->GetGUID(), GetFocusDistance(me, me->GetGUID(), Koegler->GetGUID())).GetPositionY(), CalculateMovement(me, Position::NormalizeOrientation(Koegler->GetAngle(me) + M_PI / 6), Koegler->GetGUID(), GetFocusDistance(me, me->GetGUID(), Koegler->GetGUID())).GetPositionZ());
                                Koegler->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
                                Koegler->SetTarget(me->GetGUID());
                            }
                        }
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dragon_breath_targetingAI(creature);
        }
};

// Dragon Breath 113641
class spell_dragon_breath_sc : public SpellScriptLoader
{
    public:
        spell_dragon_breath_sc() : SpellScriptLoader("spell_dragon_breath_sc") { }

        class spell_dragon_breath_sc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dragon_breath_sc_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    caster->SummonCreature(NPC_DRAGON_BREATH_TARGET, DragonBreathMover, TEMPSUMMON_TIMED_DESPAWN, Milliseconds(10 * IN_MILLISECONDS));
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dragon_breath_sc_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dragon_breath_sc_SpellScript();
        }
};

void AddSC_boss_flameweaver_koegler()
{
    new boss_flameweaver_koegler();
    new npc_dragon_breath_targeting();
    new spell_koegler_book_burner();
    new spell_dragon_breath_sc();
}
