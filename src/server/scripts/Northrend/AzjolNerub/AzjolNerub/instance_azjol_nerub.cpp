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
#include "azjol_nerub.h"

#define MAX_ENCOUNTER     3

/* Azjol Nerub encounters:
 0 - Krik'thir the Gatewatcher
 1 - Hadronox
 2 - Anub'arak
*/

enum Events
{
    EVENT_RESET_HADRONOX = 1,
    EVENT_SPAWN_HADRONOX_CRUSHERS,
    EVENT_SPAWN_HADRONOX_TRASH,
    EVENT_ANUBARAK_DOORS,
};

Position const hadronoxPos = { 518, 544, 674, 5 };
Position const crusherPos = { 529, 547, 732, 4 };

Position const crusherAddsPos[2] =
{
    { 539, 549, 732, 4 },
    { 520, 548, 732, 5 },
};

class instance_azjol_nerub : public InstanceMapScript
{
    public:
        instance_azjol_nerub() : InstanceMapScript("instance_azjol_nerub", 601) { }

        struct instance_azjol_nerub_InstanceScript : public InstanceScript
        {
            instance_azjol_nerub_InstanceScript(Map* map) : InstanceScript(map) { }

            ObjectGuid uiKrikthir;
            ObjectGuid uiHadronox;
            ObjectGuid uiAnubarak;
            ObjectGuid uiWatcherGashra;
            ObjectGuid uiWatcherSilthik;
            ObjectGuid uiWatcherNarjil;
            ObjectGuid uiAnubarakDoor[3];

            ObjectGuid uiKrikthirDoor;
            ObjectGuid questGiverGUID;
            ObjectGuid questEnderGUID;

            std::list<ObjectGuid> hadronoxTrash;
            std::list<ObjectGuid> questCreatures;
            ObjectGuid hadronoxDoors[3];
            bool hadronoxAchievementFailed;
            bool crushersSpawned;
            uint32 trashLeft;

            uint32 auiEncounter[MAX_ENCOUNTER];

            EventMap events;

           void Initialize() override
           {
                memset(&auiEncounter, 0, sizeof(auiEncounter));
                memset(&uiAnubarakDoor, 0, sizeof(uiAnubarakDoor));
                memset(&hadronoxDoors, 0, sizeof(hadronoxDoors));

                uiKrikthir = ObjectGuid::Empty;
                uiHadronox = ObjectGuid::Empty;
                uiAnubarak = ObjectGuid::Empty;
                questGiverGUID = ObjectGuid::Empty;
                questEnderGUID = ObjectGuid::Empty;
                uiWatcherGashra = ObjectGuid::Empty;
                uiWatcherSilthik = ObjectGuid::Empty;
                uiWatcherNarjil = ObjectGuid::Empty;
                uiKrikthirDoor = ObjectGuid::Empty;

                hadronoxAchievementFailed = false;
                crushersSpawned = false;
                trashLeft = 0;

                events.Reset();
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_KRIKTHIR:
                        uiKrikthir = creature->GetGUID();
                        break;
                    case NPC_HADRONOX:
                        uiHadronox = creature->GetGUID();
                        break;
                    case NPC_ANUBARAK:
                        uiAnubarak = creature->GetGUID();
                        break;
                    case NPC_WATCHER_GASHRA:
                        uiWatcherGashra = creature->GetGUID();
                        break;
                    case NPC_WATCHER_SILTHIK:
                        uiWatcherSilthik = creature->GetGUID();
                        break;
                    case NPC_WATCHER_NARJIL:
                        uiWatcherNarjil = creature->GetGUID();
                        break;
                    case NPC_ANUBAR_CHAMPION:
                    case NPC_ANUBAR_CRYPT_FIEND:
                    case NPC_ANUBAR_CRUSHER:
                    case NPC_ANUBAR_CHAMPION_TRASH:
                    case NPC_ANUBAR_CRYPT_FIEND_TRASH:
                        hadronoxTrash.push_back(creature->GetGUID());
                        break;
                    case NPC_WORLD_TRIGGER_LARGE_AOI:
                        if (creature->GetPositionZ() < 750)
                            hadronoxDoors[2] = creature->GetGUID();
                        else if (hadronoxDoors[0])
                            hadronoxDoors[1] = creature->GetGUID();
                        else
                            hadronoxDoors[0] = creature->GetGUID();
                        break;
                    case NPC_RECLAIMER_AZAK:
                        if (creature->GetPositionZ() < 800)
                        {
                            creature->SetVisible(false);
                            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            questEnderGUID = creature->GetGUID();
                        }
                        else
                            questGiverGUID = creature->GetGUID();
                        break;
                    case NPC_AZJOL_ANAK_WARRIOR:
                    case NPC_NERUBIAN_FLYER:
                        if (GetData64(DATA_ANUBARAK) != DONE)
                            creature->SetVisible(false);
                        questCreatures.push_back(creature->GetGUID());
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_HADRONOX:
                        uiHadronox = ObjectGuid::Empty;
                        break;
                    case NPC_ANUBAR_CHAMPION:
                    case NPC_ANUBAR_CRYPT_FIEND:
                    case NPC_ANUBAR_CRUSHER:
                    case NPC_ANUBAR_CHAMPION_TRASH:
                    case NPC_ANUBAR_CRYPT_FIEND_TRASH:
                        hadronoxTrash.remove(creature->GetGUID());
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_KRIKTHIR_DOOR:
                        uiKrikthirDoor = go->GetGUID();
                        if (auiEncounter[0] == DONE)
                            HandleGameObject(ObjectGuid::Empty,true,go);
                        break;
                    case GO_ANUBARAK_DOOR_1:
                        uiAnubarakDoor[0] = go->GetGUID();
                        break;
                    case GO_ANUBARAK_DOOR_2:
                        uiAnubarakDoor[1] = go->GetGUID();
                        break;
                    case GO_ANUBARAK_DOOR_3:
                        uiAnubarakDoor[2] = go->GetGUID();
                        break;
                }
            }


            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_KRIKTHIR_THE_GATEWATCHER:     return uiKrikthir;
                    case DATA_HADRONOX:                     return uiHadronox;
                    case DATA_ANUBARAK:                     return uiAnubarak;
                    case DATA_WATCHER_GASHRA:               return uiWatcherGashra;
                    case DATA_WATCHER_SILTHIK:              return uiWatcherSilthik;
                    case DATA_WATCHER_NARJIL:               return uiWatcherNarjil;
                    case DATA_HADRONOX_FRONT_DOOR_1:        return hadronoxDoors[0];
                    case DATA_HADRONOX_FRONT_DOOR_2:        return hadronoxDoors[1];
                    case DATA_HADRONOX_SIDE_DOOR:           return hadronoxDoors[2];
                    case NPC_RECLAIMER_AZAK:                return questGiverGUID;
                    case NPC_RECLAIMER_AZAK + 1:            return questEnderGUID;
                }

                return ObjectGuid::Empty;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_KRIKTHIR_THE_GATEWATCHER_EVENT:
                        auiEncounter[0] = data;
                        if (data == DONE)
                        {
                            HandleGameObject(uiKrikthirDoor,true);
                            events.RescheduleEvent(EVENT_RESET_HADRONOX, 1);
                            events.RescheduleEvent(EVENT_SPAWN_HADRONOX_TRASH, 1);
                        }
                        break;
                    case DATA_HADRONOX_EVENT:
                        if (auiEncounter[1] == DONE)
                            return;

                        if (data == SPECIAL)
                        {
                            if (!crushersSpawned)
                                events.ScheduleEvent(EVENT_SPAWN_HADRONOX_CRUSHERS, 5000);
                            crushersSpawned = true;
                            return;
                        }
                        else if (data == FAIL)
                        {
                            hadronoxAchievementFailed = false;
                            crushersSpawned = false;
                            trashLeft = 0;

                            if (Creature* hadronox = instance->GetCreature(uiHadronox))
                                if (hadronox->IsAlive())
                                    hadronox->DespawnOrUnsummon();

                            for (auto&& guid : hadronoxTrash)
                                if (Creature* trash = instance->GetCreature(guid))
                                    if (trash->IsAlive())
                                        trash->DespawnOrUnsummon(1);

                            for (auto&& hadronoxDoorGUID : hadronoxDoors)
                                if (Creature* door = instance->GetCreature(hadronoxDoorGUID))
                                    door->RemoveAllAuras();

                            events.CancelEvent(EVENT_SPAWN_HADRONOX_CRUSHERS);
                            events.CancelEvent(EVENT_SPAWN_HADRONOX_TRASH);
                            events.RescheduleEvent(EVENT_RESET_HADRONOX, 30000);
                            events.RescheduleEvent(EVENT_SPAWN_HADRONOX_TRASH, 30000);
                        }
                        else if (data == DONE)
                            events.CancelEvent(EVENT_SPAWN_HADRONOX_TRASH);

                        auiEncounter[1] = data;
                        break;
                    case DATA_ANUBARAK_EVENT:
                        auiEncounter[2] = data;
                        if (data == IN_PROGRESS)
                            events.ScheduleEvent(EVENT_ANUBARAK_DOORS, 5000);
                        else if (data == NOT_STARTED || data == DONE)
                        {
                            events.CancelEvent(EVENT_ANUBARAK_DOORS);
                            for (uint8 i = 0; i < 3; ++i)
                                HandleGameObject(uiAnubarakDoor[i], true);
                        }
                        if (data == DONE)
                        {
                            for (auto&& questCreatureGUID : questCreatures)
                                if (Creature* creature = instance->GetCreature(questCreatureGUID))
                                    creature->SetVisible(true);

                            if (Creature* qStarter = instance->GetCreature(GetGuidData(NPC_RECLAIMER_AZAK)))
                            {
                                qStarter->SetVisible(false);
                                qStarter->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            }

                            if (Creature* qEnder = instance->GetCreature(GetGuidData(NPC_RECLAIMER_AZAK + 1)))
                            {
                                qEnder->SetVisible(true);
                                qEnder->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            }
                        }
                        break;
                    case DATA_TRASH_LEFT:
                        if (data == IN_PROGRESS)
                            ++trashLeft;
                        else
                            --trashLeft;
                        break;
                }

                if (data == DONE)
                {
                    SaveToDB();
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_KRIKTHIR_THE_GATEWATCHER_EVENT:   return auiEncounter[0];
                    case DATA_HADRONOX_EVENT:                   return auiEncounter[1];
                    case DATA_ANUBARAK_EVENT:                   return auiEncounter[2];
                    case DATA_TRASH_LEFT:                       return trashLeft;
                }

                return 0;
            }

            void ProcessEvent(WorldObject* /*source*/, uint32 eventId) override
            {
                if (eventId == SCRIPT_EVENT_WEB_FRONT_DOORS || eventId == SCRIPT_EVENT_WEB_SIDE_DOOR)
                    instance->SetWorldState(WORLD_STATE_HADRONOX_DENIED, 0);
            }

            void Update(uint32 diff) override
            {
                if (!instance->HavePlayers())
                {
                    if (GetData(DATA_HADRONOX_EVENT) == IN_PROGRESS)
                    {
                        // Cancel the event and reschedule the timers so that it will restart immediately when a player enters the instance
                        SetData(DATA_HADRONOX_EVENT, FAIL);
                        events.RescheduleEvent(EVENT_RESET_HADRONOX, 1);
                        events.RescheduleEvent(EVENT_SPAWN_HADRONOX_TRASH, 1);
                    }
                    return;
                }

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RESET_HADRONOX:
                            instance->SummonCreature(NPC_HADRONOX, hadronoxPos);
                            instance->SummonCreature(NPC_ANUBAR_CRUSHER, crusherPos);
                            instance->SummonCreature(NPC_ANUBAR_CHAMPION, crusherAddsPos[0]);
                            instance->SummonCreature(NPC_ANUBAR_CRYPT_FIEND, crusherAddsPos[1]);
                            break;
                        case EVENT_SPAWN_HADRONOX_CRUSHERS:
                            for (uint32 i = 0; i < 2; ++i)
                            {
                                if (Creature* door = instance->GetCreature(hadronoxDoors[i]))
                                {
                                    if (!door->HasAura(SPELL_WEB_FRONT_DOORS) && !door->HasAura(SPELL_WEB_SIDE_DOORS))
                                    {
                                        door->SummonCreature(NPC_ANUBAR_CRUSHER, *door, TEMPSUMMON_DEAD_DESPAWN);
                                        door->SummonCreature(NPC_ANUBAR_CHAMPION, *door, TEMPSUMMON_DEAD_DESPAWN);
                                        door->SummonCreature(NPC_ANUBAR_CRYPT_FIEND, *door, TEMPSUMMON_DEAD_DESPAWN);
                                    }
                                }
                            }
                            break;
                        case EVENT_SPAWN_HADRONOX_TRASH:
                            for (uint32 i = 0; i < sizeof(hadronoxDoors) / sizeof(hadronoxDoors[0]); ++i)
                                if (Creature* door = instance->GetCreature(hadronoxDoors[i]))
                                    if (!door->HasAura(SPELL_WEB_FRONT_DOORS) && !door->HasAura(SPELL_WEB_SIDE_DOORS))
                                        door->SummonCreature(i == 2 ? NPC_ANUBAR_CRYPT_FIEND_TRASH : NPC_ANUBAR_CHAMPION_TRASH, *door, TEMPSUMMON_DEAD_DESPAWN);

                            events.ScheduleEvent(EVENT_SPAWN_HADRONOX_TRASH, 10000);
                            break;
                        case EVENT_ANUBARAK_DOORS:
                            for (uint8 i = 0; i < 3; ++i)
                                HandleGameObject(uiAnubarakDoor[i], false);
                            break;
                    }
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "A N " << auiEncounter[0] << " " << auiEncounter[1] << " "
                    << auiEncounter[2];

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
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
                uint16 data0,data1,data2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2;

                if (dataHead1 == 'A' && dataHead2 == 'N')
                {
                    auiEncounter[0] = data0;
                    auiEncounter[1] = data1;
                    auiEncounter[2] = data2;

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (auiEncounter[i] == IN_PROGRESS)
                            auiEncounter[i] = NOT_STARTED;

                    if (auiEncounter[0] == DONE && auiEncounter[1] != DONE)
                    {
                        events.ScheduleEvent(EVENT_RESET_HADRONOX, 1);
                        events.ScheduleEvent(EVENT_SPAWN_HADRONOX_TRASH, 1);
                    }

                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_azjol_nerub_InstanceScript(map);
        }
};

void AddSC_instance_azjol_nerub()
{
   new instance_azjol_nerub;
}
