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
#include "throne_of_thunder.h"
#include "Group.h"
#include "LFGMgr.h"

static std::vector<DoorData> const doorData =
{
    { GO_JIN_ROKH_ENTRANCE,              DATA_JINROKH,              DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_JIN_ROKH_EXIT,                  DATA_JINROKH,              DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_HORRIDON_EXIT,                  DATA_HORRIDON,             DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_COUNCIL_ENTRANCE1,              DATA_COUNCIL_OF_ELDERS,    DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_COUNCIL_ENTRANCE2,              DATA_COUNCIL_OF_ELDERS,    DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_COUNCIL_EXIT,                   DATA_COUNCIL_OF_ELDERS,    DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_TORTOS_DOOR,                    DATA_TORTOS,               DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_MEGAERA_EXIT,                   DATA_MEGAERA,              DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_JI_KUN_EXIT,                    DATA_JI_KUN,               DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_BEHOLDER_GRATE_ENTRANCE,        DATA_DURUMU_THE_FORGOTTEN, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_NONE },
    { GO_ANCIENT_STONE_DOOR_DURUMU_EXIT, DATA_DURUMU_THE_FORGOTTEN, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_BEHOLDER_GRATE_EXIT,            DATA_DURUMU_THE_FORGOTTEN, DOOR_TYPE_SPAWN_HOLE, BOUNDARY_NONE },
    { GO_MOGU_GATE_DURUMU,               DATA_DURUMU_THE_FORGOTTEN, DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_PRIMORDIUS_ENTRANCE,            DATA_PRIMORDIUS,           DOOR_TYPE_ROOM,       BOUNDARY_S    },
    { GO_PRIMORDIUS_EXIT,                DATA_PRIMORDIUS,           DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_ANCIENT_STONE_DOOR,             DATA_DARK_ANIMUS,          DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_IRON_QON_BACK_DOOR,             DATA_IRON_QON,             DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_IRON_QON_EXIT_1,                DATA_IRON_QON,             DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_IRON_QON_EXIT_2,                DATA_IRON_QON,             DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_TWIN_CONSORT_BACK_DOOR,         DATA_TWIN_CONSORTS,        DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_TWIN_CONSORT_PASSAGE_DOOR,      DATA_TWIN_CONSORTS,        DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
    { GO_TWIN_CONSORT_FORWARD_DOOR,      DATA_TWIN_CONSORTS,        DOOR_TYPE_ROOM,       BOUNDARY_NONE },
    { GO_RADEN_SECRET_DOOR,              DATA_LEI_SHEN,             DOOR_TYPE_PASSAGE,    BOUNDARY_NONE },
};

class instance_throne_of_thunder : public InstanceMapScript
{
    public:
        instance_throne_of_thunder() : InstanceMapScript("instance_throne_of_thunder", 1098) { }

        struct instance_throne_of_thunder_InstanceScript : public InstanceScript
        {
            instance_throne_of_thunder_InstanceScript(Map* map) : InstanceScript(map) { }

            EventMap m_mEvents;

            // Encounter Data
            ObjectGuid jinrokhGUID;
            ObjectGuid horidonGUID;
            ObjectGuid kazrajinGUID;
            ObjectGuid sulGUID;
            ObjectGuid malakkGUID;
            ObjectGuid marliGUID;
            ObjectGuid tortosGUID;
            ObjectGuid megeraGUID;
            ObjectGuid jikunGUID;
            ObjectGuid durumuGUID;
            ObjectGuid primordiusGUID;
            ObjectGuid animusGUID;
            ObjectGuid ironQonGUID;
            ObjectGuid suenGUID;
            ObjectGuid lulinGUID;
            ObjectGuid leishenGUID;
            ObjectGuid radenGUID;

            // PreEvent Data
            uint64 hasJinrokhEventDone;
            uint64 hasPreHorridonEventDone;
            uint64 hasHoridonEventDone;
            uint64 hasPreGarajalEvent1Done;
            uint64 hasPreGarajalEvent2Done;
            uint64 hasgarajalEventDone;
            uint64 hasTorgosEventDone;
            uint64 hasMegeraEventDone;
            uint64 hasJikunEventDone;
            uint64 hasDurumuEventDone;
            uint64 hasPrimordiusEventDone;
            uint64 hasAnimusEventDone;
            uint64 hasCelestialEventDone;
            uint64 hasThunderEventDone;
            uint64 hasRaDenEventDone;

            // Misc Data
            ObjectGuid horridonHelperGuid;
            ObjectGuid jalakGUID;
            ObjectGuid garajalHelperGUID;
            ObjectGuid preDoorJinrokhGUID;
            ObjectGuid preHorridonDoorGUID;
            ObjectGuid preCouncilDoor1GUID;
            ObjectGuid preCouncilDoor2GUID;
            ObjectGuid garajalSoulGUID;
            ObjectGuid introLeiShenGUID;
            ObjectGuid tortosBridgeGUID;
            ObjectGuid tortosDeathCollisionGuid;
            ObjectGuid rushingWindsControllerGUID;
            ObjectGuid twinConsortPhaseController_1;
            ObjectGuid twinConsortPhaseController_2;
            ObjectGuid twinConsortController; // not real
            ObjectGuid diffusionChainConduitGUID;
            ObjectGuid staticShockConduitGUID;
            ObjectGuid bouncingBoltConduitGUID;
            ObjectGuid overchargeConduitGUID;
            ObjectGuid chargingStationGUID;
            ObjectGuid discCollisionGUID;
            ObjectGuid northFloorGUID;
            ObjectGuid southFloorGUID;
            ObjectGuid westFloorGUID;
            ObjectGuid eastFloorGUID;
            ObjectGuid northPillarGUID;
            ObjectGuid southPillarGUID;
            ObjectGuid westPillarGUID;
            ObjectGuid eastPillarGUID;
            ObjectGuid windSEGUID;
            ObjectGuid windNEGUID;
            ObjectGuid windSWGUID;
            ObjectGuid windNWGUID;
            ObjectGuid windowSEGUID;
            ObjectGuid windowNEGUID;
            ObjectGuid windowSWGUID;
            ObjectGuid windowNWGUID;
            ObjectGuid megaeraAnnouncerGUID;
            ObjectGuid chamberExitGUID;
            ObjectGuid animaOrbGUID;
            ObjectGuid animusEntranceDoorGUID;
            ObjectGuid ironQonDoorGUID;
            ObjectGuid megaeraChestGUID;
            ObjectGuid megaeraHeroicChestGUID;
            ObjectGuid fadedNuzaoGUID;
            ObjectGuid fadedYulonGUID;
            ObjectGuid fadedChijiGUID;
            ObjectGuid fadedHuenGUID;

            uint32 m_auiEncounter[MAX_EVENT_DATA - MAX_DATA - 1];
            std::list<ObjectGuid> m_lMoguBellGuids, windBunnysList, thunderTrapGuids, ritualistGuids, animusGolemGuids, largeAnimusGuids, featherGuids;
            std::map<uint32, ObjectGuid> ironPets, tribesDoor;

            bool isLfr;
            bool lfrSectionFound;

            uint32 lfrQuarter;

            void Initialize() override
            {
                SetBossNumber(MAX_DATA);
                LoadDoorData(doorData);

                jinrokhGUID = ObjectGuid::Empty;
                horidonGUID = ObjectGuid::Empty;
                kazrajinGUID = ObjectGuid::Empty;
                sulGUID = ObjectGuid::Empty;
                malakkGUID = ObjectGuid::Empty;
                marliGUID = ObjectGuid::Empty;
                tortosGUID = ObjectGuid::Empty;
                megeraGUID = ObjectGuid::Empty;
                jikunGUID = ObjectGuid::Empty;
                durumuGUID = ObjectGuid::Empty;
                primordiusGUID = ObjectGuid::Empty;
                animusGUID = ObjectGuid::Empty;
                ironQonGUID = ObjectGuid::Empty;
                suenGUID = ObjectGuid::Empty;
                lulinGUID = ObjectGuid::Empty;
                leishenGUID = ObjectGuid::Empty;
                radenGUID = ObjectGuid::Empty;

                hasJinrokhEventDone          = 0;
                hasPreHorridonEventDone      = 0;
                hasHoridonEventDone          = 0;
                hasPreGarajalEvent1Done      = 0;
                hasPreGarajalEvent2Done      = 0;
                hasgarajalEventDone          = 0;
                hasTorgosEventDone           = 0;
                hasMegeraEventDone           = 0;
                hasJikunEventDone            = 0;
                hasDurumuEventDone           = 0;
                hasPrimordiusEventDone       = 0;
                hasAnimusEventDone           = 0;
                hasCelestialEventDone        = 0;
                hasThunderEventDone          = 0;
                hasRaDenEventDone            = 0;

                windBunnysList.clear();
                tribesDoor.clear();
                thunderTrapGuids.clear();
                ritualistGuids.clear();
                animusGolemGuids.clear();
                largeAnimusGuids.clear();
                featherGuids.clear();

                jalakGUID                    = ObjectGuid::Empty;
                garajalHelperGUID            = ObjectGuid::Empty;
                introLeiShenGUID             = ObjectGuid::Empty;
                tortosBridgeGUID             = ObjectGuid::Empty;
                tortosDeathCollisionGuid     = ObjectGuid::Empty;
                rushingWindsControllerGUID   = ObjectGuid::Empty;
                twinConsortPhaseController_1 = ObjectGuid::Empty;
                twinConsortPhaseController_2 = ObjectGuid::Empty;
                twinConsortController        = ObjectGuid::Empty;
                diffusionChainConduitGUID    = ObjectGuid::Empty;
                staticShockConduitGUID       = ObjectGuid::Empty;
                bouncingBoltConduitGUID      = ObjectGuid::Empty;
                overchargeConduitGUID        = ObjectGuid::Empty;
                chargingStationGUID          = ObjectGuid::Empty;
                northFloorGUID               = ObjectGuid::Empty;
                southFloorGUID               = ObjectGuid::Empty;
                westFloorGUID                = ObjectGuid::Empty;
                eastFloorGUID                = ObjectGuid::Empty;
                northPillarGUID              = ObjectGuid::Empty;
                southPillarGUID              = ObjectGuid::Empty;
                eastPillarGUID               = ObjectGuid::Empty;
                westPillarGUID               = ObjectGuid::Empty;
                windSEGUID                   = ObjectGuid::Empty;
                windNEGUID                   = ObjectGuid::Empty;
                windSWGUID                   = ObjectGuid::Empty;
                windNWGUID                   = ObjectGuid::Empty;
                windowSEGUID                 = ObjectGuid::Empty;
                windowNEGUID                 = ObjectGuid::Empty;
                windowSWGUID                 = ObjectGuid::Empty;
                windowNWGUID                 = ObjectGuid::Empty;
                megaeraAnnouncerGUID         = ObjectGuid::Empty;
                animusEntranceDoorGUID       = ObjectGuid::Empty;
                ironQonDoorGUID              = ObjectGuid::Empty;
                megaeraChestGUID             = ObjectGuid::Empty;
                megaeraHeroicChestGUID       = ObjectGuid::Empty;
                fadedChijiGUID               = ObjectGuid::Empty;
                fadedHuenGUID                = ObjectGuid::Empty;
                fadedNuzaoGUID               = ObjectGuid::Empty;
                fadedYulonGUID               = ObjectGuid::Empty;

                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                isLfr = instance->ToInstanceMap()->IsLFGMap();
                lfrQuarter = QUARTER_NONE;

                instance->SetWorldState(WORLDSTATE_I_THOUGHT_HE_WAS_SUPPOSED_TO_BE_HARD, 1);
            }

            void HandleRemoveAreatriggerAuras(Player* target)
            {
                std::vector<uint32> atAurasList = { 136889, 140208, 137176, 134821, 135153, 138372 };

                for (auto&& itr : atAurasList)
                    target->RemoveAurasDueToSpell(itr);
            }

            void OnPlayerEnter(Player* player) override
            {
                HandleRemoveAreatriggerAuras(player);

                if (isLfr && !lfrQuarter)
                    if (Group* group = player->GetGroup())
                        if (group->isLFGGroup())
                            lfrQuarter = sLFGMgr->GetDungeon(group->GetGUID());

                // Determination
                if (isLfr && GetDeterminationValue())
                    if (Aura* detAura = player->AddAura(SPELL_DETERMINATION_LFR, player))
                        detAura->SetStackAmount(GetDeterminationValue());

                if (isLfr || GetEncountersDoneCount(MAX_DATA) < 1 || player->GetSession()->PlayerLoading() || player->IsGameMaster())
                    return;

                Position pos = GetCurrentEntrancePosition();
                player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
            }

            // in this raid not enough just defeat boss for revive at his pos (not for whole).
            // U need kill boss and have complete a next event.
            // (for example - for spawn in horridon place u need kill him and complete garajal pre-event)
            Position GetCurrentEntrancePosition() override
            {
                uint32 m_encounterDone = GetEncountersDoneCount(MAX_DATA);

                switch (m_encounterDone) // begun enum from bossId where entrance was changed
                {
                    case DATA_JINROKH:
                    case DATA_HORRIDON:
                    case DATA_COUNCIL_OF_ELDERS:
                        return GetData(invDataType.find(m_encounterDone)->second) == DONE ? EncountersEntrance[m_encounterDone + 1] : EncountersEntrance[m_encounterDone];
                    case DATA_TORTOS:
                    case DATA_MEGAERA:
                    case DATA_JI_KUN:
                    case DATA_DURUMU_THE_FORGOTTEN:
                    case DATA_PRIMORDIUS:
                    case DATA_DARK_ANIMUS:
                    case DATA_IRON_QON:
                    case DATA_TWIN_CONSORTS:
                        return EncountersEntrance[m_encounterDone + 1];
                    case DATA_LEI_SHEN: // if heroic - teleport to primordius room
                        return instance->IsHeroic() ? EncountersEntrance[DATA_PRIMORDIUS + 1] : EncountersEntrance[DATA_TWIN_CONSORTS + 1];
                }

                return EncountersEntrance[0];
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (isLfr && !lfrQuarter)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            OnPlayerEnter(player);
                }

                if (lfrQuarter != QUARTER_NONE)
                {
                    if (lfrQuarter == QUARTER_LAST_STAND_OF_THE_ZANDALARI)
                    {
                        if (creature->GetAreaId() != 6705 || (creature->GetAreaId() == 6705 && creature->GetPositionY() < 5000.0f))
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < MAX_DATA; i++)
                            if (i != DATA_JINROKH && i != DATA_HORRIDON && i != DATA_COUNCIL_OF_ELDERS)
                                SetBossState(i, DONE);
                    }
                    else if (lfrQuarter == QUARTER_FORGOTTEN_DEPTH)
                    {
                        if (creature->GetAreaId() != 6706 &&
                            ((creature->GetAreaId() == 6622 && (creature->GetPositionY() > 4500.0f || creature->GetPositionX() < 4100.0f || creature->GetPositionZ() > 80.0f))
                                || creature->GetAreaId() == 6705))
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < MAX_DATA; i++)
                            if (i != DATA_TORTOS && i != DATA_MEGAERA && i != DATA_JI_KUN)
                                SetBossState(i, DONE);
                    }
                    else if (lfrQuarter == QUARTER_HALLS_OF_FLESH_SHAPING)
                    {
                        if (creature->GetAreaId() != 6622 ||
                            (creature->GetAreaId() == 6622 && (creature->GetPositionY() < 4440.0f || creature->GetPositionY() > 4860.0f)))
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < MAX_DATA; i++)
                            if (i != DATA_DURUMU_THE_FORGOTTEN && i != DATA_PRIMORDIUS && i != DATA_DARK_ANIMUS)
                                SetBossState(i, DONE);
                    }
                    else if (lfrQuarter == QUARTER_PINNACLE_OF_STORMS)
                    {
                        if ((creature->GetAreaId() != 6705 || (creature->GetAreaId() == 6705 && creature->GetPositionY() > 5000.0f)) &&
                            (creature->GetAreaId() != 6622 || (creature->GetAreaId() == 6622 && creature->GetPositionZ() < 110.0f)))
                            creature->DespawnOrUnsummon();
                        for (uint32 i = 0; i < MAX_DATA; i++)
                            if (i != DATA_IRON_QON && i != DATA_TWIN_CONSORTS && i != DATA_LEI_SHEN)
                                SetBossState(i, DONE);
                    }
                }

                switch (creature->GetEntry())
                {
                    case NPC_JINROKH:
                        jinrokhGUID = creature->GetGUID();
                        break;
                    case NPC_HORRIDON:
                        horidonGUID = creature->GetGUID();
                        break;
                    case NPC_COUNCIL_KAZRAJIN:
                        kazrajinGUID = creature->GetGUID();
                        break;
                    case NPC_COUNCIL_SUL_THE_SANDCRAWLER:
                        sulGUID = creature->GetGUID();
                        break;
                    case NPC_COUNCIL_FROST_KING_MALAKK:
                        malakkGUID = creature->GetGUID();
                        break;
                    case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
                        marliGUID = creature->GetGUID();
                        break;
                    case NPC_TORTOS:
                        tortosGUID = creature->GetGUID();
                        break;
                    case NPC_MEGAERA:
                        megeraGUID = creature->GetGUID();
                        break;
                    case NPC_JI_KUN:
                        jikunGUID = creature->GetGUID();
                        break;
                    case NPC_DURUMU_THE_FORGOTTEN:
                        durumuGUID = creature->GetGUID();
                        break;
                    case NPC_PRIMORDIUS:
                        primordiusGUID = creature->GetGUID();
                        break;
                    case NPC_DARK_ANIMUS:
                        animusGUID = creature->GetGUID();
                        break;
                    case NPC_IRON_QON:
                        ironQonGUID = creature->GetGUID();
                        break;
                    case NPC_LULIN:
                        lulinGUID = creature->GetGUID();
                        break;
                    case NPC_SUEN:
                        suenGUID = creature->GetGUID();
                        break;
                    case NPC_LEI_SHEN:
                        leishenGUID = creature->GetGUID();
                        break;
                    case NPC_RA_DEN:
                        radenGUID = creature->GetGUID();
                        break;
                    case NPC_WAR_GOD_JALAK:
                        jalakGUID = creature->GetGUID();
                        break;
                    case NPC_JINROKH_STATUE:
                        creature->SetCanFly(true);
                        creature->SetHover(true);
                        creature->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_DISABLE_GRAVITY);
                        break;
                    case NPC_GARA_JAL:
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CAN_SWIM);
                        creature->SetStandFlags(UNIT_STAND_FLAGS_CREEP);
                        garajalHelperGUID = creature->GetGUID();
                        break;
                    case NPC_HORRIDON_EVENT_HELPER:
                        horridonHelperGuid = creature->GetGUID();
                        break;
                    case NPC_ANIMUS_GOLEM:
                        animusGolemGuids.push_back(creature->GetGUID());
                        break;
                    case NPC_LARGE_ANIMA_GOLEM:
                        largeAnimusGuids.push_back(creature->GetGUID());
                        break;
                    case NPC_ROSHAK:
                    case NPC_QUETZAL:
                    case NPC_DAMREN:
                        ironPets.insert(std::pair<uint32, uint64>(creature->GetEntry(), creature->GetGUID()));
                        break;
                    case NPC_GENERAL_BUNNY_GROUND:
                        windBunnysList.push_back(creature->GetGUID());
                        break;
                    case NPC_GARAJAL_THE_SPIRITBINDER_TRASH:
                        creature->SetVisible(false);
                        break;
                    case NPC_GARA_JALS_SOUL:
                        garajalSoulGUID = creature->GetGUID();
                        break;
                    case NPC_LEI_SHEN_TRIGGER:
                        introLeiShenGUID = creature->GetGUID();
                        break;
                    case NPC_ICY_GROUND:
                    case NPC_TORRENT_OF_ICE:
                    case NPC_ACID_RAIN:
                    case NPC_CINDERS:
                        creature->SetDisplayId(11686);
                        break;
                    case NPC_WORLD_TRIGGER: // used in twin consort day/night controll phase
                        if (creature->GetPositionX() < 6000.0f) // third, idk what is
                            break;

                        if (twinConsortPhaseController_1)
                            twinConsortPhaseController_2 = creature->GetGUID();
                        else
                            twinConsortPhaseController_1 = creature->GetGUID();
                        break;
                    case NPC_CONSORT_CONTROLLER:
                        if (creature->GetPositionZ() > 150.0f)
                            twinConsortController = creature->GetGUID();
                        break;
                    case NPC_RUSHING_WINDS_CONTROLLER:
                        rushingWindsControllerGUID = creature->GetGUID();
                        break;
                    case NPC_THUNDER_TRAP:
                        thunderTrapGuids.push_back(creature->GetGUID());
                        break;
                    case NPC_DIFFUSION_CHAIN_CONDUIT:
                        diffusionChainConduitGUID = creature->GetGUID();
                        break;
                    case NPC_STATIC_SHOCK_CONDUIT:
                        staticShockConduitGUID = creature->GetGUID();
                        break;
                    case NPC_BOUNCING_BOLT_CONDUIT:
                        bouncingBoltConduitGUID = creature->GetGUID();
                        break;
                    case NPC_OVERCHARGE_CONDUIT:
                        overchargeConduitGUID = creature->GetGUID();
                        break;
                    case NPC_EXIT_CHAMBER:
                        creature->SetDisplayFromModel(1);
                        chamberExitGUID = creature->GetGUID();

                        if (GetData(DATA_JI_KUN_EVENT))
                            creature->CastSpell(creature, SPELL_EXIT_CHAMBER_AT, true);
                        break;
                    case NPC_SLG_GENERIC_MOP:
                        creature->SetDisplayFromModel(1);

                        if (creature->GetPositionZ() < -194.0f)
                            megaeraAnnouncerGUID = creature->GetGUID();
                        break;
                    case NPC_ANIMA_ORB:
                        animaOrbGUID = creature->GetGUID();
                        break;
                    case NPC_DARK_RITUALIST:
                        ritualistGuids.push_back(creature->GetGUID());

                        if (ritualistGuids.size() > 5)
                            m_mEvents.ScheduleEvent(EVENT_RITUALIST_INITIALIZE, 2 * IN_MILLISECONDS);
                        break;
                    case NPC_CHOKING_GAS:
                    case NPC_JUMP_TO_BOSS_PLATFORM:
                    case NPC_LIGHTNING_VISUAL_FIRST_STAGE:
                    case NPC_LIGHTNING_VISUAL_SECOND_STAGE:
                    case NPC_NETHER_TEAR_SPAWNER:
                    case NPC_SPLASH_GRATE:
                    case NPC_GARAJAL_THRASH_SOUL:
                    case NPC_INVIS_STALKER_UNK:
                        creature->SetDisplayFromModel(1);
                        break;
                    case NPC_FADED_IMAGE_OF_CHI_JI:
                        fadedChijiGUID = creature->GetGUID();
                        break;
                    case NPC_FADED_IMAGE_OF_NIUZAO:
                        fadedNuzaoGUID = creature->GetGUID();
                        break;
                    case NPC_FADED_IMAGE_OF_XUEN:
                        fadedHuenGUID = creature->GetGUID();
                        break;
                    case NPC_FADED_IMAGE_OF_YULON:
                        fadedYulonGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_JIN_ROKH_ENTRANCE:
                    case GO_JIN_ROKH_EXIT:
                    case GO_HORRIDON_EXIT:
                    case GO_COUNCIL_ENTRANCE1:
                    case GO_COUNCIL_ENTRANCE2:
                    case GO_TORTOS_DOOR:
                    case GO_MEGAERA_EXIT:
                    case GO_BEHOLDER_GRATE_ENTRANCE:
                    case GO_ANCIENT_STONE_DOOR_DURUMU_EXIT:
                    case GO_MOGU_GATE_DURUMU:
                    case GO_BEHOLDER_GRATE_EXIT:
                    case GO_PRIMORDIUS_ENTRANCE:
                    case GO_PRIMORDIUS_EXIT:
                    case GO_IRON_QON_BACK_DOOR:
                    case GO_IRON_QON_EXIT_1:
                    case GO_IRON_QON_EXIT_2:
                    case GO_TWIN_CONSORT_BACK_DOOR:
                    case GO_TWIN_CONSORT_FORWARD_DOOR:
                    case GO_TWIN_CONSORT_PASSAGE_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_COUNCIL_EXIT:
                    case GO_JI_KUN_EXIT:
                    case GO_ANCIENT_STONE_DOOR:
                        if (!isLfr)
                            AddDoor(go, true);
                        break;
                    case GO_RADEN_SECRET_DOOR:
                        if (instance->IsHeroic())
                            AddDoor(go, true);
                        break;
                    case GO_JIN_ROKH_PREDOOR:
                        preDoorJinrokhGUID = go->GetGUID();

                        if (GetData(DATA_JINROKH_EVENT) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_HORRIDON_ENTRANCE:
                        preHorridonDoorGUID = go->GetGUID();

                        if (GetData(DATA_PRE_HORIDON_EVENT) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_COUNCIL_TRASH_DOOR_ONE:
                        preCouncilDoor1GUID = go->GetGUID();

                        if (GetData(DATA_PRE_COUNCIL_EVENT_1) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_COUNCIL_TRASH_DOOR_TWO:
                        preCouncilDoor2GUID = go->GetGUID();

                        if (GetData(DATA_PRE_COUNCIL_EVENT_2) == DONE)
                            HandleGameObject(ObjectGuid::Empty, true, go);
                        break;
                    case GO_TRIBAL_DOOR_FARRAKI:
                    case GO_TRIBAL_DOOR_GURUBASHI:
                    case GO_TRIBAL_DOOR_DRAKKARI: 
                    case GO_TRIBAL_DOOR_AMANI:
                        tribesDoor.insert(std::pair<uint32, uint64>(go->GetEntry(), go->GetGUID()));
                        break;
                        //case GO_MOGU_STATUE_1:
                        //case GO_MOGU_STATUE_2:
                        //case GO_MOGU_STATUE_3:
                        //case GO_MOGU_STATUE_4:
                        //case GO_JIKUN_FEATHER:
                        //case GO_JIN_ROKH_PREDOOR:
                        break;
                    case GO_MOGU_BELL:
                        m_lMoguBellGuids.push_back(go->GetGUID());
                        break;
                    case GO_TORTOS_BRIDGE:
                        tortosBridgeGUID = go->GetGUID();

                        if (GetData(DATA_TORTOS_EVENT) == DONE)
                            go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        break;
                    case GO_TORTOS_COLLISION:
                        tortosDeathCollisionGuid = go->GetGUID();
                        if (Creature* tortos = instance->GetCreature(tortosGUID))
                            if (tortos->getDeathState() != CORPSE)
                                go->SetPhaseMask(0, true);
                        break;
                    case GO_CHARGING_STATION:
                        chargingStationGUID = go->GetGUID();
                        break;
                    case GO_THUNDER_KING_DISC_COLLISION:
                        discCollisionGUID = go->GetGUID();
                        break;
                    case GO_FLOOR_FX_SOUTH:
                        southFloorGUID = go->GetGUID();
                        break;
                    case GO_FLOOR_FX_NORTH:
                        northFloorGUID = go->GetGUID();
                        break;
                    case GO_FLOOR_FX_WEST:
                        westFloorGUID = go->GetGUID();
                        break;
                    case GO_FLOOR_FX_EAST:
                        eastFloorGUID = go->GetGUID();
                        break;
                    case GO_NORTH_PILLAR:
                        northPillarGUID = go->GetGUID();
                        break;
                    case GO_SOUTH_PILLAR:
                        southPillarGUID = go->GetGUID();
                        break;
                    case GO_EAST_PILLAR:
                        eastPillarGUID = go->GetGUID();
                        break;
                    case GO_WEST_PILLAR:
                        westPillarGUID = go->GetGUID();
                        break;
                    case GO_WIND_FX_NE:
                        windNEGUID = go->GetGUID();
                        break;
                    case GO_WIND_FX_NW:
                        windNWGUID = go->GetGUID();
                        break;
                    case GO_WIND_FX_SE:
                        windSEGUID = go->GetGUID();
                        break;
                    case GO_WIND_FX_SW:
                        windSWGUID = go->GetGUID();
                        break;
                    case GO_SE_WINDOW:
                        windowSEGUID = go->GetGUID();
                        break;
                    case GO_SW_WINDOW:
                        windowSWGUID = go->GetGUID();
                        break;
                    case GO_NW_WINDOW:
                        windowNWGUID = go->GetGUID();
                        break;
                    case GO_NE_WINDOW:
                        windowNEGUID = go->GetGUID();
                        break;
                    case GO_JIKUN_FEATHER:
                    case GO_JIKUN_FEATHER_2:
                        featherGuids.push_back(go->GetGUID());

                        if (GetData(DATA_JI_KUN_EVENT) == DONE)
                            go->SetRespawnTime(7 * DAY);
                        break;
                    case GO_ANIMUS_ENTRANCE:
                        animusEntranceDoorGUID = go->GetGUID();
                        break;
                    case GO_IRON_QON_SEWER_GATE:
                        ironQonDoorGUID = go->GetGUID();
                        break;
                    case GO_MEGAERA_CHEST_NORMAL:
                        megaeraChestGUID = go->GetGUID();
                        break;
                    case GO_MEGAERA_CHEST_HEROIC:
                        megaeraHeroicChestGUID = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (type == DATA_JI_KUN && state == DONE)
                {
                    // Activate Feathers
                    for (auto&& itr : featherGuids)
                        DoRespawnGameObject(itr, 7 * DAY);
                }

                // On loading from DB
                if (type == DATA_RA_DEN && state == FAIL)
                    instance->SetWorldState(WORLDSTATE_I_THOUGHT_HE_WAS_SUPPOSED_TO_BE_HARD, 0);

                PromotionIfLFR(state);

                return true;
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit->GetTypeId() == TYPEID_PLAYER && GetBossState(DATA_RA_DEN) == IN_PROGRESS)
                    instance->SetWorldState(WORLDSTATE_I_THOUGHT_HE_WAS_SUPPOSED_TO_BE_HARD, 0);
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_JINROKH_EVENT:
                        hasJinrokhEventDone = data;           
                        HandleGameObject(GetGuidData(GO_JIN_ROKH_PREDOOR), true, NULL);
                        break;
                    case DATA_HORIDON_EVENT:
                        hasHoridonEventDone = data;
                        break;
                    case DATA_PRE_COUNCIL_EVENT_1:
                        hasPreGarajalEvent1Done = data;
                        HandleGameObject(GetGuidData(GO_COUNCIL_TRASH_DOOR_ONE), true, NULL);
                        break;
                    case DATA_PRE_COUNCIL_EVENT_2:
                        hasPreGarajalEvent2Done = data;
                        HandleGameObject(GetGuidData(GO_COUNCIL_TRASH_DOOR_TWO), true, NULL);
                        break;
                    case DATA_COUNCIL_EVENT:
                        hasgarajalEventDone = data;
                        break;
                    case DATA_TORTOS_EVENT:
                        hasTorgosEventDone = data;
                        break;
                    case DATA_MEGERA_EVENT:
                        if (data == IN_PROGRESS)
                            hasMegeraEventDone++;
                        else
                            hasMegeraEventDone = data;

                        SaveToDB();
                        break;
                    case DATA_JI_KUN_EVENT:
                        hasJikunEventDone = data;
                        break;
                    case DATA_PRIMORDIUS_EVENT:
                        hasPrimordiusEventDone = data;
                        break;
                    case DATA_LEI_SHEN_EVENT:
                        hasThunderEventDone = data;

                        switch (data)
                        {
                            case IN_PROGRESS:
                                m_mEvents.ScheduleEvent(EVENT_THUNDER_TRAP_INTRO, 1 * IN_MILLISECONDS);
                                break;
                            case DONE:
                                m_mEvents.CancelEvent(EVENT_THUNDER_TRAP_INTRO);

                                for (auto&& itr : thunderTrapGuids)
                                    if (Creature* thunderTrap = instance->GetCreature(itr))
                                        thunderTrap->DespawnOrUnsummon();
                                break;
                        }
                        break;
                    case DATA_RA_DEN_EVENT:
                        hasRaDenEventDone = data;
                        break;
                    case GOLEM_DATA:
                        if (instance->Is25ManRaid())
                            break;

                        if (animusGolemGuids.size() < 24 || largeAnimusGuids.size() < 7)
                        {
                            m_mEvents.ScheduleEvent(EVENT_GOLEM_INITIALIZE, 1 * IN_MILLISECONDS);
                            break;
                        }

                        Trinity::Containers::RandomResizeList(animusGolemGuids, ANIMUS_GOLEM_BROKEN_MAX);
                        Trinity::Containers::RandomResizeList(largeAnimusGuids, LARGE_ANIMUS_GOLEM_BROKEN_MAX);

                        for (auto&& itr : animusGolemGuids)
                            if (Creature* golem = instance->GetCreature(itr))
                                golem->AI()->DoAction(ACTION_BROKEN_GOLEM);

                        for (auto&& itr : largeAnimusGuids)
                            if (Creature* largeGolem = instance->GetCreature(itr))
                                largeGolem->AI()->DoAction(ACTION_BROKEN_GOLEM);
                        break;
                    case DATA_PRE_HORIDON_EVENT:
                        hasPreHorridonEventDone = data;

                        switch (data)
                        {
                            case DONE:
                                DoRemoveAurasDueToSpellOnPlayers(139485);

                                if (GameObject* preDoor = instance->GetGameObject(GetGuidData(GO_HORRIDON_ENTRANCE)))
                                    preDoor->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                                for (auto&& itr : windBunnysList)
                                    if (Creature* bunny = instance->GetCreature(itr))
                                        bunny->RemoveAllAreasTrigger();

                                break;
                            case SPECIAL:
                            {
                                for (auto&& itr : windBunnysList)
                                {
                                    if (Creature* bunny = instance->GetCreature(itr))
                                    {
                                        uint32 _spellid = 0;
                            
                                        switch (bunny->GetDBTableGUIDLow())
                                        {
                                            case 554505:
                                                _spellid = 140781;
                                                break;
                                            case 554538:
                                                _spellid = 139535;
                                                break;
                                            default:
                                                _spellid = 139499;
                                                break;
                                        }
                            
                                        bunny->CastSpell(bunny, _spellid, true);
                                    }
                                }
                            
                                break;
                            }
                        }
                        break;
                }

                if (data == DONE || data == SPECIAL)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_JINROKH_EVENT:
                        return hasJinrokhEventDone;
                    case DATA_PRE_HORIDON_EVENT:
                        return hasPreHorridonEventDone;
                    case DATA_HORIDON_EVENT:
                        return hasHoridonEventDone;
                    case DATA_PRE_COUNCIL_EVENT_1:
                        return hasPreGarajalEvent1Done;
                    case DATA_PRE_COUNCIL_EVENT_2:
                        return hasPreGarajalEvent2Done;
                    case DATA_COUNCIL_EVENT:
                        return hasgarajalEventDone;
                    case DATA_TORTOS_EVENT:
                        return hasTorgosEventDone;
                    case DATA_MEGERA_EVENT:
                        return hasMegeraEventDone;
                    case DATA_JI_KUN_EVENT:
                        return hasJikunEventDone;
                    case DATA_PRIMORDIUS_EVENT:
                        return hasPrimordiusEventDone;
                    case DATA_RA_DEN_EVENT:
                        return hasRaDenEventDone;
                    case DATA_LEI_SHEN_EVENT:
                        return hasThunderEventDone;
                    case LFR_DATA:
                        return isLfr ? 1 : 0;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    // Creatures here
                    case DATA_JINROKH:
                        return jinrokhGUID;
                    case DATA_HORRIDON:
                        return horidonGUID;
                    case DATA_COUNCIL_OF_ELDERS:
                        return garajalHelperGUID;
                    case DATA_TORTOS:
                        return tortosGUID;
                    case DATA_MEGAERA:
                        return megeraGUID;
                    case DATA_JI_KUN:
                        return jikunGUID;
                    case DATA_DURUMU_THE_FORGOTTEN:
                        return durumuGUID;
                    case DATA_PRIMORDIUS:
                        return primordiusGUID;
                    case DATA_DARK_ANIMUS:
                        return animusGUID;
                    case DATA_IRON_QON:
                        return ironQonGUID;
                    case DATA_TWIN_CONSORTS:
                        return suenGUID;
                    case DATA_LEI_SHEN:
                        return leishenGUID;
                    case DATA_RA_DEN:
                        return radenGUID;
                    case NPC_WAR_GOD_JALAK:
                        return jalakGUID;
                    case GO_JIN_ROKH_PREDOOR:
                        return preDoorJinrokhGUID;
                    case GO_HORRIDON_ENTRANCE:
                        return preHorridonDoorGUID;
                    case GO_COUNCIL_TRASH_DOOR_ONE:
                        return preCouncilDoor1GUID;
                    case GO_COUNCIL_TRASH_DOOR_TWO:
                        return preCouncilDoor2GUID;
                    case NPC_ROSHAK:
                    case NPC_QUETZAL:
                    case NPC_DAMREN:
                        return ironPets.find(type)->second;
                    case GO_TRIBAL_DOOR_FARRAKI:
                    case GO_TRIBAL_DOOR_GURUBASHI:
                    case GO_TRIBAL_DOOR_DRAKKARI:
                    case GO_TRIBAL_DOOR_AMANI:
                        return tribesDoor.find(type)->second;
                    case NPC_GARA_JALS_SOUL:
                        return garajalSoulGUID;
                    case NPC_COUNCIL_FROST_KING_MALAKK:
                        return malakkGUID;
                    case NPC_COUNCIL_HIGH_PRIESTESS_MARLI:
                        return marliGUID;
                    case NPC_COUNCIL_KAZRAJIN:
                        return kazrajinGUID;
                    case NPC_COUNCIL_SUL_THE_SANDCRAWLER:
                        return sulGUID;
                    case NPC_LEI_SHEN_TRIGGER:
                        return introLeiShenGUID;
                    case GO_TORTOS_BRIDGE:
                        return tortosBridgeGUID;
                    case GO_TORTOS_COLLISION:
                        return tortosDeathCollisionGuid;
                    case NPC_RUSHING_WINDS_CONTROLLER:
                        return rushingWindsControllerGUID;
                    case NPC_WORLD_TRIGGER: // phase controller
                        return twinConsortPhaseController_1;
                    case NPC_WORLD_TRIGGER + 1: // star sky
                        return twinConsortPhaseController_2;
                    case NPC_LULIN:
                        return lulinGUID;
                    case NPC_CONSORT_CONTROLLER:
                        return twinConsortController;
                    case NPC_DIFFUSION_CHAIN_CONDUIT:
                        return diffusionChainConduitGUID;
                    case NPC_STATIC_SHOCK_CONDUIT:
                        return staticShockConduitGUID;
                    case NPC_BOUNCING_BOLT_CONDUIT:
                        return bouncingBoltConduitGUID;
                    case NPC_OVERCHARGE_CONDUIT:
                        return overchargeConduitGUID;
                    case GO_CHARGING_STATION:
                        return chargingStationGUID;
                    case GO_THUNDER_KING_DISC_COLLISION:
                        return discCollisionGUID;
                    case GO_FLOOR_FX_SOUTH:
                        return southFloorGUID;
                    case GO_FLOOR_FX_NORTH:
                        return northFloorGUID;
                    case GO_FLOOR_FX_WEST:
                        return westFloorGUID;
                    case GO_FLOOR_FX_EAST:
                        return eastFloorGUID;
                    case GO_NORTH_PILLAR:
                        return northPillarGUID;
                    case GO_SOUTH_PILLAR:
                        return southPillarGUID;
                    case GO_EAST_PILLAR:
                        return eastPillarGUID;
                    case GO_WEST_PILLAR:
                        return westPillarGUID;
                    case GO_WIND_FX_NE:
                        return windNEGUID;
                    case GO_WIND_FX_NW:
                        return windNWGUID;
                    case GO_WIND_FX_SE:
                        return windSEGUID;
                    case GO_WIND_FX_SW:
                        return windSWGUID;
                    case GO_SE_WINDOW:
                        return windowSEGUID;
                    case GO_SW_WINDOW:
                        return windowSWGUID;
                    case GO_NW_WINDOW:
                        return windowNWGUID;
                    case GO_NE_WINDOW:
                        return windowNEGUID;
                    case NPC_SLG_GENERIC_MOP:
                        return megaeraAnnouncerGUID;
                    case NPC_EXIT_CHAMBER:
                        return chamberExitGUID;
                    case NPC_ANIMA_ORB:
                        return animaOrbGUID;
                    case GO_ANIMUS_ENTRANCE:
                        return animusEntranceDoorGUID;
                    case GO_IRON_QON_SEWER_GATE:
                        return ironQonDoorGUID;
                    case GO_MEGAERA_CHEST_HEROIC:
                        return megaeraHeroicChestGUID;
                    case GO_MEGAERA_CHEST_NORMAL:
                        return megaeraChestGUID;
                    case NPC_FADED_IMAGE_OF_CHI_JI:
                        return fadedChijiGUID;
                    case NPC_FADED_IMAGE_OF_NIUZAO:
                        return fadedNuzaoGUID;
                    case NPC_FADED_IMAGE_OF_XUEN:
                        return fadedHuenGUID;
                    case NPC_FADED_IMAGE_OF_YULON:
                        return fadedYulonGUID;
                }

                return ObjectGuid::Empty;
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "T O T " << GetBossSaveData() << hasJinrokhEventDone << ' ' << hasPreHorridonEventDone << ' ' << hasHoridonEventDone << ' ' << hasPreGarajalEvent1Done << ' ' << hasPreGarajalEvent2Done << ' ' << hasgarajalEventDone << ' ' << hasTorgosEventDone << ' ' << hasMegeraEventDone << ' ' << hasJikunEventDone << ' ' << hasDurumuEventDone << ' ' << hasPrimordiusEventDone << ' ' << hasAnimusEventDone << ' ' << hasCelestialEventDone << ' ' << hasThunderEventDone << ' ' << hasRaDenEventDone;

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

                char dataHead1, dataHead2, dataHead3;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> dataHead3;

                if (dataHead1 == 'T' && dataHead2 == 'O' && dataHead3 == 'T')
                {
                    for (uint8 i = 0; i < MAX_DATA; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp; // jinrokh event complete
                    hasJinrokhEventDone = temp;
                    SetData(DATA_JINROKH_EVENT, hasJinrokhEventDone);
                    loadStream >> temp;  // horridon pre event complete
                    hasPreHorridonEventDone = temp;
                    SetData(DATA_PRE_HORIDON_EVENT, hasPreHorridonEventDone);
                    loadStream >> temp;  // horridon event complete
                    hasHoridonEventDone = temp;
                    SetData(DATA_HORIDON_EVENT, hasHoridonEventDone);
                    loadStream >> temp; // Garajal prev event 1 complete
                    hasPreGarajalEvent1Done = temp;
                    SetData(DATA_PRE_COUNCIL_EVENT_1, hasPreGarajalEvent1Done);
                    loadStream >> temp; // Garajal prev event 2 complete
                    hasPreGarajalEvent2Done = temp;
                    SetData(DATA_PRE_COUNCIL_EVENT_2, hasPreGarajalEvent2Done);
                    loadStream >> temp; // Garajal event complete
                    hasgarajalEventDone = temp;
                    SetData(DATA_COUNCIL_EVENT, hasgarajalEventDone);
                    loadStream >> temp; // Megera event complete
                    hasTorgosEventDone = temp;
                    SetData(DATA_TORTOS_EVENT, hasTorgosEventDone);
                    loadStream >> temp; // Megera event complete
                    hasMegeraEventDone = temp;
                    SetData(DATA_MEGERA_EVENT, hasMegeraEventDone);
                    loadStream >> temp; // Jikun event complete
                    hasJikunEventDone = temp;
                    SetData(DATA_JI_KUN_EVENT, hasJikunEventDone);
                    loadStream >> temp; // Durumu event complete
                    hasDurumuEventDone = temp;
                    SetData(DATA_DURUMU_EVENT, hasDurumuEventDone);
                    loadStream >> temp; // Primordius event complete
                    hasPrimordiusEventDone = temp;
                    SetData(DATA_PRIMORDIUS_EVENT, hasPrimordiusEventDone);
                    loadStream >> temp; // Animus event complete
                    hasAnimusEventDone = temp;
                    SetData(DATA_ANIMUS_EVENT, hasAnimusEventDone);
                    loadStream >> temp; // Celestial event complete
                    hasCelestialEventDone = temp;
                    SetData(DATA_TWIN_CONSORT_EVENT, hasCelestialEventDone);
                    loadStream >> temp; // Lei Shen intro complete
                    hasThunderEventDone = temp;
                    SetData(DATA_LEI_SHEN_EVENT, hasThunderEventDone);
                    loadStream >> temp; // Ra-Den intro complete
                    hasRaDenEventDone = temp;
                    SetData(DATA_RA_DEN_EVENT, hasRaDenEventDone);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const override
            {
                if (!InstanceScript::CheckRequiredBosses(bossId, player))
                    return false;

                /*switch (bossId)
                {
                    case DATA_RA_DEN:
                    case DATA_LEI_SHEN:
                    case DATA_TWIN_CONSORTS:
                    case DATA_IRON_QON:
                    case DATA_DARK_ANIMUS:
                    case DATA_PRIMORDIUS:
                    case DATA_DURUMU_THE_FORGOTTEN:
                    case DATA_JI_KUN:
                    case DATA_MEGAERA:
                    case DATA_TORTOS:
                    case DATA_COUNCIL_OF_ELDERS:
                    case DATA_HORRIDON:
                        if (GetBossState(bossId - 1) != DONE)
                            return false;
                }*/

                return true;
            }

            void Update(uint32 diff) override
            {
                m_mEvents.Update(diff);

                while (uint32 eventId = m_mEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_JINROKH_DOOR:
                            HandleGameObject(GetGuidData(GO_JIN_ROKH_PREDOOR), true);
                            break;
                        case EVENT_PRIMORDIUS_INTRO:
                            if (Creature* pBoss = instance->GetCreature(GetGuidData(DATA_PRIMORDIUS)))
                                if (pBoss->AI())
                                    pBoss->AI()->DoAction(ACTION_START_INTRO);
                            break;
                        case EVENT_THUNDER_TRAP_INTRO:
                            for (auto&& itr : thunderTrapGuids)
                                if (Creature* thunderTrap = instance->GetCreature(itr))
                                    thunderTrap->CastSpell(thunderTrap, SPELL_THUNDER_TRAP, false);

                            m_mEvents.ScheduleEvent(EVENT_THUNDER_TRAP_INTRO, 27 * IN_MILLISECONDS);
                            break;
                        case EVENT_GOLEM_INITIALIZE:
                            SetData(GOLEM_DATA, DONE);
                            break;
                        case EVENT_RITUALIST_INITIALIZE:
                        {
                            // copy our spells to temp container and resize him
                            std::list<uint32> temp_ritualistReformId = ritualistReformId;
                            Trinity::Containers::RandomResizeList(temp_ritualistReformId, 6);

                            // Cast this spells to ritualists (it`ll form their name plates too)
                            for (auto&& itr : ritualistGuids)
                            {
                                if (Creature* darkRitualist = instance->GetCreature(itr))
                                {
                                    // just in case
                                    if (temp_ritualistReformId.empty())
                                        break;

                                    uint32 transformSpellID = *temp_ritualistReformId.begin();
                                    darkRitualist->CastSpell(darkRitualist, transformSpellID, true);
                                    darkRitualist->AI()->SetData(TYPE_RITUALIST, transformSpellID);
                                    temp_ritualistReformId.pop_front();
                                }
                            }
                            break;
                        }
                    }
                }
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
            return new instance_throne_of_thunder_InstanceScript(map);
        }
};

void AddSC_instance_throne_of_thunder()
{
    new instance_throne_of_thunder();
}