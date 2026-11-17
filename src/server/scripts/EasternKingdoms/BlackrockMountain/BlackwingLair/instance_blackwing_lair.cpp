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
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "blackwing_lair.h"
#include "Player.h"

/*
Blackwing Lair Encounter:
1 - boss_razorgore.cpp
2 - boss_vaelastrasz.cpp
3 - boss_broodlord_lashlayer.cpp
4 - boss_firemaw.cpp
5 - boss_ebonroc.cpp
6 - boss_flamegor.cpp
7 - boss_chromaggus.cpp
8 - boss_nefarian.cpp
*/

Position const SummonPosition[8] =
{
    {-7661.207520f, -1043.268188f, 407.199554f, 6.280452f},
    {-7644.145020f, -1065.628052f, 407.204956f, 0.501492f},
    {-7624.260742f, -1095.196899f, 407.205017f, 0.544694f},
    {-7608.501953f, -1116.077271f, 407.199921f, 0.816443f},
    {-7531.841797f, -1063.765381f, 407.199615f, 2.874187f},
    {-7547.319336f, -1040.971924f, 407.205078f, 3.789175f},
    {-7568.547852f, -1013.112488f, 407.204926f, 3.773467f},
    {-7584.175781f, -989.6691289f, 407.199585f, 4.527447f},
};

uint32 const Entry[5] = {12422, 12458, 12416, 12420, 12459};

static std::vector<DoorData> const doorData =
{
    { GO_PORTCULLIS_RAZORGORE_ENT, BOSS_RAZORGORE,  DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_PORTCULLIS_RAZORGORE,     BOSS_RAZORGORE,  DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_PORTCULLIS_LASHLAYER,     BOSS_BROODLORD,  DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_PORTCULLIS_FLAMEGOR,      BOSS_FLAMEGOR,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_PORTCULLIS_CHROMAGGUS,    BOSS_CHROMAGGUS, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_PORTCULLIS_NEFARIAN,      BOSS_NEFARIAN,   DOOR_TYPE_ROOM,    BOUNDARY_NONE },
};

class instance_blackwing_lair : public InstanceMapScript
{
    public:
        instance_blackwing_lair() : InstanceMapScript(BRLScriptName, 469) { }

        struct instance_blackwing_lair_InstanceMapScript : public InstanceScript
        {
            instance_blackwing_lair_InstanceMapScript(Map* map) : InstanceScript(map) { }
            uint32 m_auiEncounter[MAX_TYPES];

            std::list<ObjectGuid> EggList;
            // Misc
            EventMap _events;
            // Razorgore
            uint8 EggCount;
            uint32 EggEvent;
            ObjectGuid RazorgoreTheUntamedGUID;

            // Vaelastrasz the Corrupt
            ObjectGuid VaelastraszTheCorruptGUID;

            // Broodlord Lashlayer
            ObjectGuid BroodlordLashlayerGUID;

            // 3 Dragons
            ObjectGuid FiremawGUID;
            ObjectGuid EbonrocGUID;
            ObjectGuid FlamegorGUID;

            // Chormaggus
            ObjectGuid ChromaggusGUID;

            // Nefarian
            ObjectGuid LordVictorNefariusGUID;
            ObjectGuid NefarianGUID;

            void Initialize() override
            {
                SetBossNumber(MAX_TYPES);
                LoadDoorData(doorData);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
                // Razorgore
                EggCount = 0;
                RazorgoreTheUntamedGUID = ObjectGuid::Empty;
                EggList.clear();
                // Vaelastrasz the Corrupt
                VaelastraszTheCorruptGUID = ObjectGuid::Empty;
                // Broodlord Lashlayer
                BroodlordLashlayerGUID = ObjectGuid::Empty;
                // 3 Dragons
                FiremawGUID = ObjectGuid::Empty;
                EbonrocGUID = ObjectGuid::Empty;
                FlamegorGUID = ObjectGuid::Empty;
                // Chormaggus
                ChromaggusGUID = ObjectGuid::Empty;
                // Nefarian
                LordVictorNefariusGUID = ObjectGuid::Empty;
                NefarianGUID = ObjectGuid::Empty;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_RAZORGORE:
                        RazorgoreTheUntamedGUID = creature->GetGUID();
                        break;
                    case NPC_BLACKWING_DRAGON:
                    case NPC_BLACKWING_TASKMASTER:
                    case NPC_BLACKWING_LEGIONAIRE:
                    case NPC_BLACKWING_WARLOCK:
                        if (Creature* razor = instance->GetCreature(RazorgoreTheUntamedGUID))
                            razor->AI()->JustSummoned(creature);
                        break;
                    case NPC_VAELASTRAZ:
                        VaelastraszTheCorruptGUID = creature->GetGUID();
                        break;
                    case NPC_BROODLORD:
                        BroodlordLashlayerGUID = creature->GetGUID();
                        break;
                    case NPC_FIRENAW:
                        FiremawGUID = creature->GetGUID();
                        break;
                    case NPC_EBONROC:
                        EbonrocGUID = creature->GetGUID();
                        break;
                    case NPC_FLAMEGOR:
                        FlamegorGUID = creature->GetGUID();
                        break;
                    case NPC_CHROMAGGUS:
                        ChromaggusGUID = creature->GetGUID();
                        break;
                    case NPC_VICTOR_NEFARIUS:
                        LordVictorNefariusGUID = creature->GetGUID();
                        break;
                    case NPC_NEFARIAN:
                        NefarianGUID = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_BLACK_DRAGON_EGG: // Egg
                        GetBossState(BOSS_FIREMAW) == DONE ? go->Delete() : EggList.push_back(go->GetGUID());
                        break;
                    case GO_PORTCULLIS_RAZORGORE_ENT:
                    case GO_PORTCULLIS_RAZORGORE:
                    case GO_PORTCULLIS_LASHLAYER:
                    case GO_PORTCULLIS_FLAMEGOR:
                    case GO_PORTCULLIS_CHROMAGGUS:
                    case GO_PORTCULLIS_NEFARIAN:
                        AddDoor(go, true);
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                if (go->GetEntry() == GO_BLACK_DRAGON_EGG) // Egg
                    EggList.remove(go->GetGUID());
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case BOSS_RAZORGORE:
                        if (state == DONE)
                            for (auto &&eggItr : EggList)
                                if (GameObject* egg = instance->GetGameObject(eggItr))
                                    egg->SetPhaseMask(2, true);
                        break;
                    case BOSS_NEFARIAN:
                        switch (state)
                        {
                            case NOT_STARTED:
                                if (Creature* nefarian = instance->GetCreature(NefarianGUID))
                                    nefarian->DespawnOrUnsummon();
                                break;
                            case FAIL:
                                _events.ScheduleEvent(EVENT_RESPAWN_NEFARIUS, 15 * IN_MILLISECONDS * MINUTE);
                                SetBossState(BOSS_NEFARIAN, NOT_STARTED);
                                break;
                            case DONE:
                                if (Creature* nefarius = instance->GetCreature(LordVictorNefariusGUID))
                                    nefarius->DespawnOrUnsummon();
                                break;
                            default:
                                break;
                        }
                        break;
                }
                return true;
            }

            ObjectGuid GetGuidData(uint32 id) const override
            {
                switch (id)
                {
                    case DATA_RAZORGORE_THE_UNTAMED:  return RazorgoreTheUntamedGUID;
                    case DATA_VAELASTRAZ_THE_CORRUPT: return VaelastraszTheCorruptGUID;
                    case DATA_BROODLORD_LASHLAYER:    return BroodlordLashlayerGUID;
                    case DATA_FIRENAW:                return FiremawGUID;
                    case DATA_EBONROC:                return EbonrocGUID;
                    case DATA_FLAMEGOR:               return FlamegorGUID;
                    case DATA_CHROMAGGUS:             return ChromaggusGUID;
                    case DATA_LORD_VICTOR_NEFARIUS:   return LordVictorNefariusGUID;
                    case DATA_NEFARIAN:               return NefarianGUID;
                }

                return ObjectGuid::Empty;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_EGG_EVENT)
                {
                    switch (data)
                    {
                        case IN_PROGRESS:
                            _events.ScheduleEvent(EVENT_RAZOR_SPAWN, 45*IN_MILLISECONDS);
                            EggEvent = data;
                            EggCount = 0;
                            break;
                        case NOT_STARTED:
                            _events.CancelEvent(EVENT_RAZOR_SPAWN);
                            EggEvent = data;
                            EggCount = 0;
                            break;
                        case SPECIAL:
                            if (++EggCount == 15)
                            {
                                if (Creature* razor = instance->GetCreature(RazorgoreTheUntamedGUID))
                                {
                                    SetData(DATA_EGG_EVENT, DONE);
                                    razor->RemoveAurasDueToSpell(42013); // MindControl
                                    DoRemoveAurasDueToSpellOnPlayers(42013);
                                }
                                _events.ScheduleEvent(EVENT_RAZOR_PHASE_TWO, IN_MILLISECONDS);
                                _events.CancelEvent(EVENT_RAZOR_SPAWN);
                            }
                            if (EggEvent == NOT_STARTED)
                                SetData(DATA_EGG_EVENT, IN_PROGRESS);
                            break;
                    }
                }
            }

            void Update(uint32 diff) override
            {
                if (_events.Empty())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RAZOR_SPAWN:
                            for (uint8 i = urand(2, 5); i > 0 ; --i)
                                if (Creature* summon =  instance->SummonCreature(Entry[urand(0, 4)], SummonPosition[urand(0, 7)]))
                                    summon->SetInCombatWithZone();

                            _events.ScheduleEvent(EVENT_RAZOR_SPAWN, urand(12 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                            break;
                        case EVENT_RAZOR_PHASE_TWO:
                            _events.CancelEvent(EVENT_RAZOR_SPAWN);

                            if (Creature* razor = instance->GetCreature(RazorgoreTheUntamedGUID))
                                razor->AI()->DoAction(ACTION_PHASE_TWO);
                            break;
                        case EVENT_RESPAWN_NEFARIUS:
                            if (Creature* nefarius = instance->GetCreature(LordVictorNefariusGUID))
                            {
                                nefarius->SetPhaseMask(1, true);
                                nefarius->setActive(true);
                                nefarius->Respawn();
                                nefarius->GetMotionMaster()->MoveTargetedHome();
                            }
                            break;
                    }
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "B W L " << GetBossSaveData();

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

                if (dataHead1 == 'B' && dataHead2 == 'W' && dataHead3 == 'L')
                {
                    for (uint8 i = 0; i < MAX_TYPES; ++i)
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

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_blackwing_lair_InstanceMapScript(map);
        }
};

void AddSC_instance_blackwing_lair()
{
    new instance_blackwing_lair();
}
