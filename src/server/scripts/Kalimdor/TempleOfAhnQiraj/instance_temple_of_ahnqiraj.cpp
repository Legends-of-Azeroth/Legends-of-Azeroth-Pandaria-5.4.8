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
#include "temple_of_ahnqiraj.h"

static std::vector<DoorData> const doorData =
{
    { GO_SKERAM_EXIT_DOOR,       DATA_SKERAM,        DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_TWIN_EMPEROR_EXIT_DOOR, DATA_TWIN_EMPERORS, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { 0,                         0,                  DOOR_TYPE_ROOM,    BOUNDARY_NONE }, // END
};

class instance_temple_of_ahnqiraj : public InstanceMapScript
{
    public:
        instance_temple_of_ahnqiraj() : InstanceMapScript("instance_temple_of_ahnqiraj", 531) { }

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_temple_of_ahnqiraj_InstanceMapScript(map);
        }

        struct instance_temple_of_ahnqiraj_InstanceMapScript : public InstanceScript
        {
            instance_temple_of_ahnqiraj_InstanceMapScript(Map* map) : InstanceScript(map) { }

            //If Vem is dead...
            bool IsBossDied[3];

            //Storing Skeram, Vem and Kri.
            ObjectGuid SkeramGUID;
            ObjectGuid VemGUID;
            ObjectGuid KriGUID;
            ObjectGuid VeklorGUID;
            ObjectGuid VeknilashGUID;
            ObjectGuid ViscidusGUID;

            uint32 BugTrioDeathCount;

            uint32 CthunPhase;

            void Initialize() override
            {
                SetBossNumber(ENCOUNTERS);
                LoadDoorData(doorData);

                IsBossDied[0] = false;
                IsBossDied[1] = false;
                IsBossDied[2] = false;

                SkeramGUID = ObjectGuid::Empty;
                VemGUID = ObjectGuid::Empty;
                KriGUID = ObjectGuid::Empty;
                VeklorGUID = ObjectGuid::Empty;
                VeknilashGUID = ObjectGuid::Empty;
                ViscidusGUID = ObjectGuid::Empty;

                BugTrioDeathCount = 0;

                CthunPhase = 0;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SKERAM:
                        SkeramGUID = creature->GetGUID();
                        break;
                    case NPC_VEM:
                        VemGUID = creature->GetGUID();
                        break;
                    case NPC_KRI:
                        KriGUID = creature->GetGUID();
                        break;
                    case NPC_VEKLOR:
                        VeklorGUID = creature->GetGUID();
                        break;
                    case NPC_VEKNILASH:
                        VeknilashGUID = creature->GetGUID();
                        break;
                    case NPC_VISCIDUS:
                        ViscidusGUID = creature->GetGUID();
                        break;
                }
            }

            bool IsEncounterInProgress() const override
            {
                //not active in AQ40
                return false;
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_SKERAM_EXIT_DOOR:
                    case GO_TWIN_EMPEROR_EXIT_DOOR:
                        AddDoor(go, true);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {    
                    case DATA_BUG_TRIO_DEATH:
                        return BugTrioDeathCount;      
                    case DATA_CTHUN_PHASE:
                        return CthunPhase;
                }
                return 0;
            }

            ObjectGuid GetGuidData(uint32 identifier) const override
            {
                switch (identifier)
                {
                    case DATA_SKERAM:
                        return SkeramGUID;
                    case DATA_VEM:
                        return VemGUID;
                    case DATA_KRI:
                        return KriGUID;
                    case NPC_VEKLOR:
                        return VeklorGUID;
                    case NPC_VEKNILASH:
                        return VeknilashGUID;
                    case DATA_VISCIDUS:
                        return ViscidusGUID;
                }
                return ObjectGuid::Empty;
            }                                                       // end GetData64

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_VEM_DEATH:
                        IsBossDied[0] = true;
                        break;     
                    case DATA_BUG_TRIO_DEATH:
                        ++BugTrioDeathCount;

                        if (BugTrioDeathCount >= 3 && GetBossState(DATA_BUG_TRIO_DEATH) != DONE)
                            SetBossState(DATA_BUG_TRIO_DEATH, DONE);
                        break;
                    case DATA_CTHUN_PHASE:
                        CthunPhase = data;
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "T A Q " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
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

                if (dataHead1 == 'T' && dataHead2 == 'A' && dataHead3 == 'Q')
                {
                    for (uint8 i = 0; i < ENCOUNTERS; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

};

void AddSC_instance_temple_of_ahnqiraj()
{
    new instance_temple_of_ahnqiraj();
}
