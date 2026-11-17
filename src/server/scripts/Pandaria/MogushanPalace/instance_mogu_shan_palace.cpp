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
#include "mogu_shan_palace.h"
#include "Log.h"
#include "Containers.h"

#include <numeric>

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { DATA_TRIAL_OF_THE_KING,    CRITERIA_TRIAL_KING             },
    { DATA_GEKKAN,               CRITERIA_GEKKAN                 },
    { DATA_XIN_THE_WEAPONMASTER, CRITERIA_XIN_MASTER             },
};

class instance_mogu_shan_palace : public InstanceMapScript
{
    public:
        instance_mogu_shan_palace() : InstanceMapScript("instance_mogu_shan_palace", 994) { }

        struct instance_mogu_shan_palace_InstanceMapScript : public InstanceScript
        {
            instance_mogu_shan_palace_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_TYPES];
            EventMap m_mEvents;

            // Trial of the king.
            uint32 delay;
            ObjectGuid m_uiXinGuid;
            ObjectGuid m_uiKuaiGuid;
            ObjectGuid m_uiMingGuid;
            ObjectGuid m_uiHaiyanGuid;
            ObjectGuid m_uiBeaconGuid;
            std::list<ObjectGuid> m_lScrapperList;
            std::list<ObjectGuid> m_lAdeptList;
            std::list<ObjectGuid> m_lGruntList;

            uint32 m_uiBossCount;
            uint32 m_auiBossNumber[3];

            // m_uiGekkanGuid.
            ObjectGuid m_uiGekkanGuid;
            ObjectGuid m_uiAncientTreasureGuid;

            // Xin the weaponmaster.
            std::list<ObjectGuid> m_lStaffList;
            std::list<ObjectGuid> m_lAxeList;

            ObjectGuid m_uiScoutGuid;

            // Storage
            ObjectGuid m_auiGuids64[MAX_GUIDS];
            std::unordered_map<uint32, ObjectGuid> m_mGoEntryGuidMap;

            void Initialize() override
            {
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                // pretty dumb to even try to utilize data without this... lol
                SetBossNumber(MAX_ENCOUNTER);

                delay = 0;
                m_uiXinGuid = ObjectGuid::Empty;
                m_uiKuaiGuid = ObjectGuid::Empty;
                m_uiMingGuid = ObjectGuid::Empty;
                m_uiHaiyanGuid = ObjectGuid::Empty;
                m_uiBeaconGuid = ObjectGuid::Empty;

                m_uiGekkanGuid = ObjectGuid::Empty;
                m_uiAncientTreasureGuid = ObjectGuid::Empty;

                InitializeTrialOfKing();

                m_uiScoutGuid = ObjectGuid::Empty;

                if (instance->IsChallengeDungeon())
                    LoadScenarioInfo(scenarioBosses, CRITERIA_ENEMIES);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (instance->IsChallengeDungeon())
                    SendChallengeInfo(player, SCENARIO_ID);
            }

            void InitializeTrialOfKing()
            {
                m_uiBossCount = 0;

                std::iota(std::begin(m_auiBossNumber), std::end(m_auiBossNumber), 0);
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(std::begin(m_auiBossNumber), std::end(m_auiBossNumber), g);
            }

            GameObject* GetGameObjectFromStorage(uint32 uiEntry)
            {
                GameObject* go = NULL;

                std::unordered_map<uint32, ObjectGuid>::iterator find = m_mGoEntryGuidMap.find(uiEntry);

                if (find != m_mGoEntryGuidMap.cend())
                    go = instance->GetGameObject(find->second);

                return go;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_GEKKAN:
                        HandleGameObject(ObjectGuid::Empty, state == DONE, GetGameObjectFromStorage(GO_DOOR_AFTER_TRIAL));
                        if (GameObject* pTreasure = instance->GetGameObject(m_uiAncientTreasureGuid))
                            pTreasure->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                }

                return true;
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_TRIAL_CHEST:
                    case GO_TRIAL_CHEST_HC:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        break;
                    case GO_DOOR_BEFORE_TRIAL:
                    case GO_DOOR_AFTER_TRIAL:
                    case GO_DOOR_SECRET_STEPS:
                    case GO_DOOR_BEFORE_KING:
                    case GO_DOOR_BEFORE_KING2:
                    case GO_GEKKAN_TREASURE_DOOR:
                    case GO_ANCIENT_MOGU_TREASURE:
                    case GO_ANCIENT_MOGU_TREASURE_HC:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_USELESS_DOOR:
                        // dont need to use memory space for this
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        break;
                    case GO_CHALLENGE_DOOR:
                        SetChallengeDoorGuid(go->GetGUID());
                        break;
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (instance->IsChallengeDungeon() && creature->isDead())
                    creature->Respawn();

                OnCreatureCreate_trial_of_the_king(creature);
                OnCreatureCreate_xin_the_weaponmaster(creature);

                switch (creature->GetEntry())
                {
                    case CREATURE_GLINTROK_SCOUT:
                        m_uiScoutGuid = creature->GetGUID();
                        creature->setActive(true);
                        creature->SetVisible(false);
                        break;
                    case CREATURE_BEACON:
                        m_uiBeaconGuid = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature() && unit->ToCreature()->GetEntry() == CREATURE_GEKKAN)
                {
                    SetBossState(DATA_GEKKAN, DONE);

                    if (GameObject* go = GetGameObjectFromStorage(instance->GetDifficulty() == DUNGEON_DIFFICULTY_HEROIC ? GO_ANCIENT_MOGU_TREASURE_HC : GO_ANCIENT_MOGU_TREASURE))
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                    HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GO_GEKKAN_TREASURE_DOOR));
                }

                if (instance->IsChallengeDungeon() && !IsChallengeModeCompleted())
                {
                    if (Creature* creature = unit->ToCreature())
                    {
                        switch (creature->GetEntry())
                        {
                            case CREATURE_GLINTROK_IRONHIDE:
                            case CREATURE_GLINTROK_SKULKER:
                            case CREATURE_GLINTROK_ORACLE:
                            case CREATURE_GLINTROK_HEXXER:
                                break;
                            default:
                                UpdateConditionInfo(creature, ENEMIES_COUNT);
                                break;
                        }
                    }
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type >= MAX_TYPES)
                    return;

                SetData_trial_of_the_king(type, data);
                SetData_xin_the_weaponmaster(type, data);

                if (type == TYPE_SHUFFLE_ORDER && m_uiBossCount >= 3)
                    m_auiEncounter[type] = DONE;
                else
                    m_auiEncounter[type] = data;

                if (data == DONE)
                    SaveToDB();
            }

            std::string GetSaveData() override
            {
                std::ostringstream saveStream;
                saveStream << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2] << ' '
                           << m_auiEncounter[3] << ' ' << m_auiEncounter[4] << ' ' << m_auiEncounter[5] << ' '
                           << m_auiEncounter[6] << ' ' << m_auiEncounter[7] << ' ' << m_auiEncounter[8] << ' '
                           << m_auiEncounter[9] << ' ' << m_auiEncounter[10] << ' ' << m_auiEncounter[11] << ' '
                           << m_auiEncounter[12] << ' ' << m_auiEncounter[13];
                return saveStream.str();
            }

            uint32 GetData(uint32 type) const override
            {
                if (type >= MAX_TYPES)
                    return 0;

                return m_auiEncounter[type];
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                if (type < MAX_GUIDS)
                    return m_auiGuids64[type];

                auto const find = m_mGoEntryGuidMap.find(type);
                if (find != m_mGoEntryGuidMap.cend())
                    return find->second;

                return ObjectGuid::Empty;
            }

            bool isWipe()
            {
                Map::PlayerList const& PlayerList = instance->GetPlayers();

                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Player* plr = i->GetSource())
                            if (plr->IsAlive() && !plr->IsGameMaster())
                                return false;
                    }
                }
                return true;
            }

            void SetData_xin_the_weaponmaster(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case TYPE_ACTIVATE_ANIMATED_STAFF:
                    {
                        if (Creature* creature = instance->GetCreature(Trinity::Containers::SelectRandomContainerElement(m_lStaffList)))
                            if (creature->GetAI())
                                creature->GetAI()->DoAction(0); //ACTION_ACTIVATE
                        break;
                    }
                    case TYPE_ACTIVATE_ANIMATED_AXE:
                    {
                        for (auto&& guid : m_lAxeList)
                        {
                            if (Creature* creature = instance->GetCreature(guid))
                            {
                                if (data)
                                {
                                    if (creature->AI())
                                        creature->AI()->DoAction(0);
                                }
                                else
                                {
                                    if (creature->AI())
                                        creature->AI()->DoAction(1);
                                }
                            }
                        }
                        break;
                    }
                    case TYPE_XIN:
                        HandleGameObject(ObjectGuid::Empty, type != IN_PROGRESS, GetGameObjectFromStorage(GO_DOOR_BEFORE_KING));
                        HandleGameObject(ObjectGuid::Empty, type != IN_PROGRESS, GetGameObjectFromStorage(GO_DOOR_BEFORE_KING2));
                        break;
                }
            }

            void OnCreatureCreate_xin_the_weaponmaster(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case 59481:
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case CREATURE_ANIMATED_STAFF:
                        m_lStaffList.push_back(creature->GetGUID());
                        break;
                    case CREATURE_ANIMATED_AXE:
                        m_lAxeList.push_back(creature->GetGUID());
                        break;
                    case CREATURE_CROSSBOW:
                        creature->SetReactState(REACT_PASSIVE);
                        creature->CastSpell(creature, SPELL_PERMANENT_FEIGN_DEATH, true);
                        creature->SetDisplayId(42197);
                        break;
                }
            }

            void SetData_trial_of_the_king(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case TYPE_WIPE_FIRST_BOSS:
                    {
                        HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GO_DOOR_BEFORE_TRIAL));
                        switch (data)
                        {
                            case 0:
                                for (auto&& guid : m_lAdeptList)
                                {
                                    Creature* creature = instance->GetCreature(guid);
                                    if (!creature)
                                        continue;

                                    if (creature && creature->GetAI())
                                        creature->GetAI()->DoAction(1); //EVENT_RETIRE
                                        creature->RemoveAllAuras();
                                }
                                break;
                            case 1:
                                for (auto&& guid : m_lScrapperList)
                                {
                                    Creature* creature = instance->GetCreature(guid);
                                    if (!creature)
                                        continue;

                                    if (creature && creature->GetAI())
                                        creature->GetAI()->DoAction(1); //EVENT_RETIRE
                                        creature->RemoveAllAuras();
                                }
                                break;
                            case 2:
                                for (auto&& guid : m_lGruntList)
                                {
                                    Creature* creature = instance->GetCreature(guid);
                                    if (!creature)
                                        continue;

                                    if (creature && creature->GetAI())
                                        creature->GetAI()->DoAction(1); //EVENT_RETIRE
                                        creature->RemoveAllAuras();
                                }
                                break;
                        }
                        break;
                    }
                    case TYPE_MING:
                    {
                        //Move the m_lAdeptList
                        for (auto&& guid : m_lAdeptList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(0); //EVENT_ENCOURAGE
                        }
                        Creature* pMing = instance->GetCreature(m_uiMingGuid);
                        if (!pMing)
                            return;

                        Movement::MoveSplineInit init(pMing);
                        init.MoveTo(pTrialIntroPositions[type].GetPositionX(), pTrialIntroPositions[type].GetPositionY(), pTrialIntroPositions[type].GetPositionZ());

                        init.Launch();

                        delay = 0;
                        ObjectGuid tempGUID = pMing->GetGUID();
                        pMing->m_Events.Schedule(delay += pMing->GetSplineDuration(), 10, [this,tempGUID]()
                        {
                            if (Creature* Ming = instance->GetCreature(tempGUID))
                            {
                                Ming->StopMoving();
                                Ming->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                Ming->SetReactState(REACT_AGGRESSIVE);
                                Ming->SetInCombatWithZone();
                            }
                        });

                        if (pMing->GetAI())
                            pMing->GetAI()->DoAction(0);
                    }
                    break;
                    case TYPE_KUAI:
                    {
                        //Move the m_lScrapperList
                        for (auto&& guid : m_lScrapperList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(0); //EVENT_ENCOURAGE
                        }
                        Creature* pKuai = instance->GetCreature(m_uiKuaiGuid);
                        if (!pKuai)
                            return;

                        Movement::MoveSplineInit init(pKuai);
                        init.MoveTo(pTrialIntroPositions[type].GetPositionX(), pTrialIntroPositions[type].GetPositionY(), pTrialIntroPositions[type].GetPositionZ());

                        init.Launch();

                        delay = 0;
                        ObjectGuid tempGUID = pKuai->GetGUID();
                        pKuai->m_Events.Schedule(delay += pKuai->GetSplineDuration(), 10, [this, tempGUID]()
                        {
                            if (Creature* Kuai = instance->GetCreature(tempGUID))
                            {
                                Kuai->StopMoving();
                                Kuai->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                Kuai->SetReactState(REACT_AGGRESSIVE);
                                Kuai->SetInCombatWithZone();
                            }
                        });

                        // special case action 2 for kuai
                        if (pKuai->GetAI())
                            pKuai->GetAI()->DoAction(2);
                        break;
                    }
                    case TYPE_HAIYAN:
                    {
                        //Move the m_lScrapperList
                        for (auto&& guid : m_lGruntList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(0); //EVENT_ENCOURAGE
                        }
                        Creature* pHaiyan = instance->GetCreature(m_uiHaiyanGuid);
                        if (!pHaiyan)
                            return;

                        Movement::MoveSplineInit init(pHaiyan);
                        init.MoveTo(pTrialIntroPositions[type].GetPositionX(), pTrialIntroPositions[type].GetPositionY(), pTrialIntroPositions[type].GetPositionZ());

                        init.Launch();

                        delay = 0;
                        ObjectGuid tempGUID = pHaiyan->GetGUID();
                        pHaiyan->m_Events.Schedule(delay += pHaiyan->GetSplineDuration(), 10, [this, tempGUID]()
                        {
                            if (Creature* Haiyan = instance->GetCreature(tempGUID))
                            {
                                Haiyan->StopMoving();
                                Haiyan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                Haiyan->SetReactState(REACT_AGGRESSIVE);
                                Haiyan->SetInCombatWithZone();
                            }
                        });

                        // Make sure he talks for the intro
                        if (pHaiyan->GetAI())
                            pHaiyan->GetAI()->DoAction(0);
                        break;
                    }
                    case TYPE_TRIAL_ENDED:
                    {
                        for (auto&& guid : m_lAdeptList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(2); //ACTION_ATTACK

                            std::list<ObjectGuid>::iterator itr = m_lGruntList.begin();
                            std::advance(itr, urand(0, m_lGruntList.size() - 1));

                            Creature* pGrunt = instance->GetCreature(*itr);
                            if (creature && pGrunt)
                            {
                                creature->Attack(pGrunt, true);
                                if (creature->AI())
                                    creature->AI()->AttackStart(pGrunt);
                            }
                        }
                        for (auto&& guid : m_lGruntList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(2); //ACTION_ATTACK

                            std::list<ObjectGuid>::iterator itr = m_lScrapperList.begin();
                            std::advance(itr, urand(0, m_lScrapperList.size() - 1));

                            Creature* pScrapper = instance->GetCreature(*itr);
                            if (creature && pScrapper)
                            {
                                creature->Attack(pScrapper, true);
                                if (creature->AI())
                                    creature->AI()->AttackStart(pScrapper);
                            }
                        }
                        for (auto&& guid : m_lScrapperList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(2); //ACTION_ATTACK

                            std::list<ObjectGuid>::iterator itr = m_lAdeptList.begin();
                            std::advance(itr, urand(0, m_lAdeptList.size() - 1));

                            Creature* pAdept = instance->GetCreature(*itr);
                            if (creature && pAdept)
                            {
                                creature->Attack(pAdept, true);
                                if (creature->AI())
                                    creature->AI()->AttackStart(pAdept);
                            }
                        }

                        if (Creature* pKuai = instance->GetCreature(m_uiKuaiGuid))
                            pKuai->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                        if (Creature* pHaiyan = instance->GetCreature(m_uiHaiyanGuid))
                            pHaiyan->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                        if (Creature* pMing = instance->GetCreature(m_uiMingGuid))
                            pMing->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        break;
                    }
                    case TYPE_MING_RETIRED:
                        // This boss is down, move on to next
                        ++m_uiBossCount;

                        //Retire the m_lAdeptList
                        for (auto&& guid : m_lAdeptList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(1); //EVENT_RETIRE
                        }
                        break;
                    case TYPE_KUAI_RETIRED:
                        // This boss is down, move on to next
                        ++m_uiBossCount;

                        //Retire the m_lAdeptList
                        for (auto&& guid : m_lScrapperList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(1); //EVENT_RETIRE
                        }
                        break;
                    case TYPE_HAIYAN_RETIRED:
                        // This boss is down, move on to next
                        ++m_uiBossCount;

                        //Retire the m_lAdeptList
                        for (auto&& guid : m_lGruntList)
                        {
                            Creature* creature = instance->GetCreature(guid);

                            if (creature && creature->GetAI())
                                creature->GetAI()->DoAction(1); //EVENT_RETIRE
                        }
                        break;
                    case TYPE_SHUFFLE_ORDER:
                        if (GetData(TYPE_SHUFFLE_ORDER) == DONE)
                            return;

                        if (m_uiBossCount < 3)
                        {
                            SetData(m_auiBossNumber[m_uiBossCount], 0);
                            HandleGameObject(ObjectGuid::Empty, false, GetGameObjectFromStorage(GO_DOOR_BEFORE_TRIAL));
                        }
                        else
                        {
                            SetBossState(DATA_TRIAL_OF_THE_KING, DONE);

                            if (GameObject* go = GetGameObjectFromStorage(GO_TRIAL_CHEST))
                                SetData64(TYPE_TRIAL_ENDED, go->GetGUID());

                            if (Creature* pScout = instance->GetCreature(m_uiScoutGuid))
                            {
                                if (pScout->AI())
                                    pScout->AI()->DoAction(0);
                            }

                            HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GO_DOOR_BEFORE_TRIAL));
                        }
                        break;
                    case TYPE_TRIAL_CHEST:
                        if (GameObject* go = GetGameObjectFromStorage(instance->GetDifficulty() == DUNGEON_DIFFICULTY_HEROIC ? GO_TRIAL_CHEST_HC : GO_TRIAL_CHEST))
                            DoRespawnGameObject(go->GetGUID(), 7 * DAY);

                        if (Creature* pBeacon = instance->GetCreature(m_uiBeaconGuid))
                            pBeacon->SetVisible(true);

                        if (!instance->IsChallengeDungeon())
                            for (auto&& itr : instance->GetPlayers())
                                itr.GetSource()->ModifyCurrency(395, (instance->IsHeroic() ? 125 : 70) * CURRENCY_PRECISION);

                        HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GO_DOOR_SECRET_STEPS));
                        m_mEvents.ScheduleEvent(1, 5 * IN_MILLISECONDS);
                        break;
                }
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                if (type >= MAX_GUIDS)
                    return;

                m_auiGuids64[type] = data;
            }

            void OnCreatureCreate_trial_of_the_king(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case CREATURE_GURTHAN_SCRAPPER:
                    case CREATURE_GURTHAN_SCRAPPER_2:
                        m_lScrapperList.push_back(creature->GetGUID());
                        break;
                    case CREATURE_HARTHAK_ADEPT:
                    case CREATURE_HARTHAK_ADEPT_2:
                        m_lAdeptList.push_back(creature->GetGUID());
                        break;
                    case CREATURE_KARGESH_GRUNT:
                    case CREATURE_KARGESH_GRUNT_2:
                        m_lGruntList.push_back(creature->GetGUID());
                        break;
                    case CREATURE_KUAI_THE_BRUTE:
                        m_uiKuaiGuid = creature->GetGUID();
                        SetData64(TYPE_KUAI, m_uiKuaiGuid);
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case CREATURE_MING_THE_CUNNING:
                        m_uiMingGuid = creature->GetGUID();
                        SetData64(TYPE_MING, m_uiMingGuid);
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case CREATURE_HAIYAN_THE_UNSTOPPABLE:
                        m_uiHaiyanGuid = creature->GetGUID();
                        SetData64(TYPE_HAIYAN, m_uiHaiyanGuid);
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case CREATURE_XIN_THE_WEAPONMASTER_TRIGGER:
                        m_uiXinGuid = creature->GetGUID();
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case CREATURE_WHIRLING_DERVISH:
                        break;
                }
            }

            void Update(uint32 diff) override
            {
                m_mEvents.Update(diff);

                while (uint32 eventId = m_mEvents.ExecuteEvent())
                {
                    if (eventId == 1)
                    {
                        if (Creature* Saurok = instance->SummonCreature(CREATURE_GLINTROK_PILLAGER, MiscCosmeticGlintrokPath[0]))
                        {
                            Movement::MoveSplineInit init(Saurok);
                            for (auto itr : MiscCosmeticGlintrokPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 2.0f), itr.GetPositionY() + frand(-2.0f, 2.0f), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.Launch();
                            Saurok->DespawnOrUnsummon(Saurok->GetSplineDuration());
                        }

                        m_mEvents.ScheduleEvent(1, 3 * IN_MILLISECONDS);
                    }
                    break;
                }

                ScheduleBeginningTimeUpdate(diff);
                ScheduleChallengeStartup(diff);
                ScheduleChallengeTimeUpdate(diff);
            }

            void Load(const char* in) override
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);
                std::istringstream loadStream(in);

                loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
                    >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
                    >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11]
                    >> m_auiEncounter[12] >> m_auiEncounter[13];

                // Do not load an encounter as "In Progress" - reset it instead.
                for (uint8 i = 0; i < MAX_TYPES; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_mogu_shan_palace_InstanceMapScript(map);
        }
};

void AddSC_instance_mogu_shan_palace()
{
    new instance_mogu_shan_palace();
}
