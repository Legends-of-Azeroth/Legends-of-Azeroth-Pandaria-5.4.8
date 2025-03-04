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

#include "GameObjectAI.h"
#include "Battleground.h"
#include "CellImpl.h"
#include "CreatureAISelector.h"
#include "DynamicTree.h"
#include "GameObjectModel.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Guild.h"
#include "IteratorPair.h"
#include "ObjectMgr.h"
#include "OutdoorPvPMgr.h"
#include "PoolMgr.h"
#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "UpdateFieldFlags.h"
#include "World.h"
#include "Transport.h"
#include "DisableMgr.h"
#include <G3D/Box.h>
#include <G3D/CoordinateFrame.h>

GameObject::GameObject() : WorldObject(false), MapObject(),
    m_model(NULL), m_goValue(), m_AI(NULL), _animKitId(0)
{
    m_objectType |= TYPEMASK_GAMEOBJECT;
    m_objectTypeId = TYPEID_GAMEOBJECT;

    m_updateFlag = (UPDATEFLAG_STATIONARY_POSITION | UPDATEFLAG_ROTATION);

    m_valuesCount = GAMEOBJECT_END;
    m_respawnTime = 0;
    m_respawnDelayTime = 300;
    m_lootState = GO_NOT_READY;
    m_spawnedByDefault = true;
    m_usetimes = 0;
    m_spellId = 0;
    m_cooldownTime = 0;
    m_prevGoState = GO_STATE_ACTIVE;
    m_goInfo = NULL;
    m_ritualOwner = NULL;
    m_goData = NULL;
    m_goTemplateAddon = nullptr;
    m_packedRotation = 0;

    m_DBTableGuid = 0;

    m_lootRecipient = ObjectGuid::Empty;
    m_lootRecipientGroup = 0;
    m_groupLootTimer = 0;
    lootingGroupLowGUID = 0;

    ResetLootMode(); // restore default loot mode
    m_stationaryPosition.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
}

GameObject::~GameObject()
{
    delete m_AI;
    delete m_model;
    //if (m_uint32Values)                                      // field array can be not exist if GameOBject not loaded
    //    CleanupsBeforeDelete();
}

bool ForcedGoDespawnDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.ForcedDespawn(); // since we are here, we are not TempSummon as object type cannot change during runtime
    return true;
}

bool GameObject::AIM_Initialize()
{
    if (m_AI)
        delete m_AI;

    m_AI = FactorySelector::SelectGameObjectAI(this);

    if (!m_AI)
        return false;

    m_AI->InitializeAI();
    return true;
}

std::string GameObject::GetAIName() const
{
    if (GameObjectTemplate const* got = sObjectMgr->GetGameObjectTemplate(GetEntry()))
        return got->AIName;

    return "";
}

void GameObject::CleanupsBeforeDelete(bool finalCleanup)
{
    WorldObject::CleanupsBeforeDelete(finalCleanup);

    if (IsInWorld())
        RemoveFromWorld();

    if (m_uint32Values)                                      // field array can be not exist if GameOBject not loaded
        RemoveFromOwner();
}

void GameObject::RemoveFromOwner()
{
    ObjectGuid ownerGUID = GetOwnerGUID();
    if (ownerGUID.IsEmpty())
        return;

    if (Unit* owner = ObjectAccessor::GetUnit(*this, ownerGUID))
    {
        owner->RemoveGameObject(this, false);
        ASSERT(!GetOwnerGUID());
        return;
    }

    const char * ownerType = "creature";
    if (ownerGUID.IsPlayer())
        ownerType = "player";
    else if (ownerGUID.IsPet())
        ownerType = "pet";

    TC_LOG_FATAL("misc", "Removed GameObject (GUID: %u Entry: %u SpellId: %u LinkedGO: %u) that just lost any reference to the owner (GUID: %u Type: '%s') GO list",
        GetGUID().GetCounter(), GetGOInfo()->entry, m_spellId, GetGOInfo()->GetLinkedGameObjectEntry(), ownerGUID.GetCounter(), ownerType);
    SetOwnerGUID(ObjectGuid::Empty);
}

void GameObject::AddToWorld()
{
    ///- Register the gameobject for guid lookup
    if (!IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnGameObjectCreate(this);

        GetMap()->GetObjectsStore().Insert<GameObject>(GetGUID(), this);
        if (m_DBTableGuid) // m_spawnId
            GetMap()->GetGameObjectBySpawnIdStore().insert(std::make_pair(m_DBTableGuid, this));

        if (m_model)
        {
            if (Transport* trans = ToTransport())
                trans->SetDelayedAddModelToMap();
            else
                GetMap()->InsertGameObjectModel(*m_model);
        }

        UpdateCollision();
        WorldObject::AddToWorld();
    }
}

void GameObject::RemoveFromWorld()
{
    ///- Remove the gameobject from the accessor
    if (IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->GameObjectRemoved(this);

        RemoveFromOwner();
        if (m_model)
            if (GetMap()->ContainsGameObjectModel(*m_model))
                GetMap()->RemoveGameObjectModel(*m_model);

        WorldObject::RemoveFromWorld();

        if (m_DBTableGuid) // m_spawnId
            Trinity::Containers::MultimapErasePair(GetMap()->GetGameObjectBySpawnIdStore(), m_DBTableGuid, this);

        GetMap()->GetObjectsStore().Remove<GameObject>(GetGUID());
    }
}

bool GameObject::Create(ObjectGuid::LowType guidlow, uint32 name_id, Map* map, uint32 phaseMask, float x, float y, float z, float ang, G3D::Quat const& rotation, uint32 animprogress, GOState go_state, uint32 artKit)
{
    ASSERT(map);
    SetMap(map);

    Relocate(x, y, z, ang);
    m_stationaryPosition.Relocate(x, y, z, ang);
    if (!IsPositionValid())
    {
        TC_LOG_ERROR("misc", "Gameobject (GUID: %u Entry: %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", guidlow, name_id, x, y);
        return false;
    }

    SetPhaseMask(phaseMask, false);
    UpdatePositionData();

    SetZoneScript();
    if (m_zoneScript)
    {
        name_id = m_zoneScript->GetGameObjectEntry(guidlow, name_id);
        if (!name_id)
            return false;
    }

    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(name_id);
    if (!goinfo)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u Entry: %u) not created: non-existing entry in `gameobject_template`. Map: %u (X: %f Y: %f Z: %f)", guidlow, name_id, map->GetId(), x, y, z);
        return false;
    }

    if (goinfo->type == GAMEOBJECT_TYPE_MO_TRANSPORT)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u Entry: %u) not created: gameobject type GAMEOBJECT_TYPE_MO_TRANSPORT cannot be manually created.", guidlow, name_id);
        return false;
    }

    if (goinfo->type == GAMEOBJECT_TYPE_TRANSPORT && goinfo->entry != 216666)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u Entry: %u) not created: gameobject type GAMEOBJECT_TYPE_TRANSPORT cannot be manually created.", guidlow, name_id);
        return false;
    }

    Object::_Create(guidlow, goinfo->entry, HighGuid::GameObject);

    m_goInfo = goinfo;
    m_goTemplateAddon = sObjectMgr->GetGameObjectTemplateAddon(name_id);

    if (goinfo->type >= MAX_GAMEOBJECT_TYPE)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u Entry: %u) not created: non-existing GO type '%u' in `gameobject_template`. It will crash client if created.", guidlow, name_id, goinfo->type);
        return false;
    }

    // I-Core: Sanitize rotation: if only Z axis rotation is present in DB or the data is invalid (not unit quaternion) - just use orientation instead
    if (!rotation.x && !rotation.y || !rotation.isUnit())
        SetWorldRotation({ G3D::Matrix3::fromEulerAnglesZYX(GetOrientation(), 0.0f, 0.0f) });
    else
        SetWorldRotation(rotation);

    // For most of gameobjects is (0, 0, 0, 1) quaternion, there are only some transports with not standard rotation
    G3D::Quat parentRotation;
    //GameObjectAddon const* gameObjectAddon = sObjectMgr->GetGameObjectAddon(GetSpawnId());

    //if (gameObjectAddon)
    //    parentRotation = gameObjectAddon->ParentRotation;

    SetParentRotation(parentRotation);

    SetObjectScale(goinfo->size);

    if (m_goTemplateAddon)
    {
        SetFaction(m_goTemplateAddon->faction);
        SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, m_goTemplateAddon->flags);
    }

    SetEntry(goinfo->entry);

    // set name for logs usage, doesn't affect anything ingame
    SetName(goinfo->name);

    SetDisplayId(goinfo->displayId);

    CreateModel();
    // GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, index at 0, 1, 2 and 3
    SetGoType(GameobjectTypes(goinfo->type));
    m_prevGoState = go_state;
    SetGoState(go_state);
    SetGoArtKit(artKit);

    SetGoHealth(0xFF);

    switch (goinfo->type)
    {
        case GAMEOBJECT_TYPE_FISHINGHOLE:
            SetGoAnimProgress(animprogress);
            m_goValue.FishingHole.MaxOpens = urand(GetGOInfo()->fishinghole.minSuccessOpens, GetGOInfo()->fishinghole.maxSuccessOpens);
            break;
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
            m_goValue.Building.Health = goinfo->building.intactNumHits + goinfo->building.damagedNumHits;
            m_goValue.Building.MaxHealth = m_goValue.Building.Health;
            SetGoHealth(255);
            SetUInt32Value(GAMEOBJECT_FIELD_PARENT_ROTATION, m_goInfo->building.destructibleData);
            break;
        case GAMEOBJECT_TYPE_TRANSPORT:
            SetUInt32Value(GAMEOBJECT_FIELD_LEVEL, goinfo->transport.pause);
            SetGoState(goinfo->transport.startOpen ? GO_STATE_ACTIVE : GO_STATE_READY);
            SetGoAnimProgress(animprogress);
            m_goValue.Transport.PathProgress = 0;
            m_goValue.Transport.AnimationInfo = sTransportMgr->GetTransportAnimInfo(goinfo->entry);
            m_goValue.Transport.CurrentSeg = 0;
            break;
        case GAMEOBJECT_TYPE_FISHINGNODE:
            SetGoAnimProgress(0);
            break;
        case GAMEOBJECT_TYPE_TRAP:
            if (GetGOInfo()->trap.stealthed)
            {
                m_stealth.AddFlag(STEALTH_TRAP);
                m_stealth.AddValue(STEALTH_TRAP, 70);
            }

            if (GetGOInfo()->trap.invisible)
            {
                m_invisibility.AddFlag(INVISIBILITY_TRAP);
                m_invisibility.AddValue(INVISIBILITY_TRAP, 300);
            }
            break;
        default:
            SetGoAnimProgress(animprogress);
            break;
    }

    if (GameObjectAddon const* addon = sObjectMgr->GetGameObjectAddon(guidlow))
    {
        if (addon->InvisibilityValue)
        {
            m_invisibility.AddFlag(addon->InvisibilityType);
            m_invisibility.AddValue(addon->InvisibilityType, addon->InvisibilityValue);
        }
    }

    LastUsedScriptID = GetGOInfo()->ScriptId;
    AIM_Initialize();

    // Initialize loot duplicate count depending on raid difficulty or db field
    if (m_goTemplateAddon && m_goTemplateAddon->maxDuplicates)
        loot.maxDuplicates = m_goTemplateAddon->maxDuplicates;
    else if (map->Is25ManRaid())
        loot.maxDuplicates = 3;

    if (GetGOInfo()->IsServerOnly())
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_GAMEMASTER);

    LoadCustomVisibility();

    return true;
}

void GameObject::Update(uint32 diff)
{
    // WARNING! Order of execution here is important, do not change.
    // Spells must be processed with event system BEFORE they go to _UpdateSpells.
    // Or else we may have some SPELL_STATE_FINISHED spells stalled in pointers, that is bad.
    m_Events.Update(diff);

    UpdateStealthVisibility(diff);

    if (AI())
        AI()->UpdateAI(diff);
    else if (!AIM_Initialize())
        TC_LOG_ERROR("misc", "Could not initialize GameObjectAI");

    switch (m_lootState)
    {
        case GO_NOT_READY:
        {
            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_TRAP:
                {
                    // Arming Time for GAMEOBJECT_TYPE_TRAP (6)
                    GameObjectTemplate const* goInfo = GetGOInfo();
                    // Bombs
                    if (goInfo->trap.type == 2)
                        m_trapCooldownTime = getMSTime() + 10 * IN_MILLISECONDS;   // Hardcoded tooltip value
                    else if (Unit* owner = GetOwner())
                    {
                        if (owner->IsInCombat())
                            m_trapCooldownTime = getMSTime() + goInfo->trap.startDelay * IN_MILLISECONDS;
                        else
                            m_trapCooldownTime = getMSTime() + 1 * IN_MILLISECONDS;
                    }
                    m_lootState = GO_READY;
                    break;
                }
                case GAMEOBJECT_TYPE_TRANSPORT:
                {
                    if (!m_goValue.Transport.AnimationInfo)
                        break;

                    if (GetGoState() == GO_STATE_READY)
                    {
                        m_goValue.Transport.PathProgress += diff;
                        /* TODO: Fix movement in unloaded grid - currently GO will just disappear
                        uint32 timer = m_goValue.Transport.PathProgress % m_goValue.Transport.AnimationInfo->TotalTime;
                        TransportAnimationEntry const* node = m_goValue.Transport.AnimationInfo->GetAnimNode(timer);
                        if (node && m_goValue.Transport.CurrentSeg != node->TimeSeg)
                        {
                            m_goValue.Transport.CurrentSeg = node->TimeSeg;

                            G3D::Quat rotation = m_goValue.Transport.AnimationInfo->GetAnimRotation(timer);
                            G3D::Vector3 pos = rotation.toRotationMatrix()
                                             * G3D::Matrix3::fromEulerAnglesZYX(GetOrientation(), 0.0f, 0.0f)
                                             * G3D::Vector3(node->X, node->Y, node->Z);

                            pos += G3D::Vector3(GetStationaryX(), GetStationaryY(), GetStationaryZ());

                            G3D::Vector3 src(GetPositionX(), GetPositionY(), GetPositionZ());

                            sLog->outDebug("misc", "Src: %s Dest: %s", src.toString().c_str(), pos.toString().c_str());

                            GetMap()->GameObjectRelocation(this, pos.x, pos.y, pos.z, GetOrientation());
                        }
                        */
                    }
                    break;
                }
                case GAMEOBJECT_TYPE_FISHINGNODE:
                {
                    // fishing code (bobber ready)
                    if (time(NULL) > m_respawnTime - FISHING_BOBBER_READY_TIME)
                    {
                        // splash bobber (bobber ready now)
                        Unit* caster = GetOwner();
                        if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                        {
                            SetGoState(GO_STATE_ACTIVE);
                            SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NODESPAWN);

                            UpdateData udata(caster->GetMapId());
                            WorldPacket packet;
                            BuildValuesUpdateBlockForPlayer(&udata, caster->ToPlayer());
                            udata.BuildPacket(&packet);
                            caster->ToPlayer()->SendDirectMessage(&packet);

                            SendCustomAnim(GetGoAnimProgress());
                        }

                        m_lootState = GO_READY;                 // can be successfully open with some chance
                    }
                    return;
                }
                default:
                    m_lootState = GO_READY;                         // for other GOis same switched without delay to GO_READY
                    break;
            }
            // NO BREAK for switch (m_lootState)
        }
        case GO_READY:
        {
            if (m_respawnTime > 0)                          // timer on
            {
                time_t now = time(NULL);
                if (m_respawnTime <= now)            // timer expired
                {
                    ObjectGuid dbtableHighGuid(HighGuid::GameObject, GetEntry(), m_DBTableGuid);
                    time_t linkedRespawntime = GetMap()->GetLinkedRespawnTime(dbtableHighGuid);
                    if (linkedRespawntime)             // Can't respawn, the master is dead
                    {
                        uint64 targetGuid = sObjectMgr->GetLinkedRespawnGuid(dbtableHighGuid);
                        if (targetGuid == dbtableHighGuid) // if linking self, never respawn (check delayed to next day)
                            SetRespawnTime(DAY);
                        else
                            m_respawnTime = (now > linkedRespawntime ? now : linkedRespawntime)+urand(5, MINUTE); // else copy time from master and add a little
                        SaveRespawnTime(); // also save to DB immediately
                        return;
                    }

                    m_respawnTime = 0;
                    m_SkillupList.clear();
                    m_usetimes = 0;

                    switch (GetGoType())
                    {
                        case GAMEOBJECT_TYPE_FISHINGNODE:   //  can't fish now
                        {
                            Unit* caster = GetOwner();
                            if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                            {
                                caster->ToPlayer()->RemoveGameObject(this, false);

                                WorldPacket data(SMSG_FISH_ESCAPED, 0);
                                caster->ToPlayer()->SendDirectMessage(&data);
                            }
                            // can be delete
                            m_lootState = GO_JUST_DEACTIVATED;
                            return;
                        }
                        case GAMEOBJECT_TYPE_DOOR:
                        case GAMEOBJECT_TYPE_BUTTON:
                            //we need to open doors if they are closed (add there another condition if this code breaks some usage, but it need to be here for battlegrounds)
                            if (GetGoState() != GO_STATE_READY)
                                ResetDoorOrButton();
                            break;
                        case GAMEOBJECT_TYPE_FISHINGHOLE:
                            // Initialize a new max fish count on respawn
                            m_goValue.FishingHole.MaxOpens = urand(GetGOInfo()->fishinghole.minSuccessOpens, GetGOInfo()->fishinghole.maxSuccessOpens);
                            break;
                        default:
                            break;
                    }

                    if (!m_spawnedByDefault)        // despawn timer
                    {
                                                    // can be despawned or destroyed
                        SetLootState(GO_JUST_DEACTIVATED);
                        return;
                    }
                                                    // respawn timer
                    uint32 poolid = GetDBTableGUIDLow() ? sPoolMgr->IsPartOfAPool<GameObject>(GetDBTableGUIDLow()) : 0;
                    if (poolid)
                        sPoolMgr->UpdatePool<GameObject>(GetMap()->GetPoolData(), poolid, GetDBTableGUIDLow());
                    else
                        GetMap()->AddToMap(this);
                }
            }

            if (isSpawned())
            {
                // traps can have time and can not have
                GameObjectTemplate const* goInfo = GetGOInfo();
                if (goInfo->type == GAMEOBJECT_TYPE_TRAP)
                {
                    uint32 now = getMSTime();
                    if (m_trapCooldownTime >= now)
                        return;

                    // Type 2 - Bomb (will go away after casting it's spell)
                    if (goInfo->trap.type == 2)
                    {
                        if (goInfo->trap.spellId)
                            CastSpell(NULL, goInfo->trap.spellId);  // FIXME: null target won't work for target type 1
                        SetLootState(GO_JUST_DEACTIVATED);
                        break;
                    }
                    // Type 0 and 1 - trap (type 0 will not get removed after casting a spell)
                    Unit* owner = GetOwner();
                    Unit* ok = NULL;                            // pointer to appropriate target if found any

                    bool IsBattlegroundTrap = false;
                    /// @todo move activated state code (cast itself) to GO_ACTIVATED, in this place only check activating and set state
                    float radius = goInfo->trap.radius;
                    if (!radius)
                    {
                        if (goInfo->trap.cooldown != 3)            // cast in other case (at some triggering/linked go/etc explicit call)
                            return;
                        else
                        {
                            if (m_respawnTime > 0)
                                break;

                            radius = (float)goInfo->trap.cooldown;       // battlegrounds gameobjects has data2 == 0 && data5 == 3
                            IsBattlegroundTrap = true;

                            if (!radius)
                                return;
                        }
                    }

                    // Note: this hack with search required until GO casting not implemented
                    // search unfriendly creature
                    if (owner)                    // hunter trap
                    {
                        auto check = [this, &owner, &radius](Unit* u)
                        {
                            return u->IsAlive()
                                && u->GetCreatureType() != CREATURE_TYPE_NON_COMBAT_PET
                                && !u->IsTotem()
                                && u->IsWithinDistInMap(this, std::max(0.0f, radius - GetObjectSize() - u->GetObjectSize())) // Contact distance comparison for owned traps
                                && u->isTargetableForAttack(false)
                                && owner->IsValidAttackTarget(u, nullptr, this, true);
                        };
                        Trinity::NearestCheck<decltype(check), Unit> nearest(this, check);
                        Trinity::UnitLastSearcher<Trinity::NearestCheck<decltype(check)>> searcher(this, ok, nearest);
                        VisitNearbyGridObject(radius + SIZE_OF_GRID_CELL, searcher);
                        if (!ok) VisitNearbyWorldObject(radius + SIZE_OF_GRID_CELL, searcher);
                    }
                    else                                        // environmental trap
                    {
                        // environmental damage spells already have around enemies targeting but this not help in case not existed GO casting support
                        // affect only players
                        auto check = [this, &owner, &radius](Player* u)
                        {
                            return u->IsAlive()
                                && u->IsInMap(this)
                                && u->InSamePhase(this)
                                && u->GetExactDistSq(this) <= radius * radius // Exact distance comparison for environmental traps
                                && !u->IsGameMaster();
                        };
                        Player* player = NULL;
                        Trinity::NearestCheck<decltype(check), Player> nearest(this, check);
                        Trinity::PlayerLastSearcher<Trinity::NearestCheck<decltype(check), Player>> searcher(this, player, nearest);
                        VisitNearbyWorldObject(radius + SIZE_OF_GRID_CELL, searcher);
                        ok = player;
                    }

                    if (ok)
                    {
                        // some traps do not have spell but should be triggered
                        if (goInfo->trap.spellId)
                            CastSpell(ok, goInfo->trap.spellId);

                        m_trapCooldownTime = now + (goInfo->trap.cooldown ? goInfo->trap.cooldown : uint32(4)) * IN_MILLISECONDS; // template or 4 seconds

                        if (goInfo->trap.type == 1)
                            SetLootState(GO_JUST_DEACTIVATED);

                        if (IsBattlegroundTrap && ok->GetTypeId() == TYPEID_PLAYER)
                        {
                            //Battleground gameobjects case
                            if (ok->ToPlayer()->InBattleground())
                                if (Battleground* bg = ok->ToPlayer()->GetBattleground())
                                    bg->HandleTriggerBuff(GetGUID());
                        }
                    }
                }
                else if (uint32 max_charges = goInfo->GetCharges())
                {
                    if (m_usetimes >= max_charges)
                    {
                        m_usetimes = 0;
                        SetLootState(GO_JUST_DEACTIVATED);      // can be despawned or destroyed
                    }
                }
            }

            break;
        }
        case GO_ACTIVATED:
        {
            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_DOOR:
                case GAMEOBJECT_TYPE_BUTTON:
                    if (GetGOInfo()->GetAutoCloseTime() && (m_cooldownTime < time(NULL)))
                        ResetDoorOrButton();
                    break;
                case GAMEOBJECT_TYPE_GOOBER:
                    if (m_cooldownTime < time(NULL))
                    {
                        RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);

                        SetLootState(GO_JUST_DEACTIVATED);
                        m_cooldownTime = 0;
                    }
                    break;
                case GAMEOBJECT_TYPE_CHEST:
                    if (m_groupLootTimer)
                    {
                        if (m_groupLootTimer <= diff)
                        {
                            uint32 gguid = lootingGroupLowGUID;
                            ObjectGuid goguid = GetGUID();
                            // TaskMgr::Default()->ScheduleInvocation([=]
                            // {
                            //     if (GameObject* go = ObjectAccessor::FindGameObject(goguid))
                            //         if (Group* group = sGroupMgr->GetGroupByGUID(gguid))
                            //             group->EndRoll(&go->loot);
                            // });
                            if (Map* map = FindMap())
                                if (GameObject* go = map->GetGameObject(goguid))
                                    if (Group* group = sGroupMgr->GetGroupByGUID(gguid))
                                        group->EndRoll(&go->loot);
                            m_groupLootTimer = 0;
                            lootingGroupLowGUID = 0;
                        }
                        else m_groupLootTimer -= diff;
                    }
                default:
                    break;
            }
            break;
        }
        case GO_JUST_DEACTIVATED:
        {
            //if Gameobject should cast spell, then this, but some GOs (type = 10) should be destroyed
            if (GetGoType() == GAMEOBJECT_TYPE_GOOBER)
            {
                uint32 spellId = GetGOInfo()->goober.spellId;

                if (spellId)
                {
                    for (std::set<ObjectGuid>::const_iterator it = m_unique_users.begin(); it != m_unique_users.end(); ++it)
                        // m_unique_users can contain only player GUIDs
                        if (Player* owner = ObjectAccessor::GetPlayer(*this, *it))
                            owner->CastSpell(owner, spellId, false);

                    m_unique_users.clear();
                    m_usetimes = 0;
                }

                SetGoState(GO_STATE_READY);

                //any return here in case battleground traps
                if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
                    if (addon->flags & GO_FLAG_NODESPAWN)
                        return;

                // Summoned by spell and not consumable
                if (GetSpellId() && m_respawnTime && !GetGOInfo()->IsDespawnAtAction())
                {
                    SetLootState(GO_READY);
                    break;
                }
            }

            loot.clear();

            //! If this is summoned by a spell with ie. SPELL_EFFECT_SUMMON_OBJECT_WILD, with or without owner, we check respawn criteria based on spell
            //! The GetOwnerGUID() check is mostly for compatibility with hacky scripts - 99% of the time summoning should be done trough spells.
            if (GetSpellId() || GetOwnerGUID())
            {
                SetRespawnTime(0);
                Delete();
                return;
            }

            SetLootState(GO_READY);

            //burning flags in some battlegrounds, if you find better condition, just add it
            if (GetGOInfo()->IsDespawnAtAction() || GetGoAnimProgress() > 0)
            {
                SendObjectDeSpawnAnim(GetGUID());
                //reset flags
                if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
                    SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, addon->flags);
            }

            if (!m_respawnDelayTime)
                return;

            if (!m_spawnedByDefault)
            {
                m_respawnTime = 0;
                UpdateObjectVisibility();
                return;
            }

            m_respawnTime = time(NULL) + m_respawnDelayTime;

            // if option not set then object will be saved at grid unload
            if (sWorld->getBoolConfig(CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY))
                SaveRespawnTime();

            UpdateObjectVisibility();

            break;
        }
    }
    sScriptMgr->OnGameObjectUpdate(this, diff);
}

void GameObject::Refresh()
{
    // not refresh despawned not casted GO (despawned casted GO destroyed in all cases anyway)
    if (m_respawnTime > 0 && m_spawnedByDefault)
        return;

    if (isSpawned())
        GetMap()->AddToMap(this);
}

void GameObject::AddUniqueUse(Player* player)
{
    AddUse();
    m_unique_users.insert(player->GetGUID());
}

void GameObject::Delete()
{
    SetLootState(GO_NOT_READY);
    RemoveFromOwner();

    SendObjectDeSpawnAnim(GetGUID());

    SetGoState(GO_STATE_READY);

    if (GameObjectTemplateAddon const* addon = GetTemplateAddon())
        SetUInt32Value(GAMEOBJECT_FIELD_FLAGS, addon->flags);

    uint32 poolid = GetDBTableGUIDLow() ? sPoolMgr->IsPartOfAPool<GameObject>(GetDBTableGUIDLow()) : 0;
    if (poolid)
        sPoolMgr->UpdatePool<GameObject>(GetMap()->GetPoolData(), poolid, GetDBTableGUIDLow());
    else
        AddObjectToRemoveList();
}

void GameObject::getFishLoot(Loot* fishloot, Player* loot_owner)
{
    fishloot->clear();

    uint32 zone, subzone;
    GetZoneAndAreaId(zone, subzone);

    // if subzone loot exist use it
    if (!fishloot->FillLoot(this, subzone, LootTemplates_Fishing, loot_owner, true, true))
        // else use zone loot (must exist in like case)
        fishloot->FillLoot(this, zone, LootTemplates_Fishing, loot_owner, true);
}

void GameObject::SaveToDB()
{
    // this should only be used when the gameobject has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    GameObjectData const* data = sObjectMgr->GetGOData(m_DBTableGuid);
    if (!data)
    {
        TC_LOG_ERROR("misc", "GameObject::SaveToDB failed, cannot get gameobject data!");
        return;
    }

    SaveToDB(GetMapId(), data->spawnMask, data->phaseMask);
}

void GameObject::SaveToDB(uint32 mapid, uint16 spawnMask, uint32 phaseMask)
{
    const GameObjectTemplate* goI = GetGOInfo();

    if (!goI)
        return;

    if (!m_DBTableGuid)
        m_DBTableGuid = sObjectMgr->GenerateGameObjectSpawnId();

    // update in loaded data (changing data only in this place)
    GameObjectData& data = sObjectMgr->NewGOData(m_DBTableGuid);

    // data->guid = guid must not be updated at save
    data.id = GetEntry();
    data.mapid = mapid;
    data.phaseMask = phaseMask;
    data.posX = GetPositionX();
    data.posY = GetPositionY();
    data.posZ = GetPositionZ();
    data.orientation = GetOrientation();
    data.rotation = m_worldRotation;
    data.spawntimesecs = m_spawnedByDefault ? m_respawnDelayTime : -(int32)m_respawnDelayTime;
    data.animprogress = GetGoAnimProgress();
    data.go_state = GetGoState();
    data.spawnMask = spawnMask;
    data.artKit = GetGoArtKit();

    // Update in DB
    WorldDatabaseTransaction trans = WorldDatabase.BeginTransaction();

    uint8 index = 0;

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAMEOBJECT);
    stmt->setUInt32(0, m_DBTableGuid);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GAMEOBJECT);
    stmt->setUInt32(index++, m_DBTableGuid);
    stmt->setUInt32(index++, GetEntry());
    stmt->setUInt16(index++, uint16(mapid));
    stmt->setUInt16(index++, spawnMask);
    stmt->setUInt32(index++, GetPhaseMask());
    stmt->setFloat(index++, GetPositionX());
    stmt->setFloat(index++, GetPositionY());
    stmt->setFloat(index++, GetPositionZ());
    stmt->setFloat(index++, GetOrientation());
    stmt->setFloat(index++, m_worldRotation.x);
    stmt->setFloat(index++, m_worldRotation.y);
    stmt->setFloat(index++, m_worldRotation.z);
    stmt->setFloat(index++, m_worldRotation.w);
    stmt->setInt32(index++, int32(m_respawnDelayTime));
    stmt->setUInt8(index++, GetGoAnimProgress());
    stmt->setUInt8(index++, uint8(GetGoState()));
    trans->Append(stmt);

    WorldDatabase.CommitTransaction(trans);
}

bool GameObject::LoadGameObjectFromDB(uint32 guid, Map* map, bool addToMap)
{
    GameObjectData const* data = sObjectMgr->GetGOData(guid);

    if (!data)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u) not found in table `gameobject`, can't load. ", guid);
        return false;
    }

    if (data->gameEventId)
        if (!map->CheckGameEventSpawn(data->gameEventId))
            return false;

    uint32 entry = data->id;
    //uint32 map_id = data->mapid;                          // already used before call
    uint32 phaseMask = data->phaseMask;
    float x = data->posX;
    float y = data->posY;
    float z = data->posZ;
    float ang = data->orientation;

    uint32 animprogress = data->animprogress;
    GOState go_state = data->go_state;
    uint32 artKit = data->artKit;

    m_DBTableGuid = guid;
    if (map->GetInstanceId() != 0)
        guid = map->GenerateLowGuid<HighGuid::GameObject>();

    if (!Create(guid, entry, map, phaseMask, x, y, z, ang, data->rotation, animprogress, go_state, artKit))
        return false;

    if (data->phaseid)
        SetPhased(data->phaseid, false, true);

    if (data->phaseGroup)
    {
        std::vector<uint32> const* phasesInGroup = sDBCManager.GetPhasesForGroup(data->phaseGroup);
        for (uint32 phaseId : *phasesInGroup)
            SetPhased(phaseId, false, true);
        //for (auto ph : sDBCManager.GetPhasesForGroup(data->phaseGroup))
            //SetPhased(ph, false, true);
    }

    if (data->spawntimesecs >= 0)
    {
        m_spawnedByDefault = true;

        if (!GetGOInfo()->GetDespawnPossibility() && !GetGOInfo()->IsDespawnAtAction())
        {
            SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NODESPAWN);
            m_respawnDelayTime = 0;
            m_respawnTime = 0;
        }
        else
        {
            m_respawnDelayTime = data->spawntimesecs;
            m_respawnTime = GetMap()->GetGORespawnTime(m_DBTableGuid);

            // ready to respawn
            if (m_respawnTime && m_respawnTime <= time(NULL))
            {
                m_respawnTime = 0;
                GetMap()->RemoveGORespawnTime(m_DBTableGuid);
            }
        }
    }
    else
    {
        m_spawnedByDefault = false;
        m_respawnDelayTime = -data->spawntimesecs;
        m_respawnTime = 0;
    }

    m_goData = data;

    if (addToMap && !GetMap()->AddToMap(this))
        return false;

    return true;
}

void GameObject::DeleteFromDB()
{
    GameObjectData const* data = sObjectMgr->GetGOData(m_DBTableGuid);
    if (!data)
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    sMapMgr->DoForAllMapsWithMapId(data->mapid,
        [this, trans](Map* map) -> void
        {
            // despawn all active objects, and remove their respawns
            std::vector<GameObject*> toUnload;
            for (auto const& pair : Trinity::Containers::MapEqualRange(map->GetGameObjectBySpawnIdStore(), m_DBTableGuid))
                toUnload.push_back(pair.second);
            for (GameObject* obj : toUnload)
                map->AddObjectToRemoveList(obj);
            map->RemoveGORespawnTime(m_DBTableGuid);
        }
    );

    WorldDatabaseTransaction trans2 = WorldDatabase.BeginTransaction();

    sObjectMgr->DeleteGOData(m_DBTableGuid);

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAMEOBJECT);
    stmt->setUInt32(0, m_DBTableGuid);
    trans2->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT_GAMEOBJECT);
    stmt->setUInt32(0, m_DBTableGuid);
    trans2->Append(stmt);

    WorldDatabase.CommitTransaction(trans2);
}

GameObject* GameObject::GetGameObject(WorldObject& object, ObjectGuid guid)
{
    return object.GetMap()->GetGameObject(guid);
}

/*********************************************************/
/***                    QUEST SYSTEM                   ***/
/*********************************************************/
bool GameObject::hasQuest(uint32 quest_id) const
{
    return sObjectMgr->GetGOQuestRelations(GetEntry()).HasQuest(quest_id);
}

bool GameObject::hasInvolvedQuest(uint32 quest_id) const
{
    return sObjectMgr->GetGOQuestInvolvedRelations(GetEntry()).HasQuest(quest_id);
}

bool GameObject::IsTransport() const
{
    // If something is marked as a transport, don't transmit an out of range packet for it.
    GameObjectTemplate const* gInfo = GetGOInfo();
    if (!gInfo)
        return false;

    return gInfo->type == GAMEOBJECT_TYPE_TRANSPORT || gInfo->type == GAMEOBJECT_TYPE_MO_TRANSPORT;
}

// is Dynamic transport = non-stop Transport
bool GameObject::IsDynTransport() const
{
    // If something is marked as a transport, don't transmit an out of range packet for it.
    GameObjectTemplate const* gInfo = GetGOInfo();
    if (!gInfo)
        return false;

    return gInfo->type == GAMEOBJECT_TYPE_MO_TRANSPORT || (gInfo->type == GAMEOBJECT_TYPE_TRANSPORT && !gInfo->transport.pause);
}

bool GameObject::IsDestructibleBuilding() const
{
    GameObjectTemplate const* gInfo = GetGOInfo();
    if (!gInfo)
        return false;

    return gInfo->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING;
}

Unit* GameObject::GetOwner() const
{
    return ObjectAccessor::GetUnit(*this, GetOwnerGUID());
}

void GameObject::SaveRespawnTime()
{
    if (m_goData && m_goData->dbData && m_respawnTime > time(NULL) && m_spawnedByDefault)
        GetMap()->SaveGORespawnTime(m_DBTableGuid, m_respawnTime);
}

bool GameObject::IsNeverVisibleFor(WorldObject const* seer, bool allowServersideObjects) const
{
    if (WorldObject::IsNeverVisibleFor(seer))
        return true;

    if (GetGOInfo()->GetServerOnly() && !allowServersideObjects)
        return true;

    return false;
}

bool GameObject::IsAlwaysVisibleFor(WorldObject const* seer) const
{
    if (GetGOInfo()->IsServerOnly())
        return false;

    if (WorldObject::IsAlwaysVisibleFor(seer))
        return true;

    if (IsTransport() || IsDestructibleBuilding())
        return true;

    if (!seer)
        return false;

    // Always seen by owner and friendly units
    if (uint64 guid = GetOwnerGUID())
    {
        if (seer->GetGUID() == guid)
            return true;

        Unit* owner = GetOwner();
        if (owner && seer->isType(TYPEMASK_UNIT) && owner->IsFriendlyTo(((Unit*)seer)))
            return true;
    }

    return false;
}

bool GameObject::IsInvisibleDueToDespawn() const
{
    if (WorldObject::IsInvisibleDueToDespawn())
        return true;

    // Despawned
    if (!isSpawned())
        return true;

    return false;
}

void GameObject::Respawn()
{
    if (m_spawnedByDefault && m_respawnTime > 0)
    {
        m_respawnTime = time(NULL);
        GetMap()->RemoveGORespawnTime(m_DBTableGuid);
    }
}

void GameObject::ForcedDespawn(uint32 msTimeToDespawn /*= 0*/)
{
    if (msTimeToDespawn)
    {
        ForcedGoDespawnDelayEvent* pEvent = new ForcedGoDespawnDelayEvent(*this);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(msTimeToDespawn));
        return;
    }

    SetRespawnTime(GetGOData() ? GetGOData()->spawntimesecs : 1);
    UpdateObjectVisibility();
}

bool GameObject::ActivateToQuest(Player* target) const
{
    if (target->HasQuestForGO(GetEntry()))
        return true;

    if (!sObjectMgr->IsGameObjectForQuests(GetEntry()))
        return false;

    switch (GetGoType())
    {
        // scan GO chest with loot including quest items
        case GAMEOBJECT_TYPE_CHEST:
        {
            if (LootTemplates_Gameobject.HaveQuestLootForPlayer(GetGOInfo()->GetLootId(), target))
            {
                if (Battleground const* bg = target->GetBattleground())
                    return bg->CanActivateGO(GetEntry(), target->GetTeam());
                return true;
            }
            break;
        }
        case GAMEOBJECT_TYPE_GENERIC:
        {
            if (GetGOInfo()->_generic.questID == -1 || target->GetQuestStatus(GetGOInfo()->_generic.questID) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        case GAMEOBJECT_TYPE_GOOBER:
        {
            if (GetGOInfo()->goober.questId == -1 || target->GetQuestStatus(GetGOInfo()->goober.questId) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        default:
            break;
    }

    return false;
}

void GameObject::TriggeringLinkedGameObject(uint32 trapEntry, Unit* target)
{
    GameObjectTemplate const* trapInfo = sObjectMgr->GetGameObjectTemplate(trapEntry);
    if (!trapInfo || trapInfo->type != GAMEOBJECT_TYPE_TRAP)
        return;

    SpellInfo const* trapSpell = sSpellMgr->GetSpellInfo(trapInfo->trap.spellId);
    if (!trapSpell)                                          // checked at load already
        return;

    float range = float(target->GetSpellMaxRangeForTarget(GetOwner(), trapSpell));

    // search nearest linked GO
    GameObject* trapGO = NULL;
    {
        // using original GO distance
        CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
        Cell cell(p);

        Trinity::NearestGameObjectEntryInObjectRangeCheck go_check(*target, trapEntry, range);
        Trinity::GameObjectLastSearcher<Trinity::NearestGameObjectEntryInObjectRangeCheck> checker(this, trapGO, go_check);

        TypeContainerVisitor<Trinity::GameObjectLastSearcher<Trinity::NearestGameObjectEntryInObjectRangeCheck>, GridTypeMapContainer > object_checker(checker);
        cell.Visit(p, object_checker, *GetMap(), *target, range);
    }

    // found correct GO
    if (trapGO)
        trapGO->CastSpell(target, trapInfo->trap.spellId);
}

GameObject* GameObject::LookupFishingHoleAround(float range)
{
    GameObject* ok = NULL;

    CellCoord p(Trinity::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    Trinity::NearestGameObjectFishingHole u_check(*this, range);
    Trinity::GameObjectSearcher<Trinity::NearestGameObjectFishingHole> checker(this, ok, u_check);

    TypeContainerVisitor<Trinity::GameObjectSearcher<Trinity::NearestGameObjectFishingHole>, GridTypeMapContainer > grid_object_checker(checker);
    cell.Visit(p, grid_object_checker, *GetMap(), *this, range);

    return ok;
}

void GameObject::ResetDoorOrButton()
{
    if (m_lootState == GO_READY || m_lootState == GO_JUST_DEACTIVATED)
        return;

    RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
    SetGoState(m_prevGoState);

    SetLootState(GO_JUST_DEACTIVATED);
    m_cooldownTime = 0;
}

void GameObject::UseDoorOrButton(uint32 time_to_restore, bool alternative /* = false */, Unit* user /*=NULL*/)
{
    if (m_lootState != GO_READY)
        return;

    if (!time_to_restore)
        time_to_restore = GetGOInfo()->GetAutoCloseTime();

    SwitchDoorOrButton(true, alternative);
    SetLootState(GO_ACTIVATED, user);

    m_cooldownTime = time(NULL) + time_to_restore;
}

uint32 GameObject::GetTransportPeriod() const
{
    ASSERT(GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT);
    if (m_goValue.Transport.AnimationInfo)
        return m_goValue.Transport.AnimationInfo->TotalTime;

    return 0;
}

void GameObject::SetGoArtKit(uint8 kit)
{
    SetByteValue(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 1, kit);
    GameObjectData* data = const_cast<GameObjectData*>(sObjectMgr->GetGOData(m_DBTableGuid));
    if (data)
        data->artKit = kit;
}

void GameObject::SetGoArtKit(uint8 artkit, GameObject* go, uint32 lowguid)
{
    const GameObjectData* data = NULL;
    if (go)
    {
        go->SetGoArtKit(artkit);
        data = go->GetGOData();
    }
    else if (lowguid)
        data = sObjectMgr->GetGOData(lowguid);

    if (data)
        const_cast<GameObjectData*>(data)->artKit = artkit;
}

void GameObject::SwitchDoorOrButton(bool activate, bool alternative /* = false */)
{
    if (activate)
        SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
    else
        RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);

    if (GetGoState() == GO_STATE_READY)                      //if closed -> open
        SetGoState(alternative ? GO_STATE_ACTIVE_ALTERNATIVE : GO_STATE_ACTIVE);
    else                                                    //if open -> close
        SetGoState(GO_STATE_READY);
}

void GameObject::Use(Unit* user)
{
    // by default spell caster is user
    Unit* spellCaster = user;
    uint32 spellId = 0;
    bool triggered = false;

    if (Player* playerUser = user->ToPlayer())
    {
        if (sScriptMgr->OnGossipHello(playerUser, this))
            return;

        if (AI()->OnGossipHello(playerUser, true))
            return;
    }

    // If cooldown data present in template
    if (uint32 cooldown = GetGOInfo()->GetCooldown())
    {
        if (m_cooldownTime > sWorld->GetGameTime())
            return;

        m_cooldownTime = sWorld->GetGameTime() + cooldown;
    }

    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_DOOR:                          //0
        case GAMEOBJECT_TYPE_BUTTON:                        //1
            //doors/buttons never really despawn, only reset to default state/flags
            UseDoorOrButton(0, false, user);
            return;
        case GAMEOBJECT_TYPE_QUESTGIVER:                    //2
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            player->PrepareGossipMenu(this, GetGOInfo()->questgiver.gossipID, true);
            player->SendPreparedGossip(this);
            return;
        }
        case GAMEOBJECT_TYPE_TRAP:                          //6
        {
            GameObjectTemplate const* goInfo = GetGOInfo();
            if (goInfo->trap.spellId)
                CastSpell(user, goInfo->trap.spellId);

            m_trapCooldownTime = getMSTime() + (goInfo->trap.cooldown ? goInfo->trap.cooldown : uint32(4)) * IN_MILLISECONDS; // template or 4 seconds

            if (goInfo->trap.type == 1)         // Deactivate after trigger
                SetLootState(GO_JUST_DEACTIVATED);

            return;
        }
        //Sitting: Wooden bench, chairs enzz
        case GAMEOBJECT_TYPE_CHAIR:                         //7
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            if (ChairListSlots.empty())        // this is called once at first chair use to make list of available slots
            {
                if (info->chair.slots > 0)     // sometimes chairs in DB have error in fields and we dont know number of slots
                    for (uint32 i = 0; i < info->chair.slots; ++i)
                        ChairListSlots[i].Clear(); // Last user of current slot set to 0 (none sit here yet)
                else
                    ChairListSlots[0].Clear();     // error in DB, make one default slot
            }

            Player* player = user->ToPlayer();

            // a chair may have n slots. we have to calculate their positions and teleport the player to the nearest one

            float lowestDist = DEFAULT_VISIBILITY_DISTANCE;

            uint32 nearest_slot = 0;
            float x_lowest = GetPositionX();
            float y_lowest = GetPositionY();

            // the object orientation + 1/2 pi
            // every slot will be on that straight line
            float orthogonalOrientation = GetOrientation()+M_PI*0.5f;
            // find nearest slot
            bool found_free_slot = false;
            for (ChairSlotAndUser::iterator itr = ChairListSlots.begin(); itr != ChairListSlots.end(); ++itr)
            {
                // the distance between this slot and the center of the go - imagine a 1D space
                float relativeDistance = (info->size*itr->first)-(info->size*(info->chair.slots-1)/2.0f);

                float x_i = GetPositionX() + relativeDistance * std::cos(orthogonalOrientation);
                float y_i = GetPositionY() + relativeDistance * std::sin(orthogonalOrientation);

                if (itr->second)
                {
                    if (Player* ChairUser = ObjectAccessor::FindPlayer(itr->second))
                    {
                        if (ChairUser->IsSitState() && ChairUser->GetStandState() != UNIT_STAND_STATE_SIT && ChairUser->GetExactDist2d(x_i, y_i) < 0.1f)
                            continue;        // This seat is already occupied by ChairUser. NOTE: Not sure if the ChairUser->GetStandState() != UNIT_STAND_STATE_SIT check is required.
                        else
                            itr->second.Clear(); // This seat is unoccupied.
                    }
                    else
                        itr->second.Clear();     // The seat may have had an occupant, but they're offline.
                }

                found_free_slot = true;

                // calculate the distance between the player and this slot
                float thisDistance = player->GetDistance2d(x_i, y_i);

                if (thisDistance <= lowestDist)
                {
                    nearest_slot = itr->first;
                    lowestDist = thisDistance;
                    x_lowest = x_i;
                    y_lowest = y_i;
                }
            }

            if (found_free_slot)
            {
                ChairSlotAndUser::iterator itr = ChairListSlots.find(nearest_slot);
                if (itr != ChairListSlots.end())
                {
                    itr->second = player->GetGUID(); //this slot in now used by player
                    player->TeleportTo(GetMapId(), x_lowest, y_lowest, GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
                    player->SetStandState(UnitStandStateType(UNIT_STAND_STATE_SIT_LOW_CHAIR+info->chair.height));
                    return;
                }
            }

            return;
        }
        //big gun, its a spell/aura
        case GAMEOBJECT_TYPE_GOOBER:                        //10
        {
            GameObjectTemplate const* info = GetGOInfo();

            if (user->GetTypeId() == TYPEID_PLAYER || user->IsVehicle())
            {
                if (Player* player = user->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if (info->goober.pageId)                    // show page...
                    {
                        ObjectGuid guid = GetGUID();
                        WorldPacket data(SMSG_GAMEOBJECT_PAGETEXT, 8);
                        data.WriteBit(guid[0]);
                        data.WriteBit(guid[3]);
                        data.WriteBit(guid[2]);
                        data.WriteBit(guid[6]);
                        data.WriteBit(guid[5]);
                        data.WriteBit(guid[1]);
                        data.WriteBit(guid[7]);
                        data.WriteBit(guid[4]);
                        data.WriteByteSeq(guid[6]);
                        data.WriteByteSeq(guid[2]);
                        data.WriteByteSeq(guid[7]);
                        data.WriteByteSeq(guid[0]);
                        data.WriteByteSeq(guid[5]);
                        data.WriteByteSeq(guid[3]);
                        data.WriteByteSeq(guid[1]);
                        data.WriteByteSeq(guid[4]);
                        player->SendDirectMessage(&data);
                    }
                    else if (info->goober.gossipID)
                    {
                        player->PrepareGossipMenu(this, info->goober.gossipID);
                        player->SendPreparedGossip(this);
                    }

                    if (info->goober.eventId)
                    {
                        TC_LOG_DEBUG("maps.script", "Goober ScriptStart id %u for GO entry %u (GUID %u).", info->goober.eventId, GetEntry(), GetDBTableGUIDLow());
                        GetMap()->ScriptsStart(sEventScripts, info->goober.eventId, player, this);
                        EventInform(info->goober.eventId);
                    }

                    // possible quest objective for active quests
                    if (info->goober.questId && sObjectMgr->GetQuestTemplate(info->goober.questId))
                    {
                        //Quest require to be active for GO using
                        if (player->GetQuestStatus(info->goober.questId) != QUEST_STATUS_INCOMPLETE)
                            break;
                    }

                    if (Battleground* bg = player->GetBattleground())
                        bg->EventPlayerUsedGO(player, this);

                    player->KillCreditGO(info->entry, GetGUID());
                }
            }

            if (uint32 trapEntry = info->goober.linkedTrapId)
                TriggeringLinkedGameObject(trapEntry, user);

            SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
            SetLootState(GO_ACTIVATED, user);

            // this appear to be ok, however others exist in addition to this that should have custom (ex: 190510, 188692, 187389)
            if (info->goober.customAnim)
                SendCustomAnim(GetGoAnimProgress());
            else
                SetGoState(GO_STATE_ACTIVE);

            m_cooldownTime = time(NULL) + info->GetAutoCloseTime();

            // cast this spell later if provided
            spellId = info->goober.spellId;
            spellCaster = NULL;

            break;
        }
        case GAMEOBJECT_TYPE_CAMERA:                        //13
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            if (info->camera.cinematicId)
                player->SendCinematicStart(info->camera.cinematicId);

            if (info->camera.eventID)
                GetMap()->ScriptsStart(sEventScripts, info->camera.eventID, player, this);

            return;
        }
        //fishing bobber
        case GAMEOBJECT_TYPE_FISHINGNODE:                   //17
        {
            Player* player = user->ToPlayer();
            if (!player)
                return;

            if (player->GetGUID() != GetOwnerGUID())
                return;

            switch (getLootState())
            {
                case GO_READY:                              // ready for loot
                {
                    uint32 zone, subzone;
                    GetZoneAndAreaId(zone, subzone);

                    int32 zone_skill = sObjectMgr->GetFishingBaseSkillLevel(subzone);
                    if (!zone_skill)
                        zone_skill = sObjectMgr->GetFishingBaseSkillLevel(zone);

                    //provide error, no fishable zone or area should be 0
                    if (!zone_skill)
                        TC_LOG_ERROR("sql.sql", "Fishable areaId %u are not properly defined in `skill_fishing_base_level`.", subzone);

                    int32 skill = player->GetSkillValue(SKILL_FISHING);

                    int32 chance;
                    if (skill < zone_skill)
                    {
                        chance = int32(pow((double)skill/zone_skill, 2) * 100);
                        if (chance < 1)
                            chance = 1;
                    }
                    else
                        chance = 100;

                    int32 roll = irand(1, 100);

                    TC_LOG_DEBUG("misc", "Fishing check (skill: %i zone min skill: %i chance %i roll: %i", skill, zone_skill, chance, roll);

                    // but you will likely cause junk in areas that require a high fishing skill (not yet implemented)
                    if (chance >= roll)
                    {
                        player->UpdateFishingSkill();

                        /// @todo I do not understand this hack. Need some explanation.
                        // prevent removing GO at spell cancel
                        RemoveFromOwner();
                        SetOwnerGUID(player->GetGUID());

                        /// @todo find reasonable value for fishing hole search
                        GameObject* ok = LookupFishingHoleAround(20.0f + CONTACT_DISTANCE);
                        if (ok)
                        {
                            ok->Use(player);
                            SetLootState(GO_JUST_DEACTIVATED);
                        }
                        else
                            player->SendLoot(GetGUID(), LOOT_FISHING);
                    }
                    /// @todo else: junk
                    else
                        m_respawnTime = time(NULL);

                    break;
                }
                case GO_JUST_DEACTIVATED:                   // nothing to do, will be deleted at next update
                    break;
                default:
                {
                    SetLootState(GO_JUST_DEACTIVATED);

                    WorldPacket data(SMSG_FISH_NOT_HOOKED, 0);
                    player->SendDirectMessage(&data);
                    break;
                }
            }

            player->FinishSpell(CURRENT_CHANNELED_SPELL);
            return;
        }

        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:              //18
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            Unit* owner = GetOwner();

            GameObjectTemplate const* info = GetGOInfo();

            // ritual owner is set for GO's without owner (not summoned)
            if (!m_ritualOwner && !owner)
                m_ritualOwner = player;

            if (owner)
            {
                if (owner->GetTypeId() != TYPEID_PLAYER)
                    return;

                // accept only use by player from same group as owner, excluding owner itself (unique use already added in spell effect)
                if (player == owner->ToPlayer() || (info->summoningRitual.castersGrouped && !player->IsInSameRaidWith(owner->ToPlayer())))
                    return;

                // expect owner to already be channeling, so if not...
                if (!owner->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    return;

                // in case summoning ritual caster is GO creator
                spellCaster = owner;
            }
            else
            {
                if (player != m_ritualOwner && (info->summoningRitual.castersGrouped && !player->IsInSameRaidWith(m_ritualOwner)))
                    return;

                spellCaster = player;
            }

            AddUniqueUse(player);

            if (info->summoningRitual.animSpell)
            {
                player->CastSpell(player, info->summoningRitual.animSpell, true);

                // for this case, summoningRitual.spellId is always triggered
                triggered = true;
            }

            // full amount unique participants including original summoner
            if (GetUniqueUseCount() == info->summoningRitual.reqParticipants)
            {
                if (m_ritualOwner)
                    spellCaster = m_ritualOwner;

                spellId = info->summoningRitual.spellId;

                if (spellId == 62330)                       // GO store nonexistent spell, replace by expected
                {
                    // spell have reagent and mana cost but it not expected use its
                    // it triggered spell in fact casted at currently channeled GO
                    spellId = 61993;
                    triggered = true;
                }

                // Cast casterTargetSpell at a random GO user
                // on the current DB there is only one gameobject that uses this (Ritual of Doom)
                // and its required target number is 1 (outter for loop will run once)
                if (info->summoningRitual.casterTargetSpell && info->summoningRitual.casterTargetSpell != 1) // No idea why this field is a bool in some cases
                    for (uint32 i = 0; i < info->summoningRitual.casterTargetSpellTargets; i++)
                        // m_unique_users can contain only player GUIDs
                        if (Player* target = ObjectAccessor::GetPlayer(*this, Trinity::Containers::SelectRandomContainerElement(m_unique_users)))
                            spellCaster->CastSpell(target, info->summoningRitual.casterTargetSpell, true);

                // finish owners spell
                if (owner)
                    owner->FinishSpell(CURRENT_CHANNELED_SPELL);

                // can be deleted now, if
                if (!info->summoningRitual.ritualPersistent)
                    SetLootState(GO_JUST_DEACTIVATED);
                else
                {
                    // reset ritual for this GO
                    m_ritualOwner = NULL;
                    m_unique_users.clear();
                    m_usetimes = 0;
                }
            }
            else
                return;

            // go to end function to spell casting
            break;
        }
        case GAMEOBJECT_TYPE_SPELLCASTER:                   //22
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (info->spellcaster.partyOnly)
            {
                Unit* caster = GetOwner();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (user->GetTypeId() != TYPEID_PLAYER || !user->ToPlayer()->IsInSameRaidWith(caster->ToPlayer()))
                    return;
            }

            if (info->entry != 220367)
                user->RemoveAurasByType(SPELL_AURA_MOUNTED);
            else
                triggered = true;

            spellId = info->spellcaster.spellId;

            AddUse();
            break;
        }
        case GAMEOBJECT_TYPE_MEETINGSTONE:                  //23
        {
            GameObjectTemplate const* info = GetGOInfo();

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            Player* targetPlayer = ObjectAccessor::FindPlayer(player->GetTarget());

            // accept only use by player from same raid as caster, except caster itself
            if (!targetPlayer || targetPlayer == player || !targetPlayer->IsInSameRaidWith(player))
                return;

            //required lvl checks!
            uint8 level = player->GetLevel();
            if (level < info->meetingstone.minLevel)
                return;
            level = targetPlayer->GetLevel();
            if (level < info->meetingstone.minLevel)
                return;

            if (info->entry == 194097)
                spellId = 61994;                            // Ritual of Summoning
            else
                spellId = 59782;                            // Summoning Stone Effect

            break;
        }

        case GAMEOBJECT_TYPE_FLAGSTAND:                     // 24
        case GAMEOBJECT_TYPE_FLAGSTAND_2:                   // 36
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            if (player->CanUseBattlegroundObject(this))
            {
                // in battleground check
                Battleground* bg = player->GetBattleground();
                if (!bg)
                    return;

                if (player->GetVehicle())
                    return;

                player->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                player->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
                // BG flag click
                // AB:
                // 15001
                // 15002
                // 15003
                // 15004
                // 15005
                bg->EventPlayerClickedOnFlag(player, this);
                return;                                     //we don;t need to delete flag ... it is despawned!
            }
            break;
        }

        case GAMEOBJECT_TYPE_FISHINGHOLE:                   // 25
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            player->SendLoot(GetGUID(), LOOT_FISHINGHOLE);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT, GetGOInfo()->entry);
            if (Guild* guild = player->GetGuild())
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CATCH_FROM_POOL, 1, 0, 0, player, player);
            return;
        }

        case GAMEOBJECT_TYPE_FLAGDROP:                      // 26
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            if (player->CanUseBattlegroundObject(this))
            {
                // in battleground check
                Battleground* bg = player->GetBattleground();
                if (!bg)
                    return;

                if (player->GetVehicle())
                    return;

                player->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                player->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
                // BG flag dropped
                // WS:
                // 179785 - Silverwing Flag
                // 179786 - Warsong Flag
                // EotS:
                // 184142 - Netherstorm Flag
                GameObjectTemplate const* info = GetGOInfo();
                if (info)
                {
                    switch (info->entry)
                    {
                        case 179785:                        // Silverwing Flag
                        case 179786:                        // Warsong Flag
                            if (bg->GetTypeID(true) == BATTLEGROUND_WS || bg->GetTypeID(true) == BATTLEGROUND_TP)
                                bg->EventPlayerClickedOnFlag(player, this);
                            break;
                        case 184142:                        // Netherstorm Flag
                            if (bg->GetTypeID(true) == BATTLEGROUND_EY)
                                bg->EventPlayerClickedOnFlag(player, this);
                            break;
                        case 220174:                        // Horde Mine Cart
                        case 220165:                        // Alliance Mine Cart
                            if (bg->GetTypeID(true) == BATTLEGROUND_DG)
                                bg->EventPlayerClickedOnFlag(player, this);
                    }
                }
                //this cause to call return, all flags must be deleted here!!
                spellId = 0;
                Delete();
            }
            break;
        }
        case GAMEOBJECT_TYPE_BARBER_CHAIR:                  //32
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            // fallback, will always work
            player->TeleportTo(GetMapId(), GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);

            WorldPacket data(SMSG_ENABLE_BARBER_SHOP, 0);
            player->SendDirectMessage(&data);

            player->SetStandState(UnitStandStateType(UNIT_STAND_STATE_SIT_LOW_CHAIR + info->barberChair.chairheight));
            return;
        }
        default:
            if (GetGoType() >= MAX_GAMEOBJECT_TYPE)
                TC_LOG_ERROR("misc", "GameObject::Use(): unit (type: %u, guid: %u, name: %s) tries to use object (guid: %u, entry: %u, name: %s) of unknown type (%u)",
                    user->GetTypeId(), user->GetGUID().GetCounter(), user->GetName().c_str(), GetGUID().GetCounter(), GetEntry(), GetGOInfo()->name.c_str(), GetGoType());
            break;
    }

    if (!spellId)
        return;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        if (user->GetTypeId() != TYPEID_PLAYER || !sOutdoorPvPMgr->HandleCustomSpell(user->ToPlayer(), spellId, this))
            TC_LOG_ERROR("misc", "WORLD: unknown spell id %u at use action for gameobject (Entry: %u GoType: %u)", spellId, GetEntry(), GetGoType());
        else
            TC_LOG_DEBUG("outdoorpvp", "WORLD: %u non-dbc spell was handled by OutdoorPvP", spellId);
        return;
    }

    if (spellCaster)
        spellCaster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_LOOTING);

    if (spellCaster)
        spellCaster->CastSpell(user, spellInfo, triggered);
    else
        CastSpell(user, spellId);
}

void GameObject::CastSpell(Unit* target, uint32 spellId)
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return;

    bool self = false;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CASTER)
        {
            self = true;
            break;
        }
    }

    if (self)
    {
        if (target)
            target->CastSpell(target, spellInfo, true);
        return;
    }

    //summon world trigger
    Creature* trigger = SummonTrigger(GetPositionX(), GetPositionY(), GetPositionZ(), 0, spellInfo->CalcCastTime() + 100);
    if (!trigger)
        return;

    trigger->SetPhaseMask(GetPhaseMask(), false);

    if (Unit* owner = GetOwner())
    {
        trigger->SetFaction(owner->GetFaction());
        // needed for GO casts for proper target validation checks
        trigger->SetOwnerGUID(owner->GetGUID());
        if (owner->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE))
            trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        if (owner->HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP))
            trigger->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        trigger->CastSpell(target ? target : trigger, spellInfo, true, 0, 0, owner->GetGUID());
    }
    else
    {
        trigger->SetFaction(14);
        // Set owner guid for target if no owner available - needed by trigger auras
        // - trigger gets despawned and there's no caster avalible (see AuraEffect::TriggerSpell())
        trigger->CastSpell(target ? target : trigger, spellInfo, true, 0, 0, target ? target->GetGUID() : ObjectGuid::Empty);
    }
}

void GameObject::SendCustomAnim(uint32 anim)
{
    ObjectGuid Guid = GetGUID();
    WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 8 + 4);

    data.WriteBit(Guid[4]);
    data.WriteBit(Guid[7]);
    data.WriteBit(Guid[1]);
    data.WriteBit(Guid[0]);
    data.WriteBit(Guid[5]);
    data.WriteBit(Guid[3]);
    data.WriteBit(Guid[2]);
    data.WriteBit(0); // Play As Despawn
    data.WriteBit(Guid[6]);
    data.FlushBits();

    data << uint32(anim);
    data.WriteByteSeq(Guid[5]);
    data.WriteByteSeq(Guid[6]);
    data.WriteByteSeq(Guid[7]);
    data.WriteByteSeq(Guid[3]);
    data.WriteByteSeq(Guid[4]);
    data.WriteByteSeq(Guid[0]);
    data.WriteByteSeq(Guid[2]);
    data.WriteByteSeq(Guid[1]);

    SendMessageToSet(&data, true);
}

void GameObject::SetAnimKitId(uint16 animKitId, bool oneshot)
{
    if (_animKitId == animKitId)
        return;

    if (animKitId && !sAnimKitStore.LookupEntry(animKitId))
        return;

    if (!oneshot)
        _animKitId = animKitId;
    else
        _animKitId = 0;

    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT);

    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[7]);
    data.WriteBit(!oneshot);
    data.WriteBit(guid[5]);

    data.FlushBits();

    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data << uint32(_animKitId);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);

    SendMessageToSet(&data, true);
}

void GameObject::UpdateModelPosition()
{
    if (!m_model)
        return;

    if (GetMap()->ContainsGameObjectModel(*m_model))
    {
        GetMap()->RemoveGameObjectModel(*m_model);
        m_model->UpdatePosition();
        GetMap()->InsertGameObjectModel(*m_model);
    }
}

class GameObjectModelOwnerImpl : public GameObjectModelOwnerBase
{
public:
    explicit GameObjectModelOwnerImpl(GameObject* owner) : _owner(owner) { }

    bool IsSpawned() const override { return _owner->isSpawned(); }
    uint32 GetDisplayId() const override { return _owner->GetDisplayId(); }
    uint32 GetPhaseMask() const override { return _owner->GetPhaseMask(); }
    G3D::Vector3 GetPosition() const override { return G3D::Vector3(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ()); }
    float GetOrientation() const override { return _owner->GetOrientation(); }
    float GetScale() const override { return _owner->GetObjectScale(); }
    void DebugVisualizeCorner(G3D::Vector3 const& corner) const override { const_cast<GameObject*>(_owner)->SummonCreature(1, corner.x, corner.y, corner.z, 0, TEMPSUMMON_MANUAL_DESPAWN); }

private:
    GameObject* _owner;
};

void GameObject::CreateModel()
{
    m_model = GameObjectModel::Create(std::make_unique<GameObjectModelOwnerImpl>(this), sWorld->GetDataPath());
}

SpellInfo const* GameObject::GetSpellForLock(Player const* player) const
{
    if (!player)
        return nullptr;

    uint32 lockId = GetGOInfo()->GetLockId();
    if (!lockId)
        return nullptr;

    LockEntry const* lock = sLockStore.LookupEntry(lockId);
    if (!lock)
        return nullptr;

    for (uint8 i = 0; i < MAX_LOCK_CASE; ++i)
    {
        if (!lock->Type[i])
            continue;

        if (lock->Type[i] == LOCK_KEY_SPELL)
            if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(lock->Index[i]))
                return spell;

        if (lock->Type[i] != LOCK_KEY_SKILL)
            break;

        for (auto&& playerSpell : player->GetSpellMap())
            if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(playerSpell.first))
                for (auto&& effect : spell->Effects)
                    if (effect.Effect == SPELL_EFFECT_OPEN_LOCK && effect.MiscValue == lock->Index[i])
                        if (effect.CalcValue(player) >= int32(lock->Skill[i]))
                            return spell;
    }

    return nullptr;
}

bool GameObject::IsAtInteractDistance(Player const* player, SpellInfo const* spell) const
{
    if (spell || (spell = GetSpellForLock(player)))
    {
        float minRange = spell->GetMinRange(spell->IsPositive());
        float maxRange = spell->GetMaxRange(spell->IsPositive());

        if (GetGoType() == GAMEOBJECT_TYPE_SPELL_FOCUS)
            return maxRange * maxRange >= GetExactDistSq(player);

        // Not yet decompiled
        if (GetGoType() == GAMEOBJECT_TYPE_DOOR)
            return true;

        if (GameObjectDisplayInfoEntry const* displayInfo = sGameObjectDisplayInfoStore.LookupEntry(GetGOInfo()->displayId))
            return IsAtInteractDistance(*player, maxRange);
    }

    float distance;
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_AREADAMAGE:
            distance = 0.0f;
            break;
        case GAMEOBJECT_TYPE_QUESTGIVER:
        case GAMEOBJECT_TYPE_TEXT:
        case GAMEOBJECT_TYPE_FLAGSTAND:
        case GAMEOBJECT_TYPE_FLAGDROP:
        case GAMEOBJECT_TYPE_MINI_GAME:
            distance = 5.5555553f;
            break;
        case GAMEOBJECT_TYPE_BINDER:
            distance = 10.0f;
            break;
        case GAMEOBJECT_TYPE_CHAIR:
        case GAMEOBJECT_TYPE_BARBER_CHAIR:
            distance = 3.0f;
            break;
        case GAMEOBJECT_TYPE_FISHINGNODE:
            distance = 100.0f;
            break;
        case GAMEOBJECT_TYPE_CAMERA:
        case GAMEOBJECT_TYPE_MAP_OBJECT:
        case GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY:
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
        default:
            distance = 5.0f;
            break;

        // Following values are not blizzlike
        case GAMEOBJECT_TYPE_MAILBOX:
            // Successful mailbox interaction is rather critical to the client, failing it will start a minute-long cooldown until the next mail query may be executed.
            // And since movement info update is not sent with mailbox interaction query, server may find the player outside of interaction range. Thus we increase it.
            distance = 10.0f; // 5.0f is blizzlike
            break;
    }

    return IsAtInteractDistance(*player, distance);
}

bool GameObject::IsAtInteractDistance(Position const & pos, float radius) const
{
    if (GameObjectDisplayInfoEntry const* displayInfo = sGameObjectDisplayInfoStore.LookupEntry(GetGOInfo()->displayId))
    {
        float scale = GetObjectScale();

        float minX = displayInfo->minX * scale - radius;
        float minY = displayInfo->minY * scale - radius;
        float minZ = displayInfo->minZ * scale - radius;
        float maxX = displayInfo->maxX * scale + radius;
        float maxY = displayInfo->maxY * scale + radius;
        float maxZ = displayInfo->maxZ * scale + radius;

        float x, y, z;
        pos.GetPosition(x, y, z);

        x -= GetPositionX();
        y -= GetPositionY();
        z -= GetPositionZ();

        return G3D::CoordinateFrame { { GetWorldRotation() }, { GetPositionX(), GetPositionY(), GetPositionZ() } }
                   .toWorldSpace(G3D::Box { { minX, minY, minZ }, { maxX, maxY, maxZ } })
                   .contains({ pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() });
    }

    return GetExactDist(&pos) <= std::max(radius, INTERACTION_DISTANCE * 2);
}

void GameObject::EventInform(uint32 eventId)
{
    if (!eventId)
        return;

    if (AI())
        AI()->EventInform(eventId);

    if (m_zoneScript)
        m_zoneScript->ProcessEvent(this, eventId);
}

uint32 GameObject::GetScriptId() const
{
    if (GameObjectData const* gameObjectData = GetGOData())
        if (uint32 scriptId = gameObjectData->ScriptId)
            return scriptId;

    return GetGOInfo()->ScriptId;
}

// overwrite WorldObject function for proper name localization
std::string const & GameObject::GetNameForLocaleIdx(LocaleConstant loc_idx) const
{
    if (loc_idx != DEFAULT_LOCALE)
    {
        uint8 uloc_idx = uint8(loc_idx);
        if (GameObjectLocale const* cl = sObjectMgr->GetGameObjectLocale(GetEntry()))
            if (cl->Name.size() > uloc_idx && !cl->Name[uloc_idx].empty())
                return cl->Name[uloc_idx];
    }

    return GetName();
}

void GameObject::UpdatePackedRotation()
{
    static const int32 PACK_YZ = 1 << 20;
    static const int32 PACK_X = PACK_YZ << 1;

    static const int32 PACK_YZ_MASK = (PACK_YZ << 1) - 1;
    static const int32 PACK_X_MASK = (PACK_X << 1) - 1;

    int8 w_sign = (m_worldRotation.w >= 0.f ? 1 : -1);
    int64 x = int32(m_worldRotation.x * PACK_X)  * w_sign & PACK_X_MASK;
    int64 y = int32(m_worldRotation.y * PACK_YZ) * w_sign & PACK_YZ_MASK;
    int64 z = int32(m_worldRotation.z * PACK_YZ) * w_sign & PACK_YZ_MASK;
    m_packedRotation = z | (y << 21) | (x << 42);
}

void GameObject::SetWorldRotation(G3D::Quat rotation)
{
    rotation.unitize();
    m_worldRotation.x = rotation.x;
    m_worldRotation.y = rotation.y;
    m_worldRotation.z = rotation.z;
    m_worldRotation.w = rotation.w;
    UpdatePackedRotation();

    // Update final orientation. In some instances gameobject.orientation doesn't match orientation that results from quaternion rotations.
    Position::SetOrientation(atan2(2 * (rotation.w * rotation.z + rotation.x * rotation.y), 1 - 2 * (rotation.y * rotation.y + rotation.z * rotation.z)));

    if (Transport* transport = GetTransport())
        m_movementInfo.transport.pos.SetOrientation(GetOrientation() - transport->GetOrientation());
}

void GameObject::SetParentRotation(G3D::Quat const& rotation)
{
    SetFloatValue(GAMEOBJECT_FIELD_PARENT_ROTATION + 0, rotation.x);
    SetFloatValue(GAMEOBJECT_FIELD_PARENT_ROTATION + 1, rotation.y);
    SetFloatValue(GAMEOBJECT_FIELD_PARENT_ROTATION + 2, rotation.z);
    SetFloatValue(GAMEOBJECT_FIELD_PARENT_ROTATION + 3, rotation.w);
}

void GameObject::SetWorldRotationAngles(float z_rot, float y_rot, float x_rot)
{
    SetWorldRotation(G3D::Matrix3::fromEulerAnglesZYX(z_rot, y_rot, x_rot));
}

void GameObject::ModifyHealth(int32 change, Unit* attackerOrHealer /*= NULL*/, uint32 spellId /*= 0*/)
{
    if (!m_goValue.Building.MaxHealth || !change)
        return;

    // prevent double destructions of the same object
    if (change < 0 && !m_goValue.Building.Health)
        return;

    if (int32(m_goValue.Building.Health) + change <= 0)
        m_goValue.Building.Health = 0;
    else if (int32(m_goValue.Building.Health) + change >= int32(m_goValue.Building.MaxHealth))
        m_goValue.Building.Health = m_goValue.Building.MaxHealth;
    else
        m_goValue.Building.Health += change;

    // Set the health bar, value = 255 * healthPct;
    SetGoHealth(m_goValue.Building.Health * 255 / m_goValue.Building.MaxHealth);

    Player* player = attackerOrHealer->GetCharmerOrOwnerPlayerOrPlayerItself();

    // dealing damage, send packet
    if (player)
    {
        ObjectGuid playerGUID = player->GetGUID();
        ObjectGuid vehicleGUID = attackerOrHealer->GetGUID();
        ObjectGuid objectGUID = GetGUID();

        WorldPacket data(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, 8 + 8 + 8 + 4 + 4);
        data.WriteBit(vehicleGUID[2]);
        data.WriteBit(objectGUID[2]);
        data.WriteBit(playerGUID[7]);
        data.WriteBit(objectGUID[4]);
        data.WriteBit(playerGUID[3]);
        data.WriteBit(playerGUID[5]);
        data.WriteBit(objectGUID[0]);
        data.WriteBit(playerGUID[2]);
        data.WriteBit(playerGUID[6]);
        data.WriteBit(vehicleGUID[4]);
        data.WriteBit(objectGUID[5]);
        data.WriteBit(playerGUID[0]);
        data.WriteBit(playerGUID[1]);
        data.WriteBit(objectGUID[1]);
        data.WriteBit(vehicleGUID[0]);
        data.WriteBit(vehicleGUID[3]);
        data.WriteBit(vehicleGUID[6]);
        data.WriteBit(objectGUID[7]);
        data.WriteBit(vehicleGUID[7]);
        data.WriteBit(vehicleGUID[5]);
        data.WriteBit(objectGUID[6]);
        data.WriteBit(playerGUID[4]);
        data.WriteBit(vehicleGUID[1]);
        data.WriteBit(objectGUID[3]);

        data.WriteByteSeq(vehicleGUID[2]);
        data << uint32(spellId);
        data.WriteByteSeq(playerGUID[5]);
        data.WriteByteSeq(playerGUID[6]);
        data.WriteByteSeq(objectGUID[7]);
        data.WriteByteSeq(playerGUID[1]);
        data.WriteByteSeq(objectGUID[1]);
        data.WriteByteSeq(playerGUID[2]);
        data.WriteByteSeq(vehicleGUID[7]);
        data.WriteByteSeq(vehicleGUID[5]);
        data.WriteByteSeq(objectGUID[0]);
        data.WriteByteSeq(objectGUID[2]);
        data.WriteByteSeq(objectGUID[6]);
        data.WriteByteSeq(vehicleGUID[1]);
        data.WriteByteSeq(vehicleGUID[6]);
        data << int32(-change);                    // change  < 0 triggers SPELL_BUILDING_HEAL combat log event
                                                   // change >= 0 triggers SPELL_BUILDING_DAMAGE event
        data.WriteByteSeq(objectGUID[5]);
        data.WriteByteSeq(playerGUID[0]);
        data.WriteByteSeq(vehicleGUID[0]);
        data.WriteByteSeq(playerGUID[7]);
        data.WriteByteSeq(playerGUID[4]);
        data.WriteByteSeq(objectGUID[3]);
        data.WriteByteSeq(vehicleGUID[3]);
        data.WriteByteSeq(objectGUID[4]);
        data.WriteByteSeq(vehicleGUID[4]);
        data.WriteByteSeq(playerGUID[3]);

        player->SendDirectMessage(&data);
    }

    GameObjectDestructibleState newState = GetDestructibleState();

    if (!m_goValue.Building.Health)
        newState = GO_DESTRUCTIBLE_DESTROYED;
    else if (m_goValue.Building.Health <= GetGOInfo()->building.damagedNumHits)
        newState = GO_DESTRUCTIBLE_DAMAGED;
    else if (m_goValue.Building.Health == m_goValue.Building.MaxHealth)
        newState = GO_DESTRUCTIBLE_INTACT;

    if (newState == GetDestructibleState())
        return;

    SetDestructibleState(newState, player, false);
}

void GameObject::SetDestructibleState(GameObjectDestructibleState state, Player* eventInvoker /*= NULL*/, bool setHealth /*= false*/)
{
    // the user calling this must know he is already operating on destructible gameobject
    ASSERT(GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING);

    switch (state)
    {
        case GO_DESTRUCTIBLE_INTACT:
            RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_DESTROYED);
            SetDisplayId(m_goInfo->displayId);
            if (setHealth)
            {
                m_goValue.Building.Health = m_goValue.Building.MaxHealth;
                SetGoHealth(255);
            }
            UpdateCollision();
            break;
        case GO_DESTRUCTIBLE_DAMAGED:
        {
            EventInform(m_goInfo->building.damagedEvent);
            sScriptMgr->OnGameObjectDamaged(this, eventInvoker);
            if (eventInvoker)
                if (Battleground* bg = eventInvoker->GetBattleground())
                    bg->EventPlayerDamagedGO(eventInvoker, this, m_goInfo->building.damagedEvent);

            RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DESTROYED);
            SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DAMAGED);

            uint32 modelId = m_goInfo->building.damagedDisplayId;
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->building.destructibleData))
                if (modelData->DamagedDisplayId)
                    modelId = modelData->DamagedDisplayId;
            SetDisplayId(modelId);

            if (setHealth)
            {
                m_goValue.Building.Health = m_goInfo->building.damagedNumHits;
                uint32 maxHealth = m_goValue.Building.MaxHealth;
                // in this case current health is 0 anyway so just prevent crashing here
                if (!maxHealth)
                    maxHealth = 1;
                SetGoHealth(m_goValue.Building.Health * 255 / maxHealth);
            }
            break;
        }
        case GO_DESTRUCTIBLE_DESTROYED:
        {
            sScriptMgr->OnGameObjectDestroyed(this, eventInvoker);
            EventInform(m_goInfo->building.destroyedEvent);
            if (eventInvoker)
            {
                if (Battleground* bg = eventInvoker->GetBattleground())
                {
                    bg->EventPlayerDamagedGO(eventInvoker, this, m_goInfo->building.destroyedEvent);
                    bg->DestroyGate(eventInvoker, this);
                }
            }

            RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DAMAGED);
            SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DESTROYED);

            uint32 modelId = m_goInfo->building.destroyedDisplayId;
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->building.destructibleData))
                if (modelData->DestroyedDisplayId)
                    modelId = modelData->DestroyedDisplayId;
            SetDisplayId(modelId);

            if (setHealth)
            {
                m_goValue.Building.Health = 0;
                SetGoHealth(0);
            }
            UpdateCollision();
            break;
        }
        case GO_DESTRUCTIBLE_REBUILDING:
        {
            EventInform(m_goInfo->building.rebuildingEvent);
            RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_DESTROYED);

            uint32 modelId = m_goInfo->displayId;
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->building.destructibleData))
                if (modelData->RebuildingDisplayId)
                    modelId = modelData->RebuildingDisplayId;
            SetDisplayId(modelId);

            // restores to full health
            if (setHealth)
            {
                m_goValue.Building.Health = m_goValue.Building.MaxHealth;
                SetGoHealth(255);
            }
            UpdateCollision();
            break;
        }
    }
}

void GameObject::SetLootState(LootState state, Unit* unit)
{
    m_lootState = state;
    AI()->OnStateChanged(state, unit);
    sScriptMgr->OnGameObjectLootStateChanged(this, state, unit);
}

void GameObject::SetGoState(GOState state)
{
    SetByteValue(GAMEOBJECT_BYTES_1, 0, state);
    sScriptMgr->OnGameObjectStateChanged(this, state);
    if (m_model)
    {
        if (!IsInWorld())
            return;

        UpdateCollision();
    }
}

void GameObject::SetDisplayId(uint32 displayid)
{
    SetUInt32Value(GAMEOBJECT_FIELD_DISPLAY_ID, displayid);
    UpdateModel();
}

void GameObject::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    WorldObject::SetPhaseMask(newPhaseMask, update);
    if (m_model && m_model->isEnabled())
        UpdateCollision();
}

bool GameObject::SetPhased(uint32 id, bool update, bool apply)
{
    bool res = WorldObject::SetPhased(id, update, apply);
    if (m_model && m_model->isEnabled())
        EnableCollision(true);
    return res;
}

void GameObject::EnableCollision(bool enable)
{
    if (!m_model)
        return;

    /*if (enable && !GetMap()->ContainsGameObjectModel(*m_model))
        GetMap()->InsertGameObjectModel(*m_model);*/

    m_model->enable(enable ? GetPhaseMask() : 0);
}

void GameObject::UpdateCollision()
{
    bool enabled;
    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_GAMEOBJECT_COLLISION, GetEntry(), nullptr))
        enabled = false;
    else if (GetGOInfo()->IsServerOnly())
        enabled = false;
    else
    {
        switch (GetGoType())
        {
            case GAMEOBJECT_TYPE_CHEST:
                enabled = getLootState() == GO_READY;
                break;
            case GAMEOBJECT_TYPE_DOOR:
            case GAMEOBJECT_TYPE_TRAPDOOR:
                enabled = GetGoState() == GO_STATE_READY;
                break;
            case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
                //enabled = !HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                enabled = true;  // Client keeps collisions enabled even for destroyed objects. Display ID swap will handle the rest
                break;
            case GAMEOBJECT_TYPE_TRANSPORT:
            case GAMEOBJECT_TYPE_MO_TRANSPORT:
                enabled = true;
                break;
            default:
                enabled = true;
                break;
        }
    }

    EnableCollision(enabled);
}

void GameObject::UpdateModel()
{
    if (!IsInWorld())
        return;
    if (m_model)
        if (GetMap()->ContainsGameObjectModel(*m_model))
            GetMap()->RemoveGameObjectModel(*m_model);
    delete m_model;
    CreateModel();
    if (m_model)
        GetMap()->InsertGameObjectModel(*m_model);
}

Player* GameObject::GetLootRecipient() const
{
    if (!m_lootRecipient)
        return nullptr;
    return ObjectAccessor::FindPlayer(m_lootRecipient);
}

Group* GameObject::GetLootRecipientGroup() const
{
    if (!m_lootRecipientGroup)
        return nullptr;
    return sGroupMgr->GetGroupByGUID(m_lootRecipientGroup);
}

void GameObject::SetLootRecipient(Unit* unit)
{
    // set the player whose group should receive the right
    // to loot the creature after it dies
    // should be set to NULL after the loot disappears

    if (!unit)
    {
        m_lootRecipient.Clear();
        m_lootRecipientGroup = 0;
        m_lootRecipientGroupMembers.clear();
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
}

bool GameObject::IsLootAllowedFor(Player const* player) const
{
    // If creature is quest tracked and player have the quest, player isn't allowed to loot
    auto trackingQuestId = GetGOInfo()->GetTrackingQuestId();
    if (trackingQuestId && player->IsQuestRewarded(trackingQuestId))
        return false;

    if (!m_lootRecipient && !m_lootRecipientGroup)
        return true;

    if (player->GetGUID() == m_lootRecipient)
        return true;

    Group const* playerGroup = player->GetGroup();
    if (!playerGroup || playerGroup != GetLootRecipientGroup()) // if we dont have a group we arent the recipient
        return false;                                           // if go doesnt have group bound it means it was solo killed by someone else

   if (!IsLootRecipientGroupMember(player->GetGUID()))
        return false;

    return true;
}

void GameObject::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
{
    if (!target)
        return;

    bool forcedFlags = GetGoType() == GAMEOBJECT_TYPE_CHEST && GetGOInfo()->chest.groupLootRules && HasLootRecipient();
    bool targetIsGM = target->IsGameMaster();

    UpdateBuilder builder;
    builder.SetSource(updateType == UPDATETYPE_VALUES ? _changesMask.GetBits() : m_uint32Values, m_valuesCount);
    builder.SetDest(data);

    uint32* flags = nullptr;
    uint32 visibleFlag = GetUpdateFieldData(target, flags);
    if (GetOwnerGUID() == target->GetGUID())
        visibleFlag |= UF_FLAG_OWNER;

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (_fieldNotifyFlags & flags[index] ||
            (builder.GetSrcBit(index) && (flags[index] & visibleFlag)) ||
            (index == GAMEOBJECT_FIELD_FLAGS && forcedFlags)) // There are not many iterations, we can afford it
        {
            builder.SetDestBit(index);

            if (index == OBJECT_FIELD_DYNAMIC_FLAGS)
            {
                uint16 dynFlags = 0;
                int16 pathProgress = -1;
                switch (GetGoType())
                {
                    case GAMEOBJECT_TYPE_CHEST:
                    case GAMEOBJECT_TYPE_GOOBER:
                        if (ActivateToQuest(target))
                            dynFlags |= GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE;
                        else if (targetIsGM)
                            dynFlags |= GO_DYNFLAG_LO_ACTIVATE;
                        break;
                    case GAMEOBJECT_TYPE_GENERIC:
                        if (ActivateToQuest(target))
                            dynFlags |= GO_DYNFLAG_LO_SPARKLE;
                        break;
                    case GAMEOBJECT_TYPE_TRANSPORT:
                    case GAMEOBJECT_TYPE_MO_TRANSPORT:
                    {
                        if (uint32 transportPeriod = GetTransportPeriod())
                        {
                            float timer = float(m_goValue.Transport.PathProgress % transportPeriod);
                            pathProgress = int16(timer / float(transportPeriod) * 65535.0f);
                        }
                        break;
                    }
                    default:
                        break;
                }

                *data << uint16(dynFlags);
                *data << int16(pathProgress);
            }
            else if (index == GAMEOBJECT_FIELD_FLAGS)
            {
                uint32 flags = m_uint32Values[GAMEOBJECT_FIELD_FLAGS];
                if (GetGoType() == GAMEOBJECT_TYPE_CHEST)
                    if (((GetGOInfo()->chest.groupLootRules || GetGOInfo()->GetTrackingQuestId()) && !IsLootAllowedFor(target)) || GetMap()->IsChallengeDungeon())
                        flags |= GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE;

                *data << flags;
            }
            else
                *data << m_uint32Values[index];                // other cases
        }
    }

    builder.Finish();
    *data << uint8(0);
}

void GameObject::GetRespawnPosition(float &x, float &y, float &z, float* ori /* = NULL*/) const
{
    if (m_DBTableGuid)
    {
        if (GameObjectData const* data = sObjectMgr->GetGOData(GetDBTableGUIDLow()))
        {
            x = data->posX;
            y = data->posY;
            z = data->posZ;
            if (ori)
                *ori = data->orientation;
            return;
        }
    }

    x = GetPositionX();
    y = GetPositionY();
    z = GetPositionZ();
    if (ori)
        *ori = GetOrientation();
}
