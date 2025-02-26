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

#include "grim_batol.h"
#include "ScriptPCH.h"

enum ScriptTexts
{
    SAY_AGGRO,
    SAY_KILL,
    SAY_DEATH,
    SAY_GALE,
    SAY_ADDS,
};

enum Spells
{
    SPELL_ENFEEBLING_BLOW           = 75789,
    SPELL_BINDING_SHADOWS           = 79466,
    SPELL_BINDING_SHADOWS_AURA      = 75861,
    SPELL_SIPHON_ESSENSE            = 75755,
    SPELL_UMBRAL_MENDING            = 75763,
    SPELL_SHADOW_GALE_SPEED_TRIGGER = 75675,
    SPELL_SHADOW_GALE_SPEED         = 75694,
    SPELL_SHADOW_GALE               = 75664,
    SPELL_SHADOW_GALE_DMG           = 75692,
    SPELL_TWILIGHT_CORRUPTION       = 75520,
    SPELL_TWILIGHT_CORRUPTION_DMG   = 75566,
    SPELL_SUMMON_TWILIGHT_HATCHLING = 91058,
    SPELL_SPAWN_FACELESS            = 75704,
    SPELL_TWILIGHT_PORTAL_VISUAL    = 95716,
    SPELL_TWILIGHT_BLAST_TRIGGER    = 76192,
    SPELL_TWILIGHT_BLAST_DMG        = 76194,
    SPELL_SHIELD_OF_NIGHTMARES      = 75809,
};

enum Adds
{
    NPC_FACELESS_CORRUPTOR      = 40600,
    NPC_FACELESS_CORRUPTOR_H    = 48844,
    NPC_FACELESS_PORTAL_STALKER = 44314,
    NPC_ALEXSTRASZA_EGG         = 40486,
    NPC_TWILIGHT_HATCHLING      = 39388,
    NPC_SHADOW_GALE_STALKER     = 40567,
    NPC_SHADOW_GALE_CONTROLLER  = 40566,
};

enum Events
{
    EVENT_ENFEEBLING_BLOW = 1,
    EVENT_BINDING_SHADOWS,
    EVENT_ADDS,
    EVENT_SIPHON_ESSENSE,
    EVENT_UMBRAL_MENDING,
    EVENT_REMOVE_TWILIGHT_PORTAL,
    EVENT_SHADOW_GALE,
    EVENT_TWILIGHT_CORRUPTION,
    EVENT_CORRUPTOR_MOVE,
    EVENT_SHIELD_OF_NIGHTMARES,
};

enum Points
{
    POINT_SHADOWGALE = 1001,
    POINT_EGG        = 1002,

    // Misc
    ACTION_INIT_COMBAT,
};

class boss_erudax : public CreatureScript
{
    public:
        boss_erudax() : CreatureScript("boss_erudax") { }

        struct boss_erudaxAI : public BossAI
        {
            boss_erudaxAI(Creature* creature) : BossAI(creature, DATA_ERUDAX)
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
                me->setActive(true);
            }

            ObjectGuid FacelessPortalStalkerGUID;
            ObjectGuid ShadowGaleTriggerGUID;
            ObjectGuid targetGUID;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
                targetGUID = ObjectGuid::Empty;
                ShadowGaleTriggerGUID = ObjectGuid::Empty;
                FacelessPortalStalkerGUID = ObjectGuid::Empty;

                for (uint8 i = 0; i < 13; i++)
                    me->SummonCreature(NPC_ALEXSTRASZA_EGG, eggPos[i]);

                me->GetMap()->SetWorldState(WORLDSTATE_BREAK_THE_EGGS, 1);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_FACELESS_CORRUPTOR ||
                    summon->GetEntry() == NPC_FACELESS_CORRUPTOR_H)
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        summon->AI()->AttackStart(target);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(SAY_AGGRO);

                if (Creature* facelessPortalStalker = me->SummonCreature(NPC_FACELESS_PORTAL_STALKER, erudaxportalPos, TEMPSUMMON_MANUAL_DESPAWN))
                    FacelessPortalStalkerGUID = facelessPortalStalker->GetGUID();

                events.ScheduleEvent(EVENT_SHADOW_GALE, urand(25000, 26000));
                events.ScheduleEvent(EVENT_ENFEEBLING_BLOW, urand(4000, 6000));
                events.ScheduleEvent(EVENT_BINDING_SHADOWS, urand(9000, 11000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
                summons.DespawnAll();
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_GALE:
                            Talk(SAY_GALE);
                            if (Creature* shadowGaleTrigger = me->SummonCreature(NPC_SHADOW_GALE_STALKER, shadowgalePos[urand(0, 2)]))
                            {
                                ShadowGaleTriggerGUID = shadowGaleTrigger->GetGUID();
                                shadowGaleTrigger->CastSpell(shadowGaleTrigger, SPELL_SHADOW_GALE_SPEED_TRIGGER, false);
                            }

                            DoCast(me, SPELL_SHADOW_GALE);
                            events.ScheduleEvent(EVENT_ADDS, 16500);
                            break;
                        case EVENT_REMOVE_TWILIGHT_PORTAL:
                            if (Creature* facelessPortalStalker = ObjectAccessor::GetCreature(*me, FacelessPortalStalkerGUID))
                                facelessPortalStalker->RemoveAurasDueToSpell(SPELL_TWILIGHT_PORTAL_VISUAL);
                            break;
                        case EVENT_ADDS:
                            if (Creature* shadowGaleTrigger = ObjectAccessor::GetCreature(*me, ShadowGaleTriggerGUID))
                                shadowGaleTrigger->DespawnOrUnsummon();

                            Talk(SAY_ADDS);
                            if (Creature* facelessPortalStalker = ObjectAccessor::GetCreature(*me, FacelessPortalStalkerGUID))
                            {
                                facelessPortalStalker->CastSpell(facelessPortalStalker, SPELL_TWILIGHT_PORTAL_VISUAL, true);
                                DoCast(facelessPortalStalker, SPELL_SPAWN_FACELESS, true);
                            }

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SHIELD_OF_NIGHTMARES, urand(4000, 7000));

                            events.ScheduleEvent(EVENT_REMOVE_TWILIGHT_PORTAL, 7000);
                            events.ScheduleEvent(EVENT_SHADOW_GALE, urand(40000, 44000));
                            break;
                        case EVENT_ENFEEBLING_BLOW:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_ENFEEBLING_BLOW);

                            events.ScheduleEvent(EVENT_ENFEEBLING_BLOW, urand(20000, 30000));
                            break;
                        case EVENT_BINDING_SHADOWS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                if (Unit* target = me->GetVictim())
                                    targetGUID = target->GetGUID();

                                me->PrepareChanneledCast(me->GetOrientation());

                                ObjectGuid bindingGUID = target->GetGUID();
                                scheduler
                                    .Schedule(Milliseconds(500), [this, bindingGUID](TaskContext /*context*/)
                                {
                                    if (Unit* tempTarget = ObjectAccessor::GetUnit(*me, bindingGUID))
                                        DoCast(tempTarget, SPELL_BINDING_SHADOWS);
                                });

                                scheduler
                                    .Schedule(Milliseconds(2500), [this](TaskContext /*context*/)
                                {
                                    me->RemoveChanneledCast(targetGUID);
                                });
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                if (Unit* target = me->GetVictim())
                                    targetGUID = target->GetGUID();

                                me->PrepareChanneledCast(me->GetOrientation());

                                ObjectGuid bindingGUID = target->GetGUID();
                                scheduler
                                    .Schedule(Milliseconds(500), [this, bindingGUID](TaskContext /*context*/)
                                {
                                    if (Unit* tempTarget = ObjectAccessor::GetUnit(*me, bindingGUID))
                                        DoCast(tempTarget, SPELL_BINDING_SHADOWS);
                                });

                                scheduler
                                    .Schedule(Milliseconds(2500), [this](TaskContext /*context*/)
                                {
                                    me->RemoveChanneledCast(targetGUID);
                                });
                            }

                            events.ScheduleEvent(EVENT_BINDING_SHADOWS, urand(20000, 30000));
                            break;
                        case EVENT_SHIELD_OF_NIGHTMARES:
                            DoCast(me, SPELL_SHIELD_OF_NIGHTMARES);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 100.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_erudaxAI>(creature);
        }
};

// Faceless Corruptor 40600, 48844
struct npc_erudax_faceless_corruptor : public ScriptedAI
{
    npc_erudax_faceless_corruptor(Creature* creature) : ScriptedAI(creature)
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
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;
    TaskScheduler scheduler;
    EventMap events;

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        me->SetInCombatWithZone();
        me->SetReactState(REACT_PASSIVE);
        me->AttackStop();

        me->GetMotionMaster()->MovePoint(0, eggsSpawnPath[me->GetEntry() == NPC_FACELESS_CORRUPTOR_H ? 0 : 1]);

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext /*context*/)
        {
            me->GetMap()->SetWorldState(WORLDSTATE_BREAK_THE_EGGS, 0);
            DoCast(me, SPELL_TWILIGHT_CORRUPTION);
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INIT_COMBAT)
        {
            me->RemoveChanneledCast();
            me->SetInCombatWithZone();

            events.ScheduleEvent(EVENT_UMBRAL_MENDING, urand(15000, 20000));
            events.ScheduleEvent(EVENT_SIPHON_ESSENSE, urand(5000, 7000));
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon(1000);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!instance || !UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_UMBRAL_MENDING:
                    if (Creature* erudax = Unit::GetCreature(*me, instance->GetGuidData(DATA_ERUDAX)))
                        DoCast(erudax, SPELL_UMBRAL_MENDING);

                    events.ScheduleEvent(EVENT_UMBRAL_MENDING, urand(15000, 20000));
                    break;
                case EVENT_SIPHON_ESSENSE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        DoCast(target, SPELL_SIPHON_ESSENSE);

                    events.ScheduleEvent(EVENT_SIPHON_ESSENSE, urand(7000, 10000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Alexstraza`s Egg 40486
struct npc_alexstrasza_egg : public ScriptedAI
{
    npc_alexstrasza_egg(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        instance = creature->GetInstanceScript();
        SetCombatMovement(false);
    }

    void Reset() override
    {
        me->setRegeneratingHealth(false);
    }

    InstanceScript* instance;

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_SUMMON_TWILIGHT_HATCHLING, true);
    }

    void JustSummoned(Creature* summon) override
    {
        if (!instance)
            return;

        if (summon->GetEntry() == NPC_TWILIGHT_HATCHLING)
        {
            if (Creature* _erudax = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ERUDAX)))
            {
                if (Unit* target = _erudax->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                {
                    summon->AI()->AttackStart(target);
                    summon->GetMotionMaster()->MoveChase(target);
                }
            }
        }
    }
};

// Shadow Gale Stalker 44314
struct npc_shadow_gale_stalker : public ScriptedAI
{
    npc_shadow_gale_stalker(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void Reset() override { }
};

// Twilight Hatchling 39388
struct npc_erudax_twilight_hatchling : public ScriptedAI
{
    npc_erudax_twilight_hatchling(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    void Reset() override
    {
        me->SetCanFly(true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon();
    }

    void IsSummonedBy(Unit* /*owner*/) override
    {
        DoCast(me, SPELL_TWILIGHT_BLAST_TRIGGER);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!instance)
            return;

        if (instance->GetBossState(DATA_ERUDAX) != IN_PROGRESS)
            me->DespawnOrUnsummon();
    }
};

// Shadow Gale 75692
class spell_erudax_shadow_gale : public SpellScript
{
    PrepareSpellScript(spell_erudax_shadow_gale);

    void HandleScript()
    {
        if (Unit* target = GetHitUnit())
            if (target->FindNearestCreature(NPC_SHADOW_GALE_STALKER, 5.0f))
                SetHitDamage(0);
    }

    void Register() override
    {
        BeforeHit += SpellHitFn(spell_erudax_shadow_gale::HandleScript);
    }
};

// Shadow Gale Launcher 75664
class spell_erudax_shadow_gale_launcher : public SpellScript
{
    PrepareSpellScript(spell_erudax_shadow_gale_launcher);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHADOW_GALE_CONTROLLER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_erudax_shadow_gale_launcher::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Shield of Nightmares 75809
class spell_erudax_shield_of_nightmares : public SpellScript
{
    PrepareSpellScript(spell_erudax_shield_of_nightmares);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_FACELESS_CORRUPTOR; });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_erudax_shield_of_nightmares::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class twilightCorruptionOrderPred
{
    public:
        twilightCorruptionOrderPred(const WorldObject* object, bool ascending = true) : m_object(object), m_ascending(ascending) {}
        bool operator() (const WorldObject* a, const WorldObject* b) const
        {
            return m_ascending ? a->GetDistance(m_object) < b->GetDistance(m_object) :
                a->GetDistance(m_object) > b->GetDistance(m_object);
        }
    private:
        const WorldObject* m_object;
        const bool m_ascending;
};

// Twilight Corruption 75520
class spell_erudax_twilight_corruption : public SpellScript
{
    PrepareSpellScript(spell_erudax_twilight_corruption);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ALEXSTRASZA_EGG; });

        if (targets.empty())
            return;

        targets.sort(twilightCorruptionOrderPred(GetCaster(), true));

        if (targets.size() > 1)
            targets.resize(1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_erudax_twilight_corruption::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Twilight Corruption 75520
class spell_erudax_twilight_corruption_aura : public AuraScript
{
    PrepareAuraScript(spell_erudax_twilight_corruption_aura);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_INIT_COMBAT);

        if (aurEff->GetBase()->GetDuration() > 0) // not summon if cast was interrupted by death
            return;

        if (Creature* owner = GetOwner()->ToCreature())
            owner->Kill(owner);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_erudax_twilight_corruption_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Umbral Mending 75763
class spell_erudax_umbral_mending : public SpellScript
{
    PrepareSpellScript(spell_erudax_umbral_mending);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ERUDAX; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_erudax_umbral_mending::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

void AddSC_boss_erudax()
{
    new boss_erudax();
    new creature_script<npc_erudax_faceless_corruptor>("npc_erudax_faceless_corruptor");
    new creature_script<npc_alexstrasza_egg>("npc_alexstrasza_egg");
    new creature_script<npc_shadow_gale_stalker>("npc_shadow_gale_stalker");
    new creature_script<npc_erudax_twilight_hatchling>("npc_erudax_twilight_hatchling");

    new spell_script<spell_erudax_shadow_gale>("spell_erudax_shadow_gale");
    new spell_script<spell_erudax_shadow_gale_launcher>("spell_erudax_shadow_gale_launcher");
    new spell_script<spell_erudax_shield_of_nightmares>("spell_erudax_shield_of_nightmares");
    new spell_script<spell_erudax_twilight_corruption>("spell_erudax_twilight_corruption");
    new aura_script<spell_erudax_twilight_corruption_aura>("spell_erudax_twilight_corruption_aura");
    new spell_script<spell_erudax_umbral_mending>("spell_erudax_umbral_mending");
}