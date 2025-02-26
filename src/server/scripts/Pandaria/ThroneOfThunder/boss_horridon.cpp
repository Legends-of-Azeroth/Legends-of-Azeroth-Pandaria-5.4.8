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
#include "GameObjectAI.h"

enum eSpells
{
    // Triple Puncture
    SPELL_TRIPLE_PUNCTURE                   = 136767, // Casted on Main Tank

    // Double Swipe
    SPELL_DOUBLE_SWIPE                      = 136741, // Launch everything
    SPELL_DOUBLE_SWIPE_BACK                 = 136740, // Frontal damages
    SPELL_DOUBLE_SWIPE_FRONT                = 136739, // Back damages

    // Charge
    SPELL_CHARGE                            = 136769,
    SPELL_CHARGE_SWIPE                      = 136770,
    
    // Dire Call
    SPELL_DIRE_CALL                         = 137458, // Launch everything
    SPELL_SUMMON_DIREHORN_SPIRIT_SELECTOR   = 140947,
    SPELL_SUMMON_DIREHORN_SPIRIT            = 140945,

    // After a charge at a door
    SPELL_HEADACHE                          = 137294,
    SPELL_CRACKED_SHELL                     = 137240,

    // Enrage
    SPELL_RAMPAGE                           = 136821,

    // Farraki
    SPELL_STONE_GAZE                        = 136708, // Stonegazer
    SPELL_BLAZING_SUNLIGHT                  = 136719, // Wastewalker
    SPELL_SUMMON_SAND_TRAP                  = 136725, // Wastewalker

    // Gurubashi
    SPELL_RENDING_CHARGE                    = 136653, // Bloodlord
    SPELL_RENDING_CHARGE_DAMAGES            = 136654, // Bloodlord 
    SPELL_VENOM_BOLT_VOLLEY                 = 136587, // Venom Priest
    SPELL_SUMMON_VENOMOUS_EFFUSION          = 136644, // Venom Priest

    // Drakkari
    SPELL_UNCONTROLLED_ABOMINATION          = 136709, // Risen Drakkari Champion / Warrior
    SPELL_DEADLY_PLAGUE                     = 136710, // Risen Drakkari Champion / Warrior 
    SPELL_MORTAL_STRIKE                     = 136670, // Frozen Warlord
    SPELL_SUMMON_FROZEN_ORB                 = 136564, // Frozen Warlord

    // Amani
    SPELL_FIREBALL                          = 136465, // Flame Caster
    SPELL_SWIPE                             = 136463, // Warbear
    SPELL_CHAIN_LIGHTNING                   = 136480, // Beast Shaman
    SPELL_HEX_OF_CONFUSION                  = 136512, // 
    SPELL_SUMMON_LIGHTNING_NOVA_TOTEM       = 136487, // 

    // Zandalari
    SPELL_DINO_MENDING                      = 136797, // Dinomancer
    SPELL_DINO_FORM                         = 137237, // 
    SPELL_SUMMON_ORB_OF_CONTROL_FARRAKI     = 137445, // 
    SPELL_SUMMON_ORB_OF_CONTROL_GURUBASHI   = 137447, // 
    SPELL_SUMMON_ORB_OF_CONTROL_DRAKKARI    = 137448, // 
    SPELL_SUMMON_ORB_OF_CONTROL_AMANI       = 137449, // 

    // Summons
    SPELL_DIRE_FIXATION                     = 140946, // Dire Horn Spirit
    SPELL_WEAK_LINK                         = 140948, // Dire Horn Spirit
    SPELL_WEAK_LINK_EFF                     = 140949,
    SPELL_SAND_TRAP_PERIODIC                = 136724,
    SPELL_SAND_TRAP                         = 136723, // SpellScript needed (radius)
    SPELL_SUMMON_LIVING_POISON              = 136644, // Three times ?
    SPELL_LIVING_POISON_PERIODIC            = 136645,
    SPELL_LIVING_POISON                     = 136646,
    SPELL_FROZEN_BOLT_PERIODIC              = 136572,
    SPELL_FROZEN_BOLT                       = 136573, // Frozen Orb
    SPELL_LIGHTNING_NOVA_PERIODIC           = 136489, // Lightning Nova Totem
    SPELL_LIGHTNING_NOVA                    = 136490,

    // Jalak
    SPELL_BESTIAL_CRY                       = 136817,

    // Orb of Control
    SPELL_CONTROL_HORRIDON_FARRAKI          = 137433,
    SPELL_CONTROL_HORRIDON_GURUBASHI        = 137442,
    SPELL_CONTROL_HORRIDON_DRAKKARI         = 137443,
    SPELL_CONTROL_HORRIDON_AMANI            = 137444,
};

enum eEvents
{
    // Horridon
    EVENT_TRIPLE_PUNCTURE                   = 1,
    EVENT_DOUBLE_SWIPE                      = 2,
    EVENT_HORRIDON_CHARGE                   = 3, // 
    EVENT_DIRE_CALL                         = 4,
    EVENT_PREPARE_CHARGE                    = 5, // Pseudo hack
    EVENT_CHARGE_AT_DOOR                    = 6,
    EVENT_BERSERK                           = 34,
    EVENT_EVADE_CHECK                       = 35,

    // Farraki
    EVENT_STONE_GAZE                        = 7,
    EVENT_BLAZING_LIGHT                     = 8,
    EVENT_SUMMON_SAND_TRAP                  = 9,

    // Gurubashi
    EVENT_RENDING_CHARGE                    = 10,
    EVENT_GURUBASHI_VENOM_BOLT_VOLLEY       = 11,
    EVENT_SUMMON_VENOMOUS_EFFUSION          = 12,

    // Drakkari
    EVENT_SWITCH_TARGET                     = 13, // Risen Drakkari doesn't have a real threat list
    EVENT_MORTAL_STRIKE                     = 14,
    EVENT_SUMMON_FROZEN_ORB                 = 15,

    // Amani
    EVENT_FIREBALL                          = 16,
    EVENT_SWIPE                             = 17,
    EVENT_CHAIN_LIGHTNING                   = 18,
    EVENT_HEX_OF_CONFUSION                  = 19,
    EVENT_SUMMON_LIGHTNING_NOVA_TOTEM       = 20,

    // Zandalari
    EVENT_DINO_MENDING                      = 21,

    // Summons
    EVENT_EFFUSION_VENOM_BOLT_VOLLEY        = 22,

    // Jalak
    EVENT_BESTIAL_CRY                       = 23,

    // Helper
    EVENT_SUMMON_MINOR                      = 24, // 
    EVENT_SUMMON_MAJOR                      = 25, // 
    EVENT_TRANSITION                        = 28, // 

    // RP Event Helper
    EVENT_CHECK_PLAYERS                     = 29, // Every 500 ms to not flood the world
    EVENT_INTRO_PART_I                      = 30, // Jalak begins it's speech... 
    EVENT_INTRO_PART_II                     = 31, // Continues
    EVENT_INTRO_PART_III                    = 32, // Horridon breaks free and go and kill every... oh wait... that's not the time
    EVENT_INTRO_PART_IV                     = 33, // Now Horridon breaks free
    EVENT_MOVE,
};

enum eActions
{
    // Controller
    ACTION_FIGHT_BEGIN,
    ACTION_FIGHT_RESET,
    ACTION_ENTER_NEXT_TRASH_PHASE,
    ACTION_FIGHT_END,
    ACTION_PREPARE_TRANSITION,
    ACTION_RESET_AFTER_CRASH,

    // Jalak
    ACTION_ENTER_PHASE_TWO,

    // Horridon
    ACTION_INTRO,
    ACTION_PREPARE_CHARGE,
    ACTION_CONTROL_APPLY,
    ACTION_REMOVE_DOUBLE_SWIPE,
    ACTION_REMOVE_HEADACHED,

    //Living Poison
    ACTION_LIVING_POISON_DESPAWN,

    // Misc
    ACTION_TRIBE_JUMP_INTO_BATTLE,
    ACTION_AMANI_SHAMAN,
};

enum eMotions
{
    // Majors
    MOTION_MAJOR_JUMP                       = 6845,

    // Zandalari Dinomancers
    MOTION_DINOMANCER_JUMP                  = 7849,

    // Horridon
    MOTION_HORRIDON_CHARGE                  = 4551,
    MOTION_HORRIDON_DOOR_CHARGE             = 3205,
};


enum eTalks
{
    TALK_INTRO_FIRST                        = 0, // Welcome weaklings
    TALK_INTRO_SECOND                       = 1, // The tribes have assembled
    TALK_INTRO_THIRD                        = 2, // Now, witness the true might
    TALK_ENTER_PHASE_2                      = 3, // Hahaha, now it be my turn
    TALK_DRAKKARI                           = 4, // Drakkari tribe
    TALK_GURUBASHI                          = 5,
    TALK_FARRAKI                            = 6,
    TALK_AMANI                              = 7,
    TALK_ON_JALAK_KILLED_UNIT               = 8, // Ya skull
    TALK_ON_HORRIDON_KILLED_UNIT            = 9, // Da'kala koraste

    EMOTE_FARRAKI                           = 0,
    EMOTE_GURUBASHI                         = 1,
    EMOTE_DRAKKARI                          = 2,
    EMOTE_AMANI                             = 3,
    EMOTE_CHARGE                            = 4,
    EMOTE_FARAKKI_DOOR                      = 5,
    EMOTE_GURUBASHI_DOOR                    = 6,
    EMOTE_DRAKKARI_DOOR                     = 7,
    EMOTE_AMANI_DOOR                        = 8,
};

enum eDatas
{
    DATA_AMANI_BEAST_SHAMAN_LIGHTNING_COUNT = 0, // To know how many time we stroke
};

/* Each phase means:
    1) Summon trash at door:
    2) Call creature from tribune in sequence: 1 - 2 - Dinomancer
    gates - 1 - gates - 2 - gates - dinomancer - gates - gates... etc till door not closed
    
    Seems like Horridon controll it whole, Jalak just say something if alive.
*/
enum eTrashPhases
{
    TRASH_PHASE_FARRAKI,
    TRASH_PHASE_GURUBASHI,
    TRASH_PHASE_DRAKKARI,
    TRASH_PHASE_AMANI,
    TRASH_PHASE_NONE,
};

// Drakari from gates have a seq: 2 - 1 - 2 - 1...
enum eDrakkariCycle
{
    DRAKKARI_CYCLE_FIRST                    = 0,
    DRAKKARI_CYCLE_SECOND                   = 1,
};


// Relocation of Horridon's Home Position
static const Position middlePosition = { 5428.461f, 5701.532f, 129.606461f, 1.57860f };

static uint32 GetNewEmoteBySpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_CONTROL_HORRIDON_FARRAKI:
            return EMOTE_FARAKKI_DOOR;
        case SPELL_CONTROL_HORRIDON_GURUBASHI:
            return EMOTE_GURUBASHI_DOOR;
        case SPELL_CONTROL_HORRIDON_DRAKKARI:
            return EMOTE_DRAKKARI_DOOR;
        case SPELL_CONTROL_HORRIDON_AMANI:
            return EMOTE_AMANI_DOOR;
    }

    return 0;
}

static uint32 GetDoorIdByPhase(eTrashPhases phase)
{
    switch (phase)
    {
        case TRASH_PHASE_FARRAKI:
            return GO_TRIBAL_DOOR_FARRAKI;
        case TRASH_PHASE_GURUBASHI:
            return GO_TRIBAL_DOOR_GURUBASHI;
        case TRASH_PHASE_DRAKKARI:
            return GO_TRIBAL_DOOR_DRAKKARI;
        case TRASH_PHASE_AMANI:
            return GO_TRIBAL_DOOR_AMANI;
    }

    return 0;
}

static uint32 GetControlSpellByOrb(uint32 goEntry)
{
    switch (goEntry)
    {
        case GO_ORB_OF_CONTROL_FARRAKI:
            return SPELL_CONTROL_HORRIDON_FARRAKI;
        case GO_ORB_OF_CONTROL_GURUBASHI:
            return SPELL_CONTROL_HORRIDON_GURUBASHI;
        case GO_ORB_OF_CONTROL_DRAKKARI:
            return SPELL_CONTROL_HORRIDON_DRAKKARI;
        case GO_ORB_OF_CONTROL_AMANI:
            return SPELL_CONTROL_HORRIDON_AMANI;
        default:
            return 0;
    }
}

static uint32 GetControlOrbSpellByPhase(eTrashPhases ePhase)
{
    switch (ePhase)
    {
        case TRASH_PHASE_FARRAKI:
            return SPELL_SUMMON_ORB_OF_CONTROL_FARRAKI;
        case TRASH_PHASE_GURUBASHI:
            return SPELL_SUMMON_ORB_OF_CONTROL_GURUBASHI;
        case TRASH_PHASE_DRAKKARI:
            return SPELL_SUMMON_ORB_OF_CONTROL_DRAKKARI;
        case TRASH_PHASE_AMANI:
            return SPELL_SUMMON_ORB_OF_CONTROL_AMANI;
        default:
            return 0;
    }
}

const std::map<uint32, Position> invTribalDoorType =
{
    { GO_TRIBAL_DOOR_FARRAKI,   { 5502.86f, 5825.90f, 130.03f, 1.03f } },
    { GO_TRIBAL_DOOR_GURUBASHI, { 5504.43f, 5682.82f, 130.03f, 5.49f } },
    { GO_TRIBAL_DOOR_DRAKKARI,  { 5359.95f, 5681.67f, 130.03f, 3.86f } },
    { GO_TRIBAL_DOOR_AMANI,     { 5359.99f, 5825.55f, 130.03f, 2.45f } },
};

const std::vector<uint32> tribeDefine=
{
    NPC_WAR_GOD_JALAK,
    NPC_SUL_LITHUZ_STONEGAZER,
    NPC_FARRAKI_SKIRMISHER,
    NPC_FARRAKI_WASTEWALKER,
    NPC_GURUBASHI_BLOODLORD,
    NPC_GURUBASHI_VENOM_PRIEST,
    NPC_RISEN_DRAKKARI_WARRIOR,
    NPC_RISEN_DRAKKARI_CHAMPION,
    NPC_DRAKKARI_FROZEN_WARLORD,
    NPC_AMANI_SHI_PROTECTOR,
    NPC_AMANI_SHI_FLAME_CASTER,
    NPC_AMANI_WARBEAR,
    NPC_AMANI_SHI_BEAST_SHAMAN,
    NPC_ZANDALARI_DINOMANCER,
};

// Horridon AI
class boss_horridon : public CreatureScript
{
    public:
        boss_horridon() : CreatureScript("boss_horridon") { }

        struct boss_horridon_AI : public BossAI
        {
            boss_horridon_AI(Creature* creature) : BossAI(creature, DATA_HORRIDON) { }

            EventMap m_mBerserkEvents;
            std::map <uint32, uint32> tribeDoorData;
            uint32 majorCount, uPhase, direhornPhase;
            ObjectGuid targetGUID, chargeGUID;
            bool bJalakCalled;
            bool hasInCharge;

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_PASSIVE);

                me->m_Events.Schedule(2500, [this]()
                {
                    if (instance && instance->GetData(DATA_HORIDON_EVENT) == DONE)
                    {
                        if (GameObject* pDoor = me->FindNearestGameObject(GO_HORRIDON_PRISON_DOOR, 50000.0f))
                            pDoor->SetGoState(GO_STATE_ACTIVE);

                        me->HandleEmoteStateCommand(EMOTE_STATE_ATTACK_UNARMED);

                        me->m_Events.Schedule(4000, [this]()
                        {
                            me->HandleEmoteStateCommand(EMOTE_STATE_POINT);
                            me->AI()->DoAction(ACTION_INTRO);
                        });
                    }
                });

                Reset();
            }

            void Reset() override
            {
                _Reset();

                events.Reset();
                m_mBerserkEvents.Reset();
                summons.DespawnAll();
                bJalakCalled = false;
                hasInCharge  = false;
                targetGUID = ObjectGuid::Empty;
                chargeGUID = ObjectGuid::Empty;
                majorCount   = 1;
                direhornPhase = 2;
                uPhase = TRASH_PHASE_NONE;

                me->RemoveAurasDueToSpell(SPELL_HEADACHE);
                me->RemoveAurasDueToSpell(SPELL_CRACKED_SHELL);
                me->RemoveAurasDueToSpell(SPELL_RAMPAGE);

                if (instance && instance->GetData(DATA_HORIDON_EVENT) == DONE)
                    me->SetReactState(REACT_AGGRESSIVE);

                me->GetMap()->SetWorldState(WORLDSTATE_CRETACEOUS_COLLECTOR, 1);

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 392.0m in 10 normal
                    me->SetCreateHealth(392000000);
                    me->SetMaxHealth(392000000);
                    me->SetHealth(392000000);
                    me->ResetPlayerDamageReq();
                }

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext /*context*/)
                {
                    if (instance && instance->GetData(DATA_HORIDON_EVENT) == DONE)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
                summons.DespawnAll();
                scheduler.CancelAll();
                HandleDespawnGameObjects();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_HORRIDON, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TRIPLE_PUNCTURE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STONE_GAZE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLAZING_SUNLIGHT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VENOM_BOLT_VOLLEY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEADLY_PLAGUE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HEX_OF_CONFUSION);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_HORRIDON_ENTRANCE)))
                        go->SetGoState(GO_STATE_ACTIVE);
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                HandleResetAnyTribes();
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, middlePosition.GetPositionX(), middlePosition.GetPositionY(), middlePosition.GetPositionZ(), middlePosition.GetOrientation());
                Reset();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                m_mBerserkEvents.ScheduleEvent(EVENT_BERSERK, 12 * MINUTE * IN_MILLISECONDS);
                InitCombatEvents();
                
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_HORRIDON_ENTRANCE)))
                        go->SetGoState(GO_STATE_READY);
                }

                uPhase = TRASH_PHASE_FARRAKI;
            }

            void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
            {
                // Hanle set facing to victim (issue with high hitbox encounters)
                if (victim && damageType == DIRECT_DAMAGE)
                    me->SetFacingTo(me->GetAngle(victim));
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                HandleDespawnGameObjects();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TRIPLE_PUNCTURE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STONE_GAZE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLAZING_SUNLIGHT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VENOM_BOLT_VOLLEY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEADLY_PLAGUE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HEX_OF_CONFUSION);

                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_HORRIDON_ENTRANCE)))
                        go->SetGoState(GO_STATE_ACTIVE);
                }

                if (Creature* jalak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WAR_GOD_JALAK) : ObjectGuid::Empty))
                    jalak->DespawnOrUnsummon();
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                switch (pointId)
                {
                    case MOTION_HORRIDON_DOOR_CHARGE:
                        DoCast(me, SPELL_HEADACHE, true);

                        if (GameObject* curTribalDoor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GetDoorIdByPhase(eTrashPhases(uPhase))) : ObjectGuid::Empty))
                            curTribalDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                        uPhase++;
                        majorCount = 1;
                        InitCombatEvents();

                        // We should call Jalak if it was the last door. Not matter about 30%
                        if (uPhase > TRASH_PHASE_AMANI && !bJalakCalled)
                        {
                            bJalakCalled = true;

                            if (Creature* jalak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WAR_GOD_JALAK) : ObjectGuid::Empty))
                                jalak->AI()->DoAction(ACTION_ENTER_PHASE_TWO);
                        }
                        break;
                    case MOTION_HORRIDON_CHARGE:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    case EVENT_CHARGE:
                        if (Unit* chargeTarget = ObjectAccessor::GetUnit(*me, chargeGUID))
                        {
                            me->InterruptNonMeleeSpells(true, SPELL_CHARGE);
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            me->SetFacingTo(me->GetAngle(chargeTarget));
                        }
                        else
                        {
                            me->InterruptNonMeleeSpells(true, SPELL_CHARGE);
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                        }

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_HORRIDON) == IN_PROGRESS; })
                            .Schedule(Milliseconds(300), [this](TaskContext /*context*/) // very very low delay or it`ll have a wrong anim (hacky, hitbox issue)
                        {
                            DoCast(SPELL_CHARGE_SWIPE);
                        });

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_HORRIDON) == IN_PROGRESS; })
                            .Schedule(Milliseconds(5300), [this](TaskContext context)
                        {
                            if (me->HasAura(SPELL_CHARGE_SWIPE))
                            {
                                context.Repeat(Milliseconds(1000));
                                return;
                            }
                            hasInCharge = false;
                            me->RemoveChanneledCast(targetGUID);
                        });
                        break;
                }

            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_TRIBE_PHASE:
                        return uPhase;
                    case TYPE_DIREHORN_SPIRIT:
                        return direhornPhase;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_DIREHORN_SPIRIT)
                    direhornPhase = data;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        me->GetMotionMaster()->MoveCharge(middlePosition.GetPositionX(), middlePosition.GetPositionY(), middlePosition.GetPositionZ(), 42.0f, MOTION_HORRIDON_CHARGE);
                        me->SetHomePosition(middlePosition);
                        break;
                    case ACTION_PREPARE_TRANSITION:
                        me->InterruptNonMeleeSpells(true);
                        events.Reset();
                        m_mBerserkEvents.CancelEvent(EVENT_SUMMON_MINOR);
                        m_mBerserkEvents.CancelEvent(EVENT_SUMMON_MAJOR);

                        if (Unit* target = me->GetVictim())
                            targetGUID = target->GetGUID();

                        me->PrepareChanneledCast(me->GetOrientation());
                        me->HandleEmoteCommand(EMOTE_ONESHOT_MOUNT_SPECIAL);

                        scheduler
                            .SetValidator([this] { return instance && instance->GetBossState(DATA_HORRIDON) == IN_PROGRESS; })
                            .Schedule(Seconds(5), [this](TaskContext /*context*/)
                        {
                            if (GameObject* curTribalDoor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GetDoorIdByPhase(eTrashPhases(uPhase))) : ObjectGuid::Empty))
                            {
                                me->GetMotionMaster()->MovementExpired();
                                me->GetMotionMaster()->MoveCharge(invTribalDoorType.find(curTribalDoor->GetEntry())->second.GetPositionX(), invTribalDoorType.find(curTribalDoor->GetEntry())->second.GetPositionY(), invTribalDoorType.find(curTribalDoor->GetEntry())->second.GetPositionZ(), 42.0f, MOTION_HORRIDON_DOOR_CHARGE);
                            }
                        });
                        break;
                    case ACTION_REMOVE_HEADACHED:
                        me->RemoveChanneledCast(targetGUID);
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(30) && !bJalakCalled)
                {
                    bJalakCalled = true;

                    if (Creature* jalak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WAR_GOD_JALAK) : ObjectGuid::Empty))
                        jalak->AI()->DoAction(ACTION_ENTER_PHASE_TWO);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                m_mBerserkEvents.Update(diff);

                while (uint32 eventId = m_mBerserkEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_SUMMON_MINOR:
                            if (uPhase == TRASH_PHASE_NONE)
                                break;
                        
                            HandleSummonMinorTribes();
                            m_mBerserkEvents.ScheduleEvent(EVENT_SUMMON_MINOR, 20 * IN_MILLISECONDS);
                        
                            if (majorCount < 4)
                                m_mBerserkEvents.ScheduleEvent(EVENT_SUMMON_MAJOR, 18.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMON_MAJOR:
                            HandleSummonMajorTribes();
                            majorCount++;
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || hasInCharge)
                    return;

                if (me->HasUnitState(UNIT_STATE_CANNOT_TURN) && !me->HasAura(SPELL_HEADACHE))
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TRIPLE_PUNCTURE:
                            if (Unit* victim = me->GetVictim())
                                DoCast(victim, SPELL_TRIPLE_PUNCTURE);

                            events.ScheduleEvent(EVENT_TRIPLE_PUNCTURE, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case EVENT_DOUBLE_SWIPE:
                            if (Unit* target = me->GetVictim())
                            {
                                targetGUID = target->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_DOUBLE_SWIPE);
                            }

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_HORRIDON) == IN_PROGRESS; })
                                .Schedule(Milliseconds(4500), [this](TaskContext /*context*/)
                            {
                                me->RemoveChanneledCast(targetGUID);
                            });

                            events.ScheduleEvent(EVENT_DOUBLE_SWIPE, urand(16 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                            break;
                        case EVENT_HORRIDON_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                            {
                                hasInCharge = true;
                                Talk(EMOTE_CHARGE, target);

                                if (Unit* vict = me->GetVictim())
                                    targetGUID = vict->GetGUID();

                                chargeGUID = target->GetGUID();

                                me->PrepareChanneledCast(me->GetAngle(target));

                                DoCast(target, SPELL_CHARGE);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                hasInCharge = true;
                                Talk(EMOTE_CHARGE, target);

                                if (Unit* vict = me->GetVictim())
                                    targetGUID = vict->GetGUID();

                                chargeGUID = target->GetGUID();

                                me->PrepareChanneledCast(me->GetAngle(target));

                                DoCast(target, SPELL_CHARGE);
                            }

                            // In case hunter fake death or smth else
                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_HORRIDON) == IN_PROGRESS; })
                                .Schedule(Milliseconds(8000), [this](TaskContext context)
                            {
                                if (me->HasAura(SPELL_CHARGE_SWIPE))
                                {
                                    context.Repeat(Milliseconds(1000));
                                    return;
                                }

                                hasInCharge = false;
                                me->RemoveChanneledCast(targetGUID);
                            });

                            events.ScheduleEvent(EVENT_HORRIDON_CHARGE, urand(40 * IN_MILLISECONDS, 44 * IN_MILLISECONDS));
                            events.RescheduleEvent(EVENT_DOUBLE_SWIPE, 16 * IN_MILLISECONDS);
                            break;
                        case EVENT_DIRE_CALL:
                            DoCastAOE(SPELL_DIRE_CALL);
                            events.ScheduleEvent(EVENT_DIRE_CALL, 60 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                //EnterEvadeIfOutOfCombatArea(diff);
            }

            private:
                void HandleSummonMinorTribes()
                {
                    // We should open the gates each time when summon minors
                    if (GameObject* curTribalDoor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GetDoorIdByPhase(eTrashPhases(uPhase))) : ObjectGuid::Empty))
                        curTribalDoor->UseDoorOrButton();

                    std::list<Creature*> minorsList;
                    GetCreatureListWithEntryInGrid(minorsList, me, NPC_GENERAL_PUR_BUNNY, 400.0f);

                    uint32 tribesId = 0;
                    float fixedOri = 0.0f;

                    switch (uPhase)
                    {
                        case TRASH_PHASE_FARRAKI:
                            tribesId = NPC_FARRAKI_SKIRMISHER; // 2
                            fixedOri = 5 * M_PI / 4;
                            break;
                        case TRASH_PHASE_GURUBASHI:
                            tribesId = NPC_GURUBASHI_BLOODLORD; // 1
                            fixedOri = 3 * M_PI / 4;
                            break;
                        case TRASH_PHASE_DRAKKARI:
                            tribesId = NPC_RISEN_DRAKKARI_CHAMPION; // 2
                            fixedOri =  M_PI / 4;
                            break;
                        case TRASH_PHASE_AMANI:
                            tribesId = NPC_AMANI_SHI_PROTECTOR;
                            fixedOri = 7 * M_PI / 4;
                            break;
                    }

                    minorsList.remove_if([=](Creature* target) { return !target->FindNearestCreature(tribesId, 60.0f, true); });

                    uint32 tribesCount = tribesId == NPC_GURUBASHI_BLOODLORD ? 1 : 2;

                    if (minorsList.size() > tribesCount)
                        Trinity::Containers::RandomResizeList(minorsList, tribesCount);

                    for (auto&& itr : minorsList)
                    {
                        // Not sure why it has ori to 0
                        me->SummonCreature(tribesId, itr->GetPositionX(), itr->GetPositionY(), itr->GetPositionZ(), Position::NormalizeOrientation(fixedOri), TEMPSUMMON_MANUAL_DESPAWN);
                        tribesId--;
                    }
                }

                void HandleSummonMajorTribes()
                {
                    uint32 majorId = 0;

                    switch (uPhase)
                    {
                        case TRASH_PHASE_FARRAKI:
                            majorId = NPC_FARRAKI_WASTEWALKER;
                            break;
                        case TRASH_PHASE_GURUBASHI:
                            majorId = NPC_GURUBASHI_VENOM_PRIEST;
                            break;
                        case TRASH_PHASE_DRAKKARI:
                            majorId = NPC_DRAKKARI_FROZEN_WARLORD;
                            break;
                        case TRASH_PHASE_AMANI:
                            majorId = NPC_AMANI_WARBEAR;
                            break;
                    }

                    // Summon Majors
                    if (majorCount < 3)
                    {
                        std::list<Creature*> triblesList;
                        GetCreatureListWithEntryInGrid(triblesList, me, majorId, 400.0f);

                        if (triblesList.empty())
                            return;

                        triblesList.sort(TribesZOrderPred(false));
                        triblesList.remove_if([=](Creature* target) { return target->IsInCombat(); });

                        if (triblesList.size() > majorCount)
                            triblesList.resize(majorCount);

                        for (auto&& itr : triblesList)
                            itr->AI()->DoAction(ACTION_TRIBE_JUMP_INTO_BATTLE);
                    }
                    // Summon Dinomancer from current tribe
                    // Just Filter it like dinomancer around selected type of creature and lowest Z
                    else
                    {
                        std::list<Creature*> dinomancerList;
                        GetCreatureListWithEntryInGrid(dinomancerList, me, NPC_ZANDALARI_DINOMANCER, 400.0f);

                        uint32 nearController = 0;

                        switch (uPhase)
                        {
                            case TRASH_PHASE_FARRAKI:
                                nearController = NPC_FARRAKI_SKIRMISHER;
                                break;
                            case TRASH_PHASE_GURUBASHI:
                                nearController = NPC_GURUBASHI_BLOODLORD;
                                break;
                            case TRASH_PHASE_DRAKKARI:
                                nearController = NPC_RISEN_DRAKKARI_CHAMPION;
                                break;
                            case TRASH_PHASE_AMANI:
                                nearController = NPC_AMANI_SHI_FLAME_CASTER;
                                break;
                        }

                        dinomancerList.remove_if([=](Creature* target) { return !target->FindNearestCreature(nearController, 40.0f, true) || target->GetPositionZ() > 181.0f; });

                        if (dinomancerList.size() > 1)
                            dinomancerList.resize(1);

                        for (auto&& itr : dinomancerList)
                            itr->AI()->DoAction(ACTION_TRIBE_JUMP_INTO_BATTLE);
                    }
                }

                void HandleResetAnyTribes()
                {
                    std::list<Creature*> tribes;
                    GetCreatureListWithEntryInGrid(tribes, me, NPC_FARRAKI_WASTEWALKER, 400.0f);
                    GetCreatureListWithEntryInGrid(tribes, me, NPC_GURUBASHI_VENOM_PRIEST, 400.0f);
                    GetCreatureListWithEntryInGrid(tribes, me, NPC_DRAKKARI_FROZEN_WARLORD, 400.0f);
                    GetCreatureListWithEntryInGrid(tribes, me, NPC_AMANI_WARBEAR, 400.0f);
                    GetCreatureListWithEntryInGrid(tribes, me, NPC_ZANDALARI_DINOMANCER, 400.0f);
                    GetCreatureListWithEntryInGrid(tribes, me, NPC_WAR_GOD_JALAK, 400.0f);

                    for (auto&& itr : tribes)
                    {
                        if (!itr->IsAlive())
                            itr->Respawn();

                        itr->AI()->EnterEvadeMode();
                    }

                    // Also reset Tribals Door if it was destroyed
                    for (uint32 i = GO_TRIBAL_DOOR_GURUBASHI; i < GO_TRIBAL_DOOR_AMANI + 1; i++)
                        if (GameObject* curTribalDoor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(i) : ObjectGuid::Empty))
                            curTribalDoor->SetGoState(GO_STATE_READY);
                }

                void InitCombatEvents()
                {
                    events.ScheduleEvent(EVENT_TRIPLE_PUNCTURE, 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_DOUBLE_SWIPE, 15 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_HORRIDON_CHARGE, 30 * IN_MILLISECONDS);
                    m_mBerserkEvents.ScheduleEvent(EVENT_SUMMON_MINOR, 18.9 * IN_MILLISECONDS);

                    if (IsHeroic())
                        events.ScheduleEvent(EVENT_DIRE_CALL, 60 * IN_MILLISECONDS);
                }

                void HandleDespawnGameObjects()
                {
                    std::list<GameObject*> sphereList;
                    GetGameObjectListWithEntryInGrid(sphereList, me, GO_ORB_OF_CONTROL_FARRAKI, 200.0f);
                    GetGameObjectListWithEntryInGrid(sphereList, me, GO_ORB_OF_CONTROL_GURUBASHI, 200.0f);
                    GetGameObjectListWithEntryInGrid(sphereList, me, GO_ORB_OF_CONTROL_DRAKKARI, 200.0f);
                    GetGameObjectListWithEntryInGrid(sphereList, me, GO_ORB_OF_CONTROL_AMANI, 200.0f);

                    for (auto&& itr : sphereList)
                        itr->Delete();
                }

                class TribesZOrderPred
                {
                    public:
                        TribesZOrderPred(bool ascending = true) : m_ascending(ascending) {}
                        bool operator() (const WorldObject* a, const WorldObject* b) const
                        {
                            return m_ascending ? a->GetPositionZ() < b->GetPositionZ() :
                                a->GetPositionZ() > b->GetPositionZ();
                        }
                    private:
                        const bool m_ascending;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_horridon_AI(creature);
        }
};

struct horridon_tribeAI : public ScriptedAI
{
    horridon_tribeAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    InstanceScript* instance;

    void JustSummoned(Creature* summon) override
    {
        if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
            horridon->AI()->JustSummoned(summon);
    }

    void EnterEvadeMode() override
    {
        if (!me->GetDBTableGUIDLow())
            return;

        _EnterEvadeMode();
        ScriptedAI::EnterEvadeMode();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);
        me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
    }
};

// Jalak AI
class npc_war_god_jalak : public CreatureScript
{
    public:
        npc_war_god_jalak() : CreatureScript("npc_war_god_jalak") { }

        struct npc_war_god_jalak_AI : public horridon_tribeAI
        {
            npc_war_god_jalak_AI(Creature* creature) : horridon_tribeAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            void Reset() override
            {   
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                events.Reset();
                secondPhase = false;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ENTER_PHASE_TWO:
                        if (!secondPhase)
                        {
                            secondPhase = true;
                            Talk(TALK_ENTER_PHASE_2);
                            me->GetMotionMaster()->MoveJump(middlePosition, 32.0f, 32.0f, EVENT_JUMP);
                        }
                        break;
                    case ACTION_START_INTRO:
                    {
                        if (instance)
                        {
                            // Prevent Intro if complete
                            if (instance->GetData(DATA_HORIDON_EVENT) == DONE)
                                break;

                            instance->SetData(DATA_HORIDON_EVENT, DONE);
                        }

                        uint32 delay = 0;
                        me->m_Events.Schedule(delay += 1000, 20, [this]()
                        {
                            Talk(TALK_INTRO_FIRST);
                        });

                        me->m_Events.Schedule(delay += 7500, 21, [this]()
                        {
                            Talk(TALK_INTRO_SECOND);
                        });

                        me->m_Events.Schedule(delay += 27000, 22, [this]()
                        {
                            Talk(TALK_INTRO_THIRD);
                        });

                        me->m_Events.Schedule(delay += 3000, 23, [this]()
                        {
                            if (GameObject *pDoor = me->FindNearestGameObject(GO_HORRIDON_PRISON_DOOR, 50000.0f))
                                pDoor->SetGoState(GO_STATE_ACTIVE);

                            if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON): ObjectGuid::Empty))
                                horridon->HandleEmoteStateCommand(EMOTE_STATE_ATTACK_UNARMED);
                        });

                        me->m_Events.Schedule(delay += 4000, 24, [this]()
                        {
                            if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
                            {
                                horridon->HandleEmoteStateCommand(EMOTE_STATE_POINT);
                                horridon->AI()->DoAction(ACTION_INTRO);
                            }
                        });
                        break;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetInCombatWithZone();

                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, me, 500.0f);

                    playerList.sort([this](Player const* first, Player const* second) -> bool
                    {
                        return first->GetExactDist2d(this->me) < second->GetExactDist2d(this->me);
                    });

                    if (!playerList.empty() && playerList.front())
                    {
                        Player *pFirst = playerList.front();
                        me->AddThreat(pFirst, 100000.0f);
                        ScriptedAI::AttackStart(pFirst);
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BESTIAL_CRY, 10 * IN_MILLISECONDS);
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
                    if (eventId == EVENT_BESTIAL_CRY)
                    {
                        DoCast(me, SPELL_BESTIAL_CRY);
                        events.ScheduleEvent(EVENT_BESTIAL_CRY, urand(10, 20) * IN_MILLISECONDS);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }

            void KilledUnit(Unit* victim) override
            {
                Talk(TALK_ON_JALAK_KILLED_UNIT);
            }

            void JustDied(Unit* killer) override
            {
                if (ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON):ObjectGuid::Empty) && ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON):ObjectGuid::Empty)->isDead())
                    instance->SetBossState(DATA_HORRIDON, DONE);
                else 
                {
                    if (Creature* pHorridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON):ObjectGuid::Empty))
                        pHorridon->AddAura(SPELL_RAMPAGE, pHorridon);
                }

                Talk(TALK_ON_HORRIDON_KILLED_UNIT);
            }

        private:
            bool secondPhase;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_war_god_jalak_AI(creature);
        }
};

// Trashs AI (does not include summons of spells)
struct npc_horridon_trashs : public horridon_tribeAI
{
    npc_horridon_trashs(Creature* creature) : horridon_tribeAI(creature)
    {
        instance = me->GetInstanceScript();
    }

    void Reset() override
    {
        events.Reset();
        pRendingChargeTarget = NULL;
        uiChainLightningCount = 0;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        // this creatures shouldn`t take collisions
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

        if (me->GetEntry() == NPC_AMANI_SHI_BEAST_SHAMAN)
            return;

        float x, y;
        GetPositionWithDistInOrientation(me, 35.0f, me->GetOrientation(), x, y);

        Movement::MoveSplineInit init(me);
        init.MoveTo(x, y, me->GetPositionZ());
        init.Launch();

        me->m_Events.Schedule(me->GetSplineDuration(), [this]()
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetInCombatWithZone();
            uint32 healthValue = me->GetMaxHealth();

            // Schedule events
            switch (me->GetEntry())
            {
                case NPC_SUL_LITHUZ_STONEGAZER:
                    events.ScheduleEvent(EVENT_STONE_GAZE, 5 * IN_MILLISECONDS);
                    healthValue = 1630000;
                    break;
                case NPC_GURUBASHI_BLOODLORD:
                    events.ScheduleEvent(EVENT_RENDING_CHARGE, 5 * IN_MILLISECONDS);
                    healthValue = 6320000;
                    break;
                case NPC_RISEN_DRAKKARI_CHAMPION:
                case NPC_RISEN_DRAKKARI_WARRIOR:
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                    DoCast(me, SPELL_UNCONTROLLED_ABOMINATION);
                    events.ScheduleEvent(EVENT_SWITCH_TARGET, 10 * IN_MILLISECONDS);
                    healthValue = me->GetEntry() == NPC_RISEN_DRAKKARI_WARRIOR ? 815000 : 2450000;
                    break;
                case NPC_AMANI_SHI_FLAME_CASTER:
                    events.ScheduleEvent(EVENT_FIREBALL, 5 * IN_MILLISECONDS);
                    healthValue = 1630000;
                    break;
                case NPC_FARRAKI_SKIRMISHER:
                    healthValue = 2040000;
                    break;
                case NPC_AMANI_SHI_PROTECTOR:
                    healthValue = 3260000;
                    break;
            }

            // Temp Hackfix (mostly wrong value in 10N)
            if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
            {
                // should be 244.0m in 10 normal
                me->SetCreateHealth(healthValue);
                me->SetMaxHealth(healthValue);
                me->SetHealth(healthValue);
                me->ResetPlayerDamageReq();
            }
        });
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_TRIBE_JUMP_INTO_BATTLE:
            {
                float x, y;
                GetPositionWithDistInOrientation(me, 35.0f, me->GetOrientation(), x, y);
            
                me->GetMotionMaster()->MoveJump({ x, y, 130.037354f, me->GetOrientation() }, 20.0f, 20.0f, MOTION_MAJOR_JUMP);
                break;
            }
            case ACTION_AMANI_SHAMAN:
                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEX_OF_CONFUSION, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SUMMON_LIGHTNING_NOVA_TOTEM, 15 * IN_MILLISECONDS);
                break;
        }
    }

    void JustDied(Unit* killer) override
    {
        switch (me->GetEntry())
        {
            case NPC_AMANI_WARBEAR:
                if (Vehicle *pKit = me->GetVehicleKit())
                    pKit->RemoveAllPassengers(); // Check this >_<
                else
                    me->SummonCreature(NPC_AMANI_SHI_BEAST_SHAMAN, *me);
                break;
            case NPC_GURUBASHI_VENOM_PRIEST:
                DoCast(me, SPELL_SUMMON_VENOMOUS_EFFUSION, true);
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == DATA_AMANI_BEAST_SHAMAN_LIGHTNING_COUNT)
            return uiChainLightningCount;

        return 0;
    }

    void SetData(uint32 type, uint32 /*data*/) override
    {
        if (type == DATA_AMANI_BEAST_SHAMAN_LIGHTNING_COUNT)
        {
            if (++uiChainLightningCount == 3)
                uiChainLightningCount = 0;
        }
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        if (pointId == MOTION_MAJOR_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetInCombatWithZone(); // Call it now to not bug the npcs around

            switch (me->GetEntry())
            {
                case NPC_FARRAKI_WASTEWALKER:
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                    events.ScheduleEvent(EVENT_BLAZING_LIGHT, 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_SAND_TRAP, 5000 + rand() % 5000);
                    break;
                case NPC_GURUBASHI_VENOM_PRIEST:
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                    events.ScheduleEvent(EVENT_GURUBASHI_VENOM_BOLT_VOLLEY, 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_VENOMOUS_EFFUSION, 15 * IN_MILLISECONDS);
                    break;
                case NPC_DRAKKARI_FROZEN_WARLORD:
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                    events.ScheduleEvent(EVENT_MORTAL_STRIKE, 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SUMMON_FROZEN_ORB, 15 * IN_MILLISECONDS);
                    break;
                case NPC_AMANI_WARBEAR:
                    events.ScheduleEvent(EVENT_SWIPE, 10 * IN_MILLISECONDS);
                    break;
            }

            // Select target and move towards it
            if (Creature* pHorridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
            {
                if (CreatureAI* pHorridonAI = pHorridon->AI())
                {
                    if (Unit* target = pHorridonAI->SelectTarget(SELECT_TARGET_RANDOM))
                    {
                        me->SetInCombatWith(target);
                        ScriptedAI::AttackStart(target);
                    }
                }
            }
        }

        if (pointId == EVENT_CHARGE && pRendingChargeTarget)
            DoCast(pRendingChargeTarget, SPELL_RENDING_CHARGE_DAMAGES);
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_SUMMON_SAND_TRAP:
                events.RescheduleEvent(EVENT_SUMMON_SAND_TRAP, 3500);
                break;
            case SPELL_VENOM_BOLT_VOLLEY:
                events.RescheduleEvent(EVENT_GURUBASHI_VENOM_BOLT_VOLLEY, 4000);
                break;
            case SPELL_BLAZING_SUNLIGHT:
                events.RescheduleEvent(EVENT_BLAZING_LIGHT, 3500);
                break;
            case SPELL_SUMMON_FROZEN_ORB:
                events.RescheduleEvent(EVENT_SUMMON_FROZEN_ORB, 3500);
                break;
            case SPELL_FIREBALL:
                events.RescheduleEvent(EVENT_FIREBALL, 3000);
                break;
            case SPELL_HEX_OF_CONFUSION:
                events.RescheduleEvent(EVENT_HEX_OF_CONFUSION, 4500);
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 uiEventId = events.ExecuteEvent())
        {
            switch (uiEventId)
            {
                    // Farraki
                case EVENT_STONE_GAZE:
                    if (Unit* victim = me->GetVictim())
                    {
                        if (!victim->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE) || victim->HasAura(SPELL_STONE_GAZE))
                        {
                            DoCast(victim, SPELL_STONE_GAZE);
                            events.ScheduleEvent(EVENT_STONE_GAZE, 10 * IN_MILLISECONDS);
                
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_STONE_GAZE))
                                ScriptedAI::AttackStart(target);
                
                            return;
                        }
                        events.ScheduleEvent(EVENT_STONE_GAZE, 1000);
                    }
                    break;
                case EVENT_BLAZING_LIGHT:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_BLAZING_SUNLIGHT)))
                        DoCast(target, SPELL_BLAZING_SUNLIGHT);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_BLAZING_SUNLIGHT);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_BLAZING_SUNLIGHT);
                
                    events.ScheduleEvent(EVENT_BLAZING_LIGHT, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_SAND_TRAP:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_SUMMON_SAND_TRAP);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_SUMMON_SAND_TRAP);
                
                    events.ScheduleEvent(EVENT_SUMMON_SAND_TRAP, 15000 + rand() % 10000);
                    break;
                    // Gurubashi
                case EVENT_RENDING_CHARGE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                    {
                        pRendingChargeTarget = target;
                        DoCast(target, SPELL_RENDING_CHARGE);
                    }
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    {
                        pRendingChargeTarget = target;
                        DoCast(target, SPELL_RENDING_CHARGE);
                    }
                
                    events.ScheduleEvent(EVENT_RENDING_CHARGE, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_GURUBASHI_VENOM_BOLT_VOLLEY:
                    DoCastAOE(SPELL_VENOM_BOLT_VOLLEY);
                    events.ScheduleEvent(EVENT_GURUBASHI_VENOM_BOLT_VOLLEY, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_VENOMOUS_EFFUSION:
                    DoCast(me, SPELL_SUMMON_VENOMOUS_EFFUSION, true);
                    events.ScheduleEvent(EVENT_SUMMON_VENOMOUS_EFFUSION, 30 * IN_MILLISECONDS);
                    break;
                    // Drakkari
                case EVENT_SWITCH_TARGET:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                    {
                        DoResetThreat();
                        me->AddThreat(target, 100000.0f);
                        ScriptedAI::AttackStart(target);
                    }
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    {
                        DoResetThreat();
                        me->AddThreat(target, 100000.0f);
                        ScriptedAI::AttackStart(target);
                    }
                
                    events.ScheduleEvent(EVENT_SWITCH_TARGET, 7 * IN_MILLISECONDS);
                    break;
                case EVENT_MORTAL_STRIKE:
                    if (Unit* victim = me->GetVictim())
                        DoCast(victim, SPELL_MORTAL_STRIKE);
                
                    events.ScheduleEvent(EVENT_MORTAL_STRIKE, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_FROZEN_ORB:
                    DoCast(me, SPELL_SUMMON_FROZEN_ORB);
                    events.ScheduleEvent(EVENT_SUMMON_FROZEN_ORB, 30 * IN_MILLISECONDS);
                    break;
                    // Amani
                case EVENT_FIREBALL:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_FIREBALL);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_FIREBALL);
                
                    events.ScheduleEvent(EVENT_FIREBALL, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_SWIPE:
                    DoCastAOE(SPELL_SWIPE);
                    events.ScheduleEvent(EVENT_SWIPE, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_CHAIN_LIGHTNING);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_CHAIN_LIGHTNING);
                
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_HEX_OF_CONFUSION:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector(SPELL_HEX_OF_CONFUSION)))
                        DoCast(target, SPELL_HEX_OF_CONFUSION);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true, -SPELL_HEX_OF_CONFUSION))
                        DoCast(target, SPELL_HEX_OF_CONFUSION);
                
                    events.ScheduleEvent(EVENT_HEX_OF_CONFUSION, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_LIGHTNING_NOVA_TOTEM:
                    DoCast(me, SPELL_SUMMON_LIGHTNING_NOVA_TOTEM);
                    events.ScheduleEvent(EVENT_SUMMON_LIGHTNING_NOVA_TOTEM, 30 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    private:
        Unit* pRendingChargeTarget;
        uint32 uiChainLightningCount;
};


// Summons AI
class npc_horridon_summons : public CreatureScript
{
    public:
        npc_horridon_summons() : CreatureScript("npc_horridon_summons") { }

        struct npc_horridon_summonsAI : public horridon_tribeAI
        {
            npc_horridon_summonsAI(Creature* creature) : horridon_tribeAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                instance = me->GetInstanceScript();
                events.Reset();
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetInCombatWithZone();

                switch (me->GetEntry())
                {
                    case NPC_SAND_TRAP:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        DoCast(me, SPELL_SAND_TRAP_PERIODIC);
                        break;
                    case NPC_FROZEN_ORB:
                        me->SetDisplayId(11686);
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        me->GetMotionMaster()->MoveRandom(15.f);
                        DoCast(me, SPELL_FROZEN_BOLT_PERIODIC);
                        break;
                    case NPC_LIGHTNING_NOVA_TOTEM:
                        me->PrepareChanneledCast(me->GetOrientation());
                        DoCast(me, SPELL_LIGHTNING_NOVA_PERIODIC);
                        break;
                    default:
                        break;
                }
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
                    if (eventId == EVENT_EFFUSION_VENOM_BOLT_VOLLEY)
                    {
                        DoCast(me, SPELL_VENOM_BOLT_VOLLEY);
                        events.ScheduleEvent(EVENT_EFFUSION_VENOM_BOLT_VOLLEY, 5 * IN_MILLISECONDS);
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_horridon_summonsAI(creature);
        }
};

// Zandalari Dinomancer AI
class npc_zandalari_dinomancer : public CreatureScript
{
    public:
        npc_zandalari_dinomancer() : CreatureScript("npc_zandalari_dinomancer") { }

        struct npc_zandalari_dinomancer_AI : public horridon_tribeAI
        {
            npc_zandalari_dinomancer_AI(Creature* creature) : horridon_tribeAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                bIsUnderFiftyPercent = false;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == MOTION_DINOMANCER_JUMP)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetInCombatWithZone();

                    if (Creature* pHorridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
                        events.ScheduleEvent(EVENT_DINO_MENDING, 8 * IN_MILLISECONDS);
                }
            }

            void JustDied(Unit* killer) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_CRETACEOUS_COLLECTOR, 0);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_DINO_MENDING, 7.5 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_TRIBE_JUMP_INTO_BATTLE)
                {
                    float x, y;
                    GetPositionWithDistInOrientation(me, 35.0f, me->GetOrientation(), x, y);

                    me->GetMotionMaster()->MoveJump({ x, y, 130.037354f, me->GetOrientation() }, 20.0f, 20.0f, MOTION_DINOMANCER_JUMP);
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(50) && !bIsUnderFiftyPercent)
                {
                    bIsUnderFiftyPercent = true;
                    events.Reset();
                    me->InterruptNonMeleeSpells(true, SPELL_DINO_MENDING);
                    DoCast(me, SPELL_DINO_FORM);
                    //Talk();

                    if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
                        DoCast(me, GetControlOrbSpellByPhase(eTrashPhases(horridon->AI()->GetData(TYPE_TRIBE_PHASE))));
                }
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
                    if (eventId == EVENT_DINO_MENDING)
                    {
                        DoCast(me, SPELL_DINO_MENDING);
                        events.ScheduleEvent(EVENT_DINO_MENDING, 30 * IN_MILLISECONDS);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool bIsUnderFiftyPercent;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zandalari_dinomancer_AI(creature);
        }
};


// Direhorn Spirit 70688
struct npc_direhorn_spirit : public ScriptedAI
{
    npc_direhorn_spirit(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    TaskScheduler scheduler;
    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        targetGUID = summoner->GetGUID();
        me->SetSpeed(MOVE_RUN, 0.24f); // no sniff data

        if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
        {
            horridon->AI()->JustSummoned(me);

            // Set Phase for me and summoner
            uint32 phase = horridon->AI()->GetData(TYPE_DIREHORN_SPIRIT);
            me->SetPhaseMask(1 << phase, true);
            summoner->SetPhaseMask((1 << phase) + 1, true);
            DoCast(summoner, SPELL_DIRE_FIXATION);
            DoCast(me, SPELL_WEAK_LINK, true);

            horridon->AI()->SetData(TYPE_DIREHORN_SPIRIT, ++phase);

            // Init delay before start attack
            scheduler
                .Schedule(Milliseconds(1500), [this](TaskContext /*context*/)
            {
                me->ClearUnitState(UNIT_STATE_CASTING);

                if (Unit* owner = ObjectAccessor::GetUnit(*me, targetGUID))
                    SetGazeOn(owner);
            });
        }
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (victim)
        {
            me->Kill(victim);
            me->DisappearAndDie();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Living Poison 69313
struct npc_living_poison : public ScriptedAI
{
    npc_living_poison(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetDisplayFromModel(1);
        me->SetReactState(REACT_PASSIVE);
        me->AddAura(SPELL_LIVING_POISON_PERIODIC, me);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        scheduler
            .Schedule(Milliseconds(2500), [this](TaskContext /*context*/)
        {
            me->GetMotionMaster()->MoveRandom(8.5f);
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_LIVING_POISON_DESPAWN)
        {
            me->GetMotionMaster()->MovementExpired();
            me->GetMotionMaster()->MoveRandom(30.f);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

};

class npc_venomous_effusion : public CreatureScript
{
    enum eEvents : uint32
    {
        EVENT_NONE,
        EVENT_VOLLEY,
    };

    public:
        npc_venomous_effusion() : CreatureScript("npc_venomous_effusion") { }

        struct npc_venomous_effusionAI : public ScriptedAI
        {
            npc_venomous_effusionAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();
                events.ScheduleEvent(EVENT_VOLLEY, 3000 + rand() % 2000);

                if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
                        horridon->AI()->JustSummoned(me);

                DoZoneInCombat(me, 100.f);
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
                    case EVENT_VOLLEY:
                        DoCast(SPELL_VENOM_BOLT_VOLLEY);
                        events.ScheduleEvent(EVENT_VOLLEY, 4000 + rand() % 2000);
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_venomous_effusionAI(creature);
        }
};

class npc_amani_shi_warbear : public CreatureScript
{
    public:
        npc_amani_shi_warbear() : CreatureScript("npc_amani_shi_warbear") { }

        struct npc_amani_shi_warbearAI : public ScriptedAI
        {
            npc_amani_shi_warbearAI(Creature* creature) : ScriptedAI(creature) 
            {
                m_shamanGuid = ObjectGuid::Empty;
                instance = me->GetInstanceScript();
            }

            ObjectGuid m_shamanGuid;
            EventMap events;
            InstanceScript* instance;

            void EnterEvadeMode() override
            {
                if (Creature* passenger = ObjectAccessor::GetCreature(*me, m_shamanGuid))
                    passenger->DespawnOrUnsummon();

                float x, y, z, o;

                me->GetHomePosition(x, y, z, o);
                me->NearTeleportTo(x, y, z, o);
                me->SetFacingTo(o);

                ScriptedAI::EnterEvadeMode();

                uint32 corpseDelay = me->GetCorpseDelay();
                uint32 respawnDelay = me->GetRespawnDelay();

                me->SetCorpseDelay(1);
                me->SetRespawnDelay(29);

                me->DespawnOrUnsummon();

                me->SetCorpseDelay(corpseDelay);
                me->SetRespawnDelay(respawnDelay);
            }

            void JustSummoned(Creature* summon) override
            {
                if (Creature* horridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
                    horridon->AI()->JustSummoned(summon);
            }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);

                if (Creature* pShaman = ObjectAccessor::GetCreature(*me, m_shamanGuid))
                {
                    pShaman->DespawnOrUnsummon();
                    m_shamanGuid = ObjectGuid::Empty;
                }

                // Vehicle Issue
                me->m_Events.Schedule(1500, [this]()
                {
                    if (Creature* pNewShaman = me->SummonCreature(NPC_AMANI_SHI_BEAST_SHAMAN, *me))
                    {
                        pNewShaman->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                        pNewShaman->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        m_shamanGuid = pNewShaman->GetGUID();
                    }
                });
            }

            void JustDied(Unit* killer) override
            {
                if (Creature* pShaman = ObjectAccessor::GetCreature(*me, m_shamanGuid))
                    pShaman->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == MOTION_MAJOR_JUMP)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    DoZoneInCombat(me, 400.f);
                    events.ScheduleEvent(EVENT_SWIPE, 6000 + rand() % 4000);

                    if (Vehicle* pKit = me->GetVehicleKit())
                        if (pKit->GetPassenger(0) && pKit->GetPassenger(0)->ToCreature())
                            pKit->GetPassenger(0)->ToCreature()->AI()->DoAction(ACTION_AMANI_SHAMAN);

                    if (Creature* pHorridon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_HORRIDON):ObjectGuid::Empty))
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        {
                            AttackStart(target);

                            if (Creature* pShaman = ObjectAccessor::GetCreature(*me, m_shamanGuid))
                                if (pShaman->AI())
                                    pShaman->AI()->AttackStart(target);
                        }
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 400.0f, true))
                        {
                            AttackStart(target);

                            if (Creature* pShaman = ObjectAccessor::GetCreature(*me, m_shamanGuid))
                                if (pShaman->AI())
                                    pShaman->AI()->AttackStart(target);
                        }
                    }
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_TRIBE_JUMP_INTO_BATTLE)
                {
                    float x, y;
                    GetPositionWithDistInOrientation(me, 35.0f, me->GetOrientation(), x, y);

                    me->GetMotionMaster()->MoveJump({ x, y, 130.037354f, me->GetOrientation() }, 20.0f, 20.0f, MOTION_MAJOR_JUMP);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_SWIPE:
                        DoCastAOE(SPELL_SWIPE);
                        events.ScheduleEvent(EVENT_SWIPE, 10 * IN_MILLISECONDS);
                        break;
                }

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amani_shi_warbearAI(creature);
        }
};

// Orb of Control AI
class go_horridon_orb_of_control : public GameObjectScript
{
    public:
        go_horridon_orb_of_control() : GameObjectScript("go_horridon_orb_of_control") { }

        struct go_horridon_orb_of_control_AI : public GameObjectAI
        {
            go_horridon_orb_of_control_AI(GameObject *go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {
                if (!isUse)
                    return true;

                if (uint32 spellId = GetControlSpellByOrb(me->GetEntry()))
                {
                    player->CastSpell(player, spellId, true);
                    me->RemoveFromWorld();
                }
                return true;
            }
        };

        GameObjectAI* GetAI(GameObject *go) const override
        {
            return new go_horridon_orb_of_control_AI(go);
        }
};

// Chain Lightning
class spell_horridon_chain_lightning : public SpellScriptLoader
{
    public:
        spell_horridon_chain_lightning() : SpellScriptLoader("spell_horridon_chain_lightning") { }

        class spell_horridon_chain_lightning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_horridon_chain_lightning_SpellScript)

            void HandleEffectHitTarget(SpellEffIndex effectIndex)
            {
                if (Unit* pHitUnit = GetHitUnit())
                {
                    Unit* caster = GetOriginalCaster();
                    if (!caster)
                        caster = GetCaster();

                    if (!caster)
                        return;

                    if (UnitAI *pCasterAI = caster->GetAI())
                    {
                        uint32 uiCount = pCasterAI->GetData(DATA_AMANI_BEAST_SHAMAN_LIGHTNING_COUNT);

                        if (uiCount == 0 && caster == GetOriginalCaster())
                            return;

                        Unit* pNewTarget = NULL;
                        uint32 uiLoopCount = 0; // Prevent infinite loop in case boss is being soloed

                        do
                        {
                            pNewTarget = pCasterAI->SelectTarget(SELECT_TARGET_RANDOM);
                            ++uiLoopCount;
                        } while (pNewTarget == pHitUnit && uiLoopCount <= 10);

                        if (!pNewTarget || pNewTarget == pHitUnit)
                            return;

                        pHitUnit->CastSpell(pNewTarget, SPELL_CHAIN_LIGHTNING, false, NULL, NULL, caster->GetGUID());

                        pCasterAI->SetData(DATA_AMANI_BEAST_SHAMAN_LIGHTNING_COUNT, 0);
                    }
                }
            }

            void Register() override
            {
                //OnEffectHitTarget += SpellEffectFn(spell_horridon_chain_lightning_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_horridon_chain_lightning_SpellScript();
        }
};


// Control Horridon Spells
class spell_control_horridon : public SpellScriptLoader
{
    public:
        spell_control_horridon() : SpellScriptLoader("spell_control_horridon") { }

        class spell_control_horridon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_control_horridon_AuraScript)

            void HandleApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
            {
                if (WorldObject *pOwner = GetOwner())
                    if (Creature* horridon = ObjectAccessor::GetCreature(*pOwner, pOwner->GetInstanceScript() ? pOwner->GetInstanceScript()->GetGuidData(DATA_HORRIDON) : ObjectGuid::Empty))
                        horridon->AI()->DoAction(ACTION_PREPARE_TRANSITION);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_control_horridon_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_MOD_PACIFY_SILENCE, AURA_EFFECT_HANDLE_REAL);
            }

            uint32 uiCount;
        };

        AuraScript* GetAuraScript() const override
        {
           return new spell_control_horridon_AuraScript();
        }
};


// Living Sand (radius calc) 136723
class spell_horridon_sand_trap : public SpellScript
{
    PrepareSpellScript(spell_horridon_sand_trap)

    void HandleSelectTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            // Remove target that are not in the radius
            targets.remove_if([caster](WorldObject const* target) -> bool {return target && target->ToUnit() && target->ToUnit()->GetExactDist2d(caster) > 1.24f * caster->GetFloatValue(OBJECT_FIELD_SCALE); });
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_horridon_sand_trap::HandleSelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Headache 137294
class spell_headache : public AuraScript
{
    PrepareAuraScript(spell_headache);

    void HandleAuraRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* horridon = GetOwner()->ToCreature())
            horridon->AI()->DoAction(ACTION_REMOVE_HEADACHED);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_headache::HandleAuraRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_double_swipe_aura : public SpellScriptLoader
{
    public:
        spell_double_swipe_aura() : SpellScriptLoader("spell_double_swipe_aura") { }

        class spell_double_swipe_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_double_swipe_aura_AuraScript);

            void HandleOnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Creature* owner = GetOwner()->ToCreature())
                    owner->SetControlled(true, UNIT_STATE_CANNOT_TURN);
            }

            void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    owner->SetControlled(false, UNIT_STATE_CANNOT_TURN);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_double_swipe_aura_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_double_swipe_aura_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_double_swipe_aura_AuraScript();
        }
};

// Tribe orb os controls 137445, 137447, 137448, 137449
class spell_tribe_orb_of_control : public SpellScript
{
    PrepareSpellScript(spell_tribe_orb_of_control);

    void SelectTargets(SpellDestination& dest)
    {
        dest._position.m_positionZ += 2.0f;
    }

    private:
        Position newPos;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_tribe_orb_of_control::SelectTargets, EFFECT_0, TARGET_DEST_CASTER_RADIUS);
    }
};

struct DireCallPredicate
{
    bool operator ()(WorldObject const* object) const
    {
        for (auto&& itr : tribeDefine)
            if (object && object->ToUnit() && object->ToUnit()->GetEntry() == itr && object->ToUnit()->IsInCombat())
                return false;

        return true;
    }
};

// Dire Call 137458
class spell_horridon_dire_call : public SpellScript
{
    PrepareSpellScript(spell_horridon_dire_call)

    void HandleSelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(DireCallPredicate());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_horridon_dire_call::HandleSelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Summon Direhorn Spirit Selector 140947
class spell_horridon_summon_direhorn_spririt_selector : public SpellScript
{
    PrepareSpellScript(spell_horridon_summon_direhorn_spririt_selector)

    void HandleSelectTargets(std::list<WorldObject*>& targets)
    {
        tempList = targets;

        if (Unit* caster = GetCaster())
        {
            uint32 targetsCount = caster->GetMap()->Is25ManRaid() ? 2 : 1;

            // First try select only HEALERS or CASTERS
            targets.remove_if(TankSpecTargetSelector());
            targets.remove_if(MeeleSpecTargetSelector());

            // if enough - break
            if (targets.size() >= targetsCount)
                return;

            // If not - select any meelee as last sequence
            targets.clear();
            tempList.remove_if(TankSpecTargetSelector());

            for (auto&& itr : tempList)
                targets.push_back(itr);
        }
    }

    private:
        std::list<WorldObject*> tempList;

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_horridon_summon_direhorn_spririt_selector::HandleSelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Weak Link 140948
class spell_horridon_weak_link : public AuraScript
{
    PrepareAuraScript(spell_horridon_weak_link);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetPositionZ() < 129.90f;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_horridon_weak_link::CheckProc);
    }
};

// AreaTrigger 8870
class AreaTrigger_at_forward_jalak : public AreaTriggerScript
{
    public:
        AreaTrigger_at_forward_jalak() : AreaTriggerScript("AreaTrigger_at_forward_jalak") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* jalak = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_WAR_GOD_JALAK)))
                    jalak->AI()->DoAction(ACTION_START_INTRO);
    
            return false;
        }
};

void AddSC_boss_horridon()
{
    // boss / helpers  
    new boss_horridon();
    new npc_war_god_jalak();

    // trash/mobs
    new creature_script<npc_horridon_trashs>("npc_horridon_trashs");
    new npc_horridon_summons();
    new npc_zandalari_dinomancer();
    new creature_script<npc_direhorn_spirit>("npc_direhorn_spirit");
    new creature_script<npc_living_poison>("npc_living_poison");
    new npc_venomous_effusion();
    new npc_amani_shi_warbear();
   
    // gameobject
    new go_horridon_orb_of_control();

    // Spells
    new spell_horridon_chain_lightning();
    new spell_control_horridon();
    new spell_script<spell_horridon_sand_trap>("spell_horridon_sand_trap");
    new aura_script<spell_headache>("spell_headache");
    new spell_double_swipe_aura();
    new spell_script<spell_tribe_orb_of_control>("spell_tribe_orb_of_control");
    new spell_script<spell_horridon_dire_call>("spell_horridon_dire_call");
    new spell_script<spell_horridon_summon_direhorn_spririt_selector>("spell_horridon_summon_direhorn_spririt_selector");
    new aura_script<spell_horridon_weak_link>("spell_horridon_weak_link");

    // AreaTriggers
    new AreaTrigger_at_forward_jalak();
}