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

#include "throne_of_thunder.h"
#include "CreatureTextMgr.h"
#include "Pet.h"
#include <array>
#include "Random.h"

enum Spells
{
    SPELL_PRIMORDIUS_INTRO_COSMETIC         = 137064,
    SPELL_SUBMERGE_COSMETIC                 = 140483,
    SPELL_EMERGE                            = 139832,

    SPELL_PRIMORDIAL_STRIKE                 = 136037,   // Cast on main target next melee
    SPELL_MALFORMED_BLOOD                   = 136050,   // Stacking dot, applied to tank
    SPELL_CONGEAL_BLOOD                     = 136051,   // Proc Eff
    SPELL_EVOLUTION                         = 136209,   // Boss gains damage each time he evolves
    SPELL_MUTATED_ABOMINATION               = 140544,   // Boss aura

    SPELL_BERSERK_PRIMORDIUS                = 47008,
    SPELL_ZERO_POWER                        = 72242,

    SPELL_VENTRAL_SACS                      = 136210,   // Applies ventral sacs aura
    SPELL_VENTRAL_SACS_DAMAGE               = 136211,

    SPELL_GAS_BLADDER                       = 136215,   // Enables caustic gas
    SPELL_CAUSTIC_GAS                       = 136216,   // Split between all targets hit, 800k damage

    SPELL_ACIDIC_SPINES                     = 136218,   // Triggers missile aoe on random players
    SPELL_ACIDIC_EXPLOSION_SELECTOR         = 136856,   // Target selection spell, basepoints is trigger missile
    SPELL_ACIDIC_SPINES_MISSILE             = 136219,
    SPELL_ACIDIC_SPINES_DAMAGE              = 136220,

    SPELL_PATHOGEN_GLANDS                   = 136225,   // Enables volatile pathogen
    SPELL_VOLATILE_PATHOGEN                 = 136228,   // DoT spell, needs explicit target
    SPELL_VOLATILE_PATHOGEN_DAMAGE          = 136231,

    SPELL_METABOLIC_BOOST                   = 136245,   // Mods spell cooldown, timers need to be explicitly checked here (possibly through event function)

    SPELL_ERUPTING_PUSTULES                 = 136246,
    SPELL_ERUPTING_PUSTULES_SELECTOR        = 140499, // for caster group
    SPELL_ERUPTING_PUSTULES_MISSLE          = 136248,
    SPELL_PUSTULE_ERUPTION                  = 140499,   // Script effect for targeted pustule eruption
    SPELL_PUSTULE_ERUPTION_RAND             = 136249,
    SPELL_PUSTULE_ERUPTION_PLAYER           = 136248,
    SPELL_PUSTULE_ERUPTION_DAMAGE           = 136247,


    // Add / player spells
    SPELL_VOLATILE_POOL                     = 140506,   // Area trigger aura on pools
    SPELL_VOLATILE_MUTATION                 = 140508,   // Volatile mutation spell, probably from pools
    SPELL_VOLATILE_MUTATION_PRIMORDIUS      = 140509,   // Volatile mutation  on primordius
    SPELL_VOLATILE_MUTATION_AURA            = 141094,   // Aura added on players from area trigger
    
    SPELL_MUTAGENIC_POOL                    = 136049,   // Create areatrigger
    SPELL_MUTATION                          = 136178,   // Trigger

    SPELL_MUTATE_PRIMORDIUS                 = 136203,   // Living fluid

    SPELL_KEEN_EYESIGHT                     = 136180,   // Crit increase
    SPELL_IMPAIRED_EYESIGHT                 = 136181,   // Crit decrease
    SPELL_IMPROVED_SYNAPSES                 = 136182,   // Haste increase
    SPELL_DULLED_SYNAPSES                   = 136183,   // Haste decrease
    SPELL_THICK_BONES                       = 136184,   // Stats increase
    SPELL_FRAGILE_BONES                     = 136185,   // Stats reduce
    SPELL_CLEAR_MIND                        = 136186,   // Mastery increase
    SPELL_CLOUDED_MIND                      = 136187,   // Mastery decrease

    SPELL_FULLY_MUTATED                     = 140546,   // Stacks of any combination
    SPELL_FULLY_MUTATED_PET                 = 140706,

    /*Heroic*/
    SPELL_BLACK_BLOOD                       = 137000,
    SPELL_BLACK_BLOOD_EFF                   = 136999,
    SPELL_DEADLY_MUTAGEN                    = 136995,
    SPELL_PROXIMITY_CHECK_AURA              = 137051,
    SPELL_PROXIMITY_CHECK_EFF               = 137052,
};

const std::vector<uint32> possitiveEff=
{
    SPELL_KEEN_EYESIGHT,
    SPELL_IMPROVED_SYNAPSES,
    SPELL_THICK_BONES,
    SPELL_CLEAR_MIND,
};

const std::vector<uint32> negativeEff =
{
    SPELL_IMPAIRED_EYESIGHT,
    SPELL_DULLED_SYNAPSES,
    SPELL_FRAGILE_BONES,
    SPELL_CLOUDED_MIND,
};

enum Events
{
    EVENT_EMPTY,
    EVENT_RELOCATE,
    EVENT_SUBMERGE,

    EVENT_INTRO_1,
    EVENT_INTRO_2,
    EVENT_INTRO_3,
    EVENT_INTRO_4,

    EVENT_LIVING_FLUID,
    EVENT_PRIMORDIAL_STRIKE,
    EVENT_MALFORMED_BLOOD,
    EVENT_GAIN_ENERGY,
    EVENT_GAS_BLADDER,
    EVENT_PATHOGEN_GLANDS,
    EVENT_BERSERK,

    EVENT_MOVE,
    EVENT_CHECK_PROXIMITY,
    EVENT_VISCOUS_HORROR,
    EVENT_BLACK_BLOOD,
};

enum Talks
{
    TALK_INTRO_1,
    TALK_INTRO_2,
    TALK_INTRO_3,

    TALK_AGGRO,
    TALK_EVOLVE,
    TALK_DEATH,

    EMOTE_VENTRAL_SACS,
    EMOTE_GAS_BLADDER,
    EMOTE_ACIDIC_SPINES,
    EMOTE_PATHOGEN_GLANDS,
    EMOTE_METABOLIC_BOOST,
    EMOTE_ERUPTING_PUSTULES
};

const int32 ACTION_ACTIVATE_PRIMORDIUS = 32;

static const Position aFluidPos[10] =
{
    { 5660.9f, 4656.5f, 55.5f, 3.1f },
    { 5650.3f, 4689.8f, 55.5f, 3.6f },
    { 5625.5f, 4713.5f, 55.5f, 4.1f },
    { 5558.9f, 4714.1f, 55.5f, 5.2f },
    { 5535.1f, 4689.2f, 55.5f, 5.8f },
    { 5525.8f, 4656.1f, 55.5f, 0.02f},
    { 5534.9f, 4622.8f, 55.5f, 0.5f },
    { 5558.4f, 4597.4f, 55.5f, 1.0f },
    { 5626.2f, 4597.7f, 55.5f, 2.1f },
    { 5650.9f, 4622.4f, 55.5f, 2.6f }
};

static const std::pair<uint32, uint32> m_pMutations[4] = 
{
    { SPELL_KEEN_EYESIGHT, SPELL_IMPAIRED_EYESIGHT },
    { SPELL_IMPROVED_SYNAPSES, SPELL_DULLED_SYNAPSES },
    { SPELL_THICK_BONES, SPELL_FRAGILE_BONES },
    { SPELL_CLEAR_MIND, SPELL_CLOUDED_MIND }
};

static const uint32 m_auiEvolutions[6] =
{
    SPELL_VENTRAL_SACS,
    SPELL_ACIDIC_SPINES,
    SPELL_ERUPTING_PUSTULES,
    SPELL_METABOLIC_BOOST,
    SPELL_PATHOGEN_GLANDS,
    SPELL_GAS_BLADDER
};

const std::map<uint32, std::array<uint32, 3>> invEvolveType =
{
    { SPELL_VENTRAL_SACS, { 0, 0, EMOTE_VENTRAL_SACS } },
    { SPELL_ACIDIC_SPINES, { 0, 0, EMOTE_ACIDIC_SPINES } },
    { SPELL_ERUPTING_PUSTULES, { 0, 0, EMOTE_ERUPTING_PUSTULES } },
    { SPELL_METABOLIC_BOOST, { 0, 0, EMOTE_METABOLIC_BOOST } },
    { SPELL_PATHOGEN_GLANDS, { EVENT_PATHOGEN_GLANDS , 5000, EMOTE_PATHOGEN_GLANDS} }, // grant ability
    { SPELL_GAS_BLADDER, { EVENT_GAS_BLADDER, 10000, EMOTE_GAS_BLADDER} }, // grant ability
};

enum _displays
{
    DISPLAYID_INVIS = 11686
};

static inline const uint32 GetTotalMutations(Unit* unit)
{
    uint32 m_uiMutations = 0;
    Aura* aura;

    for (uint8 i = 0; i < 4; ++i)
    {
        if (aura = unit->GetAura(m_pMutations[i].first))
            m_uiMutations += aura->GetStackAmount();
    }

    return m_uiMutations;
};

class boss_primordius : public CreatureScript
{
    public:
        boss_primordius() : CreatureScript("boss_primordius") { }

        struct boss_primordiusAI : public BossAI
        {
            boss_primordiusAI(Creature* creature) : BossAI(creature, DATA_PRIMORDIUS)
            {
                m_uiPosition = 10;
                introDone = false;
            }

            bool m_bDouble;
            bool introDone;
            bool hasInGasBladder;
            uint32 m_uiPosition;
            std::list<uint32> evolveList;
            EventMap berserkEvents;

            void Reset() override
            {
                me->SetPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, 60);
                me->SetPower(POWER_MANA, 0);
                me->setRegeneratingMana(false);
                evolveList.clear();

                if (instance)
                {
                    instance->SetBossState(DATA_PRIMORDIUS, NOT_STARTED);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                events.Reset();
                berserkEvents.Reset();

                summons.DespawnAll();

                me->GetMap()->SetWorldState(WORLDSTATE_GEN_UNMODIFIED_ORGANISM, 1);

                m_bDouble = false;
                hasInGasBladder = false;
                DoCast(me, SPELL_MUTATED_ABOMINATION, true);

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if (introDone || instance && instance->GetData(DATA_PRIMORDIUS_EVENT) == DONE)
                            break;

                        introDone = true;

                        if (instance)
                            instance->SetData(DATA_PRIMORDIUS_EVENT, DONE);

                        Talk(TALK_INTRO_1);

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_PRIMORDIUS) != IN_PROGRESS; })
                            .Schedule(Seconds(7), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO_2);
                        });

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_PRIMORDIUS) != IN_PROGRESS; })
                            .Schedule(Seconds(17), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO_3);
                        });
                        break;
                    case ACTION_EVOLVE:
                        Talk(TALK_EVOLVE);
                        me->SetPower(POWER_MANA, 0);
                        // Update Container exclude using elements
                        std::list<uint32> evolveDef = { SPELL_VENTRAL_SACS, SPELL_ACIDIC_SPINES, SPELL_ERUPTING_PUSTULES, SPELL_METABOLIC_BOOST, SPELL_PATHOGEN_GLANDS, SPELL_GAS_BLADDER };

                        evolveDef.remove_if([=](uint32 evolveId) { return std::find(evolveList.begin(), evolveList.end(), evolveId) != evolveList.end(); });

                        // Remove Last One if have with same eff
                        uint32 evolveSize = IsHeroic() ? 3 : 2;
                        if (evolveList.size() > evolveSize)
                        {
                            if (invEvolveType.find(evolveList.front())->second[0])
                                events.CancelEvent(invEvolveType.find(evolveList.front())->second[0]);

                            me->RemoveAurasDueToSpell(invEvolveType.find(evolveList.front())->first);

                            evolveList.pop_front();
                        }

                        // Take new one, that we haven`t last 4-5 times depend of difficulty
                        uint32 prevSpellId = Trinity::Containers::SelectRandomContainerElement(evolveDef);
                        evolveList.push_back(prevSpellId);

                        DoCast(me, prevSpellId, true);

                        Talk(invEvolveType.find(prevSpellId)->second[2]);

                        // Schedule events if it possible (some evolve spells just periodic aura with smth eff)
                        if (invEvolveType.find(prevSpellId)->second[0])
                            events.ScheduleEvent(invEvolveType.find(prevSpellId)->second[0], invEvolveType.find(prevSpellId)->second[1]);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_LIVING_FLUID, 3000);
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 8 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_GAIN_ENERGY, 800);

                events.ScheduleEvent(EVENT_PRIMORDIAL_STRIKE, 10000 + rand() % 5000);
                events.ScheduleEvent(EVENT_MALFORMED_BLOOD, 15 * IN_MILLISECONDS);

                if (IsHeroic())
                {
                    DoCast(me, SPELL_PROXIMITY_CHECK_AURA, true);
                    events.ScheduleEvent(EVENT_VISCOUS_HORROR, 11 * IN_MILLISECONDS);
                }

                me->RemoveAurasDueToSpell(SPELL_SUBMERGE_COSMETIC);

                Talk(TALK_AGGRO);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MALFORMED_BLOOD);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOLATILE_PATHOGEN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FULLY_MUTATED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLACK_BLOOD);

                    for (auto itr : possitiveEff)
                        instance->DoRemoveAurasDueToSpellOnPlayers(itr);

                    for (auto&& itr : negativeEff)
                        instance->DoRemoveAurasDueToSpellOnPlayers(itr);
                }

                berserkEvents.Reset();
                Talk(TALK_DEATH);
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_PRIMORDIUS, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MALFORMED_BLOOD);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOLATILE_PATHOGEN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FULLY_MUTATED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLACK_BLOOD);

                    for (auto itr : possitiveEff)
                        instance->DoRemoveAurasDueToSpellOnPlayers(itr);

                    for (auto&& itr : negativeEff)
                        instance->DoRemoveAurasDueToSpellOnPlayers(itr);
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_BERSERK)
                        DoCast(me, SPELL_BERSERK_PRIMORDIUS, true);
                    break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || hasInGasBladder)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PRIMORDIAL_STRIKE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_PRIMORDIAL_STRIKE);

                            events.ScheduleEvent(EVENT_PRIMORDIAL_STRIKE, (20000 + rand() % 5000) * GetEventTimer());
                            break;
                        case EVENT_MALFORMED_BLOOD:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_MALFORMED_BLOOD);

                            events.ScheduleEvent(EVENT_MALFORMED_BLOOD, 15 * IN_MILLISECONDS * GetEventTimer());
                            break;
                        case EVENT_GAS_BLADDER:
                            DoCast(SPELL_CAUSTIC_GAS);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            hasInGasBladder = true;

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_PRIMORDIUS) == IN_PROGRESS; })
                                .Schedule(Milliseconds(3500), [this](TaskContext context)
                            {
                                hasInGasBladder = false;
                            });
                            events.ScheduleEvent(EVENT_GAS_BLADDER, (12000 + rand() % 5000) * GetEventTimer());
                            break;
                        case EVENT_PATHOGEN_GLANDS:
                            if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_VOLATILE_PATHOGEN)))
                                DoCast(unit, SPELL_VOLATILE_PATHOGEN);
                            else if (Unit* unit = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                                DoCast(unit, SPELL_VOLATILE_PATHOGEN);

                            events.ScheduleEvent(EVENT_PATHOGEN_GLANDS, 25000 * GetEventTimer());
                            break;
                        case EVENT_GAIN_ENERGY:
                            me->SetPower(POWER_MANA, me->GetPower(POWER_MANA) + 1);

                            if (me->GetPower(POWER_MANA) >= 60)
                            {
                                DoCast(me, SPELL_EVOLUTION);
                                events.ScheduleEvent(EVENT_GAIN_ENERGY, m_bDouble ? 800 + 3500 : 1500 + 3500);
                                m_bDouble = !m_bDouble;
                                break;
                            }

                            events.ScheduleEvent(EVENT_GAIN_ENERGY, m_bDouble ? 800 : 1500);
                            m_bDouble = !m_bDouble;
                            break;
                        case EVENT_LIVING_FLUID:
                            SummonLivingFluid();
                            break;
                        case EVENT_VISCOUS_HORROR:
                            me->SummonCreature(NPC_VISCOUS_HORROR, aFluidPos[urand(0, 9)], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000 + rand() % 2000);
                            events.ScheduleEvent(EVENT_VISCOUS_HORROR, 30 * IN_MILLISECONDS);
                            break;
                    }
                }
                
                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

            void SummonLivingFluid()
            {
                for (uint8 i = 0; i < 10; ++i)
                    me->SummonCreature(NPC_LIVING_FLUID, aFluidPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000 + rand() % 2000);

                events.ScheduleEvent(EVENT_LIVING_FLUID, 14000);
            }

            float GetEventTimer() const
            {
                if (me->HasAura(SPELL_METABOLIC_BOOST))
                    return 0.8f;

                return 1.f;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_primordiusAI(creature);
        }
};

// Living Fluid 69069
class npc_living_fluid : public CreatureScript
{
    public:
        npc_living_fluid() : CreatureScript("npc_living_fluid") { }

        struct npc_living_fluidAI : public ScriptedAI
        {
            npc_living_fluidAI(Creature* creature) : ScriptedAI(creature) { }

            bool m_bIsVolatile;
            bool m_bHasDied;
            EventMap events;
            TaskScheduler scheduler;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();

                me->SetReactState(REACT_PASSIVE);

                events.ScheduleEvent(EVENT_MOVE, 1000);
                events.ScheduleEvent(EVENT_CHECK_PROXIMITY, 1500);

                DecideVolatility();

                m_bHasDied = false;

                me->SetWalk(true);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && m_bIsVolatile)
                {
                    if (m_bHasDied)
                    {
                        damage = 0;
                        return;
                    }
                    else
                    {
                        damage = 0;

                        me->SetWalk(false);
                        me->SetSpeed(MOVE_RUN, 0.1f, true);
                        me->SetSpeed(MOVE_WALK, 0.1f, true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(me, SPELL_VOLATILE_POOL);
                        me->ClearUnitState(UNIT_STATE_CASTING);

                        me->StopMoving();
                        me->GetMotionMaster()->MoveIdle();

                        events.Reset();

                        m_bHasDied = true;

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_PRIMORDIUS) == IN_PROGRESS; })
                            .Schedule(Seconds(1), [this](TaskContext context)
                        {
                            if (Creature* primordius = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_PRIMORDIUS) : ObjectGuid::Empty))
                                me->GetMotionMaster()->MoveFollow(primordius, 0.0f, me->GetAngle(primordius));

                            context.Repeat(Seconds(1));
                        });

                        me->SetDisplayId(DISPLAYID_INVIS);
                        me->UpdateObjectVisibility();
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* primordius = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_PRIMORDIUS) : ObjectGuid::Empty))
                    if (Creature* pStalker = primordius->SummonCreature(NPC_AT_STALKER, me->GetPositionX(), me->GetPositionY(), 55.8f, TEMPSUMMON_MANUAL_DESPAWN))
                        pStalker->CastSpell(pStalker, SPELL_MUTAGENIC_POOL, true);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                scheduler.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_MOVE:
                        Move();
                        break;
                    case EVENT_CHECK_PROXIMITY:
                        CheckProximity();
                        break;
                }
            }

            void Move()
            {
                if (Creature* primordius = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_PRIMORDIUS) : ObjectGuid::Empty))
                    me->GetMotionMaster()->MovePoint(4, *primordius);

                events.ScheduleEvent(EVENT_MOVE, 200);
            }

            void CheckProximity()
            {
                if (Creature* primordius = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_PRIMORDIUS) : ObjectGuid::Empty))
                {
                    if (me->GetExactDist2d(primordius) < 5.1f)
                    {
                        me->StopMoving();
                        me->GetMotionMaster()->MoveIdle();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(SPELL_MUTATE_PRIMORDIUS);
                        me->DespawnOrUnsummon(200);
                        return;
                    }
                }

                events.ScheduleEvent(EVENT_CHECK_PROXIMITY, 300);
            }

            void DecideVolatility()
            {
                if (roll_chance_i(IsHeroic() ? 10 : 8))
                    m_bIsVolatile = true;
                else
                    m_bIsVolatile = false;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_living_fluidAI(creature);
        }
};

// Primordius AT Stalker 69081
struct npc_primordius_at_stalker : public ScriptedAI
{
    npc_primordius_at_stalker(Creature* creature) : ScriptedAI(creature) { }

    uint32 usingCount;

    void IsSummonedBy(Unit* summoner) override
    {
        usingCount = Is25ManRaid() ? 3 : 1;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_LIVING_FLUID)
            return usingCount;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_LIVING_FLUID)
            usingCount = data;
    }
};

// Viscous Horror 69070
struct npc_primordius_viscous_horror : public ScriptedAI
{
    npc_primordius_viscous_horror(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetInCombatWithZone();
        events.ScheduleEvent(EVENT_BLACK_BLOOD, 15 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_BLACK_BLOOD)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_BLACK_BLOOD);

                events.ScheduleEvent(EVENT_BLACK_BLOOD, 15 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class hasAuraPredicate
{
    private:
        Unit* unit;
    public:
        hasAuraPredicate(Unit* _pUnit) : unit(_pUnit) { }

        bool operator()(const uint32 mValue) const
        {
            return unit->HasAura(mValue);
        }
};

// Evolve 136209
class spell_evolve_primordius : public SpellScript
{
    PrepareSpellScript(spell_evolve_primordius);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_EVOLVE);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_evolve_primordius::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Acidic Explosion 136856
class spell_acidic_explosion_selector : public SpellScript
{
    PrepareSpellScript(spell_acidic_explosion_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_ACIDIC_SPINES_MISSILE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 7 : 3;

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // if we have enough players without tanks, meelee
            if (pList.size() >= targetCount)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                return;
            }

            // if we haven`t enough players
            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_acidic_explosion_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_acidic_explosion_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Caustic Gas 136216
class spell_caustic_gas : public SpellScript
{
    PrepareSpellScript(spell_caustic_gas);

    uint32 targetCount;

    bool Load() override
    {
        targetCount = 1;
        return true;
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->IsGameMaster(); });
        targetCount = targets.size();
    }

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (targetCount > 1)
            SetHitDamage(GetHitDamage() / targetCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_caustic_gas::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_caustic_gas::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

class spell_mutation_primordius : public SpellScriptLoader
{
    public:
        spell_mutation_primordius() : SpellScriptLoader("spell_mutation_primordius") { }

        class spell_mutation_primordius_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mutation_primordius_SpellScript);

            void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
            {
                if (Unit* unit = GetHitUnit())
                {
                    if (unit->HasAura(SPELL_FULLY_MUTATED))
                    {
                        unit->GetMap()->SetWorldState(WORLDSTATE_GEN_UNMODIFIED_ORGANISM, 0);
                        uint32 m_uiInfection = m_pMutations[urand(0, 3)].second;

                        if (Aura* aura = unit->GetAura(m_uiInfection))
                            aura->ModStackAmount(1);
                        else
                            unit->AddAura(m_uiInfection, unit);
                    }
                    else
                    {
                        const std::pair<uint32, uint32> m_pMutationPair = m_pMutations[urand(0, 3)];
                        const uint32 m_uiTotalStacks = GetTotalMutations(unit);

                        if (Aura* aura = unit->GetAura(m_pMutationPair.first))
                        {
                            if (aura->GetStackAmount() >= 4)
                            {
                                aura->Remove(AURA_REMOVE_BY_ENEMY_SPELL);

                                if (Aura* pNegAura = unit->GetAura(m_pMutationPair.second))
                                    pNegAura->ModStackAmount(1);
                                else
                                    unit->AddAura(m_pMutationPair.second, unit);
                            }
                            else
                                aura->ModStackAmount(1);
                        }
                        else
                            unit->AddAura(m_pMutationPair.first, unit);

                        if (m_uiTotalStacks + 1 >= 5)
                        {
                            for (uint8 i = 0; i < 4; ++i)
                            {
                                if (Aura* aura = unit->GetAura(m_pMutations[i].first))
                                    aura->RefreshDuration();
                            }

                            unit->CastSpell(unit, SPELL_FULLY_MUTATED, true);

                            if (Pet* pet = unit->ToPlayer()->GetPet())
                                unit->AddAura(SPELL_FULLY_MUTATED_PET, pet);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mutation_primordius_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mutation_primordius_SpellScript();
        }
};

class spell_primordius_mutation_aura : public SpellScriptLoader
{
    public:
        spell_primordius_mutation_aura() : SpellScriptLoader("spell_primordius_mutation_aura") { }

        class spell_primordius_mutation_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_primordius_mutation_aura_AuraScript);

            void HandleOnDispel(DispelInfo* /*info*/)
            {
                for (uint8 i = 0; i < 4; ++i)
                {
                    uint32 m_uiMutation = m_pMutations[i].first;
                    uint32 m_uiInfection = m_pMutations[i].second;

                    if (GetId() != m_uiMutation)
                        GetOwner()->ToUnit()->RemoveAura(m_uiMutation);

                    if (GetId() != m_uiInfection)
                        GetOwner()->ToUnit()->RemoveAura(m_uiInfection);
                }
            }

            void Register() override
            {
                OnDispel += AuraDispelFn(spell_primordius_mutation_aura_AuraScript::HandleOnDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_primordius_mutation_aura_AuraScript();
        }
};

// 140483
class spell_submerge_cosmetic_aura : public SpellScriptLoader
{
    public:
        spell_submerge_cosmetic_aura() : SpellScriptLoader("spell_submerge_cosmetic_aura") { }

        class spell_submerge_cosmetic_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_submerge_cosmetic_aura_AuraScript);

            void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetOwner()->ToUnit())
                {
                    target->SetDisplayId(DISPLAYID_INVIS);
                    target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetOwner()->ToUnit())
                {
                    target->SetDisplayId(target->GetNativeDisplayId());
                    target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_submerge_cosmetic_aura_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_submerge_cosmetic_aura_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_submerge_cosmetic_aura_AuraScript();
        }
};

// Malformed Blood 136050
class spell_malformed_blood : public AuraScript
{
    PrepareAuraScript(spell_malformed_blood);

    bool CheckProc(ProcEventInfo& rProcInfo)
    {
        if (!rProcInfo.GetProcTarget())
            return false;

        uint32 entry = rProcInfo.GetProcTarget()->GetEntry();
        if (entry != NPC_LIVING_FLUID && entry != NPC_VISCOUS_HORROR)
            return false;

        return true;
    }

    void HandleOnProc(ProcEventInfo& rProcInfo)
    {
        if (Unit* owner = rProcInfo.GetActor())
        {
            if (Unit* target = rProcInfo.GetProcTarget())
            {
                int32 bp = GetStackAmount() * GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                owner->CastCustomSpell(target, SPELL_CONGEAL_BLOOD, &bp, 0, 0, true);
            }
        }
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_malformed_blood::CheckProc);
        OnProc += AuraProcFn(spell_malformed_blood::HandleOnProc);
    }
};

// Black Blood 137000
class spell_primordius_black_blood : public AuraScript
{
    PrepareAuraScript(spell_primordius_black_blood);

    bool CheckProc(ProcEventInfo& rProcInfo)
    {
        if (!rProcInfo.GetProcTarget())
            return false;

        uint32 entry = rProcInfo.GetProcTarget()->GetEntry();
        if (entry != NPC_PRIMORDIUS)
            return false;

        return true;
    }

    void HandleOnProc(ProcEventInfo& rProcInfo)
    {
        if (Unit* owner = rProcInfo.GetActor())
        {
            if (Unit* target = rProcInfo.GetProcTarget())
            {
                int32 bp = GetStackAmount() * GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                owner->CastCustomSpell(target, SPELL_BLACK_BLOOD_EFF, &bp, 0, 0, true);
            }
        }
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_primordius_black_blood::CheckProc);
        OnProc += AuraProcFn(spell_primordius_black_blood::HandleOnProc);
    }
};

// Erupring Pustules Selector 140499
class spell_erupting_pustules_selector : public SpellScript
{
    PrepareSpellScript(spell_erupting_pustules_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_ERUPTING_PUSTULES_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 7 : 3;

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // if we have enough players without tanks, meelee
            if (pList.size() >= targetCount)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                return;
            }

            // if we haven`t enough players
            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_erupting_pustules_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_erupting_pustules_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Mutation 136178
class spell_primordius_player_mutation : public SpellScript
{
    PrepareSpellScript(spell_primordius_player_mutation);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (hasEnoughPossitiveEff(target))
                target->CastSpell(target, Trinity::Containers::SelectRandomContainerElement(negativeEff), true);
            else
            {
                target->CastSpell(target, Trinity::Containers::SelectRandomContainerElement(possitiveEff), true);

                if (hasEnoughPossitiveEff(target))
                {
                    target->CastSpell(target, SPELL_FULLY_MUTATED, true);

                    // Refresh at mutation
                    for (auto&& itr : possitiveEff)
                        if (Aura* aur = target->GetAura(itr))
                            aur->RefreshDuration();

                    for (auto&& itr : negativeEff)
                        if (Aura* aur = target->GetAura(itr))
                            aur->RefreshDuration();
                }
            }
        }
    }

    bool hasEnoughPossitiveEff(Unit* m_target)
    {
        uint32 count = 0;

        for (auto&& itr : possitiveEff)
            if (Aura* aur = m_target->GetAura(itr))
                count += aur->GetStackAmount();

        return count >= 5;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_primordius_player_mutation::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Volatile Mutation 141094
class spell_volatile_mutation_player : public AuraScript
{
    PrepareAuraScript(spell_volatile_mutation_player);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetOwner()->ToUnit())
            target->CastSpell(target, Trinity::Containers::SelectRandomContainerElement(negativeEff), true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_volatile_mutation_player::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Fully Mutated 140546
class spell_fully_mutated_by_player : public AuraScript
{
    PrepareAuraScript(spell_fully_mutated_by_player);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetOwner()->ToPlayer())
        {
            if (Pet* pet = target->GetPet())
            {
                int32 m_bp = target->GetMap()->IsHeroic() ? 300 : 54;
                target->CastCustomSpell(target, SPELL_FULLY_MUTATED_PET, &m_bp, 0, 0, true);
            }
        }
    }

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetOwner()->ToPlayer())
            if (Pet* pet = target->GetPet())
                pet->RemoveAurasDueToSpell(SPELL_FULLY_MUTATED_PET);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_fully_mutated_by_player::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_fully_mutated_by_player::HandleOnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Primordius Proximity Check Eff 137052
class spell_primordius_proximity_check_eff : public SpellScript
{
    PrepareSpellScript(spell_primordius_proximity_check_eff);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_DEADLY_MUTAGEN, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_VISCOUS_HORROR; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_primordius_proximity_check_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_primordius_proximity_check_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// 659, 884. Summoned by 136049 - Mutagenic Pool
class sat_mutagenic_pool : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* pTriggering)
    {
        return pTriggering && pTriggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* pTriggering)
    {
        if (Player* target = pTriggering->ToPlayer())
        {
            target->CastSpell(target, SPELL_MUTATION, true);

            if (Creature* caster = GetCaster()->ToCreature())
            {
                caster->AI()->SetData(TYPE_LIVING_FLUID, caster->AI()->GetData(TYPE_LIVING_FLUID) - 1);

                if (!caster->AI()->GetData(TYPE_LIVING_FLUID) && GetAreaTrigger())
                    GetAreaTrigger()->SetDuration(0);
            }
        }
    }
};

// 886, 930. Summoned by 140506 - Volatile Pool
class sat_volatile_pool_player : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* pTriggering)
    {
        return pTriggering && (pTriggering->ToPlayer() || pTriggering->GetEntry() == NPC_PRIMORDIUS);
    }

    void OnTriggeringApply(WorldObject* pTriggering)
    {
        if (Player* target = pTriggering->ToPlayer())
        {
            target->GetMap()->SetWorldState(WORLDSTATE_GEN_UNMODIFIED_ORGANISM, 0);
            target->CastSpell(target, SPELL_VOLATILE_MUTATION_AURA, true);
        }

        if (Creature* target = pTriggering->ToCreature())
        {
            target->CastSpell(target, SPELL_VOLATILE_MUTATION_PRIMORDIUS, true);
            
            if (AreaTrigger* areaTrigger = GetAreaTrigger())
                areaTrigger->SetDuration(0);

            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
        }
    }

    void OnTriggeringRemove(WorldObject* pTriggering)
    {
        if (Player* target = pTriggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_VOLATILE_MUTATION_AURA);
    }
};

// Areatrigger 9006
class AreaTrigger_at_primordius_intro : public AreaTriggerScript
{
    public:
        AreaTrigger_at_primordius_intro() : AreaTriggerScript("AreaTrigger_at_primordius_intro") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* primordius = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_PRIMORDIUS) : ObjectGuid::Empty))
                primordius->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

void AddSC_boss_primordius()
{
    new boss_primordius();
    new npc_living_fluid();
    new creature_script<npc_primordius_at_stalker>("npc_primordius_at_stalker");
    new creature_script<npc_primordius_viscous_horror>("npc_primordius_viscous_horror");

    new spell_script<spell_acidic_explosion_selector>("spell_acidic_explosion_selector");
    new spell_script<spell_caustic_gas>("spell_caustic_gas");
    new spell_script<spell_evolve_primordius>("spell_evolve_primordius");
    new spell_mutation_primordius();
    new spell_primordius_mutation_aura();
    new spell_submerge_cosmetic_aura();
    new aura_script<spell_malformed_blood>("spell_malformed_blood");
    new aura_script<spell_primordius_black_blood>("spell_primordius_black_blood");
    new spell_script<spell_erupting_pustules_selector>("spell_erupting_pustules_selector");
    new spell_script<spell_primordius_player_mutation>("spell_primordius_player_mutation");
    new aura_script<spell_volatile_mutation_player>("spell_volatile_mutation_player");
    new aura_script<spell_fully_mutated_by_player>("spell_fully_mutated_by_player");
    new spell_script<spell_primordius_proximity_check_eff>("spell_primordius_proximity_check_eff");
    new atrigger_script<sat_mutagenic_pool>("sat_mutagenic_pool");
    new atrigger_script<sat_volatile_pool_player>("sat_volatile_pool_player");
    new AreaTrigger_at_primordius_intro();
}
