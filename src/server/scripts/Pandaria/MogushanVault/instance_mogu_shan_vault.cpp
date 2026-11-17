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
#include "mogu_shan_vault.h"
#include "Group.h"
#include "LFGMgr.h"

static std::vector<DoorData> const doorData =
{
    {GO_STONE_GUARD_DOOR_ENTRANCE,          DATA_STONE_GUARD,          DOOR_TYPE_ROOM,       BOUNDARY_E   },
    {GO_STONE_GUARD_DOOR_EXIT,              DATA_STONE_GUARD,          DOOR_TYPE_PASSAGE,    BOUNDARY_W   },
    {GO_FENG_DOOR_FENCE,                    DATA_FENG,                 DOOR_TYPE_ROOM,       BOUNDARY_NONE},
    {GO_FENG_DOOR_EXIT,                     DATA_FENG,                 DOOR_TYPE_PASSAGE,    BOUNDARY_N   },
    {GO_INVERSION,                          DATA_FENG,                 DOOR_TYPE_SPAWN_HOLE, BOUNDARY_NONE},
    {GO_CANCEL,                             DATA_FENG,                 DOOR_TYPE_SPAWN_HOLE, BOUNDARY_NONE},
    {GO_GARAJAL_FENCE,                      DATA_GARAJAL,              DOOR_TYPE_ROOM,       BOUNDARY_NONE},
    {GO_GARAJAL_EXIT,                       DATA_GARAJAL,              DOOR_TYPE_PASSAGE,    BOUNDARY_W   },
    {GO_SPIRIT_KINGS_WIND_WALL,             DATA_SPIRIT_KINGS,         DOOR_TYPE_ROOM,       BOUNDARY_NONE},
    {GO_SPIRIT_KINGS_EXIT,                  DATA_SPIRIT_KINGS,         DOOR_TYPE_PASSAGE,    BOUNDARY_N   },
    {GO_CELESTIAL_DOOR,                     DATA_ELEGON,               DOOR_TYPE_ROOM,       BOUNDARY_E   },
    {GO_ELEGON_DOOR_ENTRANCE,               DATA_SPIRIT_KINGS,         DOOR_TYPE_ROOM,       BOUNDARY_NONE},
    {GO_ELEGON_CELESTIAL_DOOR,              DATA_ELEGON,               DOOR_TYPE_ROOM,       BOUNDARY_E   },
    {GO_WILL_OF_EMPEROR_ENTRANCE,           DATA_ELEGON,               DOOR_TYPE_PASSAGE,    BOUNDARY_NONE},
    {GO_TERRACOTTA_DOOR,                    DATA_WILL_OF_EMPEROR,      DOOR_TYPE_ROOM,       BOUNDARY_NONE},
};

#define DIST_BETWEEN_TWO_Z      32.39f
#define ACHIEVEMENT_SHOWMOVES   6455

class instance_mogu_shan_vault : public InstanceMapScript
{
    public:
        instance_mogu_shan_vault() : InstanceMapScript("instance_mogu_shan_vault", 1008) { }

        struct instance_mogu_shan_vault_InstanceMapScript : public InstanceScript
        {
            instance_mogu_shan_vault_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;

            uint32 FrenzyDiff;
            uint32 stoneGuardiansState;
            uint32 actualPetrifierEntry;

            ObjectGuid cursedMogu1Guid;
            ObjectGuid cursedMogu2Guid;
            ObjectGuid ghostEssenceGuid;

            ObjectGuid stoneGuardControllerGuid;
            ObjectGuid fengGuid;
            ObjectGuid siphonShieldGuid;
            ObjectGuid spiritKingsControllerGuid;
            ObjectGuid elegonGuid;
            ObjectGuid infiniteEnergyGuid;
            ObjectGuid GarajalGuid;

            ObjectGuid inversionGobGuid;
            ObjectGuid stoneGuardExit;
            ObjectGuid cancelGobGuid;
            ObjectGuid ancientMoguDoorGuid;
            ObjectGuid emperorsDoorGuid;
            ObjectGuid celestialCommandGuid;

            ObjectGuid energyPlatformGuid;
            ObjectGuid titanDiskGuid;
            ObjectGuid janxiGuid;
            ObjectGuid qinxiGuid;
            ObjectGuid ancienControlCondoleGUID;

            uint32 HasFengEventDone;
            uint32 HasGarajalEventDone;
            uint32 HasSpiritKingsEventDone;
            uint32 SpiritKingsSeq;
            uint32 SecondSpirit;
            uint32 ThirdSpirit;
            uint32 FourSpirit;

            std::vector<ObjectGuid> stoneGuardGUIDs;
            std::vector<ObjectGuid> fengStatuesGUIDs;
            std::vector<ObjectGuid> spiritKingsGUIDs;
            std::vector<ObjectGuid> titanCirclesGuids;
            std::vector<uint32> achievementGuids;
            std::vector<uint32> m_spiritKingsSeq;
            std::list<ObjectGuid> SkullChargerGuids;

            bool isLfr;
            bool lfrSectionFound;
            bool isSecondLfrQuarter;

            void Initialize() override
            {
                SetBossNumber(ENCOUNTERS);
                LoadDoorData(doorData);
                stoneGuardiansState       = NOT_STARTED;
                stoneGuardControllerGuid  = ObjectGuid::Empty;
                fengGuid                  = ObjectGuid::Empty;
                siphonShieldGuid          = ObjectGuid::Empty;
                inversionGobGuid          = ObjectGuid::Empty;
                cancelGobGuid             = ObjectGuid::Empty;
                ancientMoguDoorGuid       = ObjectGuid::Empty;
                emperorsDoorGuid          = ObjectGuid::Empty;
                celestialCommandGuid      = ObjectGuid::Empty;
                energyPlatformGuid        = ObjectGuid::Empty;
                titanDiskGuid             = ObjectGuid::Empty;
                cursedMogu1Guid           = ObjectGuid::Empty;
                cursedMogu2Guid           = ObjectGuid::Empty;
                ghostEssenceGuid          = ObjectGuid::Empty;
                spiritKingsControllerGuid = ObjectGuid::Empty;
                qinxiGuid                 = ObjectGuid::Empty;
                janxiGuid                 = ObjectGuid::Empty;
                ancienControlCondoleGUID  = ObjectGuid::Empty;
                GarajalGuid               = ObjectGuid::Empty;
                FrenzyDiff = instance->Is25ManRaid() ? 2 : 1;

                stoneGuardGUIDs.clear();
                fengStatuesGUIDs.clear();
                spiritKingsGUIDs.clear();
                SkullChargerGuids.clear();
                m_spiritKingsSeq.clear();

                HasFengEventDone                = 0;
                HasGarajalEventDone             = 0;
                HasSpiritKingsEventDone         = 0;
                SpiritKingsSeq                  = 0;
                SecondSpirit                    = 0;
                ThirdSpirit                     = 0;
                FourSpirit                      = 0;

                isLfr = instance->ToInstanceMap()->IsLFGMap();
                lfrSectionFound = false;
                isSecondLfrQuarter = false;
            }

            void OnPlayerEnter(Player* player) override
            {
                if (isLfr && !lfrSectionFound)
                {
                    lfrSectionFound = true;
                    Group* group = player->GetGroup();
                    if (!group)
                        return;
                    if (group->isLFGGroup())
                        isSecondLfrQuarter = sLFGMgr->GetDungeon(group->GetGUID()) == 528;

                    if (isSecondLfrQuarter)
                    {
                        for (auto&& stoneGuard : stoneGuardGUIDs)
                            if (Creature* boss = instance->GetCreature(stoneGuard))
                                boss->DespawnOrUnsummon();
                        if (Creature* boss = instance->GetCreature(fengGuid))
                            boss->DespawnOrUnsummon();
                        if (Creature* boss = instance->GetCreature(GarajalGuid))
                            boss->DespawnOrUnsummon();
                        SetBossState(DATA_STONE_GUARD, DONE);
                        SetBossState(DATA_FENG, DONE);
                        SetBossState(DATA_GARAJAL, DONE);
                    }
                }

                if (isSecondLfrQuarter)
                    player->NearTeleportTo(EncountersEntrance[2].GetPositionX(), EncountersEntrance[2].GetPositionY(), EncountersEntrance[2].GetPositionZ(), EncountersEntrance[2].GetOrientation());

                // Determination
                if (isLfr && GetDeterminationValue())
                    if (Aura* detAura = player->AddAura(SPELL_DETERMINATION_LFR, player))
                        detAura->SetStackAmount(GetDeterminationValue());

                if (isLfr || GetEncountersDoneCount(ENCOUNTERS) < 1 || player->GetSession()->PlayerLoading() || player->IsGameMaster())
                    return;

                Position pos = GetCurrentEntrancePosition();
                player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
            }

            void OnPlayerLeave(Player* player) override
            {
                if (player->HasAura(116541))
                    player->RemoveAurasDueToSpell(116541);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (isLfr && !lfrSectionFound)
                {
                    Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (Player* player = players.begin()->GetSource())
                            OnPlayerEnter(player);
                }

                if (isLfr)
                {
                    uint32 entry = creature->GetEntry();
                    if (isSecondLfrQuarter)
                    {
                        if (entry == NPC_JASPER || entry == NPC_JADE || entry == NPC_AMETHYST || entry == NPC_COBALT || entry == NPC_STONE_GUARD_CONTROLLER || entry == NPC_FENG || entry == NPC_GARAJAL || (entry == NPC_LOREWALKER_CHO && creature->GetDBTableGUIDLow()))
                        {
                            creature->DespawnOrUnsummon();
                            return;
                        }
                    }
                    else
                    {
                        if (entry == NPC_ZIAN || entry == NPC_MENG || entry == NPC_QIANG || entry == NPC_SUBETAI || entry == NPC_ELEGON || entry == NPC_QIN_XI || entry == NPC_JAN_XI)
                        {
                            creature->DespawnOrUnsummon();
                            return;
                        }
                    }
                }

                switch (creature->GetEntry())
                {
                    case NPC_FRENZIED_STONE_QUILEN: // Frenzyed Stone Quilen selector
                    {
                        creature->SetVisible(false);

                        std::list<Creature*> StoneQuilens;
                        GetCreatureListWithEntryInGrid(StoneQuilens, creature, NPC_STONE_QUILEN, 20.0f);

                        if (!StoneQuilens.empty())
                        {
                            Trinity::Containers::RandomResizeList(StoneQuilens, FrenzyDiff);

                            for (auto itr : StoneQuilens)
                                if (itr->AI())
                                    itr->AI()->DoAction(1);
                        }
                        break;
                    }
                    case NPC_STONE_GUARD_CONTROLLER:
                        stoneGuardControllerGuid = creature->GetGUID();
                        break;
                    case NPC_JASPER:
                    case NPC_JADE:
                    case NPC_AMETHYST:
                    case NPC_COBALT:
                    {
                        stoneGuardGUIDs.push_back(creature->GetGUID());

                        auto const difficulty = instance->GetSpawnMode();
                        auto const turnOver = difficulty == RAID_DIFFICULTY_10MAN_NORMAL || difficulty == RAID_DIFFICULTY_10MAN_HEROIC || difficulty == RAID_DIFFICULTY_25MAN_LFR;

                        if (stoneGuardGUIDs.size() >= 4 && GetData(DATA_STONE_GUARD_STATE) == NOT_STARTED && GetBossState(DATA_STONE_GUARD) != DONE && turnOver)
                        {
                            SetData(DATA_STONE_GUARD_STATE, DONE);

                            QueryResult result = WorldDatabase.PQuery("SELECT `creature_id` FROM `instance_mogushan_system` ORDER BY `id` DESC LIMIT 1;");
                            uint64 toDespawnEntry = 0;
                            if (result)
                                toDespawnEntry = (*result)[0].GetUInt64();

                            ObjectGuid toDespawn = creature->GetEntry() == toDespawnEntry ? creature->GetGUID() : this->GetGuidData(toDespawnEntry);
                            Creature* stoneGuard = (toDespawn != creature->GetGUID())
                                    ? instance->GetCreature(toDespawn)
                                    : creature;

                            if (stoneGuard)
                            {
                                stoneGuard->DespawnOrUnsummon();
                                stoneGuardGUIDs.erase(std::remove(stoneGuardGUIDs.begin(), stoneGuardGUIDs.end(), toDespawn));
                            }
                        }
                        break;
                    }
                    case NPC_CURSED_MOGU_SCULPTURE_2:
                        if (!cursedMogu1Guid)
                            cursedMogu1Guid = creature->GetGUID();
                        else
                            cursedMogu2Guid = creature->GetGUID();
                        break;
                    case NPC_GHOST_ESSENCE:
                        ghostEssenceGuid = creature->GetGUID();
                        break;
                    case NPC_FENG:
                        fengGuid = creature->GetGUID();
                        break;
                    case NPC_SIPHONING_SHIELD:
                        siphonShieldGuid = creature->GetGUID();
                        break;
                    case NPC_ZANDALARI_SKULLCHARGER:
                        SkullChargerGuids.push_back(creature->GetGUID());
                        break;
                    case NPC_GARAJAL:
                        GarajalGuid = creature->GetGUID();
                        break;
                    case NPC_SPIRIT_GUID_CONTROLLER:
                        spiritKingsControllerGuid = creature->GetGUID();
                        break;
                    case NPC_QIANG:
                    case NPC_SUBETAI:
                    case NPC_ZIAN:
                    case NPC_MENG:
                        spiritKingsGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_ELEGON:
                        elegonGuid = creature->GetGUID();
                        break;
                    case NPC_INFINITE_ENERGY:
                        infiniteEnergyGuid = creature->GetGUID();
                        break;
                    case NPC_QIN_XI:
                        qinxiGuid = creature->GetGUID();
                        break;
                    case NPC_JAN_XI:
                        janxiGuid = creature->GetGUID();
                        break;
                    case NPC_EXPLOSION_BUNNY_N_M:
                    case NPC_EXPLOSION_BUNNY_N_P:
                    case NPC_EXPLOSION_BUNNY_S_M:
                    case NPC_EXPLOSION_BUNNY_S_P:
                    case NPC_EXPLOSION_BUNNY_E_M:
                    case NPC_EXPLOSION_BUNNY_E_P:
                    case NPC_EXPLOSION_BUNNY_W_M:
                    case NPC_EXPLOSION_BUNNY_W_P:
                        creature->SetDisplayId(17612);
                        break;
                    case NPC_TERRACOTA_BOSS:
                        creature->SetDisplayId(11686);
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                if (unit && unit->ToPlayer() && unit->ToPlayer()->HasAura(116541))
                    unit->RemoveAurasDueToSpell(116541);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_STONE_GUARD_DOOR_ENTRANCE:
                    case GO_FENG_DOOR_FENCE:
                    case GO_FENG_DOOR_EXIT:
                    case GO_GARAJAL_FENCE:
                    case GO_SPIRIT_KINGS_WIND_WALL:
                    case GO_SPIRIT_KINGS_EXIT:
                    case GO_CELESTIAL_DOOR:
                    case GO_TERRACOTTA_DOOR:
                        AddDoor(go, true);
                        break;
                    case GO_GARAJAL_EXIT:
                        if (!isLfr)
                            AddDoor(go, true);
                        break;
                    // Feng
                    case GO_SPEAR_STATUE:
                    case GO_FIST_STATUE:
                    case GO_STAFF_STATUE:
                        fengStatuesGUIDs.push_back(go->GetGUID());
                        break;
                    case GO_SHIELD_STATUE:
                    {
                        if (!instance->IsHeroic())
                            go->SetObjectScale(0.001f);
                        else
                            fengStatuesGUIDs.push_back(go->GetGUID());
                        break;
                    }
                    case GO_STONE_GUARD_DOOR_EXIT:
                        AddDoor(go, true);
                        stoneGuardExit = go->GetGUID();
                        break;
                    case GO_INVERSION:
                        AddDoor(go, true);
                        inversionGobGuid = go->GetGUID();
                        break;
                    case GO_CANCEL:
                        AddDoor(go, true);
                        cancelGobGuid = go->GetGUID();
                        break;
                    case GO_ENERGY_PLATFORM:
                        energyPlatformGuid = go->GetGUID();
                        go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    case GO_ELEGON_DOOR_ENTRANCE:
                        ancientMoguDoorGuid = go->GetGUID();
                        break;
                    case GO_WILL_OF_EMPEROR_ENTRANCE:
                        emperorsDoorGuid = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_ENERGY_TITAN_DISK:
                        titanDiskGuid = go->GetGUID();
                        break;
                    case GO_ENERGY_TITAN_CIRCLE_1:
                    case GO_ENERGY_TITAN_CIRCLE_2:
                    case GO_ENERGY_TITAN_CIRCLE_3:
                        go->SetGoState(GO_STATE_ACTIVE);
                        titanCirclesGuids.push_back(go->GetGUID());
                        break;
                    case GO_CELESTIAL_COMMAND:
                        if (isLfr && !isSecondLfrQuarter)
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        else
                            celestialCommandGuid = go->GetGUID();
                        break;
                    case GO_ANCIEN_CONTROL_CONSOLE:
                        if (isLfr && !isSecondLfrQuarter)
                            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
                        else
                            ancienControlCondoleGUID = go->GetGUID();
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_STONE_GUARD:
                    {
                        switch (state)
                        {
                            case IN_PROGRESS:
                                if (Creature* stoneGuardController = instance->GetCreature(stoneGuardControllerGuid))
                                    stoneGuardController->AI()->DoAction(ACTION_ENTER_COMBAT);

                                for (auto&& stoneGuardGuid : stoneGuardGUIDs)
                                    if (Creature* stoneGuard = instance->GetCreature(stoneGuardGuid))
                                        stoneGuard->AI()->DoAction(ACTION_ENTER_COMBAT);
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    case DATA_FENG:
                    {
                        if (state == DONE)
                            for (auto &itr : SkullChargerGuids)
                                if (Creature* SkullCharger = instance->GetCreature(itr))
                                    if (SkullCharger->AI())
                                        SkullCharger->AI()->DoAction(1);
                        break;
                    }
                    case DATA_SPIRIT_KINGS:
                    {
                        switch (state)
                        {
                            case IN_PROGRESS:
                            {
                                if (Creature* spiritKingsController = instance->GetCreature(spiritKingsControllerGuid))
                                    spiritKingsController->AI()->DoAction(ACTION_ENTER_COMBAT);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    case DATA_ELEGON:
                    {
                        switch (state)
                        {
                            case TO_BE_DECIDED:
                            {
                                if (GameObject* titanDisk = instance->GetGameObject(titanDiskGuid))
                                    titanDisk->SetGoState(GO_STATE_ACTIVE);

                                if (GameObject* energyPlatform = instance->GetGameObject(energyPlatformGuid))
                                    energyPlatform->SetGoState(GO_STATE_ACTIVE);

                                for (auto guid: titanCirclesGuids)
                                    if (GameObject* titanCircles = instance->GetGameObject(guid))
                                        titanCircles->SetGoState(GO_STATE_ACTIVE);

                                break;
                            }
                            case FAIL:
                            {
                                if (GameObject* titanDisk = instance->GetGameObject(titanDiskGuid))
                                    titanDisk->SetGoState(GO_STATE_READY);

                                if (GameObject* energyPlatform = instance->GetGameObject(energyPlatformGuid))
                                    energyPlatform->SetGoState(GO_STATE_READY);

                                for (auto guid: titanCirclesGuids)
                                    if (GameObject* titanCircles = instance->GetGameObject(guid))
                                        titanCircles->SetGoState(GO_STATE_READY);

                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    }
                    case DATA_WILL_OF_EMPEROR:
                    {
                        switch (state)
                        {
                            case NOT_STARTED:
                            case FAIL:
                                events.ScheduleEvent(EVENT_RESET_WOE_CONSOLE, 35 * IN_MILLISECONDS);

                                if (GameObject* console = instance->GetGameObject(ancienControlCondoleGUID))
                                    console->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                break;
                            case IN_PROGRESS:
                                if (GameObject* console = instance->GetGameObject(ancienControlCondoleGUID))
                                    console->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);
                                break;
                            case DONE:
                                DoFinishLFGDungeon(528);
                                break;
                            default:
                                break;
                        }
                    }
                }

                PromotionIfLFR(state);

                return true;
            }

            Position GetCurrentEntrancePosition() override
            {
                uint32 m_encounterDone = GetEncountersDoneCount(ENCOUNTERS);

                switch (m_encounterDone) // begun enum from bossId where entrance was changed
                {
                    case DATA_FENG:
                    case DATA_GARAJAL:
                        return EncountersEntrance[1];
                    case DATA_SPIRIT_KINGS:
                    case DATA_ELEGON:
                    case DATA_WILL_OF_EMPEROR:
                        return EncountersEntrance[2];
                }

                return EncountersEntrance[0];
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case ACHIEVEMENT_SHOWMOVES:
                        SetAchievementValid(ACHIEVEMENT_SHOWMOVES);
                        break;
                    case DATA_STONE_GUARD_STATE:
                        stoneGuardiansState = data;
                        break;
                    case DATA_FENG:
                        HasFengEventDone = data;
                        break;
                    case DATA_GARAJAL:
                        HasGarajalEventDone = data;
                        break;
                    case DATA_SPIRIT_KINGS:
                        HasSpiritKingsEventDone = data;
                        break;
                    case DATA_SPIRIT_KINGS_STATE:
                        SpiritKingsSeq = data;
                        break;
                    case DATA_SPIRIT_SECOND:
                        SecondSpirit = data;
                        SaveToDB();
                        break;
                    case DATA_SPIRIT_THIRD:
                        ThirdSpirit = data;
                        SaveToDB();
                        break;
                    case DATA_SPIRIT_FOUR:
                        FourSpirit = data;
                        SaveToDB();
                        break;
                }

                if (data == DONE || data == SPECIAL)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case ACHIEVEMENT_SHOWMOVES:
                        return IsAchievementValid(ACHIEVEMENT_SHOWMOVES);
                    case DATA_STONE_GUARD_STATE:
                        return stoneGuardiansState;
                    case DATA_FENG:
                        return HasFengEventDone;
                    case DATA_GARAJAL:
                        return HasGarajalEventDone;
                    case DATA_SPIRIT_KINGS:
                        return HasSpiritKingsEventDone;
                    case DATA_SPIRIT_KINGS_STATE:
                        return SpiritKingsSeq;
                    case DATA_SPIRIT_SECOND:
                        return SecondSpirit;
                    case DATA_SPIRIT_THIRD:
                        return ThirdSpirit;
                    case DATA_SPIRIT_FOUR:
                        return FourSpirit;
                    case DATA_LFR:
                        return isLfr;
                    case DATA_LFR_SECOND_QUARTER:
                        return isSecondLfrQuarter;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    // Creature
                    // Stone Guard
                    case NPC_STONE_GUARD_CONTROLLER:
                        return stoneGuardControllerGuid;
                    case NPC_CURSED_MOGU_SCULPTURE_1:
                        return cursedMogu1Guid;
                    case NPC_CURSED_MOGU_SCULPTURE_2:
                        return cursedMogu2Guid;
                    case NPC_GHOST_ESSENCE:
                        return ghostEssenceGuid;
                    case NPC_JASPER:
                    case NPC_JADE:
                    case NPC_AMETHYST:
                    case NPC_COBALT:
                    {
                        for (auto&& guid: stoneGuardGUIDs)
                            if (Creature* stoneGuard = instance->GetCreature(guid))
                                if (stoneGuard->GetEntry() == type)
                                    return guid;
                                break;
                    }
                    // Feng
                    case NPC_FENG:
                        return fengGuid;
                        // SiphonShield
                    case NPC_SIPHONING_SHIELD:
                        return siphonShieldGuid;
                    case NPC_GARAJAL:
                        return GarajalGuid;
                        // Spirit Kings
                    case NPC_SPIRIT_GUID_CONTROLLER:
                        return spiritKingsControllerGuid;
                    case NPC_ZIAN:
                    case NPC_MENG:
                    case NPC_QIANG:
                    case NPC_SUBETAI:
                    {
                        for (auto guid: spiritKingsGUIDs)
                            if (Creature* spiritKing = instance->GetCreature(guid))
                                if (spiritKing->GetEntry() == type)
                                    return guid;
                                break;
                    }
                    // Elegon
                    case NPC_ELEGON:
                        return elegonGuid;
                    case NPC_INFINITE_ENERGY:
                        return infiniteEnergyGuid;
                        // Will of Emperor
                    case NPC_QIN_XI:
                        return qinxiGuid;
                    case NPC_JAN_XI:
                        return janxiGuid;
                        // Gameobject
                    case GO_SPEAR_STATUE:
                    case GO_FIST_STATUE:
                    case GO_SHIELD_STATUE:
                    case GO_STAFF_STATUE:
                    {
                        for (auto guid: fengStatuesGUIDs)
                            if (GameObject* fengStatue = instance->GetGameObject(guid))
                                if (fengStatue->GetEntry() == type)
                                    return guid;
                                break;
                    }
                    case GO_STONE_GUARD_DOOR_EXIT:
                        return stoneGuardExit;
                    case GO_INVERSION:
                        return inversionGobGuid;
                    case GO_CANCEL:
                        return cancelGobGuid;
                    case GO_ENERGY_PLATFORM:
                        return energyPlatformGuid;
                    case GO_ENERGY_TITAN_DISK:
                        return titanDiskGuid;
                    case GO_ELEGON_DOOR_ENTRANCE:
                        return ancientMoguDoorGuid;
                    case GO_WILL_OF_EMPEROR_ENTRANCE:
                        return emperorsDoorGuid;
                    case GO_CELESTIAL_COMMAND:
                        return celestialCommandGuid;
                    case GO_ANCIEN_CONTROL_CONSOLE:
                        return ancienControlCondoleGUID;
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

                        if (player->IsAlive() && !player->IsGameMaster() && !player->HasAura(115877)) // Aura 115877 = Totaly Petrified
                            return false;
                    }
                }

                return true;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_RESET_WOE_CONSOLE)
                {
                    if (GameObject* console = instance->GetGameObject(ancienControlCondoleGUID))
                    {
                        console->SetGoState(GO_STATE_READY);
                        console->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE | GO_FLAG_IN_USE);
                    }
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "M S V " << GetBossSaveData() << stoneGuardiansState << ' ' << HasFengEventDone << ' ' << HasGarajalEventDone << ' ' << HasSpiritKingsEventDone << ' ' << SpiritKingsSeq << ' ' << SecondSpirit << ' ' << ThirdSpirit << ' ' << FourSpirit;

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

                if (dataHead1 == 'M' && dataHead2 == 'S' && dataHead3 == 'V')
                {
                    for (uint8 i = 0; i < ENCOUNTERS; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    uint32 temp = 0;
                    loadStream >> temp;
                    stoneGuardiansState = temp ? DONE : NOT_STARTED;
                    loadStream >> temp; // feng event complete
                    HasFengEventDone = temp;
                    SetData(DATA_FENG, HasFengEventDone);
                    loadStream >> temp; // Garajal event complete
                    HasGarajalEventDone = temp;
                    SetData(DATA_GARAJAL, HasGarajalEventDone);
                    loadStream >> temp; // Spirit Kings event complete
                    HasSpiritKingsEventDone = temp;
                    SetData(DATA_SPIRIT_KINGS, HasSpiritKingsEventDone);
                    loadStream >> temp; // Spirit Kings seq is complete
                    SpiritKingsSeq = temp;
                    SetData(DATA_SPIRIT_KINGS_STATE, SpiritKingsSeq);
                    loadStream >> temp; // Second spirit that`ll activated after Qiang
                    SecondSpirit = temp;
                    SetData(DATA_SPIRIT_SECOND, SecondSpirit);
                    loadStream >> temp; // Third spirit that`ll activated after Qiang
                    ThirdSpirit = temp;
                    SetData(DATA_SPIRIT_THIRD, ThirdSpirit);
                    loadStream >> temp; // Four spirit that`ll activated after Qiang
                    FourSpirit = temp;
                    SetData(DATA_SPIRIT_FOUR, FourSpirit);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            bool CheckRequiredBosses(uint32 bossId, Player const* player = NULL) const override
            {
                if (!InstanceScript::CheckRequiredBosses(bossId, player))
                    return false;

                switch (bossId)
                {
                    case DATA_WILL_OF_EMPEROR:
                    case DATA_ELEGON:
                    case DATA_SPIRIT_KINGS:
                    case DATA_GARAJAL:
                    case DATA_FENG:
                        if (GetBossState(bossId - 1) != DONE)
                            return false;
                }

                return true;
            }

            bool IsAchievementValid(uint32 id) const
            {
                if (achievementGuids[id])
                    return true;

                return false;
            }

            void SetAchievementValid(uint32 id)
            {
                if (achievementGuids[id])
                    return;

                achievementGuids.push_back(id);
                return;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_mogu_shan_vault_InstanceMapScript(map);
        }
};

void AddSC_instance_mogu_shan_vault()
{
    new instance_mogu_shan_vault();
}
