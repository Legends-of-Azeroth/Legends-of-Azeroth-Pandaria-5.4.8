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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "VMapFactory.h"
#include "unga_ingoo.h"
#include "ScenarioMgr.h"

class instance_unga_ingoo : public InstanceMapScript
{
    public:
        instance_unga_ingoo() : InstanceMapScript("instance_unga_ingoo", 1048) { }

        struct instance_unga_ingoo_InstanceMapScript : public InstanceScript
        {
            instance_unga_ingoo_InstanceMapScript(Map* map) : InstanceScript(map) { }

            ObjectGuid gongGUID, templeDoorGUID;
            uint32 m_auiEncounter[CHAPTERS];
            std::list<ObjectGuid> m_UngaObjects, m_PastObjects, m_firstPirates;
            std::list<ObjectGuid> ungaBosses;
            uint32 chapterOne;
            uint32 chapterTwo;
            uint32 chapterThree;
            uint32 m_barrelsCount;
            uint32 prevType;
            ObjectGuid brewmasterBoGUID;
            ObjectGuid mainBrewmasterGUID;
            ObjectGuid birdhaverGUID;
            ObjectGuid ookOokGUID;
            ObjectGuid okuOkuGUID;
            ObjectGuid gagoonGUID;
            ObjectGuid brewKegGUID;
            EventMap m_mEvents;

            void Initialize() override
            {
                SetBossNumber(CHAPTERS);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne         = 0;
                chapterTwo         = 0;
                chapterThree       = 0;
                m_barrelsCount     = 0;
                prevType           = 0;
                brewmasterBoGUID = ObjectGuid::Empty;
                mainBrewmasterGUID = ObjectGuid::Empty;
                birdhaverGUID = ObjectGuid::Empty;
                ookOokGUID = ObjectGuid::Empty;
                gagoonGUID = ObjectGuid::Empty;

                m_firstPirates.clear();
                m_PastObjects.clear();
                m_UngaObjects.clear();
                ungaBosses.clear();
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1048, DATA_ESCORT, 0);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_BREWMASTER_BO_BREW:
                        brewmasterBoGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_BREWMASTER_BO_ESCORT:
                        mainBrewmasterGUID = creature->GetGUID();
                        break;
                    case NPC_UNGA_BIRD_HAVER:
                        birdhaverGUID = creature->GetGUID();
                        break;
                    case NPC_CAPTAIN_OOK:
                        ookOokGUID = creature->GetGUID();
                        break;
                    case NPC_OKU_OKU:
                        okuOkuGUID = creature->GetGUID();
                        break;
                    case NPC_GAGOON:
                        gagoonGUID = creature->GetGUID();
                        break;
                    case NPC_TRAINED_JUNGLE_LORY:
                        creature->setRegeneratingHealth(false);
                        creature->SetHealth((uint32)creature->GetMaxHealth() * 0.1f);
                        creature->GetMotionMaster()->MoveRandom(4.0f);
                        break;
                    case NPC_UNGA_BREW_KEG:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        brewKegGUID = creature->GetGUID();
                        ungaBosses.push_back(creature->GetGUID());
                        break;
                    case NPC_UNGA_BREWSTEALER:
                    case NPC_UNGA_SCALLYWAG:
                    case NPC_BREW_DEFENDER:
                        if (creature->GetDBTableGUIDLow())
                        {
                            creature->SetVisible(false);
                            creature->SetReactState(REACT_PASSIVE);
                            m_firstPirates.push_back(creature->GetGUID());
                        }
                        break;
                    case NPC_ZIPLINE_TO_SHIP:
                        creature->SetDisplayId(11686);
                        break;
                        // This only in heroic version, but not sure that check isHeroic() is right.
                    case NPC_GROOKA_GROOKA:
                    case NPC_RIK_RIK:
                    case NPC_BA_BAM:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        break;
                    case NPC_BUNNY_ZTO_2:
                    case NPC_BUNNY_ZTO:
                    case NPC_HOZEN_SWING:
                        creature->SetDisplayFromModel(1);
                        break;
                    case NPC_OOKIE:
                    case NPC_CHIHOZEN_BINONO:
                        creature->SetVisible(false);
                        ungaBosses.push_back(creature->GetGUID());
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature())
                {
                    switch (unit->GetEntry())
                    {
                        case NPC_TRAINED_JUNGLE_LORY:
                            if (Creature* m_haver = instance->GetCreature(GetGuidData(NPC_UNGA_BIRD_HAVER)))
                                m_haver->AI()->DoAction(ACTION_INTRO);
                            break;
                        case NPC_UNGA_BIRD_HAVER:
                            if (Creature* m_bo = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_ESCORT)))
                                m_bo->AI()->DoAction(ACTION_HAVER_DEATH);
                            break;
                        case NPC_OKU_OKU:
                        case NPC_GAGOON:
                            if (Creature* m_bo = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_ESCORT)))
                                m_bo->AI()->DoAction(ACTION_GLADIATORS_DEATH);
                            break;
                    }
                }
            }

            void OnGameObjectCreate(GameObject* go) override 
            {
                switch (go->GetEntry())
                {
                    case GO_BIGGA_UNGA_KEG:
                    case GO_BIGGA_UNGA_KEG_2:
                    case GO_UNGA_KEG:
                    case GO_UNGA_MUG:
                    case GO_BEACH_BOMB:
                    case GO_UNGA_KEG_2:
                    case GO_UNGA_MUG_2:
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        m_UngaObjects.push_back(go->GetGUID());
                        break;
                    case GO_BANQUET:
                    case GO_JUNGLE_BREW:
                        m_PastObjects.push_back(go->GetGUID());
                        break;
                }
            }

            void ActivateObjectsOnPhase()
            {
                // Pirates
                for (auto&& itr : m_firstPirates)
                {
                    if (Creature* hozenPirate = instance->GetCreature(itr))
                    {
                        hozenPirate->SetVisible(true);
                        hozenPirate->SetReactState(REACT_AGGRESSIVE);
                        hozenPirate->AI()->DoAction(ACTION_INTRO);
                    }
                }

                // Bosses
                for (auto&& itr : ungaBosses)
                {
                    if (Creature* ungaBoss = instance->GetCreature(itr))
                    {
                        ungaBoss->SetVisible(true);
                        ungaBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                    }
                }
                // Any objects
                for (auto&& itr : m_UngaObjects)
                    if (GameObject* ungaObject = instance->GetGameObject(itr))
                        ungaObject->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                m_barrelsCount = 100;
                m_mEvents.ScheduleEvent(1, 8 * IN_MILLISECONDS);
            }

            // We should make some event each ~40s, we have 4 type of assault on cauldron
            // 1. Hozen Pirates on ships, 2.Stealers, that have camouflage and try stole brew from cauldron
            // 3. Beach, that attack from north-west or east and 4. Hozens from behind jungle
            bool CreateAssault(int m_count)
            {
                if (m_count < 1)
                    return false;

                std::vector<uint32> m_type = { TYPE_PIRATE, TYPE_STEALER, TYPE_JUNGLER, TYPE_BEACH };

                // Not use same events twice
                if (prevType && std::find(m_type.begin(), m_type.end(), prevType) != m_type.end())
                    m_type.erase(std::find(m_type.begin(), m_type.end(), prevType));

                prevType = Trinity::Containers::SelectRandomContainerElement(m_type);
                AssaultByType(prevType);

                return CreateAssault(m_count - 1);
            }

            // Just return near pos as point to another
            Position GetRecalculatedPos(Position m_pos)
            {
                return { m_pos.GetPositionX() + frand(-4.0f, 4.0f), m_pos.GetPositionY() + frand(-2.5f, 2.5f), m_pos.GetPositionZ(), m_pos.GetOrientation() };
            }

            // Switch type and select how it`ll interaction
            void AssaultByType(uint32 m_type)
            {
                switch (m_type)
                {
                    case TYPE_PIRATE:
                        if (Creature* brewMaster = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_BREW)))
                            brewMaster->AI()->Talk(TALK_SPECIAL_13);

                        for (uint8 i = 0; i < 2; i++)
                            instance->SummonCreature(NPC_HOZEN_PIRATE_SHIP, ShipSpawnPoints[urand(0, 2)]);
                        break;
                    case TYPE_STEALER:
                        for (uint8 i = 0; i < 2; i++)
                            instance->SummonCreature(NPC_JUNGLE_BREWSTEALER, BrewstealerSpawnPoints[urand(0, 2)]);
                        break;
                    case TYPE_JUNGLER:
                    case TYPE_BEACH:
                    {
                        uint8 m_itr = urand(0, 1);
                        for (uint8 i = 0; i < 3; i++)
                            instance->SummonCreature(NPC_UNGA_HOZEKATEER, GetRecalculatedPos(JunglerSpawnPoints[m_itr]));
                        break;
                    }
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_ESCORT:
                        chapterOne = data;

                        if (chapterOne == DONE)
                        {
                            if (Creature* boEscort = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_ESCORT)))
                                boEscort->DespawnOrUnsummon();

                            if (Creature* boBrew = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_BREW)))
                                boBrew->AI()->DoAction(ACTION_INTRO);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1048, DATA_BARRELS, 0);

                            m_mEvents.ScheduleEvent(2, 2000); // for synch update progress
                            m_mEvents.ScheduleEvent(3, 3 * MINUTE * IN_MILLISECONDS + 59 * IN_MILLISECONDS);

                            ActivateObjectsOnPhase();
                        }
                        break;
                    case DATA_BARRELS:
                    {
                        chapterTwo = data;

                        if (chapterTwo == DONE)
                        {
                            if (Creature* boBrew = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_BREW)))
                            {
                                // Keg runer achievement
                                if (boBrew->AI()->GetData(TYPE_KEG_RUNNED))
                                    boBrew->CastSpell(boBrew, SPELL_KEG_RUNNER_ACHIEVEMENT, true);

                                // Spill no evil achievement
                                if (!boBrew->AI()->GetData(TYPE_KEG_STILLED))
                                    boBrew->CastSpell(boBrew, SPELL_SPILL_NO_EVIL_ACHIEV, true);
                            }

                            m_mEvents.Reset();
                            DoRemoveAurasDueToSpellOnPlayers(SPELL_UNGA_BREW_COLLECTED_AURA);

                            if (Creature* Ook = instance->GetCreature(GetGuidData(NPC_CAPTAIN_OOK)))
                                Ook->AI()->DoAction(ACTION_INTRO);

                            if (Creature* boBrew = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_BREW)))
                                boBrew->AI()->DoAction(ACTION_CAPTAIN_ASSAULT);

                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    sScenarioMgr->SendScenarioState(player, 1048, DATA_CAPTAIN_OOK, 0);
                        }
                        break;
                    }
                    case DATA_CAPTAIN_OOK:
                    {
                        chapterThree = data;

                        if (chapterThree == DONE)
                        {
                            if (Creature* boBrew = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_BREW)))
                                boBrew->AI()->DoAction(ACTION_CAPTAIN_DEFEAT);

                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_OOK_OOK_DEFEATED, 1, GetScenarioGUID(), time(NULL), 0, 0));

                            DoFinishLFGDungeon(499);

                            // Banquet
                            for (auto&& itr : m_PastObjects)
                                if (GameObject* ungaObject = instance->GetGameObject(itr))
                                    DoRespawnGameObject(itr, 1 * DAY);
                        }
                        break;
                    }
                    case DATA_BARRELS_PROGRESS:
                    {
                        m_barrelsCount = data;

                        if (m_barrelsCount < 201)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_CAULDRON_COUNT, m_barrelsCount, GetScenarioGUID(), time(NULL), 0, 0));
                        SaveToDB();

                        if (m_barrelsCount >= 200)
                            SetData(DATA_BARRELS, DONE);
                        break;
                    }
                }

                // Synch with data
                SetBossState(type, EncounterState(data));

                if (data == DONE)
                    SaveToDB();
            }

            void Update(uint32 diff) override
            {
                m_mEvents.Update(diff);

                while (uint32 eventId = m_mEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case 1:
                            CreateAssault(urand(1, 2));
                            m_mEvents.ScheduleEvent(1, urand(38 * IN_MILLISECONDS, 50 * IN_MILLISECONDS));
                            break;
                        case 2:
                            SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_CAULDRON_COUNT, m_barrelsCount, GetScenarioGUID(), time(NULL), 0, 0));
                            break;
                        case 3:
                            if (Creature* boBrew = instance->GetCreature(GetGuidData(NPC_BREWMASTER_BO_BREW)))
                                boBrew->AI()->SetData(TYPE_KEG_RUNNED, 0);
                            break;
                    }
                    break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_ESCORT:
                        return chapterOne;
                    case DATA_BARRELS:
                        return chapterTwo;
                    case DATA_BARRELS_PROGRESS:
                        return m_barrelsCount;
                    case DATA_CAPTAIN_OOK:
                        return chapterThree;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_BREWMASTER_BO_BREW:
                        return brewmasterBoGUID;
                    case NPC_BREWMASTER_BO_ESCORT:
                        return mainBrewmasterGUID;
                    case NPC_UNGA_BIRD_HAVER:
                        return birdhaverGUID;
                    case NPC_CAPTAIN_OOK:
                        return ookOokGUID;
                    case NPC_OKU_OKU:
                        return okuOkuGUID;
                    case NPC_GAGOON:
                        return gagoonGUID;
                    case NPC_UNGA_BREW_KEG:
                        return brewKegGUID;
                }

                return ObjectGuid::Empty;
            }

            bool IsWipe(float range, Unit* source) override
            {
                Map::PlayerList const &playerList = instance->GetPlayers();

                for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    Player* player = itr->GetSource();
                    if (!player)
                        continue;

                    if (player->IsAlive() && !player->IsGameMaster())
                        return false;
                }

                return true;
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
                saveStream << "U I " << chapterOne << ' ' << chapterTwo << ' ' << chapterThree << ' ' << m_barrelsCount;

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

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'U' && dataHead2 == 'I')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_ESCORT, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_BARRELS, chapterTwo);
                    loadStream >> temp; // chapterThree complete
                    chapterThree = temp;
                    SetData(DATA_CAPTAIN_OOK, chapterThree);
                    loadStream >> temp; // Barrels progress
                    m_barrelsCount = temp;
                    SetData(DATA_BARRELS_PROGRESS, m_barrelsCount);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_unga_ingoo_InstanceMapScript(map);
        }
};

void AddSC_instance_unga_ingoo()
{
    new instance_unga_ingoo();
}