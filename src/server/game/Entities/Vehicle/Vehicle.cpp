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

#include "Log.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Unit.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "CreatureAI.h"
#include "ZoneScript.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "MoveSplineInit.h"
#include "TemporarySummon.h"
#include "EventProcessor.h"
#include "Player.h"
#include "Battleground.h"

Vehicle::Vehicle(Unit* unit, VehicleEntry const* vehInfo, uint32 creatureEntry) :
UsableSeatNum(0), _me(unit), _vehicleInfo(vehInfo), _creatureEntry(creatureEntry), _status(STATUS_NONE)
{
    for (uint32 i = 0; i < MAX_VEHICLE_SEATS; ++i)
    {
        if (uint32 seatId = _vehicleInfo->m_seatID[i])
            if (VehicleSeatEntry const* veSeat = sVehicleSeatStore.LookupEntry(seatId))
            {
                Seats.insert(std::make_pair(i, VehicleSeat(veSeat)));
                if (veSeat->CanEnterOrExit())
                    ++UsableSeatNum;
            }
    }

    // Set or remove correct flags based on available seats. Will overwrite db data (if wrong).
    if (UsableSeatNum)
        _me->SetFlag(UNIT_FIELD_NPC_FLAGS, (_me->GetTypeId() == TYPEID_PLAYER ? UNIT_NPC_FLAG_PLAYER_VEHICLE : UNIT_NPC_FLAG_SPELLCLICK));
    else
        _me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, (_me->GetTypeId() == TYPEID_PLAYER ? UNIT_NPC_FLAG_PLAYER_VEHICLE : UNIT_NPC_FLAG_SPELLCLICK));

    // HACKY WAY, We must found a more generic way to handle this
    // Set inmunities since db ones are rewritten with player's ones
    switch (GetVehicleInfo()->m_ID)
    {
        // Strand of the Ancients
        case 160: // Antipersonnel Cannon
                  // Wintergrasp
        case 244: // Wintergrasp Tower Cannon
                  // Isle of Conquest
        case 510: // Keep Cannon
        case 452: // Alliance Gunship Cannon
        case 453: // Horde Gunship Cannon
            _me->SetControlled(true, UNIT_STATE_ROOT);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            // no break
        // Strand of the Ancients
        case 158: // Battleground Demolisher
        // Isle of Conquest
        case 438: // Catapult
        case 509: // Demolisher
        case 447: // Glaive Thrower, Siege Engine, Glaive Thrower
        case 436: // Siege Engine, Flame Turret, Flame Turret, Siege Turret
        // Ulduar
        case 336: // Salvaged Siege Engine
        case 338: // Salvaged Demolisher
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_KNOCKOUT, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHACKLE, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
            _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_MAX_HEALTH, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_ABSORB, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_INCREASE_SPEED, true);
            _me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true); // Death Grip (jump effect)
            _me->ApplySpellImmune(0, IMMUNITY_ID, 13810, true); // Frost Trap Aura
            _me->ApplySpellImmune(0, IMMUNITY_ID, 55741, true); // Desecration (Rank 1)
            _me->ApplySpellImmune(0, IMMUNITY_ID, 68766, true); // Desecration (Rank 2)
            _me->ApplySpellImmune(0, IMMUNITY_ID, 19752, true); // Divine Intervention
            _me->ApplySpellImmune(0, IMMUNITY_ID, 53601, true); // Sacred Shield
            _me->ApplySpellImmune(0, IMMUNITY_ID, 53551, true); // Sacred Cleansing
            break;
        // Icecrown Citadel
        case 554: // Horde Gunship Cannon / Alliance Gunship Cannon
            _me->SetControlled(true, UNIT_STATE_ROOT);
            break;
    }

    InitMovementInfoForBase();
}

Vehicle::~Vehicle()
{
    /// @Uninstall must be called before this.
    ASSERT(_status == STATUS_UNINSTALLING);
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
        ASSERT(itr->second.IsEmpty());
}

/**
 * @fn void Vehicle::Install()
 *
 * @brief Initializes power type for vehicle. Nothing more.
 *
 * @author Machiavelli
 * @date 17-2-2013
 */

void Vehicle::Install()
{
    if (Creature* creature = _me->ToCreature())
    {
        switch (_vehicleInfo->m_powerType)
        {
            case POWER_STEAM:
            case POWER_HEAT:
            case POWER_BLOOD:
            case POWER_OOZE:
            case POWER_WRATH:
                _me->SetPowerType(POWER_ENERGY);
                _me->SetMaxPower(POWER_ENERGY, 100);
                break;
            case POWER_PYRITE:
                _me->SetPowerType(POWER_ENERGY);
                _me->SetMaxPower(POWER_ENERGY, 50);
                break;
            default:
                for (uint32 i = 0; i < MAX_SPELL_VEHICLE; ++i)
                {
                    if (!creature->m_spells[i])
                        continue;

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(creature->m_spells[i]);
                    if (!spellInfo)
                        continue;

                    auto powerEntry = spellInfo->GetSpellPowerEntry(_me);
                    if (powerEntry && powerEntry->PowerType == POWER_ENERGY)
                    {
                        _me->SetPowerType(POWER_ENERGY);
                        _me->SetMaxPower(POWER_ENERGY, 100);
                        break;
                    }
                }
                break;
        }
    }

    _status = STATUS_INSTALLED;
}

void Vehicle::InstallAllAccessories(bool evading)
{
    if (GetBase()->GetTypeId() == TYPEID_PLAYER || !evading)
        RemoveAllPassengers();   // We might have aura's saved in the DB with now invalid casters - remove

    VehicleAccessoryList const* accessories = sObjectMgr->GetVehicleAccessoryList(this);
    if (!accessories)
        return;

    for (VehicleAccessoryList::const_iterator itr = accessories->begin(); itr != accessories->end(); ++itr)
        if (!evading || itr->IsMinion)  // only install minions on evade mode
            InstallAccessory(itr->AccessoryEntry, itr->SeatId, itr->IsMinion, itr->SummonedType, itr->SummonTime);
}

/**
 * @fn void Vehicle::Uninstall()
 *
 * @brief Removes all passengers and sets status to STATUS_UNINSTALLING.
 *        No new passengers can be added to the vehicle after this call.
 *
 * @author Machiavelli
 * @date 17-2-2013
 */

void Vehicle::Uninstall()
{
    /// @Prevent recursive uninstall call. (Bad script in OnUninstall/OnRemovePassenger/PassengerBoarded hook.)
    if (_status == STATUS_UNINSTALLING && !GetBase()->HasUnitTypeMask(UNIT_MASK_MINION))
    {
        TC_LOG_ERROR("entities.vehicle", "Vehicle GuidLow: %u, Entry: %u attempts to uninstall, but already has STATUS_UNINSTALLING! "
            "Check Uninstall/PassengerBoarded script hooks for errors.", _me->GetGUID().GetCounter(), _me->GetEntry());
        return;
    }

    _status = STATUS_UNINSTALLING;
    TC_LOG_DEBUG("entities.vehicle", "Vehicle::Uninstall Entry: %u, GuidLow: %u", _creatureEntry, _me->GetGUID().GetCounter());
    RemoveAllPassengers();
}

/**
 * @fn void Vehicle::Reset(bool evading )
 *
 * @brief Reapplies immunities and reinstalls accessories. Only has effect for creatures.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param evading true if called from CreatureAI::EnterEvadeMode
 */

void Vehicle::Reset(bool evading /*= false*/)
{
    if (GetBase()->GetTypeId() != TYPEID_UNIT)
        return;

    TC_LOG_DEBUG("entities.vehicle", "Vehicle::Reset (Entry: %u, GuidLow: %u, DBGuid: %u)", GetCreatureEntry(), _me->GetGUID().GetCounter(), _me->ToCreature()->GetDBTableGUIDLow());

    ApplyAllImmunities();
    InstallAllAccessories(evading);
}

/**
 * @fn void Vehicle::ApplyAllImmunities()
 *
 * @brief Applies specific immunities that cannot be set in DB.
 *
 * @author Machiavelli
 * @date 17-2-2013
 */

void Vehicle::ApplyAllImmunities()
{
    // This couldn't be done in DB, because some spells have MECHANIC_NONE

    // Vehicles should be immune on Knockback ...
    _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);

    // Mechanical units & vehicles ( which are not Bosses, they have own immunities in DB ) should be also immune on healing ( exceptions in switch below )
    if (_me->ToCreature() && _me->ToCreature()->GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL && !_me->ToCreature()->isWorldBoss())
    {
        // Heal & dispel ...
        _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
        _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
        _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);

        // ... Shield & Immunity grant spells ...
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_IMMUNITY, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_UNATTACKABLE, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_ABSORB, true);
        _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHIELD, true);
        _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_IMMUNE_SHIELD, true);

        // ... Resistance, Split damage, Change stats ...
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_DAMAGE_SHIELD, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SPLIT_DAMAGE_PCT, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_RESISTANCE, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STAT, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, true);
    }

    // Different immunities for vehicles goes below
    switch (GetVehicleInfo()->m_ID)
    {
        // code below prevents a bug with movable cannons
        // Strand of the Ancients
        case 160: // Antipersonnel Cannon
        // Wintergrasp
        case 244: // Wintergrasp Tower Cannon
        // Isle of Conquest
        case 510: // Keep Cannon
        case 452: // Alliance Gunship Cannon
        case 453: // Horde Gunship Cannon
            _me->SetControlled(true, UNIT_STATE_ROOT);
            // why we need to apply this? we can simple add immunities to slow mechanic in DB
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
            break;
        // Ulduar
        case 336: // Salvaged Siege Engine
        case 337: // Salvaged Siege Turret
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_ABSORB, false);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, false);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, false);
            break;
        case 335: // Salvaged Chopper
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, false);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, false);
            break;
        case 338: // Salvaged Demolisher
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, false);
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, false);
            break;
        // Icecrown Citadel
        case 532: // Val'kyr Shadowguard
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
            _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
            break;
        case 321: // Pilgrims Bounty: Chair
            _me->SetControlled(true, UNIT_STATE_ROOT);
            break;
        default:
            break;
    }
}

/**
 * @fn void Vehicle::RemoveAllPassengers()
 *
 * @brief Removes all current and pending passengers from the vehicle.
 *
 * @author Machiavelli
 * @date 17-2-2013
 */

void Vehicle::RemoveAllPassengers()
{
    TC_LOG_DEBUG("entities.vehicle", "Vehicle::RemoveAllPassengers. Entry: %u, GuidLow: %u", _creatureEntry, _me->GetGUID().GetCounter());

    // Passengers always cast an aura with SPELL_AURA_CONTROL_VEHICLE on the vehicle
    // We just remove the aura and the unapply handler will make the target leave the vehicle.
    // We don't need to iterate over Seats
    _me->RemoveAurasByType(SPELL_AURA_CONTROL_VEHICLE);

    // Following the above logic, this assertion should NEVER fail.
    // Even in 'hacky' cases, there should at least be VEHICLE_SPELL_RIDE_HARDCODED on us.
    // SeatMap::const_iterator itr;
    // for (itr = Seats.begin(); itr != Seats.end(); ++itr)
    //    ASSERT(!itr->second.passenger);
}

/**
 * @fn bool Vehicle::HasEmptySeat(int8 seatId) const
 *
 * @brief Checks if vehicle's seat specified by 'seatId' is empty.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param seatId Identifier for the seat.
 *
 * @return true if empty seat, false if not.
 */

bool Vehicle::HasEmptySeat(int8 seatId) const
{
    SeatMap::const_iterator seat = Seats.find(seatId);
    if (seat == Seats.end())
        return false;
    return seat->second.IsEmpty();
}

/**
 * @fn Unit* Vehicle::GetPassenger(int8 seatId) const
 *
 * @brief Gets a passenger on specified seat.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param seatId Seat to look on.
 *
 * @return null if it not found, else pointer to passenger if in world
 */

Unit* Vehicle::GetPassenger(int8 seatId) const
{
    SeatMap::const_iterator seat = Seats.find(seatId);
    if (seat == Seats.end())
        return NULL;

    return ObjectAccessor::GetUnit(*GetBase(), seat->second.Passenger.Guid);
}

/**
 * @fn SeatMap::const_iterator Vehicle::GetNextEmptySeat(int8 seatId, bool next) const
 *
 * @brief Gets the next empty seat based on current seat.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param seatId Identifier for the current seat.
 * @param next   true if iterating forward, false means iterating backwards.
 *
 * @return The next empty seat.
 */

int8 Vehicle::GetNextEmptySeat(int8 seatId, bool next) const
{
    SeatMap::const_iterator seat = Seats.find(seatId);
    if (seat == Seats.end())
        return -1;

    while (!seat->second.IsEmpty() || (!seat->second.SeatInfo->CanEnterOrExit() && !seat->second.SeatInfo->IsUsableByOverride()))
    {
        if (next)
        {
            if (++seat == Seats.end())
                seat = Seats.begin();
        }
        else
        {
            if (seat == Seats.begin())
                seat = Seats.end();
            --seat;
        }

        // Make sure we don't loop indefinetly
        if (seat->first == seatId)
            return -1;
    }

    return seat->first;
}

/**
 * @fn void Vehicle::InstallAccessory(uint32 entry, int8 seatId, bool minion, uint8 type,
 *     uint32 summonTime)
 *
 * @brief Installs an accessory.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param entry      The NPC entry of accessory.
 * @param seatId     Identifier for the seat to add the accessory to.
 * @param minion     true if accessory considered a 'minion'. Implies that the accessory will despawn when the vehicle despawns.
 *                   Essentially that it has no life without the vehicle. Their fates are bound.
 * @param type       See enum @SummonType.
 * @param summonTime Time after which the minion is despawned in case of a timed despawn @type specified.
 */

void Vehicle::InstallAccessory(uint32 entry, int8 seatId, bool minion, uint8 type, uint32 summonTime)
{
    /// @Prevent adding accessories when vehicle is uninstalling. (Bad script in OnUninstall/OnRemovePassenger/PassengerBoarded hook.)
    if (_status == STATUS_UNINSTALLING)
    {
        TC_LOG_ERROR("entities.vehicle", "Vehicle (GuidLow: %u, DB GUID: %u, Entry: %u) attempts to install accessory (Entry: %u) on seat %d with STATUS_UNINSTALLING! "
            "Check Uninstall/PassengerBoarded script hooks for errors.", _me->GetGUID().GetCounter(),
            (_me->GetTypeId() == TYPEID_UNIT ? _me->ToCreature()->GetDBTableGUIDLow() : _me->GetGUID().GetCounter()), GetCreatureEntry(), entry, (int32)seatId);
        return;
    }

    TC_LOG_DEBUG("entities.vehicle", "Vehicle (GuidLow: %u, DB Guid: %u, Entry %u): installing accessory (Entry: %u) on seat: %d",
        _me->GetGUID().GetCounter(), (_me->GetTypeId() == TYPEID_UNIT ? _me->ToCreature()->GetDBTableGUIDLow() : _me->GetGUID().GetCounter()), GetCreatureEntry(),
        entry, (int32)seatId);

    TempSummon* accessory = _me->SummonCreature(entry, *_me, TempSummonType(type), summonTime);
    ASSERT(accessory);

    if (minion)
        accessory->AddUnitTypeMask(UNIT_MASK_ACCESSORY);

    (void)_me->HandleSpellClick(accessory, seatId);

    /// If for some reason adding accessory to vehicle fails it will unsummon in
    /// @VehicleJoinEvent::Abort
}

/**
 * @fn bool Vehicle::AddPassenger(Unit* unit, int8 seatId)
 *
 * @brief Attempts to add a passenger to the vehicle on 'seatId'.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param [in, out] The prospective passenger.
 * @param seatId        Identifier for the seat. Value of -1 indicates the next available seat.
 *
 * @return true if it succeeds, false if it fails.
 */

bool Vehicle::AddPassenger(Unit* unit, int8 seatId)
{
    /// @Prevent adding passengers when vehicle is uninstalling. (Bad script in OnUninstall/OnRemovePassenger/PassengerBoarded hook.)
    if (_status == STATUS_UNINSTALLING)
    {
        TC_LOG_ERROR("entities.vehicle", "Passenger GuidLow: %u, Entry: %u, attempting to board vehicle GuidLow: %u, Entry: %u during uninstall! SeatId: %d",
            unit->GetGUID().GetCounter(), unit->GetEntry(), _me->GetGUID().GetCounter(), _me->GetEntry(), (int32)seatId);
        return false;
    }

    TC_LOG_DEBUG("entities.vehicle", "Unit %s scheduling enter vehicle (entry: %u, vehicleId: %u, guid: %u (dbguid: %u) on seat %d",
        unit->GetName().c_str(), _me->GetEntry(), _vehicleInfo->m_ID, _me->GetGUID().GetCounter(),
        (_me->GetTypeId() == TYPEID_UNIT ? _me->ToCreature()->GetDBTableGUIDLow() : 0), (int32)seatId);

    // The seat selection code may kick other passengers off the vehicle.
    // While the validity of the following may be arguable, it is possible that when such a passenger
    // exits the vehicle will dismiss. That's why the actual adding the passenger to the vehicle is scheduled
    // asynchronously, so it can be cancelled easily in case the vehicle is uninstalled meanwhile.
    SeatMap::iterator seat;

    if (seatId < 0) // no specific seat requirement
    {
        for (seat = Seats.begin(); seat != Seats.end(); ++seat)
            if (seat->second.IsEmpty() && (seat->second.SeatInfo->CanEnterOrExit() || seat->second.SeatInfo->IsUsableByOverride()))
                break;

        if (seat == Seats.end()) // no available seat
            return false;
    }
    else
    {
        seat = Seats.find(seatId);
        if (seat == Seats.end())
            return false;

        if (!seat->second.IsEmpty())
        {
            if (Unit* passenger = ObjectAccessor::GetUnit(*GetBase(), seat->second.Passenger.Guid))
                passenger->ExitVehicle();
            else
                seat->second.Passenger.Reset();
        }

        ASSERT(seat->second.IsEmpty());
    }

    // Remove auras that might interfere with vehicles
    unit->RemoveAurasDueToSpell(51690); // Killing Spree
    unit->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_MOUNT);

    unit->SetVehicle(this);
    seat->second.Passenger.Guid = unit->GetGUID();
    seat->second.Passenger.IsUnselectable = unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    if (seat->second.SeatInfo->CanEnterOrExit())
    {
        ASSERT(UsableSeatNum);
        --(UsableSeatNum);
        if (!UsableSeatNum)
        {
            if (GetBase()->GetTypeId() == TYPEID_PLAYER)
                GetBase()->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
            else
                GetBase()->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    if (seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_PASSENGER_NOT_SELECTABLE || _me->GetEntry() == 73531)
        unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    VehicleSeatEntry const* veSeat = seat->second.SeatInfo;

    Position attachmentPos{ veSeat->m_attachmentOffsetX, veSeat->m_attachmentOffsetY, veSeat->m_attachmentOffsetZ, 0.0f };

    if (Creature* creature = GetBase()->ToCreature())
        if (creature->IsAIEnabled)
            creature->AI()->GetPassengerEnterPosition(unit, seat->first, attachmentPos);

    unit->m_movementInfo.transport.pos.Relocate(attachmentPos);
    unit->m_movementInfo.transport.time = 0;
    unit->m_movementInfo.transport.seat = seat->first;
    unit->m_movementInfo.transport.guid = GetBase()->GetGUID();

    // Hacks to correct seat position until we are able to read attachment locations from model files (i.e. never)
    switch (_me->GetEntry())
    {
        // Instant Statue Pedestal
        case 40246:
            unit->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
            break;
    }

    if (GetBase()->GetTypeId() == TYPEID_UNIT && unit->GetTypeId() == TYPEID_PLAYER &&
        seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_CAN_CONTROL)
        ASSERT(GetBase()->SetCharmedBy(unit, CHARM_TYPE_VEHICLE));  // SMSG_CLIENT_CONTROL
    else if (unit->GetTypeId() == TYPEID_PLAYER)
        unit->SetDisableGravity(true);

    unit->SendClearTarget();                            // SMSG_BREAK_TARGET
    unit->SetControlled(true, UNIT_STATE_ROOT);         // SMSG_FORCE_ROOT - In some cases we send SMSG_SPLINE_MOVE_ROOT here (for creatures)
                                                        // also adds MOVEMENTFLAG_ROOT

    Movement::MoveSplineInit init(unit);
    init.DisableTransportPathTransformations();
    init.MoveTo(attachmentPos.GetPositionX(), attachmentPos.GetPositionY(), attachmentPos.GetPositionZ(), false, true);
    init.SetFacing(0.0f);
    init.SetTransportEnter();
    init.Launch();

    if (Creature* creature = GetBase()->ToCreature())
    {
        if (creature->IsAIEnabled)
            creature->AI()->PassengerBoarded(unit, seat->first, true);

        // update all passenger's positions
        RelocatePassengers();
    }

    return true;
}

/**
 * @fn void Vehicle::RemovePassenger(Unit* unit)
 *
 * @brief Removes the passenger from the vehicle.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param [in, out] unit The passenger to remove.
 */

Vehicle* Vehicle::RemovePassenger(Unit* unit, Position* exitPosition)
{
    if (unit->GetVehicle() != this)
        return NULL;

    SeatMap::iterator seat = GetSeatIteratorForPassenger(unit);
    ASSERT(seat != Seats.end());

    TC_LOG_DEBUG("entities.vehicle", "Unit %s exit vehicle entry %u id %u dbguid %u seat %d",
        unit->GetName().c_str(), _me->GetEntry(), _vehicleInfo->m_ID, _me->GetGUID().GetCounter(), (int32)seat->first);

    if (seat->second.SeatInfo->CanEnterOrExit() && ++UsableSeatNum)
        _me->SetFlag(UNIT_FIELD_NPC_FLAGS, (_me->GetTypeId() == TYPEID_PLAYER ? UNIT_NPC_FLAG_PLAYER_VEHICLE : UNIT_NPC_FLAG_SPELLCLICK));

    // Remove UNIT_FLAG_NOT_SELECTABLE if passenger did not have it before entering vehicle
    if ((seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_PASSENGER_NOT_SELECTABLE && !seat->second.Passenger.IsUnselectable) || _me->GetEntry() == 73531)
        unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    seat->second.Passenger.Reset();

    if (_me->GetTypeId() == TYPEID_UNIT && unit->GetTypeId() == TYPEID_PLAYER && seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_CAN_CONTROL)
        _me->RemoveCharmedBy(unit);
    else if (unit->GetTypeId() == TYPEID_PLAYER)
        unit->SetDisableGravity(false);

    Position pos;
    pos.Relocate(_me);
    if (_me->GetTypeId() == TYPEID_UNIT && _me->IsAIEnabled)
        _me->ToCreature()->AI()->GetPassengerExitPosition(unit, seat->first, pos);
    if (unit->GetTypeId() == TYPEID_UNIT && unit->IsAIEnabled)
        unit->ToCreature()->AI()->GetVehicleExitPosition(this, seat->first, pos);
    if (exitPosition)
        exitPosition->Relocate(pos);

    if (_me->IsInWorld())
    {
        unit->Relocate(pos);
        unit->m_movementInfo.pos.Relocate(pos);
        if (!_me->GetTransport())
        {
            unit->m_movementInfo.ResetTransport();
        }
        else
            unit->m_movementInfo.transport = _me->m_movementInfo.transport;
    }

    // only for flyable vehicles
    if (unit->IsFlying())
        _me->CastSpell(unit, VEHICLE_SPELL_PARACHUTE, true);

    unit->SetVehicle(nullptr);

    if (_me->GetTypeId() == TYPEID_UNIT && _me->IsAIEnabled)
        _me->ToCreature()->AI()->PassengerBoarded(unit, seat->first, false);
    if (unit->GetTypeId() == TYPEID_UNIT && unit->IsAIEnabled)
        unit->ToCreature()->AI()->BoardedVehicle(this, seat->first, false);

    return this;
}

/**
 * @fn void Vehicle::RelocatePassengers()
 *
 * @brief Relocate passengers. Must be called after m_base::Relocate
 *
 * @author Machiavelli
 * @date 17-2-2013
 */

void Vehicle::RelocatePassengers()
{
    ASSERT(_me->GetMap());

    // not sure that absolute position calculation is correct, it must depend on vehicle pitch angle
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
    {
        if (Unit* passenger = ObjectAccessor::GetUnit(*GetBase(), itr->second.Passenger.Guid))
        {
            ASSERT(passenger->IsInWorld());

            float px, py, pz, po;
            passenger->m_movementInfo.transport.pos.GetPosition(px, py, pz, po);
            CalculatePassengerPosition(px, py, pz, &po);

            passenger->UpdatePosition(px, py, pz, po);
        }
    }
}

/**
 * @fn bool Vehicle::IsVehicleInUse() const
 *
 * @brief Returns information whether the vehicle is currently used by any unit
 *
 * @author Shauren
 * @date 26-2-2013
 *
 * @return true if any passenger is boarded on vehicle, false otherwise.
 */

bool Vehicle::IsVehicleInUse() const
{
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (!itr->second.IsEmpty())
            return true;

    return false;
}

/**
 * @fn void Vehicle::InitMovementInfoForBase()
 *
 * @brief Sets correct MovementFlags2 based on VehicleFlags from DBC.
 *
 * @author Machiavelli
 * @date 17-2-2013
 */

void Vehicle::InitMovementInfoForBase()
{
    uint32 vehicleFlags = GetVehicleInfo()->m_flags;

    if (vehicleFlags & VEHICLE_FLAG_NO_STRAFE)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_NO_STRAFE);
    if (vehicleFlags & VEHICLE_FLAG_NO_JUMPING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);
    if (vehicleFlags & VEHICLE_FLAG_FULLSPEEDTURNING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_FULL_SPEED_TURNING);
    if (vehicleFlags & VEHICLE_FLAG_ALLOW_PITCHING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING);
    if (vehicleFlags & VEHICLE_FLAG_FULLSPEEDPITCHING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_FULL_SPEED_PITCHING);
}

/**
 * @fn VehicleSeatEntry const* Vehicle::GetSeatForPassenger(Unit* passenger)
 *
 * @brief Returns information on the seat of specified passenger, represented by the format in VehicleSeat.dbc
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param [in, out] The passenger for which we check the seat info.
 *
 * @return null if passenger not found on vehicle, else the DBC record for the seat.
 */

VehicleSeatEntry const* Vehicle::GetSeatForPassenger(Unit const* passenger) const
{
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (itr->second.Passenger.Guid == passenger->GetGUID())
            return itr->second.SeatInfo;

    return NULL;
}

/**
 * @fn SeatMap::iterator Vehicle::GetSeatIteratorForPassenger(Unit* passenger)
 *
 * @brief Gets seat iterator for specified passenger.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @param [in, out] passenger Passenger to look up.
 *
 * @return The seat iterator for specified passenger if it's found on the vehicle. Otherwise Seats.end() (invalid iterator).
 */

SeatMap::iterator Vehicle::GetSeatIteratorForPassenger(Unit* passenger)
{
    SeatMap::iterator itr;
    for (itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (itr->second.Passenger.Guid == passenger->GetGUID())
            return itr;

    return Seats.end();
}

/**
 * @fn uint8 Vehicle::GetAvailableSeatCount() const
 *
 * @brief Gets the available seat count.
 *
 * @author Machiavelli
 * @date 17-2-2013
 *
 * @return The available seat count.
 */

uint8 Vehicle::GetAvailableSeatCount() const
{
    uint8 ret = 0;
    SeatMap::const_iterator itr;
    for (itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (itr->second.IsEmpty() && (itr->second.SeatInfo->CanEnterOrExit() || itr->second.SeatInfo->IsUsableByOverride()))
            ++ret;

    return ret;
}
