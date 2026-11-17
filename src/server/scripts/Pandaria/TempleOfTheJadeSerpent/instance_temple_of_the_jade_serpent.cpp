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
#include "ScriptedCreature.h"
#include "MoveSplineInit.h"
#include "temple_of_the_jade_serpent.h"

static std::vector<DoorData> const doorData =
{
    { GAMEOBJECT_DOOR_LOREWALKER_STONSTEP,  DATA_LOREWALKER,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GAMEOBJECT_DOOR_WISE_MARI,            DATA_WISE_MARI,    DOOR_TYPE_ROOM,    BOUNDARY_NONE },
};

static std::vector<ScenarioBosses> const scenarioBosses =
{
    { DATA_WISE_MARI,    CRITERIA_WISE_MARI     },
    { DATA_LOREWALKER,   CRITERIA_LOREWALKER    },
    { DATA_LIU,          CRITERIA_LIU           },
    { DATA_SHA_OF_DOUBT, CRITERIA_SHA_OF_DOUBT  },
};

class instance_temple_of_jade_serpent : public InstanceMapScript
{
    public:
        instance_temple_of_jade_serpent() : InstanceMapScript("instance_temple_of_jade_serpent", 960) { }

        struct instance_temple_of_jade_serpent_InstanceMapScript : public InstanceScript
        {
            instance_temple_of_jade_serpent_InstanceMapScript(Map* map) : InstanceScript(map) { }

            std::list<ObjectGuid> corruptedCreatures;
            std::list<ObjectGuid> sunfires;
            std::list<ObjectGuid> suns;
            std::list<ObjectGuid> sunTriggers;
            std::list<ObjectGuid> shaSummoned;
            std::unordered_map<uint32, ObjectGuid> m_mGoEntryGuidMap;

            uint8 eventChoosen;
            uint32 dataStorage[MAX_DATA];
            uint32 countMinionDeads;
            uint32 sunsDefeated;
            uint32 trialDefeated;
            uint32 twinDragonsDoorState;
            uint32 liuState;
            ObjectGuid wiseMariGUID;
            ObjectGuid lorewalkerGUID;
            ObjectGuid zaoGUID;
            ObjectGuid osongGUID;
            ObjectGuid scrollGUID;
            ObjectGuid perilGUID;
            ObjectGuid strifeGUID;
            ObjectGuid liuGUID;
            ObjectGuid yuLonGUID;
            ObjectGuid shaGUID;
            ObjectGuid twinDragonsDoorGUID;
            ObjectGuid shaDoorGUID;

            void Initialize() override
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                twinDragonsDoorState  = NOT_STARTED;
                liuState              = NOT_STARTED;
                sunsDefeated          = 0;
                trialDefeated         = 0;
                eventChoosen          = 0;
                countMinionDeads      = 0;
                wiseMariGUID = ObjectGuid::Empty;
                lorewalkerGUID = ObjectGuid::Empty;
                zaoGUID = ObjectGuid::Empty;
                osongGUID = ObjectGuid::Empty;
                scrollGUID = ObjectGuid::Empty;
                perilGUID = ObjectGuid::Empty;
                strifeGUID = ObjectGuid::Empty;
                liuGUID = ObjectGuid::Empty;
                twinDragonsDoorGUID = ObjectGuid::Empty;
                shaDoorGUID = ObjectGuid::Empty;
                yuLonGUID = ObjectGuid::Empty;
                shaGUID = ObjectGuid::Empty;
                memset(dataStorage, 0, MAX_DATA * sizeof(uint32));

                if (instance->IsChallengeDungeon())
                    LoadScenarioInfo(scenarioBosses, CRITERIA_ENEMIES);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (instance->IsChallengeDungeon())
                    SendChallengeInfo(player, SCENARIO_ID);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GAMEOBJECT_DOOR_LIU_FLAMEHEART:
                        twinDragonsDoorGUID = go->GetGUID();
                        break;
                    case GAMEOBJECT_DOOR_SHA_OF_DOUBT:
                        shaDoorGUID = go->GetGUID();
                        break;
                    case GAMEOBJECT_DOOR_WISE_MARI:
                        AddDoor(go, true);
                        break;
                    case GAMEOBJECT_DOOR_WISE_MARI_EX_EXIT:
                    case GAMEOBJECT_DOOR_LOREWALKER_STONSTEP:
                        m_mGoEntryGuidMap.insert(std::make_pair(go->GetEntry(), go->GetGUID()));
                        break;
                    case GO_CHALLENGE_DOOR:
                        SetChallengeDoorGuid(go->GetGUID());
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

            void OnCreatureCreate(Creature* creature) override
            {
                if (instance->IsChallengeDungeon() && creature->isDead())
                    creature->Respawn();

                switch (creature->GetEntry())
                {
                    case NPC_WISE_MARI:
                        wiseMariGUID = creature->GetGUID();
                        break;
                    case NPC_YU_LON:
                        yuLonGUID = creature->GetGUID();
                        break;
                    case NPC_STRIFE:
                        strifeGUID = creature->GetGUID();
                        break;
                    case NPC_PERIL:
                        perilGUID = creature->GetGUID();
                        break;
                    case NPC_HAUNTING_SHA:
                        shaSummoned.push_back(creature->GetGUID());
                        break;
                    case NPC_ZAO_SUN_TRIGGER:
                        creature->SetFaction(14);
                        creature->SetDisplayId(11686);
                        creature->SetReactState(REACT_PASSIVE);
                        sunTriggers.push_back(creature->GetGUID());
                        break;
                    case NPC_ZAO_SUN:
                        creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        suns.push_back(creature->GetGUID());
                        break;
                    case NPC_ZAO_SUNSEEKER:
                        zaoGUID = creature->GetGUID();
                        break;
                    case NPC_OSONG:
                        osongGUID = creature->GetGUID();
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION, false);
                        break;
                    case NPC_LOREWALKTER_STONESTEP:
                        lorewalkerGUID = creature->GetGUID();
                        creature->CastSpell(creature, SPELL_ROOT_SELF, true);
                        break;
                    case NPC_SCROLL:
                        scrollGUID = creature->GetGUID();
                        break;
                    case NPC_WISE_DRAGON:
                    case NPC_WELL:
                    case NPC_STRIF:
                    case NPC_HEROINE:
                    case NPC_MOON:
                    case NPC_GOLDEN_BEETLE:
                    case NPC_CRYBABY_HOZEN:
                    case NPC_THE_NODDING_TIGER:
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION, false);
                        corruptedCreatures.push_back(creature->GetGUID());
                        break;
                    case NPC_HAUNTING_SHA_3:
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION, false);
                        creature->CastSpell(creature, SPELL_HAUNTING_GAZE, false);
                        corruptedCreatures.push_back(creature->GetGUID());
                        break;
                    case NPC_THE_SONGBIRD_QUEEN:
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION, false);
                        creature->CastSpell(creature, SPELL_SINGING_SONGBIRD, false);
                        corruptedCreatures.push_back(creature->GetGUID());
                        break;
                    case NPC_TALKING_FISH:
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION, false);
                        creature->CastSpell(creature, SPELL_WATER_BUBBLE, false);
                        corruptedCreatures.push_back(creature->GetGUID());
                        break;
                    case NPC_JIANG:
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION_2, false);
                        creature->CastSpell(creature, SPELL_JUGGLER_JIANG, false);
                        corruptedCreatures.push_back(creature->GetGUID());
                        break;
                    case NPC_XIANG:
                        creature->CastSpell(creature, SPELL_SHA_CORRUPTION_2, false);
                        creature->CastSpell(creature, SPELL_JUGGLER_XIANG, false);
                        corruptedCreatures.push_back(creature->GetGUID());
                        break;
                    case NPC_SWIRLING_SUNFIRE:
                        creature->SetDisplayId(11686);
                        creature->SetReactState(REACT_PASSIVE);
                        sunfires.push_back(creature->GetGUID());
                        break;
                    case NPC_JADE_FIRE:
                        creature->SetFaction(14);
                        creature->SetDisplayId(11686);
                        creature->SetReactState(REACT_PASSIVE);
                        creature->CastSpell(creature, SPELL_JADE_FIRE_PERIODIC, true);
                        creature->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                        break;
                    case NPC_LIU_FLAMEHEART:
                        liuGUID = creature->GetGUID();
                        break;
                    case NPC_SHA_OF_DOUBT:
                        shaGUID = creature->GetGUID();
                        break;
                    case NPC_FIGMENT_OF_DOUBT:
                        if (creature->ToTempSummon())
                        {
                            if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
                            {
                                creature->CastSpell(creature, SPELL_FIGMENT_OF_DOUBT, false);
                                creature->SetDisplayId(summoner->GetDisplayId());
                                creature->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
                                summoner->CastSpell(creature, SPELL_COPY_WEAPON, false);
                                summoner->CastSpell(creature, SPELL_COPY_WEAPON_2, false);

                                uint32 prevItem = creature->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID);
                                if (Player* player = summoner->ToPlayer())
                                {
                                    if (Item* mainItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                        creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, mainItem->GetEntry());

                                }
                                else
                                    creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, summoner->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID));

                                prevItem = creature->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1);

                                if (Player* player = summoner->ToPlayer())
                                {
                                    if (Item* offItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                                        creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, offItem->GetEntry());
                                }
                                else
                                    creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1, summoner->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 1));

                                prevItem = creature->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2);

                                if (Player* player = summoner->ToPlayer())
                                {
                                    if (Item* rangedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                                        creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, rangedItem->GetEntry());
                                }
                                else
                                    creature->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2, summoner->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 2));
                            }
                        }
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->ToCreature())
                {
                    switch (unit->GetEntry())
                    {
                        case NPC_ZAO_SUN:
                            unit->CastSpell(unit, SPELL_EXTRACT_SHA, false);
                            break;
                        case NPC_SCROLL:
                            eventChoosen = urand(EVENT_LOREWALKER_STONESTEP_SUNS, EVENT_LOREWALKER_STONESTEP_TRIAL);
                            switch (eventChoosen)
                            {
                                case EVENT_LOREWALKER_STONESTEP_SUNS:
                                    if (Creature* lorewalker = instance->GetCreature(lorewalkerGUID))
                                       if (lorewalker->IsAIEnabled)
                                          lorewalker->AI()->DoAction(0);

                                    for (auto guid : corruptedCreatures)
                                    {
                                        if (Creature* creature = instance->GetCreature(guid))
                                        {
                                            creature->AddAura(SPELL_DRAW_SHA, creature);
                                            creature->CastSpell(unit, SPELL_DRAW_SHA_2, false);
                                            creature->DespawnOrUnsummon(2 * IN_MILLISECONDS);
                                        }
                                    }

                                    for (int i = 0; i < 5; ++i)
                                        instance->SummonCreature(NPC_ZAO_SUN, SunsSummonPosition[i]);

                                    for (int i = 0; i < 5; ++i)
                                        instance->SummonCreature(NPC_ZAO_SUN_TRIGGER, TriggerSunsSummonPosition[i]);

                                    if (TempSummon* sum = unit->SummonCreature(NPC_ZAO_SUNSEEKER, 846.877f, -2449.110f, 175.044f))
                                        sum->SetFacingTo(4.450f);

                                    for (auto guid : sunfires)
                                    {
                                        if (Creature* creature = instance->GetCreature(guid))
                                            creature->CastSpell(creature, SPELL_BLUSTERING_VORTEX, false);
                                    }
                                    break;
                                case EVENT_LOREWALKER_STONESTEP_TRIAL:
                                    if (Creature* lorewalker = instance->GetCreature(lorewalkerGUID))
                                       if (lorewalker->IsAIEnabled)
                                          lorewalker->AI()->DoAction(1);

                                    for (auto guid : corruptedCreatures)
                                    {
                                        if (Creature* creature = instance->GetCreature(guid))
                                        {
                                            creature->AddAura(SPELL_DRAW_SHA, creature);
                                            creature->CastSpell(unit, SPELL_DRAW_SHA_2, false);
                                            creature->DespawnOrUnsummon(2 * IN_MILLISECONDS);
                                        }
                                    }

                                    if (TempSummon* strife = unit->SummonCreature(NPC_STRIFE, 847.530f, -2469.184f, 174.960f))
                                    {
                                        strife->SetFacingTo(1.525f);
                                        strife->SetReactState(REACT_PASSIVE);
                                    }

                                    if (TempSummon* peril = unit->SummonCreature(NPC_PERIL, 836.906f, -2465.859f, 174.960f))
                                    {
                                        peril->SetFacingTo(1.014f);
                                        peril->SetReactState(REACT_PASSIVE);
                                    }
                                    break;
                            }
                            break;
                        case NPC_MINION_OF_DOUBTS:
                        case NPC_LESSER_SHA:
                            if (GetData(DATA_LIU_STATE) != DONE)
                            {
                                ++countMinionDeads;
                                if (countMinionDeads == 17)
                                {
                                    instance->SummonCreature(NPC_LIU_FLAMEHEART, LiuSpawnPosition);
                                    SetData(DATA_LIU_STATE, DONE);
                                }
                            }
                            break;
                    }
                }

                if (instance->IsChallengeDungeon() && !IsChallengeModeCompleted())
                    if (Creature* creature = unit->ToCreature())
                        UpdateConditionInfo(creature, ENEMIES_COUNT);
            }

            void Update(uint32 diff) override
            {
                ScheduleBeginningTimeUpdate(diff);
                ScheduleChallengeStartup(diff);
                ScheduleChallengeTimeUpdate(diff);
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_TWIN_DRAGONS_DOOR:
                        twinDragonsDoorState = data;
                        if (data == DONE)
                        {
                            if (GameObject* go = instance->GetGameObject(twinDragonsDoorGUID))
                                go->SetGoState(GO_STATE_ACTIVE);
                        }
                        else if (data == NOT_STARTED)
                        {
                            if (GameObject* go = instance->GetGameObject(twinDragonsDoorGUID))
                                go->SetGoState(GO_STATE_READY);
                        }
                        break;
                    case DATA_LIU_STATE:
                        liuState = data;
                        break;
                    case DATA_SUN_STATE:
                        sunsDefeated += data;
                        if (sunsDefeated == 5)
                        {
                            Creature* zao = instance->GetCreature(zaoGUID);
                            {
                                if (zao->IsAIEnabled)
                                    zao->AI()->DoAction(2);
                            }

                            for (auto guid : suns)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                    creature->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                            }

                            for (auto&& guid : shaSummoned)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                {
                                    creature->Respawn(true);
                                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                                    if (creature->IsAIEnabled)
                                        creature->AI()->DoAction(0);
                                }
                            }

                            for (auto guid : sunfires)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                    creature->RemoveAura(SPELL_BLUSTERING_VORTEX);
                            }
                        }
                        break;
                    case DATA_SUN:
                        if (data == DONE)
                        {
                            sunsDefeated = 0;

                            for (auto guid : suns)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                {
                                    creature->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, creature);
                                    creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    if (creature->GetAI())
                                        creature->GetAI()->DoAction(0);
                                }
                            }

                            for (auto guid : sunTriggers)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                   creature->CastSpell(creature, SPELL_SUNFIRE_EXPLOSION, false);
                            }
                        }
                        break;
                    case DATA_LIU:
                        if (data == DONE)
                        {
                            if (Creature* creature = instance->GetCreature(liuGUID))
                            {
                                creature->RemoveAura(SPELL_JADE_ESSENCE);
                                creature->CastSpell(creature, SPELL_TRANSFORM_VISUAL, false);
                                creature->RemoveAura(SPELL_POSSESSED_BY_SHA);
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                                for (int i = 0; i < 3; i++)
                                {
                                    if (Creature* sha = instance->SummonCreature(NPC_MINION_OF_DOUBTS, ShaSummonPosition[i]))
                                    {
                                        float x, y, z;
                                        x = sha->GetPositionX();
                                        y = sha->GetPositionY();
                                        z = sha->GetPositionZ();
                                        GetPositionWithDistInOrientation(sha, 18.0f, sha->GetOrientation(), x, y);
                                        Movement::MoveSplineInit init(sha);
                                        init.MoveTo(x, y, z);
                                        init.SetVelocity(9.0f);
                                        init.Launch();
                                    }
                                }

                                if (GameObject* go = instance->GetGameObject(shaDoorGUID))
                                    go->SetGoState(GO_STATE_ACTIVE);

                                if (Creature* sha = instance->GetCreature(shaGUID))
                                    sha->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            }
                        }
                        break;
                    case DATA_ZAO:
                        if (data == DONE)
                        {
                            SetBossState(DATA_LOREWALKER, DONE);
                            SetData(DATA_LOREWALKER, DONE);
                        }
                        break;
                    case DATA_TRIAL:
                        if (data == DONE)
                        {
                            trialDefeated++;
                            if (trialDefeated == 2)
                            {                     
                                SetBossState(DATA_LOREWALKER, DONE);
                                SetData(DATA_LOREWALKER, DONE);
                            }
                        }
                        break;
                    case DATA_LOREWALKER:
                        dataStorage[type] = data;
                        if (data == FAIL)
                        {
                            if (Creature* creature = instance->GetCreature(lorewalkerGUID))
                            {
                                creature->RemoveAura(SPELL_MEDITATION);
                                creature->Respawn();
                                if (creature->GetAI())
                                    creature->GetAI()->Reset();
                            }

                            if (Creature* creature = instance->GetCreature(perilGUID))
                            {
                                creature->DespawnOrUnsummon();
                                creature->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                            }

                            if (Creature* creature = instance->GetCreature(strifeGUID))
                            {
                                creature->DespawnOrUnsummon();
                                creature->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                            }

                            if (Creature* creature = instance->GetCreature(zaoGUID))
                            {
                                creature->DespawnOrUnsummon();
                                creature->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                            }

                            if (Creature* osong = instance->GetCreature(osongGUID))
                                osong->DespawnOrUnsummon(1.5 * IN_MILLISECONDS);

                            if (Creature* creature = instance->GetCreature(scrollGUID))
                                creature->Respawn();

                            for (auto guid : suns)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                {
                                    creature->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                                    creature->DespawnOrUnsummon();
                                }
                            }

                            suns.clear();

                            for (auto guid : shaSummoned)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                    creature->DespawnOrUnsummon();
                            }

                            shaSummoned.clear();

                            for (auto guid : sunfires)
                            {
                                if (Creature* creature = instance->GetCreature(guid))
                                    creature->RemoveAura(SPELL_BLUSTERING_VORTEX);
                            }
                        }
                        else if (data == DONE)
                        {
                            if (Creature* osong = instance->GetCreature(osongGUID))
                                osong->DespawnOrUnsummon(1.5 * IN_MILLISECONDS);

                            if (Creature* lorewalker = instance->GetCreature(lorewalkerGUID))
                            {
                                lorewalker->SetReactState(REACT_PASSIVE);

                                if (lorewalker->IsAIEnabled)
                                    lorewalker->AI()->DoAction(2);
                            }

                            Map::PlayerList const& PlayerList = instance->GetPlayers();

                            if (!PlayerList.isEmpty())
                            {
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                {
                                    if (Player* plr = i->GetSource())
                                        plr->CastSpell(plr, SPELL_LOREWALKER_ALACRITY, false);
                                }
                            }
                        }
                        break;
                    case DATA_SHA_OF_DOUBT:
                        switch (data)
                        {
                            case DONE:
                                if (Creature* sha = instance->GetCreature(shaGUID))
                                {
                                    if (GameObject* scroll = sha->FindNearestGameObject(221413, 100.0f))
                                        scroll->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                    std::list<GameObject*> gobjs;
                                    GetGameObjectListWithEntryInGrid(gobjs, sha, 213183, 100.0f);
                                    for (auto&& go : gobjs)
                                        go->Delete();
                                }
                            case NOT_STARTED:
                            case FAIL:
                                if (GameObject* go = instance->GetGameObject(shaDoorGUID))
                                    go->SetGoState(GO_STATE_ACTIVE);
                                break;
                            case IN_PROGRESS:
                                if (GameObject* go = instance->GetGameObject(shaDoorGUID))
                                    go->SetGoState(GO_STATE_READY);
                                break;
                        }
                        break;
                    default:
                        if (type < MAX_DATA)
                            dataStorage[type] = data;
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_TWIN_DRAGONS_DOOR:
                        return twinDragonsDoorState;
                    case DATA_LIU_STATE:
                        return liuState;
                    default:
                        if (type < MAX_DATA)
                            return dataStorage[type];
                        break;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_LIU:
                        return liuGUID;
                    case DATA_WISE_MARI:
                        return wiseMariGUID;
                    case DATA_ZAO:
                        return zaoGUID;
                    case DATA_SHA_OF_DOUBT:
                        return shaGUID;
                    case DATA_LOREWALKER:
                        return lorewalkerGUID;
                }

                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state == DONE)
                {
                    switch (type)
                    {
                        case DATA_LOREWALKER:
                        case DATA_WISE_MARI:
                            if (GetBossState(DATA_WISE_MARI) == DONE && GetBossState(DATA_LOREWALKER) == DONE)
                            {
                                HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GAMEOBJECT_DOOR_WISE_MARI_EX_EXIT));
                                HandleGameObject(ObjectGuid::Empty, true, GetGameObjectFromStorage(GAMEOBJECT_DOOR_LOREWALKER_STONSTEP));

                                SetData(DATA_TWIN_DRAGONS_DOOR, DONE); // should be opened only if both encounters is done
                            }
                            break;
                        default:
                            break;
                    }
                }
                return true;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "T J S " << GetBossSaveData() << twinDragonsDoorState << " " << liuState;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* in) override
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

                if (dataHead1 == 'T' && dataHead2 == 'J' && dataHead3 == 'S')
                {
                    for (uint8 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp;
                    twinDragonsDoorState = temp ? DONE : NOT_STARTED;

                    loadStream >> temp;
                    liuState = temp ? DONE : NOT_STARTED;
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_temple_of_jade_serpent_InstanceMapScript(map);
        }
};

void AddSC_instance_temple_of_the_jade_serpent()
{
    new instance_temple_of_jade_serpent();
}
