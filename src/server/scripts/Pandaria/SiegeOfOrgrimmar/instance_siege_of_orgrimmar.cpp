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
#include "CreatureTextMgr.h"
#include "siege_of_orgrimmar.h"
#include "Group.h"
#include "LFGMgr.h"

static std::vector<DoorData> const doorData =
{
    { GO_IMMERSEUS_DOOR,               DATA_IMMERSEUS,              DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_SHA_FIELD,                    DATA_FALLEN_PROTECTORS,      DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_NORUSHEN_EXIT,                DATA_NORUSHEN,               DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_PRIDE_ENTRANCE,               DATA_SHA_OF_PRIDE,           DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_JUGGERNAUT_WIND_DOOR,         DATA_IRON_JUGGERNAUT,        DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NAZGRIM_ENTRY_DOOR,           DATA_GENERAL_NAZGRIM,        DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NAZGRIM_EXIT_DOOR,            DATA_GENERAL_NAZGRIM,        DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_NORUSHEN_LIGHT_CONTAIN_WALL1, DATA_NORUSHEN,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NORUSHEN_LIGHT_CONTAIN_WALL2, DATA_NORUSHEN,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NORUSHEN_LIGHT_CONTAIN_WALL3, DATA_NORUSHEN,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NORUSHEN_LIGHT_CONTAIN_WALL4, DATA_NORUSHEN,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NORUSHEN_LIGHT_CONTAIN_CEIL,  DATA_NORUSHEN,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_NORUSHEN_LIGHT_CONTAIN_FLOOR, DATA_NORUSHEN,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_RUSTED_IRON_PORTCULLIS,       DATA_KORKRON_DARK_SHAMANS,   DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_DARK_SHAMANS_FRONTGATE,       DATA_KORKRON_DARK_SHAMANS,   DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_JAGGED_IRON_GATE_ENTRANCE,    DATA_MALKOROK,               DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_JAGGED_IRON_GATE_EXIT,        DATA_MALKOROK,               DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_THOK_ENTRANCE,                DATA_THOK_THE_BLOODTHIRSTY,  DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_SPOILS_IRON_GATE_1,           DATA_SPOILS_OF_PANDARIA,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_SPOILS_IRON_GATE_2,           DATA_SPOILS_OF_PANDARIA,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_SPOILS_IRON_GATE_3,           DATA_SPOILS_OF_PANDARIA,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_SPOILS_IRON_GATE_4,           DATA_SPOILS_OF_PANDARIA,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_MASSIVE_IRONBOUND_DOOR,       DATA_SPOILS_OF_PANDARIA,     DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_PARAGON_ARENA_WALL,           DATA_PARAGONS_OF_THE_KLAXXI, DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_PARAGON_EXIT,                 DATA_PARAGONS_OF_THE_KLAXXI, DOOR_TYPE_PASSAGE, BOUNDARY_NONE },
    { GO_ENTRANCE_RIGHT,               DATA_GARROSH_HELLSCREAM,     DOOR_TYPE_ROOM,    BOUNDARY_NONE },
    { GO_ENTRANCE_LEFT,                DATA_GARROSH_HELLSCREAM,     DOOR_TYPE_ROOM,    BOUNDARY_NONE },
};

class instance_siege_of_orgrimmar : public InstanceMapScript
{
    public: instance_siege_of_orgrimmar() : InstanceMapScript("instance_siege_of_orgrimmar", 1136) { }

        struct instance_siege_of_orgrimmar_InstanceMapScript : public InstanceScript
        {
            instance_siege_of_orgrimmar_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 m_auiEncounter[MAX_TYPES];
            std::map<uint32, uint64> ParagonEncounters;
            std::vector<ObjectGuid> m_LasserArray, ShaVortexGUIDs, ShaVortexGoGUIDs, EmbodiedMinionGUIDs;
            std::list<uint32> m_paragonSeq;
            std::vector<ObjectGuid> korkronSniperGUIDs;
            std::vector<ObjectGuid> boxesGUIDs;
            std::vector<ObjectGuid> leverGUIDs;
            std::vector<ObjectGuid> garroshOutroGUIDs;
            std::vector<ObjectGuid> galakrasIntruderGUIDs;
            std::vector<ObjectGuid> korkronBarrackGUIDs;
            std::vector<ObjectGuid> korkronPrisonerGUIDs;
            std::vector<ObjectGuid> theramoreCitizenGUIDs;
            uint32 teamInInstance;
            uint32 fallenProtectorsPreEvent;
            uint32 norushenPreEvent;
            uint32 norushenPastEvent;
            uint32 shaofPridePreEvent;
            uint32 shaofPridePastEvent;
            uint32 galakrasPreEvent;
            uint32 ironJuggernautPastEvent;
            uint32 thokBloodthirstyPreEvent;
            uint32 paragonPreEvent;
            uint32 garroshHellscreamPreEvent;
            uint32 shamansTotemEvent;
            uint32 siegecrafterPreEvent;
            uint32 beachCannonsCount;
            uint32 aquaDefendersCount;
            uint32 aquaDefendersEvent;
            uint32 rescuePrisonersCount;
            ObjectGuid invisStalkerGUID;
            ObjectGuid loreWalkerChoEntranceGUID;
            ObjectGuid oozeControllerGUID;
            ObjectGuid massiveAncientDoorGUID;
            ObjectGuid pressurePlateGUID;
            ObjectGuid immerseusGUID;
            ObjectGuid rookGUID;
            ObjectGuid heGUID;
            ObjectGuid sunGUID;
            ObjectGuid norushenGUID;
            ObjectGuid shaOfPrideGUID;
            ObjectGuid shaEnergyWallGUID;
            ObjectGuid mysteriousDoorGUID;
            ObjectGuid amalgamGUID;
            ObjectGuid quarantineMeasureGUID;
            ObjectGuid harommGUID;
            ObjectGuid kardrisGUID;
            ObjectGuid nazgrimGUID;
            ObjectGuid malkorokGUID;
            ObjectGuid siegecraftGUID;
            ObjectGuid garroshGUID;
            ObjectGuid aysaGUID;
            ObjectGuid overseerKomakGUID;
            ObjectGuid jiFirepawGUID;
            ObjectGuid eitriggGUID;
            ObjectGuid ironJuggernautGUID;
            ObjectGuid immerseusChestGUID;
            ObjectGuid immerseusChestHeroicGUID;
            ObjectGuid zealGUID;
            ObjectGuid shaChestGUID;
            ObjectGuid shaChestHeroicGUID;
            ObjectGuid lorthemarGUID;
            ObjectGuid aethasGUID;
            ObjectGuid sylvanasGUID;
            ObjectGuid galakrasGUID;
            ObjectGuid zaelaGUID;
            ObjectGuid norushenRoomGUID;
            ObjectGuid jainaRoomGUID;
            ObjectGuid lorthemarRoomGUID;
            ObjectGuid anguishGUID;
            ObjectGuid southTowerDoorGUID;
            ObjectGuid northTowerDoorGUID;
            ObjectGuid voljinGUID;
            ObjectGuid baineGUID;
            ObjectGuid orgrimmarGatesGUID;
            ObjectGuid nazgrimPreEventGUID;
            ObjectGuid tyrandeGUID;
            ObjectGuid glaiweGUID;
            ObjectGuid doyodaGUID;
            ObjectGuid wildhummerShamanGUID;
            ObjectGuid runthakGUID;
            ObjectGuid groldarGUID;
            ObjectGuid malkorokControllerGUID;
            ObjectGuid miasmaGUID;
            ObjectGuid rexDoorGUID;
            ObjectGuid thokGUID;
            ObjectGuid akolikGUID;
            ObjectGuid montakGUID;
            ObjectGuid goraiGUID;
            ObjectGuid spoilsChestGUID;
            ObjectGuid spoilsHeroicChestGUID;
            ObjectGuid moguSpoils1GUID;
            ObjectGuid moguSpoils2GUID;
            ObjectGuid mantidSpoils1GUID;
            ObjectGuid mantidSpoils2GUID;
            ObjectGuid spoilsAccessLowRightGUID;
            ObjectGuid spoilsAccessLowLeftGUID;
            ObjectGuid spoilsAccessHighRightGUID;
            ObjectGuid spoilsAccessHighLeftGUID;
            ObjectGuid spoilsCrossRightGUID;
            ObjectGuid spoilsCrossLeftGUID;
            ObjectGuid spoilsOfPandariaGUID;
            ObjectGuid leverUpRightGUID;
            ObjectGuid leverUpLeftGUID;
            ObjectGuid leverLowRightGUID;
            ObjectGuid leverLowLeftGUID;
            ObjectGuid siegeCrafterHelperGUID;
            ObjectGuid siegecrafterGUID;
            ObjectGuid southCannonGUID;
            ObjectGuid northCannonGUID;
            ObjectGuid southRopeGUID;
            ObjectGuid northRopeGUID;
            ObjectGuid varianGUID;
            ObjectGuid jainaGUID;
            ObjectGuid vereesaGUID;
            ObjectGuid cosmeticSpoilsGUID;
            ObjectGuid siegecrafterVehicleGUID;
            ObjectGuid paragonControllerGUID;
            ObjectGuid kilrukGUID;
            ObjectGuid xarilGUID;
            ObjectGuid kaztikGUID;
            ObjectGuid korvenGUID;
            ObjectGuid iyyokukGUID;
            ObjectGuid karozGUID;
            ObjectGuid skeerGUID;
            ObjectGuid rikkalGUID;
            ObjectGuid hisekGUID;
            ObjectGuid mantidAmberGUID;
            ObjectGuid siegecrafterDoorGUID;
            ObjectGuid aggronGUID;
            ObjectGuid gorodanGUID;
            ObjectGuid shannaGUID;
            ObjectGuid thrallGUID;
            ObjectGuid korkronGunnerGUID;
            ObjectGuid korkronGunnerGUID_2;
            ObjectGuid gamonGUID;
            ObjectGuid heartOfYshaarjGUID;
            ObjectGuid garroshWestGatesGUID;
            ObjectGuid garroshEastGatesGUID;
            ObjectGuid saurfangGUID;
            ObjectGuid paragonEntranceGUID;
            ObjectGuid gunshipMainCannonGUID;
            ObjectGuid starvedYetiThrashGUID;
            ObjectGuid enragedMushanThrashGUID;
            ObjectGuid darkFarseerBarracksGUID;
            ObjectGuid darkFarseerSkullsplitterGUID;
            ObjectGuid encaseInAmberGUID;
            ObjectGuid thriceLockedCageGUID;
            ObjectGuid gastropodGUID;

            bool isLfr;
            bool lfrSectionFound;
            uint32 lfrQuarter;

            bool isFlex;

            void Initialize() override
            {
                SetBossNumber(MAX_TYPES);
                LoadDoorData(doorData);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
                m_LasserArray.clear();
                m_paragonSeq.clear();
                ShaVortexGUIDs.clear();
                ShaVortexGoGUIDs.clear();
                EmbodiedMinionGUIDs.clear();
                korkronSniperGUIDs.clear();
                boxesGUIDs.clear();
                leverGUIDs.clear();
                garroshOutroGUIDs.clear();
                galakrasIntruderGUIDs.clear();
                korkronBarrackGUIDs.clear();
                korkronPrisonerGUIDs.clear();
                theramoreCitizenGUIDs.clear();

                fallenProtectorsPreEvent     = 0;
                norushenPreEvent             = 0;
                norushenPastEvent            = 0;
                shaofPridePreEvent           = 0;
                shaofPridePastEvent          = 0;
                galakrasPreEvent             = 0;
                ironJuggernautPastEvent      = 0;
                thokBloodthirstyPreEvent     = 0;
                paragonPreEvent              = 0;
                shamansTotemEvent            = 0;
                invisStalkerGUID             = ObjectGuid::Empty;
                loreWalkerChoEntranceGUID    = ObjectGuid::Empty;
                massiveAncientDoorGUID       = ObjectGuid::Empty;
                pressurePlateGUID            = ObjectGuid::Empty;
                immerseusGUID                = ObjectGuid::Empty;
                rookGUID                     = ObjectGuid::Empty;
                heGUID                       = ObjectGuid::Empty;
                sunGUID                      = ObjectGuid::Empty;
                norushenGUID                 = ObjectGuid::Empty;
                shaOfPrideGUID               = ObjectGuid::Empty;
                shaEnergyWallGUID            = ObjectGuid::Empty;
                mysteriousDoorGUID           = ObjectGuid::Empty;
                amalgamGUID                  = ObjectGuid::Empty;
                quarantineMeasureGUID        = ObjectGuid::Empty;
                nazgrimGUID                  = ObjectGuid::Empty;
                malkorokGUID                 = ObjectGuid::Empty;
                harommGUID                   = ObjectGuid::Empty;
                kardrisGUID                  = ObjectGuid::Empty;
                overseerKomakGUID            = ObjectGuid::Empty;
                jiFirepawGUID                = ObjectGuid::Empty;
                eitriggGUID                  = ObjectGuid::Empty;
                ironJuggernautGUID           = ObjectGuid::Empty;
                immerseusChestGUID           = ObjectGuid::Empty;
                immerseusChestHeroicGUID     = ObjectGuid::Empty;
                zealGUID                     = ObjectGuid::Empty;
                shaChestGUID                 = ObjectGuid::Empty;
                shaChestHeroicGUID           = ObjectGuid::Empty;
                teamInInstance               = 0;
                lorthemarGUID                = ObjectGuid::Empty;
                aethasGUID                   = ObjectGuid::Empty;
                sylvanasGUID                 = ObjectGuid::Empty;
                galakrasGUID                 = ObjectGuid::Empty;
                zaelaGUID                    = ObjectGuid::Empty;
                norushenRoomGUID             = ObjectGuid::Empty;
                jainaRoomGUID                = ObjectGuid::Empty;
                lorthemarRoomGUID            = ObjectGuid::Empty;
                anguishGUID                  = ObjectGuid::Empty;
                southTowerDoorGUID           = ObjectGuid::Empty;
                northTowerDoorGUID           = ObjectGuid::Empty;
                voljinGUID                   = ObjectGuid::Empty;
                baineGUID                    = ObjectGuid::Empty;
                orgrimmarGatesGUID           = ObjectGuid::Empty;
                nazgrimPreEventGUID          = ObjectGuid::Empty;
                tyrandeGUID                  = ObjectGuid::Empty;
                glaiweGUID                   = ObjectGuid::Empty;
                doyodaGUID                   = ObjectGuid::Empty;
                wildhummerShamanGUID         = ObjectGuid::Empty;
                runthakGUID                  = ObjectGuid::Empty;
                groldarGUID                  = ObjectGuid::Empty;
                malkorokControllerGUID       = ObjectGuid::Empty;
                miasmaGUID                   = ObjectGuid::Empty;
                rexDoorGUID                  = ObjectGuid::Empty;
                thokGUID                     = ObjectGuid::Empty;
                akolikGUID                   = ObjectGuid::Empty;
                montakGUID                   = ObjectGuid::Empty;
                goraiGUID                    = ObjectGuid::Empty;
                spoilsChestGUID              = ObjectGuid::Empty;
                spoilsHeroicChestGUID        = ObjectGuid::Empty;
                moguSpoils1GUID              = ObjectGuid::Empty;
                moguSpoils2GUID              = ObjectGuid::Empty;
                mantidSpoils1GUID            = ObjectGuid::Empty;
                mantidSpoils2GUID            = ObjectGuid::Empty;
                spoilsAccessLowRightGUID     = ObjectGuid::Empty;
                spoilsAccessLowLeftGUID      = ObjectGuid::Empty;
                spoilsAccessHighRightGUID    = ObjectGuid::Empty;
                spoilsAccessHighLeftGUID     = ObjectGuid::Empty;
                spoilsCrossRightGUID         = ObjectGuid::Empty;
                spoilsCrossLeftGUID          = ObjectGuid::Empty;
                spoilsOfPandariaGUID         = ObjectGuid::Empty;
                leverUpRightGUID             = ObjectGuid::Empty;
                leverUpLeftGUID              = ObjectGuid::Empty;
                leverLowRightGUID            = ObjectGuid::Empty;
                leverLowLeftGUID             = ObjectGuid::Empty;
                siegeCrafterHelperGUID       = ObjectGuid::Empty;
                siegecrafterGUID             = ObjectGuid::Empty;
                garroshGUID                  = ObjectGuid::Empty;
                southCannonGUID              = ObjectGuid::Empty;
                northCannonGUID              = ObjectGuid::Empty;
                southRopeGUID                = ObjectGuid::Empty;
                northRopeGUID                = ObjectGuid::Empty;
                varianGUID                   = ObjectGuid::Empty;
                jainaGUID                    = ObjectGuid::Empty;
                vereesaGUID                  = ObjectGuid::Empty;
                cosmeticSpoilsGUID           = ObjectGuid::Empty;
                siegecrafterVehicleGUID      = ObjectGuid::Empty;
                paragonControllerGUID        = ObjectGuid::Empty;
                kilrukGUID                   = ObjectGuid::Empty;
                xarilGUID                    = ObjectGuid::Empty;
                kaztikGUID                   = ObjectGuid::Empty;
                korvenGUID                   = ObjectGuid::Empty;
                iyyokukGUID                  = ObjectGuid::Empty;
                karozGUID                    = ObjectGuid::Empty;
                skeerGUID                    = ObjectGuid::Empty;
                rikkalGUID                   = ObjectGuid::Empty;
                hisekGUID                    = ObjectGuid::Empty;
                mantidAmberGUID              = ObjectGuid::Empty;
                siegecrafterDoorGUID         = ObjectGuid::Empty;
                siegecrafterPreEvent         = 0;
                thrallGUID                   = ObjectGuid::Empty;
                korkronGunnerGUID            = ObjectGuid::Empty;
                korkronGunnerGUID_2          = ObjectGuid::Empty;
                gamonGUID                    = ObjectGuid::Empty;
                heartOfYshaarjGUID           = ObjectGuid::Empty;
                garroshWestGatesGUID         = ObjectGuid::Empty;
                garroshEastGatesGUID         = ObjectGuid::Empty;
                saurfangGUID                 = ObjectGuid::Empty;
                paragonEntranceGUID          = ObjectGuid::Empty;
                garroshHellscreamPreEvent    = 0;
                gunshipMainCannonGUID        = ObjectGuid::Empty;
                starvedYetiThrashGUID        = ObjectGuid::Empty;
                enragedMushanThrashGUID      = ObjectGuid::Empty;
                beachCannonsCount            = 0;
                aquaDefendersCount           = 0;
                aquaDefendersEvent           = 0;
                darkFarseerBarracksGUID      = ObjectGuid::Empty;
                darkFarseerSkullsplitterGUID = ObjectGuid::Empty;
                rescuePrisonersCount         = 0;
                encaseInAmberGUID            = ObjectGuid::Empty;
                thriceLockedCageGUID         = ObjectGuid::Empty;
                gastropodGUID                = ObjectGuid::Empty;
                isLfr                        = instance->ToInstanceMap()->IsLFGMap();
                isFlex                       = instance->GetDifficulty() == RAID_DIFFICULTY_1025MAN_FLEX;

                lfrQuarter = QUARTER_NONE;
                instance->SetWorldState(WORLDSTATE_RESCUE_RAIDERS, 0);
                instance->SetWorldState(WORLDSTATE_UNLIMITED_POTENTIAL, 0);
                instance->SetWorldState(WORLDSTATE_UNLIMITED_POTENTIAL_2, 0);
                instance->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US, 0);
                instance->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US_2, 0);
            }

            bool IsFirstDFBoss()
            {
                if ((lfrQuarter == QUARTER_VALE_OF_ETERNAL_SORROWS || lfrQuarter == QUARTER_VALE_OF_ETERNAL_SORROWS_F) && GetBossState(DATA_IMMERSEUS) != DONE)
                    return true;
                if ((lfrQuarter == QUARTER_GATES_OF_RETRIBUTION || lfrQuarter == QUARTER_GATES_OF_RETRIBUTION_F) && GetBossState(DATA_GALAKRAS) != DONE)
                    return true;
                if ((lfrQuarter == QUARTER_THE_UNDERHOLD || lfrQuarter == QUARTER_THE_UNDERHOLD_F) && GetBossState(DATA_MALKOROK) != DONE)
                    return true;
                if ((lfrQuarter == QUARTER_DOWNFALL || lfrQuarter == QUARTER_DOWNFALL_F) && GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) != DONE)
                    return true;
                return false;
            }

            void OnPlayerEnter(Player* player) override
            {
                UpdateDynamicHealth();

                if (!teamInInstance)
                    teamInInstance = player->GetTeam();

                if (isLfr && !lfrQuarter)
                    if (Group* group = player->GetGroup())
                        if (group->isLFGGroup())
                            lfrQuarter = sLFGMgr->GetDungeon(group->GetGUID());

                // Determination
                if (isLfr && GetDeterminationValue() && !isFlex)
                    if (Aura* detAura = player->AddAura(SPELL_DETERMINATION_LFR, player))
                        detAura->SetStackAmount(GetDeterminationValue());

                // Norushen Trial Exploit
                if (GetBossState(DATA_NORUSHEN) == IN_PROGRESS && !player->HasAura(144421))
                {
                    player->CastSpell(player, 144421, true);
                    player->SetPower(POWER_ALTERNATE_POWER, 75);
                }

                // Sha of Pride Exploit
                if (GetBossState(DATA_SHA_OF_PRIDE) == IN_PROGRESS && !player->HasAura(144343))
                {
                    player->CastSpell(player, 144343, true);
                    player->SetPower(POWER_ALTERNATE_POWER, 70); // shoudn`t set this value but exclude exploit with logout with high pride value
                }

                if (GetEncountersDoneCount(MAX_TYPES) < 1 || player->GetSession()->PlayerLoading() || player->IsGameMaster())
                    return;

                if ((isLfr || isFlex) && IsFirstDFBoss())
                    return;

                Position pos = GetCurrentEntrancePosition();
                player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
            }

            void OnPlayerLeave(Player* player) override
            {
                UpdateDynamicHealth();

                // Remove death persistent auras
                if (player)
                {
                    player->RemoveAurasDueToSpell(144421); // corruption bar - by amalgam of corruption
                    player->RemoveAurasDueToSpell(144452); // purified - by amalgam of corruption
                    player->RemoveAurasDueToSpell(144343); // pride bar - by sha of pride
                    player->RemoveAurasDueToSpell(145458); // Rune of Power - Spoils of Pandaria (LFR only)

                    // Fallen Protectors - Mark of Anguish - On Logout or instance Leave cause it
                    if (GetBossState(DATA_FALLEN_PROTECTORS) == IN_PROGRESS)
                    {
                        if (Creature* anguish = instance->GetCreature(GetGuidData(NPC_EMBODIED_ANGUISH)))
                            if (player->GetGUID() == anguish->AI()->GetGUID())
                                anguish->AI()->DoAction(2);
                    }
                }
            }

            Position GetCurrentEntrancePosition() override
            {
                uint32 lastType = MAX_TYPES;
                if (isFlex || isLfr)
                {
                    switch (lfrQuarter)
                    {
                        case QUARTER_VALE_OF_ETERNAL_SORROWS:
                        case QUARTER_VALE_OF_ETERNAL_SORROWS_F:
                            lastType = DATA_NORUSHEN;
                            break;
                        case QUARTER_GATES_OF_RETRIBUTION:
                        case QUARTER_GATES_OF_RETRIBUTION_F:
                            lastType = DATA_KORKRON_DARK_SHAMANS;
                            break;
                        case QUARTER_THE_UNDERHOLD:
                        case QUARTER_THE_UNDERHOLD_F:
                            lastType = DATA_SPOILS_OF_PANDARIA;
                            break;
                    }
                }
                uint32 m_encounterDone = GetEncountersDoneCount(lastType);

                switch (m_encounterDone) // begun enum from bossId where entrance was changed
                {
                    case DATA_IMMERSEUS:
                    case DATA_FALLEN_PROTECTORS:
                    case DATA_NORUSHEN:
                    case DATA_SHA_OF_PRIDE:
                    case DATA_GALAKRAS:
                    case DATA_IRON_JUGGERNAUT:
                    case DATA_KORKRON_DARK_SHAMANS:
                    case DATA_GENERAL_NAZGRIM:
                    case DATA_MALKOROK:
                        return EncountersEntrance[m_encounterDone + 1];
                    case DATA_SPOILS_OF_PANDARIA:
                    case DATA_THOK_THE_BLOODTHIRSTY:
                        return EncountersEntrance[10];
                    case DATA_SIEGECRAFTER_BLACKFUSE:
                        return EncountersEntrance[11];
                    case DATA_PARAGONS_OF_THE_KLAXXI:
                    case DATA_GARROSH_HELLSCREAM:
                        return EncountersEntrance[12];
                }

                return EncountersEntrance[0];
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!teamInInstance)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            teamInInstance = player->GetTeam();
                }

                if (isLfr && !lfrQuarter)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            OnPlayerEnter(player);
                }

                if (lfrQuarter != QUARTER_NONE)
                {
                    if (lfrQuarter == QUARTER_VALE_OF_ETERNAL_SORROWS || lfrQuarter == QUARTER_VALE_OF_ETERNAL_SORROWS_F)
                    {
                        if (creature->GetPositionY() > 1190.0f)
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < DATA_MAX_BOSS_DATA; i++)
                            if (i != DATA_IMMERSEUS && i != DATA_FALLEN_PROTECTORS && i != DATA_NORUSHEN && i != DATA_SHA_OF_PRIDE)
                                SetBossState(i, DONE);
                    }
                    else if (lfrQuarter == QUARTER_GATES_OF_RETRIBUTION || lfrQuarter == QUARTER_GATES_OF_RETRIBUTION_F)
                    {
                        if ((creature->GetPositionY() < -5055.0f || creature->GetPositionY() > -4130.0f) || (creature->GetPositionZ() > 145.0f || creature->GetPositionZ() < -70.0f))
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < DATA_MAX_BOSS_DATA; i++)
                            if (i != DATA_GALAKRAS && i != DATA_IRON_JUGGERNAUT && i != DATA_KORKRON_DARK_SHAMANS && i != DATA_GENERAL_NAZGRIM)
                                SetBossState(i, DONE);
                    }
                    else if (lfrQuarter == QUARTER_THE_UNDERHOLD || lfrQuarter == QUARTER_THE_UNDERHOLD_F)
                    {
                        if ((creature->GetPositionX() > 2110.0f || creature->GetPositionX() < 1050.0f) || creature->GetPositionY() < -5380.0f || (creature->GetPositionZ() > -100.0f || creature->GetPositionZ() < -300.0f))
                            creature->DespawnOrUnsummon();
                        if (creature->GetEntry() == NPC_BLACKFUSE_KORKRON_OVERSEER || creature->GetEntry() == NPC_BLACKFUSE_SELLSWORD ||
                            creature->GetEntry() == NPC_BLACKFUSE_ENGINEER || creature->GetEntry() == NPC_GORODAN || creature->GetEntry() == NPC_AGGRON)
                            creature->SetVisible(false);
                        for (uint32 i = 0; i < DATA_MAX_BOSS_DATA; i++)
                            if (i != DATA_MALKOROK && i != DATA_SPOILS_OF_PANDARIA && i != DATA_THOK_THE_BLOODTHIRSTY)
                                SetBossState(i, DONE);
                    }
                    else if (lfrQuarter == QUARTER_DOWNFALL || lfrQuarter == QUARTER_DOWNFALL_F)
                    {
                        if (GameObject* go = instance->GetGameObject(GetGuidData(GO_MASSIVE_IRON_DOOR_BLACKFUSE)))
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                        if ((creature->GetPositionX() > 1920.0f || creature->GetPositionZ() > -235.0f) &&
                            creature->GetEntry() != NPC_SIEGECRAFTER_BLACKFUSE && creature->GetEntry() != NPC_SIEGECRAFTER_BLACKFUSE_1 && creature->GetEntry() != NPC_LASSER_ARRAY && creature->GetEntry() != NPC_SIEGECRAFTER_HELPER && creature->GetDBTableGUIDLow())
                            creature->DespawnOrUnsummon();
                        if (creature->GetEntry() == NPC_THOK_THE_BLOODTHIRSTY || creature->GetEntry() == NPC_KORKRON_JAILER_THRASH || creature->GetEntry() == NPC_STOREROOM_GUARD)
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < DATA_MAX_BOSS_DATA; i++)
                            if (i != DATA_SIEGECRAFTER_BLACKFUSE && i != DATA_PARAGONS_OF_THE_KLAXXI && i != DATA_GARROSH_HELLSCREAM)
                                SetBossState(i, DONE);
                    }
                }

                switch (creature->GetEntry())
                {
                    case NPC_IMMERSEUS:
                        immerseusGUID = creature->GetGUID();
                        break;
                    case NPC_ROOK_STONETOE:
                        rookGUID = creature->GetGUID();
                        break;
                    case NPC_HE_SOFTFOOT:
                        heGUID = creature->GetGUID();
                        break;
                    case NPC_SUN_TENDERHEART:
                        sunGUID = creature->GetGUID();
                        break;
                    case NPC_NORUSHEN:
                        norushenGUID = creature->GetGUID();
                        break;
                    case NPC_SHA_OF_PRIDE:
                        shaOfPrideGUID = creature->GetGUID();
                        break;
                    case NPC_AMALGAM_OF_CORRUPTION:
                        amalgamGUID = creature->GetGUID();
                        break;
                    case NPC_GALAKRAS:
                        galakrasGUID = creature->GetGUID();
                        break;
                    case NPC_IRON_JUGGERNAUT:
                        ironJuggernautGUID = creature->GetGUID();
                        break;
                    case NPC_EARTHBREAKER_HAROMM:
                        harommGUID = creature->GetGUID();
                        break;
                    case NPC_WAVEBINDER_KARDIS:
                        kardrisGUID = creature->GetGUID();
                        break;
                    case NPC_GENERAL_NAZGRIM:
                        nazgrimGUID = creature->GetGUID();
                        break;
                    case NPC_MALKOROK:
                        malkorokGUID = creature->GetGUID();
                        break;
                    case NPC_THOK_THE_BLOODTHIRSTY:
                        thokGUID = creature->GetGUID();
                        break;
                    case NPC_SPOILS_OF_PANDARIA:
                        spoilsOfPandariaGUID = creature->GetGUID();
                        break;
                    case NPC_SIEGECRAFTER_BLACKFUSE:
                        siegecrafterGUID = creature->GetGUID();
                        break;
                    case NPC_GARROSH_HELLSCREAM:
                        garroshGUID = creature->GetGUID();
                        break;
                    case NPC_PARAGON_OF_THE_KLAXXI:
                        paragonControllerGUID = creature->GetGUID();
                        break;
                    case NPC_MANTID_AMBER:
                        mantidAmberGUID = creature->GetGUID();
                        break;
                    case NPC_HEART_OF_YSHAARJ:
                        heartOfYshaarjGUID = creature->GetGUID();
                        creature->SetDisplayId(39810);
                        break;
                    case NPC_SHA_VORTEX:
                        ShaVortexGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_LASSER_ARRAY:
                        if (creature->GetDBTableGUIDLow() < 561190 || creature->GetDBTableGUIDLow() > 561204)
                            m_LasserArray.push_back(creature->GetGUID());
                        break;
                    case NPC_KILRUK_THE_WIND_REAVER:
                        kilrukGUID = creature->GetGUID();
                        break;
                    case NPC_XARIL_THE_POISONED_MIND:
                        xarilGUID = creature->GetGUID();
                        break;
                    case NPC_KAZTIK_THE_MANIPULATOR:
                        kaztikGUID = creature->GetGUID();
                        break;
                    case NPC_KORVEN_THE_PRIME:
                        korvenGUID = creature->GetGUID();
                        break;
                    case NPC_IYYOKUK_THE_LUCID:
                        iyyokukGUID = creature->GetGUID();
                        break;
                    case NPC_KAROZ_THE_LOCUST:
                        karozGUID = creature->GetGUID();
                        break;
                    case NPC_SKEER_THE_BLOODSEEKER:
                        skeerGUID = creature->GetGUID();
                        break;
                    case NPC_RIKKAL_THE_DISSECTOR:
                        rikkalGUID = creature->GetGUID();
                        break;
                    case NPC_HISEK_THE_SWARMKEEPER:
                        hisekGUID = creature->GetGUID();
                        break;
                    case NPC_LASER_TARGET_BUNNY:
                        creature->SetDisplayId(43164);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    case NPC_INVISIBLE_STALKER:
                        if (creature->GetPositionZ() < 250.0f)
                            invisStalkerGUID = creature->GetGUID();
                        break;
                    case NPC_LOREWALKER_CHO:
                        if (creature->GetPositionY() < 500.0f)
                            loreWalkerChoEntranceGUID = creature->GetGUID();
                        break;
                    case NPC_OOZE_CONTROLLER:
                        oozeControllerGUID = creature->GetGUID();
                        break;
                    case NPC_PRESSURE_PLATE:
                        pressurePlateGUID = creature->GetGUID();
                        break;
                    case NPC_QUARANTINE_MEASURES:
                        quarantineMeasureGUID = creature->GetGUID();
                        break;
                    case NPC_LORTHEMAR_PRIDE_ROOM:
                        creature->SetVisible(false);
                        lorthemarRoomGUID = creature->GetGUID();
                        break;
                    case NPC_JAINA_PRIDE_ROOM:
                        jainaRoomGUID = creature->GetGUID();
                        creature->SetVisible(false);
                        break;
                    case NPC_AYSA_CLOUDSINGER:
                        aysaGUID = creature->GetGUID();
                        break;
                    case NPC_OVERSEER_KOMAK:
                        overseerKomakGUID = creature->GetGUID();
                        break;
                    case NPC_JI_FIREPAW:
                        jiFirepawGUID = creature->GetGUID();
                        break;
                    case NPC_EITRIGG:
                        eitriggGUID = creature->GetGUID();
                        break;
                    case NPC_ZEAL:
                        zealGUID = creature->GetGUID();
                        break;
                    case NPC_PORTAL_TO_ORGRIMMAR:
                        creature->SetDisplayFromModel(2);
                        break;
                    case NPC_LORTHEMAR_THERON:
                        lorthemarGUID = creature->GetGUID();
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == ALLIANCE)
                            creature->SetVisible(false);
                        break;
                    case NPC_KING_VARIAN_WRYNN:
                        varianGUID = creature->GetGUID();
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == HORDE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_LADY_SYLVANAS_WINDRUNNER:
                        sylvanasGUID = creature->GetGUID();
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == ALLIANCE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_LADY_JAINA_PROUDMOORE:
                        jainaGUID = creature->GetGUID();
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == HORDE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_ARCHMAGE_AETHAS_SUNREAVER:
                        aethasGUID = creature->GetGUID();
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == ALLIANCE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_VEREESA_WINDRUNNER:
                        vereesaGUID = creature->GetGUID();
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == HORDE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_SUNREAVER_MAGUS:
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALLIANCE_VANGUARD_2, ALLIANCE);
                        break;
                    case NPC_SUNREAVER_SENTINEL:
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALLIANCE_VANGUARD, ALLIANCE);
                        break;
                    case NPC_SUNREAVER_CONSTRUCT:
                        galakrasIntruderGUIDs.push_back(creature->GetGUID());

                        if (GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->SetVisible(false);

                        if (teamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ALLIANCE_VANGUARD_1, ALLIANCE);
                        break;
                    case NPC_ZAELA:
                        zaelaGUID = creature->GetGUID();
                        break;
                    case NPC_KORKRON_ELITE_SNIPER:
                        korkronSniperGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_NORUSHEN_2:
                        norushenRoomGUID = creature->GetGUID();
                        break;
                    case NPC_BLOOD_POOL:
                        creature->SetDisplayFromModel(0);
                        break;
                    case NPC_EMBODIED_ANGUISH:
                        anguishGUID = creature->GetGUID();
                        break;
                    case NPC_VOLJIN:
                        voljinGUID = creature->GetGUID();
                        break;
                    case NPC_BAINE_BLOODHOOF:
                        baineGUID = creature->GetGUID();
                        break;
                    case NPC_NAZGRIM_PRE_EVENT:
                        nazgrimPreEventGUID = creature->GetGUID();
                        break;
                    case NPC_TYRANDE:
                        tyrandeGUID = creature->GetGUID();
                        break;
                    case NPC_GLAIWE_THROWER:
                        glaiweGUID = creature->GetGUID();
                        break;
                    case NPC_DOYODA:
                        doyodaGUID = creature->GetGUID();
                        break;
                    case NPC_WILDHUMMER_SHAMAN:
                        wildhummerShamanGUID = creature->GetGUID();
                        break;
                    case NPC_OVERLORD_RUNTHAK:
                        runthakGUID = creature->GetGUID();
                        break;
                    case NPC_GROLDAR:
                        groldarGUID = creature->GetGUID();
                        break;
                    case NPC_MALKOROK_CONTROLLER:
                        malkorokControllerGUID = creature->GetGUID();
                        break;
                    case NPC_ANCIENT_MIASMA:
                        miasmaGUID = creature->GetGUID();
                        break;
                    case NPC_SLG_GENERIC_MOP:
                        creature->SetDisplayFromModel(1);
                        break;
                    case NPC_AKOLIK:
                        akolikGUID = creature->GetGUID();
                        break;
                    case NPC_WATERSPEAKER_GORAI:
                        goraiGUID = creature->GetGUID();
                        break;
                    case NPC_MONTAK:
                        montakGUID = creature->GetGUID();
                        break;
                    case NPC_MANTID_SPOILS_1:
                        mantidSpoils1GUID = creature->GetGUID();
                        break;
                    case NPC_MANTID_SPOILS_2:
                        mantidSpoils2GUID = creature->GetGUID();
                        break;
                    case NPC_MOGU_SPOILS_1:
                        moguSpoils1GUID = creature->GetGUID();
                        break;
                    case NPC_MOGU_SPOILS_2:
                        moguSpoils2GUID = creature->GetGUID();
                        break;
                    case NPC_BLACKFUSE_SELLSWORD: // temp
                    case NPC_BLACKFUSE_ENGINEER:
                    case NPC_BLACKFUSE_KORKRON_OVERSEER:
                        if (creature->GetPositionZ() < -300.0f)
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        break;
                    case NPC_AGGRON:
                        aggronGUID = creature->GetGUID();
                        break;
                    case NPC_GORODAN:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        gorodanGUID = creature->GetGUID();
                        break;
                    case NPC_SHANNA_SPARKFIZZ:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        shannaGUID = creature->GetGUID();
                        break;
                    case NPC_SIEGECRAFTER_HELPER:
                        siegeCrafterHelperGUID = creature->GetGUID();
                        break;
                    case NPC_ANTI_AIR_CANNON:
                        if (creature->GetPositionX() > 1400.0f)
                            northCannonGUID = creature->GetGUID();
                        else
                            southCannonGUID = creature->GetGUID();
                        break;
                    case NPC_ROPE:
                        if (creature->GetPositionX() > 1400.0f)
                            northRopeGUID = creature->GetGUID();
                        else
                            southRopeGUID = creature->GetGUID();
                        break;
                    case NPC_SIEGECRAFTER_BLACKFUSE_1:
                        siegecrafterVehicleGUID = creature->GetGUID();
                        break;
                    case NPC_THRALL_GARROSH_EVENT:
                        thrallGUID = creature->GetGUID();
                        break;
                    case NPC_STORMWIND_ROYAL_GUARD:
                    case NPC_ORGRIMMAR_GRUNT:
                    case NPC_IRONFORGE_GUARD:
                    case NPC_DARNASSUS_SENTINEL:
                    case NPC_TYRANDE_WHISPERWIND_GARROSH:
                    case NPC_ANDUIN_WRYNN_GARROSH:
                    case NPC_TINKER_MEKKATORQUE_GARROSH:
                    case NPC_LOREWALKER_CHO_GARROSH:
                    case NPC_TARAN_ZHU_GARROSH:
                    case NPC_GALLYWIX_GARROSH:
                    case NPC_SYLVANAS_GARROSH:
                    case NPC_THRALL_GARROSH:
                    case NPC_VOLJIN_GARROSH:
                    case NPC_BAINE_GARROSH:
                    case NPC_VARIAN_GARROSH:
                        if (GetBossState(DATA_GARROSH_HELLSCREAM) == DONE)
                            break;

                        creature->SetVisible(false);
                        garroshOutroGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_MOIRA_GARROSH:
                        if (creature->GetPositionZ() > 0 || GetBossState(DATA_GARROSH_HELLSCREAM) == DONE)
                            break;

                        creature->SetVisible(false);
                        garroshOutroGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_KORKRON_GUNNER_RP:
                        if (creature->GetPositionX() < 2040.0f)
                            korkronGunnerGUID = creature->GetGUID();
                        else
                            korkronGunnerGUID_2 = creature->GetGUID();
                        break;
                    case NPC_GAMON:
                        gamonGUID = creature->GetGUID();
                        break;
                    case NPC_OVERLORD_SAURFANG:
                        saurfangGUID = creature->GetGUID();
                        break;
                    case NPC_KORKRON_TRAINING_DUMMY:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        break;
                    case NPC_GUNSHIP_MAIN_CANNON:
                        gunshipMainCannonGUID = creature->GetGUID();
                        break;
                    case NPC_STARVED_YETI_TRASH:
                        if (creature->GetPositionX() < 1190.0f &&creature->GetPositionX() > 1180.0f)
                            starvedYetiThrashGUID = creature->GetGUID();
                        break;
                    case NPC_ENRAGED_MUSHAN_BEAST:
                        if (creature->GetPositionX() < 1155.0f &&creature->GetPositionX() > 1153.0f)
                            enragedMushanThrashGUID = creature->GetGUID();
                        break;
                    case NPC_MANIFESTATION_OF_RAGE:
                        creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                        break;
                    case NPC_BEACH_CANNON:
                        if (!creature->IsAlive() && GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            creature->Respawn();
                        break;
                    case NPC_AQUEOUS_DEFENDER:
                        if (!creature->IsAlive() && GetData(DATA_AQUEOUS_DEFENDERS_FALLEN) != DONE)
                            creature->Respawn();
                        break;
                    case NPC_KORKRON_BLOOD_AXE:
                    case NPC_KORKRON_GUNNER:
                    case NPC_KORKRON_SKULL_SPLITTER:
                        if (creature->GetWaypointPath())
                            korkronBarrackGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_KORKRON_DARK_FARSEER:
                        if (creature->GetDBTableGUIDLow() == 558933)
                            darkFarseerBarracksGUID = creature->GetGUID();
                        else if (creature->GetDBTableGUIDLow() == 559010)
                            darkFarseerSkullsplitterGUID = creature->GetGUID();
                        break;
                    case NPC_SHIMRA:
                    case NPC_TRAK_GEN:
                    case NPC_ZAZO:
                    case NPC_GOMA:
                    case NPC_SAROK:
                        korkronPrisonerGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_THERAMORE_CITIZEN:
                        if (creature->GetPositionX() > 1500.0f)
                            theramoreCitizenGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_MONSTROUS_GASTROPOD:
                        gastropodGUID = creature->GetGUID();
                        break;
                }

                if (isFlex)
                {
                    AddFlexCreature(creature->GetGUID());
                    UpdateDynamicHealth(creature->GetGUID());
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit && unit->ToPlayer() && unit->ToPlayer()->HasAura(143840))
                    if (Creature* anguish = instance->GetCreature(GetGuidData(NPC_EMBODIED_ANGUISH)))
                        anguish->AI()->DoAction(2);

                if (unit && unit->ToCreature() && unit->ToCreature()->GetEntry() == NPC_BEACH_CANNON)
                    SetData(DATA_BEACH_CANNONS_COUNT, GetData(DATA_BEACH_CANNONS_COUNT) + 1);
            }

            void OnGameObjectCreate(GameObject* go) override 
            {
                switch (go->GetEntry())
                {
                    case GO_NAZGRIM_EXIT_DOOR:
                        if (!isLfr)
                            AddDoor(go, true);
                        break;
                    case GO_NORUSHEN_EXIT:
                    case GO_IMMERSEUS_DOOR:
                    case GO_SHA_FIELD:
                    case GO_PRIDE_ENTRANCE:
                    case GO_JUGGERNAUT_WIND_DOOR:
                    case GO_NAZGRIM_ENTRY_DOOR:
                    case GO_NORUSHEN_LIGHT_CONTAIN_WALL1:
                    case GO_NORUSHEN_LIGHT_CONTAIN_WALL2:
                    case GO_NORUSHEN_LIGHT_CONTAIN_WALL3:
                    case GO_NORUSHEN_LIGHT_CONTAIN_WALL4:
                    case GO_NORUSHEN_LIGHT_CONTAIN_CEIL:
                    case GO_NORUSHEN_LIGHT_CONTAIN_FLOOR:
                    case GO_RUSTED_IRON_PORTCULLIS:
                    case GO_DARK_SHAMANS_FRONTGATE:
                    case GO_JAGGED_IRON_GATE_ENTRANCE:
                    case GO_JAGGED_IRON_GATE_EXIT:
                    case GO_SPOILS_IRON_GATE_1:
                    case GO_SPOILS_IRON_GATE_2:
                    case GO_SPOILS_IRON_GATE_3:
                    case GO_SPOILS_IRON_GATE_4:
                    case GO_MASSIVE_IRONBOUND_DOOR:
                    case GO_THOK_ENTRANCE:
                    case GO_PARAGON_ARENA_WALL:
                    case GO_PARAGON_EXIT:
                    case GO_ENTRANCE_RIGHT:
                    case GO_ENTRANCE_LEFT:
                        AddDoor(go, true);
                        break;
                    case GO_MASSIVE_ANCIENT_DOOR:
                        massiveAncientDoorGUID = go->GetGUID();

                        if (GetData(DATA_FALLEN_PROTECTORS_PRE_EVENT) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_GARROSH_SHA_VORTEX:
                        ShaVortexGoGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_SHA_ENERGY_WALL:
                        shaEnergyWallGUID = go->GetGUID();

                        if (GetData(DATA_NORUSHEN_PRE_EVENT_1) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_MYSTERIOUS_ORNATE_DOOR:
                        mysteriousDoorGUID = go->GetGUID();

                        if (GetData(DATA_NORUSHEN_PRE_EVENT_1) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_TEARS_OF_THE_VALE:
                        immerseusChestGUID = go->GetGUID();
                        break;
                    case GO_TEARS_OF_THE_VALE_HC:
                        immerseusChestHeroicGUID = go->GetGUID();
                        break;
                    case GO_VAULT_OF_FORBIDDEN_TREASURES:
                        shaChestGUID = go->GetGUID();
                        break;
                    case GO_VAULT_OF_FORBIDDEN_TREASURES_HC:
                        shaChestHeroicGUID = go->GetGUID();
                        break;
                    case GO_PORTCULLIS:
                        if (go->GetPositionX() > 1400.0f)
                            northTowerDoorGUID = go->GetGUID();
                        else
                            southTowerDoorGUID = go->GetGUID();
                        break;
                    case GO_THE_GATES_OF_ORGRIMMAR:
                        orgrimmarGatesGUID = go->GetGUID();

                        if (GetData(DATA_IRON_JUGGERNAUT_PAST_EVENT) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_REX_DOOR:
                        rexDoorGUID = go->GetGUID();

                        if (GetData(DATA_THOK_BLOODTHIRSTY_PRE_EVENT) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_SPOILS_CHEST:
                        spoilsChestGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_CHEST_HC:
                        spoilsHeroicChestGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_ACCESS_LOW_LEFT:
                        spoilsAccessLowLeftGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_ACCESS_LOW_RIGHT:
                        spoilsAccessLowRightGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_ACCESS_HIGH_RIGHT:
                        spoilsAccessHighRightGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_ACCESS_HIGH_LEFT:
                        spoilsAccessHighLeftGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_CROSS_RIGHT:
                        spoilsCrossRightGUID = go->GetGUID();
                        break;
                    case GO_SPOILS_CROSS_LEFT:
                        spoilsCrossLeftGUID = go->GetGUID();
                        break;
                    case GO_MANTID_LOW_BOX_1:
                    case GO_MANTID_LOW_BOX_2:
                    case GO_MANTID_LOW_BOX_3:
                    case GO_MANTID_LOW_BOX_4:
                    case GO_MANTID_LOW_BOX_5:
                    case GO_MANTID_LOW_BOX_6:
                    case GO_MANTID_LOW_BOX_7:
                    case GO_MANTID_LOW_BOX_8:
                    case GO_MANTID_LOW_BOX_9:
                    case GO_MANTID_LOW_BOX_10:
                    case GO_MANTID_LOW_BOX_11:
                    case GO_MANTID_LOW_BOX_12:
                    case GO_MANTID_LOW_BOX_13:
                    case GO_MANTID_LOW_BOX_14:
                    case GO_MANTID_LOW_BOX_15:
                    case GO_MANTID_LOW_BOX_16:
                    case GO_MANTID_LOW_BOX_17:
                    case GO_MANTID_LOW_BOX_18:
                    case GO_MANTID_LOW_BOX_19:
                    case GO_MANTID_LOW_BOX_20:
                    case GO_MANTID_LOW_BOX_21:
                    case GO_MANTID_LOW_BOX_22:
                    case GO_MANTID_LOW_BOX_23:
                    case GO_MANTID_LOW_BOX_24:
                    case GO_MANTID_LOW_BOX_25:
                    case GO_MANTID_LOW_BOX_26:
                    case GO_MANTID_LOW_BOX_27:
                    case GO_MANTID_LOW_BOX_28:
                    case GO_MANTID_LOW_BOX_29:
                    case GO_MANTID_LOW_BOX_30:
                    case GO_MANTID_LOW_BOX_31:
                    case GO_MANTID_LOW_BOX_32:
                    case GO_MANTID_LOW_BOX_33:
                    case GO_MANTID_LOW_BOX_34:
                    case GO_MANTID_LOW_BOX_35:
                    case GO_MANTID_LOW_BOX_36:
                    case GO_MANTID_MIDDLE_BOX_1:
                    case GO_MANTID_MIDDLE_BOX_2:
                    case GO_MANTID_MIDDLE_BOX_3:
                    case GO_MANTID_MIDDLE_BOX_4:
                    case GO_MANTID_MIDDLE_BOX_5:
                    case GO_MANTID_MIDDLE_BOX_6:
                    case GO_MANTID_MIDDLE_BOX_7:
                    case GO_MANTID_MIDDLE_BOX_8:
                    case GO_MANTID_MIDDLE_BOX_9:
                    case GO_MANTID_MIDDLE_BOX_10:
                    case GO_MANTID_MIDDLE_BOX_11:
                    case GO_MANTID_MIDDLE_BOX_12:
                    case GO_MANTID_HIGH_BOX_1:
                    case GO_MANTID_HIGH_BOX_2:
                    case GO_MANTID_HIGH_BOX_3:
                    case GO_MANTID_HIGH_BOX_4:
                    case GO_MOGU_LOW_BOX_1:
                    case GO_MOGU_LOW_BOX_2:
                    case GO_MOGU_LOW_BOX_3:
                    case GO_MOGU_LOW_BOX_4:
                    case GO_MOGU_LOW_BOX_5:
                    case GO_MOGU_LOW_BOX_6:
                    case GO_MOGU_LOW_BOX_7:
                    case GO_MOGU_LOW_BOX_8:
                    case GO_MOGU_LOW_BOX_9:
                    case GO_MOGU_LOW_BOX_10:
                    case GO_MOGU_LOW_BOX_11:
                    case GO_MOGU_LOW_BOX_12:
                    case GO_MOGU_LOW_BOX_13:
                    case GO_MOGU_LOW_BOX_14:
                    case GO_MOGU_LOW_BOX_15:
                    case GO_MOGU_LOW_BOX_16:
                    case GO_MOGU_LOW_BOX_17:
                    case GO_MOGU_LOW_BOX_18:
                    case GO_MOGU_LOW_BOX_19:
                    case GO_MOGU_LOW_BOX_20:
                    case GO_MOGU_LOW_BOX_21:
                    case GO_MOGU_LOW_BOX_22:
                    case GO_MOGU_LOW_BOX_23:
                    case GO_MOGU_LOW_BOX_24:
                    case GO_MOGU_LOW_BOX_25:
                    case GO_MOGU_LOW_BOX_26:
                    case GO_MOGU_LOW_BOX_27:
                    case GO_MOGU_LOW_BOX_28:
                    case GO_MOGU_LOW_BOX_29:
                    case GO_MOGU_LOW_BOX_30:
                    case GO_MOGU_LOW_BOX_31:
                    case GO_MOGU_LOW_BOX_32:
                    case GO_MOGU_LOW_BOX_33:
                    case GO_MOGU_LOW_BOX_34:
                    case GO_MOGU_LOW_BOX_35:
                    case GO_MOGU_LOW_BOX_36:
                    case GO_MOGU_MIDDLE_BOX_1:
                    case GO_MOGU_MIDDLE_BOX_2:
                    case GO_MOGU_MIDDLE_BOX_3:
                    case GO_MOGU_MIDDLE_BOX_4:
                    case GO_MOGU_MIDDLE_BOX_5:
                    case GO_MOGU_MIDDLE_BOX_6:
                    case GO_MOGU_MIDDLE_BOX_7:
                    case GO_MOGU_MIDDLE_BOX_8:
                    case GO_MOGU_MIDDLE_BOX_9:
                    case GO_MOGU_MIDDLE_BOX_10:
                    case GO_MOGU_MIDDLE_BOX_11:
                    case GO_MOGU_MIDDLE_BOX_12:
                    case GO_MOGU_HIGH_BOX_1:
                    case GO_MOGU_HIGH_BOX_2:
                    case GO_MOGU_HIGH_BOX_3:
                    case GO_MOGU_HIGH_BOX_4:
                    case NPC_CRATE_OF_PANDAREN_RELIC_1:
                    case NPC_CRATE_OF_PANDAREN_RELIC_2:
                    case NPC_CRATE_OF_PANDAREN_RELIC_3:
                    case NPC_CRATE_OF_PANDAREN_RELIC_4:
                    case NPC_CRATE_OF_PANDAREN_RELIC_5:
                    case NPC_CRATE_OF_PANDAREN_RELIC_6:
                    case NPC_CRATE_OF_PANDAREN_RELIC_7:
                    case NPC_CRATE_OF_PANDAREN_RELIC_8:
                        boxesGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_DEFENSE_SYSTEM_LEVER_LOW_RIGHT:
                        leverLowRightGUID = go->GetGUID();
                        leverGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_DEFENSE_SYSTEM_LEVER_UP_RIGHT:
                        leverUpRightGUID = go->GetGUID();
                        leverGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_DEFENSE_SYSTEM_LEVER_UP_LEFT:
                        leverUpLeftGUID = go->GetGUID();
                        leverGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_DEFENSE_SYSTEM_LEVER_LOW_LEFT:
                        leverLowLeftGUID = go->GetGUID();
                        leverGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_SECURED_STOCKPILE_OF_SPOILS:
                        cosmeticSpoilsGUID = go->GetGUID();
                        break;
                    case GO_MASSIVE_IRON_DOOR_BLACKFUSE:
                        siegecrafterDoorGUID = go->GetGUID();

                        if (GetBossState(DATA_THOK_THE_BLOODTHIRSTY) == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                        break;
                    case GO_GARROSH_SOUTH_WEST_DOOR:
                        garroshWestGatesGUID = go->GetGUID();
                        break;
                    case GO_GARROSH_SOUTH_EAST_DOOR:
                        garroshEastGatesGUID = go->GetGUID();
                        break;
                    case GO_PARAGON_ENTRANCE:
                        if (GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == DONE)
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                        paragonEntranceGUID = go->GetGUID();
                        break;
                    case GO_ENCASE_IN_AMBER:
                        encaseInAmberGUID = go->GetGUID();
                        break;
                    case GO_THRICE_LOCKED_CAGE:
                        thriceLockedCageGUID = go->GetGUID();
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_GALAKRAS:
                        if (state == DONE)
                        {
                            for (auto&& itr : korkronSniperGUIDs)
                                if (Creature* korkronSniper = instance->GetCreature(itr))
                                    korkronSniper->DespawnOrUnsummon();
                        }
                        break;
                    case DATA_SIEGECRAFTER_BLACKFUSE:
                        switch (state)
                        {
                            case IN_PROGRESS:
                                for (auto&& itr : m_LasserArray)
                                    if (Creature* LasserArray = instance->GetCreature(itr))
                                        LasserArray->AI()->DoAction(ACTION_CONVEYOR_RUN);
                                break;
                            case FAIL:
                            case NOT_STARTED:
                                for (auto&& itr : m_LasserArray)
                                    if (Creature* LasserArray = instance->GetCreature(itr))
                                        LasserArray->AI()->DoAction(ACTION_CONVEYOR_RESET);
                                break;
                            case DONE:
                                for (auto&& itr : m_LasserArray)
                                    if (Creature* LasserArray = instance->GetCreature(itr))
                                        LasserArray->AI()->DoAction(ACTION_CONVEYOR_RESET);

                                if (GameObject* go = instance->GetGameObject(GetGuidData(GO_PARAGON_ENTRANCE)))
                                    go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                break;
                        }
                        break;
                    case DATA_SPOILS_OF_PANDARIA:
                    {
                        switch (state)
                        {
                            case IN_PROGRESS:
                                for (auto&& itr : boxesGUIDs)
                                    if (GameObject* spoil = instance->GetGameObject(itr))
                                        spoil->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                break;
                            case FAIL:
                            case DONE:
                            case NOT_STARTED:
                                for (auto&& itr : boxesGUIDs)
                                {
                                    if (GameObject* spoil = instance->GetGameObject(itr))
                                    { 
                                        spoil->SetGoState(GO_STATE_READY);
                                        spoil->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                                        spoil->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                        spoil->SetLootState(GO_NOT_READY);
                                    }
                                }

                                for (auto&& itr : leverGUIDs)
                                {
                                    if (GameObject* lever = instance->GetGameObject(itr))
                                    {
                                        lever->SetGoState(GO_STATE_READY);
                                        lever->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                                        lever->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                        lever->SetLootState(GO_NOT_READY);
                                    }
                                }
                                break;
                        }
                    }
                    case DATA_THOK_THE_BLOODTHIRSTY:
                        if (isFlex || isLfr || state != DONE)
                            break;

                        if (GameObject* go = instance->GetGameObject(GetGuidData(GO_MASSIVE_IRON_DOOR_BLACKFUSE)))
                            go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                        break;
                    case DATA_GARROSH_HELLSCREAM:
                        if (state == DONE)
                        {
                            for (auto&& itr : garroshOutroGUIDs)
                                if (Creature* outroMember = instance->GetCreature(itr))
                                    outroMember->SetVisible(true);

                            if (Creature* thrall = instance->GetCreature(GetGuidData(NPC_THRALL_GARROSH_EVENT)))
                                thrall->SetVisible(false);
                        }
                        break;
                }

                PromotionIfLFR(state);
                return true;
            }

            void SetData(uint32 type, uint32 data) override 
            {
                switch (type)
                {
                    case DATA_SHA_VORTEX:
                        for (auto&& sVortex : ShaVortexGUIDs)
                        {
                            if (Creature* ShaVortex = instance->GetCreature(sVortex))
                            {
                                if (data == DONE)
                                    ShaVortex->RemoveAurasDueToSpell(144920);
                                else
                                    ShaVortex->CastSpell(ShaVortex, 144920, true);
                            }
                        }

                        for (auto&& sVortex : ShaVortexGoGUIDs)
                            if (GameObject* ShaVortex = instance->GetGameObject(sVortex))
                                HandleGameObject(ShaVortex->GetGUID(), data == DONE, NULL);
                        break;
                    case DATA_FALLEN_PROTECTORS_PRE_EVENT:
                        fallenProtectorsPreEvent = data;
                        break;
                    case DATA_NORUSHEN_PRE_EVENT_1:
                        norushenPreEvent = data;
                        HandleGameObject(GetGuidData(GO_SHA_ENERGY_WALL), true, NULL);
                        HandleGameObject(GetGuidData(GO_MYSTERIOUS_ORNATE_DOOR), true, NULL);
                        break;
                    case DATA_NORUSHEN_PRE_EVENT_2:
                        norushenPastEvent = data;
                        break;
                    case DATA_SHA_OF_PRIDE_PRE_EVENT:
                        shaofPridePreEvent = data;
                        break;
                    case DATA_SHA_OF_PRIDE_PAST_EVENT:
                        shaofPridePastEvent = data;

                        if (Creature* jaina = instance->GetCreature(GetGuidData(NPC_JAINA_PRIDE_ROOM)))
                            jaina->AI()->DoAction(ACTION_START_INTRO);
                        break;
                    case DATA_GALAKRAS_PRE_EVENT:
                        galakrasPreEvent = data;
                        /*DoUpdateWorldState(WORLDSTATE_CANNONS_REMAINING, 1);
                        DoUpdateWorldState(WORLDSTATE_CANNONS_REMAINING_COUNT, 7);*/

                        // Allow to start encounter
                        if (galakrasPreEvent == DONE)
                        {
                            for (auto&& itr : galakrasIntruderGUIDs)
                            {
                                if (Creature* intruder = instance->GetCreature(itr))
                                {
                                    if (teamInInstance == ALLIANCE && (intruder->GetEntry() == NPC_ARCHMAGE_AETHAS_SUNREAVER || intruder->GetEntry() == NPC_LORTHEMAR_THERON || intruder->GetEntry() == NPC_LADY_SYLVANAS_WINDRUNNER))
                                        continue;

                                    intruder->SetVisible(true);
                                    intruder->CastSpell(intruder, SPELL_SIMPLE_TELEPORT, true);
                                }
                            }
                        }

                        SaveToDB();
                        break;
                    case DATA_IRON_JUGGERNAUT_PAST_EVENT:
                        ironJuggernautPastEvent = data;
                        break;
                    case DATA_THOK_BLOODTHIRSTY_PRE_EVENT:
                        thokBloodthirstyPreEvent = data;
                        break;
                    case DATA_SHAMANS_PRISON:
                        shamansTotemEvent = data;
                        break;
                    case DATA_SIEGECRAFTER_PRE_EVENT:
                        siegecrafterPreEvent = data;
                        break;
                    case DATA_GARROSH_HELLSCREAM_PREVE_EVENT:
                        garroshHellscreamPreEvent = data;
                        break;
                    case DATA_BEACH_CANNONS_COUNT:
                        if (++beachCannonsCount > 6 && GetData(DATA_GALAKRAS_PRE_EVENT) != DONE)
                            SetData(DATA_GALAKRAS_PRE_EVENT, DONE);
                        break;
                    case DATA_AQUEOUS_DEFENDERS_FALLEN_COUNT:
                        if (++aquaDefendersCount > 2 && GetData(DATA_AQUEOUS_DEFENDERS_FALLEN) != DONE)
                            SetData(DATA_AQUEOUS_DEFENDERS_FALLEN, DONE);
                        break;
                    case DATA_AQUEOUS_DEFENDERS_FALLEN:
                        aquaDefendersEvent = data;

                        if (data == DONE)
                            if (Creature* immerseus = instance->GetCreature(GetGuidData(DATA_IMMERSEUS)))
                                immerseus->AI()->DoAction(ACTION_AQUEOUS_DEFENDERS);
                        break;
                    case DATA_KORKRON_BARRACKS_EVENT:
                        for (auto&& itr : korkronBarrackGUIDs)
                            if (Creature* korkronSoldier = instance->GetCreature(itr))
                                korkronSoldier->AI()->DoAction(ACTION_START_INTRO);
                        break;
                    case DATA_MOJKA_PRISONERS_COMBAT:
                        for (auto&& itr : korkronPrisonerGUIDs)
                            if (Creature* prisoner = instance->GetCreature(itr))
                                prisoner->SetInCombatWithZone();
                        break;
                    case DATA_MOJKA_PRISONERS_LEAVE:
                        for (auto&& itr : korkronPrisonerGUIDs)
                            if (Creature* prisoner = instance->GetCreature(itr))
                                if (prisoner->IsAlive())
                                    prisoner->AI()->DoAction(ACTION_START_INTRO);
                        break;
                    case DATA_THATUNG_PRISONERS_LEAVE:
                        for (auto&& itr : theramoreCitizenGUIDs)
                            if (Creature* prisoner = instance->GetCreature(itr))
                                if (prisoner->IsAlive())
                                    prisoner->AI()->DoAction(ACTION_START_INTRO);
                        break;
                    case DATA_PRISONERS_COUNT:
                        if (++rescuePrisonersCount > 8 && !instance->GetWorldState(WORLDSTATE_RESCUE_RAIDERS))
                            instance->SetWorldState(WORLDSTATE_RESCUE_RAIDERS, 1);
                        break;
                    case DATA_PARAGON_PRE_EVENT:
                        paragonPreEvent = data;
                        break;
                }

                if (data == DONE || data == SPECIAL)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_FALLEN_PROTECTORS_PRE_EVENT:
                        return fallenProtectorsPreEvent;
                    case DATA_NORUSHEN_PRE_EVENT_1:
                        return norushenPreEvent;
                    case DATA_NORUSHEN_PRE_EVENT_2:
                        return norushenPastEvent;
                    case DATA_SHA_OF_PRIDE_PRE_EVENT:
                        return shaofPridePreEvent;
                    case DATA_SHA_OF_PRIDE_PAST_EVENT:
                        return shaofPridePastEvent;
                    case DATA_GALAKRAS_PRE_EVENT:
                        return galakrasPreEvent;
                    case DATA_IRON_JUGGERNAUT_PAST_EVENT:
                        return ironJuggernautPastEvent;
                    case DATA_THOK_BLOODTHIRSTY_PRE_EVENT:
                        return thokBloodthirstyPreEvent;
                    case DATA_GROUP_FACTION:
                        return teamInInstance == ALLIANCE ? 0 : 1;
                    case DATA_LFR:
                        return isLfr ? 1 : 0;
                    case DATA_FLEX:
                        return isFlex ? 1 : 0;
                    case DATA_SHAMANS_PRISON:
                        return shamansTotemEvent;
                    case DATA_SIEGECRAFTER_PRE_EVENT:
                        return siegecrafterPreEvent;
                    case DATA_GARROSH_HELLSCREAM_PREVE_EVENT:
                        return garroshHellscreamPreEvent;
                    case DATA_AQUEOUS_DEFENDERS_FALLEN_COUNT:
                        return aquaDefendersCount;
                    case DATA_AQUEOUS_DEFENDERS_FALLEN:
                        return aquaDefendersEvent;
                    case DATA_PARAGON_PRE_EVENT:
                        return paragonPreEvent;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_IMMERSEUS:
                        return immerseusGUID;
                    case DATA_FALLEN_PROTECTORS:
                    case NPC_ROOK_STONETOE:
                        return rookGUID;
                    case NPC_HE_SOFTFOOT:
                        return heGUID;
                    case NPC_SUN_TENDERHEART:
                        return sunGUID;
                    case NPC_NORUSHEN:
                        return norushenGUID;
                    case DATA_SHA_OF_PRIDE:
                        return shaOfPrideGUID;
                    case DATA_GALAKRAS:
                        return galakrasGUID;
                    case DATA_IRON_JUGGERNAUT:
                        return ironJuggernautGUID;
                    case DATA_KORKRON_DARK_SHAMANS:
                        return harommGUID;
                    case DATA_GENERAL_NAZGRIM:
                        return nazgrimGUID;
                    case DATA_MALKOROK:
                        return malkorokGUID;
                    case DATA_SPOILS_OF_PANDARIA:
                        return spoilsOfPandariaGUID;
                    case DATA_THOK_THE_BLOODTHIRSTY:
                        return thokGUID;
                    case DATA_SIEGECRAFTER_BLACKFUSE:
                        return siegecrafterGUID;
                    case DATA_GARROSH_HELLSCREAM:
                        return garroshGUID;
                    case NPC_KILRUK_THE_WIND_REAVER:
                        return kilrukGUID;
                    case NPC_XARIL_THE_POISONED_MIND:
                        return xarilGUID;
                    case NPC_KAZTIK_THE_MANIPULATOR:
                        return kaztikGUID;
                    case NPC_KORVEN_THE_PRIME:
                        return korvenGUID;
                    case NPC_IYYOKUK_THE_LUCID:
                        return iyyokukGUID;
                    case NPC_KAROZ_THE_LOCUST:
                        return karozGUID;
                    case NPC_SKEER_THE_BLOODSEEKER:
                        return skeerGUID;
                    case NPC_RIKKAL_THE_DISSECTOR:
                        return rikkalGUID;
                    case NPC_HISEK_THE_SWARMKEEPER:
                        return hisekGUID;
                    case NPC_INVISIBLE_STALKER:
                        return invisStalkerGUID;
                    case NPC_LOREWALKER_CHO:
                        return loreWalkerChoEntranceGUID;
                    case NPC_OOZE_CONTROLLER:
                        return oozeControllerGUID;
                    case GO_MASSIVE_ANCIENT_DOOR:
                        return massiveAncientDoorGUID;
                    case NPC_PRESSURE_PLATE:
                        return pressurePlateGUID;
                    case GO_SHA_ENERGY_WALL:
                        return shaEnergyWallGUID;
                    case GO_MYSTERIOUS_ORNATE_DOOR:
                        return mysteriousDoorGUID;
                    case NPC_AMALGAM_OF_CORRUPTION:
                        return amalgamGUID;
                    case NPC_QUARANTINE_MEASURES:
                        return quarantineMeasureGUID;
                    case NPC_WAVEBINDER_KARDIS:
                        return kardrisGUID;
                    case NPC_AYSA_CLOUDSINGER:
                        return aysaGUID;
                    case NPC_OVERSEER_KOMAK:
                        return overseerKomakGUID;
                    case NPC_JI_FIREPAW:
                        return jiFirepawGUID;
                    case NPC_EITRIGG:
                        return eitriggGUID;
                    case GO_TEARS_OF_THE_VALE:
                        return immerseusChestGUID;
                    case GO_TEARS_OF_THE_VALE_HC:
                        return immerseusChestHeroicGUID;
                    case NPC_ZEAL:
                        return zealGUID;
                    case GO_VAULT_OF_FORBIDDEN_TREASURES:
                        return shaChestGUID;
                    case GO_VAULT_OF_FORBIDDEN_TREASURES_HC:
                        return shaChestHeroicGUID;
                    case NPC_LORTHEMAR_THERON:
                        return lorthemarGUID;
                    case NPC_LADY_SYLVANAS_WINDRUNNER:
                        return sylvanasGUID;
                    case NPC_ARCHMAGE_AETHAS_SUNREAVER:
                        return aethasGUID;
                    case NPC_ZAELA:
                        return zaelaGUID;
                    case NPC_NORUSHEN_2:
                        return norushenRoomGUID;
                    case NPC_LORTHEMAR_PRIDE_ROOM:
                        return lorthemarRoomGUID;
                    case NPC_JAINA_PRIDE_ROOM:
                        return jainaRoomGUID;
                    case NPC_EMBODIED_ANGUISH:
                        return anguishGUID;
                    case GO_PORTCULLIS:
                        return southTowerDoorGUID;
                    case GO_PORTCULLIS + 1:
                        return northTowerDoorGUID;
                    case NPC_VOLJIN:
                        return voljinGUID;
                    case NPC_BAINE_BLOODHOOF:
                        return baineGUID;
                    case GO_THE_GATES_OF_ORGRIMMAR:
                        return orgrimmarGatesGUID;
                    case NPC_NAZGRIM_PRE_EVENT:
                        return nazgrimPreEventGUID;
                    case NPC_TYRANDE:
                        return tyrandeGUID;
                    case NPC_GLAIWE_THROWER:
                        return glaiweGUID;
                    case NPC_DOYODA:
                        return doyodaGUID;
                    case NPC_WILDHUMMER_SHAMAN:
                        return wildhummerShamanGUID;
                    case NPC_OVERLORD_RUNTHAK:
                        return runthakGUID;
                    case NPC_GROLDAR:
                        return groldarGUID;
                    case NPC_MALKOROK_CONTROLLER:
                        return malkorokControllerGUID;
                    case NPC_ANCIENT_MIASMA:
                        return miasmaGUID;
                    case GO_REX_DOOR:
                        return rexDoorGUID;
                    case NPC_AKOLIK:
                        return akolikGUID;
                    case NPC_WATERSPEAKER_GORAI:
                        return goraiGUID;
                    case NPC_MONTAK:
                        return montakGUID;
                    case GO_SPOILS_CHEST:
                        return spoilsChestGUID;
                    case GO_SPOILS_CHEST_HC:
                        return spoilsHeroicChestGUID;
                    case NPC_MANTID_SPOILS_1:
                        return mantidSpoils1GUID;
                    case NPC_MANTID_SPOILS_2:
                        return mantidSpoils2GUID;
                    case NPC_MOGU_SPOILS_1:
                        return moguSpoils1GUID;
                    case NPC_MOGU_SPOILS_2:
                        return moguSpoils2GUID;
                    case GO_SPOILS_ACCESS_LOW_LEFT:
                        return spoilsAccessLowLeftGUID;
                    case GO_SPOILS_ACCESS_LOW_RIGHT:
                        return spoilsAccessLowRightGUID;
                    case GO_SPOILS_ACCESS_HIGH_RIGHT:
                        return spoilsAccessHighRightGUID;
                    case GO_SPOILS_ACCESS_HIGH_LEFT:
                        return spoilsAccessHighLeftGUID;
                    case GO_SPOILS_CROSS_RIGHT:
                        return spoilsCrossRightGUID;
                    case GO_SPOILS_CROSS_LEFT:
                        return spoilsCrossLeftGUID;
                    case GO_DEFENSE_SYSTEM_LEVER_LOW_RIGHT:
                        return leverLowRightGUID;
                    case GO_DEFENSE_SYSTEM_LEVER_UP_RIGHT:
                        return leverUpRightGUID;
                    case GO_DEFENSE_SYSTEM_LEVER_UP_LEFT:
                        return leverUpLeftGUID;
                    case GO_DEFENSE_SYSTEM_LEVER_LOW_LEFT:
                        return leverLowLeftGUID;
                    case NPC_SIEGECRAFTER_HELPER:
                        return siegeCrafterHelperGUID;
                    case NPC_ANTI_AIR_CANNON:
                        return southCannonGUID;
                    case NPC_ANTI_AIR_CANNON + 1:
                        return northCannonGUID;
                    case NPC_ROPE:
                        return southRopeGUID;
                    case NPC_ROPE + 1:
                        return northRopeGUID;
                    case NPC_KING_VARIAN_WRYNN:
                        return varianGUID;
                    case NPC_LADY_JAINA_PROUDMOORE:
                        return jainaGUID;
                    case NPC_VEREESA_WINDRUNNER:
                        return vereesaGUID;
                    case GO_SECURED_STOCKPILE_OF_SPOILS:
                        return cosmeticSpoilsGUID;
                    case NPC_SIEGECRAFTER_BLACKFUSE_1:
                        return siegecrafterVehicleGUID;
                    case NPC_PARAGON_OF_THE_KLAXXI:
                        return paragonControllerGUID;
                    case NPC_MANTID_AMBER:
                        return mantidAmberGUID;
                    case GO_MASSIVE_IRON_DOOR_BLACKFUSE:
                        return siegecrafterDoorGUID;
                    case NPC_AGGRON:
                        return aggronGUID;
                    case NPC_GORODAN:
                        return gorodanGUID;
                    case NPC_SHANNA_SPARKFIZZ:
                    case DATA_SHANNA_SPARKFIZZ:
                        return shannaGUID;
                    case NPC_THRALL_GARROSH_EVENT:
                        return thrallGUID;
                    case NPC_KORKRON_GUNNER_RP:
                        return korkronGunnerGUID;
                    case NPC_KORKRON_GUNNER_RP + 1:
                        return korkronGunnerGUID_2;
                    case NPC_GAMON:
                        return gamonGUID;
                    case NPC_HEART_OF_YSHAARJ:
                        return heartOfYshaarjGUID;
                    case GO_GARROSH_SOUTH_WEST_DOOR:
                        return garroshWestGatesGUID;
                    case GO_GARROSH_SOUTH_EAST_DOOR:
                        return garroshEastGatesGUID;
                    case NPC_OVERLORD_SAURFANG:
                        return saurfangGUID;
                    case GO_PARAGON_ENTRANCE:
                        return paragonEntranceGUID;
                    case NPC_GUNSHIP_MAIN_CANNON:
                        return gunshipMainCannonGUID;
                    case NPC_STARVED_YETI_TRASH:
                        return starvedYetiThrashGUID;
                    case NPC_ENRAGED_MUSHAN_BEAST:
                        return enragedMushanThrashGUID;
                    case NPC_KORKRON_DARK_FARSEER:
                        return darkFarseerBarracksGUID;
                    case NPC_KORKRON_DARK_FARSEER + 1:
                        return darkFarseerSkullsplitterGUID;
                    case GO_ENCASE_IN_AMBER:
                        return encaseInAmberGUID;
                    case GO_THRICE_LOCKED_CAGE:
                        return thriceLockedCageGUID;
                    case NPC_MONSTROUS_GASTROPOD:
                        return gastropodGUID;
                }

                return ObjectGuid::Empty;
            }

            void ProcessEvent(WorldObject* /*gameObject*/, uint32 eventId) override
            {
                switch (eventId)
                {
                    case INSTANCE_EVENT_SCARRED_VALE:
                        HandleGameObject(GetGuidData(GO_MASSIVE_ANCIENT_DOOR), true, NULL);
                        SetData(DATA_FALLEN_PROTECTORS_PRE_EVENT, DONE);

                        if (Creature* pressurePlate = instance->GetCreature(GetGuidData(NPC_PRESSURE_PLATE)))
                        {
                            pressurePlate->RemoveAurasDueToSpell(SPELL_PRESSURE_PLATE);

                            // Cho Event
                            pressurePlate->SummonCreature(NPC_LOREWALKER_CHO, choScarredValePath[0], TEMPSUMMON_MANUAL_DESPAWN);
                        }

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* target = itr.GetSource())
                                target->GetSceneMgr().PlayScene(SCENE_SCARED_VALE);
                        break;
                    case INSTANCE_EVENT_GAMON:
                        if (Creature* gamon = instance->GetCreature(GetGuidData(NPC_GAMON)))
                            gamon->AI()->DoAction(ACTION_START_INTRO);
                        break;
                    default:
                        break;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S O O " << GetBossSaveData() << norushenPreEvent << ' ' << norushenPastEvent << ' ' << fallenProtectorsPreEvent << ' ' << shaofPridePreEvent << ' ' << galakrasPreEvent << ' ' << shaofPridePastEvent << ' ' << ironJuggernautPastEvent << ' ' << thokBloodthirstyPreEvent << ' ' << shamansTotemEvent << ' ' << siegecrafterPreEvent << ' ' << garroshHellscreamPreEvent << ' ' << aquaDefendersEvent << ' ' << paragonPreEvent;

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            bool IsWipe(float range, Unit* source) override
            {
                for (auto&& itr : instance->GetPlayers())
                {
                    if (Player* player = itr.GetSource())
                    {
                        if (source && player->GetExactDist2d(source) > range)
                            continue;

                        if (player->HasAura(144287) || player->HasAura(5384) || player->HasAura(32612) || player->HasAura(1784)) // On Conveyor, Feign Death, Invisibility, Stealth
                            continue;

                        if (player->IsAlive() && !player->IsGameMaster())
                            return false;
                    }
                }

                return true;
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

                if (dataHead1 == 'S' && dataHead2 == 'O' && dataHead3 == 'O')
                {
                    for (uint8 i = 0; i < MAX_TYPES; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp; // Event in Scarred Vale
                    norushenPreEvent = temp;
                    SetData(DATA_NORUSHEN_PRE_EVENT_1, norushenPreEvent);
                    loadStream >> temp; // Event In Norushen Room
                    norushenPastEvent = temp;
                    SetData(DATA_NORUSHEN_PRE_EVENT_2, norushenPastEvent);
                    loadStream >> temp; // Event Forward Protectors
                    fallenProtectorsPreEvent = temp;
                    SetData(DATA_FALLEN_PROTECTORS_PRE_EVENT, fallenProtectorsPreEvent);
                    loadStream >> temp; // Event Forward Sha of Pride
                    shaofPridePreEvent = temp;
                    SetData(DATA_SHA_OF_PRIDE_PRE_EVENT, shaofPridePreEvent);
                    loadStream >> temp; // Event Forward Galakras
                    galakrasPreEvent = temp;
                    SetData(DATA_GALAKRAS_PRE_EVENT, galakrasPreEvent);
                    loadStream >> temp; // Event Past Sha of Pride
                    shaofPridePastEvent = temp;
                    SetData(DATA_SHA_OF_PRIDE_PAST_EVENT, shaofPridePastEvent);
                    loadStream >> temp; // Event Past Iron Juggernaut
                    ironJuggernautPastEvent = temp;
                    SetData(DATA_IRON_JUGGERNAUT_PAST_EVENT, ironJuggernautPastEvent);
                    loadStream >> temp; // Event Forward Thok Bloodthirsty
                    thokBloodthirstyPreEvent = temp;
                    SetData(DATA_THOK_BLOODTHIRSTY_PRE_EVENT, thokBloodthirstyPreEvent);
                    loadStream >> temp; // Shaman from cage was imprisoned
                    shamansTotemEvent = temp;
                    SetData(DATA_SHAMANS_PRISON, shamansTotemEvent);
                    loadStream >> temp; // Siegecrafter Blackfuse Pre Event
                    siegecrafterPreEvent = temp;
                    SetData(DATA_SIEGECRAFTER_PRE_EVENT, siegecrafterPreEvent);
                    loadStream >> temp; // Garrosh Hellscream Pre Event
                    garroshHellscreamPreEvent = temp;
                    SetData(DATA_GARROSH_HELLSCREAM_PREVE_EVENT, garroshHellscreamPreEvent);
                    loadStream >> temp; // Immerseus Aqua Defenders Event
                    aquaDefendersEvent = temp;
                    SetData(DATA_AQUEOUS_DEFENDERS_FALLEN, aquaDefendersEvent);
                    loadStream >> temp; // Parago of the Klaxxi Pre Event
                    paragonPreEvent = temp;
                    SetData(DATA_PARAGON_PRE_EVENT, paragonPreEvent);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_siege_of_orgrimmar_InstanceMapScript(map);
        }
};

void AddSC_instance_siege_of_orgrimmar()
{
    new instance_siege_of_orgrimmar();
}
