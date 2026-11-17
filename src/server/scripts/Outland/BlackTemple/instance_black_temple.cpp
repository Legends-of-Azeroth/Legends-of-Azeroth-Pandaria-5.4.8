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
SDName: Instance_Black_Temple
SD%Complete: 100
SDComment: Instance Data Scripts and functions to acquire mobs and set encounter status for use in various Black Temple Scripts
SDCategory: Black Temple
EndScriptData */

#include "ScriptPCH.h"
#include "black_temple.h"

#define MAX_ENCOUNTER      9

/* Black Temple encounters:
0 - High Warlord Naj'entus event
1 - Supremus Event
2 - Shade of Akama Event
3 - Teron Gorefiend Event
4 - Gurtogg Bloodboil Event
5 - Reliquary Of Souls Event
6 - Mother Shahraz Event
7 - Illidari Council Event
8 - Illidan Stormrage Event
*/

class instance_black_temple : public InstanceMapScript
{
    public:
        instance_black_temple() : InstanceMapScript("instance_black_temple", 564) { }

        struct instance_black_temple_InstanceMapScript : public InstanceScript
        {
            instance_black_temple_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_ENCOUNTER];
            std::string str_data;

            ObjectGuid Najentus;
            ObjectGuid Akama;       // This is the Akama that starts the Illidan encounter.
            ObjectGuid Akama_Shade; // This is the Akama that starts the Shade of Akama encounter.
            ObjectGuid ShadeOfAkama;
            ObjectGuid Supremus;
            ObjectGuid Teron;
            ObjectGuid LadyMalande;
            ObjectGuid GathiosTheShatterer;
            ObjectGuid HighNethermancerZerevor;
            ObjectGuid VerasDarkshadow;
            ObjectGuid IllidariCouncil;
            ObjectGuid BloodElfCouncilVoice;
            ObjectGuid IllidanStormrage;

            ObjectGuid NajentusGate;
            ObjectGuid MainTempleDoors;
            ObjectGuid ShadeOfAkamaDoor;
            ObjectGuid CommonDoor; // Teron
            ObjectGuid TeronDoor;
            ObjectGuid GuurtogDoor;
            ObjectGuid MotherDoor;
            ObjectGuid TempleDoor; // Befor mother
            ObjectGuid CouncilDoor;
            ObjectGuid SimpleDoor; // council
            ObjectGuid IllidanGate;
            ObjectGuid IllidanDoor[2];

            uint32 IllidanGateOpend;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                Najentus = ObjectGuid::Empty;
                Akama = ObjectGuid::Empty;
                Akama_Shade = ObjectGuid::Empty;
                ShadeOfAkama = ObjectGuid::Empty;
                Supremus = ObjectGuid::Empty;
                Teron = ObjectGuid::Empty;
                LadyMalande = ObjectGuid::Empty;
                GathiosTheShatterer = ObjectGuid::Empty;
                HighNethermancerZerevor = ObjectGuid::Empty;
                VerasDarkshadow = ObjectGuid::Empty;
                IllidariCouncil = ObjectGuid::Empty;
                BloodElfCouncilVoice = ObjectGuid::Empty;
                IllidanStormrage = ObjectGuid::Empty;

                NajentusGate    = ObjectGuid::Empty;
                MainTempleDoors = ObjectGuid::Empty;
                ShadeOfAkamaDoor= ObjectGuid::Empty;
                CommonDoor              = ObjectGuid::Empty;//teron
                TeronDoor               = ObjectGuid::Empty;
                GuurtogDoor             = ObjectGuid::Empty;
                MotherDoor              = ObjectGuid::Empty;
                TempleDoor              = ObjectGuid::Empty;
                SimpleDoor              = ObjectGuid::Empty;//Bycouncil
                CouncilDoor             = ObjectGuid::Empty;
                IllidanGate     = ObjectGuid::Empty;
                IllidanDoor[0]  = ObjectGuid::Empty;
                IllidanDoor[1]  = ObjectGuid::Empty;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    m_auiEncounter[i] = NOT_STARTED;

                IllidanGateOpend = NOT_STARTED;
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

            Player* GetPlayerInMap()
            {
                Map::PlayerList const& players = instance->GetPlayers();

                if (!players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if (Player* plr = itr->GetSource())
                            return plr;
                    }
                }

                TC_LOG_DEBUG("misc", "TSCR: Instance Black Temple: GetPlayerInMap, but PlayerList is empty!");
                return nullptr;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case 22887:    Najentus = creature->GetGUID();                  break;
                    case 23089:    Akama = creature->GetGUID();                     break;
                    case 22990:    Akama_Shade = creature->GetGUID();               break;
                    case 22841:    ShadeOfAkama = creature->GetGUID();              break;
                    case 22898:    Supremus = creature->GetGUID();                  break;
                    case 22871:    Teron = creature->GetGUID();                     break;
                    case 22917:    IllidanStormrage = creature->GetGUID();          break;
                    case 22949:    GathiosTheShatterer = creature->GetGUID();       break;
                    case 22950:    HighNethermancerZerevor = creature->GetGUID();   break;
                    case 22951:    LadyMalande = creature->GetGUID();               break;
                    case 22952:    VerasDarkshadow = creature->GetGUID();           break;
                    case 23426:    IllidariCouncil = creature->GetGUID();           break;
                    case 23499:    BloodElfCouncilVoice = creature->GetGUID();      break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case 185483:
                        NajentusGate = go->GetGUID(); // Gate past Naj'entus (at the entrance to Supermoose's courtyards)
                        if (m_auiEncounter[0] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185882:
                        MainTempleDoors = go->GetGUID(); // Main Temple Doors - right past Supermoose (Supremus)
                        if (m_auiEncounter[1] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185478:
                        ShadeOfAkamaDoor = go->GetGUID();
                        break;
                    case 185480:
                        CommonDoor = go->GetGUID();
                        if (m_auiEncounter[3] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 186153:
                        TeronDoor = go->GetGUID();
                        if (m_auiEncounter[3] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185892:
                        GuurtogDoor = go->GetGUID();
                        if (m_auiEncounter[4] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185479:
                        TempleDoor = go->GetGUID();
                        // if (m_auiEncounter[5] == DONE)
                        //     HandleGameObject(ObjectGuid::Empty, true, go);
                        // break;
                        if (m_auiEncounter[2] == DONE && m_auiEncounter[3] == DONE && m_auiEncounter[4] == DONE && m_auiEncounter[5] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185482:
                        MotherDoor = go->GetGUID();
                        if (m_auiEncounter[6] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185481:
                        CouncilDoor = go->GetGUID();
                        if (m_auiEncounter[7] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 186152:
                        SimpleDoor = go->GetGUID();
                        if (m_auiEncounter[7] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case 185905:
                        IllidanGate = go->GetGUID(); // Gate leading to Temple Summit
                        // go->setActive(true);
                        if (IllidanGateOpend == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break; // Gate leading to Temple Summit
                    case 186261:
                        IllidanDoor[0] = go->GetGUID();
                        break; // Right door at Temple Summit
                    case 186262:
                        IllidanDoor[1] = go->GetGUID();
                        break; // Left door at Temple Summit
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_TERON_GOREFIEND:             return Teron;
                    case DATA_HIGH_WARLORD_NAJENTUS:       return Najentus;
                    case DATA_AKAMA:                       return Akama;
                    case DATA_AKAMA_SHADE:                 return Akama_Shade;
                    case DATA_SHADE_OF_AKAMA:              return ShadeOfAkama;
                    case DATA_SUPREMUS:                    return Supremus;
                    case DATA_ILLIDAN_STORMRAGE:           return IllidanStormrage;
                    case DATA_GATHIOS_THE_SHATTERER:       return GathiosTheShatterer;
                    case DATA_HIGH_NETHERMANCER_ZEREVOR:   return HighNethermancerZerevor;
                    case DATA_LADY_MALANDE:                return LadyMalande;
                    case DATA_VERAS_DARK_SHADOW:           return VerasDarkshadow;
                    case DATA_ILLIDARI_COUNCIL:            return IllidariCouncil;
                    case DATA_GAMEOBJECT_NAJENTUS_GATE:    return NajentusGate;
                    case DATA_GAMEOBJECT_ILLIDAN_GATE:     return IllidanGate;
                    case DATA_GAMEOBJECT_ILLIDAN_DOOR_R:   return IllidanDoor[0];
                    case DATA_GAMEOBJECT_ILLIDAN_DOOR_L:   return IllidanDoor[1];
                    case DATA_GAMEOBJECT_SUPREMUS_DOORS:   return MainTempleDoors;
                    case DATA_BLOOD_ELF_COUNCIL_VOICE:     return BloodElfCouncilVoice;
                }

                return ObjectGuid::Empty;
            }

            void HandleTempleDoor()
            {
                if (m_auiEncounter[2] == DONE && m_auiEncounter[3] == DONE && m_auiEncounter[4] == DONE && m_auiEncounter[5] == DONE)
                    HandleGameObject(TempleDoor, true);
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_HIGH_WARLORD_NAJENTUS_EVENT:
                        if (data == DONE)
                            HandleGameObject(NajentusGate, true);
                        // m_auiEncounter[0] = data;break;
                        if (m_auiEncounter[0] != DONE)
                            m_auiEncounter[0] = data;
                        break;
                    case DATA_SUPREMUS_EVENT:
                        if (data == DONE)
                            HandleGameObject(MainTempleDoors, true);
                        // m_auiEncounter[1] = data; break;
                        if (m_auiEncounter[1] != DONE)
                            m_auiEncounter[1] = data;
                        break;
                    case DATA_SHADE_OF_AKAMA_EVENT:
                        if (data == IN_PROGRESS)
                            HandleGameObject(ShadeOfAkamaDoor, false);
                        else
                            HandleGameObject(ShadeOfAkamaDoor, true);
                        // m_auiEncounter[2] = data; break;
                        if (m_auiEncounter[2] != DONE)
                            m_auiEncounter[2] = data;

                        HandleTempleDoor();
                        break;
                    case DATA_TERON_GOREFIEND_EVENT:
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(TeronDoor, false);
                            HandleGameObject(CommonDoor, false);
                        }
                        else
                        {
                            HandleGameObject(TeronDoor, true);
                            HandleGameObject(CommonDoor, true);
                        }
                        // m_auiEncounter[3] = data; break;
                        if (m_auiEncounter[3] != DONE)
                            m_auiEncounter[3] = data;

                        HandleTempleDoor();
                        break;
                    case DATA_GURTOGG_BLOOD_BOIL_EVENT:
                        if (data == DONE)
                            HandleGameObject(GuurtogDoor, true);
                        // m_auiEncounter[4] = data; break;
                        if (m_auiEncounter[4] != DONE)
                            m_auiEncounter[4] = data;

                        HandleTempleDoor();
                        break;
                    case DATA_RELIQUARY_OF_SOULS_EVENT:
                        // if (data == DONE)
                        //     HandleGameObject(TempleDoor, true);
                        // m_auiEncounter[5] = data; break;
                        if (m_auiEncounter[5] != DONE)
                            m_auiEncounter[5] = data;

                        HandleTempleDoor();
                        break;
                    case DATA_MOTHER_SHAHRAZ_EVENT:
                        if (data == DONE)
                            HandleGameObject(MotherDoor, true);
                        // m_auiEncounter[6] = data; break;
                        if (m_auiEncounter[6] != DONE)
                            m_auiEncounter[6] = data; 

                        break;
                    case DATA_ILLIDARI_COUNCIL_EVENT:
                        if (data == IN_PROGRESS)
                        {
                            HandleGameObject(CouncilDoor, false);
                            HandleGameObject(SimpleDoor, false);
                        }
                        else
                        {
                            HandleGameObject(CouncilDoor, true);
                            HandleGameObject(SimpleDoor, true);
                        }
                        // m_auiEncounter[7] = data; break;
                        if (m_auiEncounter[7] != DONE)
                            m_auiEncounter[7] = data;

                        break;
                    case DATA_ILLIDAN_STORMRAGE_EVENT: 
                        // m_auiEncounter[8] = data; break;
                        if (m_auiEncounter[8] != DONE)
                            m_auiEncounter[8] = data;

                        break;
                    case DATA_ILLIDAN_GATE_OPENED:
                        if (IllidanGateOpend != DONE)
                            IllidanGateOpend = data;
                        break;
                    case DATA_KICK_PLAYERS:
                        for (auto&& ref : instance->GetPlayers())
                            if (Player* player = ref.GetSource())
                                player->TeleportTo(564, 96.4462f, 1002.35f, -86.9984f, 6.15675f);
                        break;
                }

                if (data == DONE)
                {
                    OUT_SAVE_INST_DATA;

                    std::ostringstream saveStream;
                    saveStream  << "B T " << m_auiEncounter[0] << " " << m_auiEncounter[1]
                    << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4]
                    << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7]
                    << " " << m_auiEncounter[8] << " " << IllidanGateOpend;

                    str_data = saveStream.str();

                    SaveToDB();
                    OUT_SAVE_INST_DATA_COMPLETE;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_HIGH_WARLORD_NAJENTUS_EVENT:         return m_auiEncounter[0];
                    case DATA_SUPREMUS_EVENT:                    return m_auiEncounter[1];
                    case DATA_SHADE_OF_AKAMA_EVENT:                return m_auiEncounter[2];
                    case DATA_TERON_GOREFIEND_EVENT:              return m_auiEncounter[3];
                    case DATA_GURTOGG_BLOOD_BOIL_EVENT:            return m_auiEncounter[4];
                    case DATA_RELIQUARY_OF_SOULS_EVENT:            return m_auiEncounter[5];
                    case DATA_MOTHER_SHAHRAZ_EVENT:               return m_auiEncounter[6];
                    case DATA_ILLIDARI_COUNCIL_EVENT:             return m_auiEncounter[7];
                    case DATA_ILLIDAN_STORMRAGE_EVENT:            return m_auiEncounter[8];
                    case DATA_ILLIDAN_GATE_OPENED:              return IllidanGateOpend;
                }

                return 0;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player) const override
            {
                switch (bossId)
                {
                    // no breaks intended
                    case DATA_ILLIDAN_STORMRAGE_EVENT:
                        if (m_auiEncounter[7] != DONE) // DATA_ILLIDARI_COUNCIL_EVENT
                            return false;
                    case DATA_ILLIDARI_COUNCIL_EVENT:
                        if (m_auiEncounter[6] != DONE) // DATA_MOTHER_SHAHRAZ_EVENT
                            return false;
                    case DATA_MOTHER_SHAHRAZ_EVENT:
                        if (m_auiEncounter[2] != DONE || // DATA_SHADE_OF_AKAMA_EVENT
                            m_auiEncounter[3] != DONE || // DATA_TERON_GOREFIEND_EVENT
                            m_auiEncounter[4] != DONE || // DATA_GURTOGG_BLOOD_BOIL_EVENT
                            m_auiEncounter[5] != DONE)   // DATA_RELIQUARY_OF_SOULS_EVENT
                            return false;
                    case DATA_SHADE_OF_AKAMA_EVENT:
                    case DATA_TERON_GOREFIEND_EVENT:
                    case DATA_GURTOGG_BLOOD_BOIL_EVENT:
                    case DATA_RELIQUARY_OF_SOULS_EVENT:
                        if (m_auiEncounter[1] != DONE) // DATA_SUPREMUS_EVENT
                            return false;
                    case DATA_SUPREMUS_EVENT:
                        if (m_auiEncounter[0] != DONE) // DATA_HIGH_WARLORD_NAJENTUS_EVENT
                            return false;
                    case DATA_HIGH_WARLORD_NAJENTUS_EVENT:
                    default:
                        return true;
                }
            }

            std::string GetSaveData() override
            {
                //return str_data;
                return str_data.c_str();
            }

            bool IsWipe(float range, Unit* source) override
            {
                for (auto&& itr : instance->GetPlayers())
                {
                    if (Player* player = itr.GetSource())
                    {
                        if (source && player->GetExactDist2d(source) > range)
                            continue;

                        if (player->IsAlive() && !player->IsGameMaster())
                            return false;
                    }
                }

                return true;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                uint32 Temp_Encounter[MAX_ENCOUNTER];
                uint32 Temp_IlliGateEvent;
                char dataHead1, dataHead2;

                OUT_LOAD_INST_DATA(in);

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> Temp_Encounter[0] >> Temp_Encounter[1] >> Temp_Encounter[2]
                >> Temp_Encounter[3] >> Temp_Encounter[4] >> Temp_Encounter[5] >> Temp_Encounter[6]
                >> Temp_Encounter[7] >> Temp_Encounter[8] >> Temp_IlliGateEvent;
                //loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2]
                //>> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6]
                //>> m_auiEncounter[7] >> m_auiEncounter[8];

                if (dataHead1 == 'B' && dataHead2 == 'T')
                {
                    for (int i = 0; i < MAX_ENCOUNTER; i++)
                          m_auiEncounter[i] = Temp_Encounter[i];
                    IllidanGateOpend = Temp_IlliGateEvent;
                }

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_black_temple_InstanceMapScript(map);
        }
};


void AddSC_instance_black_temple()
{
    new instance_black_temple();
}
