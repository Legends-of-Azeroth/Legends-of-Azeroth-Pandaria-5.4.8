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
SDName: Instance_Sunwell_Plateau
SD%Complete: 25
SDComment: VERIFY SCRIPT
SDCategory: Sunwell_Plateau
EndScriptData */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

#define MAX_ENCOUNTER 6

/* Sunwell Plateau:
0 - Kalecgos and Sathrovarr
1 - Brutallus
2 - Felmyst
3 - Eredar Twins (Alythess and Sacrolash)
4 - M'uru
5 - Kil'Jaeden
*/

class instance_sunwell_plateau : public InstanceMapScript
{
    public:
        instance_sunwell_plateau() : InstanceMapScript("instance_sunwell_plateau", 580) { }

        struct instance_sunwell_plateau_InstanceMapScript : public InstanceScript
        {
            instance_sunwell_plateau_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_ENCOUNTER];

            // Creatures
            ObjectGuid KalecgosDragonGUID;
            ObjectGuid KalecgosHumanGUID;
            ObjectGuid SathrovarrGUID;
            ObjectGuid BrutallusGUID;
            ObjectGuid MadrigosaGUID;
            ObjectGuid FelmystGUID;
            ObjectGuid AlythessGUID;
            ObjectGuid SacrolashGUID;
            ObjectGuid MuruGUID;
            ObjectGuid KilJaedenGUID;
            ObjectGuid KilJaedenControllerGUID;
            ObjectGuid AnveenaGUID;
            ObjectGuid KalecgosKJGUID;
            uint32 SpectralPlayersGUID;

            // GameObjects
            ObjectGuid ForceFieldGUID;                                      // Kalecgos Encounter
            ObjectGuid OrbsGUIDs[4];
            ObjectGuid KalecgosWall[2];
            ObjectGuid FireBarrierGUID;                                     // Felmysts Encounter
            ObjectGuid MurusGateGUIDs[2];                                   // Murus Encounter

            // Misc
            uint32 SpectralRealmTimer;
            std::vector<uint64> SpectralRealmList;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                // Creatures
                KalecgosDragonGUID = ObjectGuid::Empty;
                KalecgosHumanGUID = ObjectGuid::Empty;
                SathrovarrGUID = ObjectGuid::Empty;
                BrutallusGUID = ObjectGuid::Empty;
                MadrigosaGUID = ObjectGuid::Empty;
                FelmystGUID = ObjectGuid::Empty;
                AlythessGUID = ObjectGuid::Empty;
                SacrolashGUID = ObjectGuid::Empty;
                MuruGUID = ObjectGuid::Empty;
                KilJaedenGUID = ObjectGuid::Empty;
                KilJaedenControllerGUID = ObjectGuid::Empty;
                AnveenaGUID = ObjectGuid::Empty;
                KalecgosKJGUID = ObjectGuid::Empty;
                SpectralPlayersGUID = ObjectGuid::Empty;

                // GameObjects
                ForceFieldGUID = ObjectGuid::Empty;
                OrbsGUIDs[0]               = ObjectGuid::Empty;
                OrbsGUIDs[1]               = ObjectGuid::Empty;
                OrbsGUIDs[2]               = ObjectGuid::Empty;
                OrbsGUIDs[3]               = ObjectGuid::Empty;
                FireBarrierGUID = ObjectGuid::Empty;
                MurusGateGUIDs[0]          = ObjectGuid::Empty;
                MurusGateGUIDs[1]          = ObjectGuid::Empty;
                KalecgosWall[0]            = ObjectGuid::Empty;
                KalecgosWall[1]            = ObjectGuid::Empty;

                // Misc
                SpectralRealmTimer         = 5000;
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        return true;

                return false;
            }

            Player* GetPlayerInMap() const
            {
                for (auto&& itr : instance->GetPlayers())
                {
                    Player* player = itr.GetSource();
                    if (player && !player->HasAura(45839, 0))
                            return player;
                }

                TC_LOG_DEBUG("scripts", "TSCR: Instance Sunwell Plateau: GetPlayerInMap, but PlayerList is empty!");
                return nullptr;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case 24850: KalecgosDragonGUID      = creature->GetGUID(); break;
                    case 24891: KalecgosHumanGUID       = creature->GetGUID(); break;
                    case 24892: SathrovarrGUID          = creature->GetGUID(); break;
                    case 24882: BrutallusGUID           = creature->GetGUID(); break;
                    case 24895:
                        MadrigosaGUID = creature->GetGUID();
                        if (GetData(DATA_FELMYST_EVENT) == DONE)
                            creature->SetVisible(false);
                        else if (GetData(DATA_BRUTALLUS_EVENT) == DONE)
                        {
                            creature->CastSpell(creature, 29266, true); // Permanent Feign Death
                            uint32 delay = 0;
                            creature->m_Events.Schedule(delay += 1000, [creature] { creature->CastSpell(creature, SPELL_SUMMON_FELBLAZE, true); });
                            creature->m_Events.Schedule(delay += 9000, [creature] { creature->SetVisible(false); });
                        }
                        break;
                    case 25038: FelmystGUID             = creature->GetGUID(); break;
                    case 25166: AlythessGUID            = creature->GetGUID(); break;
                    case 25165: SacrolashGUID           = creature->GetGUID(); break;
                    case 25741: MuruGUID                = creature->GetGUID(); break;
                    case 25315: KilJaedenGUID           = creature->GetGUID(); break;
                    case 25608: KilJaedenControllerGUID = creature->GetGUID(); break;
                    case 26046: AnveenaGUID             = creature->GetGUID(); break;
                    case 25319: KalecgosKJGUID          = creature->GetGUID(); break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case 188421: ForceFieldGUID         = go->GetGUID(); break;
                    case 188523: KalecgosWall[0]        = go->GetGUID(); break;
                    case 188524: KalecgosWall[0]        = go->GetGUID(); break;
                    case 188075:
                        if (m_auiEncounter[2] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        FireBarrierGUID = go->GetGUID();
                        break;
                    case 187990: MurusGateGUIDs[0]      = go->GetGUID(); break;
                    case 188118:
                        if (m_auiEncounter[4] == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        MurusGateGUIDs[1]               = go->GetGUID();
                        break;
                    case 187869: OrbsGUIDs[0]           = go->GetGUID(); break;
                    case 188114: OrbsGUIDs[1]           = go->GetGUID(); break;
                    case 188115: OrbsGUIDs[2]           = go->GetGUID(); break;
                    case 188116: OrbsGUIDs[3]           = go->GetGUID(); break;
                }
            }

            uint32 GetData(uint32 id) const override
            {
                switch (id)
                {
                    case DATA_KALECGOS_EVENT:       return m_auiEncounter[0];
                    case DATA_BRUTALLUS_EVENT:      return m_auiEncounter[1];
                    case DATA_FELMYST_EVENT:        return m_auiEncounter[2];
                    case DATA_EREDAR_TWINS_EVENT:   return m_auiEncounter[3];
                    case DATA_MURU_EVENT:           return m_auiEncounter[4];
                    case DATA_KILJAEDEN_EVENT:      return m_auiEncounter[5];
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 id) const override
            {
                switch (id)
                {
                    case DATA_KALECGOS_DRAGON:      return KalecgosDragonGUID;
                    case DATA_KALECGOS_HUMAN:       return KalecgosHumanGUID;
                    case DATA_SATHROVARR:           return SathrovarrGUID;
                    case DATA_GO_FORCEFIELD:        return ForceFieldGUID;
                    case DATA_BRUTALLUS:            return BrutallusGUID;
                    case DATA_MADRIGOSA:            return MadrigosaGUID;
                    case DATA_FELMYST:              return FelmystGUID;
                    case DATA_ALYTHESS:             return AlythessGUID;
                    case DATA_SACROLASH:            return SacrolashGUID;
                    case DATA_MURU:                 return MuruGUID;
                    case DATA_KILJAEDEN:            return KilJaedenGUID;
                    case DATA_KILJAEDEN_CONTROLLER: return KilJaedenControllerGUID;
                    case DATA_ANVEENA:              return AnveenaGUID;
                    case DATA_KALECGOS_KJ:          return KalecgosKJGUID;
                    case DATA_PLAYER_GUID:
                    {
                        Player* target = GetPlayerInMap();
                        return target ? target->GetGUID() : ObjectGuid::Empty;
                    }
                    case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1:
                    case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2:
                    case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3:
                    case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4:
                        return OrbsGUIDs[id - DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1];
                }
                return ObjectGuid::Empty;
            }

            void SetData(uint32 id, uint32 data) override
            {
                switch (id)
                {
                    case DATA_KALECGOS_EVENT:
                        {
                            if (data == NOT_STARTED || data == DONE)
                            {
                                HandleGameObject(ForceFieldGUID,true);
                                HandleGameObject(KalecgosWall[0],true);
                                HandleGameObject(KalecgosWall[1],true);
                            }
                            else if (data == IN_PROGRESS)
                            {
                                HandleGameObject(ForceFieldGUID,false);
                                HandleGameObject(KalecgosWall[0],false);
                                HandleGameObject(KalecgosWall[1],false);
                            }
                            m_auiEncounter[0] = data;
                        }
                        break;
                    case DATA_BRUTALLUS_EVENT:
                        m_auiEncounter[1] = data;
                        break;
                    case DATA_FELMYST_EVENT:
                        if (data == DONE)
                            HandleGameObject(FireBarrierGUID, true);
                        m_auiEncounter[2] = data; break;
                    case DATA_EREDAR_TWINS_EVENT:
                        m_auiEncounter[3] = data;
                        break;
                    case DATA_MURU_EVENT:
                        switch (data)
                        {
                            case DONE:
                                HandleGameObject(MurusGateGUIDs[0], true);
                                HandleGameObject(MurusGateGUIDs[1], true);
                                break;
                            case IN_PROGRESS:
                                HandleGameObject(MurusGateGUIDs[0], false);
                                HandleGameObject(MurusGateGUIDs[1], false);
                                break;
                            case NOT_STARTED:
                                HandleGameObject(MurusGateGUIDs[0], true);
                                HandleGameObject(MurusGateGUIDs[1], false);
                                break;
                        }
                        m_auiEncounter[4] = data; break;
                    case DATA_KILJAEDEN_EVENT:     m_auiEncounter[5] = data; break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            void Update(uint32 /*diff*/) override
            {
                if (GetData(DATA_FELMYST_EVENT) != DONE)
                    for (auto&& ref : instance->GetPlayers())
                        if (Player* player = ref.GetSource())
                            if (!player->IsGameMaster())
                                if (player->GetPositionX() >= 1519.0f && player->GetPositionY() >= 474.0f &&
                                    player->GetPositionX() <= 1605.0f && player->GetPositionY() <= 516.0f)
                                    player->Kill(player);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;
                std::ostringstream stream;
                stream << m_auiEncounter[0] << " "  << m_auiEncounter[1] << " "  << m_auiEncounter[2] << " "  << m_auiEncounter[3] << " "
                    << m_auiEncounter[4] << " "  << m_auiEncounter[5];
                char* out = new char[stream.str().length() + 1];
                strcpy(out, stream.str().c_str());
                if (out)
                {
                    OUT_SAVE_INST_DATA_COMPLETE;
                    return out;
                }
                return nullptr;
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);
                std::istringstream stream(in);
                stream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
                    >> m_auiEncounter[4] >> m_auiEncounter[5];
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)                // Do not load an encounter as "In Progress" - reset it instead.
                        m_auiEncounter[i] = NOT_STARTED;
                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_sunwell_plateau_InstanceMapScript(map);
        }
};


void AddSC_instance_sunwell_plateau()
{
    new instance_sunwell_plateau();
}
