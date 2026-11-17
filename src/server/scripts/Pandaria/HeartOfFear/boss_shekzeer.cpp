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

enum Spells
{
    /*Grand Empress Shek`zeer*/
    SPELL_SERVANT_OF_THE_EMPRESS        = 123713, // charmed players to minion after 5 stuck 123707
    SPELL_SERVANT_OF_THE_EMPRESS_EFF    = 123723,
    SPELL_EYES_OF_THE_EMPRESS           = 123707, // max 5
    SPELL_SONIC_DISCHARGE               = 123504, // used by trigger after dissonance field destroy
    SPELL_DISSONANCE_FIELD              = 123255,
    SPELL_DISSONANCE_FIELD_KNOCK_BACK   = 123596,
    SPELL_DISSONANCE_FIELD_PROC         = 123184, // @TODO: rewrite spell burn on proc
    SPELL_DREAD_SCREECH                 = 123735,
    SPELL_CRY_OF_TERROR                 = 123788, // aura on player inficts dmg to all players [if not in dissonance]
    /*Phase 3*/
    SPELL_SHA_ENERGY                    = 125464, // same like  123735
    SPELL_CALAMITY                      = 124845, // dmg on 50% of current hp [need recalculate dmg]
    SPELL_CONSUMING_TERROR              = 124849, // cone aoe, inficts fear effect
    SPELL_AMASSING_DARKNESS             = 124844, // hit dmg
    SPELL_AMASSING_DARKNESS_AURA        = 124842, // periodic trigger spell for hit dmg and recast to nearby player
    SPELL_AMASSING_DARKNESS_SCRIPT      = 124843,
    SPELL_VISIONS_OF_DEMISE             = 124863,
    SPELL_VISION_OF_DEMISE_MARK         = 124862,

    /*Setthik Windblade*/
    SPELL_FIXATE                        = 125390, // follow to one target
    SPELL_DISPATCH                      = 124077, // aoe [interupted]
    SPELL_SONIC_BLADE                   = 125886, // 150% w.d. + interrupt cast
    SPELL_STICKY_RESIN                  = 124097, // aura decr speed+ dmg
    SPELL_STICKY_RESIN_TRANSFORM_AURA   = 124748,
    SPELL_STICKY_RESIN_TRIGGER_MISSLE   = 124092, // cast to player pos and form pool
    SPELL_STICKY_RESIN_POOL             = 124310, // just dummy
    SPELL_AMBER_TRAP                    = 125803, // triggered by
    SPELL_LARGE_RESIN_COMBINE           = 125704, // check in 4y players with resin
    SPELL_BUBBLING_AMBER                = 125719,
    SPELL_TRAPPED                       = 125824,

    /*Korthik Reaver*/
    SPELL_POISON_BOMB                   = 124777,
    SPELL_TOXIC_SLIME                   = 124807, // cone aoe
    SPELL_POISON_DRENCHED_ARMOR         = 124821, // aura, give a change to deal 75% dmg for nearby ally

    SPELL_BAND_OF_VALOR                 = 125422,

    /*Cosmetic*/
    SPELL_ULTIMATE_CORRAPTION           = 125451, // before 3td phase, Sha of fear inficts Grand Empress by Sha energy
    SPELL_NON_REGENERATE_POWER          = 72242,
    SPELL_RETREAT                       = 125098,
    SPELL_BERSERK                       = 26662,
    SPELL_SHA_CORRUPTION_EFF            = 125283,

    /*Heroic*/
    SPELL_HEART_OF_FEAR                 = 123846, // summon
    SPELL_HEART_OF_FEAR_AURA            = 123840,
    SPELL_HEART_OF_FEAR_LAUNCH          = 125639, // shadow chain?
    SPELL_HEART_OF_FEAR_TARGETING       = 123845,
    SPELL_HEART_OF_FEAR_EFF             = 125638,
    SPELL_CORRUPTED_DISS_FIELD          = 126121,
    SPELL_CORRUPTED_DISS_FEILD_VISUAL   = 126125,
};

enum Events
{
    // Shek`zeer
    EVENT_EYES_OF_THE_EMPRESS           = 1,
    EVENT_DISSONANCE_FIELD              = 2,
    EVENT_DREAD_SCREECH                 = 3,
    EVENT_CRY_OF_TERROR                 = 4,
    EVENT_SHA_ENERGY                    = 5,
    EVENT_CALAMITY                      = 6,
    EVENT_CONSUMING_TERROR              = 7,
    EVENT_AMASSING_DARKNESS             = 8,
    EVENT_VISIONS_OF_DEMISE             = 9,
    EVENT_MODIFY_POWER                  = 10,
    EVENT_LEAVE_CHRYSALIS               = 11,
    EVENT_CORRUPT_FIELD                 = 12,
    EVENT_BERSERK                       = 13,

    // Wind Blade
    EVENT_DISPATCH                      = 1,
    EVENT_SONIC_BLADE                   = 2,
    EVENT_STICKY_RESIN                  = 3,
    EVENT_FIXATE                        = 4,

    // Reaver
    EVENT_POISON_BOMB                   = 1,
    EVENT_TOXIC_SLIME                   = 2,
    EVENT_POISON_DRENCHED_ARMOR         = 3,

    // Sha of fear
    EVENT_FLY_AWAY                      = 1,

    // Cosmetic
    EVENT_INTRO                         = 1,
};

enum Phases
{
    FIRST_PHASE                         = 1,
    SECOND_PHASE                        = 2,
    THIRD_PHASE                         = 3
};

enum Actions
{
    ACTION_RETREAT                      = 0,
    ACTION_ROYAL_GUARDS                 = 1,
    ACTION_SHA_INTRO                    = 2,
    ACTION_SHA_AGGRO                    = 3,
    ACTION_EMPRESS_DEFEAT               = 4,
    ACTION_CONSUMING_TERROR             = 5,
    ACTION_SHEKZEER_INTRO               = 6,
};

enum Yells
{
    // Grand Empress Shek`zeer
    SAY_INTRO                           = 0,
    SAY_AGGRO                           = 1,
    SAY_CRY_OF_TERROR                   = 2,
    SAY_SLAY                            = 3,
    SAY_PHASE_2                         = 4,
    SAY_RESET                           = 5,
    SAY_RECHARGE                        = 6,
    SAY_DEATH                           = 7,

    // Sha of Fear
    SAY_AGGRO_S                         = 0,
    SAY_INTRO_S                         = 1,
    SAY_DEFEAT_S_1                      = 2,
    SAY_DEFEAT_S_2                      = 3
};

class boss_grand_empress_shekzeer : public CreatureScript
{
    public:
        boss_grand_empress_shekzeer() : CreatureScript("boss_grand_empress_shekzeer") { }

        struct boss_grand_empress_shekzeerAI : public BossAI
        {
            boss_grand_empress_shekzeerAI(Creature* creature) : BossAI(creature, DATA_SHEKZEER) { }

            EventMap nonCombatEvents, berserkEvents;
            uint32 _phases, introWp, guardianDied, delay;
            ObjectGuid targetGUID;
            bool intro;

            void Reset() override
            {
                _Reset();
                events.Reset();
                berserkEvents.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                _phases = FIRST_PHASE;
                targetGUID = ObjectGuid::Empty;
                guardianDied = 0;
                delay        = 0;
                SetPowerType();
                Talk(SAY_RESET);
                HandleRemoveTraps();
                HandleRemoveCharmedPlayers();

                if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                    Crysalis->ResetDoorOrButton();

                me->RemoveAurasDueToSpell(SPELL_RETREAT);
                me->RemoveAurasDueToSpell(SPELL_NON_REGENERATE_POWER);

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 117.8m in 10 normal
                    me->SetCreateHealth(196000000);
                    me->SetMaxHealth(196000000);
                    me->SetHealth(196000000);
                    me->ResetPlayerDamageReq();
                }
            }

            void InitializeAI() override
            {
                Reset();
                introWp = 1;
                intro = instance->GetData(DATA_SHEKZEER) > DONE ? true : false;

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 117.8m in 10 normal
                    me->SetCreateHealth(196000000);
                    me->SetMaxHealth(196000000);
                    me->SetHealth(196000000);
                    me->ResetPlayerDamageReq();
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VISION_OF_DEMISE_MARK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VISIONS_OF_DEMISE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRY_OF_TERROR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STICKY_RESIN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STICKY_RESIN_POOL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EYES_OF_THE_EMPRESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SERVANT_OF_THE_EMPRESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMASSING_DARKNESS_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_SHEKZEER, FAIL);
                    summons.DespawnAll();
                }
                HandleRemoveTraps();
                HandleRemoveCharmedPlayers();

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void HandleRemoveTraps()
            {
                std::list<Creature*> m_traps;
                GetCreatureListWithEntryInGrid(m_traps, me, NPC_STICKY_RESIN, 300.0f);
                GetCreatureListWithEntryInGrid(m_traps, me, NPC_BUBBLING_RESIN, 300.0f);
                GetCreatureListWithEntryInGrid(m_traps, me, NPC_AMBER_TRAP_SHEKZEER, 300.0f);

                for (auto &&iTraps : m_traps)
                    iTraps->DespawnOrUnsummon();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_SHEKZEER, FAIL);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_SHEKZEER, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    who->NearTeleportTo(EncountersEntrance[0].GetPositionX(), EncountersEntrance[0].GetPositionY(), EncountersEntrance[0].GetPositionZ(), EncountersEntrance[0].GetOrientation());
                    return;
                }

                // @TODO: Set in combat for other protectors
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(SAY_AGGRO);
                GetPhaseFirstEvents();
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 12 * MINUTE * IN_MILLISECONDS);
                me->AddAura(SPELL_NON_REGENERATE_POWER, me);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                // Set init combat only for royal guardians
                if (_phases == SECOND_PHASE && me->IsInCombat())
                    summon->AI()->DoZoneInCombat();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                            Crysalis->ResetDoorOrButton();

                        events.ScheduleEvent(EVENT_LEAVE_CHRYSALIS, 2 * MINUTE * IN_MILLISECONDS + 45 * IN_MILLISECONDS); // right timing
                        guardianDied = 0;
                        me->AI()->DoAction(ACTION_ROYAL_GUARDS);
                        break;
                    case 1:
                        if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                            Crysalis->ResetDoorOrButton();

                        introWp++;
                        nonCombatEvents.ScheduleEvent(EVENT_INTRO, 3000);
                        break;
                }
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_RETREAT:
                        _phases = SECOND_PHASE;
                        InChrysalis();
                        break;
                    case ACTION_ROYAL_GUARDS:
                        SpawnRoyalGuards();
                        break;
                    case ACTION_CONSUMING_TERROR:
                        me->RemoveChanneledCast(targetGUID);
                        break;
                    case ACTION_SHEKZEER_INTRO:
                    {
                        if (!intro)
                        {
                            intro = true;
                            Talk(SAY_INTRO);
                            me->AddAura(SPELL_RETREAT, me);

                            Movement::MoveSplineInit init(me);
                            init.MoveTo(EmpressCrysalis.GetPositionX(), EmpressCrysalis.GetPositionY(), EmpressCrysalis.GetPositionZ());
                            init.Launch();

                            if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                                Crysalis->ResetDoorOrButton();

                            delay = 0;
                            me->m_Events.Schedule(delay += me->GetSplineDuration(), 11, [this]()
                            {
                                if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                                    Crysalis->ResetDoorOrButton();

                                me->StopMoving();
                                nonCombatEvents.ScheduleEvent(EVENT_INTRO, 3000);
                            });
                        }
                        break;
                    }
                    case ACTION_ROYAL_GUARDIAN_DIE:
                        if (++guardianDied >= 8 && me->HasAura(SPELL_RETREAT))
                            events.RescheduleEvent(EVENT_LEAVE_CHRYSALIS, 1 * IN_MILLISECONDS);
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override 
            {
                if (HealthBelowPct(30) && _phases == FIRST_PHASE)
                {
                    _phases = THIRD_PHASE;

                    if (Creature* Sha = me->SummonCreature(NPC_SHA_OF_FEAR, ShaOfFearSpawn, TEMPSUMMON_MANUAL_DESPAWN))
                        Sha->AI()->DoAction(ACTION_SHA_AGGRO);

                    GetPhaseThirdEvents();
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
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VISION_OF_DEMISE_MARK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VISIONS_OF_DEMISE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRY_OF_TERROR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STICKY_RESIN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STICKY_RESIN_POOL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EYES_OF_THE_EMPRESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SERVANT_OF_THE_EMPRESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMASSING_DARKNESS_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }
                if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                    Crysalis->ResetDoorOrButton();

                if (Creature* Sha = me->SummonCreature(NPC_SHA_OF_FEAR, ShaWaypoints[0], TEMPSUMMON_MANUAL_DESPAWN))
                    Sha->AI()->DoAction(ACTION_EMPRESS_DEFEAT);

                HandleRemoveTraps();
                HandleRemoveCharmedPlayers(false);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INTRO)
                    {
                        if (!HasAnyGuardianInRange())
                        {
                            if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                                Crysalis->ResetDoorOrButton();

                            Movement::MoveSplineInit init(me);
                            init.MoveTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ());
                            init.Launch();
                            delay = 0;
                            me->m_Events.Schedule(delay += me->GetSplineDuration(), 10, [this]()
                            {
                                me->RemoveAurasDueToSpell(SPELL_RETREAT);

                                if (instance)
                                    instance->SetData(DATA_SHEKZEER, SPECIAL);
                            });
                        }
                        else 
                            nonCombatEvents.ScheduleEvent(EVENT_INTRO, 100);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_BERSERK)
                        DoCast(me, SPELL_BERSERK, true);
                    break;
                }

                events.Update(diff);

                if (me->GetPower(POWER_ENERGY) <= 2 && _phases == FIRST_PHASE)
                    me->AI()->DoAction(ACTION_RETREAT);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MODIFY_POWER:
                            ModifyPower();
                            events.ScheduleEvent(EVENT_MODIFY_POWER, urand(1000, 1500));
                            break;
                        case EVENT_LEAVE_CHRYSALIS:
                            LeaveChrysalis();
                            break;
                        case EVENT_CRY_OF_TERROR:
                            Talk(SAY_CRY_OF_TERROR);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                target->AddAura(SPELL_CRY_OF_TERROR, target);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, MeeleSpecTargetSelector()))
                                target->AddAura(SPELL_CRY_OF_TERROR, target);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, TankSpecTargetSelector()))
                                target->AddAura(SPELL_CRY_OF_TERROR, target);

                            events.ScheduleEvent(EVENT_CRY_OF_TERROR, urand(19000, 29000));
                            break;
                        case EVENT_DISSONANCE_FIELD:
                        {
                            // If any field not destroyed yet - make delay
                            if (Creature* field = GetClosestCreatureWithEntry(me, NPC_DISSONANCE_FIELD, 200.0f, true))
                            {
                                events.RescheduleEvent(EVENT_DISSONANCE_FIELD, 5 * IN_MILLISECONDS);
                                break;
                            }

                            for (uint32 i = 0; i < DISSONANCE_FIELD; i++)
                                me->SummonCreature(NPC_DISSONANCE_FIELD, DissonanceField[i].GetPositionX() + frand(-5.0f, 5.0f), DissonanceField[i].GetPositionY() + frand(-6.0f, 6.0f), DissonanceField[i].GetPositionZ(), DissonanceField[i].GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_CORRUPT_FIELD, urand(10, 11) * IN_MILLISECONDS);

                            events.ScheduleEvent(EVENT_DISSONANCE_FIELD, 1 * MINUTE * IN_MILLISECONDS + 5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_CORRUPT_FIELD:
                            me->CastSpell(me, SPELL_CORRUPTED_DISS_FEILD_VISUAL, false);
                            break;
                        case EVENT_DREAD_SCREECH:
                            DoCast(me, SPELL_DREAD_SCREECH);
                            events.ScheduleEvent(EVENT_DREAD_SCREECH, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_CALAMITY:
                            DoCastAOE(SPELL_CALAMITY);
                            events.ScheduleEvent(EVENT_CALAMITY, 18000);
                            break;
                        case EVENT_AMASSING_DARKNESS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                target->CastSpell(target, SPELL_AMASSING_DARKNESS_AURA, false);

                            events.ScheduleEvent(EVENT_AMASSING_DARKNESS, 85 * IN_MILLISECONDS);
                            break;
                        case EVENT_CONSUMING_TERROR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                if (Unit* victim = me->GetVictim())
                                    targetGUID = victim->GetGUID();

                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_CONSUMING_TERROR);
                            }
                            else if (Unit* target = me->GetVictim())
                            {
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_CONSUMING_TERROR);
                                targetGUID = target->GetGUID();
                            }

                            events.ScheduleEvent(EVENT_CONSUMING_TERROR, urand(15000, 25000));
                            break;
                        case EVENT_SHA_ENERGY:
                            DoCast(me, SPELL_SHA_ENERGY);
                            events.ScheduleEvent(EVENT_SHA_ENERGY, urand(8000, 15000));
                            break;
                        case EVENT_VISIONS_OF_DEMISE:
                        {
                            std::list<Player*> PlayersInArea;
                            GetPlayerListInGrid(PlayersInArea, me, VISIBLE_RANGE);
                            PlayersInArea.remove_if(TankSpecTargetSelector());

                            if (!PlayersInArea.empty())
                            {
                                if (PlayersInArea.size() > 2)
                                    PlayersInArea.resize(2);

                                for (auto&& itr : PlayersInArea)
                                    me->AddAura(SPELL_VISION_OF_DEMISE_MARK, itr);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                me->AddAura(SPELL_VISION_OF_DEMISE_MARK, target);

                            events.ScheduleEvent(EVENT_VISIONS_OF_DEMISE, urand(25000, 39000));
                            break;
                        }
                        case EVENT_EYES_OF_THE_EMPRESS:
                            if (Unit* victim = me->GetVictim())
                                me->CastSpell(victim, SPELL_EYES_OF_THE_EMPRESS, false);

                            events.ScheduleEvent(EVENT_EYES_OF_THE_EMPRESS, urand(10000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

        private:
            void GetPhaseFirstEvents()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_EYES_OF_THE_EMPRESS, 10000);
                events.ScheduleEvent(EVENT_DISSONANCE_FIELD, 23 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DREAD_SCREECH, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CRY_OF_TERROR, urand(19000,29000));
                events.ScheduleEvent(EVENT_MODIFY_POWER, 1000);
            }

            void GetPhaseThirdEvents()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CALAMITY, 18000);
                events.ScheduleEvent(EVENT_CONSUMING_TERROR, urand(15000, 25000));
                events.ScheduleEvent(EVENT_SHA_ENERGY, urand(8000, 15000));
                events.ScheduleEvent(EVENT_AMASSING_DARKNESS, urand(20000, 39000));
                events.ScheduleEvent(EVENT_VISIONS_OF_DEMISE, urand(25000, 39000));
            }

            void SetPowerType()
            {
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 150);
                me->SetPower(POWER_ENERGY, 150);
            }

            void ModifyPower()
            {
                if (me->GetPower(POWER_ENERGY) > 0)
                    me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) - urand(1, 2));
            }

            void InChrysalis()
            {
                events.Reset();
                Talk(SAY_PHASE_2);
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->AddAura(SPELL_RETREAT, me);
                me->GetMotionMaster()->MovePoint(0, EmpressCrysalis);

                if (me->HasAura(SPELL_NON_REGENERATE_POWER))
                    me->RemoveAura(SPELL_NON_REGENERATE_POWER);

                if (GameObject* Crysalis = GetClosestGameObjectWithEntry(me, GO_EMPRESS_CHRYSALIS, 150.0f))
                    Crysalis->UseDoorOrButton();

            }

            void LeaveChrysalis()
            {
                me->RemoveAurasDueToSpell(SPELL_RETREAT);
                _phases = FIRST_PHASE;
                guardianDied = 0;
                SetPowerType();
                GetPhaseFirstEvents();
                me->SetReactState(REACT_AGGRESSIVE);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                {
                    me->GetMotionMaster()->MoveChase(target, 0, me->GetAngle(target));
                    me->Attack(target, true);
                }
                Talk(SAY_RECHARGE);
                if (Creature* Sha = me->SummonCreature(NPC_SHA_OF_FEAR, ShaOfFearSpawn, TEMPSUMMON_MANUAL_DESPAWN))
                    Sha->AI()->DoAction(ACTION_SHA_INTRO);

                if (!me->HasAura(SPELL_NON_REGENERATE_POWER))
                    me->AddAura(SPELL_NON_REGENERATE_POWER, me);
            }

            void SpawnRoyalGuards()
            {
                for (uint32 i = 0; i < ROYAL_GUARDS; i++)
                {
                    for (uint32 j = 0; j < ROYAL_GUARDS + 1; j++)
                        me->SummonCreature(NPC_SETTHIK_WINDBLADE, RoyalGuardsSpawn[i].GetPositionX() + frand(-8.0f, 8.0f), RoyalGuardsSpawn[i].GetPositionY() + frand(-5.0f, 5.0f), RoyalGuardsSpawn[i].GetPositionZ(), RoyalGuardsSpawn[i].GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                    me->SummonCreature(NPC_KORTHIK_REAVER, RoyalGuardsSpawn[i], TEMPSUMMON_MANUAL_DESPAWN);
                }
            }

            bool HasAnyGuardianInRange()
            {
                std::list<Creature*> Guardians;
                GetCreatureListWithEntryInGrid(Guardians, me, NPC_ZARTHIK_AUGURER, 150.0f);
                GetCreatureListWithEntryInGrid(Guardians, me, NPC_KORTHIK_WARSINGER, 150.0f);
                GetCreatureListWithEntryInGrid(Guardians, me, NPC_SETTHIK_WINDBLADE, 150.0f);

                for (auto &&itr : Guardians)
                    if (itr->IsAlive()) // once check
                        return true;

                return false;
            }

            void HandleRemoveCharmedPlayers(bool death = true)
            {
                std::list<Player*> m_targets;
                GetPlayerListInGrid(m_targets, me, 300.0f);

                for (auto&& itr : m_targets)
                    if (itr->IsCharmed())
                        death ? me->Kill(itr) : itr->RemoveCharmAuras();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_grand_empress_shekzeerAI(creature);
        }
};

struct royal_guards_typeAI : public ScriptedAI
{
    royal_guards_typeAI(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap events;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
    }

    void JustDied(Unit* /*killer*/) override 
    {
        if (Creature* Shekzeer = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SHEKZEER) : ObjectGuid::Empty))
            Shekzeer->AI()->DoAction(ACTION_ROYAL_GUARDIAN_DIE);
    } 
};

// Set`thik Windblade 64453
class npc_setthik_windblade : public CreatureScript
{
    public:
        npc_setthik_windblade() : CreatureScript("npc_setthik_windblade") { }

        struct npc_setthik_windbladeAI : public royal_guards_typeAI
        {
            npc_setthik_windbladeAI(Creature* creature) : royal_guards_typeAI(creature) { }

            ObjectGuid targetGUID;

            void IsSummonedBy(Unit* summoner) override
            {
                targetGUID = ObjectGuid::Empty;
                SetEquipmentSlots(false, MELJARAK_2H_WEAPON, 0, EQUIP_NO_CHANGE);

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                {
                    me->GetMotionMaster()->MoveChase(target, 0, me->GetAngle(target));
                    me->Attack(target, true);
                }
                events.ScheduleEvent(EVENT_DISPATCH, urand(9000, 29000));
                events.ScheduleEvent(EVENT_SONIC_BLADE, urand(8000, 9500));
                events.ScheduleEvent(EVENT_STICKY_RESIN, urand(12000, 39000));
                events.ScheduleEvent(EVENT_FIXATE, urand(4000, 16000));
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_MISC)
                    events.ScheduleEvent(EVENT_FIXATE, urand(4000, 16000));
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
                        case EVENT_DISPATCH:
                            DoCast(me, SPELL_DISPATCH);
                            events.ScheduleEvent(EVENT_DISPATCH, urand(29000, 49000));
                            break;
                        case EVENT_SONIC_BLADE:
                            if (Unit* Victim = me->GetVictim())
                                me->CastSpell(Victim, SPELL_SONIC_BLADE, false);

                            events.ScheduleEvent(EVENT_SONIC_BLADE, urand(8000, 9500));
                            break;
                        case EVENT_STICKY_RESIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                me->CastSpell(target, SPELL_STICKY_RESIN_TRIGGER_MISSLE, false);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, TankSpecTargetSelector()))
                                me->CastSpell(target, SPELL_STICKY_RESIN_TRIGGER_MISSLE, false);

                            events.ScheduleEvent(EVENT_STICKY_RESIN, urand(12000, 39000));
                            break;
                        case EVENT_FIXATE:
                            DoCast(me, SPELL_FIXATE);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_setthik_windbladeAI(creature);
        }
};

// Bubbling Resin 64383
class npc_bubbling_resin : public CreatureScript
{
    public:
        npc_bubbling_resin() : CreatureScript("npc_bubbling_resin") { }

        struct npc_bubbling_resinAI : public ScriptedAI
        {
            npc_bubbling_resinAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                me->CastSpell(me, SPELL_BUBBLING_AMBER, false);
                me->CastSpell(me, SPELL_LARGE_RESIN_COMBINE, false);
                me->SetDisplayId(43536);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bubbling_resinAI(creature);
        }
};

// Sticky Resin 63730
class npc_sticky_resin : public CreatureScript
{
    public:
        npc_sticky_resin() : CreatureScript("npc_sticky_resin") { }

        struct npc_sticky_resinAI : public ScriptedAI
        {
            npc_sticky_resinAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap events;
            uint32 m_rResin;

            void IsSummonedBy(Unit* summoner) override
            {
                m_rResin = 0;
                me->SetDisplayId(31379);
                me->CastSpell(me, SPELL_STICKY_RESIN_POOL, false);
                events.ScheduleEvent(EVENT_STICKY_RESIN, 2 * IN_MILLISECONDS);
            }

            void DoAction(int32 /*actionId*/) override { }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_TARGET_ID)
                    return m_rResin;
                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_TARGET_ID)
                    m_rResin = data;
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_STICKY_RESIN)
                        m_rResin = 1;
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sticky_resinAI(creature);
        }
};

// Kor`thik Reaver 63591
class npc_korthik_reaver : public CreatureScript
{
    public:
        npc_korthik_reaver() : CreatureScript("npc_korthik_reaver") { }

        struct npc_korthik_reaverAI : public royal_guards_typeAI
        {
            npc_korthik_reaverAI(Creature* creature) : royal_guards_typeAI(creature) { }

            void IsSummonedBy(Unit* summoner) override 
            {
                SetEquipmentSlots(false, MELJARAK_2H_WEAPON, 0, EQUIP_NO_CHANGE); 

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                {
                    me->GetMotionMaster()->MoveChase(target, 0, me->GetAngle(target));
                    me->Attack(target, true);
                }

                events.ScheduleEvent(EVENT_POISON_BOMB, 8000);
                events.ScheduleEvent(EVENT_TOXIC_SLIME, 19000);
                events.ScheduleEvent(EVENT_POISON_DRENCHED_ARMOR, urand(12000, 16000));
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
                        case EVENT_POISON_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                                me->CastSpell(target, SPELL_POISON_BOMB, false);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, TankSpecTargetSelector()))
                                me->CastSpell(target, SPELL_POISON_BOMB, false);

                            events.ScheduleEvent(EVENT_POISON_BOMB, urand(15000, 25000));
                            break;
                        case EVENT_TOXIC_SLIME:
                            if (Unit* Victim = me->GetVictim())
                                me->CastSpell(Victim, SPELL_TOXIC_SLIME, false);

                            events.ScheduleEvent(EVENT_TOXIC_SLIME, urand(19000, 29000));
                            break;
                        case EVENT_POISON_DRENCHED_ARMOR:
                            if (Unit* Victim = me->GetVictim())
                                me->CastSpell(Victim, SPELL_POISON_DRENCHED_ARMOR, false);

                            events.ScheduleEvent(EVENT_POISON_DRENCHED_ARMOR, urand(19000, 29000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                // The only criteria (linked to achievement) that has 0 and 0...
                // May be this is CRITERIA_START_TYPE_CREATURE but I don't realy think so, where are must be more than one achievement with this criteria
                // And yep this must be here, JustDied called after the criteria update
                if (instance && damage >= me->GetHealth())
                    instance->DoStartCriteria(CriteriaStartTypes(0), 0);

                royal_guards_typeAI::DamageTaken(attacker, damage);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_korthik_reaverAI(creature);
        }
};

// Dissonance field 62847
class npc_dissonance_field : public CreatureScript
{
    public:
        npc_dissonance_field() : CreatureScript("npc_dissonance_field") { }

        struct npc_dissonance_fieldAI : public ScriptedAI
        {
            npc_dissonance_fieldAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool toBeDespawned = false;

            void IsSummonedBy(Unit* summoner) override 
            { 
                me->setRegeneratingHealth(false);
                me->SetMaxHealth(200000);
                me->SetHealth(200000);
                me->CastSpell(me, SPELL_DISSONANCE_FIELD, false); 
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void DoAction(int32 actionId) override 
            {
                if (actionId == ACTION_SHA_AGGRO)
                    events.ScheduleEvent(EVENT_INTRO, 1500);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->GetHealth() <= damage && !toBeDespawned)
                {
                    toBeDespawned = true;
                    me->CastSpell(me, SPELL_SONIC_DISCHARGE, false);
                    me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_INTRO)
                        me->AddAura(SPELL_CORRUPTED_DISS_FIELD, me);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dissonance_fieldAI(creature);
        }
};

// Sha of Fear 63942
class npc_sha_of_fear_shekzeer : public CreatureScript
{
    public:
        npc_sha_of_fear_shekzeer() : CreatureScript("npc_sha_of_fear_shekzeer") { }

        struct npc_sha_of_fear_shekzeerAI : public ScriptedAI
        {
            npc_sha_of_fear_shekzeerAI(Creature* creature) : ScriptedAI(creature) { }
            EventMap nonCombatEvents;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override 
            {
                instance = me->GetInstanceScript();
                me->SetCanFly(true);
                me->SetDisableGravity(true); 
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void JustDied(Unit* /*killer*/) override { }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_SHA_INTRO:
                        Talk(SAY_INTRO_S);
                        me->DespawnOrUnsummon(7000);
                        break;
                    case ACTION_SHA_AGGRO:
                        Talk(SAY_AGGRO_S);
                        if (Unit* Shekzeer = ObjectAccessor::GetUnit(*me, instance ? instance->GetGuidData(DATA_SHEKZEER) : ObjectGuid::Empty))
                        {
                            me->CastSpell(Shekzeer, SPELL_ULTIMATE_CORRAPTION, false);

                            if (IsHeroic())
                                Shekzeer->CastSpell(Shekzeer, SPELL_HEART_OF_FEAR, false);
                        }
                        break;
                    case ACTION_EMPRESS_DEFEAT:
                        Talk(SAY_DEFEAT_S_1);
                        nonCombatEvents.ScheduleEvent(EVENT_FLY_AWAY, 8500);
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_FLY_AWAY)
                    {
                        Talk(SAY_DEFEAT_S_2);
                        if (GameObject* Top = GetClosestGameObjectWithEntry(me, GO_TOP_HEART_OF_FEAR, 200.0f))
                            Top->UseDoorOrButton();

                        me->GetMotionMaster()->MovePoint(0, ShaWaypoints[1]);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sha_of_fear_shekzeerAI(creature);
        }
};

// Heart of Fear 63445
struct npc_heart_of_fear : public ScriptedAI
{
    npc_heart_of_fear(Creature* creature) : ScriptedAI(creature) 
    {
        SetCombatMovement(false);
    }
    InstanceScript* instance;
    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        me->SetDisplayId(11686);
        DoCast(me, SPELL_HEART_OF_FEAR_AURA, true);
        me->SetCanFly(true);
        me->SetDisableGravity(true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 17.0f, me->GetOrientation());

        DoZoneInCombat(me, 200.0f);

        scheduler
            .CancelAll()
            .Schedule(Seconds(10), [this](TaskContext channelEvent)
        {
            DoCast(me, SPELL_HEART_OF_FEAR_TARGETING);
            channelEvent.Repeat(Seconds(10));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Dissonance field 123255
class spell_dissonance_field : public SpellScriptLoader
{
    public:
        spell_dissonance_field() : SpellScriptLoader("spell_dissonance_field") { }

        class spell_dissonance_field_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dissonance_field_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (Unit* caster = GetCaster())
                {
                    caster->DealDamage(caster, caster->GetMaxHealth() * 0.01, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                    std::list<Player*> PlayersInDissonance;
                    GetPlayerListInGrid(PlayersInDissonance, caster, 10.0f);

                    for (auto &&itr : PlayersInDissonance)
                        if (itr->GetCurrentSpell(CURRENT_CHANNELED_SPELL) || itr->HasAura(SPELL_CRY_OF_TERROR))
                            caster->DealDamage(caster, caster->GetMaxHealth() * 0.01, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dissonance_field_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dissonance_field_AuraScript();
        }
};

// Corrupted Dissonance Field 126121
class spell_corrupted_dissonance_field : public SpellScriptLoader
{
    public:
        spell_corrupted_dissonance_field() : SpellScriptLoader("spell_corrupted_dissonance_field") { }

        class spell_corrupted_dissonance_field_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_corrupted_dissonance_field_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->DealDamage(caster, caster->GetMaxHealth() * 0.01, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                    std::list<Player*> PlayersInDissonance;
                    GetPlayerListInGrid(PlayersInDissonance, caster, 10.0f);

                    for (auto &&itr : PlayersInDissonance)
                        if (itr->GetCurrentSpell(CURRENT_CHANNELED_SPELL) || itr->HasAura(SPELL_CRY_OF_TERROR))
                            caster->DealDamage(caster, caster->GetMaxHealth() * 0.02, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_corrupted_dissonance_field_AuraScript::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        class spell_corrupted_dissonance_field_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_corrupted_dissonance_field_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* m_caster = GetCaster())
                    if (m_caster->ToCreature())
                        m_caster->ToCreature()->AI()->DoAction(ACTION_SHA_AGGRO);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_corrupted_dissonance_field_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_REMOVE_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_corrupted_dissonance_field_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_corrupted_dissonance_field_AuraScript();
        }
};

// Dissonance field knockback 123596
class spell_dissonance_field_knockback : public SpellScriptLoader
{
    public:
        spell_dissonance_field_knockback() : SpellScriptLoader("spell_dissonance_field_knockback") { }

        class spell_dissonance_field_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dissonance_field_knockback_SpellScript);

            void HandlePreventEffect(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (GetHitUnit()->HasAura(SPELL_CRY_OF_TERROR))
                    PreventHitEffect(EFFECT_0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_dissonance_field_knockback_SpellScript::HandlePreventEffect, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dissonance_field_knockback_SpellScript();
        }
};

class AmazingDarknessTargetSelector
{
    public:
        AmazingDarknessTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToPlayer() && object->ToPlayer()->HasAura(SPELL_AMASSING_DARKNESS_AURA);
        }
};

// Amassing darkness effect 124843
class spell_amassing_darkness_effect : public SpellScriptLoader
{
    public:
        spell_amassing_darkness_effect() : SpellScriptLoader("spell_amassing_darkness_effect") { }

        class spell_amassing_darkness_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_amassing_darkness_effect_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                InstanceScript* instance = GetCaster()->GetInstanceScript();
                if (!instance)
                    return;

                if (Unit* m_caster = GetCaster())
                {
                    m_caster->CastSpell(m_caster, SPELL_AMASSING_DARKNESS, false);

                    std::list<Player*> AmazingDarknessAffected;
                    GetPlayerListInGrid(AmazingDarknessAffected, m_caster, VISIBLE_RANGE);
                    // Remove already affected by this spell players, it working like pyramids
                    AmazingDarknessAffected.remove_if(AmazingDarknessTargetSelector());

                    if (!AmazingDarknessAffected.empty())
                    {
                        if (Player* pTarget = Trinity::Containers::SelectRandomContainerElement(AmazingDarknessAffected))
                            pTarget->CastSpell(pTarget, SPELL_AMASSING_DARKNESS_AURA, false);
                    }
                    else
                    {
                        // If it had hit to each player, then remove whole
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AMASSING_DARKNESS_AURA);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_amassing_darkness_effect_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_amassing_darkness_effect_SpellScript();
        }
};

// Calamity 124845
class spell_calamity : public SpellScriptLoader
{
    public:
        spell_calamity() : SpellScriptLoader("spell_calamity") { }

        class spell_calamity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_calamity_SpellScript);

            void DealDamage()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                SetHitDamage(target->GetHealth()*0.5);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_calamity_SpellScript::DealDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_calamity_SpellScript();
        }
};

// Consuming Terror 124849
class spell_consuming_terror : public SpellScriptLoader
{
    public:
        spell_consuming_terror() : SpellScriptLoader("spell_consuming_terror") { }

        class spell_consuming_terror_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_consuming_terror_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature() && caster->ToCreature()->AI())
                        caster->ToCreature()->AI()->DoAction(ACTION_CONSUMING_TERROR);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_consuming_terror_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_consuming_terror_SpellScript();
        }
};

// 123792 Cry of Terror
class spell_cry_of_terror : public SpellScriptLoader
{
    public:
        spell_cry_of_terror() : SpellScriptLoader("spell_cry_of_terror") { }

        class spell_cry_of_terror_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cry_of_terror_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<Creature*> DissonanceFields;
                    GetCreatureListWithEntryInGrid(DissonanceFields, caster, NPC_DISSONANCE_FIELD, 50.0f);

                    if (DissonanceFields.empty())
                        return;

                    for (auto &&itr : DissonanceFields)
                    {
                        if (caster->GetDistance2d(itr) < 7.0f)
                        {
                            SetHitDamage(0); // check only once
                            return;
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cry_of_terror_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cry_of_terror_SpellScript();
        }
};

// Retreat Aura 125098
class spell_hof_retreat : public SpellScriptLoader
{
    public:
        spell_hof_retreat() : SpellScriptLoader("spell_hof_retreat") { }

        class spell_hof_retreat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hof_retreat_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* owner = GetOwner()->ToCreature())
                {
                    owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    owner->SetReactState(REACT_PASSIVE);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* owner = GetOwner()->ToCreature())
                {
                    owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    owner->SetReactState(REACT_AGGRESSIVE);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_hof_retreat_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_hof_retreat_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hof_retreat_AuraScript();
        }
};

// Sticky Resin Dummy 124310
class spell_sticky_resin_pool : public SpellScriptLoader
{
    public:
        spell_sticky_resin_pool() : SpellScriptLoader("spell_sticky_resin_pool") { }

        class spell_sticky_resin_pool_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sticky_resin_pool_AuraScript);

            void OnTrigger(AuraEffect const* /*aurEff*/)
            {
                if (GetOwner() && GetOwner()->ToCreature() && GetOwner()->ToCreature()->AI()->GetData(TYPE_TARGET_ID) == 1)
                {
                    if (Player* itr = GetOwner()->FindNearestPlayer(1.5f))
                    {
                        if (!itr->HasAura(SPELL_STICKY_RESIN))
                        {
                            itr->CastSpell(itr, SPELL_STICKY_RESIN, true);
                            GetOwner()->ToCreature()->DespawnOrUnsummon();
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sticky_resin_pool_AuraScript::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sticky_resin_pool_AuraScript();
        }
};

// Sticky Resin Aura 124097
class spell_sticky_resin_aura : public SpellScriptLoader
{
    public:
        spell_sticky_resin_aura() : SpellScriptLoader("spell_sticky_resin_aura") { }

        class spell_sticky_resin_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sticky_resin_aura_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    owner->CastSpell(owner, SPELL_STICKY_RESIN_POOL, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    owner->RemoveAurasDueToSpell(SPELL_STICKY_RESIN_POOL);
            }

            void OnResinCross(AuraEffect const* /*aurEff*/)
            {
                if (Unit* m_owner = GetOwner()->ToUnit())
                {
                    std::list<Player*> AffectedResinPlayers;
                    GetPlayerListInGrid(AffectedResinPlayers, m_owner, 1.5f);

                    for (auto &&itr : AffectedResinPlayers)
                    {
                        // Exept us and players without debuff or if we could find near bubble resin
                        if (!itr->HasAura(SPELL_STICKY_RESIN) || itr->GetGUID() == m_owner->GetGUID() || GetClosestCreatureWithEntry(m_owner, NPC_BUBBLING_RESIN, 4.0f, true))
                            continue;

                        itr->RemoveAurasDueToSpell(SPELL_STICKY_RESIN);
                        m_owner->RemoveAurasDueToSpell(SPELL_STICKY_RESIN);
                        m_owner->SummonCreature(NPC_BUBBLING_RESIN, *m_owner, TEMPSUMMON_MANUAL_DESPAWN);
                        return;
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_sticky_resin_aura_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_sticky_resin_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sticky_resin_aura_AuraScript::OnResinCross, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sticky_resin_aura_AuraScript();
        }
};

// Large Resin Combine 125705
class spell_large_resin_combine : public SpellScriptLoader
{
    public:
        spell_large_resin_combine() : SpellScriptLoader("spell_large_resin_combine") { }

        class spell_large_resin_combine_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_large_resin_combine_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* m_caster = GetCaster())
                {
                    if (Unit* m_target = GetHitUnit())
                    {
                        if (m_target->HasAura(SPELL_STICKY_RESIN))
                        {
                            m_target->RemoveAurasDueToSpell(SPELL_STICKY_RESIN);
                            m_caster->CastSpell(m_caster, SPELL_STICKY_RESIN_TRANSFORM_AURA, false);

                            if (Aura* m_aResin = m_caster->GetAura(SPELL_STICKY_RESIN_TRANSFORM_AURA))
                            {
                                // Transform to Amber Trap if more or equal 6 stucks
                                if (m_aResin->GetStackAmount() >= 5)
                                {
                                    m_caster->RemoveAurasDueToSpell(SPELL_STICKY_RESIN_TRANSFORM_AURA);
                                    m_caster->RemoveAurasDueToSpell(SPELL_BUBBLING_AMBER);
                                    m_caster->CastSpell(m_caster, SPELL_AMBER_TRAP, false);
                                    m_caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_large_resin_combine_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_large_resin_combine_SpellScript();
        }
};

class CorruptedFieldTargetSelector
{
    public:
        CorruptedFieldTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->GetEntry() != NPC_DISSONANCE_FIELD;
        }
};

// Corrupted Field Visual 126125
class spell_corrupted_field_visual : public SpellScriptLoader
{
    public:
        spell_corrupted_field_visual() : SpellScriptLoader("spell_corrupted_field_visual") { }

        class spell_corrupted_field_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_corrupted_field_visual_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(CorruptedFieldTargetSelector());

                if (targets.size() > 1)
                    targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_corrupted_field_visual_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_corrupted_field_visual_SpellScript();
        }
};

class TrappedPredicate
{
    public:
        TrappedPredicate() { }

        bool operator()(WorldObject* object)
        {
            return object && object->GetEntry() != NPC_SETTHIK_WINDBLADE && object->GetEntry() != NPC_KORTHIK_REAVER;
        }
};

// Trapped! 125824
class spell_bubbling_amber_trapped : public SpellScriptLoader
{
    public:
        spell_bubbling_amber_trapped() : SpellScriptLoader("spell_bubbling_amber_trapped") { }

        class spell_bubbling_amber_trapped_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bubbling_amber_trapped_SpellScript);

            void CheckTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(TrappedPredicate());

                if (targets.size() > 1)
                    targets.resize(1);
            }

            void HandleHitEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* m_caster = GetHitUnit())
                    m_caster->RemoveAurasDueToSpell(SPELL_AMBER_TRAP);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_bubbling_amber_trapped_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_bubbling_amber_trapped_SpellScript::HandleHitEffect, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bubbling_amber_trapped_SpellScript();
        }
};

// Eyes of the Empress 123707
class spell_eyes_of_the_empress : public SpellScript
{
    PrepareSpellScript(spell_eyes_of_the_empress);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (Aura* eyesAura = target->GetAura(SPELL_EYES_OF_THE_EMPRESS))
                if (eyesAura->GetStackAmount() > 4)
                    target->CastSpell(target, SPELL_SERVANT_OF_THE_EMPRESS, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_eyes_of_the_empress::HandleEffectHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// Servant of the Empress 123713
class spell_servant_of_the_empress : public AuraScript
{
    PrepareAuraScript(spell_servant_of_the_empress);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (Creature* shekzeer = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(DATA_SHEKZEER)))
                    shekzeer->CastSpell(caster, SPELL_SERVANT_OF_THE_EMPRESS_EFF, true);
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (Creature* Shekzeer = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(DATA_SHEKZEER)))
                    caster->RemoveAurasDueToSpell(SPELL_SERVANT_OF_THE_EMPRESS_EFF);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_servant_of_the_empress::OnApply, EFFECT_7, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_servant_of_the_empress::OnRemove, EFFECT_7, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// Corruption 130756
class spell_dissonance_field_corruption : public SpellScript
{
    PrepareSpellScript(spell_dissonance_field_corruption);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            uint32 maxVictims = caster->GetInstanceScript()->instance->Is25ManRaid() ? 7 : 3;

            if (targets.size() > maxVictims)
                Trinity::Containers::RandomResizeList(targets, maxVictims);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dissonance_field_corruption::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Heart of Fear Selector 123845
class spell_heart_of_fear_selector_aura : public AuraScript
{
    PrepareAuraScript(spell_heart_of_fear_selector_aura);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->CastSpell(owner, SPELL_HEART_OF_FEAR_EFF, false);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_heart_of_fear_selector_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Heart of Fear Selector 123845
class spell_heart_of_fear_selector : public SpellScript
{
    PrepareSpellScript(spell_heart_of_fear_selector);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove tanks
            pList.remove_if(TankSpecTargetSelector());

            // If we not found any dps then try select anyone without same aura
            if (pList.empty())
            {
                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            targets.clear();

            // Doesn`t matter push only 1 target or more cuz maxAffectTargets is 1
            for (auto&& itr : pList)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_heart_of_fear_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Fixate 125390
class spell_windblade_fixate_aura : public AuraScript
{
    PrepareAuraScript(spell_windblade_fixate_aura);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->ToCreature() && caster->ToCreature()->AI())
                caster->ToCreature()->AI()->DoAction(ACTION_MISC);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_windblade_fixate_aura::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Fixate 125390
class spell_windblade_fixate : public SpellScript
{
    PrepareSpellScript(spell_windblade_fixate);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = GetHitUnit())
                caster->FixateOnTarget(target->GetGUID(), 15 * IN_MILLISECONDS);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove tanks
            pList.remove_if(TankSpecTargetSelector());

            // If we not found any dps then try select anyone without same aura
            if (pList.empty())
            {
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

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_windblade_fixate::HandleEffectHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_windblade_fixate::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// AreaTrigger 8565
class AreaTrigger_at_behind_shekzeer : public AreaTriggerScript
{
    public:
        AreaTrigger_at_behind_shekzeer() : AreaTriggerScript("AreaTrigger_at_behind_shekzeer") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* Shekzeer = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SHEKZEER)))
                    Shekzeer->AI()->DoAction(ACTION_SHEKZEER_INTRO);

            return false;
        }
};

// 380. Summoned by 125270 - Sha Corruption
class sat_shekzeer_sha_corruption : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_SHA_CORRUPTION_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SHA_CORRUPTION_EFF);
    }
};

void AddSC_boss_shekzeer()
{
    new boss_grand_empress_shekzeer();
    new npc_setthik_windblade();
    new npc_bubbling_resin();
    new npc_sticky_resin();
    new npc_korthik_reaver();
    new npc_dissonance_field();
    new npc_sha_of_fear_shekzeer();
    new creature_script<npc_heart_of_fear>("npc_heart_of_fear");
    new spell_dissonance_field();
    new spell_corrupted_dissonance_field();
    new spell_dissonance_field_knockback();
    new spell_amassing_darkness_effect();
    new spell_calamity();
    new spell_consuming_terror();
    new spell_cry_of_terror();
    new spell_hof_retreat();
    new spell_sticky_resin_pool();
    new spell_sticky_resin_aura();
    new spell_large_resin_combine();
    new spell_corrupted_field_visual();
    new spell_bubbling_amber_trapped();
    new spell_script<spell_eyes_of_the_empress>("spell_eyes_of_the_empress");
    new aura_script<spell_servant_of_the_empress>("spell_servant_of_the_empress");
    new spell_script<spell_dissonance_field_corruption>("spell_dissonance_field_corruption");
    new aura_script<spell_heart_of_fear_selector_aura>("spell_heart_of_fear_selector_aura");
    new spell_script<spell_heart_of_fear_selector>("spell_heart_of_fear_selector");
    new aura_script<spell_windblade_fixate_aura>("spell_windblade_fixate_aura");
    new spell_script<spell_windblade_fixate>("spell_windblade_fixate");
    new AreaTrigger_at_behind_shekzeer();
    new atrigger_script<sat_shekzeer_sha_corruption>("sat_shekzeer_sha_corruption");
}
