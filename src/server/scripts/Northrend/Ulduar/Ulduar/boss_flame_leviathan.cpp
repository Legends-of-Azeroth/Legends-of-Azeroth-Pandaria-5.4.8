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

/*
 * Comment: there is missing code on triggers,
 *          brann bronzebeard needs correct gossip info.
 *          requires more work involving area triggers.
 *          if reached brann speaks through his radio..
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "ObjectMgr.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "ulduar.h"
#include "Spell.h"
#include "GameObject.h"
#include "Random.h"

enum Spells
{
    // Npc Collosus
    SPELL_GROUND_SLAM              = 62625,

    SPELL_PURSUED                  = 62374,
    SPELL_GATHERING_SPEED          = 62375,
    SPELL_BATTERING_RAM            = 62376,
    SPELL_FLAME_VENTS              = 62396,
    SPELL_MISSILE_BARRAGE          = 62400,
    SPELL_SYSTEMS_SHUTDOWN         = 62475,
    SPELL_OVERLOAD_CIRCUIT         = 62399,
    SPELL_START_THE_ENGINE         = 62472,
    SPELL_SEARING_FLAME            = 62402,
    SPELL_BLAZE                    = 62292,
    SPELL_TAR_PASSIVE              = 62288,
    SPELL_SMOKE_TRAIL              = 63575,
    SPELL_ELECTROSHOCK             = 62522,
    SPELL_NAPALM                   = 63666,
    SPELL_INVIS_AND_STEALTH_DETECT = 18950, // Passive
    // TOWER Additional SPELLS
    SPELL_THORIM_S_HAMMER          = 62911, // Tower of Storms
    SPELL_MIMIRON_S_INFERNO        = 62909, // Tower of Flames
    SPELL_HODIR_S_FURY             = 62533, // Tower of Frost
    SPELL_FREYA_S_WARD             = 62906, // Tower of Nature
    SPELL_HODIRS_FURY_AURA         = 62297,
    SPELL_TARGET_SEARCH_A          = 63761,
    SPELL_TARGET_SEARCH_B          = 63762,
    SPELL_TARGET_SEARCH_C          = 63763,
    SPELL_TARGET_SEARCH_D          = 63764,
    // TOWER ap & health spells
    SPELL_BUFF_TOWER_OF_STORMS     = 65076,
    SPELL_BUFF_TOWER_OF_FLAMES     = 65075,
    SPELL_BUFF_TOWER_OF_FROST      = 65077,
    SPELL_BUFF_TOWER_OF_LIFE       = 64482,
    // Additional Spells
    SPELL_LASH                     = 65062,
    SPELL_AUTO_REPAIR              = 62705,
    SPELL_LIQUID_PYRITE            = 62494,
    SPELL_DUSTY_EXPLOSION          = 63360,
    SPELL_DUST_CLOUD_IMPACT        = 54740,
    SPELL_FLAMES_TAR               = 65044,
    SPELL_FLAMES_VEHICLES          = 65045,
    SPELL_HOOKSHOT_AURA            = 62336,
    SPELL_PASSENGER_LOADED         = 62340,
};

enum Events
{
    EVENT_PURSUE               = 1,
    EVENT_MISSILE              = 2,
    EVENT_VENT                 = 3,
    EVENT_SPEED                = 4,
    EVENT_SUMMON               = 5,
    EVENT_SHUTDOWN             = 6,
    EVENT_REPAIR               = 7,
    EVENT_THORIM_S_HAMMER      = 8,    // Tower of Storms
    EVENT_MIMIRON_S_INFERNO    = 9,    // Tower of Flames
    EVENT_HODIR_S_FURY         = 10,   // Tower of Frost
    EVENT_FREYA_S_WARD         = 11,   // Tower of Nature
    EVENT_PREPARE              = 12,
    EVENT_CHECK_WIPE           = 13,
    EVENT_PYRITE_DESPAWN       = 14,
    EVENT_SAY_ORBITAL_ONLINE   = 15,
    EVENT_SAY_ORBITAL_OFFLINE  = 16,
};

enum Seats
{
    // Flame Leviathan Seat
    SEAT_PLAYER    = 0,
    SEAT_TURRET    = 1,
    SEAT_DEVICE    = 2,

    // Flame Leviathan
    SEAT_SEAT_BACK_LEFT  = 0,
    SEAT_SEAT_BACK_RIGHT = 1,
    SEAT_SEAT_FRONT_LEFT  = 2,
    SEAT_SEAT_FRONT_RIGHT = 3,
    SEAT_CANNON     = 7,

    // Hodir's Fury Targetting Reticle, Thorim's Hammer Targetting Reticle, Freya's Ward Targetting Reticle, Mimiron's Inferno Targetting Reticle
    SEAT_CASTER = 0,
};

enum DisplayIds
{
    DISPLAY_ID_DAMAGED_PYRITE_CONTAINER     = 28783
};

enum Misc
{
    DATA_SHUTOUT               = 29112912, // 2911, 2912 are achievement IDs
    DATA_ORBIT_ACHIEVEMENTS    = 1,
    FREYA_SPAWNS               = 4,
    ACTION_EVADE               = 15,
    ACTION_NEW_PURSUED         = 16,
    ACTION_INCREMENT_SHUTDOWN  = 17,
    POINT_PREPARE              = 1
};

enum Yells
{
    SAY_AGGRO            = 0,
    SAY_SLAY             = 1,
    SAY_DEATH            = 2,
    SAY_TARGET           = 3,
    SAY_HARDMODE         = 4,
    SAY_TOWER_NONE       = 5,
    SAY_TOWER_FROST      = 6,
    SAY_TOWER_FLAME      = 7,
    SAY_TOWER_NATURE     = 8,
    SAY_TOWER_STORM      = 9,
    SAY_PLAYER_RIDING    = 10,
    SAY_OVERLOAD         = 11,
    EMOTE_PURSUE         = 12,
    EMOTE_OVERLOAD       = 13,
    EMOTE_REPAIR         = 14,
    EMOTE_REACTIVATE     = 15,
    EMOTE_OFFLINE        = 16, // Unused, purpose unknown
    EMOTE_TOWER_FROST    = 17,
    EMOTE_TOWER_FLAME    = 18,
    EMOTE_TOWER_NATURE   = 19,
    EMOTE_TOWER_STORM    = 20,
};

enum MiscellanousData
{
    // Amount of seats depending on Raid mode
    TWO_SEATS                 = 2,
    FOUR_SEATS                = 4,
};

#define RETICLE_HOVER_HEIGHT 1.42716f

Position const Center = { 354.8771f, -12.90240f, 409.803650f, 3.1376f };
Position const InfernoStart = { 390.93f, -13.91f, 409.81f, 0.0f };

Position const FreyaBeacons[FREYA_SPAWNS] =
{
    {377.02f, -119.10f, 409.81f, 0.0f},
    {185.62f, -119.10f, 409.81f, 0.0f},
    {377.02f, 54.78f, 409.81f, 0.0f},
    {185.62f, 54.78f, 409.81f, 0.0f},
};

struct FlameLeviathanPursuedTargetSelector 
{
    bool operator() (WorldObject* target)
    {
        // Check target type
        Unit* unit = target->ToUnit();
        if (!unit || unit->isDead())
            return true;

        // check area
        if (unit->GetAreaId() != AREA_FORMATION_GROUNDS)
            return true;

        // ignore players loaded on leviathan seats
        if (unit->GetVehicleBase() && unit->GetVehicleBase()->GetEntry() == NPC_LEVIATHAN_SEATS)
            return true;

        // if target is creature
        Creature* creatureTarget = unit->ToCreature();
        if (creatureTarget)
        {
            // needs to be one of the 3 vehicles
            if (creatureTarget->GetEntry() != NPC_SALVAGED_DEMOLISHER &&
                creatureTarget->GetEntry() != NPC_SALVAGED_SIEGE_ENGINE &&
                creatureTarget->GetEntry() != NPC_CHOPPER)
                return true;

            // must be a valid vehicle installation
            Vehicle* vehicle = creatureTarget->GetVehicleKit();
            if (!vehicle)
                return true;

            // vehicle must be in use by player
            bool playerFound = false;
            if (!playerFound)
                for (auto&& itr : vehicle->Seats)
                    if (itr.second.Passenger.Guid.IsPlayer())
                        playerFound = true;

            return !playerFound;
        }

        return false;
    }
};

class ThorimsHammerSummon : public BasicEvent
{
    public:
        ThorimsHammerSummon(Creature* leviathan, uint32 startTime) : leviathan(leviathan), instance(leviathan->GetInstanceScript()), timeToStop(startTime + 30000) { }

        bool Execute(uint64 time, uint32 diff)
        {
            if (getMSTime() >= timeToStop || !leviathan->IsAlive() || instance->GetBossState(BOSS_LEVIATHAN) != IN_PROGRESS)
                return true;

            leviathan->SummonCreature(NPC_THORIMS_HAMMER_TARGETTING_RETICLE, frand(160.0f, 376.0f), frand(-132.0f, 68.0f), 409.8f);
            leviathan->m_Events.Schedule(1200, this);
            return false;
        }

    private:
        Creature* leviathan;
        InstanceScript* instance;
        uint32 timeToStop;
};

class boss_flame_leviathan : public CreatureScript
{
    public:
        boss_flame_leviathan() : CreatureScript("boss_flame_leviathan") { }

        struct boss_flame_leviathanAI : public BossAI
        {
            boss_flame_leviathanAI(Creature* creature) : BossAI(creature, BOSS_LEVIATHAN), vehicle(creature->GetVehicleKit()) { }

            void InitializeAI() override
            {
                ASSERT(vehicle);

                ActiveTowersCount = 4;
                Shutdown = 0;
                towerOfStorms = false;
                towerOfLife = false;
                towerOfFlames = false;
                towerOfFrost = false;
                Shutout = true;
                me->GetMap()->SetWorldState(WORLDSTATE_SHUTOUT, 1);
                Unbroken = true;
                loaded = false;

                DoCast(me, SPELL_INVIS_AND_STEALTH_DETECT);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED);
                me->SetReactState(REACT_PASSIVE);

                if (!me->isDead())
                    Reset();

                me->GetMap()->SetWorldState(WORLDSTATE_ORBITAL_BOMBARDMENT, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_ORBITAL_DEVASTATION, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_NUKED_FROM_ORBIT, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_ORBIT_UARY, 0);
            }

            Vehicle* vehicle;
            uint8 ActiveTowersCount;
            uint8 Shutdown;
            bool towerOfStorms;
            bool towerOfLife;
            bool towerOfFlames;
            bool towerOfFrost;
            bool Shutout;
            bool Unbroken;
            bool loaded;
            bool firstPursuit = true;

            void Reset() override
            {
                _Reset();
                // resets shutdown counter to 0.  2 or 4 depending on raid mode
                Shutdown = 0;
                _pursueTarget = ObjectGuid::Empty;

                me->SetVisible(true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                if (loaded)
                    return;

                me->GetMap()->SetWorldState(WORLDSTATE_UNBROKEN, 1);
                loaded = true;
                me->setActive(true);
                me->m_Events.Schedule(1, [this]()
                {
                    me->setActive(false);
                    if (instance->GetData(DATA_LEVI_HARD_MODE))
                    {
                        towerOfStorms = true;
                        towerOfLife = true;
                        towerOfFlames = true;
                        towerOfFrost = true;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                        {
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_STORMS, true);
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_FROST, true);
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_FLAMES, true);
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_LIFE, true);
                        }
                        me->SetLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2 | LOOT_MODE_HARD_MODE_3 | LOOT_MODE_HARD_MODE_4);
                    }
                    else
                    {
                        towerOfStorms = false;
                        towerOfLife = false;
                        towerOfFlames = false;
                        towerOfFrost = false;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                        {
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_STORMS);
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FROST);
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FLAMES);
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_LIFE);
                        }
                    }
                });

                if (instance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                {
                    vehicle->InstallAccessory(NPC_LEVIATHAN_SEATS, 0, true, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 0);
                    vehicle->InstallAccessory(NPC_LEVIATHAN_SEATS, 1, true, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 0);
                }

                me->GetMap()->SetWorldState(WORLDSTATE_ORBITAL_BOMBARDMENT, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_ORBITAL_DEVASTATION, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_NUKED_FROM_ORBIT, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_ORBIT_UARY, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                // idk fow it must works, but Shutdown increments after reset but not in combat
                Shutdown = 0;

                if (!FindNearestAttackableTarget())
                {
                    EnterEvadeMode();
                    return;
                }

                _JustEngagedWith();
                firstPursuit = true;
                events.ScheduleEvent(EVENT_PURSUE, 1);
                events.ScheduleEvent(EVENT_MISSILE, urand(1500, 4*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VENT, 30*IN_MILLISECONDS);
                //events.ScheduleEvent(EVENT_SHUTDOWN, 150*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SPEED, 10*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SUMMON, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                ActiveTower(); //void ActiveTower

                // Achievement controller
                if (ActiveTowersCount >= 1)
                    me->GetMap()->SetWorldState(WORLDSTATE_ORBITAL_BOMBARDMENT, 1);

                if (ActiveTowersCount >= 2)
                    me->GetMap()->SetWorldState(WORLDSTATE_ORBITAL_DEVASTATION, 1);

                if (ActiveTowersCount >= 3)
                    me->GetMap()->SetWorldState(WORLDSTATE_NUKED_FROM_ORBIT, 1);

                if (ActiveTowersCount == 4)
                    me->GetMap()->SetWorldState(WORLDSTATE_ORBIT_UARY, 1);
            }

            void EnterEvadeMode() override
            {
                Reset();
                instance->SetBossState(BOSS_LEVIATHAN, FAIL);
                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                me->DeleteThreatList();
                me->CombatStop(true);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PURSUED);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BATTERING_RAM);
                me->GetMotionMaster()->MoveTargetedHome();
                events.Reset();
            }

            void ActiveTower()
            {
                Talk(SAY_AGGRO);
                if (instance->GetData(DATA_LEVI_HARD_MODE))
                {
                    Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT));

                    if (towerOfFrost)
                    {
                        if (orbitalSupport)
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_FROST, true);
                        events.ScheduleEvent(EVENT_HODIR_S_FURY, 20*IN_MILLISECONDS);
                    }
                    if (towerOfLife)
                    {
                        if (orbitalSupport)
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_LIFE, true);
                        events.ScheduleEvent(EVENT_FREYA_S_WARD, 30*IN_MILLISECONDS);
                    }
                    if (towerOfFlames)
                    {
                        if (orbitalSupport)
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_FLAMES, true);
                        events.ScheduleEvent(EVENT_MIMIRON_S_INFERNO, 40*IN_MILLISECONDS);
                    }
                    if (towerOfStorms)
                    {
                        if (orbitalSupport)
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_STORMS, true);
                        events.ScheduleEvent(EVENT_THORIM_S_HAMMER, 60*IN_MILLISECONDS);
                    }

                    events.ScheduleEvent(towerOfLife || towerOfFrost || towerOfFlames || towerOfStorms ? EVENT_SAY_ORBITAL_ONLINE : EVENT_SAY_ORBITAL_OFFLINE, 11000);
                }
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                vehicle->Reset();
                me->SetFullHealth();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAllAuras();
                _JustDied();
                // Set Field Flags 67108928 = 64 | 67108864 = UNIT_FLAG_UNK_6 | UNIT_FLAG_SKINNABLE
                // Set DynFlags 12
                // Set NPCFlags 0
                Talk(SAY_DEATH);
                if (GameObject* door = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_LEVI_DOOR)))
                    instance->HandleGameObject(door->GetGUID(), true);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_START_THE_ENGINE)
                {
                    me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                    vehicle->InstallAllAccessories(false);
                }

                if (spell->Id == SPELL_ELECTROSHOCK)
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SHUTOUT:
                        return Shutout ? 1 : 0;
                    case DATA_UNBROKEN:
                        return Unbroken ? 1 : 0;
                    case DATA_ORBIT_ACHIEVEMENTS:
                        if (instance->GetData(DATA_LEVI_HARD_MODE)) // Only on HardMode
                            return ActiveTowersCount;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_UNBROKEN)
                {
                    me->GetMap()->SetWorldState(WORLDSTATE_UNBROKEN, 0);
                    Unbroken = data ? true : false;
                }
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type == POINT_MOTION_TYPE && point == POINT_PREPARE)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED);
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (Unit* target = FindNearestAttackableTarget())
                        me->SetInCombatWith(target);
                    if (GameObject* door = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_LEVI_DOOR)))
                        instance->HandleGameObject(door->GetGUID(), false);
                    me->UpdateObjectVisibility();
                    me->SetHomePosition(Center);
                }
            }

            void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
            {
                if (apply && (seatId == SEAT_SEAT_BACK_LEFT || seatId == SEAT_SEAT_BACK_RIGHT || seatId == SEAT_SEAT_FRONT_LEFT || seatId == SEAT_SEAT_FRONT_RIGHT))
                {
                    who->m_movementInfo.transport.pos.SetOrientation(M_PI);
                    who->SendMovementFlagUpdate();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat() || !CheckInRoom())
                    return;

                if ((me->GetDistance(me->GetHomePosition()) > 240.0f) || (me->GetPositionX() < 140.00f))
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (Shutdown == RAID_MODE(TWO_SEATS, FOUR_SEATS))
                {
                    Shutdown = 0;
                    events.ScheduleEvent(EVENT_SHUTDOWN, 4000);
                    me->RemoveAurasDueToSpell(SPELL_OVERLOAD_CIRCUIT);
                    me->InterruptNonMeleeSpells(true);
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!me->GetVictim() && !me->HasAura(SPELL_SYSTEMS_SHUTDOWN))
                    events.RescheduleEvent(EVENT_PURSUE, 0);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_WIPE:
                            {
                                if (!FindNearestAttackableTarget())
                                    EnterEvadeMode();
                                else
                                    events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                            }
                            break;
                        case EVENT_PURSUE:
                            if (firstPursuit)
                                firstPursuit = false;
                            else
                                Talk(SAY_TARGET);
                            DoCastAOE(SPELL_PURSUED);  // Will select target in spellscript
                            events.ScheduleEvent(EVENT_PURSUE, 35*IN_MILLISECONDS);
                            break;
                        case EVENT_MISSILE:
                            DoCast(me, SPELL_MISSILE_BARRAGE, true);
                            events.ScheduleEvent(EVENT_MISSILE, 2*IN_MILLISECONDS);
                            break;
                        case EVENT_VENT:
                            DoCastAOE(SPELL_FLAME_VENTS);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_VENT, 20*IN_MILLISECONDS);
                            break;
                        case EVENT_SPEED:
                            DoCastAOE(SPELL_GATHERING_SPEED);
                            events.ScheduleEvent(EVENT_SPEED, 10*IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMON:
                            if (summons.size() < 15)
                                if (Creature* lift = DoSummonFlyer(NPC_MECHANOLIFT, me, 30.0f, 50.0f, 0))
                                    lift->GetMotionMaster()->MoveRandom(100);
                            events.ScheduleEvent(EVENT_SUMMON, 2*IN_MILLISECONDS);
                            break;
                        case EVENT_SHUTDOWN:
                            Talk(SAY_OVERLOAD);
                            Talk(EMOTE_OVERLOAD);
                            me->CastSpell(me, SPELL_SYSTEMS_SHUTDOWN, true);
                            if (Shutout)
                            {
                                me->GetMap()->SetWorldState(WORLDSTATE_SHUTOUT, 0);
                                Shutout = false;
                            }
                            events.DelayEvents(20 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_REPAIR, 4000);
                            break;
                        case EVENT_REPAIR:
                            for (uint8 seatId = SEAT_SEAT_BACK_LEFT; seatId <= SEAT_SEAT_FRONT_RIGHT; ++seatId)
                            {
                                if (Unit* seat = vehicle->GetPassenger(seatId))
                                {
                                    if (Vehicle* seatVehicle = seat->GetVehicleKit())
                                    {
                                        if (Unit* player = seatVehicle->GetPassenger(SEAT_PLAYER))
                                        {
                                            player->GetMotionMaster()->MoveKnockbackFrom(me->GetPositionX(), me->GetPositionY(), 30, 30);
                                            player->ExitVehicle();
                                            player->CastSpell(player, SPELL_SMOKE_TRAIL, true);
                                        }
                                    }
                                }
                            }
                            Talk(EMOTE_REPAIR);
                            //events.ScheduleEvent(EVENT_SHUTDOWN, 150*IN_MILLISECONDS);
                            events.CancelEvent(EVENT_REPAIR);
                            break;
                        case EVENT_THORIM_S_HAMMER: // Tower of Storms
                            me->m_Events.Schedule(1, new ThorimsHammerSummon(me, getMSTime()));
                            Talk(SAY_TOWER_STORM);
                            Talk(EMOTE_TOWER_STORM);
                            events.CancelEvent(EVENT_THORIM_S_HAMMER);
                            break;
                        case EVENT_MIMIRON_S_INFERNO: // Tower of Flames
                            me->SummonCreature(NPC_MIMIRONS_INFERNO_TARGETTING_RETICLE, InfernoStart);
                            Talk(SAY_TOWER_FLAME);
                            Talk(EMOTE_TOWER_FLAME);
                            events.CancelEvent(EVENT_MIMIRON_S_INFERNO);
                            break;
                        case EVENT_HODIR_S_FURY:      // Tower of Frost
                            for (uint8 i = 0; i < 2; ++i)
                                me->SummonCreature(NPC_HODIRS_FURY_TARGETTING_RETICLE, frand(160.0f, 376.0f), frand(-132.0f, 68.0f), 409.8f + RETICLE_HOVER_HEIGHT);
                            Talk(SAY_TOWER_FROST);
                            Talk(EMOTE_TOWER_FROST);
                            events.CancelEvent(EVENT_HODIR_S_FURY);
                            break;
                        case EVENT_FREYA_S_WARD:    // Tower of Nature
                            for (int32 i = 0; i < 4; ++i)
                                me->SummonCreature(NPC_FREYAS_WARD_TARGETTING_RETICLE, FreyaBeacons[i]);
                            Talk(SAY_TOWER_NATURE);
                            Talk(EMOTE_TOWER_NATURE);
                            events.CancelEvent(EVENT_FREYA_S_WARD);
                            break;
                        case EVENT_SAY_ORBITAL_ONLINE:
                            Talk(SAY_HARDMODE);
                            instance->SetData(DATA_AREATRIGGER_EVENT, RADIO_ORBITAL_SUPPORT);
                            break;
                        case EVENT_SAY_ORBITAL_OFFLINE:
                            Talk(SAY_TOWER_NONE);
                            break;
                    }
                }

                DoBatteringRamIfReady();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_PURSUED)
                    _pursueTarget = target->GetGUID();
            }

            void DoAction(int32 action) override
            {
                if (action && action <= 4) // Tower destruction, debuff leviathan loot and reduce active tower count
                {
                    if (me->HasLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2 | LOOT_MODE_HARD_MODE_3 | LOOT_MODE_HARD_MODE_4) && ActiveTowersCount == 4)
                    {
                        me->RemoveLootMode(LOOT_MODE_HARD_MODE_4);
                        --ActiveTowersCount;
                    }
                    else if (me->HasLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2 | LOOT_MODE_HARD_MODE_3) && ActiveTowersCount == 3)
                    {
                        me->RemoveLootMode(LOOT_MODE_HARD_MODE_3);
                        --ActiveTowersCount;
                    }
                    else if (me->HasLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2) && ActiveTowersCount == 2)
                    {
                        me->RemoveLootMode(LOOT_MODE_HARD_MODE_2);
                        --ActiveTowersCount;
                    }
                    else if (me->HasLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1) && ActiveTowersCount == 1)
                    {
                        me->RemoveLootMode(LOOT_MODE_HARD_MODE_1);
                        --ActiveTowersCount;
                    }
                }

                switch (action)
                {
                    case ACTION_TOWER_OF_STORM_DESTROYED:
                        towerOfStorms = false;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_STORMS);
                        break;
                    case ACTION_TOWER_OF_FROST_DESTROYED:
                        towerOfFrost = false;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FROST);
                        break;
                    case ACTION_TOWER_OF_FLAMES_DESTROYED:
                        towerOfFlames = false;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FLAMES);
                        break;
                    case ACTION_TOWER_OF_LIFE_DESTROYED:
                        towerOfLife = false;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                            orbitalSupport->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_LIFE);
                        break;
                    case ACTION_START_HARD_MODE:  // Activate hard-mode enable all towers, apply buffs on leviathan
                        towerOfStorms = true;
                        towerOfLife = true;
                        towerOfFlames = true;
                        towerOfFrost = true;
                        if (Creature* orbitalSupport = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ORBITAL_SUPPORT)))
                        {
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_STORMS, true);
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_FROST, true);
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_FLAMES, true);
                            orbitalSupport->CastSpell(orbitalSupport, SPELL_BUFF_TOWER_OF_LIFE, true);
                        }
                        me->SetLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2 | LOOT_MODE_HARD_MODE_3 | LOOT_MODE_HARD_MODE_4);
                        break;
                    case ACTION_MOVE_TO_CENTER_POSITION: // Triggered by 2 Collossus near door
                        if (!me->isDead())
                            me->GetMotionMaster()->MoveCharge(Center.GetPositionX(), Center.GetPositionY(), Center.GetPositionZ(), SPEED_CHARGE, POINT_PREPARE); // position center
                        break;
                    case ACTION_EVADE:
                        EnterEvadeMode();
                        break;
                    case ACTION_NEW_PURSUED:
                        events.ScheduleEvent(EVENT_PURSUE, 1000);
                        break;
                    case ACTION_INCREMENT_SHUTDOWN:
                        ++Shutdown;
                        break;
                    default:
                        break;
                }
            }

            private:
                // Copypasta from DoSpellAttackIfReady, only difference is the target - it cannot be selected trough getVictim this way -
                // I also removed the spellInfo check
                void DoBatteringRamIfReady()
                {
                    if (me->isAttackReady())
                    {
                        Unit* target = ObjectAccessor::GetUnit(*me, _pursueTarget);
                        if (me->IsWithinCombatRange(target, 30.0f))
                        {
                            DoCast(target, SPELL_BATTERING_RAM);
                            me->resetAttackTimer();
                        }
                    }
                }

                Unit* FindNearestAttackableTarget() const
                {
                    std::list<Unit*> targets;
                    HostileCheck check(me);
                    Trinity::UnitListSearcher<HostileCheck> searcher(me, targets, check);
                    me->VisitNearbyObject(240, searcher);
                    targets.remove_if(FlameLeviathanPursuedTargetSelector());
                    if (targets.empty())
                        return nullptr;
                    targets.sort(Trinity::ObjectDistanceOrderPred(me));
                    return targets.front();
                }

                ObjectGuid _pursueTarget;

                struct HostileCheck
                {
                    HostileCheck(Unit* me) : _me(me) { }

                    bool operator()(WorldObject* u)
                    {
                        if (Unit* unit = u->ToUnit())
                            return _me->IsValidAttackTarget(unit);
                        return false;
                    }

                    Unit* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_flame_leviathanAI>(creature);
        }
};

class boss_flame_leviathan_seat : public CreatureScript
{
    public:
        boss_flame_leviathan_seat() : CreatureScript("boss_flame_leviathan_seat") { }

        struct boss_flame_leviathan_seatAI : public ScriptedAI
        {
            boss_flame_leviathan_seatAI(Creature* creature) : ScriptedAI(creature), vehicle(creature->GetVehicleKit())
            {
                ASSERT(vehicle);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayFromModel(1);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            Vehicle* vehicle;

            void Reset() override
            {
                me->SetVisible(true);
            }

            void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
            {
                if (!me->GetVehicle())
                    return;

                if (seatId == SEAT_PLAYER)
                {
                    if (!apply)
                        return;

                    if (Creature* leviathan = me->GetVehicleCreatureBase())
                        leviathan->AI()->Talk(SAY_PLAYER_RIDING);

                    if (Unit* passenger = me->GetVehicleKit()->GetPassenger(SEAT_TURRET))
                        if (Creature* turret = passenger->ToCreature())
                            turret->AI()->AttackStart(who);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                else if (seatId == SEAT_TURRET)
                {
                    if (apply)
                        return;

                    if (Unit* device = vehicle->GetPassenger(SEAT_DEVICE))
                        device->CastSpell(device, SPELL_OVERLOAD_CIRCUIT, true);

                    if (Creature* turret = who->ToCreature())
                        turret->DespawnOrUnsummon(1);
                }
                else if (seatId == SEAT_DEVICE)
                {
                    if (!apply)
                        if (Creature* device = who->ToCreature())
                            device->DespawnOrUnsummon(1);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_flame_leviathan_seatAI>(creature);
        }
};

class boss_flame_leviathan_defense_cannon : public CreatureScript
{
    public:
        boss_flame_leviathan_defense_cannon() : CreatureScript("boss_flame_leviathan_defense_cannon") { }

        struct boss_flame_leviathan_defense_cannonAI : public ScriptedAI
        {
            boss_flame_leviathan_defense_cannonAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 NapalmTimer;

            void Reset() override
            {
                NapalmTimer = 5*IN_MILLISECONDS;
                DoCast(me, SPELL_INVIS_AND_STEALTH_DETECT);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_OVERLOAD_CIRCUIT)
                    if (Creature* leviathan = me->GetVehicleCreatureBase())
                        leviathan->AI()->DoAction(ACTION_INCREMENT_SHUTDOWN);
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (NapalmTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        if (CanAIAttack(target))
                            DoCast(target, SPELL_NAPALM, true);

                    NapalmTimer = 5000;
                }
                else
                    NapalmTimer -= diff;
            }

            bool CanAIAttack(Unit const* who) const override
            {
                if (who->GetTypeId() != TYPEID_PLAYER || !who->GetVehicle() || who->GetVehicleBase()->GetEntry() == NPC_LEVIATHAN_SEATS)
                    return false;
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_flame_leviathan_defense_cannonAI>(creature);
        }
};

class boss_flame_leviathan_defense_turret : public CreatureScript
{
    public:
        boss_flame_leviathan_defense_turret() : CreatureScript("boss_flame_leviathan_defense_turret") { }

        struct boss_flame_leviathan_defense_turretAI : public TurretAI
        {
            boss_flame_leviathan_defense_turretAI(Creature* creature) : TurretAI(creature) { }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                if (!CanAIAttack(who))
                    damage = 0;
            }

            bool CanAIAttack(Unit const* who) const override
            {
                if (who->GetTypeId() != TYPEID_PLAYER || !who->GetVehicle() || who->GetVehicleBase()->GetEntry() != NPC_LEVIATHAN_SEATS)
                    return false;
                return true;
            }

            void JustDied(Unit* killer) override
            {
                me->ExitVehicle();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!me->SelectVictim())
                    EnterEvadeMode();
                else
                    DoSpellAttackIfReady(me->m_spells[0]);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_flame_leviathan_defense_turretAI>(creature);
        }
};

class boss_flame_leviathan_overload_device : public CreatureScript
{
    public:
        boss_flame_leviathan_overload_device() : CreatureScript("boss_flame_leviathan_overload_device") { }

        struct boss_flame_leviathan_overload_deviceAI : public PassiveAI
        {
            boss_flame_leviathan_overload_deviceAI(Creature* creature) : PassiveAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_flame_leviathan_overload_deviceAI>(creature);
        }
};

class boss_flame_leviathan_safety_container : public CreatureScript
{
    public:
        boss_flame_leviathan_safety_container() : CreatureScript("boss_flame_leviathan_safety_container") { }

        struct boss_flame_leviathan_safety_containerAI : public PassiveAI
        {
            boss_flame_leviathan_safety_containerAI(Creature* creature) : PassiveAI(creature) { }

            void JustDied(Unit* /*killer*/) override
            {
                //float x, y, z;
                //me->GetPosition(x, y, z);
                //z = me->GetMap()->GetHeight(/*me->GetPhaseMask(),*/ x, y, z);
                //me->GetMotionMaster()->MovePoint(0, x, y, z);
                //me->SetPosition(x, y, z, 0);
                //me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_flame_leviathan_safety_containerAI>(creature);
        }
};

class npc_mechanolift : public CreatureScript
{
    public:
        npc_mechanolift() : CreatureScript("npc_mechanolift") { }

        struct npc_mechanoliftAI : public PassiveAI
        {
            npc_mechanoliftAI(Creature* creature) : PassiveAI(creature)
            {
                ASSERT(me->GetVehicleKit());
                if (me->IsAlive())
                    me->SetFullHealth();
            }

            uint32 MoveTimer;

            void Reset() override
            {
                MoveTimer = 0;
                me->GetMotionMaster()->MoveRandom(50);
                me->SetVisible(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMotionMaster()->MoveTargetedHome();
                DoCast(me, SPELL_DUSTY_EXPLOSION);
                Creature* liquid = DoSummon(NPC_LIQUID, me, 0);
                if (liquid)
                {
                    liquid->CastSpell(liquid, SPELL_LIQUID_PYRITE, true);
                    liquid->CastSpell(liquid, SPELL_DUST_CLOUD_IMPACT, true);
                    float x, y, z;
                    me->GetPosition(x, y, z);
                    z = me->GetMap()->GetHeight(me->GetPhaseMask(), x, y, z, false, MAX_HEIGHT);
                    liquid->Relocate(x, y, z);
                    liquid->CastSpell(liquid, SPELL_DUSTY_EXPLOSION, true);
                    liquid->SetDisplayId(DISPLAY_ID_DAMAGED_PYRITE_CONTAINER);
                    liquid->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    liquid->AI()->DoAction(5 * MINUTE * IN_MILLISECONDS);
                }
                me->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type == POINT_MOTION_TYPE && point == 1)
                    if (Creature* container = me->FindNearestCreature(NPC_CONTAINER, 5, true))
                        container->EnterVehicle(me);
            }

            void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) override
            {
                if (passenger->GetTypeId() == TYPEID_UNIT && passenger->GetEntry() == NPC_CONTAINER && !apply)
                    passenger->ToCreature()->DespawnOrUnsummon(1);
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
            }

            void UpdateAI(uint32 diff) override
            {
                PassiveAI::UpdateAI(diff);

                if (MoveTimer <= diff)
                {
                    if (me->GetVehicleKit() && me->GetVehicleKit()->HasEmptySeat(-1))
                    {
                        Creature* container = me->FindNearestCreature(NPC_CONTAINER, 50, true);
                        if (container && !container->GetVehicle())
                            me->GetMotionMaster()->MovePoint(1, container->GetPositionX(), container->GetPositionY(), container->GetPositionZ());
                    }

                    MoveTimer = 30000; //check next 30 seconds
                }
                else
                    MoveTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mechanoliftAI>(creature);
        }
};

class npc_liquid_pyrite : public CreatureScript
{
    public:
        npc_liquid_pyrite() : CreatureScript("npc_liquid_pyrite") { }

        struct npc_liquid_pyriteAI : public PassiveAI
        {
            npc_liquid_pyriteAI(Creature* creature) : PassiveAI(creature) { }

            void Reset() override
            {
                DoCast(me, SPELL_LIQUID_PYRITE, true);
                me->SetDisplayId(DISPLAY_ID_DAMAGED_PYRITE_CONTAINER);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                damage = 0;
            }

            void DoAction(int32 action) override
            {
                despawnTimer = action;
            }

            void UpdateAI(uint32 diff) override
            {
                if (despawnTimer)
                {
                    if (despawnTimer <= diff)
                    {
                        despawnTimer = 0;
                        me->DespawnOrUnsummon();
                    }
                    else
                        despawnTimer -= diff;
                }
            }

        private:
            uint32 despawnTimer = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_liquid_pyriteAI>(creature);
        }
};

class npc_pool_of_tar : public CreatureScript
{
    public:
        npc_pool_of_tar() : CreatureScript("npc_pool_of_tar") { }

        struct npc_pool_of_tarAI : public ScriptedAI
        {
            npc_pool_of_tarAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, SPELL_TAR_PASSIVE, true);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                damage = 0;
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if ((spell->Id == SPELL_FLAMES_TAR || spell->Id == SPELL_FLAMES_VEHICLES) && !me->HasAura(SPELL_BLAZE))
                    me->CastSpell(me, SPELL_BLAZE, true);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_pool_of_tarAI>(creature);
        }
};

class npc_colossus : public CreatureScript
{
    public:
        npc_colossus() : CreatureScript("npc_colossus") { }

        struct npc_colossusAI : public ScriptedAI
        {
            npc_colossusAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                instance = creature->GetInstanceScript();
                groundSlamTimer = 0;
                availablecheck = 0;
            }

            void Reset() override
            {
                groundSlamTimer = 0;
                availablecheck = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                groundSlamTimer = urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS);
                availablecheck = 5*IN_MILLISECONDS;
            }

            void JustDied(Unit* /*killer*/) override
            {
                // Check is required, since there are 5 colossus in the instance :: Check if the distance is enough
                // Given distance ends up with ~106 x/y distance.
                if (me->GetHomePosition().IsInDist(&Center, 50.f))
                    if (instance->GetData(DATA_COLOSSUS) != 2)
                        instance->SetData(DATA_COLOSSUS, instance->GetData(DATA_COLOSSUS)+1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (groundSlamTimer <= diff)
                {
                    DoCastVictim(SPELL_GROUND_SLAM);
                    groundSlamTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
                }
                else
                    groundSlamTimer -= diff;

                if (availablecheck <= diff)
                {
                    if (!me->FindNearestPlayer(100.0f) && (!me->FindNearestCreature(NPC_SALVAGED_SIEGE_ENGINE, 100.0f, true) || !me->FindNearestCreature(NPC_CHOPPER, 100.0f, true) || !me->FindNearestCreature(NPC_SALVAGED_DEMOLISHER, 100.0f, true)))
                    {
                        _EnterEvadeMode();
                        me->GetMotionMaster()->MoveTargetedHome();
                    }
                    availablecheck = 5*IN_MILLISECONDS;
                }
                else
                    groundSlamTimer -= diff;

                DoMeleeAttackIfReady();
            }

            private:
                InstanceScript* instance;
                uint32 groundSlamTimer;
                uint32 availablecheck;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_colossusAI>(creature);
        }
};

struct targetting_reticle_ai : public ScriptedAI
{
    targetting_reticle_ai(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFloatValue(UNIT_FIELD_HOVER_HEIGHT, RETICLE_HOVER_HEIGHT); // Probably wrong
    }

    void Reset() override
    {
        me->SetVisible(true);
        me->SetHover(true); // Probably wrong
    }

    void JustSummoned(Creature* summon) override
    {
        if (InstanceScript* instance = me->GetInstanceScript())
            if (Creature* leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_LEVIATHAN)))
                leviathan->AI()->JustSummoned(summon);
    }
};

class npc_thorims_hammer : public CreatureScript
{
    public:
        npc_thorims_hammer() : CreatureScript("npc_thorims_hammer") { }

        struct npc_thorims_hammerAI : public targetting_reticle_ai
        {
            npc_thorims_hammerAI(Creature* creature) : targetting_reticle_ai(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                me->m_Events.Schedule(1000, [this]()
                {
                    if (Unit* passenger = me->GetVehicleKit()->GetPassenger(SEAT_CASTER))
                        passenger->CastSpell(passenger, SPELL_THORIM_S_HAMMER, true);
                });
                me->m_Events.Schedule(6000, [this](){ me->DespawnOrUnsummon(); });

            }

            void UpdateAI(uint32 diff) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_thorims_hammerAI>(creature);
        }
};

class npc_mimirons_inferno : public CreatureScript
{
    public:
        npc_mimirons_inferno() : CreatureScript("npc_mimirons_inferno") { }

        struct npc_mimirons_infernoAI : public targetting_reticle_ai
        {
            npc_mimirons_infernoAI(Creature* creature) : targetting_reticle_ai(creature), instance(creature->GetInstanceScript()) { }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetOrientation(frand(-135.0f, -105.0f) / 180.0f * M_PI);
                Position pos;
                pos.Relocate(me);
                pos.RelocateOffset(0.0f, 500.0f);
                pos.m_positionZ = InfernoStart.GetPositionZ();
                me->GetMotionMaster()->MovePoint(1, pos);
                DoCastAOE(SPELL_TARGET_SEARCH_A, true);
                lastSide = Side::North;
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(BOSS_LEVIATHAN) != IN_PROGRESS || me->GetPositionX() < 130.0f || me->GetPositionX() > 408.0f || me->GetPositionY() < -168.0f || me->GetPositionY() > 107.0f)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (lastSide != Side::North && me->GetPositionX() >=  382.0f ||
                    lastSide != Side::East  && me->GetPositionY() <= -137.0f ||
                    lastSide != Side::South && me->GetPositionX() <=  158.0f ||
                    lastSide != Side::West  && me->GetPositionY() >=   72.0f)
                {
                    me->SetOrientation(me->GetOrientation() + frand(-95.0f, -85.0f) / 180.0f * M_PI);
                    Position pos;
                    pos.Relocate(me);
                    pos.RelocateOffset(0.0f, 500.0f);
                    pos.m_positionZ = InfernoStart.GetPositionZ();
                    me->GetMotionMaster()->MovePoint(1, pos);
                    lastSide = (Side)(((uint8)lastSide + 1) % 4);
                    switch (lastSide)
                    {
                        case Side::North: DoCastAOE(SPELL_TARGET_SEARCH_A, true); break;
                        case Side::East:  DoCastAOE(SPELL_TARGET_SEARCH_B, true); break;
                        case Side::South: DoCastAOE(SPELL_TARGET_SEARCH_C, true); break;
                        case Side::West:  DoCastAOE(SPELL_TARGET_SEARCH_D, true); break;
                        default: break;
                    }
                }

                if (infernoTimer <= diff)
                {
                    if (Unit* passenger = me->GetVehicleKit()->GetPassenger(SEAT_CASTER))
                        passenger->CastSpell(passenger, SPELL_MIMIRON_S_INFERNO, true);
                    infernoTimer = 2500;
                }
                else
                    infernoTimer -= diff;
            }

        private:
            enum class Side : uint8 { North, East, South, West };

            uint32 infernoTimer = 2500;
            Side lastSide = Side::North;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mimirons_infernoAI>(creature);
        }
};

class npc_hodirs_fury : public CreatureScript
{
    public:
        npc_hodirs_fury() : CreatureScript("npc_hodirs_fury") { }

        struct npc_hodirs_furyAI : public targetting_reticle_ai
        {
            npc_hodirs_furyAI(Creature* creature) : targetting_reticle_ai(creature) { }

            void UpdateAI(uint32 diff) override
            {
                if (me->GetVictim())
                {
                    if (me->IsWithinDist(me->GetVictim(), 2.5f, false))
                    {
                        if (Unit* passenger = me->GetVehicleKit()->GetPassenger(SEAT_CASTER))
                            passenger->CastSpell(passenger, SPELL_HODIR_S_FURY, true);

                        nextTargetTimer = 5000;
                        me->AttackStop();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                    }
                }
                else if (!nextTargetTimer)
                    nextTargetTimer = 1;

                if (nextTargetTimer)
                {
                    if (nextTargetTimer <= diff)
                    {
                        DoZoneInCombat(me, 400.0f);
                        if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 0.0f, false, -SPELL_HODIRS_FURY_AURA))
                        {
                            AttackStart(target);
                            nextTargetTimer = 0;
                        }
                        else
                        {
                            me->AttackStop();
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveIdle();
                            nextTargetTimer = 5000;
                        }
                    }
                    else
                        nextTargetTimer -= diff;
                }
            }

        private:
            uint32 nextTargetTimer = 10000;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_hodirs_furyAI>(creature);
        }
};

class npc_freyas_ward : public CreatureScript
{
    public:
        npc_freyas_ward() : CreatureScript("npc_freyas_ward") { }

        struct npc_freyas_wardAI : public targetting_reticle_ai
        {
            npc_freyas_wardAI(Creature* creature) : targetting_reticle_ai(creature) { }

            void UpdateAI(uint32 diff) override
            {
                if (summonTimer <= diff)
                {
                    if (Unit* passenger = me->GetVehicleKit()->GetPassenger(SEAT_CASTER))
                        passenger->CastSpell(passenger, SPELL_FREYA_S_WARD, true);
                    summonTimer = 20000;
                }
                else
                    summonTimer -= diff;
            }

        private:
            uint32 summonTimer = 5000;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_freyas_wardAI>(creature);
        }
};

class npc_freya_ward_summon : public CreatureScript
{
    public:
        npc_freya_ward_summon() : CreatureScript("npc_freya_ward_summon") { }

        struct npc_freya_ward_summonAI : public ScriptedAI
        {
            npc_freya_ward_summonAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 lashTimer;

            void Reset() override
            {
                lashTimer = 5000;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoZoneInCombat(me, 250.0f);

                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_LEVIATHAN)))
                        leviathan->AI()->JustSummoned(me);
            }

            void JustDied(Unit* killer) override
            {
                me->DespawnOrUnsummon(5000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (lashTimer <= diff)
                {
                    DoCastVictim(SPELL_LASH);
                    lashTimer = 20000;
                }
                else
                    lashTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_freya_ward_summonAI>(creature);
        }
};

class npc_vehicle_ulduar : public CreatureScript
{
    public:
        npc_vehicle_ulduar() : CreatureScript("npc_vehicle_ulduar") { }

        struct npc_vehicle_ulduarAI : public VehicleAI
        {
            npc_vehicle_ulduarAI(Creature* creature) : VehicleAI(creature)
            {
                pInstance = creature->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset() override
            {
                if (pInstance->GetData(DATA_SHIELD_DISABLED))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                me->SetVisible(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!me->IsAlive() && pInstance->GetBossState(BOSS_LEVIATHAN) != DONE)
                {
                    me->DespawnOrUnsummon();
                    me->SetRespawnTime(5);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasAura(SPELL_PURSUED) && pInstance->GetBossState(BOSS_LEVIATHAN) != IN_PROGRESS)
                    me->RemoveAura(SPELL_PURSUED);

                if (pInstance->GetBossState(BOSS_LEVIATHAN) == IN_PROGRESS)
                    if (Vehicle* seat = me->GetVehicleKit())
                        if (!seat->GetPassenger(0) && me->HasAura(SPELL_PURSUED))
                            me->RemoveAura(SPELL_PURSUED);
            }

            void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
            {
                if (me->GetEntry() == NPC_SALVAGED_DEMOLISHER && seatId == 3)
                {
                    if (apply)
                        me->CastSpell(me, SPELL_PASSENGER_LOADED, true);
                    else
                        me->RemoveAurasDueToSpell(SPELL_PASSENGER_LOADED);
                }
            }

        private:
            InstanceScript* pInstance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_vehicle_ulduarAI>(creature);
        }
};

class npc_accessory_of_vehicle_ulduar : public CreatureScript
{
    public:
        npc_accessory_of_vehicle_ulduar() : CreatureScript("npc_accessory_of_vehicle_ulduar") { }

        struct npc_accessory_of_vehicle_ulduarAI : public VehicleAI
        {
            npc_accessory_of_vehicle_ulduarAI(Creature* creature) : VehicleAI(creature)
            {
                pInstance = creature->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset() override
            {
                if (pInstance->GetData(DATA_SHIELD_DISABLED))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                me->SetVisible(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!me->IsAlive())
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasAura(SPELL_PURSUED))
                    me->RemoveAura(SPELL_PURSUED);

                if (!me->GetVehicle())
                    me->DespawnOrUnsummon();

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_PYRITE_DESPAWN && me->GetVehicleKit() && me->GetVehicleKit()->GetPassenger(1))
                        if (Creature* pyrite =  me->GetVehicleKit()->GetPassenger(1)->ToCreature())
                        {
                            pyrite->ExitVehicle();
                            pyrite->DisappearAndDie();
                        }
            }

            void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
            {
                if (!me->GetVehicle())
                    return;

                if (apply && seatId == 1 && me->GetEntry() == NPC_SALVAGED_DEMOLISHER_MECHANIC_SEAT && who->GetEntry() == NPC_LIQUID)
                {
                    who->GetAI()->DoAction(0);
                    events.ScheduleEvent(EVENT_PYRITE_DESPAWN, 5000);
                }
            }

        private:
            InstanceScript* pInstance;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_accessory_of_vehicle_ulduarAI>(creature);
        }
};

class go_ulduar_tower : public GameObjectScript
{
    public:
        go_ulduar_tower() : GameObjectScript("go_ulduar_tower") { }

        void OnDestroyed(GameObject* go, Player* /*player*/)
        {
            InstanceScript* instance = go->GetInstanceScript();
            if (!instance)
                return;

            switch (go->GetEntry())
            {
                case GO_TOWER_OF_STORMS:
                    CreatureDies(go);
                    if (GameObject* HightTower = go->FindNearestGameObject(194666, 150.0f))
                        HightTower->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* Crystal = ObjectAccessor::GetGameObject(*go, instance->GetGuidData(DATA_TOWER_OF_STORMS_CRYSTAL)))
                        Crystal->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_TOWER_OF_FLAMES:
                    CreatureDies(go);
                    if (GameObject* HightTower = go->FindNearestGameObject(194664, 150.0f))
                        HightTower->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* Crystal = ObjectAccessor::GetGameObject(*go, instance->GetGuidData(DATA_TOWER_OF_FLAMES_CRYSTAL)))
                        Crystal->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_TOWER_OF_FROST:
                    CreatureDies(go);
                    if (GameObject* HightTower = go->FindNearestGameObject(194665, 150.0f))
                        HightTower->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* Crystal = ObjectAccessor::GetGameObject(*go, instance->GetGuidData(DATA_TOWER_OF_FROST_CRYSTAL)))
                        Crystal->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_TOWER_OF_LIFE:
                    CreatureDies(go);
                    if (GameObject* HightTower = go->FindNearestGameObject(194663, 150.0f))
                        HightTower->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* Crystal = ObjectAccessor::GetGameObject(*go, instance->GetGuidData(DATA_TOWER_OF_LIFE_CRYSTAL)))
                        Crystal->SetGoState(GO_STATE_ACTIVE);
                    break;
            }

            if (go->GetEntry() != GO_TOWER_OF_STORMS || go->GetEntry() != GO_TOWER_OF_FLAMES || go->GetEntry() != GO_TOWER_OF_FROST || go->GetEntry() != GO_TOWER_OF_LIFE)
                if (Creature* trigger = go->FindNearestCreature(NPC_ULDUAR_GAUNTLET_GENERATOR, 15.0f, true))
                    trigger->DisappearAndDie();
        }

        void CreatureDies(GameObject* go)
        {
            std::list<Creature*> trashmobs;
            GetCreatureListWithEntryInGrid(trashmobs, go, NPC_ULDUAR_GAUNTLET_GENERATOR, 25);
            GetCreatureListWithEntryInGrid(trashmobs, go, NPC_IRONWORK_CANNON, 25);
            GetCreatureListWithEntryInGrid(trashmobs, go, 34159, 25);
            for (auto&& trash : trashmobs)
                if (trash->IsAlive())
                    trash->DisappearAndDie();
        }
};

class achievement_three_car_garage_demolisher : public AchievementCriteriaScript
{
    public:
        achievement_three_car_garage_demolisher() : AchievementCriteriaScript("achievement_three_car_garage_demolisher") { }

        bool OnCheck(Player* source, Unit* /*target*/)
        {
            if (Creature* vehicle = source->GetVehicleCreatureBase())
            {
                if (vehicle->GetEntry() == NPC_SALVAGED_DEMOLISHER)
                    return true;
            }

            return false;
        }
};

class achievement_three_car_garage_chopper : public AchievementCriteriaScript
{
    public:
        achievement_three_car_garage_chopper() : AchievementCriteriaScript("achievement_three_car_garage_chopper") { }

        bool OnCheck(Player* source, Unit* /*target*/)
        {
            if (Creature* vehicle = source->GetVehicleCreatureBase())
            {
                if (vehicle->GetEntry() == NPC_CHOPPER)
                    return true;
            }

            return false;
        }
};

class achievement_three_car_garage_siege : public AchievementCriteriaScript
{
    public:
        achievement_three_car_garage_siege() : AchievementCriteriaScript("achievement_three_car_garage_siege") { }

        bool OnCheck(Player* source, Unit* /*target*/)
        {
            if (Creature* vehicle = source->GetVehicleCreatureBase())
            {
                if (vehicle->GetEntry() == NPC_SALVAGED_SIEGE_ENGINE)
                    return true;
            }

            return false;
        }
};

class spell_auto_repair : public SpellScriptLoader
{
    public:
        spell_auto_repair() : SpellScriptLoader("spell_auto_repair") { }

        class spell_auto_repair_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_auto_repair_SpellScript);

            void CheckCooldownForTarget()
            {
                if (GetHitUnit()->HasAuraEffect(SPELL_AUTO_REPAIR, EFFECT_2))   // Check presence of dummy aura indicating cooldown
                {
                    PreventHitEffect(EFFECT_0);
                    PreventHitDefaultEffect(EFFECT_1);
                    PreventHitDefaultEffect(EFFECT_2);
                    // Currently this doesn't work: if we call PreventHitAura(), the existing aura will be removed
                    // because of recent aura refreshing changes. Since removing the existing aura negates the idea
                    // of a cooldown marker, we just let the dummy aura refresh itself without executing the other spelleffects.
                    // The spelleffects can be executed by letting the dummy aura expire naturally.
                    // This is a temporary solution only.
                    //PreventHitAura();
                }
            }

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Vehicle* vehicle = GetHitUnit()->GetVehicleKit();
                if (!vehicle)
                    return;

                Player* driver = vehicle->GetPassenger(0) ? vehicle->GetPassenger(0)->ToPlayer() : nullptr;
                if (!driver)
                    return;

                InstanceScript* instance = driver->GetInstanceScript();
                if (!instance)
                    return;

                if ((instance->GetBossState(BOSS_LEVIATHAN) == IN_PROGRESS) || driver->HasAura(62705))
                    return;

                // Actually should/could use basepoints (100) for this spell effect as percentage of health, but oh well.
                vehicle->GetBase()->SetFullHealth();

                // For achievement
                instance->SetData(DATA_UNBROKEN, 0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_auto_repair_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                BeforeHit += SpellHitFn(spell_auto_repair_SpellScript::CheckCooldownForTarget);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_auto_repair_SpellScript();
        }
};

class spell_systems_shutdown : public SpellScriptLoader
{
    public:
        spell_systems_shutdown() : SpellScriptLoader("spell_systems_shutdown") { }

        class spell_systems_shutdown_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_systems_shutdown_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                // This could probably in the SPELL_EFFECT_SEND_EVENT handler too:
                owner->AddUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                owner->RemoveAurasDueToSpell(SPELL_GATHERING_SPEED);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                owner->AI()->Talk(EMOTE_REACTIVATE);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_systems_shutdown_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_systems_shutdown_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_systems_shutdown_AuraScript();
        }
};

class spell_pursue : public SpellScriptLoader
{
    public:
        spell_pursue() : SpellScriptLoader("spell_pursue") { }

        class spell_pursue_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pursue_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PURSUED))
                    return false;
                return true;
            }

            bool Load() override
            {
                _target = nullptr;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                // Step 1: Remove all theoretical targets that cannot be our target
                targets.remove_if(FlameLeviathanPursuedTargetSelector());
                if (targets.empty())
                {
                    if (Creature* caster = GetCaster()->ToCreature())
                        caster->AI()->DoAction(ACTION_EVADE);
                    return;
                }

                // Step 2: Try to find Demolisher or Siege Engine
                std::list<WorldObject*> tempList;
                // try to find demolisher or siege engine first (not the current target)
                for (auto&& _target : targets)
                {
                    if (Unit* unit = _target->ToUnit())
                        if (!unit->ToCreature() || unit->HasAura(SPELL_PURSUED))
                        continue;

                    if (_target->ToCreature()->GetEntry() == NPC_SALVAGED_SIEGE_ENGINE || _target->ToCreature()->GetEntry() == NPC_SALVAGED_DEMOLISHER)
                        tempList.push_back(_target);
                }

                if (tempList.empty())
                {
                    // no demolisher or siege engine, find a chopper (not the current target)
                    for (auto&& _target : targets)
                    {
                        if (Unit* unit = _target->ToUnit())
                            if (!unit->ToCreature() || unit->HasAura(SPELL_PURSUED))
                            continue;

                        if (_target->ToCreature()->GetEntry() == NPC_CHOPPER)
                            tempList.push_back(_target);
                    }
                }

                _target = Trinity::Containers::SelectRandomContainerElement(!tempList.empty() ? tempList : targets); // No vehicles found, select a random target
                SetSelectedTarget(targets);
            }

            void SetSelectedTarget(std::list<WorldObject*>& targets)
            {
                targets.clear();
                if (_target)
                    targets.push_back(_target);
            }

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Creature* caster = GetCaster()->ToCreature();
                Unit* target = GetHitUnit();
                if (!caster || !target)
                    return;

                caster->AI()->AttackStart(target);    // Chase target

                if (target->GetTypeId() == TYPEID_PLAYER)
                    caster->AI()->Talk(EMOTE_PURSUE, target);
                else if (Vehicle* vehicle = target->GetVehicleKit())
                {
                    for (auto&& seat : vehicle->Seats)
                    {
                        if (seat.second.Passenger.Guid && seat.second.Passenger.Guid.IsPlayer())
                        {
                            if (Player* passenger = ObjectAccessor::GetPlayer(*caster, seat.second.Passenger.Guid))
                            {
                                caster->AI()->Talk(EMOTE_PURSUE, passenger);
                                break;
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pursue_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pursue_SpellScript::SetSelectedTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_pursue_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }

            WorldObject* _target;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pursue_SpellScript();
        }
};

class spell_vehicle_throw_passenger : public SpellScriptLoader
{
    public:
        spell_vehicle_throw_passenger() : SpellScriptLoader("spell_vehicle_throw_passenger") { }

        class spell_vehicle_throw_passenger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vehicle_throw_passenger_SpellScript);

            void HandleAfterCast()
            {
                SpellCastTargets targets = GetSpell()->m_targets;
                if (targets.HasTraj())
                    if (Vehicle* vehicle = GetCaster()->GetVehicleKit())
                        if (Unit* passenger = vehicle->GetPassenger(3))
                        {
                            passenger->ExitVehicle();
                            float x, y, z;
                            targets.GetDstPos()->GetPosition(x, y, z);
                            passenger->StopMoving();
                            passenger->GetMotionMaster()->Clear();
                            passenger->GetMotionMaster()->MoveJump(x, y, z, targets.GetSpeedXY(), targets.GetSpeedZ());
                            passenger->CastSpell(passenger, SPELL_HOOKSHOT_AURA, true);
                        }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_vehicle_throw_passenger_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vehicle_throw_passenger_SpellScript();
        }
};

class spell_hookshot_aura : public SpellScriptLoader
{
    public:
        spell_hookshot_aura() : SpellScriptLoader("spell_hookshot_aura") { }

        class spell_hookshot_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hookshot_aura_AuraScript);

            void DummyTick(AuraEffect const* aurEff)
            {
                if (Unit* target = GetTarget())
                {
                    std::list<Creature*> seats;
                    GetCreatureListWithEntryInGrid(seats, target, NPC_LEVIATHAN_SEATS, 30.0f);
                    seats.sort(Trinity::ObjectDistanceOrderPred(target));
                    for (auto&& seat : seats)
                    {
                        if (Creature* leviathan = seat->GetVehicleCreatureBase())
                            if (!leviathan->IsAlive() || !leviathan->IsInCombat())
                                return;

                        if (Vehicle* vehicle = seat->GetVehicleKit())
                        {
                            if (vehicle->GetPassenger(SEAT_TURRET) && !vehicle->GetPassenger(SEAT_PLAYER))
                            {
                                Remove(AURA_REMOVE_BY_DEFAULT);
                                target->CastSpell(target, aurEff->GetAmount(), true);
                                break;
                            }
                        }
                    }

                    if (!target->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
                        Remove(AURA_REMOVE_BY_DEFAULT);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hookshot_aura_AuraScript::DummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hookshot_aura_AuraScript();
        }
};

class spell_grab_crate : public SpellScriptLoader
{
    public:
        spell_grab_crate() : SpellScriptLoader("spell_grab_crate") { }

        class spell_grab_crate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grab_crate_SpellScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(62479))
                    return false;
                return true;
            }

            SpellCastResult CheckRequirement()
            {
                Unit* caster = GetCaster();

                if (!caster || !caster->GetVehicleKit() || caster->GetVehicleKit()->GetPassenger(1))
                    return SPELL_FAILED_NOT_READY;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_grab_crate_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grab_crate_SpellScript();
        }
};

class spell_leviathan_tower_buff : public SpellScriptLoader
{
    public:
        spell_leviathan_tower_buff() : SpellScriptLoader("spell_leviathan_tower_buff") { }

        class spell_leviathan_tower_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_leviathan_tower_buff_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                return target->GetTypeId() == TYPEID_UNIT && (target->GetEntry() == NPC_LEVIATHAN || target->GetEntry() == NPC_ORBITAL_SUPPORT);
            }

            void Register() override
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_leviathan_tower_buff_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_leviathan_tower_buff_AuraScript();
        }
};

class spell_freyas_ward : public SpellScriptLoader
{
    public:
        spell_freyas_ward() : SpellScriptLoader("spell_freyas_ward") { }

        class spell_freyas_ward_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_freyas_ward_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    if (WorldLocation const* dest = GetExplTargetDest())
                        for (uint8 i = 0; i < 5; ++i)
                            caster->CastSpell(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ(), GetEffectValue(), true);
            }

            void HandleSummon(SpellEffIndex effIndex)
            {
                if (!roll_chance_i(25))
                    PreventHitDefaultEffect(effIndex);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_freyas_ward_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnEffectHit += SpellEffectFn(spell_freyas_ward_SpellScript::HandleSummon, EFFECT_2, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_freyas_ward_SpellScript();
        };
};

class spell_thorims_hammer : public SpellScriptLoader
{
    public:
        spell_thorims_hammer() : SpellScriptLoader("spell_thorims_hammer") { }

        class spell_thorims_hammer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thorims_hammer_SpellScript);

            void HandleDamage(SpellEffIndex effIndex)
            {
                if (Creature* target = GetHitCreature())
                    SetHitDamage(CalculatePct(target->GetMaxHealth(), 0.5f)); // 0.5%, yes
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_thorims_hammer_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_thorims_hammer_SpellScript();
        };
};

class spell_flames : public SpellScriptLoader
{
    public:
        spell_flames() : SpellScriptLoader("spell_flames") { }

        class spell_flames_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_flames_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                if (Unit* target = GetHitUnit())
                    if (target->HasAura(GetEffectValue()))
                        target->RemoveAuraFromStack(GetEffectValue());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_flames_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_flames_SpellScript();
        };
};

class cond_hookshot : public ConditionScript
{
    public:
        cond_hookshot() : ConditionScript("cond_hookshot") { }

        bool OnConditionCheck(const Condition* condition, ConditionSourceInfo& sourceInfo) override
        {
            if (Unit* seat = sourceInfo.mConditionTargets[0]->ToUnit())
                if (Vehicle* vehicle = seat->GetVehicleKit())
                    if (vehicle->GetPassenger(SEAT_TURRET) && !vehicle->GetPassenger(SEAT_PLAYER))
                        return true;

            return false;
        }
};

void AddSC_boss_flame_leviathan()
{
    new boss_flame_leviathan();
    new boss_flame_leviathan_seat();
    new boss_flame_leviathan_defense_turret();
    new boss_flame_leviathan_defense_cannon();
    new boss_flame_leviathan_overload_device();
    new boss_flame_leviathan_safety_container();
    new npc_mechanolift();
    new npc_liquid_pyrite();
    new npc_pool_of_tar();
    new npc_colossus();
    new npc_thorims_hammer();
    new npc_mimirons_inferno();
    new npc_hodirs_fury();
    new npc_freyas_ward();
    new npc_freya_ward_summon();
    new npc_vehicle_ulduar();
    new npc_accessory_of_vehicle_ulduar();
    new go_ulduar_tower();
    new achievement_three_car_garage_demolisher();
    new achievement_three_car_garage_chopper();
    new achievement_three_car_garage_siege();
    new spell_auto_repair();
    new spell_systems_shutdown();
    new spell_pursue();
    new spell_vehicle_throw_passenger();
    new spell_hookshot_aura();
    new spell_grab_crate();
    new spell_leviathan_tower_buff();
    new spell_freyas_ward();
    new spell_thorims_hammer();
    new spell_flames();
    new cond_hookshot();
}
