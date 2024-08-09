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

#include "Log.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "ObjectMgr.h"
#include "TemporarySummon.h"
#include "Pet.h"
#include "Player.h"
#include "BattlePetMgr.h"
#include "InstanceScript.h"

TempSummon::TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject) :
Creature(isWorldObject), m_Properties(properties), m_type(TEMPSUMMON_MANUAL_DESPAWN),
m_timer(0), m_lifetime(0)
{
    m_summonerGUID = owner ? owner->GetGUID() : ObjectGuid::Empty;
    m_unitTypeMask |= UNIT_MASK_SUMMON;
}

Unit* TempSummon::GetSummoner() const
{
    return m_summonerGUID ? ObjectAccessor::GetUnit(*this, m_summonerGUID) : nullptr;
}

Creature* TempSummon::GetSummonerCreatureBase() const
{
    return m_summonerGUID ? ObjectAccessor::GetCreature(*this, m_summonerGUID) : nullptr;
}

void TempSummon::Update(uint32 diff)
{
    Creature::Update(diff);

    // Could be despawned due to event in Unit::Update
    if (!IsInWorld())
        return;

    if (m_deathState == DEAD)
    {
        UnSummon();
        return;
    }
    switch (m_type)
    {
        case TEMPSUMMON_MANUAL_DESPAWN:
        case TEMPSUMMON_DEAD_DESPAWN:    
            break;
        case TEMPSUMMON_TIMED_DESPAWN:
        {
            if (m_timer <= diff)
            {
                m_timer = 0;    // To know that despawned by timer
                UnSummon();
                return;
            }

            m_timer -= diff;
            break;
        }
        case TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT:
        {
            if (!IsInCombat())
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }

                m_timer -= diff;
            }
            else if (m_timer != m_lifetime)
                m_timer = m_lifetime;

            break;
        }

        case TEMPSUMMON_CORPSE_TIMED_DESPAWN:
        {
            if (m_deathState == CORPSE)
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }

                m_timer -= diff;
            }
            break;
        }
        case TEMPSUMMON_CORPSE_DESPAWN:
        {
            // if m_deathState is DEAD, CORPSE was skipped
            if (m_deathState == CORPSE || m_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            break;
        }
        case TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN:
        {
            // if m_deathState is DEAD, CORPSE was skipped
            if (m_deathState == CORPSE || m_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            if (!IsInCombat())
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }
                else
                    m_timer -= diff;
            }
            else if (m_timer != m_lifetime)
                m_timer = m_lifetime;
            break;
        }
        case TEMPSUMMON_TIMED_OR_DEAD_DESPAWN:
        {
            // if m_deathState is DEAD, CORPSE was skipped
            if (m_deathState == DEAD)
            {
                UnSummon();
                return;
            }

            if (!IsInCombat() && IsAlive())
            {
                if (m_timer <= diff)
                {
                    UnSummon();
                    return;
                }
                else
                    m_timer -= diff;
            }
            else if (m_timer != m_lifetime)
                m_timer = m_lifetime;
            break;
        }
        default:
            UnSummon();
            TC_LOG_ERROR("entities.unit", "Temporary summoned creature (entry: %u) have unknown type %u of ", GetEntry(), m_type);
            break;
    }
}

bool TempSummon::Create(uint32 guidlow, Map* map, uint32 phaseMask, uint32 entry, uint32 vehId, uint32 team, float x, float y, float z, float ang, CreatureData const* data)
{
    if (!Creature::Create(guidlow, map, phaseMask, entry, vehId, team, x, y, z, ang, data))
        return false;


    if (m_Properties)
    {
        m_slot = m_Properties->Type != SUMMON_TYPE_STATUE ? m_Properties->Slot : 1;
        if (GetEntry() == 59190)
            m_slot = 1;
        
        if (Unit* owner = GetSummoner())
        {
            uint8 maxslot = 0;
            switch (GetEntry())
            {
                case 47649: maxslot = 3; break; // Wild Mushroom
                case 60199: maxslot = 2; break; // Rune of Power
            }

            if (maxslot)
            {
                bool found = false;
                std::list<TempSummon*> summons;
                owner->GetSummons(summons, GetEntry());
                uint8 slot = 1;
                for (; slot <= maxslot; ++slot)
                {
                    bool exists = false;
                    for (auto&& itr : summons)
                        if (itr->m_slot == slot)
                            exists = true;

                    if (!exists)
                    {
                        found = true;
                        break;
                    }
                }
                if (found)
                    m_slot = slot;
                else
                    m_slot = summons.front()->m_slot;
            }

            // Psyfiend, Lightwell
            if (GetEntry() == 59190 || GetEntry() == 31897 || GetEntry() == 64571)
            {
                std::list<TempSummon*> summons;
                owner->GetSummons(summons, GetEntry());
                for (auto&& itr : summons) // Fuuuuck
                    itr->DespawnOrUnsummon();

                summons.clear();

                if (GetEntry() == 59190)
                {
                    owner->GetSummons(summons, 31897);
                    owner->GetSummons(summons, 64571);
                }
                else if (GetEntry() == 31897 || GetEntry() == 64571)
                    owner->GetSummons(summons, 59190);

                for (auto&& itr : summons)
                    if (itr->m_slot == m_slot)
                        m_slot = itr->m_slot == 1 ? 2 : 1;
            }
        }
    }
    return true;
}

void TempSummon::InitStats(uint32 duration)
{
    ASSERT(!IsPet());

    m_timer = duration;
    m_lifetime = duration;

    if (m_type == TEMPSUMMON_MANUAL_DESPAWN)
        m_type = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

    Unit* owner = GetSummoner();

    if (owner && IsTrigger() && m_spells[0])
    {
        SetFaction(owner->GetFaction());
        SetLevel(owner->GetLevel());
        if (owner->GetTypeId() == TYPEID_PLAYER)
            m_ControlledByPlayer = true;
    }

    if (owner && owner->GetTypeId() == TYPEID_PLAYER && m_slot >= SUMMON_SLOT_TOTEM && m_slot < SUMMON_SLOT_MAX_TOTEM)
        owner->ToPlayer()->GetSession()->SendTotemCreated(GetGUID(), duration, GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL), m_slot - 1);

    if (!m_Properties)
        return;

    if (owner)
    {
        int32 slot = m_slot;
        if (m_slot > 0)
        {
            if (owner->m_SummonSlot[slot] && owner->m_SummonSlot[slot] != GetGUID())
            {
                Creature* oldSummon = GetMap()->GetCreature(owner->m_SummonSlot[slot]);
                if (oldSummon && oldSummon->IsSummon())
                {
                    // Totemic Persistence. Exclude fire totems
                    if (slot > SUMMON_SLOT_TOTEM_FIRE && slot < SUMMON_SLOT_MAX_TOTEM && owner->HasAura(108284) && !owner->m_SummonSlot[SUMMON_SLOT_TOTEM_EXTRA])
                        owner->m_SummonSlot[SUMMON_SLOT_TOTEM_EXTRA] = oldSummon->GetGUID();
                    else
                        oldSummon->ToTempSummon()->UnSummon();
                }
            }
            owner->m_SummonSlot[slot] = GetGUID();
        }
    }

    if (m_Properties->Faction)
        SetFaction(m_Properties->Faction);
    else if (IsVehicle() && owner) // properties should be vehicle
        SetFaction(owner->GetFaction());
}

void TempSummon::InitSummon()
{
    Unit* owner = GetSummoner();
    if (owner)
    {
        if (owner->GetTypeId() == TYPEID_UNIT && owner->ToCreature()->IsAIEnabled)
            owner->ToCreature()->AI()->JustSummoned(this);
        if (IsAIEnabled)
            AI()->IsSummonedBy(owner);
    }
}

void TempSummon::SetTempSummonType(TempSummonType type)
{
    m_type = type;
}

void TempSummon::UnSummon(uint32 msTime)
{
    if (msTime)
    {
        ForcedUnsummonDelayEvent* pEvent = new ForcedUnsummonDelayEvent(*this);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(msTime));
        return;
    }

    //ASSERT(!IsPet());
    if (IsPet() && !ToPet()->IsTemporary())
    {
        ToPet()->Remove(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
        ASSERT(!IsInWorld());
        return;
    }

    Unit* owner = GetSummoner();
    if (owner && owner->GetTypeId() == TYPEID_UNIT && owner->ToCreature()->IsAIEnabled)
        owner->ToCreature()->AI()->SummonedCreatureDespawn(this);

    if (IsAIEnabled)
        AI()->Unsummoned();

    AddObjectToRemoveList();
}

bool ForcedUnsummonDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.UnSummon();
    return true;
}

void TempSummon::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    if (m_Properties)
    {
        int32 slot = m_Properties->Slot;
        if (slot > 0)
        {
            if (Unit* owner = GetSummoner())
            {
                if (owner->m_SummonSlot[slot] == GetGUID())
                    owner->m_SummonSlot[slot].Clear();
                else if (owner->m_SummonSlot[SUMMON_SLOT_TOTEM_EXTRA] == GetGUID())
                    owner->m_SummonSlot[SUMMON_SLOT_TOTEM_EXTRA].Clear();
            }
        }
    }

    if (Unit* owner = GetSummoner())
    {
        owner->RemoveSummon(this);
        if (Player* player = owner->ToPlayer())
            if (player->GetBattlePetMgr().GetCurrentSummon() == this)
                player->GetBattlePetMgr().SetCurrentSummon(ObjectGuid::Empty);
    }

    //if (GetOwnerGUID())
    //    TC_LOG_ERROR("entities.unit", "Unit %u has owner guid when removed from world", GetEntry());

    Creature::RemoveFromWorld();
}

uint32 TempSummon::GetBaseAttackTimer() const
{
    switch (GetEntry())
    {
        case 1863:  // Succubus
        case 58963: // Shivarra
        case 29264: // Spirit Wolf
        case 15438: // Fire Elemental
        case 61029: // Pirmal Fire Elemental
        case 19668: // Shadowfiend
        case 27893: // Dancing Rune Weapon
        case 62982: // Mindbender
        case 63508: // Xuen
            return ToCreature()->GetCreatureTemplate()->BaseAttackTime;
    }
    return BASE_ATTACK_TIME;
}

Minion::Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject)
    : TempSummon(properties, owner, isWorldObject), m_owner(owner)
{
    ASSERT(m_owner);
    m_unitTypeMask |= UNIT_MASK_MINION;
    m_followAngle = PET_FOLLOW_ANGLE;
    // Uncomment this if crashes
    // InitCharmInfo();
}

void Minion::InitStats(uint32 duration)
{
    TempSummon::InitStats(duration);

    SetReactState(REACT_PASSIVE);

    //SetUInt64Value(UNIT_FIELD_DEMON_CREATOR, GetOwner()->GetGUID());
    SetCreatorGUID(GetOwner()->GetGUID());
    SetFaction(GetOwner()->GetFaction());

    GetOwner()->SetMinion(this, true);
}

void Minion::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    GetOwner()->SetMinion(this, false);
    TempSummon::RemoveFromWorld();
}

bool Minion::IsGuardianPet() const
{
    if (IsPet())
        return !ToPet()->IsTemporary();

    return m_Properties && m_Properties->Category == SUMMON_CATEGORY_PET;
}

Guardian::Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject) : Minion(properties, owner, isWorldObject), m_bonusSpellDamage(0)
{
    memset(m_statFromOwner, 0, sizeof(float)*MAX_STATS);
    m_unitTypeMask |= UNIT_MASK_GUARDIAN;
    if (properties && (properties->Category == SUMMON_CATEGORY_PET ||
        (properties->Type == SUMMON_TYPE_PET && properties->Category != SUMMON_CATEGORY_ALLY)))
    {
        m_unitTypeMask |= UNIT_MASK_CONTROLABLE_GUARDIAN;
        InitCharmInfo();
    }
}

void Guardian::InitStats(uint32 duration)
{
    Minion::InitStats(duration);

    InitStatsForLevel(GetOwner()->GetLevel());

    if (GetOwner()->GetTypeId() == TYPEID_PLAYER && HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
    {
        SetReactState(REACT_ASSIST);
        m_charmInfo->InitCharmCreateSpells();
    }
    else
        SetReactState(REACT_AGGRESSIVE);
}

void Guardian::InitSummon()
{
    TempSummon::InitSummon();

    if (GetOwner()->GetTypeId() == TYPEID_PLAYER
            && GetOwner()->GetMinionGUID() == GetGUID()
            && !GetOwner()->GetCharmGUID())
    {
        GetOwner()->ToPlayer()->CharmSpellInitialize();
    }
}

Puppet::Puppet(SummonPropertiesEntry const* properties, Unit* owner)
    : Minion(properties, owner, false) //maybe true?
{
    ASSERT(m_owner->GetTypeId() == TYPEID_PLAYER);
    m_unitTypeMask |= UNIT_MASK_PUPPET;
}

void Puppet::InitStats(uint32 duration)
{
    Minion::InitStats(duration);
    SetLevel(GetOwner()->GetLevel());
    SetReactState(REACT_PASSIVE);
}

void Puppet::InitSummon()
{
    Minion::InitSummon();
    if (!SetCharmedBy(GetOwner(), CHARM_TYPE_POSSESS))
        ASSERT(false);
}

void Puppet::Update(uint32 time)
{
    Minion::Update(time);
    //check if caster is channelling?
    if (IsInWorld())
    {
        if (!IsAlive())
        {
            UnSummon();
            /// @todo why long distance .die does not remove it
        }
    }
}

void Puppet::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    RemoveCharmedBy(nullptr);
    Minion::RemoveFromWorld();
}
