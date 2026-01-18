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

#include <mutex>
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "ArenaTeam.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BigNumber.h"
#include "CellImpl.h"
#include "CharacterPackets.h"
#include "Chat.h"
#include "CinematicMgr.h"
#include "Common.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "DBCEnums.h"
#include "GameObjectAI.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "Group.h"
#include "GuildMgr.h"
#include "InstanceScript.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Log.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvP.h"
#include "Pet.h"
#include "Player.h"
#include "Realm.h"
#include "SocialMgr.h"
#include "Spell.h"
#include "ScriptMgr.h"
#include "UpdateData.h"
#include "Vehicle.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "zlib.h"

#ifdef ELUNA
#include "HookMgr.h"
#endif
void WorldSession::HandleRepopRequestOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_REPOP_REQUEST Message");

    recvData.read_skip<uint8>();

    if (GetPlayer()->IsAlive() || GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    if (GetPlayer()->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        return; // silently return, client should display the error by itself

    // the world update order is sessions, players, creatures
    // the netcode runs in parallel with all of these
    // creatures can kill players
    // so if the server is lagging enough the player can
    // release spirit after he's killed but before he is updated
    if (GetPlayer()->getDeathState() == JUST_DIED)
    {
        TC_LOG_DEBUG("network", "HandleRepopRequestOpcode: got request after player %s(%d) was killed and before he was updated",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        GetPlayer()->KillPlayer();
    }
#ifdef ELUNA
    sHookMgr->OnRepop(GetPlayer());
#endif

    //this is spirit release confirm?
    GetPlayer()->BuildPlayerRepop();
    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_GOSSIP_SELECT_OPTION");

    ObjectGuid guid;
    uint32 gossipListId;
    uint32 menuId;
    uint8 boxTextLength = 0;
    std::string code = "";

    recvData >> gossipListId >> menuId;

    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    boxTextLength = recvData.ReadBits(8);
    guid[2] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);

    if (_player->PlayerTalkClass->IsGossipOptionCoded(gossipListId))
        code = recvData.ReadString(boxTextLength);

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[1]);

    Item* item = NULL;
    Creature* unit = NULL;
    GameObject* go = NULL;
    if (guid.IsCreatureOrVehicle())
    {
        unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
        if (!unit)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - Unit (GUID: %u) not found or you can't interact with him.", guid.GetCounter());
            return;
        }
    }
    else if (guid.IsGameObject())
    {
        go = _player->GetMap()->GetGameObject(guid);
        if (!go)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - GameObject (GUID: %u) not found.", guid.GetCounter());
            return;
        }
    }
    else if (guid.IsItem())
    {
        item = _player->GetItemByGuid(guid);
        if (!item || _player->IsBankPos(item->GetPos()))
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - %u not found.", guid.GetCounter());
            return;
        }
    }
    else if (guid.IsPlayer())
    {
        if (guid != _player->GetGUID() || menuId != _player->PlayerTalkClass->GetGossipMenu().GetMenuId())
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - %u not found.", guid.GetCounter());
            return;
        }      
    }
    else
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - unsupported GUID type for highguid %u. lowpart %u.", guid.GetHigh(), guid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if ((unit && unit->GetScriptId() != unit->LastUsedScriptID) || (go && go->GetGOInfo()->ScriptId != go->LastUsedScriptID))
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - Script reloaded while in use, ignoring and set new scipt id");
        if (unit)
            unit->LastUsedScriptID = unit->GetCreatureTemplate()->ScriptID;
        if (go)
            go->LastUsedScriptID = go->GetGOInfo()->ScriptId;
        _player->PlayerTalkClass->SendCloseGossip();
        return;
    }
    if (!code.empty())
    {
        if (unit)
        {
            if (!sScriptMgr->OnGossipSelectCode(_player, unit, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str()) && !unit->AI()->OnGossipSelectCode(_player, menuId, gossipListId, code.c_str()))
            {
                _player->OnGossipSelect(unit, gossipListId, menuId);
            }
            // if (!unit->AI()->OnGossipSelectCode(_player, menuId, gossipListId, code.c_str())) // hack, add to support ScriptAI
            // {
            //     _player->OnGossipSelect(unit, gossipListId, menuId);  
            // }
        }
        else if (go)
        {
            go->AI()->OnGossipSelectCode(_player, menuId, gossipListId, code.c_str());
            sScriptMgr->OnGossipSelectCode(_player, go, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str());
        }
        else if (item)
        {
            sScriptMgr->OnGossipSelectCode(_player, item, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str());
        }
        else
        {
            sScriptMgr->OnGossipSelectCode(_player, menuId, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str());
        }
    }
    else
    {
        if (unit)
        {
            if (!sScriptMgr->OnGossipSelect(_player, unit, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId)) && !unit->AI()->OnGossipSelect(_player, menuId, gossipListId))
            {
                 _player->OnGossipSelect(unit, gossipListId, menuId);   
            }
        }
        else if (go)
        {
            go->AI()->OnGossipSelect(_player, menuId, gossipListId);
            if (!sScriptMgr->OnGossipSelect(_player, go, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId)))
                _player->OnGossipSelect(go, gossipListId, menuId);
        }
        else if (item)
        {
            sScriptMgr->OnGossipSelect(_player, item, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId));
        }
        else
        {
            sScriptMgr->OnGossipSelect(_player, menuId, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId));
        }
    }
}

void WorldSession::HandleWhoOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_WHO Message");

    time_t now = time(NULL);
    if (now - timeLastWhoCommand < 5)
    {
        recvData.rfinish();
        return;
    }
    else timeLastWhoCommand = now;

    uint32 levelMin, levelMax, zonesCount, wordCount;
    int32 raceMask, classMask;
    uint32 zoneIds[10];                                     // 10 is client limit
    bool requestServerInfo, showEnemies, showArenaPlayers, exactName;
    uint8 playerLen, guildLen, realmNameLen, guildRealmNameLen;
    std::string playerName, guildName, realmName, guildRealmName;

    recvData >> classMask;                                  // race mask
    recvData >> raceMask;                                   // minimal player level, default 100 (MAX_LEVEL)
    recvData >> levelMax;                                   // maximal player level, default 0
    recvData >> levelMin;                                   // class mask

    showEnemies = recvData.ReadBit();
    exactName = recvData.ReadBit();
    requestServerInfo = recvData.ReadBit();
    guildRealmNameLen = recvData.ReadBits(9);
    showArenaPlayers = recvData.ReadBit();
    playerLen = recvData.ReadBits(6);

    zonesCount = recvData.ReadBits(4);                     // zones count, client limit = 10 (2.0.10)
    if (zonesCount > 10)                                   // can't be received from real client or broken packet
        return;

    realmNameLen = recvData.ReadBits(9);
    guildLen = recvData.ReadBits(7);

    wordCount = recvData.ReadBits(3);
    if (wordCount > 4)
        return;                                            // can't be received from real client or broken packet

    uint8* wordLens = new uint8[wordCount];
    std::string* words = new std::string[wordCount];

    for (uint8 i = 0; i < wordCount; i++)
        wordLens[i] = recvData.ReadBits(7);

    std::wstring wWords[4];
    for (uint32 i = 0; i < wordCount; ++i)
    {
        std::string temp = recvData.ReadString(wordLens[i]); // user entered string, it used as universal search pattern(guild+player name)?

        if (!Utf8toWStr(temp, wWords[i]))
            continue;

        wstrToLower(wWords[i]);

        TC_LOG_DEBUG("network", "String %u: %s", i, temp.c_str());
    }

    guildRealmName = recvData.ReadString(guildRealmNameLen);

    for (uint32 i = 0; i < zonesCount; ++i)
    {
        recvData >> zoneIds[i];                            // zone id, 0 if zone is unknown...
        TC_LOG_DEBUG("network", "Zone %u: %u", i, zoneIds[i]);
    }

    // NYI
    playerName = recvData.ReadString(playerLen);
    realmName = recvData.ReadString(realmNameLen);
    guildName = recvData.ReadString(guildLen);

    uint32 virtualRealmAddress = 0;
    int32 faction = 0, locale = 0;

    if (requestServerInfo)
    {
        recvData >> locale;
        recvData >> virtualRealmAddress;
        recvData >> faction;
    }

    TC_LOG_DEBUG("network", "Minlvl %u, maxlvl %u, name %s, guild %s, racemask %u, classmask %u, zones %u, strings %u", levelMin, levelMax, playerName.c_str(), guildName.c_str(), raceMask, classMask, zonesCount, wordCount);

    std::wstring wPlayerName, wGuildName;
    if (!(Utf8toWStr(playerName, wPlayerName) && Utf8toWStr(guildName, wGuildName)))
        return;

    wstrToLower(wPlayerName);
    wstrToLower(wGuildName);

    // SMSG_WHO
    // client send in case not set max level value 100 but Trinity supports 255 max level,
    // update it to show GMs with characters after 100 level
    if (levelMax >= MAX_LEVEL)
        levelMax = STRONG_MAX_LEVEL;

    uint32 team = _player->GetTeam();
    uint32 security = GetSecurity();
    bool allowTwoSideWhoList = sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_WHO_LIST);
    uint32 gmLevelInWhoList  = sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST);
    uint8 displaycount = 0;

    ByteBuffer bytesData;
    WorldPacket data(SMSG_WHO);

    size_t pos = data.bitwpos();
    data.WriteBits(displaycount, 6);

    std::shared_lock<std::shared_mutex> lock(*HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType const& m = ObjectAccessor::GetPlayers();
    for (auto itr = m.begin(); itr != m.end() && displaycount <= sWorld->getIntConfig(CONFIG_MAX_WHO); ++itr)
    {
        Player* target = itr->second;
        if (security == SEC_PLAYER)
        {
            // player can see member of other team only if CONFIG_ALLOW_TWO_SIDE_WHO_LIST
            if (target->GetTeam() != team && !allowTwoSideWhoList)
                continue;

            // player can see MODERATOR, GAME MASTER, ADMINISTRATOR only if CONFIG_GM_IN_WHO_LIST
            if (target->GetSession()->GetSecurity() > AccountTypes(gmLevelInWhoList))
                continue;
        }

        // do not process players which are not in world
        if (!target->IsInWorld())
            continue;

        // check if target is globally visible for player
        if (!target->IsVisibleGloballyFor(_player))
            continue;

        // check if target's level is in level range
        uint8 level = target->GetLevel();
        if (level < levelMin || level > levelMax)
            continue;

        // check if class matches classmask
        uint8 class_ = target->GetClass();
        if (!(classMask & (1 << class_)))
            continue;

        // check if race matches racemask
        uint32 race = target->GetRace();
        if (!(raceMask & (1 << race)))
            continue;

        uint32 zoneId = target->GetZoneId();
        uint8 gender = target->GetGender();

        bool z_show = true;
        for (uint32 i = 0; i < zonesCount; ++i)
        {
            if (zoneIds[i] == zoneId)
            {
                z_show = true;
                break;
            }

            z_show = false;
        }
        if (!z_show)
            continue;

        std::string pname = target->GetName();
        std::wstring wpname;
        if (!Utf8toWStr(pname, wpname))
            continue;
        wstrToLower(wpname);

        if (!(wPlayerName.empty() || wpname.find(wPlayerName) != std::wstring::npos))
            continue;

        std::string gname = sGuildMgr->GetGuildNameById(target->GetGuildId());
        std::wstring wgname;
        if (!Utf8toWStr(gname, wgname))
            continue;
        wstrToLower(wgname);

        if (!(wGuildName.empty() || wgname.find(wGuildName) != std::wstring::npos))
            continue;

        std::string aname;
        if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(zoneId))
            aname = areaEntry->area_name[GetSessionDbcLocale()];

        bool s_show = true;
        for (uint32 i = 0; i < wordCount; ++i)
        {
            if (!wWords[i].empty())
            {
                if (wgname.find(wWords[i]) != std::wstring::npos ||
                    wpname.find(wWords[i]) != std::wstring::npos ||
                    Utf8FitTo(aname, wWords[i]))
                {
                    s_show = true;
                    break;
                }
                s_show = false;
            }
        }
        if (!s_show)
            continue;

        ObjectGuid playerGuid = target->GetGUID();
        ObjectGuid accountId = ObjectGuid(HighGuid::WowAccount, target->GetSession()->GetAccountId());
        ObjectGuid guildGuid = target->GetGuild() ? target->GetGuild()->GetGUID() : ObjectGuid::Empty;

        data.WriteBit(accountId[2]);
        data.WriteBit(playerGuid[2]);
        data.WriteBit(accountId[7]);
        data.WriteBit(guildGuid[5]);
        data.WriteBits(gname.size(), 7);
        data.WriteBit(accountId[1]);
        data.WriteBit(accountId[5]);
        data.WriteBit(guildGuid[7]);
        data.WriteBit(playerGuid[5]);
        data.WriteBit(false);
        data.WriteBit(guildGuid[1]);
        data.WriteBit(playerGuid[6]);
        data.WriteBit(guildGuid[2]);
        data.WriteBit(playerGuid[4]);
        data.WriteBit(guildGuid[0]);
        data.WriteBit(guildGuid[3]);
        data.WriteBit(accountId[6]);
        data.WriteBit(false);
        data.WriteBit(playerGuid[1]);
        data.WriteBit(guildGuid[4]);
        data.WriteBit(accountId[0]);

        if (DeclinedName const* names = itr->second->GetDeclinedNames())
        {
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                data.WriteBits(names->name[i].size(), 7);
        }
        else
        {
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                data.WriteBits(0, 7);
        }

        data.WriteBit(playerGuid[3]);
        data.WriteBit(guildGuid[6]);
        data.WriteBit(playerGuid[0]);
        data.WriteBit(accountId[4]);
        data.WriteBit(accountId[3]);
        data.WriteBit(playerGuid[7]);
        data.WriteBits(pname.size(), 6);

        bytesData.WriteByteSeq(playerGuid[1]);
        bytesData << uint32(realm.Id.Realm);
        bytesData.WriteByteSeq(playerGuid[7]);
        bytesData << uint32(realm.Id.Realm);
        bytesData.WriteByteSeq(playerGuid[4]);
        bytesData.WriteString(pname);
        bytesData.WriteByteSeq(guildGuid[1]);
        bytesData.WriteByteSeq(playerGuid[0]);
        bytesData.WriteByteSeq(guildGuid[2]);
        bytesData.WriteByteSeq(guildGuid[0]);
        bytesData.WriteByteSeq(guildGuid[4]);
        bytesData.WriteByteSeq(playerGuid[3]);
        bytesData.WriteByteSeq(guildGuid[6]);
        bytesData << uint32(target->GetSession()->GetAccountId());
        bytesData.WriteString(gname);
        bytesData.WriteByteSeq(guildGuid[3]);
        bytesData.WriteByteSeq(accountId[4]);
        bytesData << uint8(class_);
        bytesData.WriteByteSeq(accountId[7]);
        bytesData.WriteByteSeq(playerGuid[6]);
        bytesData.WriteByteSeq(playerGuid[2]);

        if (DeclinedName const* names = itr->second->GetDeclinedNames())
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                bytesData.WriteString(names->name[i]);

        bytesData.WriteByteSeq(accountId[2]);
        bytesData.WriteByteSeq(accountId[3]);
        bytesData << uint8(race);
        bytesData.WriteByteSeq(guildGuid[7]);
        bytesData.WriteByteSeq(accountId[1]);
        bytesData.WriteByteSeq(accountId[5]);
        bytesData.WriteByteSeq(accountId[6]);
        bytesData.WriteByteSeq(playerGuid[5]);
        bytesData.WriteByteSeq(accountId[0]);
        bytesData << uint8(gender);
        bytesData.WriteByteSeq(guildGuid[5]);
        bytesData << uint8(level);
        bytesData << int32(zoneId);

        ++displaycount;
    }

    data.FlushBits();
    data.PutBits(pos, displaycount, 6);
    data.append(bytesData);

    SendPacket(&data);

    delete[] words;
    delete[] wordLens;

    TC_LOG_DEBUG("network", "WORLD: Send SMSG_WHO Message");
}

void WorldSession::HandleLogoutRequestOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_LOGOUT_REQUEST Message, security - %u", GetSecurity());

    if (ObjectGuid lguid = GetPlayer()->GetLootGUID())
        DoLootRelease(lguid);

    bool instantLogout = GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || GetPlayer()->IsInFlight() ||
        GetSecurity() >= AccountTypes(sWorld->getIntConfig(CONFIG_INSTANT_LOGOUT));

    /// TODO: Possibly add RBAC permission to log out in combat
    bool canLogoutInCombat = GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_RESTING);

    uint32 reason = 0;
    if (GetPlayer()->IsInCombat() && !canLogoutInCombat)
        reason = 1;
    else if (GetPlayer()->IsFalling())
        reason = 3;                                         // is jumping or falling
    else if (GetPlayer()->duel || GetPlayer()->HasAura(9454)) // is dueling or frozen by GM via freeze command
        reason = 2;                                         // FIXME - Need the correct value

    WorldPackets::Character::LogoutResponse logoutResponse;
    logoutResponse.LogoutResult = reason;
    logoutResponse.Instant = instantLogout;
    SendPacket(logoutResponse.Write());

    if (reason)
    {
        SetLogoutStartTime(0);
        return;
    }

    // instant logout in taverns/cities or on taxi or for admins, gm's, mod's if its enabled in worldserver.conf
    if (instantLogout)
    {
        LogoutPlayer(true);
        return;
    }

    // not set flags if player can't free move to prevent lost state at logout cancel
    if (GetPlayer()->CanFreeMove())
    {
        GetPlayer()->SetStandState(UNIT_STAND_STATE_SIT);
        GetPlayer()->SetRooted(true);
        GetPlayer()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    SetLogoutStartTime(GameTime::GetGameTime());
}

void WorldSession::HandlePlayerLogoutOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_PLAYER_LOGOUT Message");
}

void WorldSession::HandleLogoutCancelOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_LOGOUT_CANCEL Message");

    // Player have already logged out serverside, too late to cancel
    if (!GetPlayer())
        return;

    SetLogoutStartTime(0);

    WorldPacket data(SMSG_LOGOUT_CANCEL_ACK, 0);
    SendPacket(&data);

    // not remove flags if can't free move - its not set in Logout request code.
    if (GetPlayer()->CanFreeMove())
    {
        //!we can move again
        GetPlayer()->SetRooted(false);

        //! Stand Up
        GetPlayer()->SetStandState(UNIT_STAND_STATE_STAND);

        //! DISABLE_ROTATE
        GetPlayer()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_LOGOUT_CANCEL_ACK Message");
}

void WorldSession::HandleSetPvP(WorldPackets::Misc::SetPvP& packet)
{
    if (packet.EnablePVP) // TODO missing PVP timer
    {
        GetPlayer()->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
    }
    else
    {
        GetPlayer()->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
    }

    GetPlayer()->UpdatePvP(GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP));
}

void WorldSession::HandleTogglePvP(WorldPackets::Misc::TogglePvP& togglePvP)
{
    // this opcode can be used in two ways: Either set explicit new status or toggle old status
    if (togglePvP.Enable) // TODO missing PVP timer
    {
        GetPlayer()->ApplyModFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP, *togglePvP.Enable);
    }
    else
    {
        GetPlayer()->ToggleFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
    }
    
    GetPlayer()->UpdatePvP(GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP));
}

void WorldSession::HandleZoneUpdateOpcode(WorldPacket& recvData)
{
    uint32 newZone;
    recvData >> newZone;

    TC_LOG_DEBUG("network", "WORLD: Recvd ZONE_UPDATE: %u", newZone);

    // use server side data, but only after update the player position. See Player::UpdatePosition().
    GetPlayer()->SetNeedsZoneUpdate(true);

    //GetPlayer()->SendInitWorldStates(true, newZone);
}

void WorldSession::HandleReturnToGraveyard(WorldPacket& /*recvPacket*/)
{
    if (GetPlayer()->IsAlive() || !GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleRequestCemeteryList(WorldPacket& /*recvPacket*/)
{
    uint32 zoneId = _player->GetZoneId();
    uint32 team = _player->GetTeam();

    std::vector<uint32> GraveyardIds;

    GraveYardContainer::const_iterator it;
    std::pair<GraveYardContainer::const_iterator, GraveYardContainer::const_iterator> range;
    range = sObjectMgr->GraveYardStore.equal_range(zoneId);
        
    for (it = range.first; it != range.second && GraveyardIds.size() < 16; ++it) // client max
    {
        if (it->second.team == 0 || it->second.team == team)
            GraveyardIds.push_back(it->first);
    }

    if (GraveyardIds.empty())
    {
        TC_LOG_DEBUG("network", "No graveyards found for zone %u for %u (team %u) in CMSG_REQUEST_CEMETERY_LIST", zoneId, m_GUIDLow, team);
        return;
    }

    bool IsGossipTriggered = false;
    WorldPacket data(SMSG_REQUEST_CEMETERY_LIST_RESPONSE, 4 + 4 * GraveyardIds.size());

    data.WriteBits(GraveyardIds.size(), 22);
    data.WriteBit(IsGossipTriggered);
    data.FlushBits();

    for (uint32 i = 0; i < GraveyardIds.size(); ++i)
        data << uint32(GraveyardIds[i]);

    SendPacket(&data);
}

void WorldSession::HandleSetSelectionOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[7] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);

    _player->SetSelection(guid);
}

void WorldSession::HandleStandStateChangeOpcode(WorldPacket& recvData)
{
    // TC_LOG_DEBUG("network", "WORLD: Received CMSG_STANDSTATECHANGE"); -- too many spam in log at lags/debug stop
    uint32 animstate;
    recvData >> animstate;

    switch (animstate)
    {
        case UNIT_STAND_STATE_STAND:
        case UNIT_STAND_STATE_SIT:
        case UNIT_STAND_STATE_SLEEP:
        case UNIT_STAND_STATE_KNEEL:
            break;
        default:
            return;
    }

    _player->SetStandState(UnitStandStateType(animstate));
}

void WorldSession::HandleContactListOpcode(WorldPacket& recvData)
{
    recvData.read_skip<uint32>(); // always 1
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_CONTACT_LIST");
    _player->GetSocial()->SendSocialList(_player);
}

void WorldSession::HandleAddFriendOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_ADD_FRIEND");

    std::string friendName = GetTrinityString(LANG_FRIEND_IGNORE_UNKNOWN);
    std::string friendNote;

    recvData >> friendName;
    recvData >> friendNote;

    if (!normalizePlayerName(friendName))
        return;

    TC_LOG_DEBUG("network", "WORLD: %s asked to add friend : '%s'",
        GetPlayer()->GetName().c_str(), friendName.c_str());

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_RACE_ACC_BY_NAME);

    stmt->setString(0, friendName);

    // _addFriendCallback.SetParam(friendNote);
    // _addFriendCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));

    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery(stmt)
        .WithChainingPreparedCallback([this, friendNote](QueryCallback& queryCallback, PreparedQueryResult result)
    {

        if (!GetPlayer())
            return;

        ObjectGuid friendGuid = ObjectGuid::Empty;
        uint32 friendAccountId;
        uint32 team;
        FriendsResult friendResult;

        friendResult = FRIEND_NOT_FOUND;

        if (result)
        {
            Field* fields = result->Fetch();

            friendGuid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
            team = Player::TeamForRace(fields[1].GetUInt8());
            friendAccountId = fields[2].GetUInt32();

            if (GetSecurity() >= SEC_MODERATOR || sWorld->getBoolConfig(CONFIG_ALLOW_GM_FRIEND) || AccountMgr::GetSecurity(friendAccountId, realm.Id.Realm) < SEC_MODERATOR)
            {
                if (friendGuid)
                {
                    if (friendGuid == GetPlayer()->GetGUID())
                        friendResult = FRIEND_SELF;
                    else if (GetPlayer()->GetTeam() != team && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND) && GetSecurity() < SEC_MODERATOR)
                        friendResult = FRIEND_ENEMY;
                    else if (GetPlayer()->GetSocial()->HasFriend(friendGuid))
                        friendResult = FRIEND_ALREADY;
                    else
                    {
                        Player* pFriend = ObjectAccessor::FindPlayer(friendGuid);
                        if (pFriend && pFriend->IsInWorld() && pFriend->IsVisibleGloballyFor(GetPlayer()))
                            friendResult = FRIEND_ADDED_ONLINE;
                        else
                            friendResult = FRIEND_ADDED_OFFLINE;
                        if (!GetPlayer()->GetSocial()->AddToSocialList(friendGuid, false))
                        {
                            friendResult = FRIEND_LIST_FULL;
                            TC_LOG_DEBUG("network", "WORLD: %s's friend list is full.", GetPlayer()->GetName().c_str());
                        }
                    }
                    GetPlayer()->GetSocial()->SetFriendNote(friendGuid, friendNote);
                }
            }
        }

        sSocialMgr->SendFriendStatus(GetPlayer(), friendResult, friendGuid, false);

        TC_LOG_DEBUG("network", "WORLD: Sent (SMSG_FRIEND_STATUS)");

    }));



}

void WorldSession::HandleAddFriendOpcodeCallBack(PreparedQueryResult result, std::string const& friendNote)
{
    // if (!GetPlayer())
    //     return;

    // uint64 friendGuid;
    // uint32 friendAccountId;
    // uint32 team;
    // FriendsResult friendResult;

    // friendResult = FRIEND_NOT_FOUND;
    // friendGuid = 0;

    // if (result)
    // {
    //     Field* fields = result->Fetch();

    //     friendGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HighGuid::Player);
    //     team = Player::TeamForRace(fields[1].GetUInt8());
    //     friendAccountId = fields[2].GetUInt32();

    //     if (GetSecurity() >= SEC_MODERATOR || sWorld->getBoolConfig(CONFIG_ALLOW_GM_FRIEND) || AccountMgr::GetSecurity(friendAccountId, realm.Id.Realm) < SEC_MODERATOR)
    //     {
    //         if (friendGuid)
    //         {
    //             if (friendGuid == GetPlayer()->GetGUID())
    //                 friendResult = FRIEND_SELF;
    //             else if (GetPlayer()->GetTeam() != team && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND) && GetSecurity() < SEC_MODERATOR)
    //                 friendResult = FRIEND_ENEMY;
    //             else if (GetPlayer()->GetSocial()->HasFriend(GUID_LOPART(friendGuid)))
    //                 friendResult = FRIEND_ALREADY;
    //             else
    //             {
    //                 Player* pFriend = ObjectAccessor::FindPlayer(friendGuid);
    //                 if (pFriend && pFriend->IsInWorld() && pFriend->IsVisibleGloballyFor(GetPlayer()))
    //                     friendResult = FRIEND_ADDED_ONLINE;
    //                 else
    //                     friendResult = FRIEND_ADDED_OFFLINE;
    //                 if (!GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(friendGuid), false))
    //                 {
    //                     friendResult = FRIEND_LIST_FULL;
    //                     TC_LOG_DEBUG("network", "WORLD: %s's friend list is full.", GetPlayer()->GetName().c_str());
    //                 }
    //             }
    //             GetPlayer()->GetSocial()->SetFriendNote(GUID_LOPART(friendGuid), friendNote);
    //         }
    //     }
    // }

    // sSocialMgr->SendFriendStatus(GetPlayer(), friendResult, friendGuid, false);

    // TC_LOG_DEBUG("network", "WORLD: Sent (SMSG_FRIEND_STATUS)");
}

void WorldSession::HandleDelFriendOpcode(WorldPacket& recvData)
{
    ObjectGuid friendGUID;

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_DEL_FRIEND");

    recvData >> friendGUID;

    _player->GetSocial()->RemoveFromSocialList(friendGUID, false);

    sSocialMgr->SendFriendStatus(GetPlayer(), FRIEND_REMOVED, friendGUID, false);

    TC_LOG_DEBUG("network", "WORLD: Sent motd (SMSG_FRIEND_STATUS)");
}

void WorldSession::HandleAddIgnoreOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_ADD_IGNORE");

    std::string ignoreName = GetTrinityString(LANG_FRIEND_IGNORE_UNKNOWN);

    recvData >> ignoreName;

    if (!normalizePlayerName(ignoreName))
        return;

    TC_LOG_DEBUG("network", "WORLD: %s asked to Ignore: '%s'",
        GetPlayer()->GetName().c_str(), ignoreName.c_str());

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);

    stmt->setString(0, ignoreName);

    //_addIgnoreCallback = CharacterDatabase.AsyncQuery(stmt);
    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery(stmt)
        .WithPreparedCallback(std::bind(&WorldSession::HandleAddIgnoreOpcodeCallBack, this, std::placeholders::_1 )));
}

void WorldSession::HandleAddIgnoreOpcodeCallBack(PreparedQueryResult result)
{
    if (!GetPlayer())
        return;

    ObjectGuid ignoreGuid = ObjectGuid::Empty;
    FriendsResult ignoreResult;

    ignoreResult = FRIEND_IGNORE_NOT_FOUND;

    if (result)
    {
        ignoreGuid = ObjectGuid(HighGuid::Player, (*result)[0].GetUInt32());

        if (ignoreGuid)
        {
            if (ignoreGuid == GetPlayer()->GetGUID())              //not add yourself
                ignoreResult = FRIEND_IGNORE_SELF;
            else if (GetPlayer()->GetSocial()->HasIgnore(ignoreGuid))
                ignoreResult = FRIEND_IGNORE_ALREADY;
            else
            {
                ignoreResult = FRIEND_IGNORE_ADDED;

                // ignore list full
                if (!GetPlayer()->GetSocial()->AddToSocialList(ignoreGuid, true))
                    ignoreResult = FRIEND_IGNORE_FULL;
            }
        }
    }

    sSocialMgr->SendFriendStatus(GetPlayer(), ignoreResult, ignoreGuid, false);

    TC_LOG_DEBUG("network", "WORLD: Sent (SMSG_FRIEND_STATUS)");
}

void WorldSession::HandleDelIgnoreOpcode(WorldPacket& recvData)
{
    ObjectGuid ignoreGUID;

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_DEL_IGNORE");

    recvData >> ignoreGUID;

    _player->GetSocial()->RemoveFromSocialList(ignoreGUID, true);

    sSocialMgr->SendFriendStatus(GetPlayer(), FRIEND_IGNORE_REMOVED, ignoreGUID, false);

    TC_LOG_DEBUG("network", "WORLD: Sent motd (SMSG_FRIEND_STATUS)");
}

void WorldSession::HandleSetContactNotesOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_SET_CONTACT_NOTES");
    ObjectGuid guid;
    std::string note;
    recvData >> guid >> note;
    _player->GetSocial()->SetFriendNote(guid, note);
}

void WorldSession::HandleBugOpcode(WorldPacket& recvData)
{
    uint32 suggestion, contentlen, typelen;
    std::string content, type;

    recvData >> suggestion >> contentlen;
    content = recvData.ReadString(contentlen);

    recvData >> typelen;
    type = recvData.ReadString(typelen);

    if (suggestion == 0)
        TC_LOG_DEBUG("network", "WORLD: Received CMSG_BUG [Bug Report]");
    else
        TC_LOG_DEBUG("network", "WORLD: Received CMSG_BUG [Suggestion]");

    TC_LOG_DEBUG("network", "%s", type.c_str());
    TC_LOG_DEBUG("network", "%s", content.c_str());

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_BUG_REPORT);

    stmt->setString(0, type);
    stmt->setString(1, content);

    CharacterDatabase.Execute(stmt);
}

void WorldSession::HandleReclaimCorpseOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RECLAIM_CORPSE");

    ObjectGuid guid;

    guid[1] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);

    if (GetPlayer()->IsAlive())
        return;

    // do not allow corpse reclaim in arena
    if (GetPlayer()->InArena())
        return;

    // body not released yet
    if (!GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    Corpse* corpse = GetPlayer()->GetCorpse();
    if (!corpse)
        return;

    // prevent resurrect before 30-sec delay after body release not finished
    if (time_t(corpse->GetGhostTime() + GetPlayer()->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP)) > time_t(time(NULL)))
        return;

    if (!corpse->IsWithinDistInMap(GetPlayer(), CORPSE_RECLAIM_RADIUS, true))
        return;
#ifdef ELUNA
    sHookMgr->OnResurrect(GetPlayer());
#endif

    // resurrect
    GetPlayer()->ResurrectPlayer(GetPlayer()->InBattleground() ? 1.0f : 0.5f);

    // spawn bones
    GetPlayer()->SpawnCorpseBones();
}

void WorldSession::HandleResurrectResponseOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RESURRECT_RESPONSE");

    ObjectGuid guid;
    uint32 status;

    recvData >> status;

    guid[3] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);

    if (GetPlayer()->IsAlive())
        return;

    if (status == 1)
    {
        GetPlayer()->ClearResurrectRequestData();           // reject
        return;
    }

    if (!GetPlayer()->IsRessurectRequestedBy(guid))
        return;

    GetPlayer()->ResurrectUsingRequestData();
}

void WorldSession::HandleAreaTriggerOpcode(WorldPacket& recvData)
{
    uint32 areaTriggerId;
    uint8 entered, fromClient;
    recvData >> areaTriggerId;
    fromClient = recvData.ReadBit();
    entered = recvData.ReadBit();

    TC_LOG_DEBUG("network", "CMSG_AREATRIGGER. Trigger ID: %u, Entered: %u, From Client: %u", areaTriggerId, entered, fromClient);

    Player* player = GetPlayer();
    if (player->IsInFlight())
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) in flight, ignore Area Trigger ID:%u",
            player->GetName().c_str(), player->GetGUID().GetCounter(), areaTriggerId);
        return;
    }

    AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(areaTriggerId);
    if (!atEntry)
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) send unknown (by DBC) Area Trigger ID:%u",
            player->GetName().c_str(), player->GetGUID().GetCounter(), areaTriggerId);
        return;
    }

    if (entered != player->IsInAreaTrigger(atEntry))
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) too far (trigger map: %u player map: %u), ignore Area Trigger ID: %u",
                     player->GetName().c_str(), atEntry->ContinentID, player->GetMapId(), player->GetGUID().GetCounter(), areaTriggerId);
        return;
    }

    if (player->isDebugAreaTriggers)
      ChatHandler(player->GetSession()).PSendSysMessage(entered ? LANG_DEBUG_AREATRIGGER_ENTERED : LANG_DEBUG_AREATRIGGER_LEFT, areaTriggerId);

    if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_AREATRIGGER_CLIENT_TRIGGERED, atEntry->ID, player))
        return;

    if (sScriptMgr->OnAreaTrigger(player, atEntry, entered))
        return;

    if (player->IsAlive() && entered)
    {
        if (std::unordered_set<uint32> const* quests = sObjectMgr->GetQuestGiverForAreaTrigger(areaTriggerId))
        {
            for (uint32 questId : *quests)
            {
                Quest const *quest = sObjectMgr->GetQuestTemplate(questId);
                if (quest && player->GetQuestStatus(questId) == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false) && player->CanAddQuest(quest, false))
                {
                    player->AddQuest(quest, player);
                    player->PlayerTalkClass->SendQuestGiverQuestDetails(quest, player->GetGUID(), true, true);
                    if (_player->CanCompleteQuest(quest->GetQuestId()))
                        _player->CompleteQuest(quest->GetQuestId());
                }
            }
        }

        if (std::unordered_set<uint32> const* quests = sObjectMgr->GetQuestsForAreaTrigger(areaTriggerId))
        {
            for (uint32 questId : *quests)
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
                uint16 slot = player->FindQuestSlot(questId);
                if (quest && slot < MAX_QUEST_LOG_SIZE && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                {
                    for (const auto& objective : quest->Objectives)
                    {
                        if (objective.Type == QUEST_OBJECTIVE_AREATRIGGER)
                        {
                            player->m_questObjectiveStatus[objective.ID] += 1;
                            player->MarkQuestObjectiveToSave(questId, objective.ID);
                            player->SendQuestUpdateAddCreditSimple(quest, &objective);
                            break;
                        }
                    }

                    if (quest->HasFlag(QUEST_FLAGS_COMPLETION_AREA_TRIGGER))
                        player->AreaExploredOrEventHappens(questId);

                    if (player->CanCompleteQuest(questId))
                        player->CompleteQuest(questId);
                }
            }
        }
    }

    if (sObjectMgr->IsTavernAreaTrigger(areaTriggerId))
    {
        // set resting flag we are in the inn
        if (entered)
            player->SetRestFlag(REST_FLAG_IN_TAVERN, atEntry->ID);
        else
            player->RemoveRestFlag(REST_FLAG_IN_TAVERN);

        if (sWorld->IsFFAPvPRealm())
        {
            if (entered)
                player->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
            else
                player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
        }

        return;
    }

    if (Battleground* bg = player->GetBattleground())
        if (bg->GetStatus() == STATUS_IN_PROGRESS)
            bg->HandleAreaTrigger(player, areaTriggerId, entered);

    if (OutdoorPvP* pvp = player->GetOutdoorPvP())
        if (pvp->HandleAreaTrigger(_player, areaTriggerId, entered))
            return;

    AreaTriggerStruct const* at = sObjectMgr->GetAreaTrigger(areaTriggerId);
    if (!at)
        return;

    bool teleported = false;
    if (GetPlayer()->GetGroup() && GetPlayer()->GetGroup()->isLFGGroup() && GetPlayer()->GetMap()->IsDungeon() && at->target_mapId != GetPlayer()->GetMapId())
    {
        if (!sMapMgr->CanPlayerEnter(at->target_mapId, player, false))
        {
            player->SendAreaTriggerDenied(areaTriggerId, false);
            return;
        }

        if (GetPlayer()->GetGroup()->GetMembersCount() == 1)
        {
            GetPlayer()->RemoveFromGroup(GROUP_REMOVEMETHOD_LEAVE);
            teleported = GetPlayer()->IsBeingTeleportedFar();
        }
        else
            teleported = GetPlayer()->TeleportToBGEntryPoint();
    }

    if (!teleported)
    {
        float x = at->target_X;
        float y = at->target_Y;
        float z = at->target_Z;
        float o = at->target_Orientation;
        if (player->GetGroup() && player->GetGroup()->isLFGGroup())
        {
            if (auto const* dungeon = sLFGMgr->GetLFGDungeon(sLFGMgr->GetDungeon(player->GetGroup()->GetGUID())))
            {
                if (at->target_mapId == dungeon->map)
                {
                    x = dungeon->x;
                    y = dungeon->y;
                    z = dungeon->z;
                    o = dungeon->o;
                }
            }
        }
        player->TeleportTo(at->target_mapId, x, y, z, o, TELE_TO_NOT_LEAVE_TRANSPORT);
    }
}

void WorldSession::HandleUpdateAccountData(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_UPDATE_ACCOUNT_DATA");

    uint32 timestamp, type = 0, decompressedSize, compressedSize;
    recvData >> decompressedSize  >> timestamp >> compressedSize;

    if (decompressedSize == 0)                               // erase
    {
        type = recvData.ReadBits(3);
        recvData.FlushBits();

        SetAccountData(AccountDataType(type), 0, "");
        return;
    }

    if (decompressedSize > 0xFFFF)
    {
        recvData.rfinish();                   // unnneded warning spam in this case
        TC_LOG_ERROR("network", "UAD: Account data packet too big, size %u", decompressedSize);
        return;
    }

    ByteBuffer dest;
    dest.resize(decompressedSize);

    uLongf realSize = decompressedSize;
    if (uncompress(const_cast<uint8*>(dest.contents()), &realSize, const_cast<uint8*>(recvData.contents() + recvData.rpos()), compressedSize) != Z_OK)
    {
        recvData.rfinish();                   // unnneded warning spam in this case
        TC_LOG_ERROR("network", "UAD: Failed to decompress account data");
        return;
    }

    std::string adata = dest.ReadString(decompressedSize);

    recvData.read_skip(compressedSize);
    type = recvData.ReadBits(3);

    if (type > NUM_ACCOUNT_DATA_TYPES)
        return;

    TC_LOG_DEBUG("network", "UAD: type %u, time %u, decompressedSize %u", type, timestamp, decompressedSize);

    SetAccountData(AccountDataType(type), timestamp, adata);
}

void WorldSession::HandleRequestAccountData(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_REQUEST_ACCOUNT_DATA");

    uint32 type = recvData.ReadBits(3);

    TC_LOG_DEBUG("network", "RAD: type %u", type);

    if (type > NUM_ACCOUNT_DATA_TYPES)
        return;

    AccountData* adata = GetAccountData(AccountDataType(type));

    uint32 size = adata->Data.size();

    uLongf destSize = compressBound(size);

    ByteBuffer dest;
    dest.resize(destSize);

    if (size && compress(dest.contents(), &destSize, (uint8 const*)adata->Data.c_str(), size) != Z_OK)
    {
        TC_LOG_DEBUG("network", "RAD: Failed to compress account data");
        return;
    }

    dest.resize(destSize);

    WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA, 8 + 4 + 4 + 4 + destSize);

    ObjectGuid guid = GetPlayer() ? GetPlayer()->GetGUID() : ObjectGuid::Empty;

    data.WriteBits(type, 3);       // type (0-7)
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);

    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[5]);
    data << uint32(size);         // decompressed length
    data << uint32(destSize);
    data.append(dest);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[2]);
    data << uint32(adata->Time); // unix time
    
    SendPacket(&data);
}

int32 WorldSession::HandleEnableNagleAlgorithm()
{
    // Instructs the server we wish to receive few amounts of large packets (SMSG_MULTIPLE_PACKETS?)
    // instead of large amount of small packets
    return 0;
}

void WorldSession::HandleSetActionButtonOpcode(WorldPacket& recvData)
{
    ObjectGuid buttonStream;
    uint8 slotId;

    recvData >> slotId;

    buttonStream[7] = recvData.ReadBit();
    buttonStream[0] = recvData.ReadBit();
    buttonStream[5] = recvData.ReadBit();
    buttonStream[2] = recvData.ReadBit();
    buttonStream[1] = recvData.ReadBit();
    buttonStream[6] = recvData.ReadBit();
    buttonStream[3] = recvData.ReadBit();
    buttonStream[4] = recvData.ReadBit();

    recvData.ReadByteSeq(buttonStream[6]);
    recvData.ReadByteSeq(buttonStream[7]);
    recvData.ReadByteSeq(buttonStream[3]);
    recvData.ReadByteSeq(buttonStream[5]);
    recvData.ReadByteSeq(buttonStream[2]);
    recvData.ReadByteSeq(buttonStream[1]);
    recvData.ReadByteSeq(buttonStream[4]);
    recvData.ReadByteSeq(buttonStream[0]);

    ActionButtonPACKET* button = reinterpret_cast<ActionButtonPACKET*>(&buttonStream);
    button->type >>= 24;

    TC_LOG_DEBUG("network", "CMSG_SET_ACTION_BUTTON slotId: %u actionId: %u", slotId, button->id);

    if (!button->id)
        GetPlayer()->RemoveActionButton(slotId);
    else
        GetPlayer()->AddActionButton(slotId, button->id, button->type);
}

void WorldSession::HandleCompleteCinematic(WorldPackets::Misc::CompleteCinematic& /*packet*/)
{
    // If player has sight bound to visual waypoint NPC we should remove it
    GetPlayer()->GetCinematicMgr()->EndCinematic();    
}

void WorldSession::HandleNextCinematicCamera(WorldPackets::Misc::NextCinematicCamera& /*packet*/)
{
    // Sent by client when cinematic actually begun. So we begin the server side process
    GetPlayer()->GetCinematicMgr()->NextCinematicCamera();
}

void WorldSession::HandleCompleteMovie(WorldPackets::Misc::CompleteMovie& /*packet*/)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_COMPLETE_MOVIE");
    uint32 movie = _player->GetMovie();
    if (!movie)
        return;

    _player->SetMovie(0);
    //sScriptMgr->OnMovieComplete(_player, movie);
    if (_player)
        if (InstanceScript* instance = _player->GetInstanceScript())
            instance->OnMovieEnded(_player);
}

void WorldSession::HandleMoveTimeSkippedOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_MOVE_TIME_SKIPPED");

    ObjectGuid guid;
    uint32 time;
    recvData >> time;

    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);

    //TODO!

    /*
        GetPlayer()->ModifyLastMoveTime(-int32(time_skipped));
    */
}

void WorldSession::HandleFeatherFallAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_MOVE_FEATHER_FALL_ACK");

    // no used
    recvData.rfinish();                       // prevent warnings spam
}

void WorldSession::HandleMoveUnRootAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
/*
    uint64 guid;
    recvData >> guid;

    // now can skip not our packet
    if (_player->GetGUID() != guid)
    {
        recvData.rfinish();                   // prevent warnings spam
        return;
    }

    TC_LOG_DEBUG("network", "WORLD: CMSG_FORCE_MOVE_UNROOT_ACK");

    recvData.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recvData, &movementInfo);
    recvData.read_skip<float>();                           // unk2
*/
}

void WorldSession::HandleMoveRootAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
/*
    uint64 guid;
    recvData >> guid;

    // now can skip not our packet
    if (_player->GetGUID() != guid)
    {
        recvData.rfinish();                   // prevent warnings spam
        return;
    }

    TC_LOG_DEBUG("network", "WORLD: CMSG_FORCE_MOVE_ROOT_ACK");

    recvData.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);
*/
}

void WorldSession::HandleMoveGravityAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
}

void WorldSession::HandleSetActionBarToggles(WorldPacket& recvData)
{
    uint8 actionBar;

    recvData >> actionBar;

    if (!GetPlayer())                                        // ignore until not logged (check needed because STATUS_AUTHED)
    {
        if (actionBar != 0)
            TC_LOG_ERROR("network", "WorldSession::HandleSetActionBarToggles in not logged state with value: %u, ignored", uint32(actionBar));
        return;
    }

    GetPlayer()->SetByteValue(PLAYER_FIELD_BYTES, 2, actionBar);
}

void WorldSession::HandlePlayedTime(WorldPacket& recvData)
{
    uint8 unk1;
    recvData >> unk1;                                      // 0 or 1 expected

    WorldPacket data(SMSG_PLAYED_TIME, 4 + 4 + 1);
    data << uint32(_player->GetTotalPlayedTime());
    data << uint32(_player->GetLevelPlayedTime());
    data << uint8(unk1);                                    // 0 - will not show in chat frame
    SendPacket(&data);
}

void WorldSession::HandleInspectOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[7]);

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_INSPECT");

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
    {
        TC_LOG_DEBUG("network", "CMSG_INSPECT: No player found from GUID: " UI64FMTD, (uint64)guid);
        return;
    }

    _player->SendInspectResult(player);
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
    {
        TC_LOG_DEBUG("network", "CMSG_INSPECT_HONOR_STATS: No player found from GUID: " UI64FMTD, (uint64)guid);
        return;
    }

    ObjectGuid playerGuid = player->GetGUID();
    WorldPacket data(SMSG_INSPECT_HONOR_STATS, 8 + 1 + 4 + 4);

    data << uint32(player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS));
    data << uint16(player->GetUInt16Value(PLAYER_FIELD_YESTERDAY_HONORABLE_KILLS, 1));  // yesterday kills
    data << uint16(player->GetUInt16Value(PLAYER_FIELD_YESTERDAY_HONORABLE_KILLS, 0));  // today kills
    data << uint8(0);                                               // rank

    data.WriteBit(playerGuid[2]);
    data.WriteBit(playerGuid[1]);
    data.WriteBit(playerGuid[6]);
    data.WriteBit(playerGuid[4]);
    data.WriteBit(playerGuid[5]);
    data.WriteBit(playerGuid[3]);
    data.WriteBit(playerGuid[7]);
    data.WriteBit(playerGuid[0]);
    data.FlushBits();
    data.WriteByteSeq(playerGuid[1]);
    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(playerGuid[6]);
    data.WriteByteSeq(playerGuid[7]);
    data.WriteByteSeq(playerGuid[2]);
    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(playerGuid[0]);

    SendPacket(&data);
}

void WorldSession::HandleWorldTeleportOpcode(WorldPacket& recvData)
{
    uint32 time;
    uint32 mapid;
    float PositionX;
    float PositionY;
    float PositionZ;
    float Orientation;

    recvData >> time;                                      // time in m.sec.
    recvData >> mapid;
    recvData >> PositionX;
    recvData >> PositionY;
    recvData >> PositionZ;
    recvData >> Orientation;                               // o (3.141593 = 180 degrees)

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_WORLD_TELEPORT");

    if (GetPlayer()->IsInFlight())
    {
        TC_LOG_DEBUG("network", "Player '%s' (GUID: %u) in flight, ignore worldport command.",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    TC_LOG_DEBUG("network", "CMSG_WORLD_TELEPORT: Player = %s, Time = %u, map = %u, x = %f, y = %f, z = %f, o = %f",
        GetPlayer()->GetName().c_str(), time, mapid, PositionX, PositionY, PositionZ, Orientation);

    if (GetSecurity() >= SEC_ADMINISTRATOR)
        GetPlayer()->TeleportTo(mapid, PositionX, PositionY, PositionZ, Orientation);
    else
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
}

void WorldSession::HandleWhoisOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_WHOIS");
    std::string charname;
    recvData >> charname;

    if (GetSecurity() < SEC_ADMINISTRATOR)
    {
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
        return;
    }

    if (charname.empty() || !normalizePlayerName (charname))
    {
        SendNotification(LANG_NEED_CHARACTER_NAME);
        return;
    }

    Player* player = ObjectAccessor::FindPlayerByName(charname);

    if (!player)
    {
        SendNotification(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, charname.c_str());
        return;
    }

    uint32 accid = player->GetSession()->GetAccountId();

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WHOIS);

    stmt->setUInt32(0, accid);

    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
    {
        SendNotification(LANG_ACCOUNT_FOR_PLAYER_NOT_FOUND, charname.c_str());
        return;
    }

    Field* fields = result->Fetch();
    std::string acc = fields[0].GetString();
    if (acc.empty())
        acc = "Unknown";
    std::string email = fields[1].GetString();
    if (email.empty())
        email = "Unknown";
    std::string lastip = fields[2].GetString();
    if (lastip.empty())
        lastip = "Unknown";

    std::string msg = charname + "'s " + "account is " + acc + ", e-mail: " + email + ", last ip: " + lastip;

    WorldPacket data(SMSG_WHOIS, msg.size()+1);
    data.WriteBits(msg.size(), 11);
    data.FlushBits();
    if (msg.size())
        data.WriteString(msg);
    SendPacket(&data);

    TC_LOG_DEBUG("network", "Received whois command from player %s for character %s",
        GetPlayer()->GetName().c_str(), charname.c_str());
}

// not in 5.4.8
// void WorldSession::HandleComplainOpcode(WorldPacket& recvData)
// {
//     TC_LOG_DEBUG("network", "WORLD: CMSG_COMPLAIN");

//     uint8 spam_type;                                        // 0 - mail, 1 - chat
//     uint64 spammer_guid;
//     uint32 unk1 = 0;
//     uint32 unk2 = 0;
//     uint32 unk3 = 0;
//     uint32 unk4 = 0;
//     std::string description = "";
//     recvData >> spam_type;                                 // unk 0x01 const, may be spam type (mail/chat)
//     recvData >> spammer_guid;                              // player guid
//     switch (spam_type)
//     {
//         case 0:
//             recvData >> unk1;                              // const 0
//             recvData >> unk2;                              // probably mail id
//             recvData >> unk3;                              // const 0
//             break;
//         case 1:
//             recvData >> unk1;                              // probably language
//             recvData >> unk2;                              // message type?
//             recvData >> unk3;                              // probably channel id
//             recvData >> unk4;                              // time
//             recvData >> description;                       // spam description string (messagetype, channel name, player name, message)
//             break;
//     }

//     // NOTE: all chat messages from this spammer automatically ignored by spam reporter until logout in case chat spam.
//     // if it's mail spam - ALL mails from this spammer automatically removed by client

//     // Complaint Received message
//     WorldPacket data(SMSG_COMPLAIN_RESULT, 2);
//     data << uint8(0); // value 1 resets CGChat::m_complaintsSystemStatus in client. (unused?)
//     data << uint8(0); // value 0xC generates a "CalendarError" in client.
//     SendPacket(&data);

//     TC_LOG_DEBUG("network", "REPORT SPAM: type %u, guid %u, unk1 %u, unk2 %u, unk3 %u, unk4 %u, message %s", spam_type, GUID_LOPART(spammer_guid), unk1, unk2, unk3, unk4, description.c_str());
// }

void WorldSession::HandleRealmSplitOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_REALM_SPLIT");

    uint32 unk;
    std::string split_date = "01/01/01";
    recvData >> unk;

    WorldPacket data(SMSG_REALM_SPLIT, 4+4+split_date.size()+1);
    data << unk;
    data << uint32(0x00000000);                             // realm split state
    // split states:
    // 0x0 realm normal
    // 0x1 realm split
    // 0x2 realm split pending
    data.WriteBits(split_date.size(), 7);
    data.WriteString(split_date);
    SendPacket(&data);
    //TC_LOG_DEBUG("response sent %u", unk);
}

void WorldSession::HandleFarSightOpcode(WorldPackets::Misc::FarSight& packet)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_FAR_SIGHT");

    if (packet.Enable)
    {
        TC_LOG_DEBUG("network", "Added FarSight %s to player %s", _player->GetGuidValue(PLAYER_FIELD_FARSIGHT_OBJECT).ToString().c_str(), _player->GetGUID().ToString().c_str());
        if (WorldObject* target = _player->GetViewpoint())
            _player->SetSeer(target);
        else
            TC_LOG_ERROR("network", "Player %s %s requests non-existing seer %s", _player->GetName().c_str(), _player->GetGUID().ToString().c_str(), _player->GetGuidValue(PLAYER_FIELD_FARSIGHT_OBJECT).ToString().c_str());
    }
    else
    {
        TC_LOG_DEBUG("network", "Player %s set vision to self", _player->GetGUID().ToString().c_str());
        _player->SetSeer(_player);
    }

    GetPlayer()->UpdateVisibilityForPlayer();
}

void WorldSession::HandleSetTitleOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_SET_TITLE");

    int32 title;
    recvData >> title;

    // -1 at none
    if (title > 0 && title < MAX_TITLE_INDEX)
    {
       if (!GetPlayer()->HasTitle(title))
            return;
    }
    else
        title = 0;

    GetPlayer()->SetUInt32Value(PLAYER_FIELD_PLAYER_TITLE, title);
}

void WorldSession::SendTitleEarned(uint32 TitleIndex)
{
    WorldPacket data(SMSG_TITLE_EARNED, 4);
    data << uint32(TitleIndex);
    SendPacket(&data);
}

void WorldSession::SendTitleLost(uint32 TitleIndex)
{
    WorldPacket data(SMSG_TITLE_LOST, 4);
    data << uint32(TitleIndex);
    SendPacket(&data);
}

void WorldSession::HandleTimeSyncResp(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_TIME_SYNC_RESP");

    uint32 counter, clientTicks;
    recvData >> counter >> clientTicks;

    if (counter != _player->m_timeSyncQueue.front())
        TC_LOG_DEBUG("network", "Wrong time sync counter from player %s (cheater?)", _player->GetName().c_str());

    TC_LOG_DEBUG("network", "Time sync received: counter %u, client ticks %u, time since last sync %u", counter, clientTicks, clientTicks - _player->m_timeSyncClient);

    uint32 ourTicks = clientTicks + (getMSTime() - _player->m_timeSyncServer);

    // diff should be small
    TC_LOG_DEBUG("network", "Our ticks: %u, diff %u, latency %u", ourTicks, ourTicks - clientTicks, GetLatency());

    _player->m_timeSyncClient = clientTicks;
    _player->m_timeSyncQueue.pop();
}

void WorldSession::HandleResetInstancesOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_RESET_INSTANCES");

    if (Group* group = _player->GetGroup())
    {
        if (group->IsLeader(_player->GetGUID()))
            group->ResetInstances(INSTANCE_RESET_ALL, false, _player);
    }
    else
        _player->ResetInstances(INSTANCE_RESET_ALL, false);
}

void WorldSession::HandleSetDungeonDifficultyOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "MSG_SET_DUNGEON_DIFFICULTY");

    uint32 mode;
    recvData >> mode;

    if (mode >= MAX_DUNGEON_DIFFICULTY)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUID().GetCounter(), mode);
        return;
    }

    if (Difficulty(mode) == _player->GetDungeonDifficulty())
        return;

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->IsDungeon())
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player (Name: %s, GUID: %u) tried to reset the instance while player is inside!",
            _player->GetName().c_str(), _player->GetGUID().GetCounter());
        return;
    }

    Group* group = _player->GetGroup();
    if (group)
    {
        if (group->IsLeader(_player->GetGUID()))
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* groupGuy = itr->GetSource();
                if (!groupGuy)
                    continue;

                if (!groupGuy->IsInMap(groupGuy))
                    return;

                if (groupGuy->GetMap()->IsNonRaidDungeon())
                {
                    TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player %d tried to reset the instance while group member (Name: %s, GUID: %u) is inside!",
                        _player->GetGUID().GetCounter(), groupGuy->GetName().c_str(), groupGuy->GetGUID().GetCounter());
                    return;
                }
            }
            // the difficulty is set even if the instances can't be reset
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, _player);
            group->SetDungeonDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false);
        _player->SetDungeonDifficulty(Difficulty(mode));
        _player->SendDungeonDifficulty(mode);
    }
}

void WorldSession::HandleSetRaidDifficultyOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_SET_RAID_DIFFICULTY");

    uint32 mode;
    recvData >> mode;

    if (mode >= MAX_RAID_DIFFICULTY)
    {
        TC_LOG_ERROR("network", "WorldSession::HandleSetRaidDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUID().GetCounter(), mode);
        return;
    }

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->IsDungeon())
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUID().GetCounter());
        return;
    }

    if (Difficulty(mode) == _player->GetRaidDifficulty())
        return;

    Group* group = _player->GetGroup();
    if (group)
    {
        if (group->IsLeader(_player->GetGUID()))
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* groupGuy = itr->GetSource();
                if (!groupGuy)
                    continue;

                if (!groupGuy->IsInMap(groupGuy))
                    return;

                if (groupGuy->GetMap()->IsRaid())
                {
                    TC_LOG_DEBUG("network", "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUID().GetCounter());
                    return;
                }
            }
            // the difficulty is set even if the instances can't be reset
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true, _player);
            group->SetRaidDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true);
        _player->SetRaidDifficulty(Difficulty(mode));
        _player->SendRaidDifficulty(mode);
    }
}

void WorldSession::HandleCancelMountAuraOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_CANCEL_MOUNT_AURA");

    //If player is not mounted, so go out :)
    if (!_player->IsMounted())                              // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_CHAR_NON_MOUNTED);
        return;
    }

    if (_player->IsInFlight())                               // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_YOU_IN_FLIGHT);
        return;
    }

    _player->RemoveAurasByType(SPELL_AURA_MOUNTED); // Calls Dismount()
}

void WorldSession::HandleMoveSetCanFlyAckOpcode(WorldPacket& recvData)
{
    // fly mode on/off
    TC_LOG_DEBUG("network", "WORLD: CMSG_MOVE_SET_CAN_FLY_ACK");

    MovementInfo movementInfo;
    _player->ReadMovementInfo(recvData, &movementInfo);

    _player->m_mover->m_movementInfo.flags = movementInfo.GetMovementFlags();
}

void WorldSession::HandleRequestPetInfoOpcode(WorldPacket& /*recvData */)
{
    if (_player->GetVehicle())
        _player->VehicleSpellInitialize();
    else if (Unit* minion = _player->GetFirstControlled())
    {
        if (minion->IsPet())
            _player->PetSpellInitialize();
        else if (minion->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
            _player->CharmSpellInitialize();
    }
}

void WorldSession::HandleSetTaxiBenchmarkOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SET_TAXI_BENCHMARK_MODE");

    uint8 mode;
    recvData >> mode;

    mode ? _player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK) : _player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK);

    TC_LOG_DEBUG("network", "Client used \"/timetest %d\" command", mode);
}

void WorldSession::HandleQueryInspectAchievements(WorldPacket& recvData)
{
    ObjectGuid guid;
    guid[2] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
    {
        TC_LOG_DEBUG("network", "CMSG_QUERY_INSPECT_ACHIEVEMENTS: Inspected Player " UI64FMTD, (uint64)guid);
        return;
    }

    player->SendRespondInspectAchievements(_player);
}

void WorldSession::HandleGuildAchievementProgressQuery(WorldPacket& recvData)
{
    uint32 achievementId;
    recvData >> achievementId;

    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
        guild->GetAchievementMgr().SendAchievementInfo(_player, achievementId);
}

void WorldSession::HandleWorldStateUITimerUpdate(WorldPacket& /*recvData*/)
{
    // empty opcode
    TC_LOG_DEBUG("network", "WORLD: CMSG_WORLD_STATE_UI_TIMER_UPDATE");

    WorldPackets::Misc::UITime response;
    response.Time = GameTime::GetGameTime();
    SendPacket(response.Write());
}

void WorldSession::HandleReadyForAccountDataTimes(WorldPacket& /*recvData*/)
{
    // empty opcode
    TC_LOG_DEBUG("network", "WORLD: CMSG_READY_FOR_ACCOUNT_DATA_TIMES");

    SendAccountDataTimes(GLOBAL_CACHE_MASK);
}

void WorldSession::SendSetPhaseShift(std::set<uint32> const& phaseIds, std::set<uint32> const& terrainswaps, std::set<uint32> const& worldMapAreas, uint32 flags)
{
    ObjectGuid guid = _player->GetGUID();

    WorldPacket data(SMSG_SET_PHASE_SHIFT, 1 + 8 + 2 * phaseIds.size() + 4 + 2 * worldMapAreas.size() + 2 * terrainswaps.size() + 4);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);

    data << uint32(phaseIds.size()) * 2;        // Phase.dbc ids
    for (std::set<uint32>::const_iterator itr = phaseIds.begin(); itr != phaseIds.end(); ++itr)
        data << uint16(*itr);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[6]);

    data << uint32(0);                          // Inactive terrain swaps
    //for (uint8 i = 0; i < inactiveSwapsCount; ++i)
    //    data << uint16(0);

    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);

    data << uint32(worldMapAreas.size()) * 2;     // WorldMapArea.dbc id (controls map display)
    for (std::set<uint32>::const_iterator itr = worldMapAreas.begin(); itr != worldMapAreas.end(); ++itr)
        data << uint16(*itr);

    data << uint32(terrainswaps.size()) * 2;    // Active terrain swaps
    for (std::set<uint32>::const_iterator itr = terrainswaps.begin(); itr != terrainswaps.end(); ++itr)
        data << uint16(*itr);

    data.WriteByteSeq(guid[5]);

    data << uint32(!flags ? 0x1F : flags);

    SendPacket(&data);
}

// Battlefield and Battleground
void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_AREA_SPIRIT_HEALER_QUERY");

    Battleground* bg = _player->GetBattleground();

    ObjectGuid guid;

    guid[5] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.FlushBits();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->IsSpiritService())                            // it's not spirit service
        return;

    if (bg)
        sBattlegroundMgr->SendAreaSpiritHealerQueryOpcode(_player, bg, guid);

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
        bf->SendAreaSpiritHealerQueryOpcode(_player, guid);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_AREA_SPIRIT_HEALER_QUEUE");

    ObjectGuid guid;
    Battleground* bg = _player->GetBattleground();

    guid[5] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->IsSpiritService())                            // it's not spirit service
        return;

    if (bg)
        bg->AddPlayerToResurrectQueue(guid, _player->GetGUID());

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
        bf->AddPlayerToResurrectQueue(guid, _player->GetGUID());
}

void WorldSession::HandleHearthAndResurrect(WorldPacket& /*recvData*/)
{
    if (_player->IsInFlight())
        return;

    if (/*Battlefield* bf = */sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
    {
        // bf->PlayerAskToLeave(_player); FIXME
        return;
    }

    AreaTableEntry const* atEntry = sAreaTableStore.LookupEntry(_player->GetAreaId());
    if (!atEntry || !(atEntry->Flags & AREA_FLAG_WINTERGRASP_2))
        return;

    _player->BuildPlayerRepop();
    _player->ResurrectPlayer(100);
    _player->TeleportTo(_player->m_homebindMapId, _player->m_homebindX, _player->m_homebindY, _player->m_homebindZ, _player->GetOrientation());
}

void WorldSession::HandleInstanceLockResponse(WorldPacket& recvPacket)
{
    bool accept = recvPacket.ReadBit();

    if (!_player->HasPendingBind())
    {
        TC_LOG_INFO("network", "InstanceLockResponse: Player %s (guid %u) tried to bind himself/teleport to graveyard without a pending bind!",
            _player->GetName().c_str(), _player->GetGUID().GetCounter());
        return;
    }

    if (accept)
        _player->BindToInstance();
    else
        _player->RepopAtGraveyard();

    _player->SetPendingBind(0, 0);
}

void WorldSession::HandleRequestHotfix(WorldPacket& recvPacket)
{
    uint32 type, count;
    recvPacket >> type;

    DB2StorageBase const* store = GetDB2Storage(type);
    if (!store)
    {
        TC_LOG_DEBUG("network", "CMSG_REQUEST_HOTFIX: Received unknown hotfix type: %u", type);
        recvPacket.rfinish();
        return;
    }

    count = recvPacket.ReadBits(21);

    ObjectGuid* guids = new ObjectGuid[count];
    for (uint32 i = 0; i < count; ++i)
    {
        guids[i][6] = recvPacket.ReadBit();
        guids[i][3] = recvPacket.ReadBit();
        guids[i][0] = recvPacket.ReadBit();
        guids[i][1] = recvPacket.ReadBit();
        guids[i][4] = recvPacket.ReadBit();
        guids[i][5] = recvPacket.ReadBit();
        guids[i][7] = recvPacket.ReadBit();
        guids[i][2] = recvPacket.ReadBit();
    }

    uint32 entry;
    std::unordered_map<uint32, uint32> requestedEntries;
    for (uint32 i = 0; i < count; ++i)
    {
        recvPacket.ReadByteSeq(guids[i][1]);
        recvPacket >> entry;
        requestedEntries[i] = entry;
        recvPacket.ReadByteSeq(guids[i][0]);
        recvPacket.ReadByteSeq(guids[i][5]);
        recvPacket.ReadByteSeq(guids[i][6]);
        recvPacket.ReadByteSeq(guids[i][4]);
        recvPacket.ReadByteSeq(guids[i][7]);
        recvPacket.ReadByteSeq(guids[i][2]);
        recvPacket.ReadByteSeq(guids[i][3]);
    }

    for (uint32 i = 0; i < count; ++i)
    {
        ByteBuffer db2Buffer;
        switch (type) // overwrite db2 data from db
        {
            case DB2_REPLY_BROADCAST_TEXT:
                SendBroadcastTextDb2Reply(requestedEntries[i], db2Buffer);
                break;
            case DB2_REPLY_ITEM:
                SendItemDb2Reply(requestedEntries[i], db2Buffer);
                break;
            case DB2_REPLY_SPARSE:
                SendItemSparseDb2Reply(requestedEntries[i], db2Buffer);
                break;
            default:
                break;
        }

        if (store->HasRecord(requestedEntries[i]) && !db2Buffer.size()) // load requested data from db2 (only for missing in db)
            store->WriteRecord(requestedEntries[i],(uint32) GetSessionDbcLocale(), db2Buffer);

        if (!db2Buffer.size())
        {
            TC_LOG_ERROR("network", "SMSG_DB_REPLY: Cant send hotfix entry: %u type: %u, because has no record.", requestedEntries[i], type);
            continue;
        }

        WorldPacket data(SMSG_DB_REPLY);
        data << uint32(requestedEntries[i]);
        data << uint32(time(NULL));
        data << uint32(type);
        data << uint32(db2Buffer.size());
        data.append(db2Buffer);
        SendPacket(&data);

        TC_LOG_DEBUG("network", "SMSG_DB_REPLY: Sent hotfix entry: %u type: %u", requestedEntries[i], type);
    }

    delete [] guids;
}

void WorldSession::SendBroadcastTextDb2Reply(uint32 entry, ByteBuffer& buffer)
{
    /*
     *  This is a hack fix! Still uses Gossip Id's instead of Broadcast Id's.
     *  Major database changed required at some point.
     */
    LocaleConstant locale = GetSessionDbLocaleIndex();
    std::string Text_0;
    std::string Text_1;
    BroadcastText const* bct = sObjectMgr->GetBroadcastText(entry);
    if (bct)
    {
        Text_0 = bct->GetText(locale, GENDER_MALE, true);
        Text_1 = bct->GetText(locale, GENDER_FEMALE, true);
    }
    else
    {
        return; // return without buffer for trying load data from db2 stores
    }
    if (locale >= 0)
    {
        if (NpcTextLocale const* localeData = sObjectMgr->GetNpcTextLocale(entry))
        {
            ObjectMgr::GetLocaleString(localeData->Text_0[0], locale, Text_0);
            ObjectMgr::GetLocaleString(localeData->Text_1[0], locale, Text_1);
        }
    }
    buffer << uint32(entry);
    buffer << uint32(!bct ? 0 : bct->LanguageID);
    buffer << uint16(Text_0.length());
    if (Text_0.length())
        buffer << std::string(Text_0);    
    buffer << uint16(Text_1.length());
    if (Text_1.length())
        buffer << std::string(Text_1); 
    buffer << uint32(!bct ? 0 : bct->EmoteId1);  
    buffer << uint32(!bct ? 0 : bct->EmoteId2);
    buffer << uint32(!bct ? 0 : bct->EmoteId3);
    buffer << uint32(!bct ? 0 : bct->EmoteDelay1);
    buffer << uint32(!bct ? 0 : bct->EmoteDelay2);
    buffer << uint32(!bct ? 0 : bct->EmoteDelay3);
    buffer << uint32(!bct ? 0 : bct->SoundEntriesID); // Sound Id
    buffer << uint32(0); // UnkMoP1
    buffer << uint32(0); // UnkMoP2    
}

void WorldSession::HandleUpdateMissileTrajectory(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_UPDATE_MISSILE_TRAJECTORY");

    ObjectGuid guid;
    uint32 spellId;
    float elevation, speed;
    float curX, curY, curZ;
    float targetX, targetY, targetZ;
    uint8 moveStop;

    recvPacket >> guid >> spellId >> elevation >> speed;
    recvPacket >> curX >> curY >> curZ;
    recvPacket >> targetX >> targetY >> targetZ;
    recvPacket >> moveStop;

    Unit* caster = ObjectAccessor::GetUnit(*_player, guid);
    Spell* spell = caster ? caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) : NULL;
    if (!spell || spell->m_spellInfo->Id != spellId || !spell->m_targets.HasDst() || !spell->m_targets.HasSrc())
    {
        recvPacket.rfinish();
        return;
    }

    Position pos = *spell->m_targets.GetSrcPos();
    pos.Relocate(curX, curY, curZ);
    spell->m_targets.ModSrc(pos);

    pos = *spell->m_targets.GetDstPos();
    pos.Relocate(targetX, targetY, targetZ);
    spell->m_targets.ModDst(pos);

    spell->m_targets.SetElevation(elevation);
    spell->m_targets.SetSpeed(speed);

    if (moveStop)
    {
        uint32 opcode;
        recvPacket >> opcode;
        recvPacket.SetOpcode(MSG_MOVE_STOP); // always set to MSG_MOVE_STOP in client SetOpcode
        HandleMovementOpcodes(recvPacket);
    }
}

void WorldSession::HandleViolenceLevel(WorldPacket& recvPacket)
{
    uint8 violenceLevel;
    recvPacket >> violenceLevel;

    // do something?
}

void WorldSession::HandleObjectUpdateFailedOpcode(WorldPacket& recvPacket)
{
    ObjectGuid guid;

    guid[3] = recvPacket.ReadBit();
    guid[5] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();
    guid[0] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();
    guid[7] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[0]);
    recvPacket.ReadByteSeq(guid[6]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[7]);
    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[3]);
    recvPacket.ReadByteSeq(guid[4]);

    WorldObject* obj = ObjectAccessor::GetWorldObject(*GetPlayer(), guid);
        TC_LOG_ERROR("network", "Object update failed for object %s (%s) for player %s (%u)", guid.ToString().c_str(), obj ? obj->GetName().c_str() : "object-not-found", GetPlayerName().c_str(), GetGuidLow());

    // If create object failed for current player then client will be stuck on loading screen
    if (_player->GetGUID() == guid)
    {
        LogoutPlayer(true);
        return;
    }

    // Pretend we've never seen this object
    _player->m_clientGUIDs.erase(guid);
}

void WorldSession::HandleSaveCUFProfiles(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SAVE_CUF_PROFILES");

    uint8 count = (uint8)recvPacket.ReadBits(19);

    if (count > MAX_CUF_PROFILES)
    {
        TC_LOG_ERROR("entities.player", "HandleSaveCUFProfiles - %s tried to save more than %i CUF profiles. Hacking attempt?", GetPlayerName().c_str(), MAX_CUF_PROFILES);
        recvPacket.rfinish();
        return;
    }

    CUFProfile* profiles[MAX_CUF_PROFILES];
    uint8 strlens[MAX_CUF_PROFILES];

    for (uint8 i = 0; i < count; ++i)
    {
        profiles[i] = new CUFProfile;
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_SPEC_2,             recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_MAIN_TANK_AND_ASSIST,     recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_POWER_BAR,                recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_10_PLAYERS,         recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_3_PLAYERS,          recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_UNK_156,                          recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_40_PLAYERS,         recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_2_PLAYERS,          recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_KEEP_GROUPS_TOGETHER,             recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_USE_CLASS_COLORS,                 recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_25_PLAYERS,         recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_UNK_145,                          recvPacket.ReadBit());
        strlens[i] = (uint8)recvPacket.ReadBits(7);
        profiles[i]->BoolOptions.set(CUF_DISPLAY_PETS,                     recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_PVP,                recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_ONLY_DISPELLABLE_DEBUFFS, recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_NON_BOSS_DEBUFFS,         recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_15_PLAYERS,         recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_UNK_157,                          recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_BORDER,                   recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_HORIZONTAL_GROUPS,        recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_SPEC_1,             recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_5_PLAYERS,          recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_AUTO_ACTIVATE_PVE,                recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_HEAL_PREDICTION,          recvPacket.ReadBit());
        profiles[i]->BoolOptions.set(CUF_DISPLAY_AGGRO_HIGHLIGHT,          recvPacket.ReadBit());
    }

    for (uint8 i = 0; i < count; ++i)
    {
        recvPacket >> profiles[i]->FrameHeight;
        recvPacket >> profiles[i]->Unk146;
        recvPacket >> profiles[i]->HealthText;
        recvPacket >> profiles[i]->FrameWidth;
        recvPacket >> profiles[i]->Unk148;
        recvPacket >> profiles[i]->SortBy;
        recvPacket >> profiles[i]->Unk150;
        profiles[i]->ProfileName = recvPacket.ReadString(strlens[i]);
        recvPacket >> profiles[i]->Unk147;
        recvPacket >> profiles[i]->Unk152;
        recvPacket >> profiles[i]->Unk154;

        GetPlayer()->SaveCUFProfile(i, profiles[i]);
    }

    for (uint8 i = count; i < MAX_CUF_PROFILES; ++i)
        GetPlayer()->SaveCUFProfile(i, NULL);
}

void WorldSession::SendLoadCUFProfiles()
{
    Player* player = GetPlayer();

    uint8 count = player->GetCUFProfilesCount();

    ByteBuffer byteBuffer;
    WorldPacket data(SMSG_LOAD_CUF_PROFILES, 5 * count + 25 * count);

    data.WriteBits(count, 19);
    for (uint8 i = 0; i < MAX_CUF_PROFILES; ++i)
    {
        CUFProfile* profile = player->GetCUFProfile(i);
        if (!profile)
            continue;

        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_SPEC_1]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_3_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_UNK_157]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_10_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_40_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_BORDER]);
        data.WriteBit(profile->BoolOptions[CUF_USE_CLASS_COLORS]);
        data.WriteBit(profile->BoolOptions[CUF_KEEP_GROUPS_TOGETHER]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_POWER_BAR]);
        data.WriteBits(profile->ProfileName.size(), 7);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_PETS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_AGGRO_HIGHLIGHT]);
        data.WriteBit(profile->BoolOptions[CUF_UNK_145]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_PVP]);
        data.WriteBit(profile->BoolOptions[CUF_UNK_156]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_MAIN_TANK_AND_ASSIST]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_NON_BOSS_DEBUFFS]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_HORIZONTAL_GROUPS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_SPEC_2]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_HEAL_PREDICTION]);
        data.WriteBit(profile->BoolOptions[CUF_DISPLAY_ONLY_DISPELLABLE_DEBUFFS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_25_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_PVE]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_5_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_15_PLAYERS]);
        data.WriteBit(profile->BoolOptions[CUF_AUTO_ACTIVATE_2_PLAYERS]);

        byteBuffer << uint16(profile->Unk152);
        byteBuffer << uint16(profile->Unk154);
        byteBuffer << uint8(profile->HealthText);
        byteBuffer.WriteString(profile->ProfileName);
        byteBuffer << uint8(profile->Unk147);
        byteBuffer << uint8(profile->Unk146);
        byteBuffer << uint16(profile->FrameHeight);
        byteBuffer << uint8(profile->Unk148);
        byteBuffer << uint8(profile->SortBy);
        byteBuffer << uint16(profile->FrameWidth);
        byteBuffer << uint16(profile->Unk150);
    }

    data.FlushBits();
    data.append(byteBuffer);
    SendPacket(&data);
}

void WorldSession::SendStreamingMovie()
{
    WorldPackets::Misc::StreamingMovies packet;

    // To-do: implement
    SendPacket(packet.Write());
}

#define JOIN_THE_ALLIANCE 1
#define JOIN_THE_HORDE    0

void WorldSession::HandleSelectFactionOpcode(WorldPacket& recvPacket)
{
    uint32 choice = recvPacket.read<uint32>();

    if (_player->GetRace() != RACE_PANDAREN_NEUTRAL)
        return;

    if (_player->GetQuestStatus(31450) == QUEST_STATUS_INCOMPLETE)
        _player->KilledMonsterCredit(64594);

    if (choice == JOIN_THE_HORDE)
    {
        _player->SetByteValue(UNIT_FIELD_BYTES_0, 0, RACE_PANDAREN_HORDE);
        _player->setFactionForRace(RACE_PANDAREN_HORDE);
        _player->SaveToDB();
        WorldLocation location(1, 1357.62f, -4373.55f, 26.13f, 0.13f);
        _player->TeleportTo(location);
        _player->SetHomebind(location, 363);
        _player->LearnSpell(669, false); // Language Orcish
        _player->LearnSpell(108127, false); // Language Pandaren
    }
    else if (choice == JOIN_THE_ALLIANCE)
    {
        _player->SetByteValue(UNIT_FIELD_BYTES_0, 0, RACE_PANDAREN_ALLIANCE);
        _player->setFactionForRace(RACE_PANDAREN_ALLIANCE);
        _player->SaveToDB();
        WorldLocation location(0, -8960.02f, 516.10f, 96.36f, 0.67f);
        _player->TeleportTo(location);
        _player->SetHomebind(location, 9);
        _player->LearnSpell(668, false); // Language Common
        _player->LearnSpell(108127, false); // Language Pandaren
    }

    _player->SendMovieStart(116);
}

void WorldSession::HandleDiscardedTimeSyncAcks(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_DISCARDED_TIME_SYNC_ACKS");

    bool hasInfo = !recvData.ReadBit();

    if (hasInfo)
        recvData.read_skip<uint32>();
}

void WorldSession::HandleLogStreamingError(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_LOG_STREAMING_ERROR");

    uint32 lenght = recvData.ReadBits(9);
    std::string missingData = recvData.ReadString(lenght);
    
    TC_LOG_DEBUG("network", "CMSG_LOG_STREAMING_ERROR: cant't download data: %s", missingData.c_str());
}

void WorldSession::HandleInspectRatedBGStatsOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_INSPECT_RATED_BG_STATS");

    ObjectGuid guid;

    recvData.read_skip<uint32>(); // realmId
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player)
    {
        TC_LOG_DEBUG("network", "CMSG_REQUEST_INSPECT_RATED_BG_STATS: No player found from GUID: " UI64FMTD, uint64(guid));
        return;
    }

    uint8 numSlots = 0;
    ObjectGuid playerGuid = player->GetGUID();
    WorldPacket data(SMSG_INSPECT_RATED_BG_STATS);

    data.WriteBit(playerGuid[4]);
    data.WriteBit(playerGuid[2]);
    data.WriteBit(playerGuid[3]);
    data.WriteBit(playerGuid[6]);
    data.WriteBit(playerGuid[0]);
    data.WriteBit(playerGuid[5]);
    data.WriteBit(playerGuid[7]);
    data.WriteBit(playerGuid[1]);

    size_t pos = data.bitwpos();
    data.WriteBits(numSlots, 3);

    data.FlushBits();

    for (uint8 i = 0; i < PVP_SLOT_MAX; i++)
    {
        auto arenaInfo = RatedPvpMgr::Instance()->GetInfo(RatedPvpSlot(i), player->GetGUID());
        if (!arenaInfo)
            continue;

        data << uint32(arenaInfo->SeasonWins);
        data << uint32(arenaInfo->Rating);
        data << uint32(arenaInfo->SeasonGames);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint8(i);

        numSlots++;
    }
    data.WriteByteSeq(playerGuid[1]);
    data.WriteByteSeq(playerGuid[7]);
    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(playerGuid[2]);
    data.WriteByteSeq(playerGuid[0]);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(playerGuid[6]);
    data.WriteByteSeq(playerGuid[4]);

    data.PutBits(pos, numSlots, 3);

    SendPacket(&data);
}

void WorldSession::HandleShowTradeSkill(WorldPacket& recvData)
{
    uint32 spellId, skillId;
    recvData >> spellId >> skillId;
    ObjectGuid guid;
    guid[0] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[6]);

    Player* player = ObjectAccessor::FindConnectedPlayer(guid);
    if (!player)
        return;

    SkillLineEntry const* skill = sSkillLineStore.LookupEntry(skillId);
    if (!skill || !skill->canLink)
        return;

    uint32 val = player->GetSkillValue(skillId);
    if (!val)
        return;

    bool match = false;
    auto bounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellId);
    for (auto it = bounds.first; it != bounds.second; ++it)
        if (it->second->skillId == skillId)
            match = true;

    if (!match)
        return;

    WorldPacket data{ SMSG_SHOW_TRADE_SKILL_RESPONSE };
    data << uint32(spellId);
    data.WriteBits(1, 22);
    data.WriteBits(1, 22);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[2]);
    data.WriteBits(1, 22);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[3]);
    auto pos = data.bitwpos();
    data.WriteBits(1, 22);
    data.WriteBit(guid[7]);

    data << uint32(val);

    data.WriteByteSeq(guid[3]);

    data << uint32(skillId);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);

    data << uint32(player->GetMaxSkillValue(skillId));

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);

    uint32 count = 0;
    for (auto&& it : player->GetSpellMap())
        if (auto spellInfo = sSpellMgr->GetSpellInfo(it.first))
            if (spellInfo->Attributes & SPELL_ATTR0_TRADESPELL && spellInfo->IsAbilityOfSkillType(skillId))
                ++count, data << uint32(it.first);

    data.WriteByteSeq(guid[2]);

    data.PutBits(pos, count, 22);

    SendPacket(&data);
}
