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

#include "ScriptPCH.h"
#include "violet_hold.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellScript.h"

#define MAX_ENCOUNTER          3

/* Violet Hold encounters:
 0 - First boss
 1 - Second boss
 2 - Cyanigosa
*/

/* Violet hold bosses:
 1 - Moragg
 2 - Erekem
 3 - Ichoron
 4 - Lavanthor
 5 - Xevozz
 6 - Zuramat
 7 - Cyanigosa
*/

enum GameObjects
{
    GO_MAIN_DOOR                                    = 191723,
    GO_XEVOZZ_DOOR                                  = 191556,
    GO_LAVANTHOR_DOOR                               = 191566,
    GO_ICHORON_DOOR                                 = 191722,
    GO_ZURAMAT_DOOR                                 = 191565,
    GO_EREKEM_DOOR                                  = 191564,
    GO_EREKEM_GUARD_1_DOOR                          = 191563,
    GO_EREKEM_GUARD_2_DOOR                          = 191562,
    GO_MORAGG_DOOR                                  = 191606,
    GO_INTRO_ACTIVATION_CRYSTAL                     = 193615,
    GO_ACTIVATION_CRYSTAL                           = 193611
};

enum AzureSaboteurSpells
{
    SABOTEUR_SHIELD_DISRUPTION                      = 58291,
    SABOTEUR_SHIELD_EFFECT                          = 45775
};

enum CrystalSpells
{
    SPELL_DEFENSE_SYSTEM_SPAWN_EFFECT               = 57886,
    SPELL_DEFENSE_SYSTEM_VISUAL                     = 57887,
    SPELL_ARCANE_LIGHTNING                          = 57912,
    SPELL_ARCANE_LIGHTNING_VISUAL                   = 57930,
    SPELL_ARCANE_LIGHTNING_INSTAKILL                = 58152,
};

const Position PortalLocation[] =
{
    { 1877.51f, 850.104f, 44.6599f, 4.7822f  }, // WP 1
    { 1918.37f, 853.437f, 47.1624f, 4.12294f }, // WP 2
    { 1936.07f, 803.198f, 53.3749f, 3.12414f }, // WP 3
    { 1927.61f, 758.436f, 51.4533f, 2.20891f }, // WP 4
    { 1890.64f, 753.471f, 48.7224f, 1.71042f }, // WP 5
    { 1908.31f, 809.657f, 38.7037f, 3.08701f }  // WP 6
};

const Position ArcaneSphere          = { 1888.146f,  803.382f, 58.60389f, 3.071779f  };
const Position MoraggMovePoint       = { 1894.684448f, 739.390503f, 47.668003f, 0.0f };
const Position ErekemMovePoint       = { 1875.173950f, 860.832703f, 43.333565f, 0.0f };
const Position ErekemGuardMovePoint1 = { 1858.854614f, 855.071411f, 43.333565f, 0.0f };
const Position ErekemGuardMovePoint2 = { 1891.926636f, 863.388977f, 43.333565f, 0.0f };
const Position IchoronMovePoint      = { 1916.138062f, 778.152222f, 35.772308f, 0.0f };
const Position LavanthorMovePoint    = { 1853.618286f, 758.557617f, 38.657505f, 0.0f };
const Position XevozzMovePoint       = { 1906.683960f, 842.348022f, 38.637459f, 0.0f };
const Position ZuramatMovePoint      = { 1928.207031f, 852.864441f, 47.200813f, 0.0f };

const Position CyanigosasSpawnLocation      = { 1930.281250f, 804.407715f, 52.410946f, 3.139621f };
const Position MiddleRoomLocation           = { 1892.291260f, 805.696838f, 38.438862f, 3.139621f };
const Position MiddleRoomPortalSaboLocation = { 1896.622925f, 804.854126f, 38.504772f, 3.139621f };

enum Yells
{
    SAY_CYANIGOSA_SPAWN                           = 3,
    SAY_XEVOZZ_SPAWN                              = 3,
    SAY_EREKEM_SPAWN                              = 3,
    SAY_ICHORON_SPAWN                             = 3,
    SAY_ZURAMAT_SPAWN                             = 3,

    SOUND_MORAGG_SPAWN                            = 10112,
};

enum Spells
{
    SPELL_CYANIGOSA_TRANSFORM                     = 58668,
    SPELL_CYANIGOSA_BLUE_AURA                     = 47759,
    SPELL_CYANIGOSA_ARCANE_POWER_STATE            = 49411,
    SPELL_MORAGG_EMOTE_ROAR                       = 48350,
    SPELL_LAVANTHOR_SPECIAL_UNARMED               = 33334,
    SPELL_ZURAMAT_COSMETIC_CHANNEL_OMNI           = 57552,
};

enum Misc
{
    EVENT_GROUP_BOSS_INTRO = 1,
};

class instance_violet_hold : public InstanceMapScript
{
    public:
        instance_violet_hold() : InstanceMapScript("instance_violet_hold", 608) { }

        struct instance_violet_hold_InstanceMapScript : public InstanceScript
        {
            instance_violet_hold_InstanceMapScript(Map* map) : InstanceScript(map) { }

            ObjectGuid uiMoragg;
            ObjectGuid uiErekem;
            ObjectGuid uiErekemGuard[2];
            ObjectGuid uiIchoron;
            ObjectGuid uiLavanthor;
            ObjectGuid uiXevozz;
            ObjectGuid uiZuramat;
            ObjectGuid uiCyanigosa;
            ObjectGuid uiSinclari;
            ObjectGuid uiSinclariTeleport;
            ObjectGuid uiSinclariTrigger;

            ObjectGuid uiMoraggCell;
            ObjectGuid uiErekemCell;
            ObjectGuid uiErekemLeftGuardCell;
            ObjectGuid uiErekemRightGuardCell;
            ObjectGuid uiIchoronCell;
            ObjectGuid uiLavanthorCell;
            ObjectGuid uiXevozzCell;
            ObjectGuid uiZuramatCell;
            ObjectGuid uiMainDoor;
            ObjectGuid uiTeleportationPortal;
            ObjectGuid uiSaboteurPortal;

            std::vector<ObjectGuid> uiActivationCrystals;

            uint32 uiActivationTimer;
            uint32 uiCyanigosaEventTimer;
            uint32 uiDoorSpellTimer;

            uint8 uiWaveCount;
            uint8 uiLocation;
            uint8 uiFirstBoss;
            uint8 uiSecondBoss;
            uint8 uiRemoveNpc;

            uint8 uiDoorIntegrity;

            uint16 m_auiEncounter[MAX_ENCOUNTER];
            uint8 uiCountErekemGuards;
            uint8 uiCyanigosaEventPhase;
            uint8 uiMainEventPhase; // SPECIAL: pre event animations, IN_PROGRESS: event itself

            bool bActive;
            bool bWiped;
            bool bIsDoorSpellCasted;
            bool defenseless;

            std::list<uint8> NpcAtDoorCastingList;

            std::string str_data;

            void Initialize() override
            {
                uiMoragg = ObjectGuid::Empty;
                uiErekem = ObjectGuid::Empty;
                uiIchoron = ObjectGuid::Empty;
                uiLavanthor = ObjectGuid::Empty;
                uiXevozz = ObjectGuid::Empty;
                uiZuramat = ObjectGuid::Empty;
                uiCyanigosa = ObjectGuid::Empty;
                uiSinclari = ObjectGuid::Empty;
                uiSinclariTeleport = ObjectGuid::Empty;
                uiSinclariTrigger = ObjectGuid::Empty;

                uiMoraggCell = ObjectGuid::Empty;
                uiErekemCell = ObjectGuid::Empty;
                uiErekemGuard[0] = ObjectGuid::Empty;
                uiErekemGuard[1] = ObjectGuid::Empty;
                uiIchoronCell = ObjectGuid::Empty;
                uiLavanthorCell = ObjectGuid::Empty;
                uiXevozzCell = ObjectGuid::Empty;
                uiZuramatCell = ObjectGuid::Empty;
                uiMainDoor = ObjectGuid::Empty;
                uiTeleportationPortal = ObjectGuid::Empty;
                uiSaboteurPortal = ObjectGuid::Empty;

                uiRemoveNpc = 0;

                uiDoorIntegrity = 100;

                uiWaveCount = 0;
                uiLocation = urand(0, 5);
                uiFirstBoss = 0;
                uiSecondBoss = 0;
                uiCountErekemGuards = 0;
                uiCyanigosaEventPhase = 1;
                uiMainEventPhase = NOT_STARTED;

                uiActivationTimer = 5000;
                uiDoorSpellTimer = 2000;
                uiCyanigosaEventTimer = 3 * IN_MILLISECONDS;

                bActive = false;
                bWiped = false;
                bIsDoorSpellCasted = false;
                defenseless = true;
                instance->SetWorldState(WORLDSTATE_DEFENSELESS, 1);
                uiMainEventPhase = NOT_STARTED;

                memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
            }

            bool IsEncounterInProgress() const override
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (m_auiEncounter[i] == IN_PROGRESS) return true;

                return false;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_XEVOZZ:
                        uiXevozz = creature->GetGUID();
                        break;
                    case NPC_LAVANTHOR:
                        uiLavanthor = creature->GetGUID();
                        break;
                    case NPC_ICHORON:
                        uiIchoron = creature->GetGUID();
                        break;
                    case NPC_ZURAMAT:
                        uiZuramat = creature->GetGUID();
                        break;
                    case NPC_EREKEM:
                        uiErekem = creature->GetGUID();
                        break;
                    case NPC_EREKEM_GUARD:
                        if (uiCountErekemGuards < 2)
                        {
                            uiErekemGuard[uiCountErekemGuards++] = creature->GetGUID();
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        }
                        break;
                    case NPC_MORAGG:
                        uiMoragg = creature->GetGUID();
                        break;
                    case NPC_CYANIGOSA:
                        uiCyanigosa = creature->GetGUID();
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        break;
                    case NPC_SINCLARI:
                        (creature->IsSummon() ? uiSinclariTeleport : uiSinclari) = creature->GetGUID();
                        if (!creature->IsSummon() && creature->isDead())
                            creature->Respawn();
                        break;
                    case NPC_SINCLARI_TRIGGER:
                        uiSinclariTrigger = creature->GetGUID();
                        break;
                }

                if (creature->GetGUID() == uiFirstBoss || creature->GetGUID() == uiSecondBoss)
                {
                    creature->AllLootRemovedFromCorpse();
                    creature->RemoveLootMode(1);
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_EREKEM_GUARD_1_DOOR:
                        uiErekemLeftGuardCell = go->GetGUID();
                        break;
                    case GO_EREKEM_GUARD_2_DOOR:
                        uiErekemRightGuardCell = go->GetGUID();
                        break;
                    case GO_EREKEM_DOOR:
                        uiErekemCell = go->GetGUID();
                        break;
                    case GO_ZURAMAT_DOOR:
                        uiZuramatCell = go->GetGUID();
                        break;
                    case GO_LAVANTHOR_DOOR:
                        uiLavanthorCell = go->GetGUID();
                        break;
                    case GO_MORAGG_DOOR:
                        uiMoraggCell = go->GetGUID();
                        break;
                    case GO_ICHORON_DOOR:
                        uiIchoronCell = go->GetGUID();
                        break;
                    case GO_XEVOZZ_DOOR:
                        uiXevozzCell = go->GetGUID();
                        break;
                    case GO_MAIN_DOOR:
                        uiMainDoor = go->GetGUID();
                        break;
                    case GO_ACTIVATION_CRYSTAL:
                        uiActivationCrystals.push_back(go->GetGUID());
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_1ST_BOSS_EVENT:
                        if (m_auiEncounter[0] != DONE)
                        {
                            m_auiEncounter[0] = data;
                            if (data == DONE)
                            {
                                if (uiFirstBoss != BOSS_EREKEM)
                                    UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_EREKEM, NULL); // Credit "First Prisoner" encounter
                                SaveToDB();
                            }
                        }
                        break;
                    case DATA_2ND_BOSS_EVENT:
                        if (m_auiEncounter[1] != DONE)
                        {
                            m_auiEncounter[1] = data;
                            if (data == DONE)
                            {
                                if (uiSecondBoss != BOSS_MORAGG)
                                    UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_MORAGG, NULL); // Credit "Second Prisoner" encounter
                                SaveToDB();
                            }
                        }
                        break;
                    case DATA_CYANIGOSA_EVENT:
                        m_auiEncounter[2] = data;
                        if (data == DONE)
                        {
                            SaveToDB();
                            uiMainEventPhase = DONE;
                            if (GameObject* pMainDoor = instance->GetGameObject(uiMainDoor))
                                pMainDoor->SetGoState(GO_STATE_ACTIVE);
                        }
                        break;
                    case DATA_WAVE_COUNT:
                        uiWaveCount = data;
                        bActive = true;
                        break;
                    case DATA_REMOVE_NPC:
                        uiRemoveNpc = data;
                        break;
                    case DATA_PORTAL_LOCATION:
                        uiLocation = (uint8)data;
                        break;
                    case DATA_DOOR_INTEGRITY:
                        uiDoorIntegrity = data;
                        defenseless = false;
                        instance->SetWorldState(WORLDSTATE_DEFENSELESS, 0);
                        DoUpdateWorldState(WORLD_STATE_VH_PRISON_STATE, uiDoorIntegrity);
                        break;
                    case DATA_NPC_PRESENCE_AT_DOOR_ADD:
                        NpcAtDoorCastingList.push_back(data);
                        break;
                    case DATA_NPC_PRESENCE_AT_DOOR_REMOVE:
                        if (!NpcAtDoorCastingList.empty())
                            NpcAtDoorCastingList.pop_back();
                        break;
                    case DATA_MAIN_DOOR:
                        if (GameObject* pMainDoor = instance->GetGameObject(uiMainDoor))
                        {
                            switch (data)
                            {
                                case GO_STATE_ACTIVE:
                                    pMainDoor->SetGoState(GO_STATE_ACTIVE);
                                    break;
                                case GO_STATE_READY:
                                    pMainDoor->SetGoState(GO_STATE_READY);
                                    break;
                                case GO_STATE_ACTIVE_ALTERNATIVE:
                                    pMainDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                    break;
                            }
                        }
                        break;
                    case DATA_START_BOSS_ENCOUNTER:
                        switch (uiWaveCount)
                        {
                            case 6:
                                StartBossEncounter(uiFirstBoss);
                                break;
                            case 12:
                                StartBossEncounter(uiSecondBoss);
                                break;
                        }
                        break;
                    case DATA_FIRST_BOSS:
                        uiFirstBoss = data;
                        break;
                    case DATA_SECOND_BOSS:
                        uiSecondBoss = data;
                        break;
                    case DATA_ACTIVATE_CRYSTAL:
                        if (data)
                        {
                            defenseless = false;
                            instance->SetWorldState(WORLDSTATE_DEFENSELESS, 0);
                        }
                        ActivateCrystal();
                        break;
                    case DATA_MAIN_EVENT_PHASE:
                        uiMainEventPhase = data;
                        if (data == IN_PROGRESS) // Start event
                        {
                            if (GameObject* pMainDoor = instance->GetGameObject(uiMainDoor))
                                pMainDoor->SetGoState(GO_STATE_READY);
                            uiWaveCount = 1;
                            uiActivationTimer = 5000;
                            bActive = true;
                            for (auto&& guid : uiActivationCrystals)
                                if (GameObject* pCrystal = instance->GetGameObject(guid))
                                    pCrystal->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            uiRemoveNpc = 0; // might not have been reset after a wipe on a boss.
                        }
                        break;
                    case DATA_WIPE:
                        bWiped = data;
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_1ST_BOSS_EVENT:           return m_auiEncounter[0];
                    case DATA_2ND_BOSS_EVENT:           return m_auiEncounter[1];
                    case DATA_CYANIGOSA_EVENT:          return m_auiEncounter[2];
                    case DATA_WAVE_COUNT:               return uiWaveCount;
                    case DATA_REMOVE_NPC:               return uiRemoveNpc;
                    case DATA_PORTAL_LOCATION:          return uiLocation;
                    case DATA_DOOR_INTEGRITY:           return uiDoorIntegrity;
                    case DATA_NPC_PRESENCE_AT_DOOR:     return NpcAtDoorCastingList.size();
                    case DATA_FIRST_BOSS:               return uiFirstBoss;
                    case DATA_SECOND_BOSS:              return uiSecondBoss;
                    case DATA_MAIN_EVENT_PHASE:         return uiMainEventPhase;
                    case DATA_DEFENSELESS:              return defenseless ? 1 : 0;
                }

                return 0;
            }

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_MORAGG:                   return uiMoragg;
                    case DATA_EREKEM:                   return uiErekem;
                    case DATA_EREKEM_GUARD_1:           return uiErekemGuard[0];
                    case DATA_EREKEM_GUARD_2:           return uiErekemGuard[1];
                    case DATA_ICHORON:                  return uiIchoron;
                    case DATA_LAVANTHOR:                return uiLavanthor;
                    case DATA_XEVOZZ:                   return uiXevozz;
                    case DATA_ZURAMAT:                  return uiZuramat;
                    case DATA_CYANIGOSA:                return uiCyanigosa;
                    case DATA_MORAGG_CELL:              return uiMoraggCell;
                    case DATA_EREKEM_CELL:              return uiErekemCell;
                    case DATA_EREKEM_RIGHT_GUARD_CELL:  return uiErekemRightGuardCell;
                    case DATA_EREKEM_LEFT_GUARD_CELL:   return uiErekemLeftGuardCell;
                    case DATA_ICHORON_CELL:             return uiIchoronCell;
                    case DATA_LAVANTHOR_CELL:           return uiLavanthorCell;
                    case DATA_XEVOZZ_CELL:              return uiXevozzCell;
                    case DATA_ZURAMAT_CELL:             return uiZuramatCell;
                    case DATA_MAIN_DOOR:                return uiMainDoor;
                    case DATA_SINCLARI:                 return uiSinclari;
                    case DATA_SINCLARI_TRIGGER:         return uiSinclariTrigger;
                    case DATA_TELEPORTATION_PORTAL:     return uiTeleportationPortal;
                    case DATA_SABOTEUR_PORTAL:          return uiSaboteurPortal;
                }

                return ObjectGuid::Empty;
            }

            void SpawnPortal()
            {
                SetData(DATA_PORTAL_LOCATION, (GetData(DATA_PORTAL_LOCATION) + urand(1, 5)) % 6);
                if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                    if (Creature* portal = pSinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, PortalLocation[GetData(DATA_PORTAL_LOCATION)], TEMPSUMMON_CORPSE_DESPAWN))
                        uiTeleportationPortal = portal->GetGUID();
            }

            void StartBossEncounter(uint8 uiBoss, bool bForceRespawn = true)
            {
                Creature* pBoss = NULL;

                switch (uiBoss)
                {
                    case BOSS_MORAGG:
                        HandleGameObject(uiMoraggCell, bForceRespawn);
                        pBoss = instance->GetCreature(uiMoragg);
                        if (pBoss && bForceRespawn)
                        {
                            uint32 delay = 0;
                            pBoss->m_Events.Schedule(delay += 2000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->PlayDirectSound(SOUND_MORAGG_SPAWN);
                                pBoss->CastSpell(pBoss, SPELL_MORAGG_EMOTE_ROAR);
                            });
                            pBoss->m_Events.Schedule(delay += 3000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->SetWalk(false);
                                pBoss->GetMotionMaster()->MovePoint(0, MoraggMovePoint);
                            });
                            pBoss->m_Events.Schedule(delay += 8000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pBoss->AI()->DoZoneInCombat(pBoss, 200.0f);
                                pBoss->SetReactState(REACT_AGGRESSIVE);
                            });
                        }
                        break;
                    case BOSS_EREKEM:
                        HandleGameObject(uiErekemCell, bForceRespawn);
                        HandleGameObject(uiErekemRightGuardCell, bForceRespawn);
                        HandleGameObject(uiErekemLeftGuardCell, bForceRespawn);
                        pBoss = instance->GetCreature(uiErekem);
                        if (pBoss && bForceRespawn)
                        {
                            uint32 delay = 0;
                            pBoss->m_Events.Schedule(delay += 3000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->AI()->Talk(SAY_EREKEM_SPAWN);
                            });
                            pBoss->m_Events.Schedule(delay += 5000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->SetWalk(false);
                                pBoss->GetMotionMaster()->MovePoint(0, ErekemMovePoint);

                                for (uint8 i = 0; i < 2; ++i)
                                {
                                    if (Creature* guard = instance->GetCreature(uiErekemGuard[i]))
                                    {
                                        if (guard->isDead())
                                            guard->Respawn();
                                        guard->SetWalk(false);
                                        guard->GetMotionMaster()->MovePoint(0, i ? ErekemGuardMovePoint2 : ErekemGuardMovePoint1);
                                    }
                                }
                            });
                            pBoss->m_Events.Schedule(delay += 6000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                            });
                            pBoss->m_Events.Schedule(delay += 1000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                for (uint8 i = 0; i < 2; ++i)
                                    if (Creature* guard = instance->GetCreature(uiErekemGuard[i]))
                                        guard->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

                                pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pBoss->AI()->DoZoneInCombat(pBoss, 200.0f);
                                pBoss->SetReactState(REACT_AGGRESSIVE);
                            });
                        }
                        break;
                    case BOSS_ICHORON:
                        HandleGameObject(uiIchoronCell, bForceRespawn);
                        pBoss = instance->GetCreature(uiIchoron);
                        if (pBoss && bForceRespawn)
                        {
                            uint32 delay = 0;
                            pBoss->m_Events.Schedule(delay += 2000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->AI()->Talk(SAY_ICHORON_SPAWN);
                            });
                            pBoss->m_Events.Schedule(delay += 3000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->SetWalk(false);
                                pBoss->GetMotionMaster()->MovePoint(0, IchoronMovePoint);
                            });
                            pBoss->m_Events.Schedule(delay += 14000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pBoss->AI()->DoZoneInCombat(pBoss, 200.0f);
                                pBoss->SetReactState(REACT_AGGRESSIVE);
                            });
                        }
                        break;
                    case BOSS_LAVANTHOR:
                        HandleGameObject(uiLavanthorCell, bForceRespawn);
                        pBoss = instance->GetCreature(uiLavanthor);
                        if (pBoss && bForceRespawn)
                        {
                            uint32 delay = 0;
                            pBoss->m_Events.Schedule(delay += 1000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->CastSpell(pBoss, SPELL_LAVANTHOR_SPECIAL_UNARMED);
                            });
                            pBoss->m_Events.Schedule(delay += 3000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->SetWalk(false);
                                pBoss->GetMotionMaster()->MovePoint(0, LavanthorMovePoint);
                            });
                            pBoss->m_Events.Schedule(delay += 8000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pBoss->AI()->DoZoneInCombat(pBoss, 200.0f);
                                pBoss->SetReactState(REACT_AGGRESSIVE);
                            });
                        }
                        break;
                    case BOSS_XEVOZZ:
                        HandleGameObject(uiXevozzCell, bForceRespawn);
                        pBoss = instance->GetCreature(uiXevozz);
                        if (pBoss && bForceRespawn)
                        {
                            uint32 delay = 0;
                            pBoss->m_Events.Schedule(delay += 2000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->AI()->Talk(SAY_XEVOZZ_SPAWN);
                            });
                            pBoss->m_Events.Schedule(delay += 3000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->HandleEmoteCommand(EMOTE_ONESHOT_TALK_NO_SHEATHE);
                            });
                            pBoss->m_Events.Schedule(delay += 4000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->SetWalk(false);
                                pBoss->GetMotionMaster()->MovePoint(0, XevozzMovePoint);
                            });
                            pBoss->m_Events.Schedule(delay += 4000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pBoss->AI()->DoZoneInCombat(pBoss, 200.0f);
                                pBoss->SetReactState(REACT_AGGRESSIVE);
                            });
                        }
                        break;
                    case BOSS_ZURAMAT:
                        HandleGameObject(uiZuramatCell, bForceRespawn);
                        pBoss = instance->GetCreature(uiZuramat);
                        if (pBoss && bForceRespawn)
                        {
                            uint32 delay = 0;
                            pBoss->m_Events.Schedule(delay += 2000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->CastSpell(pBoss, SPELL_ZURAMAT_COSMETIC_CHANNEL_OMNI);
                                pBoss->AI()->Talk(SAY_ZURAMAT_SPAWN);
                            });
                            pBoss->m_Events.Schedule(delay += 6000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->SetWalk(false);
                                pBoss->GetMotionMaster()->MovePoint(0, ZuramatMovePoint);
                            });
                            pBoss->m_Events.Schedule(delay += 4000, EVENT_GROUP_BOSS_INTRO, [this, pBoss]()
                            {
                                pBoss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pBoss->AI()->DoZoneInCombat(pBoss, 200.0f);
                                pBoss->SetReactState(REACT_AGGRESSIVE);
                            });
                        }
                        break;
                }

                // generic boss state changes
                if (pBoss)
                {
                    if (!bForceRespawn)
                    {
                        pBoss->AI()->EnterEvadeMode();
                        pBoss->InterruptSpell(CURRENT_MELEE_SPELL);
                        pBoss->InterruptNonMeleeSpells(true);
                        pBoss->m_Events.KillEventsByGroup(EVENT_GROUP_BOSS_INTRO);
                        pBoss->StopMoving();
                        pBoss->GetMotionMaster()->Clear();
                        pBoss->GetMotionMaster()->MoveTargetedHome();
                        if (pBoss->isDead())
                        {
                            // respawn but avoid to be looted again
                            pBoss->Respawn();
                            pBoss->RemoveLootMode(1);
                        }
                        if (uiBoss == BOSS_EREKEM)
                        {
                            for (uint8 i = 0; i < 2; ++i)
                            {
                                if (Creature* guard = instance->GetCreature(uiErekemGuard[i]))
                                {
                                    guard->StopMoving();
                                    guard->GetMotionMaster()->Clear();
                                    guard->GetMotionMaster()->MoveTargetedHome();
                                    if (guard->isDead())
                                        guard->Respawn();
                                    guard->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                }
                            }
                        }
                        pBoss->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        uiWaveCount = 0;
                        bWiped = false;
                    }
                }
            }

            void AddWave()
            {
                DoUpdateWorldState(WORLD_STATE_VH, 1);
                DoUpdateWorldState(WORLD_STATE_VH_WAVE_COUNT, uiWaveCount);

                switch (uiWaveCount)
                {
                    case 6:
                        if (uiFirstBoss == 0)
                            uiFirstBoss = urand(1, 6);
                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                        {
                            if (Creature* pPortal = pSinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, MiddleRoomPortalSaboLocation, TEMPSUMMON_CORPSE_DESPAWN))
                                uiSaboteurPortal = pPortal->GetGUID();
                            if (Creature* pAzureSaboteur = pSinclari->SummonCreature(NPC_SABOTEOUR, MiddleRoomLocation, TEMPSUMMON_DEAD_DESPAWN))
                                pAzureSaboteur->CastSpell(pAzureSaboteur, SABOTEUR_SHIELD_EFFECT, false);
                        }
                        break;
                    case 12:
                        if (uiSecondBoss == 0)
                            do
                            {
                                uiSecondBoss = urand(1, 6);
                            } while (uiSecondBoss == uiFirstBoss);
                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                        {
                            if (Creature* pPortal = pSinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, MiddleRoomPortalSaboLocation, TEMPSUMMON_CORPSE_DESPAWN))
                                uiSaboteurPortal = pPortal->GetGUID();
                            if (Creature* pAzureSaboteur = pSinclari->SummonCreature(NPC_SABOTEOUR, MiddleRoomLocation, TEMPSUMMON_DEAD_DESPAWN))
                                pAzureSaboteur->CastSpell(pAzureSaboteur, SABOTEUR_SHIELD_EFFECT, false);
                        }
                        break;
                    case 18:
                    {
                        uiCyanigosaEventPhase = 1;
                        uiCyanigosaEventTimer = 3 * IN_MILLISECONDS;
                        if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                            if (Creature* cyanigosa = pSinclari->SummonCreature(NPC_CYANIGOSA, CyanigosasSpawnLocation, TEMPSUMMON_DEAD_DESPAWN))
                                cyanigosa->CastSpell(cyanigosa, SPELL_CYANIGOSA_ARCANE_POWER_STATE, true);
                        break;
                    }
                    case 1:
                    {
                        if (GameObject* pMainDoor = instance->GetGameObject(uiMainDoor))
                            pMainDoor->SetGoState(GO_STATE_READY);
                        DoUpdateWorldState(WORLD_STATE_VH_PRISON_STATE, 100);
                    }
                    default:
                        SpawnPortal();
                        break;
                }
            }

            std::string GetSaveData() override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "V H " << (uint16)m_auiEncounter[0]
                    << " " << (uint16)m_auiEncounter[1]
                    << " " << (uint16)m_auiEncounter[2]
                    << " " << (uint16)uiFirstBoss
                    << " " << (uint16)uiSecondBoss;

                str_data = saveStream.str();

                OUT_SAVE_INST_DATA_COMPLETE;
                return str_data;
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
                uint16 data0, data1, data2, data3, data4;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4;

                if (dataHead1 == 'V' && dataHead2 == 'H')
                {
                    m_auiEncounter[0] = data0;
                    m_auiEncounter[1] = data1;
                    m_auiEncounter[2] = data2;

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        if (m_auiEncounter[i] == IN_PROGRESS)
                            m_auiEncounter[i] = NOT_STARTED;

                    uiFirstBoss = uint8(data3);
                    uiSecondBoss = uint8(data4);
                } else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            bool CheckWipe()
            {
                if (bWiped)
                {
                    bWiped = false;
                    return true;
                }
                bWiped = false;

                for (auto&& ref : instance->GetPlayers())
                {
                    Player* player = ref.GetSource();
                    if (player->IsGameMaster())
                        continue;

                    if (player->IsAlive())
                        return false;
                }

                return true;
            }

            void Update(uint32 diff) override
            {
                if (!instance->HavePlayers())
                    return;

                // portals should spawn if other portal is dead and doors are closed
                if (bActive && uiMainEventPhase == IN_PROGRESS)
                {
                    if (uiActivationTimer < diff)
                    {
                        AddWave();
                        bActive = false;
                        uiActivationTimer = 5000;
                    } else uiActivationTimer -= diff;
                }

                // if main event is in progress and players have wiped then reset instance
                if (uiMainEventPhase == IN_PROGRESS && CheckWipe())
                {
                    SetData(DATA_REMOVE_NPC, 1);
                    StartBossEncounter(uiFirstBoss, false);
                    StartBossEncounter(uiSecondBoss, false);

                    SetData(DATA_MAIN_DOOR,GO_STATE_ACTIVE);
                    SetData(DATA_WAVE_COUNT, 0);
                    uiMainEventPhase = NOT_STARTED;

                    for (auto&& guid : uiActivationCrystals)
                        if (GameObject* pCrystal = instance->GetGameObject(guid))
                            pCrystal->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

                    if (Creature* sinclari = instance->GetCreature(uiSinclariTeleport))
                        sinclari->DespawnOrUnsummon();

                    if (Creature* pSinclari = instance->GetCreature(uiSinclari))
                    {
                        pSinclari->Respawn(true);

                        std::list<Creature*> GuardList;
                        pSinclari->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                        if (!GuardList.empty())
                        {
                            for (auto&& pGuard : GuardList)
                            {
                                if (pGuard)
                                {
                                    if (pGuard->isDead())
                                        pGuard->Respawn(true);
                                    else
                                        pGuard->GetMotionMaster()->MovePoint(1,pGuard->GetHomePosition());

                                    pGuard->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                    pGuard->SetVisible(true);
                                    pGuard->SetReactState(REACT_AGGRESSIVE);
                                }
                            }
                        }
                    }
                }

                // Cyanigosa is spawned but not tranformed, prefight event
                Creature* pCyanigosa = instance->GetCreature(uiCyanigosa);
                if (pCyanigosa && !pCyanigosa->HasAura(SPELL_CYANIGOSA_TRANSFORM))
                {
                    if (uiCyanigosaEventTimer <= diff)
                    {
                        switch (uiCyanigosaEventPhase)
                        {
                            case 1:
                                pCyanigosa->CastSpell(pCyanigosa, SPELL_CYANIGOSA_BLUE_AURA, false);
                                pCyanigosa->AI()->Talk(SAY_CYANIGOSA_SPAWN);
                                uiCyanigosaEventTimer = 7 * IN_MILLISECONDS;
                                ++uiCyanigosaEventPhase;
                                break;
                            case 2:
                                pCyanigosa->GetMotionMaster()->MoveJump(MiddleRoomLocation.GetPositionX(), MiddleRoomLocation.GetPositionY(), MiddleRoomLocation.GetPositionZ(), 10.0f, 20.0f);
                                pCyanigosa->CastSpell(pCyanigosa, SPELL_CYANIGOSA_BLUE_AURA, false);
                                uiCyanigosaEventTimer = 7 * IN_MILLISECONDS;
                                ++uiCyanigosaEventPhase;
                                break;
                            case 3:
                                pCyanigosa->RemoveAurasDueToSpell(SPELL_CYANIGOSA_BLUE_AURA);
                                pCyanigosa->RemoveAurasDueToSpell(SPELL_CYANIGOSA_ARCANE_POWER_STATE);
                                pCyanigosa->CastSpell(pCyanigosa, SPELL_CYANIGOSA_TRANSFORM, 0);
                                pCyanigosa->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                                pCyanigosa->SetReactState(REACT_AGGRESSIVE);
                                uiCyanigosaEventTimer = 2 * IN_MILLISECONDS;
                                ++uiCyanigosaEventPhase;
                                break;
                            case 4:
                                uiCyanigosaEventPhase = 0;
                                break;
                        }
                    } else uiCyanigosaEventTimer -= diff;
                }

                // if there are NPCs in front of the prison door, which are casting the door seal spell and doors are active
                if (GetData(DATA_NPC_PRESENCE_AT_DOOR) && uiMainEventPhase == IN_PROGRESS)
                {
                    // if door integrity is > 0 then decrase it's integrity state
                    if (GetData(DATA_DOOR_INTEGRITY))
                    {
                        if (uiDoorSpellTimer < diff)
                        {
                            SetData(DATA_DOOR_INTEGRITY, GetData(DATA_DOOR_INTEGRITY) -1);
                            uiDoorSpellTimer =2000;
                        } else uiDoorSpellTimer -= diff;
                    }
                    // else set door state to active (means door will open and group have failed to sustain mob invasion on the door)
                    else
                    {
                        SetData(DATA_MAIN_DOOR,GO_STATE_ACTIVE);
                        uiMainEventPhase = FAIL;
                    }
                }
            }

            void ActivateCrystal()
            {
                if (TempSummon* trigger = instance->SummonCreature(NPC_DEFENSE_SYSTEM, ArcaneSphere))
                {
                    uint32 delay = 0;
                    trigger->CastSpell(trigger, SPELL_DEFENSE_SYSTEM_SPAWN_EFFECT);
                    trigger->CastSpell(trigger, SPELL_DEFENSE_SYSTEM_VISUAL);
                    trigger->m_Events.Schedule(delay += 4000, [trigger]() { trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING); trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING_VISUAL); });
                    trigger->m_Events.Schedule(delay += 2000, [trigger]() { trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING); trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING_VISUAL); });
                    trigger->m_Events.Schedule(delay += 2000, [trigger]() { trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING); trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING_VISUAL); trigger->CastSpell(trigger, SPELL_ARCANE_LIGHTNING_INSTAKILL); });
                    trigger->DespawnOrUnsummon(10000);
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_violet_hold_InstanceMapScript(map);
        }
};

void AddSC_instance_violet_hold()
{
    new instance_violet_hold();
}
