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

#include "throne_of_thunder.h"
#include "GridNotifiers.h"
#include "Vehicle.h"
#include "Random.h"

enum eSpells
{
    SPELL_CONDUCTIVE_WATER_SUMMON           = 137145,
    SPELL_CONDUCTIVE_WATERFALL              = 137340,
    SPELL_CONDUCTIVE_WATER_VISUAL           = 137277,
    SPELL_CONDUCTIVE_WATER_DUMMY            = 137168,
    SPELL_ELECTRIFY_WATERS                  = 138568,
    SPELL_ELECTRIFIED_WATER_VISUAL          = 137978,

    SPELL_LIGHTNING_FISSURE_SUMMON          = 137479,
    SPELL_LIGHTNING_FISSURE_VISUAL          = 137480,
    SPELL_LIGHTNING_FISSURE_CONDUCTION      = 138133,
    SPELL_LIGHTNING_FISSURE_AURA            = 137484,
    SPELL_LIGHTNING_FISSURE_EFF             = 137485,

    SPELL_FOCUSED_LIGHTNING                 = 137399,
    //SPELL_FOCUSED_LIGHTNING_VISUAL          = 137425,
    //SPELL_FOCUSED_LIGHTNING_FIXATE          = 137422,
    SPELL_FOCUSED_LIGHTNING_SPEED           = 137389,
    SPELL_FOCUSED_LIGHTNING_AOE             = 137429,
    SPELL_FOCUSED_LIGHTNING_DETONATION      = 137374,
    SPELL_VIOLENT_LIGHTNING_DETONATION      = 138990,
    SPELL_FOCUSED_LIGHTNING_DAMAGE          = 137423,
    SPELL_FOCUSED_LIGHTNING_TARGET          = 137194,
    SPELL_FOCUSED_LIGHTNING_CONDUCTION      = 137530,

    SPELL_IMPLOSION                         = 137507,

    SPELL_CONDUCTIVE_WATER_GROW             = 137676,
    SPELL_CONDUCTIVE_WATER_GROW_EFF         = 137694,
    SPELL_CONDUCTIVE_WATERS                 = 138470,
    SPELL_ELECTRIFIED_WATERS                = 138006,
    SPELL_FLUIDITY                          = 138002,

    SPELL_STATIC_BURST                      = 137162,
    SPELL_STATIC_WOUND                      = 138349,
    SPELL_STATIC_WOUND_DAMAGE               = 138389,

    SPELL_LIGHTNING_STORM                   = 137313,
    SPELL_LIGHTNING_STORM_VISUAL            = 138568,
    SPELL_LIGHTNING_STORM_SUMM              = 140774, // summ creature for lightning strike and(or) visual storm?

    // Thundering Throw
    SPELL_THUNDERING_THROW_JUMP             = 137173, // Casted by player on a statue
    SPELL_THUNDERING_THROW_SILENCE          = 137161, // Silence, visuals
    SPELL_THUNDERING_THROW_FLY_VISUAL       = 140594, // Visual in flight
    SPELL_THUNDERING_THROW_HIT_DAMAGE       = 137370, // Damage on hit statue
    SPELL_THUNDERING_THROW_HIT_AOE_DAMAGE   = 137167, // AOE Damage on hit statue
    SPELL_THUNDERING_THROW_STUN             = 137371, // Stun after aoe damage on hit statue
    SPELL_THUNDERING_THROW_IMPACT_VISUAL    = 140606, // Visual of the impact on ground
    SPELL_THUNTERING_THROW_FORCED           = 137175, // Pre Event Throw
    SPELL_THUNDERING_THROW_FORCED_PLAYER    = 137180, // Need SpellScript to handle ScriptEffect
    SPELL_THUNDERING_THROW_VEHICLE          = 43671,

    // Heroic
    SPELL_IONIZATION                        = 138732,
    SPELL_IONIZATION_PROC                   = 138733,
    SPELL_IONIZATION_DAMAGES_CONDUCTION     = 138743,
    SPELL_LIGHTNING_DEFFUSION               = 137905, // proc dmg on target
    SPELL_LIGHTNING_STRIKE_VISUAL           = 138299, // Visual (blue column), trigger 137647 on hit
    SPELL_LIGHTNING_STRIKE_EXPLOSION        = 137647, // AOE Damages
    SPELL_LIGHTNING_STRIKE_LIGHTNING_CHARGE = 137966, // Visual charge
    SPELL_LIGHTNING_STRIKE_AREATRIGGER      = 139982, // Dynamic areatrigger, now doesn`t work

    // Misc
    SPELL_LIGHTNING_OVERLOAD_ACHIEV         = 139163,
};

enum eCreatures
{
    NPC_LIGHTNING_FISSURE               = 69609,
    NPC_CONDUCTIVE_WATER                = 69469,
    NPC_FOCUSED_LIGHTNING               = 69593
};

enum iActions
{
    ACTION_NONE,
    ACTION_DESTROYED,
    ACTION_RESET,
    ACTION_ELECTRIFY
};

static const float floorZ = 124.03f;

enum eJDatas
{
    DATA_STATUE_DESTROYED,
};

class conductionPredicate
{
    public:
        conductionPredicate(Creature* _waters) : waters(_waters) { }

        bool operator()(WorldObject* target) const
        {
            if (target && target->ToUnit())
            {
                if (target->ToUnit()->HasAura(SPELL_FLUIDITY, waters->GetGUID()))
                    return false;
                if (target->ToUnit()->HasAura(SPELL_ELECTRIFIED_WATERS, waters->GetGUID()))
                    return false;
            }

            return true;
        }
    private:
        Creature* waters;
};

class focusedLightningPredicate
{
    public:
        bool operator()(WorldObject* target) const
        {
            if (target)
            {
                if (target->ToCreature())
                {
                    if (target->ToCreature()->GetEntry() != NPC_LIGHTNING_FISSURE && target->ToCreature()->GetEntry() != NPC_FOCUSED_LIGHTNING)
                        return true;
                    else
                        return false;
                }

                if (target->GetTypeId() == TYPEID_PLAYER)
                    return false;

                return true;
            }
            return true;
        }
};

static const Position aWaterPos[4] = 
{
    { 5864.490f, 6290.628f, 124.03f, 5.51f },
    { 5917.633f, 6289.476f, 124.03f, 3.93f },
    { 5918.487f, 6236.848f, 124.03f, 2.36f },
    { 5865.241f, 6236.743f, 128.03f, 0.77f }
};

static const Position aCenterPos = { 5892.16f, 6263.58f, 124.7f, 0.0f };

class boss_jinrokh : public CreatureScript
{
    enum eEvents
    {
        EVENT_NONE,
        EVENT_STATIC_BURST,
        EVENT_FOCUSED_LIGHTNING,
        EVENT_THUNDERING_THROW,
        EVENT_LIGHTNING_STORM,
        EVENT_IONIZATION,
        EVENT_BERSERK,
        EVENT_HEIGHT_CHECK,
        EVENT_PROPAGATE_STORM,
        EVENT_INTRO_YELL,
    };

    enum eTalks
    {
        TALK_INTRO,
        TALK_AGGRO,
        TALK_STATIC_BURST,
        TALK_THUNDERING_THROW,
        TALK_LIGHTNING_STORM,
        TALK_FOCUSED_LIGHTNING,
        EMOTE_THUNDERING_THROW,
        EMOTE_LIGHTNING_STORM,
        EMOTE_LIGHTNING_STORM_2,
        TALK_SLAY,
        TALK_BERSERK,
        TALK_DEATH
    };

    public:
        boss_jinrokh() : CreatureScript("boss_jinrokh") { }

        struct boss_jinrokhAI : public BossAI
        {
            boss_jinrokhAI(Creature* creature) : BossAI(creature, DATA_JINROKH) 
            {
                if (instance && instance->GetData(DATA_JINROKH_EVENT) != DONE)
                    if (Creature* stormCaller = me->SummonCreature(NPC_ZANDALARI_STORM_CALLER, stormCallerSumPos, TEMPSUMMON_MANUAL_DESPAWN))
                        stormCaller->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
            }

            uint32 m_uiPushTimer;
            ObjectGuid targetGUID;
            bool inStorm;

            void Reset() override
            {
                _Reset();
                ResetStatues();
                targetGUID = ObjectGuid::Empty;
                me->SetReactState(REACT_AGGRESSIVE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                inStorm = false;

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_STATIC_BURST, 13 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FOCUSED_LIGHTNING, 7500);
                events.ScheduleEvent(EVENT_THUNDERING_THROW, 30000);
                events.ScheduleEvent(EVENT_LIGHTNING_STORM, 90000); // 1,5 minutes
                events.ScheduleEvent(EVENT_BERSERK, 6 * MINUTE*IN_MILLISECONDS + 5000);
                events.ScheduleEvent(EVENT_HEIGHT_CHECK, 2000);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_IONIZATION, 60 * IN_MILLISECONDS);

                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(TALK_AGGRO);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_JINROKH, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIGHTNING_FISSURE_AURA);
                    summons.DespawnAll();
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
            {
                // Hanle set facing to victim (issue with high hitbox encounters)
                if (victim && damageType == DIRECT_DAMAGE)
                    me->SetFacingTo(me->GetAngle(victim));
            }

            void CheckHeight()
            {
                if (me->GetPositionZ() > floorZ + 3.5f || me->GetPositionZ() < floorZ - 1.2f)
                    me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), floorZ + 0.2f, me->GetOrientation());

                events.ScheduleEvent(EVENT_HEIGHT_CHECK, 2000);
            }

            void DoCastBossSpell(Unit* target, uint32 spellId, bool trig, uint32 push = 0)
            {
                DoCast(target, spellId, trig);

                if (push)
                    m_uiPushTimer = push;
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIGHTNING_FISSURE_AURA);
                }

                ResetStatues();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_START_INTRO)
                {
                    if (instance && instance->GetData(DATA_JINROKH_EVENT) == DONE)
                        return;

                    instance->SetData(DATA_JINROKH_EVENT, DONE);

                    if (Aura* pVisual = me->AddAura(SPELL_LIGHTNING_STORM_VISUAL, me))
                        pVisual->SetDuration(8000);

                    me->HandleEmoteStateCommand(EMOTE_STATE_ATTACK1H);

                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += 5000, 20, [this]()
                    {
                        me->HandleEmoteStateCommand(EMOTE_STATE_NONE);

                        // Prevent Event if stormcaller was killed
                        if (Creature* stormCaller = me->FindNearestCreature(NPC_ZANDALARI_STORM_CALLER, 20.0f, true))
                        {
                            Talk(TALK_INTRO);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);

                            stormCaller->CastSpell(me, SPELL_THUNDERING_THROW_VEHICLE, true);
                            DoCast(me, SPELL_THUNTERING_THROW_FORCED);
                        }
                    });
                }
            }

            void ResetStatues()
            {
                std::list<Creature*> pStatues;
                GetCreatureListWithEntryInGrid(pStatues, me, NPC_JINROKH_STATUE, 200.f);

                for (auto&& itr : pStatues)
                {
                    if (itr->AI())
                        itr->AI()->DoAction(ACTION_RESET);
                }
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (target && spell->Id == SPELL_THUNDERING_THROW_FORCED_PLAYER)
                {
                    if (me->GetThreatManager().getThreat(target))
                        me->GetThreatManager().modifyThreatPercent(target, -100);
                }
            }

            void DoHandleLightningStorm()
            {
                std::list<Creature*> pWaters;
                GetCreatureListWithEntryInGrid(pWaters, me, NPC_CONDUCTIVE_WATER, 200.f);

                pWaters.remove_if([=](Creature* target) { return target && target->HasAura(SPELL_ELECTRIFIED_WATER_VISUAL); });

                for (auto&& itr : pWaters)
                {
                    if (itr->AI())
                        itr->AI()->DoAction(ACTION_ELECTRIFY);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == 1948)
                {
                    DoHandleLightningStorm();
                    events.ScheduleEvent(EVENT_PROPAGATE_STORM, 300);
                }
            }

            void PropagateStorm()
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                Talk(TALK_LIGHTNING_STORM);
                Talk(EMOTE_LIGHTNING_STORM);

                DoCastBossSpell(me, SPELL_LIGHTNING_STORM, false, 3000);

                scheduler.Schedule(Milliseconds(18000), [this](TaskContext context)
                {
                    if (me->IsAlive() && instance && instance->GetBossState(DATA_JINROKH) == IN_PROGRESS)
                    {
                        me->RemoveChanneledCast(targetGUID);
                        inStorm = false;
                    }
                });

                if (Aura* aura = me->AddAura(SPELL_LIGHTNING_STORM_VISUAL, me))
                    aura->SetDuration(15000);
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
                        case EVENT_STATIC_BURST:
                            Talk(TALK_STATIC_BURST);

                            if (Unit* target = me->GetVictim())
                                DoCastBossSpell(target, SPELL_STATIC_BURST, false, 1000);

                            events.ScheduleEvent(EVENT_STATIC_BURST, 22 * IN_MILLISECONDS);
                            break;
                        case EVENT_FOCUSED_LIGHTNING:
                            if (inStorm)
                            {
                                events.RescheduleEvent(eventId, 5 * IN_MILLISECONDS);
                                break;
                            }

                            Talk(TALK_FOCUSED_LIGHTNING);

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                            {
                                DoCast(target, SPELL_FOCUSED_LIGHTNING);
                                me->PrepareChanneledCast(me->GetAngle(target));
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                DoCast(target, SPELL_FOCUSED_LIGHTNING);
                                me->PrepareChanneledCast(me->GetAngle(target));
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                DoCast(target, SPELL_FOCUSED_LIGHTNING);
                                me->PrepareChanneledCast(me->GetAngle(target));
                            }

                            scheduler.Schedule(Milliseconds(1500), [this](TaskContext context)
                            {
                                if (me->IsAlive() && instance && instance->GetBossState(DATA_JINROKH) == IN_PROGRESS)
                                    me->RemoveChanneledCast(targetGUID);
                            });

                            events.ScheduleEvent(EVENT_FOCUSED_LIGHTNING, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_LIGHTNING_STORM:
                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            inStorm = true;
                            me->PrepareChanneledCast(me->GetOrientation());
                            me->GetMotionMaster()->MoveJump(aCenterPos, 35.f, 35.f, 1948);
                            events.ScheduleEvent(EVENT_LIGHTNING_STORM, 90000);
                            events.ScheduleEvent(EVENT_THUNDERING_THROW, 30000);
                            break;
                        case EVENT_PROPAGATE_STORM:
                            PropagateStorm();
                            break;
                        case EVENT_THUNDERING_THROW:
                            Talk(TALK_THUNDERING_THROW);
                            if (Unit* vict = me->GetVictim())
                            {
                                DoCast(vict, SPELL_THUNDERING_THROW_SILENCE, true);
                                vict->CastSpell(vict, SPELL_THUNDERING_THROW_FLY_VISUAL, true);
                                vict->CastSpell(me, SPELL_THUNDERING_THROW_VEHICLE, true);
                                DoCast(vict, SPELL_THUNDERING_THROW_FORCED_PLAYER);
                                Talk(EMOTE_THUNDERING_THROW, vict);
                            }
                            break;
                        case EVENT_BERSERK:
                            Talk(TALK_BERSERK);
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_HEIGHT_CHECK:
                            CheckHeight();
                            break;
                        case EVENT_IONIZATION:
                            DoCast(me, SPELL_IONIZATION);
                            events.ScheduleEvent(EVENT_IONIZATION, 90 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 90.0f);
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_jinrokhAI(creature);
        }
};

// Focused Lightning 69593
struct npc_focused_lightning : public ScriptedAI
{
    npc_focused_lightning(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    ObjectGuid targetGUID;

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        targetGUID = guid;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        targetGUID = ObjectGuid::Empty;
        me->SetInCombatWithZone();
        me->SetObjectScale(0.15f);
        me->SetDisplayId(47698);
        me->SetSpeed(MOVE_RUN, 0.1f);
        me->AddUnitState(UNIT_STATE_ROOT);

        DoCast(me, SPELL_FOCUSED_LIGHTNING_VISUAL, true);

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            if (me->GetObjectScale() >= 1.0f)
                return;

            me->SetObjectScale(me->GetObjectScale() + 0.15f);

            context.Repeat(Milliseconds(400));
        });

        scheduler
            .SetValidator([this] { return instance && instance->GetBossState(DATA_JINROKH) == IN_PROGRESS; })
            .Schedule(Milliseconds(3000), [this](TaskContext context)
        {

            DoCast(me, SPELL_FOCUSED_LIGHTNING_AOE, true);
            me->ClearUnitState(UNIT_STATE_ROOT);
            me->ClearUnitState(UNIT_STATE_CASTING); // why?

            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                if (!target->IsAlive())
                    DoCast(me, SPELL_FOCUSED_LIGHTNING_TARGET, true);
                else
                    me->GetMotionMaster()->MoveFollow(target, 0.f, 0.f);
            }

            // Need this cuz SPELL_FOCUSED_LIGHTNING_SPEED Affect so fast
            //me->SetSpeed(MOVE_RUN, me->GetSpeed(MOVE_RUN) + IsHeroic() ? 0.07f : 0.05f);
            if (!me->HasAura(SPELL_FOCUSED_LIGHTNING_SPEED))
                DoCast(me, SPELL_FOCUSED_LIGHTNING_SPEED, true);

            context.Repeat(Milliseconds(500));
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

};

// Lightning Fissure 69609
struct npc_lightning_fissure : public ScriptedAI
{
    npc_lightning_fissure(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        me->SetDisplayFromModel(1);
        DoCast(me, SPELL_LIGHTNING_FISSURE_VISUAL, true);

        scheduler
            .SetValidator([this] { return instance && instance->GetBossState(DATA_JINROKH) == IN_PROGRESS; })
            .Schedule(Milliseconds(200), [this](TaskContext context)
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, me, 50.0f);

            for (auto&& itr : pList)
            {
                if (itr->HasAura(SPELL_LIGHTNING_FISSURE_AURA, me->GetGUID()) && itr->GetExactDist2d(me) > 2.0f)
                    itr->RemoveAurasDueToSpell(SPELL_LIGHTNING_FISSURE_AURA, me->GetGUID());
                else if (!itr->HasAura(SPELL_LIGHTNING_FISSURE_AURA, me->GetGUID()) && itr->GetExactDist2d(me) <= 2.0f)
                    DoCast(itr, SPELL_LIGHTNING_FISSURE_AURA, true);
            }

            context.Repeat(Milliseconds(200));
        });
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Focused lightning Targeting 137194
class spell_focused_lightning_targeting : public SpellScript
{
    PrepareSpellScript(spell_focused_lightning_targeting);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // If we not found any dps then try select anyone but not tank
            if (pList.empty())
            {
                targets.remove_if(TankSpecTargetSelector());

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            targets.clear();

            for (auto&& itr : pList)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void HandleHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        caster->CastSpell(target, SPELL_FOCUSED_LIGHTNING_FIXATE, true);

        if (caster->ToCreature() && caster->ToCreature()->AI())
        {
            caster->ToCreature()->AI()->Talk(0, target);
            caster->ToCreature()->AI()->SetGUID(target->GetGUID());
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_focused_lightning_targeting::HandleHit);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_focused_lightning_targeting::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Focused Lightning AOE 137429
class spell_focused_lightning_aoe : public SpellScript
{
    PrepareSpellScript(spell_focused_lightning_aoe);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(focusedLightningPredicate());
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* pLightningFissure = GetHitCreature())
            {
                if (pLightningFissure->GetEntry() != NPC_FOCUSED_LIGHTNING)
                {
                    if (!pLightningFissure->HasAura(SPELL_LIGHTNING_FISSURE_VISUAL))
                        return;

                    // implosion
                    pLightningFissure->DisappearAndDie();
                    caster->CastSpell(caster, SPELL_IMPLOSION, true);
                    caster->Kill(caster);
                }
                else if (caster->GetInstanceScript())// Achievement Condition
                {
                    for (auto&& itr : caster->GetInstanceScript()->instance->GetPlayers())
                        if (Player* target = itr.GetSource())
                            target->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_LIGHTNING_OVERLOAD_ACHIEV);
                }
                return;
            }

            if (Unit* unit = GetHitUnit())
            {
                // Should we detonate?
                if (unit->HasAura(SPELL_FOCUSED_LIGHTNING_FIXATE, caster->GetGUID()))
                {
                    bool violent = false;
                    bool should_conduct = false;

                    if (unit->HasAura(SPELL_FLUIDITY))
                    {
                        unit->CastSpell(unit, SPELL_FOCUSED_LIGHTNING_CONDUCTION, true, 0, 0, caster->GetGUID());
                        should_conduct = true;
                    }
                    else if (unit->HasAura(SPELL_ELECTRIFIED_WATERS))
                    {
                        unit->CastSpell(unit, SPELL_FOCUSED_LIGHTNING_CONDUCTION, true, 0, 0, caster->GetGUID());
                        violent = true;
                        should_conduct = true;
                    }

                    caster->CastSpell(unit, violent ? SPELL_VIOLENT_LIGHTNING_DETONATION : SPELL_FOCUSED_LIGHTNING_DETONATION, true);

                    if (!should_conduct)
                    {
                        if (Unit* pBoss = ObjectAccessor::GetUnit(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_JINROKH) : ObjectGuid::Empty))
                            pBoss->CastSpell(unit, SPELL_LIGHTNING_FISSURE_SUMMON, true);
                    }

                    if (caster->ToCreature())
                    {
                        caster->ToCreature()->RemoveAllAuras();
                        unit->RemoveAurasDueToSpell(SPELL_FOCUSED_LIGHTNING_FIXATE, caster->GetGUID());
                        caster->ToCreature()->DisappearAndDie();
                        return;
                    }
                    caster->Kill(caster);
                    return;
                }

                caster->CastSpell(unit, SPELL_FOCUSED_LIGHTNING_DAMAGE, true);
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_focused_lightning_aoe::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_focused_lightning_aoe::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Focused Lightning Detonation 137374, 138990
class spell_focused_lightning_detonation : public SpellScript
{
    PrepareSpellScript(spell_focused_lightning_detonation);

    void HandleAfterHit()
    {
        if (Unit* caster = GetCaster())
            caster->UpdateObjectVisibility();
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_focused_lightning_detonation::HandleAfterHit);
    }

};

// Implosion 137507
class spell_implosion : public SpellScript
{
    PrepareSpellScript(spell_implosion)

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });
    }

    void HandleAfterHit()
    {
        if (Unit* caster = GetCaster())
            caster->Kill(caster);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_implosion::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_implosion::SelectTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        AfterHit += SpellHitFn(spell_implosion::HandleAfterHit);
    }
};

// Static Wound 138349
class spell_static_wound : public AuraScript
{
    PrepareAuraScript(spell_static_wound)

    bool Load() override
    {
        m_timer = 3000;
        return true;
    }

    uint32 m_timer;

    void HandleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        uint32 m_stacks = GetAura()->GetStackAmount();

        if (const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(SPELL_STATIC_WOUND/*, GetOwner()->GetMap()->GetDifficulty()*/))
        {
            int32 m_bp = spellInfo->Effects[0].BasePoints;
            int32 final_dmg = (m_bp * m_stacks); /// 3;

            if (Unit* pOwner = GetOwner()->ToUnit())
                pOwner->CastCustomSpell(pOwner, SPELL_STATIC_WOUND_DAMAGE, &final_dmg, 0, 0, true);
        }
    }

    void HandleOnUpdate(const uint32 uiDiff)
    {
        if (m_timer <= uiDiff)
        {
            m_timer = 3000;
            ModStackAmount(-1);
        }
        else
            m_timer -= uiDiff;
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_static_wound::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
        OnAuraUpdate += AuraUpdateFn(spell_static_wound::HandleOnUpdate);
    }
};

// Static Wound Damage 138389
class spell_static_wound_damage : public SpellScript
{
    PrepareSpellScript(spell_static_wound_damage);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* pHit = GetHitUnit())
                if (caster == pHit)
                    SetHitDamage(GetHitDamage() * 3);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_static_wound_damage::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Static Burst 137162
class spell_static_burst : public AuraScript
{
    PrepareAuraScript(spell_static_burst);

    void HandleOnPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* unit = GetOwner()->ToUnit())
        {
            if (Aura* aura = unit->GetAura(SPELL_STATIC_WOUND))
            {
                aura->ModStackAmount(aura->GetStackAmount() + 10 < 30 ? (aura->GetStackAmount() + 10) : 30);
                aura->SetDuration(25000);
            }
            else if (Aura* aura = unit->AddAura(SPELL_STATIC_WOUND, unit))
                aura->SetStackAmount(10);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_static_burst::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Lightning Storm Visual spell_lightning_storm_visual
class spell_lightning_storm_visual : public AuraScript
{
    PrepareAuraScript(spell_lightning_storm_visual);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->HasAura(SPELL_LIGHTNING_STORM))
            SetDuration(15000);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_lightning_storm_visual::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Lightning Storm 137313
class spell_lightning_storm : public AuraScript
{
    PrepareAuraScript(spell_lightning_storm);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->GetMap()->IsHeroic() && roll_chance_i(85))
                caster->CastSpell(caster, SPELL_LIGHTNING_STORM_SUMM, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_lightning_storm::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Thundering Throw 137161
class spell_thundering_throw_silence : public SpellScript
{
    PrepareSpellScript(spell_thundering_throw_silence);

    void HandleScriptEffect(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Creature* jinRokh = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_JINROKH) : ObjectGuid::Empty))
                caster->CastSpell(jinRokh, SPELL_THUNDERING_THROW_VEHICLE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thundering_throw_silence::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Thundering Throw Stun effect 137371
class spell_thundering_throw_stun : public SpellScript
{
    PrepareSpellScript(spell_thundering_throw_stun);

    void HandleOnEffectHit(SpellEffIndex /*effIdx*/)
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(caster, SPELL_THUNDERING_THROW_HIT_DAMAGE, true);
            caster->CastSpell(caster, SPELL_THUNDERING_THROW_HIT_AOE_DAMAGE, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thundering_throw_stun::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Thundering Throw 137180
class spell_thundering_throw : public SpellScript
{
    PrepareSpellScript(spell_thundering_throw);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            if (Vehicle* m_veh = caster->GetVehicleKit())
            {
                if (Unit* m_victim = m_veh->GetPassenger(0))
                {
                    std::list<Creature*> StatueList;
                    GetCreatureListWithEntryInGrid(StatueList, caster, NPC_JINROKH_STATUE, VISIBLE_RANGE);
                    StatueList.remove_if([=](Creature* target) { return target && (target->ToPlayer() || ((target->ToCreature()->AI() && target->ToCreature()->AI()->GetData(DATA_STATUE_DESTROYED) == 1) || target->GetEntry() != NPC_JINROKH_STATUE)); });

                    // If no valid statue
                    if (StatueList.empty())
                    {
                        // just break our target in hand without throw
                        m_victim->ExitVehicle();
                        return;
                    }

                    StatueList.sort(Trinity::ObjectDistanceOrderPred(caster));

                    // select farthest statue
                    target = StatueList.back();
                    m_victim->ExitVehicle();
                    m_victim->CastSpell(target, SPELL_THUNDERING_THROW_JUMP, true);

                    if (target->AI())
                    {
                        target->AI()->DoAction(ACTION_DESTROYED);
                        target->AI()->SetGUID(caster->GetGUID());
                    }
                    
                }
            }
        }
    }

    private:
        Creature* target;

    void Register() override
    {
        AfterCast += SpellCastFn(spell_thundering_throw::HandleAfterCast);
    }
};

// Thundering Throw 137175
class spell_thundering_throw_forced : public SpellScript
{
    PrepareSpellScript(spell_thundering_throw_forced);

    void HandleDummy(SpellEffIndex effIdx)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        target->ExitVehicle();
        target->CastSpell(stormCallerThrowPos.GetPositionX(), stormCallerThrowPos.GetPositionY(), stormCallerThrowPos.GetPositionZ(), SPELL_THUNDERING_THROW_JUMP, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_thundering_throw_forced::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Thundering Throw Jump 137173
class spell_thundering_throw_jump : public SpellScript
{
    PrepareSpellScript(spell_thundering_throw_jump);

    void HandleJump(SpellEffIndex /*effIdx*/)
    {
        if (Unit* caster = GetCaster())
        {
            // Pre Event
            if (caster->ToCreature())
            {
                GetCaster()->RemoveAurasDueToSpell(SPELL_THUNDERING_THROW_SILENCE);
                GetCaster()->RemoveAurasDueToSpell(SPELL_THUNDERING_THROW_FLY_VISUAL);
                GetCaster()->Kill(GetCaster());
                return;
            }

            // If we`d hit to statue, then repeat cast to ground
            if (GetHitDest()->GetPositionZ() > 130.0f)
            {
                GetPositionWithDistInOrientation(GetCaster(), frand(32.0f, 35.0f), Position::NormalizeOrientation(GetCaster()->GetOrientation() - M_PI), x, y);
                GetCaster()->CastSpell(x, y, 124.033f, SPELL_THUNDERING_THROW_JUMP, true);
            }
            // If we`d hit to ground, then remove any auras and cast self-dmg/aoe dmg
            else
            {
                GetCaster()->RemoveAurasDueToSpell(SPELL_THUNDERING_THROW_SILENCE);
                GetCaster()->RemoveAurasDueToSpell(SPELL_THUNDERING_THROW_FLY_VISUAL);
                GetCaster()->CastSpell(GetCaster(), SPELL_THUNDERING_THROW_STUN, false);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_thundering_throw_jump::HandleJump, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

class npc_jinrokh_statue : public CreatureScript
{
    enum eEvents
    {
        EVENT_NONE,
        EVENT_WATER_BEAM,
        EVENT_SPAWN_WATER,
    };

    public:
        npc_jinrokh_statue() : CreatureScript("npc_jinrokh_statue") { }

        struct npc_jinrokh_statueAI : public ScriptedAI
        {
            npc_jinrokh_statueAI(Creature* creature) : ScriptedAI(creature) 
            {
                statueGuid = ObjectGuid::Empty;
                statueData = 0;
                m_phase = 0;
                playerGuid = ObjectGuid::Empty;
                me->SetFloatValue(OBJECT_FIELD_SCALE, me->GetFloatValue(OBJECT_FIELD_SCALE) * 1.4f);
            }

            uint32 statueData;
            uint32 m_phase;
            ObjectGuid playerGuid;
            ObjectGuid statueGuid;
            EventMap events;

            void InitializeStatue()
            {
                statueGuid = ObjectGuid::Empty;

                float fDist = 1000.f;

                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    for (uint32 i = 0; i < 4; ++i)
                    {
                        if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_MOGU_STATUE_1 + i)))
                        {
                            if (go->GetExactDist2d(me) < fDist)
                            {
                                fDist = go->GetExactDist2d(me);
                                statueGuid = go->GetGUID();
                            }
                        }
                    }
                }

            }
            
            void HandleStatue(bool active)
            {
                if (GameObject* pStatue = ObjectAccessor::GetGameObject(*me, statueGuid))
                    pStatue->SetGoState(active ? GO_STATE_ACTIVE : GO_STATE_READY);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_STATUE_DESTROYED)
                    statueData = data;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_STATUE_DESTROYED)
                    return statueData;

                return 0;
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                playerGuid = guid;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_DESTROYED:
                        InitializeStatue();
                        m_phase = 1;
                        events.ScheduleEvent(EVENT_WATER_BEAM, 4000);
                        events.ScheduleEvent(EVENT_SPAWN_WATER, 7000);
                        SetData(DATA_STATUE_DESTROYED, 1);
                        break;
                    case ACTION_RESET:
                        HandleStatue(false);
                        m_phase = 0;
                        playerGuid = ObjectGuid::Empty;
                        me->SetVisible(false);
                        me->SetVisible(true);
                        me->UpdateObjectVisibility();
                        me->RemoveAllAuras();
                        events.Reset();
                        SetData(DATA_STATUE_DESTROYED, 0);
                    
                        if (Creature* pWaters = GetClosestCreatureWithEntry(me, NPC_CONDUCTIVE_WATER, 100.f))
                            pWaters->DespawnOrUnsummon();
                        break;
                }
            }

            Position DoSpawnWater()
            {
                uint8 dist = 0;
                float i_range = 1000.f;

                for (uint8 i = 0; i < 4; ++i)
                {
                    float new_dist = me->GetDistance(aWaterPos[i]);

                    if (new_dist < i_range)
                    {
                        dist = i;
                        i_range = new_dist;
                    }
                }

                return aWaterPos[dist];
            }

            void DoTossPlayer(Player* player)
            {
                HandleStatue(true);
                playerGuid = player->GetGUID();
                player->CastSpell(DoSpawnWater(), SPELL_THUNDERING_THROW_JUMP, true);
                player->CastSpell(player, SPELL_THUNDERING_THROW_HIT_DAMAGE, true);
                m_phase = 2;
            }

            void CheckPlayerState()
            {

                switch (m_phase)
                {
                    case 1:
                    case 2:
                        if (Player* player = ObjectAccessor::GetPlayer(*me, playerGuid))
                        {
                            if (!player->HasUnitState(UNIT_STATE_JUMPING))
                                DoTossPlayer(player);
                        }
                        break;
                }
            }

            void DoStunPlayer(Player* player)
            {
                player->CastSpell(player, SPELL_THUNDERING_THROW_HIT_AOE_DAMAGE, true);
                player->CastSpell(player, SPELL_THUNDERING_THROW_STUN, true);
                player->RemoveAurasDueToSpell(SPELL_THUNDERING_THROW_SILENCE);

                m_phase = 0;
                playerGuid = ObjectGuid::Empty;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                CheckPlayerState();

                switch (events.ExecuteEvent())
                {
                    case EVENT_WATER_BEAM:
                        DoCast(me, SPELL_CONDUCTIVE_WATERFALL, true);
                        break;
                    case EVENT_SPAWN_WATER:
                        if (Creature* pWater = me->SummonCreature(NPC_CONDUCTIVE_WATER, DoSpawnWater()))
                        {
                            if (pWater->AI())
                                pWater->AI()->DoAction(ACTION_RESET);
                        }
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jinrokh_statueAI(creature);
        }
};

// Conductive Water 69469
class npc_conductive_water : public CreatureScript
{
    enum eEvents
    {
        EVENT_NONE,
        EVENT_GROW,
        EVENT_ELECTRIFY,
        EVENT_FINALIZE_ELECTRIFY
    };

    public:
        npc_conductive_water() : CreatureScript("npc_conductive_water") { }

        struct npc_conductive_waterAI : public ScriptedAI
        {
            npc_conductive_waterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_RESET:
                        me->SetObjectScale(0.8f);
                        DoCast(me, SPELL_CONDUCTIVE_WATER_VISUAL, true);
                        events.ScheduleEvent(EVENT_GROW, 500);
                        break;
                    case ACTION_ELECTRIFY:
                        events.ScheduleEvent(EVENT_ELECTRIFY, 5000);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GROW:
                            DoCast(me, SPELL_CONDUCTIVE_WATER_GROW, true);
                            break;
                        case EVENT_ELECTRIFY:
                            events.ScheduleEvent(EVENT_FINALIZE_ELECTRIFY, 1500);
                            DoCast(me, SPELL_ELECTRIFY_WATERS, true);
                            break;
                        case EVENT_FINALIZE_ELECTRIFY:
                            DoCast(me, SPELL_ELECTRIFIED_WATER_VISUAL, true);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_conductive_waterAI(creature);
        }
};

// De call Storm 69676
class npc_lightning_storm_bunny : public CreatureScript
{
    public:
        npc_lightning_storm_bunny() : CreatureScript("npc_lightning_storm_bunny") { }

        enum iEvents
        {
            EVENT_LAUNCH = 1,
        };

        struct npc_lightning_storm_bunnyAI : public ScriptedAI
        {
            npc_lightning_storm_bunnyAI(Creature* creature) : ScriptedAI(creature) 
            {
                SetCombatMovement(false); // // no idea why it happened...
            }

            EventMap nonCombatEvents;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetDisplayFromModel(1);
                DoCast(me, SPELL_LIGHTNING_STRIKE_VISUAL, true);
                nonCombatEvents.ScheduleEvent(EVENT_LAUNCH, 4 * IN_MILLISECONDS);
            }

            void EnterEvadeMode() override { }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_LAUNCH)
                    {
                        DoCast(me, SPELL_LIGHTNING_STRIKE_EXPLOSION, true);

                        for (uint8 m_oriIndex = 1; m_oriIndex <= 4; ++m_oriIndex)
                            me->SummonCreature(NPC_LIGHTNING_STIKE_CHARGES, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), Position::NormalizeOrientation(me->GetOrientation() + m_oriIndex * (M_PI / 2)), TEMPSUMMON_MANUAL_DESPAWN);

                        me->DespawnOrUnsummon();
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lightning_storm_bunnyAI(creature);
        }
};

// Lightning Strike Charges 69344
struct npc_lightning_strike_charges : public ScriptedAI
{
    npc_lightning_strike_charges(Creature* creature) : ScriptedAI(creature) { }

    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(me, SPELL_LIGHTNING_STRIKE_LIGHTNING_CHARGE, true);
        DoCast(me, SPELL_LIGHTNING_STRIKE_AREATRIGGER, true);
        GetPositionWithDistInOrientation(me, 100.0f, me->GetOrientation(), x, y);
        me->GetMotionMaster()->MoveCharge(x, y, me->GetPositionZ(), 15.5f, EVENT_CHARGE);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_CHARGE)
            me->DespawnOrUnsummon();
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Conductive Water Grow 137676
class spell_conductive_water_grow : public AuraScript
{
    PrepareAuraScript(spell_conductive_water_grow);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_CONDUCTIVE_WATER_DUMMY, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_conductive_water_grow::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

class ConductiveWaterDummyPredicate
{
    public:
        ConductiveWaterDummyPredicate() { }
    
        bool operator()(WorldObject* target) const
        {
            if (target && target->ToPlayer())
                return false;

            if (target && target->ToCreature() && target->ToCreature()->GetEntry() == NPC_LIGHTNING_FISSURE)
                return false;

            return true;
        }
};

// Conductive Water Dummy 137168
class spell_conductive_water_dummy : public SpellScript
{
    PrepareSpellScript(spell_conductive_water_dummy);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
        {
            target->CastSpell(target, SPELL_LIGHTNING_FISSURE_CONDUCTION, true);
            target->DespawnOrUnsummon();
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(ConductiveWaterDummyPredicate());
        targets.remove_if([=](WorldObject* target) { return target && target->GetExactDist2d(GetCaster()) > 3.3f * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_conductive_water_dummy::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_conductive_water_dummy::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Conductive Water Dummy 137168
class spell_conductive_water_dummy_aura : public AuraScript
{
    PrepareAuraScript(spell_conductive_water_dummy_aura);

    void HandleOnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player *owner = GetOwner()->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                if (caster->HasAura(SPELL_ELECTRIFIED_WATER_VISUAL))
                    caster->CastSpell(owner, SPELL_ELECTRIFIED_WATERS, true);
                else
                    owner->CastSpell(owner, SPELL_FLUIDITY, true);

                caster->CastSpell(owner, SPELL_CONDUCTIVE_WATERS, true);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_conductive_water_dummy_aura::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// Fluidity 138002
class spell_fluidity : public AuraScript
{
    PrepareAuraScript(spell_fluidity);

    void HandleOnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Aura* aura = owner->GetAura(SPELL_CONDUCTIVE_WATERS))
            {
                aura->RefreshDuration();
            }
            else if (Aura* aura = owner->AddAura(SPELL_CONDUCTIVE_WATERS, owner))
            {
                aura->SetMaxDuration(GetMaxDuration());
                aura->RefreshDuration();
            }
        }
    }

    void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_CONDUCTIVE_WATERS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_fluidity::HandleOnApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_fluidity::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_fluidity::HandleOnRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_fluidity::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Focused Lightning Conduction 137530
class spell_focused_lightning_conduction : public SpellScript
{
    PrepareSpellScript(spell_focused_lightning_conduction);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
            if (Creature* pWaters = GetClosestCreatureWithEntry(caster, NPC_CONDUCTIVE_WATER, 100.f))
                targets.remove_if(conductionPredicate(pWaters));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_focused_lightning_conduction::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// Lightning Fissure Conduction 138133
class spell_lightning_fissure_conduction : public SpellScript
{
    PrepareSpellScript(spell_lightning_fissure_conduction);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });

        if (Unit* caster = GetCaster())
        {
            if (Creature* pWaters = GetClosestCreatureWithEntry(caster, NPC_CONDUCTIVE_WATER, 100.f))
            {
                targets.remove_if(conductionPredicate(pWaters));
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lightning_fissure_conduction::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Focused Lightning 137399
class spell_focused_lightning : public SpellScript
{
    PrepareSpellScript(spell_focused_lightning);

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                GetPositionWithDistInOrientation(caster, 10.0f, caster->GetAngle(target), x, y);

                if (Creature* focusedLightning = caster->SummonCreature(NPC_FOCUSED_LIGHTNING, x, y, caster->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
                {
                    focusedLightning->AI()->Talk(0, target);
                    focusedLightning->AI()->SetGUID(target->GetGUID());
                    focusedLightning->CastSpell(target, SPELL_FOCUSED_LIGHTNING_FIXATE, true);
                }
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnHit += SpellHitFn(spell_focused_lightning::HandleOnHit);
    }
};

// Ionization 138732
class spell_ionization_aura : public AuraScript
{
    PrepareAuraScript(spell_ionization_aura);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Creature* jinrokh = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_JINROKH) : ObjectGuid::Empty))
            {
                int32 bp = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                jinrokh->CastCustomSpell(owner, SPELL_IONIZATION_PROC, &bp, 0, 0, true);
                
                if (owner->HasAura(SPELL_FLUIDITY))
                    owner->CastCustomSpell(owner, SPELL_IONIZATION_DAMAGES_CONDUCTION, &bp, 0, 0, true);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_ionization_aura::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ionization 138732
class spell_ionization : public SpellScript
{
    PrepareSpellScript(spell_ionization);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if(TankSpecTargetSelector());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ionization::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Lightning Storm Summ 140774
class spell_lightning_storm_summ : public SpellScript
{
    PrepareSpellScript(spell_lightning_storm_summ);

    void SelectTargets(SpellDestination& dest)
    {
        if (Unit* caster = GetCaster())
        {
            float x, y;
            GetPositionWithDistInOrientation(caster, urand(10.0f, 50.0f), frand(0.0f, 2 * M_PI), x, y);

            Position newPos = { x, y, caster->GetPositionZ() + 1.0f, 0 };
            dest.Relocate(newPos);
        }
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_lightning_storm_summ::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Ionization Proc Conduction 138743
class spell_ionization_proc_conduction : public SpellScript
{
    PrepareSpellScript(spell_ionization_proc_conduction);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && !target->ToUnit()->HasAura(SPELL_FLUIDITY); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ionization_proc_conduction::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// 775. Summoned by 139982 - Lightning Storm
class sat_jinrokh_lightning_storm : public IAreaTriggerAura
{
    std::list<uint64> targetGUIDs;

    void OnInit() override
    {
        targetGUIDs.clear();
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && std::find(targetGUIDs.begin(), targetGUIDs.end(), triggering->ToPlayer()->GetGUID()) == targetGUIDs.end();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(target, SPELL_LIGHTNING_DEFFUSION, true);
                targetGUIDs.push_back(target->GetGUID());
            }
        }
    }
};

void AddSC_boss_jinrokh()
{
    new boss_jinrokh();
    new creature_script<npc_focused_lightning>("npc_focused_lightning");
    new creature_script<npc_lightning_fissure>("npc_lightning_fissure");
    new npc_jinrokh_statue();
    new npc_conductive_water();
    new npc_lightning_storm_bunny();
    new creature_script<npc_lightning_strike_charges>("npc_lightning_strike_charges");

    new spell_script<spell_focused_lightning_targeting>("spell_focused_lightning_targeting");
    new spell_script<spell_focused_lightning_aoe>("spell_focused_lightning_aoe");
    new spell_script<spell_focused_lightning_detonation>("spell_focused_lightning_detonation");
    new spell_script<spell_implosion>("spell_implosion");
    new aura_script<spell_static_wound>("spell_static_wound");
    new spell_script<spell_static_wound_damage>("spell_static_wound_damage");
    new aura_script<spell_static_burst>("spell_static_burst");
    new aura_script<spell_lightning_storm_visual>("spell_lightning_storm_visual");
    new aura_script<spell_lightning_storm>("spell_lightning_storm");
    new spell_script<spell_thundering_throw_silence>("spell_thundering_throw_silence");
    new spell_script<spell_thundering_throw_stun>("spell_thundering_throw_stun");
    new spell_script<spell_thundering_throw>("spell_thundering_throw");
    new spell_script<spell_thundering_throw_forced>("spell_thundering_throw_forced");
    new spell_script<spell_thundering_throw_jump>("spell_thundering_throw_jump");
    new aura_script<spell_conductive_water_grow>("spell_conductive_water_grow");
    new spell_script<spell_conductive_water_dummy>("spell_conductive_water_dummy");
    new aura_script<spell_conductive_water_dummy_aura>("spell_conductive_water_dummy_aura");
    new aura_script<spell_fluidity>("spell_fluidity");
    new spell_script<spell_focused_lightning_conduction>("spell_focused_lightning_conduction");
    new spell_script<spell_lightning_fissure_conduction>("spell_lightning_fissure_conduction");
    new spell_script<spell_focused_lightning>("spell_focused_lightning");
    new aura_script<spell_ionization_aura>("spell_ionization_aura");
    new spell_script<spell_ionization>("spell_ionization");
    new spell_script<spell_lightning_storm_summ>("spell_lightning_storm_summ");
    new spell_script<spell_ionization_proc_conduction>("spell_ionization_proc_conduction");
    new atrigger_script<sat_jinrokh_lightning_storm>("sat_jinrokh_lightning_storm");
}
