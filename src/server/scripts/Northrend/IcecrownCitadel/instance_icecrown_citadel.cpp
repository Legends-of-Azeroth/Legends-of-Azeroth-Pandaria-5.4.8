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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "Map.h"
#include "Transport.h"
#include "Group.h"
#include "icecrown_citadel.h"

enum EventIds
{
    EVENT_PLAYERS_GUNSHIP_SPAWN     = 22663,
    EVENT_PLAYERS_GUNSHIP_COMBAT    = 22664,
    EVENT_PLAYERS_GUNSHIP_SAURFANG  = 22665,
    EVENT_ENEMY_GUNSHIP_COMBAT      = 22860,
    EVENT_ENEMY_GUNSHIP_DESPAWN     = 22861,
    EVENT_LICH_KING_WHISPER         = 23402,
    EVENT_OOZE_RELEASE_VALVE        = 23426,
    EVENT_GAS_RELEASE_VALVE         = 23438,
    EVENT_QUAKE                     = 23437,
    EVENT_SECOND_REMORSELESS_WINTER = 23507,
    EVENT_TELEPORT_TO_FROSMOURNE    = 23617,
};

enum TimedEvents
{
    EVENT_UPDATE_EXECUTION_TIME = 1,
    EVENT_QUAKE_SHATTER         = 2,
    EVENT_REBUILD_PLATFORM      = 3,
    EVENT_RESPAWN_GUNSHIP       = 4,
    EVENT_TOGGLE_ELEVATOR,
};

static std::vector<DoorData> doorData =
{
    { GO_LORD_MARROWGAR_S_ENTRANCE,           DATA_LORD_MARROWGAR,        DOOR_TYPE_ROOM,    BOUNDARY_N    },
    { GO_ICEWALL,                             DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_DOODAD_ICECROWN_ICEWALL02,           DATA_LORD_MARROWGAR,        DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_ORATORY_OF_THE_DAMNED_ENTRANCE,      DATA_LADY_DEATHWHISPER,     DOOR_TYPE_ROOM,    BOUNDARY_N    },
    { GO_SAURFANG_S_DOOR,                     DATA_DEATHBRINGER_SAURFANG, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    // { GO_BLOODWING_DOOR,                      DATA_PROFESSOR_PUTRICIDE,   DOOR_TYPE_ROOM/*DOOR_TYPE_PASSAGE*/, DOOR_TYPE_ROOM   /*BOUNDARY_NONE*/ },
    // { GO_FROSTWING_DOOR,                      DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_ROOM/*DOOR_TYPE_PASSAGE*/, DOOR_TYPE_ROOM    /*BOUNDARY_NONE*/ },
    { GO_ORANGE_PLAGUE_MONSTER_ENTRANCE,      DATA_FESTERGUT,             DOOR_TYPE_ROOM,    BOUNDARY_E    },
    { GO_GREEN_PLAGUE_MONSTER_ENTRANCE,       DATA_ROTFACE,               DOOR_TYPE_ROOM,    BOUNDARY_E    },
    { GO_SCIENTIST_ENTRANCE,                  DATA_PROFESSOR_PUTRICIDE,   DOOR_TYPE_ROOM,    BOUNDARY_E    },
    { GO_CRIMSON_HALL_DOOR,                   DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_ROOM,    BOUNDARY_S    },
    { GO_BLOOD_ELF_COUNCIL_DOOR,              DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_PASSAGE, BOUNDARY_W    },
    { GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT,        DATA_BLOOD_PRINCE_COUNCIL,  DOOR_TYPE_PASSAGE, BOUNDARY_E    },
    { GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01, DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_ROOM,    BOUNDARY_S    },
    { GO_DOODAD_ICECROWN_GRATE_01,            DATA_BLOOD_QUEEN_LANA_THEL, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_GREEN_DRAGON_BOSS_ENTRANCE,          DATA_SISTER_SVALNA,         DOOR_TYPE_PASSAGE, BOUNDARY_S    },
    { GO_GREEN_DRAGON_BOSS_ENTRANCE,          DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_ROOM,    BOUNDARY_N    },
    { GO_GREEN_DRAGON_BOSS_EXIT,              DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_PASSAGE, BOUNDARY_S    },
    { GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_N    },
    { GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_S    },
    { GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_N    },
    { GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04,  DATA_VALITHRIA_DREAMWALKER, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_S    },
    // { GO_SINDRAGOSA_ENTRANCE_DOOR,            DATA_SINDRAGOSA_GAUNTLET,   DOOR_TYPE_PASSAGE,    BOUNDARY_N    },
    // { GO_SINDRAGOSA_ENTRANCE_DOOR,            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM,    BOUNDARY_S    },
    { GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR,   DATA_SINDRAGOSA,            DOOR_TYPE_PASSAGE, BOUNDARY_E    },
    { GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR,       DATA_SINDRAGOSA,            DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_ICE_WALL,                            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM,    BOUNDARY_SE   },
    { GO_ICE_WALL,                            DATA_SINDRAGOSA,            DOOR_TYPE_ROOM,    BOUNDARY_SW   },
    { 0,                                      0,                          DOOR_TYPE_ROOM,    BOUNDARY_NONE }, // END
};

std::map<uint32, std::pair<uint32, uint32>> const ICCBuffData =
{
    { 5,  std::make_pair(BUFF_5A,  BUFF_5H)  },
    { 10, std::make_pair(BUFF_10A, BUFF_10H) },
    { 15, std::make_pair(BUFF_15A, BUFF_15H) },
    { 20, std::make_pair(BUFF_20A, BUFF_20H) },
    { 25, std::make_pair(BUFF_25A, BUFF_25H) },
    { 30, std::make_pair(BUFF_30A, BUFF_30H) },
};

class instance_icecrown_citadel : public InstanceMapScript
{
    public:
        instance_icecrown_citadel() : InstanceMapScript(ICCScriptName, 631) { }

        struct instance_icecrown_citadel_InstanceMapScript : public InstanceScript
        {
            instance_icecrown_citadel_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                TeamInInstance = 0;
                HeroicAttempts = MaxHeroicAttempts;
                LadyDeathwisperElevatorGUID = ObjectGuid::Empty;
                DeathbringerSaurfangGUID = ObjectGuid::Empty;
                DeathbringerSaurfangDoorGUID = ObjectGuid::Empty;
                DeathbringerSaurfangEventGUID = ObjectGuid::Empty;
                DeathbringersCacheGUID = ObjectGuid::Empty;
                BloodwingDoor                   = ObjectGuid::Empty;
                FrostwingDoor                   = ObjectGuid::Empty;
                PlagueSigilGUID = ObjectGuid::Empty;
                BloodwingSigilGUID = ObjectGuid::Empty;
                FrostwingSigilGUID = ObjectGuid::Empty;
                memset(PutricideValveGUIDs, 0, 2 * sizeof(uint64));
                memset(PutricideValveState, 0, 2 * sizeof(bool));
                memset(PutricidePipeGUIDs, 0, 2 * sizeof(uint64));
                memset(PutricideGateGUIDs, 0, 2 * sizeof(uint64));
                PutricideCollisionGUID = ObjectGuid::Empty;
                FestergutGUID = ObjectGuid::Empty;
                RotfaceGUID = ObjectGuid::Empty;
                ProfessorPutricideGUID = ObjectGuid::Empty;
                PutricideTableGUID = ObjectGuid::Empty;
                memset(BloodCouncilGUIDs, 0, 3 * sizeof(uint64));
                BloodCouncilControllerGUID = ObjectGuid::Empty;
                BloodQueenLanaThelGUID = ObjectGuid::Empty;
                CrokScourgebaneGUID = ObjectGuid::Empty;
                memset(CrokCaptainGUIDs, 0, 4 * sizeof(uint64));
                SisterSvalnaGUID = ObjectGuid::Empty;
                ValithriaDreamwalkerGUID = ObjectGuid::Empty;
                ValithriaLichKingGUID = ObjectGuid::Empty;
                ValithriaTriggerGUID = ObjectGuid::Empty;
                SindragosaGUID = ObjectGuid::Empty;
                SindragosaDoorGUID = ObjectGuid::Empty;
                SpinestalkerGUID = ObjectGuid::Empty;
                RimefangGUID = ObjectGuid::Empty;
                TheLichKingGUID = ObjectGuid::Empty;
                HighlordTirionFordringGUID = ObjectGuid::Empty;
                TerenasMenethilGUID = ObjectGuid::Empty;
                ArthasPlatformGUID = ObjectGuid::Empty;
                ArthasPrecipiceGUID = ObjectGuid::Empty;
                FrozenThroneEdgeGUID = ObjectGuid::Empty;
                FrozenThroneWindGUID = ObjectGuid::Empty;
                FrozenThroneWarningGUID = ObjectGuid::Empty;
                FrostwyrmCount = 0;
                SpinestalkerTrashCount = 0;
                RimefangTrashCount = 0;
                FrostyEdgeInner = ObjectGuid::Empty;
                FrostyEdgeOuter = ObjectGuid::Empty;
                EdgeDestroyWarning = ObjectGuid::Empty;
                FrozenLavaman = ObjectGuid::Empty;
                LavamanPillars = ObjectGuid::Empty;
                IsBonedEligible = true;
                IsOozeDanceEligible = true;
                IsNauseaEligible = true;
                IsOrbWhispererEligible = true;
                ColdflameJetsState = NOT_STARTED;
                BloodQuickeningState = NOT_STARTED;
                BloodQuickeningMinutes = 0;
                SindragosasWardGUID = ObjectGuid::Empty;
                UnstableExperimentStalkerRedGUID = ObjectGuid::Empty;
                UnstableExperimentStalkerGreenGUID = ObjectGuid::Empty;
                PutricideTrapState = NOT_STARTED;
                IccBuffSelected = 30;
                // Gunship Battle
                FirstSquadState = 0;
                SecondSquadState = 0;
                SpireSquadState = 0;
                SkybreakerBossGUID = ObjectGuid::Empty;
                OrgrimmarBossGUID = ObjectGuid::Empty;
                DeathbringerSaurfangGbGUID = ObjectGuid::Empty;
                MuradinBronzebeardGbGUID = ObjectGuid::Empty;
                GbBattleMageGUID = ObjectGuid::Empty;
                isPrepared = false;
                // Gunship Battle
            }

            void FillInitialWorldStates(WorldStateBuilder& builder) override
            {
                builder.AppendState(WORLDSTATE_SHOW_TIMER,         BloodQuickeningState == IN_PROGRESS);
                builder.AppendState(WORLDSTATE_EXECUTION_TIME,     BloodQuickeningMinutes);
                builder.AppendState(WORLDSTATE_SHOW_ATTEMPTS,      instance->IsHeroic());
                builder.AppendState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                builder.AppendState(WORLDSTATE_ATTEMPTS_MAX,       MaxHeroicAttempts);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!TeamInInstance)
                    TeamInInstance = player->GetTeam();

                player->CastSpell(player, TeamInInstance == HORDE ? 300108 : 300109, true);

                PickWeeklyQuestIfNeeded();
                SpawnGunship(); // Spawn Gunship Event
                ApplyIccBuff(player); // ICC buff
                if (GetData(DATA_ZONE_BUFF_STATUS) == 1)
                {
                    if (player->HasAura(AURA_ZONE_PCT_BUFF_A_10))
                        DoRemoveAurasDueToSpellOnPlayers(AURA_ZONE_PCT_BUFF_A_10);
                    else if (player->HasAura(AURA_ZONE_PCT_BUFF_H_10))
                        DoRemoveAurasDueToSpellOnPlayers(AURA_ZONE_PCT_BUFF_H_10);
                }

                // Apply Worldstates for Achievements
                if (player->GetMap()->Is25ManRaid())
                    player->GetMap()->SetWorldState(WORLDSTATE_DEFAULT_25_NORMAL, 1);
                else
                    player->GetMap()->SetWorldState(WORLDSTATE_DEFAULT_10_NORMAL, 1);

                if (!TrapsRandomized)
                {
                    TrapsRandomized = true;
                    // Ensure all traps are loaded
                    instance->LoadGrid(-250.0f, 2211.0f);
                    instance->LoadGrid(4357.0f, 3028.0f);
                    auto randomizeTraps = [this](std::list<ObjectGuid>& guids, uint32 count)
                    {
                        if (guids.size() <= count)
                            return;

                        std::list<ObjectGuid> remove = guids;
                        Trinity::Containers::RandomResizeList(remove, remove.size() - count);
                        for (auto&& guid : remove)
                        {
                            guids.remove(guid);
                            if (GameObject* trap = instance->GetGameObject(guid))
                            {
                                trap->SetLootState(GO_JUST_DEACTIVATED); // Will set and save respawn time
                                trap->Delete();
                            }
                        }
                    };
                    randomizeTraps(TrapGUIDs[GO_SPIRIT_ALARM_1], 1);
                    randomizeTraps(TrapGUIDs[GO_SPIRIT_ALARM_2], 1);
                    randomizeTraps(TrapGUIDs[GO_SPIRIT_ALARM_3], 1);
                    randomizeTraps(TrapGUIDs[GO_SPIRIT_ALARM_4], 1);
                    randomizeTraps(TrapGUIDs[GO_GEIST_ALARM_1], 4);
                    SaveToDB();
                }
                else if (!TrapsDeleted)
                {
                    TrapsDeleted = true;
                    // Ensure all traps are loaded
                    instance->LoadGrid(-250.0f, 2211.0f);
                    instance->LoadGrid(4357.0f, 3028.0f);
                    auto deleteTraps = [this](std::list<ObjectGuid>& guids)
                    {
                        for (auto&& guid : guids)
                            if (GameObject* trap = instance->GetGameObject(guid))
                                if (!trap->isSpawned())
                                    trap->Delete();
                    };
                    deleteTraps(TrapGUIDs[GO_SPIRIT_ALARM_1]);
                    deleteTraps(TrapGUIDs[GO_SPIRIT_ALARM_2]);
                    deleteTraps(TrapGUIDs[GO_SPIRIT_ALARM_3]);
                    deleteTraps(TrapGUIDs[GO_SPIRIT_ALARM_4]);
                    deleteTraps(TrapGUIDs[GO_GEIST_ALARM_1]);
                }

                if (!QuestNPCsSpawned)
                {
                    QuestNPCsSpawned = true;
                    if (GetBossState(DATA_THE_LICH_KING) == DONE && instance->Is25ManRaid())
                    {
                        instance->SummonCreature(NPC_LADY_JAINA_PROUDMOORE_QUEST, JainaSpawnPos);
                        instance->SummonCreature(NPC_MURADIN_BRONZEBEARD_QUEST, MuradinSpawnPos);
                        instance->SummonCreature(NPC_UTHER_THE_LIGHTBRINGER_QUEST, UtherSpawnPos);
                        instance->SummonCreature(NPC_LADY_SYLVANAS_WINDRUNNER_QUEST, SylvanasSpawnPos);
                    }
                }

                if (!ValithriaQuestgiveSpawned)
                {
                    ValithriaQuestgiveSpawned = true;
                    if (WeeklyIndex == WeeklyIndexICC::RespiteForATormentedSoul && GetBossState(DATA_VALITHRIA_DREAMWALKER) == DONE)
                        instance->SummonCreature(NPC_VALITHRIA_DREAMWALKER_QUEST, ValithriaSpawnPos);
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                PickTeamInInstanceIfNeeded();
                PickWeeklyQuestIfNeeded();

                if (TeamInInstance == ALLIANCE && (creature->GetPhaseMask() & 64))
                    creature->SetPhaseMask(128, true);
                if (TeamInInstance == HORDE && (creature->GetPhaseMask() & 128))
                    creature->SetPhaseMask(64, true);

                switch (creature->GetEntry())
                {
                    case NPC_LORD_MARROWGAR:
                        LordMarrowgarGUID = creature->GetGUID();
                        break;
                    case NPC_LADY_DEATHWHISPER:
                        LadyDeahtwhisperGUID = creature->GetGUID();
                        break;
                    case NPC_GB_SKYBREAKER:
                    case NPC_GB_SKYBREAKER10H:
                    case NPC_GB_SKYBREAKER25:
                    case NPC_GB_SKYBREAKER25H:
                        SkybreakerBossGUID = creature->GetGUID();
                        break;
                    case NPC_GB_ORGRIMS_HAMMER:
                    case NPC_GB_ORGRIMS_HAMMER10H:
                    case NPC_GB_ORGRIMS_HAMMER25:
                    case NPC_GB_ORGRIMS_HAMMER25H:
                        OrgrimmarBossGUID = creature->GetGUID();
                        break;
                    case NPC_GB_HIGH_OVERLORD_SAURFANG:
                        DeathbringerSaurfangGbGUID = creature->GetGUID();
                        break;
                    case NPC_GB_MURADIN_BRONZEBEARD:
                        MuradinBronzebeardGbGUID = creature->GetGUID();
                        break;
                    case NPC_GB_ALLIANCE_CANNON:
                    case NPC_GB_HORDE_CANNON:
                        if (GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            creature->m_Events.Schedule(1, [creature] { creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK); });
                        break;
                    case NPC_GB_ZAFOD_BOOMBOX:
                        if (GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                    case NPC_KOR_KRON_GENERAL:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALLIANCE_COMMANDER, ALLIANCE);
                        if (LightsHammerNPCsLeft)
                            creature->SetVisible(false);
                        LightsHammerKorkronGeneralAllianceCommanderGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_KOR_KRON_LIEUTENANT:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_SKYBREAKER_LIEUTENANT, ALLIANCE);
                        if (creature->GetPositionZ() < 50.0f)
                        {
                            KorkronLieutenantGUID = creature->GetGUID();
                            if (RottingFrostGiantState != DONE)
                                creature->SetVisible(false);
                        }
                        break;
                    case NPC_TORTUNOK:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALANA_MOONSTRIKE, ALLIANCE);
                        break;
                    case NPC_GERARDO_THE_SUAVE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_TALAN_MOONSTRIKE, ALLIANCE);
                        break;
                    case NPC_UVLUS_BANEFIRE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MALFUS_GRIMFROST, ALLIANCE);
                        break;
                    case NPC_IKFIRUS_THE_VILE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_YILI, ALLIANCE);
                        break;
                    case NPC_VOL_GUK:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JEDEBIA, ALLIANCE);
                        break;
                    case NPC_HARAGG_THE_UNSEEN:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_NIBY_THE_ALMIGHTY, ALLIANCE);
                        break;
                    case NPC_GARROSH_HELLSCREAM:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_KING_VARIAN_WRYNN, ALLIANCE);
                        break;
                    case NPC_DEATHBRINGER_SAURFANG:
                        DeathbringerSaurfangGUID = creature->GetGUID();
                        break;
                    case NPC_SE_HIGH_OVERLORD_SAURFANG:
                        if (creature->IsSummon() || creature->GetTransport())
                            break;
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_SE_MURADIN_BRONZEBEARD, ALLIANCE, creature->GetCreatureData());
                        if (creature->GetPositionZ() < 100.0f)
                        {
                            if (LightsHammerNPCsLeft)
                                creature->SetVisible(false);
                            LightsHammerSaurfangMuradinGUID = creature->GetGUID();
                            break;
                        }
                        // no break;
                    case NPC_SE_MURADIN_BRONZEBEARD:
                        DeathbringerSaurfangEventGUID = creature->GetGUID();
                        creature->LastUsedScriptID = creature->GetScriptId();
                        if (GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            creature->SetPhaseMask(PHASEMASK_NORMAL, true);
                        if (GetBossState(DATA_DEATHBRINGER_SAURFANG) == DONE && TeamInInstance == HORDE)
                            creature->SetVisible(false);
                        break;
                    case NPC_SE_KOR_KRON_REAVER:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_SE_SKYBREAKER_MARINE, ALLIANCE);
                        if (GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            creature->SetPhaseMask(PHASEMASK_NORMAL, true);
                        break;
                    case NPC_FESTERGUT:
                        FestergutGUID = creature->GetGUID();
                        break;
                    case NPC_ROTFACE:
                        RotfaceGUID = creature->GetGUID();
                        break;
                    case NPC_PROFESSOR_PUTRICIDE:
                        ProfessorPutricideGUID = creature->GetGUID();
                        break;
                    case NPC_PRINCE_KELESETH:
                        BloodCouncilGUIDs[0] = creature->GetGUID();
                        break;
                    case NPC_PRINCE_TALDARAM:
                        BloodCouncilGUIDs[1] = creature->GetGUID();
                        break;
                    case NPC_PRINCE_VALANAR:
                        BloodCouncilGUIDs[2] = creature->GetGUID();
                        break;
                    case NPC_BLOOD_ORB_CONTROLLER:
                        BloodCouncilControllerGUID = creature->GetGUID();
                        break;
                    case NPC_BLOOD_QUEEN_LANA_THEL_COUNCIL:
                        BloodQueenLanaThelCouncilGUID = creature->GetGUID();
                        break;
                    case NPC_BLOOD_QUEEN_LANA_THEL:
                        BloodQueenLanaThelGUID = creature->GetGUID();
                        break;
                    case NPC_CROK_SCOURGEBANE:
                        if (!creature->GetDBTableGUIDLow())
                            break;
                        CrokScourgebaneGUID = creature->GetGUID();
                        break;
                        // we can only do this because there are no gaps in their entries
                    case NPC_CAPTAIN_ARNATH:
                    case NPC_CAPTAIN_BRANDON:
                    case NPC_CAPTAIN_GRONDEL:
                    case NPC_CAPTAIN_RUPERT:
                        if (!creature->GetDBTableGUIDLow())
                            break;
                        CrokCaptainGUIDs[creature->GetEntry() - NPC_CAPTAIN_ARNATH] = creature->GetGUID();
                        break;
                    case NPC_SISTER_SVALNA:
                        SisterSvalnaGUID = creature->GetGUID();
                        break;
                    case NPC_VALITHRIA_DREAMWALKER:
                        ValithriaDreamwalkerGUID = creature->GetGUID();
                        break;
                    case NPC_THE_LICH_KING_VALITHRIA:
                        ValithriaLichKingGUID = creature->GetGUID();
                        break;
                    case NPC_GREEN_DRAGON_COMBAT_TRIGGER:
                        ValithriaTriggerGUID = creature->GetGUID();
                        break;
                    case NPC_SINDRAGOSA:
                        SindragosaGUID = creature->GetGUID();
                        break;
                    case NPC_SPINESTALKER:
                        SpinestalkerGUID = creature->GetGUID();
                        break;
                    case NPC_RIMEFANG:
                        RimefangGUID = creature->GetGUID();
                        break;
                    case NPC_INVISIBLE_STALKER:
                        // Teleporter visual at center
                        if (creature->GetExactDist2d(4357.052f, 2769.421f) < 10.0f && GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && GetBossState(DATA_SINDRAGOSA) == DONE)
                            creature->CastSpell(creature, SPELL_ARTHAS_TELEPORTER_CEREMONY, true);
                        break;
                    case NPC_THE_LICH_KING:
                        TheLichKingGUID = creature->GetGUID();
                        break;
                    case NPC_HIGHLORD_TIRION_FORDRING_LK:
                        HighlordTirionFordringGUID = creature->GetGUID();
                        break;
                    case NPC_TERENAS_MENETHIL_FROSTMOURNE:
                    case NPC_TERENAS_MENETHIL_FROSTMOURNE_H:
                        TerenasMenethilGUID = creature->GetGUID();
                        break;
                    case NPC_WICKED_SPIRIT:
                        // Remove corpse as soon as it dies (and respawn 10 seconds later)
                        creature->SetCorpseDelay(0);
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_SINDRAGOSAS_WARD:
                        SindragosasWardGUID = creature->GetGUID();
                        break;
                    case NPC_ABOMINATION_WING_MAD_SCIENTIST_STALKER:
                        if (creature->GetPositionX() < 4356.0f)
                            UnstableExperimentStalkerRedGUID = creature->GetGUID();
                        else
                            UnstableExperimentStalkerGreenGUID = creature->GetGUID();
                        break;
                    case NPC_THE_LICH_KING_LH:
                        LightsHammerTheLichKingGUID = creature->GetGUID();
                        break;
                    default:
                        if (creature->IsGuardian() && creature->GetOwnerGUID().IsPlayer())
                            ApplyIccBuff(creature);
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                if (creature->GetEntry() == NPC_SINDRAGOSA)
                    SindragosaGUID = ObjectGuid::Empty;
            }

            // Weekly quest spawn prevention
            uint32 GetCreatureEntry(uint32 guidLow, CreatureData const* data) override
            {
                PickTeamInInstanceIfNeeded();
                PickWeeklyQuestIfNeeded();

                uint32 entry = data->id;
                switch (entry)
                {
                    case NPC_INFILTRATOR_MINCHAR:
                        return WeeklyIndex == WeeklyIndexICC::Deprogramming ? entry : 0;
                    case NPC_KOR_KRON_LIEUTENANT:
                        return WeeklyIndex == WeeklyIndexICC::SecuringTheRamparts ? entry : 0;
                    case NPC_ALCHEMIST_ADRIANNA:
                        return WeeklyIndex == WeeklyIndexICC::ResidueRendezvous ? entry : 0;
                    case NPC_ALRIN_THE_AGILE:
                    case NPC_INFILTRATOR_MINCHAR_BQ:
                    case NPC_MINCHAR_BEAM_STALKER:
                        return WeeklyIndex == WeeklyIndexICC::BloodQuickening ? entry : 0;
                    case NPC_VALITHRIA_DREAMWALKER_QUEST:
                        return WeeklyIndex == WeeklyIndexICC::RespiteForATormentedSoul ? entry : 0;
                    case NPC_SE_KOR_KRON_REAVER:
                        return guidLow != 582302 || TeamInInstance == HORDE ? entry : 0;
                    case NPC_GB_HORDE_CANNON:
                    case NPC_GB_ORGRIMS_HAMMER_CREW:
                    case NPC_GB_SKY_REAVER_KORM_BLACKSCAR:
                    case NPC_GB_KORKRON_NECROLYTE:
                        if (TeamInInstance == ALLIANCE)
                            return 0;
                        break;
                    case NPC_GB_ALLIANCE_CANNON:
                    case NPC_GB_SKYBREAKER_DECKHAND:
                    case NPC_GB_HIGH_CAPTAIN_JUSTIN_BARTLETT:
                        if (TeamInInstance == HORDE)
                            return 0;
                        break;
                    case NPC_GB_ZAFOD_BOOMBOX:
                        if (GameObjectTemplate const* go = sObjectMgr->GetGameObjectTemplate(GO_THE_SKYBREAKER_ALLIANCE_ICC))
                            if ((TeamInInstance == ALLIANCE && data->mapId == go->moTransport.mapID) ||
                                (TeamInInstance == HORDE && data->mapId != go->moTransport.mapID))
                                return entry;
                        return 0;
                    case NPC_GB_HIGH_OVERLORD_SAURFANG:
                        return GetBossState(DATA_GUNSHIP_EVENT) != DONE ? entry : 0;
                    case NPC_GB_MURADIN_BRONZEBEARD:
                        if ((TeamInInstance == ALLIANCE && data->posX > 10.0f) ||
                            (TeamInInstance == HORDE && data->posX < 10.0f))
                            return GetBossState(DATA_GUNSHIP_EVENT) != DONE ? entry : 0;
                        return 0;
                    case NPC_SE_HIGH_OVERLORD_SAURFANG:
                        return TeamInInstance == ALLIANCE ? NPC_SE_MURADIN_BRONZEBEARD : NPC_SE_HIGH_OVERLORD_SAURFANG;                        
                    default:
                        break;
                }

                return entry;
            }

            uint32 GetGameObjectEntry(uint32 guidlow, uint32 entry) override
            {
                switch (entry)
                {
                    case GO_GUNSHIP_ARMORY_H_10N:
                    case GO_GUNSHIP_ARMORY_H_25N:
                    case GO_GUNSHIP_ARMORY_H_10H:
                    case GO_GUNSHIP_ARMORY_H_25H:
                        if (TeamInInstance == ALLIANCE || GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            return 0;
                        break;
                    case GO_GUNSHIP_ARMORY_A_10N:
                    case GO_GUNSHIP_ARMORY_A_25N:
                    case GO_GUNSHIP_ARMORY_A_10H:
                    case GO_GUNSHIP_ARMORY_A_25H:
                        if (TeamInInstance == HORDE || GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            return 0;
                        break;
                    default:
                        break;
                }

                return entry;
            }

            void OnUnitDeath(Unit* unit) override
            {
                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                switch (creature->GetEntry())
                {
                    case NPC_YMIRJAR_BATTLE_MAIDEN:
                    case NPC_YMIRJAR_DEATHBRINGER:
                    case NPC_YMIRJAR_FROSTBINDER:
                    case NPC_YMIRJAR_HUNTRESS:
                    case NPC_YMIRJAR_WARLORD:
                        if (Creature* crok = instance->GetCreature(CrokScourgebaneGUID))
                            crok->AI()->SetGUID(creature->GetGUID(), ACTION_VRYKUL_DEATH);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                PickTeamInInstanceIfNeeded();

                switch (go->GetEntry())
                {
                    case GO_DOODAD_ICECROWN_ICEWALL02:
                    case GO_ICEWALL:
                    case GO_LORD_MARROWGAR_S_ENTRANCE:
                    case GO_ORATORY_OF_THE_DAMNED_ENTRANCE:
                    case GO_ORANGE_PLAGUE_MONSTER_ENTRANCE:
                    case GO_GREEN_PLAGUE_MONSTER_ENTRANCE:
                    case GO_BLOOD_ELF_COUNCIL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT:
                    case GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01:
                    case GO_DOODAD_ICECROWN_GRATE_01:
                    case GO_GREEN_DRAGON_BOSS_ENTRANCE:
                    case GO_GREEN_DRAGON_BOSS_EXIT:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03:
                    //case GO_SINDRAGOSA_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR:
                    case GO_ICE_WALL:
                        AddDoor(go, true);
                        break;
                    case GO_SCIENTIST_ENTRANCE:
                        PutricideEntranceGUID = go->GetGUID();
                        AddDoor(go, true);
                        HandleGameObject(PutricideEntranceGUID, PutricideTrapState == DONE, go);
                        break;
                    case GO_CRIMSON_HALL_DOOR:
                        CrimsonHallDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    // these 2 gates are functional only on 25man modes
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04:
                        if (instance->Is25ManRaid())
                            AddDoor(go, true);
                        break;
                    case GO_LADY_DEATHWHISPER_ELEVATOR:
                        LadyDeathwisperElevatorGUID = go->GetGUID();
                        if (GetBossState(DATA_LADY_DEATHWHISPER) == DONE)
                            Events.ScheduleEvent(EVENT_TOGGLE_ELEVATOR, 20000);
                        break;
                    case GO_SAURFANG_S_DOOR:
                        DeathbringerSaurfangDoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_BLOODWING_DOOR:
                        BloodwingDoor = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_FROSTWING_DOOR:
                        FrostwingDoor = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_DEATHBRINGER_S_CACHE_10N:
                    case GO_DEATHBRINGER_S_CACHE_25N:
                    case GO_DEATHBRINGER_S_CACHE_10H:
                    case GO_DEATHBRINGER_S_CACHE_25H:
                        DeathbringersCacheGUID = go->GetGUID();
                        break;
                    case GO_PLAGUE_SIGIL:
                        PlagueSigilGUID = go->GetGUID();
                        if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE)
                            HandleGameObject(PlagueSigilGUID, false, go);
                        break;
                    case GO_BLOODWING_SIGIL:
                        BloodwingSigilGUID = go->GetGUID();
                        if (GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE)
                            HandleGameObject(BloodwingSigilGUID, false, go);
                        break;
                    case GO_SIGIL_OF_THE_FROSTWING:
                        FrostwingSigilGUID = go->GetGUID();
                        if (GetBossState(DATA_SINDRAGOSA) == DONE)
                            HandleGameObject(FrostwingSigilGUID, false, go);
                        break;
                    case GO_SCIENTIST_AIRLOCK_DOOR_COLLISION:
                        PutricideCollisionGUID = go->GetGUID();
                        if (PutricideValveState[0] && PutricideValveState[1])
                            HandleGameObject(PutricideCollisionGUID, true, go);
                        break;
                    case GO_SCIENTIST_AIRLOCK_DOOR_ORANGE:
                        PutricideGateGUIDs[0] = go->GetGUID();
                        if (PutricideValveState[0] && PutricideValveState[1])
                            go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        else if (PutricideValveState[0])
                            HandleGameObject(PutricideGateGUIDs[0], false, go);
                        break;
                    case GO_SCIENTIST_AIRLOCK_DOOR_GREEN:
                        PutricideGateGUIDs[1] = go->GetGUID();
                        if (PutricideValveState[0] && PutricideValveState[1])
                            go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        else if (PutricideValveState[1])
                            HandleGameObject(PutricideGateGUIDs[1], false, go);
                        break;
                    case GO_GAS_RELEASE_VALVE:
                        PutricideValveGUIDs[0] = go->GetGUID();
                        go->ApplyModFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE, PutricideValveState[0] || GetBossState(DATA_FESTERGUT) != DONE);
                        break;
                    case GO_OOZE_RELEASE_VALVE:
                        PutricideValveGUIDs[1] = go->GetGUID();
                        go->ApplyModFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE, PutricideValveState[1] || GetBossState(DATA_ROTFACE) != DONE);
                        break;
                    case GO_DOODAD_ICECROWN_ORANGETUBES02:
                        PutricidePipeGUIDs[0] = go->GetGUID();
                        if (PutricideValveState[0])
                            HandleGameObject(PutricidePipeGUIDs[0], true, go);
                        break;
                    case GO_DOODAD_ICECROWN_GREENTUBES02:
                        PutricidePipeGUIDs[1] = go->GetGUID();
                        if (PutricideValveState[1])
                            HandleGameObject(PutricidePipeGUIDs[1], true, go);
                        break;
                    case GO_DRINK_ME:
                        PutricideTableGUID = go->GetGUID();
                        if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE)
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case GO_DREAMWALKER_CACHE_10N:
                    case GO_DREAMWALKER_CACHE_25N:
                    case GO_DREAMWALKER_CACHE_10H:
                    case GO_DREAMWALKER_CACHE_25H:
                        //dreamwalkerCache = go->GetGUID();
                        if (Creature* valithria = instance->GetCreature(ValithriaDreamwalkerGUID))
                        {
                            Player* looter = valithria->GetLootRecipient();
                            Group* group = valithria->GetLootRecipientGroup();
                            if (group && (!looter || looter->GetGroup() != group))
                                if (GroupReference const* memberRef = group->GetFirstMember())
                                    if (Player* member = memberRef->GetSource())
                                        looter = member;

                            go->SetLootRecipient(looter);
                        }
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                        break;
                    case GO_GUNSHIP_ARMORY_H_10N:
                    case GO_GUNSHIP_ARMORY_H_25N:
                    case GO_GUNSHIP_ARMORY_H_10H:
                    case GO_GUNSHIP_ARMORY_H_25H:
                    case GO_GUNSHIP_ARMORY_A_10N:
                    case GO_GUNSHIP_ARMORY_A_25N:
                    case GO_GUNSHIP_ARMORY_A_10H:
                    case GO_GUNSHIP_ARMORY_A_25H:
                        GunshipArmoryGUID = go->GetGUID();
                        break;
                    case GO_SINDRAGOSA_ENTRANCE_DOOR:
                        SindragosaDoorGUID = go->GetGUID();
                        break;
                    case GO_ARTHAS_PLATFORM:
                        // this enables movement at The Frozen Throne, when printed this value is 0.000000f
                        // however, when represented as integer client will accept only this value
                        // go->SetUInt32Value(GAMEOBJECT_FIELD_PARENT_ROTATION, 5535469); // disappear when we set it, fixme
                        ArthasPlatformGUID = go->GetGUID();
                        break;
                    case GO_ARTHAS_PRECIPICE:
                        // go->SetUInt32Value(GAMEOBJECT_FIELD_PARENT_ROTATION, 4178312); // disappear when we set it, fixme
                        ArthasPrecipiceGUID = go->GetGUID();
                        break;
                    case GO_DOODAD_ICECROWN_THRONEFROSTYEDGE01:
                        FrozenThroneEdgeGUID = go->GetGUID();
                        break;
                   case GO_DOODAD_ICECROWN_THRONEFROSTYWIND01:
                        FrozenThroneWindGUID = go->GetGUID();
                        break;
                    case GO_DOODAD_ICECROWN_SNOWEDGEWARNING01:
                        FrozenThroneWarningGUID = go->GetGUID();
                        break;
                   case GO_FROZEN_LAVAMAN:
                        FrozenBolvarGUID = go->GetGUID();
                        if (GetBossState(DATA_THE_LICH_KING) == DONE)
                            go->SetRespawnTime(7 * DAY);
                        break;
                    case GO_LAVAMAN_PILLARS_CHAINED:
                        PillarsChainedGUID = go->GetGUID();
                        if (GetBossState(DATA_THE_LICH_KING) == DONE)
                            go->SetRespawnTime(7 * DAY);
                        break;
                    case GO_LAVAMAN_PILLARS_UNCHAINED:
                        PillarsUnchainedGUID = go->GetGUID();
                        if (GetBossState(DATA_THE_LICH_KING) == DONE)
                            go->SetRespawnTime(7 * DAY);
                        break;
                    case GO_SPIRIT_ALARM_1:
                    case GO_SPIRIT_ALARM_2:
                    case GO_SPIRIT_ALARM_3:
                    case GO_SPIRIT_ALARM_4:
                        TrapGUIDs[go->GetEntry()].push_back(go->GetGUID());
                        break;
                    case GO_GEIST_ALARM_1:
                    case GO_GEIST_ALARM_2:
                        TrapGUIDs[GO_GEIST_ALARM_1].push_back(go->GetGUID());
                        break;
                    case GO_ALLIANCE_TELEPORTER:
                    case GO_HORDE_TELEPORTER:
                        if (TeamInInstance == (go->GetEntry() == GO_ALLIANCE_TELEPORTER ? ALLIANCE : HORDE) && GetBossState(DATA_DEATHBRINGER_SAURFANG) == DONE)
                        {
                            go->SetPhaseMask(PHASEMASK_NORMAL, true);
                            go->SetGoState(GO_STATE_ACTIVE);
                        }
                        break;
                    case GO_ALLIANCE_TENT:
                    case GO_BLACKSMITHS_ANVIL_A:
                    case GO_FORGE_A:
                    case GO_ALLIANCE_BANNER:
                        if (TeamInInstance == ALLIANCE && GetBossState(DATA_DEATHBRINGER_SAURFANG) == DONE)
                            go->SetPhaseMask(PHASEMASK_NORMAL, true);
                        break;
                    case GO_HORDE_TENT_1:
                    case GO_HORDE_TENT_2:
                    case GO_BLACKSMITHS_ANVIL_H:
                    case GO_FORGE_H:
                    //case GO_BONFIRE:
                        if (TeamInInstance == HORDE && GetBossState(DATA_DEATHBRINGER_SAURFANG) == DONE)
                            go->SetPhaseMask(PHASEMASK_NORMAL, true);
                        break;
                    case GO_SCOURGE_TRANSPORTER_LIGHTS_HAMMER:
                        ScourgeTransporterLightsHammerGUID = go->GetGUID();
                        if (GetBossState(DATA_LORD_MARROWGAR) != DONE)
                        {
                            go->SetGoState(GO_STATE_READY);
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        }
                        break;
                    case GO_SCOURGE_TRANSPORTER_DEATHBRINGERS:
                        ScourgeTransporterDeathbringersRiseGUID = go->GetGUID();
                        break;
                    case GO_SCOURGE_TRANSPORTER_UPPER_SPIRE:
                        ScourgeTransporterUpperSpireGUID = go->GetGUID();
                        if (GetData(DATA_COLDFLAME_JETS) != DONE)
                        {
                            go->SetGoState(GO_STATE_READY);
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        }
                        break;
                    case GO_SCOURGE_TRANSPORTER_LK:
                        ScourgeTransporterTheLickKingGUID = go->GetGUID();
                        if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && GetBossState(DATA_SINDRAGOSA) == DONE)
                        {
                            go->SetGoState(GO_STATE_ACTIVE);
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        }
                        else
                        {
                            go->SetGoState(GO_STATE_READY);
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        }
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_DOODAD_ICECROWN_ICEWALL02:
                    case GO_ICEWALL:
                    case GO_LORD_MARROWGAR_S_ENTRANCE:
                    case GO_ORATORY_OF_THE_DAMNED_ENTRANCE:
                    case GO_SAURFANG_S_DOOR:
                    case GO_FROSTWING_DOOR:
                    case GO_BLOODWING_DOOR:
                    case GO_ORANGE_PLAGUE_MONSTER_ENTRANCE:
                    case GO_GREEN_PLAGUE_MONSTER_ENTRANCE:
                    case GO_SCIENTIST_ENTRANCE:
                    case GO_CRIMSON_HALL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR:
                    case GO_BLOOD_ELF_COUNCIL_DOOR_RIGHT:
                    case GO_DOODAD_ICECROWN_BLOODPRINCE_DOOR_01:
                    case GO_DOODAD_ICECROWN_GRATE_01:
                    case GO_GREEN_DRAGON_BOSS_ENTRANCE:
                    case GO_GREEN_DRAGON_BOSS_EXIT:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_01:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_02:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_03:
                    case GO_DOODAD_ICECROWN_ROOSTPORTCULLIS_04:
                    //case GO_SINDRAGOSA_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_ENTRANCE_DOOR:
                    case GO_SINDRAGOSA_SHORTCUT_EXIT_DOOR:
                    case GO_ICE_WALL:
                        AddDoor(go, false);
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_FIRST_SQUAD_STATE:
                        return FirstSquadState;
                    case DATA_SECOND_SQUAD_STATE:
                        return SecondSquadState;
                    case DATA_SPIRE_FROSTWYRM_STATE:
                        return SpireSquadState;
                    case DATA_SINDRAGOSA_FROSTWYRMS:
                        return FrostwyrmCount;
                    case DATA_SPINESTALKER:
                        return SpinestalkerTrashCount;
                    case DATA_RIMEFANG:
                        return RimefangTrashCount;
                    case DATA_COLDFLAME_JETS:
                        return ColdflameJetsState;
                    case DATA_TEAM_IN_INSTANCE:
                        return TeamInInstance;
                    case DATA_BLOOD_QUICKENING_STATE:
                        return BloodQuickeningState;
                    case DATA_PUTRICIDE_TRAP:
                        return PutricideTrapState;
                    case DATA_HEROIC_ATTEMPTS:
                        return HeroicAttempts;
                    case DATA_ZONE_BUFF_STATUS:
                        return ZoneBuffState;
                    case DATA_ICC_BUFF:
                        return IccBuffSelected;
                    case DATA_LIGHTS_HAMMER_NPCS_LEFT:
                        return LightsHammerNPCsLeft;
                    case DATA_WEEKLY_QUEST_INDEX:
                        return (uint32)WeeklyIndex;
                    case DATA_CHECK_LICH_KING_AVAILABILITY:
                        return CheckLichKingAvailability();
                    default:
                        break;
                }

                return 0;
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_GB_BATTLE_MAGE:
                        GbBattleMageGUID = data;
                        break;
                    default:
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_LORD_MARROWGAR:
                        return LordMarrowgarGUID;
                    case DATA_LADY_DEATHWHISPER:
                        return LadyDeahtwhisperGUID;
                   //Gunship battle
                       case DATA_SKYBREAKER_BOSS:
                        return SkybreakerBossGUID;
                    case DATA_ORGRIMMAR_HAMMER_BOSS:
                        return OrgrimmarBossGUID;
                    case DATA_GB_HIGH_OVERLORD_SAURFANG:
                        return DeathbringerSaurfangGbGUID;
                    case DATA_GB_MURADIN_BRONZEBEARD:
                        return MuradinBronzebeardGbGUID;
                    case DATA_GB_BATTLE_MAGE:
                        return GbBattleMageGUID;
                    case DATA_DEATHBRINGER_SAURFANG:
                        return DeathbringerSaurfangGUID;
                    case DATA_SAURFANG_EVENT_NPC:
                        return DeathbringerSaurfangEventGUID;
                    case GO_BLOODWING_DOOR:
                        return BloodwingDoor;
                    case GO_FROSTWING_DOOR:
                        return FrostwingDoor;
                    case GO_SAURFANG_S_DOOR:
                        return DeathbringerSaurfangDoorGUID;
                    case DATA_FESTERGUT:
                        return FestergutGUID;
                    case DATA_ROTFACE:
                        return RotfaceGUID;
                    case DATA_PROFESSOR_PUTRICIDE:
                        return ProfessorPutricideGUID;
                    case DATA_PUTRICIDE_TABLE:
                        return PutricideTableGUID;
                    case DATA_PRINCE_KELESETH_GUID:
                        return BloodCouncilGUIDs[0];
                    case DATA_PRINCE_TALDARAM_GUID:
                        return BloodCouncilGUIDs[1];
                    case DATA_PRINCE_VALANAR_GUID:
                        return BloodCouncilGUIDs[2];
                    case DATA_BLOOD_PRINCES_CONTROL:
                        return BloodCouncilControllerGUID;
                    case DATA_BLOOD_QUEEN_LANA_THEL_COUNCIL:
                        return BloodQueenLanaThelCouncilGUID;
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                        return BloodQueenLanaThelGUID;
                    case DATA_CROK_SCOURGEBANE:
                        return CrokScourgebaneGUID;
                    case DATA_CAPTAIN_ARNATH:
                    case DATA_CAPTAIN_BRANDON:
                    case DATA_CAPTAIN_GRONDEL:
                    case DATA_CAPTAIN_RUPERT:
                        return CrokCaptainGUIDs[type - DATA_CAPTAIN_ARNATH];
                    case DATA_SISTER_SVALNA:
                        return SisterSvalnaGUID;
                    case DATA_VALITHRIA_DREAMWALKER:
                        return ValithriaDreamwalkerGUID;
                    case DATA_VALITHRIA_LICH_KING:
                        return ValithriaLichKingGUID;
                    case DATA_VALITHRIA_TRIGGER:
                        return ValithriaTriggerGUID;
                    case DATA_SINDRAGOSA:
                        return SindragosaGUID;
                    case DATA_SPINESTALKER:
                        return SpinestalkerGUID;
                    case DATA_RIMEFANG:
                        return RimefangGUID;
                    case DATA_THE_LICH_KING:
                        return TheLichKingGUID;
                    case DATA_HIGHLORD_TIRION_FORDRING:
                        return HighlordTirionFordringGUID;
                    case DATA_ARTHAS_PLATFORM:
                        return ArthasPlatformGUID;
                    case DATA_TERENAS_MENETHIL:
                        return TerenasMenethilGUID;
                    case DATA_FROZEN_LAVAMAN:
                        return FrozenLavaman;
                    case DATA_LAVAMAN_PILLARS:
                        return LavamanPillars;
                    case DATA_FROSTY_EDGE_OUTER:
                        return FrostyEdgeOuter;
                    case DATA_FROSTY_EDGE_INNER:
                        return FrostyEdgeInner;
                    case DATA_EDGE_DESTROY_WARNING:
                        return EdgeDestroyWarning;
                   case DATA_SINDRAGOSA_GAUNTLET:
                        return SindragosasWardGUID;
                    case GO_SINDRAGOSA_ENTRANCE_DOOR:
                        return SindragosaDoorGUID;
                    case DATA_UNSTABLE_EXPERIMENT_STALKER_RED:
                        return UnstableExperimentStalkerRedGUID;
                    case DATA_UNSTABLE_EXPERIMENT_STALKER_GREEN:
                        return UnstableExperimentStalkerGreenGUID;
                    case GO_CRIMSON_HALL_DOOR:
                        return CrimsonHallDoorGUID;
                    case GO_SCOURGE_TRANSPORTER_DEATHBRINGERS:
                        return ScourgeTransporterDeathbringersRiseGUID;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (GetBossState(type) == DONE)
                    return false;

                uint32 oldState = GetBossState(type);

                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_LORD_MARROWGAR:
                        if (state == DONE)
                        {
                            if (Creature* saurfangMuradin = instance->GetCreature(LightsHammerSaurfangMuradinGUID))
                                saurfangMuradin->SetVisible(false);
                            for (auto&& guid : LightsHammerKorkronGeneralAllianceCommanderGUIDs)
                                if (Creature* generalCommander = instance->GetCreature(guid))
                                    generalCommander->SetVisible(false);

                            if (!LightsHammerNPCsLeft)
                            {
                                // Failsafe, shouldn't happen often
                                LightsHammerNPCsLeft = true;
                                SaveToDB();
                            }

                            if (GameObject* transporter = instance->GetGameObject(ScourgeTransporterLightsHammerGUID))
                            {
                                transporter->SetGoState(GO_STATE_ACTIVE);
                                transporter->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                            }
                        }
                        break;
                    case DATA_LADY_DEATHWHISPER:
                        if (state == DONE)
                            Events.ScheduleEvent(EVENT_TOGGLE_ELEVATOR, 1);
                        break;
                   case DATA_GUNSHIP_EVENT:
                        if (state == DONE)
                        {
                            if (GameObject* loot = instance->GetGameObject(GunshipArmoryGUID))
                                loot->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                        }
                        else if (state == FAIL)
                            Events.ScheduleEvent(EVENT_RESPAWN_GUNSHIP, 30000);
                        break;
                    case DATA_DEATHBRINGER_SAURFANG:
                        switch (state)
                        {
                            case DONE:
                                if (GameObject* loot = instance->GetGameObject(DeathbringersCacheGUID))
                                {
                                    if (Creature* deathbringer = instance->GetCreature(DeathbringerSaurfangGUID))
                                    {
                                        Player* looter = deathbringer->GetLootRecipient();
                                        Group* group = deathbringer->GetLootRecipientGroup();
                                        if (group && (!looter || looter->GetGroup() != group))
                                            if (GroupReference const* memberRef = group->GetFirstMember())
                                                if (Player* member = memberRef->GetSource())
                                                    looter = member;

                                        loot->SetLootRecipient(looter);
                                    }
                                    loot->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                                }
                                // no break
                            case NOT_STARTED:
                            case FAIL:
                                if (GameObject* teleporter = instance->GetGameObject(ScourgeTransporterDeathbringersRiseGUID))
                                {
                                    HandleGameObject(ScourgeTransporterDeathbringersRiseGUID, true, teleporter);
                                    teleporter->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case DATA_FESTERGUT:
                        if (state == DONE)
                            if (GameObject* go = instance->GetGameObject(PutricideValveGUIDs[0]))
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case DATA_ROTFACE:
                        if (state == DONE)
                            if (GameObject* go = instance->GetGameObject(PutricideValveGUIDs[1]))
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case DATA_PROFESSOR_PUTRICIDE:
                        HandleGameObject(PlagueSigilGUID, state != DONE);
                        if (state == DONE)
                            if (GameObject* go = instance->GetGameObject(PutricideTableGUID))
                                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        if (PutricideTrapState != DONE)
                            HandleGameObject(PutricideEntranceGUID, false);
                        if (instance->IsHeroic())
                        {
                            if (state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if (!HeroicAttempts)
                                    if (Creature* putricide = instance->GetCreature(ProfessorPutricideGUID))
                                        putricide->DespawnOrUnsummon();
                            }
                        }
                        break;
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                        HandleGameObject(BloodwingSigilGUID, state != DONE);
                        if (instance->IsHeroic())
                        {
                            if (state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if (!HeroicAttempts)
                                    if (Creature* bq = instance->GetCreature(BloodQueenLanaThelGUID))
                                        bq->DespawnOrUnsummon();
                            }
                        }
                        break;
                    case DATA_VALITHRIA_DREAMWALKER:
                        if (state == DONE)
                        {
                            if (WeeklyIndex == WeeklyIndexICC::RespiteForATormentedSoul)
                            {
                                instance->SummonCreature(NPC_VALITHRIA_DREAMWALKER_QUEST, ValithriaSpawnPos);
                                ValithriaQuestgiveSpawned = true;
                            }
                        }
                        break;
                    case DATA_SINDRAGOSA:
                        HandleGameObject(FrostwingSigilGUID, state != DONE);
                        if (instance->IsHeroic())
                        {
                            if (state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if (!HeroicAttempts)
                                    if (Creature* sindra = instance->GetCreature(SindragosaGUID))
                                        sindra->DespawnOrUnsummon();
                            }
                        }
                        break;
                    case DATA_THE_LICH_KING:
                    {
                        // set the platform as active object to dramatically increase visibility range
                        // note: "active" gameobjects do not block grid unloading
                        if (GameObject* precipice = instance->GetGameObject(ArthasPrecipiceGUID))
                            precipice->setActive(state == IN_PROGRESS);
                        if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                            platform->setActive(state == IN_PROGRESS);

                        if (instance->IsHeroic())
                        {
                            if (state == FAIL && HeroicAttempts)
                            {
                                --HeroicAttempts;
                                DoUpdateWorldState(WORLDSTATE_ATTEMPTS_REMAINING, HeroicAttempts);
                                if (!HeroicAttempts)
                                    if (Creature* theLichKing = instance->GetCreature(TheLichKingGUID))
                                        theLichKing->DespawnOrUnsummon();
                            }
                        }

                        if (state == FAIL)
                            Events.CancelEvent(EVENT_QUAKE_SHATTER);

                        if (state == DONE)
                        {
                            if (GameObject* bolvar = instance->GetGameObject(FrozenBolvarGUID))
                                bolvar->SetRespawnTime(7 * DAY);
                            if (GameObject* pillars = instance->GetGameObject(PillarsChainedGUID))
                                pillars->SetRespawnTime(7 * DAY);
                            if (GameObject* pillars = instance->GetGameObject(PillarsUnchainedGUID))
                                pillars->SetRespawnTime(7 * DAY);

                            if (instance->Is25ManRaid())
                            {
                                instance->SummonCreature(NPC_LADY_JAINA_PROUDMOORE_QUEST, JainaSpawnPos);
                                instance->SummonCreature(NPC_MURADIN_BRONZEBEARD_QUEST, MuradinSpawnPos);
                                instance->SummonCreature(NPC_UTHER_THE_LIGHTBRINGER_QUEST, UtherSpawnPos);
                                instance->SummonCreature(NPC_LADY_SYLVANAS_WINDRUNNER_QUEST, SylvanasSpawnPos);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                 }

                 return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_FIRST_SQUAD_STATE:
                        FirstSquadState = data;
                        break;
                    case DATA_SECOND_SQUAD_STATE:
                        SecondSquadState = data;
                        break;
                    case DATA_SPIRE_FROSTWYRM_STATE:
                        SpireSquadState = data;
                        break;
                    case DATA_BONED_ACHIEVEMENT:
                        IsBonedEligible = data ? true : false;
                        break;
                    case DATA_OOZE_DANCE_ACHIEVEMENT:
                        IsOozeDanceEligible = data ? true : false;
                        break;
                    case DATA_NAUSEA_ACHIEVEMENT:
                        IsNauseaEligible = data ? true : false;
                        break;
                    case DATA_ORB_WHISPERER_ACHIEVEMENT:
                        IsOrbWhispererEligible = data ? true : false;
                        break;
                    case DATA_SINDRAGOSA_FROSTWYRMS:
                    {
                        if (FrostwyrmCount == 255)
                            return;

                        if (instance->IsHeroic() && !HeroicAttempts)
                            return;

                        if (GetBossState(DATA_SINDRAGOSA) == DONE)
                            return;

                        switch (data)
                        {
                            case 0:
                                if (FrostwyrmCount)
                                {
                                    --FrostwyrmCount;
                                    Creature* rimefang = instance->GetCreature(RimefangGUID);
                                    Creature* spinestalker = instance->GetCreature(SpinestalkerGUID);
                                    if (!FrostwyrmCount || rimefang && rimefang->isDead() && spinestalker && spinestalker->isDead())
                                    {
                                        instance->LoadGrid(SindragosaSpawnPos.GetPositionX(), SindragosaSpawnPos.GetPositionY());
                                        if (Creature* boss = instance->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawnPos))
                                            boss->AI()->DoAction(ACTION_START_FROSTWYRM);
                                    }
                                }
                                break;
                            case 1:
                                ++FrostwyrmCount;
                                break;
                            default:
                                FrostwyrmCount = data;
                                break;
                        }
                        break;
                    }
                    case DATA_SPINESTALKER:
                    {
                        if (SpinestalkerTrashCount == 255)
                            return;

                        switch (data)
                        {
                            case 0:
                                if (SpinestalkerTrashCount)
                                {
                                    --SpinestalkerTrashCount;
                                    if (!SpinestalkerTrashCount)
                                        if (Creature* spinestalk = instance->GetCreature(SpinestalkerGUID))
                                            spinestalk->AI()->DoAction(ACTION_START_FROSTWYRM);
                                }
                                break;
                            case 1:
                                ++SpinestalkerTrashCount;
                                break;
                            default:
                                SpinestalkerTrashCount = data;
                                break;
                        }
                        break;
                    }
                    case DATA_RIMEFANG:
                    {
                        if (RimefangTrashCount == 255)
                            return;

                        switch (data)
                        {
                            case 0:
                                if (RimefangTrashCount)
                                {
                                    --RimefangTrashCount;
                                    if (!RimefangTrashCount)
                                        if (Creature* rime = instance->GetCreature(RimefangGUID))
                                            rime->AI()->DoAction(ACTION_START_FROSTWYRM);
                                }
                                break;
                            case 1:
                                ++RimefangTrashCount;
                                break;
                            default:
                                RimefangTrashCount = data;
                                break;
                        }
                        break;
                    }
                    case DATA_COLDFLAME_JETS:
                        ColdflameJetsState = data;
                        if (ColdflameJetsState == DONE)
                        {
                            SaveToDB();
                            if (GameObject* transporter = instance->GetGameObject(ScourgeTransporterUpperSpireGUID))
                            {
                                transporter->SetGoState(GO_STATE_ACTIVE);
                                transporter->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                            }
                        }
                        break;
                    case DATA_BLOOD_QUICKENING_STATE:
                    {
                        // skip if nothing changes
                        if (BloodQuickeningState == data)
                            break;

                        // 5 is the index of Blood Quickening
                        if (WeeklyIndex != WeeklyIndexICC::BloodQuickening)
                            break;

                        switch (data)
                        {
                            case IN_PROGRESS:
                                Events.ScheduleEvent(EVENT_UPDATE_EXECUTION_TIME, 60000);
                                BloodQuickeningMinutes = 30;
                                DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 1);
                                DoUpdateWorldState(WORLDSTATE_EXECUTION_TIME, BloodQuickeningMinutes);
                                break;
                            case DONE:
                                Events.CancelEvent(EVENT_UPDATE_EXECUTION_TIME);
                                BloodQuickeningMinutes = 0;
                                DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 0);
                                break;
                            default:
                                break;
                        }

                        BloodQuickeningState = data;
                        SaveToDB();
                        break;
                    }
                    case DATA_PUTRICIDE_TRAP:
                        if (!PutricideValveState[0] || !PutricideValveState[1])
                            return;
                        switch (PutricideTrapState = data)
                        {
                            case DONE:
                                SaveToDB();
                                HandleGameObject(PutricideEntranceGUID, true);
                            case NOT_STARTED:
                                HandleGameObject(PutricideCollisionGUID, true);
                                if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[0]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[1]))
                                    go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                break;
                            case IN_PROGRESS:
                                HandleGameObject(PutricideEntranceGUID, false);
                                HandleGameObject(PutricideCollisionGUID, false);
                                HandleGameObject(PutricideGateGUIDs[0], false);
                                HandleGameObject(PutricideGateGUIDs[1], false);
                                break;
                            default:
                                break;
                        }
                        break;
                    case DATA_ZONE_BUFF_STATUS:
                        ZoneBuffState = data;
                        break;
                    case DATA_ICC_BUFF:
                        IccBuffSelected = data;
                        for (auto&& ref : instance->GetPlayers())
                            if (Player* player = ref.GetSource())
                                ApplyIccBuff(player);
                        break;
                    case DATA_LIGHTS_HAMMER_NPCS_LEFT:
                        LightsHammerNPCsLeft = data;
                        break;
                    case DATA_ROTTING_FROST_GIANT_STATE:
                        if (RottingFrostGiantState == DONE)
                            return;
                        RottingFrostGiantState = (EncounterState)data;
                        if (RottingFrostGiantState == DONE)
                        {
                            if (Creature* lieutenant = instance->GetCreature(KorkronLieutenantGUID))
                                lieutenant->SetVisible(true);
                            SaveToDB();
                        }
                        break;
                    default:
                        break;
                }
            }

            void RemoveIccBuff(Unit* target)
            {
                for (auto&& buff : ICCBuffData)
                {
                    target->RemoveAurasDueToSpell(buff.second.first);
                    target->RemoveAurasDueToSpell(buff.second.second);
                }
            }

            void ApplyIccBuff(Unit* target)
            {
                RemoveIccBuff(target);

                bool alliance = false;
                if (target->GetTypeId() == TYPEID_PLAYER)
                    alliance = target->ToPlayer()->GetTeam() == ALLIANCE;
                else if (Player* owner = target->GetCharmerOrOwnerPlayerOrPlayerItself())
                    alliance = owner->GetTeam() == ALLIANCE;
                else
                    return;

                auto itr = ICCBuffData.find(IccBuffSelected);
                if (itr == ICCBuffData.end())
                    return;

                target->CastSpell(target, alliance ? itr->second.first : itr->second.second, true);
            }

            bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target, uint32 miscvalue1) override
            {
                switch (criteria_id)
                {
                    case CRITERIA_BONED_10N:
                    case CRITERIA_BONED_25N:
                    case CRITERIA_BONED_10H:
                    case CRITERIA_BONED_25H:
                        return IsBonedEligible;
                    case CRITERIA_DANCES_WITH_OOZES_10N:
                    case CRITERIA_DANCES_WITH_OOZES_25N:
                    case CRITERIA_DANCES_WITH_OOZES_10H:
                    case CRITERIA_DANCES_WITH_OOZES_25H:
                        return IsOozeDanceEligible;
                    case CRITERIA_NAUSEA_10N:
                    case CRITERIA_NAUSEA_25N:
                    case CRITERIA_NAUSEA_10H:
                    case CRITERIA_NAUSEA_25H:
                        return IsNauseaEligible;
                    case CRITERIA_ORB_WHISPERER_10N:
                    case CRITERIA_ORB_WHISPERER_25N:
                    case CRITERIA_ORB_WHISPERER_10H:
                    case CRITERIA_ORB_WHISPERER_25H:
                        return IsOrbWhispererEligible;
                    // Only one criteria for both modes, need to do it like this
                    case CRITERIA_KILL_LANA_THEL_10M:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_10N:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_10V:
                        return instance->ToInstanceMap()->GetMaxPlayers() == 10;
                    case CRITERIA_KILL_LANA_THEL_25M:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_25N:
                    case CRITERIA_ONCE_BITTEN_TWICE_SHY_25V:
                        return instance->Is25ManRaid();
                    default:
                        break;
                }

                //return InstanceScript::CheckAchievementCriteriaMeet(criteria_id, source, target, miscvalue1);
                return false;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = nullptr) const override
            {
                if (player && player->IsGameMaster())
                    return true;

                switch (bossId)
                {
                    case DATA_THE_LICH_KING:
                        if (!CheckPlagueworks(bossId))
                            return false;
                        if (!CheckCrimsonHalls(bossId))
                            return false;
                        if (!CheckFrostwingHalls(bossId))
                            return false;
                        break;
                    case DATA_SINDRAGOSA:
                    case DATA_VALITHRIA_DREAMWALKER:
                        if (!CheckFrostwingHalls(bossId))
                            return false;
                        break;
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                    case DATA_BLOOD_PRINCE_COUNCIL:
                        if (!CheckCrimsonHalls(bossId))
                            return false;
                        break;
                    case DATA_FESTERGUT:
                    case DATA_ROTFACE:
                    case DATA_PROFESSOR_PUTRICIDE:
                        if (!CheckPlagueworks(bossId))
                            return false;
                        break;
                    default:
                        break;
                }

                if (!CheckLowerSpire(bossId))
                    return false;

                return true;
            }

            bool CheckPlagueworks(uint32 bossId) const
            {
                switch (bossId)
                {
                    case DATA_THE_LICH_KING:
                        if (GetBossState(DATA_PROFESSOR_PUTRICIDE) != DONE)
                            return false;
                        // no break
                    case DATA_PROFESSOR_PUTRICIDE:
                        if (GetBossState(DATA_FESTERGUT) != DONE || GetBossState(DATA_ROTFACE) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckCrimsonHalls(uint32 bossId) const
            {
                switch (bossId)
                {
                    case DATA_THE_LICH_KING:
                        if (GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) != DONE)
                            return false;
                        // no break
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                        if (GetBossState(DATA_BLOOD_PRINCE_COUNCIL) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckFrostwingHalls(uint32 bossId) const
            {
                switch (bossId)
                {
                    case DATA_THE_LICH_KING:
                        if (GetBossState(DATA_SINDRAGOSA) != DONE)
                            return false;
                        // no break
                    case DATA_SINDRAGOSA:
                        if (GetBossState(DATA_VALITHRIA_DREAMWALKER) != DONE)
                            return false;
                        // no break
                    //case DATA_VALITHRIA_DREAMWALKER:
                    //    if (GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) != DONE)
                    //        return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            bool CheckLowerSpire(uint32 bossId) const
            {
                switch (bossId)
                {
                    case DATA_THE_LICH_KING:
                    case DATA_SINDRAGOSA:
                    case DATA_VALITHRIA_DREAMWALKER:
                    case DATA_BLOOD_QUEEN_LANA_THEL:
                    case DATA_PROFESSOR_PUTRICIDE:
                    case DATA_BLOOD_PRINCE_COUNCIL:
                    case DATA_ROTFACE:
                    case DATA_FESTERGUT:
                        if (GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
                            return false;
                        // no break
                    // case DATA_DEATHBRINGER_SAURFANG:
                    //     if (GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                    //         return false;
                        // no break
                    case DATA_GUNSHIP_EVENT: // todo: finish gunship battle
                        if (GetBossState(DATA_LADY_DEATHWHISPER) != DONE)
                            return false;
                        // no break
                    case DATA_LADY_DEATHWHISPER:
                        if (GetBossState(DATA_LORD_MARROWGAR) != DONE)
                            return false;
                        // no break
                    case DATA_LORD_MARROWGAR:
                    default:
                        break;
                }

                return true;
            }

            bool CheckLichKingAvailability() const
            {
                if (GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && GetBossState(DATA_SINDRAGOSA) == DONE)
                {
                    if (GameObject* teleporter = instance->GetGameObject(ScourgeTransporterTheLickKingGUID))
                    {
                        teleporter->SetGoState(GO_STATE_ACTIVE);

                        std::list<Creature*> stalkers;
                        GetCreatureListWithEntryInGrid(stalkers, teleporter, NPC_INVISIBLE_STALKER, 100.0f);
                        if (stalkers.empty())
                            return true;

                        stalkers.sort(Trinity::ObjectDistanceOrderPred(teleporter));
                        stalkers.front()->CastSpell((Unit*)nullptr, SPELL_ARTHAS_TELEPORTER_CEREMONY, false);
                        stalkers.pop_front();
                        for (std::list<Creature*>::iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                            (*itr)->AI()->Reset();
                    }
                    return true;
                }
                return false;
            }

            void PickTeamInInstanceIfNeeded()
            {
                if (!TeamInInstance)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            TeamInInstance = player->GetTeam();
                }
            }

            void PickWeeklyQuestIfNeeded()
            {
                // deprecated in mop
                /*if (WeeklyIndex == WeeklyIndexICC::None)
                {
                    WeeklyIndex = RAND(WeeklyIndexICC::Deprogramming, WeeklyIndexICC::SecuringTheRamparts, WeeklyIndexICC::ResidueRendezvous, WeeklyIndexICC::BloodQuickening, WeeklyIndexICC::RespiteForATormentedSoul);
                    SaveToDB();
                }*/
            }

            bool IsEncounterInProgress() const override
            {
                return InstanceScript::IsEncounterInProgress() || (RottingFrostGiantState == IN_PROGRESS);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "I C " << GetBossSaveData() << HeroicAttempts << " "
                    << ColdflameJetsState << " " << BloodQuickeningState << " " << BloodQuickeningMinutes << " " << (PutricideValveState[0] ? 1 : 0) << " " << (PutricideValveState[1] ? 1 : 0) << " " << PutricideTrapState << " " << (TrapsRandomized ? 1 : 0) << " " << (LightsHammerNPCsLeft ? 1 : 0) << " " << (uint32)WeeklyIndex << " " << (uint32)RottingFrostGiantState;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* str) override
            {
                if (!str)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(str);

                char dataHead1, dataHead2;

                std::istringstream loadStream(str);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'I' && dataHead2 == 'C')
                {
                    for (uint32 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    loadStream >> HeroicAttempts;

                    uint32 temp = 0;
                    // Clearing temp each time in case of old instances that don't have this data saved yet
                    temp = 0; loadStream >> temp; ColdflameJetsState = temp ? DONE : NOT_STARTED;
                    temp = 0; loadStream >> temp; BloodQuickeningState = temp ? DONE : NOT_STARTED; // DONE means finished (not success/fail)
                    temp = 0; loadStream >> BloodQuickeningMinutes;
                    temp = 0; loadStream >> temp; PutricideValveState[0] = temp;
                    temp = 0; loadStream >> temp; PutricideValveState[1] = temp;
                    temp = 0; loadStream >> temp; PutricideTrapState = temp ? DONE : NOT_STARTED;
                    temp = 0; loadStream >> temp; TrapsRandomized = temp;
                    temp = 0; loadStream >> temp; LightsHammerNPCsLeft = temp || GetBossState(DATA_LORD_MARROWGAR);
                    temp = 0; loadStream >> temp; WeeklyIndex = (WeeklyIndexICC)temp;
                    temp = 0; loadStream >> temp; RottingFrostGiantState = temp == IN_PROGRESS ? NOT_STARTED : (EncounterState)temp;
                    PickWeeklyQuestIfNeeded();
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            void Update(uint32 diff) override
            {
                Events.Update(diff);

                while (uint32 eventId = Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UPDATE_EXECUTION_TIME:
                        {
                            --BloodQuickeningMinutes;
                            if (BloodQuickeningMinutes)
                            {
                                Events.ScheduleEvent(EVENT_UPDATE_EXECUTION_TIME, 60000);
                                DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 1);
                                DoUpdateWorldState(WORLDSTATE_EXECUTION_TIME, BloodQuickeningMinutes);
                            }
                            else
                            {
                                BloodQuickeningState = DONE;
                                DoUpdateWorldState(WORLDSTATE_SHOW_TIMER, 0);
                                if (Creature* bq = instance->GetCreature(BloodQueenLanaThelGUID))
                                    bq->AI()->DoAction(ACTION_KILL_MINCHAR);
                            }
                            SaveToDB();
                            break;
                        }
                        case EVENT_QUAKE_SHATTER:
                        {
                            if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                                platform->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
                            if (GameObject* edge = instance->GetGameObject(FrozenThroneEdgeGUID))
                                edge->SetGoState(GO_STATE_ACTIVE);
                            if (GameObject* wind = instance->GetGameObject(FrozenThroneWindGUID))
                                wind->SetGoState(GO_STATE_READY);
                            if (GameObject* warning = instance->GetGameObject(FrozenThroneWarningGUID))
                                warning->SetGoState(GO_STATE_READY);
                            if (Creature* theLichKing = instance->GetCreature(TheLichKingGUID))
                                theLichKing->AI()->DoAction(ACTION_RESTORE_LIGHT);
                            break;
                        }
                        case EVENT_REBUILD_PLATFORM:
                            if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                                platform->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
                            if (GameObject* edge = instance->GetGameObject(FrozenThroneEdgeGUID))
                                edge->SetGoState(GO_STATE_READY);
                            if (GameObject* wind = instance->GetGameObject(FrozenThroneWindGUID))
                                wind->SetGoState(GO_STATE_ACTIVE);
                            if (Creature* theLichKing = instance->GetCreature(TheLichKingGUID))
                                theLichKing->AI()->DoAction(ACTION_REBUILD_PLATFORM);
                            break;
                        case EVENT_RESPAWN_GUNSHIP:
                            SpawnGunship(true);
                            break;
                        case EVENT_TOGGLE_ELEVATOR:
                            // if (Transport* elevator = instance->GetTransport(LadyDeathwisperElevatorGUID)) // temp hack (elevators must be fixed)
                            //    elevator->EnableMovement(elevator->GetGoState() == GO_STATE_READY);
                            Events.ScheduleEvent(EVENT_TOGGLE_ELEVATOR, 20000);
                            break;
                        default:
                            break;
                    }
                }
            }

            void ProcessEvent(WorldObject* source, uint32 eventId) override
            {
                switch (eventId)
                {
                    case EVENT_ENEMY_GUNSHIP_DESPAWN:
                        if (GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                            source->AddObjectToRemoveList();
                        break;
                    case EVENT_ENEMY_GUNSHIP_COMBAT:
                        if (Creature* captain = source->FindNearestCreature(TeamInInstance == HORDE ? NPC_GB_HIGH_OVERLORD_SAURFANG : NPC_GB_MURADIN_BRONZEBEARD, 100.0f))
                            captain->AI()->DoAction(ACTION_ENEMY_GUNSHIP_TALK);
                        // no break;
                    case EVENT_PLAYERS_GUNSHIP_SPAWN:
                    case EVENT_PLAYERS_GUNSHIP_COMBAT:
                        if (GameObject* go = source->ToGameObject())
                            if (Transport* transport = go->ToTransport())
                                transport->EnableMovement(false);
                        break;
                    case EVENT_PLAYERS_GUNSHIP_SAURFANG:
                    {
                        if (Creature* captain = source->FindNearestCreature(TeamInInstance == HORDE ? NPC_GB_HIGH_OVERLORD_SAURFANG : NPC_GB_MURADIN_BRONZEBEARD, 100.0f))
                            captain->AI()->DoAction(ACTION_EXIT_SHIP);
                        if (GameObject* go = source->ToGameObject())
                            if (Transport* transport = go->ToTransport())
                                transport->EnableMovement(false);
                        break;
                    }
                    case EVENT_LICH_KING_WHISPER:
                        instance->LoadGrid(-123.963997f, 2211.560059f);
                        if (source && source->GetTypeId() == TYPEID_PLAYER)
                            if (Creature* lich = instance->GetCreature(LightsHammerTheLichKingGUID))
                                lich->AI()->Talk(5, source);
                        break;
                    case EVENT_OOZE_RELEASE_VALVE:
                        if (PutricideValveState[1])
                            return;
                        PutricideValveState[1] = true;
                        if (PutricideValveState[0])
                        {
                            HandleGameObject(PutricideCollisionGUID, true);
                            if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[0]))
                                go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                            if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[1]))
                                go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        }
                        else
                            HandleGameObject(PutricideGateGUIDs[1], false);
                        HandleGameObject(PutricidePipeGUIDs[1], true);
                        if (GameObject* go = instance->GetGameObject(PutricideValveGUIDs[1]))
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        SaveToDB();
                        break;
                    case EVENT_GAS_RELEASE_VALVE:
                        if (PutricideValveState[0])
                            return;
                        PutricideValveState[0] = true;
                        if (PutricideValveState[1])
                        {
                            HandleGameObject(PutricideCollisionGUID, true);
                            if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[0]))
                                go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                            if (GameObject* go = instance->GetGameObject(PutricideGateGUIDs[1]))
                                go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        }
                        else
                            HandleGameObject(PutricideGateGUIDs[0], false);
                        HandleGameObject(PutricidePipeGUIDs[0], true);
                        if (GameObject* go = instance->GetGameObject(PutricideValveGUIDs[0]))
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        SaveToDB();
                        break;
                    case EVENT_QUAKE:
                        if (GameObject* warning = instance->GetGameObject(FrozenThroneWarningGUID))
                            warning->SetGoState(GO_STATE_ACTIVE);
                        Events.ScheduleEvent(EVENT_QUAKE_SHATTER, 5000);
                        break;
                    case EVENT_SECOND_REMORSELESS_WINTER:
                        if (GameObject* platform = instance->GetGameObject(ArthasPlatformGUID))
                        {
                            platform->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                            Events.ScheduleEvent(EVENT_REBUILD_PLATFORM, 1500);
                        }
                        break;
                    case EVENT_TELEPORT_TO_FROSMOURNE: // Harvest Soul (normal mode)
                        if (Creature* terenas = instance->SummonCreature(NPC_TERENAS_MENETHIL_FROSTMOURNE, TerenasSpawn, nullptr, Milliseconds(63000)))
                        {
                            terenas->AI()->DoAction(ACTION_FROSTMOURNE_INTRO);
                            std::list<Creature*> triggers;
                            GetCreatureListWithEntryInGrid(triggers, terenas, NPC_WORLD_TRIGGER_INFINITE_AOI, 100.0f);
                            if (!triggers.empty())
                            {
                                triggers.sort(Trinity::ObjectDistanceOrderPred(terenas, false));
                                Unit* visual = triggers.front();
                                visual->CastSpell(visual, SPELL_FROSTMOURNE_TELEPORT_VISUAL, true);
                            }

                            if (Creature* warden = instance->SummonCreature(NPC_SPIRIT_WARDEN, SpiritWardenSpawn, nullptr, Milliseconds(63000)))
                            {
                                terenas->AI()->AttackStart(warden);
                                warden->AddThreat(terenas, 300000.0f);
                            }
                        }
                        break;
                }
            }

            void SpawnGunship(bool respawn = false)
            {
                if (isPrepared && !respawn)
                    return;

                if (GetBossState(DATA_GUNSHIP_EVENT) != DONE && !respawn)
                {
                    // Init boss state, otherwise it will be TO_BE_DECIDED and the first call to InstanceScript::SetBossState will return false
                    SetBossState(DATA_GUNSHIP_EVENT, NOT_STARTED);
                }

                sTransportMgr->CreateTransport(TeamInInstance == ALLIANCE ? GO_THE_SKYBREAKER_ALLIANCE_ICC : GO_ORGRIM_S_HAMMER_HORDE_ICC, 0, instance);

                isPrepared = true;
            }

        protected:
            EventMap Events;
            ObjectGuid LightsHammerTheLichKingGUID;
            ObjectGuid LightsHammerSaurfangMuradinGUID;
            std::vector<ObjectGuid> LightsHammerKorkronGeneralAllianceCommanderGUIDs;
            ObjectGuid LordMarrowgarGUID;
            ObjectGuid LadyDeahtwhisperGUID;
            ObjectGuid LadyDeathwisperElevatorGUID;
            // Gunship battle
            uint32 FirstSquadState;
            uint32 SecondSquadState;
            ObjectGuid SkybreakerBossGUID;
            ObjectGuid OrgrimmarBossGUID;
            ObjectGuid DeathbringerSaurfangGbGUID;
            ObjectGuid MuradinBronzebeardGbGUID;
            ObjectGuid GbBattleMageGUID;
            ObjectGuid GunshipArmoryGUID;
            ObjectGuid DeathbringerSaurfangGUID;
            ObjectGuid DeathbringerSaurfangDoorGUID;
            ObjectGuid DeathbringerSaurfangEventGUID;   // Muradin Bronzebeard or High Overlord Saurfang
            ObjectGuid DeathbringersCacheGUID;
            ObjectGuid PlagueSigilGUID;
            ObjectGuid BloodwingDoor;
            ObjectGuid FrostwingDoor;
            ObjectGuid BloodwingSigilGUID;
            ObjectGuid FrostwingSigilGUID;
            ObjectGuid PutricideValveGUIDs[2];
            bool PutricideValveState[2];
            ObjectGuid PutricidePipeGUIDs[2];
            ObjectGuid PutricideGateGUIDs[2];
            ObjectGuid PutricideCollisionGUID;
            ObjectGuid FestergutGUID;
            ObjectGuid RotfaceGUID;
            ObjectGuid ProfessorPutricideGUID;
            ObjectGuid PutricideTableGUID;
            ObjectGuid BloodCouncilGUIDs[3];
            ObjectGuid BloodCouncilControllerGUID;
            ObjectGuid BloodQueenLanaThelCouncilGUID;
            ObjectGuid BloodQueenLanaThelGUID;
            ObjectGuid CrokScourgebaneGUID;
            ObjectGuid CrokCaptainGUIDs[4];
            ObjectGuid SisterSvalnaGUID;
            ObjectGuid ValithriaDreamwalkerGUID;
            ObjectGuid ValithriaLichKingGUID;
            ObjectGuid ValithriaTriggerGUID;
            ObjectGuid SindragosaGUID;
            ObjectGuid SindragosaDoorGUID;
            ObjectGuid SpinestalkerGUID;
            ObjectGuid RimefangGUID;
            ObjectGuid TheLichKingGUID;
            ObjectGuid HighlordTirionFordringGUID;
            ObjectGuid TerenasMenethilGUID;
            ObjectGuid ArthasPlatformGUID;
            ObjectGuid ArthasPrecipiceGUID;
            ObjectGuid FrozenThroneEdgeGUID;
            ObjectGuid FrozenThroneWindGUID;
            ObjectGuid FrozenThroneWarningGUID;
            ObjectGuid FrozenBolvarGUID;
            ObjectGuid PillarsChainedGUID;
            ObjectGuid PillarsUnchainedGUID;
            ObjectGuid FrostyEdgeInner;
            ObjectGuid FrostyEdgeOuter;
            ObjectGuid EdgeDestroyWarning;
            ObjectGuid FrozenLavaman;
            ObjectGuid LavamanPillars;
            ObjectGuid GunShipControllerGUID;
            ObjectGuid GBMuradinGUID;
            ObjectGuid GBSaurfangGUID;
            ObjectGuid GBSkybreakerGUID;
            ObjectGuid GBOgrimsHammerGUID;
            ObjectGuid SindragosasWardGUID;
            ObjectGuid UnstableExperimentStalkerRedGUID;
            ObjectGuid UnstableExperimentStalkerGreenGUID;
            ObjectGuid PutricideEntranceGUID;
            ObjectGuid CrimsonHallDoorGUID;
            uint32 SpireSquadState;
            uint32 TeamInInstance;
            uint32 ColdflameJetsState;
            uint32 FrostwyrmCount;
            uint32 SpinestalkerTrashCount;
            uint32 RimefangTrashCount;
            uint32 BloodQuickeningState;
            uint32 HeroicAttempts;
            uint16 BloodQuickeningMinutes;
            uint32 PutricideTrapState;
            uint8 IccBuffSelected;
            bool IsBonedEligible;
            bool IsOozeDanceEligible;
            bool IsNauseaEligible;
            bool IsOrbWhispererEligible;
            bool ZoneBuffState;
            bool isPrepared;
            std::map<uint32, std::list<ObjectGuid>> TrapGUIDs;
            bool TrapsRandomized = false;
            bool TrapsDeleted = false;
            bool LightsHammerNPCsLeft = false;
            bool QuestNPCsSpawned = false;
            WeeklyIndexICC WeeklyIndex = WeeklyIndexICC::None;
            bool ValithriaQuestgiveSpawned = false;
            ObjectGuid KorkronLieutenantGUID;
            EncounterState RottingFrostGiantState = NOT_STARTED;
            ObjectGuid ScourgeTransporterLightsHammerGUID;
            ObjectGuid ScourgeTransporterDeathbringersRiseGUID;
            ObjectGuid ScourgeTransporterUpperSpireGUID;
            ObjectGuid ScourgeTransporterTheLickKingGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_icecrown_citadel_InstanceMapScript(map);
        }
};

void AddSC_instance_icecrown_citadel()
{
    new instance_icecrown_citadel();
}
