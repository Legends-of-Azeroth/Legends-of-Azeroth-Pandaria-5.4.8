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
#include "InstanceScript.h"
#include "Vehicle.h"
#include "Transport.h"
#include "Player.h"
#include "ulduar.h"

static std::vector<DoorData> const doorData =
{
    { GO_LEVIATHAN_DOOR_BACK,            BOSS_LEVIATHAN,        DOOR_TYPE_ROOM,         BOUNDARY_S      },
    { GO_XT_002_DOOR,                    BOSS_XT002,            DOOR_TYPE_ROOM,         BOUNDARY_S      },
    { GO_IRON_COUNCIL_ENTRANCE,          BOSS_ASSEMBLY_OF_IRON, DOOR_TYPE_ROOM,         BOUNDARY_N      },
    { GO_ARCHIVUM_DOOR,                  BOSS_ASSEMBLY_OF_IRON, DOOR_TYPE_PASSAGE,      BOUNDARY_S      },
    { GO_KOLOGARN_DOOR,                  BOSS_KOLOGARN,         DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
    { GO_HODIR_ENTRANCE,                 BOSS_HODIR,            DOOR_TYPE_ROOM,         BOUNDARY_E      },
    { GO_HODIR_DOOR,                     BOSS_HODIR,            DOOR_TYPE_PASSAGE,      BOUNDARY_NONE   },
    { GO_HODIR_ICE_DOOR,                 BOSS_HODIR,            DOOR_TYPE_PASSAGE,      BOUNDARY_W      },
    { GO_THORIM_LIGHTNING_FIELD,         BOSS_THORIM,           DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
    { GO_MIMIRON_DOOR_1,                 BOSS_MIMIRON,          DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
    { GO_MIMIRON_DOOR_2,                 BOSS_MIMIRON,          DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
    { GO_MIMIRON_DOOR_3,                 BOSS_MIMIRON,          DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
    { GO_VEZAX_DOOR,                     BOSS_VEZAX,            DOOR_TYPE_PASSAGE,      BOUNDARY_E      },
    { GO_YOGG_SARON_DOOR,                BOSS_YOGG_SARON,       DOOR_TYPE_ROOM,         BOUNDARY_S      },
    { GO_DOODAD_UL_SIGILDOOR_03,         BOSS_ALGALON,          DOOR_TYPE_ROOM,         BOUNDARY_W      },
    { GO_DOODAD_UL_UNIVERSEFLOOR_01,     BOSS_ALGALON,          DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
    { GO_DOODAD_UL_UNIVERSEFLOOR_02,     BOSS_ALGALON,          DOOR_TYPE_SPAWN_HOLE,   BOUNDARY_NONE   },
    { GO_DOODAD_UL_UNIVERSEGLOBE01,      BOSS_ALGALON,          DOOR_TYPE_SPAWN_HOLE,   BOUNDARY_NONE   },
    { GO_DOODAD_UL_ULDUAR_TRAPDOOR_03,   BOSS_ALGALON,          DOOR_TYPE_SPAWN_HOLE,   BOUNDARY_NONE   },
    { 0,                                 0,                     DOOR_TYPE_ROOM,         BOUNDARY_NONE   },
};

enum BronzebeardRadioTexts
{
    SAY_ORBITAL_SUPPORT     = 0,
    SAY_TOWERS              = 1,
    SAY_TOWER_OF_FROST      = 2,
    SAY_TOWER_OF_LIFE       = 3,
    SAY_TOWER_OF_FLAME      = 4,
    SAY_TOWER_OF_STORM      = 5,
    SAY_MISSILE_GENERATOR   = 6, // unknown
    SAY_REPAIR_STATION      = 7,
    SAY_FLAME_LEVIATHAN_1   = 8,
    SAY_FLAME_LEVIATHAN_2   = 9,
    SAY_FLAME_LEVIATHAN_3   = 10,
    SAY_RADIO_START         = 11,
};

class instance_ulduar : public InstanceMapScript
{
    public:
        instance_ulduar() : InstanceMapScript("instance_ulduar", 603) { }

        struct instance_ulduarinstanceMapScript : public InstanceScript
        {
            instance_ulduarinstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

            // Creatures
            ObjectGuid LoreKeeperOfNorgannonGUID;
            ObjectGuid BronzebeardRadioGUID;
            ObjectGuid LeviathanGUID;
            ObjectGuid OrbitalSupportGUID;
            ObjectGuid IgnisGUID;
            ObjectGuid RazorscaleGUID;
            ObjectGuid RazorscaleController;
            ObjectGuid ExpeditionCommanderGUID;
            ObjectGuid XT002GUID;
            ObjectGuid XTToyPileGUIDs[4];
            ObjectGuid AssemblyGUIDs[3];
            ObjectGuid KologarnGUID;
            ObjectGuid AuriayaGUID;
            ObjectGuid HodirGUID;
            ObjectGuid ThorimGUID;
            ObjectGuid ThorimCtrlGUID;
            ObjectGuid RunicColossusGUID;
            ObjectGuid RuneGiantGUID;
            ObjectGuid FreyaGUID;
            ObjectGuid ElderGUIDs[3];
            ObjectGuid MimironGUID;
            ObjectGuid LeviathanMKIIGUID;
            ObjectGuid VX001GUID;
            ObjectGuid AerialUnitGUID;
            ObjectGuid VezaxGUID;
            ObjectGuid YoggSaronGUID;
            ObjectGuid KeeperGUIDs[4];
            ObjectGuid VoiceOfYoggSaronGUID;
            ObjectGuid SaraGUID;
            ObjectGuid BrainOfYoggSaronGUID;
            ObjectGuid AlgalonGUID;
            ObjectGuid BrannBronzebeardAlgGUID;

            // GameObjects
            ObjectGuid LeviathanGateGUID;
            ObjectGuid LeviDoorGUID;
            ObjectGuid TowerofStormsCrystalGUID;
            ObjectGuid TowerofFlamesCrystalGUID;
            ObjectGuid TowerofFrostCrystalGUID;
            ObjectGuid TowerofLifeCrystalGUID;
            ObjectGuid RazorHarpoonGUIDs[4];
            ObjectGuid KologarnChestGUID;
            ObjectGuid KologarnBridgeGUID;
            ObjectGuid ActivateTramGUID;
            ObjectGuid CallTramLastGUID;
            ObjectGuid CallTramFirstGUID;
            ObjectGuid MimironTrainGUID;
            ObjectGuid MimironElevatorGUID;
            ObjectGuid MimironChestGUID;
            ObjectGuid MimironChestHardGUID;
            ObjectGuid HodirChestGUID;
            ObjectGuid HodirChestHardGUID;
            uint32 HodirRareCacheData;
            ObjectGuid RunicDoorGUID;
            ObjectGuid StoneDoorGUID;
            ObjectGuid ThorimChestGUID;
            ObjectGuid ThorimChestHardGUID;
            ObjectGuid ThorimDarkIronPortCullisGUID;
            ObjectGuid AlgalonSigilDoorGUID[3];
            ObjectGuid BrainRoomDoorGUIDs[3];
            ObjectGuid AlgalonFloorGUID[2];
            ObjectGuid AlgalonUniverseGUID;
            ObjectGuid AlgalonTrapdoorGUID;
            ObjectGuid GiftOfTheObserverGUID;

            // Teleports
            ObjectGuid TeleporterBaseCampGUID;
            ObjectGuid TeleporterGroundsGUID;
            ObjectGuid TeleporterForgeGUID;
            ObjectGuid TeleporterScrapyardGUID;
            ObjectGuid TeleporterAntchemberGUID;
            ObjectGuid TeleporterShatteredGUID;
            ObjectGuid TeleporterConservatoryGUID;
            ObjectGuid TeleporterImaginationGUID;
            ObjectGuid TeleporterDescentGUID;

            // Miscellaneous
            uint32 TeamInInstance;
            uint32 uiUpdateTimer;
            uint32 ColossusData;
            uint32 TramData;
            time_t BrightleafDeath;
            time_t StonebarkDeath;
            time_t IronbranchDeath;
            uint8 elderCount;
            uint8 illusion;
            uint8 keepersCount;
            bool Unbroken;
            bool LeviathanMove;
            bool conSpeedAtory;
            bool IsDriveMeCrazyEligible;
            bool LeviHardMod;
            bool ShieldDisabled;
            bool ThorimHardMode;
            bool MimironHardMode;

            void Initialize() override
            {
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                TeleporterBaseCampGUID = ObjectGuid::Empty;
                TeleporterGroundsGUID = ObjectGuid::Empty;
                TeleporterForgeGUID = ObjectGuid::Empty;
                TeleporterScrapyardGUID = ObjectGuid::Empty;
                TeleporterAntchemberGUID = ObjectGuid::Empty;
                TeleporterShatteredGUID = ObjectGuid::Empty;
                TeleporterConservatoryGUID = ObjectGuid::Empty;
                TeleporterImaginationGUID = ObjectGuid::Empty;
                TeleporterDescentGUID = ObjectGuid::Empty;
                LeviathanGUID = ObjectGuid::Empty;
                OrbitalSupportGUID = ObjectGuid::Empty;
                IgnisGUID = ObjectGuid::Empty;
                RazorscaleGUID = ObjectGuid::Empty;
                RazorscaleController             = ObjectGuid::Empty;
                ExpeditionCommanderGUID = ObjectGuid::Empty;
                XT002GUID                        = ObjectGuid::Empty;
                KologarnGUID = ObjectGuid::Empty;
                AuriayaGUID = ObjectGuid::Empty;
                MimironGUID = ObjectGuid::Empty;
                HodirGUID = ObjectGuid::Empty;
                ThorimGUID = ObjectGuid::Empty;
                FreyaGUID = ObjectGuid::Empty;
                VezaxGUID = ObjectGuid::Empty;
                YoggSaronGUID = ObjectGuid::Empty;
                VoiceOfYoggSaronGUID = ObjectGuid::Empty;
                SaraGUID = ObjectGuid::Empty;
                BrainOfYoggSaronGUID = ObjectGuid::Empty;
                AlgalonGUID = ObjectGuid::Empty;
                KologarnBridgeGUID = ObjectGuid::Empty;
                ThorimCtrlGUID = ObjectGuid::Empty;
                RunicColossusGUID = ObjectGuid::Empty;
                RuneGiantGUID = ObjectGuid::Empty;
                RunicDoorGUID = ObjectGuid::Empty;
                StoneDoorGUID = ObjectGuid::Empty;
                ThorimChestGUID = ObjectGuid::Empty;
                ThorimDarkIronPortCullisGUID = ObjectGuid::Empty;
                LeviathanGateGUID = ObjectGuid::Empty;
                AlgalonUniverseGUID = ObjectGuid::Empty;
                AlgalonTrapdoorGUID = ObjectGuid::Empty;
                BrannBronzebeardAlgGUID = ObjectGuid::Empty;
                GiftOfTheObserverGUID = ObjectGuid::Empty;
                MimironTrainGUID = ObjectGuid::Empty;
                LeviathanMKIIGUID = ObjectGuid::Empty;
                VX001GUID                        = ObjectGuid::Empty;
                AerialUnitGUID = ObjectGuid::Empty;
                MimironElevatorGUID = ObjectGuid::Empty;
                TramData                         = 0;
                _algalonTimer                    = 61;
                _maxArmorItemLevel               = 0;
                _maxWeaponItemLevel              = 0;
                TeamInInstance                   = 0;
                HodirRareCacheData               = 0;
                ColossusData                     = 0;
                elderCount                       = 0;
                illusion                         = 0;
                keepersCount                     = 0;
                BrightleafDeath                  = 0;
                StonebarkDeath                   = 0;
                IronbranchDeath                  = 0;
                TowerofStormsCrystalGUID = ObjectGuid::Empty;
                TowerofFlamesCrystalGUID = ObjectGuid::Empty;
                TowerofFrostCrystalGUID = ObjectGuid::Empty;
                TowerofLifeCrystalGUID = ObjectGuid::Empty;
                LeviDoorGUID = ObjectGuid::Empty;
                KologarnChestGUID = ObjectGuid::Empty;
                MimironChestGUID = ObjectGuid::Empty;
                MimironChestHardGUID = ObjectGuid::Empty;
                HodirChestGUID = ObjectGuid::Empty;
                HodirChestHardGUID = ObjectGuid::Empty;
                ThorimChestGUID = ObjectGuid::Empty;
                ThorimChestHardGUID = ObjectGuid::Empty;
                ThorimHardMode                   = false;
                MimironHardMode                  = false;
                uiUpdateTimer                    = 3000;
                conSpeedAtory                    = false;
                Unbroken                         = true;
                IsDriveMeCrazyEligible           = false;
                _algalonSummoned                 = false;
                _summonAlgalon                   = false;
                 _CoUAchivePlayerDeathMask       = 0;
                LeviathanMove                    = false;
                LeviHardMod                      = false;
                ShieldDisabled                   = false;

                memset(AlgalonSigilDoorGUID, 0, sizeof(AlgalonSigilDoorGUID));
                memset(AlgalonFloorGUID, 0, sizeof(AlgalonFloorGUID));
                memset(XTToyPileGUIDs, 0, sizeof(XTToyPileGUIDs));
                memset(AssemblyGUIDs, 0, sizeof(AssemblyGUIDs));
                memset(RazorHarpoonGUIDs, 0, sizeof(RazorHarpoonGUIDs));
                memset(KeeperGUIDs, 0, sizeof(KeeperGUIDs));
                memset(BrainRoomDoorGUIDs, 0, sizeof(BrainRoomDoorGUIDs));
                memset(ElderGUIDs, 0, sizeof(ElderGUIDs));
                memset(_summonObservationRingKeeper, false, sizeof(_summonObservationRingKeeper));
                memset(_summonYSKeeper, false, sizeof(_summonYSKeeper));
            }

            void FillInitialWorldStates(WorldStateBuilder& builder) override
            {
                builder.AppendState(uint32(WORLD_STATE_LEVIATHAN),             uint32(GetBossState(BOSS_LEVIATHAN) == IN_PROGRESS));
                builder.AppendState(uint32(WORLD_STATE_IGNIS),                 uint32(GetBossState(BOSS_IGNIS) == IN_PROGRESS));
                builder.AppendState(uint32(WORLD_STATE_RAZORSCALE),            uint32(GetBossState(BOSS_RAZORSCALE) == IN_PROGRESS));
                builder.AppendState(uint32(WORLD_STATE_XT002),                 uint32(GetBossState(BOSS_XT002) == IN_PROGRESS));
                builder.AppendState(uint32(WORLD_STATE_MIMIRON),               uint32(GetBossState(BOSS_MIMIRON) == DONE));
                builder.AppendState(uint32(WORLD_STATE_YOGG_SARON),            uint32(GetBossState(BOSS_YOGG_SARON) == DONE));
                builder.AppendState(uint32(WORLD_STATE_ALGALON),               uint32(GetBossState(BOSS_ALGALON) == IN_PROGRESS));
                builder.AppendState(uint32(WORLD_STATE_ALGALON_TIMER_ENABLED), uint32(_algalonTimer && _algalonTimer <= 60));
                builder.AppendState(uint32(WORLD_STATE_ALGALON_DESPAWN_TIMER), uint32(std::min<uint32>(_algalonTimer, 60)));
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!LeviathanGUID || !OrbitalSupportGUID)
                {
                    instance->LoadGrid(-2125.778809f, 2658.954590f); // Bronzebeard Radio
                    instance->LoadGrid(267.936f, -41.6376f); // Orbital Support
                    instance->LoadGrid(364, -16); // make sure leviathan is loaded

                    // Load Storm Beacons
                    instance->LoadGrid(235.4430f,  364.50000f);
                    instance->LoadGrid(108.6300f,  361.27400f);
                    instance->LoadGrid( 84.2587f,  -80.51040f);
                    instance->LoadGrid(144.4240f, -285.08700f);
                    instance->LoadGrid(222.2900f, -100.59400f);
                    instance->LoadGrid(227.6860f,   33.21010f);
                    instance->LoadGrid(199.6980f,  192.85800f);
                    instance->LoadGrid(-18.0903f,  -81.32290f);
                    instance->LoadGrid(-17.0642f,   11.04510f);
                    instance->LoadGrid( 82.1840f,    9.73611f);
                    instance->LoadGrid( 36.5660f,  194.34700f);
                    instance->LoadGrid(114.2330f,  -34.45140f);
                    instance->LoadGrid(385.6270f,   44.96700f);
                    instance->LoadGrid(482.2730f,   46.18580f);
                    instance->LoadGrid(311.7620f,  -32.39760f);
                    instance->LoadGrid(384.3130f,  -99.53300f);
                    instance->LoadGrid(483.2570f,  -98.30380f);
                    instance->LoadGrid(481.0450f,  -29.44790f);
                    instance->LoadGrid(305.7190f, -286.34700f);

                    // Load Call Tram
                    instance->LoadGrid(2338.32f, 2565.08f);
                    instance->LoadGrid(2277.42f, 298.693f);
                }

                if (!TeamInInstance)
                    TeamInInstance = player->GetTeam();

                if (_summonAlgalon)
                {
                    _summonAlgalon = false;
                    TempSummon* algalon = instance->SummonCreature(NPC_ALGALON, AlgalonLandPos);
                    algalon->AI()->DoAction(_algalonTimer && _algalonTimer <= 60 ? ACTION_INIT_ALGALON : ACTION_INIT_ALGALON_FIRST);
                }
                
                // Keepers at Observation Ring
                if (GetBossState(BOSS_FREYA) == DONE && _summonObservationRingKeeper[0] && !KeeperGUIDs[0])
                {
                    _summonObservationRingKeeper[0] = false;
                    instance->SummonCreature(NPC_FREYA_OBSERVATION_RING, ObservationRingKeepersPos[0]);
                }
                if (GetBossState(BOSS_HODIR) == DONE && _summonObservationRingKeeper[1] && !KeeperGUIDs[1])
                {
                    _summonObservationRingKeeper[1] = false;
                    instance->SummonCreature(NPC_HODIR_OBSERVATION_RING, ObservationRingKeepersPos[1]);
                }
                if (GetBossState(BOSS_THORIM) == DONE && _summonObservationRingKeeper[2] && !KeeperGUIDs[2])
                {
                    _summonObservationRingKeeper[2] = false;
                    instance->SummonCreature(NPC_THORIM_OBSERVATION_RING, ObservationRingKeepersPos[2]);
                }
                if (GetBossState(BOSS_MIMIRON) == DONE && _summonObservationRingKeeper[3] && !KeeperGUIDs[3])
                {
                    _summonObservationRingKeeper[3] = false;
                    instance->SummonCreature(NPC_MIMIRON_OBSERVATION_RING, ObservationRingKeepersPos[3]);
                }

                // Keepers in Yogg-Saron's room
                if (_summonYSKeeper[0])
                    instance->SummonCreature(NPC_FREYA_YS, YSKeepersPos[0]);
                if (_summonYSKeeper[1])
                    instance->SummonCreature(NPC_HODIR_YS, YSKeepersPos[1]);
                if (_summonYSKeeper[2])
                    instance->SummonCreature(NPC_THORIM_YS, YSKeepersPos[2]);
                if (_summonYSKeeper[3])
                    instance->SummonCreature(NPC_MIMIRON_YS, YSKeepersPos[3]);
            }

            void OnPlayerLeave(Player* player) override
            {
                player->RemoveTimedAchievement(CRITERIA_START_TYPE_EVENT, 20951); // Flame Leviathan
                player->RemoveTimedAchievement(CRITERIA_START_TYPE_EVENT, 21001); // Yogg-Saron
                player->RemoveTimedAchievement(CRITERIA_START_TYPE_EVENT, 21027); // XT-002 Deconstructor
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!TeamInInstance)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            TeamInInstance = player->GetTeam();
                }

                switch (creature->GetEntry())
                {
                    case NPC_LOREKEEPER:
                        LoreKeeperOfNorgannonGUID = creature->GetGUID();
                        break;
                    case NPC_BRONZEBEARD_RADIO:
                        BronzebeardRadioGUID = creature->GetGUID();
                        break;
                    // Leviathan
                    case NPC_LEVIATHAN:
                        LeviathanGUID = creature->GetGUID();
                        if (GetData(DATA_COLOSSUS) == 2 && (GetBossState(BOSS_LEVIATHAN) != DONE))
                            _events.ScheduleEvent(EVENT_FLAME_LEVIATHAN_4, 3000);
                        break;
                    case NPC_ORBITAL_SUPPORT:
                        OrbitalSupportGUID = creature->GetGUID();
                        break;
                    case NPC_CHOPPER:
                    case NPC_SALVAGED_DEMOLISHER:
                    case NPC_SALVAGED_SIEGE_ENGINE:
                    case NPC_SALVAGED_SIEGE_TURRET:
                    case NPC_SALVAGED_DEMOLISHER_MECHANIC_SEAT:
                        if (GetBossState(BOSS_LEVIATHAN) == DONE)
                        {
                            creature->DespawnOrUnsummon(1);
                            break;
                        }
                        vehiclesGUIDs.push_back(creature->GetGUID());
                        if (GetBossState(BOSS_LEVIATHAN) != DONE)
                            creature->Respawn();
                        break;
                    case NPC_IGNIS:
                        IgnisGUID = creature->GetGUID();
                        break;
                    case NPC_MECHANOLIFT:
                    case NPC_ULDUAR_GAUNTLET_GENERATOR:
                    case NPC_ULDUAR_GAUNTLET_GENERATOR_SMALL:
                    case NPC_IRONWORK_CANNON:
                        creature->Respawn();
                        break;
                    // Razorscale
                    case NPC_RAZORSCALE:
                        RazorscaleGUID = creature->GetGUID();
                        break;
                    case NPC_RAZORSCALE_CONTROLLER:
                        RazorscaleController = creature->GetGUID();
                        break;
                    case NPC_EXPEDITION_COMMANDER:
                        ExpeditionCommanderGUID = creature->GetGUID();
                        break;

                    // XT-002 Deconstructor
                    case NPC_XT002:
                        XT002GUID = creature->GetGUID();
                        break;
                    case NPC_XT_TOY_PILE:
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            if (!XTToyPileGUIDs[i])
                            {
                                XTToyPileGUIDs[i] = creature->GetGUID();
                                break;
                            }
                        }
                        break;

                    // Assembly of Iron
                    case NPC_STEELBREAKER:
                        AssemblyGUIDs[0] = creature->GetGUID();
                        break;
                    case NPC_MOLGEIM:
                        AssemblyGUIDs[1] = creature->GetGUID();
                        break;
                    case NPC_BRUNDIR:
                        AssemblyGUIDs[2] = creature->GetGUID();
                        break;

                    // Kologarn
                    case NPC_KOLOGARN:
                        KologarnGUID = creature->GetGUID();
                        break;

                    // Auraya
                    case NPC_AURIAYA:
                        AuriayaGUID = creature->GetGUID();
                        break;

                    // Hodir
                    case NPC_HODIR:
                        HodirGUID = creature->GetGUID();
                        if (creature->isDead())
                            SetBossState(BOSS_HODIR, DONE);
                        break;
                    case NPC_EIVI_NIGHTFEATHER:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_TOR_GREYCLOUD, HORDE);
                        break;
                    case NPC_ELLIE_NIGHTFEATHER:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_KAR_GREYCLOUD, HORDE);
                        break;
                    case NPC_ELEMENTALIST_MAHFUUN:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_SPIRITWALKER_TARA, HORDE);
                        break;
                    case NPC_ELEMENTALIST_AVUUN:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_SPIRITWALKER_YONA, HORDE);
                        break;
                    case NPC_MISSY_FLAMECUFFS:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_AMIRA_BLAZEWEAVER, HORDE);
                        break;
                    case NPC_SISSY_FLAMECUFFS:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_VEESHA_BLAZEWEAVER, HORDE);
                        break;
                    case NPC_FIELD_MEDIC_PENNY:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_BATTLE_PRIEST_ELIZA, HORDE);
                        break;
                    case NPC_FIELD_MEDIC_JESSI:
                        if (TeamInInstance == HORDE)
                            creature->UpdateEntry(NPC_BATTLE_PRIEST_GINA, HORDE);
                        break;

                    // Thorim
                    case NPC_THORIM:
                        ThorimGUID = creature->GetGUID();
                        if (creature->isDead())
                            SetBossState(BOSS_THORIM, DONE);
                        break;
                    case NPC_THORIM_CTRL:
                        ThorimCtrlGUID = creature->GetGUID();
                        break;
                    case NPC_MERCENARY_CAPTAIN_A:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MERCENARY_CAPTAIN_H, 1692);
                        break;
                    case NPC_MERCENARY_SOLDIER_A:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MERCENARY_SOLDIER_H, 1692);
                        break;
                    case NPC_RUNIC_COLOSSUS:
                        RunicColossusGUID = creature->GetGUID();
                        break;
                    case NPC_RUNE_GIANT:
                        RuneGiantGUID = creature->GetGUID();
                        break;

                    // Freya
                    case NPC_FREYA:
                        FreyaGUID = creature->GetGUID();
                        if (creature->isDead())
                            SetBossState(BOSS_FREYA, DONE);
                        break;
                    case NPC_BRIGHTLEAF:
                        ElderGUIDs[0] = creature->GetGUID();
                        if (GetBossState(BOSS_FREYA) == DONE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_IRONBRANCH:
                        ElderGUIDs[1] = creature->GetGUID();
                        if (GetBossState(BOSS_FREYA) == DONE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_STONEBARK:
                        ElderGUIDs[2] = creature->GetGUID();
                        if (GetBossState(BOSS_FREYA) == DONE)
                            creature->DespawnOrUnsummon();
                         break;

                    // Mimiron
                    case NPC_MIMIRON:
                        MimironGUID = creature->GetGUID();
                        if (creature->isDead())
                            SetBossState(BOSS_MIMIRON, DONE);
                        break;
                    case NPC_LEVIATHAN_MKII:
                        LeviathanMKIIGUID = creature->GetGUID();
                        break;
                    case NPC_VX_001:
                        VX001GUID = creature->GetGUID();
                        break;
                    case NPC_AERIAL_COMMAND_UNIT:
                        AerialUnitGUID = creature->GetGUID();
                        break;

                    // General Vezax
                    case NPC_VEZAX:
                        VezaxGUID = creature->GetGUID();
                        break;

                    // Yogg-Saron
                    case NPC_YOGG_SARON:
                        YoggSaronGUID = creature->GetGUID();
                        break;
                    case NPC_VOICE_OF_YOGG_SARON:
                        VoiceOfYoggSaronGUID = creature->GetGUID();
                        break;
                    case NPC_BRAIN_OF_YOGG_SARON:
                        BrainOfYoggSaronGUID = creature->GetGUID();
                        break;
                    case NPC_SARA:
                        SaraGUID = creature->GetGUID();
                        break;
                    case NPC_FREYA_YS:
                        KeeperGUIDs[0] = creature->GetGUID();
                        _summonYSKeeper[0] = false;
                        SaveToDB();
                        ++keepersCount;
                        break;
                    case NPC_HODIR_YS:
                        KeeperGUIDs[1] = creature->GetGUID();
                        _summonYSKeeper[1] = false;
                        SaveToDB();
                        ++keepersCount;
                        break;
                    case NPC_THORIM_YS:
                        KeeperGUIDs[2] = creature->GetGUID();
                        _summonYSKeeper[2] = false;
                        SaveToDB();
                        ++keepersCount;
                        break;
                    case NPC_MIMIRON_YS:
                        KeeperGUIDs[3] = creature->GetGUID();
                        _summonYSKeeper[3] = false;
                        SaveToDB();
                        ++keepersCount;
                        break;
                    case NPC_SANITY_WELL:
                        creature->SetReactState(REACT_PASSIVE);
                        break;

                    // Algalon
                    case NPC_ALGALON:
                        AlgalonGUID = creature->GetGUID();
                        break;
                    case NPC_BRANN_BRONZBEARD_ALG:
                        BrannBronzebeardAlgGUID = creature->GetGUID();
                        break;
                    // These creatures are summoned by something else than Algalon
                    // but need to be controlled/despawned by him - so they need to be
                    // registered in his summon list
                    case NPC_ALGALON_VOID_ZONE_VISUAL_STALKER:
                    case NPC_ALGALON_STALKER_ASTEROID_TARGET_01:
                    case NPC_ALGALON_STALKER_ASTEROID_TARGET_02:
                        if (Creature* algalon = instance->GetCreature(AlgalonGUID))
                            algalon->AI()->JustSummoned(creature);
                        break;
                    default:
                        break;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_XT_TOY_PILE:
                        for (uint8 i = 0; i < 4; ++i)
                            if (XTToyPileGUIDs[i] == creature->GetGUID())
                            {
                                XTToyPileGUIDs[i] = ObjectGuid::Empty;
                                break;
                            }
                        break;
                    case NPC_BRANN_BRONZBEARD_ALG:
                        if (BrannBronzebeardAlgGUID == creature->GetGUID())
                            BrannBronzebeardAlgGUID = ObjectGuid::Empty;
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_ULDUAR_TELEPORTER:
                    {
                        float teleportPosX = go->GetPositionX();

                        // Teleport to the Expedition Base Camp
                        if (teleportPosX > -708.0f && teleportPosX < -704.0f)
                        {
                            TeleporterBaseCampGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, (GetData(DATA_COLOSSUS) == 2 || GetBossState(BOSS_LEVIATHAN) == DONE), go);
                            go->ApplyModFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE, !(GetData(DATA_COLOSSUS) == 2 || GetBossState(BOSS_LEVIATHAN) == DONE));
                        }
                        // Teleport to the Formation Grounds
                        else if (teleportPosX > 130.0f && teleportPosX < 132.0f)
                        {
                            TeleporterGroundsGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, (GetData(DATA_COLOSSUS) == 2 || GetBossState(BOSS_LEVIATHAN) == DONE), go);
                            go->ApplyModFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE, !(GetData(DATA_COLOSSUS) == 2 || GetBossState(BOSS_LEVIATHAN) == DONE));
                        }
                        // Teleport to the Colossal Forge
                        else if (teleportPosX > 552.0f && teleportPosX < 554.0f)
                        {
                            TeleporterForgeGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, GetBossState(BOSS_LEVIATHAN) == DONE, go);
                        }
                        // Teleport to the Scrapyard
                        else if (teleportPosX > 925.0f && teleportPosX < 927.0f)
                        {
                            TeleporterScrapyardGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, GetBossState(BOSS_XT002) == DONE, go);
                        }
                        // Teleport to the Antechamber of Ulduar
                        else if (teleportPosX > 1497.0f && teleportPosX < 1499.0f)
                        {
                            TeleporterAntchemberGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, GetBossState(BOSS_XT002) == DONE, go);
                        }
                        // Teleport to the Shattered Walkway
                        else if (teleportPosX > 1858.0f && teleportPosX < 1860.0f)
                        {
                            TeleporterShatteredGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, GetBossState(BOSS_KOLOGARN) == DONE, go);
                        }
                        // Teleport to the Conservatory of Life
                        else if (teleportPosX > 2085.0f && teleportPosX < 2087.0f)
                        {
                            TeleporterConservatoryGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, GetBossState(BOSS_AURIAYA) == DONE, go);
                        }
                        // Teleport to the Spark of Imagination
                        else if (teleportPosX > 2516.0f && teleportPosX < 2518.0f)
                        {
                            TeleporterImaginationGUID = go->GetGUID();  // NIY
                            HandleGameObject(ObjectGuid::Empty, (GetData(DATA_TRAM) == DONE || GetBossState(BOSS_MIMIRON) == DONE), go);
                        }
                        // Teleport to the Descent into Madness
                        else if (teleportPosX > 1854.0f && teleportPosX < 1856.0f)
                        {
                            TeleporterDescentGUID = go->GetGUID();
                            HandleGameObject(ObjectGuid::Empty, GetBossState(BOSS_VEZAX) == DONE, go);
                        }
                        break;
                    }
                    case GO_CRYSTAL_STORMS:
                        TowerofStormsCrystalGUID = go->GetGUID();
                        break;
                    case GO_CRYSTAL_FLAMES:
                        TowerofFlamesCrystalGUID = go->GetGUID();
                        break;
                    case GO_CRYSTAL_FROST:
                        TowerofFrostCrystalGUID = go->GetGUID();
                        break;
                    case GO_CRYSTAL_LIFE:
                        TowerofLifeCrystalGUID = go->GetGUID();
                        break;
                    case GO_LEVIATHAN_DOOR:
                        LeviDoorGUID = go->GetGUID();
                        if (GetBossState(BOSS_LEVIATHAN) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_RAZOR_HARPOON_1:
                        RazorHarpoonGUIDs[0] = go->GetGUID();
                        break;
                    case GO_RAZOR_HARPOON_2:
                        RazorHarpoonGUIDs[1] = go->GetGUID();
                        break;
                    case GO_RAZOR_HARPOON_3:
                        RazorHarpoonGUIDs[2] = go->GetGUID();
                        break;
                    case GO_RAZOR_HARPOON_4:
                        RazorHarpoonGUIDs[3] = go->GetGUID();
                        break;
                    case GO_LEVIATHAN_GATE:
                        LeviathanGateGUID = go->GetGUID();
                        if (GetData(DATA_COLOSSUS) == 2)
                            go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        break;
                    case GO_KOLOGARN_CHEST_10:
                    case GO_KOLOGARN_CHEST_25:
                        KologarnChestGUID = go->GetGUID();
                        break;
                    case GO_KOLOGARN_BRIDGE:
                        KologarnBridgeGUID = go->GetGUID();
                        if (GetBossState(BOSS_KOLOGARN) == DONE)
                            HandleGameObject(ObjectGuid::Empty, false, go);
                        break;
                    case GO_HODIR_CHEST_10:
                    case GO_HODIR_CHEST_25:
                        HodirChestGUID = go->GetGUID();
                        break;
                    case GO_HODIR_RARE_HARDMODE_10:
                    case GO_HODIR_RARE_HARDMODE_25:
                        HodirChestHardGUID = go->GetGUID();
                        break;
                    case GO_THORIM_DARK_IRON_PROTCULLIS:
                        ThorimDarkIronPortCullisGUID = go->GetGUID();
                        break;
                    case GO_CACHE_OF_STORMS_10:
                    case GO_CACHE_OF_STORMS_25:
                        ThorimChestGUID = go->GetGUID();
                        break;
                    case GO_CACHE_OF_STORMS_HARDMODE_10:
                    case GO_CACHE_OF_STORMS_HARDMODE_25:
                        ThorimChestHardGUID = go->GetGUID();
                        break;
                    case GO_THORIM_STONE_DOOR:
                        StoneDoorGUID = go->GetGUID();
                        break;
                    case GO_THORIM_RUNIC_DOOR:
                        RunicDoorGUID = go->GetGUID();
                        break;
                    case GO_ACTIVATE_TRAM:
                        ActivateTramGUID = go->GetGUID();
                        break;
                    case GO_CALL_TRAM_LAST:
                        CallTramLastGUID = go->GetGUID();
                        break;
                    case GO_CALL_TRAM_FIRST:
                        CallTramFirstGUID = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case GO_MIMIRON_TRAIN:
                        MimironTrainGUID = go->GetGUID();
                        break;
                    case GO_MIMIRON_ELEVATOR:
                        MimironElevatorGUID = go->GetGUID();
                        break;
                    case CACHE_OF_INNOVATION_10:
                    case CACHE_OF_INNOVATION_25:
                        MimironChestGUID = go->GetGUID();
                        break;
                    case CACHE_OF_INNOVATION_HARDMODE_10:
                    case CACHE_OF_INNOVATION_HARDMODE_25:
                        MimironChestHardGUID = go->GetGUID();
                        break;
                    case GO_BRAIN_ROOM_DOOR_1:
                        BrainRoomDoorGUIDs[0] = go->GetGUID();
                        break;
                    case GO_BRAIN_ROOM_DOOR_2:
                        BrainRoomDoorGUIDs[1] = go->GetGUID();
                        break;
                    case GO_BRAIN_ROOM_DOOR_3:
                        BrainRoomDoorGUIDs[2] = go->GetGUID();
                        break;
                    case GO_CELESTIAL_PLANETARIUM_ACCESS_10:
                    case GO_CELESTIAL_PLANETARIUM_ACCESS_25:
                        if (_algalonSummoned)
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        break;
                    case GO_DOODAD_UL_SIGILDOOR_01:
                        AlgalonSigilDoorGUID[0] = go->GetGUID();
                        if (_algalonSummoned)
                            go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case GO_DOODAD_UL_SIGILDOOR_02:
                        AlgalonSigilDoorGUID[1] = go->GetGUID();
                        if (_algalonSummoned)
                            go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case GO_GIFT_OF_THE_OBSERVER_10:
                    case GO_GIFT_OF_THE_OBSERVER_25:
                        GiftOfTheObserverGUID = go->GetGUID();
                        break;

                    // Doors Data
                    case GO_LEVIATHAN_DOOR_BACK:
                    case GO_XT_002_DOOR:
                    case GO_IRON_COUNCIL_ENTRANCE:
                    case GO_ARCHIVUM_DOOR:
                    case GO_KOLOGARN_DOOR:
                    case GO_HODIR_ENTRANCE:
                    case GO_HODIR_DOOR:
                    case GO_HODIR_ICE_DOOR:
                    case GO_THORIM_LIGHTNING_FIELD:
                    case GO_MIMIRON_DOOR_1:
                    case GO_MIMIRON_DOOR_2:
                    case GO_MIMIRON_DOOR_3:
                    case GO_VEZAX_DOOR:
                    case GO_YOGG_SARON_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_DOODAD_UL_SIGILDOOR_03:
                        AlgalonSigilDoorGUID[2] = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_DOODAD_UL_UNIVERSEFLOOR_01:
                        AlgalonFloorGUID[0] = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_DOODAD_UL_UNIVERSEFLOOR_02:
                        AlgalonFloorGUID[1] = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_DOODAD_UL_UNIVERSEGLOBE01:
                        AlgalonUniverseGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_DOODAD_UL_ULDUAR_TRAPDOOR_03:
                        AlgalonTrapdoorGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_WAY_TO_YOGG:
                        _ancientGateOfTheKeepersGUID = go->GetGUID();
                        if (GetBossState(BOSS_HODIR) == DONE && GetBossState(BOSS_THORIM) == DONE && GetBossState(BOSS_FREYA) == DONE && GetBossState(BOSS_MIMIRON) == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_LEVIATHAN_DOOR_BACK:
                    case GO_XT_002_DOOR:
                    case GO_IRON_COUNCIL_ENTRANCE:
                    case GO_ARCHIVUM_DOOR:
                    case GO_KOLOGARN_DOOR:
                    case GO_HODIR_ENTRANCE:
                    case GO_HODIR_DOOR:
                    case GO_HODIR_ICE_DOOR:
                    case GO_THORIM_LIGHTNING_FIELD:
                    case GO_MIMIRON_DOOR_1:
                    case GO_MIMIRON_DOOR_2:
                    case GO_MIMIRON_DOOR_3:
                    case GO_VEZAX_DOOR:
                    case GO_YOGG_SARON_DOOR:
                    case GO_DOODAD_UL_SIGILDOOR_03:
                    case GO_DOODAD_UL_UNIVERSEFLOOR_01:
                    case GO_DOODAD_UL_UNIVERSEFLOOR_02:
                    case GO_DOODAD_UL_UNIVERSEGLOBE01:
                    case GO_DOODAD_UL_ULDUAR_TRAPDOOR_03:
                        AddDoor(go, false);
                        break;
                    case GO_WAY_TO_YOGG:
                        _ancientGateOfTheKeepersGUID = ObjectGuid::Empty;
                        break;
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                // Champion / Conqueror of Ulduar
                if (unit->GetTypeId() == TYPEID_PLAYER)
                {
                    for (uint8 i = 0; i < BOSS_ALGALON; i++)
                    {
                        if (GetBossState(i) == IN_PROGRESS)
                        {
                            _CoUAchivePlayerDeathMask |= (1 << i);
                            SaveToDB();
                        }
                    }
                }
            
                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                switch (creature->GetEntry())
                {
                    case NPC_CORRUPTED_SERVITOR:
                    case NPC_MISGUIDED_NYMPH:
                    case NPC_GUARDIAN_LASHER:
                    case NPC_FOREST_SWARMER:
                    case NPC_MANGROVE_ENT:
                    case NPC_IRONROOT_LASHER:
                    case NPC_NATURES_BLADE:
                    case NPC_GUARDIAN_OF_LIFE:
                        if (!conSpeedAtory)
                        {
                            DoStartCriteria(CRITERIA_START_TYPE_EVENT, CRITERIA_CON_SPEED_ATORY);
                            conSpeedAtory = true;
                        }
                        break;
                    case NPC_STEELFORGED_DEFENDER:
                    case NPC_STEELFORGED_DEFFENDER:
                        DoStartCriteria(CRITERIA_START_TYPE_SPELL_TARGET, SPELL_DWARFAGEDDON_ACHIEVEMENT);
                        DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_DWARFAGEDDON_ACHIEVEMENT);
                        break;
                    default:
                        break;
                }
            }

            void ProcessEvent(WorldObject* /*go*/, uint32 eventId) override
            {
                // Flame Leviathan's Tower event triggers
                Creature* flameLeviathan = instance->GetCreature(LeviathanGUID);

                switch (eventId)
                {
                    case EVENT_TOWER_OF_STORM_DESTROYED:
                        if (flameLeviathan && flameLeviathan->IsAlive())
                            flameLeviathan->AI()->DoAction(ACTION_TOWER_OF_STORM_DESTROYED);
                        break;
                    case EVENT_TOWER_OF_FROST_DESTROYED:
                        if (flameLeviathan && flameLeviathan->IsAlive())
                            flameLeviathan->AI()->DoAction(ACTION_TOWER_OF_FROST_DESTROYED);
                        break;
                    case EVENT_TOWER_OF_FLAMES_DESTROYED:
                        if (flameLeviathan && flameLeviathan->IsAlive())
                            flameLeviathan->AI()->DoAction(ACTION_TOWER_OF_FLAMES_DESTROYED);
                        break;
                    case EVENT_TOWER_OF_LIFE_DESTROYED:
                        if (flameLeviathan && flameLeviathan->IsAlive())
                            flameLeviathan->AI()->DoAction(ACTION_TOWER_OF_LIFE_DESTROYED);
                        break;
                    case EVENT_TRAM_FIRST_STOP:
                        for (auto&& guid : { ActivateTramGUID, CallTramLastGUID })
                            if (GameObject* go = instance->GetGameObject(guid))
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case EVENT_TRAM_LAST_STOP:
                        for (auto&& guid : { ActivateTramGUID, CallTramFirstGUID })
                            if (GameObject* go = instance->GetGameObject(guid))
                                go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case EVENT_ACTIVATE_SANITY_WELL:
                        if (Creature* freya = instance->GetCreature(KeeperGUIDs[0]))
                            freya->AI()->DoAction(4/*ACTION_SANITY_WELLS*/);
                        break;
                    case EVENT_HODIRS_PROTECTIVE_GAZE_PROC:
                        if (Creature* hodir = instance->GetCreature(KeeperGUIDs[1]))
                            hodir->AI()->DoAction(5/*ACTION_FLASH_FREEZE*/);
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (type >= BOSS_HODIR && type <= BOSS_MIMIRON && GetBossState(type) == DONE && state != DONE)
                    return false;

                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case BOSS_LEVIATHAN:
                        DoUpdateWorldState(WORLD_STATE_LEVIATHAN, state == IN_PROGRESS);
                        HandleGameObject(TeleporterForgeGUID, state == DONE);
                        if (state == DONE)
                            DespawnLeviVehicles();
                        break;
                    case BOSS_IGNIS:
                        DoUpdateWorldState(WORLD_STATE_IGNIS, state == IN_PROGRESS);
                        break;
                    case BOSS_RAZORSCALE:
                        DoUpdateWorldState(WORLD_STATE_RAZORSCALE, state == IN_PROGRESS);
                        break;
                    case BOSS_XT002:
                        DoUpdateWorldState(WORLD_STATE_XT002, state == IN_PROGRESS);
                        HandleGameObject(TeleporterScrapyardGUID, state == DONE);
                        HandleGameObject(TeleporterAntchemberGUID, state == DONE);
                        break;
                    case BOSS_AURIAYA:
                        HandleGameObject(TeleporterConservatoryGUID, state == DONE);
                        if (state == DONE)
                            if (GameObject* train = instance->GetGameObject(MimironTrainGUID))
                                train->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case BOSS_MIMIRON:
                        HandleGameObject(TeleporterImaginationGUID, state == DONE);
                        if (state == DONE)
                        {
                            if (GetData(DATA_MIMIRON_HARDMODE))
                                DoRespawnGameObject(MimironChestHardGUID, WEEK);
                            else
                                DoRespawnGameObject(MimironChestGUID, WEEK);

                            instance->SummonCreature(NPC_MIMIRON_OBSERVATION_RING, ObservationRingKeepersPos[3]);
                            UpdateAncientGateOfTheKeepers();
                        }
                        break;
                    case BOSS_FREYA:
                        if (state == DONE)
                        {
                            instance->SummonCreature(NPC_FREYA_OBSERVATION_RING, ObservationRingKeepersPos[0]);
                            UpdateAncientGateOfTheKeepers();
                        }
                        break;
                    case BOSS_VEZAX:
                        HandleGameObject(TeleporterDescentGUID, state == DONE);
                        break;
                    case BOSS_YOGG_SARON:
                        break;
                    case BOSS_KOLOGARN:
                        if (state == DONE)
                        {
                            DoRespawnGameObject(KologarnChestGUID, WEEK);

                            HandleGameObject(TeleporterShatteredGUID, true);
                            HandleGameObject(KologarnBridgeGUID, false);
                        }
                        break;
                    case BOSS_HODIR:
                        if (state == DONE)
                        {
                            if (GetData(DATA_HODIR_RARE_CACHE))
                                if (GameObject* HodirRareCache = instance->GetGameObject(HodirChestHardGUID))
                                    HodirRareCache->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);

                            if (GameObject* HodirChest = instance->GetGameObject(HodirChestGUID))
                                HodirChest->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);

                            instance->SummonCreature(NPC_HODIR_OBSERVATION_RING, ObservationRingKeepersPos[1]);
                            UpdateAncientGateOfTheKeepers();
                        }
                        else if (state == FAIL)
                        {
                            if (GameObject* HodirRareCache = instance->GetGameObject(HodirChestHardGUID))
                            {
                                HodirRareCache->SetGoState(GO_STATE_READY);
                                HodirRareCache->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                                HodirRareCache->SetPhaseMask(PHASEMASK_NORMAL, true);
                            }
                        }
                        break;
                    case BOSS_THORIM:
                        if (state == DONE)
                        {
                            if (GetData(DATA_THORIM_HARDMODE))
                                DoRespawnGameObject(ThorimChestHardGUID, WEEK);
                            else
                                DoRespawnGameObject(ThorimChestGUID, WEEK);

                            instance->SummonCreature(NPC_THORIM_OBSERVATION_RING, ObservationRingKeepersPos[2]);
                            UpdateAncientGateOfTheKeepers();
                        }
                        break;
                    case BOSS_BRIGHTLEAF:
                        if (state == DONE && GetBossState(BOSS_FREYA) != DONE)
                        {
                            BrightleafDeath = time(nullptr);
                            DoCastSpellOnPlayers(SPELL_LUMBERJACKED_ACHIEVEMENT_CHECK);
                        }
                        break;
                    case BOSS_STONEBARK:
                        if (state == DONE && GetBossState(BOSS_FREYA) != DONE)
                        {
                            StonebarkDeath = time(nullptr);
                            DoCastSpellOnPlayers(SPELL_LUMBERJACKED_ACHIEVEMENT_CHECK);
                        }
                        break;
                    case BOSS_IRONBRANCH:
                        if (state == DONE && GetBossState(BOSS_FREYA) != DONE)
                        {
                            IronbranchDeath = time(nullptr);
                            DoCastSpellOnPlayers(SPELL_LUMBERJACKED_ACHIEVEMENT_CHECK);
                        }
                        break;
                    case BOSS_ALGALON:
                        DoUpdateWorldState(WORLD_STATE_ALGALON, state == IN_PROGRESS);
                        if (state == DONE)
                        {
                            _events.CancelEvent(EVENT_UPDATE_ALGALON_TIMER);
                            _events.CancelEvent(EVENT_DESPAWN_ALGALON);
                            DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 0);
                            _algalonTimer = 61;
                            if (GameObject* go = instance->GetGameObject(GiftOfTheObserverGUID))
                                DoRespawnGameObject(GiftOfTheObserverGUID, WEEK);
                            // get item level (recheck weapons)
                            for (auto&& itr : instance->GetPlayers())
                                if (Player* player = itr.GetSource())
                                    for (uint8 slot = EQUIPMENT_SLOT_MAINHAND; slot <= EQUIPMENT_SLOT_RANGED; ++slot)
                                        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                                            if (item->GetTemplate()->ItemLevel > _maxWeaponItemLevel)
                                                _maxWeaponItemLevel = item->GetTemplate()->ItemLevel;
                        }
                        else if (state == IN_PROGRESS)
                        {
                            // get item level (armor cannot be swapped in combat)
                            _maxArmorItemLevel = 0;
                            _maxWeaponItemLevel = 0;
                            for (auto&& itr : instance->GetPlayers())
                            {
                                if (Player* player = itr.GetSource())
                                {
                                    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                                    {
                                        if (slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY)
                                            continue;

                                        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                                        {
                                            if (slot >= EQUIPMENT_SLOT_MAINHAND && slot <= EQUIPMENT_SLOT_RANGED)
                                            {
                                                if (item->GetTemplate()->ItemLevel > _maxWeaponItemLevel)
                                                    _maxWeaponItemLevel = item->GetTemplate()->ItemLevel;
                                            }
                                            else if (item->GetTemplate()->ItemLevel > _maxArmorItemLevel)
                                                _maxArmorItemLevel = item->GetTemplate()->ItemLevel;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }

                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_COLOSSUS:
                        ColossusData = data;
                        if (data == 2)
                        {
                            _events.ScheduleEvent(EVENT_FLAME_LEVIATHAN_1, 1);
                            _events.ScheduleEvent(EVENT_FLAME_LEVIATHAN_2, 7500);
                            _events.ScheduleEvent(EVENT_FLAME_LEVIATHAN_3, 12000);
                            _events.ScheduleEvent(EVENT_FLAME_LEVIATHAN_4, 15000);
                            HandleGameObject(TeleporterBaseCampGUID, true);
                            HandleGameObject(TeleporterGroundsGUID, true);
                            if (GameObject* teleport = instance->GetGameObject(TeleporterBaseCampGUID))
                                teleport->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            if (GameObject* teleport = instance->GetGameObject(TeleporterGroundsGUID))
                                teleport->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            triggeredAreaTriggers.erase(AT_TELEPORTER_EVENT); // Different event from now on
                            SaveToDB();
                        }
                        break;
                    case DATA_LEVI_VEHICLES:
                        for (auto&& vehicle : vehiclesGUIDs)
                            if (Creature* vechicle = instance->GetCreature(vehicle))
                                if (data)
                                    vechicle->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                else
                                    vechicle->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    case DATA_LEVI_HARD_MODE:
                        LeviHardMod = bool(data);
                        if (LeviHardMod)
                            SaveToDB();
                        break;
                    case DATA_SHIELD_DISABLED:
                        ShieldDisabled = bool(data);
                        if (ShieldDisabled)
                            SaveToDB();
                        break;
                    case DATA_HODIR_RARE_CACHE:
                        HodirRareCacheData = data;
                        break;
                    case DATA_THORIM_HARDMODE:
                        ThorimHardMode = data;
                        break;
                    case DATA_MIMIRON_HARDMODE:
                        MimironHardMode = data;
                        break;
                    case DATA_UNBROKEN:
                        Unbroken = bool(data);
                        break;
                    case DATA_ILLUSION:
                        illusion = data;
                        break;
                    case DATA_TRAM:
                        TramData = data;
                        break;
                    case DATA_CALL_TRAM:
                        if (GetData(DATA_TRAM) == NOT_STARTED)
                            SetData(DATA_TRAM, DONE);
                        if (GameObject* tram = instance->GetGameObject(MimironTrainGUID))
                        {
                            if (data == GO_STATE_ACTIVE_ALTERNATIVE)
                                data = tram->GetGoState() == GO_STATE_ACTIVE ? GO_STATE_READY : GO_STATE_ACTIVE;

                            if (tram->GetGoState() == data)
                                break;

                            tram->SetGoState(GOState(data));

                            for (auto&& guid : { ActivateTramGUID, CallTramLastGUID, CallTramFirstGUID })
                                if (GameObject* go = instance->GetGameObject(guid))
                                    go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                        break;
                    case DATA_MIMIRON_ELEVATOR:
                        if (GameObject* go = instance->GetGameObject(MimironElevatorGUID))
                            go->SetGoState(GOState(data));
                        break;
                    case DATA_RUNIC_DOOR:
                        if (GameObject* go = instance->GetGameObject(RunicDoorGUID))
                            go->SetGoState(GOState(data));
                        break;
                    case DATA_STONE_DOOR:
                        if (GameObject* go = instance->GetGameObject(StoneDoorGUID))
                            go->SetGoState(GOState(data));
                        break;
                    case DATA_DRIVE_ME_CRAZY:
                        IsDriveMeCrazyEligible = data ? true : false;
                        break;
                    case EVENT_DESPAWN_ALGALON:
                        DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 1);
                        DoUpdateWorldState(WORLD_STATE_ALGALON_DESPAWN_TIMER, 60);
                        _algalonTimer = 60;
                        _events.ScheduleEvent(EVENT_DESPAWN_ALGALON, 3600000);
                        _events.ScheduleEvent(EVENT_UPDATE_ALGALON_TIMER, 60000);
                        break;
                    case DATA_ALGALON_SUMMON_STATE:
                        _algalonSummoned = true;
                        break;
                    case DATA_AREATRIGGER_EVENT:
                        if (triggeredAreaTriggers.insert(data).second)
                        {
                            switch (data)
                            {
                                case AT_REPAIR_STATION_1:
                                case AT_REPAIR_STATION_2:
                                    triggeredAreaTriggers.insert(AT_REPAIR_STATION_1);
                                    triggeredAreaTriggers.insert(AT_REPAIR_STATION_2);
                                    if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                        radio->AI()->Talk(SAY_REPAIR_STATION);
                                    break;
                                case AT_TOWER_OF_LIFE:
                                    if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                        radio->AI()->Talk(SAY_TOWER_OF_LIFE);
                                    break;
                                case AT_TOWER_OF_FLAME:
                                    if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                        radio->AI()->Talk(SAY_TOWER_OF_FLAME);
                                    break;
                                case AT_TOWER_OF_FROST:
                                    if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                        radio->AI()->Talk(SAY_TOWER_OF_FROST);
                                    break;
                                case AT_TOWER_OF_STORM:
                                    if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                        radio->AI()->Talk(SAY_TOWER_OF_STORM);
                                    break;
                                case AT_TELEPORTER_EVENT:
                                    if (GameObject* teleporter = instance->GetGameObject(TeleporterBaseCampGUID))
                                        if (Creature* engineer = teleporter->FindNearestCreature(NPC_HIRED_ENGINEER, 20.0f))
                                            engineer->AI()->DoAction(0);
                                    break;
                                case AT_LORE_KEEPER_EVENT:
                                    if (Creature* lorekeeper = instance->GetCreature(LoreKeeperOfNorgannonGUID))
                                        lorekeeper->AI()->DoAction(0);
                                    break;
                                case AT_BUNKERS_1:
                                case AT_BUNKERS_2:
                                case AT_BUNKERS_3:
                                case AT_BUNKERS_4:
                                    // not yet implemented
                                    break;
                                case RADIO_ORBITAL_SUPPORT:
                                    if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                        radio->AI()->Talk(SAY_ORBITAL_SUPPORT);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch (data)
                {
                    // Leviathan
                    case BOSS_LEVIATHAN:
                        return LeviathanGUID;
                    case NPC_ORBITAL_SUPPORT:
                        return OrbitalSupportGUID;
                    case DATA_TOWER_OF_STORMS_CRYSTAL:
                        return TowerofStormsCrystalGUID;
                    case DATA_TOWER_OF_FLAMES_CRYSTAL:
                        return TowerofFlamesCrystalGUID;
                    case DATA_TOWER_OF_FROST_CRYSTAL:
                        return TowerofFrostCrystalGUID;
                    case DATA_TOWER_OF_LIFE_CRYSTAL:
                        return TowerofLifeCrystalGUID;
                    case DATA_LEVI_DOOR:
                        return LeviDoorGUID;

                    case BOSS_IGNIS:
                        return IgnisGUID;

                    // Razorscale
                    case BOSS_RAZORSCALE:
                        return RazorscaleGUID;
                    case DATA_RAZORSCALE_CONTROL:
                        return RazorscaleController;
                    case DATA_EXPEDITION_COMMANDER:
                        return ExpeditionCommanderGUID;
                    case GO_RAZOR_HARPOON_1:
                        return RazorHarpoonGUIDs[0];
                    case GO_RAZOR_HARPOON_2:
                        return RazorHarpoonGUIDs[1];
                    case GO_RAZOR_HARPOON_3:
                        return RazorHarpoonGUIDs[2];
                    case GO_RAZOR_HARPOON_4:
                        return RazorHarpoonGUIDs[3];

                    // XT-002 Deconstructor
                    case BOSS_XT002:
                        return XT002GUID;
                    case DATA_TOY_PILE_0:
                    case DATA_TOY_PILE_1:
                    case DATA_TOY_PILE_2:
                    case DATA_TOY_PILE_3:
                        return XTToyPileGUIDs[data - DATA_TOY_PILE_0];

                    // Assembly of Iron
                    case BOSS_STEELBREAKER:
                        return AssemblyGUIDs[0];
                    case BOSS_MOLGEIM:
                        return AssemblyGUIDs[1];
                    case BOSS_BRUNDIR:
                        return AssemblyGUIDs[2];

                    case BOSS_KOLOGARN:
                        return KologarnGUID;
                    case BOSS_AURIAYA:
                        return AuriayaGUID;
                    case BOSS_HODIR:
                        return HodirGUID;

                    // Thorim
                    case BOSS_THORIM:
                        return ThorimGUID;
                    case DATA_RUNIC_COLOSSUS:
                        return RunicColossusGUID;
                    case DATA_RUNE_GIANT:
                        return RuneGiantGUID;
                    case GO_THORIM_DARK_IRON_PROTCULLIS:
                        return ThorimDarkIronPortCullisGUID;
                    case NPC_THORIM_CTRL:
                        return ThorimCtrlGUID;

                    // Freya
                    case BOSS_FREYA:
                        return FreyaGUID;
                    case BOSS_BRIGHTLEAF:
                        return ElderGUIDs[0];
                    case BOSS_IRONBRANCH:
                        return ElderGUIDs[1];
                    case BOSS_STONEBARK:
                        return ElderGUIDs[2];

                    // Mimiron
                    case BOSS_MIMIRON:
                        return MimironGUID;
                    case DATA_LEVIATHAN_MK_II: 
                        return LeviathanMKIIGUID;
                    case DATA_VX_001: 
                        return VX001GUID;
                    case DATA_AERIAL_UNIT: 
                        return AerialUnitGUID;

                    case BOSS_VEZAX:
                        return VezaxGUID;
                        
                    // Yogg-Saron
                    case BOSS_YOGG_SARON:
                        return YoggSaronGUID;
                    case DATA_VOICE_OF_YOGG_SARON:
                        return VoiceOfYoggSaronGUID;
                    case DATA_BRAIN_OF_YOGG_SARON:
                        return BrainOfYoggSaronGUID;
                    case DATA_SARA:
                        return SaraGUID;
                    case GO_BRAIN_ROOM_DOOR_1:
                        return BrainRoomDoorGUIDs[0];
                    case GO_BRAIN_ROOM_DOOR_2:
                        return BrainRoomDoorGUIDs[1];
                    case GO_BRAIN_ROOM_DOOR_3:
                        return BrainRoomDoorGUIDs[2];
                    case DATA_FREYA_YS:
                        return KeeperGUIDs[0];
                    case DATA_HODIR_YS:
                        return KeeperGUIDs[1];
                    case DATA_THORIM_YS:
                        return KeeperGUIDs[2];
                    case DATA_MIMIRON_YS:
                        return KeeperGUIDs[3];

                    // Algalon
                    case BOSS_ALGALON:
                        return AlgalonGUID;
                    case DATA_SIGILDOOR_01:
                        return AlgalonSigilDoorGUID[0];
                    case DATA_SIGILDOOR_02:
                        return AlgalonSigilDoorGUID[1];
                    case DATA_SIGILDOOR_03:
                        return AlgalonSigilDoorGUID[2];
                    case DATA_UNIVERSE_FLOOR_01:
                        return AlgalonFloorGUID[0];
                    case DATA_UNIVERSE_FLOOR_02:
                        return AlgalonFloorGUID[1];
                    case DATA_UNIVERSE_GLOBE:
                        return AlgalonUniverseGUID;
                    case DATA_ALGALON_TRAPDOOR:
                        return AlgalonTrapdoorGUID;
                    case DATA_BRANN_BRONZEBEARD_ALG:
                        return BrannBronzebeardAlgGUID;
                }

                return ObjectGuid::Empty;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_COLOSSUS:
                        return ColossusData;
                    case DATA_LEVI_HARD_MODE:
                        return LeviHardMod;
                    case DATA_SHIELD_DISABLED:
                        return ShieldDisabled;
                    case DATA_HODIR_RARE_CACHE:
                        return HodirRareCacheData;
                    case DATA_THORIM_HARDMODE:
                        return ThorimHardMode;
                    case DATA_MIMIRON_HARDMODE:
                        return MimironHardMode;
                    case DATA_UNBROKEN:
                        return uint32(Unbroken);
                    case DATA_TRAM:
                        return TramData;
                    case DATA_ILLUSION:
                        return illusion;
                    case DATA_KEEPERS_COUNT:
                        return keepersCount;
                    default:
                        break;
                }

                return 0;
            }

            bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscvalue1) override
            {
                switch (criteriaId)
                {
                    case CRITERIA_HERALD_OF_TITANS:
                        return _maxArmorItemLevel <= MAX_HERALD_ARMOR_ITEMLEVEL && _maxWeaponItemLevel <= MAX_HERALD_WEAPON_ITEMLEVEL;
                    case CRITERIA_REALM_FIRST_CELESTIAL_DEFENDER:
                        return true;
                    case CRITERIA_LUMBERJACKED_10:
                    case CRITERIA_LUMBERJACKED_25:
                        if (GetBossState(BOSS_FREYA) == NOT_STARTED)
                        {
                            if (BrightleafDeath && StonebarkDeath && IronbranchDeath)
                            {
                                if (abs(int(BrightleafDeath-StonebarkDeath)) <= 15 &&
                                    abs(int(StonebarkDeath-IronbranchDeath)) <= 15 &&
                                    abs(int(IronbranchDeath-BrightleafDeath)) <= 15)
                                {
                                    DoCompleteAchievement(instance->Is25ManRaid() ? 3118 : 2979);
                                    return true;
                                }
                            }
                        }
                        break;
                    case CRITERIA_WAITS_DREAMING_STORMWIND_25:
                    case CRITERIA_WAITS_DREAMING_STORMWIND_10:
                        return illusion == STORMWIND_ILLUSION;
                    case CRITERIA_WAITS_DREAMING_CHAMBER_25:
                    case CRITERIA_WAITS_DREAMING_CHAMBER_10:
                        return illusion == CHAMBER_ILLUSION;
                    case CRITERIA_WAITS_DREAMING_ICECROWN_25:
                    case CRITERIA_WAITS_DREAMING_ICECROWN_10:
                        return illusion == ICECROWN_ILLUSION;
                    case CRITERIA_DRIVE_ME_CRAZY_10:
                    case CRITERIA_DRIVE_ME_CRAZY_25:
                        return IsDriveMeCrazyEligible;
                    case CRITERIA_THREE_LIGHTS_IN_THE_DARKNESS_10:
                    case CRITERIA_THREE_LIGHTS_IN_THE_DARKNESS_25:
                        return keepersCount <= 3;
                    case CRITERIA_TWO_LIGHTS_IN_THE_DARKNESS_10:
                    case CRITERIA_TWO_LIGHTS_IN_THE_DARKNESS_25:
                        return keepersCount <= 2;
                    case CRITERIA_ONE_LIGHT_IN_THE_DARKNESS_10:
                    case CRITERIA_ONE_LIGHT_IN_THE_DARKNESS_25:
                        return keepersCount <= 1;
                    case CRITERIA_ALONE_IN_THE_DARKNESS_10:
                    case CRITERIA_ALONE_IN_THE_DARKNESS_25:
                    case CRITERIA_REALM_FIRST_DEATHS_DEMISE:
                        return keepersCount == 0;
                    case CRITERIA_C_O_U_LEVIATHAN_10:
                    case CRITERIA_C_O_U_LEVIATHAN_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_LEVIATHAN)) == 0;
                    case CRITERIA_C_O_U_IGNIS_10:
                    case CRITERIA_C_O_U_IGNIS_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_IGNIS)) == 0;
                    case CRITERIA_C_O_U_RAZORSCALE_10:
                    case CRITERIA_C_O_U_RAZORSCALE_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_RAZORSCALE)) == 0;
                    case CRITERIA_C_O_U_XT002_10:
                    case CRITERIA_C_O_U_XT002_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_XT002)) == 0;
                    case CRITERIA_C_O_U_IRON_COUNCIL_10:
                    case CRITERIA_C_O_U_IRON_COUNCIL_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_ASSEMBLY_OF_IRON)) == 0;
                    case CRITERIA_C_O_U_KOLOGARN_10:
                    case CRITERIA_C_O_U_KOLOGARN_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_KOLOGARN)) == 0;
                    case CRITERIA_C_O_U_AURIAYA_10:
                    case CRITERIA_C_O_U_AURIAYA_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_AURIAYA)) == 0;
                    case CRITERIA_C_O_U_HODIR_10:
                    case CRITERIA_C_O_U_HODIR_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_HODIR)) == 0;
                    case CRITERIA_C_O_U_THORIM_10:
                    case CRITERIA_C_O_U_THORIM_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_THORIM)) == 0;
                    case CRITERIA_C_O_U_FREYA_10:
                    case CRITERIA_C_O_U_FREYA_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_FREYA)) == 0;
                    case CRITERIA_C_O_U_MIMIRON_10:
                    case CRITERIA_C_O_U_MIMIRON_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_MIMIRON)) == 0;
                    case CRITERIA_C_O_U_VEZAX_10:
                    case CRITERIA_C_O_U_VEZAX_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_VEZAX)) == 0;
                    case CRITERIA_C_O_U_YOGG_SARON_10:
                    case CRITERIA_C_O_U_YOGG_SARON_25:
                        return (_CoUAchivePlayerDeathMask & (1 << BOSS_YOGG_SARON)) == 0;
                    default:
                        break;
                }

                //return InstanceScript::CheckAchievementCriteriaMeet(criteriaId, source, target, miscvalue1);
                return false;
            }
            
            bool CheckRequiredBosses(uint32 bossId, Player const* player = nullptr) const override
            {
                switch (bossId)
                {
                    case BOSS_ASSEMBLY_OF_IRON:
                    case BOSS_KOLOGARN:
                        if (GetBossState(BOSS_XT002) != DONE)
                            return false;
                        break;
                    case BOSS_AURIAYA:
                    case BOSS_HODIR:
                    case BOSS_THORIM:
                    case BOSS_FREYA:
                    case BOSS_MIMIRON:
                        if (GetBossState(BOSS_KOLOGARN) != DONE)
                            return false;
                        break;
                    case BOSS_VEZAX:
                        if (GetBossState(BOSS_HODIR) != DONE || GetBossState(BOSS_THORIM) != DONE
                            || GetBossState(BOSS_FREYA) != DONE || GetBossState(BOSS_MIMIRON) != DONE)
                            return false;
                        break;
                    case BOSS_YOGG_SARON:
                        if (GetBossState(BOSS_VEZAX) != DONE)
                            return false;
                        break;
                    default:
                        break;
                }

                return true;
            }

            void UpdateAncientGateOfTheKeepers()
            {
                if (GetBossState(BOSS_HODIR) == DONE && GetBossState(BOSS_THORIM) == DONE && GetBossState(BOSS_FREYA) == DONE && GetBossState(BOSS_MIMIRON) == DONE)
                    if (GameObject* go = instance->GetGameObject(_ancientGateOfTheKeepersGUID))
                        go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_LOCKED);
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "U U " << GetBossSaveData() << GetData(DATA_COLOSSUS) << ' ' << LeviHardMod << ' '  << ShieldDisabled << ' ' << _algalonTimer << ' ' << (_algalonSummoned ? 1 : 0);

                for (uint8 i = 0; i < 4; ++i)
                    saveStream << ' ' << (KeeperGUIDs[i] ? 1 : 0);

                saveStream << ' ' << _CoUAchivePlayerDeathMask;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* strIn) override
            {
                if (!strIn)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(strIn);

                char dataHead1, dataHead2;

                std::istringstream loadStream(strIn);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'U' && dataHead2 == 'U')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 tempState;
                    loadStream >> tempState;
                    if (tempState == IN_PROGRESS || tempState > SPECIAL)
                        tempState = NOT_STARTED;
                    ColossusData = tempState;

                    loadStream >> tempState;
                    if (tempState != 1)
                        tempState = NOT_STARTED;
                    LeviHardMod = tempState;

                    loadStream >> tempState;
                    if (tempState != 1)
                        tempState = NOT_STARTED;
                    ShieldDisabled = tempState;

                    loadStream >> _algalonTimer;
                    loadStream >> tempState;
                    _algalonSummoned = tempState != 0;
                    if (_algalonSummoned && GetBossState(BOSS_ALGALON) != DONE)
                    {
                        _summonAlgalon = true;
                        if (_algalonTimer && _algalonTimer <= 60)
                        {
                            _events.ScheduleEvent(EVENT_UPDATE_ALGALON_TIMER, 60000);
                            DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 1);
                            DoUpdateWorldState(WORLD_STATE_ALGALON_DESPAWN_TIMER, _algalonTimer);
                        }
                    }
                    
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        loadStream >> tempState;
                         _summonYSKeeper[i] = tempState != 0;
                    }

                    if (GetBossState(BOSS_FREYA) == DONE && !_summonYSKeeper[0])
                        _summonObservationRingKeeper[0] = true;
                    if (GetBossState(BOSS_HODIR) == DONE && !_summonYSKeeper[1])
                        _summonObservationRingKeeper[1] = true;
                    if (GetBossState(BOSS_THORIM) == DONE && !_summonYSKeeper[2])
                        _summonObservationRingKeeper[2] = true;
                    if (GetBossState(BOSS_MIMIRON) == DONE && !_summonYSKeeper[3])
                        _summonObservationRingKeeper[3] = true;

                    loadStream >> _CoUAchivePlayerDeathMask;
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
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
                        case EVENT_FLAME_LEVIATHAN_1:
                            if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                radio->AI()->Talk(SAY_FLAME_LEVIATHAN_1);
                            break;
                        case EVENT_FLAME_LEVIATHAN_2:
                            if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                radio->AI()->Talk(SAY_FLAME_LEVIATHAN_2);
                            break;
                        case EVENT_FLAME_LEVIATHAN_3:
                            if (Creature* radio = instance->GetCreature(BronzebeardRadioGUID))
                                radio->AI()->Talk(SAY_FLAME_LEVIATHAN_3);
                            break;
                        case EVENT_FLAME_LEVIATHAN_4:
                            if (Creature* leviathan = instance->GetCreature(LeviathanGUID))
                                leviathan->AI()->DoAction(ACTION_MOVE_TO_CENTER_POSITION);
                            if (GameObject* gate = instance->GetGameObject(LeviathanGateGUID))
                                gate->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                            break;
                        case EVENT_UPDATE_ALGALON_TIMER:
                            SaveToDB();
                            DoUpdateWorldState(WORLD_STATE_ALGALON_DESPAWN_TIMER, --_algalonTimer);
                            if (_algalonTimer)
                                _events.ScheduleEvent(EVENT_UPDATE_ALGALON_TIMER, 60000);
                            else
                            {
                                DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 0);
                                _events.CancelEvent(EVENT_UPDATE_ALGALON_TIMER);
                                if (Creature* algalon = instance->GetCreature(AlgalonGUID))
                                    algalon->AI()->DoAction(EVENT_DESPAWN_ALGALON);
                            }
                            break;
                    }
                }
            }

            void DespawnLeviVehicles()
            {
                for (auto&& guid : vehiclesGUIDs)
                    if (Creature* vehicle = instance->GetCreature(guid))
                        vehicle->DespawnOrUnsummon(1);
            }

        private:
            EventMap _events;
            uint32 _algalonTimer;
            bool _summonAlgalon;
            bool _algalonSummoned;
            bool _summonObservationRingKeeper[4];
            bool _summonYSKeeper[4];
            uint32 _maxArmorItemLevel;
            uint32 _maxWeaponItemLevel;
            uint32 _CoUAchivePlayerDeathMask;
            std::vector<ObjectGuid> vehiclesGUIDs;
            ObjectGuid _ancientGateOfTheKeepersGUID;
            std::set<uint32> triggeredAreaTriggers;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_ulduarinstanceMapScript(map);
        }
};

class go_call_tram : public GameObjectScript
{
    public:
        go_call_tram() : GameObjectScript("go_call_tram") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();
            if (!instance)
                return false;

            switch (go->GetEntry())
            {
                case GO_ACTIVATE_TRAM:   instance->SetData(DATA_CALL_TRAM, GO_STATE_ACTIVE_ALTERNATIVE); break;
                case GO_CALL_TRAM_FIRST: instance->SetData(DATA_CALL_TRAM, GO_STATE_READY);              break;
                case GO_CALL_TRAM_LAST:  instance->SetData(DATA_CALL_TRAM, GO_STATE_ACTIVE);             break;
                default:
                    break;
            }
            return false;
        }
};

class transport_ulduar_tram : public TransportScript
{
    public:
        transport_ulduar_tram() : TransportScript("transport_ulduar_tram") { }

        void OnCreate(Transport* transport) override
        {
            Map* map = transport->GetMap();
            GameObject* go = new GameObject();
            if (!go->Create(map->GenerateLowGuid<HighGuid::GameObject>(), GO_ACTIVATE_TRAM, map, PHASEMASK_NORMAL, 0.0f, -10.30644f, 0.0f, 1.570796f, { 0.0f, 0.0f, 0.7071066f, 0.7071069f }, 0/*Blizzlike: 255, but we handle it in a strange way, making it despawn*/, GO_STATE_READY))
            {
                delete go;
                return;
            }

            go->SetRespawnTime(0);
            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NODESPAWN);

            float x, y, z, o;
            go->GetPosition(x, y, z, o);

            go->SetTransport(transport);
            go->m_movementInfo.transport.guid = transport->GetGUID();
            go->m_movementInfo.transport.pos.Relocate(x, y, z, o);
            transport->CalculatePassengerPosition(x, y, z, &o);
            go->Relocate(x, y, z, o);

            bool spawned = false;
            if (go->IsInWorld())
            {
                go->UpdateObjectVisibility(true);
                spawned = true;
            }
            else if (go->GetMap()->AddToMap(go))
                spawned = true;
            else
                spawned = false;

            if (go->IsPositionValid() && spawned)
                transport->AddPassenger(go);
            else
                delete go;
        }
};

void AddSC_instance_ulduar()
{
    new instance_ulduar();
    new go_call_tram();
    new transport_ulduar_tram();
}
