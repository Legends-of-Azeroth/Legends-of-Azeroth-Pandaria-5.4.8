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
#include "VMapFactory.h"
#include "terrace_of_endless_spring.h"
#include "Log.h"

class isProtectorDeadPredicate
{
    public:
        bool operator()(Unit* target) const
        {
            return target && !target->IsAlive();
        }
};

class instance_terrace_of_endless_spring : public InstanceMapScript
{
    public:
        instance_terrace_of_endless_spring() : InstanceMapScript("instance_terrace_of_endless_spring", 996) { }

        struct instance_terrace_of_endless_spring_InstanceMapScript : public InstanceScript
        {
            instance_terrace_of_endless_spring_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap m_mEvents;
            uint32 m_auiEncounter[MAX_TYPES];
            std::string strSaveData;

            std::list<Unit*> protectorList;
            std::vector<ObjectGuid> animatedList;
            std::vector<ObjectGuid> returnTerraceList;
            std::unordered_map<uint32, ObjectGuid> m_mGoEntryGuidMap;

            bool ritualOfPurification;
            bool introDone;
            uint32 leiShiProtectorsCount;
            // Protectors of the Endless
            ObjectGuid ancientRegailGuid;
            ObjectGuid ancientAsaniGuid;
            ObjectGuid protectorKaolanGuid;
            ObjectGuid minionOfFearControllerGuid;

            // Tsulong
            ObjectGuid tsulongGuid;
            ObjectGuid enchantedPlantGuid;

            // Lei Shi
            ObjectGuid leiShiGuid;
            ObjectGuid leiShiReflectionGuid;

            // Sha of Fear
            ObjectGuid shaOfFearGuid;
            ObjectGuid pureLightTerraceGuid;

            ObjectGuid shaVortexGuid;
            ObjectGuid shaVortexWallGuid;

            // Council's Vortex
            ObjectGuid wallOfCouncilsVortexGuid;
            ObjectGuid councilsVortexGuid;

            // Lei Shi's Vortex
            ObjectGuid wallOfLeiShisVortexGuid;
            ObjectGuid leiShisVortexGuid;

            void Initialize() override
            {
                SetBossNumber(DATA_MAX_BOSS_DATA);

                // 5.2
                ritualOfPurification        = false;
                introDone                   = false;

                ancientRegailGuid = ObjectGuid::Empty;
                ancientAsaniGuid = ObjectGuid::Empty;
                protectorKaolanGuid = ObjectGuid::Empty;
                minionOfFearControllerGuid = ObjectGuid::Empty;

                tsulongGuid = ObjectGuid::Empty;

                leiShiGuid = ObjectGuid::Empty;
                leiShiReflectionGuid = ObjectGuid::Empty;

                shaOfFearGuid = ObjectGuid::Empty;
                pureLightTerraceGuid = ObjectGuid::Empty;

                wallOfCouncilsVortexGuid = ObjectGuid::Empty;
                councilsVortexGuid = ObjectGuid::Empty;

                wallOfLeiShisVortexGuid = ObjectGuid::Empty;
                leiShisVortexGuid = ObjectGuid::Empty;
                leiShiProtectorsCount       = 0;

                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
            }

            void OnPlayerEnter(Player* player) override
            {
                // Determination
                if (instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_LFR && GetDeterminationValue())
                    if (Aura* detAura = player->AddAura(SPELL_DETERMINATION_LFR, player))
                        detAura->SetStackAmount(GetDeterminationValue());
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_RETURN_TO_TERRACE:
                        returnTerraceList.push_back(creature->GetGUID());
                        break;
                    case NPC_SLG_GENERIC_MOP:
                    case NPC_DEFILED_GROUND:
                        creature->SetDisplayId(11686); // not visible
                        break;
                    case NPC_ANCIENT_ASANI:
                        ancientAsaniGuid = creature->GetGUID();
                        break;
                    case NPC_ANCIENT_REGAIL:
                        ancientRegailGuid = creature->GetGUID();
                        break;
                    case NPC_PROTECTOR_KAOLAN:
                        protectorKaolanGuid = creature->GetGUID();
                        break;
                    case NPC_MINION_OF_FEAR_CONTROLLER:
                        minionOfFearControllerGuid = creature->GetGUID();
                        break;
                    case NPC_TSULONG:
                        tsulongGuid = creature->GetGUID();
                        break;
                    case NPC_LEI_SHI:
                        leiShiGuid = creature->GetGUID();
                        instance->LoadGrid(creature->GetPositionX(), creature->GetPositionY());
                        creature->SetDisplayId(48588);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_SHA_OF_FEAR:
                        shaOfFearGuid = creature->GetGUID();
                        break;
                    case NPC_PURE_LIGHT_TERRACE:
                        pureLightTerraceGuid = creature->GetGUID();
                        creature->SetDisplayId(11686); // not visible
                        break;
                    case NPC_CORRUPTED_PROTECTOR:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        protectorList.push_back(creature);
                        break;
                    case NPC_ANIMATED_PROTECTOR:
                        creature->SetVisible(false);
                        animatedList.push_back(creature->GetGUID());
                        break;
                    case NPC_REFLECTION_OF_LEI_SHI:
                        leiShiReflectionGuid = creature->GetGUID();
                    case NPC_LIGHT_OF_DAY:
                        creature->SetDisplayId(42960);
                        break;
                    case NPC_ENCHANTED_PLANT:
                        enchantedPlantGuid = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            GameObject* GetGameObjectFromStorage(uint32 uiEntry)
            {
                GameObject* go = NULL;

                std::unordered_map<uint32, ObjectGuid>::iterator find = m_mGoEntryGuidMap.find(uiEntry);

                if (find != m_mGoEntryGuidMap.cend())
                    go = instance->GetGameObject(find->second);

                return go;
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_COUNCILS_VORTEX:
                        councilsVortexGuid = go->GetGUID();
                        break;
                    case GO_WALL_OF_COUNCILS_VORTEX:
                        wallOfCouncilsVortexGuid = go->GetGUID();
                        break;
                    case GO_WALL_OF_LEI_SHI:
                        wallOfLeiShisVortexGuid = go->GetGUID();
                        break;
                    case GO_LEI_SHIS_VORTEX:
                        leiShisVortexGuid = go->GetGUID();
                        break;
                    case GO_SHA_VORTEX:
                        shaVortexGuid = go->GetGUID();
                        break;
                    case GO_SHA_VORTEX_WALL:
                        shaVortexWallGuid = go->GetGUID();
                        break;
                    case GO_TSULONG_CHEST_LFR:
                    case GO_TSULONG_CHEST_NORMAL:
                    case GO_TSULONG_CHEST_HEROIC:
                    case GO_LEI_SHI_CHEST_NORMAL:
                    case GO_LEI_SHI_CHEST_HEROIC:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        break;
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit && unit->ToPlayer() && unit->ToPlayer()->HasAura(120268))
                {
                    unit->RemoveAurasDueToSpell(120268);

                    if (Creature* sha = instance->GetCreature(GetGuidData(NPC_SHA_OF_FEAR)))
                        sha->AI()->DoAction(4);
                }

                if (unit->ToCreature())
                {
                    if (unit->GetEntry() == NPC_CORRUPTED_PROTECTOR && GetData(TYPE_LEI_INTRO) != DONE)
                    {
                        if (GetData(TYPE_LEI_INTRO) != DONE)
                        {
                            ++leiShiProtectorsCount;

                            if (leiShiProtectorsCount == 10)
                                SetData(TYPE_LEI_INTRO, DONE);
                        }
                    }
                }
            }

            void HandleActivateProtectors()
            {
                for (auto&& sItr : protectorList)
                {
                    sItr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                    sItr->SetVisible(true);
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == DATA_PROTECTORS && state == DONE)
                {
                    if (Creature* creature = instance->GetCreature(GetGuidData(NPC_TSULONG)))
                    {
                        instance->LoadGrid(creature->GetPositionX(), creature->GetPositionY());
                        creature->AI()->DoAction(ACTION_START_TSULONG_WAYPOINT);
                    }
                }

                if (type == DATA_TSULONG && state == DONE)
                {
                    if (GameObject* go = GetGameObjectFromStorage(instance->IsHeroic() ? GO_TSULONG_CHEST_HEROIC : GO_TSULONG_CHEST_NORMAL))
                        DoRespawnGameObject(go->GetGUID(), 7 * DAY);

                    HandleActivateProtectors();
                }

                if (type == DATA_LEI_SHI && state == DONE)
                {
                    if (GameObject* go = GetGameObjectFromStorage(instance->IsHeroic() ? GO_LEI_SHI_CHEST_HEROIC : GO_LEI_SHI_CHEST_NORMAL))
                        DoRespawnGameObject(go->GetGUID(), 7 * DAY);
                }

                if (type < MAX_TYPES && state == DONE)
                    SetData(type, (uint32)state);

                PromotionIfLFR(state);

                return true;
            }

            void Update(uint32 diff) override
            {
                m_mEvents.Update(diff);

                while (uint32 eventId = m_mEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case 1: // Start Tsulong Movement
                            if (Creature* c = instance->GetCreature(GetGuidData(NPC_TSULONG)))
                            {
                                instance->LoadGrid(c->GetPositionX(), c->GetPositionY());
                                c->AI()->DoAction(ACTION_START_TSULONG_WAYPOINT);
                            }
                            break;
                        case 3: // Set Tsulong Invisible
                            if (Creature* c = instance->GetCreature(GetGuidData(NPC_TSULONG)))
                            {
                                instance->LoadGrid(c->GetPositionX(), c->GetPositionY());
                                c->SetDisplayId(17612);
                                c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                            }
                            break;
                        case 4: // Make Lei Shi accessible
                            if (Creature* c = instance->GetCreature(GetGuidData(leiShiGuid)))
                            {
                                instance->LoadGrid(c->GetPositionX(), c->GetPositionY());
                                c->AI()->DoAction(ACTION_LEISHI_INTRO);
                            }
                            break;
                        case 5: // 
                            if (GameObject* pVortexWall = instance->GetGameObject(wallOfCouncilsVortexGuid))
                            {
                                pVortexWall->SetGoState(GO_STATE_ACTIVE);
                            }
                            if (GameObject* pVortex = instance->GetGameObject(councilsVortexGuid))
                            {
                                pVortex->SetGoState(GO_STATE_ACTIVE);
                            }
                            break;
                        case 6:
                            if (Creature* asani = instance->GetCreature(ancientAsaniGuid))
                                asani->AI()->DoAction(ACTION_INTRO_FINISHED);

                            if (Creature* regail = instance->GetCreature(ancientRegailGuid))
                                regail->AI()->DoAction(ACTION_INTRO_FINISHED);

                            if (Creature* kaolan = instance->GetCreature(protectorKaolanGuid))
                                kaolan->AI()->DoAction(ACTION_INTRO_FINISHED);
                            break;
                        case 7:
                            for (auto const pGuid : animatedList)
                            {
                                if (Creature* creature = instance->GetCreature(pGuid))
                                {
                                    if (creature->IsInWorld())
                                        creature->AddObjectToRemoveList();
                                }
                            }

                            if (Creature* leiShi = instance->GetCreature(leiShiGuid))
                            {
                                leiShi->SetFaction(35);
                                leiShi->SetVisible(false);

                                if (Creature* leiShiReflection = instance->GetCreature(leiShiReflectionGuid))
                                {
                                    leiShiReflection->SetVisible(false);
                                }

                                if (GameObject* pVortex = instance->GetGameObject(leiShisVortexGuid))
                                    pVortex->SetGoState(GO_STATE_ACTIVE);

                                if (GameObject* pVortexWall = instance->GetGameObject(wallOfLeiShisVortexGuid))
                                    pVortexWall->SetGoState(GO_STATE_ACTIVE);
                            }

                            if (Creature* c = instance->GetCreature(shaOfFearGuid))
                            {
                                instance->LoadGrid(c->GetPositionX(), c->GetPositionY());
                                c->AI()->DoAction(ACTION_SHA_INTRO);

                            }
                            break;
                        case 9:
                            for (auto const pGuid : returnTerraceList)
                            {
                                if (Creature* creature = instance->GetCreature(pGuid))
                                {
                                    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    creature->UpdateObjectVisibility();
                                    creature->AddAura(120216, creature);
                                }
                            }
                            break;
                        case 2:
                            if (Creature* sha = instance->GetCreature(GetGuidData(NPC_SHA_OF_FEAR)))
                            {
                                // Lei`s Hope active only till second phase
                                if (!sha->AI()->GetData(TYPE_DREAD_TERRACE))
                                {
                                    Map::PlayerList const& lPlayers = instance->GetPlayers();

                                    for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                    {
                                        if (Player* player = itr->GetSource())
                                        {
                                            if (!player->HasAura(SPELL_LEIS_HOPE))
                                            {
                                                if (Aura* aura = player->AddAura(SPELL_LEIS_HOPE, player))
                                                {
                                                    aura->SetMaxDuration(3 * HOUR * IN_MILLISECONDS);
                                                    aura->SetDuration(3 * HOUR * IN_MILLISECONDS);
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            m_mEvents.ScheduleEvent(2, 40000);
                            break;
                    }
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type >= MAX_TYPES)
                {
                    switch (type)
                    {
                        case SPELL_RITUAL_OF_PURIFICATION:
                            ritualOfPurification = data;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    switch (type)
                    {
                        case TYPE_PROTECTORS:
                        case TYPE_TSULONG:
                            m_auiEncounter[type] = data;
                            break;
                        case TYPE_SHA:
                            for (auto const pGuid : returnTerraceList)
                            {
                                if (Creature* creature = instance->GetCreature(pGuid))
                                {
                                    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    creature->UpdateObjectVisibility();
                                    creature->AddAura(120216, creature);
                                }
                            }
                            m_auiEncounter[type] = data;
                            break;
                        case TYPE_LEI_SHI:
                            if (m_auiEncounter[type] == data)
                                return;

                            for (auto const pGuid : animatedList)
                            {
                                if (Creature* creature = instance->GetCreature(pGuid))
                                {
                                    if (creature->IsInWorld())
                                        creature->AddObjectToRemoveList();
                                }
                            }
                            m_auiEncounter[type] = data;
                            break;
                        case TYPE_LEIS_HOPE:
                            if (data == DONE)
                                m_mEvents.ScheduleEvent(2, 200);
                            m_auiEncounter[type] = data;
                            break;
                        case TYPE_LEI_INTRO:
                            switch (data)
                            {
                                case DONE:
                                {
                                    for (auto &aItr : animatedList)
                                        if (Creature* AnimatedProtector = instance->GetCreature(aItr))
                                            AnimatedProtector->SetVisible(true);
                                
                                    if (Creature* pReflection = instance->GetCreature(leiShiReflectionGuid))
                                    {
                                        if (pReflection->AI())
                                            pReflection->AI()->DoAction(ACTION_LEISHI_INTRO);
                                    }
                                    break;
                                }
                                case SPECIAL:
                                    HandleActivateProtectors();
                                    break;
                            }
                            m_auiEncounter[type] = data;
                            break;
                        case INTRO_DONE:
                            if (data == DONE)
                            {
                                Creature* asani = instance->GetCreature(ancientAsaniGuid);
                                if (asani)
                                    asani->AI()->DoAction(ACTION_INTRO_FINISHED);

                                Creature* regail = instance->GetCreature(ancientRegailGuid);
                                if (regail)
                                    regail->AI()->DoAction(ACTION_INTRO_FINISHED);

                                Creature* kaolan = instance->GetCreature(protectorKaolanGuid);
                                if (kaolan)
                                    kaolan->AI()->DoAction(ACTION_INTRO_FINISHED);
                            }

                            introDone = data == DONE;
                            m_auiEncounter[type] = data;
                            break;
                    }

                    if (data >= DONE)
                    {
                        OUT_SAVE_INST_DATA;

                        std::ostringstream saveStream;
                        saveStream << "T O E S " << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2] << ' '
                            << m_auiEncounter[3] << ' ' << m_auiEncounter[4] << ' ' << m_auiEncounter[5] << ' ' << m_auiEncounter[6];

                        strSaveData = saveStream.str();

                        SaveToDB();
                        OUT_SAVE_INST_DATA_COMPLETE;
                    }
                }
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2, dataHead3, dataHead4;

                std::istringstream loadStream(in);

                loadStream >> dataHead1 >> dataHead2 >> dataHead3 >> dataHead4;

                if (dataHead1 == 'T' && dataHead2 == 'O' && dataHead3 == 'E' && dataHead4 == 'S')
                {
                    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6];
                    for (uint8 i = 0; i < MAX_TYPES; ++i)
                        if (m_auiEncounter[i] == IN_PROGRESS)                // Do not load an encounter as "In Progress" - reset it instead.
                            m_auiEncounter[i] = NOT_STARTED;


                    if (m_auiEncounter[TYPE_PROTECTORS] == DONE)
                    {
                        if (m_auiEncounter[TYPE_TSULONG] == DONE)
                            m_mEvents.ScheduleEvent(3, 200); // Set Tsulong Invisible
                        else
                            m_mEvents.ScheduleEvent(1, 200); // Reactivate Tsulong
                    }

                    if (m_auiEncounter[TYPE_LEIS_HOPE] == DONE && instance->IsHeroic())
                        m_mEvents.ScheduleEvent(2, 200); // Reapply Lei's Hope

                    if (m_auiEncounter[TYPE_LEI_INTRO] == DONE && m_auiEncounter[TYPE_LEI_SHI] != DONE)
                        SetData(TYPE_LEI_INTRO, DONE); // Activate Lei Shi

                    if (m_auiEncounter[INTRO_DONE] == DONE)
                    {
                        m_mEvents.ScheduleEvent(5, 200); // Deactivate Protectors Vortex

                        if (m_auiEncounter[TYPE_PROTECTORS] != DONE)
                            m_mEvents.ScheduleEvent(6, 200); // Activate Protectors
                    }

                    if (m_auiEncounter[TYPE_LEI_SHI] == DONE)
                        m_mEvents.ScheduleEvent(7, 200); // Deactivate Sha of Fear Vortex

                    if (m_auiEncounter[TYPE_SHA] == DONE)
                        m_mEvents.ScheduleEvent(9, 200); // Activate return to terrace;
                }
                else OUT_LOAD_INST_DATA_FAIL;
                
                OUT_LOAD_INST_DATA_COMPLETE;
            }

            std::string GetSaveData() override
            {
                return strSaveData;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case SPELL_RITUAL_OF_PURIFICATION:
                        return ritualOfPurification;
                    case INTRO_DONE:
                        return introDone;
                    case TYPE_PROTECTORS:
                    case TYPE_TSULONG:
                    case TYPE_LEI_SHI:
                    case TYPE_SHA:
                    case TYPE_LEIS_HOPE:
                    case TYPE_LEI_INTRO:
                        return m_auiEncounter[type];
                    default:
                        return 0;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_ANCIENT_ASANI:
                        return ancientAsaniGuid;
                    case NPC_ANCIENT_REGAIL:
                        return ancientRegailGuid;
                    case NPC_PROTECTOR_KAOLAN:
                        return protectorKaolanGuid;
                    case NPC_MINION_OF_FEAR_CONTROLLER:
                        return minionOfFearControllerGuid;
                    case NPC_TSULONG:
                        return tsulongGuid;
                    case NPC_LEI_SHI:
                        return leiShiGuid;
                    case NPC_REFLECTION_OF_LEI_SHI:
                        return leiShiReflectionGuid;
                    case NPC_SHA_OF_FEAR:
                        return shaOfFearGuid;
                    case NPC_PURE_LIGHT_TERRACE:
                        return pureLightTerraceGuid;
                    case NPC_ENCHANTED_PLANT:
                        return enchantedPlantGuid;
                    case GO_COUNCILS_VORTEX:
                        return councilsVortexGuid;
                    case GO_WALL_OF_COUNCILS_VORTEX:
                        return wallOfCouncilsVortexGuid;
                    case GO_WALL_OF_LEI_SHI:
                        return wallOfLeiShisVortexGuid;
                    case GO_LEI_SHIS_VORTEX:
                        return leiShisVortexGuid;
                    case GO_SHA_VORTEX:
                        return shaVortexGuid;
                    case GO_SHA_VORTEX_WALL:
                        return shaVortexWallGuid;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
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
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_terrace_of_endless_spring_InstanceMapScript(map);
        }
};

void AddSC_instance_terrace_of_endless_spring()
{
    new instance_terrace_of_endless_spring();
}
