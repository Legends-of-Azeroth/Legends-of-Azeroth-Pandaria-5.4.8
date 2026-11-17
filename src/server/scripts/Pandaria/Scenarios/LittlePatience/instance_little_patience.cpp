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
#include "little_patience.h"
#include "ScenarioMgr.h"

class instance_little_patience : public InstanceMapScript
{
    public:
        instance_little_patience() : InstanceMapScript("instance_little_patience", 1104) { }

        struct instance_little_patience_InstanceMapScript : public InstanceScript
        {
            instance_little_patience_InstanceMapScript(Map* map) : InstanceScript(map) { }

            EventMap events;
            uint32 m_auiEncounter[2];
            uint32 chapterOne, chapterTwo;
            ObjectGuid varianGUID;
            ObjectGuid varianIntroGUID;
            ObjectGuid tyrandeGUID;
            ObjectGuid tyrandeIntroGUID;
            ObjectGuid scargashGUID;
            ObjectGuid chiJiIntroGUID;
            ObjectGuid chiJiGUID;
            ObjectGuid adlerGUID;
            ObjectGuid roseyGUID;
            ObjectGuid feraGUID;
            ObjectGuid duffGUID;
            ObjectGuid brownstoneGUID;
            uint32 constructionsCompleted;
            uint32 factionData;
            std::vector<ObjectGuid> complitionWorkCreatureGUIDs;
            std::vector<ObjectGuid> constructionGUIDs;
            std::vector<ObjectGuid> korkronAssaultGUIDs;
            bool hasSecondStage;
            bool hasInit;

            void Initialize() override
            {
                SetBossNumber(2);
                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                chapterOne       = 0;
                chapterTwo       = 0;

                varianGUID = ObjectGuid::Empty;
                varianIntroGUID = ObjectGuid::Empty;
                tyrandeGUID = ObjectGuid::Empty;
                tyrandeIntroGUID = ObjectGuid::Empty;
                scargashGUID = ObjectGuid::Empty;
                chiJiIntroGUID = ObjectGuid::Empty;
                chiJiGUID = ObjectGuid::Empty;
                adlerGUID = ObjectGuid::Empty;
                roseyGUID = ObjectGuid::Empty;
                feraGUID = ObjectGuid::Empty;
                duffGUID = ObjectGuid::Empty;
                brownstoneGUID = ObjectGuid::Empty;
                factionData      = 0;
                hasSecondStage   = false;
                hasInit          = false;

                complitionWorkCreatureGUIDs.clear();
                constructionGUIDs.clear();
                korkronAssaultGUIDs.clear();
                events.ScheduleEvent(1, 40 * IN_MILLISECONDS + 8 * IN_MILLISECONDS);
            }

            void OnPlayerEnter(Player* player) override
            {
                // Init Scenario
                sScenarioMgr->SendScenarioState(player, 1104, DATA_PREPARE_CONSTRUCTS, 0);
                player->GetSceneMgr().PlaySceneByPackageId(316);
                player->CastSpell(player, SPELL_PERIODIC_CONSTRUCTION_CHECK, true);

                if (!hasInit)
                {
                    hasInit = true;
                    factionData = player->GetTeam();
                }
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_KING_VARIAN_WRYNN:
                        if (creature->GetPositionX() < -1100.0f)
                        {
                            varianGUID = creature->GetGUID();
                            creature->SetVisible(false);
                        }
                        else
                            varianIntroGUID = creature->GetGUID();
                        break;
                    case NPC_TYRANDE_WHISPERWIND:
                        if (creature->GetPositionX() < -1100.0f)
                        {
                            tyrandeGUID = creature->GetGUID();
                            creature->SetVisible(false);
                        }
                        else
                            tyrandeIntroGUID = creature->GetGUID();
                        break;
                    case NPC_COMMANDER_SCARGASH:
                        creature->SetVisible(false);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                        scargashGUID = creature->GetGUID();
                        break;
                    case NPC_GLAIVE_THROWER:
                    case NPC_CANNON:
                    case NPC_ANCIENT_DEFENDER:
                    case NPC_BLAST_BREW_KEG:
                        creature->SetVisible(false);
                        complitionWorkCreatureGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_MOUND_OF_SOIL:
                    case NPC_SAPLING:
                    case NPC_BURGEONING_SAPLING:
                    case NPC_WATER_BUCKET:
                    case NPC_WOODPILE:
                    case NPC_FOOD:
                    case NPC_NOURSIHMENT:
                    case NPC_CANNON_BARREL:
                    case NPC_CANNON_BALLS:
                    case NPC_WOODPILE_2:
                    case NPC_KRASARI_IRON_SUPPLY:
                    case NPC_CNAKE_OIL_BOILER:
                    case NPC_WOODPILE_3:
                        constructionGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_CHI_JI:
                        if (creature->GetPositionX() < -1100.0f)
                        {
                            chiJiGUID = creature->GetGUID();
                            creature->SetVisible(false);
                        }
                        else
                            chiJiIntroGUID = creature->GetGUID();
                        break;
                    case NPC_ELDER_ADLER:
                        if (!creature->GetDBTableGUIDLow())
                            adlerGUID = creature->GetGUID();
                        break;
                    case NPC_ROSEY_AXLEROD:
                        if (!creature->GetDBTableGUIDLow())
                            roseyGUID = creature->GetGUID();
                        break;
                    case NPC_FERA_PEARL:
                        if (!creature->GetDBTableGUIDLow())
                            feraGUID = creature->GetGUID();
                        break;
                    case NPC_DUFF_MCSTRUM:
                        if (!creature->GetDBTableGUIDLow())
                            duffGUID = creature->GetGUID();
                        break;
                    case NPC_MASTER_BROWNSTONE:
                        if (!creature->GetDBTableGUIDLow())
                            brownstoneGUID = creature->GetGUID();
                        break;
                    case NPC_KORKRON_ASSAILANT:
                        korkronAssaultGUIDs.push_back(creature->GetGUID());
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_PREPARE_CONSTRUCTS:
                        chapterOne = data;

                        for (auto&& itr : instance->GetPlayers())
                            if (Player* player = itr.GetSource())
                                sScenarioMgr->SendScenarioState(player, 1104, DATA_SCARGASH_DEFEAT, 0);

                        // Set visible new objects
                        for (auto&& itr : complitionWorkCreatureGUIDs)
                            if (Creature* construction = instance->GetCreature(itr))
                                construction->SetVisible(true);

                        // Remove old constructions
                        for (auto&& itr : constructionGUIDs)
                            if (Creature* construction = instance->GetCreature(itr))
                                construction->SetVisible(false);

                        DoRemoveAurasDueToSpellOnPlayers(SPELL_PERIODIC_CONSTRUCTION_CHECK);

                        if (Creature* varian = instance->GetCreature(GetGuidData(NPC_KING_VARIAN_WRYNN)))
                            varian->AI()->DoAction(ACTION_FORCE_ASSAULT_TEMPLE);

                        // Set him Visible
                        if (Creature* scargash = instance->GetCreature(GetGuidData(NPC_COMMANDER_SCARGASH)))
                            scargash->SetVisible(true);
                        break;
                    case DATA_SCARGASH_DEFEAT:
                        chapterTwo = data;

                        // Remove old creatures & set new
                        if (Creature* varian = instance->GetCreature(GetGuidData(NPC_KING_VARIAN_WRYNN)))
                            varian->DespawnOrUnsummon();

                        if (Creature* tyrande = instance->GetCreature(GetGuidData(NPC_TYRANDE_WHISPERWIND)))
                            tyrande->DespawnOrUnsummon();

                        if (Creature* chiji = instance->GetCreature(GetGuidData(NPC_CHI_JI)))
                            chiji->DespawnOrUnsummon();

                        // Send past event progress
                        if (Creature* varian = instance->GetCreature(GetGuidData(NPC_KING_VARIAN_WRYNN + 1)))
                            varian->AI()->DoAction(ACTION_SCARGASH_SLAIN);

                        if (Creature* tyrande = instance->GetCreature(GetGuidData(NPC_TYRANDE_WHISPERWIND + 1)))
                        {
                            tyrande->SetVisible(true);
                            tyrande->GetMotionMaster()->MovePoint(0, tyrandeOuterPos);
                        }

                        if (Creature* chiJi = instance->GetCreature(GetGuidData(NPC_CHI_JI + 1)))
                        {
                            chiJi->SetVisible(true);
                            
                            // Back to my temple!
                            Movement::MoveSplineInit init(chiJi);
                            for (auto&& itr : chiJiTemplePath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetUncompressed();
                            init.SetVelocity(6.5f);
                            init.Launch();
                        }

                        // Credit scenario there
                        SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_SCARGASH_SLAIN, 1, GetScenarioGUID(), time(NULL), 0, 0));
                        DoFinishLFGDungeon(GetData(FACTION_DATA) ? LFG_LITTLE_PATIENCE_H : LFG_LITTLE_PATIENCE_A);
                        break;
                    case DATA_CONSTRUCTION_COUNT:
                        if (++constructionsCompleted > 1 && !hasSecondStage)
                        {
                            hasSecondStage = true;
                            SetData(DATA_PREPARE_CONSTRUCTS, DONE);
                        }

                        if (constructionsCompleted < 3)
                            SendScenarioProgressUpdate(CriteriaProgressData(CRTITERIA_DEFENSES_CONSTRUCTED, constructionsCompleted, GetScenarioGUID(), time(NULL), 0, 0));
                        break;
                    case DATA_MASSIVE_ATTACK:
                        // @ todo: Create massive attack to traps
                        if (Creature* scargash = instance->GetCreature(GetGuidData(NPC_COMMANDER_SCARGASH)))
                            scargash->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                        for (auto&& itr : korkronAssaultGUIDs)
                            if (Creature* korkronSoldier = instance->GetCreature(itr))
                                korkronSoldier->DespawnOrUnsummon();
                        break;
                }

                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_PREPARE_CONSTRUCTS:
                        return chapterOne;
                    case DATA_SCARGASH_DEFEAT:
                        return chapterTwo;
                    case FACTION_DATA:
                        return factionData == HORDE ? 1 : 0;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case NPC_KING_VARIAN_WRYNN:
                        return varianIntroGUID;
                    case NPC_KING_VARIAN_WRYNN + 1:
                        return varianGUID;
                    case NPC_TYRANDE_WHISPERWIND:
                        return tyrandeIntroGUID;
                    case NPC_TYRANDE_WHISPERWIND + 1:
                        return tyrandeGUID;
                    case NPC_COMMANDER_SCARGASH:
                        return scargashGUID;
                    case NPC_CHI_JI:
                        return chiJiIntroGUID;
                    case NPC_CHI_JI + 1:
                        return chiJiGUID;
                    case NPC_ELDER_ADLER:
                        return adlerGUID;
                    case NPC_ROSEY_AXLEROD:
                        return roseyGUID;
                    case NPC_FERA_PEARL:
                        return feraGUID;
                    case NPC_DUFF_MCSTRUM:
                        return duffGUID;
                    case NPC_MASTER_BROWNSTONE:
                        return brownstoneGUID;
                }

                return ObjectGuid::Empty;
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                return true;
            }

            void Update(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == 1)
                    {
                        if (Creature* varian = instance->GetCreature(GetGuidData(NPC_KING_VARIAN_WRYNN)))
                            varian->AI()->DoAction(ACTION_START_INTRO);
                    }
                    break;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "L P " << chapterOne << ' ' << chapterTwo;
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

                if (dataHead1 == 'L' && dataHead2 == 'P')
                {
                    uint32 temp = 0;
                    loadStream >> temp; // chapterOne complete
                    chapterOne = temp;
                    SetData(DATA_PREPARE_CONSTRUCTS, chapterOne);
                    loadStream >> temp; // chapterTwo complete
                    chapterTwo = temp;
                    SetData(DATA_SCARGASH_DEFEAT, chapterTwo);
                }
                else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_little_patience_InstanceMapScript(map);
        }
};

void AddSC_instance_little_patience()
{
    new instance_little_patience();
}