/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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
#include "gundrak.h"

#define MAX_ENCOUNTER     5

/* GunDrak encounters:
 0 - Slad'Ran
 1 - Moorabi
 2 - Drakkari Colossus
 3 - Gal'Darah
 4 - Eck the Ferocious
*/

class instance_gundrak : public InstanceMapScript
{
    public:
        instance_gundrak() : InstanceMapScript("instance_gundrak", 604) { }

        struct instance_gundrak_InstanceMapScript : public InstanceScript
        {
            instance_gundrak_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                bHeroicMode = map->IsHeroic();
            }

            bool bHeroicMode;
            bool spawnSupport;

            uint32 timer;
            uint32 phase;
            ObjectGuid toActivate;

            ObjectGuid uiSladRan;
            ObjectGuid uiMoorabi;
            ObjectGuid uiDrakkariColossus;
            ObjectGuid uiGalDarah;
            ObjectGuid uiEckTheFerocious;

            ObjectGuid uiSladRanAltar;
            ObjectGuid uiMoorabiAltar;
            ObjectGuid uiDrakkariColossusAltar;
            ObjectGuid uiSladRanStatue;
            ObjectGuid uiMoorabiStatue;
            ObjectGuid uiDrakkariColossusStatue;
            ObjectGuid uiGalDarahStatue;
            ObjectGuid uiEckTheFerociousDoor;
            ObjectGuid uiEckTheFerociousDoorBehind;
            ObjectGuid uiGalDarahDoor;
            ObjectGuid uiGalDarahDoor1;
            ObjectGuid uiGalDarahDoor2;
            ObjectGuid uiBridge;
            ObjectGuid uiCollision;
            ObjectGuid middleGalDarahRhinoGuid;
            ObjectGuid frontGalDarahRhinoGuid;

            uint32 m_auiEncounter[MAX_ENCOUNTER];

            GOState uiSladRanStatueState;
            GOState uiMoorabiStatueState;
            GOState uiDrakkariColossusStatueState;
            GOState uiGalDarahStatueState;
            GOState uiBridgeState;
            GOState uiCollisionState;

            std::set<uint64> DwellerGUIDs;
            std::vector<ObjectGuid> LivingMojoGUIDs;

            std::string str_data;

            void Initialize() override
            {
                spawnSupport = false;

                timer = 0;
                phase = 0;
                toActivate = ObjectGuid::Empty;

                uiSladRan = ObjectGuid::Empty;
                uiMoorabi = ObjectGuid::Empty;
                uiDrakkariColossus = ObjectGuid::Empty;
                uiGalDarah = ObjectGuid::Empty;
                uiEckTheFerocious = ObjectGuid::Empty;

                uiSladRanAltar = ObjectGuid::Empty;
                uiMoorabiAltar = ObjectGuid::Empty;
                uiDrakkariColossusAltar = ObjectGuid::Empty;

                uiSladRanStatue = ObjectGuid::Empty;
                uiMoorabiStatue = ObjectGuid::Empty;
                uiDrakkariColossusStatue = ObjectGuid::Empty;
                uiGalDarahStatue = ObjectGuid::Empty;

                uiEckTheFerociousDoor = ObjectGuid::Empty;
                uiEckTheFerociousDoorBehind = ObjectGuid::Empty;
                uiGalDarahDoor1 = ObjectGuid::Empty;
                uiGalDarahDoor2 = ObjectGuid::Empty;

                uiBridge = ObjectGuid::Empty;
                uiCollision = ObjectGuid::Empty;

                uiSladRanStatueState = GO_STATE_ACTIVE;
                uiMoorabiStatueState = GO_STATE_ACTIVE;
                uiDrakkariColossusStatueState = GO_STATE_ACTIVE;
                uiGalDarahStatueState = GO_STATE_READY;
                uiBridgeState = GO_STATE_ACTIVE;
                uiCollisionState = GO_STATE_READY;

                DwellerGUIDs.clear();
                LivingMojoGUIDs.clear();

                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
            }

           bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

            uint32 GetCreatureEntry(uint32 guidlow, CreatureData const* data) override
            {
                switch (data->id)
                {
                    case NPC_DRAKKARI_LANCER:
                    case NPC_DRAKKARI_GOD_HUNTER:
                    case NPC_DRAKKARI_FIRE_WEAVER:
                    case NPC_DRAKKARI_MEDICINE_MAN:
                        return RAND(NPC_DRAKKARI_LANCER, NPC_DRAKKARI_GOD_HUNTER, NPC_DRAKKARI_FIRE_WEAVER, NPC_DRAKKARI_MEDICINE_MAN);
                    default:
                        return data->id;
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_SLAD_RAN: uiSladRan = creature->GetGUID(); break;
                    case NPC_MOORABI: uiMoorabi = creature->GetGUID(); break;
                    case NPC_GALDARAH: uiGalDarah = creature->GetGUID(); break;
                    case NPC_DRAKKARICOLOSSUS: uiDrakkariColossus = creature->GetGUID(); break;
                    case NPC_ECK: uiEckTheFerocious = creature->GetGUID(); break;
                    case NPC_RUIN_DWELLER:
                        if (creature->IsAlive())
                            DwellerGUIDs.insert(creature->GetGUID());
                        break;
                    case NPC_LIVING_MOJO:
                        if (creature->IsAlive())
                            if (Creature* pDrakkari = instance->GetCreature(uiDrakkariColossus))
                                if (creature->IsWithinDist(pDrakkari, 20.0f))
                                    LivingMojoGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_DRAKKARI_RHINO:
                        if (creature->GetDBTableGUIDLow() == 509819)
                            middleGalDarahRhinoGuid = creature->GetGUID();
                        break;
                    case NPC_DRAKKARI_RHINO_2:
                        if (creature->GetDBTableGUIDLow() == 509817)
                            frontGalDarahRhinoGuid = creature->GetGUID();
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_ALTAR_OF_SLADRAN:
                        uiSladRanAltar = go->GetGUID();
                        // Make sure that they start out as unusuable
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                        if (m_auiEncounter[0] == DONE)
                        {
                            if (uiSladRanStatueState == GO_STATE_ACTIVE)
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                            else
                            {
                                ++phase;
                                go->SetGoState(GO_STATE_ACTIVE);
                            }
                        }
                        break;
                    case GO_ALTAR_OF_MOORABI:
                        uiMoorabiAltar = go->GetGUID();
                        // Make sure that they start out as unusuable
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                        if (m_auiEncounter[0] == DONE)
                        {
                            if (uiMoorabiStatueState == GO_STATE_ACTIVE)
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                            else
                            {
                                ++phase;
                                go->SetGoState(GO_STATE_ACTIVE);
                            }
                        }
                        break;
                    case GO_ALTAR_OF_DRAKKARI_COLOSSUS:
                        uiDrakkariColossusAltar = go->GetGUID();
                        // Make sure that they start out as unusuable
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                        if (m_auiEncounter[0] == DONE)
                        {
                            if (uiDrakkariColossusStatueState == GO_STATE_ACTIVE)
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                            else
                            {
                                ++phase;
                                go->SetGoState(GO_STATE_ACTIVE);
                            }
                        }
                        break;
                    case GO_SLADRAN_STATUE:
                        uiSladRanStatue = go->GetGUID();
                        go->SetGoState(uiSladRanStatueState);
                        break;
                    case GO_MOORABI_STATUE:
                        uiMoorabiStatue = go->GetGUID();
                        go->SetGoState(uiMoorabiStatueState);
                        break;
                    case GO_GALDARAH_STATUE:
                        uiGalDarahStatue = go->GetGUID();
                        go->SetGoState(uiGalDarahStatueState);
                        break;
                    case GO_DRAKKARI_COLOSSUS_STATUE:
                        uiDrakkariColossusStatue = go->GetGUID();
                        go->SetGoState(uiDrakkariColossusStatueState);
                        break;
                    case GO_ECK_DOOR:
                        uiEckTheFerociousDoor = go->GetGUID();
                        if (bHeroicMode && m_auiEncounter[1] == DONE)
                            HandleGameObject(ObjectGuid::Empty,true,go);
                        break;
                    case GO_ECK_DOOR_BEHIND:
                        uiEckTheFerociousDoorBehind = go->GetGUID();
                        if (bHeroicMode && m_auiEncounter[4] == DONE)
                            HandleGameObject(ObjectGuid::Empty,true,go);
                    case GO_GALDARAH_DOOR:
                        uiGalDarahDoor = go->GetGUID();
                        if (m_auiEncounter[3] == IN_PROGRESS)
                            HandleGameObject(ObjectGuid::Empty, false, go);
                        break;
                    case GO_GALDARAH_DOOR_1:
                        uiGalDarahDoor1 = go->GetGUID();
                        if (m_auiEncounter[3] == DONE)
                            HandleGameObject(ObjectGuid::Empty,true,go);
                        break;
                    case GO_GALDARAH_DOOR_2:
                        uiGalDarahDoor2 = go->GetGUID();
                        if (m_auiEncounter[3] == DONE)
                            HandleGameObject(ObjectGuid::Empty,true,go);
                        break;
                    case GO_BRIDGE:
                        uiBridge = go->GetGUID();
                        go->SetGoState(uiBridgeState);
                        break;
                    case GO_COLLISION:
                        uiCollision = go->GetGUID();
                        go->SetGoState(uiCollisionState);

                        // Can't spawn here with SpawnGameObject because go isn't added to world yet...
                        if (uiCollisionState == GO_STATE_ACTIVE_ALTERNATIVE)
                            spawnSupport = true;
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_SLAD_RAN_EVENT:
                        m_auiEncounter[0] = data;
                        if (data == DONE)
                        {
                          GameObject* go = instance->GetGameObject(uiSladRanAltar);
                          if (go)
                              go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                        }
                        break;
                    case DATA_MOORABI_EVENT:
                        m_auiEncounter[1] = data;
                        if (data == DONE)
                        {
                          GameObject* go = instance->GetGameObject(uiMoorabiAltar);
                          if (go)
                              go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                          if (bHeroicMode)
                              HandleGameObject(uiEckTheFerociousDoor,true);
                        }
                        break;
                    case DATA_DRAKKARI_COLOSSUS_EVENT:
                        m_auiEncounter[2] = data;
                        if (data == DONE)
                        {
                          if (GameObject* go = instance->GetGameObject(uiDrakkariColossusAltar))
                              go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS,GO_FLAG_NOT_SELECTABLE);
                        }
                        else if (data == NOT_STARTED)
                        {
                            for (auto&& guid : LivingMojoGUIDs)
                                if (Creature* pLivingMojo = instance->GetCreature(guid))
                                    pLivingMojo->Respawn();
                        }
                        break;
                    case DATA_GAL_DARAH_EVENT:
                        m_auiEncounter[3] = data;
                        HandleGameObject(uiGalDarahDoor, data != IN_PROGRESS);
                        if (data == DONE)
                        {
                            HandleGameObject(uiGalDarahDoor1,true);
                            HandleGameObject(uiGalDarahDoor2,true);
                        }
                        break;
                    case DATA_ECK_THE_FEROCIOUS_EVENT:
                        m_auiEncounter[4] = data;
                        if (bHeroicMode && data == DONE)
                            HandleGameObject(uiEckTheFerociousDoorBehind,true);
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                if (type == DATA_RUIN_DWELLER_DIED)
                    DwellerGUIDs.erase(data);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SLAD_RAN_EVENT:             return m_auiEncounter[0];
                    case DATA_MOORABI_EVENT:              return m_auiEncounter[1];
                    case DATA_GAL_DARAH_EVENT:            return m_auiEncounter[2];
                    case DATA_DRAKKARI_COLOSSUS_EVENT:    return m_auiEncounter[3];
                    case DATA_ECK_THE_FEROCIOUS_EVENT:    return m_auiEncounter[4];
                    case DATA_ALIVE_RUIN_DWELLERS:        return DwellerGUIDs.size();
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SLAD_RAN_ALTAR:             return uiSladRanAltar;
                    case DATA_MOORABI_ALTAR:              return uiMoorabiAltar;
                    case DATA_DRAKKARI_COLOSSUS_ALTAR:    return uiDrakkariColossusAltar;
                    case DATA_SLAD_RAN_STATUE:            return uiSladRanStatue;
                    case DATA_MOORABI_STATUE:             return uiMoorabiStatue;
                    case DATA_DRAKKARI_COLOSSUS_STATUE:   return uiDrakkariColossusStatue;
                    case DATA_DRAKKARI_COLOSSUS:          return uiDrakkariColossus;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "G D " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                     << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " "
                     << (uiSladRanStatue ? GetObjState(uiSladRanStatue) : GO_STATE_ACTIVE) << " " << (uiMoorabiStatue ? GetObjState(uiMoorabiStatue) : GO_STATE_ACTIVE) << " "
                     << (uiDrakkariColossusStatue ? GetObjState(uiDrakkariColossusStatue) : GO_STATE_ACTIVE) << " " << (uiGalDarahStatue ? GetObjState(uiGalDarahStatue) : GO_STATE_READY) << " "
                     << (uiBridge ? GetObjState(uiBridge) : GO_STATE_ACTIVE) << " " << (uiCollision ? GetObjState(uiCollision) : GO_STATE_READY);


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
                uint16 data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3
                           >> data4 >> data5 >> data6 >> data7 >> data8 >> data9 >> data10;

                if (dataHead1 == 'G' && dataHead2 == 'D')
                {
                    m_auiEncounter[0] = data0;
                    m_auiEncounter[1] = data1;
                    m_auiEncounter[2] = data2;
                    m_auiEncounter[3] = data3;
                    m_auiEncounter[4] = data4;
                    uiSladRanStatueState            = GOState(data5);
                    uiMoorabiStatueState            = GOState(data6);
                    uiDrakkariColossusStatueState   = GOState(data7);
                    uiGalDarahStatueState           = GOState(data8);
                    uiBridgeState                   = GOState(data9);
                    uiCollisionState                = GOState(data10);

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (m_auiEncounter[i] == IN_PROGRESS)
                            m_auiEncounter[i] = NOT_STARTED;
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

             bool QueueActivation(ObjectGuid guid, uint32 time)
             {
                 if (timer)
                     return false;

                 toActivate = guid;
                 timer = time;
                 phase++;
                 return true;
             }

             void Update(uint32 diff) override
             {
                 // Spawn the support for the bridge if necessary
                 if (spawnSupport)
                 {
                     if (GameObject* pCollision = instance->GetGameObject(uiCollision))
                         pCollision->SummonGameObject(192743, pCollision->GetPositionX(), pCollision->GetPositionY(), pCollision->GetPositionZ(), pCollision->GetOrientation(), { }, 0);

                     if (Creature* rhino = instance->GetCreature(middleGalDarahRhinoGuid))
                         rhino->GetMotionMaster()->MovePath(rhino->GetDBTableGUIDLow() * 10, false);

                     if (Creature* rhino = instance->GetCreature(frontGalDarahRhinoGuid))
                         rhino->GetMotionMaster()->MovePath(rhino->GetDBTableGUIDLow() * 10, false);

                     spawnSupport = false;
                 }

                 // If there is nothing to activate, then return
                 if (!toActivate)
                     return;

                 if (timer < diff)
                 {
                     timer = 0;
                     if (toActivate == uiBridge)
                     {
                         toActivate = ObjectGuid::Empty;
                         GameObject* pBridge = instance->GetGameObject(uiBridge);
                         GameObject* pCollision = instance->GetGameObject(uiCollision);
                         GameObject* pSladRanStatue = instance->GetGameObject(uiSladRanStatue);
                         GameObject* pMoorabiStatue = instance->GetGameObject(uiMoorabiStatue);
                         GameObject* pDrakkariColossusStatue = instance->GetGameObject(uiDrakkariColossusStatue);
                         GameObject* pGalDarahStatue = instance->GetGameObject(uiGalDarahStatue);

                         if (pBridge && pCollision && pSladRanStatue && pMoorabiStatue && pDrakkariColossusStatue && pGalDarahStatue)
                         {
                             pBridge->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                             pCollision->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                             pSladRanStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                             pMoorabiStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                             pDrakkariColossusStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                             pGalDarahStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                             // Add the GO that solidifies the bridge so you can walk on it
                             spawnSupport = true;
                             SaveToDB();
                         }
                     }
                     else
                     {
                         uint32 spell = 0;
                         GameObject* pAltar = NULL;
                         if (toActivate == uiSladRanStatue)
                         {
                             spell = 57071;
                             pAltar = instance->GetGameObject(uiSladRanAltar);
                         } else if (toActivate == uiMoorabiStatue)
                         {
                             spell = 57068;
                             pAltar = instance->GetGameObject(uiMoorabiAltar);
                         } else if (toActivate == uiDrakkariColossusStatue)
                         {
                             spell = 57072;
                             pAltar = instance->GetGameObject(uiDrakkariColossusAltar);
                         }

                         // This is a workaround to make the beam cast properly. The caster should be ID 30298 but since the spells
                         // all are with scripted target for that same ID, it will hit itself.
                         if (pAltar)
                             if (Creature* trigger = pAltar->FindNearestCreature(30298, 10.0f))
                                 trigger->CastSpell(trigger, spell, false);

                         if (GameObject* statueGO = instance->GetGameObject(toActivate))
                             statueGO->SetGoState(GO_STATE_READY);

                         toActivate = ObjectGuid::Empty;

                         if (phase == 3)
                             QueueActivation(uiBridge, 3000);
                         else
                             SaveToDB(); // Don't save in between last statue and bridge turning in case of crash leading to stuck instance
                     }
                 } else timer -= diff;
            }

             GOState GetObjState(ObjectGuid guid)
             {
                 if (GameObject* go = instance->GetGameObject(guid))
                     return go->GetGoState();
                 return GO_STATE_ACTIVE;
             }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_gundrak_InstanceMapScript(map);
        }
};

class go_gundrak_altar : public GameObjectScript
{
    public:
        go_gundrak_altar() : GameObjectScript("go_gundrak_altar") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go)
        {
            InstanceScript* instance = go->GetInstanceScript();
            ObjectGuid uiStatue = ObjectGuid::Empty;

            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
            go->SetGoState(GO_STATE_ACTIVE);

            if (instance)
            {
                switch (go->GetEntry())
                {
                    case GO_ALTAR_OF_SLADRAN:           uiStatue = instance->GetGuidData(DATA_SLAD_RAN_STATUE);          break;
                    case GO_ALTAR_OF_MOORABI:           uiStatue = instance->GetGuidData(DATA_MOORABI_STATUE);           break;
                    case GO_ALTAR_OF_DRAKKARI_COLOSSUS: uiStatue = instance->GetGuidData(DATA_DRAKKARI_COLOSSUS_STATUE); break;
                }
                if (dynamic_cast<instance_gundrak::instance_gundrak_InstanceMapScript*>(instance)->QueueActivation(uiStatue, 3500))
                {
                    go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    go->SetGoState(GO_STATE_ACTIVE);
                }
                return true;
            }
            return false;
        }
};

class spell_drakkari_frenzy_leap : public SpellScriptLoader
{
    public:
        spell_drakkari_frenzy_leap() : SpellScriptLoader("spell_drakkari_frenzy_leap") { }

        class spell_drakkari_frenzy_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_drakkari_frenzy_leap_SpellScript);

            void TargetSelect(SpellDestination& dest)
            {
                if (Unit* caster = GetCaster())
                {
                    Position pos = caster->GetPosition();
                    dest._position = WorldLocation(caster->GetMapId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
                    dest._position.RelocateOffset(0, 5.0f);
                }
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_drakkari_frenzy_leap_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_CASTER_FRONT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_drakkari_frenzy_leap_SpellScript();
        }
};

void AddSC_instance_gundrak()
{
    new instance_gundrak();
    new go_gundrak_altar();
    new spell_drakkari_frenzy_leap();
}
