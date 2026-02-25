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

#ifndef TRINITY_INSTANCE_DATA_H
#define TRINITY_INSTANCE_DATA_H

#include "ZoneScript.h"
#include "World.h"
#include "ObjectMgr.h"
#include "WorldStateBuilder.h"

#define OUT_SAVE_INST_DATA             TC_LOG_DEBUG("scripts", "Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    TC_LOG_DEBUG("scripts", "Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          TC_LOG_DEBUG("scripts", "Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    TC_LOG_DEBUG("scripts", "Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        TC_LOG_ERROR("scripts", "Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

class Map;
class Unit;
class Player;
class GameObject;
class Creature;

typedef std::set<GameObject*> DoorSet;
typedef std::set<Creature*> MinionSet;

enum EncounterFrameType
{
    ENCOUNTER_FRAME_SET_COMBAT_RES_LIMIT    = 0,
    ENCOUNTER_FRAME_RESET_COMBAT_RES_LIMIT  = 1,
    ENCOUNTER_FRAME_ENGAGE                  = 2,
    ENCOUNTER_FRAME_DISENGAGE               = 3,
    ENCOUNTER_FRAME_UPDATE_PRIORITY         = 4,
    ENCOUNTER_FRAME_ADD_TIMER               = 5,
    ENCOUNTER_FRAME_ENABLE_OBJECTIVE        = 6,
    ENCOUNTER_FRAME_UPDATE_OBJECTIVE        = 7,
    ENCOUNTER_FRAME_DISABLE_OBJECTIVE       = 8,
    ENCOUNTER_FRAME_UNK7                    = 9,    // Seems to have something to do with sorting the encounter units
    ENCOUNTER_FRAME_ADD_COMBAT_RES_LIMIT    = 10
};

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4,
    TO_BE_DECIDED = 5
};

enum DoorType
{
    DOOR_TYPE_ROOM          = 0,    // Door can open if encounter is not in progress
    DOOR_TYPE_PASSAGE       = 1,    // Door can open if encounter is done
    DOOR_TYPE_SPAWN_HOLE    = 2,    // Door can open if encounter is in progress, typically used for spawning places
    MAX_DOOR_TYPES
};

enum BoundaryType
{
    BOUNDARY_NONE = 0,
    BOUNDARY_N,
    BOUNDARY_S,
    BOUNDARY_E,
    BOUNDARY_W,
    BOUNDARY_NE,
    BOUNDARY_NW,
    BOUNDARY_SE,
    BOUNDARY_SW,
    BOUNDARY_MAX_X = BOUNDARY_N,
    BOUNDARY_MIN_X = BOUNDARY_S,
    BOUNDARY_MAX_Y = BOUNDARY_W,
    BOUNDARY_MIN_Y = BOUNDARY_E
};

enum HighRecoveryTypeSpells
{
    SPELL_INSANITY_R              = 95809,
    SPELL_TEMPORAL_DISPLACEMENT_R = 80354,
    SPELL_EXHAUSTION_R            = 57723, // from heroism
    SPELL_SATED_R                 = 57724, // From Bloodlust

    // Determination Spells
    SPELL_DETERMINATION_LFR       = 139068,
};

enum DampeningSpells
{
    SPELL_DAMPENING_FIELD_R        = 126816, // Mogushan Vaults
    SPELL_KLAXXI_RESONANCE_R       = 126841, // Heart of Fear
    SPELL_RITUAL_OF_PURIFICATION_R = 126848, // Terrace of the Endless Spring
    SPELL_SHADO_PAN_ONSLAUGHT_R    = 149070, // Throne of Thunder
};

const std::vector<uint32> BloodLustType =
{
    SPELL_INSANITY_R,
    SPELL_TEMPORAL_DISPLACEMENT_R,
    SPELL_EXHAUSTION_R,
    SPELL_SATED_R,
};

typedef std::map<BoundaryType, float> BossBoundaryMap;

struct DoorData
{
    uint32 entry, bossId;
    DoorType type;
    uint32 boundary;
};

struct MinionData
{
    uint32 entry, bossId;
};

struct BossInfo
{
    BossInfo() : state(TO_BE_DECIDED) { }
    EncounterState state;
    DoorSet door[MAX_DOOR_TYPES];
    MinionSet minion;
    BossBoundaryMap boundary;
};

struct DoorInfo
{
    explicit DoorInfo(BossInfo* _bossInfo, DoorType _type, BoundaryType _boundary)
        : bossInfo(_bossInfo), type(_type), boundary(_boundary) { }
    BossInfo* bossInfo;
    DoorType type;
    BoundaryType boundary;
};

struct MinionInfo
{
    explicit MinionInfo(BossInfo* _bossInfo) : bossInfo(_bossInfo) { }
    BossInfo* bossInfo;
};

typedef std::multimap<uint32 /*entry*/, DoorInfo> DoorInfoMap;
typedef std::pair<DoorInfoMap::const_iterator, DoorInfoMap::const_iterator> DoorInfoMapBounds;

typedef std::map<uint32 /*entry*/, MinionInfo> MinionInfoMap;
typedef std::map<uint32 /*type*/, ObjectGuid /*guid*/> ObjectGuidMap;

enum ChallengeMedals
{
    MedalTypeNone,
    MedalTypeBronze,
    MedalTypeSilver,
    MedalTypeGold,
    MaxMedalType,
};

struct ScenarioBosses
{
    ScenarioBosses()
    {
        bossId = 0;
        scenarioId = 0;
    }

    ScenarioBosses(uint32 id, uint32 scenario)
    {
        bossId = id;
        scenarioId = scenario;
    }

    uint32 bossId;
    uint32 scenarioId;
};

struct CriteriaProgressData
{
    CriteriaProgressData(uint32 id, uint64 quantity, ObjectGuid instanceGuid, uint32 date, uint32 startTime, uint8 flags)
    {
        Id                = id;
        Quantity          = quantity;
        InstanceGuid      = instanceGuid;
        Date              = date;
        TimeFromStart     = startTime;
        TimeFromCreate    = startTime;
        Flags             = flags;
    }

    CriteriaProgressData()
    {
        Id                = 0;
        Quantity          = 0;
        InstanceGuid      = ObjectGuid::Empty;
        Date              = 0;
        TimeFromStart     = 0;
        TimeFromCreate    = 0;
        Flags             = 0;
    }

    ObjectGuid InstanceGuid;
    uint64 Quantity;
    uint32 Id;
    uint32 Date;
    uint32 TimeFromStart;
    uint32 TimeFromCreate;
    uint8  Flags;
};

struct ScenarioData
{
    ScenarioData(uint32 scenarioId, uint32 stepId, uint32 currWave, uint32 maxWave, uint32 timer, uint32 criteriaCount, uint32 bonusAvailable, bool complete)
    {
        ScenarioId              = scenarioId;
        StepId                  = stepId;
        WaveCurrent             = currWave;
        WaveMax                 = maxWave;
        TimerDuration           = timer;
        CriteriaCount           = criteriaCount;
        BonusObjectiveDataCount = bonusAvailable;
        ScenarioComplete        = complete;

        criteriaProgress.resize(criteriaCount);
    }

    ScenarioData(uint32 scenarioId, uint32 stepId)
    {
        ScenarioId              = scenarioId;
        StepId                  = stepId;
        WaveCurrent             = 0;
        WaveMax                 = 0;
        TimerDuration           = 0;
        CriteriaCount           = 0;
        BonusObjectiveDataCount = 0;
        ScenarioComplete        = false;

        criteriaProgress.clear();
    }

    ScenarioData()
    {
        ScenarioId              = 0;
        StepId                  = 0;
        WaveCurrent             = 0;
        WaveMax                 = 0;
        TimerDuration           = 0;
        CriteriaCount           = 0;
        BonusObjectiveDataCount = 0;
        ScenarioComplete        = false;

        criteriaProgress.clear();
    }

    void AddCriteriaProgress(CriteriaProgressData data)
    {
        criteriaProgress.push_back(data);
    }

    uint32 ScenarioId;
    uint32 StepId;
    uint32 WaveCurrent;
    uint32 WaveMax;
    uint32 TimerDuration;
    uint32 CriteriaCount;
    uint32 BonusObjectiveDataCount;
    bool  ScenarioComplete;

    std::vector<CriteriaProgressData> criteriaProgress;
};

class InstanceScript : public ZoneScript
{
    public:
        explicit InstanceScript(Map* map);

        virtual ~InstanceScript() { }

        Map* instance;

        //On creation, NOT load.
        virtual void Initialize() { }

        //On load
        virtual void Load(char const* data) { LoadBossState(data); }

        //When save is needed, this function generates the data
        virtual std::string GetSaveData() { return GetBossSaveData(); }

        void SaveToDB();

        virtual void Update(uint32 /*diff*/) { }

        //Used by the map's CanEnter function.
        //This is to prevent players from entering during boss encounters.
        virtual bool IsEncounterInProgress() const;

        //Called when a player successfully enters the instance.
        virtual void OnPlayerEnter(Player* /*player*/) { }

        //Called when a player leaves the instance.
        virtual void OnPlayerLeave(Player* /*player*/) { }

        //Called when a player stops watching a movie
        virtual void OnMovieEnded(Player* /*player*/) { }

        //Handle open / close objects
        //use HandleGameObject(0, boolen, GO); in OnObjectCreate in instance scripts
        //use HandleGameObject(GUID, boolen, NULL); in any other script
        void HandleGameObject(ObjectGuid guid, bool open, GameObject* go = nullptr);

        //change active state of doors or buttons
        void DoUseDoorOrButton(ObjectGuid guid, uint32 withRestoreTime = 0, bool useAlternativeState = false);

        //Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(ObjectGuid guid, uint32 timeToDespawn = MINUTE);

        //sends world state update to all players in instance
        void DoUpdateWorldState(uint32 worldstateId, uint32 worldstateValue);

        // Send Notify to all players in instance
        void DoSendNotifyToInstance(char const* format, ...);

        // Reset Achievement Criteria
        void DoResetAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, bool evenIfCriteriaComplete = false);

        // Complete Achievement for all players in instance
        DECLSPEC_DEPRECATED void DoCompleteAchievement(uint32 achievement) ATTR_DEPRECATED;
        //void DoCompleteAchievement(uint32 achievement);

        // Update Achievement Criteria for all players in instance
        void DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1 = 0, uint32 miscValue2 = 0, Unit* unit = NULL);

        void DoStartCriteria(CriteriaStartTypes type, uint32 entry);
        void DoStopTimedAchievement(CriteriaStartTypes type, uint32 entry);

        // Remove Auras due to Spell on all players in instance
        void DoRemoveAurasDueToSpellOnPlayers(uint32 spell);
        void DoRemoveBloodLustDebuffSpellOnPlayers();

        // Add aura on all players in instance
        void DoAddAuraOnPlayers(uint32 spell);

        // Dampening creatures with specific aura
        void DoDampeningForCreatures(Creature* creature);
        void DoRemoveDampeningFromCreatures();

        // Remove aura from stack on all players in instance
        void DoRemoveAuraFromStackOnPlayers(uint32 spell, uint64 casterGUID = 0, AuraRemoveMode mode = AURA_REMOVE_BY_DEFAULT, uint32 num = 1);

        // Cast spell on all players in instance
        void DoCastSpellOnPlayers(uint32 spell, bool aliveOnly = false);

        void DoSetAlternatePowerOnPlayers(int32 value);

        void DoModifyPlayerCurrencies(uint32 id, int32 value);

        void DoNearTeleportPlayers(const Position pos, bool casting = false);

        void DoStartMovie(uint32 movieId);

        void DoKilledMonsterKredit(uint32 questId, uint32 entry, ObjectGuid guid = ObjectGuid::Empty);

        void DoFinishLFGDungeon(uint32 dungeonId);

        // Return wether server allow two side groups or not
        bool ServerAllowsTwoSideGroups() { return sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP); }

        virtual bool SetBossState(uint32 id, EncounterState state);
        EncounterState GetBossState(uint32 id) const { return id < bosses.size() ? bosses[id].state : TO_BE_DECIDED; }
        BossBoundaryMap const* GetBossBoundary(uint32 id) const { return id < bosses.size() ? &bosses[id].boundary : NULL; }

        // Achievement criteria additional requirements check
        // NOTE: not use this if same can be checked existed requirement types from AchievementCriteriaRequirementType
        virtual bool CheckAchievementCriteriaMeet(uint32 /*criteria_id*/, Player const* /*source*/, Unit const* /*target*/ = NULL, uint32 /*miscvalue1*/ = 0);

        // Checks boss requirements (one boss required to kill other)
        virtual bool CheckRequiredBosses(uint32 /*bossId*/, Player const* /*player*/ = NULL) const { return true; }

        // Checks encounter state at kill/spellcast
        void UpdateEncounterState(EncounterCreditType type, uint32 creditEntry, Unit* source);

        // Used only during loading
        void SetCompletedEncountersMask(uint32 newMask) { completedEncounters = newMask; }

        // Returns completed encounters mask for packets
        uint32 GetCompletedEncounterMask() const { return completedEncounters; }

        void SendEncounterUnit(uint32 type, Unit* unit = NULL, uint8 param1 = 0, uint8 param2 = 0);

        // Check if all players are dead (except gamemasters)
        // If set value then chech not whole instance
        virtual bool IsWipe(float range = 0.0f, Unit* source = NULL);

        virtual void FillInitialWorldStates(WorldStateBuilder& /*builder*/) { }

        // For Raids where entrance is dynamic
        virtual Position GetCurrentEntrancePosition() { return {0.0f, 0.0f, 0.0f, 0.0f };}

        // ReCheck PhaseTemplate related conditions
        void UpdatePhasing();

        // Promotion players by buff if wipe. Clear it if encounter was killed.
        void PromotionIfLFR(uint32 data);
        uint8 GetDeterminationValue() { return determinationCount; }

        // Dampening condition
        void SetDampening(bool force = true) { hasDampening = force; }
        bool GetDampening() { return hasDampening; }
        void SetDampeningSpell(uint32 spellId) { dampeningSpellId = spellId; }
        uint32 GetDampeningSpell() { return dampeningSpellId; }

        void UpdateResurrectionsCount() { resurrections++; }
        bool CanUseResurrection();
        void ResetResurrectionsCount() { resurrections = 0; }

        ObjectGuid GetObjectGuid(uint32 type) const;
        virtual ObjectGuid GetGuidData(uint32 type) const override;

        // Scenarios
        void SendScenarioState(ScenarioData data, Player* player = nullptr);
        void SendScenarioProgressUpdate(CriteriaProgressData data, Player* player = nullptr);
        void SetScenarioId(uint32 newScenarioId) { scenarioId = newScenarioId; scenarioGuid = ObjectGuid::Create<HighGuid::Scenario>(instance->GetInstanceId(), scenarioId); } // used only for 5ppl dungeons
        uint32 GetScenarioId() const { return scenarioId; }
        uint8 GetcurrentScenarioStep() const { return scenarioStep; }
        ObjectGuid GetScenarioGUID() const { return scenarioGuid; }

        // Challenges
        void SendChallengeStartTimer(uint32 time);
        void SendChallengeStartElapsedTimer(uint32 timerId, uint32 time);
        void SendChallengeStopElapsedTimer(uint32 timerId, bool keepTimer = false);
        void SetChallengeModeStarted();
        void ScheduleChallengeStartup(uint32 diff);
        void ScheduleChallengeTimeUpdate(uint32 diff);
        void ScheduleBeginningTimeUpdate(uint32 diff);
        void SendChallengeNewPlayerRecord();
        void SendChallengeModeComplete(uint32 money, uint32 valor);
        void SaveChallengeDatasIfNeeded();
        void SaveNewGroupChallenge(uint32 guildId = 0);
        void RewardChallengers(uint32& money, uint32& valor);
        void RewardNewRealmRecord(RealmCompletedChallenge* oldChallenge = nullptr);
        void SetChallengeDoorGuid(ObjectGuid guid) { challengeDoorGuids.push_back(guid); }
        void SetConditionCompleted(bool completed) { conditionCompleted = completed; }
        void SendChallengeInfo(Player* player, uint32 scenarioId);
        void UpdateConditionInfo(Creature* creature, uint32 counter);
        bool IsChallengeModeStarted() const { return challengeStarted; }
        bool IsChallengeModeCompleted() const { return challengeCompleted; }
        bool IsConditionCompleted() const { return conditionCompleted; }
        uint32 GetBeginingTime() const { return beginningTime; }
        uint32 GetChallengeTime() const { return challengeTime; }

        void GameObjectRemoved(GameObject* go) override;

        void UpdateDynamicHealth(ObjectGuid single = ObjectGuid::Empty);
        void AddFlexCreature(ObjectGuid guid) { flexCreatures.push_back(guid); }

    protected:
        void SetBossNumber(uint32 number);
        void LoadDoorData(std::vector<DoorData> const& data);
        void LoadMinionData(std::vector<MinionData> const& data);
        void LoadScenarioInfo(std::vector<ScenarioBosses> const& scenarios, uint32 condition);

        void AddDoor(GameObject* door, bool add);
        void AddMinion(Creature* minion, bool add);

        void UpdateDoorState(GameObject* door);
        void UpdateMinionState(Creature* minion, EncounterState state);

        std::string LoadBossState(char const* data);
        std::string GetBossSaveData();
        uint32 GetEncountersDoneCount(uint32 encounters);
    private:
        std::vector<BossInfo> bosses;
        std::vector<ScenarioBosses> bossesScenarios;
        std::vector<ObjectGuid> dampenedGUIDs;
        std::vector<ObjectGuid> flexCreatures;
        DoorInfoMap doors;
        MinionInfoMap minions;
        ObjectGuidMap _objectGuids;
        uint32 completedEncounters; // completed encounter mask, bit indexes are DungeonEncounter.dbc boss numbers, used for packets
        uint8 resurrections;
        uint8 determinationCount;

        ObjectGuid scenarioGuid;
        uint32 scenarioId;

        std::vector<ObjectGuid> challengeDoorGuids;
        uint32 startChallengeTime;
        uint32 challengeTime;
        uint32 beginningTime;
        uint32 conditionId;
        uint32 dampeningSpellId;
        uint8 medalType;
        uint8 scenarioStep;
        uint8 conditionCounter;
        bool challengeStarted;
        bool challengeCompleted;
        bool conditionCompleted;
        bool hasDampening;
        time_t startFightTime;
};

TC_GAME_API bool InstanceHasScript(WorldObject const* obj, char const* scriptName);

template <class AI, class T>
AI* GetInstanceAI(T* obj, char const* scriptName)
{
    if (InstanceHasScript(obj, scriptName))
        return new AI(obj);

    return nullptr;
}

// template<class AI, class T>
// AI* GetInstanceAI(T* obj, char const* scriptName)
// {
//     if (InstanceMap* instance = obj->GetMap()->ToInstanceMap())
//         if (instance->GetInstanceScript())
//             if (instance->GetScriptId() == sObjectMgr->GetScriptId(scriptName))
//                 return new AI(obj);

//     return NULL;
// }

#endif // TRINITY_INSTANCE_DATA_H
