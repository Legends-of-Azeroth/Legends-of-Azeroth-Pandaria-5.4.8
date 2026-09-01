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

#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "heart_of_fear.h"
#include "Vehicle.h"

enum spells
{
    // Amber-Shaper Unsok
    SPELL_MOLTEN_AMBER            = 122004, // triggered 122005 in 1y 
    SPELL_AMBER_SCALPEL           = 121994, // triggered 121995 in 3y
    SPELL_BURNING_AMBER           = 122503, // after living amber death
    SPELL_PARASITIC_GROWTH        = 121949,
    SPELL_RESHAPE_LIFE            = 122370, // polimorph player to abomination
    SPELL_RESHAPE_LIFE_CHANNELED  = 122784,
    SPELL_DESTROY_WILL            = 124824, // only if attack player abomination
    // 3td phase
    SPELL_CONCENTRATED_MUTATION   = 122556, // dmg and attack speed inc by 25%
    SPELL_VOLATILE_AMBER          = 123014,
    SPELL_GROWTH                  = 122547,
    SPELL_GROWTH_DUMMY            = 122551, // visual channeling to pools
    SPELL_AMBER_VOLLEY            = 126939,

    // Player Abomination
    SPELL_AMBER_EXPLOSION         = 122398,
    SPELL_AMBER_STRIKE            = 122389,
    SPELL_DESTABILIZE             = 123059, // must be triggered by amber strike
    SPELL_BREAK_FREE              = 123060, // used at health below 20%
    SPELL_CONSUME_AMBER           = 123156,
    SPELL_CONSUME_AMBER_INC       = 122516,
    SPELL_STRUGGLE_FOR_CONTROLL   = 122395, // used for interupt amber explosion

    // Amber Monstrosity
    SPELL_AMBER_EXPLOSION_M       = 122398,
    SPELL_MASSIVE_STOMP           = 122408, // damage and knockback
    SPELL_FLING                   = 122413, // take and pull active tank
    SPELL_FLING_PULL              = 122420, 
    SPELL_FLING_VISUAL_KICK       = 122419,
    SPELL_FLING_DUMMY             = 122418, // pick up
    SPELL_FLING_DAMAGE            = 122457, // damage and stun after fling
    SPELL_AMBER_CARAPACE          = 122540, // cast to unsok at summon [reduce dmg on 99%]
    SPELL_DESTROY_WILL_M          = 124824, // only if attack player abomination
    SPELL_GRAB                    = 122415, // take active tank in hand
    SPELL_RIDE_VEHICLE            = 43671,  // Scripted by Grab

    // Living amber
    SPELL_CORROSIVE_AURA          = 122348,
    SPELL_BURST                   = 122532,

    // Heroic
    SPELL_AMBER_GLOBULE_DET       = 142191, // encase in amber and + 50% dmg? for players if detonate...
    SPELL_AMBER_GLOBULE_AURA      = 125498, // just visual sphere on npc
    SPELL_AMBERGEDDON             = 125508, // babah!
    SPELL_AMBER_BEACON            = 125502,
    SPELL_SUMMON_GLOBULE          = 142189,
};

enum Events
{
    EVENT_AMBER_SCALPEL           = 1,
    EVENT_LIVING_AMBER            = 2,
    EVENT_PARASITIC_GROWTH        = 3,
    EVENT_RESHAPE_LIFE            = 4,
    EVENT_RESHAPE_LIFE_TO_TARGET  = 5,
    EVENT_AMBER_EXPLOSION_P       = 6,
    EVENT_AMBER_EXPLOSION_M       = 7,
    EVENT_MASSIVE_STOMP           = 8,
    EVENT_FLING                   = 9,
    EVENT_AMBER_VOLLEY            = 10,
    EVENT_COANGULATE_AMBER        = 11,
    EVENT_AMBER_GLOBULE           = 12,
    EVENT_PREPARE_VOLLEY          = 13,
    EVENT_FLING_PULL              = 14,
    EVENT_REMOVE_SCALPER          = 15,
    EVENT_DESTROY_WILL            = 16,
};

enum Actions
{
    ACTION_LIVING_AMBER           = 0,
    ACTION_GREATEST_CREATION      = 1,
    ACTION_UNLEASHED              = 2,
    ACTION_CONSUME_AMBER          = 3,
    ACTION_AMBER_VOLLEY           = 4,
    ACTION_AMBER_VOLLEY_STOP      = 5,
    ACTION_AMBER_GLOBULE          = 6, // Heroic
    ACTION_DRAW_POWER             = 7,
    ACTION_UNSOK_INTRO            = 8,
};

enum Phases
{
    PHASE_1 = 1,
    PHASE_2 = 2,
    PHASE_3 = 3
};

enum Yells
{
    // Unsok
    SAY_INTRO           = 0,
    SAY_AGGRO           = 1,
    SAY_AMBER_SCALPEL   = 2,
    SAY_PARASITIC       = 3,
    SAY_ABOMINATION     = 4,
    SAY_PHASE_2         = 5,
    SAY_PHASE_3         = 6,
    SAY_SLAY            = 7,
    SAY_DEATH           = 8,

    // Amber Monstrosity
    SAY_DEATH_AM        = 0,
    SAY_AMBER_EXPLOSION = 1,
};

class boss_amber_shaper_unsok : public CreatureScript
{
    public:
        boss_amber_shaper_unsok() : CreatureScript("boss_amber_shaper_unsok") { }

        struct boss_amber_shaper_unsokAI : public BossAI
        {
            boss_amber_shaper_unsokAI(Creature* creature) : BossAI(creature, DATA_UNSOK) { }

            std::list<Creature*> LivingAmber;
            std::list<ObjectGuid> protList;
            uint32 _phase, delay;
            ObjectGuid targetGUID;
            bool IntroDone, IsFirstReshape, InShift;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                summons.DespawnAll();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                LivingAmber.clear();
                protList.clear();
                RemoveLivingAmberFromMap();
                _phase         = PHASE_1;
                IsFirstReshape = true;
                InShift        = false;
                SetVolatileAmberPool(ACTION_AMBER_VOLLEY_STOP);
                delay = 0;

                me->GetMap()->SetWorldState(WORLDSTATE_I_HEARD_YOU_LIKE_AMBER, 0);
            }

            void InitializeAI() override
            {
                IntroDone = false;
                SetEquipmentSlots(false, UNSOK_1H_WEAPON, 0, EQUIP_NO_CHANGE);
                Reset();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RESHAPE_LIFE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PARASITIC_GROWTH);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONSUME_AMBER_INC);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMBER_BEACON);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_UNSOK, FAIL);
                    summons.DespawnAll();
                }

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();

            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_UNSOK, FAIL);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_UNSOK, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    who->NearTeleportTo(EncountersEntrance[0].GetPositionX(), EncountersEntrance[0].GetPositionY(), EncountersEntrance[0].GetPositionZ(), EncountersEntrance[0].GetOrientation());
                    return;
                }

                _JustEngagedWith();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(DATA_UNSOK, IN_PROGRESS);
                }

                events.ScheduleEvent(EVENT_AMBER_SCALPEL, 10000);
                events.ScheduleEvent(EVENT_PARASITIC_GROWTH, urand(14000, 17000));
                events.ScheduleEvent(EVENT_RESHAPE_LIFE, 20000);
                Talk(SAY_AGGRO);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat())
                    summon->AI()->DoZoneInCombat();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_UNSOK_PHASE)
                    return _phase;

                return 0;
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_GREATEST_CREATION:
                        Talk(SAY_PHASE_2);
                        me->SummonCreature(NPC_AMBER_MONSTROSITY, AbominationSpawn, TEMPSUMMON_MANUAL_DESPAWN);
                        events.ScheduleEvent(EVENT_AMBER_EXPLOSION_P, urand(10000, 15000));
                        break;
                    case ACTION_UNLEASHED:
                    {
                        Talk(SAY_PHASE_3);
                        _phase = PHASE_3;
                        events.CancelEvent(EVENT_AMBER_SCALPEL);

                        if (Unit* victim = me->GetVictim())
                        {
                            me->PrepareChanneledCast(me->GetAngle(victim));
                            targetGUID = victim->GetGUID();
                        }
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                        Movement::MoveSplineInit init(me);
                        init.MoveTo(UnsokUnleashed.GetPositionX(), UnsokUnleashed.GetPositionY(), UnsokUnleashed.GetPositionZ());
                        init.Launch();
                        delay = 0;
                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 10, [this]()
                        {
                            InShift = false;
                            SetVolatileAmberPool(ACTION_DRAW_POWER);
                            SetVolatileAmberPool(ACTION_AMBER_VOLLEY);

                            if (IsHeroic())
                                SetVolatileAmberPool(ACTION_AMBER_GLOBULE);

                            me->RemoveAurasDueToSpell(SPELL_AMBER_CARAPACE);
                            me->CastSpell(me, SPELL_GROWTH, false);
                            events.ScheduleEvent(EVENT_PREPARE_VOLLEY, 4500);
                            me->AddAura(SPELL_CONCENTRATED_MUTATION, me);

                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                        });
                        break;
                    }
                    case ACTION_UNSOK_INTRO:
                        if (!IntroDone)
                        {
                            IntroDone = true;
                            Talk(SAY_INTRO);
                        }
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override 
            {
                // blizzlike, while casting scalpel - not break phase
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (HealthBelowPct(70) && _phase == PHASE_1)
                {
                    _phase = PHASE_2;
                    me->AI()->DoAction(ACTION_GREATEST_CREATION);
                }
            }

            void KilledUnit(Unit* /*victim*/) override 
            { 
                Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_UNSOK, DONE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }
                SetVolatileAmberPool(ACTION_AMBER_VOLLEY_STOP);

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RESHAPE_LIFE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PARASITIC_GROWTH);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CONSUME_AMBER_INC);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMBER_BEACON);
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
                        case EVENT_AMBER_SCALPEL:
                            Talk(SAY_AMBER_SCALPEL);
                            // We shouldn`t cast any spell that affect dmg on players with mutation (as result we help)
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_RESHAPE_LIFE)))
                            {
                                targetGUID = target->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(target));

                                if (TempSummon* scalpel = target->SummonCreature(NPC_AMBER_SCALPEL, *target, TEMPSUMMON_MANUAL_DESPAWN))
                                    DoCast(scalpel, SPELL_AMBER_SCALPEL);
                            }
                            else if (Unit* target = me->GetVictim())
                            {
                                targetGUID = target->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(target));

                                if (TempSummon* scalpel = target->SummonCreature(NPC_AMBER_SCALPEL, *target, TEMPSUMMON_MANUAL_DESPAWN))
                                    DoCast(scalpel, SPELL_AMBER_SCALPEL);
                            }

                            events.ScheduleEvent(EVENT_REMOVE_SCALPER, 10.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_AMBER_SCALPEL, 45 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_LIVING_AMBER, urand(14000, 15000));
                            break;
                        case EVENT_PARASITIC_GROWTH:
                            if (HasInShift(eventId))
                                break;

                            Talk(SAY_PARASITIC);
                            // We shouldn`t cast any spell that affect dmg on players with mutation (as result we help)
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_RESHAPE_LIFE)))
                                me->CastSpell(target, SPELL_PARASITIC_GROWTH, false);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_PARASITIC_GROWTH, false);

                            events.ScheduleEvent(EVENT_PARASITIC_GROWTH, urand(35000, 50000));
                            break;
                        case EVENT_RESHAPE_LIFE:
                            if (HasInShift(eventId))
                                break;

                            Talk(SAY_ABOMINATION);

                            if (IsFirstReshape)
                            {
                                IsFirstReshape = false;
                                InitializeTanksSequence();
                            }

                            switch (_phase)
                            {
                                case PHASE_1:
                                    if (Unit* pTank = ObjectAccessor::GetUnit(*me, GetReshapeLifeTarget()))
                                        DoCast(pTank, SPELL_RESHAPE_LIFE_CHANNELED);
                                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_RESHAPE_LIFE)))
                                        DoCast(target, SPELL_RESHAPE_LIFE_CHANNELED);
                                    break;
                                case PHASE_2:
                                case PHASE_3:
                                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_RESHAPE_LIFE)))
                                        DoCast(target, SPELL_RESHAPE_LIFE_CHANNELED);
                                    else if (Unit* target = me->GetVictim())
                                        if (!target->HasAura(SPELL_RESHAPE_LIFE))
                                            DoCast(target, SPELL_RESHAPE_LIFE_CHANNELED);
                                    break;
                            }

                            _phase == PHASE_3 ? events.ScheduleEvent(EVENT_RESHAPE_LIFE, 15000) : events.ScheduleEvent(EVENT_RESHAPE_LIFE, 50 * IN_MILLISECONDS);
                            break;
                        case EVENT_PREPARE_VOLLEY:
                        case EVENT_REMOVE_SCALPER:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 175.5f);
            }

            private:
                void RemoveLivingAmberFromMap()
                {
                    std::list<Creature*> LivingAmber;
                    GetCreatureListWithEntryInGrid(LivingAmber, me, NPC_LIVING_AMBER, 200.0f);
                    GetCreatureListWithEntryInGrid(LivingAmber, me, NPC_BURNING_AMBER, 200.0f);

                    for (auto&& itr : LivingAmber)
                        itr->DespawnOrUnsummon();
                }

                void SetVolatileAmberPool(uint32 action)
                {
                    std::list<Creature*> AmberPool;
                    GetCreatureListWithEntryInGrid(AmberPool, me, NPC_AMBER_POOL_STALKER, 200.0f);

                    for (auto&& itr : AmberPool)
                        itr->AI()->DoAction(action);
                }

                struct ReshapeLifeSecondTankPredicate 
                {
                    public:
                        ReshapeLifeSecondTankPredicate(Creature const* me) : _me(me) { }

                        bool operator() (Unit* const& object) const
                        {
                            return object && object->ToPlayer() && (object->ToPlayer()->GetRoleForGroup(object->ToPlayer()->GetTalentSpecialization()) != ROLES_TANK || (_me->GetVictim() && object->GetGUID() == _me->GetVictim()->GetGUID()));
                        }

                    private:
                        Creature const* _me;
                };

                // At first phase encounter using Reshape only for players with prot spec.
                // It makes sequence, that will loop till phase two.
                void InitializeTanksSequence()
                {
                    std::list<Player*> pList;
                    GetPlayerListInGrid(pList, me, 200.0f);
                    pList.remove_if(ReshapeLifeSecondTankPredicate(me));

                    for (auto&& pItr : pList)
                        protList.push_back(pItr->GetGUID());

                    if (Unit* vict = me->GetVictim())
                        protList.push_back(vict->GetGUID());
                }

                ObjectGuid GetReshapeLifeTarget()
                {
                    for (auto it = protList.begin(); it != protList.end(); ++it)
                    {
                        ObjectGuid guid = *it;
                        if (Unit* target = ObjectAccessor::GetUnit(*me, guid))
                        {
                            if (target->IsAlive())
                            {
                                protList.push_back(guid);
                                protList.pop_front();
                                return guid;
                            }
                        }
                    }

                    return ObjectGuid::Empty;
                }

                void DoMeleeAttackIfReady()
                {
                    if (Unit* vict = me->GetVictim())
                    {
                        if (me->isAttackReady() && me->IsWithinMeleeRange(vict))
                        {
                            if (vict->HasAura(SPELL_RESHAPE_LIFE))
                                DoCast(vict, SPELL_DESTROY_WILL);

                            me->AttackerStateUpdate(vict);
                            me->resetAttackTimer();
                        }
                    }
                }

                bool HasInShift(uint32 _eventId)
                {
                    if (InShift)
                    {
                        events.RescheduleEvent(_eventId, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                        return true;
                    }

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_amber_shaper_unsokAI(creature);
        }
};

// Amber monstrosity 62711
class npc_amber_monstrosity : public CreatureScript
{
    public:
        npc_amber_monstrosity() : CreatureScript("npc_amber_monstrosity") { }

        struct npc_amber_monstrosityAI : public ScriptedAI
        {
            npc_amber_monstrosityAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            ObjectGuid targetGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();
                me->SetInCombatWithZone();

                if (Creature* unsok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_UNSOK) : ObjectGuid::Empty))
                    me->CastSpell(unsok, SPELL_AMBER_CARAPACE, false);

                events.ScheduleEvent(EVENT_MASSIVE_STOMP, urand(18000, 25000));
                events.ScheduleEvent(EVENT_AMBER_EXPLOSION_M, 10000);
                events.ScheduleEvent(EVENT_FLING, urand(25000, 40000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH_AM);

                if (Creature* unsok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_UNSOK) : ObjectGuid::Empty))
                    unsok->AI()->DoAction(ACTION_UNLEASHED);
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (target && spell->Id == SPELL_GRAB)
                {
                    if (me->GetThreatManager().getThreat(target))
                        me->GetThreatManager().modifyThreatPercent(target, -100);
                }
            }

            void DoAction(int32 actionId) override 
            {
                if (actionId == ACTION_GREATEST_CREATION)
                    events.ScheduleEvent(EVENT_FLING_PULL, 3 * IN_MILLISECONDS);
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
                        case EVENT_AMBER_EXPLOSION_M:
                            Talk(SAY_AMBER_EXPLOSION);
                            DoCast(me, SPELL_AMBER_EXPLOSION_M);
                            events.ScheduleEvent(EVENT_AMBER_EXPLOSION_M, 46 * IN_MILLISECONDS);
                            break;
                        case EVENT_MASSIVE_STOMP:
                            DoCast(me, SPELL_MASSIVE_STOMP);
                            events.ScheduleEvent(EVENT_MASSIVE_STOMP, urand(15000, 29000));
                            break;
                        case EVENT_FLING:
                            DoCast(me, SPELL_FLING_DUMMY);
                            events.ScheduleEvent(EVENT_FLING, 35000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_monstrosityAI(creature);
        }
};

// Burning amber Stalker: 62512, 62513
class npc_burning_amber_stalker : public CreatureScript
{
    public:
        npc_burning_amber_stalker() : CreatureScript("npc_burning_amber_stalker") { }

        enum iEvents
        {
            EVENT_RISE = 1,
        };

        struct npc_burning_amber_stalkerAI : public ScriptedAI
        {
            npc_burning_amber_stalkerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();

                switch (me->GetEntry())
                {
                    case NPC_MOLTEN_AMBER:
                        DoCast(me, SPELL_MOLTEN_AMBER);
                        me->DespawnOrUnsummon(20000);
                        break;
                    case NPC_BURNING_AMBER:
                        if (Creature* unsok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_UNSOK) : ObjectGuid::Empty))
                            unsok->AI()->JustSummoned(me);

                        DoCast(me, SPELL_BURNING_AMBER);

                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_RISE, 30 * IN_MILLISECONDS);
                        break;
                }
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_LIVING_AMBER:
                        if (Creature* unsok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_UNSOK) : ObjectGuid::Empty))
                            unsok->SummonCreature(NPC_LIVING_AMBER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);

                        me->DespawnOrUnsummon();
                        break;
                    case ACTION_CONSUME_AMBER:
                        me->DespawnOrUnsummon();
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_RISE)
                        DoAction(ACTION_LIVING_AMBER);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_burning_amber_stalkerAI(creature);
        }
};

// Mutated Construct 62701
class npc_mutated_construct_p : public CreatureScript
{
    public:
        npc_mutated_construct_p() : CreatureScript("npc_mutated_construct_p") { }

        struct npc_mutated_construct_pAI : public ScriptedAI
        {
            npc_mutated_construct_pAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            ObjectGuid ownerGUID;
            bool Apply;

            // Using as actor for each player that was affecter by reshaper.
            // Why? Cuz Reshape spell haven`t any periodic or proc effects, but we need smth trigger amber explosion
            void IsSummonedBy(Unit* summoner) override
            {
                ownerGUID = summoner->GetGUID();
                Apply = false;
                events.ScheduleEvent(EVENT_AMBER_EXPLOSION_P, 13 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DESTROY_WILL, 1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AMBER_EXPLOSION_P:
                            if (Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID))
                            {
                                if (owner->IsAlive() && owner->HasAura(SPELL_RESHAPE_LIFE))
                                    owner->CastSpell(owner, SPELL_AMBER_EXPLOSION, false);
                                else
                                    me->DespawnOrUnsummon();
                            }
                            else
                                me->DespawnOrUnsummon();

                            events.ScheduleEvent(EVENT_AMBER_EXPLOSION_P, 13 * IN_MILLISECONDS);
                            break;
                        case EVENT_DESTROY_WILL:
                            if (Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID))
                            {
                                if (owner->IsAlive() && owner->HasAura(SPELL_RESHAPE_LIFE))
                                {
                                    if (!Apply)
                                    {
                                        Apply = true;
                                        owner->SetPower(POWER_ALTERNATE_POWER, 100);
                                    }

                                    if (uint32 wPower = owner->GetPower(POWER_ALTERNATE_POWER))
                                        owner->SetPower(POWER_ALTERNATE_POWER, --wPower);
                                    else
                                        me->Kill(owner);
                                }
                            }
                            events.ScheduleEvent(EVENT_DESTROY_WILL, 1 * IN_MILLISECONDS);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mutated_construct_pAI(creature);
        }
};

// Amber Scalpel 62510
class npc_amber_scalpel : public CreatureScript
{
    public:
        npc_amber_scalpel() : CreatureScript("npc_amber_scalpel") { }

        enum iEvents
        {
            EVENT_RISE_AMBER = 1,
        };

        struct npc_amber_scalpelAI : public ScriptedAI
        {
            npc_amber_scalpelAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            ObjectGuid ownerGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetDisplayId(43164);
                ownerGUID = summoner->GetGUID();
                AttackStart(summoner);
                me->SetInCombatWith(summoner);
                me->GetThreatManager().addThreat(summoner, 300.0f);
                me->GetMotionMaster()->MoveChase(summoner);
                me->DespawnOrUnsummon(11 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_RISE_AMBER, 3 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (Unit* m_owner = ObjectAccessor::GetUnit(*me, ownerGUID))
                {
                    if (!m_owner->IsAlive())
                        me->DespawnOrUnsummon();
                }
                else
                    me->DespawnOrUnsummon();

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_RISE_AMBER)
                    {
                        if (Creature* aStalker = ObjectAccessor::GetCreature(*me, SelectLivingAmberGUID()))
                            aStalker->AI()->DoAction(ACTION_LIVING_AMBER);

                        events.ScheduleEvent(EVENT_RISE_AMBER, 2.5 * IN_MILLISECONDS);
                    }
                    break;
                }
            }

            private:
                ObjectGuid SelectLivingAmberGUID()
                {
                    std::list<Creature*> AmberStalkers;
                    GetCreatureListWithEntryInGrid(AmberStalkers, me, NPC_MOLTEN_AMBER, 150.0f);

                    if (!AmberStalkers.empty())
                        return Trinity::Containers::SelectRandomContainerElement(AmberStalkers)->GetGUID();

                    return ObjectGuid::Empty;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_scalpelAI(creature);
        }
};

// Living amber 62691
class npc_living_amber : public CreatureScript
{
    public:
        npc_living_amber() : CreatureScript("npc_living_amber") { }

        struct npc_living_amberAI : public ScriptedAI
        {
            npc_living_amberAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;

            void IsSummonedBy(Unit* summoner) override 
            {
                me->CastSpell(me, SPELL_CORROSIVE_AURA, false);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                {
                    me->GetThreatManager().addThreat(target, 10000.0f);
                    me->GetMotionMaster()->MoveChase(target, 0, me->GetAngle(target));
                    me->Attack(target, true);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                {
                    me->GetThreatManager().addThreat(target, 10000.0f);
                    me->GetMotionMaster()->MoveChase(target, 0, me->GetAngle(target));
                    me->Attack(target, true);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_BURST);
                Position pos {me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()};
                me->SummonCreature(NPC_BURNING_AMBER, pos, TEMPSUMMON_MANUAL_DESPAWN);
                me->SetCorpseDelay(3); // set delay
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_living_amberAI(creature);
        }
};

// Amber pool stalker 62762
class npc_amber_pool_stalker : public CreatureScript
{
    public:
        npc_amber_pool_stalker() : CreatureScript("npc_amber_pool_stalker") { }

        struct npc_amber_pool_stalkerAI : public ScriptedAI
        {
            npc_amber_pool_stalkerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;
            uint32 delay;

            void InitializeAI() override 
            {
                delay = 0;
                instance = me->GetInstanceScript();
                nonCombatEvents.ScheduleEvent(EVENT_COANGULATE_AMBER, urand(1500, 5500));
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_AMBER_VOLLEY:
                        nonCombatEvents.ScheduleEvent(EVENT_AMBER_VOLLEY, 3000);
                        break;
                    case ACTION_AMBER_VOLLEY_STOP:
                        nonCombatEvents.Reset();

                        if (IsHeroic())
                            RemoveAllGlobules();
                        break;
                    case ACTION_AMBER_GLOBULE:
                        nonCombatEvents.ScheduleEvent(EVENT_AMBER_GLOBULE, urand(10, 29)*IN_MILLISECONDS);
                        break;
                    case ACTION_DRAW_POWER:
                        if (Creature* Unsok = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_UNSOK) : ObjectGuid::Empty))
                            me->CastSpell(Unsok, SPELL_GROWTH_DUMMY, false);
                        break;
                }
            }

            bool HasCosmeticEventDone()
            {
                std::list<Creature*> PoolTenders;
                GetCreatureListWithEntryInGrid(PoolTenders, me, NPC_SRATHIK_POOL_TENDER, 75.0f);

                for (auto&& itr : PoolTenders)
                    if (itr->IsAlive())
                        return false;

                return true;
            }

            void RemoveAllGlobules()
            {
                std::list<Creature*> GlobulesList;
                GetCreatureListWithEntryInGrid(GlobulesList, me, NPC_AMBER_GLOBULE, 200.0f);

                for (auto&& itr : GlobulesList)
                    itr->DespawnOrUnsummon();
            }

            uint32 GetAliveGlobule()
            {
                std::list<Creature*> Globules;
                GetCreatureListWithEntryInGrid(Globules, me, NPC_AMBER_GLOBULE, 200.0f);

                if (Globules.empty())
                    return 0;

                return Globules.size();
            }

            void UpdateAI(uint32 diff) override 
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AMBER_VOLLEY:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                me->CastSpell(target, SPELL_AMBER_VOLLEY, false);

                            me->CastSpell(me, SPELL_VOLATILE_AMBER, true);
                            nonCombatEvents.ScheduleEvent(EVENT_AMBER_VOLLEY, urand(1500, 2000));
                            break;
                        }
                        case EVENT_COANGULATE_AMBER:
                        {
                            if (HasCosmeticEventDone())
                                return;

                            if (TempSummon* amber = me->SummonCreature(NPC_COAGULATED_AMBER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 0.1f, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                float x, y;
                                GetPositionWithDistInOrientation(amber, 34.0f, amber->GetOrientation(), x, y);
                                Movement::MoveSplineInit init(amber);
                                init.MoveTo(x, y, amber->GetPositionZ() + 2.0f);

                                init.SetUncompressed();
                                init.Launch();

                                ObjectGuid amberGUID = amber->GetGUID();
                                delay = 0;
                                me->m_Events.Schedule(delay += amber->GetSplineDuration(), 13, [this, amberGUID]()
                                {
                                    me->StopMoving();

                                    if (Creature* amber = ObjectAccessor::GetCreature(*me, amberGUID))
                                        amber->AI()->DoAction(ACTION_MISC);

                                });
                            }

                            nonCombatEvents.ScheduleEvent(EVENT_COANGULATE_AMBER, urand(8000, 12000));
                            break;
                        }
                        case EVENT_AMBER_GLOBULE:
                        {
                            if (GetAliveGlobule() < 2)
                            {
                                DoCast(me, SPELL_SUMMON_GLOBULE);
                                nonCombatEvents.ScheduleEvent(EVENT_AMBER_GLOBULE, urand(16, 39)*IN_MILLISECONDS);
                            }
                            else
                                nonCombatEvents.RescheduleEvent(EVENT_AMBER_GLOBULE, urand(5, 7)*IN_MILLISECONDS);

                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_pool_stalkerAI(creature);
        }
};

// 73332 - Amber Globule (so, not sure that`s right here, but this should using in Proving Grounds)
class npc_amber_globule : public CreatureScript
{
    public:
        npc_amber_globule() : CreatureScript("npc_amber_globule") { }

        struct npc_amber_globuleAI : public ScriptedAI
        {
            npc_amber_globuleAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            bool phase;
            uint32 switchPhaseTimer;
            float scale;
            ObjectGuid targetGuid;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                if (me->GetMap()->GetId() == 1148)
                {
                    me->DespawnOrUnsummon(10.5 * IN_MILLISECONDS);
                    DoCast(me, 147287);
                    DoCast(me, 147292);
                }
                else
                    me->CastSpell(me, SPELL_AMBER_GLOBULE_AURA, true);

                me->SetSpeed(MOVE_WALK, 0.65f);
                me->SetSpeed(MOVE_RUN, 0.65f);
            }

            void Reset() override
            {
                DoZoneInCombat(me, 400.0f);

                targetGuid = ObjectGuid::Empty;

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                {
                    if (me->GetMap()->GetId() != 1148)
                        DoCast(target, SPELL_AMBER_BEACON, true);

                    me->GetMotionMaster()->MoveChase(target, me->GetMap()->GetId() == 1148 ? 1.0f : 0.0f);
                    targetGuid = target->GetGUID();
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 400.0f, true))
                {
                    if (me->GetMap()->GetId() != 1148)
                        DoCast(target, SPELL_AMBER_BEACON, true);

                    me->GetMotionMaster()->MoveChase(target, me->GetMap()->GetId() == 1148 ? 1.0f : 0.0f);
                    targetGuid = target->GetGUID();
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon();
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_MISC)
                    if (Player* itr = ObjectAccessor::GetPlayer(*me, targetGuid))
                        itr->RemoveAurasDueToSpell(SPELL_AMBER_BEACON);
            }

            void HasAnyGlobuleAround()
            {
                std::list<Creature*> Globules;
                GetCreatureListWithEntryInGrid(Globules, me, NPC_AMBER_GLOBULE, 200.0f);

                for (auto&& itr : Globules)
                {
                    if (itr->GetGUID() != me->GetGUID())
                    {
                        if (me->GetDistance2d(itr) < 3.0f)
                        {
                            DoAction(ACTION_MISC);
                            itr->AI()->DoAction(ACTION_MISC);
                            itr->DespawnOrUnsummon();
                            me->DespawnOrUnsummon();
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (Player* itr = ObjectAccessor::GetPlayer(*me, targetGuid))
                {
                    if (me->GetMap()->GetId() != 1148)
                    {
                        // if we are around player then booom!
                        if (me->GetDistance2d(itr) < 1.0f)
                        {
                            me->CastSpell(me, SPELL_AMBERGEDDON, false);
                            DoCastAOE(SPELL_AMBER_GLOBULE_DET);
                            DoAction(ACTION_MISC);
                            me->DespawnOrUnsummon();
                        }

                        // If we are around another globule, make despawn twice
                        HasAnyGlobuleAround();
                    }

                    me->GetMotionMaster()->MoveChase(itr, me->GetMap()->GetId() == 1148 ? 1.0f : 0.0f);
                }
                else if (me->GetMap()->GetId() != 1148)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                    {
                        DoCast(target, SPELL_AMBER_BEACON, true);
                        me->GetMotionMaster()->MoveChase(target);
                        targetGuid = target->GetGUID();
                    }
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 400.0f, true))
                    {
                        DoCast(target, SPELL_AMBER_BEACON, true);
                        me->GetMotionMaster()->MoveChase(target);
                        targetGuid = target->GetGUID();
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amber_globuleAI(creature);
        }
};

// Amber scalpel 121994
class spell_amber_scalpel : public SpellScriptLoader
{
    public:
        spell_amber_scalpel() : SpellScriptLoader("spell_amber_scalpel") { }

        class spell_amber_scalpel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_amber_scalpel_AuraScript);

            void OnTrigger(AuraEffect const* /*aurEff*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    if (InstanceScript* instance = owner->GetInstanceScript())
                        if (Creature* m_unsok = ObjectAccessor::GetCreature(*owner, instance->GetGuidData(DATA_UNSOK)))
                            m_unsok->SummonCreature(NPC_MOLTEN_AMBER, owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_amber_scalpel_AuraScript::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_amber_scalpel_AuraScript();
        }
};

// Amber strike 122389
class spell_amber_strike : public SpellScript
{
    PrepareSpellScript(spell_amber_strike);

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->GetTarget()))
                if (target->ToPlayer())
                    return SPELL_FAILED_TARGET_FRIENDLY;

        return SPELL_CAST_OK;
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AddAura(SPELL_DESTABILIZE, target);

                // Interrupt amber explosion on hit
                if (target->HasUnitState(UNIT_STATE_CASTING) && target->GetCurrentSpell(CURRENT_GENERIC_SPELL) && target->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_AMBER_EXPLOSION_M)
                {
                    target->RemoveAurasDueToSpell(SPELL_AMBER_EXPLOSION_M);
                    target->InterruptNonMeleeSpells(true, SPELL_AMBER_EXPLOSION_M);
                }
            }
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_amber_strike::CheckCast);
        OnHit += SpellHitFn(spell_amber_strike::HandleOnHit);
    }
};

// Consume amber 123156
class spell_consume_amber : public SpellScriptLoader
{
    public:
        spell_consume_amber() : SpellScriptLoader("spell_consume_amber") { }

        class spell_consume_amber_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_consume_amber_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* caster = GetCaster())
                {
                    if (Creature* BurningAmberStalker = GetClosestCreatureWithEntry(caster, NPC_BURNING_AMBER, 8.0f, true))
                    {
                        BurningAmberStalker->GetAI()->DoAction(ACTION_CONSUME_AMBER);
                        return SPELL_CAST_OK;
                    }
                }

                return SPELL_FAILED_SPELL_UNAVAILABLE;
            }

            void HandleAfterCast()
            {
                if (Unit* owner = GetCaster())
                    owner->CastSpell(owner, SPELL_CONSUME_AMBER_INC, false);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_consume_amber_SpellScript::CheckCast);
                AfterCast += SpellCastFn(spell_consume_amber_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_consume_amber_SpellScript();
        }
};

// Break free 123060
class spell_break_free : public SpellScript
{
    PrepareSpellScript(spell_break_free);

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->HealthAbovePct(20))
                return SPELL_FAILED_NOT_READY;

            // At third phase players into abomination couldn`t use break free
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (Creature* unsok = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(DATA_UNSOK)))
                    if (unsok->AI()->GetData(TYPE_UNSOK_PHASE) == PHASE_3)
                        return SPELL_FAILED_CHARMED;
        }

        return SPELL_CAST_OK;
    }

    void HandleAfterCast()
    {
        if (Unit* owner = GetCaster())
            owner->RemoveAurasDueToSpell(SPELL_RESHAPE_LIFE);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_break_free::CheckCast);
        AfterCast += SpellCastFn(spell_break_free::HandleAfterCast);
    }
};

// Fling 122418
class spell_fling_dummy : public SpellScript
{
    PrepareSpellScript(spell_fling_dummy);

    void HandleHit()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = caster->GetVictim())
            {
                target->CastSpell(caster, SPELL_RIDE_VEHICLE, true);
                caster->CastSpell(caster, SPELL_FLING);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_fling_dummy::HandleHit);
    }
};

// Fling 122413
class spell_fling : public SpellScript
{
    PrepareSpellScript(spell_fling);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                if (Vehicle* vehicle = caster->GetVehicleKit())
                {
                    if (Unit* passenger = vehicle->GetPassenger(0))
                    {
                        passenger->ExitVehicle();
                        passenger->CastSpell(target, SPELL_FLING_PULL);
                    }
                }
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        if (Unit* caster = GetCaster())
            targets.sort(Trinity::ObjectDistanceOrderPred(caster, false));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_fling::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fling::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Fling pull 122420
class spell_fling_jump : public SpellScript
{
    PrepareSpellScript(spell_fling_jump);

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_FLING_DAMAGE, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_fling_jump::HandleOnCast);
    }
};

// Burst 122532
class spell_burst_living_amber : public SpellScriptLoader
{
    public:
        spell_burst_living_amber() : SpellScriptLoader("spell_burst_living_amber") { }

        class spell_burst_living_amber_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_burst_living_amber_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_LIVING_AMBER; });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_burst_living_amber_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_burst_living_amber_SpellScript();
        }
};

// Reshape of Life 122370
class spell_unsok_reshape_of_life : public AuraScript
{
    PrepareAuraScript(spell_unsok_reshape_of_life);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            owner->SummonCreature(NPC_MUTATED_CONSTRUCT, *owner, TEMPSUMMON_MANUAL_DESPAWN);
            owner->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (auto const owner = GetOwner()->ToPlayer())
        {
            owner->RemoveAurasDueToSpell(SPELL_CONSUME_AMBER_INC);

            // Temp hack:: SPELL_AURA_MOD_INCREASE_HEALTH not right calculated at remove
            owner->SetHealth(owner->GetMaxHealth());
            owner->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK1);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_unsok_reshape_of_life::OnApply, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_unsok_reshape_of_life::OnRemove, EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

struct PlayerTargetSelector
{
    bool operator ()(WorldObject const* object) const
    {
        if (object->GetTypeId() == TYPEID_PLAYER)
            return false;
        return true;
    }
};

// Parasitic Growth 121949
class spell_unsok_parasitic_growth : public SpellScriptLoader
{
    public:
        spell_unsok_parasitic_growth() : SpellScriptLoader("spell_unsok_parasitic_growth") { }

        class spell_unsok_parasitic_growth_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_unsok_parasitic_growth_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PlayerTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unsok_parasitic_growth_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_unsok_parasitic_growth_SpellScript();
        }
};

// Amber Globule Detonation 142191
class spell_amber_globule_detonation : public SpellScriptLoader
{
    public:
        spell_amber_globule_detonation() : SpellScriptLoader("spell_amber_globule_detonation") { }

        class spell_amber_globule_detonation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_amber_globule_detonation_SpellScript);

            std::list<WorldObject*> m_targets;

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PlayerTargetSelector());
                m_targets = targets;
            }

            void CopyTargets(std::list<WorldObject*>& targets)
            {
                targets = m_targets;
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_amber_globule_detonation_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_amber_globule_detonation_SpellScript::CopyTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_amber_globule_detonation_SpellScript();
        }
};

// Reshape Life 122784
class spell_reshape_life : public SpellScriptLoader
{
    public:
        spell_reshape_life() : SpellScriptLoader("spell_reshape_life") { }

        class spell_reshape_life_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_reshape_life_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                {
                    if (Unit* caster = GetCaster())
                    {
                        caster->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, owner->GetGUID());
                        caster->CastSpell(owner, SPELL_RESHAPE_LIFE, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_reshape_life_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_reshape_life_AuraScript();
        }
};

// Destroy Will 124824
class spell_destroy_will : public SpellScriptLoader
{
    public:
        spell_destroy_will() : SpellScriptLoader("spell_destroy_will") { }

        class spell_destroy_will_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_destroy_will_SpellScript);

            void HandleOnEffectHit(SpellEffIndex effIdx)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (SpellInfo const* sInfo = GetSpellInfo())
                        {
                            if (uint32 wPower = target->GetPower(POWER_ALTERNATE_POWER))
                            {
                                if ((int32)wPower - sInfo->Effects[EFFECT_0].BasePoints <= 0)
                                    caster->Kill(target);
                                else
                                    target->SetPower(POWER_ALTERNATE_POWER, wPower - sInfo->Effects[EFFECT_0].BasePoints);
                            }
                            else
                                caster->Kill(target);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_destroy_will_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_destroy_will_SpellScript();
        }
};

// Burning Amber Triggering 122503
class spell_unsok_burning_amber_triggering : public AuraScript
{
    PrepareAuraScript(spell_unsok_burning_amber_triggering);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit()) // so, targeting possessed not implemented - we can check near pools by this way
        {
            if (owner->GetMap()->GetWorldState(WORLDSTATE_I_HEARD_YOU_LIKE_AMBER) == 1)
                return;

            std::list<Creature*> livingAmbers;
            GetCreatureListWithEntryInGrid(livingAmbers, owner, NPC_BURNING_AMBER, 3.0f);

            livingAmbers.remove_if([=](Creature* target) { return target && target->GetGUID() == owner->GetGUID(); });

            // required 8 pools for achiev allowing
            if (livingAmbers.size() > 6 && owner->GetMap()->GetWorldState(WORLDSTATE_I_HEARD_YOU_LIKE_AMBER) != 1)
                owner->GetMap()->SetWorldState(WORLDSTATE_I_HEARD_YOU_LIKE_AMBER, 1);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_unsok_burning_amber_triggering::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// AreaTrigger 8238
class AreaTrigger_at_behind_unsok : public AreaTriggerScript
{
    public:
        AreaTrigger_at_behind_unsok() : AreaTriggerScript("AreaTrigger_at_behind_unsok") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* Unsok = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_UNSOK)))
                    Unsok->AI()->DoAction(ACTION_UNSOK_INTRO);

            return false;
        }
};

void AddSC_boss_unsok()
{
    new boss_amber_shaper_unsok();
    new npc_amber_monstrosity();
    new npc_burning_amber_stalker();
    new npc_mutated_construct_p();
    new npc_amber_scalpel();
    new npc_living_amber();
    new npc_amber_pool_stalker();
    new npc_amber_globule();
    new spell_amber_scalpel();
    new spell_script<spell_amber_strike>("spell_amber_strike");
    new spell_consume_amber();
    new spell_script<spell_break_free>("spell_break_free");
    new spell_script<spell_fling_dummy>("spell_fling_dummy");
    new spell_script<spell_fling>("spell_fling");
    new spell_script<spell_fling_jump>("spell_fling_jump");
    new spell_burst_living_amber();
    new spell_unsok_reshape_of_life();
    new aura_script<spell_unsok_reshape_of_life>("spell_unsok_reshape_of_life");
    new spell_unsok_parasitic_growth();
    new spell_amber_globule_detonation();
    new spell_reshape_life();
    new spell_destroy_will();
    new aura_script<spell_unsok_burning_amber_triggering>("spell_unsok_burning_amber_triggering");
    new AreaTrigger_at_behind_unsok();
}
