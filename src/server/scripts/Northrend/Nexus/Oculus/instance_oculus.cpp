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
#include "oculus.h"
#include "LFGMgr.h"

static std::vector<DoorData> const doorData =
{
    { GO_DRAGON_CAGE_DOOR,  DATA_DRAKOS,    DOOR_TYPE_PASSAGE,  BOUNDARY_NONE },
};

Position const VerdisaMove       = { 949.188f, 1032.91f, 359.967f, 1.093027f  };
Position const BelgaristraszMove = { 941.453f, 1044.1f,  359.967f, 0.1984709f };
Position const EternosMove       = { 943.202f, 1059.35f, 359.967f, 5.757278f  };

class instance_oculus : public InstanceMapScript
{
    public:
        instance_oculus() : InstanceMapScript(OculusScriptName, 578) { }

        struct instance_oculus_InstanceMapScript : public InstanceScript, public CacheEligibilityCheckAccessor
        {
            instance_oculus_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);

                DrakosGUID = ObjectGuid::Empty;
                VarosGUID = ObjectGuid::Empty;
                UromGUID = ObjectGuid::Empty;
                EregosGUID = ObjectGuid::Empty;

                CentrifugueConstructCounter = 0;

                EregosCacheGUID = ObjectGuid::Empty;
                EregosCacheSpotlightGUID = ObjectGuid::Empty;

                GreaterWhelpList.clear();

                BelgaristraszGUID = ObjectGuid::Empty;
                EternosGUID = ObjectGuid::Empty;
                VerdisaGUID = ObjectGuid::Empty;

                instance->SetWorldState(WORLDSTATE_RUBY_VOID, 1);
                instance->SetWorldState(WORLDSTATE_EMERALD_VOID, 1);
                instance->SetWorldState(WORLDSTATE_AMBER_VOID, 1);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_DRAKOS:
                        DrakosGUID = creature->GetGUID();
                        break;
                    case NPC_VAROS:
                        VarosGUID = creature->GetGUID();
                        if (GetBossState(DATA_DRAKOS) == DONE)
                            creature->SetPhaseMask(1, true);
                        break;
                    case NPC_UROM:
                        UromGUID = creature->GetGUID();
                        if (GetBossState(DATA_VAROS) == DONE)
                            creature->SetPhaseMask(1, true);
                        break;
                    case NPC_EREGOS:
                        EregosGUID = creature->GetGUID();
                        if (GetBossState(DATA_UROM) == DONE)
                            creature->SetPhaseMask(1, true);
                        break;
                    case NPC_CENTRIFUGE_CONSTRUCT:
                        if (creature->IsAlive())
                            DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, ++CentrifugueConstructCounter);
                        break;
                    case NPC_BELGARISTRASZ:
                        BelgaristraszGUID = creature->GetGUID();
                        if (GetBossState(DATA_DRAKOS) == DONE)
                        {
                            creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            creature->Relocate(BelgaristraszMove);
                        }
                        break;
                    case NPC_ETERNOS:
                        EternosGUID = creature->GetGUID();
                        if (GetBossState(DATA_DRAKOS) == DONE)
                        {
                            creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            creature->Relocate(EternosMove);
                        }
                        break;
                    case NPC_VERDISA:
                        VerdisaGUID = creature->GetGUID();
                        if (GetBossState(DATA_DRAKOS) == DONE)
                        {
                            creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            creature->Relocate(VerdisaMove);
                        }
                        break;
                    case NPC_GREATER_WHELP:
                        if (GetBossState(DATA_UROM) == DONE)
                        {
                            creature->SetPhaseMask(1, true);
                            GreaterWhelpList.push_back(creature->GetGUID());
                        }
                        break;
                    case NPC_AZURE_RING_GUARDIAN_2:
                        creature->SetFlying(true);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_DRAGON_CAGE_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_EREGOS_CACHE_N:
                    case GO_EREGOS_CACHE_H:
                        if (instance->IsHeroic() == (go->GetEntry() == GO_EREGOS_CACHE_H))
                            EregosCacheGUID = go->GetGUID();
                        break;
                    case GO_SPOTLIGHT:
                        EregosCacheSpotlightGUID = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                if (creature->GetEntry() == NPC_CENTRIFUGE_CONSTRUCT)
                {
                     DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, --CentrifugueConstructCounter);

                     if (!CentrifugueConstructCounter)
                        if (Creature* varos = instance->GetCreature(VarosGUID))
                            varos->RemoveAllAuras();
                }
            }

            void OnPlayerEnter(Player* player) override
            {
                if (GetBossState(DATA_EREGOS) == IN_PROGRESS)
                    if (Creature* eregos = instance->GetCreature(EregosGUID))
                        eregos->SetInCombatWithZone();

                if (uint32 queueId = sLFGMgr->GetActiveQueueId(player->GetGUID()))
                    if (uint32 random = sLFGMgr->GetRandomDungeon(player->GetGUID(), queueId))
                        if (auto dungeon = sLFGMgr->GetLFGDungeon(random))
                            if (dungeon->map == player->GetMap()->GetId())
                                playersEligibleForCache.insert(player->GetGUID());
            }

            void FillInitialWorldStates(WorldStateBuilder& builder) override
            {
                if (GetBossState(DATA_DRAKOS) == DONE && GetBossState(DATA_VAROS) != DONE)
                {
                    builder.AppendState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 1);
                    builder.AppendState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, CentrifugueConstructCounter);
                }
                else
                {
                    builder.AppendState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 0);
                    builder.AppendState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, 0);
                }
            }

            void ProcessEvent(WorldObject* /*unit*/, uint32 eventId) override
            {
                if (eventId != EVENT_CALL_DRAGON)
                    return;

                if (Creature* varos = instance->GetCreature(VarosGUID))
                    if (Creature* drake = varos->SummonCreature(NPC_AZURE_RING_GUARDIAN, varos->GetPositionX(), varos->GetPositionY(), varos->GetPositionZ() + 40))
                        drake->AI()->DoAction(ACTION_CALL_DRAGON_EVENT);
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_DRAKOS:
                        if (state == DONE)
                        {
                            DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 1);
                            DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, CentrifugueConstructCounter);
                            FreeDragons();
                            if (Creature* varos = instance->GetCreature(VarosGUID))
                                varos->SetPhaseMask(1, true);
                            events.ScheduleEvent(EVENT_VAROS_INTRO, 15000);
                        }
                        break;
                    case DATA_VAROS:
                        if (state == DONE)
                        {
                            DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 0);
                            if (Creature* urom = instance->GetCreature(UromGUID))
                                urom->SetPhaseMask(1, true);
                        }
                        break;
                    case DATA_UROM:
                        if (state == DONE)
                        {
                            if (Creature* eregos = instance->GetCreature(EregosGUID))
                            {
                                eregos->SetPhaseMask(1, true);
                                GreaterWhelps();
                                events.ScheduleEvent(EVENT_EREGOS_INTRO, 5000);
                            }
                        }
                        break;
                    case DATA_EREGOS:
                        if (state == DONE)
                        {
                            if (GameObject* cache = instance->GetGameObject(EregosCacheGUID))
                            {
                                cache->SetRespawnTime(1 * DAY);
                                cache->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            }
                            if (GameObject* spotlight = instance->GetGameObject(EregosCacheSpotlightGUID))
                                spotlight->SetRespawnTime(1 * DAY);
                        }
                        break;
                }

                return true;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_CENTRIFUGUE_CONSTRUCTS:
                        return CentrifugueConstructCounter;
                    default:
                        break;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_DRAKOS:
                        return DrakosGUID;
                    case DATA_VAROS:
                        return VarosGUID;
                    case DATA_UROM:
                        return UromGUID;
                    case DATA_EREGOS:
                        return EregosGUID;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            bool IsPlayerEligibleForCache(ObjectGuid playerGuid) const override
            {
                return playersEligibleForCache.find(playerGuid) != playersEligibleForCache.end();
            }

            void FreeDragons()
            {
                if (Creature* belgaristrasz = instance->GetCreature(BelgaristraszGUID))
                {
                    belgaristrasz->SetWalk(true);
                    belgaristrasz->GetMotionMaster()->MovePoint(POINT_MOVE_OUT, BelgaristraszMove);
                }
                if (Creature* eternos = instance->GetCreature(EternosGUID))
                {
                    eternos->SetWalk(true);
                    eternos->GetMotionMaster()->MovePoint(POINT_MOVE_OUT, EternosMove);
                }
                if (Creature* verdisa = instance->GetCreature(VerdisaGUID))
                {
                    verdisa->SetWalk(true);
                    verdisa->GetMotionMaster()->MovePoint(POINT_MOVE_OUT, VerdisaMove);
                }
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_VAROS_INTRO:
                            if (Creature* varos = instance->GetCreature(VarosGUID))
                                varos->AI()->Talk(SAY_VAROS_INTRO_TEXT);
                            break;
                        case EVENT_EREGOS_INTRO:
                            if (Creature* eregos = instance->GetCreature(EregosGUID))
                                eregos->AI()->Talk(SAY_EREGOS_INTRO_TEXT);
                            break;
                        default:
                            break;
                    }
                }
            }

            void GreaterWhelps()
            {
                for (ObjectGuid guid : GreaterWhelpList)
                    if (Creature* gwhelp = instance->GetCreature(guid))
                        gwhelp->SetPhaseMask(1, true);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "T O " << GetBossSaveData();

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

                if (dataHead1 == 'T' && dataHead2 == 'O')
                {
                    for (uint8 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        protected:
            ObjectGuid DrakosGUID;
            ObjectGuid VarosGUID;
            ObjectGuid UromGUID;
            ObjectGuid EregosGUID;

            ObjectGuid BelgaristraszGUID;
            ObjectGuid EternosGUID;
            ObjectGuid VerdisaGUID;

            uint8 CentrifugueConstructCounter;

            ObjectGuid EregosCacheGUID;
            ObjectGuid EregosCacheSpotlightGUID;

            std::string str_data;

            std::list<ObjectGuid> GreaterWhelpList;
            std::set<ObjectGuid> playersEligibleForCache;

            EventMap events;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_oculus_InstanceMapScript(map);
        }
};

void AddSC_instance_oculus()
{
    new instance_oculus();
}
