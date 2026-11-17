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
#include "drak_tharon_keep.h"

#define MAX_ENCOUNTER     4

 /* Drak'Tharon Keep encounters:
  0 - Trollgore
  1 - Novos
  2 - King Dred
  3 - Tharon Ja
 */

enum Creatures
{
    NPC_TROLLGORE = 26630,
    NPC_NOVOS = 26631,
    NPC_KING_DRED = 27483,
    NPC_THARON_JA = 26632,
};

enum GameObjects
{
    GO_NOVOS_CRYSTAL_1 = 189299,
    GO_NOVOS_CRYSTAL_2 = 189300,
    GO_NOVOS_CRYSTAL_3 = 189301,
    GO_NOVOS_CRYSTAL_4 = 189302
};

enum Achievements
{
    ACM_CRITERIA_OH_NOVOS = 7361
};

class instance_drak_tharon_keep : public InstanceMapScript
{
public:
    instance_drak_tharon_keep() : InstanceMapScript("instance_drak_tharon_keep", 600) { }

    struct instance_drak_tharon_keep_InstanceScript : public InstanceScript
    {
        instance_drak_tharon_keep_InstanceScript(Map* map) : InstanceScript(map) { }

        uint8 uiDredAchievCounter;

        ObjectGuid uiTrollgore;
        ObjectGuid uiNovos;
        ObjectGuid uiDred;
        ObjectGuid uiTharonJa;

        ObjectGuid uiNovosCrystal1;
        ObjectGuid uiNovosCrystal2;
        ObjectGuid uiNovosCrystal3;
        ObjectGuid uiNovosCrystal4;

        ObjectGuid novosSummonerGUID1;
        ObjectGuid novosSummonerGUID2;
        ObjectGuid novosSummonerGUID3;
        ObjectGuid novosSummonerGUID4;

        bool introReanimatorDone = false;
        bool introWarriorsDone = false;
        ObjectGuid introReanimatorGUID = ObjectGuid::Empty;
        ObjectGuid introGuardianRightGUID = ObjectGuid::Empty;
        ObjectGuid introGuardianLeftGUID = ObjectGuid::Empty;
        std::vector<ObjectGuid> introWarriorGUIDs;

        ObjectGuid kurzelCocoonGUID = ObjectGuid::Empty;

        uint16 m_auiEncounter[MAX_ENCOUNTER];

        std::string str_data;

        void Initialize() override
        {
            memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
            uiTrollgore = ObjectGuid::Empty;
            uiNovos = ObjectGuid::Empty;
            uiDred = ObjectGuid::Empty;
            uiTharonJa = ObjectGuid::Empty;
            uiNovosCrystal1 = ObjectGuid::Empty;
            uiNovosCrystal2 = ObjectGuid::Empty;
            uiNovosCrystal3 = ObjectGuid::Empty;
            uiNovosCrystal4 = ObjectGuid::Empty;
            uiDredAchievCounter = ObjectGuid::Empty;
            novosSummonerGUID1 = ObjectGuid::Empty;
            novosSummonerGUID2 = ObjectGuid::Empty;
            novosSummonerGUID3 = ObjectGuid::Empty;
            novosSummonerGUID4 = ObjectGuid::Empty;
        }

        bool IsEncounterInProgress() const override
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS) return true;

            return false;
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case GO_NOVOS_CRYSTAL_1:
                uiNovosCrystal1 = go->GetGUID();
                break;
            case GO_NOVOS_CRYSTAL_2:
                uiNovosCrystal2 = go->GetGUID();
                break;
            case GO_NOVOS_CRYSTAL_3:
                uiNovosCrystal3 = go->GetGUID();
                break;
            case GO_NOVOS_CRYSTAL_4:
                uiNovosCrystal4 = go->GetGUID();
                break;
            }
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
            case NPC_TROLLGORE:
                uiTrollgore = creature->GetGUID();
                break;
            case NPC_NOVOS:
                uiNovos = creature->GetGUID();
                break;
            case NPC_KING_DRED:
                uiDred = creature->GetGUID();
                break;
            case NPC_THARON_JA:
                uiTharonJa = creature->GetGUID();
                break;
            case NPC_CRYSTAL_CHANNEL_TARGET:
                InitializeNovosSummoner(creature);
                break;
            case NPC_DRAKKARI_GUARDIAN:
            case NPC_SCOURGE_REANIMATOR:
            case NPC_RISEN_DRAKKARI_WARRIOR:
            case NPC_RISEN_DRAKKARI_SOULMAGE:
                switch (creature->GetDBTableGUIDLow())
                {
                case 79261:
                    introReanimatorGUID = creature->GetGUID();
                    introReanimatorDone |= creature->isDead();
                    break;
                case 79287:
                    introGuardianRightGUID = creature->GetGUID();
                    introReanimatorDone |= creature->isDead();
                    break;
                case 79289:
                    introGuardianLeftGUID = creature->GetGUID();
                    introReanimatorDone |= creature->isDead();
                    break;
                case 79240:
                case 79246:
                    creature->SetReactState(REACT_PASSIVE);
                case 509859:
                case 509860:
                case 509861:
                case 509862:
                    introWarriorGUIDs.push_back(creature->GetGUID());
                    introWarriorsDone |= creature->isDead();
                    break;
                }

                if (!creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED))
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                break;
            case NPC_DRAKTHARON_COCOON_BUNNY:
                creature->m_Events.Schedule(1, [this, creature]()
                    {
                        std::list<Creature*> cocoons;
                        GetCreatureListWithEntryInGrid(cocoons, creature, NPC_DARKWEB_VICTIM, 50.0f);
                        ASSERT(cocoons.size() == 6);
                        cocoons.remove_if([](Creature* cocoon) { return cocoon->isDead(); });
                        if (!cocoons.empty())
                            kurzelCocoonGUID = Trinity::Containers::SelectRandomContainerElement(cocoons)->GetGUID();
                    });
                break;
            }
        }

        void InitializeNovosSummoner(Creature* creature)
        {
            float x = creature->GetPositionX();
            float y = creature->GetPositionY();
            float z = creature->GetPositionZ();

            if (x < -374.0f && x > -379.0f && y > -820.0f && y < -815.0f && z < 60.0f && z > 58.0f)
                novosSummonerGUID1 = creature->GetGUID();
            else if (x < -379.0f && x > -385.0f && y > -820.0f && y < -815.0f && z < 60.0f && z > 58.0f)
                novosSummonerGUID2 = creature->GetGUID();
            else if (x < -374.0f && x > -385.0f && y > -827.0f && y < -820.0f && z < 60.0f && z > 58.0f)
                novosSummonerGUID3 = creature->GetGUID();
            else if (x < -376.0f && x > -380.0f && y > -720.0f && y < -714.0f && z < 30.0f && z > 26.0f)
                novosSummonerGUID4 = creature->GetGUID();
        }

        ObjectGuid GetGuidData(uint32 type) const override
        {
            switch (type)
            {
            case DATA_TROLLGORE:          return uiTrollgore;
            case DATA_NOVOS:              return uiNovos;
            case DATA_DRED:               return uiDred;
            case DATA_THARON_JA:          return uiTharonJa;
            case DATA_NOVOS_CRYSTAL_1:    return uiNovosCrystal1;
            case DATA_NOVOS_CRYSTAL_2:    return uiNovosCrystal2;
            case DATA_NOVOS_CRYSTAL_3:    return uiNovosCrystal3;
            case DATA_NOVOS_CRYSTAL_4:    return uiNovosCrystal4;
            case DATA_NOVOS_SUMMONER_1:   return novosSummonerGUID1;
            case DATA_NOVOS_SUMMONER_2:   return novosSummonerGUID2;
            case DATA_NOVOS_SUMMONER_3:   return novosSummonerGUID3;
            case DATA_NOVOS_SUMMONER_4:   return novosSummonerGUID4;
            }

            return ObjectGuid::Empty;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
            case DATA_TROLLGORE_EVENT:
                m_auiEncounter[0] = data;
                break;
            case DATA_NOVOS_EVENT:
                m_auiEncounter[1] = data;
                break;
            case DATA_DRED_EVENT:
                m_auiEncounter[2] = data;
                break;
            case DATA_THARON_JA_EVENT:
                m_auiEncounter[3] = data;
                break;

            case DATA_KING_DRED_ACHIEV:
                uiDredAchievCounter = data;
                break;
            }

            if (data == DONE)
            {
                SaveToDB();
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
            case DATA_TROLLGORE_EVENT:    return m_auiEncounter[0];
            case DATA_NOVOS_EVENT:        return m_auiEncounter[1];
            case DATA_DRED_EVENT:         return m_auiEncounter[2];
            case DATA_THARON_JA_EVENT:    return m_auiEncounter[3];
            case DATA_KING_DRED_ACHIEV:   return uiDredAchievCounter;
            }
            return 0;
        }

        void ProcessEvent(WorldObject* source, uint32 eventId) override
        {
            switch (eventId)
            {
            case EVENT_RAISE_DEAD_RIGHT:
            case EVENT_RAISE_DEAD_LEFT:
                if (Creature* guardian = instance->GetCreature(eventId == EVENT_RAISE_DEAD_RIGHT ? introGuardianRightGUID : introGuardianLeftGUID))
                {
                    guardian->m_Events.Schedule(1000, [guardian]()
                        {
                            if (Creature* warrior = guardian->SummonCreature(NPC_RISEN_DRAKKARI_WARRIOR, *guardian))
                            {
                                warrior->CastSpell(warrior, SPELL_BIRTH_DEAD_VISUAL);
                                warrior->m_Events.Schedule(2000, [warrior]()
                                    {
                                        if (!warrior->IsInCombat())
                                            warrior->GetMotionMaster()->MoveRandom(5.0f);
                                    });
                            }

                            guardian->DespawnOrUnsummon();
                        });
                }
                break;
            default:
                break;
            }
        }

        void Update(uint32 diff) override
        {
            if (!instance->HavePlayers())
                return;

            if (introReanimatorDone && introWarriorsDone)
                return;

            for (auto&& ref : instance->GetPlayers())
            {
                if (Player* player = ref.GetSource())
                {
                    ObjectGuid playerGuid = player->GetGUID();
                    if (!introReanimatorDone && player->GetPositionY() <= -525.0f)
                    {
                        introReanimatorDone = true;
                        if (Creature* reanimator = instance->GetCreature(introReanimatorGUID))
                        {
                            reanimator->GetMotionMaster()->MovePath(reanimator->GetDBTableGUIDLow() * 10, false);
                            uint32 delay = 0;
                            reanimator->m_Events.Schedule(delay += 200, [=]() { reanimator->AI()->Talk(2, ObjectAccessor::GetPlayer(*reanimator, playerGuid)); });
                            reanimator->m_Events.Schedule(delay += 5000, [=]() { reanimator->CastSpell(reanimator, SPELL_RAISE_DEAD_RIGHT); });
                            reanimator->m_Events.Schedule(delay += 200, [=]() { reanimator->AI()->Talk(0, ObjectAccessor::GetPlayer(*reanimator, playerGuid)); });
                            reanimator->m_Events.Schedule(delay += 3500, [=]() { reanimator->CastSpell(reanimator, SPELL_RAISE_DEAD_LEFT); });
                            reanimator->m_Events.Schedule(delay += 3500, [=]()
                                {
                                    reanimator->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                                    reanimator->CastSpell(reanimator, SPELL_TELEPORT_REANIMATOR);
                                });
                            reanimator->m_Events.Schedule(delay += 200, [=]() { reanimator->AI()->Talk(1, ObjectAccessor::GetPlayer(*reanimator, playerGuid)); });
                            reanimator->m_Events.Schedule(delay += 1400, [=]() { reanimator->DespawnOrUnsummon(); });
                        }
                        if (Creature* guardian = instance->GetCreature(introGuardianLeftGUID))
                        {
                            guardian->RemoveAurasDueToSpell(SPELL_SIPHON_LIFE_VISUAL);
                            guardian->GetMotionMaster()->MovePath(guardian->GetDBTableGUIDLow() * 10, false);
                            // Suicide handled via waypoint_scripts
                        }
                        if (Creature* guardian = instance->GetCreature(introGuardianRightGUID))
                        {
                            guardian->RemoveAurasDueToSpell(SPELL_SIPHON_LIFE_VISUAL);
                            guardian->GetMotionMaster()->MovePath(guardian->GetDBTableGUIDLow() * 10, false);
                            // Suicide handled via waypoint_scripts
                        }
                    }
                    if (!introWarriorsDone && player->GetPositionX() >= -408.0f)
                    {
                        introWarriorsDone = true;
                        for (auto&& guid : introWarriorGUIDs)
                        {
                            if (Creature* warrior = instance->GetCreature(guid))
                            {
                                warrior->GetMotionMaster()->MovePath(warrior->GetDBTableGUIDLow() * 10, false);
                                if (warrior->GetDBTableGUIDLow() == 79246)
                                    warrior->AI()->Talk(0, ObjectAccessor::GetPlayer(*warrior, playerGuid));
                                if (warrior->GetDBTableGUIDLow() == 79240)
                                    warrior->AI()->Talk(1, ObjectAccessor::GetPlayer(*warrior, playerGuid));
                                // Despawn handled via waypoint_scripts
                            }
                        }
                    }
                }
            }
        }

        std::string GetSaveData() override
        {
            OUT_SAVE_INST_DATA;

            std::string str_data;

            std::ostringstream saveStream;
            saveStream << "D K " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
                << m_auiEncounter[2] << " " << m_auiEncounter[3];

            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void OnUnitDeath(Unit* unit) override
        {
            if (unit->GetEntry() == NPC_CRYSTAL_HANDLER)
            {
                if (uiNovos)
                    if (Creature* novos = instance->GetCreature(uiNovos))
                        novos->AI()->DoAction(ACTION_CRYSTAL_HANDLER_DIED);
            }
            else if (unit->GetEntry() == NPC_DARKWEB_VICTIM)
                unit->CastSpell(unit, unit->GetGUID() == kurzelCocoonGUID ? SPELL_SUMMON_KURZEL : SPELL_SUMMON_RANDOM_DRAKKARI, true);
        }

        void Load(const char* in) override
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'D' && dataHead2 == 'K')
            {
                m_auiEncounter[0] = data0;
                m_auiEncounter[1] = data1;
                m_auiEncounter[2] = data2;
                m_auiEncounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS)
                        m_auiEncounter[i] = NOT_STARTED;
            }
            else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_drak_tharon_keep_InstanceScript(map);
    }
};

void AddSC_instance_drak_tharon_keep()
{
    new instance_drak_tharon_keep;
}
