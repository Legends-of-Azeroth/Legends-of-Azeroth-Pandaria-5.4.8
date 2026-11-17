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

/*
This placeholder for the instance is needed for dungeon finding to be able
to give credit after the boss defined in lastEncounterDungeon is killed.
Without it, the party doing random dungeon won't get satchel of spoils and
gets instead the deserter debuff.
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "dire_maul.h"

class instance_dire_maul : public InstanceMapScript
{
    public:
        instance_dire_maul() : InstanceMapScript("instance_dire_maul", 429) { }

        struct instance_dire_maul_InstanceMapScript : public InstanceScript
        {
            instance_dire_maul_InstanceMapScript(Map* map) : InstanceScript(map) { }

            ObjectGuid conservatoryDoorGUID;

            void Initialize() override
            {
                conservatoryDoorGUID = ObjectGuid::Empty;
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                if (go->GetEntry() == GO_CONSERVATORY_DOOR)
                    conservatoryDoorGUID = go->GetGUID();
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                if (type == GO_CONSERVATORY_DOOR)
                    return conservatoryDoorGUID;

                return ObjectGuid::Empty;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_dire_maul_InstanceMapScript(map);
        }
};

void AddSC_instance_dire_maul()
{
    new instance_dire_maul();
}
