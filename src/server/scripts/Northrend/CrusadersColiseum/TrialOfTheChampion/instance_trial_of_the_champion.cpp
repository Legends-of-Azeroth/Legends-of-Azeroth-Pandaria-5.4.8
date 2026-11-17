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
#include "trial_of_the_champion.h"

#define MAX_ENCOUNTER  4

class instance_trial_of_the_champion : public InstanceMapScript
{
    public:
        instance_trial_of_the_champion() : InstanceMapScript("instance_trial_of_the_champion", 650) { }

        struct instance_trial_of_the_champion_InstanceMapScript : public InstanceScript
        {
            instance_trial_of_the_champion_InstanceMapScript(Map* map) : InstanceScript(map) {Initialize();}

            uint32 m_auiEncounter[MAX_ENCOUNTER];
            uint32 teamInInstance;

            uint8 uiBlackKnightEvent2;
            uint16 movementDone;
            uint16 grandChampionsDeaths;
            uint8 argentSoldierDeaths;
            uint32 memoryEntry;
            uint32 grandChampionEntry[3];

            ObjectGuid announcerGUID;
            ObjectGuid mainGateGUID;
            ObjectGuid portcullisGUID;
            ObjectGuid grandChampion1GUID;
            ObjectGuid grandChampion2GUID;
            ObjectGuid grandChampion3GUID;
            ObjectGuid championLootGUID;
            ObjectGuid argentChampionGUID;
            ObjectGuid blackKnightGUID;

            std::list<ObjectGuid> vehicleList;
            std::string str_data;

            void Initialize() override
            {
                movementDone = 0;
                grandChampionsDeaths = 0;
                argentSoldierDeaths = 0;
                teamInInstance = 0;
                memoryEntry = 0;

                for (uint8 i = 0; i < 3; ++i)
                    grandChampionEntry[i] = 0;

                announcerGUID = ObjectGuid::Empty;
                mainGateGUID = ObjectGuid::Empty;
                portcullisGUID = ObjectGuid::Empty;
                grandChampion1GUID = ObjectGuid::Empty;
                grandChampion2GUID = ObjectGuid::Empty;
                grandChampion3GUID = ObjectGuid::Empty;
                championLootGUID = ObjectGuid::Empty;
                argentChampionGUID = ObjectGuid::Empty;
                blackKnightGUID = ObjectGuid::Empty;

                vehicleList.clear();
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        return true;
                }

                return false;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                Map::PlayerList const& players = instance->GetPlayers();

                if (!players.isEmpty())
                {
                    if (Player* player = players.begin()->GetSource())
                        teamInInstance = player->GetTeam();
                }

                uint32 oldEntry = creature->GetEntry();
                switch (oldEntry)
                {
                    // Grand Champions
                    case NPC_MOKRA:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_JACOB, ALLIANCE);
                        break;
                    case NPC_ERESSEA:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_AMBROSE, ALLIANCE);
                        break;
                    case NPC_RUNOK:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_COLOSOS, ALLIANCE);
                        break;
                    case NPC_ZULTORE:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_JAELYNE, ALLIANCE);
                        break;
                    case NPC_VISCERI:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_LANA, ALLIANCE);
                        break;
                    // Faction Champios
                    case NPC_ORGRIMMAR_CHAMPION:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_STORMWIND_CHAMPION, ALLIANCE);
                        break;
                    case NPC_SILVERMOON_CHAMPION:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_GNOMEREGAN_CHAMPION, ALLIANCE);
                        break;
                    case NPC_THUNDER_CHAMPION:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_EXODAR_CHAMPION, ALLIANCE);
                        break;
                    case NPC_TROLL_CHAMPION:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_DARNASSUS_CHAMPION, ALLIANCE);
                        break;
                    case NPC_UNDERCITY_CHAMPION:
                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_IRONFORGE_CHAMPION, ALLIANCE);
                        break;
                    // Coliseum Announcer, Just NPC_ARELAS must be spawned.
                    case NPC_ARELAS:
                        announcerGUID = creature->GetGUID();

                        if (teamInInstance == HORDE)
                            creature->UpdateEntry(NPC_JAEREN, HORDE);
                        break;
                    case NPC_JAEREN_AN:
                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ARELAS_AN, ALLIANCE);
                        break;
                    case VEHICLE_ARGENT_WARHORSE:
                    case VEHICLE_ARGENT_BATTLEWORG:
                        vehicleList.push_back(creature->GetGUID());
                        break;
                    case NPC_EADRIC:
                    case NPC_PALETRESS:
                        argentChampionGUID = creature->GetGUID();
                        break;
                    case NPC_BLACK_KNIGHT:
                        blackKnightGUID = creature->GetGUID();
                        break;
                }

                if (creature->GetCreatureTemplate() && creature->GetCreatureTemplate()->Entry != oldEntry)
                    creature->LoadCreaturesAddon(true);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_MAIN_GATE:
                        mainGateGUID = go->GetGUID();
                        break;
                    case GO_NORTH_PORTCULLIS:
                        portcullisGUID = go->GetGUID();
                        break;
                    case GO_CHAMPIONS_LOOT:
                    case GO_CHAMPIONS_LOOT_H:
                        championLootGUID = go->GetGUID();
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    /*
                    case DATA_KNIGHT:
                        uiBlackKnightEvent2 = uiData;
                        if (uiData == NOT_STARTED)
                        {
                            if (Creature* pBlackKnight =  instance->GetCreature(blackKnightGUID))
                            {
                                pBlackKnight->SetOrientation(4.714f);
                            }

                        }
                        else if (uiData == IN_PROGRESS)
                        {
                            if (Creature* pBlackKnight =  instance->GetCreature(blackKnightGUID))
                            {
                                DoScriptText(SAY_BLACK_KNIGHT_2, pBlackKnight);
                                if (Creature* pAnnouncer =  instance->GetCreature(uiAnnouncerGUID))
                                {
                                    pAnnouncer->setDeathState(JUST_DIED);
                                }
                            }

                        }
                        else if (uiData == DONE)
                        {
                            if (Creature* pBlackKnight =  instance->GetCreature(uiBlackKnightGUID))
                            {
                                pBlackKnight->setFaction(14);
                                pBlackKnight->SetReactState(REACT_AGGRESSIVE);
                                pBlackKnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                                AggroAllPlayers(pBlackKnight);
                                if (Creature* pAnnouncer =  instance->GetCreature(uiAnnouncerGUID))
                                {
                                    pAnnouncer->DisappearAndDie();
                                    if (Creature* pGhoul = pAnnouncer->SummonCreature(NPC_RISEN_JAEREN,pAnnouncer->GetPositionX(),pAnnouncer->GetPositionY(),pAnnouncer->GetPositionZ(),pAnnouncer->GetOrientation()))
                                    {
                                        pGhoul->setFaction(14);
                                        AggroAllPlayers(pGhoul);
                                    }
                                }
                            }
                        }
                        break;
                        */
                    case DATA_MOVEMENT_DONE:
                        movementDone = data;
                        if (movementDone == 3)
                        {
                            if (Creature* announcer = instance->GetCreature(GetGuidData(DATA_ANNOUNCER)))
                                announcer->AI()->SetData(DATA_IN_POSITION, 0);
                        }
                        break;
                    case BOSS_GRAND_CHAMPIONS:
                        if (data == IN_PROGRESS)
                        {
                            m_auiEncounter[0] = data;
                            for (auto&& guid : vehicleList)
                                if (Creature* summon = instance->GetCreature(guid))
                                    summon->RemoveFromWorld();
                        }
                        else if (data == DONE)
                        {
                            ++grandChampionsDeaths;
                            if (grandChampionsDeaths >= 3)
                            {
                                for (uint8 i = 0; i < 3; ++i)
                                    if (Creature* GrandChampion = instance->GetCreature(GetGuidData(DATA_GRAND_CHAMPION_1 + i)))
                                        GrandChampion->AI()->SetData(11, 0);

                                if (Creature* announcer = instance->GetCreature(announcerGUID))
                                {
                                    m_auiEncounter[0] = data;
                                    DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_GRAND_CHAMPION_CREDIT);
                                    announcer->GetMotionMaster()->MovePoint(0, 748.309f, 619.487f, 411.171f);
                                    announcer->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                                    if (GameObject* chest = announcer->SummonGameObject(instance->IsHeroic() ? GO_CHAMPIONS_LOOT_H : GO_CHAMPIONS_LOOT, 746.59f, 618.49f, 411.09f, 1.42f, { }, 90000000))
                                        chest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                                    BindPlayersToInstance(announcer);
                                }
                            }
                        }
                        break;
                    case DATA_ARGENT_SOLDIER_DEFEATED:
                        argentSoldierDeaths = data;
                        if (argentSoldierDeaths == 9)
                        {
                            if (Creature* boss = instance->GetCreature(argentChampionGUID))
                            {
                                boss->GetMotionMaster()->MovePoint(0, 746.88f, 618.74f, 411.06f);
                                boss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                                boss->SetReactState(REACT_AGGRESSIVE);
                                boss->SetFaction(16);
                            }
                        }
                        break;
                    case BOSS_ARGENT_CHALLENGE_E:
                        if (m_auiEncounter[1] != DONE)
                            m_auiEncounter[1] = data;
                        if (data == IN_PROGRESS)
                        {
                            for (auto&& guid : vehicleList)
                                if (Creature* summon = instance->GetCreature(guid))
                                    summon->RemoveFromWorld();
                        }
                        else if (data == DONE)
                        {
                            if (Creature* announcer = instance->GetCreature(announcerGUID))
                            {
                                announcer->GetMotionMaster()->MovePoint(0, 748.309f, 619.487f, 411.171f);
                                announcer->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                                if (GameObject* chest = announcer->SummonGameObject(instance->IsHeroic() ? GO_EADRIC_LOOT_H : GO_EADRIC_LOOT, 746.59f, 618.49f, 411.09f, 1.42f, { }, 90000000))
                                    chest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                                BindPlayersToInstance(announcer);
                            }
                        }
                        break;
                    case BOSS_ARGENT_CHALLENGE_P:
                        if (m_auiEncounter[2] != DONE)
                            m_auiEncounter[2] = data;
                        if (data == IN_PROGRESS)
                        {
                            for (auto&& guid : vehicleList)
                                if (Creature* summon = instance->GetCreature(guid))
                                    summon->RemoveFromWorld();
                        }
                        else if (data == DONE)
                        {
                            if (Creature* announcer = instance->GetCreature(announcerGUID))
                            {
                                announcer->GetMotionMaster()->MovePoint(0, 748.309f, 619.487f, 411.171f);
                                announcer->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                                if (GameObject* chest = announcer->SummonGameObject(instance->IsHeroic() ? GO_PALETRESS_LOOT_H : GO_PALETRESS_LOOT, 746.59f, 618.49f, 411.09f, 1.42f, { }, 90000000))
                                    chest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                                BindPlayersToInstance(announcer);
                            }
                        }
                        break;
                    case BOSS_BLACK_KNIGHT:
                        m_auiEncounter[3] = data;
                        break;
                    case DATA_GRAND_CHAMPION_ENTRY:
                        for (uint8 i = 0; i < 3; ++i)
                        {
                            if (grandChampionEntry[i] == 0)
                            {
                                grandChampionEntry[i] = data;
                                return;
                            }
                        }
                        break;
                    case DATA_MEMORY_ENTRY:
                        memoryEntry = data;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 data) const override
            {
                switch (data)
                {
                    case BOSS_GRAND_CHAMPIONS:         return m_auiEncounter[0];
                    case BOSS_ARGENT_CHALLENGE_E:      return m_auiEncounter[1];
                    case BOSS_ARGENT_CHALLENGE_P:      return m_auiEncounter[2];
                    case BOSS_BLACK_KNIGHT:            return m_auiEncounter[3];

                    case DATA_MOVEMENT_DONE:           return movementDone;
                    case DATA_ARGENT_SOLDIER_DEFEATED: return argentSoldierDeaths;
                    case DATA_TEAM_IN_INSTANCE:        return teamInInstance;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_ANNOUNCER:               return announcerGUID;
                    case DATA_MAIN_GATE:               return mainGateGUID;
                    case DATA_PORTCULLIS:              return portcullisGUID;

                    case DATA_GRAND_CHAMPION_1:        return grandChampion1GUID;
                    case DATA_GRAND_CHAMPION_2:        return grandChampion2GUID;
                    case DATA_GRAND_CHAMPION_3:        return grandChampion3GUID;
                    case DATA_BLACK_KNIGHT:            return blackKnightGUID;
                }

                return ObjectGuid::Empty;
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_GRAND_CHAMPION_1:
                        grandChampion1GUID = data;
                        break;
                    case DATA_GRAND_CHAMPION_2:
                        grandChampion2GUID = data;
                        break;
                    case DATA_GRAND_CHAMPION_3:
                        grandChampion3GUID = data;
                        break;
                }
            }

            bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1) override
            {
                switch (criteria_id)
                {
                    case CRITERIA_CHAMPION_JACOB:
                    case CRITERIA_CHAMPION_LANA:
                    case CRITERIA_CHAMPION_COLOSOS:
                    case CRITERIA_CHAMPION_AMBROSE:
                    case CRITERIA_CHAMPION_JAELYNE:
                    case CRITERIA_CHAMPION_MOKRA:
                    case CRITERIA_CHAMPION_VISCERI:
                    case CRITERIA_CHAMPION_RUNOK:
                    case CRITERIA_CHAMPION_ERESSEA:
                    case CRITERIA_CHAMPION_ZULTORE:
                    case CRITERIA_CHAMPION_JACOB_H:
                    case CRITERIA_CHAMPION_LANA_H:
                    case CRITERIA_CHAMPION_COLOSOS_H:
                    case CRITERIA_CHAMPION_AMBROSE_H:
                    case CRITERIA_CHAMPION_JAELYNE_H:
                    case CRITERIA_CHAMPION_MOKRA_H:
                    case CRITERIA_CHAMPION_VISCERI_H:
                    case CRITERIA_CHAMPION_RUNOK_H:
                    case CRITERIA_CHAMPION_ERESSEA_H:
                    case CRITERIA_CHAMPION_ZULTORE_H:
                        for (uint8 i = 0; i < 3; ++i)
                            if (grandChampionEntry[i] == GetRelatedCreatureEntry(criteria_id))
                                return true;
                        return false;
                    case CRITERIA_MEMORY_HOGGER:
                    case CRITERIA_MEMORY_VANCLEEF:
                    case CRITERIA_MEMORY_MUTANUS:
                    case CRITERIA_MEMORY_HEROD:
                    case CRITERIA_MEMORY_LUCIFROM:
                    case CRITERIA_MEMORY_THUNDERAAN:
                    case CRITERIA_MEMORY_CHROMAGGUS:
                    case CRITERIA_MEMORY_HAKKAR:
                    case CRITERIA_MEMORY_VEKNILASH:
                    case CRITERIA_MEMORY_KALITHRESH:
                    case CRITERIA_MEMORY_MALCHEZAAR:
                    case CRITERIA_MEMORY_GRUUL:
                    case CRITERIA_MEMORY_VASHJ:
                    case CRITERIA_MEMORY_ARCHIMONDE:
                    case CRITERIA_MEMORY_ILLIDAN:
                    case CRITERIA_MEMORY_DELRISSA:
                    case CRITERIA_MEMORY_MURU:
                    case CRITERIA_MEMORY_INGVAR:
                    case CRITERIA_MEMORY_CYANIGOSA:
                    case CRITERIA_MEMORY_ECK:
                    case CRITERIA_MEMORY_ONYXIA:
                    case CRITERIA_MEMORY_HEIGAN:
                    case CRITERIA_MEMORY_IGNIS:
                    case CRITERIA_MEMORY_VEZAX:
                    case CRITERIA_MEMORY_ALGALON:
                        return (memoryEntry == GetRelatedCreatureEntry(criteria_id));
                }
                return InstanceScript::CheckAchievementCriteriaMeet(criteria_id, source, target, miscvalue1);
            }

            uint32 GetRelatedCreatureEntry(uint32 criteria_id)
            {
                switch (criteria_id)
                {
                    case CRITERIA_CHAMPION_JACOB:
                    case CRITERIA_CHAMPION_JACOB_H:
                        return NPC_JACOB;
                    case CRITERIA_CHAMPION_LANA:
                    case CRITERIA_CHAMPION_LANA_H:
                        return NPC_LANA;
                    case CRITERIA_CHAMPION_COLOSOS:
                    case CRITERIA_CHAMPION_COLOSOS_H:
                        return NPC_COLOSOS;
                    case CRITERIA_CHAMPION_AMBROSE:
                    case CRITERIA_CHAMPION_AMBROSE_H:
                        return NPC_AMBROSE;
                    case CRITERIA_CHAMPION_JAELYNE:
                    case CRITERIA_CHAMPION_JAELYNE_H:
                        return NPC_JAELYNE;
                    case CRITERIA_CHAMPION_MOKRA:
                    case CRITERIA_CHAMPION_MOKRA_H:
                        return NPC_MOKRA;
                    case CRITERIA_CHAMPION_VISCERI:
                    case CRITERIA_CHAMPION_VISCERI_H:
                        return NPC_VISCERI;
                    case CRITERIA_CHAMPION_RUNOK:
                    case CRITERIA_CHAMPION_RUNOK_H:
                        return NPC_RUNOK;
                    case CRITERIA_CHAMPION_ERESSEA:
                    case CRITERIA_CHAMPION_ERESSEA_H:
                        return NPC_ERESSEA;
                    case CRITERIA_CHAMPION_ZULTORE:
                    case CRITERIA_CHAMPION_ZULTORE_H:
                        return NPC_ZULTORE;

                    case CRITERIA_MEMORY_HOGGER:     return NPC_MEMORY_HOGGER;
                    case CRITERIA_MEMORY_VANCLEEF:   return NPC_MEMORY_VANCLEEF;
                    case CRITERIA_MEMORY_MUTANUS:    return NPC_MEMORY_MUTANUS;
                    case CRITERIA_MEMORY_HEROD:      return NPC_MEMORY_HEROD;
                    case CRITERIA_MEMORY_LUCIFROM:   return NPC_MEMORY_LUCIFROM;
                    case CRITERIA_MEMORY_THUNDERAAN: return NPC_MEMORY_THUNDERAAN;
                    case CRITERIA_MEMORY_CHROMAGGUS: return NPC_MEMORY_CHROMAGGUS;
                    case CRITERIA_MEMORY_HAKKAR:     return NPC_MEMORY_HAKKAR;
                    case CRITERIA_MEMORY_VEKNILASH:  return NPC_MEMORY_VEKNILASH;
                    case CRITERIA_MEMORY_KALITHRESH: return NPC_MEMORY_KALITHRESH;
                    case CRITERIA_MEMORY_MALCHEZAAR: return NPC_MEMORY_MALCHEZAAR;
                    case CRITERIA_MEMORY_GRUUL:      return NPC_MEMORY_GRUUL;
                    case CRITERIA_MEMORY_VASHJ:      return NPC_MEMORY_VASHJ;
                    case CRITERIA_MEMORY_ARCHIMONDE: return NPC_MEMORY_ARCHIMONDE;
                    case CRITERIA_MEMORY_ILLIDAN:    return NPC_MEMORY_ILLIDAN;
                    case CRITERIA_MEMORY_DELRISSA:   return NPC_MEMORY_DELRISSA;
                    case CRITERIA_MEMORY_MURU:       return NPC_MEMORY_MURU;
                    case CRITERIA_MEMORY_INGVAR:     return NPC_MEMORY_INGVAR;
                    case CRITERIA_MEMORY_CYANIGOSA:  return NPC_MEMORY_CYANIGOSA;
                    case CRITERIA_MEMORY_ECK:        return NPC_MEMORY_ECK;
                    case CRITERIA_MEMORY_ONYXIA:     return NPC_MEMORY_ONYXIA;
                    case CRITERIA_MEMORY_HEIGAN:     return NPC_MEMORY_HEIGAN;
                    case CRITERIA_MEMORY_IGNIS:      return NPC_MEMORY_IGNIS;
                    case CRITERIA_MEMORY_VEZAX:      return NPC_MEMORY_VEZAX;
                    case CRITERIA_MEMORY_ALGALON:    return NPC_MEMORY_ALGALON;
                }
                return 0;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;

                saveStream << "T C " << m_auiEncounter[0]
                    << ' ' << m_auiEncounter[1]
                    << ' ' << m_auiEncounter[2]
                    << ' ' << m_auiEncounter[3]
                    << ' ' << grandChampionsDeaths
                    << ' ' << movementDone;

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
                uint16 data0, data1, data2, data3, data4, data5;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4 >> data5;

                if (dataHead1 == 'T' && dataHead2 == 'C')
                {
                    m_auiEncounter[0] = data0;
                    m_auiEncounter[1] = data1;
                    m_auiEncounter[2] = data2;
                    m_auiEncounter[3] = data3;

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (m_auiEncounter[i] == IN_PROGRESS)
                            m_auiEncounter[i] = NOT_STARTED;

                    grandChampionsDeaths = data4;
                    movementDone = data5;
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_trial_of_the_champion_InstanceMapScript(map);
        }
};

void AddSC_instance_trial_of_the_champion()
{
    new instance_trial_of_the_champion();
}
