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

#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "GameObject.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "Pet.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "ScenarioMgr.h"
#include "GuildMgr.h"
#include "SpellHistory.h"
#include "Config.h"

InstanceScript::InstanceScript(Map* map) : instance(map), completedEncounters(0), resurrections(0)
{
    challengeStarted = false;
    challengeCompleted = false;
    conditionCompleted = false;
    startChallengeTime = 0;
    challengeTime = 0;
    scenarioStep = 0;
    conditionCounter = 0;
    determinationCount = 0;
    dampeningSpellId = 0;
    hasDampening = true; // by default

    switch (map->GetId())
    {
        case 1008:
            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.MogushanVaults", 1))
                dampeningSpellId = SPELL_DAMPENING_FIELD_R;
            break;
        case 1009:
            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.HeartOfFear", 1))
                dampeningSpellId = SPELL_KLAXXI_RESONANCE_R;
            break;
        case 996:
            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.TerraceSpring", 1))
                dampeningSpellId = SPELL_RITUAL_OF_PURIFICATION_R;
            break;
        case 1098:
            if(sConfigMgr->GetIntDefault("FirstOfTheKingdom.ThroneOfThunder", 1))
                dampeningSpellId = SPELL_SHADO_PAN_ONSLAUGHT_R;
            break;
    }

    dampenedGUIDs.clear();
    challengeDoorGuids.clear();
    flexCreatures.clear();

    medalType = ChallengeMedals::MedalTypeNone;

    scenarioId = sScenarioMgr->GetScenarioIdForMap(map->GetId());
    beginningTime = 0;

    scenarioGuid = ObjectGuid(HighGuid::Scenario, instance->GetInstanceId(), scenarioId);
}

void InstanceScript::SaveToDB()
{
    std::string data = GetSaveData();
    if (data.empty())
        return;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_INSTANCE_DATA);
    stmt->setUInt32(0, GetCompletedEncounterMask());
    stmt->setString(1, data);
    stmt->setUInt32(2, instance->GetInstanceId());
    CharacterDatabase.Execute(stmt);
}

void InstanceScript::HandleGameObject(ObjectGuid GUID, bool open, GameObject* go)
{
    if (!go)
        go = instance->GetGameObject(GUID);
    if (go)
        go->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: HandleGameObject failed");
}

bool InstanceScript::IsEncounterInProgress() const
{
    for (std::vector<BossInfo>::const_iterator itr = bosses.begin(); itr != bosses.end(); ++itr)
        if (itr->state == IN_PROGRESS)
            return true;

    return false;
}

void InstanceScript::GameObjectRemoved(GameObject* go)
{
    AddDoor(go, false);

    OnGameObjectRemove(go);
}

ObjectGuid InstanceScript::GetObjectGuid(uint32 type) const
{
    ObjectGuidMap::const_iterator i = _objectGuids.find(type);
    if (i != _objectGuids.end())
        return i->second;
    return ObjectGuid::Empty;
}

ObjectGuid InstanceScript::GetGuidData(uint32 type) const
{
    return GetObjectGuid(type);
}

void InstanceScript::SetBossNumber(uint32 number)
{
    bosses.resize(number);
    bossesScenarios.resize(number);
}

void InstanceScript::LoadMinionData(std::vector<MinionData> const& data)
{
    for (auto&& minion : data)
        if (minion.bossId < bosses.size())
            minions.insert(std::make_pair(minion.entry, MinionInfo(&bosses[minion.bossId])));

    TC_LOG_DEBUG("scripts", "InstanceScript::LoadMinionData: " UI64FMTD " minions loaded.", uint64(minions.size()));
}

void InstanceScript::LoadDoorData(std::vector<DoorData> const& data)
{
    for (auto&& door : data)
        if (door.bossId < bosses.size())
            doors.insert(std::make_pair(door.entry, DoorInfo(&bosses[door.bossId], door.type, BoundaryType(door.boundary))));

    TC_LOG_DEBUG("scripts", "InstanceScript::LoadDoorData: " UI64FMTD " doors loaded.", uint64(doors.size()));
}

void InstanceScript::LoadScenarioInfo(std::vector<ScenarioBosses> const& scenarios, uint32 condition)
{
    for (auto&& scenario : scenarios)
        if (scenario.bossId < bosses.size())
            bossesScenarios[scenario.bossId] = ScenarioBosses(scenario.bossId, scenario.scenarioId);
    conditionId = condition;
}

void InstanceScript::UpdateMinionState(Creature* minion, EncounterState state)
{
    switch (state)
    {
        case NOT_STARTED:
            if (!minion->IsAlive())
                minion->Respawn();
            else if (minion->IsInCombat())
                minion->AI()->EnterEvadeMode();
            break;
        case IN_PROGRESS:
            if (!minion->IsAlive())
                minion->Respawn();
            else if (!minion->GetVictim())
                minion->AI()->DoZoneInCombat();
            break;
        default:
            break;
    }
}

void InstanceScript::UpdateDoorState(GameObject* door)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    bool open = true;
    for (; range.first != range.second && open; ++range.first)
    {
        DoorInfo const& info = range.first->second;
        switch (info.type)
        {
            case DOOR_TYPE_ROOM:
                open = (info.bossInfo->state != IN_PROGRESS);
                break;
            case DOOR_TYPE_PASSAGE:
                open = (info.bossInfo->state == DONE);
                break;
            case DOOR_TYPE_SPAWN_HOLE:
                open = (info.bossInfo->state == IN_PROGRESS);
                break;
            default:
                break;
        }
    }

    door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

void InstanceScript::AddDoor(GameObject* door, bool add)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    for (; range.first != range.second; ++range.first)
    {
        DoorInfo const& data = range.first->second;

        if (add)
        {
            data.bossInfo->door[data.type].insert(door);
            switch (data.boundary)
            {
                default:
                case BOUNDARY_NONE:
                    break;
                case BOUNDARY_N:
                case BOUNDARY_S:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX();
                    break;
                case BOUNDARY_E:
                case BOUNDARY_W:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionY();
                    break;
                case BOUNDARY_NW:
                case BOUNDARY_SE:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() + door->GetPositionY();
                    break;
                case BOUNDARY_NE:
                case BOUNDARY_SW:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() - door->GetPositionY();
                    break;
            }
        }
        else
            data.bossInfo->door[data.type].erase(door);
    }

    if (add)
        UpdateDoorState(door);
}

void InstanceScript::AddMinion(Creature* minion, bool add)
{
    MinionInfoMap::iterator itr = minions.find(minion->GetEntry());
    if (itr == minions.end())
        return;

    if (add)
        itr->second.bossInfo->minion.insert(minion);
    else
        itr->second.bossInfo->minion.erase(minion);
}

bool InstanceScript::SetBossState(uint32 id, EncounterState state)
{
    if (id < bosses.size())
    {
        BossInfo* bossInfo = &bosses[id];
        ScenarioBosses* bossScenario = &bossesScenarios[id];

        if (bossInfo->state == TO_BE_DECIDED) // loading
        {
            bossInfo->state = state;
            //TC_LOG_ERROR("misc", "Inialize boss %u state as %u.", id, (uint32)state);
            return false;
        }
        else
        {
            if (bossInfo->state == state)
                return false;

            if (state == DONE)
            {
                for (MinionSet::iterator i = bossInfo->minion.begin(); i != bossInfo->minion.end(); ++i)
                    if ((*i)->isWorldBoss() && (*i)->IsAlive())
                        return false;

                if (instance->IsChallengeDungeon() || instance->IsScenario())
                {
                    SendScenarioProgressUpdate(CriteriaProgressData(bossScenario->scenarioId, 1, GetScenarioGUID(), time(NULL), GetBeginingTime(), 0));

                    // End of challenge
                    if (id == (bosses.size() - 1))
                    {
                        SendChallengeStopElapsedTimer(TIMER_CHALLENGE);

                        if (challengeStarted && conditionCompleted)
                        {
                            challengeStarted = false;
                            challengeCompleted = true;

                            SendChallengeNewPlayerRecord();

                            uint32 money = 0, valor = 0;
                            RewardChallengers(money, valor);
                            SendChallengeModeComplete(money, valor);

                            SaveChallengeDatasIfNeeded();

                            DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON, instance->GetId(), medalType);

                            if (auto ref = instance->GetPlayers().getFirst())
                            {
                                if (Group* group = instance->GetInstanceGroup())
                                {
                                    group->UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_CHALLENGE_DUNGEON, instance->GetId(), medalType, 0, nullptr, ref->GetSource());
                                    group->UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE_GUILD, 0, medalType, 0, nullptr, ref->GetSource());
                                }
                            }
                        }
                    }
                }

                if (instance->IsScenario())
                    SendScenarioState(ScenarioData(GetScenarioId(), scenarioStep++));
            }
            else if (state == IN_PROGRESS)
            {
                for (auto&& itr : instance->GetPlayers())
                {
                    if (Player* player = itr.GetSource())
                    {
                        player->GetSpellHistory()->OnEncounterStart();
                        if (instance->IsRaid()) // Idk about others
                            player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG2_ENCOUNTER_START);
                    }
                }
            }

            bossInfo->state = state;
            SaveToDB();
        }

        // Clear resurrections count
        ResetResurrectionsCount();

        for (uint32 type = 0; type < MAX_DOOR_TYPES; ++type)
            for (DoorSet::iterator i = bossInfo->door[type].begin(); i != bossInfo->door[type].end(); ++i)
                UpdateDoorState(*i);

        for (MinionSet::iterator i = bossInfo->minion.begin(); i != bossInfo->minion.end(); ++i)
            UpdateMinionState(*i, state);

        return true;
    }
    return false;
}

std::string InstanceScript::LoadBossState(const char * data)
{
    if (!data)
        return NULL;
    std::istringstream loadStream(data);
    uint32 buff;
    uint32 bossId = 0;
    for (std::vector<BossInfo>::iterator i = bosses.begin(); i != bosses.end(); ++i, ++bossId)
    {
        loadStream >> buff;
        if (buff < TO_BE_DECIDED)
            SetBossState(bossId, (EncounterState)buff);
    }
    return loadStream.str();
}

std::string InstanceScript::GetBossSaveData()
{
    std::ostringstream saveStream;
    for (std::vector<BossInfo>::iterator i = bosses.begin(); i != bosses.end(); ++i)
        saveStream << (uint32)i->state << ' ';
    return saveStream.str();
}

uint32 InstanceScript::GetEncountersDoneCount(uint32 encounters)
{
    if (!encounters)
        return 0;

    while (encounters > 0)
    {
        // we need get id of last encounter that was done
        if (GetBossState(encounters) == DONE)
            return encounters; 

        encounters--;
    }

    return 0;
}

void InstanceScript::DoUseDoorOrButton(ObjectGuid uiGuid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
{
    if (!uiGuid)
        return;

    GameObject* go = instance->GetGameObject(uiGuid);

    if (go)
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (go->getLootState() == GO_READY)
                go->UseDoorOrButton(uiWithRestoreTime, bUseAlternativeState);
            else if (go->getLootState() == GO_ACTIVATED)
                go->ResetDoorOrButton();
        }
        else
            TC_LOG_ERROR("misc", "SD2: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.", go->GetEntry(), go->GetGoType());
    }
}

void InstanceScript::DoRespawnGameObject(ObjectGuid uiGuid, uint32 uiTimeToDespawn)
{
    if (GameObject* go = instance->GetGameObject(uiGuid))
    {
        //not expect any of these should ever be handled
        if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE || go->GetGoType() == GAMEOBJECT_TYPE_DOOR ||
            go->GetGoType() == GAMEOBJECT_TYPE_BUTTON || go->GetGoType() == GAMEOBJECT_TYPE_TRAP)
            return;

        if (go->isSpawned())
            return;

        go->SetRespawnTime(uiTimeToDespawn);
    }
}

void InstanceScript::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            if (Player* player = itr->GetSource())
                player->SendUpdateWorldState(uiStateId, uiStateData);
    }
    else
        TC_LOG_DEBUG("scripts", "DoUpdateWorldState attempt send data but no players in map.");
}

// Send Notify to all players in instance
void InstanceScript::DoSendNotifyToInstance(char const* format, ...)
{
    InstanceMap::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        va_list ap;
        va_start(ap, format);
        char buff[1024];
        vsnprintf(buff, 1024, format, ap);
        va_end(ap);
        for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            if (Player* player = i->GetSource())
                if (WorldSession* session = player->GetSession())
                    session->SendNotification("%s", buff);
    }
}

// Reset Achievement Criteria for all players in instance
void InstanceScript::DoResetAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1 /*= 0*/, uint64 miscValue2 /*= 0*/, bool evenIfCriteriaComplete /*= false*/)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                player->ResetAchievementCriteria(type, miscValue1, miscValue2, evenIfCriteriaComplete);
}

// Complete Achievement for all players in instance
void InstanceScript::DoCompleteAchievement(uint32 achievement)
{
    AchievementEntry const* pAE = sAchievementStore.LookupEntry(achievement);
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!pAE)
    {
        TC_LOG_DEBUG("scripts", "DoCompleteAchievement called for not existing achievement %u", achievement);
        return;
    }

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* pPlayer = i->GetSource())
                pPlayer->CompletedAchievement(pAE);
}

// Update Achievement Criteria for all players in instance
void InstanceScript::DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1 /*= 0*/, uint32 miscValue2 /*= 0*/, Unit* unit /*= NULL*/)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->GetSource())
                player->UpdateAchievementCriteria(type, miscValue1, miscValue2, 0, unit);
}

// Start timed achievement for all players in instance
void InstanceScript::DoStartCriteria(CriteriaStartTypes type, uint32 entry)
{
    for (auto&& itr : instance->GetPlayers())
        if (Player* player = itr.GetSource())
            player->StartCriteria(type, entry);
}

// Stop timed achievement for all players in instance
void InstanceScript::DoStopTimedAchievement(CriteriaStartTypes type, uint32 entry)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->GetSource())
                player->RemoveTimedAchievement(type, entry);
}

// Remove Auras due to Spell on all players in instance
void InstanceScript::DoRemoveAurasDueToSpellOnPlayers(uint32 spell)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
            {
                player->RemoveAurasDueToSpell(spell);
                if (Pet* pet = player->GetPet())
                    pet->RemoveAurasDueToSpell(spell);
            }
        }
    }
}

void InstanceScript::DoRemoveBloodLustDebuffSpellOnPlayers()
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
            {
                for (auto sBloodlust : BloodLustType)
                {
                    player->RemoveAurasDueToSpell(sBloodlust);
                    if (Pet* pet = player->GetPet())
                        pet->RemoveAurasDueToSpell(sBloodlust);
                }

                // Remove high cooldowns
                player->GetSpellHistory()->RemoveCooldowns([](SpellHistory::CooldownEntry const& cooldownEntry)
                {
                    SpellInfo const* entry = sSpellMgr->GetSpellInfo(cooldownEntry.SpellId);
                    if (!entry)
                        return false;
                    if (entry->RecoveryTime < 5 * MINUTE * IN_MILLISECONDS && entry->CategoryRecoveryTime < 5 * MINUTE * IN_MILLISECONDS)
                        return false;
                    if (entry->CategoryEntry && entry->CategoryEntry->Flags & SPELL_CATEGORY_FLAG_COOLDOWN_EXPIRES_AT_MIDNIGHT)
                        return false;
                    if (entry->Id == 21169) // Reincarnation
                        return false;
                    return true;
                });
            }
        }
    }
}

// Add aura on all players in instance
void InstanceScript::DoAddAuraOnPlayers(uint32 spell)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* player = i->GetSource())
                player->AddAura(spell, player);
}

// Apply Dampening Spell for Creature
void InstanceScript::DoDampeningForCreatures(Creature* creature)
{
    if (!creature || !GetDampening() || !GetDampeningSpell())
        return;

    // Only normal/heroic mode
    if (instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_LFR)
    {
        SetDampeningSpell(0);
        return;
    }

    if (Pet* pet = creature->ToPet())
        if (pet->GetOwner())
            return;

    if (TempSummon* temp = creature->ToTempSummon())
        if (temp->GetSummonerGUID().IsPlayer())
            return;

    if (creature->HasAura(GetDampeningSpell()))
        return;

    creature->CastSpell(creature, GetDampeningSpell(), true);
    dampenedGUIDs.push_back(creature->GetGUID());
}

// Remove Specific Dampening Spell from affected creatures and prevent apply for future
void InstanceScript::DoRemoveDampeningFromCreatures()
{
    SetDampening(false);

    for (auto&& itr : dampenedGUIDs)
        if (Creature* creature = instance->GetCreature(itr))
            creature->RemoveAurasDueToSpell(GetDampeningSpell());

    // Not apply anymore in reset/evade
    SetDampeningSpell(0);
}

// Remove aura from stack on all players in instance
void InstanceScript::DoRemoveAuraFromStackOnPlayers(uint32 spell, uint64 casterGUID, AuraRemoveMode mode, uint32 num)
{
    Map::PlayerList const& plrList = instance->GetPlayers();
    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator itr = plrList.begin(); itr != plrList.end(); ++itr)
            if (Player* player = itr->GetSource())
                player->RemoveAuraFromStack(spell, casterGUID, mode, num);
}

// Cast spell on all players in instance - Nefertum: ignore gms, also this method should be able to distinguish between alive or not in some cases.
void InstanceScript::DoCastSpellOnPlayers(uint32 spell, bool aliveOnly /*= false*/)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (aliveOnly)
    {
        if (!PlayerList.isEmpty())
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (Player* player = i->GetSource())
                    if (!player->IsGameMaster() && player->IsAlive())
                        player->CastSpell(player, spell, true);
    }
    else
    {
        if (!PlayerList.isEmpty())
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (Player* player = i->GetSource())
                    if (!player->IsGameMaster())
                        player->CastSpell(player, spell, true);
    }
}

void InstanceScript::DoSetAlternatePowerOnPlayers(int32 value)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                player->SetPower(POWER_ALTERNATE_POWER, value);
}

void InstanceScript::DoModifyPlayerCurrencies(uint32 id, int32 value)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                player->ModifyCurrency(id, value);
}

void InstanceScript::DoNearTeleportPlayers(const Position pos, bool casting /*=false*/)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), casting);
}

void InstanceScript::DoStartMovie(uint32 movieId)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                player->SendMovieStart(movieId);
}

void InstanceScript::DoKilledMonsterKredit(uint32 questId, uint32 entry, ObjectGuid guid/* =0*/)
{
    Map::PlayerList const &plrList = instance->GetPlayers();
    
    if (!plrList.isEmpty())
        for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
            if (Player* player = i->GetSource())
                if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(entry, guid);
}

void InstanceScript::DoFinishLFGDungeon(uint32 dungeonId)
{
    Map::PlayerList const &plrList = instance->GetPlayers();

    if (!plrList.isEmpty())
        if (plrList.begin()->GetSource()->GetGroup())
            sLFGMgr->FinishDungeon(plrList.begin()->GetSource()->GetGroup()->GetGUID(), dungeonId, instance);
}

bool InstanceScript::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= NULL*/, uint32 /*miscvalue1*/ /*= 0*/)
{
    TC_LOG_ERROR("misc", "Achievement system call InstanceScript::CheckAchievementCriteriaMeet but instance script for map %u not have implementation for achievement criteria %u",
        instance->GetId(), criteria_id);
    return false;
}

void InstanceScript::SendEncounterUnit(uint32 type, Unit* unit /*= NULL*/, uint8 param1 /*= 0*/, uint8 param2 /*= 0*/)
{
    // size of this packet is at most 15 (usually less)
    WorldPacket data(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, 15);
    data << uint32(type);

    switch (type)
    {
        case ENCOUNTER_FRAME_ENGAGE:
        case ENCOUNTER_FRAME_DISENGAGE:
        case ENCOUNTER_FRAME_UPDATE_PRIORITY:
            if (!unit)
                return;
            data << unit->GetPackGUID();
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_ADD_TIMER:
        case ENCOUNTER_FRAME_ENABLE_OBJECTIVE:
        case ENCOUNTER_FRAME_DISABLE_OBJECTIVE:
        case ENCOUNTER_FRAME_SET_COMBAT_RES_LIMIT:
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_UPDATE_OBJECTIVE:
            data << uint8(param1);
            data << uint8(param2);
            break;
        case ENCOUNTER_FRAME_UNK7:
        case ENCOUNTER_FRAME_ADD_COMBAT_RES_LIMIT:
        case ENCOUNTER_FRAME_RESET_COMBAT_RES_LIMIT:
        default:
            break;
    }

    instance->SendToPlayers(&data);
}

bool InstanceScript::IsWipe(float /*range*/, Unit* /*source*/)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();

    if (PlayerList.isEmpty())
        return true;

    for (const auto & Itr : PlayerList)
    {
        Player* player = Itr.GetSource();

        if (!player)
            continue;

        if (player->IsAlive() && !player->IsGameMaster())
            return false;
    }

    return true;
}

void InstanceScript::UpdateEncounterState(EncounterCreditType type, uint32 creditEntry, Unit* /*source*/)
{
    DungeonEncounterList const* encounters = sObjectMgr->GetDungeonEncounterList(instance->GetId(), instance->GetDifficulty());
    if (!encounters)
        return;

    uint32 dungeonId = 0;

    for (auto encounter : *encounters)
    {
        if (encounter->creditType == type && encounter->creditEntry == creditEntry)
        {
            completedEncounters |= 1 << encounter->dbcEntry->encounterIndex;
            if (encounter->lastEncounterDungeon)
            {
                dungeonId = encounter->lastEncounterDungeon;
                TC_LOG_DEBUG("lfg", "UpdateEncounterState: Instance %s (instanceId %u) completed encounter %s. Credit Dungeon: %u", instance->GetMapName(), instance->GetInstanceId(), encounter->dbcEntry->encounterName[DEFAULT_LOCALE], dungeonId);
                break;
            }
        }
    }


    // Raid challenge is be rewarded for any raid boss, unlike dungeon/scenario
    if (dungeonId || instance->IsRaid())
    {
        Player* player = instance->GetFirstPlayerInInstance();
        if (Group* group = player->GetGroup())
        {
            if (dungeonId && group->IsFlex())
                dungeonId = sLFGMgr->GetDungeon(group->GetGUID());

            if (dungeonId && group->isLFGGroup())
                sLFGMgr->FinishDungeon(group->GetGUID(), dungeonId, instance);

            uint32 level = 0;
            uint32 count = 0;
            for (auto&& member : *group)
            {
                if (member->FindMap() == instance)
                {
                    level += member->GetLevel();
                    ++count;
                }
            }

            level /= count;

            bool rewardChallenge = true;

            if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(dungeonId))
                if ((dungeonId || instance->IsRaid()) && dungeon->maxlevel && level > dungeon->maxlevel)
                    rewardChallenge = false;

            Player* participant;
            if (Guild* guild = group->GetGroupGuild(player, &participant))
            {
                if (rewardChallenge)
                {
                    GuildChallengeType challenge = instance->IsRaid() ? CHALLENGE_RAID : CHALLENGE_DUNGEON;
                    if (instance->GetDifficulty() == DUNGEON_DIFFICULTY_CHALLENGE)
                        challenge = CHALLENGE_DUNGEON_CHALLENGE;
                    guild->CompleteGuildChallenge(challenge, participant);
                }
            }
        }
    }
}

void InstanceScript::UpdatePhasing()
{
    PhaseUpdateData phaseUdateData;
    phaseUdateData.AddConditionType(CONDITION_INSTANCE_INFO);

    Map::PlayerList const& players = instance->GetPlayers();
    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        if (Player* player = itr->GetSource())
        {
            player->GetPhaseMgr().NotifyConditionChanged(phaseUdateData);
            player->UpdatePhasing();
        }
}

void InstanceScript::SendScenarioState(ScenarioData scenarioData, Player* player /*= nullptr*/)
{
    WorldPacket data(SMSG_SCENARIO_STATE);

    data << int32(0);
    data << int32(0);
    data << int32(0);
    data << int32(scenarioData.ScenarioId);
    data << int32(0);
    data << int32(scenarioData.BonusObjectiveDataCount);
    data << int32(scenarioData.StepId);

    data.WriteBits(scenarioData.CriteriaCount, 19);
    data.WriteBit(false);

    ByteBuffer byteBuffer;
    for (CriteriaProgressData progressData : scenarioData.criteriaProgress)
    {
        ObjectGuid quantity(progressData.Quantity); // sad, but true...
        ObjectGuid instanceGuid = progressData.InstanceGuid;

        data.WriteBit(instanceGuid[3]);
        data.WriteBit(instanceGuid[5]);
        data.WriteBit(quantity[2]);
        data.WriteBit(instanceGuid[6]);
        data.WriteBit(quantity[7]);
        data.WriteBit(instanceGuid[1]);
        data.WriteBit(instanceGuid[7]);
        data.WriteBit(instanceGuid[0]);
        data.WriteBit(quantity[3]);
        data.WriteBit(instanceGuid[4]);
        data.WriteBit(quantity[0]);
        data.WriteBit(quantity[5]);
        data.WriteBit(quantity[1]);
        data.WriteBit(quantity[4]);
        data.WriteBit(instanceGuid[2]);
        data.WriteBit(quantity[6]);
        data.WriteBits(progressData.Flags, 4);

        byteBuffer.WriteByteSeq(quantity[3]);
        byteBuffer.WriteByteSeq(quantity[7]);
        byteBuffer.WriteByteSeq(quantity[5]);
        byteBuffer << uint32(progressData.TimeFromStart);
        byteBuffer.WriteByteSeq(quantity[2]);
        byteBuffer << uint32(progressData.TimeFromCreate);
        byteBuffer.WriteByteSeq(quantity[6]);
        byteBuffer.WriteByteSeq(quantity[4]);
        byteBuffer.AppendPackedTime(progressData.Date);
        byteBuffer.WriteByteSeq(quantity[1]);
        byteBuffer.WriteByteSeq(instanceGuid[3]);
        byteBuffer.WriteByteSeq(instanceGuid[2]);
        byteBuffer.WriteByteSeq(instanceGuid[1]);
        byteBuffer.WriteByteSeq(instanceGuid[5]);
        byteBuffer.WriteByteSeq(instanceGuid[4]);
        byteBuffer.WriteByteSeq(instanceGuid[7]);
        byteBuffer.WriteByteSeq(quantity[0]);
        byteBuffer.WriteByteSeq(instanceGuid[6]);
        byteBuffer.WriteByteSeq(instanceGuid[0]);
        byteBuffer << uint32(progressData.Id);
    }

    data.WriteBit(false);

    data.FlushBits();

    if (byteBuffer.size())
        data.append(byteBuffer);

    if (!player)
        instance->SendToPlayers(&data);
    else
        player->SendDirectMessage(&data);
}

void InstanceScript::SendScenarioProgressUpdate(CriteriaProgressData progressData, Player* player /*= nullptr*/)
{
    ObjectGuid quantity(progressData.Quantity); // sad, but true...
    ObjectGuid instanceGuid = progressData.InstanceGuid;
    WorldPacket data(SMSG_SCENARIO_PROGRESS_UPDATE);

    data.WriteBit(instanceGuid[6]);
    data.WriteBit(instanceGuid[4]);
    data.WriteBit(instanceGuid[5]);
    data.WriteBit(quantity[1]);
    data.WriteBit(quantity[5]);
    data.WriteBit(quantity[6]);
    data.WriteBit(quantity[7]);
    data.WriteBit(quantity[4]);
    data.WriteBit(instanceGuid[1]);
    data.WriteBit(quantity[0]);
    data.WriteBit(instanceGuid[2]);
    data.WriteBit(instanceGuid[3]);
    data.WriteBit(quantity[2]);
    data.WriteBit(quantity[3]);
    data.WriteBit(instanceGuid[0]);
    data.WriteBits(progressData.Flags, 4);
    data.WriteBit(instanceGuid[7]);

    data.WriteByteSeq(quantity[5]);
    data.WriteByteSeq(quantity[2]);
    data.WriteByteSeq(instanceGuid[7]);
    data.WriteByteSeq(instanceGuid[4]);
    data.WriteByteSeq(quantity[4]);
    data.WriteByteSeq(quantity[0]);
    data.WriteByteSeq(instanceGuid[1]);
    data.WriteByteSeq(instanceGuid[2]);
    data << uint32(progressData.TimeFromStart);
    data.WriteByteSeq(quantity[3]);
    data << uint32(progressData.Id);
    data.WriteByteSeq(quantity[1]);
    data.AppendPackedTime(progressData.Date);
    data << uint32(progressData.TimeFromCreate);
    data.WriteByteSeq(instanceGuid[0]);
    data.WriteByteSeq(instanceGuid[3]);
    data.WriteByteSeq(instanceGuid[6]);
    data.WriteByteSeq(quantity[7]);
    data.WriteByteSeq(quantity[6]);
    data.WriteByteSeq(instanceGuid[5]);

    if (!player)
        instance->SendToPlayers(&data);
    else
        player->SendDirectMessage(&data);
}

void InstanceScript::SendChallengeStartTimer(uint32 time)
{
    for (auto&& itr : instance->GetPlayers())
        if (Player* player = itr.GetSource())
            player->SendStartTimer(time, time, TIMER_CHALLENGE);
}

void InstanceScript::SendChallengeStartElapsedTimer(uint32 timerId, uint32 time)
{
    WorldPacket data(SMSG_START_ELAPSED_TIMER, 8);
    data << uint32(time);
    data << uint32(timerId);
    instance->SendToPlayers(&data);
}

void InstanceScript::SendChallengeStopElapsedTimer(uint32 timerID, bool keepTimer /*= false*/)
{
    WorldPacket data(SMSG_STOP_ELAPSED_TIMER, 5);
    data.WriteBit(keepTimer);
    data.FlushBits();
    data << uint32(timerID);
    instance->SendToPlayers(&data);
}

void InstanceScript::SetChallengeModeStarted()
{
    if (challengeStarted || !instance->IsChallengeDungeon())
        return;

    challengeStarted = true;

    // Challenge mode start timer is 5 seconds
    SendChallengeStartTimer(5);
    startChallengeTime = 5000;
}

void InstanceScript::ScheduleChallengeStartup(uint32 diff)
{
    if (startChallengeTime)
    {
        if (startChallengeTime <= diff)
        {
            startChallengeTime = 0;

            for (auto&& doorGuid : challengeDoorGuids)
                if (GameObject* challengeDoor = instance->GetGameObject(doorGuid))
                    challengeDoor->SetGoState(GO_STATE_ACTIVE);

            SendChallengeStartElapsedTimer(TIMER_CHALLENGE, 0);

            medalType = ChallengeMedals::MedalTypeGold;
        }
        else
            startChallengeTime -= diff;
    }
}

void InstanceScript::ScheduleChallengeTimeUpdate(uint32 diff)
{
    if (!challengeStarted || startChallengeTime)
        return;

    challengeTime += diff;

    MapChallengeModeEntry const* challengeEntry = nullptr;
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
    {
        if (MapChallengeModeEntry const* challenge = sMapChallengeModeStore.LookupEntry(i))
        {
            if (challenge->MapID == instance->GetId())
            {
                challengeEntry = challenge;
                break;
            }
        }
    }

    if (!challengeEntry) // Should not happens
        return;

    uint32 times[ChallengeMedals::MedalTypeGold];
    times[ChallengeMedals::MedalTypeBronze - 1] = challengeEntry->BronzeTime * TimeConstants::IN_MILLISECONDS;
    times[ChallengeMedals::MedalTypeSilver - 1] = challengeEntry->SilverTime * TimeConstants::IN_MILLISECONDS;
    times[ChallengeMedals::MedalTypeGold - 1] = challengeEntry->GoldTime * TimeConstants::IN_MILLISECONDS;

    // Downgrade Medal if needed
    if (medalType > ChallengeMedals::MedalTypeNone && challengeTime > times[medalType - 1])
        medalType--;
}

void InstanceScript::ScheduleBeginningTimeUpdate(uint32 diff)
{
    beginningTime += diff;
}

void InstanceScript::SendChallengeNewPlayerRecord()
{
    uint32 mapId = instance->GetId();

    for (auto&& itr : instance->GetPlayers())
    {
        if (Player* player = itr.GetSource())
        {
            if (player->HasChallengeCompleted(mapId))
            {
                CompletedChallenge* challenge = player->GetCompletedChallenge(mapId);
                if (!challenge) // Should not happens
                    continue;

                bool newBestTime = challengeTime < challenge->BestTime;
                bool newBestMedal = medalType > challenge->BestMedal;

                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_COMPLETED_CHALLENGE);
                stmt->setUInt32(0, newBestTime ? challengeTime : challenge->BestTime);
                stmt->setUInt32(1, challengeTime);
                stmt->setUInt8(2, newBestMedal ? medalType : challenge->BestMedal);
                stmt->setUInt32(3, newBestMedal ? time(NULL) : challenge->BestMedalDate);
                stmt->setUInt32(4, player->GetGUID().GetCounter());
                stmt->setUInt32(5, mapId);
                CharacterDatabase.Execute(stmt);

                if (newBestMedal)
                {
                    challenge->BestMedal = medalType;
                    challenge->BestMedalDate = time(nullptr);
                }

                if (newBestTime)
                    challenge->BestTime = challengeTime;

                challenge->LastTime = challengeTime;

                // Send new record only for new best time
                if (!newBestTime)
                    continue;
            }
            else
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_COMPLETED_CHALLENGE);
                stmt->setUInt32(0, player->GetGUID().GetCounter());
                stmt->setUInt32(1, mapId);
                stmt->setUInt32(2, challengeTime);
                stmt->setUInt32(3, challengeTime);
                stmt->setUInt8(4, medalType);
                stmt->setUInt32(5, time(nullptr));
                CharacterDatabase.Execute(stmt);

                CompletedChallenge challenge;
                challenge.BestMedal = medalType;
                challenge.BestMedalDate = time(nullptr);
                challenge.BestTime = challengeTime;
                challenge.LastTime = challengeTime;

                player->AddCompletedChallenge(mapId, challenge);
                player->GetSession()->SendChallengeModeMapStatsUpdate(mapId);
            }

            WorldPacket data(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD, 12);
            data << uint32(mapId);
            data << uint32(medalType);
            data << uint32(challengeTime);
            player->SendDirectMessage(&data);
        }
    }
}

void InstanceScript::SendChallengeModeComplete(uint32 money, uint32 valor)
{
    uint32 itemRewards = 0;
    uint32 currencyRewards = valor != 0;

    WorldPacket data(SMSG_CHALLENGE_MODE_COMPLETE, 4 * 4 + (itemRewards * 12) + (currencyRewards * 8));

    data << uint32(money);
    data << uint32(instance->GetId());
    data << uint32(medalType);
    data << uint32(challengeTime);

    data.WriteBits(currencyRewards, 21);
    data.WriteBits(itemRewards, 20);

    for (uint32 i = 0; i < currencyRewards; i++)
    {
        data << uint32(CURRENCY_TYPE_VALOR_POINTS);
        data << uint32(valor);
    }

    for (uint32 i = 0; i < itemRewards; i++)
    {
        data << int32(0); // ItemEntry
        data << int32(0); // RandomPropertiesSeed
        data << int32(0); // RandomPropertiesID
    }

    instance->SendToPlayers(&data);

}

void InstanceScript::SaveChallengeDatasIfNeeded()
{
    uint32 mapId = instance->GetId();
    RealmCompletedChallenge* groupChallenge = sObjectMgr->GetGroupCompletedChallengeForMap(mapId);
    RealmCompletedChallenge* guildChallenge = sObjectMgr->GetGuildCompletedChallengeForMap(mapId);

    SaveNewGroupChallenge();

    // Delete old group record if it's a new realm-best time (or if it's the first), and reward titles/achievements
    if (!groupChallenge || (groupChallenge && groupChallenge->CompletionTime > challengeTime))
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_CHALLENGE);
        stmt->setUInt32(0, mapId);
        CharacterDatabase.Execute(stmt);

        RewardNewRealmRecord(groupChallenge);
    }

    bool isGuildGroup = false;
    uint32 guildId = 0;
    for (auto&& itr : instance->GetPlayers())
    {
        if (Player* player = itr.GetSource())
        {
            if (Group* group = player->GetGroup())
            {
                if (!group->IsGuildGroup(player->GetGuildId(), player))
                    continue;

                isGuildGroup = true;
                guildId = player->GetGuildId();
            }
        }
    }

    // New best time for the guild
    if (isGuildGroup)
    {
        SaveNewGroupChallenge(guildId);

        // Delete old guild record if it's a new realm-best time
        if (guildChallenge && guildChallenge->CompletionTime > challengeTime)
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_CHALLENGE);
            stmt->setUInt32(0, mapId);
            stmt->setUInt32(1, guildId);
            CharacterDatabase.Execute(stmt);
        }
    }
}

void InstanceScript::SaveNewGroupChallenge(uint32 guildId /*= 0*/)
{
    uint32 index = 0;
    uint32 mapId = instance->GetId();
    uint32 date  = time(nullptr);

    RealmCompletedChallenge newGroup;
    newGroup.AttemptId      = 0;
    newGroup.CompletionDate = date;
    newGroup.CompletionTime = challengeTime;
    newGroup.GuildId        = guildId;
    newGroup.MedalEarned    = medalType;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(guildId ? CHAR_INS_GUILD_CHALLENGE : CHAR_INS_GROUP_CHALLENGE);

    stmt->setUInt32(index++, instance->GetId());

    if (guildId)
        stmt->setUInt32(index++, guildId);

    stmt->setUInt32(index++, 0); // AttemptId
    stmt->setUInt32(index++, challengeTime);
    stmt->setUInt32(index++, date);
    stmt->setUInt8(index++, medalType);

    uint32 count = 0;
    for (auto&& itr : instance->GetPlayers())
        if (itr.GetSource())
            count++;

    stmt->setUInt8(index++, count);
    newGroup.MembersCount = count;

    uint8 j = 0;
    if (j < 5)
    {
        for (auto&& itr : instance->GetPlayers())
        {
            if (Player* player = itr.GetSource())
            {
                newGroup.Members[j].Guid = ObjectGuid(HighGuid::Player, player->GetGUID().GetCounter());
                newGroup.Members[j].SpecId = player->GetTalentSpecialization(player->GetActiveSpec());
                j++;

                stmt->setUInt32(index++, player->GetGUID().GetCounter());
                stmt->setUInt32(index++, player->GetTalentSpecialization(player->GetActiveSpec()));
            }
        }
    }

    if (count < 5)
    {
        for (uint8 i = 0; i < (5 - count); i++)
        {
            newGroup.Members[j].Guid.Clear();
            newGroup.Members[j].SpecId = 0;
            j++;

            stmt->setUInt32(index++, 0);
            stmt->setUInt32(index++, 0);
        }
    }

    CharacterDatabase.Execute(stmt);

    sObjectMgr->AddGroupCompletedChallenge(mapId, newGroup);

    if (guildId)
        sObjectMgr->AddGuildCompletedChallenge(mapId, newGroup);
}

void InstanceScript::RewardChallengers(uint32& money, uint32& valor)
{
    ChallengeReward* reward = sObjectMgr->GetChallengeRewardsForMap(instance->GetId());
    if (!reward)
        return;

    if (medalType < ChallengeMedals::MaxMedalType)
    {
        money = reward->MoneyReward[medalType];
        valor = reward->ValorReward[medalType];

        for (auto&& itr : instance->GetPlayers())
        {
            if (Player* player = itr.GetSource())
            {
                if (money)
                    player->ModifyMoney(int64(money));

                if (valor)
                    player->ModifyCurrency(CURRENCY_TYPE_VALOR_POINTS, valor, true);
            }
        }
    }
}

void InstanceScript::RewardNewRealmRecord(RealmCompletedChallenge* oldChallenge /*= nullptr*/)
{
    ChallengeReward* reward = sObjectMgr->GetChallengeRewardsForMap(instance->GetId());
    if (!reward)
        return;

    CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward->TitleID);
    if (!title)
        return;

    AchievementEntry const* achievement = sAchievementStore.LookupEntry(reward->AchievementID);
    if (!achievement)
        return;

    // Remove title to previous challengers - Achievement will stay
    if (oldChallenge)
    {
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        for (uint8 i = 0; i < 5; i++)
        {
            // Check if player is online, then remove the title
            if (Player* player = HashMapHolder<Player>::Find(oldChallenge->Members[i].Guid))
                player->SetTitle(title, true);
            else
            {
                uint32 index = title->bit_index / 32;
                uint32 flag = 1 << (title->bit_index % 32);
                uint32 lowGuid = oldChallenge->Members[i].Guid.GetCounter();

                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_AT_LOGIN_TITLES);
                stmt->setUInt32(0, lowGuid);

                if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
                {
                    Field* fields = result->Fetch();
                    char const* knownTitlesStr = fields[1].GetCString();

                    // Title removal
                    if (knownTitlesStr)
                    {
                        uint32 const titleSize = KNOWN_TITLES_SIZE * 2;
                        uint32 knownTitles[titleSize];
                        Tokenizer tokens(knownTitlesStr, ' ', titleSize);

                        uint32 actualSize = tokens.size();

                        for (uint32 j = 0; j < titleSize; j++)
                        {
                            if (j < actualSize)
                                knownTitles[j] = atol(tokens[j]);
                            else
                                knownTitles[j] = 0;
                        }

                        knownTitles[index] &= ~flag;

                        std::ostringstream stream;
                        for (uint32 j = 0; j < titleSize; j++)
                            stream << knownTitles[j] << ' ';

                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_TITLES_FACTION_CHANGE);
                        stmt->setString(0, stream.str().c_str());
                        stmt->setUInt32(1, lowGuid);
                        trans->Append(stmt);

                        // Unset any currently chosen title
                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_RES_CHAR_TITLES_FACTION_CHANGE);
                        stmt->setUInt32(0, lowGuid);
                        trans->Append(stmt);
                    }
                }
            }
        }

        CharacterDatabase.CommitTransaction(trans);
    }

    for (auto&& itr : instance->GetPlayers())
    {
        if (Player* player = itr.GetSource())
        {
            player->SetTitle(title);
            player->CompletedAchievement(achievement);
        }
    }
}

void InstanceScript::SendChallengeInfo(Player* player, uint32 scenarioId)
{
    if (!GetScenarioId())
        SetScenarioId(scenarioId);

    if (instance->IsChallengeDungeon())
    {
        if (!IsChallengeModeCompleted())
        {
            SendScenarioState(ScenarioData(GetScenarioId(), GetcurrentScenarioStep()), player);

            if (IsChallengeModeStarted())
            {
                for (auto&& bossScenario : bossesScenarios)
                {
                    if (bossScenario.bossId < bosses.size())
                        if (GetBossState(bossScenario.bossId) == DONE)
                            SendScenarioProgressUpdate(CriteriaProgressData(bossScenario.scenarioId, 1, GetScenarioGUID(), time(NULL), GetBeginingTime(), 0));
                }
                SendScenarioProgressUpdate(CriteriaProgressData(conditionId, conditionCounter, GetScenarioGUID(), time(NULL), GetBeginingTime(), 0));
            }
        }

        if (IsChallengeModeStarted() && !IsChallengeModeCompleted())
            SendChallengeStartElapsedTimer(TIMER_CHALLENGE, uint32(GetChallengeTime() / 100));
    }
}

void InstanceScript::UpdateConditionInfo(Creature* creature, uint32 counter)
{
    if (instance->IsChallengeDungeon() && IsChallengeModeStarted() && conditionCounter < counter)
    {
        if (IsConditionCompleted()) // Enemies already killed
            return;

        if (!creature->isElite() || creature->IsDungeonBoss())
            return;

        conditionCounter++;
        SendScenarioProgressUpdate(CriteriaProgressData(conditionId, conditionCounter, GetScenarioGUID(), time(NULL), GetBeginingTime(), 0));

        if (conditionCounter >= counter)
            SetConditionCompleted(true);
    }
}

bool InstanceScript::CanUseResurrection()
{
    if (!instance)
        return true;

    if (!IsEncounterInProgress())
        return true;

    uint8 max_count = (instance->Is25ManRaid() ? 3 : 1);

    return (resurrections < max_count);
}

void InstanceScript::PromotionIfLFR(uint32 data)
{
    if (!instance->ToInstanceMap()->IsLFGMap())
        return;

    if (instance->GetDifficulty() == RAID_DIFFICULTY_1025MAN_FLEX)
        return;

    if (data == IN_PROGRESS)
        startFightTime = time(nullptr);
    if (data == DONE)
    {
        DoRemoveAurasDueToSpellOnPlayers(SPELL_DETERMINATION_LFR);

        determinationCount = 0;
        startFightTime = 0;
    }
    else if (data == FAIL)
    {
        if (startFightTime + 3 * MINUTE > time(nullptr))
            return;

        DoCastSpellOnPlayers(SPELL_DETERMINATION_LFR);

        if (determinationCount < 10)
            determinationCount++;

        startFightTime = 0;
    }
}

void InstanceScript::UpdateDynamicHealth(ObjectGuid single)
{
    if (instance->GetDifficulty() != RAID_DIFFICULTY_1025MAN_FLEX)
        return;

    // update players count on minimap
    for (auto&& itr : instance->GetPlayers())
        itr.GetSource()->GetSession()->SendServerWorldInfo();

    uint32 count = instance->GetPlayersCountExceptGMs();
    if (count > 25)
        count = 25;
    if (count < 10)
        count = 10;

    std::function<void(ObjectGuid)> updateHealth = [this, count](ObjectGuid guid)
    {
        if (Creature* creature = instance->GetCreature(guid))
            if (creature->IsHostileToPlayers())
            {
                auto info = sObjectMgr->GetCreatureScalingData(creature->GetEntry(), count);
                if (!info)
                    return;

                // Not in boss fight of course (btw should be delay in 1 minute for recalculate after player leave group/disconnected in this case)
                if (!IsEncounterInProgress())
                    creature->RecalculateDynamicHealth(info->Health);
            }
    };

    if (single)
        updateHealth(single);
    else
    {
        for (auto&& guid : flexCreatures)
            updateHealth(guid);
    }
}

bool InstanceHasScript(WorldObject const* obj, char const* scriptName)
{
    if (InstanceMap* instance = obj->GetMap()->ToInstanceMap())
        return instance->GetScriptName() == scriptName;

    return false;
}