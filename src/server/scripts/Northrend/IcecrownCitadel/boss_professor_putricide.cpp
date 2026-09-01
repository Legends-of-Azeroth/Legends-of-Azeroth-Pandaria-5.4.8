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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "Group.h"
#include "Spell.h"
#include "icecrown_citadel.h"
#include "Vehicle.h"
#include "GridNotifiers.h"
#include "IVMapManager.h"

enum ScriptTexts
{
    // Festergut
    SAY_FESTERGUT_GASEOUS_BLIGHT    = 0,
    SAY_FESTERGUT_DEATH             = 1,

    // Rotface
    SAY_ROTFACE_OOZE_FLOOD          = 2,
    SAY_ROTFACE_DEATH               = 3,

    // Professor Putricide
    SAY_AGGRO                       = 4,
    EMOTE_UNSTABLE_EXPERIMENT       = 5,
    SAY_PHASE_TRANSITION_HEROIC     = 6,
    SAY_TRANSFORM_1                 = 7,
    SAY_TRANSFORM_2                 = 8,    // always used for phase2 change, DO NOT GROUP WITH SAY_TRANSFORM_1
    EMOTE_MALLEABLE_GOO             = 9,
    EMOTE_CHOKING_GAS_BOMB          = 10,
    SAY_KILL                        = 11,
    SAY_BERSERK                     = 12,
    SAY_DEATH                       = 13,
};

enum Spells
{
    // Festergut
    SPELL_RELEASE_GAS_VISUAL              = 69125,
    SPELL_GASEOUS_BLIGHT_LARGE            = 69157,
    SPELL_GASEOUS_BLIGHT_MEDIUM           = 69162,
    SPELL_GASEOUS_BLIGHT_SMALL            = 69164,
    SPELL_MALLEABLE_GOO_H                 = 72296,
    SPELL_MALLEABLE_GOO_SUMMON            = 72298,
    SPELL_MALLEABLE_GOO_SUMMON_TRIGGER    = 72299,

    // Professor Putricide
    SPELL_SLIME_PUDDLE_TRIGGER            = 70341,
    SPELL_MALLEABLE_GOO                   = 72295,
    SPELL_UNSTABLE_EXPERIMENT             = 70351,
    SPELL_TEAR_GAS                        = 71617,    // phase transition
    SPELL_TEAR_GAS_1                      = 71615,    // phase transition
    SPELL_TEAR_GAS_2                      = 71618,    // phase transition
    SPELL_TEAR_GAS_CANCEL                 = 71620,
    SPELL_CREATE_CONCOCTION               = 71621,
    SPELL_GUZZLE_POTIONS                  = 71893,
    SPELL_OOZE_TANK_PROTECTION            = 71770,    // protects the tank
    SPELL_CHOKING_GAS_BOMB                = 71255,
    SPELL_OOZE_VARIABLE                   = 74118,
    SPELL_GAS_VARIABLE                    = 74119,
    SPELL_UNBOUND_PLAGUE                  = 70911,
    SPELL_UNBOUND_PLAGUE_SEARCHER         = 70917,
    SPELL_PLAGUE_SICKNESS                 = 70953,
    SPELL_UNBOUND_PLAGUE_PROTECTION       = 70955,
    SPELL_MUTATED_PLAGUE                  = 72451,
    SPELL_MUTATED_PLAGUE_CLEAR            = 72618,
    SPELL_VOLATILE_EXPERIMENT             = 72840,

    // Slime Puddle
    SPELL_GROW_STACKER                    = 70345,
    SPELL_GROW                            = 70347,
    SPELL_SLIME_PUDDLE_AURA               = 70343,
    SPELL_SLIME_PUDDLE                    = 70346,

    // Gas Cloud
    SPELL_GASEOUS_BLOAT_PROC              = 70215,
    SPELL_GASEOUS_BLOAT                   = 70672,
    SPELL_GASEOUS_BLOAT_PROTECTION        = 70812,
    SPELL_EXPUNGED_GAS                    = 70701,

    // Volatile Ooze
    SPELL_OOZE_ERUPTION                   = 70492,
    SPELL_VOLATILE_OOZE_ADHESIVE          = 70447,
    SPELL_OOZE_ERUPTION_SEARCH            = 70459,
    SPELL_VOLATILE_OOZE_PROTECTION        = 70530,

    // Choking Gas Bomb
    SPELL_CHOKING_GAS_BOMB_PERIODIC       = 71259,
    SPELL_CHOKING_GAS_EXPLOSION_TRIGGER   = 71280,

    // Mutated Abomination vehicle
    SPELL_ABOMINATION_VEHICLE_POWER_DRAIN = 70385,
    SPELL_MUTATED_TRANSFORMATION          = 70311,
    SPELL_MUTATED_TRANSFORMATION_DAMAGE   = 70405,
    SPELL_MUTATED_TRANSFORMATION_NAME     = 72401,
    SPELL_UNHOLY_INFUSION_CREDIT          = 71518,
};

enum Events
{
    // Festergut
    EVENT_FESTERGUT_DIES          = 1,
    EVENT_FESTERGUT_GOO           = 2,

    // Rotface
    EVENT_ROTFACE_DIES            = 3,
    EVENT_ROTFACE_OOZE_FLOOD      = 5,

    // Professor Putricide
    EVENT_BERSERK                 = 6,    // all phases
    EVENT_SLIME_PUDDLE            = 7,    // all phases
    EVENT_UNSTABLE_EXPERIMENT     = 8,    // P1 && P2
    EVENT_TEAR_GAS                = 9,    // phase transition not heroic
    EVENT_RESUME_ATTACK           = 10,
    EVENT_MALLEABLE_GOO           = 11,
    EVENT_CHOKING_GAS_BOMB        = 12,
    EVENT_UNBOUND_PLAGUE          = 13,
    EVENT_MUTATED_PLAGUE          = 14,
    EVENT_PHASE_TRANSITION        = 15,
    EVENT_VOLATILE_EXPERIMENT     = 16,   // phase transition heroic

    // Refresh ooze protection manually
    EVENT_REFRESH_OOZE_PROTECTION = 17,
    EVENT_MOVE_TO_TABLE_FAILSAFE  = 18,
    EVENT_MOVE_TO_TABLE_FORCED    = 19,
    EVENT_CHANGE_PHASE            = 20, // using event for this to wait until casts end
};

enum Phases
{
    PHASE_NONE          = 0,
    PHASE_FESTERGUT     = 1,
    PHASE_ROTFACE       = 2,
    PHASE_COMBAT_1      = 4,
    PHASE_COMBAT_2      = 5,
    PHASE_COMBAT_3      = 6,
};

enum Points
{
    POINT_FESTERGUT = 366260,
    POINT_ROTFACE   = 366270,
    POINT_TABLE     = 366780,
};

Position const festergutWatchPos = {4324.820f, 3166.03f, 389.3831f, 3.316126f}; //emote 432 (release gas)
Position const rotfaceWatchPos   = {4390.371f, 3164.50f, 389.3890f, 5.497787f}; //emote 432 (release ooze)
Position const tablePos          = {4356.190f, 3262.90f, 389.4820f, 1.483530f};

// used in Rotface encounter
uint32 const oozeFloodSpells[4] = {69782, 69796, 69798, 69801};

enum PutricideData
{
    DATA_EXPERIMENT_STAGE   = 1,
    DATA_PHASE              = 2,
    DATA_ABOMINATION        = 3,
    DATA_OOZE_MODE          = 4,
};

enum PutricideActions
{
    ACTION_OOZE_SKIP_DELAY  = 1,
    ACTION_OOZE_RESUME      = 2,
    ACTION_OOZE_TEAR_GAS    = 3,
};

#define EXPERIMENT_STATE_OOZE   false
#define EXPERIMENT_STATE_GAS    true

class AbominationDespawner
{
    public:
        explicit AbominationDespawner(Unit* owner) : _owner(owner) { }

        bool operator()(ObjectGuid guid)
        {
            if (Unit* summon = ObjectAccessor::GetUnit(*_owner, guid))
            {
                if (summon->GetEntry() == NPC_MUTATED_ABOMINATION_10 || summon->GetEntry() == NPC_MUTATED_ABOMINATION_25)
                {
                    if (Vehicle* veh = summon->GetVehicleKit())
                        veh->RemoveAllPassengers(); // also despawns the vehicle

                    // Found unit is Mutated Abomination, remove it
                    return true;
                }

                // Found unit is not Mutated Abomintaion, leave it
                return false;
            }

            // No unit found, remove from SummonList
            return true;
        }

    private:
        Unit* _owner;
};

class boss_professor_putricide : public CreatureScript
{
    public:
        boss_professor_putricide() : CreatureScript("boss_professor_putricide") { }

        struct boss_professor_putricideAI : public BossAI
        {
            boss_professor_putricideAI(Creature* creature) : BossAI(creature, DATA_PROFESSOR_PUTRICIDE), _baseSpeed(creature->GetSpeedRate(MOVE_RUN)), _experimentState(EXPERIMENT_STATE_OOZE)
            {
                _phase = PHASE_NONE;
            }

            void Reset() override
            {
                if (!(events.IsInPhase(PHASE_ROTFACE) || events.IsInPhase(PHASE_FESTERGUT)))
                    instance->SetBossState(DATA_PROFESSOR_PUTRICIDE, NOT_STARTED);
                me->GetMap()->SetWorldState(WORLDSTATE_NOUSEA_HEARTBURN, 1);

                events.Reset();
                summons.DespawnAll();
                SetPhase(PHASE_COMBAT_1);
                _experimentState = EXPERIMENT_STATE_OOZE;
                _tearGasActive = false;
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetWalk(false);
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
                    me->GetMotionMaster()->MovementExpired();

                if (instance->GetBossState(DATA_ROTFACE) == DONE && instance->GetBossState(DATA_FESTERGUT) == DONE)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);

                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_TEAR_GAS, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_TEAR_GAS_1, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_TEAR_GAS_2, true);

                me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (events.IsInPhase(PHASE_ROTFACE) || events.IsInPhase(PHASE_FESTERGUT))
                    return;

                if (!instance->CheckRequiredBosses(DATA_PROFESSOR_PUTRICIDE, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                me->setActive(true);
                events.Reset();
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_SLIME_PUDDLE, 10000);
                events.ScheduleEvent(EVENT_UNSTABLE_EXPERIMENT, 30000);
                events.ScheduleEvent(EVENT_REFRESH_OOZE_PROTECTION, 5000);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_UNBOUND_PLAGUE, 10000);

                SetPhase(PHASE_COMBAT_1);
                Talk(SAY_AGGRO);
                //DoCast(me, SPELL_OOZE_TANK_PROTECTION, true);
                DoZoneInCombat(me);

                instance->SetBossState(DATA_PROFESSOR_PUTRICIDE, IN_PROGRESS);

                me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                me->SetWalk(false);
                if (events.IsInPhase(PHASE_COMBAT_1) || events.IsInPhase(PHASE_COMBAT_2) || events.IsInPhase(PHASE_COMBAT_3))
                    instance->SetBossState(DATA_PROFESSOR_PUTRICIDE, FAIL);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                // Remove several nasty debuffs from players on death
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MUTATED_PLAGUE); // Mutated Plague
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNBOUND_PLAGUE); // Unbound Plague
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAGUE_SICKNESS); // Plague Sickness
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNBOUND_PLAGUE_SEARCHER); // Unbound Plague Search Periodic

                _JustDied();
                Talk(SAY_DEATH);
                DoCast(SPELL_MUTATED_PLAGUE_CLEAR);
                DoCast(me, SPELL_UNHOLY_INFUSION_CREDIT);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                switch (summon->GetEntry())
                {
                    case NPC_GROWING_OOZE_PUDDLE:

                        // These should only spawned in combat, if this one here is true, all spawns are invalid
                        // Avoids puddles staying around if boss is dead or resetted, since they are summoned after death or reset
                        if (instance)
                        {
                            if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) != IN_PROGRESS)
                            {
                                summons.DespawnAll();
                                return;
                            }
                        }

                        summon->CastSpell(summon, SPELL_GROW_STACKER, true);
                        summon->CastSpell(summon, SPELL_SLIME_PUDDLE_AURA, true);
                        // blizzard casts this spell 7 times initially (confirmed in sniff)
                        for (uint8 i = 0; i < 7; ++i)
                            summon->CastSpell(summon, SPELL_GROW, true);
                        break;
                    case NPC_GAS_CLOUD:
                        // no possible aura seen in sniff adding the aurastate
                        summon->ModifyAuraState(AURA_STATE_UNKNOWN22, true);
                        summon->CastSpell(summon, SPELL_GASEOUS_BLOAT_PROC, true);
                        // summon->CastCustomSpell(SPELL_GASEOUS_BLOAT, SPELLVALUE_AURA_STACK, 10, summon, false);
                        summon->SetReactState(REACT_PASSIVE);
                        if (_tearGasActive && summon->IsAIEnabled)
                            summon->GetAI()->DoAction(ACTION_OOZE_TEAR_GAS);
                        return;
                    case NPC_VOLATILE_OOZE:
                        // no possible aura seen in sniff adding the aurastate
                        summon->ModifyAuraState(AURA_STATE_UNKNOWN19, true);
                        // summon->CastSpell(summon, SPELL_VOLATILE_OOZE_ADHESIVE, false);
                        summon->SetReactState(REACT_PASSIVE);
                        if (_tearGasActive && summon->IsAIEnabled)
                            summon->GetAI()->DoAction(ACTION_OOZE_TEAR_GAS);
                        return;
                    case NPC_CHOKING_GAS_BOMB:
                        summon->CastSpell(summon, SPELL_CHOKING_GAS_BOMB_PERIODIC, true);
                        summon->CastSpell(summon, SPELL_CHOKING_GAS_EXPLOSION_TRIGGER, true);
                        return;
                    case NPC_MUTATED_ABOMINATION_10:
                    case NPC_MUTATED_ABOMINATION_25:
                        if (!me->IsInCombat())
                            AttackStart(summon);
                        else
                            me->AddThreat(summon, 1.0f);

                        if (summon->GetVehicleKit())
                            if (summon->GetVehicleKit()->GetPassenger(0))
                                me->AddThreat(summon->GetVehicleKit()->GetPassenger(0), 1.0f);

                        return;
                    default:
                        break;
                }

                if (me->IsInCombat())
                    DoZoneInCombat(summon);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
            {
                switch (_phase)
                {
                    case PHASE_COMBAT_1:
                        if (HealthAbovePct(80))
                            return;
                        me->SetReactState(REACT_PASSIVE);
                        DoAction(ACTION_CHANGE_PHASE);
                        break;
                    case PHASE_COMBAT_2:
                        if (HealthAbovePct(35))
                            return;
                        me->SetReactState(REACT_PASSIVE);
                        DoAction(ACTION_CHANGE_PHASE);
                        break;
                    default:
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->GetExactDist(who) <= 15.0f)
                    BossAI::MoveInLineOfSight(who);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_FESTERGUT:
                        instance->SetBossState(DATA_FESTERGUT, IN_PROGRESS); // needed here for delayed gate close
                        me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                        DoAction(ACTION_FESTERGUT_GAS);
                        if (Creature* festergut = Unit::GetCreature(*me, instance->GetGuidData(DATA_FESTERGUT)))
                            festergut->CastSpell(festergut, SPELL_GASEOUS_BLIGHT_LARGE, false, nullptr, nullptr, festergut->GetGUID());
                        break;
                    case POINT_ROTFACE:
                        instance->SetBossState(DATA_ROTFACE, IN_PROGRESS);   // needed here for delayed gate close
                        me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                        DoAction(ACTION_ROTFACE_OOZE);
                        events.ScheduleEvent(EVENT_ROTFACE_OOZE_FLOOD, 25000, 0, PHASE_ROTFACE);
                        break;
                    case POINT_TABLE:
                        // stop attack
                        events.CancelEvent(EVENT_MOVE_TO_TABLE_FAILSAFE);
                        events.CancelEvent(EVENT_MOVE_TO_TABLE_FORCED);
                        me->GetMotionMaster()->MoveIdle();
                        me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                        if (GameObject* table = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_PUTRICIDE_TABLE)))
                            me->SetFacingToObject(table);
                        if (IsHeroic())
                            Talk(SAY_PHASE_TRANSITION_HEROIC);
                        // operating on new phase already
                        switch (_phase)
                        {
                            case PHASE_COMBAT_2:
                            {
                                SpellInfo const* spell = sSpellMgr->GetSpellInfo(SPELL_CREATE_CONCOCTION);
                                DoCast(me, SPELL_CREATE_CONCOCTION);
                                events.ScheduleEvent(EVENT_PHASE_TRANSITION, sSpellMgr->GetSpellForDifficultyFromSpell(spell, me)->CalcCastTime() + 100);
                                break;
                            }
                            case PHASE_COMBAT_3:
                            {
                                SpellInfo const* spell = sSpellMgr->GetSpellInfo(SPELL_GUZZLE_POTIONS);
                                DoCast(me, SPELL_GUZZLE_POTIONS);
                                events.ScheduleEvent(EVENT_PHASE_TRANSITION, sSpellMgr->GetSpellForDifficultyFromSpell(spell, me)->CalcCastTime() + 100);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_FESTERGUT_COMBAT:
                        SetPhase(PHASE_FESTERGUT);
                        me->SetSpeed(MOVE_RUN, _baseSpeed*2.0f, true);
                        me->SetReactState(REACT_PASSIVE);
                        DoZoneInCombat(me);
                        me->GetMotionMaster()->MovePoint(POINT_FESTERGUT, festergutWatchPos);
                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_FESTERGUT_GOO, urand(13000, 18000), 0, PHASE_FESTERGUT);
                        break;
                    case ACTION_FESTERGUT_GAS:
                        Talk(SAY_FESTERGUT_GASEOUS_BLIGHT);
                        DoCast(me, SPELL_RELEASE_GAS_VISUAL, true);
                        break;
                    case ACTION_FESTERGUT_DEATH:
                        me->InterruptNonMeleeSpells(true); // Otherwise remorseless winter might fail
                        me->SetReactState(REACT_PASSIVE); // Needs to be done here, otherwise LK might get taunted while moving, destroying script mechanisms
                        me->AttackStop();
                        events.ScheduleEvent(EVENT_FESTERGUT_DIES, 4000, 0, PHASE_FESTERGUT);
                        break;
                    case ACTION_ROTFACE_COMBAT:
                    {
                        SetPhase(PHASE_ROTFACE);
                        me->SetSpeed(MOVE_RUN, _baseSpeed*2.0f, true);
                        me->SetReactState(REACT_PASSIVE);
                        _oozeFloodStage = 0;
                        DoZoneInCombat(me);
                        me->GetMotionMaster()->MovePoint(POINT_ROTFACE, rotfaceWatchPos);
                        // init random sequence of floods
                        if (Creature* rotface = Unit::GetCreature(*me, instance->GetGuidData(DATA_ROTFACE)))
                        {
                            std::list<Creature*> list;
                            GetCreatureListWithEntryInGrid(list, rotface, NPC_PUDDLE_STALKER, 36.0f);
                            if (list.size() > 4)
                            {
                                list.sort(Trinity::ObjectDistanceOrderPred(rotface));
                                do
                                {
                                    list.pop_back();
                                } while (list.size() > 4);
                            }

                            uint8 i = 0;
                            while (!list.empty())
                            {
                                std::list<Creature*>::iterator itr = list.begin();
                                std::advance(itr, urand(0, list.size()-1));
                                _oozeFloodDummyGUIDs[i++] = (*itr)->GetGUID();
                                list.erase(itr);
                            }
                        }
                        break;
                    }
                    case ACTION_ROTFACE_OOZE:
                        Talk(SAY_ROTFACE_OOZE_FLOOD);
                        if (Creature* dummy = Unit::GetCreature(*me, _oozeFloodDummyGUIDs[_oozeFloodStage]))
                            dummy->CastSpell(dummy, oozeFloodSpells[_oozeFloodStage], true, nullptr, nullptr, me->GetGUID()); // cast from self for LoS (with prof's GUID for logs)
                        if (++_oozeFloodStage == 4)
                            _oozeFloodStage = 0;
                        break;
                    case ACTION_ROTFACE_DEATH:
                        me->InterruptNonMeleeSpells(true); // Otherwise remorseless winter might fail
                        events.ScheduleEvent(EVENT_ROTFACE_DIES, 4500, 0, PHASE_ROTFACE);
                        break;
                    case ACTION_CHANGE_PHASE:
                        me->SetSpeed(MOVE_RUN, _baseSpeed*2.0f, true);
                        events.CancelEvent(EVENT_UNSTABLE_EXPERIMENT);
                        events.CancelEvent(EVENT_MALLEABLE_GOO);
                        events.CancelEvent(EVENT_SLIME_PUDDLE);
                        events.CancelEvent(EVENT_CHOKING_GAS_BOMB);
                        events.CancelEvent(EVENT_UNBOUND_PLAGUE);
                        events.CancelEvent(EVENT_MUTATED_PLAGUE);
                        events.ScheduleEvent(EVENT_CHANGE_PHASE, 1);
                        switch (_phase)
                        {
                            case PHASE_COMBAT_1:
                                SetPhase(PHASE_COMBAT_2);
                                break;
                            case PHASE_COMBAT_2:
                                SetPhase(PHASE_COMBAT_3);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_EXPERIMENT_STAGE:
                    {
                        // ALSO MODIFIES!
                        uint32 ret = uint32(_experimentState);
                        _experimentState ^= true;
                        return ret;
                    }
                    case DATA_PHASE:
                        return _phase;
                    case DATA_ABOMINATION:
                        summons.RemoveNotExisting();
                        return uint32(summons.HasEntry(NPC_MUTATED_ABOMINATION_10) || summons.HasEntry(NPC_MUTATED_ABOMINATION_25));
                    default:
                        break;
                }

                return 0;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell && spell->Id == SPELL_TEAR_GAS_1 && !IsHeroic() && me->IsInCombat())
                    events.RescheduleEvent(EVENT_RESUME_ATTACK, 16000);
                else if (spell && spell->Id == SPELL_TEAR_GAS_CANCEL)
                {
                    target->RemoveAurasDueToSpell(SPELL_TEAR_GAS_1);
                    target->RemoveAurasDueToSpell(SPELL_TEAR_GAS_2);
                    if (target->IsAIEnabled && (target->GetEntry() == NPC_VOLATILE_OOZE || target->GetEntry() == NPC_GAS_CLOUD))
                        target->GetAI()->DoAction(ACTION_OOZE_RESUME);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if ((!(events.IsInPhase(PHASE_ROTFACE) || events.IsInPhase(PHASE_FESTERGUT)) && !UpdateVictim()) || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FESTERGUT_DIES:
                            Talk(SAY_FESTERGUT_DEATH);
                            EnterEvadeMode();
                            break;
                        case EVENT_FESTERGUT_GOO:
                            me->SendMovementFlagUpdate();
                            DoCastAOE(SPELL_MALLEABLE_GOO_SUMMON_TRIGGER);
                            events.ScheduleEvent(EVENT_FESTERGUT_GOO, (Is25ManRaid() ? 10000 : 30000) + urand(0, 5000), 0, PHASE_FESTERGUT);
                            break;
                        case EVENT_ROTFACE_DIES:
                            Talk(SAY_ROTFACE_DEATH);
                            EnterEvadeMode();
                            break;
                        case EVENT_ROTFACE_OOZE_FLOOD:
                            DoAction(ACTION_ROTFACE_OOZE);
                            events.ScheduleEvent(EVENT_ROTFACE_OOZE_FLOOD, 25000, 0, PHASE_ROTFACE);
                            break;
                        case EVENT_BERSERK:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK2);
                            break;
                        case EVENT_SLIME_PUDDLE:
                        {
                            std::list<Unit*> targets;
                            SelectTargetList(targets, 2, SELECT_TARGET_RANDOM, 0.0f, true);
                            if (!targets.empty())
                                for (std::list<Unit*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                    DoCast(*itr, SPELL_SLIME_PUDDLE_TRIGGER);
                            events.ScheduleEvent(EVENT_SLIME_PUDDLE, 35000);
                            break;
                        }
                        case EVENT_UNSTABLE_EXPERIMENT:
                            Talk(EMOTE_UNSTABLE_EXPERIMENT);
                            DoCast(me, SPELL_UNSTABLE_EXPERIMENT);
                            events.ScheduleEvent(EVENT_UNSTABLE_EXPERIMENT, 38000);
                            break;
                        case EVENT_TEAR_GAS:
                            events.ScheduleEvent(EVENT_MOVE_TO_TABLE_FAILSAFE, 500);
                            events.ScheduleEvent(EVENT_MOVE_TO_TABLE_FORCED, 10000);
                            me->GetMotionMaster()->MovePoint(POINT_TABLE, tablePos);
                            break;
                        case EVENT_RESUME_ATTACK:
                            _tearGasActive = false;
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            // remove Tear Gas
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEAR_GAS_1);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEAR_GAS_2);
                            DoCastAOE(SPELL_TEAR_GAS_CANCEL);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GAS_VARIABLE);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OOZE_VARIABLE);
                            switch (_phase)
                            {
                                case PHASE_COMBAT_2:
                                    events.RescheduleEvent(EVENT_UNSTABLE_EXPERIMENT, 20000);
                                    events.RescheduleEvent(EVENT_SLIME_PUDDLE, 10000);
                                    events.RescheduleEvent(EVENT_MALLEABLE_GOO, 5000);
                                    events.RescheduleEvent(EVENT_CHOKING_GAS_BOMB, 15000);
                                    if (IsHeroic())
                                        events.RescheduleEvent(EVENT_UNBOUND_PLAGUE, 50000);
                                    break;
                                case PHASE_COMBAT_3:
                                    summons.DespawnIf(AbominationDespawner(me));
                                    events.RescheduleEvent(EVENT_SLIME_PUDDLE, 15000);
                                    events.RescheduleEvent(EVENT_MALLEABLE_GOO, 9000);
                                    events.RescheduleEvent(EVENT_CHOKING_GAS_BOMB, 12000);
                                    events.RescheduleEvent(EVENT_MUTATED_PLAGUE, 25000);
                                    if (IsHeroic())
                                        events.RescheduleEvent(EVENT_UNBOUND_PLAGUE, 50000);
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case EVENT_MALLEABLE_GOO:
                            DoCastAOE(SPELL_MALLEABLE_GOO);
                            events.ScheduleEvent(EVENT_MALLEABLE_GOO, urand(25000, 30000));
                            break;
                        case EVENT_CHOKING_GAS_BOMB:
                            Talk(EMOTE_CHOKING_GAS_BOMB);
                            DoCast(me, SPELL_CHOKING_GAS_BOMB);
                            events.ScheduleEvent(EVENT_CHOKING_GAS_BOMB, urand(35000, 40000));
                            break;
                        case EVENT_UNBOUND_PLAGUE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [this](Unit* u) { return me->GetVictim() != u && !u->HasAura(SPELL_PLAGUE_SICKNESS) && me->IsWithinCombatRange(u, 45.0f); }))
                            {
                                DoCast(target, SPELL_UNBOUND_PLAGUE);
                                DoCast(target, SPELL_UNBOUND_PLAGUE_SEARCHER);
                            }
                            events.ScheduleEvent(EVENT_UNBOUND_PLAGUE, 60000);
                            break;
                        case EVENT_MUTATED_PLAGUE:
                            DoCastVictim(SPELL_MUTATED_PLAGUE);
                            events.ScheduleEvent(EVENT_MUTATED_PLAGUE, 10000);
                            break;
                        case EVENT_PHASE_TRANSITION:
                        {
                            switch (_phase)
                            {
                                case PHASE_COMBAT_2:
                                    if (Creature* face = me->FindNearestCreature(NPC_TEAR_GAS_TARGET_STALKER, 50.0f))
                                        me->SetFacingToObject(face);
                                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                    Talk(SAY_TRANSFORM_1);
                                    if (IsHeroic())
                                        events.ScheduleEvent(EVENT_RESUME_ATTACK, 10000, 0, PHASE_COMBAT_2);
                                    break;
                                case PHASE_COMBAT_3:
                                    if (Creature* face = me->FindNearestCreature(NPC_TEAR_GAS_TARGET_STALKER, 50.0f))
                                        me->SetFacingToObject(face);
                                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                    Talk(SAY_TRANSFORM_2);
                                    if (IsHeroic())
                                        events.ScheduleEvent(EVENT_RESUME_ATTACK, 10000, 0, PHASE_COMBAT_3);
                                    break;
                                default:
                                    break;
                            }
                        }
                        case EVENT_REFRESH_OOZE_PROTECTION:
                            if (me->GetVictim())
                            {
                                me->GetVictim()->AddAura(SPELL_VOLATILE_OOZE_PROTECTION, me->GetVictim());
                                me->GetVictim()->AddAura(SPELL_GASEOUS_BLOAT_PROTECTION, me->GetVictim());
                            }
                            events.ScheduleEvent(EVENT_REFRESH_OOZE_PROTECTION, 5000);
                            break;
                        case EVENT_VOLATILE_EXPERIMENT:
                            DoCast(me, SPELL_UNSTABLE_EXPERIMENT, true);
                            DoCast(me, SPELL_UNSTABLE_EXPERIMENT, true);
                            // cast variables
                            if (Is25ManRaid())
                            {
                                std::list<Unit*> targetList;
                                {
                                    const std::list<HostileReference*>& threatlist = me->GetThreatManager().getThreatList();
                                    for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                                        if ((*itr)->getTarget()->GetTypeId() == TYPEID_PLAYER)
                                            targetList.push_back((*itr)->getTarget());
                                }

                                size_t half = targetList.size()/2;
                                // half gets ooze variable
                                while (half < targetList.size())
                                {
                                    std::list<Unit*>::iterator itr = targetList.begin();
                                    advance(itr, urand(0, targetList.size() - 1));
                                    (*itr)->CastSpell(*itr, SPELL_OOZE_VARIABLE, true);
                                    targetList.erase(itr);
                                }
                                // and half gets gas
                                for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                                    (*itr)->CastSpell(*itr, SPELL_GAS_VARIABLE, true);
                            }
                            events.ScheduleEvent(EVENT_MOVE_TO_TABLE_FAILSAFE, 500);
                            events.ScheduleEvent(EVENT_MOVE_TO_TABLE_FORCED, 10000);
                            me->GetMotionMaster()->MovePoint(POINT_TABLE, tablePos);
                            break;
                        case EVENT_MOVE_TO_TABLE_FAILSAFE:
                        {
                            events.ScheduleEvent(EVENT_MOVE_TO_TABLE_FAILSAFE, 500);
                            float x, y, z;
                            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE ||
                                !me->GetMotionMaster()->GetDestination(x, y, z) ||
                                tablePos.GetPositionX() != x || tablePos.GetPositionY() != y || tablePos.GetPositionZ() != z)
                                me->GetMotionMaster()->MovePoint(POINT_TABLE, tablePos);
                            break;
                        }
                        case EVENT_MOVE_TO_TABLE_FORCED:
                            me->NearTeleportTo(tablePos.GetPositionX(), tablePos.GetPositionY(), tablePos.GetPositionZ(), tablePos.GetOrientation());
                            MovementInform(POINT_MOTION_TYPE, POINT_TABLE);
                            break;
                        case EVENT_CHANGE_PHASE:
                            me->AttackStop();
                            if (!IsHeroic())
                            {
                                _tearGasActive = true;
                                events.RescheduleEvent(EVENT_RESUME_ATTACK, 30000); // Failsafe in case Tear Gas fails or doesn't hit anybody
                            }
                            DoCast(me, IsHeroic() ? SPELL_VOLATILE_EXPERIMENT : SPELL_TEAR_GAS);
                            events.ScheduleEvent(IsHeroic() ? EVENT_VOLATILE_EXPERIMENT : EVENT_TEAR_GAS, 2500);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            void SetPhase(Phases newPhase)
            {
                _phase = newPhase;
                events.SetPhase(newPhase);
            }

            ObjectGuid _oozeFloodDummyGUIDs[4];
            Phases _phase;          // external of EventMap because event phase gets reset on evade
            float const _baseSpeed;
            uint8 _oozeFloodStage;
            mutable bool _experimentState;
            bool _tearGasActive;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_professor_putricideAI>(creature);
        }
};

class npc_tentacle_stalker : public CreatureScript
{
    public:
        npc_tentacle_stalker() : CreatureScript("npc_tentacle_stalker") { }

        struct npc_tentacle_stalkerAI : public ScriptedAI
        {
            npc_tentacle_stalkerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void UpdateAI(uint32 diff) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_tentacle_stalkerAI>(creature);
        }
};

class npc_putricide_ooze : public CreatureScript
{
    public:
        npc_putricide_ooze() : CreatureScript("npc_putricide_ooze") { }

        struct npc_putricide_oozeAI : public ScriptedAI
        {
            npc_putricide_oozeAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _CheckTimer = 2000;
                _securityTimer = 0;
                _oozeMode = false;
                _gasMode = false;
                _needSearchNewTarget = true;
                _movementDisabled = false;
                _enableSecurityCheck = false;
                _skipDelay = false;

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_TEAR_GAS, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_TEAR_GAS_1, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_TEAR_GAS_2, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->m_modSpellHitChance += 1000.0f;

                switch (me->GetEntry())
                {
                    // Volatile Ooze
                    case 37697:
                    case 38604:
                    case 38758:
                    case 38759:
                        _oozeMode = true;
                        break;
                    // Gas Cloud
                    case 37562:
                    case 38602:
                    case 38760:
                    case 38761:
                        _gasMode = true;
                        break;
                }
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                float x, y, z;
                me->GetPosition(x, y, z);
                z = me->GetMap()->GetHeight(x, y, z, true, MAX_FALL_DISTANCE);
                if (z != VMAP_INVALID_HEIGHT_VALUE)
                    me->Relocate(x, y, z);
            }

            void AttackStart(Unit* victim) override
            {
                if (!victim)
                    return;

                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                // Lock threat to the target of spell selection
                // Prevent error message on first attack here
                me->AddThreat(victim, 50000000.0f);
                DoResetThreat();
                me->AddThreat(victim, 50000000.0f);

                UnitAI::AttackStart(victim);

                _enableSecurityCheck = false; // Prevent premature despawn if the target becomes unattackable within 4 seconds after cast
                _needSearchNewTarget = false; // Prevent casting target selection spell twice after the ooze picked a target

                // Move POINT to target, move chase will skip because of spellcasting
                if (me->GetMotionMaster())
                    me->GetMotionMaster()->MovePoint(0, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ());
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                // Start new target search, if current target exploded
                if (!_needSearchNewTarget && (spell->Id == SPELL_OOZE_ERUPTION || spell->Id == 70701))
                {
                    StopOoze();
                    _CheckTimer = _skipDelay ? 0 : 4000;
                    _skipDelay = false;
                }
            }

            bool TargetHasMyAttackingSpell(Unit* who)
            {
                return who && who->HasAura(_oozeMode ? SPELL_VOLATILE_OOZE_ADHESIVE : SPELL_GASEOUS_BLOAT, me->GetGUID());
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_OOZE_MODE)
                    return _oozeMode;
                return 0;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_OOZE_SKIP_DELAY)
                {
                    _skipDelay = true;
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                }
                else if (action == ACTION_OOZE_RESUME)
                {
                    _needSearchNewTarget = true;
                    _skipDelay = true;
                    _movementDisabled = false;
                    _CheckTimer = 0;
                }
                else if (action == ACTION_OOZE_TEAR_GAS)
                    _movementDisabled = true;
            }

            void UpdateAI(uint32 diff) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) != IN_PROGRESS)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }

                if (me->GetVictim() && me->GetMotionMaster() && (me->GetVictim()->HasAura(SPELL_TEAR_GAS_1) || me->GetVictim()->HasAura(SPELL_TEAR_GAS_2)))
                {
                    StopOoze();
                    _movementDisabled = true;
                }

                if (_movementDisabled)
                    return;

                // Start checking here
                if (_CheckTimer <= diff)
                {
                    bool victimFound = false;

                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL) || me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                        _needSearchNewTarget = false;

                    // Process new target search here
                    if (_needSearchNewTarget)
                    {
                        _needSearchNewTarget = false;
                        _securityTimer = 0;
                        _enableSecurityCheck = true;

                        if (_oozeMode)
                            me->CastSpell(me, SPELL_VOLATILE_OOZE_ADHESIVE, false);
                        else if (_gasMode)
                            me->CastCustomSpell(SPELL_GASEOUS_BLOAT, SPELLVALUE_AURA_STACK, 10, me, false);
                    }

                    // Trigger explosion effect (ooze only, gas explosion triggered via aura proc)
                    if (_oozeMode && me->GetVictim() && me->GetDistance(me->GetVictim()) < 1.5f && TargetHasMyAttackingSpell(me->GetVictim()))
                    {
                        me->GetVictim()->AddAura(SPELL_VOLATILE_OOZE_PROTECTION, me->GetVictim());
                        DoCastVictim(SPELL_OOZE_ERUPTION_SEARCH, true);
                    }

                    // Our victim had the attacking spell active => Enable system for target switching, if victim does not have attacking spell anymore
                    if (me->GetVictim() && TargetHasMyAttackingSpell(me->GetVictim()))
                    {
                        victimFound = true;

                        // Move POINT to target, move chase will skip because of spellcasting
                        if (victimFound)
                            if (me->GetMotionMaster())
                                me->GetMotionMaster()->MovePoint(0, me->GetVictim()->GetPositionX(), me->GetVictim()->GetPositionY(), me->GetVictim()->GetPositionZ());
                    }

                    // Only check for new targets, if new target search has not been scheduled yet and a victim has already been set by spell casting
                    if (!_needSearchNewTarget)
                    {
                        if (!me->GetVictim() || // Search new target if no victim
                            me->GetVictim() && (!TargetHasMyAttackingSpell(me->GetVictim()) || !me->GetVictim()->IsAlive() || !me->IsValidAttackTarget(me->GetVictim()))) // Search new target if victim does not have our attacking aura OR victim is dead OR victim is not attackable
                            StopOoze(false);
                    }

                    // Delay new target search here
                    if (_needSearchNewTarget)
                    {
                        _CheckTimer = _skipDelay ? 0 : 4000;
                        _skipDelay = false;
                        return;
                    }

                    _CheckTimer = 1000;
                }
                else
                    _CheckTimer -= diff;

                // Security check - after 4 sec after spellcast start a victim having our attacking spell should be found
                if (_enableSecurityCheck)
                {
                    if ((_securityTimer += diff) >= 4000)
                    {
                        _enableSecurityCheck = false;
                        _securityTimer = 0;

                        _needSearchNewTarget = true;
                    }
                }

                if (_gasMode && me->GetVictim())
                    DoMeleeAttackIfReady(true);
            }

        private:
            void StopOoze(bool interrupt = true)
            {
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                if (interrupt)
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                if (me->GetVictim())
                    me->GetVictim()->RemoveAurasDueToSpell(_oozeMode ? SPELL_VOLATILE_OOZE_ADHESIVE : SPELL_GASEOUS_BLOAT, me->GetGUID());
                else if (InstanceScript* instance = me->GetInstanceScript())
                {
                    Map::PlayerList const& players = instance->instance->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        if (Player* player = itr->GetSource())
                            player->RemoveAurasDueToSpell(_oozeMode ? SPELL_VOLATILE_OOZE_ADHESIVE : SPELL_GASEOUS_BLOAT, me->GetGUID());
                }

                DoStopAttack();

                _needSearchNewTarget = true;
            }

            uint32 _CheckTimer;
            uint32 _securityTimer;
            bool _oozeMode;
            bool _gasMode;
            bool _needSearchNewTarget;
            bool _movementDisabled;
            bool _enableSecurityCheck;
            bool _skipDelay;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_putricide_oozeAI>(creature);
        }
};

class npc_malleable_ooze : public CreatureScript
{
    public:
        npc_malleable_ooze() : CreatureScript("npc_malleable_ooze") { }

        struct npc_malleable_oozeAI : public ScriptedAI
        {
            npc_malleable_oozeAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* boss = instance->instance->GetCreature(instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE)))
                        boss->CastSpell(me, SPELL_MALLEABLE_GOO_H);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_malleable_oozeAI>(creature);
        }
};

class spell_putricide_gaseous_bloat : public SpellScriptLoader
{
    public:
        spell_putricide_gaseous_bloat() : SpellScriptLoader("spell_putricide_gaseous_bloat") { }

        class spell_putricide_gaseous_bloat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_gaseous_bloat_AuraScript);

            void HandleExtraEffect(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();
                if (Unit* caster = GetCaster())
                {
                    target->RemoveAuraFromStack(GetSpellInfo()->Id, GetCasterGUID());
                    if (!target->HasAura(GetId()) && caster->IsAIEnabled)
                        caster->GetAI()->DoAction(ACTION_OOZE_SKIP_DELAY);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_putricide_gaseous_bloat_AuraScript::HandleExtraEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_putricide_gaseous_bloat_AuraScript();
        }
};

class spell_putricide_ooze_channel : public SpellScriptLoader
{
    public:
        spell_putricide_ooze_channel() : SpellScriptLoader("spell_putricide_ooze_channel") { }

        class spell_putricide_ooze_channel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_ooze_channel_SpellScript);

            bool Validate(SpellInfo const* spellInfo) override
            {
                if (!spellInfo->ExcludeTargetAuraSpell)
                    return false;
                if (!sSpellMgr->GetSpellInfo(spellInfo->ExcludeTargetAuraSpell))
                    return false;
                return true;
            }

            // set up initial variables and check if caster is creature
            // this will let use safely use ToCreature() casts in entire script
            bool Load() override
            {
                _target = nullptr;
                return GetCaster()->GetTypeId() == TYPEID_UNIT && GetCaster()->IsAIEnabled;
            }

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, GetCaster()->GetAI()->GetData(DATA_OOZE_MODE) ? SPELL_GAS_VARIABLE : SPELL_OOZE_VARIABLE));

                if (targets.empty())
                {
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
                    return;
                }

                WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
                targets.clear();
                targets.push_back(target);
                _target = target;
            }

            void SetTarget(std::list<WorldObject*>& targets)
            {
                targets.clear();
                if (_target)
                    targets.push_back(_target);
            }

            void StartAttack()
            {
                GetCaster()->ClearUnitState(UNIT_STATE_CASTING);
                GetCaster()->ToCreature()->AI()->AttackStart(GetHitUnit());
            }

            // temporary, until SelectTarget are not called on empty lists
            void CheckTarget()
            {
                if (_target)
                    return;

                FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_ooze_channel_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_ooze_channel_SpellScript::SetTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_ooze_channel_SpellScript::SetTarget, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_putricide_ooze_channel_SpellScript::StartAttack);
                OnCast += SpellCastFn(spell_putricide_ooze_channel_SpellScript::CheckTarget);
            }

            WorldObject* _target;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_ooze_channel_SpellScript();
        }
};

class ExactDistanceCheck
{
    public:
        ExactDistanceCheck(Unit* source, float dist) : _source(source), _dist(dist) { }

        bool operator()(WorldObject* unit) const
        {
            return _source->GetExactDist2d(unit) > _dist;
        }

    private:
        Unit* _source;
        float _dist;
};

class spell_putricide_slime_puddle_summon : public SpellScriptLoader
{
    public:
        spell_putricide_slime_puddle_summon() : SpellScriptLoader("spell_putricide_slime_puddle_summon") { }

        class spell_putricide_slime_puddle_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_slime_puddle_summon_SpellScript);

            void SelectDest()
            {
                if (WorldLocation const* dest = GetExplTargetDest())
                    const_cast<WorldLocation*>(dest)->m_positionZ = 389.4f;
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_putricide_slime_puddle_summon_SpellScript::SelectDest);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_slime_puddle_summon_SpellScript();
        }
};

class spell_putricide_slime_puddle : public SpellScriptLoader
{
    public:
        spell_putricide_slime_puddle() : SpellScriptLoader("spell_putricide_slime_puddle") { }

        class spell_putricide_slime_puddle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_slime_puddle_SpellScript);

            void ScaleRange(std::list<WorldObject*>& targets)
            {
                targets.remove_if(ExactDistanceCheck(GetCaster(), 2.5f * GetCaster()->GetObjectScale()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_slime_puddle_SpellScript::ScaleRange, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_slime_puddle_SpellScript::ScaleRange, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_slime_puddle_SpellScript();
        }
};

// this is here only because on retail you dont actually enter HEROIC mode for ICC
class spell_putricide_slime_puddle_aura : public SpellScriptLoader
{
    public:
        spell_putricide_slime_puddle_aura() : SpellScriptLoader("spell_putricide_slime_puddle_aura") { }

        class spell_putricide_slime_puddle_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_slime_puddle_aura_SpellScript);

            void ReplaceAura()
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->AddAura(SPELL_SLIME_PUDDLE, target);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_putricide_slime_puddle_aura_SpellScript::ReplaceAura);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_slime_puddle_aura_SpellScript();
        }
};

class spell_putricide_unstable_experiment : public SpellScriptLoader
{
    public:
        spell_putricide_unstable_experiment() : SpellScriptLoader("spell_putricide_unstable_experiment") { }

        class spell_putricide_unstable_experiment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_unstable_experiment_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (GetCaster()->GetTypeId() != TYPEID_UNIT)
                    return;

                uint32 stage = GetCaster()->ToCreature()->AI()->GetData(DATA_EXPERIMENT_STAGE);
                Creature* target = nullptr;
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    if (ObjectGuid guid = instance->GetGuidData(stage ? DATA_UNSTABLE_EXPERIMENT_STALKER_RED : DATA_UNSTABLE_EXPERIMENT_STALKER_GREEN))
                        target = instance->instance->GetCreature(guid);
                /*std::list<Creature*> creList;
                GetCreatureListWithEntryInGrid(creList, GetCaster(), NPC_ABOMINATION_WING_MAD_SCIENTIST_STALKER, 200.0f);
                // 2 of them are spawned at green place - weird trick blizz
                for (std::list<Creature*>::iterator itr = creList.begin(); itr != creList.end(); ++itr)
                {
                    target = *itr;
                    std::list<Creature*> tmp;
                    GetCreatureListWithEntryInGrid(tmp, target, NPC_ABOMINATION_WING_MAD_SCIENTIST_STALKER, 10.0f);
                    if ((!stage && tmp.size() > 1) || (stage && tmp.size() == 1))
                        break;
                }*/

                if (target)
                    target->CastSpell(target, uint32(GetSpellInfo()->Effects[stage].CalcValue()), true, nullptr, nullptr, GetCaster()->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_unstable_experiment_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_unstable_experiment_SpellScript();
        }
};

class spell_putricide_ooze_eruption_searcher : public SpellScriptLoader
{
    public:
        spell_putricide_ooze_eruption_searcher() : SpellScriptLoader("spell_putricide_ooze_eruption_searcher") { }

        class spell_putricide_ooze_eruption_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_ooze_eruption_searcher_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit()->HasAura(SPELL_VOLATILE_OOZE_ADHESIVE))
                {
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_OOZE_ERUPTION, true);
                    GetHitUnit()->RemoveAurasDueToSpell(SPELL_VOLATILE_OOZE_ADHESIVE, GetCaster()->GetGUID(), 0, AURA_REMOVE_BY_ENEMY_SPELL);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_ooze_eruption_searcher_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_ooze_eruption_searcher_SpellScript();
        }
};

class spell_putricide_choking_gas_bomb : public SpellScriptLoader
{
    public:
        spell_putricide_choking_gas_bomb() : SpellScriptLoader("spell_putricide_choking_gas_bomb") { }

        class spell_putricide_choking_gas_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_choking_gas_bomb_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Effects[EFFECT_1].CalcValue(), true, nullptr, nullptr, GetCaster()->GetGUID());
                GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Effects[EFFECT_2].CalcValue(), true, nullptr, nullptr, GetCaster()->GetGUID());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_choking_gas_bomb_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_choking_gas_bomb_SpellScript();
        }
};

class spell_putricide_choking_gas_bomb_effect : public SpellScriptLoader
{
    public:
        spell_putricide_choking_gas_bomb_effect() : SpellScriptLoader("spell_putricide_choking_gas_bomb_effect") { }

        class spell_putricide_choking_gas_bomb_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_choking_gas_bomb_effect_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, VEHICLE_SPELL_RIDE_HARDCODED));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_choking_gas_bomb_effect_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_choking_gas_bomb_effect_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_choking_gas_bomb_effect_SpellScript();
        }
};

class spell_putricide_unbound_plague : public SpellScriptLoader
{
    public:
        spell_putricide_unbound_plague() : SpellScriptLoader("spell_putricide_unbound_plague") { }

        class spell_putricide_unbound_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_unbound_plague_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_UNBOUND_PLAGUE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_UNBOUND_PLAGUE_SEARCHER))
                    return false;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_UNBOUND_PLAGUE_SEARCHER, EFFECT_0))
                {
                    if (eff->GetTickNumber() <= 2)
                    {
                        targets.clear();
                        return;
                    }
                }


                targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_UNBOUND_PLAGUE));
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                InstanceScript* instance = GetCaster()->GetInstanceScript();
                if (!instance)
                    return;

                if (!GetHitUnit()->HasAura(SPELL_UNBOUND_PLAGUE))
                {
                    if (Creature* professor = ObjectAccessor::GetCreature(*GetCaster(), instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE)))
                    {
                        if (Aura* oldPlague = GetCaster()->GetAura(SPELL_UNBOUND_PLAGUE, professor->GetGUID()))
                        {
                            if (Aura* newPlague = professor->AddAura(SPELL_UNBOUND_PLAGUE, GetHitUnit()))
                            {
                                newPlague->SetMaxDuration(oldPlague->GetMaxDuration());
                                newPlague->SetDuration(oldPlague->GetDuration());
                                oldPlague->Remove();
                                GetCaster()->RemoveAurasDueToSpell(SPELL_UNBOUND_PLAGUE_SEARCHER);
                                GetCaster()->CastSpell(GetCaster(), SPELL_PLAGUE_SICKNESS, true);
                                GetCaster()->CastSpell(GetCaster(), SPELL_UNBOUND_PLAGUE_PROTECTION, true);
                                professor->CastSpell(GetHitUnit(), SPELL_UNBOUND_PLAGUE_SEARCHER, true);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_unbound_plague_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_putricide_unbound_plague_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_unbound_plague_SpellScript();
        }
};

class spell_putricide_eat_ooze : public SpellScriptLoader
{
    public:
        spell_putricide_eat_ooze() : SpellScriptLoader("spell_putricide_eat_ooze") { }

        class spell_putricide_eat_ooze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_eat_ooze_SpellScript);

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
                WorldObject* target = targets.front();
                targets.clear();
                targets.push_back(target);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Creature* target = GetHitCreature();
                if (!target)
                    return;

                if (Aura* grow = target->GetAura(uint32(GetEffectValue())))
                {
                    if (grow->GetStackAmount() < 3)
                    {
                        target->RemoveAurasDueToSpell(SPELL_GROW_STACKER);
                        target->RemoveAura(grow);
                        target->DespawnOrUnsummon(1);
                    }
                    else
                        grow->ModStackAmount(-3);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_eat_ooze_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_eat_ooze_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_eat_ooze_SpellScript();
        }
};

class spell_putricide_mutated_plague : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_plague() : SpellScriptLoader("spell_putricide_mutated_plague") { }

        class spell_putricide_mutated_plague_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_mutated_plague_AuraScript);

            void HandleTriggerSpell(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                uint32 triggerSpell = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(triggerSpell);
                spell = sSpellMgr->GetSpellForDifficultyFromSpell(spell, caster);

                int32 damage = spell->Effects[EFFECT_0].CalcValue(caster);
                float multiplier = GetTarget()->GetMap()->Is25ManRaid() ? 3.0f : 2.0f;

                damage *= int32(pow(multiplier, GetStackAmount()));
                damage = int32(damage * 1.5f);

                GetTarget()->CastCustomSpell(triggerSpell, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true, nullptr, aurEff, GetCasterGUID());
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                uint32 healSpell = uint32(GetSpellInfo()->Effects[EFFECT_0].CalcValue());
                SpellInfo const* healSpellInfo = sSpellMgr->GetSpellInfo(healSpell);

                if (!healSpellInfo)
                    return;

                int32 heal = healSpellInfo->Effects[0].CalcValue() * GetStackAmount();
                GetTarget()->CastCustomSpell(healSpell, SPELLVALUE_BASE_POINT0, heal, GetTarget(), true, nullptr, nullptr, GetCasterGUID());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_putricide_mutated_plague_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_putricide_mutated_plague_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_putricide_mutated_plague_AuraScript();
        }
};

class spell_putricide_mutated_plague_effect : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_plague_effect() : SpellScriptLoader("spell_putricide_mutated_plague_effect") { }

        class spell_putricide_mutated_plague_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutated_plague_effect_SpellScript);

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                if (GetCaster())
                {
                    if (GetCaster()->GetMap())
                    {
                        if (GetCaster()->GetMap()->IsDungeon())
                        {
                            Map::PlayerList const& players = GetCaster()->GetMap()->GetPlayers();

                            if (!players.isEmpty())
                            {
                                targets.clear();

                                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                                {
                                    Player* player = itr->GetSource();
                                    if (player && !player->IsGameMaster() && player->IsAlive())
                                    {
                                        targets.push_back(player);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_mutated_plague_effect_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_mutated_plague_effect_SpellScript();
        }
};

class spell_putricide_mutation_init : public SpellScriptLoader
{
    public:
        spell_putricide_mutation_init() : SpellScriptLoader("spell_putricide_mutation_init") { }

        class spell_putricide_mutation_init_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutation_init_SpellScript);

            SpellCastResult CheckRequirementInternal(SpellCustomErrors& extendedError)
            {
                InstanceScript* instance = GetExplTargetUnit()->GetInstanceScript();
                if (!instance)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Creature* professor = ObjectAccessor::GetCreature(*GetExplTargetUnit(), instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE));
                if (!professor)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (professor->GetVictim() && GetExplTargetUnit())
                    if (professor->GetVictim()->GetGUID() == GetExplTargetUnit()->GetGUID())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (professor->AI()->GetData(DATA_PHASE) == PHASE_COMBAT_3 || !professor->IsAlive())
                {
                    extendedError = SPELL_CUSTOM_ERROR_ALL_POTIONS_USED;
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                if (professor->AI()->GetData(DATA_ABOMINATION))
                {
                    extendedError = SPELL_CUSTOM_ERROR_TOO_MANY_ABOMINATIONS;
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            SpellCastResult CheckRequirement()
            {
                if (!GetExplTargetUnit())
                    return SPELL_FAILED_BAD_TARGETS;

                if (GetExplTargetUnit()->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_TARGET_NOT_PLAYER;

                SpellCustomErrors extension = SPELL_CUSTOM_ERROR_NONE;
                SpellCastResult result = CheckRequirementInternal(extension);
                if (result != SPELL_CAST_OK)
                {
                    Spell::SendCastResult(GetExplTargetUnit()->ToPlayer(), GetSpellInfo(), 0, result, extension);
                    return result;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_putricide_mutation_init_SpellScript::CheckRequirement);
            }
        };

        class spell_putricide_mutation_init_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_mutation_init_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), GetTarget()->GetMap()->Is25ManRaid() ? 71503 : 70311, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_putricide_mutation_init_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_mutation_init_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_putricide_mutation_init_AuraScript();
        }
};

class spell_putricide_mutated_transformation_dismiss : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_transformation_dismiss() : SpellScriptLoader("spell_putricide_mutated_transformation_dismiss") { }

        class spell_putricide_mutated_transformation_dismiss_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_mutated_transformation_dismiss_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Vehicle* veh = GetTarget()->GetVehicleKit())
                    veh->RemoveAllPassengers();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_putricide_mutated_transformation_dismiss_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_putricide_mutated_transformation_dismiss_AuraScript();
        }
};

class spell_putricide_mutated_transformation : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_transformation() : SpellScriptLoader("spell_putricide_mutated_transformation") { }

        class spell_putricide_mutated_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutated_transformation_SpellScript);

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetOriginalCaster();
                if (!caster)
                    return;

                InstanceScript* instance = caster->GetInstanceScript();
                if (!instance)
                    return;

                Creature* putricide = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(DATA_PROFESSOR_PUTRICIDE));
                if (!putricide)
                    return;

                if (putricide->GetVictim())
                    if (putricide->GetVictim()->GetGUID() == caster->GetGUID())
                        return;

                if (putricide->AI()->GetData(DATA_ABOMINATION))
                {
                    if (Player* player = caster->ToPlayer())
                        Spell::SendCastResult(player, GetSpellInfo(), 0, SPELL_FAILED_CUSTOM_ERROR, SPELL_CUSTOM_ERROR_TOO_MANY_ABOMINATIONS);
                    return;
                }

                // Remove shapeshift auras, otherwise player cannot enter vehicle
                caster->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);
                SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(uint32(GetSpellInfo()->Effects[effIndex].MiscValueB));
                uint32 duration = uint32(GetSpellInfo()->GetDuration());

                Position pos = caster->GetPosition();
                TempSummon* summon = caster->GetMap()->SummonCreature(entry, pos, properties, Milliseconds(duration), caster, GetSpellInfo()->Id);
                if (!summon || !summon->IsVehicle())
                    return;

                summon->CastSpell(summon, SPELL_ABOMINATION_VEHICLE_POWER_DRAIN, true);
                summon->CastSpell(summon, SPELL_MUTATED_TRANSFORMATION_DAMAGE, true);
                caster->CastSpell(summon, SPELL_MUTATED_TRANSFORMATION_NAME, true);

                caster->EnterVehicle(summon, 0);    // VEHICLE_SPELL_RIDE_HARDCODED is used according to sniff, this is ok
                summon->SetCreatorGUID(caster->GetGUID());
                putricide->AI()->JustSummoned(summon);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_putricide_mutated_transformation_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_mutated_transformation_SpellScript();
        }
};

class spell_putricide_mutated_transformation_dmg : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_transformation_dmg() : SpellScriptLoader("spell_putricide_mutated_transformation_dmg") { }

        class spell_putricide_mutated_transformation_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutated_transformation_dmg_SpellScript);

            void FilterTargetsInitial(std::list<WorldObject*>& targets)
            {
                if (Unit* owner = ObjectAccessor::GetUnit(*GetCaster(), GetCaster()->GetCreatorGUID()))
                    targets.remove(owner);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_mutated_transformation_dmg_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_mutated_transformation_dmg_SpellScript();
        }
};

class spell_putricide_regurgitated_ooze : public SpellScriptLoader
{
    public:
        spell_putricide_regurgitated_ooze() : SpellScriptLoader("spell_putricide_regurgitated_ooze") { }

        class spell_putricide_regurgitated_ooze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_regurgitated_ooze_SpellScript);

            // the only purpose of this hook is to fail the achievement
            void ExtraEffect(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->GetMap()->SetWorldState(WORLDSTATE_NOUSEA_HEARTBURN, 0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_regurgitated_ooze_SpellScript::ExtraEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_regurgitated_ooze_SpellScript();
        }
};

// Removes aura with id stored in effect value
class spell_putricide_clear_aura_effect_value : public SpellScriptLoader
{
    public:
        spell_putricide_clear_aura_effect_value() : SpellScriptLoader("spell_putricide_clear_aura_effect_value") { }

        class spell_putricide_clear_aura_effect_value_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_clear_aura_effect_value_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_clear_aura_effect_value_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_clear_aura_effect_value_SpellScript();
        }
};

// Stinky and Precious spell, it's here because its used for both (Festergut and Rotface "pets")
class spell_stinky_precious_decimate : public SpellScriptLoader
{
    public:
        spell_stinky_precious_decimate() : SpellScriptLoader("spell_stinky_precious_decimate") { }

        class spell_stinky_precious_decimate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stinky_precious_decimate_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit()->GetHealthPct() > float(GetEffectValue()))
                {
                    uint32 newHealth = GetHitUnit()->GetMaxHealth() * uint32(GetEffectValue()) / 100;
                    GetHitUnit()->SetHealth(newHealth);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_stinky_precious_decimate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stinky_precious_decimate_SpellScript();
        }
};

class spell_putricide_malleable_goo : public SpellScriptLoader
{
    public:
        spell_putricide_malleable_goo() : SpellScriptLoader("spell_putricide_malleable_goo") { }

        class spell_putricide_malleable_goo_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_malleable_goo_SpellScript);

            bool Load() override
            {
                return GetCaster() && GetCaster()->IsAIEnabled && GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                uint32 targetCount = dynamic_cast<ScriptedAI*>(GetCaster()->ToCreature()->AI())->RAID_MODE(1, 2, 1, 3);

                if (targets.size() > targetCount)
                {
                    std::list<WorldObject*> backup = targets;

                    // If not enough ranged players - pick among all players except the tank
                    targets.remove_if(DistanceCheck(GetCaster(), 7.0f));
                    if (targets.size() < targetCount)
                    {
                        targets = backup;
                        if (GetCaster()->GetVictim())
                            targets.remove(GetCaster()->GetVictim());
                    }
                }
                if (targets.empty())
                    return;

                Trinity::Containers::RandomResizeList(targets, targetCount);
                GetCaster()->ToCreature()->AI()->Talk(EMOTE_MALLEABLE_GOO);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell(GetHitUnit(), GetEffectValue());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_putricide_malleable_goo_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_putricide_malleable_goo_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        private:
            struct DistanceCheck
            {
                DistanceCheck(Unit* center, float dist) : _center(center), _dist(dist) { }
                bool operator()(WorldObject* target)
                {
                    if (Unit* unit = target->ToUnit())
                        return _dist > 0.0f && _center->IsWithinCombatRange(unit, _dist) || _dist < 0.0f && !_center->IsWithinCombatRange(unit, -_dist);
                    return false;
                }
                Unit* _center;
                float _dist;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_putricide_malleable_goo_SpellScript();
        }
};

void AddSC_boss_professor_putricide()
{
    new boss_professor_putricide();
    new npc_tentacle_stalker();
    new npc_putricide_ooze();
    new npc_malleable_ooze();
    new spell_putricide_gaseous_bloat();
    new spell_putricide_ooze_channel();
    new spell_putricide_slime_puddle_summon();
    new spell_putricide_slime_puddle();
    new spell_putricide_slime_puddle_aura();
    new spell_putricide_unstable_experiment();
    new spell_putricide_ooze_eruption_searcher();
    new spell_putricide_choking_gas_bomb();
    new spell_putricide_unbound_plague();
    new spell_putricide_eat_ooze();
    new spell_putricide_mutated_plague();
    new spell_putricide_mutation_init();
    new spell_putricide_mutated_transformation_dismiss();
    new spell_putricide_mutated_transformation();
    new spell_putricide_mutated_transformation_dmg();
    new spell_putricide_regurgitated_ooze();
    new spell_putricide_clear_aura_effect_value();
    new spell_stinky_precious_decimate();
    new spell_putricide_mutated_plague_effect();
    new spell_putricide_choking_gas_bomb_effect();
    new spell_putricide_malleable_goo();
}
