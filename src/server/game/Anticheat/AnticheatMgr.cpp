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

#include "AnticheatMgr.h"
#include "ScriptMgr.h"
#include "MapManager.h"
#include "Chat.h"
#include "Player.h"

#define CLIMB_ANGLE 1.9f

AnticheatData::AnticheatData()
{
    lastOpcode = 0;
    totalReports = 0;
    for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
    {
        typeReports[i] = 0;
        tempReports[i] = 0;
        tempReportsTimer[i] = 0;
        reportAnnounceCount[i] = 0;
        reportAnnounceCooldown[i] = 0;
    }
    average = 0;
    creationTime = 0;
    hasDailyReport = false;
}

void AnticheatData::Reset()
{
    totalReports = 0;
    average = 0;
    creationTime = 0;
    for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
    {
        tempReports[i] = 0;
        tempReportsTimer[i] = 0;
        typeReports[i] = 0;
    }
}

void AnticheatData::SetPosition(float x, float y, float z, float o)
{
    lastMovementInfo.pos.m_positionX = x;
    lastMovementInfo.pos.m_positionY = y;
    lastMovementInfo.pos.m_positionZ = z;
    lastMovementInfo.pos.SetOrientation(o);
}

class AnticheatScript : public PlayerScript
{
public:
    AnticheatScript() : PlayerScript("AnticheatScripts") { }

    void OnLogout(Player* player) override
    {
        sAnticheatMgr->HandlePlayerLogout(player);
    }

    void OnLogin(Player* player) override
    {
        sAnticheatMgr->HandlePlayerLogin(player);
    }
};

void AddSC_Anticheat()
{
    new AnticheatScript();
}

AnticheatMgr::AnticheatMgr()
{
}

AnticheatMgr::~AnticheatMgr()
{
}

void AnticheatMgr::JumpHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data, uint32 opcode)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & JUMP_HACK_DETECTION) == 0)
        return;

    if (data.lastOpcode == MSG_MOVE_JUMP && opcode == MSG_MOVE_JUMP)
    {
        BuildReport(player, data, JUMP_HACK_REPORT);
        //TC_LOG_ERROR("entities.player", "AnticheatMgr:: Jump-Hack detected player GUID (low) {}",player->GetGUIDLow());
    }
}

void AnticheatMgr::WalkOnWaterHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & WALK_WATER_HACK_DETECTION) == 0)
        return;

    if (!data.lastMovementInfo.HasMovementFlag(MOVEMENTFLAG_WATERWALKING))
        return;

    // if we are a ghost we can walk on water
    if (!player->IsAlive())
        return;

    if (player->HasAuraType(SPELL_AURA_FEATHER_FALL) ||
        player->HasAuraType(SPELL_AURA_SAFE_FALL) ||
        player->HasAuraType(SPELL_AURA_WATER_WALK))
        return;

    //TC_LOG_ERROR("entities.player", "AnticheatMgr:: Walk on Water - Hack detected player GUID (low) {}",player->GetGUIDLow());
    BuildReport(player, data, WALK_WATER_HACK_REPORT);

}

void AnticheatMgr::FlyHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & FLY_HACK_DETECTION) == 0)
        return;

    if (!data.lastMovementInfo.HasMovementFlag(MOVEMENTFLAG_FLYING))
        return;

    if (player->HasAuraType(SPELL_AURA_FLY) ||
        player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
        player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED))
        return;

    //TC_LOG_ERROR("entities.player", "AnticheatMgr:: Fly-Hack detected player GUID (low) {}",player->GetGUIDLow());
    BuildReport(player, data, FLY_HACK_REPORT);
}

void AnticheatMgr::TeleportPlaneHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & TELEPORT_PLANE_HACK_DETECTION) == 0)
        return;

    if (data.lastMovementInfo.pos.GetPositionZ() != 0 ||
        movementInfo.pos.GetPositionZ() != 0)
        return;

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
        return;

    float x, y, z;
    player->GetPosition(x, y, z);
    float ground_Z = player->GetMap()->GetHeight(x, y, z);
    float z_diff = fabs(ground_Z - z);

    // we are not really walking there
    if (z_diff > 1.0f)
    {
        //TC_LOG_ERROR("entities.player", "AnticheatMgr:: Teleport To Plane - Hack detected player GUID (low) {}",player->GetGUIDLow());
        BuildReport(player, data, TELEPORT_PLANE_HACK_REPORT);
    }
}

void AnticheatMgr::StartHackDetection(Player* player, MovementInfo &movementInfo, uint32 opcode)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    if (player->GetSession()->GetSecurity() > SEC_MODERATOR)
        return;

    if (!(player->GetMap()) || player->GetMap()->IsBattleground())
        return;

    AnticheatData* data = GetDataFor(player);
    if (!data)
        return;

    if (player->IsInFlight() || player->GetTransport() || player->GetVehicle())
    {

        data->lastMovementInfo = movementInfo;
        data->lastOpcode = opcode;
        return;
    }

    SpeedHackDetection(player, movementInfo, *data);
    FlyHackDetection(player, movementInfo, *data);
    WalkOnWaterHackDetection(player, movementInfo, *data);
    JumpHackDetection(player, movementInfo, *data, opcode);
    TeleportPlaneHackDetection(player, movementInfo, *data);
    ClimbHackDetection(player, movementInfo, *data, opcode);

    data->lastMovementInfo = movementInfo;
    data->lastOpcode = opcode;
}

// basic detection
void AnticheatMgr::ClimbHackDetection(Player *player, MovementInfo &movementInfo, AnticheatData& data, uint32 opcode)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & CLIMB_HACK_DETECTION) == 0)
        return;

    if (opcode != MSG_MOVE_HEARTBEAT ||
        data.lastOpcode != MSG_MOVE_HEARTBEAT)
        return;

    // in this case we don't care if they are "legal" flags, they are handled in another parts of the Anticheat Manager.
    if (player->IsInWater() ||
        player->IsFlying() ||
        player->IsFalling())
        return;

    Position playerPos = player->GetPosition();

    float deltaZ = fabs(playerPos.GetPositionZ() - movementInfo.pos.GetPositionZ());
    float deltaXY = movementInfo.pos.GetExactDist2d(&playerPos);

    float angle = Position::NormalizeOrientation(tan(deltaZ/deltaXY));

    if (angle > CLIMB_ANGLE)
    {
        //TC_LOG_ERROR("entities.player", "AnticheatMgr:: Climb-Hack detected player GUID (low) {}", player->GetGUIDLow());
        BuildReport(player, data, CLIMB_HACK_REPORT);
    }
}

void AnticheatMgr::SpeedHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & SPEED_HACK_DETECTION) == 0)
        return;

    if (player->GetMapId() == 607 || player->HasAura(35477) || player->HasAura(2983) || player->HasAura(1850) ||
        player->HasAuraType(SPELL_AURA_FEATHER_FALL) || player->HasAuraType(SPELL_AURA_SAFE_FALL) ||
        player->GetMapId() == 369 || player->GetMapId() == 582 || player->GetMapId() == 584 || player->GetMapId() == 586 ||
        player->GetMapId() == 587 || player->GetMapId() == 588 || player->GetMapId() == 589 || player->GetMapId() == 590 ||
        player->GetMapId() == 591 || player->GetMapId() == 592 || player->GetMapId() == 593 || player->GetMapId() == 594 ||
        player->GetMapId() == 596 || player->GetMapId() == 610 || player->GetMapId() == 612 || player->GetMapId() == 613 ||
        player->GetMapId() == 614 || player->GetMapId() == 620 || player->GetMapId() == 621 || player->GetMapId() == 622 ||
        player->GetMapId() == 623 || player->GetMapId() == 641 || player->GetMapId() == 642 || player->GetMapId() == 647 ||
        player->GetMapId() == 672 || player->GetMapId() == 673 || player->GetMapId() == 712 || player->GetMapId() == 713 ||
        player->GetMapId() == 718)
        return;

    if (player->IsInWater() && player->HasAura(57856))
        return;

    float distance2D = movementInfo.pos.GetExactDist2d(&data.lastMovementInfo.pos);
    uint8 moveType = 0;

    // we need to know HOW is the player moving
    // TO-DO: Should we check the incoming movement flags?
    if (player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
        moveType = MOVE_SWIM;
    else if (player->IsFlying())
        moveType = MOVE_FLIGHT;
    else if (player->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
        moveType = MOVE_WALK;
    else
        moveType = MOVE_RUN;

    // how many yards the player can do in one sec.
    float speedRate = player->GetSpeed(UnitMoveType(moveType)) + movementInfo.jump.xyspeed;

    // how long the player took to move to here.
    uint32 timeDiff = getMSTimeDiff(data.lastMovementInfo.time, movementInfo.time);

    if (int32(timeDiff) < 0)
    {
        BuildReport(player, data, TIME_MANIPULATION_REPORT);
        timeDiff = 1;
    }

    if (!timeDiff)
        timeDiff = 1;

    // this is the distance doable by the player in 1 sec, using the time done to move to this point.
    float clientSpeedRate = distance2D * 1000 / timeDiff;

    // we did the (uint32) cast to accept a margin of tolerance
    if (clientSpeedRate > speedRate * 1.25f)
    {
        BuildReport(player, data, SPEED_HACK_REPORT);
        //TC_LOG_ERROR("entities.player", "AnticheatMgr:: Speed-Hack detected player GUID (low) {}",player->GetGUIDLow());
    }
}

void AnticheatMgr::HandlePlayerLogin(Player* player)
{
    // we must delete this to prevent errors in case of crash
    //CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid={}",player->GetGUIDLow());
    // we initialize the pos of lastMovementPosition var.
    _players[player->GetGUID()].SetPosition(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    /*QueryResult resultDB = CharacterDatabase.PQuery("SELECT * FROM daily_players_reports WHERE guid={};",player->GetGUIDLow());
    
    if (resultDB)
        _players[player->GetGUIDLow()].SetDailyReportState(true);*/
}

void AnticheatMgr::HandlePlayerLogout(Player* player)
{
    // TO-DO Make a table that stores the cheaters of the day, with more detailed information.

    // We must also delete it at logout to prevent have data of offline players in the db when we query the database (IE: The GM Command)
    //CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid={}",player->GetGUIDLow());
    // Delete not needed data from the memory.
    _players.erase(player->GetGUID());
}

void AnticheatMgr::SavePlayerData(Player* player)
{
    //CharacterDatabase.PExecute("REPLACE INTO players_reports_status (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES ({},{},{},{},{},{},{},{},{},{});",player->GetGUIDLow(),_players[player->GetGUIDLow()].GetAverage(),_players[player->GetGUIDLow()].GetTotalReports(), _players[player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT),_players[player->GetGUIDLow()].GetCreationTime());
}

uint32 AnticheatMgr::GetTotalReports(ObjectGuid guid)
{
    AnticheatPlayersDataMap::iterator iter = _players.find(guid);
    if (iter == _players.end())
        return 0;

    return iter->second.totalReports;
}

float AnticheatMgr::GetAverage(ObjectGuid guid)
{
    AnticheatPlayersDataMap::iterator iter = _players.find(guid);
    if (iter == _players.end())
        return 0.0f;

    return iter->second.average;
}

uint32 AnticheatMgr::GetTypeReports(ObjectGuid guid, uint8 type)
{
    AnticheatPlayersDataMap::iterator iter = _players.find(guid);
    if (iter == _players.end())
        return 0;

    return iter->second.typeReports[type];
}

bool AnticheatMgr::MustCheckTempReports(uint8 type)
{
    if (type == JUMP_HACK_REPORT)
        return false;

    return true;
}

void AnticheatMgr::BuildReport(Player* player, AnticheatData& data, uint8 reportType)
{
    auto key = player->GetGUID();

    if (MustCheckTempReports(reportType))
    {
        uint32 actualTime = getMSTime();

        if (!data.tempReportsTimer[reportType])
            data.tempReportsTimer[reportType] = actualTime;

        if (getMSTimeDiff(data.tempReportsTimer[reportType], actualTime) < 3000)
        {
            data.tempReports[reportType] += 1;

            if (data.tempReports[reportType] < 3)
                return;
        }
        else
        {
            data.tempReportsTimer[reportType] = actualTime;
            data.tempReports[reportType] = 1;
            return;
        }
    }

    // generating creationTime for average calculation
    if (!data.totalReports)
        data.creationTime = getMSTime();

    // increasing total_reports
    data.totalReports += 1;
    // increasing specific cheat report
    data.typeReports[reportType] += 1;

    // diff time for average calculation
    uint32 diffTime = getMSTimeDiff(data.creationTime, getMSTime()) / IN_MILLISECONDS;

    if (diffTime > 0)
    {
        // Average == Reports per second
        float average = float(data.totalReports) / float(diffTime);
        data.average = average;
    }

    /*if (sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_DAILY_REPORT) < data.GetTotalReports())
    {
        if (!data.GetDailyReportState())
        {
            CharacterDatabase.PExecute("REPLACE INTO daily_players_reports (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES ({},{},{},{},{},{},{},{},{},{});",player->GetGUIDLow(),_players[player->GetGUIDLow()].GetAverage(),_players[player->GetGUIDLow()].GetTotalReports(), _players[player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT),_players[player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT),_players[player->GetGUIDLow()].GetCreationTime());
            data.SetDailyReportState(true);
        }
    }*/

    // Send warning to ingame GMs
    if (data.totalReports > sWorld->getIntConfig(CONFIG_ANTICHEAT_REPORTS_INGAME_NOTIFICATION))
    {  // mask if need 64 - Anything related to network input/output

        uint32& count = data.reportAnnounceCount[reportType];
        ++count;

        time_t now = time(NULL);
        if (data.reportAnnounceCooldown[reportType] < now)
            data.reportAnnounceCooldown[reportType] = now + 3;
        else
            return;

        const char* reportTypeName;
        switch (reportType)
        {
            case SPEED_HACK_REPORT:             reportTypeName = "SpeedHack";           break;
            case FLY_HACK_REPORT:               reportTypeName = "FlyHack";             break;
            case WALK_WATER_HACK_REPORT:        reportTypeName = "WaterWalkHack";       break;
            case JUMP_HACK_REPORT:              reportTypeName = "JumpHack";            break;
            case TELEPORT_PLANE_HACK_REPORT:    reportTypeName = "TeleportPlaneHack";   break;
            case CLIMB_HACK_REPORT:             reportTypeName = "ClimbHack";           break;
            case TIME_MANIPULATION_REPORT:      reportTypeName = "TimeManipulation";    break;
            default:                            reportTypeName = "?";                   break;
        }
        TC_LOG_DEBUG("network", "Icore GUARD: {} detected as possible cheater. HackType: {}.", player->GetName().c_str(), reportType);
        sWorld->SendGMText(LANG_CHEATER_CHATLOG, "ICORE GUARD", player->GetName().c_str(), player->GetName().c_str(), reportTypeName, count < 10 ? "|cFF00FF00" : count < 20 ? "|cFFFF8000" : "|cFFFF0000", count);
        count = 0;
    }
}

void AnticheatMgr::AnticheatGlobalCommand(ChatHandler* handler)
{
    // MySQL will sort all for us, anyway this is not the best way we must only save the anticheat data not whole player's data!.
    ObjectAccessor::SaveAllPlayers();

    QueryResult resultDB = CharacterDatabase.Query("SELECT guid,average,total_reports FROM players_reports_status WHERE total_reports != 0 ORDER BY average ASC LIMIT 3;");
    if (!resultDB)
    {
        handler->PSendSysMessage("No players found.");
        return;
    }
    else
    {
        handler->SendSysMessage("=============================");
        handler->PSendSysMessage("Players with the lowest averages:");
        do
        {
            Field *fieldsDB = resultDB->Fetch();

            uint32 guid = fieldsDB[0].GetUInt32();
            float average = fieldsDB[1].GetFloat();
            uint32 total_reports = fieldsDB[2].GetUInt32();

            if (Player* player = sObjectMgr->GetPlayerByLowGUID(guid))
                handler->PSendSysMessage("Player: %s Average: %f Total Reports: %u", player->GetName().c_str(),average,total_reports);

        } while (resultDB->NextRow());
    }

    resultDB = CharacterDatabase.Query("SELECT guid,average,total_reports FROM players_reports_status WHERE total_reports != 0 ORDER BY total_reports DESC LIMIT 3;");

    // this should never happen
    if (!resultDB)
    {
        handler->PSendSysMessage("No players found.");
        return;
    }
    else
    {
        handler->SendSysMessage("=============================");
        handler->PSendSysMessage("Players with the more reports:");
        do
        {
            Field *fieldsDB = resultDB->Fetch();

            uint32 guid = fieldsDB[0].GetUInt32();
            float average = fieldsDB[1].GetFloat();
            uint32 total_reports = fieldsDB[2].GetUInt32();

            if (Player* player = sObjectMgr->GetPlayerByLowGUID(guid))
                handler->PSendSysMessage("Player: %s Total Reports: %u Average: %f", player->GetName().c_str(),total_reports,average);

        } while (resultDB->NextRow());
    }
}

void AnticheatMgr::AnticheatDeleteCommand(ObjectGuid guid)
{
    if (!guid)
    {
        for (AnticheatPlayersDataMap::iterator it = _players.begin(); it != _players.end(); ++it)
            (*it).second.Reset();

        CharacterDatabase.PExecute("DELETE FROM players_reports_status;");
    }
    else
    {
        AnticheatPlayersDataMap::iterator iter = _players.find(guid);
        if (iter == _players.end())
            return;

        iter->second.Reset();
        CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid={};", guid);
    }
}

void AnticheatMgr::ResetDailyReportStates()
{
     for (AnticheatPlayersDataMap::iterator it = _players.begin(); it != _players.end(); ++it)
         it->second.hasDailyReport = false;
}

AnticheatData* AnticheatMgr::GetDataFor(Player* player)
{
    AnticheatPlayersDataMap::iterator iter = _players.find(player->GetGUID());
    if (iter == _players.end())
    {
        //TC_LOG_ERROR("entities.player", "AnticheatMgr::GetDataFor - player (GUID: {}) data not found, kicking out", player->GetGUIDLow());
        //player->GetSession()->KickPlayer();
        return NULL;
    }

    return &iter->second;
}
