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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "CreatureTextMgr.h"
#include "heart_of_fear.h"
#include "LFGMgr.h"

static std::vector<DoorData> const doorData =
{
    { GO_ANTECHAMBER_DOOR_ENTRANCE,   0,             DOOR_TYPE_ROOM,      BOUNDARY_S },
    { GO_ANTECHAMBER_DOOR_EXIT,       0,             DOOR_TYPE_ROOM,      BOUNDARY_E },
    { GO_ORATIUM_DOOR_ENTRANCE,       0,             DOOR_TYPE_ROOM,      BOUNDARY_W },
    { GO_QUARTERS_DOOR_ENTRANCE,      DATA_ZORLOK,   DOOR_TYPE_PASSAGE,   BOUNDARY_S },
    { GO_QUARTERS_DOOR_EXIT,          DATA_TAYAK,    DOOR_TYPE_PASSAGE,   BOUNDARY_W },
    { GO_STAIRWAYS_DOOR_EXIT,         0,             DOOR_TYPE_ROOM,      BOUNDARY_N },
    { GO_BALCONY_DOOR_EXIT,           DATA_MELJARAK, DOOR_TYPE_PASSAGE,   BOUNDARY_S },
    { GO_GARALON_DOOR_UPPER,          DATA_GARALON,  DOOR_TYPE_PASSAGE,   BOUNDARY_S },
    { GO_GARALON_DOOR,                DATA_GARALON,  DOOR_TYPE_ROOM,      BOUNDARY_S },
    { GO_ATRIUM_DOOR_ENTRANCE,        0,             DOOR_TYPE_ROOM,      BOUNDARY_N },
    { GO_ATRIUM_DOOR_EXIT,            0,             DOOR_TYPE_ROOM,      BOUNDARY_W },
    { GO_SANCTUM_DOOR_ENTRANCE,       DATA_UNSOK,    DOOR_TYPE_ROOM,      BOUNDARY_E },
    { GO_HEARTOFFEAR_DOOR_ENTRANCE,   DATA_UNSOK,    DOOR_TYPE_PASSAGE,   BOUNDARY_E },
};

class instance_heart_of_fear : public InstanceMapScript
{
    public: instance_heart_of_fear() : InstanceMapScript("instance_heart_of_fear", 1009) { }

        struct instance_heart_of_fear_InstanceMapScript : public InstanceScript
        {
            instance_heart_of_fear_InstanceMapScript(Map* map) : InstanceScript(map) { }
            uint32 m_auiEncounter[MAX_TYPES];

            void Initialize() override
            {
                SetBossNumber(MAX_TYPES);
                LoadDoorData(doorData);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                zorlokEntranceDoorGuid = ObjectGuid::Empty;
                tayakEntranceDoorGuid = ObjectGuid::Empty;
                tayakExitDoorGuid = ObjectGuid::Empty;
                garalonEntranceDoorGuid = ObjectGuid::Empty;
                meljarakExitDoorGuid = ObjectGuid::Empty;
                unsokEntranceDoorGuid = ObjectGuid::Empty;
                shekzeerEntranceDoorGuid = ObjectGuid::Empty;

                HasZorlokEventDone       = 0;
                HasTayakEventDone        = 0;
                HasGaralonEventDone      = 0;
                HasShekzeerEventDone     = 0;

                isLfr                    = instance->ToInstanceMap()->IsLFGMap();
                lfrSectionFound          = false;
                isSecondLfrQuarter       = false;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (isLfr && !lfrSectionFound)
                {
                    lfrSectionFound = true;
                    Group* group = player->GetGroup();
                    if (!group)
                        return;
                    if (group->isLFGGroup())
                        isSecondLfrQuarter = sLFGMgr->GetDungeon(group->GetGUID()) == 530;

                    if (isSecondLfrQuarter)
                    {
                        if (Creature* boss = instance->GetCreature(ZorlokGUID))
                            boss->DespawnOrUnsummon();
                        if (Creature* boss = instance->GetCreature(TayakGUID))
                            boss->DespawnOrUnsummon();
                        if (Creature* boss = instance->GetCreature(GaralonGUID))
                            boss->DespawnOrUnsummon();
                        SetBossState(DATA_ZORLOK, DONE);
                        SetBossState(DATA_TAYAK, DONE);
                        SetBossState(DATA_GARALON, DONE);
                    }
                }

                if (isSecondLfrQuarter)
                    player->NearTeleportTo(-2140.731f, 475.767f, 503.570f, 0.011911f);

                if (player->GetSession()->PlayerLoading() && player->HasAura(122740))
                    player->RemoveAurasDueToSpell(122740);

                // Determination
                if (isLfr && GetDeterminationValue())
                    if (Aura* detAura = player->AddAura(SPELL_DETERMINATION_LFR, player))
                        detAura->SetStackAmount(GetDeterminationValue());

                if (isLfr || GetEncountersDoneCount(MAX_TYPES) < 1 || player->GetSession()->PlayerLoading() || player->IsGameMaster())
                    return;

                Position pos = GetCurrentEntrancePosition();
                player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (isLfr && !lfrSectionFound)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            OnPlayerEnter(player);
                }

                if (isLfr)
                {
                    uint32 entry = creature->GetEntry();
                    if (isSecondLfrQuarter)
                    {
                        switch (entry)
                        {
                            case NPC_IMPERIAL_VIZIER_ZORLOK:
                            case NPC_BLADE_LORD_TAYAK:
                            case NPC_GARALON:
                            case NPC_ZETTHIK_ZEPHYRIAN:
                            case NPC_ZETTHIK_GUSTWIND:
                            case NPC_WIND_LORD_MELJARAK_INTRO:
                                creature->DespawnOrUnsummon();
                                break;
                            case NPC_SRATHIK_AMBER_TRAPPER:
                            case NPC_KORTHIK_ELITE_BLADEMASTER:
                            case NPC_ZARTHIK_BATTLE_MENDER:
                                // Despawn swarm only if it cause in pre event
                                if (creature->GetDBTableGUIDLow())
                                    creature->DespawnOrUnsummon();
                                break;
                        }
                    }
                    else
                    {
                        if (entry == NPC_WIND_LORD_MELJARAK || entry == NPC_AMBER_SHAPER_UNSOK || entry == NPC_GRAND_EMPRESS_SHEKZEER)
                        {
                            creature->DespawnOrUnsummon();
                            return;
                        }
                    }
                }

                switch (creature->GetEntry())
                {
                    case NPC_IMPERIAL_VIZIER_ZORLOK:
                        ZorlokGUID = creature->GetGUID();
                        break;
                    case NPC_BLADE_LORD_TAYAK:
                        TayakGUID = creature->GetGUID();
                        break;
                    case NPC_GARALON:
                        GaralonGUID = creature->GetGUID();
                        break;
                    case NPC_WIND_LORD_MELJARAK:
                        MeljarakGUID = creature->GetGUID();
                        break;
                    case NPC_AMBER_SHAPER_UNSOK:
                        UnsokGUID = creature->GetGUID();
                        break;
                    case NPC_GRAND_EMPRESS_SHEKZEER:
                        ShekzeerGUID = creature->GetGUID();
                        break;
                    case NPC_SPEARED_KLAXXI:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    case NPC_MANTID_SPEAR:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        if (Creature* SpearedKlaxxi = creature->SummonCreature(NPC_SPEARED_KLAXXI, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                            SpearedKlaxxi->CastSpell(creature, SPELL_RIDE_VEHICLE_HARDCODED, true);
                        break;
                    case NPC_KORTHIK_SPAWN_HEROIC:
                    case NPC_ZARTHIK_SPAWN_HEROIC:
                    case NPC_SRATHIK_SPAWN_HEROIC:
                        creature->SetDisplayId(44466); // invisible
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    // Generic doors
                    case GO_ANTECHAMBER_DOOR_ENTRANCE:
                    case GO_ANTECHAMBER_DOOR_EXIT:
                    case GO_ATRIUM_DOOR_ENTRANCE:
                    case GO_ATRIUM_DOOR_EXIT:
                    case GO_GARALON_DOOR:
                        AddDoor(go, true);
                        break;
                        // Specific doors
                    case GO_ORATIUM_DOOR_ENTRANCE:
                        AddDoor(go, true);
                        zorlokEntranceDoorGuid = go->GetGUID();
                        break;
                    case GO_QUARTERS_DOOR_ENTRANCE:
                        AddDoor(go, true);
                        tayakEntranceDoorGuid = go->GetGUID();
                        break;
                    case GO_QUARTERS_DOOR_EXIT:
                        AddDoor(go, true);
                        tayakExitDoorGuid = go->GetGUID();
                        break;
                    case GO_STAIRWAYS_DOOR_EXIT:
                        AddDoor(go, true);
                        garalonEntranceDoorGuid = go->GetGUID();
                        break;
                    case GO_BALCONY_DOOR_EXIT:
                        AddDoor(go, true);
                        meljarakExitDoorGuid = go->GetGUID();
                        break;
                    case GO_SANCTUM_DOOR_ENTRANCE:
                        AddDoor(go, true);
                        unsokEntranceDoorGuid = go->GetGUID();
                        break;
                    case GO_HEARTOFFEAR_DOOR_ENTRANCE:
                        AddDoor(go, true);
                        shekzeerEntranceDoorGuid = go->GetGUID();
                        break;
                    case GO_GARALON_DOOR_UPPER:
                        if (!isLfr)
                            AddDoor(go, true);
                        garalonExitDoorGuid = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                PromotionIfLFR(state);

                return true;
            }

            Position GetCurrentEntrancePosition() override
            {
                uint32 m_encounterDone = GetEncountersDoneCount(MAX_TYPES);

                switch (m_encounterDone) // begun enum from bossId where entrance was changed
                {
                    case DATA_TAYAK:
                        return EncountersEntrance[1];
                    case DATA_GARALON:
                        return EncountersEntrance[2];
                    case DATA_MELJARAK:
                        return EncountersEntrance[3];
                    case DATA_UNSOK:
                        return EncountersEntrance[4];
                }

                return EncountersEntrance[0];
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_ZORLOK:
                        HasZorlokEventDone = data;
                        break;
                    case DATA_TAYAK:
                        HasTayakEventDone = data;
                        break;
                    case DATA_GARALON:
                        HasGaralonEventDone = data;
                        break;
                    case DATA_SHEKZEER:
                        HasShekzeerEventDone = data;
                        break;
                }

                if (data == DONE || data == SPECIAL)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_ZORLOK:
                        return HasZorlokEventDone;
                    case DATA_TAYAK:
                        return HasTayakEventDone;
                    case DATA_GARALON:
                        return HasGaralonEventDone;
                    case DATA_SHEKZEER:
                        return HasShekzeerEventDone;
                    case DATA_LFR:
                        return isLfr;
                    case DATA_LFR_SECOND_QUARTER:
                        return isSecondLfrQuarter;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_ZORLOK:
                        return ZorlokGUID;
                    case DATA_TAYAK:
                        return TayakGUID;
                    case DATA_GARALON:
                        return GaralonGUID;
                    case DATA_MELJARAK:
                        return MeljarakGUID;
                    case DATA_UNSOK:
                        return UnsokGUID;
                    case DATA_SHEKZEER:
                        return ShekzeerGUID;
                        // --- GameObjects ---
                    case GO_ORATIUM_DOOR_ENTRANCE:
                        return zorlokEntranceDoorGuid;
                    case GO_QUARTERS_DOOR_ENTRANCE:
                        return tayakEntranceDoorGuid;
                    case GO_QUARTERS_DOOR_EXIT:
                        return tayakExitDoorGuid;
                    case GO_STAIRWAYS_DOOR_EXIT:
                        return garalonEntranceDoorGuid;
                    case GO_BALCONY_DOOR_EXIT:
                        return meljarakExitDoorGuid;
                    case GO_SANCTUM_DOOR_ENTRANCE:
                        return unsokEntranceDoorGuid;
                    case GO_HEARTOFFEAR_DOOR_ENTRANCE:
                        return shekzeerEntranceDoorGuid;
                    case GO_GARALON_DOOR_UPPER:
                        return garalonExitDoorGuid;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "H O F " << GetBossSaveData() << HasZorlokEventDone << ' ' << HasTayakEventDone << ' ' << HasGaralonEventDone << ' ' << HasShekzeerEventDone;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            bool isWipe()
            {
                Map::PlayerList const& PlayerList = instance->GetPlayers();

                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* plr = i->GetSource())
                            if (plr->IsAlive() && !plr->IsGameMaster())
                                return false;
                    }
                }
                return true;
            }

            void Load(char const* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2, dataHead3;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> dataHead3;

                if (dataHead1 == 'H' && dataHead2 == 'O' && dataHead3 == 'F')
                {
                    for (uint8 i = 0; i < MAX_TYPES; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp; // zorlok event complete
                    HasZorlokEventDone = temp;
                    SetData(DATA_ZORLOK, HasZorlokEventDone);
                    loadStream >> temp; // Tayak event complete
                    HasTayakEventDone = temp;
                    SetData(DATA_TAYAK, HasTayakEventDone);
                    loadStream >> temp; // Garalon event complete
                    HasGaralonEventDone = temp;
                    SetData(DATA_GARALON, HasGaralonEventDone);
                    loadStream >> temp; // Shekzeer event complete
                    HasShekzeerEventDone = temp;
                    SetData(DATA_SHEKZEER, HasShekzeerEventDone);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const override
            {
                if (!InstanceScript::CheckRequiredBosses(bossId, player))
                    return false;

                switch (bossId)
                {
                    case DATA_TAYAK:
                    case DATA_GARALON:
                    case DATA_MELJARAK:
                    case DATA_UNSOK:
                    case DATA_SHEKZEER:
                        if (GetBossState(bossId - 1) != DONE)
                            return false;
                    default:
                        break;
                }

                return true;
            }

            bool IsWipe(float range, Unit* source) override
            {
                Map::PlayerList const &playerList = instance->GetPlayers();

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    Player* player = itr->GetSource();
                    if (!player)
                        continue;

                    if (player->IsAlive() && !player->IsGameMaster())
                        return false;
                }

                return true;
            }

        protected:
            ObjectGuid ZorlokGUID;
            ObjectGuid TayakGUID;
            ObjectGuid GaralonGUID;
            ObjectGuid MeljarakGUID;
            ObjectGuid UnsokGUID;
            ObjectGuid ShekzeerGUID;

            // Special Doors GUIDs
            ObjectGuid zorlokEntranceDoorGuid;
            ObjectGuid tayakEntranceDoorGuid;
            ObjectGuid tayakExitDoorGuid;
            ObjectGuid garalonEntranceDoorGuid;
            ObjectGuid garalonExitDoorGuid;
            ObjectGuid meljarakExitDoorGuid;
            ObjectGuid unsokEntranceDoorGuid;
            ObjectGuid shekzeerEntranceDoorGuid;

            // Post Events Save Data
            uint32 HasZorlokEventDone;
            uint32 HasTayakEventDone;
            uint32 HasGaralonEventDone;
            uint32 HasShekzeerEventDone;

            bool isLfr;
            bool lfrSectionFound;
            bool isSecondLfrQuarter;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_heart_of_fear_InstanceMapScript(map);
        }
};

void AddSC_instance_heart_of_fear()
{
    new instance_heart_of_fear();
}
