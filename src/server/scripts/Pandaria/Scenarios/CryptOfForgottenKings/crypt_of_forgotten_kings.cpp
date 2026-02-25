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

#include "crypt_of_forgotten_kings.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "ScenarioMgr.h"

enum spells
{
    SPELL_UNCONTROLLED_ANGER        = 129025,
    SPELL_ANGER_SPAWN_GENERATOR     = 128777,
    SPELL_SHADOW_EXPLOSION_COSMETIC = 120686,
    SPELL_SHADOW_EXPLOSION_SUMMON   = 120689,
    SPELL_BREATH_OF_HATE            = 120222,
    SPELL_TIGER_PALM                = 120911,
    SPELL_STONE_FORM                = 117838,
    SPELL_LIGHTNING_TRAP_BUNNY      = 119747,
    SPELL_LIGHTNING_TRAP            = 119748,
    SPELL_FIRE_TRAP                 = 119667,
    SPELL_TRIGGER_FIRE_TRAP         = 120349,
    SPELL_TRIGGER_LIGHTNING_TRAP    = 123800,
    SPELL_KNOCKBACK_TRAP            = 120365,
    SPELL_ACHIEV_FIGHT_WITH_ANGER   = 143316,
};

enum Talks
{
    TALK_INTRO   = 0,
    TALK_SPECIAL = 1,
};

const std::vector<float> CircleOri =
{
    2 * M_PI,
    M_PI / 4,
    M_PI / 2,
    3 * M_PI / 4,
    M_PI,
    5 * M_PI / 4,
    3 * M_PI / 2,
    7 * M_PI / 4
};

struct cfk_sha_typeAI : public ScriptedAI
{
    cfk_sha_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    float randHealth;
    EventMap events, nonCombatEvents;
    SummonList summons;

    void InitializeAI() override
    {
        me->SetReactState(REACT_AGGRESSIVE);
        me->setActive(true);

        Reset();
    }

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->GetEntry() == NPC_JIN_IRONFIST)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);

            nonCombatEvents.ScheduleEvent(1, urand(1 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_UNCONTROLLED_ANGER, true);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == 1)
            {
                Movement::MoveSplineInit init(me);
                for (auto itr : JinShaExplosionPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 2.0f), itr.GetPositionY() + frand(-0.5f, 0.5f), itr.GetPositionZ()));

                init.SetSmooth();
                init.SetVelocity(5.0f);
                init.SetUncompressed();
                init.Launch();
                me->DespawnOrUnsummon(me->GetSplineDuration());
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class npc_cfk_sha_base : public CreatureScript
{
    public:
        npc_cfk_sha_base() : CreatureScript("npc_cfk_sha_base") { }

        struct npc_gss_trash_baseAI : public cfk_sha_typeAI
        {
            npc_gss_trash_baseAI(Creature* creature) : cfk_sha_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gss_trash_baseAI(creature);
        }
};

Position GetInitiatePosition(Creature* me, ObjectGuid m_caster, float dist = 0.0f, float m_ori = 0.0f)
{
    Unit* caster = ObjectAccessor::GetUnit(*me, m_caster);

    if (!caster)
        return { 0.0f, 0.0f, 0.0f, 0.0f };

    float x, y, z;
    GetPositionWithDistInOrientation(caster, dist ? dist : 15.0f, m_ori ? m_ori : frand(M_PI, 2 * M_PI), x, y);
    z = caster->GetPositionZ();
    caster->UpdateAllowedPositionZ(x, y, z);
    Position pos = { x, y, z, caster->GetOrientation() };

    return pos;
}

// Seething Sha 61610
class npc_cfk_seething_sha : public CreatureScript
{
    public:
        npc_cfk_seething_sha() : CreatureScript("npc_cfk_seething_sha") { }

        enum eEvents
        {
            EVENT_BREATH_OF_HATE        = 1,
            EVENT_BREATH_OF_HATE_CANCEL = 2,
        };

        struct npc_cfk_seething_shaAI : public cfk_sha_typeAI
        {
            npc_cfk_seething_shaAI(Creature* creature) : cfk_sha_typeAI(creature) { }

            InstanceScript* instance;
            EventMap events;
            ObjectGuid targetGUID;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                events.Reset();
                targetGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BREATH_OF_HATE, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
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
                        case EVENT_BREATH_OF_HATE:
                            if (Unit* vict = me->GetVictim())
                            {
                                targetGUID = vict->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(vict), SPELL_BREATH_OF_HATE);
                            }
                            events.ScheduleEvent(EVENT_BREATH_OF_HATE_CANCEL, 4500);
                            break;
                        case EVENT_BREATH_OF_HATE_CANCEL:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_seething_shaAI(creature);
        }
};

// Shado Pan Initiate 65225
class npc_cfk_shado_pan_initiate : public CreatureScript
{
    public:
        npc_cfk_shado_pan_initiate() : CreatureScript("npc_cfk_shado_pan_initiate") { }

        struct npc_cfk_shado_pan_initiateAI : public ScriptedAI
        {
            npc_cfk_shado_pan_initiateAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;

            void InitializeAI() override 
            {
                instance = me->GetInstanceScript();
                me->SetFaction(1665);

                if (Unit* Jin = ObjectAccessor::GetUnit(*me, instance ? instance->GetGuidData(NPC_JIN_IRONFIST) : ObjectGuid::Empty))
                    me->AI()->AttackStart(Jin);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(35))
                    damage = 0;
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_shado_pan_initiateAI(creature);
        }
};

// Jin Ironfist 61814
class npc_cfk_jin_ironfist : public CreatureScript
{
    public:
        npc_cfk_jin_ironfist() : CreatureScript("npc_cfk_jin_ironfist") { }

        enum iEvents
        {
            EVENT_KILL_INITIATES = 1,
            EVENT_FLYING_KICK    = 2,
            EVENT_SHADOW_STEP    = 3,
            EVENT_RELENTLESS     = 4,
        };

        enum iSpells
        {
            SPELL_ENRAGE              = 127823,
            SPELL_FLYING_SERPENT_KICK = 120816,
            SPELL_RELENTLESS          = 120817,
            SPELL_SHADOW_STEP         = 128681,
            SPELL_RAIN_OF_SHA         = 131641, // should jump and kill initiates in 7s after combat begun
        };

        struct npc_cfk_jin_ironfistAI : public BossAI
        {
            npc_cfk_jin_ironfistAI(Creature* creature) : BossAI(creature, DATA_JIN_IRONFIST) { }

            ObjectGuid targetGUID;
            bool HasEnrage;

            void Reset() override
            {
                _Reset();
                events.Reset();
                targetGUID = ObjectGuid::Empty;
                HasEnrage = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_KILL_INITIATES, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FLYING_KICK, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOW_STEP, 25 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_RELENTLESS, 30 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override { }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SetData(DATA_JIN_IRONFIST, DONE);
                    instance->SetData(DATA_POOL_OF_LIFE, IN_PROGRESS);
                    instance->DoCastSpellOnPlayers(SPELL_POOL_OF_LIFE_PROGRESS_BAR);

                    if (Creature* lifePool = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_POOL_OF_LIFE)))
                        lifePool->AI()->DoAction(ACTION_CHAPTER_TWO_BEGIN);

                    // Cosmetic event: Essence of Hate move to life pool
                    for (uint8 i = 0; i < urand(7, 9); i++)
                        me->SummonCreature(NPC_ESSENCE_OF_HATE, me->GetPositionX() + frand(-4.0f, 4.0f), me->GetPositionY() + frand(-2.0f, 2.0f), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override 
            {
                if (pointId == EVENT_JUMP)
                    me->CastSpell(me, SPELL_RAIN_OF_SHA, false);

                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(30) && !HasEnrage)
                {
                    HasEnrage = true;
                    me->CastSpell(me, SPELL_ENRAGE, false);
                }
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
                        case EVENT_KILL_INITIATES:
                            if (Unit* vict = me->GetVictim())
                            {
                                if (!vict->ToPlayer())
                                {
                                    events.RescheduleEvent(EVENT_KILL_INITIATES, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                                    break;
                                }
                            }
                            Talk(TALK_INTRO);
                            me->GetMotionMaster()->MoveJump(GetInitiatePosition(me, me->GetGUID(), frand(1.5f, 2.0f)), 10.0f, 15.0f, EVENT_JUMP);
                            break;
                        case EVENT_FLYING_KICK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, FlyingKickPredicate(me)))
                                me->CastSpell(target, SPELL_FLYING_SERPENT_KICK, false);

                            events.ScheduleEvent(EVENT_FLYING_KICK, urand(9 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                            break;
                        case EVENT_RELENTLESS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_RELENTLESS, false);

                            events.ScheduleEvent(EVENT_RELENTLESS, 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_SHADOW_STEP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_SHADOW_STEP, false);

                            events.ScheduleEvent(EVENT_SHADOW_STEP, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct FlyingKickPredicate 
                {
                    public:
                        FlyingKickPredicate(Creature const* me) : _me(me) { }

                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) > 8.0f;
                        }

                    private:
                        Creature const* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_jin_ironfistAI(creature);
        }
};

// Pool of Life 61885
class npc_cfk_pool_of_life : public CreatureScript
{
    public:
        npc_cfk_pool_of_life() : CreatureScript("npc_cfk_pool_of_life") { }

        enum iEvents
        {
            EVENT_PERIODIC_ESSENCE = 1,
        };

        struct npc_cfk_pool_of_lifeAI : public BossAI
        {
            npc_cfk_pool_of_lifeAI(Creature* creature) : BossAI(creature, DATA_POOL_OF_LIFE) { }

            EventMap nonCombatEvents;

            void InitializeAI() override 
            {
                if (instance && instance->GetData(DATA_POOL_OF_LIFE) == IN_PROGRESS)
                    me->AI()->DoAction(ACTION_CHAPTER_TWO_BEGIN);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_CHAPTER_TWO_BEGIN:
                        SendEssenceActivate();
                        break;
                    case ACTION_CHAPTER_TWO_RESET:
                        SendEssenceActivate(true);

                        if (instance)
                            instance->SetData(DATA_POOL_OF_LIFE, DONE);

                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (Player* itr = me->FindNearestPlayer(50.0f))
                    summon->AI()->AttackStart(itr);
                else
                    summon->DespawnOrUnsummon(10 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
            }

            void SendEssenceActivate(bool reset = false)
            {
                if (!reset)
                    nonCombatEvents.ScheduleEvent(EVENT_PERIODIC_ESSENCE, urand(7 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));

                std::list<Creature*> Essence;
                GetCreatureListWithEntryInGrid(Essence, me, NPC_ESSENCE_OF_HATE, 100.0f);

                for (auto&& itr : Essence)
                {
                    if (reset)
                    {
                        itr->DespawnOrUnsummon();
                        nonCombatEvents.Reset();
                    }
                    else
                    {
                        itr->SetVisible(true);
                        itr->SetReactState(REACT_AGGRESSIVE);
                    }
                }
            }

            void SendEssenceSpawn(uint8 m_count)
            {
                for (uint8 i = 0; i < m_count; ++i)
                    me->SummonCreature(NPC_ESSENCE_OF_HATE, GetInitiatePosition(me, me->GetGUID(), 10.5f, Trinity::Containers::SelectRandomContainerElement(CircleOri)));
            }

            void UpdateAI(uint32 diff) override 
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_PERIODIC_ESSENCE)
                    {
                        SendEssenceSpawn(urand(5, 8));
                        nonCombatEvents.ScheduleEvent(EVENT_PERIODIC_ESSENCE, urand(9 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_pool_of_lifeAI(creature);
        }
};

// Crypt Guardian 61783
class npc_cfk_crypt_guardian_hall : public CreatureScript
{
    public:
        npc_cfk_crypt_guardian_hall() : CreatureScript("npc_cfk_crypt_guardian_hall") { }

        enum iEvents
        {
            EVENT_BLADESTORM = 1,
            EVENT_HORR_SHOUT = 2,
        };

        enum iSpells
        {
            SPELL_BLADESTORM       = 128969,
            SPELL_HORRENDOUS_SHOUT = 120310,
            SPELL_GUARDIANS_CHAIN  = 142192,
        };

        struct npc_cfk_crypt_guardian_hallAI : public ScriptedAI
        {
            npc_cfk_crypt_guardian_hallAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                events.Reset();
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_GUARDIAN_INIT)
                {
                    me->SetFaction(16);
                    me->GetMotionMaster()->MovePoint(0, GetInitiatePosition(me, me->GetGUID(), 15.0f, me->GetOrientation()));
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_INTRO);
                events.ScheduleEvent(EVENT_BLADESTORM, 6 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HORR_SHOUT, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    me->SetHomePosition(*me);
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
                        case EVENT_BLADESTORM:
                            DoCast(me, SPELL_BLADESTORM);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_BLADESTORM, urand(11 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                            break;
                        case EVENT_HORR_SHOUT:
                            DoCast(me, SPELL_HORRENDOUS_SHOUT, true);
                            events.ScheduleEvent(EVENT_HORR_SHOUT, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                    }
                }
                
                if (Unit* vict = me->GetVictim())
                    if (!me->IsWithinMeleeRange(vict, 8.0f))
                        me->CastSpell(vict, SPELL_GUARDIANS_CHAIN, false);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_crypt_guardian_hallAI(creature);
        }
};

// Crypt Guardian 61418, 61766
class npc_cfk_crypt_guardian_vault : public CreatureScript
{
    public:
        npc_cfk_crypt_guardian_vault() : CreatureScript("npc_cfk_crypt_guardian_vault") { }

        enum iEvents
        {
            EVENT_CRUSH_ARMOR  = 1,
            EVENT_GUARD_STRIKE = 2,
            EVENT_REACH_POS    = 3,
        };

        enum iSpells
        {
            SPELL_CRUSH_ARMOR     = 120440,
            SPELL_GUARDIAN_STRIKE = 119843,
        };

        struct npc_cfk_crypt_guardian_vaultAI : public ScriptedAI
        {
            npc_cfk_crypt_guardian_vaultAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;
            InstanceScript* instance;
            ObjectGuid targetGUID;
            bool initiateGuard;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                initiateGuard = me->GetEntry() == NPC_CRYPT_GUARDIAN_3 ? true : false;
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                targetGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_GUARD_STRIKE, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));

                if (initiateGuard)
                    events.ScheduleEvent(EVENT_CRUSH_ARMOR, 7 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_GUARDIAN_INIT)
                {
                    me->SetFaction(16);
                    me->SetSpeed(MOVE_WALK, 1.58f);
                    me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), false);

                    if (me->GetDBTableGUIDLow() == 544025)
                        nonCombatEvents.ScheduleEvent(EVENT_REACH_POS, me->GetSplineDuration());
                    
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && initiateGuard)
                    return;

                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                me->HandleEmoteStateCommand(EMOTE_STATE_READY2H);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_REACH_POS)
                        Talk(TALK_INTRO);
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CRUSH_ARMOR:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_CRUSH_ARMOR, false);

                            events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(12 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                        case EVENT_GUARD_STRIKE:
                            DoCast(me, SPELL_GUARDIAN_STRIKE);
                            events.ScheduleEvent(EVENT_GUARD_STRIKE, urand(13 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_crypt_guardian_vaultAI(creature);
        }
};

// Abomination of Anger 61707
class npc_cfk_abomination_of_anger : public CreatureScript
{
    public:
        npc_cfk_abomination_of_anger() : CreatureScript("npc_cfk_abomination_of_anger") { }

        enum iEvents
        {
            EVENT_DEATHFORCE            = 1,
            EVENT_DEATHFORCE_PREPARE    = 2,
            EVENT_BREATH_OF_HATE        = 3,
            EVENT_BREATH_OF_HATE_CANCEL = 4,
            EVENT_CLOUD_OF_ANGER        = 5,
        };

        enum iSpells
        {
            SPELL_SOUL_SHIFT         = 136947,
            SPELL_DEATHFORCE         = 120215,
            SPELL_BREATH_OF_HATE     = 120929,
            SPELL_CLOUD_OF_ANGER     = 120743, // aura
            SPELL_CLOUD_OF_ANGER_SUM = 120824,
        };

        struct npc_cfk_abomination_of_angerAI : public BossAI
        {
            npc_cfk_abomination_of_angerAI(Creature* creature) : BossAI(creature, DATA_ABOMINATION_OF_ANGER) { }

            ObjectGuid targetGUID;
            uint32 initiateArrived;

            void InitializeAI() override
            {
                initiateArrived = 1;
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                targetGUID = ObjectGuid::Empty;
                me->GetMap()->SetWorldState(WORLDSTATE_FANCY_FOORWORK, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_DEATHFORCE_PREPARE, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BREATH_OF_HATE, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CLOUD_OF_ANGER, 5 * IN_MILLISECONDS);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_ABOMINATION_INIT)
                    return initiateArrived;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_ABOMINATION_INIT)
                    initiateArrived = data;
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_ACHIEV_FIGHT_WITH_ANGER, true);

                if (instance)
                    instance->SetData(DATA_ABOMINATION_OF_ANGER, DONE);
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
                        case EVENT_DEATHFORCE:
                            me->CastSpell(me, SPELL_DEATHFORCE, false);
                            break;
                        case EVENT_DEATHFORCE_PREPARE:
                            Talk(TALK_SPECIAL);
                            DoCast(me, SPELL_SOUL_SHIFT);
                            events.ScheduleEvent(EVENT_DEATHFORCE, 1000);
                            events.ScheduleEvent(EVENT_DEATHFORCE_PREPARE, urand(20 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                            break;
                        case EVENT_BREATH_OF_HATE:
                            if (Unit* vict = me->GetVictim())
                            {
                                targetGUID = vict->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(vict), SPELL_BREATH_OF_HATE);
                            }
                            events.ScheduleEvent(EVENT_BREATH_OF_HATE, urand(10 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                            events.ScheduleEvent(EVENT_BREATH_OF_HATE_CANCEL, 4500);
                            break;
                        case EVENT_BREATH_OF_HATE_CANCEL:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                        case EVENT_CLOUD_OF_ANGER:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_CLOUD_OF_ANGER_SUM, false);
                            else if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_CLOUD_OF_ANGER_SUM, false);

                            events.ScheduleEvent(EVENT_CLOUD_OF_ANGER, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_abomination_of_angerAI(creature);
        }
};

// Shadows of Anger 62009
class npc_cfk_shadows_of_anger : public CreatureScript
{
    public:
        npc_cfk_shadows_of_anger() : CreatureScript("npc_cfk_shadows_of_anger") { }

        struct npc_cfk_shadows_of_angerAI : public ScriptedAI
        {
            npc_cfk_shadows_of_angerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->RemoveAurasDueToSpell(SPELL_ANGER_SPAWN_GENERATOR);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_ANGER_SPAWN_GENERATOR);
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_DARK_FIRE:
                        summon->DespawnOrUnsummon(2.5 * IN_MILLISECONDS);
                        break;
                    case NPC_SPAWN_OF_ANGER_S:
                        if (Player* pItr = summon->FindNearestPlayer(60.0f))
                        {
                            summon->Attack(pItr, true);
                            summon->GetMotionMaster()->MoveChase(pItr);
                        }
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_ANGER_SPAWN_GENERATOR);
                DoCast(me, SPELL_SHADOW_EXPLOSION_SUMMON);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_shadows_of_angerAI(creature);
        }
};

// Sha Spirit 65607
// This npc haven`t nearest areatrigger or specific auras, so just check players on dist
class npc_cfk_sha_spirit : public CreatureScript
{
    public:
        npc_cfk_sha_spirit() : CreatureScript("npc_cfk_sha_spirit") { }

        struct npc_cfk_sha_spiritAI : public ScriptedAI
        {
            npc_cfk_sha_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            bool HasTriggered;

            void InitializeAI() override
            {
                me->setActive(true);
                HasTriggered = false;
                me->SetSpeed(MOVE_RUN, 1.69f);
            }

            void UpdateAI(uint32 /*diff*/) override 
            {
                if (me->FindNearestPlayer(8.5f) && !HasTriggered)
                {
                    HasTriggered = true;

                    Movement::MoveSplineInit init(me);
                    for (auto itr : ShaSpiritPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY() + frand(-5.0f, 5.0f), itr.GetPositionZ()));

                    init.SetSmooth();
                    init.SetUncompressed();
                    init.Launch();
                    me->DespawnOrUnsummon(me->GetSplineDuration());
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_sha_spiritAI(creature);
        }
};

// Fire Trap 71489
class npc_cfk_fire_trap : public CreatureScript
{
    public:
        npc_cfk_fire_trap() : CreatureScript("npc_cfk_fire_trap") { }

        enum iEvents
        {
            EVENT_FIRE_TRAP = 1,
        };

        struct npc_cfk_fire_trapAI : public ScriptedAI
        {
            npc_cfk_fire_trapAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.ScheduleEvent(EVENT_FIRE_TRAP, 0.5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_FIRE_TRAP)
                    {
                        DoCast(me, SPELL_TRIGGER_FIRE_TRAP);
                        events.ScheduleEvent(EVENT_FIRE_TRAP, 0.5 * IN_MILLISECONDS);
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_fire_trapAI(creature);
        }
};

// lightning Trap 65590
class npc_cfk_lightning_trap : public CreatureScript
{
    public:
        npc_cfk_lightning_trap() : CreatureScript("npc_cfk_lightning_trap") { }

        enum iEvents
        {
            EVENT_LIGHTNING_TRAP     = 1,
            EVENT_LIGHTNING_TRAP_EFF = 2,
        };

        struct npc_cfk_lightning_trapAI : public ScriptedAI
        {
            npc_cfk_lightning_trapAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool HasProc;
            ObjectGuid DummyGUID;

            void Reset() override
            {
                DummyGUID = ObjectGuid::Empty;
                HasProc   = false;
                events.ScheduleEvent(EVENT_LIGHTNING_TRAP, 0.5 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_LIGHTNING_TRAP)
                {
                    HasProc = true;
                    DoCast(me, SPELL_LIGHTNING_TRAP_BUNNY);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                DummyGUID = summon->GetGUID();
                events.ScheduleEvent(EVENT_LIGHTNING_TRAP_EFF, 1.5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LIGHTNING_TRAP:
                            if (HasProc)
                                break;

                            DoCast(me, SPELL_TRIGGER_LIGHTNING_TRAP);
                            events.ScheduleEvent(EVENT_LIGHTNING_TRAP, 0.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_LIGHTNING_TRAP_EFF:
                            if (Creature* Trap = ObjectAccessor::GetCreature(*me, DummyGUID))
                            {
                                Trap->CastSpell(Trap, SPELL_LIGHTNING_TRAP, false);
                                Trap->DespawnOrUnsummon(1 * IN_MILLISECONDS);
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cfk_lightning_trapAI(creature);
        }
};

// Treasure Urn - 213290
class go_cfk_treasure_urn : public GameObjectScript
{
    public:
        go_cfk_treasure_urn() : GameObjectScript("go_cfk_treasure_urn") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                std::list<Creature*> Quilens;
                GetCreatureListWithEntryInGrid(Quilens, go, NPC_QUILEN_STATUE, 15.0f);

                for (auto&& itr : Quilens)
                {
                    itr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    itr->RemoveAura(SPELL_STONE_FORM);

                    if (Player* m_itr = itr->FindNearestPlayer(50.0f))
                    {
                        itr->Attack(m_itr, true);
                        itr->GetMotionMaster()->MoveChase(m_itr);
                    }
                }
            }

            return false;
        }
};

// Rain of Sha 131641
class spell_cfk_sha_rain : public SpellScriptLoader
{
    public:
        spell_cfk_sha_rain() : SpellScriptLoader("spell_cfk_sha_rain") { }

        class spell_cfk_sha_rain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cfk_sha_rain_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit() && GetHitUnit()->ToCreature())
                    GetHitUnit()->ToCreature()->Kill(GetHitUnit());
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([](WorldObject* target) -> bool
                {
                    if (Creature* creature = target->ToCreature())
                        return creature->GetEntry() != NPC_SHADO_PAN_INITIATE;

                    return true;
                });
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cfk_sha_rain_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cfk_sha_rain_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cfk_sha_rain_SpellScript();
        }
};

// Shadow Step 128681
class spell_cfk_shadow_step : public SpellScriptLoader
{
    public:
        spell_cfk_shadow_step() : SpellScriptLoader("spell_cfk_shadow_step") { }

        class spell_cfk_shadow_step_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cfk_shadow_step_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_TIGER_PALM, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cfk_shadow_step_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cfk_shadow_step_SpellScript();
        }
};

// Trigger Fire Trap 120349
class spell_cfk_trigger_fire_trap : public SpellScriptLoader
{
    public:
        spell_cfk_trigger_fire_trap() : SpellScriptLoader("spell_cfk_trigger_fire_trap") { }

        class spell_cfk_trigger_fire_trap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cfk_trigger_fire_trap_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                if (Unit* target = GetHitUnit())
                {
                    if (GetCaster()->GetExactDist2d(target) > 3.15f || target->HasAura(SPELL_FIRE_TRAP))
                    {
                        PreventHitEffect(effIndex);
                        return;
                    }

                    target->CastSpell(target, SPELL_FIRE_TRAP, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cfk_trigger_fire_trap_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cfk_trigger_fire_trap_SpellScript();
        }
};

// Trigger Lightning Trap 123800
class spell_cfk_trigger_lightning_trap : public SpellScript
{
    PrepareSpellScript(spell_cfk_trigger_lightning_trap);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->Talk(TALK_INTRO, target);
                caster->AI()->DoAction(ACTION_LIGHTNING_TRAP);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_cfk_trigger_lightning_trap::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Anger 120742
class spell_cfk_anger_eff : public SpellScript
{
    PrepareSpellScript(spell_cfk_anger_eff);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Player* target = GetHitPlayer())
                caster->GetMap()->SetWorldState(WORLDSTATE_FANCY_FOORWORK, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_cfk_anger_eff::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Fight with Anger 143316
class spell_achiev_fight_with_anger : public SpellScript
{
    PrepareSpellScript(spell_achiev_fight_with_anger);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (!target->ToPlayer()->HasAura(SPELL_UNCONTROLLED_ANGER) || target->ToPlayer()->HasAura(SPELL_UNCONTROLLED_ANGER) && target->ToPlayer()->GetAura(SPELL_UNCONTROLLED_ANGER) && target->ToPlayer()->GetAura(SPELL_UNCONTROLLED_ANGER)->GetStackAmount() < 20); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_achiev_fight_with_anger::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Hate Periodic 120744
class spell_cfk_hate_periodic : public AuraScript
{
    PrepareAuraScript(spell_cfk_hate_periodic);

    bool Load() override
    {
        hasFirstTrigger = false;
        return true;
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!hasFirstTrigger) // first triggering after 1s cast finishing
        {
            hasFirstTrigger = true;
            PreventDefaultAction();
        }
    }


    private:
        bool hasFirstTrigger;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_cfk_hate_periodic::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// AreaTrigger 8340
class AreaTrigger_at_crypt_behind_abomination : public AreaTriggerScript
{
    public:
        AreaTrigger_at_crypt_behind_abomination() : AreaTriggerScript("AreaTrigger_at_crypt_behind_abomination") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (Creature* Abomination = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_ABOMINATION_OF_ANGER)))
                {
                    if (uint32 abomInit = Abomination->AI()->GetData(TYPE_ABOMINATION_INIT))
                    {
                        if (abomInit < 2)
                        {
                            Abomination->SetVisible(true);
                            Abomination->AI()->SetData(TYPE_ABOMINATION_INIT, ++abomInit);
                            Abomination->AI()->Talk(TALK_INTRO, player);

                            for (auto&& itr : instance->instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1030, DATA_ABOMINATION_OF_ANGER + 1, 0);
                        }
                    }
                }
            }

            return false;
        }
};

// AreaTrigger 7909, 7908, 7916, 7921, 7923
class AreaTrigger_at_crypt_steam_trap : public AreaTriggerScript
{
    public:
        AreaTrigger_at_crypt_steam_trap() : AreaTriggerScript("AreaTrigger_at_crypt_steam_trap") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->CastSpell(player, SPELL_KNOCKBACK_TRAP, true);
            return false;
        }
};

// AreaTrigger 7988, 7987, 7986
class AreaTrigger_at_crypt_quilen_trap : public AreaTriggerScript
{
    public:
        AreaTrigger_at_crypt_quilen_trap() : AreaTriggerScript("AreaTrigger_at_crypt_quilen_trap") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (Creature* Quilen = player->FindNearestCreature(NPC_QUILEN_STATUE, 15.0f, true))
            {
                if (!Quilen->HasAura(SPELL_STONE_FORM))
                    return false;

                Quilen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                Quilen->RemoveAura(SPELL_STONE_FORM);

                Quilen->Attack(player, true);;
                Quilen->GetMotionMaster()->MoveChase(player);
            }

            return false;
        }
};

void AddSC_crypt_of_forgotten_kings()
{
    new npc_cfk_sha_base();
    new npc_cfk_seething_sha();
    new npc_cfk_shado_pan_initiate();
    new npc_cfk_jin_ironfist();
    new npc_cfk_pool_of_life();
    new npc_cfk_crypt_guardian_hall();
    new npc_cfk_crypt_guardian_vault();
    new npc_cfk_abomination_of_anger();
    new npc_cfk_shadows_of_anger();
    new npc_cfk_sha_spirit();
    new npc_cfk_fire_trap();
    new npc_cfk_lightning_trap();
    new go_cfk_treasure_urn();
    new spell_cfk_sha_rain();
    new spell_cfk_shadow_step();
    new spell_cfk_trigger_fire_trap();
    new spell_script<spell_cfk_trigger_lightning_trap>("spell_cfk_trigger_lightning_trap");
    new spell_script<spell_cfk_anger_eff>("spell_cfk_anger_eff");
    new spell_script<spell_achiev_fight_with_anger>("spell_achiev_fight_with_anger");
    new aura_script<spell_cfk_hate_periodic>("spell_cfk_hate_periodic");
    new AreaTrigger_at_crypt_behind_abomination();
    new AreaTrigger_at_crypt_steam_trap();
    new AreaTrigger_at_crypt_quilen_trap();
}