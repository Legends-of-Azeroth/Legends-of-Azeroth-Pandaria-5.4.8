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

#include "BattlePayMgr.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Realm.h"

#define PLAYABLE_RACES_COUNT 15
#define PLAYABLE_CLASSES_COUNT 11

uint8 raceExpansion[PLAYABLE_RACES_COUNT][2] =
{
    { RACE_TAUREN,            EXPANSION_CLASSIC             },
    { RACE_UNDEAD_PLAYER,     EXPANSION_CLASSIC             },
    { RACE_ORC,               EXPANSION_CLASSIC             },
    { RACE_GNOME,             EXPANSION_CLASSIC             },
    { RACE_GOBLIN,            EXPANSION_THE_BURNING_CRUSADE },
    { RACE_HUMAN,             EXPANSION_CLASSIC             },
    { RACE_TROLL,             EXPANSION_CLASSIC             },
    { RACE_PANDAREN_NEUTRAL,  EXPANSION_THE_BURNING_CRUSADE },
    { RACE_DRAENEI,           EXPANSION_THE_BURNING_CRUSADE },
    { RACE_WORGEN,            EXPANSION_THE_BURNING_CRUSADE },
    { RACE_BLOODELF,          EXPANSION_THE_BURNING_CRUSADE },
    { RACE_NIGHTELF,          EXPANSION_CLASSIC             },
    { RACE_DWARF,             EXPANSION_CLASSIC             },
    { RACE_PANDAREN_ALLIANCE, EXPANSION_THE_BURNING_CRUSADE },
    { RACE_PANDAREN_HORDE,    EXPANSION_THE_BURNING_CRUSADE },
};

uint8 classExpansion[PLAYABLE_CLASSES_COUNT][2] =
{
    { CLASS_MONK,         EXPANSION_MISTS_OF_PANDARIA      },
    { CLASS_WARRIOR,      EXPANSION_CLASSIC                },
    { CLASS_PALADIN,      EXPANSION_CLASSIC                },
    { CLASS_HUNTER,       EXPANSION_CLASSIC                },
    { CLASS_ROGUE,        EXPANSION_CLASSIC                },
    { CLASS_PRIEST,       EXPANSION_CLASSIC                },
    { CLASS_SHAMAN,       EXPANSION_CLASSIC                },
    { CLASS_MAGE,         EXPANSION_CLASSIC                },
    { CLASS_WARLOCK,      EXPANSION_CLASSIC                },
    { CLASS_DRUID,        EXPANSION_CLASSIC                },
    { CLASS_DEATH_KNIGHT, EXPANSION_WRATH_OF_THE_LICH_KING },
};

void WorldSession::SendAuthResponse(uint8 code, bool queued, uint32 queuePos)
{
    std::map<uint32, std::string> realmNamesToSend;

    RealmNameMap::const_iterator iter = realmNameStore.find(realm.Id.Realm);
    if (iter != realmNameStore.end()) // Add local realm
        realmNamesToSend[realm.Id.Realm] = iter->second;

    TC_LOG_DEBUG("network", "SMSG_AUTH_RESPONSE");
    WorldPacket packet(SMSG_AUTH_RESPONSE, 113);

    packet.WriteBit(code == AUTH_OK);

    if (code == AUTH_OK)
    {
        packet.WriteBits(realmNamesToSend.size(), 21); // Send current realmId

        for (std::map<uint32, std::string>::const_iterator itr = realmNamesToSend.begin(); itr != realmNamesToSend.end(); itr++)
        {
            packet.WriteBits(itr->second.size(), 8);
            std::string normalized = itr->second;
            normalized.erase(std::remove_if(normalized.begin(), normalized.end(), ::isspace), normalized.end());
            packet.WriteBits(normalized.size(), 8);
            packet.WriteBit(itr->first == realm.Id.Realm); // Home realm
        }

        packet.WriteBits(PLAYABLE_CLASSES_COUNT, 23);
        packet.WriteBits(0, 21);
        packet.WriteBit(0);
        packet.WriteBit(0);
        packet.WriteBit(0);
        packet.WriteBit(0);
        packet.WriteBits(PLAYABLE_RACES_COUNT, 23);
        packet.WriteBit(0);
    }

    packet.WriteBit(queued);

    if (queued)
        packet.WriteBit(1);

    packet.FlushBits();

    if (queued)
        packet << uint32(queuePos);

    if (code == AUTH_OK)
    {
        for (std::map<uint32, std::string>::const_iterator itr = realmNamesToSend.begin(); itr != realmNamesToSend.end(); itr++)
        {
            packet << uint32(itr->first);
            packet.WriteString(itr->second);
            std::string normalized = itr->second;
            normalized.erase(std::remove_if(normalized.begin(), normalized.end(), ::isspace), normalized.end());
            packet.WriteString(normalized);
        }

        for (int i = 0; i < PLAYABLE_RACES_COUNT; i++)
        {
            packet << uint8(raceExpansion[i][1]);
            packet << uint8(raceExpansion[i][0]);
        }

        for (int i = 0; i < PLAYABLE_CLASSES_COUNT; i++)
        {
            packet << uint8(classExpansion[i][1]);
            packet << uint8(classExpansion[i][0]);
        }

        packet << uint32(0);
        packet << uint8(Expansion()); // Active Expansion
        packet << uint32(0);
        packet << uint32(0); // unk time in ms
        packet << uint8(Expansion()); // Server Expansion
        packet << uint32(sBattlePayMgr->GetStoreCurrency());
        packet << uint32(0);
        packet << uint32(0);
    }

    packet << uint8(code);

    SendPacket(&packet);
}

void WorldSession::SendClientCacheVersion(uint32 version)
{
    WorldPacket data(SMSG_CLIENTCACHE_VERSION, 4);
    data << uint32(version);
    SendPacket(&data);
}

void WorldSession::SendFeatureSystemStatusGlueScreen()
{
    WorldPacket data(SMSG_FEATURE_SYSTEM_STATUS_GLUE_SCREEN);
    data.WriteBit(true);                            // show ingame shop icon
    data.WriteBit(false);                           // ingame shop parental control (1 - "Feature has been disabled by Parental Controls.")
    data.WriteBit(sBattlePayMgr->IsStoreEnabled()); // ingame shop status (0 - "The Shop is temporarily unavailable.")
    data.FlushBits();

    SendPacket(&data);
}

void WorldSession::SendDanceStudioCreateResult()
{
    WorldPacket data(SMSG_DANCE_STUDIO_CREATE_RESULT, 17);
    data << uint32(4665878);
    data << uint32(795877);
    data << uint32(10848087);
    data << uint32(1084761);
    data.WriteBit(true);
    data.FlushBits();

    SendPacket(&data);
}

void WorldSession::SendDispalyPromotionOpcode()
{
    WorldPacket data(SMSG_DISPLAY_PROMOTION, 4);
    data << uint32(0);
    SendPacket(&data);
}
