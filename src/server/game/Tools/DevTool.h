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

#ifndef DEVTOOL_H_
#define DEVTOOL_H_

#include "Common.h"

#include "Chat.h"
#include "Creature.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "GridNotifiersImpl.h"

// When adding new tools:
// 1. Add new enum to DevToolType below
// 2. Add cases for new enum in DevToolSettings::Setup and DevToolSettings::Execute
// 3. Copy DevToolSettings::Template with a name of your enum at the end of the class, edit its contents respective to comments
// 4. In cs_misc.cpp add a new command with implementation: { return SelectToolHelper(handler, args, DevToolType::YourEnumHere); }

enum class DevToolType : uint8
{
    Move,
    Jump,
    Facing,
    Teleport,
    OutputXY,
    OutputXYZ,
    Distance,
    Angle,
    FindNearestGO,
    SpawnCreature,
    SpawnGO,
    LOS,
    MMaps,
};

union DevToolSettings
{
    bool Setup(DevToolType type, ChatHandler* handler, char const* args)
    {
        switch (type)
        {
            case DevToolType::Move:             return Move             .Setup(handler, args);
            case DevToolType::Jump:             return Jump             .Setup(handler, args);
            case DevToolType::Facing:           return Facing           .Setup(handler, args);
            case DevToolType::Teleport:         return Teleport         .Setup(handler, args);
            case DevToolType::OutputXY:         return OutputXY         .Setup(handler, args);
            case DevToolType::OutputXYZ:        return OutputXYZ        .Setup(handler, args);
            case DevToolType::Distance:         return Distance         .Setup(handler, args);
            case DevToolType::Angle:            return Angle            .Setup(handler, args);
            case DevToolType::FindNearestGO:    return FindNearestGO    .Setup(handler, args);
            case DevToolType::SpawnCreature:    return SpawnCreature    .Setup(handler, args);
            case DevToolType::SpawnGO:          return SpawnGO          .Setup(handler, args);
            case DevToolType::LOS:              return LOS              .Setup(handler, args);
            case DevToolType::MMaps:            return MMaps            .Setup(handler, args);
            default:
                ASSERT(false && "DevToolSettings::Setup: Unhandled DevToolType");
                return false;
        }
    }
    bool Execute(DevToolType type, Player* player, Unit* target, float x, float y, float z, float o)
    {
        switch (type)
        {
            case DevToolType::Move:             return Move             .Execute(player, target, x, y, z, o);
            case DevToolType::Jump:             return Jump             .Execute(player, target, x, y, z, o);
            case DevToolType::Facing:           return Facing           .Execute(player, target, x, y, z, o);
            case DevToolType::Teleport:         return Teleport         .Execute(player, target, x, y, z, o);
            case DevToolType::OutputXY:         return OutputXY         .Execute(player, target, x, y, z, o);
            case DevToolType::OutputXYZ:        return OutputXYZ        .Execute(player, target, x, y, z, o);
            case DevToolType::Distance:         return Distance         .Execute(player, target, x, y, z, o);
            case DevToolType::Angle:            return Angle            .Execute(player, target, x, y, z, o);
            case DevToolType::FindNearestGO:    return FindNearestGO    .Execute(player, target, x, y, z, o);
            case DevToolType::SpawnCreature:    return SpawnCreature    .Execute(player, target, x, y, z, o);
            case DevToolType::SpawnGO:          return SpawnGO          .Execute(player, target, x, y, z, o);
            case DevToolType::LOS:              return LOS              .Execute(player, target, x, y, z, o);
            case DevToolType::MMaps:            return MMaps            .Execute(player, target, x, y, z, o);
            default:
                ASSERT(false && "DevToolSettings::Execute: Unhandled DevToolType");
                return false;
        }
    }
    static void ReportSelectedTool(ChatHandler* handler, char const* name, char const* params)
    {
        handler->PSendSysMessage("Selected tool: %s (%s)", name, params);
    }

    enum class OrientationOptions : uint8
    {
        None,
        MatchCaster,
        FaceCaster,
    };

    struct
    {
        // Parameters go here

        char const* GetName() const { return ""/* "Name" */; }
        char const* GetParams() const { return ""/* "[ParamName:type | OtherOptionVariant:'token to use this variant' | YetAnotherOptionParam = DefaultValue] [...] ..." */; }
        bool Setup(ChatHandler* handler, char const* /*args*/)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            // Tokenize args here

            // Fill parameters from tokenized args here
            return true;
        }
        bool Execute(Player* /*player*/, Unit* /*target*/, float /*x*/, float /*y*/, float /*z*/, float /*o*/)
        {
            // Do stuff when player clicks on the ground with 30012 spell
            return true;
        }
    } Template;

    struct
    {
        uint32 PointID;
        float Speed;
        bool Walk;

        char const* GetName() const { return "Move"; }
        char const* GetParams() const { return "[PointID:int] [Speed:float] [Walk:bool]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            char* pointID = strtok((char*)args, " ");
            char* speed = strtok(nullptr, " ");
            char* walk = strtok(nullptr, " ");

            PointID = pointID ? atoi(args) : 0;
            Speed   = speed ? (float)atof(speed) : 0.0f;
            Walk    = walk && (*walk == '1' || *walk == 't');
            return true;
        }
        bool Execute(Player* /*player*/, Unit* target, float x, float y, float z, float /*o*/)
        {
            target->SetWalk(Walk);
            if (Speed)
                target->SetSpeed(target->HasUnitMovementFlag(MOVEMENTFLAG_WALKING) ? MOVE_WALK : target->HasUnitMovementFlag(MOVEMENTFLAG_FLYING) ? MOVE_FLIGHT : MOVE_RUN, Speed);
            target->GetMotionMaster()->MovePoint(PointID, x, y, z);
            return true;
        }
    } Move;
    struct
    {
        float SpeedXY;
        float SpeedZ;
        uint32 PointID;

        char const* GetName() const { return "Jump"; }
        char const* GetParams() const { return "[SpeedXY:float] [SpeedZ:float] [PointID:int]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            char* speedXY = strtok((char*)args, " ");
            char* speedZ = strtok(nullptr, " ");
            char* pointID = strtok(nullptr, " ");

            SpeedXY = speedXY ? (float)atof(speedXY) : 1.0f;
            SpeedZ  = speedZ ? (float)atof(speedZ) : 1.0f;
            PointID = pointID ? atoi(pointID) : 0;
            return true;
        }
        bool Execute(Player* /*player*/, Unit* target, float x, float y, float z, float /*o*/)
        {
            target->GetMotionMaster()->MoveJump(x, y, z, SpeedXY, SpeedZ, PointID);
            return true;
        }
    } Jump;
    struct
    {
        char const* GetName() const { return "Facing"; }
        char const* GetParams() const { return ""; }
        bool Setup(ChatHandler* handler, char const* /*args*/)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            return true;
        }
        bool Execute(Player* /*player*/, Unit* target, float x, float y, float /*z*/, float /*o*/)
        {
            target->SetFacingTo(target->GetAngle(x, y));
            return true;
        }
    } Facing;
    struct
    {
        int32 OrientationDegrees;
        float OrientationRadians;
        OrientationOptions OrientationOption;

        char const* GetName() const { return "Teleport"; }
        char const* GetParams() const { return "[OrientationDegrees:int | FaceCaster:'m' | MatchCaster:*] [OrientationRadians:float]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            char* orientation = strtok((char*)args, " ");
            char* orientationFloat = strtok(nullptr, " ");

            if (orientation && ((*orientation >= '0' && *orientation <= '9') || *orientation == '-' || *orientation == '+'))
            {
                OrientationDegrees = atoi(orientation);
                OrientationOption = OrientationOptions::None;
            }
            else
            {
                OrientationDegrees = 0;
                if (orientation && *orientation == 'm')
                    OrientationOption = OrientationOptions::FaceCaster;
                else
                    OrientationOption = OrientationOptions::MatchCaster;
            }

            OrientationRadians = orientationFloat ? (float)atof(orientationFloat) : 0.0f;
            return true;
        }
        bool Execute(Player* /*player*/, Unit* target, float x, float y, float z, float o)
        {
            switch (OrientationOption)
            {
                case OrientationOptions::None:
                    o = OrientationRadians ? OrientationRadians : (OrientationDegrees / 180.0f * M_PI);
                    break;
                case OrientationOptions::FaceCaster:
                    o = Position::NormalizeOrientation(target->GetAngle(x, y) + M_PI);
                    break;
                case OrientationOptions::MatchCaster:
                    o = target->GetOrientation();
                    break;
            }
            target->NearTeleportTo(x, y, z, o);
            return true;
        }
    } Teleport;
    struct
    {
        char const* GetName() const { return "OutputXY"; }
        char const* GetParams() const { return ""; }
        bool Setup(ChatHandler* handler, char const* /*args*/)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            return true;
        }
        bool Execute(Player* player, Unit* /*target*/, float x, float y, float /*z*/, float /*o*/)
        {
            ChatHandler(player).PSendSysMessage("|cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf", x, y);
            return true;
        }
    } OutputXY;
    struct
    {
        char const* GetName() const { return "OutputXYZ"; }
        char const* GetParams() const { return ""; }
        bool Setup(ChatHandler* handler, char const* /*args*/)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            return true;
        }
        bool Execute(Player* player, Unit* /*target*/, float x, float y, float z, float o)
        {
            ChatHandler(player).PSendSysMessage("|cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf, |cFFCCCCFF%f|rf", x, y, z);
            return true;
        }
    } OutputXYZ;
    struct
    {
        bool Use2D;
        bool UseCasterCombatReach;

        char const* GetName() const { return "Distance"; }
        char const* GetParams() const { return "[Use2D:bool] [UseCasterCombatReach:bool]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            char* d2d = strtok((char*)args, " ");
            char* bbox = strtok(nullptr, " ");

            Use2D                   = d2d && (*d2d == '1' || *d2d == 't');
            UseCasterCombatReach    = bbox && (*bbox == '1' || *bbox == 't');
            return true;
        }
        bool Execute(Player* player, Unit* target, float x, float y, float z, float o)
        {
            ChatHandler(player).PSendSysMessage("|cFFFFFFFF%f|rf (from |cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf, |cFFCCCCFF%f|rf to |cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf, |cFFCCCCFF%f|rf%s %s)", Use2D
                ? UseCasterCombatReach ? target->GetDistance2d(x, y) : target->GetExactDist2d(x, y)
                : UseCasterCombatReach ? target->GetDistance(x, y, z) : target->GetExactDist(x, y, z),
                target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),
                x, y, z,
                UseCasterCombatReach ? "" : " Exact",
                Use2D ? "2D" : "3D");
            return true;
        }
    } Distance;
    struct
    {
        bool Relative;

        char const* GetName() const { return "Angle"; }
        char const* GetParams() const { return "[Relative:bool]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            Relative = *args == '1' || *args == 't';
            return true;
        }
        bool Execute(Player* player, Unit* target, float x, float y, float /*z*/, float /*o*/)
        {
            float angle = Position::NormalizeOrientation(Relative ? target->GetRelativeAngle(x, y) : target->GetAngle(x, y));
            if (Relative && angle > M_PI)
                angle = angle - 2 * M_PI;

            ChatHandler(player).PSendSysMessage("|cFFFFFFFF%f|rf (|cFFFFFFFF%i|r deg) (from |cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf to |cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf%s)",
                angle, int32(angle / M_PI * 180),
                target->GetPositionX(), target->GetPositionY(),
                x, y,
                Relative ? " Relative" : "");
            return true;
        }
    } Angle;
    struct
    {
        char const* GetName() const { return "FindNearestGO"; }
        char const* GetParams() const { return ""; }
        bool Setup(ChatHandler* handler, char const* /*args*/)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            return true;
        }
        bool Execute(Player* player, Unit* /*target*/, float x, float y, float z, float /*o*/)
        {
            GameObject* result = nullptr;
            float dist = MAX_VISIBILITY_DISTANCE;
            auto check = [&dist, x, y, z](GameObject* go)
            {
                float d = go->GetExactDist(x, y, z);
                if (d < dist)
                {
                    dist = d;
                    return true;
                }
                return false;
            };

            Trinity::GameObjectLastSearcher<decltype(check)> searcher(player, result, check);
            player->VisitNearbyGridObject(dist, searcher);

            if (result)
            {
                char const* state = nullptr;
                char const* loot = nullptr;
                switch (result->GetGoState())
                {
                    case GO_STATE_ACTIVE: state = "GO_STATE_ACTIVE"; break;
                    case GO_STATE_READY: state = "GO_STATE_READY"; break;
                    case GO_STATE_ACTIVE_ALTERNATIVE: state = "GO_STATE_ACTIVE_ALTERNATIVE"; break;

                    default: break;
                }
                switch (result->getLootState())
                {
                    case GO_NOT_READY: loot = "GO_NOT_READY"; break;
                    case GO_READY: loot = "GO_READY"; break;
                    case GO_ACTIVATED: loot = "GO_ACTIVATED"; break;
                    case GO_JUST_DEACTIVATED: loot = "GO_JUST_DEACTIVATED"; break;

                    default: break;
                }

                ChatHandler(player).PSendSysMessage("GUID: |cFFFFFFFF%u|r\nID: |cFFFFFFFF%u|r\nName: |cFFFFFFFF%s|r\nGOState: |cFFFFFFFF%s|r\nLootState: |cFFFFFFFF%s|r\n|cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf, |cFFCCCCFF%f|rf, |cFFCCCCCC%f|rf",
                    result->GetDBTableGUIDLow(),
                    result->GetEntry(),
                    result->GetName().c_str(),
                    state,
                    loot,
                    result->GetPositionX(), result->GetPositionY(), result->GetPositionZ(), result->GetOrientation());

                return true;
            }
            return false;
        }
    } FindNearestGO;
    struct
    {
        uint32 ID;
        int32 OrientationDegrees;
        OrientationOptions OrientationOption;
        bool SaveToDB;

        char const* GetName() const { return "SpawnCreature"; }
        char const* GetParams() const { return "[ID = previous] [OrientationDegrees:int | FaceCaster:'m' | MatchCaster:*] [SaveToDB:bool]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            if (!*args)
            {
                if (CreatureTemplate const* temp = sObjectMgr->GetCreatureTemplate(ID))
                    handler->PSendSysMessage("Spawning creature ID |cFFFFFFFF%u|r - |cFFFFFFFF%s|r", ID, temp->Name.c_str());
                else
                    handler->PSendSysMessage("Spawning creature ID |cFFFFFFFF%u|r - |cFFFFFFFF???|r", ID);
            }
            else
            {
                char* ids = strtok((char*)args, " ");
                char* orientation = strtok(nullptr, " ");
                char* save = strtok(nullptr, " ");

                uint32 id = atoi(ids);
                if (CreatureTemplate const* temp = sObjectMgr->GetCreatureTemplate(id))
                {
                    handler->PSendSysMessage("Spawning creature ID |cFFFFFFFF%u|r - |cFFFFFFFF%s|r", id, temp->Name.c_str());
                    ID = id;
                    if (orientation && ((*orientation >= '0' && *orientation <= '9') || *orientation == '-' || *orientation == '+'))
                    {
                        OrientationDegrees = atoi(orientation);
                        OrientationOption = OrientationOptions::None;
                    }
                    else
                    {
                        OrientationDegrees = 0;
                        if (orientation && *orientation == 'm')
                            OrientationOption = OrientationOptions::FaceCaster;
                        else
                            OrientationOption = OrientationOptions::MatchCaster;
                    }
                    SaveToDB = save && (*save == '1' || *save == 't');
                }
                else
                    handler->PSendSysMessage("Missing creature template for ID %u", id);
            }
            return true;
        }
        bool Execute(Player* player, Unit* target, float x, float y, float z, float o)
        {
            switch (OrientationOption)
            {
                case OrientationOptions::None:
                    o = OrientationDegrees / 180.0f * M_PI;
                    break;
                case OrientationOptions::FaceCaster:
                    o = Position::NormalizeOrientation(target->GetAngle(x, y) + M_PI);
                    break;
                case OrientationOptions::MatchCaster:
                    o = target->GetOrientation();
                    break;
            }

            Creature* creature = new Creature();
            if (creature->Create(player->GetMap()->GenerateLowGuid<HighGuid::Unit>(), player->GetMap(), player->GetPhaseMask() == PHASEMASK_ANYWHERE ? PHASEMASK_NORMAL : player->GetPhaseMask(), ID, 0, 0, x, y, z, o))
            {
                player->GetMap()->AddToMap(creature);
                if (SaveToDB)
                    creature->SaveToDB(creature->GetMapId(), 1 << creature->GetMap()->GetSpawnMode(), creature->GetPhaseMask());

                ChatHandler(player).PSendSysMessage("Spawned GUID: |cFFFFFFFF%u|r ID: |cFFFFFFFF%u|r at |cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf, |cFFCCCCFF%f|rf, |cFFCCCCCC%f|rf",
                    creature->GetDBTableGUIDLow(),
                    creature->GetEntry(),
                    creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation());
                return true;
            }
            else
            {
                delete creature;
                return false;
            }
        }
    } SpawnCreature;
    struct
    {
        uint32 ID;
        int32 OrientationDegrees;
        OrientationOptions OrientationOption;
        bool SaveToDB;
        float Rotation[4];

        char const* GetName() const { return "SpawnGO"; }
        char const* GetParams() const { return "[ID = previous] [OrientationDegrees:int | FaceCaster:'m' | MatchCaster:*] [SaveToDB:bool] [RotationX:float] [RotationO:float] [RotationZ:float] [RotationW:float]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            if (!*args)
            {
                if (GameObjectTemplate const* temp = sObjectMgr->GetGameObjectTemplate(ID))
                    handler->PSendSysMessage("Spawning gameobject ID |cFFFFFFFF%u|r - |cFFFFFFFF%s|r", ID, temp->name.c_str());
                else
                    handler->PSendSysMessage("Spawning gameobject ID |cFFFFFFFF%u|r - |cFFFFFFFF???|r", ID);
            }
            else
            {
                char* ids = strtok((char*)args, " ");
                char* orientation = strtok(nullptr, " ");
                char* save = strtok(nullptr, " ");
                char* rx = strtok(nullptr, " ");
                char* ry = strtok(nullptr, " ");
                char* rz = strtok(nullptr, " ");
                char* rw = strtok(nullptr, " ");

                uint32 id = atoi(ids);
                if (GameObjectTemplate const* temp = sObjectMgr->GetGameObjectTemplate(id))
                {
                    handler->PSendSysMessage("Spawning gameobject ID |cFFFFFFFF%u|r - |cFFFFFFFF%s|r", id, temp->name.c_str());
                    ID = id;
                    if (orientation && ((*orientation >= '0' && *orientation <= '9') || *orientation == '-' || *orientation == '+'))
                    {
                        OrientationDegrees = atoi(orientation);
                        OrientationOption = OrientationOptions::None;
                    }
                    else
                    {
                        OrientationDegrees = 0;
                        if (orientation && *orientation == 'm')
                            OrientationOption = OrientationOptions::FaceCaster;
                        else
                            OrientationOption = OrientationOptions::MatchCaster;
                    }
                    SaveToDB = save && (*save == '1' || *save == 't');
                    Rotation[0] = rx ? atof(rx) : 0.0f;
                    Rotation[1] = ry ? atof(ry) : 0.0f;
                    Rotation[2] = rz ? atof(rz) : 0.0f;
                    Rotation[3] = rw ? atof(rw) : 0.0f;
                }
                else
                    handler->PSendSysMessage("Missing gameobject template for ID %u", id);
            }
            return true;
        }
        bool Execute(Player* player, Unit* target, float x, float y, float z, float o)
        {
            switch (OrientationOption)
            {
                case OrientationOptions::None:
                    o = OrientationDegrees / 180.0f * M_PI;
                    break;
                case OrientationOptions::FaceCaster:
                    o = Position::NormalizeOrientation(target->GetAngle(x, y) + M_PI);
                    break;
                case OrientationOptions::MatchCaster:
                    o = target->GetOrientation();
                    break;
            }

            GameObject* go = new GameObject();
            if (go->Create(player->GetMap()->GenerateLowGuid<HighGuid::GameObject>(), ID, player->GetMap(), player->GetPhaseMask() == PHASEMASK_ANYWHERE ? PHASEMASK_NORMAL : player->GetPhaseMask(), x, y, z, o, { Rotation[0], Rotation[1], Rotation[2], Rotation[3] }, 255, GO_STATE_READY))
            {
                player->GetMap()->AddToMap(go);
                if (SaveToDB)
                    go->SaveToDB(go->GetMapId(), 1 << go->GetMap()->GetSpawnMode(), go->GetPhaseMask());

                ChatHandler(player).PSendSysMessage("Spawned GUID: |cFFFFFFFF%u|r ID: |cFFFFFFFF%u|r at |cFFFFCCCC%f|rf, |cFFCCFFCC%f|rf, |cFFCCCCFF%f|rf, |cFFCCCCCC%f|rf",
                    go->GetDBTableGUIDLow(),
                    go->GetEntry(),
                    go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation());
                return true;
            }
            else
            {
                delete go;
                return false;
            }
        }
    } SpawnGO;
    struct
    {
        float OffsetZ;

        char const* GetName() const { return "LOS"; }
        char const* GetParams() const { return "[OffsetZ:float = 2.0f]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            OffsetZ = *args ? (float)atof(args) : 2.0f;
            return true;
        }
        bool Execute(Player* player, Unit* target, float x, float y, float z, float o)
        {
            target->m_positionZ += -2.0f + OffsetZ;
            bool los = target->IsWithinLOS(x, y, z - 2.0f + OffsetZ);
            target->m_positionZ -= -2.0f + OffsetZ;
            ChatHandler(player).SendSysMessage(los ? "|cFF00FF00Is in LOS|r" : "|cFFFF0000Is not in LOS|r");
            return true;
        }
    } LOS;
    struct
    {
        bool Continuous;
        bool UseStraightPath;
        bool ForceDest;
        bool StraightLine;

        char const* GetName() const { return "MMaps"; }
        char const* GetParams() const { return "[Continuous:bool] [UseStraightPath:bool] [ForceDest:bool] [StraightLine:bool]"; }
        bool Setup(ChatHandler* handler, char const* args)
        {
            ReportSelectedTool(handler, GetName(), GetParams());

            char* continuous = strtok((char*)args, " ");
            char* useStraightPath = strtok(nullptr, " ");
            char* forceDest = strtok(nullptr, " ");
            char* straightLine = strtok(nullptr, " ");

            Continuous      = continuous && (*continuous == '1' || *continuous == 't');
            UseStraightPath = useStraightPath && (*useStraightPath == '1' || *useStraightPath == 't');
            ForceDest       = forceDest && (*forceDest == '1' || *forceDest == 't');
            StraightLine    = straightLine && (*straightLine == '1' || *straightLine == 't');
            return true;
        }
        bool Execute(Player* player, Unit* target, float x, float y, float z, float o)
        {
            ObjectGuid playerGUID = player->GetGUID();
            static std::set<uint64> continousGUIDs;
            std::set<uint64>* guids = &continousGUIDs;
#ifdef ICORE_BUILD
            static const uint32 continuousInterval = 600;
#else
            static const uint32 continuousInterval = 100;
#endif

            bool continuous = Continuous;
            bool useStraightPath = UseStraightPath;
            bool forceDest = ForceDest;
            bool straightLine = StraightLine;
            auto func = [guids, playerGUID, target, x, y, z, continuous, useStraightPath, forceDest, straightLine]()
            {
                Player* player = ObjectAccessor::GetPlayer(*target, playerGUID);
                if (!player || (continuous && guids->find(target->GetGUID()) == guids->end()))
                    return true;
                // path
                PathGenerator path(target);
                path.SetUseStraightPath(useStraightPath);
                bool result = path.CalculatePath(x, y, z, forceDest, straightLine);

                auto&& pointPath = path.GetPath();
                if (!continuous)
                    ChatHandler(player).PSendSysMessage("%s's %s to |cFFFFCCCC%.3f|rf, |cFFCCFFCC%.3f|rf, |cFFCCCCFF%.3f|rf: %s - Length: %lu - Type: %u", target->GetName().c_str(), useStraightPath ? "StraightPath" : "SmoothPath", x, y, z, (result ? "|cFF00FF00true|r" : "|cFFFF0000false|r"), pointPath.size(), path.GetPathType());

                path.VisualizePath(continuous ? continuousInterval * 3 : 2500);
                if (continuous && path.visualizePathWaypointGUIDs)
                    for (auto&& guid : *path.visualizePathWaypointGUIDs)
                        if (guid)
                            if (Creature* waypoint = ObjectAccessor::GetCreature(*target, guid))
                                waypoint->m_Events.Schedule(continuousInterval + sWorld->GetUpdateTime(), [waypoint]()
                                {
                                    waypoint->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL, 0);
                                    waypoint->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
                                    waypoint->SetDisplayId(11686);
                                });
                return false;
            };

            if (continuous)
            {
                if (continousGUIDs.insert(target->GetGUID()).second)
                    target->m_Events.Repeated(0, continuousInterval, func);
                else
                    continousGUIDs.erase(target->GetGUID());
            }
            else
                func();
            return true;
        }
    } MMaps;
};

#endif
