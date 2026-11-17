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

#include "ScriptPCH.h"
#include "AccountMgr.h"
#include "firelands.h"

// areatrigger
// 6929 - quest1
// 6861 - near miniboss 1

#define MAX_ENCOUNTER 7

static std::vector<DoorData> const doordata = 
{
    {GO_BRIDGE_OF_RHYOLITH,  DATA_RHYOLITH,  DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {GO_FIRE_WALL_BALEROC,   DATA_BALEROC,   DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {GO_RAID_BRIDGE_FORMING, DATA_BALEROC,   DOOR_TYPE_PASSAGE,     BOUNDARY_NONE},
    {GO_STICKY_WEB,          DATA_BETHTILAC, DOOR_TYPE_ROOM,        BOUNDARY_NONE},
    {GO_BRIDGE_OF_RHYOLITH,  DATA_RHYOLITH,  DOOR_TYPE_SPAWN_HOLE,  BOUNDARY_NONE},
    {GO_FIRE_WALL_FANDRAL_1, DATA_STAGHELM,  DOOR_TYPE_PASSAGE,     BOUNDARY_NONE},
    {GO_FIRE_WALL_FANDRAL_2, DATA_STAGHELM,  DOOR_TYPE_PASSAGE,     BOUNDARY_NONE},
    {GO_SULFURON_KEEP,       DATA_RAGNAROS,  DOOR_TYPE_ROOM,        BOUNDARY_NONE},
};

class instance_firelands : public InstanceMapScript
{
    public:
        instance_firelands() : InstanceMapScript("instance_firelands", 720) { }

        struct instance_firelands_InstanceMapScript : public InstanceScript
        {
            instance_firelands_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doordata);
                uiShannoxGUID = ObjectGuid::Empty;
                uiRiplimbGUID = ObjectGuid::Empty;
                uiRagefaceGUID = ObjectGuid::Empty;
                uiBalerocGUID = ObjectGuid::Empty;
                uiRhyolithGUID = ObjectGuid::Empty;
                uiRagnarosGUID = ObjectGuid::Empty;
                uiFirewallBalerockGUID = ObjectGuid::Empty;
                uiSulfuronBridgeGUID = ObjectGuid::Empty;
                uiRhyolithHealth = 0;
                uiRagnarosFloor = ObjectGuid::Empty;
                uiRagnarosCache10 = ObjectGuid::Empty;
                uiRagnarosCache25 = ObjectGuid::Empty;
                uiTimer = 0;
                uiRitualCompleted = 0;
                bRitualAchievementFailed = false;
                bEvent = false;
                memset(uiRitualGUIDs, 0, sizeof(uiRitualGUIDs));
                creaturePortals.clear();
                gameobjectPortals.clear();

                instance->SetWorldState(WORLDSTATE_ONLY_THE_PENITENT, 1); // by default
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!uiTeamInInstance)
                    uiTeamInInstance = player->GetTeam();

                player->SendUpdateWorldState(WORLD_STATE_RITUAL_ACHIEVEMENT, !bRitualAchievementFailed);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SHANNOX:
                        uiShannoxGUID = creature->GetGUID();
                        break;
                    case NPC_RIPLIMB:
                        uiRiplimbGUID = creature->GetGUID();
                        break;
                    case NPC_RAGEFACE:
                        uiRagefaceGUID = creature->GetGUID();
                        break;
                    case NPC_BALEROC:
                        uiBalerocGUID = creature->GetGUID();
                        creature->SetPhaseMask((GetBossState(DATA_SHANNOX)==DONE) && (GetBossState(DATA_RHYOLITH)==DONE) && (GetBossState(DATA_BETHTILAC)==DONE) && (GetBossState(DATA_ALYSRAZOR)==DONE) ? 1 : 2, true);
                        break;
                    case NPC_CIRCLE_OF_THRONES_PORTAL:
                        creaturePortals.push_back(creature);
                        if (uiEvent == DONE)
                        {
                            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                        }
                        break;
                    case NPC_RHYOLITH:
                        uiRhyolithGUID = creature->GetGUID();
                        break;
                    case NPC_RAGNAROS:
                        uiRagnarosGUID = creature->GetGUID();
                        break;
                    case NPC_FIRELANDS_EVENT_BUNNY:
                        firelandsEventBunnyGUID = creature->GetGUID();
                        break;
                    case NPC_DRUID_OF_THE_FLAME:
                        if (creature->isDead() && GetBossState(DATA_BALEROC) == DONE && GetBossState(DATA_STAGHELM) != DONE)
                            creature->Respawn(true);
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_FIRE_WALL_BALEROC:
                        uiFirewallBalerockGUID = go->GetGUID();
                        HandleGameObject(ObjectGuid::Empty, (GetBossState(DATA_SHANNOX)==DONE) && (GetBossState(DATA_RHYOLITH)==DONE) && (GetBossState(DATA_BETHTILAC)==DONE) && (GetBossState(DATA_ALYSRAZOR)==DONE), go);
                        break;
                    case GO_STICKY_WEB:
                    case GO_RAID_BRIDGE_FORMING:
                    case GO_BRIDGE_OF_RHYOLITH:
                    case GO_FIRE_WALL_FANDRAL_1:
                    case GO_FIRE_WALL_FANDRAL_2:
                    case GO_SULFURON_KEEP:
                        AddDoor(go, true);
                        break;
                    case GO_SULFURON_BRIDGE:
                        uiSulfuronBridgeGUID = go->GetGUID();
                        if (GetBossState(DATA_BALEROC)==DONE)
                            go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        break;
                    case GO_RAGNAROS_FLOOR:
                        uiRagnarosFloor = go->GetGUID();
                        break;
                    case GO_CIRCLE_OF_THORNS_PORTAL3:
                        gameobjectPortals.push_back(go);
                        if (uiEvent == DONE)
                            HandleGameObject(go->GetGUID(), true, go);
                        break;
                    case GO_CACHE_OF_THE_FIRELORD_10:
                        uiRagnarosCache10 = go->GetGUID();
                        break;
                    case GO_CACHE_OF_THE_FIRELORD_25:
                        uiRagnarosCache25 = go->GetGUID();
                        break;
                    case GO_RITUAL_OF_FLAMES_1:
                    case GO_RITUAL_OF_FLAMES_2:
                        uiRitualGUIDs[go->GetEntry() == GO_RITUAL_OF_FLAMES_1 ? 0 : 1] = go->GetGUID();
                        if (GetBossState(DATA_STAGHELM) == DONE)
                            go->SetRespawnTime(WEEK);
                        break;
                    case GO_VOLCANO:
                        volcanoGUID = go->GetGUID();
                        go->SetDestructibleState(GO_DESTRUCTIBLE_INTACT);
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_RHYOLITH_HEALTH_SHARED:
                        uiRhyolithHealth = data;
                        break;
                    case DATA_EVENT:
                        uiEvent = data;
                        if (uiEvent == DONE)
                        {
                            if (!gameobjectPortals.empty())
                            {
                                for (std::list<GameObject*>::const_iterator itr = gameobjectPortals.begin(); itr != gameobjectPortals.end(); ++itr)
                                    if (GameObject* go = (*itr)->ToGameObject())
                                        HandleGameObject(go->GetGUID(), true, go);
                            }

                            if (!creaturePortals.empty())
                            {
                                for (std::list<Creature*>::const_iterator itr = creaturePortals.begin(); itr != creaturePortals.end(); ++itr)
                                    if (Creature* creature = (*itr)->ToCreature())
                                    {
                                        creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                        creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                                    }
                            }

                            SaveToDB();
                        }
                        break;
                    case DATA_RITUAL_FAILED:
                        bRitualAchievementFailed = true;
                        DoUpdateWorldState(WORLD_STATE_RITUAL_ACHIEVEMENT, 0);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_RHYOLITH_HEALTH_SHARED)
                    return uiRhyolithHealth;
                else if (type == DATA_EVENT)
                    return uiEvent;
                else if (type == DATA_BALEROC)
                    return GetBossState(DATA_BALEROC);
                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SHANNOX: return uiShannoxGUID;
                    case DATA_RIPLIMB: return uiRiplimbGUID;
                    case DATA_RAGEFACE: return uiRagefaceGUID;
                    case DATA_RHYOLITH: return uiRhyolithGUID;
                    case DATA_RAGNAROS: return uiRagnarosGUID;
                    case DATA_RAGNAROS_FLOOR: return uiRagnarosFloor;
                    case DATA_RAGNAROS_CACHE_10: return uiRagnarosCache10;
                    case DATA_RAGNAROS_CACHE_25: return uiRagnarosCache25;
                    case DATA_VOLCANO: return volcanoGUID;
                    case DATA_FIRELANDS_EVENT_BUNNY: return firelandsEventBunnyGUID;
                    default: return ObjectGuid::Empty;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                bool balerocAvailable = (GetBossState(DATA_SHANNOX)==DONE) && (GetBossState(DATA_RHYOLITH)==DONE) && (GetBossState(DATA_BETHTILAC)==DONE) && (GetBossState(DATA_ALYSRAZOR)==DONE);

                switch (type)
                {
                    case DATA_BALEROC:
                        if (state == DONE)
                            if (GameObject* obj = instance->GetGameObject(uiSulfuronBridgeGUID))
                                    obj->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        if (state == IN_PROGRESS)
                            HandleGameObject(uiFirewallBalerockGUID, false);
                        else
                            HandleGameObject(uiFirewallBalerockGUID, balerocAvailable);
                        break;
                    case DATA_SHANNOX:
                    case DATA_RHYOLITH:
                    case DATA_BETHTILAC:
                    case DATA_ALYSRAZOR:
                        if (uiFirewallBalerockGUID)
                            HandleGameObject(uiFirewallBalerockGUID, balerocAvailable);
                        if (balerocAvailable)
                            if (Creature* baleroc = instance->GetCreature(uiBalerocGUID))
                                baleroc->SetPhaseMask(1, true);
                        break;
                    case DATA_STAGHELM:
                        if (state == DONE)
                            for (uint8 i = 0; i < 2; ++i)
                                if (GameObject* go = instance->GetGameObject(uiRitualGUIDs[i]))
                                    go->SetRespawnTime(WEEK);
                        break;
                }

                return true;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const override
            {
                if (_SkipCheckRequiredBosses(player))
                    return true;

                switch (bossId)
                {
                    case DATA_RAGNAROS:
                        if (GetBossState(DATA_STAGHELM) != DONE)
                            return false;
                        break;
                    case DATA_STAGHELM:
                        if (GetBossState(DATA_BALEROC) != DONE)
                            return false;
                        break;
                    case DATA_BALEROC:
                        if (GetBossState(DATA_SHANNOX) != DONE)
                            return false;
                        if (GetBossState(DATA_ALYSRAZOR) != DONE)
                            return false;
                        if (GetBossState(DATA_BETHTILAC) != DONE)
                            return false;
                        if (GetBossState(DATA_RHYOLITH) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            void ProcessEvent(WorldObject* /*source*/, uint32 eventId) override
            {
                switch (eventId)
                {
                    case EVENT_PORTALS:
                        if ((uiEvent == DONE) || bEvent)
                            return;
                        bEvent = true;
                        uiTimer = 7000;
                        if (!creaturePortals.empty())
                            for (std::list<Creature*>::const_iterator itr = creaturePortals.begin(); itr != creaturePortals.end(); ++itr)
                                if (Creature* creature = (*itr)->ToCreature())
                                    creature->CastSpell(creature, SPELL_LEGENDARY_PORTAL_OPENING);
                        break;
                    case EVENT_RITUAL_OF_FLAMES_1:
                    case EVENT_RITUAL_OF_FLAMES_2:
                        if (GameObject* go = instance->GetGameObject(uiRitualGUIDs[eventId == EVENT_RITUAL_OF_FLAMES_1 ? 0 : 1]))
                        {
                            if (go->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE))
                                return;

                            go->SetGoState(GO_STATE_READY);
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                            if (!uiRitualCompleted)
                                uiRitualStartedTime = getMSTime();

                            if (++uiRitualCompleted >= 2 && !bRitualAchievementFailed && getMSTime() - uiRitualStartedTime <= 10000)
                                if (Creature* staghelm = go->FindNearestCreature(NPC_STAGHELM, 100))
                                    staghelm->CastSpell(staghelm, SPELL_RITUAL_ACHIEVEMENT, true);
                        }
                        break;
                }
            }

            void Update(uint32 diff) override
            {
                if (bEvent)
                {
                    if (uiTimer <= diff)
                    {
                        bEvent = false;
                        SetData(DATA_EVENT, DONE);
                    }
                    else
                        uiTimer -= diff;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::string str_data;

                std::ostringstream saveStream;
                saveStream << "F L " << GetBossSaveData() << uiEvent << ' ';

                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'F' && dataHead2 == 'L')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 tempEvent = 0;
                    loadStream >> tempEvent;
                    uiEvent = (tempEvent != DONE ? 0 : DONE);

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            private:
                uint32 uiTeamInInstance;
                uint32 uiRhyolithHealth;
                uint32 uiEvent;
                uint32 uiTimer;
                uint32 uiRitualCompleted;
                bool bRitualAchievementFailed;
                bool bEvent;
                ObjectGuid uiShannoxGUID;
                ObjectGuid uiRiplimbGUID;
                ObjectGuid uiRagefaceGUID;
                ObjectGuid uiBalerocGUID;
                ObjectGuid uiRagnarosGUID;
                ObjectGuid uiFirewallBalerockGUID;
                ObjectGuid uiSulfuronBridgeGUID;
                ObjectGuid uiRhyolithGUID;
                ObjectGuid uiRagnarosFloor;
                ObjectGuid uiRagnarosCache10;
                ObjectGuid uiRagnarosCache25;
                ObjectGuid volcanoGUID;
                ObjectGuid firelandsEventBunnyGUID;
                ObjectGuid uiRitualGUIDs[2];
                uint32 uiRitualStartedTime;
                std::list<GameObject*> gameobjectPortals;
                std::list<Creature*> creaturePortals;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_firelands_InstanceMapScript(map);
        }
};

void AddSC_instance_firelands()
{
    new instance_firelands();
}
