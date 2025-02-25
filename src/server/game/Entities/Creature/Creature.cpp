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

#include "Creature.h"
#include "BattlegroundMgr.h"
#include "CellImpl.h"
#include "Common.h"
#include "CreatureAI.h"
#include "CreatureAISelector.h"
#include "CreatureGroups.h"
#include "DatabaseEnv.h"
#include "Formulas.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "GroupMgr.h"
#include "InstanceScript.h"
#include "IteratorPair.h"
#include "Log.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvPMgr.h"
#include "Pet.h"
#include "Player.h"
#include "PoolMgr.h"
#include "QuestDef.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
#include "Util.h"
#include "Vehicle.h"
#include "WaypointMovementGenerator.h"
#include "World.h"
#include "WorldPacket.h"
#include "BattlePetSpawnMgr.h"
#include "Transport.h"
#include "Define.h"

CreatureMovementData::CreatureMovementData() : Ground(CreatureGroundMovementType::Run), Flight(CreatureFlightMovementType::None), Swim(true), Rooted(false), Chase(CreatureChaseMovementType::Run),
Random(CreatureRandomMovementType::Walk), InteractionPauseTimer(sWorld->getIntConfig(CONFIG_CREATURE_STOP_FOR_PLAYER)) { }

std::string CreatureMovementData::ToString() const
{
    char const* const GroundStates[] = { "None", "Run", "Hover" };
    char const* const FlightStates[] = { "None", "DisableGravity", "CanFly" };
    char const* const ChaseStates[]  = { "Run", "CanWalk", "AlwaysWalk" };
    char const* const RandomStates[] = { "Walk", "CanRun", "AlwaysRun" };

    std::ostringstream str;
    str << std::boolalpha
        << "Ground: " << GroundStates[AsUnderlyingType(Ground)]
        << ", Swim: " << Swim
        << ", Flight: " << FlightStates[AsUnderlyingType(Flight)]
        << ", Chase: " << ChaseStates[AsUnderlyingType(Chase)]
        << ", Random: " << RandomStates[AsUnderlyingType(Random)];
    if (Rooted)
        str << ", Rooted";
    str << ", InteractionPauseTimer: " << InteractionPauseTimer;

    return str.str();
}

TrainerSpell const* TrainerSpellData::Find(uint32 spell_id) const
{
    TrainerSpellMap::const_iterator itr = spellList.find(spell_id);
    if (itr != spellList.end())
        return &itr->second;

    return NULL;
}

bool VendorItemData::RemoveItem(uint32 item_id, uint8 type)
{
    bool found = false;
    for (VendorItemList::iterator i = m_items.begin(); i != m_items.end();)
    {
        if ((*i)->item == item_id && (*i)->Type == type)
        {
            i = m_items.erase(i++);
            found = true;
        }
        else
            ++i;
    }
    return found;
}

VendorItem const* VendorItemData::FindItemCostPair(uint32 item_id, uint32 extendedCost, uint8 type) const
{
    for (VendorItemList::const_iterator i = m_items.begin(); i != m_items.end(); ++i)
        if ((*i)->item == item_id && (*i)->ExtendedCost == extendedCost && (*i)->Type == type)
            return *i;
    return NULL;
}

CreatureModel const CreatureModel::DefaultInvisibleModel(11686, 1.0f, 1.0f);
CreatureModel const CreatureModel::DefaultVisibleModel(17519, 1.0f, 1.0f);

CreatureModel const* CreatureTemplate::GetModelByIdx(uint32 idx) const
{
    return idx < Models.size() ? &Models[idx] : nullptr;
}

CreatureModel const* CreatureTemplate::GetRandomValidModel() const
{
    if (!Models.size())
        return nullptr;

    // If only one element, ignore the Probability (even if 0)
    if (Models.size() == 1)
        return &Models[0];

    auto selectedItr = Trinity::Containers::SelectRandomWeightedContainerElement(Models, [](CreatureModel const& model)
    {
        return model.Probability;
    });

    return &(*selectedItr);
}

CreatureModel const* CreatureTemplate::GetFirstValidModel() const
{
    for (CreatureModel const& model : Models)
        if (model.CreatureDisplayID)
            return &model;

    return nullptr;
}

CreatureModel const* CreatureTemplate::GetModelWithDisplayId(uint32 displayId) const
{
    for (CreatureModel const& model : Models)
        if (displayId == model.CreatureDisplayID)
            return &model;

    return nullptr;
}

CreatureModel const* CreatureTemplate::GetFirstInvisibleModel() const
{
    for (CreatureModel const& model : Models)
        if (CreatureModelInfo const* modelInfo = sObjectMgr->GetCreatureModelInfo(model.CreatureDisplayID))
            if (modelInfo && modelInfo->is_trigger)
                return &model;

    return &CreatureModel::DefaultInvisibleModel;
}

CreatureModel const* CreatureTemplate::GetFirstVisibleModel() const
{
    for (CreatureModel const& model : Models)
        if (CreatureModelInfo const* modelInfo = sObjectMgr->GetCreatureModelInfo(model.CreatureDisplayID))
            if (modelInfo && !modelInfo->is_trigger)
                return &model;

    return &CreatureModel::DefaultVisibleModel;
}

bool AssistDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (Unit* victim = Unit::GetUnit(m_owner, m_victim))
    {
        while (!m_assistants.empty())
        {
            Creature* assistant = Unit::GetCreature(m_owner, *m_assistants.begin());
            m_assistants.pop_front();

            if (assistant && assistant->CanAssistTo(&m_owner, victim))
            {
                assistant->SetNoCallAssistance(true);
                assistant->CombatStart(victim);
                if (assistant->IsAIEnabled)
                    assistant->AI()->AttackStart(victim);
            }
        }
    }
    return true;
}

CreatureBaseStats const* CreatureBaseStats::GetBaseStats(uint8 level, uint8 unitClass)
{
    return sObjectMgr->GetCreatureBaseStats(level, unitClass);
}

bool ForcedDespawnDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.DespawnOrUnsummon();    // since we are here, we are not TempSummon as object type cannot change during runtime
    return true;
}

Creature::Creature(bool isWorldObject) : Unit(isWorldObject), MapObject(),
lootForPickPocketed(false), lootForBody(false), m_groupLootTimer(0), lootingGroupLowGUID(0),
m_PlayerDamageReq(0), m_lootRecipient(), m_lootRecipientGroup(0), m_corpseRemoveTime(0), m_respawnTime(0),
m_respawnDelay(300), m_corpseDelay(60), m_wanderDistance(0.0f), m_WalkMode(0.0f), m_reactState(REACT_AGGRESSIVE),
m_defaultMovementType(IDLE_MOTION_TYPE), m_spawnId(0), m_equipmentId(0), m_originalEquipmentId(0), m_AlreadyCallAssistance(false),
m_AlreadySearchedAssistance(false), m_cannotReachTarget(false), m_cannotReachTimer(0), m_regenHealth(true), m_AI_locked(false), _isMissingSwimmingFlagOutOfCombat(false), m_meleeDamageSchoolMask(SPELL_SCHOOL_MASK_NORMAL),
m_creatureInfo(nullptr), m_creatureData(nullptr), m_path_id(0), m_formation(nullptr), m_triggerJustAppeared(false), m_respawnDelayMax(0), dynamicHealthPlayersCount(0)
{
    m_regenTimer = 0;
    m_valuesCount = UNIT_END;

    for (uint8 i = 0; i < CREATURE_MAX_SPELLS; ++i)
        m_spells[i] = 0;

    m_CreatureSpellCooldowns.clear();
    m_CreatureCategoryCooldowns.clear();
    DisableReputationGain = false;

    m_SightDistance = sWorld->getFloatConfig(CONFIG_SIGHT_MONSTER);
    m_CombatDistance = 0;//MELEE_RANGE;
    m_ReactDistance = 0;

    ResetLootMode(); // restore default loot mode

    m_isTempWorldObject = false;
    _focusSpell = NULL;
}

Creature::~Creature()
{
    delete i_AI;
    i_AI = NULL;

    //if (m_uint32Values)
    //    TC_LOG_ERROR("entities.unit", "Deconstruct Creature Entry = %u", GetEntry());
}

void Creature::AddToWorld()
{
    ///- Register the creature for guid lookup
    if (!IsInWorld())
    {
        GetMap()->GetObjectsStore().Insert<Creature>(GetGUID(), this);
        if (m_spawnId)
            GetMap()->GetCreatureBySpawnIdStore().insert(std::make_pair(m_spawnId, this));

        Unit::AddToWorld();
        SearchFormation();
        AIM_Initialize();
        if (IsVehicle())
            GetVehicleKit()->Install();

        if (GetZoneScript())
            GetZoneScript()->OnCreatureCreate(this);

        if (((GetCreatureTemplate()->rank == CREATURE_ELITE_RARE || GetCreatureTemplate()->rank == CREATURE_ELITE_RAREELITE) && GetZoneId() == 6757) || // Timeless Isle (or only rare elite?...)
            (!GetMap()->Instanceable() && sLootMgr->GetPersonalLoot(GetEntry())))   // World Bosses
            m_hasNormalLootMode = false;
    }
}

void Creature::RemoveFromWorld()
{
    if (IsInWorld())
    {
        if (GetZoneScript())
            GetZoneScript()->OnCreatureRemove(this);
        
        if (m_formation)
            sFormationMgr->RemoveCreatureFromGroup(m_formation, this);

        Unit::RemoveFromWorld();

        if (m_spawnId)
            Trinity::Containers::MultimapErasePair(GetMap()->GetCreatureBySpawnIdStore(), m_spawnId, this);

        GetMap()->GetObjectsStore().Remove<Creature>(GetGUID());
    }
}

void Creature::DisappearAndDie()
{
    DestroyForNearbyPlayers();
    //SetVisibility(VISIBILITY_OFF);
    //ObjectAccessor::UpdateObjectVisibility(this);
    if (IsAlive())
        setDeathState(JUST_DIED);
    RemoveCorpse(false);
}

void Creature::SearchFormation()
{
    if (IsSummon())
        return;

    uint32 lowguid = GetDBTableGUIDLow();
    if (!lowguid)
        return;

    CreatureGroupInfoType::iterator frmdata = sFormationMgr->CreatureGroupMap.find(lowguid);
    if (frmdata != sFormationMgr->CreatureGroupMap.end())
        sFormationMgr->AddCreatureToGroup(frmdata->second.leaderGUID, this);
}

void Creature::RemoveCorpse(bool setSpawnTime)
{
    if (getDeathState() != CORPSE)
        return;

    m_corpseRemoveTime = time(NULL);
    setDeathState(DEAD);
    RemoveAllAuras();
    UpdateObjectVisibility();
    ClearComboPointHolders();
    loot.clear();
    uint32 respawnDelay = m_respawnDelay;
    if (IsAIEnabled)
        AI()->CorpseRemoved(respawnDelay);

    // Should get removed later, just keep "compatibility" with scripts
    if (setSpawnTime)
        m_respawnTime = time(NULL) + respawnDelay;

    float x, y, z, o;
    GetRespawnPosition(x, y, z, &o);
    SetHomePosition(x, y, z, o);
    GetMap()->CreatureRelocation(this, x, y, z, o);
}

/**
 * change the entry of creature until respawn
 */
bool Creature::InitEntry(uint32 entry, uint32 /*team*/, const CreatureData* data)
{
    CreatureTemplate const* normalInfo = sObjectMgr->GetCreatureTemplate(entry);
    if (!normalInfo)
    {
        TC_LOG_ERROR("sql.sql", "Creature::InitEntry creature entry %u does not exist.", entry);
        return false;
    }

    // Get difficulty 1 mode entry
    CreatureTemplate const* cinfo = normalInfo;

    // Initialize loot duplicate count depending on raid difficulty
    if (GetMap()->Is25ManRaid())
        loot.maxDuplicates = 3;

    SetEntry(entry);                                        // normal entry always
    m_creatureInfo = cinfo;                                 // map mode related always

    // equal to player Race field, but creature does not have race
    SetRace(0);

    // known valid are: CLASS_WARRIOR, CLASS_PALADIN, CLASS_ROGUE, CLASS_MAGE
    SetClass(uint8(cinfo->unit_class));

    // Cancel load if no model defined
    if (!(cinfo->GetFirstValidModel()))
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has no model defined in table `creature_template`, can't load. ", entry);
        return false;
    }

    CreatureModel model = *ObjectMgr::ChooseDisplayId(cinfo, data);
    CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelRandomGender(&model, cinfo);
    if (!minfo)                                             // Cancel load if no model defined
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has no model defined in table `creature_template`, can't load. ", entry);
        return false;
    }

    SetDisplayId(model.CreatureDisplayID, true);
    SetNativeDisplayId(model.CreatureDisplayID);
    SetGender(minfo->gender);

    // Load creature equipment
    if (!data || data->equipmentId == 0)
        LoadEquipment(); // use default equipment (if available)
    else if (data && data->equipmentId != 0)                // override, 0 means no equipment
    {
        m_originalEquipmentId = data->equipmentId;
        LoadEquipment(data->equipmentId);
    }

    SetName(normalInfo->Name);                              // at normal entry always

    SetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED, 1.0f);
    SetFloatValue(UNIT_FIELD_MOD_SPELL_HASTE, 1.0f);

    SetSpeed(MOVE_WALK,     cinfo->speed_walk);
    SetSpeed(MOVE_RUN,      cinfo->speed_run);
    SetSpeed(MOVE_SWIM, 1.0f);      // using 1.0 rate
    SetSpeed(MOVE_FLIGHT, 1.0f);    // using 1.0 rate

    // Will set UNIT_FIELD_BOUNDING_RADIUS and UNIT_FIELD_COMBAT_REACH
    SetObjectScale(cinfo->scale);

    SetFloatValue(UNIT_FIELD_HOVER_HEIGHT, cinfo->HoverHeight);

    // checked at loading
    m_defaultMovementType = MovementGeneratorType(cinfo->MovementType);
    if (!m_wanderDistance && m_defaultMovementType == RANDOM_MOTION_TYPE)
        m_defaultMovementType = IDLE_MOTION_TYPE;

    for (uint8 i=0; i < CREATURE_MAX_SPELLS; ++i)
        m_spells[i] = GetCreatureTemplate()->spells[i];

    LoadCustomVisibility();

    return true;
}

bool Creature::UpdateEntry(uint32 Entry, uint32 team, const CreatureData* data)
{
    if (!InitEntry(Entry, team, data))
        return false;

    CreatureTemplate const* cInfo = GetCreatureTemplate();

    m_regenHealth = cInfo->RegenHealth;

    // creatures always have melee weapon ready if any unless specified otherwise
    if (!GetCreatureAddon())
        SetSheath(SHEATH_STATE_MELEE);

    SelectLevel(GetCreatureTemplate());
    SetFaction(cInfo->faction);

    uint32 npcflag, unit_flags, dynamicflags;
    ObjectMgr::ChooseCreatureFlags(cInfo, npcflag, unit_flags, dynamicflags, data);

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_WORLDEVENT)
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, npcflag | sGameEventMgr->GetNPCFlag(this));
    else
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, npcflag);

    SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, cInfo->npcflag2);

    SetAttackTime(BASE_ATTACK,  cInfo->BaseAttackTime);
    SetAttackTime(OFF_ATTACK,   cInfo->BaseAttackTime);
    SetAttackTime(RANGED_ATTACK, cInfo->RangeAttackTime);

    SetUInt32Value(UNIT_FIELD_FLAGS, unit_flags);
    SetUInt32Value(UNIT_FIELD_FLAGS_2, cInfo->unit_flags2);

    SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, dynamicflags);

    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    SetMeleeDamageSchool(SpellSchools(cInfo->dmgschool));
    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(GetLevel(), cInfo->unit_class);
    if (!IsPet())
    {
        float armor = (float)stats->GenerateArmor(cInfo); /// @todo Why is this treated as uint32 when it's a float?
        SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, armor);
        SetModifierValue(UNIT_MOD_RESISTANCE_HOLY, BASE_VALUE, float(cInfo->resistance[SPELL_SCHOOL_HOLY]));
        SetModifierValue(UNIT_MOD_RESISTANCE_FIRE, BASE_VALUE, float(cInfo->resistance[SPELL_SCHOOL_FIRE]));
        SetModifierValue(UNIT_MOD_RESISTANCE_NATURE, BASE_VALUE, float(cInfo->resistance[SPELL_SCHOOL_NATURE]));
        SetModifierValue(UNIT_MOD_RESISTANCE_FROST, BASE_VALUE, float(cInfo->resistance[SPELL_SCHOOL_FROST]));
        SetModifierValue(UNIT_MOD_RESISTANCE_SHADOW, BASE_VALUE, float(cInfo->resistance[SPELL_SCHOOL_SHADOW]));
        SetModifierValue(UNIT_MOD_RESISTANCE_ARCANE, BASE_VALUE, float(cInfo->resistance[SPELL_SCHOOL_ARCANE]));
    }

    SetCanModifyStats(true);
    UpdateAllStats();

    // checked and error show at loading templates
    if (FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction))
    {
        if (factionTemplate->factionFlags & FACTION_TEMPLATE_FLAG_PVP)
            SetPvP(true);
        else
            SetPvP(false);
    }

    // updates spell bars for vehicles and set player's faction - should be called here, to overwrite faction that is set from the new template
    if (IsVehicle())
    {
        if (Player* owner = Creature::GetCharmerOrOwnerPlayerOrPlayerItself()) // this check comes in case we don't have a player
        {
            SetFaction(owner->GetFaction()); // vehicles should have same as owner faction
            owner->VehicleSpellInitialize();
        }
    }

    // trigger creature is always not selectable and can not be attacked
    if (IsTrigger())
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    InitializeReactState();

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_NO_TAUNT)
    {
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_NO_KNOCK_BACK)
    {
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
        ApplySpellImmune(0, IMMUNITY_ID, 10326, true);
        //ApplySpellImmune(0, IMMUNITY_ID, 49560, true); // not needed, grip not pull boss (TODO: fix 1 spell 66017)
        ApplySpellImmune(0, IMMUNITY_ID, 61391, true);
    }

    UpdateMovementFlags();

    //We must update last scriptId or it looks like we reloaded a script, breaking some things such as gossip temporarily
    LastUsedScriptID = GetScriptId();
        
    return true;
}

void Creature::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    if (newPhaseMask == GetPhaseMask())
        return;

    Unit::SetPhaseMask(newPhaseMask, false);

    if (Vehicle* vehicle = GetVehicleKit())
    {
        for (auto seat = vehicle->Seats.begin(); seat != vehicle->Seats.end(); seat++)
            if (Unit* passenger = ObjectAccessor::GetUnit(*this, seat->second.Passenger.Guid))
                passenger->SetPhaseMask(newPhaseMask, update);
    }

    if (update)
        UpdateObjectVisibility();
}

void Creature::Update(uint32 diff)
{
    if (IsAIEnabled && m_triggerJustAppeared)
    {
        m_triggerJustAppeared = false;
        AI()->JustAppeared();
        if (m_vehicleKit)
            m_vehicleKit->Reset();
        if (GetMap()->IsRaid() && ((InstanceMap*)GetMap())->GetInstanceScript())
            ((InstanceMap*)GetMap())->GetInstanceScript()->UpdateDynamicHealth(GetGUID());
    }

    UpdateMovementFlags();

    switch (m_deathState)
    {
        case JUST_RESPAWNED:
            // Must not be called, see Creature::setDeathState JUST_RESPAWNED -> ALIVE promoting.
            TC_LOG_ERROR("entities.unit", "Creature (GUID: %u Entry: %u) in wrong state: JUST_RESPAWNED (4)", GetGUID().GetCounter(), GetEntry());
            break;
        case JUST_DIED:
            // Must not be called, see Creature::setDeathState JUST_DIED -> CORPSE promoting.
            TC_LOG_ERROR("entities.unit", "Creature (GUID: %u Entry: %u) in wrong state: JUST_DEAD (1)", GetGUID().GetCounter(), GetEntry());
            break;
        case DEAD:
        {
            time_t now = time(NULL);
            if (m_respawnTime <= now)
            {
                bool allowed = IsAIEnabled ? AI()->CanRespawn() : true;     // First check if there are any scripts that object to us respawning
                if (!allowed)                                               // Will be rechecked on next Update call
                    break;

                ObjectGuid dbtableHighGuid = ObjectGuid(HighGuid::Unit, GetEntry(), m_spawnId);
                time_t linkedRespawntime = GetMap()->GetLinkedRespawnTime(dbtableHighGuid);
                if (!linkedRespawntime)             // Can respawn
                {
                    if (sObjectMgr->GetLinkedRespawnGuid(dbtableHighGuid) && GetInstanceScript() && GetInstanceScript()->IsEncounterInProgress())
                    {
                        m_respawnTime = now + MINUTE;
                        SaveRespawnTime(); // also save to DB immediately
                    }
                    else
                        Respawn();
                }
                else                                // the master is dead
                {
                    uint64 targetGuid = sObjectMgr->GetLinkedRespawnGuid(dbtableHighGuid);
                    if (targetGuid == dbtableHighGuid) // if linking self, never respawn (check delayed to next day)
                        SetRespawnTime(DAY);
                    else
                        m_respawnTime = (now > linkedRespawntime ? now : linkedRespawntime)+urand(5, MINUTE); // else copy time from master and add a little
                    SaveRespawnTime(); // also save to DB immediately
                }
            }
            break;
        }
        case CORPSE:
        {
            Unit::Update(diff);
            // deathstate changed on spells update, prevent problems
            if (m_deathState != CORPSE)
                break;

            if (m_groupLootTimer && lootingGroupLowGUID)
            {
                if (m_groupLootTimer <= diff)
                {
                    uint32 gguid = lootingGroupLowGUID;
                    ObjectGuid crguid = GetGUID();
                    if (Creature* creature = this->GetMap()->GetCreature(crguid))
                        if (Group* group = sGroupMgr->GetGroupByGUID(gguid))
                            group->EndRoll(&creature->loot);                    
                    // TaskMgr::Default()->ScheduleInvocation([=]
                    // {
                    //     if (Creature* creature = ObjectAccessor::FindCreature(crguid))
                    //         if (Group* group = sGroupMgr->GetGroupByGUID(gguid))
                    //             group->EndRoll(&creature->loot);
                    // });
                    m_groupLootTimer = 0;
                    lootingGroupLowGUID = 0;
                }
                else m_groupLootTimer -= diff;
            }
            else if (m_corpseRemoveTime <= time(NULL))
            {
                RemoveCorpse(false);
                TC_LOG_DEBUG("entities.unit", "Removing corpse... %u ", GetUInt32Value(OBJECT_FIELD_ENTRY_ID));
            }
            break;
        }
        case ALIVE:
        {
            Unit::Update(diff);

            // creature can be dead after Unit::Update call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!IsAlive())
                break;

            // if creature is charmed, switch to charmed AI (and back)
            if (NeedChangeAI)
            {
                UpdateCharmAI();
                NeedChangeAI = false;
                IsAIEnabled = true;
                if (!IsInEvadeMode())
                {
                    Unit* charmer;
                    if (LastCharmerGUID && (charmer = ObjectAccessor::GetUnit(*this, LastCharmerGUID)) && CanStartAttack(charmer, true))
                        i_AI->AttackStart(charmer);
                    else
                        AI()->EnterEvadeMode();
                }

                LastCharmerGUID.Clear();
            }

            if (!IsInEvadeMode() && IsAIEnabled)
            {
                // do not allow the AI to be changed during update
                m_AI_locked = true;
                i_AI->UpdateAI(diff);
                m_AI_locked = false;
            }

            // creature can be dead after UpdateAI call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!IsAlive())
                break;

            // Check if we should refill the pickpocketing loot
            if (lootForPickPocketed && m_pickpocketLootRestore != TimeValue::zero() && m_pickpocketLootRestore <= TimeValue::Now())
            {
                lootForPickPocketed = false;
                m_pickpocketLootRestore = TimeValue::zero();
            }

            m_regenTimer += diff;

            Regenerate();

            if (CanNotReachTarget() && !IsInEvadeMode() && !GetMap()->IsRaid())
            {
                m_cannotReachTimer += diff;
                if (m_cannotReachTimer >= CREATURE_NOPATH_EVADE_TIME)
                    if (CreatureAI* ai = AI())
                        ai->EnterEvadeMode(); // TODO missing EVADE_REASON
                        // ai->EnterEvadeMode(CreatureAI::EVADE_REASON_NO_PATH);
            }            
            break;
        }
        default:
            break;
    }

}

void Creature::RegenerateMana()
{
    if (!isRegeneratingMana())
        return;

    uint32 curValue = GetPower(POWER_MANA);
    uint32 maxValue = GetMaxPower(POWER_MANA);

    if (curValue >= maxValue)
        return;

    uint32 addvalue = 0;

    // Combat and any controlled creature
    if (IsInCombat() || GetCharmerOrOwnerGUID())
    {
        float ManaIncreaseRate = sWorld->getRate(RATE_POWER_MANA);
        float Spirit = GetStat(STAT_SPIRIT);

        addvalue = uint32((Spirit / 5.0f + 17.0f) * ManaIncreaseRate);
    }
    else
        addvalue = maxValue / 3;

    // Apply modifiers (if any).
    AuraEffectList const& ModPowerRegenPCTAuras = GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT);
    for (AuraEffectList::const_iterator i = ModPowerRegenPCTAuras.begin(); i != ModPowerRegenPCTAuras.end(); ++i)
        if ((*i)->GetMiscValue() == POWER_MANA)
            AddPct(addvalue, (*i)->GetAmount());

    addvalue += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) * CREATURE_REGEN_INTERVAL / (5 * IN_MILLISECONDS);

    ModifyPower(POWER_MANA, addvalue);
}

void Creature::DoFleeToGetAssistance()
{
    if (!GetVictim())
        return;

    if (HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    float radius = sWorld->getFloatConfig(CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS);
    if (radius >0)
    {
        Creature* creature = NULL;

        CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
        Cell cell(p);
        cell.SetNoCreate();
        Trinity::NearestAssistCreatureInCreatureRangeCheck u_check(this, GetVictim(), radius);
        Trinity::CreatureLastSearcher<Trinity::NearestAssistCreatureInCreatureRangeCheck> searcher(this, creature, u_check);

        TypeContainerVisitor<Trinity::CreatureLastSearcher<Trinity::NearestAssistCreatureInCreatureRangeCheck>, GridTypeMapContainer > grid_creature_searcher(searcher);

        cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);

        SetNoSearchAssistance(true);
        UpdateSpeed(MOVE_RUN, false);

        if (!creature)
            //SetFeared(true, GetVictim()->GetGUID(), 0, sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_FLEE_DELAY));
            /// @todo use 31365
            SetControlled(true, UNIT_STATE_FLEEING);
        else
            GetMotionMaster()->MoveSeekAssistance(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ());
    }
}

bool Creature::AIM_Initialize(CreatureAI* ai)
{
    // make sure nothing can change the AI during AI update
    if (m_AI_locked)
    {
        TC_LOG_DEBUG("scripts", "AIM_Initialize: failed to init, locked.");
        return false;
    }

    UnitAI* oldAI = i_AI;

    Motion_Initialize();

    i_AI = ai ? ai : FactorySelector::selectAI(this);
    delete oldAI;
    IsAIEnabled = true;
    i_AI->InitializeAI();
    // Initialize vehicle
    if (GetVehicleKit())
        GetVehicleKit()->Reset();
    return true;
}

void Creature::Motion_Initialize()
{
    if (!m_formation)
        GetMotionMaster()->Initialize();
    else if (m_formation->GetLeader() == this)
    {
        m_formation->FormationReset(false);
        GetMotionMaster()->Initialize();
    }
    else if (m_formation->IsFormed())
        GetMotionMaster()->MoveIdle(); //wait the order of leader
    else
        GetMotionMaster()->Initialize();
}

bool Creature::Create(uint32 guidlow, Map* map, uint32 phaseMask, uint32 Entry, uint32 vehId, uint32 team, float x, float y, float z, float ang, const CreatureData* data)
{
    ASSERT(map);
    SetMap(map);
    SetPhaseMask(phaseMask, false);

    if (data && data->phaseid)
        SetPhased(data->phaseid, false, true);

    if (data && data->phaseGroup)
        for (auto ph : GetPhasesForGroup(data->phaseGroup))
            SetPhased(ph, false, true);

    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(Entry);
    if (!cinfo)
    {
        TC_LOG_ERROR("sql.sql", "Creature::Create(): creature template (guidlow: %u, entry: %u) does not exist.", guidlow, Entry);
        return false;
    }

    //! Relocate before CreateFromProto, to initialize coords and allow
    //! returning correct zone id for selecting OutdoorPvP/Battlefield script
    Relocate(x, y, z, ang);

    // Check if the position is valid before calling CreateFromProto(), otherwise we might add Auras to Creatures at
    // invalid position, triggering a crash about Auras not removed in the destructor
    if (!IsPositionValid())
    {
        TC_LOG_ERROR("entities.unit", "Creature::Create(): given coordinates for creature (guidlow %d, entry %d) are not valid (X: %f, Y: %f, Z: %f, O: %f)", guidlow, Entry, x, y, z, ang);
        return false;
    }

    {
        // area/zone id is needed immediately for ZoneScript::GetCreatureEntry hook before it is known which creature template to load (no model/scale available yet)
        PositionFullTerrainStatus data;
        GetMap()->GetFullTerrainStatusForPosition(GetPhaseMask(), GetPositionX(), GetPositionY(), GetPositionZ(), data, MAP_ALL_LIQUIDS, DEFAULT_COLLISION_HEIGHT);
        ProcessPositionDataChanged(data);
    }

    if (!CreateFromProto(guidlow, Entry, vehId, team, data))
        return false;

    switch (GetCreatureTemplate()->rank)
    {
        case CREATURE_ELITE_RARE:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_RARE);
            break;
        case CREATURE_ELITE_ELITE:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_ELITE);
            break;
        case CREATURE_ELITE_RAREELITE:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_RAREELITE);
            break;
        case CREATURE_ELITE_WORLDBOSS:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_WORLDBOSS);
            break;
        default:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_NORMAL);
            break;
    }

    // For summons called in InitSummon because creature isn't in world yet and I fucking tired of bugs that caused by this shit
    if (!IsSummon())
        LoadCreaturesAddon();
    ApplyInstanceAuraIfNeeded();

    //! Need to be called after LoadCreaturesAddon - MOVEMENTFLAG_HOVER is set there
    if (HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
    {
        z += GetFloatValue(UNIT_FIELD_HOVER_HEIGHT);

        //! Relocate again with updated Z coord
        Relocate(x, y, z, ang);
    }

    //! Need to be called after LoadCreaturesAddon - MOVEMENTFLAG_HOVER is set there
    m_positionZ += GetHoverOffset();

    LastUsedScriptID = GetScriptId();

    /// @todo Replace with spell, handle from DB
    if (IsSpiritHealer() || IsSpiritGuide())
    {
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
        m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
    }

    if (Entry == VISUAL_WAYPOINT)
        SetVisible(false);

    return true;
}

void Creature::InitializeReactState()
{
    if (IsTotem() || IsTrigger() || GetCreatureType() == CREATURE_TYPE_CRITTER || IsSpiritService())
        SetReactState(REACT_PASSIVE);
    else
        SetReactState(REACT_AGGRESSIVE);
    /*else if (IsCivilian())
    SetReactState(REACT_DEFENSIVE);*/
}

bool Creature::isCanInteractWithBattleMaster(Player* player, bool msg) const
{
    if (!IsBattleMaster())
        return false;

    BattlegroundTypeId bgTypeId = sBattlegroundMgr->GetBattleMasterBG(GetEntry());
    if (!msg)
        return player->GetBGAccessByLevel(bgTypeId);

    if (!player->GetBGAccessByLevel(bgTypeId))
    {
        player->PlayerTalkClass->ClearMenus();
        switch (bgTypeId)
        {
            case BATTLEGROUND_AV:  player->PlayerTalkClass->SendGossipMenu(7616, GetGUID()); break;
            case BATTLEGROUND_WS:  player->PlayerTalkClass->SendGossipMenu(7599, GetGUID()); break;
            case BATTLEGROUND_AB:  player->PlayerTalkClass->SendGossipMenu(7642, GetGUID()); break;
            case BATTLEGROUND_EY:
            case BATTLEGROUND_NA:
            case BATTLEGROUND_BE:
            case BATTLEGROUND_AA:
            case BATTLEGROUND_RL:
            case BATTLEGROUND_SA:
            case BATTLEGROUND_DS:
            case BATTLEGROUND_RV: player->PlayerTalkClass->SendGossipMenu(10024, GetGUID()); break;
            default: break;
        }
        return false;
    }
    return true;
}

bool Creature::isCanTrainingAndResetTalentsOf(Player* player) const
{
    return player->GetLevel() >= 10
        && GetCreatureTemplate()->trainer_type == TRAINER_TYPE_CLASS
        && player->GetClass() == GetCreatureTemplate()->trainer_class;
}

void Creature::StartPickPocketRefillTimer()
{
    m_pickpocketLootRestore = TimeValue::Now() + Seconds(sWorld->getIntConfig(CONFIG_CREATURE_PICKPOCKET_REFILL));
}

Player* Creature::GetLootRecipient() const
{
    if (!m_lootRecipient)
        return NULL;
    return ObjectAccessor::FindPlayer(m_lootRecipient);
}

Group* Creature::GetLootRecipientGroup() const
{
    if (!m_lootRecipientGroup)
        return NULL;
    return sGroupMgr->GetGroupByGUID(m_lootRecipientGroup);
}

void Creature::SetLootRecipient(Unit* unit)
{
    // set the player whose group should receive the right
    // to loot the creature after it dies
    // should be set to NULL after the loot disappears

    if (!unit)
    {
        m_lootRecipient.Clear();
        m_lootRecipientGroup = 0;
        m_lootRecipients.clear();
        m_lootRecipientGroupMembers.clear();
        RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE|UNIT_DYNFLAG_TAPPED);
        return;
    }


    if (IsPet())
    {
        TC_LOG_ERROR("shitlog", "Creature::SetLootRecipient unit: %s guid: " UI64FMTD "\n%s\n", unit->GetName().c_str(), unit->GetGUID().GetRawValue());
        return;
    }

    if (unit->GetTypeId() != TYPEID_PLAYER && !unit->IsVehicle())
        return;

    Player* player = unit->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!player)                                             // normal creature, no player involved
        return;

    m_lootRecipient = player->GetGUID();
    if (Group* group = player->GetGroup())
    {
        m_lootRecipientGroup = group->GetLowGUID();
        if (IsInWorld() && GetMap()->IsDungeon())
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                if (Player* member = itr->GetSource())
                    if (member->IsAtGroupRewardDistance(this))
                        m_lootRecipientGroupMembers.insert(member->GetGUID());
    }

    SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED);
}

bool Creature::HasLootRecipient(Player const* player) const
{
    if (m_lootRecipients.find(player->GetGUID()) != m_lootRecipients.end())
        return true;

    for (auto&& itr : m_lootRecipients)
        if (Player* recipient = ObjectAccessor::GetPlayer(*this, itr))
            if (recipient->IsInRaidWith(player))
                return true;

    return false;
}

// return true if this creature is tapped by the player or by a member of his group.
bool Creature::IsTappedBy(Player const* player) const
{
    if (player->GetGUID() == m_lootRecipient)
        return true;

    if (HasLootRecipient(player))
        return true;

    Group const* playerGroup = player->GetGroup();
    if (!playerGroup || playerGroup != GetLootRecipientGroup()) // if we dont have a group we arent the recipient
        return false;                                           // if creature doesnt have group bound it means it was solo killed by someone else

    return true;
}

void Creature::SaveToDB()
{
    // this should only be used when the creature has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    CreatureData const* data = sObjectMgr->GetCreatureData(m_spawnId);
    if (!data)
    {
        TC_LOG_ERROR("entities.unit", "Creature::SaveToDB failed, cannot get creature data!");
        return;
    }

    uint32 mapId = GetTransport() ? GetTransport()->GetGOInfo()->moTransport.mapID : GetMapId();
    SaveToDB(mapId, data->spawnMask, GetPhaseMask());
}

void Creature::SaveToDB(uint32 mapid, uint16 spawnMask, uint32 phaseMask)
{
    // update in loaded data
    if (!m_spawnId)
        m_spawnId = sObjectMgr->GenerateCreatureSpawnId();

    CreatureData& data = sObjectMgr->NewOrExistCreatureData(m_spawnId);

    uint32 displayId = GetNativeDisplayId();
    uint32 npcflag = GetUInt32Value(UNIT_FIELD_NPC_FLAGS);
    uint32 npcflag2 = GetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1);
    uint32 unit_flags = GetUInt32Value(UNIT_FIELD_FLAGS);
    uint32 unit_flags2 = GetUInt32Value(UNIT_FIELD_FLAGS_2);
    uint32 dynamicflags = GetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS);

    // check if it's a custom model and if not, use 0 for displayId
    CreatureTemplate const* cinfo = GetCreatureTemplate();
    if (cinfo)
    {
        for (CreatureModel model : cinfo->Models)
            if (displayId && displayId == model.CreatureDisplayID)
                displayId = 0;

        if (npcflag == cinfo->npcflag)
            npcflag = 0;

        if (npcflag2 == cinfo->npcflag2)
            npcflag2 = 0;

        if (unit_flags == cinfo->unit_flags)
            unit_flags = 0;

        if (unit_flags2 == cinfo->unit_flags2)
            unit_flags2 = 0;

        if (dynamicflags == cinfo->dynamicflags)
            dynamicflags = 0;
    }

    // data->guid = guid must not be updated at save
    data.id = GetEntry();
    data.mapId = mapid;
    data.phaseMask = phaseMask;
    data.displayid = displayId;
    data.equipmentId = GetCurrentEquipmentId();
    if (!GetTransport())
    {
        data.posX = GetPositionX();
        data.posY = GetPositionY();
        data.posZ = GetPositionZMinusOffset();
        data.orientation = GetOrientation();
    }
    else
    {
        data.posX = GetTransOffsetX();
        data.posY = GetTransOffsetY();
        data.posZ = GetTransOffsetZ();
        data.orientation = GetTransOffsetO();
    }

    data.spawntimesecs = m_respawnDelay;
    data.spawntimesecs_max = m_respawnDelayMax;
    // prevent add data integrity problems
    data.wander_distance = GetDefaultMovementType() == IDLE_MOTION_TYPE ? 0.0f : m_wanderDistance;
    data.currentwaypoint = 0;
    data.curhealth = GetHealth();
    data.curmana = GetPower(POWER_MANA);
    // prevent add data integrity problems
    data.movementType = !m_wanderDistance && GetDefaultMovementType() == RANDOM_MOTION_TYPE
        ? IDLE_MOTION_TYPE : GetDefaultMovementType();
    data.spawnMask = spawnMask;
    data.npcflag = npcflag;
    data.npcflag2 = npcflag2;
    data.unit_flags = unit_flags;
    data.unit_flags2 = unit_flags2;
    data.dynamicflags = dynamicflags;
    data.WalkMode = m_WalkMode;

    // update in DB
    WorldDatabaseTransaction trans = WorldDatabase.BeginTransaction();

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CREATURE);
    stmt->setUInt32(0, m_spawnId);
    trans->Append(stmt);

    uint8 index = 0;

    stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_CREATURE);
    stmt->setUInt32(index++, m_spawnId);
    stmt->setUInt32(index++, GetEntry());
    stmt->setUInt16(index++, uint16(mapid));
    stmt->setUInt16(index++, spawnMask);
    stmt->setUInt32(index++, GetPhaseMask());
    stmt->setUInt32(index++, displayId);
    stmt->setInt32(index++, int32(GetCurrentEquipmentId()));
    stmt->setFloat(index++, data.posX);
    stmt->setFloat(index++, data.posY);
    stmt->setFloat(index++, data.posZ);
    stmt->setFloat(index++, data.orientation);
    stmt->setUInt32(index++, m_respawnDelay);
    stmt->setFloat(index++, m_wanderDistance);
    stmt->setUInt32(index++, 0);
    stmt->setUInt32(index++, GetHealth());
    stmt->setUInt32(index++, GetPower(POWER_MANA));
    stmt->setUInt8(index++, uint8(GetDefaultMovementType()));
    stmt->setUInt32(index++, npcflag);
    stmt->setUInt32(index++, npcflag2);
    stmt->setUInt32(index++, unit_flags);
    stmt->setUInt32(index++, unit_flags2);
    stmt->setUInt32(index++, dynamicflags);
    stmt->setFloat(index++, m_WalkMode);
    trans->Append(stmt);

    WorldDatabase.CommitTransaction(trans);
}

void Creature::SelectLevel(const CreatureTemplate* cinfo)
{
    uint8 dbminlevel = cinfo->minlevel;
    uint8 dbmaxlevel = cinfo->maxlevel;
    float hpmod = cinfo->ModHealth;
    float mindmg = cinfo->mindmg;
    float maxdmg = cinfo->maxdmg;
    float minrangedmg = cinfo->minrangedmg;
    float maxrangedmg = cinfo->maxrangedmg;
    uint32 attackpower = cinfo->attackpower;
    uint32 rangedattackpower = cinfo->rangedattackpower;

    CreatureDifficultyInfo const* difficultyInfo = nullptr;
    if (GetMap()->GetDifficulty() > REGULAR_DIFFICULTY || GetMap()->IsBattleground())
    {
        difficultyInfo = sObjectMgr->SelectDifficultyInfo(GetMap(), GetEntry());
        if (difficultyInfo)
        {
            dbminlevel = difficultyInfo->LevelMin;
            dbmaxlevel = difficultyInfo->LevelMax;
            hpmod = difficultyInfo->HealthMod;
            mindmg = difficultyInfo->MinDamage;
            maxdmg = difficultyInfo->MaxDamage;
            minrangedmg = difficultyInfo->MinRangeDamage;
            maxrangedmg = difficultyInfo->MaxRangeDamage;
            rangedattackpower = difficultyInfo->RangedAttackPower;
        }
    }

    uint32 rank = IsPet()? 0 : cinfo->rank;

    // level
    uint8 minlevel = std::min(dbmaxlevel, dbminlevel);
    uint8 maxlevel = std::max(dbmaxlevel, dbminlevel);
    uint8 level = minlevel == maxlevel ? minlevel : urand(minlevel, maxlevel);
    SetLevel(level);

    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(level, cinfo->unit_class);

    uint32 expansion = cinfo->expansion;
    if (GetMap()->IsBattleground())
    {
        Battleground* bg = ((BattlegroundMap*)GetMap())->GetBG();
        if (bg->GetMaxLevel() > 86)
            expansion = EXPANSION_MISTS_OF_PANDARIA;
        else if (level > 81)
            expansion = EXPANSION_CATACLYSM;
        else if (level > 71)
            expansion = EXPANSION_WRATH_OF_THE_LICH_KING;
    }

    // health
    uint32 health = uint32(std::ceil(stats->BaseHealth[expansion] * hpmod * _GetHealthMod(rank)));

    SetCreateHealth(health);
    SetMaxHealth(health);
    SetHealth(health);
    ResetPlayerDamageReq();

    // mana
    uint32 mana = stats->GenerateMana(cinfo);
    SetCreateMana(mana);

    switch (GetClass())
    {
        case CLASS_WARRIOR:
            SetPowerType(POWER_RAGE);
            break;
        case CLASS_ROGUE:
            SetPowerType(POWER_ENERGY);
            break;
        case CLASS_MONK:
            SetPowerType(POWER_CHI);
            break;
        default:
            SetMaxPower(POWER_MANA, mana);
            SetPower(POWER_MANA, mana);
            break;
    }

    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);
    SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)mana);

    float basedamage = stats->BaseDamage[cinfo->expansion];
    if (basedamage && !difficultyInfo)
    {
        mindmg = basedamage;
        maxdmg = basedamage * 1.5f;
        minrangedmg = mindmg;
        maxrangedmg = maxdmg;
        attackpower = stats->AttackPower;
        rangedattackpower = stats->RangedAttackPower;
    }

    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, mindmg);
    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, maxdmg);

    SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, mindmg);
    SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, maxdmg);

    SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, minrangedmg);
    SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, maxrangedmg);

    SetModifierValue(UNIT_MOD_ATTACK_POWER, BASE_VALUE, attackpower);
    SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, attackpower);
}

float Creature::_GetHealthMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_HP);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_HP);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_HP);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_HP);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_HP);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_HP);
    }
}

void Creature::LowerPlayerDamageReq(uint32 unDamage)
{
    if (m_PlayerDamageReq)
        m_PlayerDamageReq > unDamage ? m_PlayerDamageReq -= unDamage : m_PlayerDamageReq = 0;
}

float Creature::_GetDamageMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_DAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_DAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_DAMAGE);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
    }
}

float Creature::GetSpellDamageMod(int32 Rank) const
{
    switch (Rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_SPELLDAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_SPELLDAMAGE);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
    }
}

bool Creature::CreateFromProto(uint32 guidlow, uint32 Entry, uint32 vehId, uint32 team, const CreatureData* data)
{
    SetZoneScript();
    if (GetZoneScript() && data)
    {
        Entry = GetZoneScript()->GetCreatureEntry(guidlow, data);
        if (!Entry)
            return false;
    }

    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(Entry);
    if (!cinfo)
    {
        TC_LOG_ERROR("sql.sql", "Creature::CreateFromProto(): creature template (guidlow: %u, entry: %u) does not exist.", guidlow, Entry);
        return false;
    }

    SetOriginalEntry(Entry);

    // hackfix for crawler mine controller
    if (Entry == 71795 && (GetMap()->Is25ManRaid() || GetMap()->GetDifficulty() == RAID_DIFFICULTY_1025MAN_FLEX))
        vehId = 2916;

    Object::_Create(guidlow, Entry, (vehId || cinfo->VehicleId) ? HighGuid::Vehicle : HighGuid::Unit);

    if (!UpdateEntry(Entry, team, data))
        return false;

    if (!vehId)
    {
        if (GetCreatureTemplate()->VehicleId)
        {
            vehId = GetCreatureTemplate()->VehicleId;
            Entry = GetCreatureTemplate()->Entry;
        }
        else
            vehId = cinfo->VehicleId;
    }

    if (vehId)
        CreateVehicleKit(vehId, Entry, true);

    return true;
}

bool Creature::LoadCreatureFromDB(uint32 guid, Map* map, bool addToMap)
{
    CreatureData const* data = sObjectMgr->GetCreatureData(guid);

    if (!data)
    {
        TC_LOG_ERROR("sql.sql", "Creature (GUID: %u) not found in table `creature`, can't load. ", guid);
        return false;
    }

    if (data->gameEventId)
        if (!map->CheckGameEventSpawn(data->gameEventId))
            return false;

    m_spawnId = guid;
    if (map->GetInstanceId() == 0)
    {
        if (map->GetCreature(ObjectGuid(HighGuid::Unit, data->id, guid)))
            return false;
    }
    else
        guid = map->GenerateLowGuid<HighGuid::Unit>();

    uint16 team = 0;
    if (!Create(guid, map, data->phaseMask, data->id, 0, team, data->posX, data->posY, data->posZ, data->orientation, data))
        return false;

    //We should set first home position, because then AI calls home movement
    SetHomePosition(data->posX, data->posY, data->posZ, data->orientation);

    m_wanderDistance = data->wander_distance;

    m_respawnDelay = data->spawntimesecs;
    m_respawnDelayMax = data->spawntimesecs_max;

    m_WalkMode = data->WalkMode;

    m_deathState = ALIVE;

    m_respawnTime  = GetMap()->GetCreatureRespawnTime(m_spawnId);
    if (m_respawnTime)                          // respawn on Update
    {
        m_deathState = DEAD;
        if (CanFly())
        {
            float tz = map->GetHeight(GetPhaseMask(), data->posX, data->posY, data->posZ, false);
            if (data->posZ - tz > 0.1f)
                Relocate(data->posX, data->posY, tz);
        }
    }

    uint32 curhealth;

    if (!m_regenHealth)
    {
        curhealth = data->curhealth;
        if (curhealth)
        {
            curhealth = uint32(curhealth*_GetHealthMod(GetCreatureTemplate()->rank));
            if (curhealth < 1)
                curhealth = 1;
        }
        else // Spawning creatures with 0 health is just wrong and shouldn't be possible
            curhealth = GetMaxHealth();

        SetPower(POWER_MANA, data->curmana);
    }
    else
    {
        curhealth = GetMaxHealth();
        SetPower(POWER_MANA, GetMaxPower(POWER_MANA));
    }

    SetHealth(m_deathState == ALIVE ? curhealth : 0);

    // checked at creature_template loading
    m_defaultMovementType = MovementGeneratorType(data->movementType);

    m_creatureData = data;

    if (addToMap && !GetMap()->AddToMap(this))
        return false;

    sBattlePetSpawnMgr->OnAddToMap(this);

    return true;
}

void Creature::LoadEquipment(int8 id, bool force /*= true*/)
{
    if (id == 0)
    {
        if (force)
        {
            for (uint8 i = 0; i < MAX_EQUIPMENT_ITEMS; ++i)
                SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + i, 0);
            m_equipmentId = 0;
        }
        return;
    }

    EquipmentInfo const* einfo = sObjectMgr->GetEquipmentInfo(GetEntry(), id);
    if (!einfo)
        return;

    m_equipmentId = id;
    for (uint8 i = 0; i < 3; ++i)
        SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + i, einfo->ItemEntry[i]);
}

bool Creature::hasQuest(uint32 quest_id) const
{
    return sObjectMgr->GetCreatureQuestRelations(GetEntry()).HasQuest(quest_id);
}

bool Creature::hasInvolvedQuest(uint32 quest_id) const
{
    return sObjectMgr->GetCreatureQuestInvolvedRelations(GetEntry()).HasQuest(quest_id);
}

void Creature::DeleteFromDB()
{
    CreatureData const* data = sObjectMgr->GetCreatureData(m_spawnId);
    if (!data)
        return;

    if (!m_spawnId)
    {
        TC_LOG_ERROR("entities.unit", "Trying to delete not saved creature! LowGUID: %u, Entry: %u", GetGUID().GetCounter(), GetEntry());
        return;
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    sMapMgr->DoForAllMapsWithMapId(data->mapId,
       [this, trans](Map* map) -> void
       {
           // despawn all active creatures, and remove their respawns
           std::vector<Creature*> toUnload;
           for (auto const& pair : Trinity::Containers::MapEqualRange(map->GetCreatureBySpawnIdStore(), m_spawnId))
               toUnload.push_back(pair.second);
           for (Creature* creature : toUnload)
               map->AddObjectToRemoveList(creature);
           map->RemoveCreatureRespawnTime(m_spawnId);
       }
    );

    sObjectMgr->DeleteCreatureData(m_spawnId);

    WorldDatabaseTransaction trans2 = WorldDatabase.BeginTransaction();

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CREATURE);
    stmt->setUInt32(0, m_spawnId);
    trans2->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CREATURE_ADDON);
    stmt->setUInt32(0, m_spawnId);
    trans2->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAME_EVENT_CREATURE);
    stmt->setUInt32(0, m_spawnId);
    trans2->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAME_EVENT_MODEL_EQUIP);
    stmt->setUInt32(0, m_spawnId);
    trans2->Append(stmt);

    WorldDatabase.CommitTransaction(trans2);
}

bool Creature::IsInvisibleDueToDespawn() const
{
    if (Unit::IsInvisibleDueToDespawn())
        return true;

    if (IsAlive() || m_corpseRemoveTime > time(NULL))
        return false;

    return true;
}

bool Creature::CanAlwaysSee(WorldObject const* obj) const
{
    if (IsAIEnabled && AI()->CanSeeAlways(obj))
        return true;

    return false;
}

bool Creature::CanStartAttack(Unit const* who, bool force) const
{
    if (IsCivilian())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    // Do not attack non-combat pets
    if (who->GetTypeId() == TYPEID_UNIT && who->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET)
        return false;

    if (!CanFly() && (GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE + m_CombatDistance))
        if (who->CanFly() || (std::tanh(std::abs(GetPositionZ() - who->GetPositionZ()) / GetExactDist2d(who)) > M_PI / 6)) // Tilt angle > 30 degrees, PROBABLY target is unreachable
            return false;

    if (!force)
    {
        if (!_IsTargetAcceptable(who))
            return false;

        if (who->IsInCombat() && IsWithinDist(who, ATTACK_DISTANCE))
            if (Unit* victim = who->getAttackerForHelper())
                if (IsWithinDistInMap(victim, sWorld->getFloatConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS)))
                    force = true;

        if (!force && (IsNeutralToAll() || !IsWithinDistInMap(who, GetAttackDistance(who) + m_CombatDistance)))
            return false;
    }

    if (!CanCreatureAttack(who, force))
        return false;

    return IsWithinLOSInMap(who);
}

float Creature::GetAttackDistance(Unit const* player) const
{
    if (m_ReactDistance)
        return m_ReactDistance;

    float aggroRate = sWorld->getRate(RATE_CREATURE_AGGRO);
    if (aggroRate == 0)
        return 0.0f;

    uint32 playerlevel   = player->GetLevelForTarget(this);
    uint32 creaturelevel = GetLevelForTarget(player);

    int32 leveldif       = int32(playerlevel) - int32(creaturelevel);

    // "The maximum Aggro Radius has a cap of 25 levels under. Example: A level 30 char has the same Aggro Radius of a level 5 char on a level 60 mob."
    if (leveldif < - 25)
        leveldif = -25;

    // "The aggro radius of a mob having the same level as the player is roughly 20 yards"
    float RetDistance = 20;

    // "Aggro Radius varies with level difference at a rate of roughly 1 yard/level"
    // radius grow if playlevel < creaturelevel
    RetDistance -= (float)leveldif;

    if (creaturelevel+5 <= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
    {
        // detect range auras
        RetDistance += GetTotalAuraModifier(SPELL_AURA_MOD_DETECT_RANGE);

        // detected range auras
        RetDistance += player->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);
    }

    // "Minimum Aggro Radius for a mob seems to be combat range (5 yards)"
    if (RetDistance < 5)
        RetDistance = 5;

    return (RetDistance*aggroRate);
}

void Creature::setDeathState(DeathState s)
{
    Unit::setDeathState(s);

    if (s == JUST_DIED)
    {
        m_corpseRemoveTime = time(NULL) + m_corpseDelay;

        m_respawnTime = time(NULL) + (m_respawnDelayMax ? urand(m_respawnDelay, m_respawnDelayMax) : m_respawnDelay) + m_corpseDelay;

        // always save boss respawn time at death to prevent crash cheating
        if (sWorld->getBoolConfig(CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY) || isWorldBoss())
            SaveRespawnTime();

        SetTarget(ObjectGuid::Empty);                // remove target selection in any cases (can be set at aura remove in Unit::setDeathState)
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_NONE);

        SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0); // if creature is mounted on a virtual mount, remove it at death

        setActive(false, ActiveFlags::InCombat);

        if (!IsPet() && GetCreatureTemplate()->SkinLootId)
            if (LootTemplates_Skinning.HaveLootFor(GetCreatureTemplate()->SkinLootId))
                if (HasLootRecipient())
                    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

        if (HasSearchedAssistance())
        {
            SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }

        //Dismiss group if is leader
        if (m_formation && m_formation->GetLeader() == this)
            m_formation->FormationReset(true);

        if ((CanFly() || IsFlying()))
            GetMotionMaster()->MoveFall();

        Unit::setDeathState(CORPSE);
    }
    else if (s == JUST_RESPAWNED)
    {
        //if (IsPet())
        //    setActive(true);
        SetFullHealth();
        SetLootRecipient(NULL);
        ResetPlayerDamageReq();

        UpdateMovementFlags(true);

        CreatureTemplate const* cinfo = GetCreatureTemplate();
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, cinfo->npcflag);
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, cinfo->npcflag2);
        ClearUnitState(uint32(UNIT_STATE_ALL_STATE & ~UNIT_STATE_IGNORE_PATHFINDING));
        SetMeleeDamageSchool(SpellSchools(cinfo->dmgschool));
        LoadCreaturesAddon(true);
        ApplyInstanceAuraIfNeeded();
        Motion_Initialize();
        if (GetCreatureData() && GetPhaseMask() != GetCreatureData()->phaseMask)
            SetPhaseMask(GetCreatureData()->phaseMask, false);
        Unit::setDeathState(ALIVE);
    }
}

void Creature::Respawn(bool force)
{
    DestroyForNearbyPlayers();

    if (force)
    {
        if (IsAlive())
            setDeathState(JUST_DIED);
        else if (getDeathState() != CORPSE)
            setDeathState(CORPSE);
    }

    RemoveCorpse(false);

    if (getDeathState() == DEAD)
    {
        if (m_spawnId)
            GetMap()->RemoveCreatureRespawnTime(m_spawnId);

        TC_LOG_DEBUG("entities.unit", "Respawning creature %s (GuidLow: %u, Full GUID: " UI64FMTD " Entry: %u)",
            GetName().c_str(), GetGUID().GetCounter(), GetGUID().GetRawValue(), GetEntry());
        m_respawnTime = 0;
        lootForPickPocketed = false;
        lootForBody         = false;

        if (m_originalEntry != GetEntry())
            UpdateEntry(m_originalEntry);

        CreatureTemplate const* cinfo = GetCreatureTemplate();
        SelectLevel(cinfo);

        setDeathState(JUST_RESPAWNED);


        CreatureModel display(GetNativeDisplayId(), 0, 1.0f);
        if (sObjectMgr->GetCreatureModelRandomGender(&display, GetCreatureTemplate()))
            SetDisplayId(display.CreatureDisplayID, true);

        sBattlePetSpawnMgr->OnRespawn(this);

        GetMotionMaster()->InitDefault();

        // Re-initialize reactstate that could be altered by movementgenerators
        InitializeReactState();

        if (UnitAI* ai = AI()) // reset the AI to be sure no dirty or uninitialized values will be used till next tick
            ai->Reset();

        m_triggerJustAppeared = true;//delay event to next tick so all creatures are created on the map before processing

        uint32 poolid = GetDBTableGUIDLow() ? sPoolMgr->IsPartOfAPool<Creature>(GetDBTableGUIDLow()) : 0;
        if (poolid)
            sPoolMgr->UpdatePool<Creature>(GetMap()->GetPoolData(), poolid, GetDBTableGUIDLow());
    }

    UpdateObjectVisibility();
}

void Creature::ForcedDespawn(uint32 timeMSToDespawn)
{
    if (timeMSToDespawn)
    {
        ForcedDespawnDelayEvent* pEvent = new ForcedDespawnDelayEvent(*this);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(timeMSToDespawn));
        return;
    }

    if (IsAlive())
        setDeathState(JUST_DIED);

    RemoveCorpse(false);
}

void Creature::DespawnOrUnsummon(uint32 msTimeToDespawn /*= 0*/)
{
    if (TempSummon* summon = this->ToTempSummon())
        summon->UnSummon(msTimeToDespawn);
    else
        ForcedDespawn(msTimeToDespawn);
}

bool Creature::IsImmunedToSpell(SpellInfo const* spellInfo, uint32 effectMask) const
{
    if (!spellInfo)
        return false;

    // Creature is immune to main mechanic of the spell
    if (!IsPet() && GetCreatureTemplate()->MechanicImmuneMask & (1 << (spellInfo->Mechanic - 1)))
        return true;

    // This check must be done instead of 'if (GetCreatureTemplate()->MechanicImmuneMask & (1 << (spellInfo->Mechanic - 1)))' for not break
    // the check of mechanic immunity on DB (tested) because GetCreatureTemplate()->MechanicImmuneMask and m_spellImmune[IMMUNITY_MECHANIC] don't have same data.
    bool immunedToAllEffects = true;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (!spellInfo->Effects[i].IsEffect())
            continue;
        if (!IsImmunedToSpellEffect(spellInfo, i))
        {
            immunedToAllEffects = false;
            break;
        }
    }
    if (immunedToAllEffects)
        return true;

    return Unit::IsImmunedToSpell(spellInfo, effectMask);
}

bool Creature::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const
{
    if (!IsPet() && GetCreatureTemplate()->MechanicImmuneMask & (1 << (spellInfo->Effects[index].Mechanic - 1)))
        return true;

    if (GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL && spellInfo->Effects[index].Effect == SPELL_EFFECT_HEAL)
        return true;

    if (spellInfo->Effects[index].ApplyAuraName == SPELL_AURA_SPELL_SLOW)
        if (GetMap()->IsDungeon() && (IsDungeonBoss() || isWorldBoss()))
            return true;

    return Unit::IsImmunedToSpellEffect(spellInfo, index);
}

bool Creature::isElite() const
{
    if (IsPet())
        return false;

    uint32 rank = GetCreatureTemplate()->rank;
    return rank != CREATURE_ELITE_NORMAL && rank != CREATURE_ELITE_RARE && rank != CREATURE_WEAK;
}

bool Creature::isWorldBoss() const
{
    if (IsPet())
        return false;

    return GetCreatureTemplate()->type_flags & CREATURE_TYPEFLAGS_BOSS;
}

SpellInfo const* Creature::reachWithSpellAttack(Unit* victim)
{
    if (!victim)
        return NULL;

    for (uint32 i=0; i < CREATURE_MAX_SPELLS; ++i)
    {
        if (!m_spells[i])
            continue;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(m_spells[i]);
        if (!spellInfo)
        {
            TC_LOG_ERROR("entities.unit", "WORLD: unknown spell id %i", m_spells[i]);
            continue;
        }

        bool bcontinue = true;
        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; j++)
        {
            if ((spellInfo->Effects[j].Effect == SPELL_EFFECT_SCHOOL_DAMAGE)       ||
                (spellInfo->Effects[j].Effect == SPELL_EFFECT_INSTAKILL)            ||
                (spellInfo->Effects[j].Effect == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE) ||
                (spellInfo->Effects[j].Effect == SPELL_EFFECT_HEALTH_LEECH)
                )
            {
                bcontinue = false;
                break;
            }
        }
        if (bcontinue)
            continue;

        auto powerEntry = spellInfo->GetSpellPowerEntry(this);
        if (powerEntry && powerEntry->Cost > (uint32)GetPower(Powers(powerEntry->PowerType)))
            continue;
        float range = spellInfo->GetMaxRange(false);
        float minrange = spellInfo->GetMinRange(false);
        float dist = GetDistance(victim);
        if (dist > range || dist < minrange)
            continue;
        if (spellInfo->PreventionType & SPELL_PREVENTION_TYPE_SILENCE && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        if (spellInfo->PreventionType & SPELL_PREVENTION_TYPE_PACIFY && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            continue;
        return spellInfo;
    }
    return NULL;
}

SpellInfo const* Creature::reachWithSpellCure(Unit* victim)
{
    if (!victim)
        return NULL;

    for (uint32 i=0; i < CREATURE_MAX_SPELLS; ++i)
    {
        if (!m_spells[i])
            continue;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(m_spells[i]);
        if (!spellInfo)
        {
            TC_LOG_ERROR("entities.unit", "WORLD: unknown spell id %i", m_spells[i]);
            continue;
        }

        bool bcontinue = true;
        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; j++)
        {
            if ((spellInfo->Effects[j].Effect == SPELL_EFFECT_HEAL))
            {
                bcontinue = false;
                break;
            }
        }
        if (bcontinue)
            continue;

        auto powerEntry = spellInfo->GetSpellPowerEntry(this);
        if (powerEntry && powerEntry->Cost > (uint32)GetPower(Powers(powerEntry->PowerType)))
            continue;

        float range = spellInfo->GetMaxRange(true);
        float minrange = spellInfo->GetMinRange(true);
        float dist = GetDistance(victim);
        //if (!isInFront(victim, range) && spellInfo->AttributesEx)
        //    continue;
        if (dist > range || dist < minrange)
            continue;
        if (spellInfo->PreventionType & SPELL_PREVENTION_TYPE_SILENCE && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        if (spellInfo->PreventionType & SPELL_PREVENTION_TYPE_PACIFY && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            continue;
        return spellInfo;
    }
    return NULL;
}

// select nearest hostile unit within the given distance (regardless of threat list).
Unit* Creature::SelectNearestTarget(float dist, bool playerOnly /* = false */) const
{
    CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* target = NULL;

    {
        if (dist == 0.0f)
            dist = MAX_VISIBILITY_DISTANCE;

        Trinity::NearestHostileUnitCheck u_check(this, dist, playerOnly);
        Trinity::UnitLastSearcher<Trinity::NearestHostileUnitCheck> searcher(this, target, u_check);

        TypeContainerVisitor<Trinity::UnitLastSearcher<Trinity::NearestHostileUnitCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<Trinity::UnitLastSearcher<Trinity::NearestHostileUnitCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, dist);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, dist);
    }

    return target;
}

// select nearest hostile unit within the given attack distance (i.e. distance is ignored if > than ATTACK_DISTANCE), regardless of threat list.
Unit* Creature::SelectNearestTargetInAttackDistance(float dist) const
{
    CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* target = NULL;

    if (dist > MAX_VISIBILITY_DISTANCE)
    {
        TC_LOG_ERROR("entities.unit", "Creature (GUID: %u Entry: %u) SelectNearestTargetInAttackDistance called with dist > MAX_VISIBILITY_DISTANCE. Distance set to ATTACK_DISTANCE.", GetGUID().GetCounter(), GetEntry());
        dist = ATTACK_DISTANCE;
    }

    {
        Trinity::NearestHostileUnitInAttackDistanceCheck u_check(this, dist);
        Trinity::UnitLastSearcher<Trinity::NearestHostileUnitInAttackDistanceCheck> searcher(this, target, u_check);

        TypeContainerVisitor<Trinity::UnitLastSearcher<Trinity::NearestHostileUnitInAttackDistanceCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<Trinity::UnitLastSearcher<Trinity::NearestHostileUnitInAttackDistanceCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE > dist ? ATTACK_DISTANCE : dist);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE > dist ? ATTACK_DISTANCE : dist);
    }

    return target;
}

Player* Creature::SelectNearestPlayer(float distance) const
{
    Player* target = NULL;

    Trinity::NearestPlayerInObjectRangeCheck checker(this, distance);
    Trinity::PlayerLastSearcher<Trinity::NearestPlayerInObjectRangeCheck> searcher(this, target, checker);
    VisitNearbyObject(distance, searcher);

    return target;
}

Player* Creature::SelectNearestPlayerNotGM(float distance) const
{
    Player* target = NULL;

    Trinity::NearestPlayerNotGMInObjectRangeCheck checker(this, distance);
    Trinity::PlayerLastSearcher<Trinity::NearestPlayerNotGMInObjectRangeCheck> searcher(this, target, checker);
    VisitNearbyObject(distance, searcher);

    return target;
}

void Creature::SendAIReaction(AiReaction reactionType)
{
    WorldPacket data(SMSG_AI_REACTION, 12);

    ObjectGuid guid = GetGUID();

    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);
    data << uint32(reactionType);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);

    ((WorldObject*)this)->SendMessageToSet(&data, true);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_AI_REACTION, type %u.", reactionType);
}

void Creature::CallAssistance()
{
    if (!m_AlreadyCallAssistance && GetVictim() && !IsPet() && !IsCharmed())
    {
        SetNoCallAssistance(true);

        float radius = sWorld->getFloatConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS);

        if (radius > 0)
        {
            std::list<Creature*> assistList;

            {
                CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
                Cell cell(p);
                cell.SetNoCreate();

                Trinity::AnyAssistCreatureInRangeCheck u_check(this, GetVictim(), radius);
                Trinity::CreatureListSearcher<Trinity::AnyAssistCreatureInRangeCheck> searcher(this, assistList, u_check);

                TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AnyAssistCreatureInRangeCheck>, GridTypeMapContainer >  grid_creature_searcher(searcher);

                cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
            }

            if (!assistList.empty())
            {
                AssistDelayEvent* e = new AssistDelayEvent(GetVictim()->GetGUID(), *this);
                while (!assistList.empty())
                {
                    // Pushing guids because in delay can happen some creature gets despawned => invalid pointer
                    e->AddAssistant((*assistList.begin())->GetGUID());
                    assistList.pop_front();
                }
                m_Events.AddEvent(e, m_Events.CalculateTime(sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY)));
            }
        }
    }
}

void Creature::CallForHelp(float radius)
{
    if (radius <= 0.0f || !GetVictim() || IsPet() || IsCharmed())
        return;

    CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Trinity::CallOfHelpCreatureInRangeDo u_do(this, GetVictim(), radius);
    Trinity::CreatureWorker<Trinity::CallOfHelpCreatureInRangeDo> worker(this, u_do);

    TypeContainerVisitor<Trinity::CreatureWorker<Trinity::CallOfHelpCreatureInRangeDo>, GridTypeMapContainer >  grid_creature_searcher(worker);

    cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
}

bool Creature::CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction /*= true*/) const
{
    if (IsInEvadeMode())
        return false;

    // is it true?
    if (!HasReactState(REACT_AGGRESSIVE))
        return false;

    // we don't need help from zombies :)
    if (!IsAlive())
        return false;

    // we don't need help from non-combatant ;)
    if (IsCivilian())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    // skip fighting creature
    if (IsInCombat())
        return false;

    // only free creature
    if (GetCharmerOrOwnerGUID())
        return false;

    // only from same creature faction
    if (checkfaction)
    {
        if (GetFaction() != u->GetFaction())
            return false;
    }
    else
    {
        if (!IsFriendlyTo(u))
            return false;
    }

    // skip non hostile to caster enemy creatures
    if (!IsHostileTo(enemy))
        return false;

    return true;
}

// use this function to avoid having hostile creatures attack
// friendlies and other mobs they shouldn't attack
bool Creature::_IsTargetAcceptable(const Unit* target) const
{
    ASSERT(target);

    // if the target cannot be attacked, the target is not acceptable
    if (IsFriendlyTo(target)
        || !target->isTargetableForAttack(false)
        || (m_vehicle && (IsOnVehicle(target) || m_vehicle->GetBase()->IsOnVehicle(target))))
        return false;

    if (target->HasUnitState(UNIT_STATE_DIED))
    {
        // guards can detect fake death
        if (IsGuard() && target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
            return true;
        else
            return false;
    }

    const Unit* myVictim = getAttackerForHelper();
    const Unit* targetVictim = target->getAttackerForHelper();

    // if I'm already fighting target, or I'm hostile towards the target, the target is acceptable
    if (myVictim == target || targetVictim == this || IsHostileTo(target))
        return true;

    // if the target's victim is friendly, and the target is neutral, the target is acceptable
    if (targetVictim && IsFriendlyTo(targetVictim))
        return true;

    // if the target's victim is not friendly, or the target is friendly, the target is not acceptable
    return false;
}

void Creature::SaveRespawnTime()
{
    if (IsSummon() || !m_spawnId || (m_creatureData && !m_creatureData->dbData))
        return;

    GetMap()->SaveCreatureRespawnTime(m_spawnId, m_respawnTime);
}

// this should not be called by petAI or
bool Creature::CanCreatureAttack(Unit const* victim, bool /*force*/) const
{
    if (!victim->IsInMap(this))
        return false;

    if (!IsValidAttackTarget(victim))
        return false;

    if (!victim->isInAccessiblePlaceFor(this))
        return false;

    if (IsAIEnabled && !AI()->CanAIAttack(victim))
        return false;

    if (sMapStore.LookupEntry(GetMapId())->IsDungeon())
        return true;

    bool inAcitveCombat = false;

    // I dunno how it must work, but...
    if (!isWorldBoss())
        if (victim->GetPvECombatTimer() > 5000)   // If target in combat - chase them longer
            inAcitveCombat = true;

    //Use AttackDistance in distance check if threat radius is lower. This prevents creature bounce in and out of combat every update tick.
    float dist = std::max(GetAttackDistance(victim), sWorld->getFloatConfig(CONFIG_THREAT_RADIUS)) + m_CombatDistance;

    if (inAcitveCombat)
        dist *= 1.7f;   // About 100yd

    if (Unit* unit = GetCharmerOrOwner())
        return victim->IsWithinDist(unit, dist);
    else
        return victim->IsInDist(&m_homePosition, dist);
}

CreatureAddon const* Creature::GetCreatureAddon() const
{
    if (m_spawnId)
    {
        if (CreatureAddon const* addon = sObjectMgr->GetCreatureAddon(m_spawnId))
            return addon;
    }

    // dependent from difficulty mode entry
    return sObjectMgr->GetCreatureTemplateAddon(GetCreatureTemplate()->Entry);
}

//creature_addon table
bool Creature::LoadCreaturesAddon(bool reload)
{
    CreatureAddon const* creatureAddon = GetCreatureAddon();
    if (!creatureAddon)
        return false;

    if (creatureAddon->mount != 0)
        Mount(creatureAddon->mount);

    // UNIT_FIELD_BYTES_1 values
    SetStandState(UnitStandStateType(creatureAddon->standState));
    SetAnimTier(AnimTier(creatureAddon->animTier));
    ReplaceAllVisFlags(UnitVisFlags(creatureAddon->visFlags));

    //! Suspected correlation between UNIT_FIELD_BYTES_1, offset 3, value 0x2:
    //! If no inhabittype_fly (if no MovementFlag_DisableGravity or MovementFlag_CanFly flag found in sniffs)
    //! Check using InhabitType as movement flags are assigned dynamically
    //! basing on whether the creature is in air or not
    //! Set MovementFlag_Hover. Otherwise do nothing.
    if (CanHover())
        AddUnitMovementFlag(MOVEMENTFLAG_HOVER);

    // UNIT_FIELD_BYTES_2 values
    SetSheath(SheathState(creatureAddon->sheathState));
    ReplaceAllPvpFlags(UnitPVPStateFlags(creatureAddon->pvpFlags));

    // These fields must only be handled by core internals and must not be modified via scripts/DB data
    ReplaceAllPetFlags(UNIT_PET_FLAG_NONE);
    SetShapeshiftForm(FORM_NONE);

    if (creatureAddon->emote != 0)
        SetEmoteState(Emote(creatureAddon->emote));

    // Check if visibility distance different
    if (creatureAddon->visibilityDistanceType != VisibilityDistanceType::Normal)
        SetVisibilityDistanceOverride(creatureAddon->visibilityDistanceType);

    // Load Path
    if (creatureAddon->path_id != 0)
        m_path_id = creatureAddon->path_id;

    if (!creatureAddon->auras.empty())
    {
        for (std::vector<uint32>::const_iterator itr = creatureAddon->auras.begin(); itr != creatureAddon->auras.end(); ++itr)
        {
            SpellInfo const* AdditionalSpellInfo = sSpellMgr->GetSpellInfo(*itr);
            if (!AdditionalSpellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Creature %s has wrong spell %u defined in `auras` field.", GetGUID().ToString().c_str(), *itr);
                continue;
            }

            // skip already applied aura
            if (HasAura(*itr))
                continue;

            AddAura(*itr, this);
            TC_LOG_DEBUG("entities.unit", "Spell: %u added to creature %s", *itr, GetGUID().ToString().c_str());
        }
    }

    return true;
}

/// Send a message to LocalDefense channel for players opposition team in the zone
void Creature::SendZoneUnderAttackMessage(Player* attacker)
{
    uint32 enemy_team = attacker->GetTeam();

    WorldPacket data(SMSG_ZONE_UNDER_ATTACK, 4);
    data << uint32(GetAreaId());
    sWorld->SendGlobalMessage(&data, NULL, (enemy_team == ALLIANCE ? HORDE : ALLIANCE));
}

void Creature::SetInCombatWithZone()
{
    if (!CanHaveThreatList())
    {
        TC_LOG_ERROR("entities.unit", "Creature entry %u call SetInCombatWithZone but creature cannot have threat list.", GetEntry());
        return;
    }

    Map* map = GetMap();

    if (!map->IsDungeon())
    {
        TC_LOG_ERROR("entities.unit", "Creature entry %u call SetInCombatWithZone for map (id: %u) that isn't an instance.", GetEntry(), map->GetId());
        return;
    }

    Map::PlayerList const &PlList = map->GetPlayers();

    if (PlList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
    {
        if (Player* player = i->GetSource())
        {
            if (player->IsGameMaster())
                continue;

            if (player->IsAlive())
            {
                this->SetInCombatWith(player);
                player->SetInCombatWith(this);
                AddThreat(player, 0.0f);
            }
        }
    }
}

void Creature::_AddCreatureSpellCooldown(uint32 spell_id, time_t end_time)
{
    m_CreatureSpellCooldowns[spell_id] = end_time;
}

void Creature::_AddCreatureCategoryCooldown(uint32 category, time_t apply_time)
{
    m_CreatureCategoryCooldowns[category] = apply_time;
}

void Creature::AddCreatureSpellCooldown(uint32 spellid)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
    if (!spellInfo)
        return;

    uint32 cooldown = spellInfo->GetRecoveryTime();
    if (Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellid, SPELLMOD_COOLDOWN, cooldown);

    if (cooldown)
        _AddCreatureSpellCooldown(spellid, time(NULL) + cooldown/IN_MILLISECONDS);

    if (spellInfo->GetCategory())
        _AddCreatureCategoryCooldown(spellInfo->GetCategory(), time(NULL));
}

bool Creature::HasCategoryCooldown(uint32 spell_id) const
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
    if (!spellInfo)
        return false;

    CreatureSpellCooldowns::const_iterator itr = m_CreatureCategoryCooldowns.find(spellInfo->GetCategory());
    return (itr != m_CreatureCategoryCooldowns.end() && time_t(itr->second + (spellInfo->CategoryRecoveryTime / IN_MILLISECONDS)) > time(NULL));
}

uint32 Creature::GetCreatureSpellCooldownDelay(uint32 spellId) const
{
    CreatureSpellCooldowns::const_iterator itr = m_CreatureSpellCooldowns.find(spellId);
    time_t t = time(NULL);
    return uint32(itr != m_CreatureSpellCooldowns.end() && itr->second > t ? itr->second - t : 0);
}

bool Creature::HasSpellCooldown(uint32 spell_id) const
{
    CreatureSpellCooldowns::const_iterator itr = m_CreatureSpellCooldowns.find(spell_id);
    return (itr != m_CreatureSpellCooldowns.end() && itr->second > time(NULL)) || HasCategoryCooldown(spell_id);
}

void Creature::ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs)
{
    time_t curTime = time(NULL);
    for (uint8 i = 0; i < CREATURE_MAX_SPELLS; ++i)
    {
        if (m_spells[i] == 0)
            continue;

        uint32 unSpellId = m_spells[i];
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

        if ((idSchoolMask & spellInfo->GetSchoolMask()) && GetCreatureSpellCooldownDelay(unSpellId) < unTimeMs)
        {
            _AddCreatureSpellCooldown(unSpellId, curTime + unTimeMs/IN_MILLISECONDS);
            if (UnitAI* ai = GetAI())
                ai->SpellInterrupted(unSpellId, unTimeMs);
        }
    }
}

void Creature::PrepareChanneledCast(float facing, uint32 spell_id, bool triggered)
{
    AttackStop();
    SetReactState(REACT_PASSIVE);
    SetFacingTo(facing);

    if (spell_id)
        CastSpell(this, spell_id, triggered);
}

void Creature::RemoveChanneledCast(ObjectGuid target)
{
    SetReactState(REACT_AGGRESSIVE);

    if (Unit* itr = ObjectAccessor::GetUnit(*this, target))
    {
        GetMotionMaster()->MoveChase(itr);
        Attack(itr, true);
    }
    else if (Player* itr = FindNearestPlayer(100.0f))
    {
        GetMotionMaster()->MoveChase(itr);
        Attack(itr, true);
    }
}

void Creature::FixateOnTarget(ObjectGuid targetGUID, uint32 timer)
{
    if (Unit* target = ObjectAccessor::GetUnit(*this, targetGUID))
    {
        GetThreatManager().clearReferences();
        GetThreatManager().addThreat(target, std::numeric_limits<float>::max());

        if (AI())
            AI()->AttackStart(target);

        TauntApply(target);

        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);

        // If timer not null then remove immune to taunt by value
        if (timer)
        {
            uint32 delay = 0;
            m_Events.Schedule(delay += timer, 10, [this]()
            {
                ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, false);
            });
        }

    }
}

bool Creature::HasSpell(uint32 spellID) const
{
    uint8 i;
    for (i = 0; i < CREATURE_MAX_SPELLS; ++i)
        if (spellID == m_spells[i])
            break;
    return i < CREATURE_MAX_SPELLS;                         //broke before end of iteration of known spells
}

time_t Creature::GetRespawnTimeEx() const
{
    time_t now = time(NULL);
    if (m_respawnTime > now)
        return m_respawnTime;
    else
        return now;
}

void Creature::GetRespawnPosition(float &x, float &y, float &z, float* ori, float* dist) const
{
    if (m_spawnId)
    {
        if (CreatureData const* data = sObjectMgr->GetCreatureData(GetDBTableGUIDLow()))
        {
            x = data->posX;
            y = data->posY;
            z = data->posZ;
            if (ori)
                *ori = data->orientation;
            if (dist)
                *dist = data->wander_distance;

            return;
        }
    }

    x = GetPositionX();
    y = GetPositionY();
    z = GetPositionZ();
    if (ori)
        *ori = GetOrientation();
    if (dist)
        *dist = 0;
}

CreatureMovementData const& Creature::GetMovementTemplate() const
{
    if (CreatureMovementData const* movementOverride = sObjectMgr->GetCreatureMovementOverride(m_spawnId))
        return *movementOverride;

    return GetCreatureTemplate()->Movement;
}

bool Creature::CanSwim() const
{
    if (Unit::CanSwim() || (!Unit::CanSwim() && !CanFly()))
        return true;

    if (IsPet())
        return true;

    return false;
}

void Creature::RefreshSwimmingFlag(bool recheck)
{
    if (!_isMissingSwimmingFlagOutOfCombat || recheck)
        _isMissingSwimmingFlagOutOfCombat = !HasUnitFlag(UNIT_FLAG_CAN_SWIM);

    // Check if the creature has UNIT_FLAG_CAN_SWIM and add it if it's missing
    // Creatures must be able to chase a target in water if they can enter water
    if (_isMissingSwimmingFlagOutOfCombat && CanEnterWater())
        SetUnitFlag(UNIT_FLAG_CAN_SWIM);
}

bool Creature::CanEnterWater() const 
{
    if (CanSwim())
        return true;

    return GetMovementTemplate().IsSwimAllowed();
}

void Creature::AllLootRemovedFromCorpse()
{
    if (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
    {
        time_t now = time(NULL);
        if (m_corpseRemoveTime <= now)
            return;

        float decayRate;
        CreatureTemplate const* cinfo = GetCreatureTemplate();

        decayRate = sWorld->getRate(RATE_CORPSE_DECAY_LOOTED);
        uint32 diff = uint32((m_corpseRemoveTime - now) * decayRate);

        m_respawnTime -= diff;

        // corpse skinnable, but without skinning flag, and then skinned, corpse will despawn next update
        if (cinfo && cinfo->SkinLootId)
            m_corpseRemoveTime = time(NULL);
        else
            m_corpseRemoveTime -= diff;
    }
}

uint8 Creature::GetLevelForTarget(WorldObject const* target) const
{
    if (!isWorldBoss() || !target->ToUnit())
        return Unit::GetLevelForTarget(target);

    uint16 level = target->ToUnit()->GetLevel() + sWorld->getIntConfig(CONFIG_WORLD_BOSS_LEVEL_DIFF);
    if (level < 1)
        return 1;
    if (level > 255)
        return 255;
    return uint8(level);
}

std::string const& Creature::GetAIName() const
{
    return sObjectMgr->GetCreatureTemplate(GetEntry())->AIName;
}

std::string Creature::GetScriptName() const
{
    return sObjectMgr->GetScriptName(GetScriptId());
}

uint32 Creature::GetScriptId() const
{
    if (CreatureData const* creatureData = GetCreatureData())
        if (uint32 scriptId = creatureData->ScriptId)
            return scriptId;

    return ASSERT_NOTNULL(sObjectMgr->GetCreatureTemplate(GetEntry()))->ScriptID;
}

VendorItemData const* Creature::GetVendorItems() const
{
    return sObjectMgr->GetNpcVendorItemList(m_overrideVendorEntry ? m_overrideVendorEntry : GetEntry());
}

uint32 Creature::GetVendorItemCurrentCount(VendorItem const* vItem)
{
    if (!vItem->maxcount)
        return vItem->maxcount;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
        return vItem->maxcount;

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(NULL);

    if (time_t(vCount->lastIncrementTime + vItem->incrtime) <= ptime)
        if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(vItem->item))
        {
            uint32 diff = uint32((ptime - vCount->lastIncrementTime)/vItem->incrtime);
            if ((vCount->count + diff * pProto->BuyCount) >= vItem->maxcount)
            {
                m_vendorItemCounts.erase(itr);
                return vItem->maxcount;
            }

            vCount->count += diff * pProto->BuyCount;
            vCount->lastIncrementTime = ptime;
        }

    return vCount->count;
}

uint32 Creature::UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count)
{
    if (!vItem->maxcount)
        return 0;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
    {
        uint32 new_count = vItem->maxcount > used_count ? vItem->maxcount-used_count : 0;
        m_vendorItemCounts.push_back(VendorItemCount(vItem->item, new_count));
        return new_count;
    }

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(NULL);

    if (time_t(vCount->lastIncrementTime + vItem->incrtime) <= ptime)
        if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(vItem->item))
        {
            uint32 diff = uint32((ptime - vCount->lastIncrementTime)/vItem->incrtime);
            if ((vCount->count + diff * pProto->BuyCount) < vItem->maxcount)
                vCount->count += diff * pProto->BuyCount;
            else
                vCount->count = vItem->maxcount;
        }

    vCount->count = vCount->count > used_count ? vCount->count-used_count : 0;
    vCount->lastIncrementTime = ptime;
    return vCount->count;
}

TrainerSpellData const* Creature::GetTrainerSpells() const
{
    return sObjectMgr->GetNpcTrainerSpells(GetEntry());
}

// overwrite WorldObject function for proper name localization
std::string const & Creature::GetNameForLocaleIdx(LocaleConstant loc_idx) const
{
    if (loc_idx != DEFAULT_LOCALE)
    {
        uint8 uloc_idx = uint8(loc_idx);
        CreatureLocale const* cl = sObjectMgr->GetCreatureLocale(GetEntry());
        if (cl)
        {
            if (cl->Name.size() > uloc_idx && !cl->Name[uloc_idx].empty())
                return cl->Name[uloc_idx];
        }
    }

    return GetName();
}

uint32 Creature::GetPetAutoSpellOnPos(uint8 pos) const
{
    if (pos >= MAX_SPELL_CHARM || m_charmInfo->GetCharmSpell(pos)->GetType() != ACT_ENABLED)
        return 0;
    else
        return m_charmInfo->GetCharmSpell(pos)->GetAction();
}

void Creature::SetCannotReachTarget(bool cannotReach)
{
    if (cannotReach == m_cannotReachTarget)
        return;
    m_cannotReachTarget = cannotReach;
    m_cannotReachTimer = 0;

    if (cannotReach)
        TC_LOG_DEBUG("entities.unit.chase", "Creature::SetCannotReachTarget() called with true");
        // TC_LOG_DEBUG("entities.unit.chase", "Creature::SetCannotReachTarget() called with true. Details: s%", GetDebugInfo()); // TODO GetDebugInfo missing
}

void Creature::SetPosition(float x, float y, float z, float o)
{
    // prevent crash when a bad coord is sent by the client
    if (!Trinity::IsValidMapCoord(x, y, z, o))
    {
        TC_LOG_DEBUG("entities.unit", "Creature::SetPosition(%f, %f, %f) .. bad coordinates!", x, y, z);
        return;
    }

    GetMap()->CreatureRelocation(ToCreature(), x, y, z, o);
    if (IsVehicle())
        GetVehicleKit()->RelocatePassengers();
}

bool Creature::IsDungeonBoss() const
{
    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(GetEntry());
    return cinfo && (cinfo->flags_extra & CREATURE_FLAG_EXTRA_DUNGEON_BOSS);
}

float Creature::GetAggroRange(Unit const* target) const
{
    // Determines the aggro range for creatures (usually pets), used mainly for aggressive pet target selection.
    // Based on data from wowwiki due to lack of 3.3.5a data

    if (target && this->IsPet())
    {
        uint32 targetLevel = 0;

        if (target->GetTypeId() == TYPEID_PLAYER)
            targetLevel = target->GetLevelForTarget(this);
        else if (target->GetTypeId() == TYPEID_UNIT)
            targetLevel = target->ToCreature()->GetLevelForTarget(this);

        uint32 myLevel = GetLevelForTarget(target);
        int32 levelDiff = int32(targetLevel) - int32(myLevel);

        // The maximum Aggro Radius is capped at 45 yards (25 level difference)
        if (levelDiff < -25)
            levelDiff = -25;

        // The base aggro radius for mob of same level
        float aggroRadius = 20;

        // Aggro Radius varies with level difference at a rate of roughly 1 yard/level
        aggroRadius -= (float)levelDiff;

        // detect range auras
        aggroRadius += GetTotalAuraModifier(SPELL_AURA_MOD_DETECT_RANGE);

        // detected range auras
        aggroRadius += target->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);

        // Just in case, we don't want pets running all over the map
        if (aggroRadius > MAX_AGGRO_RADIUS)
            aggroRadius = MAX_AGGRO_RADIUS;

        // Minimum Aggro Radius for a mob seems to be combat range (5 yards)
        //  hunter pets seem to ignore minimum aggro radius so we'll default it a little higher
        if (aggroRadius < 10)
            aggroRadius = 10;

        return (aggroRadius);
    }

    // Default
    return 0.0f;
}

Unit* Creature::SelectNearestHostileUnitInAggroRange(bool useLOS) const
{
    // Selects nearest hostile target within creature's aggro range. Used primarily by
    //  pets set to aggressive. Will not return neutral or friendly targets.

    Unit* target = NULL;

    {
        Trinity::NearestHostileUnitInAggroRangeCheck u_check(this, useLOS);
        Trinity::UnitSearcher<Trinity::NearestHostileUnitInAggroRangeCheck> searcher(this, target, u_check);

        VisitNearbyGridObject(MAX_AGGRO_RADIUS, searcher);
    }

    return target;
}

void Creature::UpdateMovementFlags(bool force)
{
    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) || IsPet() || GetTransport())
        return;

    // It should be rather rare for creatures to transition between ground, water and air with random movement,
    // and since most of the creatures in the world will be using it - we can reduce strain on performance by skipping it.
    if (GetMotionMaster()->GetCurrentMovementGeneratorType() == RANDOM_MOTION_TYPE && !force)
        return;

    // Don't change flags if the creature is affected by effect motion: it is probably not in control of its movement.
    if (GetMotionMaster()->GetCurrentMovementGeneratorType() == EFFECT_MOTION_TYPE && !force)
        return;

    // Don't update flags if position hasn't changed 
    if (m_lastMovementFlagsUpdatePosition.m_positionX == m_positionX &&
        m_lastMovementFlagsUpdatePosition.m_positionY == m_positionY &&
        m_lastMovementFlagsUpdatePosition.m_positionZ == m_positionZ && !force)
        return;

    m_lastMovementFlagsUpdatePosition.Relocate(this);

    // Set the movement flags if the creature is in that mode. (Only fly if actually in air, only swim if in water, etc)
    float ground = GetFloorZ();

    bool isInAir = (G3D::fuzzyGt(GetPositionZMinusOffset(), ground + 0.05f) || G3D::fuzzyLt(GetPositionZMinusOffset(), ground - 0.05f)); // Can be underground too, prevent the falling

    if (CanFly() && (!IsFalling() || force))
    {
        SetFlying(isInAir || !CanWalk());
        SetCanFly(true);
    }
    else
    {
        SetFlying(false);
        SetCanFly(false);
    }

    if (!isInAir)
        SetFall(false);

    SetSwim(CanSwim() && IsInWater());
}

void Creature::SetFlying(bool enable)
{
    if (enable)
        SetAnimTier(AnimTier::Fly);
    else if (IsHovering())
        SetAnimTier(AnimTier::Hover);
    else
        SetAnimTier(AnimTier::Ground);

    SetDisableGravity(enable);
}

void Creature::SetObjectScale(float scale)
{
    Unit::SetObjectScale(scale);

    if (CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelInfo(GetDisplayId()))
    {
        SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, minfo->bounding_radius * scale);
        SetFloatValue(UNIT_FIELD_COMBAT_REACH, minfo->combat_reach * scale);
    }
}

void Creature::SetDisplayId(uint32 modelId, float displayScale /*= 1.f*/)
{
    Unit::SetDisplayId(modelId, displayScale);

    if (CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelInfo(modelId))
    {
        SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, minfo->bounding_radius * GetObjectScale());
        SetFloatValue(UNIT_FIELD_COMBAT_REACH, minfo->combat_reach * GetObjectScale());
    }
}

void Creature::SetDisplayFromModel(uint32 modelIdx)
{
    if (CreatureModel const* model = GetCreatureTemplate()->GetModelByIdx(modelIdx))
        SetDisplayId(model->CreatureDisplayID);
}

void Creature::SetTarget(ObjectGuid guid)
{
    if (!_focusSpell)
        SetGuidValue(UNIT_FIELD_TARGET, guid);
}

void Creature::FocusTarget(Spell const* focusSpell, WorldObject const* target)
{
    // already focused
    if (_focusSpell)
        return;

    _focusSpell = focusSpell;
    SetGuidValue(UNIT_FIELD_TARGET, target->GetGUID());
    if (focusSpell && focusSpell->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_DONT_TURN_DURING_CAST)
        AddUnitState(UNIT_STATE_ROTATING);

    // Set serverside orientation if needed (needs to be after attribute check)
    SetInFront(target);
}

void Creature::ReleaseFocus(Spell const* focusSpell)
{
    // focused to something else
    if (focusSpell != _focusSpell)
        return;

    _focusSpell = NULL;
    if (Unit* victim = GetVictim())
        SetGuidValue(UNIT_FIELD_TARGET, victim->GetGUID());
    else
        SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid::Empty);

    if (focusSpell && focusSpell->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_DONT_TURN_DURING_CAST)
        ClearUnitState(UNIT_STATE_ROTATING);
}

void Creature::SetTextRepeatId(uint8 textGroup, uint8 id)
{
    CreatureTextRepeatIds& repeats = m_textRepeat[textGroup];
    if (std::find(repeats.begin(), repeats.end(), id) == repeats.end())
        repeats.push_back(id);
    else
        TC_LOG_ERROR("sql.sql", "CreatureTextMgr: TextGroup %u for Creature(%s) GuidLow %u Entry %u, id %u already added", uint32(textGroup), GetName().c_str(), GetGUID().GetCounter(), GetEntry(), uint32(id));
}

CreatureTextRepeatIds Creature::GetTextRepeatGroup(uint8 textGroup)
{
    CreatureTextRepeatIds ids;

    CreatureTextRepeatGroup::const_iterator groupItr = m_textRepeat.find(textGroup);
    if (groupItr != m_textRepeat.end())
        ids = groupItr->second;

    return ids;
}

void Creature::ClearTextRepeatGroup(uint8 textGroup)
{
    CreatureTextRepeatGroup::iterator groupItr = m_textRepeat.find(textGroup);
    if (groupItr != m_textRepeat.end())
        groupItr->second.clear();
}

void Creature::ApplyInstanceAuraIfNeeded()
{
    uint32 aura = 0;
    if (IsSpawnedInLFRDungeon())
        aura = 109247;

    if (auto instance = GetInstanceScript())
        instance->DoDampeningForCreatures(this);

    if (GetMap()->IsBattleground())
    {
        // hack for crossfaction
        // Stormpike Clan to Alliance Generic
        if (GetCreatureTemplate()->faction == 1216 || GetCreatureTemplate()->faction == 1217 || GetCreatureTemplate()->faction == 1334 || GetCreatureTemplate()->faction == 1534 || GetCreatureTemplate()->faction == 1596)
            aura = 81748;
        // Frostwolf Clan to Horde Generic
        if (GetCreatureTemplate()->faction == 1214 || GetCreatureTemplate()->faction == 1215 || GetCreatureTemplate()->faction == 1335 || GetCreatureTemplate()->faction == 1554 || GetCreatureTemplate()->faction == 1597)
            aura = 81744;
    }

    if (aura && !HasAura(aura))
    {
        if (Pet* me = ToPet())
            if (Player* player = me->GetOwner())
                return;
 
        if (TempSummon* me = ToTempSummon())
            if (me->GetSummonerGUID().IsPlayer())
                return;

        AddAura(aura, this);
    }
}

void Creature::Regenerate()
{
    if (m_regenTimer < CREATURE_REGEN_INTERVAL)
        return;

    bool bInCombat = IsInCombat() && (!GetVictim() ||                           // if IsInCombat() is true and this has no victim
        !GetVictim()->GetCharmerOrOwnerPlayerOrPlayerItself() ||                // or the victim/owner/charmer is not a player
        !GetVictim()->GetCharmerOrOwnerPlayerOrPlayerItself()->IsGameMaster()); // or the victim/owner/charmer is not a GameMaster

    if (!IsInEvadeMode() && (!bInCombat || IsPolymorphed())) // regenerate health if not in combat or if polymorphed
        if (isRegeneratingHealth())
            RegenerateHealth();

    switch (GetPowerType())
    {
        case POWER_ENERGY:
            if (!IsVehicle() || GetVehicleKit()->GetVehicleInfo()->m_powerType != POWER_PYRITE)
                Regenerate(POWER_ENERGY);
            break;
        case POWER_FOCUS:
            Regenerate(POWER_FOCUS);
            break;
        case POWER_MANA:
            RegenerateMana();
            break;
        default:
            break;
    }

    m_regenTimer -= CREATURE_REGEN_INTERVAL;
}

void Creature::Regenerate(Powers power)
{
    uint32 curValue = GetPower(power);
    uint32 maxValue = GetMaxPower(power);

    if (curValue >= maxValue)
        return;

    float addvalue = 0.0f;

    switch (power)
    {
        case POWER_FOCUS:
        {
            // For hunter pets.
            addvalue = 24 * sWorld->getRate(RATE_POWER_FOCUS);
            break;
        }
        case POWER_ENERGY:
        {
            // For deathknight's ghoul.
            addvalue = 20;
            break;
        }
        default:
            return;
    }

    for (auto&& it : GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT))
        if (Powers(it->GetMiscValue()) == power)
            AddPct(addvalue, it->GetAmount());

    addvalue += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, power) * CREATURE_REGEN_INTERVAL / (5 * IN_MILLISECONDS);

    ModifyPower(power, int32(addvalue));
}

void Creature::RecalculateDynamicHealth(uint32 newHealth)
{
    if (!IsAlive())
        return;

    if (newHealth != GetMaxHealth())
    {
        // get already done damage and check overflow
        uint32 damage = GetMaxHealth() - GetHealth();
        if (damage > newHealth)
            damage = newHealth - 1;

        SetCreateHealth(newHealth);
        SetMaxHealth(newHealth);
        SetHealth(newHealth - damage);
    }
}

float Creature::GetSparringHealthLimit() const
{
    return sObjectMgr->GetSparringHealthLimitFor(GetEntry());
}
