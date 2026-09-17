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

/* ScriptData
SDName: Instance_Karazhan
SD%Complete: 70
SDComment: Instance Script for Karazhan to help in various encounters. @todo GameObject visibility for Opera event.
SDCategory: Karazhan
EndScriptData */

#include "ScriptPCH.h"
#include "karazhan.h"

#define MAX_ENCOUNTER      12

/*
0  - Attumen + midnightGUID (optional)
1  - Moroes
2  - Maiden of Virtue (optional)
3  - Hyakiss the Lurker /  Rokad the Ravager  / Shadikith the Glider
4  - Opera Event
5  - Curator
6  - Shade of Aran (optional)
7  - terestian Illhoof (optional)
8  - Netherspite (optional)
9  - Chess Event
10 - Prince Malchezzar
11 - Nightbane
*/

const Position OptionalSpawn[] =
{
    { -10960.981445f, -1940.138428f, 46.178097f, 4.12f  }, // Hyakiss the Lurker
    { -10899.903320f, -2085.573730f, 49.474449f, 1.38f  }, // Rokad the Ravager
    { -10945.769531f, -2040.153320f, 49.474438f, 0.077f } // Shadikith the Glider
};

class instance_karazhan : public InstanceMapScript
{
    public:
        instance_karazhan() : InstanceMapScript("instance_karazhan", 532) { }

        struct instance_karazhan_InstanceMapScript : public InstanceScript, public karazhan_accessor
        {
            instance_karazhan_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_ENCOUNTER];

            uint32 m_uiOperaEvent;
            uint32 m_uiOzDeathCount;
            uint32 OptionalBossCount;
            uint32 teamInInstance = 0; // Team of first entered player, used for the Chess event
            uint32 chessResetTimer = 1;
            uint32 chessKickTimer = 0;

            uint8 allianceStalkerCount = 0;
            uint8 hordeStalkerCount = 0;

            bool friendlyGame = false;

            ObjectGuid hordeStatusGuid = ObjectGuid::Empty;
            ObjectGuid allianceStatusGuid = ObjectGuid::Empty;

            std::list<ObjectGuid> chessHordeStalkerList;
            std::list<ObjectGuid> chessAllianceStalkerList;
            std::list<ObjectGuid> chessPiecesAlliance;
            std::list<ObjectGuid> chessPiecesHorde;
            std::list<ObjectGuid> boardSquares;
            std::vector<ObjectGuid> hordeStalkers;
            std::vector<ObjectGuid> allianceStalkers;

            ObjectGuid echoOfMedivhGUID = ObjectGuid::Empty;
            ObjectGuid chessVictoryControllerGUID = ObjectGuid::Empty;

            ObjectGuid m_uiCurtainGUID;
            ObjectGuid m_uiStageDoorLeftGUID;
            ObjectGuid m_uiStageDoorRightGUID;
            ObjectGuid m_uiKilrekGUID;
            ObjectGuid m_uiTerestianGUID;
            ObjectGuid m_uiMoroesGUID;
            ObjectGuid m_uiTenrisGUID = ObjectGuid::Empty;
            ObjectGuid m_uiLibraryDoor;                    // Door at Shade of Aran
            ObjectGuid m_uiMassiveDoor;                    // Door at Netherspite
            ObjectGuid m_uiSideEntranceDoor;               // Side Entrance
            ObjectGuid m_uiGamesmansDoor;                  // Door before Chess
            ObjectGuid m_uiGamesmansExitDoor;              // Door after Chess
            ObjectGuid m_uiNetherspaceDoor;                // Door at Malchezaar
            ObjectGuid MastersTerraceDoor[2];
            ObjectGuid ImageGUID;
            ObjectGuid DustCoveredChest;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                // 1 - OZ, 2 - HOOD, 3 - RAJ, this never gets altered.
                m_uiOperaEvent          = urand(1,3);
                m_uiOzDeathCount        = 0;
                OptionalBossCount       = 0;

                m_uiCurtainGUID = ObjectGuid::Empty;
                m_uiStageDoorLeftGUID = ObjectGuid::Empty;
                m_uiStageDoorRightGUID = ObjectGuid::Empty;

                m_uiKilrekGUID = ObjectGuid::Empty;
                m_uiTerestianGUID = ObjectGuid::Empty;
                m_uiMoroesGUID = ObjectGuid::Empty;

                m_uiLibraryDoor         = ObjectGuid::Empty;
                m_uiMassiveDoor         = ObjectGuid::Empty;
                m_uiSideEntranceDoor    = ObjectGuid::Empty;
                m_uiGamesmansDoor       = ObjectGuid::Empty;
                m_uiGamesmansExitDoor   = ObjectGuid::Empty;
                m_uiNetherspaceDoor     = ObjectGuid::Empty;
                MastersTerraceDoor[0]   = ObjectGuid::Empty;
                MastersTerraceDoor[1]   = ObjectGuid::Empty;
                ImageGUID               = ObjectGuid::Empty;
                DustCoveredChest        = ObjectGuid::Empty;
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        return true;

                return false;
            }

            void OnPlayerEnter(Player* player) override
            {
                // very first player to enter
                if (!teamInInstance)
                    teamInInstance = player->GetTeam();

                player->RemoveAurasDueToSpell(30019); // Control Piece
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case 17229: m_uiKilrekGUID    = creature->GetGUID(); break;
                    case 15688: m_uiTerestianGUID = creature->GetGUID(); break;
                    case 15687: m_uiMoroesGUID    = creature->GetGUID(); break;
                    case 28194: m_uiTenrisGUID    = creature->GetGUID(); break;
                    case NPC_INVISIBLE_STALKER:
                        if (creature->GetPositionY() < -1870.0f)
                            chessHordeStalkerList.push_back(creature->GetGUID());
                        else
                            chessAllianceStalkerList.push_back(creature->GetGUID());
                        break;
                    case NPC_CHESS_STATUS_BAR:
                        if (creature->GetPositionY() < -1870.0f)
                            hordeStatusGuid = creature->GetGUID();
                        else
                            allianceStatusGuid = creature->GetGUID();
                        break;
                    case NPC_HUMAN_CHARGER:
                    case NPC_HUMAN_CLERIC:
                    case NPC_HUMAN_CONJURER:
                    case NPC_HUMAN_FOOTMAN:
                    case NPC_CONJURED_WATER_ELEMENTAL:
                    case NPC_KING_LLANE:
                        chessPiecesAlliance.push_back(creature->GetGUID());
                        break;
                    case NPC_ORC_GRUNT:
                    case NPC_ORC_NECROLYTE:
                    case NPC_ORC_WARLOCK:
                    case NPC_ORC_WOLF:
                    case NPC_SUMMONED_DAEMON:
                    case NPC_WARCHIEF_BLACKHAND:
                        chessPiecesHorde.push_back(creature->GetGUID());
                        break;
                    case NPC_ECHO_MEDIVH:
                        echoOfMedivhGUID = creature->GetGUID();
                        break;
                    case NPC_CHESS_VICTORY_CONTROLLER:
                        chessVictoryControllerGUID = creature->GetGUID();
                        break;
                    case NPC_SQUARE_WHITE:
                    case NPC_SQUARE_BLACK:
                        creature->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 0.0f);
                        boardSquares.push_back(creature->GetGUID());
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                switch (creature->GetEntry())
                {
                    case NPC_COLDMIST_WIDOW:
                    case NPC_COLDMIST_STALKER:
                    case NPC_SHADOWBAT:
                    case NPC_VAMPIRIC_SHADOWBAT:
                    case NPC_GREATER_SHADOWBAT:
                    case NPC_PHASE_HOUND:
                    case NPC_DREADBEAST:
                    case NPC_SHADOWBEAST:
                    {
                        ++OptionalBossCount;
                        if (OptionalBossCount >= 44 &&
                            !unit->FindNearestCreature(NPC_COLDMIST_WIDOW, 500.0f) &&
                            !unit->FindNearestCreature(NPC_COLDMIST_STALKER, 500.0f) &&
                            !unit->FindNearestCreature(NPC_SHADOWBAT, 500.0f) &&
                            !unit->FindNearestCreature(NPC_VAMPIRIC_SHADOWBAT, 500.0f) &&
                            !unit->FindNearestCreature(NPC_GREATER_SHADOWBAT, 500.0f) &&
                            !unit->FindNearestCreature(NPC_PHASE_HOUND, 500.0f) &&
                            !unit->FindNearestCreature(NPC_DREADBEAST, 500.0f) &&
                            !unit->FindNearestCreature(NPC_SHADOWBEAST, 500.0f))
                        {
                            OptionalBossCount = 0;
                            switch (urand(0, 2))
                            {
                                case 0:
                                    instance->SummonCreature(NPC_HYAKISS_THE_LURKER, OptionalSpawn[0]);
                                    break;
                                case 1:
                                    instance->SummonCreature(NPC_ROKAD_THE_RAVAGER, OptionalSpawn[1]);
                                    break;
                                case 2:
                                    instance->SummonCreature(NPC_SHADIKITH_THE_GLIDER, OptionalSpawn[2]);
                                    break;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_ATTUMEN:              m_auiEncounter[0] = data; break;
                    case TYPE_MOROES:
                        if (m_auiEncounter[1] == DONE)
                            break;
                        m_auiEncounter[1] = data;
                        break;
                    case TYPE_MAIDEN:               m_auiEncounter[2] = data; break;
                    case TYPE_OPTIONAL_BOSS:        m_auiEncounter[3] = data; break;
                    case TYPE_OPERA:
                        if (data == DONE && m_auiEncounter[4] != DONE)
                            UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, 16812, nullptr);
                        else if (data == FAIL)
                            m_uiOzDeathCount = 0;
                        m_auiEncounter[4] = data;
                        break;
                    case TYPE_CURATOR:              m_auiEncounter[5] = data; break;
                    case TYPE_ARAN:                 m_auiEncounter[6] = data; break;
                    case TYPE_TERESTIAN:            m_auiEncounter[7] = data; break;
                    case TYPE_NETHERSPITE:          m_auiEncounter[8] = data; break;
                    case TYPE_CHESS:
                        if (data == DONE)
                        {
                            if (GetData(TYPE_CHESS) != SPECIAL)
                            {
                                HandleGameObject(m_uiGamesmansExitDoor, true);
                                DoRespawnGameObject(DustCoveredChest, DAY);

                                UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_CHESS_STATUS_BAR, nullptr);
                            }

                            // cast game end spells
                            if (Creature* medivh = instance->GetCreature(echoOfMedivhGUID))
                            {
                                medivh->CastSpell(medivh, SPELL_FORCE_KILL_BUNNY, true);
                                medivh->CastSpell(medivh, SPELL_GAME_OVER, true);
                                medivh->CastSpell(medivh, SPELL_CLEAR_BOARD, true);
                            }
                            if (Creature* controller = instance->GetCreature(chessVictoryControllerGUID))
                                controller->CastSpell(controller, SPELL_VICTORY_VISUAL_CHESS, true);

                            friendlyGame = false;
                            chessResetTimer = GetData(TYPE_CHESS) != SPECIAL ? 35000 : 5000;
                            chessKickTimer = chessResetTimer + 1 - 2500;
                        }
                        else if (data == FAIL)
                        {
                            // clean the board for reset
                            if (Creature* medivh = instance->GetCreature(echoOfMedivhGUID))
                            {
                                medivh->CastSpell(medivh, SPELL_GAME_OVER, true);
                                medivh->CastSpell(medivh, SPELL_CLEAR_BOARD, true);
                            }

                            chessResetTimer = 5000;
                            chessKickTimer = chessResetTimer + 1 - 2500;
                        }

                        m_auiEncounter[9]  = data;
                        if (data == IN_PROGRESS || data == SPECIAL)
                            DoPrepareChessEvent();
                        break;
                    case TYPE_MALCHEZZAR:           m_auiEncounter[10] = data; break;
                    case TYPE_NIGHTBANE:
                        if (m_auiEncounter[11] != DONE)
                            m_auiEncounter[11] = data;
                        break;
                    case DATA_OPERA_OZ_DEATHCOUNT:
                        if (data == SPECIAL)
                            ++m_uiOzDeathCount;
                        else if (data == IN_PROGRESS)
                            m_uiOzDeathCount = 0;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_IMAGE_OF_MEDIVH: ImageGUID = data;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case 181032:
                        if (m_uiTenrisGUID)
                            if (Creature* tenris = instance->GetCreature(m_uiTenrisGUID))
                                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED);
                        break;
                    case 183932:   m_uiCurtainGUID          = go->GetGUID();         break;
                    case 184278:
                        m_uiStageDoorLeftGUID = go->GetGUID();
                        if (m_auiEncounter[4] == DONE)
                            go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case 184279:
                        m_uiStageDoorRightGUID = go->GetGUID();
                        if (m_auiEncounter[4] == DONE)
                            go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case 184517:   m_uiLibraryDoor          = go->GetGUID();         break;
                    case 185521:   m_uiMassiveDoor          = go->GetGUID();         break;
                    case 184276:   m_uiGamesmansDoor        = go->GetGUID();         break;
                    case 185134:   m_uiNetherspaceDoor      = go->GetGUID();         break;
                    case 184274:    MastersTerraceDoor[0] = go->GetGUID();  break;
                    case 184280:    MastersTerraceDoor[1] = go->GetGUID();  break;
                    case 184275:
                        m_uiSideEntranceDoor = go->GetGUID();
                        if (m_auiEncounter[4] == DONE)
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED);
                        else
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED);
                        break;
                    case GO_GAMESMANS_HALL_EXIT_DOOR:
                        m_uiGamesmansExitDoor = go->GetGUID();
                        if (GetData(TYPE_CHESS) == DONE || GetData(TYPE_CHESS) == SPECIAL)
                            HandleGameObject(m_uiGamesmansExitDoor, true, go);
                        break;
                    case GO_DUST_COVERED_CHEST:
                        DustCoveredChest = go->GetGUID();
                        break;
                }

                switch (m_uiOperaEvent)
                {
                    //TODO: Set Object visibilities for Opera based on performance
                    case EVENT_OZ:
                        break;

                    case EVENT_HOOD:
                        break;

                    case EVENT_RAJ:
                        break;
                }
            }

            uint32 GetData(uint32 data) const override
            {
                switch (data)
                {
                    case TYPE_ATTUMEN:              return m_auiEncounter[0];
                    case TYPE_MOROES:               return m_auiEncounter[1];
                    case TYPE_MAIDEN:               return m_auiEncounter[2];
                    case TYPE_OPTIONAL_BOSS:        return m_auiEncounter[3];
                    case TYPE_OPERA:                return m_auiEncounter[4];
                    case TYPE_CURATOR:              return m_auiEncounter[5];
                    case TYPE_ARAN:                 return m_auiEncounter[6];
                    case TYPE_TERESTIAN:            return m_auiEncounter[7];
                    case TYPE_NETHERSPITE:          return m_auiEncounter[8];
                    case TYPE_CHESS:                return m_auiEncounter[9];
                    case TYPE_MALCHEZZAR:           return m_auiEncounter[10];
                    case TYPE_NIGHTBANE:            return m_auiEncounter[11];
                    case DATA_OPERA_PERFORMANCE:    return m_uiOperaEvent;
                    case DATA_OPERA_OZ_DEATHCOUNT:  return m_uiOzDeathCount;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch (data)
                {
                    case DATA_KILREK:                   return m_uiKilrekGUID;
                    case DATA_TERESTIAN:                return m_uiTerestianGUID;
                    case DATA_MOROES:                   return m_uiMoroesGUID;
                    case DATA_GO_STAGEDOORLEFT:         return m_uiStageDoorLeftGUID;
                    case DATA_GO_STAGEDOORRIGHT:        return m_uiStageDoorRightGUID;
                    case DATA_GO_CURTAINS:              return m_uiCurtainGUID;
                    case DATA_GO_LIBRARY_DOOR:          return m_uiLibraryDoor;
                    case DATA_GO_MASSIVE_DOOR:          return m_uiMassiveDoor;
                    case DATA_GO_SIDE_ENTRANCE_DOOR:    return m_uiSideEntranceDoor;
                    case DATA_GO_GAME_DOOR:             return m_uiGamesmansDoor;
                    case DATA_GO_GAME_EXIT_DOOR:        return m_uiGamesmansExitDoor;
                    case DATA_GO_NETHER_DOOR:           return m_uiNetherspaceDoor;
                    case DATA_MASTERS_TERRACE_DOOR_1:   return MastersTerraceDoor[0];
                    case DATA_MASTERS_TERRACE_DOOR_2:   return MastersTerraceDoor[1];
                    case DATA_IMAGE_OF_MEDIVH:          return ImageGUID;
                    case NPC_ECHO_MEDIVH:               return echoOfMedivhGUID;
                    case NPC_CHESS_VICTORY_CONTROLLER:  return chessVictoryControllerGUID;
                }

                return ObjectGuid::Empty;
            }

            void DoPrepareChessEvent() override
            {
                for (auto&& guid : boardSquares)
                    if (Creature* square = instance->GetCreature(guid))
                        square->RemoveAllAuras();

                // Allow all the chess pieces to init start position
                for (auto&& guid : chessPiecesAlliance)
                {
                    if (Creature* chessPiece = instance->GetCreature(guid))
                    {
                        chessPiece->RemoveAllAuras();

                        Creature* square = GetClosestCreatureWithEntry(chessPiece, NPC_SQUARE_BLACK, 2.0f);
                        if (!square)
                            square = GetClosestCreatureWithEntry(chessPiece, NPC_SQUARE_WHITE, 2.0f);
                        if (!square)
                        {
                            TC_LOG_ERROR("scripts", "Instance Karazhan: ERROR Failed to properly load the Chess square for {}.", chessPiece->GetDBTableGUIDLow());
                            return;
                        }

                        // send event which will prepare the current square
                        chessPiece->AI()->SetGUID(square->GetGUID());
                    }
                }

                for (auto&& guid : chessPiecesHorde)
                {
                    if (Creature* chessPiece = instance->GetCreature(guid))
                    {
                        chessPiece->RemoveAllAuras();

                        Creature* square = GetClosestCreatureWithEntry(chessPiece, NPC_SQUARE_BLACK, 2.0f);
                        if (!square)
                            square = GetClosestCreatureWithEntry(chessPiece, NPC_SQUARE_WHITE, 2.0f);
                        if (!square)
                        {
                            TC_LOG_ERROR("scripts", "Instance Karazhan: ERROR Failed to properly load the Chess square for {}.", chessPiece->GetDBTableGUIDLow());
                            return;
                        }

                        // send event which will prepare the current square
                        chessPiece->AI()->SetGUID(square->GetGUID());
                    }
                }

                allianceStalkerCount = 0;
                hordeStalkerCount = 0;
                hordeStalkers.clear();
                allianceStalkers.clear();

                // sort stalkers depending on side
                std::list<Creature*> stalkers;
                for (auto&& guid : chessHordeStalkerList)
                    if (Creature* temp = instance->GetCreature(guid))
                        stalkers.push_back(temp);

                if (stalkers.empty())
                {
                    TC_LOG_ERROR("scripts", "Instance Karazhan: ERROR Failed to properly load the horde side stalkers for the Chess Event.");
                    return;
                }

                // get the proper statusBar npc
                Creature* status = instance->GetCreature(hordeStatusGuid);
                if (!status)
                    return;

                stalkers.sort(Trinity::ObjectDistanceOrderPred(status));
                for (auto&& stalker : stalkers)
                    hordeStalkers.push_back(stalker->GetGUID());

                stalkers.clear();
                for (auto&& guid : chessAllianceStalkerList)
                    if (Creature* temp = instance->GetCreature(guid))
                        stalkers.push_back(temp);

                if (stalkers.empty())
                {
                    TC_LOG_ERROR("scripts", "Instance Karazhan: ERROR Failed to properly load the alliance side stalkers for the Chess Event.");
                    return;
                }

                // get the proper statusBar npc
                status = instance->GetCreature(allianceStatusGuid);
                if (!status)
                    return;

                stalkers.sort(Trinity::ObjectDistanceOrderPred(status));
                for (auto&& stalker : stalkers)
                    allianceStalkers.push_back(stalker->GetGUID());
            }

            uint32 GetPlayerTeam() override
            {
                if (!teamInInstance)
                    if (!instance->GetPlayers().isEmpty())
                        if (Player* player = instance->GetPlayers().begin()->GetSource())
                            teamInInstance = player->GetTeam();

                return teamInInstance;
            }

            bool IsFriendlyGameReady() const override { return friendlyGame; }

            void DoMoveChessPieceToSides(uint32 spellId, uint32 faction) override
            {
                // assign proper faction variables
                auto&& stalkers = faction == FACTION_ID_CHESS_ALLIANCE ? allianceStalkers : hordeStalkers;
                uint32 count = faction == FACTION_ID_CHESS_ALLIANCE ? allianceStalkerCount : hordeStalkerCount;

                // get the proper statusBar npc
                Creature* status = instance->GetCreature(faction == FACTION_ID_CHESS_ALLIANCE ? allianceStatusGuid : hordeStatusGuid);
                if (!status)
                    return;

                if (stalkers.size() < count + 1)
                    return;

                // handle stalker transformation
                if (Creature* stalker = instance->GetCreature(stalkers[count]))
                {
                    // need to provide specific target, in order to ensure the logic of the event
                    status->CastSpell(stalker, spellId, true);
                    faction == FACTION_ID_CHESS_ALLIANCE ? ++allianceStalkerCount : ++hordeStalkerCount;
                }
            }

            void FinishChessEvent(uint32 faction) override
            {
                // handle emote on end game
                for (auto&& guid : (faction == FACTION_ID_CHESS_ALLIANCE ? hordeStalkers : allianceStalkers))
                    if (Creature* stalker = instance->GetCreature(guid))
                        stalker->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_APPLAUD);
            }

            std::list<ObjectGuid> const& GetChessPiecesByFaction(uint32 faction) const override { return faction == FACTION_ID_CHESS_ALLIANCE ? chessPiecesAlliance : chessPiecesHorde; }

            void Update(uint32 diff) override
            {
                if (chessKickTimer)
                {
                    if (chessKickTimer <= diff)
                    {
                        for (auto&& guid : chessPiecesAlliance)
                            if (Creature* temp = instance->GetCreature(guid))
                                temp->RemoveAurasByType(SPELL_AURA_MOD_CHARM);

                        for (auto&& guid : chessPiecesHorde)
                            if (Creature* temp = instance->GetCreature(guid))
                                temp->RemoveAurasByType(SPELL_AURA_MOD_CHARM);

                        chessKickTimer = 0;
                    }
                    else
                        chessKickTimer -= diff;
                }

                if (chessResetTimer)
                {
                    if (chessPiecesAlliance.size() != 16 || chessPiecesHorde.size() != 16 || chessAllianceStalkerList.size() != 16 || chessHordeStalkerList.size() != 16)
                        return;

                    // respawn all chess pieces and side stalkers on the original position
                    if (chessResetTimer <= diff)
                    {
                        for (auto&& guid : chessPiecesAlliance)
                        {
                            if (Creature* temp = instance->GetCreature(guid))
                            {
                                temp->StopMoving();
                                temp->GetMotionMaster()->Clear();
                                temp->GetMotionMaster()->MoveIdle();
                                temp->RemoveAllAuras();
                                temp->Respawn(true);
                            }
                        }

                        for (auto&& guid : chessPiecesHorde)
                        {
                            if (Creature* temp = instance->GetCreature(guid))
                            {
                                temp->StopMoving();
                                temp->GetMotionMaster()->Clear();
                                temp->GetMotionMaster()->MoveIdle();
                                temp->RemoveAllAuras();
                                temp->Respawn(true);
                            }
                        }

                        for (auto&& guid : chessAllianceStalkerList)
                        {
                            if (Creature* temp = instance->GetCreature(guid))
                            {
                                temp->RemoveAllAuras();
                                temp->Respawn(true);
                                temp->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                            }
                        }

                        for (auto&& guid : chessHordeStalkerList)
                        {
                            if (Creature* temp = instance->GetCreature(guid))
                            {
                                temp->RemoveAllAuras();
                                temp->Respawn(true);
                                temp->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                            }
                        }

                        if (Creature* controller = instance->GetCreature(chessVictoryControllerGUID))
                            controller->RemoveAurasDueToSpell(SPELL_VICTORY_VISUAL_CHESS);

                        if (GetData(TYPE_CHESS) == FAIL)
                            SetData(TYPE_CHESS, NOT_STARTED);
                        else if (GetData(TYPE_CHESS) == DONE)
                            friendlyGame = true;

                        chessResetTimer = 0;
                    }
                    else
                        chessResetTimer -= diff;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                    << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " "
                    << m_auiEncounter[7] << " " << m_auiEncounter[8] << " " << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11];

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* chrIn) override
            {
                if (!chrIn)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(chrIn);
                std::istringstream loadStream(chrIn);

                loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
                    >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
                    >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11];
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)                // Do not load an encounter as "In Progress" - reset it instead.
                        m_auiEncounter[i] = NOT_STARTED;

                if (m_auiEncounter[4] == FAIL)
                    m_auiEncounter[4] = NOT_STARTED;

                if (m_auiEncounter[9] == SPECIAL)
                    m_auiEncounter[9] = DONE;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_karazhan_InstanceMapScript(map);
        }
};

void AddSC_instance_karazhan()
{
    new instance_karazhan();
}
