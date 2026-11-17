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
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "ulduar.h"
#include "SpellInfo.h"

enum Says
{
    // Commander
    SAY_GREET                   = 0,
    SAY_GROUND_PHASE            = 1,
    SAY_SUMMON_MOLE_MACHINES    = 2,

    // Engineer
    SAY_PREPARED                = 0,
    SAY_READY                   = 1,
    SAY_TURRETS                 = 2,

    // Razorscale Controller
    EMOTE_HARPOON               = 0,

    // Razorscale
    EMOTE_BREATH                = 0,
    EMOTE_PERMA                 = 1,
};

enum Spells
{
    SPELL_FLAMEBUFFET                            = 64016,
    SPELL_FIREBALL_10                            = 62796,
    SPELL_FIREBALL_25                            = 63815,
    SPELL_FLAME_GROUND                           = 64734,
    SPELL_WINGBUFFET                             = 62666,
    SPELL_FLAMEBREATH_10                         = 63317,
    SPELL_FLAMEBREATH_25                         = 64021,
    SPELL_FUSEARMOR                              = 64771,
    SPELL_FLAMED                                 = 62696,
    SPELL_FIREBOLT                               = 62669,
    SPELL_STUN                                   = 62794,
    SPELL_BERSERK                                = 47008,

    // Additonal Spells
    // Devouring Flame Spells
    SPELL_DEVOURING_FLAME                        = 63236,
    SPELL_DEVOURING_FLAME_DAMAGE                 = 64704,
    SPELL_DEVOURING_FLAME_TRIGGER                = 64709,

    // HarpoonSpells
    SPELL_HARPOON_TRIGGER                        = 62505,
    SPELL_HARPOON_SHOT_1                         = 63658,
    SPELL_HARPOON_SHOT_2                         = 63657,
    SPELL_HARPOON_SHOT_3                         = 63659,
    SPELL_HARPOON_SHOT_4                         = 63524,

    // MoleMachine Spells
    SPELL_SUMMON_MOLE_MACHINE                    = 62899,
    SPELL_SUMMON_IRON_DWARVES                    = 63116,
    SPELL_SUMMON_IRON_DWARVES_2                  = 63114,
    SPELL_SUMMON_IRON_DWARF_GUARDIAN             = 62926,
    SPELL_SUMMON_IRON_DWARF_WATCHER              = 63135,
    SPELL_SUMMON_IRON_VRYKUL                     = 63798
};

enum DarkRuneSpells
{
    // Dark Rune Watcher
    SPELL_CHAIN_LIGHTNING                        = 64758,
    SPELL_LIGHTNING_BOLT                         = 63809,

    // Dark Rune Guardian
    SPELL_STORMSTRIKE                            = 64757,

    // Dark Rune Sentinel
    SPELL_BATTLE_SHOUT_10                        = 46763,
    SPELL_BATTLE_SHOUT_25                        = 64062,
    SPELL_HEROIC_STRIKE                          = 45026,
    SPELL_WHIRLWIND                              = 63808,
};

enum Menus
{
    GOSSIP_MENU_RAZORSCALE_START = 10314
};

enum Actions
{
    ACTION_EVENT_START                           = 1,
    ACTION_GROUND_PHASE                          = 2,
    ACTION_HARPOON_BUILD                         = 3,
    ACTION_PLACE_BROKEN_HARPOON                  = 4,
    ACTION_REMOVE_HARPOON                        = 5,
    ACTION_COMMANDER_RESET                       = 7,
    ACTION_DESPAWN_ADDS                          = 8,
    ACTION_SUMMON_MOLE_MACHINES                  = 9,
    ACTION_FIRE_OUT                              = 10,
    ACTION_CANCEL_BUILD_HARPOON                  = 11,
    ACTION_EVADE                                 = 12,
};

enum Phases
{
    PHASE_PERMAGROUND                            = 1,
    PHASE_GROUND                                 = 2,
    PHASE_FLIGHT                                 = 3,
};

enum Events
{
    EVENT_BERSERK                                = 1,
    EVENT_BREATH                                 = 2,
    EVENT_BUFFET                                 = 3,
    EVENT_FIREBALL                               = 5,
    EVENT_FLIGHT                                 = 6,
    EVENT_DEVOURING                              = 7,
    EVENT_FLAME                                  = 8,
    EVENT_LAND                                   = 9,
    EVENT_GROUND                                 = 10,
    EVENT_FUSE                                   = 11,
    EVENT_SUMMON                                 = 12,

    // Razorscale Controller
    EVENT_BUILD_HARPOON_1                        = 13,
    EVENT_BUILD_HARPOON_2                        = 14,
    EVENT_BUILD_HARPOON_3                        = 15,
    EVENT_BUILD_HARPOON_4                        = 16,
    EVENT_REBUILD_HARPOON                        = 17,

    // Mole Machine
    EVENT_SUMMON_GOB                             = 18,
    EVENT_SUMMON_NPC                             = 19,
    EVENT_DISSAPPEAR                             = 20,

    // Darkrune Watcher
    EVENT_CHAIN_LIGHTNING                        = 21,
    EVENT_LIGHTING_BOLT                          = 22,

    // Darkrune Sentinel
    EVENT_HEROIC_STRIKE                          = 23,
    EVENT_BATTLE_SHOUT                           = 24,
    EVENT_WHIRLWIND                              = 25,
};

#define SPELL_FIREBALL      RAID_MODE(SPELL_FIREBALL_10, SPELL_FIREBALL_25)
#define SPELL_FLAMEBREATH   RAID_MODE(SPELL_FLAMEBREATH_10, SPELL_FLAMEBREATH_25)
#define SPELL_BATTLE_SHOUT  RAID_MODE(SPELL_BATTLE_SHOUT_10, SPELL_BATTLE_SHOUT_25)

#define GROUND_Z                                 391.517f
#define DATA_QUICK_SHAVE                         29192921 // 2919, 2921 are achievement IDs
#define DATA_IRON_DWARF_MEDIUM_RARE              29232924

const Position PosEngRepair[4] =
{
    { 590.442f, -130.550f, GROUND_Z, 4.789f },
    { 574.850f, -133.687f, GROUND_Z, 4.252f },
    { 606.567f, -143.369f, GROUND_Z, 4.434f },
    { 560.609f, -142.967f, GROUND_Z, 5.074f },
};

const Position PosDefSpawn[4] =
{
    { 600.75f, -104.850f, GROUND_Z, 0 },
    { 596.38f, -110.262f, GROUND_Z, 0 },
    { 566.47f, -103.633f, GROUND_Z, 0 },
    { 570.41f, -108.791f, GROUND_Z, 0 },
};

const Position PosDefCombat[4] =
{
    { 614.975f, -155.138f, GROUND_Z, 4.154f },
    { 609.814f, -204.968f, GROUND_Z, 5.385f },
    { 563.531f, -201.557f, GROUND_Z, 4.108f },
    { 560.231f, -153.677f, GROUND_Z, 5.403f },
};

const Position PosHarpoon[4] =
{
    { 571.901f, -136.554f, GROUND_Z, 0 },
    { 589.450f, -134.888f, GROUND_Z, 0 },
    { 559.119f, -140.505f, GROUND_Z, 0 },
    { 606.229f, -136.721f, GROUND_Z, 0 },
};

const Position RazorFlight      = { 588.050f, -251.191f, 470.536f,      1.498f };
const Position RazorPreGround   = { 586.966f, -175.534f, GROUND_Z + 29, 1.543f };
const Position RazorGround      = { 586.966f, -175.534f, GROUND_Z,      1.543f };
const Position PosEngSpawn      = { 591.951f, -95.9680f, GROUND_Z,      0.000f };

class boss_razorscale_controller : public CreatureScript
{
    public:
        boss_razorscale_controller() : CreatureScript("boss_razorscale_controller") { }

        struct boss_razorscale_controllerAI : public BossAI
        {
            boss_razorscale_controllerAI(Creature* creature) : BossAI(creature, DATA_RAZORSCALE_CONTROL)
            {
                me->SetDisplayFromModel(1);
                me->SetFlying(true);
            }

            ObjectGuid brokenHarpoon[4];

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_PASSIVE);

                for (uint8 i = 0; i < 4; ++i)
                    brokenHarpoon[i] = ObjectGuid::Empty;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FLAMED:
                        me->RemoveAurasDueToSpell(SPELL_FLAMED);
                        if (!Is25ManRaid())
                            DoAction(ACTION_HARPOON_BUILD);
                        events.ScheduleEvent(EVENT_REBUILD_HARPOON, 22000);
                        break;
                    case SPELL_HARPOON_SHOT_1:
                    case SPELL_HARPOON_SHOT_2:
                    case SPELL_HARPOON_SHOT_3:
                    case SPELL_HARPOON_SHOT_4:
                        DoCast(SPELL_HARPOON_TRIGGER);
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
            }

            void DoAction(int32 action) override
            {
                if (instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                    return;

                switch (action)
                {
                    case ACTION_HARPOON_BUILD:
                        events.ScheduleEvent(EVENT_BUILD_HARPOON_1, 50000);
                        if (me->GetMap()->GetSpawnMode() == RAID_DIFFICULTY_25MAN_NORMAL)
                            events.ScheduleEvent(EVENT_BUILD_HARPOON_3, 90000);
                        break;
                    case ACTION_PLACE_BROKEN_HARPOON:
                        for (uint8 n = 0; n < RAID_MODE(2, 4); n++)
                            if (GameObject* go = me->SummonGameObject(GO_RAZOR_BROKEN_HARPOON, PosHarpoon[n].GetPositionX(), PosHarpoon[n].GetPositionY(), PosHarpoon[n].GetPositionZ(), 2.286f, { }, 180000))
                                brokenHarpoon[n] = go->GetGUID();
                        break;
                    case ACTION_REMOVE_HARPOON:
                        if (GameObject* Harpoon1 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_1)))
                            Harpoon1->RemoveFromWorld();
                        if (GameObject* Harpoon2 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_2)))
                            Harpoon2->RemoveFromWorld();
                        if (GameObject* Harpoon3 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_3)))
                            Harpoon3->RemoveFromWorld();
                        if (GameObject* Harpoon4 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_RAZOR_HARPOON_4)))
                            Harpoon4->RemoveFromWorld();
                        break;
                    case ACTION_CANCEL_BUILD_HARPOON:
                        events.CancelEvent(EVENT_BUILD_HARPOON_1);
                        events.CancelEvent(EVENT_BUILD_HARPOON_2);
                        events.CancelEvent(EVENT_BUILD_HARPOON_3);
                        events.CancelEvent(EVENT_BUILD_HARPOON_4);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                    return;

                if (!me->FindNearestPlayer(200.0f))
                    if (Creature* razorscale = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_RAZORSCALE)))
                    {
                        razorscale->AI()->DoAction(ACTION_EVADE);
                        return;
                    }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId != EVENT_REBUILD_HARPOON)
                    {
                        Talk(EMOTE_HARPOON);
                        if (brokenHarpoon[eventId - 13])
                            if (GameObject* go = ObjectAccessor::GetGameObject(*me, brokenHarpoon[eventId - 13]))
                                if (go->IsInWorld())
                                    go->RemoveFromWorld();
                    }

                    switch (eventId)
                    {
                        case EVENT_REBUILD_HARPOON:
                            if (instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                                return;
                            if (Creature* commander = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_EXPEDITION_COMMANDER)))
                                commander->AI()->DoAction(ACTION_FIRE_OUT);
                            DoAction(ACTION_REMOVE_HARPOON);
                            DoAction(ACTION_PLACE_BROKEN_HARPOON);
                            if (Is25ManRaid())
                                DoAction(ACTION_HARPOON_BUILD);
                            return;
                        case EVENT_BUILD_HARPOON_1:
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_1, PosHarpoon[0].GetPositionX(), PosHarpoon[0].GetPositionY(), PosHarpoon[0].GetPositionZ(), 4.790f, { }, 180000))
                            {
                                if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f)) //only nearest broken harpoon
                                    BrokenHarpoon->RemoveFromWorld();                                  
                                events.ScheduleEvent(EVENT_BUILD_HARPOON_2, 20000);
                                events.CancelEvent(EVENT_BUILD_HARPOON_1);
                            }
                            return;
                        case EVENT_BUILD_HARPOON_2:
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_2, PosHarpoon[1].GetPositionX(), PosHarpoon[1].GetPositionY(), PosHarpoon[1].GetPositionZ(), 4.659f, { }, 180000))
                            {
                                if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f))
                                    BrokenHarpoon->RemoveFromWorld();
                                events.CancelEvent(EVENT_BUILD_HARPOON_2);
                            }
                            return;
                        case EVENT_BUILD_HARPOON_3:
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_3, PosHarpoon[2].GetPositionX(), PosHarpoon[2].GetPositionY(), PosHarpoon[2].GetPositionZ(), 5.382f, { }, 180000))
                            {
                                if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f))
                                    BrokenHarpoon->RemoveFromWorld();
                                events.ScheduleEvent(EVENT_BUILD_HARPOON_4, 20000);
                                events.CancelEvent(EVENT_BUILD_HARPOON_3);
                            }
                            return;
                        case EVENT_BUILD_HARPOON_4:
                            if (GameObject* Harpoon = me->SummonGameObject(GO_RAZOR_HARPOON_4, PosHarpoon[3].GetPositionX(), PosHarpoon[3].GetPositionY(), PosHarpoon[3].GetPositionZ(), 4.266f, { }, 180000))
                            {
                                if (GameObject* BrokenHarpoon = Harpoon->FindNearestGameObject(GO_RAZOR_BROKEN_HARPOON, 5.0f))
                                    BrokenHarpoon->RemoveFromWorld();
                                events.CancelEvent(EVENT_BUILD_HARPOON_4);
                            }
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_razorscale_controllerAI>(creature);
        }
};

class boss_razorscale : public CreatureScript
{
    public:
        boss_razorscale() : CreatureScript("boss_razorscale") { }

        struct boss_razorscaleAI : public BossAI
        {
            boss_razorscaleAI(Creature* creature) : BossAI(creature, BOSS_RAZORSCALE)
            {
                // Do not let Razorscale be affected by Battle Shout buff
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_BATTLE_SHOUT, true);
            }

            Phases phase;

            uint8 FlyCount;
            uint8 HarpoonCounter;
            bool PermaGround;

            void Reset() override
            {
                me->RemoveAllAuras();
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                {
                    controller->AI()->DoAction(ACTION_REMOVE_HARPOON);
                    controller->AI()->DoAction(ACTION_PLACE_BROKEN_HARPOON);
                    controller->AI()->DoAction(ACTION_CANCEL_BUILD_HARPOON);
                }
                EntryCheckPredicate pred(NPC_MOLE_MACHINE_TRIGGER);
                summons.DoAction(ACTION_DESPAWN_ADDS, pred);
                _Reset();
                me->SetFlying(true);
                me->SetHover(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                PermaGround = false;
                HarpoonCounter = 0;
                if (Creature* commander = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_EXPEDITION_COMMANDER)))
                    commander->AI()->DoAction(ACTION_COMMANDER_RESET);

                me->GetMap()->SetWorldState(WORLDSTATE_A_QUICK_SHAVE, 1);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (phase == PHASE_FLIGHT)
                    damage = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                    controller->AI()->DoAction(ACTION_HARPOON_BUILD);
                me->SetSpeed(MOVE_RUN, 3.0f, true);
                me->SetReactState(REACT_PASSIVE);
                phase = PHASE_GROUND;
                events.SetPhase(PHASE_GROUND);
                FlyCount = 0;
                events.ScheduleEvent(EVENT_FLIGHT, 0, 0, PHASE_GROUND);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                    controller->AI()->Reset();
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_HARPOON_TRIGGER)
                    ++HarpoonCounter;
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (point)
                {
                    case 1:
                        me->m_Events.Schedule(1000, [this]()
                        {
                            me->GetMotionMaster()->MoveLand(3, RazorGround, 5.0f);
                        });
                        break;
                    case 3:
                        phase = PHASE_GROUND;
                        events.SetPhase(PHASE_GROUND);
                        events.ScheduleEvent(EVENT_LAND, 0, 0, PHASE_GROUND);
                        break;
                    case 2:
                        me->SetFacingTo(RazorFlight.GetOrientation());
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_QUICK_SHAVE)
                    if (FlyCount <= 2)
                        return 1;

                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                    return;

                events.Update(diff);

                if (me->HealthBelowPct(50) && !PermaGround)
                    EnterPermaGround();

                if (HarpoonCounter == RAID_MODE(2, 4))
                {
                    HarpoonCounter = 0;
                    events.CancelEvent(EVENT_SUMMON);
                    me->GetMotionMaster()->MovePoint(1, RazorPreGround);
                }

                if (phase == PHASE_GROUND)
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_BERSERK:
                                DoCast(me, SPELL_BERSERK);
                                return;
                            case EVENT_FLIGHT:
                                phase = PHASE_FLIGHT;
                                events.SetPhase(PHASE_FLIGHT);
                                me->SetFlying(true);
                                me->SetHover(true);
                                me->RemoveAllAuras();
                                me->SetReactState(REACT_PASSIVE);
                                me->AttackStop();
                                me->GetMotionMaster()->MoveTakeoff(2, RazorFlight);
                                events.ScheduleEvent(EVENT_FIREBALL, 7000, 0, PHASE_FLIGHT);
                                events.ScheduleEvent(EVENT_DEVOURING, 10000, 0, PHASE_FLIGHT);
                                events.ScheduleEvent(EVENT_SUMMON, 5000, 0, PHASE_FLIGHT);
                                ++FlyCount;

                                if (FlyCount > 2)
                                    me->GetMap()->SetWorldState(WORLDSTATE_A_QUICK_SHAVE, 0);
                                return;
                            case EVENT_LAND:
                                me->SetFlying(false);
                                me->SetHover(false);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
                                DoCast(me, SPELL_STUN, true);
                                me->SetFacingTo(RazorGround.GetOrientation());
                                if (Creature* commander = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_EXPEDITION_COMMANDER)))
                                    commander->AI()->DoAction(ACTION_GROUND_PHASE);
                                events.ScheduleEvent(EVENT_BREATH, 30000, 0, PHASE_GROUND);
                                events.ScheduleEvent(EVENT_BUFFET, 33000, 0, PHASE_GROUND);
                                events.ScheduleEvent(EVENT_FLIGHT, 35000, 0, PHASE_GROUND);
                                return;
                            case EVENT_BREATH:
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED);
                                me->RemoveAurasDueToSpell(SPELL_HARPOON_TRIGGER);
                                me->SetReactState(REACT_AGGRESSIVE);
                                Talk(EMOTE_BREATH);
                                DoCastAOE(SPELL_FLAMEBREATH);
                                events.CancelEvent(EVENT_BREATH);
                                return;
                            case EVENT_BUFFET:
                                me->CastSpell(me, SPELL_FIREBOLT, true);
                                DoCastAOE(SPELL_WINGBUFFET);
                                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RAZORSCALE_CONTROL)))
                                    controller->CastSpell(controller, SPELL_FLAMED, true);
                                events.CancelEvent(EVENT_BUFFET);
                                return;
                        }
                    }
                }
                else if (phase == PHASE_PERMAGROUND)
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_FLAME:
                                DoCastAOE(SPELL_FLAMEBUFFET);
                                events.ScheduleEvent(EVENT_FLAME, 10000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_BREATH:
                                Talk(EMOTE_BREATH);
                                DoCastVictim(SPELL_FLAMEBREATH);
                                events.ScheduleEvent(EVENT_BREATH, 20000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_FIREBALL:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_FIREBALL);
                                events.ScheduleEvent(EVENT_FIREBALL, 3000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_DEVOURING:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_DEVOURING_FLAME);
                                events.ScheduleEvent(EVENT_DEVOURING, 10000, 0, PHASE_PERMAGROUND);
                                return;
                            case EVENT_BUFFET:
                                DoCastAOE(SPELL_WINGBUFFET);
                                events.CancelEvent(EVENT_BUFFET);
                                return;
                            case EVENT_FUSE:
                                DoCastVictim(SPELL_FUSEARMOR);
                                events.ScheduleEvent(EVENT_FUSE, 10000, 0, PHASE_PERMAGROUND);
                                return;
                        }
                    }
                }
                else
                {
                    if (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_FIREBALL:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_FIREBALL);
                                events.ScheduleEvent(EVENT_FIREBALL, 3000, 0, PHASE_FLIGHT);
                                return;
                            case EVENT_DEVOURING:
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                    DoCast(target, SPELL_DEVOURING_FLAME, true);
                                events.ScheduleEvent(EVENT_DEVOURING, 10000, 0, PHASE_FLIGHT);
                                return;
                            case EVENT_SUMMON:
                                if (Creature* commander = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_EXPEDITION_COMMANDER) : ObjectGuid::Empty))
                                    commander->AI()->DoAction(ACTION_SUMMON_MOLE_MACHINES);
                                SummonMoleMachines();
                                events.ScheduleEvent(EVENT_SUMMON, 45000, 0, PHASE_FLIGHT);
                                return;
                        }
                    }
                }
            }

            void EnterPermaGround()
            {
                Talk(EMOTE_PERMA);
                phase = PHASE_PERMAGROUND;
                events.SetPhase(PHASE_PERMAGROUND);
                DoMeleeAttackIfReady();
                me->SetFlying(false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_PACIFIED | UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(SPELL_STUN);
                me->RemoveAurasDueToSpell(SPELL_HARPOON_TRIGGER);
                me->SetSpeed(MOVE_RUN, 1.0f, true);
                PermaGround = true;
                DoCastAOE(SPELL_FLAMEBREATH);
                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 200.0f, true))
                {
                    me->SetTarget(target->GetGUID());
                    me->AI()->AttackStart(target);
                }
                events.ScheduleEvent(EVENT_FLAME, 15000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_DEVOURING, 15000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_BREATH, 20000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_FIREBALL, 3000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_DEVOURING, 6000, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_BUFFET, 2500, 0, PHASE_PERMAGROUND);
                events.RescheduleEvent(EVENT_FUSE, 5000, 0, PHASE_PERMAGROUND);
            }

            void SummonMoleMachines()
            {
                // Adds will come in waves from mole machines. One mole can spawn a Dark Rune Watcher
                // with 1-2 Guardians, or a lone Sentinel. Up to 4 mole machines can spawn adds at any given time.
                uint8 random = urand(2, 4);
                for (uint8 n = 0; n < random; n++)
                {
                    float x = float(irand(540, 640));       // Safe range is between 500 and 650
                    float y = float(irand(-230, -195));     // Safe range is between -235 and -145
                    float z = GROUND_Z;                     // Ground level
                    me->SummonCreature(NPC_MOLE_MACHINE_TRIGGER, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000ms);
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_EVENT_START:
                        JustEngagedWith(nullptr);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 200.0f);
                        events.ScheduleEvent(EVENT_FUSE, 15*MINUTE*IN_MILLISECONDS);
                        break;
                    case ACTION_EVADE:
                        EnterEvadeMode();
                        me->SetFullHealth();
                        instance->SetBossState(BOSS_RAZORSCALE, FAIL);
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_razorscaleAI>(creature);
        }
};

class npc_expedition_commander : public CreatureScript
{
    public:
        npc_expedition_commander() : CreatureScript("npc_expedition_commander") { }

        struct npc_expedition_commanderAI : public ScriptedAI
        {
            npc_expedition_commanderAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                instance = me->GetInstanceScript();
                Greet = false;
                memset(Engineer, 0, sizeof(Engineer));
                memset(Defender, 0, sizeof(Defender));
            }

            InstanceScript* instance;
            SummonList summons;
            bool Greet;
            uint32 AttackStartTimer;
            uint8  Phase;
            ObjectGuid Engineer[4];
            ObjectGuid Defender[4];

            void Reset() override
            {
                AttackStartTimer = 0;
                Phase = 0;
                Greet = false;
                summons.DespawnAll();
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!Greet && me->IsWithinDistInMap(who, 10.0f) && who->GetTypeId() == TYPEID_PLAYER)
                {
                    Talk(SAY_GREET);
                    Greet = true;
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_GROUND_PHASE:
                        Talk(SAY_GROUND_PHASE);
                        break;
                    case ACTION_SUMMON_MOLE_MACHINES:
                        Talk(SAY_SUMMON_MOLE_MACHINES);
                        break;
                    case ACTION_COMMANDER_RESET:
                        summons.DespawnAll();
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                    case ACTION_FIRE_OUT:
                        if (Creature* engineer = ObjectAccessor::GetCreature(*me, Engineer[0]))
                            engineer->AI()->Talk(SAY_TURRETS);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (AttackStartTimer <= diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            instance->SetBossState(BOSS_RAZORSCALE, IN_PROGRESS);
                            summons.DespawnAll();
                            AttackStartTimer = 1000;
                            Phase = 2;
                            break;
                        case 2:
                            for (uint8 n = 0; n < RAID_MODE(2, 4); n++)
                            {
                                if (Creature* engineer = me->SummonCreature(NPC_ENGINEER, PosEngSpawn, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000ms))
                                {
                                    Engineer[n] = engineer->GetGUID();
                                    engineer->SetWalk(false);
                                    engineer->SetSpeed(MOVE_RUN, 0.5f);
                                    engineer->SetHomePosition(PosEngRepair[n]);
                                    engineer->GetMotionMaster()->MoveTargetedHome();
                                }
                            }
                            if (Creature* engineer = ObjectAccessor::GetCreature(*me, Engineer[0]))
                                engineer->AI()->Talk(SAY_READY);
                            Phase = 3;
                            AttackStartTimer = 14000;
                            break;
                        case 3:
                            for (uint8 n = 0; n < 4; n++)
                            {
                                if (Creature* defender = me->SummonCreature(NPC_DEFENDER, PosDefSpawn[n], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000ms))
                                {
                                    Defender[n] = defender->GetGUID();
                                    defender->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_NPC);
                                    defender->SetWalk(false);
                                    defender->SetHomePosition(PosDefCombat[n]);
                                    defender->GetMotionMaster()->MoveTargetedHome();
                                }
                            }
                            Phase = 4;
                            break;
                        case 4:
                            for (uint8 n = 0; n < RAID_MODE(2, 4); n++)
                                if (Creature* engineer = ObjectAccessor::GetCreature(*me, Engineer[n]))
                                    engineer->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                            for (uint8 n = 0; n < 4; ++n)
                                if (Creature* defender = ObjectAccessor::GetCreature(*me, Defender[n]))
                                    defender->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                            AttackStartTimer = 16000;
                            Phase = 5;
                            break;
                        case 5:
                            if (Creature* Razorscale = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_RAZORSCALE)))
                            {
                                Razorscale->AI()->DoAction(ACTION_EVENT_START);
                                me->SetInCombatWith(Razorscale);
                            }
                            if (Creature* engineer = ObjectAccessor::GetCreature(*me, Engineer[0]))
                                engineer->AI()->Talk(SAY_PREPARED);
                            Phase = 6;
                            break;
                    }
                }
                else
                    AttackStartTimer -= diff;
            }
        };

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF:
                    player->CLOSE_GOSSIP_MENU();
                    CAST_AI(npc_expedition_commanderAI, creature->AI())->Phase = 1;
                    break;
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

            InstanceScript* instance = creature->GetInstanceScript();
            if (instance && instance->GetBossState(BOSS_RAZORSCALE) != DONE && instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
            {
                player->PrepareGossipMenu(creature);

                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Мы готовы помочь!" : "Activate Harpoons!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(13853, creature->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(13910, creature->GetGUID());

            return true;
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_expedition_commanderAI>(creature);
        }
};

class npc_mole_machine_trigger : public CreatureScript
{
    public:
        npc_mole_machine_trigger() : CreatureScript("npc_mole_machine_trigger") { }

        struct npc_mole_machine_triggerAI : public ScriptedAI
        {
            npc_mole_machine_triggerAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetVisible(false);
                SetCombatMovement(false);
            }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_SUMMON_GOB, 2000);
                events.ScheduleEvent(EVENT_SUMMON_NPC, 6000);
                events.ScheduleEvent(EVENT_DISSAPPEAR, 10000);
            }
            
            void DoAction(int32 /*action*/) override
            {
                summons.DespawnAll();
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                
                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_SUMMON_GOB:
                            //DoCast(SPELL_SUMMON_MOLE_MACHINE);
                            if (GameObject* go = me->SummonGameObject(GO_MOLE_MACHINE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 2.286f, { }, 180000))
                            {
                                go->SetLootState(GO_READY);
                                go->UseDoorOrButton(10000, false);
                            }
                            break;
                        case EVENT_SUMMON_NPC:
                        {
                            switch (RAND(SPELL_SUMMON_IRON_DWARVES, SPELL_SUMMON_IRON_DWARVES_2))
                            {
                                case SPELL_SUMMON_IRON_DWARVES:
                                    // Emulator for DoCast(SPELL_SUMMON_IRON_DWARVES); -> SpellScript did not work!
                                    for (uint8 n = 0; n < urand(1, 2); ++n) 
                                        me->CastSpell(me, SPELL_SUMMON_IRON_DWARF_GUARDIAN, false);
                                    me->CastSpell(me, SPELL_SUMMON_IRON_DWARF_WATCHER, false);
                                    break;
                                case SPELL_SUMMON_IRON_DWARVES_2:
                                    me->CastSpell(me, SPELL_SUMMON_IRON_VRYKUL, false);
                                    break;
                            }
                            break;
                        }
                        case EVENT_DISSAPPEAR:
                            if (GameObject* molemachine = me->FindNearestGameObject(GO_MOLE_MACHINE, 1))
                                molemachine->Delete();
                            
                            me->DisappearAndDie();
                            break;
                    }
                }
            }

            void JustSummoned(Creature* summoned) override
            {
                summons.Summon(summoned);
                summoned->AI()->DoZoneInCombat();
            }
            
            private:
                SummonList summons;
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mole_machine_triggerAI>(creature);
        }
};

class npc_devouring_flame : public CreatureScript
{
    public:
        npc_devouring_flame() : CreatureScript("npc_devouring_flame") { }

        struct npc_devouring_flameAI : public ScriptedAI
        {
            npc_devouring_flameAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetDisplayFromModel(1);
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void Reset() override
            {
                DoCast(SPELL_FLAME_GROUND);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override { }
            void AttackStart(Unit* who) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_devouring_flameAI>(creature);
        }
};

class npc_razorscale_harpoon_fire_state : public CreatureScript
{
    public:
        npc_razorscale_harpoon_fire_state() : CreatureScript("npc_razorscale_harpoon_fire_state") { }

        struct npc_razorscale_harpoon_fire_stateAI : public ScriptedAI
        {
            npc_razorscale_harpoon_fire_stateAI(Creature* creature) : ScriptedAI(creature)
            { 
                SetCombatMovement(false);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_razorscale_harpoon_fire_stateAI>(creature);
        }
};

class npc_darkrune_watcher : public CreatureScript
{
    public:
        npc_darkrune_watcher() : CreatureScript("npc_darkrune_watcher") { }

        struct npc_darkrune_watcherAI : public ScriptedAI
        {
            npc_darkrune_watcherAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(10000, 15000));
                events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(1000, 3000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_CHAIN_LIGHTNING:
                            DoCastVictim(SPELL_CHAIN_LIGHTNING);
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(10000, 15000));
                            return;
                        case EVENT_LIGHTING_BOLT:
                            DoCastVictim(SPELL_LIGHTNING_BOLT);
                            events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(5000, 7000));
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_darkrune_watcherAI>(creature);
        }
};

class npc_darkrune_guardian : public CreatureScript
{
    public:
        npc_darkrune_guardian() : CreatureScript("npc_darkrune_guardian") { }

        struct npc_darkrune_guardianAI : public ScriptedAI
        {
            npc_darkrune_guardianAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 StormTimer;

            void Reset() override
            {
                StormTimer = urand(3000, 6000);
                killedByBreath = false;
            }

            uint32 GetData(uint32 type) const override
            {
                return type == DATA_IRON_DWARF_MEDIUM_RARE ? killedByBreath : 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_IRON_DWARF_MEDIUM_RARE)
                    killedByBreath = data;
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (StormTimer <= diff)
                {
                    DoCastVictim(SPELL_STORMSTRIKE);
                    StormTimer = urand(4000, 8000);
                }
                else
                    StormTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            bool killedByBreath;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_darkrune_guardianAI>(creature);
        }
};

class npc_darkrune_sentinel : public CreatureScript
{
    public:
        npc_darkrune_sentinel() : CreatureScript("npc_darkrune_sentinel") { }

        struct npc_darkrune_sentinelAI : public ScriptedAI
        {
            npc_darkrune_sentinelAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                events.ScheduleEvent(EVENT_HEROIC_STRIKE, urand(4000, 8000));
                events.ScheduleEvent(EVENT_BATTLE_SHOUT, urand(15000, 30000));
                if (Is25ManRaid())
                    events.ScheduleEvent(EVENT_WHIRLWIND, urand(20000, 25000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetBossState(BOSS_RAZORSCALE) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_HEROIC_STRIKE:
                            DoCastVictim(SPELL_HEROIC_STRIKE);
                            events.ScheduleEvent(EVENT_HEROIC_STRIKE, urand(4000, 6000));
                            return;
                        case EVENT_BATTLE_SHOUT:
                            DoCast(me, SPELL_BATTLE_SHOUT);
                            events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(30000, 40000));
                            return;
                        case EVENT_WHIRLWIND:
                            DoCast(me, SPELL_WHIRLWIND);
                            events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(20000, 25000));
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_darkrune_sentinelAI>(creature);
        }
};

class go_razorscale_harpoon : public GameObjectScript
{
    public:
        go_razorscale_harpoon() : GameObjectScript("go_razorscale_harpoon") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            if (go->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE))
                return true;

            InstanceScript* instance = go->GetInstanceScript();
            if (ObjectAccessor::GetCreature(*go, instance->GetGuidData(BOSS_RAZORSCALE)))
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
            return false;
        }
};

class spell_razorscale_devouring_flame : public SpellScriptLoader
{
    public:
        spell_razorscale_devouring_flame() : SpellScriptLoader("spell_razorscale_devouring_flame") { }

        class spell_razorscale_devouring_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razorscale_devouring_flame_SpellScript);

            void SelectTarget(SpellDestination& dest)
            {
                dest._position.m_positionZ = GROUND_Z;
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_razorscale_devouring_flame_SpellScript::SelectTarget, EFFECT_0, TARGET_DEST_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razorscale_devouring_flame_SpellScript();
        }
};

class spell_razorscale_firebolt : public SpellScriptLoader
{
    public:
        spell_razorscale_firebolt() : SpellScriptLoader("spell_razorscale_firebolt") { }

        class spell_razorscale_firebolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razorscale_firebolt_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, SPELL_FLAMED, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_razorscale_firebolt_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razorscale_firebolt_SpellScript();
        }
};

class spell_razorscale_flame_breath : public SpellScriptLoader
{
    public:
        spell_razorscale_flame_breath() : SpellScriptLoader("spell_razorscale_flame_breath") { }

        class spell_razorscale_flame_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_razorscale_flame_breath_SpellScript);

            void CheckDamage()
            {
                Creature* target = GetHitCreature();
                if (!target || target->GetEntry() != NPC_DARK_RUNE_GUARDIAN)
                    return;

                if (GetHitDamage() >= int32(target->GetHealth()))
                    target->AI()->SetData(DATA_IRON_DWARF_MEDIUM_RARE, 1);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_razorscale_flame_breath_SpellScript::CheckDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_razorscale_flame_breath_SpellScript();
        }
};

class achievement_iron_dwarf_medium_rare : public AchievementCriteriaScript
{
    public:
        achievement_iron_dwarf_medium_rare() : AchievementCriteriaScript("achievement_iron_dwarf_medium_rare") { }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            return target && target->IsAIEnabled && target->GetAI()->GetData(DATA_IRON_DWARF_MEDIUM_RARE);
        }
};

void AddSC_boss_razorscale()
{
    new boss_razorscale_controller();
    new boss_razorscale();
    new npc_expedition_commander();
    new npc_mole_machine_trigger();
    new npc_devouring_flame();
    new npc_razorscale_harpoon_fire_state();
    new npc_darkrune_watcher();
    new npc_darkrune_guardian();
    new npc_darkrune_sentinel();
    new go_razorscale_harpoon();
    new spell_razorscale_devouring_flame();
    new spell_razorscale_flame_breath();
    new spell_razorscale_firebolt();
    new achievement_iron_dwarf_medium_rare();
}

#undef SPELL_FIREBALL
#undef SPELL_FLAMEBREATH
#undef SPELL_BATTLE_SHOUT
