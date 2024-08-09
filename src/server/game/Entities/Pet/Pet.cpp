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

#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Pet.h"
#include "Formulas.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "CreatureAI.h"
#include "Unit.h"
#include "Util.h"
#include "Group.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "Spell.h"
#include "SpellHistory.h"

#define PET_XP_FACTOR 0.05f

const float Pet::BaseRunSpeedRate = 8.0f / 7.0f;

Pet::Pet(Player* owner, PetType type) :
    Guardian(NULL, owner, true), m_usedTalentCount(0), m_removed(false),
    m_petType(type), m_duration(0), m_auraRaidUpdateMask(0), m_loading(false),
    m_declinedname(NULL)
{
    ASSERT(m_owner->GetTypeId() == TYPEID_PLAYER);

    m_unitTypeMask |= UNIT_MASK_PET;
    if (type == HUNTER_PET)
        m_unitTypeMask |= UNIT_MASK_HUNTER_PET;

    if (!(m_unitTypeMask & UNIT_MASK_CONTROLABLE_GUARDIAN))
    {
        m_unitTypeMask |= UNIT_MASK_CONTROLABLE_GUARDIAN;
        InitCharmInfo();
    }

    m_name = "Pet";
    m_regenTimer = PET_FOCUS_REGEN_INTERVAL;
}

Pet::~Pet()
{
    delete m_declinedname;
}

void Pet::AddToWorld()
{
    ///- Register the pet for guid lookup
    if (!IsInWorld())
    {
        ///- Register the pet for guid lookup
        GetMap()->GetObjectsStore().Insert<Pet>(GetGUID(), this);
        Unit::AddToWorld();
        AIM_Initialize();
    }

    // Prevent stuck pets when zoning. Pets default to "follow" when added to world
    // so we'll reset flags and let the AI handle things
    if (GetCharmInfo() && GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
    {
        GetCharmInfo()->SetIsCommandAttack(false);
        GetCharmInfo()->SetIsCommandFollow(false);
        GetCharmInfo()->SetIsAtStay(false);
        GetCharmInfo()->SetIsFollowing(false);
        GetCharmInfo()->SetIsReturning(false);
    }

    if (GetOwner()->HasAura(57870) && !HasAura(57894))  // Glyph of Lesser Proportion
        CastSpell(this, 57894, true);
}

void Pet::RemoveFromWorld()
{
    ///- Remove the pet from the accessor
    if (IsInWorld())
    {
        if (IsTemporary()) // hackfix, need remove in future
        {
            ASSERT(FindMap() == GetOwner()->FindMap());
            GetOwner()->SetMinion(this, false);
            GetOwner()->RemoveSummon(this);
        }
        ///- Don't call the function for Creature, normal mobs + totems go in a different storage
        Unit::RemoveFromWorld();
        GetMap()->GetObjectsStore().Remove<Pet>(GetGUID());
    }
}

bool Pet::LoadPetFromDB(PetLoadMode mode, uint32 param, Position const* pos)
{
    Player* owner = GetOwner();
    uint32 ownerid = owner->GetGUID().GetCounter();

    CharacterDatabasePreparedStatement* stmt;
    PreparedQueryResult result;

    switch (mode)
    {
        case PET_LOAD_BY_ID:
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_PET_BY_ID);
            break;
        case PET_LOAD_BY_ENTRY:
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_PET_BY_ENTRY);
            break;
        case PET_LOAD_BY_SLOT:
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_PET_BY_SLOT);
            break;
        default:
            std::abort();
    }

    stmt->setUInt32(0, ownerid);
    stmt->setUInt32(1, param);

    result = CharacterDatabase.Query(stmt);

    if (!result)
        return false;

    m_loading = true;

    Field* fields = result->Fetch();
    uint32 petId = fields[0].GetUInt32();
    bool current = petId == owner->GetCurrentPetId();
    uint32 petEntry = fields[1].GetUInt32();
    if (!petEntry)
        return false;

    uint32 summonSpellId = fields[14].GetUInt32();
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(summonSpellId);

    if (spellInfo && spellInfo->GetDuration() > 0)
        return false;

    PetType petType = PetType(fields[15].GetUInt8());
    if (petType == HUNTER_PET)
    {
        CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(petEntry);
        if (!creatureInfo || !creatureInfo->IsTameable(owner->CanTameExoticPets()))
            return false;
    }
    else
    {
        // TODO: Move this somewhere...
        switch (petEntry)
        {
            case 510:   // Water Elemental
                if (!owner->HasSpell(31687))
                    return false;
                break;
            case 17252: // Felgurad
                if (!owner->HasSpell(30146))
                    return false;
                break;
            case 58959: // Fel imp
                if (!owner->HasSpell(112866))
                    return false;
                break;
            case 58960: // Voidlord
                if (!owner->HasSpell(112867))
                    return false;
                break;
            case 58964: // Observer
                if (!owner->HasSpell(112869))
                    return false;
                break;
            case 58963: // Shivarra
                if (!owner->HasSpell(112868))
                    return false;
                break;
            case 58965: // Wrathguard
                if (!owner->HasSpell(112870))
                    return false;
                break;
        }
    }

    if (current && owner->IsPetNeedBeTemporaryUnsummoned())
    {
        owner->SetTemporaryUnsummonedPetNumber(petId);
        return false;
    }

    Map* map = owner->GetMap();
    uint32 guid = map->GenerateLowGuid<HighGuid::Pet>();
    if (!Create(guid, map, owner->GetPhaseMask(), petEntry, petId))
        return false;

    float px, py, pz;
    if (pos)
        pos->GetPosition(px, py, pz);
    else
        owner->GetClosePoint(px, py, pz, GetObjectSize(), PET_FOLLOW_DIST, GetFollowAngle());
    Relocate(px, py, pz, owner->GetOrientation());

    if (!IsPositionValid())
    {
        TC_LOG_ERROR("entities.pet", "Pet (guidlow %d, entry %d) not loaded. Suggested coordinates isn't valid (X: %f Y: %f)",
            GetGUID().GetCounter(), GetEntry(), GetPositionX(), GetPositionY());
        return false;
    }

    AddToTransportIfNeeded(owner->GetTransport());

    for (auto itr : owner->GetPhases())
        SetPhased(itr, false, true);

    setPetType(petType);
    SetFaction(owner->GetFaction());
    SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, summonSpellId);

    CreatureTemplate const* cinfo = GetCreatureTemplate();
    if (cinfo->type == CREATURE_TYPE_CRITTER)
    {
        map->AddToMap(this->ToCreature());
        m_loading = false;
        return true;
    }

    m_charmInfo->SetPetNumber(petId, IsPermanentPetFor(owner));

    SetDisplayId(fields[3].GetUInt32());
    SetNativeDisplayId(fields[3].GetUInt32());
    uint32 petlevel = owner->GetLevel();
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_NONE);
    SetName(fields[8].GetString());

    switch (getPetType())
    {
        case SUMMON_PET:
            SetClass(CLASS_MAGE);
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            // this enables popup window (pet dismiss, cancel)
            break;
        case HUNTER_PET:
            SetSheath(SHEATH_STATE_MELEE);
            SetClass(CLASS_WARRIOR);
            SetGender(GENDER_NONE);
            SetFieldPowerType(POWER_FOCUS);
            SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, 2, fields[9].GetBool() ? UNIT_CAN_BE_ABANDONED : UNIT_CAN_BE_RENAMED | UNIT_CAN_BE_ABANDONED);
            SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
                                                            // this enables popup window (pet abandon, cancel)
            SetPowerType(POWER_FOCUS);
            SetCorpseDelay(HOUR);
            break;
        default:
            if (!IsPetGhoul())
                TC_LOG_ERROR("entities.pet", "Pet have incorrect type (%u) for pet loading.", getPetType());
            break;
    }

    SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped here
    SetCreatorGUID(owner->GetGUID());

    InitStatsForLevel(petlevel);
    SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, fields[5].GetUInt32());

    SynchronizeLevelWithOwner();

    SetReactState(ReactStates(fields[6].GetUInt8()));
    SetCanModifyStats(true);

    if (getPetType() == SUMMON_PET && !current)              //all (?) summon pets come with full health when called, but not when they are current
        SetPower(POWER_MANA, GetMaxPower(POWER_MANA));
    else
    {
        uint32 savedhealth = fields[10].GetUInt32();
        uint32 savedmana = fields[11].GetUInt32();
        if (!savedhealth && getPetType() == HUNTER_PET)
            setDeathState(JUST_DIED);
        else
        {
            SetHealth(savedhealth > GetMaxHealth() ? GetMaxHealth() : savedhealth);
            SetPower(POWER_MANA, savedmana > uint32(GetMaxPower(POWER_MANA)) ? GetMaxPower(POWER_MANA) : savedmana);
        }
    }

    // Send fake summon spell cast - this is needed for correct cooldown application for spells
    // Example: 46584 - without this cooldown (which should be set always when pet is loaded) isn't set clientside
    /// @todo pets should be summoned from real cast instead of just faking it?
    // TODO FIX THIS GOD DAMN HACK
    /*if (summonSpellId)
    {
        WorldPacket data(SMSG_SPELL_GO, (8+8+4+4+2));
        data.append(owner->GetPackGUID());
        data.append(owner->GetPackGUID());
        data << uint8(0);
        data << uint32(summonSpellId);
        data << uint32(256); // CAST_FLAG_UNKNOWN3
        data << uint32(0);
        owner->SendMessageToSet(&data, true);
    }
    */

    owner->SetMinion(this, true);
    map->AddToMap(this->ToCreature());

    InitTalentForLevel();                                   // set original talents points before spell loading

    uint32 timediff = uint32(time(NULL) - fields[13].GetUInt32());
    _LoadAuras(timediff);

    m_charmInfo->LoadPetActionBar(fields[12].GetString());

    InitSpellsForLevel();
    uint16 specId = fields[16].GetUInt16();
    SetSpecialization(specId);

    // FIXME
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_SPELL_COOLDOWN);
    stmt->setUInt32(0, m_charmInfo->GetPetNumber());
    GetSpellHistory()->LoadFromDB(CharacterDatabase.Query(stmt), PreparedQueryResult{});

    _LoadSpells();
    CastPetAuras(current);

    CleanupActionBar();                                     // remove unknown spells from action bar after load
    owner->PetSpellInitialize();

    if (owner->GetGroup())
        owner->SetGroupUpdateFlag(GROUP_UPDATE_PET);

    if (getPetType() == HUNTER_PET)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_DECLINED_NAME);
        stmt->setUInt32(0, owner->GetGUID().GetCounter());
        stmt->setUInt32(1, GetCharmInfo()->GetPetNumber());
        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        if (result)
        {
            delete m_declinedname;
            m_declinedname = new DeclinedName;
            Field* fields2 = result->Fetch();
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            {
                m_declinedname->name[i] = fields2[i].GetString();
            }
        }
    }

    if (!IsTemporary())
    {
        owner->SetCurrentPetId(GetCharmInfo()->GetPetNumber());
        //set last used pet number (for use in BG's)
        if (owner->GetTypeId() == TYPEID_PLAYER && isControlled() && !isTemporarySummoned() && (getPetType() == SUMMON_PET || getPetType() == HUNTER_PET))
            owner->ToPlayer()->SetLastPetNumber(petId);
    }

    m_loading = false;

    UpdateCastingSpeed();
    UpdateAttackSpeed(BASE_ATTACK);
    UpdateAttackSpeed(OFF_ATTACK);
    UpdateHasteRegen();

    // HACK ALRET!
    // Due to performance issues passive positive area auras do target map update with 3 seconds interval.
    // But these glyphs must be applied as soon as possible, so let's just manually update the target map.
    if (GetEntry() == 510)  // Water Elemental
        for (auto&& it : { 63090, 146662, 147353 })
            if (Aura* aura = owner->GetOwnedAura(it))
                aura->UpdateTargetMap(owner);


    // Pet Summon, works as trigger
    m_owner->CastSpell(m_owner, 6962, true);

    return true;
}

void Pet::SavePetToDB(CharacterDatabaseTransaction trans)
{
    if (!GetEntry())
        return;

    // save only fully controlled creature
    if (!isControlled())
        return;

    // not save not player pets
    if (!GetOwnerGUID().IsPlayer())
        return;

    Player* owner = GetOwner();
    if (!owner)
        return;

    if (IsTemporary())
        return;

    uint32 curhealth = GetHealth();
    uint32 curmana = GetPower(POWER_MANA);

    bool standalone = !trans;
    if (standalone)
        trans = CharacterDatabase.BeginTransaction();

    // save auras before possibly removing them
    _SaveAuras(trans);
    _SaveSpells(trans);
    GetSpellHistory()->SaveToDB<Pet>(trans);

    uint32 ownerLowGUID = GetOwnerGUID().GetCounter();
    std::string name = m_name;
    CharacterDatabase.EscapeString(name);
    uint32 dbslot = 0;
    if (getPetType() == HUNTER_PET)
    {
        int8 slot = owner->GetSlotByPetId(GetCharmInfo()->GetPetNumber());
        dbslot = slot < 0 ? 255 : slot;
    }

    // save pet
    std::ostringstream ss;
    ss  << "REPLACE INTO character_pet (id, entry,  owner, modelid, level, exp, Reactstate, slot, name, renamed, curhealth, curmana, abdata, savetime, CreatedBySpell, PetType, specialization) "
        << "VALUES ("
        << m_charmInfo->GetPetNumber() << ','
        << GetEntry() << ','
        << ownerLowGUID << ','
        << GetNativeDisplayId() << ','
        << uint32(GetLevel()) << ','
        << GetUInt32Value(UNIT_FIELD_PET_EXPERIENCE) << ','
        << uint32(GetReactState()) << ','
        << uint32(dbslot) << ", '"
        << name.c_str() << "', "
        << uint32(HasByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, 2, UNIT_CAN_BE_RENAMED) ? 0 : 1) << ','
        << curhealth << ','
        << curmana << ", '";

    for (uint32 i = ACTION_BAR_INDEX_START; i < ACTION_BAR_INDEX_END; ++i)
    {
        ss << uint32(m_charmInfo->GetActionBarEntry(i)->GetType()) << ' '
            << uint32(m_charmInfo->GetActionBarEntry(i)->GetAction()) << ' ';
    };

    ss  << "', "
        << time(NULL) << ','
        << GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL) << ','
        << uint32(getPetType()) << ','
        << GetSpecializationId() << ')';

    trans->Append(ss.str().c_str());
    if (standalone)
        CharacterDatabase.CommitTransaction(trans);
}

void Pet::DeleteFromDB(uint32 guidlow)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_PET_BY_ID);
    stmt->setUInt32(0, guidlow);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_PET_DECLINEDNAME);
    stmt->setUInt32(0, guidlow);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_AURAS);
    stmt->setUInt32(0, guidlow);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_SPELLS);
    stmt->setUInt32(0, guidlow);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_SPELL_COOLDOWNS);
    stmt->setUInt32(0, guidlow);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void Pet::setDeathState(DeathState s)                       // overwrite virtual Creature::setDeathState and Unit::setDeathState
{
    Creature::setDeathState(s);
    if (getDeathState() == CORPSE)
    {
        if (getPetType() == HUNTER_PET)
        {
            // pet corpse non lootable and non skinnable
            SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
            //SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        }
    }
    else if (getDeathState() == ALIVE)
    {
        //RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        CastPetAuras(true);
    }
    if (s == JUST_DIED)
    {
        Player* owner = GetOwner();
        if (owner && owner->HasAura(108559) && !owner->HasAura(89140))  // Demonic Rebirth
        {
            Spell* spell = owner->GetCurrentSpell(CURRENT_GENERIC_SPELL);
            if (!spell || spell->GetSpellInfo()->Id != 108503)          // Killed by Grimoire of Sacrifice
                owner->CastSpell(owner, 88448, true);
        }
    }
}

void Pet::Update(uint32 diff)
{
    if (m_removed)                                           // pet already removed, just wait in remove queue, no updates
        return;

    if (m_loading)
        return;

    switch (m_deathState)
    {
        case CORPSE:
        {
            if (getPetType() != HUNTER_PET || m_temporary || m_corpseRemoveTime <= time(NULL))
            {
                Remove(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);               //hunters' pets never get removed because of death, NEVER!
                return;
            }
            break;
        }
        case ALIVE:
        {
            // unsummon pet that lost owner
            Player* owner = GetOwner();
            if (!owner || (!IsWithinDistInMap(owner, GetMap()->GetVisibilityRange()) && !isPossessed()) || (isControlled() && !owner->GetPetGUID()))
            //if (!owner || (!IsWithinDistInMap(owner, GetMap()->GetVisibilityDistance()) && (owner->GetCharmGUID() && (owner->GetCharmGUID() != GetGUID()))) || (isControlled() && !owner->GetPetGUID()))
            {
                Remove(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RETURN_REAGENT | PET_REMOVE_FLAG_RESET_CURRENT);
                return;
            }

            if (isControlled())
            {
                if (owner->GetPetGUID() != GetGUID())
                {
                    TC_LOG_ERROR("entities.pet", "Pet %u is not pet of owner %s, removed", GetEntry(), GetOwner()->GetName().c_str());
                    Remove(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
                    return;
                }
            }

            if (m_duration > 0)
            {
                if (uint32(m_duration) > diff)
                    m_duration -= diff;
                else
                {
                    Remove(PET_REMOVE_DISMISS);
                    return;
                }
            }
        }
        default:
            break;
    }
    Creature::Update(diff);
}

void Pet::Remove(PetRemoveMode mode, int32 flags)
{
    if (IsTemporary())
        UnSummon();
    else
        GetOwner()->RemovePet(mode, flags);
}

bool Pet::IsAutoCastEnabled(uint32 spellId) const
{
    for (auto&& itr : m_autospells)
        if (itr == spellId)
            return true;
    return false;
}

void Pet::GivePetLevel(uint8 level)
{
    if (!level || level == GetLevel())
        return;

    if (getPetType()==HUNTER_PET)
    {
        SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, 0);
        SetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE, uint32(sObjectMgr->GetXPForLevel(level)*PET_XP_FACTOR));
    }

    InitStatsForLevel(level);
    InitSpellsForLevel();
    InitTalentForLevel();
}

bool Pet::CreateBaseAtCreature(Creature* creature)
{
    ASSERT(creature);

    if (!CreateBaseAtTamed(creature->GetCreatureTemplate(), creature->GetMap(), creature->GetPhaseMask()))
        return false;

    Relocate(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation());

    if (!IsPositionValid())
    {
        TC_LOG_ERROR("entities.pet", "Pet (guidlow %d, entry %d) not created base at creature. Suggested coordinates isn't valid (X: %f Y: %f)",
            GetGUID().GetCounter(), GetEntry(), GetPositionX(), GetPositionY());
        return false;
    }

    CreatureTemplate const* cinfo = GetCreatureTemplate();
    if (!cinfo)
    {
        TC_LOG_ERROR("entities.pet", "CreateBaseAtCreature() failed, creatureInfo is missing!");
        return false;
    }

    SetDisplayId(creature->GetDisplayId());

    if (CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cinfo->family))
        SetName(cFamily->Name[sObjectMgr->GetDBCLocaleIndex()]);
    else
        SetName(creature->GetNameForLocaleIdx(sObjectMgr->GetDBCLocaleIndex()));

    return true;
}

bool Pet::CreateBaseAtCreatureInfo(CreatureTemplate const* cinfo, Unit* owner)
{
    if (!CreateBaseAtTamed(cinfo, owner->GetMap(), owner->GetPhaseMask()))
        return false;

    if (CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cinfo->family))
        SetName(cFamily->Name[sObjectMgr->GetDBCLocaleIndex()]);

    Relocate(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation());

    return true;
}

bool Pet::CreateBaseAtTamed(CreatureTemplate const* cinfo, Map* map, uint32 phaseMask)
{
    TC_LOG_DEBUG("entities.pet", "Pet::CreateBaseForTamed");
    uint32 guid = map->GenerateLowGuid<HighGuid::Pet>();
    uint32 petId = sObjectMgr->GeneratePetNumber();
    if (!Create(guid, map, phaseMask, cinfo->Entry, petId))
        return false;

    SetPowerType(POWER_FOCUS);
    SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
    SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, 0);
    SetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE, uint32(sObjectMgr->GetXPForLevel(GetLevel()+1)*PET_XP_FACTOR));
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_NONE);

    if (cinfo->type == CREATURE_TYPE_BEAST)
    {
        SetClass(CLASS_WARRIOR);
        SetGender(GENDER_NONE);
        SetFieldPowerType(POWER_FOCUS);
        SetSheath(SHEATH_STATE_MELEE);
        SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, 2, UNIT_CAN_BE_RENAMED | UNIT_CAN_BE_ABANDONED);
    }

    return true;
}

/// @todo Move stat mods code to pet passive auras
bool Guardian::InitStatsForLevel(uint8 petlevel)
{
    CreatureTemplate const* cinfo = GetCreatureTemplate();
    ASSERT(cinfo);

    SetLevel(petlevel);

    //Determine pet type
    PetType petType = MAX_PET_TYPE;
    if (IsPet() && GetOwner()->GetTypeId() == TYPEID_PLAYER)
    {
        switch (GetOwner()->GetClass())
        {
            case CLASS_WARLOCK:
            case CLASS_SHAMAN:
            case CLASS_DEATH_KNIGHT:
            case CLASS_MAGE:
                petType = SUMMON_PET;
                break;
            case CLASS_HUNTER:
                petType = HUNTER_PET;
                m_unitTypeMask |= UNIT_MASK_HUNTER_PET;
                break;
            default:
                TC_LOG_ERROR("entities.pet", "Unknown type pet %u is summoned by player class %u", GetEntry(), GetOwner()->GetClass());
                break;
        }
    }

    uint32 creature_ID = (petType == HUNTER_PET) ? 1 : cinfo->Entry;

    SetMeleeDamageSchool(SpellSchools(cinfo->dmgschool));

    if (!IsPet())
        SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, float(petlevel*50));

    SetAttackTime(BASE_ATTACK, GetBaseAttackTimer());
    SetAttackTime(OFF_ATTACK, GetBaseAttackTimer());
    //SetAttackTime(RANGED_ATTACK, attackTime); No one summon has ranged attack, right? Never saw it in the sniffs

    SetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED, 1.0f);
    SetFloatValue(UNIT_FIELD_MOD_SPELL_HASTE, 1.0f);

    //scale
    CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cinfo->family);
    if (cFamily && cFamily->minScale > 0.0f && petType == HUNTER_PET)
    {
        float scale;
        if (GetLevel() >= cFamily->maxScaleLevel)
            scale = cFamily->maxScale;
        else if (GetLevel() <= cFamily->minScaleLevel)
            scale = cFamily->minScale;
        else
            scale = cFamily->minScale + float(GetLevel() - cFamily->minScaleLevel) / cFamily->maxScaleLevel * (cFamily->maxScale - cFamily->minScale);

        SetObjectScale(scale);
    }

    // Resistance
    for (uint8 i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
        SetModifierValue(UnitMods(UNIT_MOD_RESISTANCE_START + i), BASE_VALUE, float(cinfo->resistance[i]));

    //health, mana, armor and resistance
    PetLevelInfo const* pInfo = sObjectMgr->GetPetLevelInfo(creature_ID, petlevel);
    if (pInfo)                                      // exist in DB
    {
        SetCreateHealth(pInfo->health);
        if (petType != HUNTER_PET) //hunter pet use focus
            SetCreateMana(pInfo->mana);

        if (pInfo->armor > 0 && !IsPet())
            SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, float(pInfo->armor));

        for (uint8 stat = 0; stat < MAX_STATS; ++stat)
            SetCreateStat(Stats(stat), float(pInfo->stats[stat]));
    }
    else                                            // not exist in DB, use some default fake data
    {
        // remove elite bonuses included in DB values
        CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(petlevel, cinfo->unit_class);
        SetCreateHealth(stats->BaseHealth[cinfo->expansion]);
        SetCreateMana(stats->BaseMana);

        SetCreateStat(STAT_STRENGTH, 22);
        SetCreateStat(STAT_AGILITY, 22);
        SetCreateStat(STAT_STAMINA, 25);
        SetCreateStat(STAT_INTELLECT, 28);
        SetCreateStat(STAT_SPIRIT, 27);
    }

    if (GetOwner()->ToPlayer())
    {
        if (auto scaling = sGtSpellScalingStore.LookupEntry((GetOwner()->GetClass() - 1) * 100 + GetOwner()->GetLevel() - 1))
        {
            float min = scaling->value;
            float max = scaling->value;
            float coeff = 1.0f;

            if (auto petScaling = sObjectMgr->GetPetScaling(GetEntry()))
                coeff = petScaling->BaseDamageCoeff;

            SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, min * coeff);
            SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, max * coeff);
            SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, min * coeff);
            SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, max * coeff);
        }
    }

    SetBonusDamage(0);
    switch (petType)
    {
        case SUMMON_PET:
        {
            switch (GetOwner()->GetClass())
            {
                case CLASS_WARLOCK:
                    if (GetOwner()->HasAura(77219) && !HasAura(115556)) // Master demonologist
                        CastSpell(this, 115556, true);
                    GetOwner()->RemoveAurasDueToSpell(108503);  // Grimoire of Sacrifice
                    break;
                case CLASS_MAGE:
                    if (GetOwner()->HasAura(146976))    // Glyph of the Unbound Elemental.
                        CastSpell(this, 147358, true);
                    break;
            }
            break;
        }
        case HUNTER_PET:
        {
            SetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE, uint32(sObjectMgr->GetXPForLevel(petlevel)*PET_XP_FACTOR));
            if (GetOwner()->HasSpell(53253) && !HasAura(53397)) // 53397 
                CastSpell(this, 53397, true);
            break;
        }
        default:
        {
            switch (GetEntry())
            {
                case 1964: //force of nature
                {
                    if (!pInfo)
                        SetCreateHealth(30 + 30*petlevel);
                    float bonusDmg = GetOwner()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_NATURE) * 0.15f;
                    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, float(petlevel * 2.5f - (petlevel / 2) + bonusDmg));
                    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, float(petlevel * 2.5f + (petlevel / 2) + bonusDmg));
                    break;
                }
                case 19833: //Snake Trap - Venomous Snake
                {
                    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, float((petlevel / 2) - 25));
                    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, float((petlevel / 2) - 18));
                    break;
                }
                case 19921: //Snake Trap - Viper
                {
                    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, float(petlevel / 2 - 10));
                    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, float(petlevel / 2));
                    break;
                }
                case 29264: // Feral Spirit
                {
                    if (!pInfo)
                        SetCreateHealth(30*petlevel);

                    SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, float(GetOwner()->GetArmor()) * 0.35f);  // Bonus Armor (35% of player armor)
                    SetModifierValue(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(GetOwner()->GetStat(STAT_STAMINA)) * 0.3f);  // Bonus Stamina (30% of player stamina)
                    if (!HasAura(58877))//prevent apply twice for the 2 wolves
                        AddAura(58877, this);//Spirit Hunt, passive, Spirit Wolves' attacks heal them and their master for 150% of damage done.
                    break;
                }
                case 31216: // Mirror Image
                {
                    SetDisplayId(GetOwner()->GetDisplayId());
                    if (!pInfo)
                    {
                        SetCreateMana(28 + 30*petlevel);
                        SetCreateHealth(28 + 10*petlevel);
                    }
                    break;
                }
            }
            break;
        }
    }

    // Glyph of the Skeleton
    if (GetEntry() == 26125 && GetOwner()->HasAura(146652) && !HasAura(147157))
        CastSpell(this, 147157, true);

    UpdateAllStats();

    SetFullHealth();
    SetPower(POWER_MANA, GetMaxPower(POWER_MANA));
    UpdateCastingSpeed();
    UpdateAttackSpeed(BASE_ATTACK);
    UpdateAttackSpeed(OFF_ATTACK);
    return true;
}

bool Pet::HaveInDiet(ItemTemplate const* item) const
{
    if (!item->FoodType)
        return false;

    CreatureTemplate const* cInfo = GetCreatureTemplate();
    if (!cInfo)
        return false;

    CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cInfo->family);
    if (!cFamily)
        return false;

    uint32 diet = cFamily->petFoodMask;
    uint32 FoodMask = 1 << (item->FoodType-1);
    return diet & FoodMask;
}

uint32 Pet::GetCurrentFoodBenefitLevel(uint32 itemlevel) const
{
    // -5 or greater food level
    if (GetLevel() <= itemlevel + 5)                         //possible to feed level 60 pet with level 55 level food for full effect
        return 35000;
    // -10..-6
    else if (GetLevel() <= itemlevel + 10)                   //pure guess, but sounds good
        return 17000;
    // -14..-11
    else if (GetLevel() <= itemlevel + 14)                   //level 55 food gets green on 70, makes sense to me
        return 8000;
    // -15 or less
    else
        return 0;                                           //food too low level
}

void Pet::_LoadAuras(uint32 timediff)
{
    TC_LOG_DEBUG("entities.pet", "Loading auras for pet %u", GetGUID().GetCounter());

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_AURAS);
    stmt->setUInt32(0, m_charmInfo->GetPetNumber());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_AURA_EFFECTS);
    stmt->setUInt32(0, m_charmInfo->GetPetNumber());
    PreparedQueryResult effectResult = CharacterDatabase.Query(stmt);

    struct EffectData
    {
        uint8 Slot;
        uint8 Index;
        int32 BaseAmount;
        int32 Amount;
    };

    std::vector<EffectData> effects;

    if (effectResult)
    {
        do
        {
            Field* fields = effectResult->Fetch();

            effects.emplace_back();
            auto& effect = effects.back();
            effect.Slot = fields[0].GetUInt8();
            effect.Index = fields[1].GetUInt8();
            effect.BaseAmount = fields[2].GetInt32();
            effect.Amount = fields[3].GetInt32();
        } while (effectResult->NextRow());
    }

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            std::array<int32, MAX_SPELL_EFFECTS> damage;
            damage.fill(0);
            std::array<int32, MAX_SPELL_EFFECTS> baseDamage;
            baseDamage.fill(0);
            ObjectGuid casterGuid(fields[0].GetUInt64());
            // null guid stored - pet is the caster of the spell - see Pet::_SaveAuras
            if (!casterGuid)
                casterGuid = GetGUID();
            uint32 spellid = fields[1].GetUInt32();
            uint32 effMask = fields[2].GetUInt32();
            uint32 recalculatemask = fields[3].GetUInt32();
            uint8 stackcount = fields[4].GetUInt8();
            int32 maxduration = fields[5].GetInt32();
            int32 remaintime = fields[6].GetInt32();
            uint8 remaincharges = fields[7].GetUInt8();
            uint8 slot = fields[8].GetUInt8();

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
            if (!spellInfo)
            {
                TC_LOG_ERROR("entities.pet", "Unknown aura (spellid %u), ignore.", spellid);
                continue;
            }

            // negative effects should continue counting down after logout
            if (remaintime != -1)
            {
                if (remaintime / IN_MILLISECONDS <= int32(timediff))
                    continue;

                remaintime -= timediff*IN_MILLISECONDS;
            }

            // prevent wrong values of remaincharges
            if (spellInfo->ProcCharges)
            {
                // we have no control over the order of applying auras and modifiers allow auras
                // to have more charges than value in SpellInfo
                if (remaincharges <= 0/* || remaincharges > spellproto->procCharges*/)
                    remaincharges = spellInfo->ProcCharges;
            }
            else
                remaincharges = 0;

            for (auto&& it : effects)
            {
                if (it.Slot == slot && it.Index < MAX_SPELL_EFFECTS)
                {
                    damage[it.Index] = it.Amount;
                    baseDamage[it.Index] = it.BaseAmount;
                }
            }

            if (Aura* aura = Aura::TryCreate(spellInfo, effMask, this, nullptr, &baseDamage[0], nullptr, casterGuid))
            {
                if (!aura->CanBeSaved())
                {
                    aura->Remove();
                    continue;
                }

                aura->SetLoadedState(maxduration, remaintime, remaincharges, stackcount, recalculatemask, &damage[0]);
                aura->ApplyForTargets();
                TC_LOG_INFO("entities.pet", "Added aura spellid %u, effectmask %u", spellInfo->Id, effMask);
            }
        } while (result->NextRow());
    }
}

void Pet::_LoadSpells()
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_SPELL);
    stmt->setUInt32(0, m_charmInfo->GetPetNumber());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 spellId = fields[0].GetUInt32();
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            auto itr = m_spells.find(spellId);
            if (itr == m_spells.end() || !spellInfo)
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_SPELL_BY_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetPetNumber());
                stmt->setUInt32(1, spellId);
                CharacterDatabase.Execute(stmt);
                continue;
            }

            itr->second.active = ActiveStates(fields[1].GetUInt8());
            itr->second.state  = PETSPELL_UNCHANGED;
            if (itr->second.active == ACT_ENABLED)
                ToggleAutocast(spellInfo, true);
            else if (itr->second.active == ACT_DISABLED)
                ToggleAutocast(spellInfo, false);
        } while (result->NextRow());
    }
}

void Pet::_SaveSpells(CharacterDatabaseTransaction trans)
{
    for (PetSpellMap::iterator itr = m_spells.begin(), next = m_spells.begin(); itr != m_spells.end(); itr = next)
    {
        ++next;

        CharacterDatabasePreparedStatement* stmt;

        switch (itr->second.state)
        {
            case PETSPELL_REMOVED:
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_SPELL_BY_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetPetNumber());
                stmt->setUInt32(1, itr->first);
                trans->Append(stmt);
                m_spells.erase(itr);
                continue;
            case PETSPELL_CHANGED:
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_SPELL_BY_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetPetNumber());
                stmt->setUInt32(1, itr->first);
                trans->Append(stmt);

                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PET_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetPetNumber());
                stmt->setUInt32(1, itr->first);
                stmt->setUInt8(2, itr->second.active);
                trans->Append(stmt);
                break;
            case PETSPELL_NEW:
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PET_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetPetNumber());
                stmt->setUInt32(1, itr->first);
                stmt->setUInt8(2, itr->second.active);
                trans->Append(stmt);
                break;
            case PETSPELL_UNCHANGED:
                continue;
        }
        itr->second.state = PETSPELL_UNCHANGED;
    }
}

void Pet::_SaveAuras(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_AURAS);
    stmt->setUInt32(0, m_charmInfo->GetPetNumber());
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_AURA_EFFECT);
    stmt->setUInt32(0, m_charmInfo->GetPetNumber());
    trans->Append(stmt);

    for (AuraMap::const_iterator itr = m_ownedAuras.begin(); itr != m_ownedAuras.end(); ++itr)
    {
        if (!itr->second->CanBeSaved())
            continue;

        Aura* aura = itr->second;
        auto aurApp = GetAuraApplication(aura->GetId(), aura->GetCasterGUID(), aura->GetCastItemGUID());
        if (!aurApp)
            continue;

        uint32 effMask = 0;
        uint32 recalculateMask = 0;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (auto effect = aura->GetEffect(i))
            {
                uint32 index = 0;
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PET_AURA_EFFECT);
                stmt->setUInt32(index++, m_charmInfo->GetPetNumber());
                stmt->setUInt8(index++, aurApp->GetSlot());
                stmt->setUInt8(index++, i);
                stmt->setInt32(index++, effect->GetBaseAmount());
                stmt->setInt32(index++, effect->GetAmount());
                trans->Append(stmt);

                effMask |= 1 << i;
                if (effect->CanBeRecalculated())
                    recalculateMask |= 1 << i;
            }
        }

        // don't save guid of caster in case we are caster of the spell - guid for pet is generated every pet load, so it won't match saved guid anyways
        uint64 casterGUID = (itr->second->GetCasterGUID() == GetGUID()) ? 0 : itr->second->GetCasterGUID();

        uint8 index = 0;
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PET_AURA);
        stmt->setUInt32(index++, m_charmInfo->GetPetNumber());
        stmt->setUInt64(index++, casterGUID);
        stmt->setUInt64(index++, itr->second->GetCastItemGUID());
        stmt->setUInt32(index++, itr->second->GetId());
        stmt->setUInt32(index++, effMask);
        stmt->setUInt32(index++, recalculateMask);
        stmt->setUInt8(index++, itr->second->GetStackAmount());
        stmt->setInt32(index++, itr->second->GetMaxDuration());
        stmt->setInt32(index++, itr->second->GetDuration());
        stmt->setUInt8(index++, itr->second->GetCharges());
        stmt->setUInt8(index++, aurApp->GetSlot());
        trans->Append(stmt);
    }
}

bool Pet::AddSpell(uint32 spellId, ActiveStates active /*= ACT_DECIDE*/)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("entities.pet", "Pet::AddSpell: Non-existed in SpellStore spell #%u request.", spellId);
        return false;
    }

    bool alreadyHas = false;
    PetSpellMap::iterator itr = m_spells.find(spellId);
    if (itr != m_spells.end())
    {
        if (itr->second.state != PETSPELL_REMOVED)
            return false;

        alreadyHas = true;
        m_spells.erase(itr);
    }

    PetSpell newspell;

    if (active == ACT_DECIDE)                               // active was not used before, so we save it's autocast/passive state here
    {
        if (spellInfo->IsAutocastable())
            newspell.active = ACT_DISABLED;
        else
            newspell.active = ACT_PASSIVE;

        if (spellId == 31707 || spellId == 3110 || spellId == 115746) // Waterbolt, Firebolt, Felbolt
            newspell.active = ACT_ENABLED;
    }
    else
        newspell.active = active;

    if (spellInfo->IsPassive())
        newspell.state = PETSPELL_UNCHANGED;
    else if (alreadyHas)
        newspell.state = PETSPELL_CHANGED;
    else
        newspell.state = PETSPELL_NEW;

    if (spellInfo->IsRanked())
    {
        for (auto&& itr2 : m_spells)
        {
            SpellInfo const* oldRankSpellInfo = sSpellMgr->GetSpellInfo(itr2.first);

            if (!oldRankSpellInfo)
                continue;

            if (spellInfo->IsDifferentRankOf(oldRankSpellInfo))
            {
                // replace by new high rank
                if (spellInfo->IsHighRankOf(oldRankSpellInfo))
                {
                    newspell.active = itr2.second.active;

                    if (newspell.active == ACT_ENABLED)
                        ToggleAutocast(oldRankSpellInfo, false);

                    UnlearnSpell(itr2.first, false, false);
                    break;
                }
                // ignore new lesser rank
                else
                    return false;
            }
        }
    }

    m_spells[spellId] = newspell;

    if (spellInfo->IsPassive() && (!spellInfo->CasterAuraState || HasAuraState(AuraStateType(spellInfo->CasterAuraState))))
        CastSpell(this, spellId, true);
    else
        m_charmInfo->AddSpellToActionBar(spellInfo, newspell.active);

    if (newspell.active == ACT_ENABLED)
        ToggleAutocast(spellInfo, true);

    return true;
}

bool Pet::LearnSpell(uint32 spell_id)
{
    // prevent duplicated entires in spell book
    if (!AddSpell(spell_id))
        return false;

    if (!m_loading)
    {
        WorldPacket data(SMSG_PET_LEARNED_SPELL, 4);
        data.WriteBits(1, 22);
        data << uint32(spell_id);
        GetOwner()->GetSession()->SendPacket(&data);
        GetOwner()->PetSpellInitialize();
    }
    return true;
}

void Pet::InitSpellsForLevel()
{
    uint8 level = GetLevel();

    if (PetLevelupSpellSet const* levelupSpells = GetCreatureTemplate()->family ? sSpellMgr->GetPetSpellList(GetCreatureTemplate()->family) : NULL)
    {
        // PetLevelupSpellSet ordered by levels, process in reversed order
        for (PetLevelupSpellSet::const_reverse_iterator itr = levelupSpells->rbegin(); itr != levelupSpells->rend(); ++itr)
        {
            // will called first if level down
            if (itr->first > level)
                UnlearnSpell(itr->second, true);                 // will learn prev rank if any
            // will called if level up
            else
                LearnSpell(itr->second);                        // will unlearn prev rank if any
        }
    }
}

bool Pet::UnlearnSpell(uint32 spellId, bool learnPrev, bool cleaActionBar)
{
    if (RemoveSpell(spellId, learnPrev, cleaActionBar))
    {
        if (!m_loading)
        {
            WorldPacket data(SMSG_PET_REMOVED_SPELL, 4);
            data.WriteBits(1, 22);
            data << uint32(spellId);
            GetOwner()->GetSession()->SendPacket(&data);
        }
        return true;
    }
    return false;
}

bool Pet::RemoveSpell(uint32 spellId, bool learnPrev, bool clearActionBar)
{
    PetSpellMap::iterator itr = m_spells.find(spellId);
    if (itr == m_spells.end())
        return false;

    if (itr->second.state == PETSPELL_NEW)
        m_spells.erase(itr);
    else
        itr->second.state = PETSPELL_REMOVED;

    m_autospells.erase(std::remove_if(m_autospells.begin(), m_autospells.end(), [=](uint32 autospell) { return autospell == spellId; }), m_autospells.end());

    RemoveAurasDueToSpell(spellId);

    if (learnPrev)
    {
        if (uint32 prevId = sSpellMgr->GetPrevSpellInChain(spellId))
            LearnSpell(prevId);
        else
            learnPrev = false;
    }

    // if remove last rank or non-ranked then update action bar at server and client if need
    if (clearActionBar && !learnPrev && m_charmInfo->RemoveSpellFromActionBar(spellId))
    {
        if (!m_loading)
        {
            // need update action bar for last removed rank
            if (Unit* owner = GetOwner())
                if (owner->GetTypeId() == TYPEID_PLAYER)
                    owner->ToPlayer()->PetSpellInitialize();
        }
    }

    return true;
}

void Pet::CleanupActionBar()
{
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
        if (UnitActionBarEntry const* ab = m_charmInfo->GetActionBarEntry(i))
            if (ab->GetAction() && ab->IsActionBarForSpell())
            {
                if (!HasSpell(ab->GetAction()))
                    m_charmInfo->SetActionBar(i, 0, ACT_PASSIVE);
                else if (ab->GetType() == ACT_ENABLED)
                {
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(ab->GetAction()))
                        ToggleAutocast(spellInfo, true);
                }
            }
}

void Pet::InitPetCreateSpells()
{
    m_charmInfo->InitPetActionBar();
    m_spells.clear();

    InitSpellsForLevel();

    CastPetAuras(false);
}

void Pet::resetTalentsForAllPetsOf(Player* owner, Pet* onlinePet /*= NULL*/)
{
    // not need after this call
    if (owner->HasAtLoginFlag(AT_LOGIN_RESET_PET_TALENTS))
        owner->RemoveAtLoginFlag(AT_LOGIN_RESET_PET_TALENTS, true);

    // now need only reset for offline pets (all pets except online case)
    uint32 exceptPetNumber = onlinePet ? onlinePet->GetCharmInfo()->GetPetNumber() : 0;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_PET);
    stmt->setUInt32(0, owner->GetGUID().GetCounter());
    stmt->setUInt32(1, exceptPetNumber);
    PreparedQueryResult resultPets = CharacterDatabase.Query(stmt);

    // no offline pets
    if (!resultPets)
        return;

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_SPELL_LIST);
    stmt->setUInt32(0, owner->GetGUID().GetCounter());
    stmt->setUInt32(1, exceptPetNumber);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return;

    bool need_comma = false;
    std::ostringstream ss;
    ss << "DELETE FROM pet_spell WHERE guid IN (";

    do
    {
        Field* fields = resultPets->Fetch();

        uint32 id = fields[0].GetUInt32();

        if (need_comma)
            ss << ',';

        ss << id;

        need_comma = true;
    } while (resultPets->NextRow());

    ss << ") AND spell IN (";

    bool need_execute = false;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell = fields[0].GetUInt32();

        if (!GetTalentSpellCost(spell))
            continue;

        if (need_execute)
            ss << ',';

        ss << spell;

        need_execute = true;
    }
    while (result->NextRow());

    if (!need_execute)
        return;

    ss << ')';

    CharacterDatabase.Execute(ss.str().c_str());
}

void Pet::InitTalentForLevel()
{
    uint8 level = GetLevel();
    uint32 talentPointsForLevel = GetMaxTalentPointsForLevel(level);

    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;
}

uint8 Pet::GetMaxTalentPointsForLevel(uint8 level)
{
    uint8 points = (level >= 20) ? ((level - 16) / 4) : 0;
    // Mod points from owner SPELL_AURA_MOD_PET_TALENT_POINTS
    if (Unit* owner = GetOwner())
        points+=owner->GetTotalAuraModifier(SPELL_AURA_MOD_PET_TALENT_POINTS);
    return points;
}

void Pet::ToggleAutocast(SpellInfo const* spellInfo, bool apply)
{
    if (!spellInfo->IsAutocastable())
        return;

    uint32 spellid = spellInfo->Id;

    PetSpellMap::iterator itr = m_spells.find(spellid);
    if (itr == m_spells.end())
        return;

    uint32 i;

    if (apply)
    {
        for (i = 0; i < m_autospells.size() && m_autospells[i] != spellid; ++i);    // just search

        if (i == m_autospells.size())
        {
            m_autospells.push_back(spellid);

            if (itr->second.active != ACT_ENABLED)
            {
                itr->second.active = ACT_ENABLED;
                if (itr->second.state != PETSPELL_NEW)
                    itr->second.state = PETSPELL_CHANGED;
            }
        }
    }
    else
    {
        AutoSpellList::iterator itr2 = m_autospells.begin();
        for (i = 0; i < m_autospells.size() && m_autospells[i] != spellid; ++i, ++itr2);    // just search

        if (i < m_autospells.size())
        {
            m_autospells.erase(itr2);
            if (itr->second.active != ACT_DISABLED)
            {
                itr->second.active = ACT_DISABLED;
                if (itr->second.state != PETSPELL_NEW)
                    itr->second.state = PETSPELL_CHANGED;
            }
        }
    }
}

bool Pet::IsPermanentPetFor(Player* owner) const
{
    switch (getPetType())
    {
        case SUMMON_PET:
            switch (owner->GetClass())
            {
                case CLASS_WARLOCK:
                    return GetCreatureTemplate()->type == CREATURE_TYPE_DEMON;
                case CLASS_DEATH_KNIGHT:
                    return GetCreatureTemplate()->type == CREATURE_TYPE_UNDEAD;
                case CLASS_MAGE:
                    return GetCreatureTemplate()->type == CREATURE_TYPE_ELEMENTAL;
                default:
                    return false;
            }
        case HUNTER_PET:
            return true;
        default:
            return false;
    }
}

bool Pet::Create(uint32 guidlow, Map* map, uint32 phaseMask, uint32 Entry, uint32 petId)
{
    ASSERT(map);
    SetMap(map);

    SetPhaseMask(phaseMask, false);
    Object::_Create(guidlow, petId, HighGuid::Pet);

    m_spawnId = guidlow;
    m_originalEntry = Entry;

    if (!InitEntry(Entry))
        return false;

    SetSpeed(MOVE_WALK, 1.0f);
    SetSpeed(MOVE_RUN, Pet::BaseRunSpeedRate);

    SetSheath(SHEATH_STATE_MELEE);

    return true;
}

bool Pet::HasSpell(uint32 spell) const
{
    auto itr = m_spells.find(spell);
    return itr != m_spells.end() && itr->second.state != PETSPELL_REMOVED;
}

void Pet::CastPetAuras(bool current)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!IsPermanentPetFor(owner->ToPlayer()))
        return;

    if (IsTemporary())
        return;

    for (PetAuraSet::const_iterator itr = owner->m_petAuras.begin(); itr != owner->m_petAuras.end();)
    {
        PetAura const* pa = *itr;
        ++itr;

        if (!current && pa->IsRemovedOnChangePet())
            owner->RemovePetAura(pa);
        else
            CastPetAura(pa);
    }
}

void Pet::CastPetAura(PetAura const* aura)
{
    uint32 auraId = aura->GetAura(GetEntry());
    if (!auraId)
        return;

    if (auraId == 35696)                                      // Demonic Knowledge
    {
        int32 basePoints = CalculatePct(aura->GetDamage(), GetStat(STAT_STAMINA) + GetStat(STAT_INTELLECT));
        CastCustomSpell(this, auraId, &basePoints, NULL, NULL, true);
    }
    else
        CastSpell(this, auraId, true);
}

bool Pet::IsPetAura(Aura const* aura)
{
    Unit* owner = GetOwner();

    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return false;

    // if the owner has that pet aura, return true
    for (PetAuraSet::const_iterator itr = owner->m_petAuras.begin(); itr != owner->m_petAuras.end(); ++itr)
    {
        if ((*itr)->GetAura(GetEntry()) == aura->GetId())
            return true;
    }
    return false;
}

void Pet::learnSpellHighRank(uint32 spellid)
{
    LearnSpell(spellid);

    if (uint32 next = sSpellMgr->GetNextSpellInChain(spellid))
        learnSpellHighRank(next);
}

void Pet::SynchronizeLevelWithOwner()
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    switch (getPetType())
    {
        // always same level
        case HUNTER_PET:
        case SUMMON_PET:
            GivePetLevel(owner->GetLevel());
            break;
        default:
            break;
    }
}

void Pet::SetSpecialization(uint16 spec)
{
    if (GetSpecializationId() == spec)
        return;

    // remove all the old spec's specalization spells, set the new spec, then add the new spec's spells
    // clearActionBars is false because we'll be updating the pet actionbar later so we don't have to do it now
    LearnSpecializationSpells(false);
    if (spec && !sChrSpecializationStore.LookupEntry(spec))
    {
        SetSpecializationId(0);
        return;
    }

    SetSpecializationId(spec);
    LearnSpecializationSpells(true);

    if (!m_loading)
    {
        // resend SMSG_PET_SPELLS_MESSAGE to remove old specialization spells from the pet action bar
        CleanupActionBar();
        GetOwner()->PetSpellInitialize();
    }

    WorldPacket data(SMSG_SET_PET_SPECIALIZATION, 2);
    data << uint16(GetSpecializationId());
    GetOwner()->SendDirectMessage(&data);
}

void Pet::LearnSpecializationSpells(bool learn, bool clearActionBar)
{
    auto const* spells = dbc::GetSpecializetionSpells(GetSpecializationId());
    if (!spells)
        return;
    for (auto&& spell : *spells)
    {
        if (learn)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell);
            if (!spellInfo || spellInfo->SpellLevel > GetLevel())
                continue;

            LearnSpell(spell);
        }
        else
            UnlearnSpell(spell, false, clearActionBar);
    }
}

void Pet::ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs)
{
        time_t curTime = time(NULL);
        ObjectGuid guid = GetGUID();
        uint32 count = 0;

        WorldPacket data(SMSG_SPELL_COOLDOWN, 9 + 3 + m_spells.size() * 8);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[6]);
        data.WriteBit(1); // Missing flags
        data.WriteBit(guid[7]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[5]);
        size_t bitpos = data.bitwpos();
        data.WriteBits(0, 21);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[4]);
        data.FlushBits();

    for (auto&& itr : m_spells)
    {
        uint32 unSpellId = itr.first;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(unSpellId);
        if (!spellInfo)
        {
            ASSERT(spellInfo);
            continue;
        }

        // Not send cooldown for this spells
        if (spellInfo->IsCooldownStartedOnEvent())
            continue;

        if (!(spellInfo->PreventionType & SPELL_PREVENTION_TYPE_SILENCE))
            continue;

        if (idSchoolMask & spellInfo->GetSchoolMask())
        {
            // Don't send but call ProhibitSpell anyway
            if (GetSpellHistory()->GetCooldownDelay(unSpellId) < unTimeMs)
            {
                data << uint32(unSpellId);
                data << uint32(unTimeMs);                       // in m.secs
                count++;
            }
            GetSpellHistory()->ProhibitSpell(unSpellId, Milliseconds(unTimeMs));
        }
    }

    data.PutBits(bitpos, count, 21);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);

    if (Player* owner = GetOwner())
        owner->GetSession()->SendPacket(&data);
}

Player* Pet::GetOwner() const
{
    return Minion::GetOwner()->ToPlayer();
}

void Pet::SetDisplayId(uint32 modelId)
{
    Guardian::SetDisplayId(modelId);

    if (!isControlled())
        return;

    if (Unit* owner = GetOwner())
        if (Player* player = owner->ToPlayer())
            if (player->GetGroup())
                player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_MODEL_ID);
}

void Pet::SetObjectScale(float scale)
{
    Unit::SetObjectScale(scale);

    SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, scale * DEFAULT_PLAYER_BOUNDING_RADIUS);
    SetFloatValue(UNIT_FIELD_COMBAT_REACH, scale * DEFAULT_PLAYER_COMBAT_REACH);
}

void Guardian::Regenerate()
{
    Unit* owner = GetOwner();
    if (!owner || !owner->ToPlayer())
        return Creature::Regenerate();

    m_regenTimerCount += m_regenTimer;
    Regenerate(GetPowerType());
    m_regenTimer = 0;

    if (m_regenTimerCount >= CREATURE_REGEN_INTERVAL)
    {
        bool bInCombat = IsInCombat() && (!GetVictim() ||                           // if IsInCombat() is true and this has no victim
            !GetVictim()->GetCharmerOrOwnerPlayerOrPlayerItself() ||                // or the victim/owner/charmer is not a player
            !GetVictim()->GetCharmerOrOwnerPlayerOrPlayerItself()->IsGameMaster()); // or the victim/owner/charmer is not a GameMaster

        if (!IsInEvadeMode() && (!bInCombat || IsPolymorphed())) // regenerate health if not in combat or if polymorphed
            RegenerateHealth();

        m_regenTimerCount = 0;
    }
}

void Guardian::Regenerate(Powers power)
{
    uint32 powerIndex = GetPowerIndex(power);
    if (powerIndex == MAX_POWERS)
        return;

    uint32 maxValue = GetMaxPower(power);
    if (!maxValue)
        return;

    Unit* owner = GetOwner();

    float addvalue = 0.0f;
    switch (power)
    {
        case POWER_MANA:
            if (IsInCombat())
                addvalue = owner->GetFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER) * (0.001f * m_regenTimer);
            else
                addvalue = owner->GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER) * (0.001f * m_regenTimer);
            break;
        case POWER_FOCUS:
            addvalue = owner->GetPowerRegen(power) / IN_MILLISECONDS * m_regenTimer * 1.25f;    // 125% of hunter
            break;
        case POWER_ENERGY:
            addvalue = GetBasePowerRegen(power) / IN_MILLISECONDS * m_regenTimer / owner->GetFloatValue(UNIT_FIELD_MOD_HASTE_REGEN);
            break;
        default:
            break;
    }

    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT))
        if (Powers(it->GetMiscValue()) == power)
            AddPct(addvalue, it->GetAmount());

    addvalue += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, power) * m_regenTimer / (5 * IN_MILLISECONDS);

    addvalue += m_powerFraction;
    uint32 curValue = GetPower(power);
    uint32 integerValue = uint32(fabs(addvalue));

    if (addvalue < 0.0f)
    {
        if (curValue == 0)
            return;
    }
    else if (addvalue > 0.0f)
    {
        if (curValue == maxValue)
            return;
    }
    else
        return;

    if (addvalue < 0.0f)
    {
        if (curValue > integerValue)
        {
            curValue -= integerValue;
            m_powerFraction = addvalue + integerValue;
        }
        else
        {
            curValue = 0;
            m_powerFraction = 0;
        }
    }
    else
    {
        curValue += integerValue;

        if (curValue > maxValue)
        {
            curValue = maxValue;
            m_powerFraction = 0;
        }
        else
            m_powerFraction = addvalue - integerValue;
    }

    if (m_regenTimerCount >= CREATURE_REGEN_INTERVAL)
        SetPower(power, curValue);
    else
        SetInt32Value(UNIT_FIELD_POWER + powerIndex, curValue);
}

bool TempSummon::IsMinorGuardian() const
{
    // Probably we need to determine is spell used for summon...
    if (!GetCreatorGUID().IsPlayer())
        return false;

    if (IsPet())
        return false;
    if (HasAura(138130))    // Storm, Earth and Fire
        return false;
    if (!m_Properties)
        return true;
    if (m_Properties->Category == SUMMON_CATEGORY_PET || m_Properties->Type == SUMMON_TYPE_PET)
        return false;
    if (m_Properties->Type != SUMMON_TYPE_GUARDIAN && m_Properties->Type != SUMMON_TYPE_GUARDIAN2)
        return true;
    return GetEntry() != 28524; // Bloodworm...
}
