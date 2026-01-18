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
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "QueryPackets.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "UpdateMask.h"
#include "NPCHandler.h"
#include "Pet.h"
#include "MapManager.h"
#include "Config.h"
#include "Group.h"
#include "Realm.h"

void WorldSession::SendNameQueryOpcode(ObjectGuid guid)
{
    ObjectGuid guid2 = ObjectGuid::Empty;
    ObjectGuid guid3 = guid;

    CharacterNameData const* nameData = sWorld->GetCharacterNameData(guid);

    WorldPacket data(SMSG_NAME_QUERY_RESPONSE, 500);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[1]);

    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);

    data << uint8(!nameData);

    if (nameData)
    {
        data << uint32(realm.Id.Realm); // realmIdSecond
        data << uint32(1); // AccID
        data << uint8(nameData->m_class);
        data << uint8(nameData->m_race);
        data << uint8(nameData->m_level);
        data << uint8(nameData->m_gender);
    }

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);

    if (!nameData)
    {
        SendPacket(&data);
        return;
    }

    data.WriteBit(guid2[2]);
    data.WriteBit(guid2[7]);
    data.WriteBit(guid3[7]);
    data.WriteBit(guid3[2]);
    data.WriteBit(guid3[0]);
    data.WriteBit(0); //isDeleted ? Wod ?
    data.WriteBit(guid2[4]);
    data.WriteBit(guid3[5]);
    data.WriteBit(guid2[1]);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid2[0]);

    DeclinedName const* names = nameData->m_declinedName;
    for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
        data.WriteBits(names ? names->name[i].size() : 0, 7);

    data.WriteBit(guid3[6]);
    data.WriteBit(guid3[3]);
    data.WriteBit(guid2[5]);
    data.WriteBit(guid3[1]);
    data.WriteBit(guid3[4]);
    data.WriteBits(nameData->m_name.size(), 6);
    data.WriteBit(guid2[6]);

    data.FlushBits();

    data.WriteByteSeq(guid3[6]);
    data.WriteByteSeq(guid3[0]);
    data.WriteString(nameData->m_name);
    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid2[2]);
    data.WriteByteSeq(guid3[3]);
    data.WriteByteSeq(guid2[4]);
    data.WriteByteSeq(guid2[3]);
    data.WriteByteSeq(guid3[4]);
    data.WriteByteSeq(guid3[2]);
    data.WriteByteSeq(guid2[7]);

    if (names)
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data.WriteString(names->name[i]);

    data.WriteByteSeq(guid2[6]);
    data.WriteByteSeq(guid3[7]);
    data.WriteByteSeq(guid3[1]);
    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid3[5]);
    data.WriteByteSeq(guid2[0]);

    SendPacket(&data);
}

void WorldSession::HandleNameQueryOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    uint8 bit14, bit1C;
    uint32 unk, unk1;

    guid[4] = recvData.ReadBit();
    bit14 = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    bit1C = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[4]);

    // virtual and native realm Addresses

    if (bit14)
        recvData >> unk;

    if (bit1C)
        recvData >> unk1;

    // This is disable by default to prevent lots of console spam
    // TC_LOG_INFO("network", "HandleNameQueryOpcode %u", guid);

    SendNameQueryOpcode(guid);
}

void WorldSession::SendRealmNameQueryOpcode(uint32 realmId)
{
    RealmNameMap::const_iterator iter = realmNameStore.find(realmId);

    bool found = iter != realmNameStore.end();
    std::string realmName = found ? iter->second : "";

    WorldPacket data(SMSG_REALM_NAME_QUERY_RESPONSE);
    data << uint8(!found);
    data << uint32(realmId);

    if (found)
    {
        data.WriteBits(realmName.length(), 8);
        data.WriteBit(realmId == realm.Id.Realm);
        data.WriteBits(realmName.length(), 8);
        data.FlushBits();

        data.WriteString(realmName);
        data.WriteString(realmName);
    }

    SendPacket(&data);
}

void WorldSession::HandleRealmNameQueryOpcode(WorldPacket& recvPacket)
{
    uint32 realmId;
    recvPacket >> realmId;
    SendRealmNameQueryOpcode(realmId);
}

void WorldSession::HandleQueryTimeOpcode(WorldPacket & /*recvData*/)
{
    SendQueryTimeResponse();
}

void WorldSession::SendQueryTimeResponse()
{
    WorldPacket data(SMSG_QUERY_TIME_RESPONSE, 4+4);
    data << uint32(time(NULL));
    data << uint32(sWorld->GetNextDailyQuestsResetTime() - time(NULL));
    SendPacket(&data);
}

void WorldSession::SendServerWorldInfo()
{
    Map* map = GetPlayer()->GetMap();
    bool IsInInstance = map->IsRaidOrHeroicDungeon();                                               // Check being in raid / heroic dungeon map.
    uint32 InstanceGroupSize = IsInInstance ? map->ToInstanceMap()->GetPlayersCountExceptGMs() : 0; // Check if we need to send the instance group size - for Flex Raids.
    uint32 difficultyNumberToDisplay = 0;                                                           // Number to display in minimap text.

    switch (map->GetDifficulty())
    {
        case DUNGEON_DIFFICULTY_HEROIC:
            difficultyNumberToDisplay = 5;
            break;
        case RAID_DIFFICULTY_10MAN_NORMAL:
        case RAID_DIFFICULTY_10MAN_HEROIC:
            difficultyNumberToDisplay = 10;
            break;
        case RAID_DIFFICULTY_25MAN_NORMAL:
        case RAID_DIFFICULTY_25MAN_HEROIC:
        case RAID_DIFFICULTY_25MAN_LFR:
            difficultyNumberToDisplay = 25;
            break;
        case RAID_DIFFICULTY_40MAN:
            difficultyNumberToDisplay = 40;
            break;
        case SCENARIO_DIFFICULTY_HEROIC:
            difficultyNumberToDisplay = 3;
            break;
        case RAID_DIFFICULTY_1025MAN_FLEX:
            if (InstanceGroupSize < 10)
                InstanceGroupSize = 10;
            difficultyNumberToDisplay = InstanceGroupSize;
            break;
        case REGULAR_DIFFICULTY:
        case DUNGEON_DIFFICULTY_NORMAL:
        case DUNGEON_DIFFICULTY_CHALLENGE:
        case SCENARIO_DIFFICULTY_NORMAL:
        default:
            break;
    }

    WorldPacket data(SMSG_WORLD_SERVER_INFO);

    // Bitfields have wrong order
    data.WriteBit(IsInInstance);                                    // In Instance
    data.WriteBit(0);                                               // HasRestrictedLevel
    data.WriteBit(0);                                               // HasRestrictedMoney
    data.WriteBit(0);                                               // HasGroupSize

    //if (unk1)
    //    data << uint32(0);

    data << uint8(0);                                               // IsOnTournamentRealm
    data << uint32(sWorld->GetNextWeeklyQuestsResetTime() - WEEK);  // LastWeeklyReset (not instance reset)
    data << uint32(map->GetDifficulty());

    //if (unk2)
    //    data << uint32(0);
    if (IsInInstance)
        data << uint32(difficultyNumberToDisplay);
    //if (unk3)
    //    data << uint32(0);

    SendPacket(&data);
}

/// Only _static_ data is sent in this packet !!!
void WorldSession::HandleCreatureQueryOpcode(WorldPacket& recvData)
{
    uint32 entry;
    recvData >> entry;

    WorldPacket data(SMSG_CREATURE_QUERY_RESPONSE, 500);

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(entry);

    data << uint32(entry);
    data.WriteBit(creatureInfo != 0); // Has data

    if (creatureInfo)
    {
        std::string Name, FemaleName, Title;
        Name = creatureInfo->Name;
        FemaleName = creatureInfo->FemaleName;
        Title = creatureInfo->SubName;

        uint8 qItemsSize = 0;

        for (int i = 0; i < MAX_CREATURE_QUEST_ITEMS; i++)
            if (creatureInfo->questItems[i] != 0)
                qItemsSize++;

        LocaleConstant locale = GetSessionDbLocaleIndex();

        if (locale >= 0)
        {
            if (CreatureLocale const* creatureLocale = sObjectMgr->GetCreatureLocale(entry))
            {
                ObjectMgr::GetLocaleString(creatureLocale->Name, locale, Name);
                ObjectMgr::GetLocaleString(creatureLocale->FemaleName, locale, FemaleName);
                ObjectMgr::GetLocaleString(creatureLocale->Title, locale, Title);
            }
        }

        TC_LOG_DEBUG("network", "WORLD: CMSG_CREATURE_QUERY '%s' - Entry: %u.", creatureInfo->Name.c_str(), entry);

        data.WriteBits(Title.length() ? Title.length() + 1 : 0, 11);
        data.WriteBits(qItemsSize, 22);                       // Quest items
        data.WriteBits(0, 11);

        for (int i = 0; i < 4; i++)
        {            
            data.WriteBits(i == 0 ? Name.length() + 1 : 0, 11);
            data.WriteBits(0, 11);
        }

        data.WriteBit(creatureInfo->RacialLeader);
        data.WriteBits(creatureInfo->IconName.length() + 1, 6);
        data.FlushBits();

        data << uint32(creatureInfo->KillCredit[0]);                  // New in 3.1, kill credit

        if (CreatureModel const* model = creatureInfo->GetModelByIdx(3))
            data << uint32(model->CreatureDisplayID);                       // Modelid4
        else
            data << uint32(0);
        if (CreatureModel const* model = creatureInfo->GetModelByIdx(1))
            data << uint32(model->CreatureDisplayID);                       // Modelid2
        else
            data << uint32(0);
        data << uint32(creatureInfo->expansion);                      // Expansion Required
        data << uint32(creatureInfo->type);                           // CreatureType.dbc
        data << float(creatureInfo->ModHealth);                       // Hp modifier
        data << uint32(creatureInfo->type_flags);                     // Flags
        data << uint32(creatureInfo->type_flags2);                    // Flags2
        data << uint32(creatureInfo->rank);                           // Creature Rank (elite, boss, etc)
        data << uint32(creatureInfo->movementId);                     // CreatureMovementInfo.dbc
        data << Name;

        if (Title != "")
            data << Title;                                            // Subname

        if (CreatureModel const* model = creatureInfo->GetModelByIdx(0))
            data << uint32(model->CreatureDisplayID);                       // Modelid1
        else
            data << uint32(0);
        if (CreatureModel const* model = creatureInfo->GetModelByIdx(2))
            data << uint32(model->CreatureDisplayID);                       // Modelid3
        else
            data << uint32(0);

        if (creatureInfo->IconName != "")
            data << creatureInfo->IconName;                           // "Directions" for guard, string for Icons 2.3.0

        for (uint32 i = 0; i < MAX_CREATURE_QUEST_ITEMS; ++i)
            if (creatureInfo->questItems[i] != 0)
                data << uint32(creatureInfo->questItems[i]);          // ItemId[6], quest drop

        data << uint32(creatureInfo->KillCredit[1]);                  // New in 3.1, kill credit
        data << float(creatureInfo->ModMana);                         // Mana modifier
        data << uint32(creatureInfo->family);                         // CreatureFamily.dbc

        TC_LOG_DEBUG("network", "WORLD: Sent SMSG_CREATURE_QUERY_RESPONSE");
    }
    else
    {
        data.FlushBits();
        TC_LOG_DEBUG("network", "WORLD: CMSG_CREATURE_QUERY - NO CREATURE INFO! (ENTRY: %u)", entry);
    }

    SendPacket(&data);
}

/// Only _static_ data is sent in this packet !!!
void WorldSession::HandleGameObjectQueryOpcode(WorldPackets::Query::QueryGameObject& packet)
{
    WorldPackets::Query::QueryGameObjectResponse response;

    response.GameObjectID = packet.GameObjectID;

    if (GameObjectTemplate const* gameObjectInfo = sObjectMgr->GetGameObjectTemplate(packet.GameObjectID))
    {
        TC_LOG_DEBUG("network", "WORLD: CMSG_GAMEOBJECT_QUERY '%s' - Entry: %u. ", gameObjectInfo->name.c_str(), gameObjectInfo->entry);

        response.Allow = true;
        WorldPackets::Query::GameObjectStats& stats = response.Stats;

        stats.Type = gameObjectInfo->type;
        stats.DisplayID = gameObjectInfo->displayId;

        stats.Name[0] = gameObjectInfo->name;
        stats.IconName = gameObjectInfo->IconName;
        stats.CastBarCaption = gameObjectInfo->castBarCaption;
        stats.UnkString = gameObjectInfo->unk1;

        LocaleConstant localeConstant = GetSessionDbLocaleIndex();
        if (localeConstant >= LOCALE_enUS)
            if (GameObjectLocale const* gameObjectLocale = sObjectMgr->GetGameObjectLocale(packet.GameObjectID))
            {
                ObjectMgr::GetLocaleString(gameObjectLocale->Name, localeConstant, stats.Name[0]);
                ObjectMgr::GetLocaleString(gameObjectLocale->CastBarCaption, localeConstant, stats.CastBarCaption);
            }

        stats.Size = gameObjectInfo->size;

        for (uint32 item : gameObjectInfo->questItems)
            stats.QuestItems.push_back(item);

        memcpy(stats.Data.data(), gameObjectInfo->raw.data, MAX_GAMEOBJECT_DATA * sizeof(uint32));
        stats.UnkInt32 = gameObjectInfo->unkInt32;

        TC_LOG_DEBUG("network", "WORLD: Sent SMSG_GAMEOBJECT_QUERY_RESPONSE");
    }
    else
    {
        TC_LOG_DEBUG("network", "WORLD: CMSG_GAMEOBJECT_QUERY - Missing gameobject info for (%s, ENTRY: %u)",
                     packet.Guid.ToString().c_str(), packet.GameObjectID);
        TC_LOG_DEBUG("network", "WORLD: Sent SMSG_GAMEOBJECT_QUERY_RESPONSE");
    }

    SendPacket(response.Write());
}

void WorldSession::HandleCorpseQueryOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_CORPSE_QUERY");

    if (!_player->HasCorpse())
    {
        WorldPacket data(SMSG_CORPSE_QUERY, 1);
        data.WriteBits(0, 9); // Not found + guid stream
        for (int i = 0; i < 5; i++)
            data << uint32(0);
        SendPacket(&data);
        return;
    }

    WorldLocation corpseLocation = _player->GetCorpseLocation();
    uint32 corpseMapId = corpseLocation.GetMapId();
    uint32 mapId = corpseLocation.GetMapId();
    float x = corpseLocation.GetPositionX();
    float y = corpseLocation.GetPositionY();
    float z = corpseLocation.GetPositionZ();

    // if corpse at different map
    if (mapId != _player->GetMapId())
    {
        // search entrance map for proper show entrance
        if (MapEntry const* corpseMapEntry = sMapStore.LookupEntry(mapId))
        {
            if (corpseMapEntry->IsDungeon() && corpseMapEntry->entrance_map >= 0)
            {
                // if corpse map have entrance
                if (Map const* entranceMap = sMapMgr->CreateBaseMap(corpseMapEntry->entrance_map))
                {
                    mapId = corpseMapEntry->entrance_map;
                    x = corpseMapEntry->entrance_x;
                    y = corpseMapEntry->entrance_y;
                    z = entranceMap->GetHeight(GetPlayer()->GetPhaseMask(), x, y, MAX_HEIGHT);
                }
            }
        }
    }

    // this might be transport guid?
    ObjectGuid corpseGuid = ObjectGuid::Empty; // need correct condition, guid shouldn't always be sent to player (corpse->GetGUID())

    WorldPacket data(SMSG_CORPSE_QUERY, 9 + 1 + (4 * 5));
    data.WriteBit(corpseGuid[0]);
    data.WriteBit(corpseGuid[3]);
    data.WriteBit(corpseGuid[2]);
    data.WriteBit(1); // Corpse Found
    data.WriteBit(corpseGuid[5]);
    data.WriteBit(corpseGuid[4]);
    data.WriteBit(corpseGuid[1]);
    data.WriteBit(corpseGuid[7]);
    data.WriteBit(corpseGuid[6]);

    data.WriteByteSeq(corpseGuid[5]);
    data << float(z);
    data.WriteByteSeq(corpseGuid[1]);
    data << uint32(mapId);
    data.WriteByteSeq(corpseGuid[6]);
    data.WriteByteSeq(corpseGuid[4]);
    data << float(x);
    data.WriteByteSeq(corpseGuid[3]);
    data.WriteByteSeq(corpseGuid[7]);
    data.WriteByteSeq(corpseGuid[2]);
    data.WriteByteSeq(corpseGuid[0]);
    data << int32(corpseMapId);
    data << float(y);
    SendPacket(&data);
}

// hack fix
void WorldSession::HandleNpcTextQueryOpcode(WorldPacket& recvData)
{
    uint32 textID;
    ObjectGuid guid;

    recvData >> textID;

    TC_LOG_DEBUG("network", "WORLD: CMSG_NPC_TEXT_QUERY ID '%u'", textID);

    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[6]);

    GossipText const* pGossip = sObjectMgr->GetGossipText(textID);

    WorldPacket data(SMSG_NPC_TEXT_UPDATE, 1 + 4 + 64);
    data << textID;
    data << uint32(64);                                 // size (8 * 4) * 2

    // hackfix
    if (!pGossip)
    {
        data << float(0);
        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS - 1; i++)
            data << float(0);

        data << uint32(1);
                            
        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS - 1; i++)
            data << uint32(0); 
    }
    else
    {
        data << float(pGossip->Options[0].Probability);
        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS - 1; i++)
            data << float(0);

        //data << textID;    // should be a broadcast id   
        if (pGossip->Options[0].BroadcastTextID > 0)
        {
            data << pGossip->Options[0].BroadcastTextID;
        }
        else
        {
            data << uint32(1);
        }
                            
        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS - 1; i++)
            data << uint32(0); 

    }

    data.WriteBit(1);                                   // has data
    data.FlushBits();

    SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_NPC_TEXT_UPDATE");
}

/// Only _static_ data is sent in this packet !!!
void WorldSession::HandlePageTextQueryOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_PAGE_TEXT_QUERY");

    uint32 pageID;
    ObjectGuid guid;

    recvData >> pageID;

    guid[2] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[2]);

    while (pageID)
    {
        WorldPacket data(SMSG_PAGE_TEXT_QUERY_RESPONSE);

        PageText const* pageText = sObjectMgr->GetPageText(pageID);

        data.WriteBit(pageText != NULL);

        if (pageText)
        {
            std::string Text = pageText->Text;
            int loc_idx = GetSessionDbLocaleIndex();
            if (loc_idx >= 0)
                if (PageTextLocale const* player = sObjectMgr->GetPageTextLocale(pageID))
                    ObjectMgr::GetLocaleStringOld(player->Text, loc_idx, Text);

            data.WriteBits(Text.size(), 12);

            data.FlushBits();

            data << uint32(pageText->NextPage);
            data << uint32(pageID);

            data.WriteString(Text);
            data << uint32(pageID);
        }
        else
        {
            data.FlushBits();
            data << uint32(0);
        }

        pageID = pageText ? pageText->NextPage : 0;

        SendPacket(&data);

        TC_LOG_DEBUG("network", "WORLD: Sent SMSG_PAGE_TEXT_QUERY_RESPONSE");
    }
}

void WorldSession::HandleCorpseMapPositionQuery(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_CORPSE_MAP_POSITION_QUERY");

    ObjectGuid corpseGuid;
    corpseGuid[7] = recvData.ReadBit();
    corpseGuid[6] = recvData.ReadBit();
    corpseGuid[3] = recvData.ReadBit();
    corpseGuid[0] = recvData.ReadBit();
    corpseGuid[4] = recvData.ReadBit();
    corpseGuid[1] = recvData.ReadBit();
    corpseGuid[5] = recvData.ReadBit();
    corpseGuid[2] = recvData.ReadBit();
    recvData.ReadByteSeq(corpseGuid[1]);
    recvData.ReadByteSeq(corpseGuid[6]);
    recvData.ReadByteSeq(corpseGuid[0]);
    recvData.ReadByteSeq(corpseGuid[5]);
    recvData.ReadByteSeq(corpseGuid[3]);
    recvData.ReadByteSeq(corpseGuid[2]);
    recvData.ReadByteSeq(corpseGuid[4]);
    recvData.ReadByteSeq(corpseGuid[7]);

    WorldPacket data(SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE, 4+4+4+4);
    data << float(0);
    data << float(0);
    data << float(0);
    data << float(0);
    SendPacket(&data);
}

void WorldSession::HandleQuestNPCQuery(WorldPacket& recvData)
{
    // use set to remove duplicates
    // proper fix would be inside GetCreatureQuestInvolvedRelationReverseBounds
    std::map<uint32, std::set<uint32>> quests;
    for (int i = 0; i < 50; ++i)
    {
        uint32 questId;
        recvData >> questId;

        if (!sObjectMgr->GetQuestTemplate(questId))
            continue;

        for (auto const& creatures : sObjectMgr->GetCreatureQuestInvolvedRelationReverseBounds(questId))
            quests[questId].emplace(creatures.second);

        for (auto const& gos : sObjectMgr->GetGOQuestInvolvedRelationReverseBounds(questId))
            quests[questId].emplace(gos.second | 0x80000000); // GO mask
    }

    uint32 count;
    recvData >> count;

    WorldPacket data(SMSG_QUEST_NPC_QUERY_RESPONSE, 3 + count * 14);
    data.WriteBits(quests.size(), 21);

    for (auto it = quests.begin(); it != quests.end(); ++it)
        data.WriteBits(it->second.size(), 22);

    for (auto it = quests.begin(); it != quests.end(); ++it)
    {
        data << uint32(it->first);
        for (const auto& entry : it->second)
            data << uint32(entry);
    }

    SendPacket(&data);
}

void WorldSession::HandleQuestPOIQuery(WorldPacket& recvData)
{
    uint32 count = recvData.ReadBits(22);

    if (count > MAX_QUEST_LOG_SIZE)
    {
        recvData.rfinish();
        return;
    }

    // Read quest ids and add the in a unordered_set so we don't send POIs for the same quest multiple times
    std::unordered_set<uint32> questIds;
    uint32 questId;
    for (int32 i = 0; i < count; ++i)
    {
        recvData >> questId;
        questIds.insert(questId);
    }

    ByteBuffer poiData;

    WorldPacket data(SMSG_QUEST_POI_QUERY_RESPONSE, 4+(4+4)*count);
    data.WriteBits(questIds.size(), 20);

    for (uint32 questId : questIds)
    {
        bool questOk = false;

        uint16 questSlot = _player->FindQuestSlot(questId);

        if (questSlot != MAX_QUEST_LOG_SIZE)
            questOk =_player->GetQuestSlotQuestId(questSlot) == questId;

        if (questOk)
        {
            QuestPOIData const* POI = sObjectMgr->GetQuestPOIData(questId);

            if (POI)
            {
                data.WriteBits(POI->Blobs.size(), 18);          // POI count bits

                for (const auto&blob : POI->Blobs)
                {
                    data.WriteBits(blob.Points.size(), 21);     // POI points count bits

                    poiData << uint32(blob.Floor);              // floor id

                    for (auto point : blob.Points)
                    {
                        poiData << int32(point.X);              // POI point x
                        poiData << int32(point.Y);              // POI point y
                    }

                    poiData << int32(blob.ObjectiveIndex);      // objective index
                    poiData << uint32(blob.Idx1);               // POI index
                    poiData << uint32(blob.QuestObjectiveId);   // quest objective id
                    poiData << uint32(0);                       // unknown (new 5.x.x)
                    poiData << uint32(blob.MapId);              // mapid
                    poiData << uint32(blob.Points.size());      // POI points count
                    poiData << uint32(blob.WorldMapAreaId);     // areaid
                    poiData << uint32(0);                       // unknown (new 5.x.x)
                    poiData << uint32(blob.Flags);              // flags
                    poiData << uint32(blob.Priority);           // priority
                }

                poiData << uint32(questId);                     // quest ID
                poiData << uint32(POI->Blobs.size());           // POI count
            }
            else
            {
                poiData << uint32(questId);
                poiData << uint32(0);

                data.WriteBits(0, 18);
            }
        }
        else
        {
            poiData << uint32(questId);
            poiData << uint32(0);

            data.WriteBits(0, 18);
        }
    }

    poiData << uint32(count);

    data.FlushBits();
    data.append(poiData);

    SendPacket(&data);
}

void WorldSession::HandleQueryCountdownTimer(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_QUERY_COUNTDOWN_TIMER");

    recvData.read_skip<uint32>();
}
