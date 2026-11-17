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

#ifndef SC_ACMGR_H
#define SC_ACMGR_H

#include "Common.h"
#include "SharedDefines.h"

class Player;
class ChatHandler;

enum ReportTypes
{
    SPEED_HACK_REPORT = 0,
    FLY_HACK_REPORT,
    WALK_WATER_HACK_REPORT,
    JUMP_HACK_REPORT,
    TELEPORT_PLANE_HACK_REPORT,
    CLIMB_HACK_REPORT,
    TIME_MANIPULATION_REPORT,

    MAX_REPORT_TYPES
};

enum DetectionTypes
{
    SPEED_HACK_DETECTION            = 1,
    FLY_HACK_DETECTION              = 2,
    WALK_WATER_HACK_DETECTION       = 4,
    JUMP_HACK_DETECTION             = 8,
    TELEPORT_PLANE_HACK_DETECTION   = 16,
    CLIMB_HACK_DETECTION            = 32
};

struct AnticheatData
{
    AnticheatData();

    void SetPosition(float x, float y, float z, float o);
    void Reset();

    uint32 lastOpcode;
    MovementInfo lastMovementInfo;
    //bool disableACCheck;
    //uint32 disableACCheckTimer;
    uint32 totalReports;
    float average;
    uint32 creationTime;
    bool hasDailyReport;

    uint32 typeReports[MAX_REPORT_TYPES];
    uint32 tempReports[MAX_REPORT_TYPES];
    uint32 tempReportsTimer[MAX_REPORT_TYPES];

    uint32 reportAnnounceCount[MAX_REPORT_TYPES];
    time_t reportAnnounceCooldown[MAX_REPORT_TYPES];
};

typedef std::unordered_map<ObjectGuid, AnticheatData> AnticheatPlayersDataMap;

class AnticheatMgr
{
    AnticheatMgr();
    ~AnticheatMgr();

    public:
        static AnticheatMgr *instance()
        {
            static AnticheatMgr _instance;
            return &_instance;
        }

        void StartHackDetection(Player* player, MovementInfo &movementInfo, uint32 opcode);
        void DeletePlayerReport(Player* player, bool login);
        void DeletePlayerData(Player* player);
        void CreatePlayerData(Player* player);
        void SavePlayerData(Player* player);

        void HandlePlayerLogin(Player* player);
        void HandlePlayerLogout(Player* player);

        uint32 GetTotalReports(ObjectGuid guid);
        float GetAverage(ObjectGuid guid);
        uint32 GetTypeReports(ObjectGuid guid, uint8 type);
        AnticheatData* GetDataFor(Player* player);

        void AnticheatGlobalCommand(ChatHandler* handler);
        void AnticheatDeleteCommand(ObjectGuid guid);

        void ResetDailyReportStates();

    private:
        void SpeedHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data);
        void FlyHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data);
        void WalkOnWaterHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data);
        void JumpHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data, uint32 opcode);
        void TeleportPlaneHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data);
        void ClimbHackDetection(Player* player, MovementInfo &movementInfo, AnticheatData& data, uint32 opcode);

        void BuildReport(Player* player, AnticheatData& data, uint8 reportType);

        bool MustCheckTempReports(uint8 type);

        AnticheatPlayersDataMap _players;
};

#define sAnticheatMgr AnticheatMgr::instance()

#endif
