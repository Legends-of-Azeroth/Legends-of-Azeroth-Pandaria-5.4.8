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

/* ScriptData
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_lunaclaw_spirit      80%    support for quests 6001/6002 (Body and Heart)
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 565
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_rogue_trainer        80%    Scripted trainers, so they are able to offer item 17126 for class quest 6681
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
npc_snake_trap_serpents  80%    AI for snakes that summoned by Snake Trap
npc_shadowfiend         100%   restore 5% of owner's mana when shadowfiend die from damage
npc_locksmith            75%    list of keys needs to be confirmed
npc_firework            100%    NPC's summoned by rockets and rocket clusters, for making them cast visual
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "World.h"
#include "PassiveAI.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "SpellAuras.h"
#include "Pet.h"
#include "BlackMarketMgr.h"
#include "CombatAI.h"
#include "Random.h"

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon Creature at smaller range
    SPAWNTYPE_ALARMBOT,                                     // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 thisCreatureEntry;
    uint32 spawnedCreatureEntry;
    SpawnType spawnType;
};

enum AirFoceBots
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 5000
};

float const RANGE_TRIPWIRE          = 15.0f;
float const RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation spawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

class npc_air_force_bots : public CreatureScript
{
public:
    npc_air_force_bots() : CreatureScript("npc_air_force_bots") { }

    struct npc_air_force_botsAI : public ScriptedAI
    {
        npc_air_force_botsAI(Creature* creature) : ScriptedAI(creature)
        {
            SpawnAssoc = NULL;
            SpawnedGUID = ObjectGuid::Empty;

            // find the correct spawnhandling
            static uint32 entryCount = sizeof(spawnAssociations) / sizeof(SpawnAssociation);

            for (uint8 i = 0; i < entryCount; ++i)
            {
                if (spawnAssociations[i].thisCreatureEntry == creature->GetEntry())
                {
                    SpawnAssoc = &spawnAssociations[i];
                    break;
                }
            }

            if (!SpawnAssoc)
                TC_LOG_ERROR("sql.sql", "TCSR: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", creature->GetEntry());
            else
            {
                CreatureTemplate const* spawnedTemplate = sObjectMgr->GetCreatureTemplate(SpawnAssoc->spawnedCreatureEntry);

                if (!spawnedTemplate)
                {
                    TC_LOG_ERROR("sql.sql", "TCSR: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", SpawnAssoc->spawnedCreatureEntry);
                    SpawnAssoc = NULL;
                    return;
                }
            }
        }

        SpawnAssociation* SpawnAssoc;
        ObjectGuid SpawnedGUID;

        void Reset() override { }

        Creature* SummonGuard()
        {
            Creature* summoned = me->SummonCreature(SpawnAssoc->spawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

            if (summoned)
                SpawnedGUID = summoned->GetGUID();
            else
            {
                TC_LOG_ERROR("sql.sql", "TCSR: npc_air_force_bots: wasn't able to spawn Creature %u", SpawnAssoc->spawnedCreatureEntry);
                SpawnAssoc = NULL;
            }

            return summoned;
        }

        Creature* GetSummonedGuard()
        {
            Creature* creature = Unit::GetCreature(*me, SpawnedGUID);

            if (creature && creature->IsAlive())
                return creature;

            return NULL;
        }

        void MoveInLineOfSight(Unit* who) override

        {
            if (!SpawnAssoc)
                return;

            if (me->IsValidAttackTarget(who))
            {
                Player* playerTarget = who->ToPlayer();

                // airforce guards only spawn for players
                if (!playerTarget)
                    return;

                Creature* lastSpawnedGuard = SpawnedGUID == 0 ? NULL : GetSummonedGuard();

                // prevent calling Unit::GetUnit at next MoveInLineOfSight call - speedup
                if (!lastSpawnedGuard)
                    SpawnedGUID = ObjectGuid::Empty;

                switch (SpawnAssoc->spawnType)
                {
                    case SPAWNTYPE_ALARMBOT:
                    {
                        if (!who->IsWithinDistInMap(me, RANGE_GUARDS_MARK))
                            return;

                        Aura* markAura = who->GetAura(SPELL_GUARDS_MARK);
                        if (markAura)
                        {
                            // the target wasn't able to move out of our range within 25 seconds
                            if (!lastSpawnedGuard)
                            {
                                lastSpawnedGuard = SummonGuard();

                                if (!lastSpawnedGuard)
                                    return;
                            }

                            if (markAura->GetDuration() < AURA_DURATION_TIME_LEFT)
                                if (!lastSpawnedGuard->GetVictim())
                                    lastSpawnedGuard->AI()->AttackStart(who);
                        }
                        else
                        {
                            if (!lastSpawnedGuard)
                                lastSpawnedGuard = SummonGuard();

                            if (!lastSpawnedGuard)
                                return;

                            lastSpawnedGuard->CastSpell(who, SPELL_GUARDS_MARK, true);
                        }
                        break;
                    }
                    case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                    {
                        if (!who->IsWithinDistInMap(me, RANGE_TRIPWIRE))
                            return;

                        if (!lastSpawnedGuard)
                            lastSpawnedGuard = SummonGuard();

                        if (!lastSpawnedGuard)
                            return;

                        // ROOFTOP only triggers if the player is on the ground
                        if (!playerTarget->IsFlying() && !lastSpawnedGuard->GetVictim())
                            lastSpawnedGuard->AI()->AttackStart(who);

                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_air_force_botsAI(creature);
    }
};

/*######
## npc_lunaclaw_spirit
######*/

enum LunaclawSpirit
{
    QUEST_BODY_HEART_A      = 6001,
    QUEST_BODY_HEART_H      = 6002,

    TEXT_ID_DEFAULT         = 4714,
    TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

class npc_lunaclaw_spirit : public CreatureScript
{
public:
    npc_lunaclaw_spirit() : CreatureScript("npc_lunaclaw_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, creature->GetGUID());
            player->AreaExploredOrEventHappens(player->GetTeam() == ALLIANCE ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
        }
        return true;
    }
};

/*########
# npc_chicken_cluck
#########*/

enum ChickenCluck
{
    EMOTE_HELLO_A       = 0,
    EMOTE_HELLO_H       = 1,
    EMOTE_CLUCK_TEXT    = 2,

    QUEST_CLUCK         = 3861,
    FACTION_FRIENDLY    = 35,
    FACTION_CHICKEN     = 31
};

class npc_chicken_cluck : public CreatureScript
{
public:
    npc_chicken_cluck() : CreatureScript("npc_chicken_cluck") { }

    struct npc_chicken_cluckAI : public ScriptedAI
    {
        npc_chicken_cluckAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 ResetFlagTimer;

        void Reset() override
        {
            ResetFlagTimer = 120000;
            me->SetFaction(FACTION_CHICKEN);
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }

        void JustEngagedWith(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            // Reset flags after a certain time has passed so that the next player has to start the 'event' again
            if (me->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            {
                if (ResetFlagTimer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else
                    ResetFlagTimer -= diff;
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote) override
        {
            switch (emote)
            {
                case TEXT_EMOTE_CHICKEN:
                    if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE && rand() % 30 == 1)
                    {
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        me->SetFaction(FACTION_FRIENDLY);
                        Talk(player->GetTeam() == HORDE ? EMOTE_HELLO_H : EMOTE_HELLO_A);
                    }
                    break;
                case TEXT_EMOTE_CHEER:
                    if (player->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
                    {
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        me->SetFaction(FACTION_FRIENDLY);
                        Talk(EMOTE_CLUCK_TEXT);
                    }
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_chicken_cluckAI(creature);
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }

    bool OnQuestComplete(Player* /*player*/, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CLUCK)
            CAST_AI(npc_chicken_cluck::npc_chicken_cluckAI, creature->AI())->Reset();

        return true;
    }
};

/*######
## npc_dancing_flames
######*/

enum DancingFlames
{
    SPELL_BRAZIER           = 45423,
    SPELL_SEDUCTION         = 47057,
    SPELL_FIERY_AURA        = 45427
};

class npc_dancing_flames : public CreatureScript
{
public:
    npc_dancing_flames() : CreatureScript("npc_dancing_flames") { }

    struct npc_dancing_flamesAI : public ScriptedAI
    {
        npc_dancing_flamesAI(Creature* creature) : ScriptedAI(creature) { }

        bool Active;
        uint32 CanIteract;

        void Reset() override
        {
            Active = true;
            CanIteract = 3500;
            DoCast(me, SPELL_BRAZIER, true);
            DoCast(me, SPELL_FIERY_AURA, false);
            float x, y, z;
            me->GetPosition(x, y, z);
            me->Relocate(x, y, z + 0.94f);
            me->SetDisableGravity(true);
            me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!Active)
            {
                if (CanIteract <= diff)
                {
                    Active = true;
                    CanIteract = 3500;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_DANCE);
                }
                else
                    CanIteract -= diff;
            }
        }

        void JustEngagedWith(Unit* /*who*/)override { }

        void ReceiveEmote(Player* player, uint32 emote) override
        {
            if (me->IsWithinLOS(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()) && me->IsWithinDistInMap(player, 30.0f))
            {
                me->SetInFront(player);
                Active = false;

                switch (emote)
                {
                    case TEXT_EMOTE_KISS:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SHY);
                        break;
                    case TEXT_EMOTE_WAVE:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        break;
                    case TEXT_EMOTE_BOW:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                        break;
                    case TEXT_EMOTE_JOKE:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                        break;
                    case TEXT_EMOTE_DANCE:
                        if (!player->HasAura(SPELL_SEDUCTION))
                            DoCast(player, SPELL_SEDUCTION, true);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_dancing_flamesAI(creature);
    }
};

/*######
## Triage quest
######*/

enum Doctor
{
    SAY_DOC             = 0,

    DOCTOR_ALLIANCE     = 12939,
    DOCTOR_HORDE        = 12920,
    ALLIANCE_COORDS     = 7,
    HORDE_COORDS        = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                      // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                      // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                      // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                      // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                      // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                      // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                      // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                      // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                      // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                      // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                      // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f}                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

uint32 const AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

uint32 const HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/
class npc_doctor : public CreatureScript
{
public:
    npc_doctor() : CreatureScript("npc_doctor") { }

    struct npc_doctorAI : public ScriptedAI
    {
        npc_doctorAI(Creature* creature) : ScriptedAI(creature) { }

        ObjectGuid PlayerGUID;

        uint32 SummonPatientTimer;
        uint32 SummonPatientCount;
        uint32 PatientDiedCount;
        uint32 PatientSavedCount;

        bool Event;

        std::list<ObjectGuid> Patients;
        std::vector<Location*> Coordinates;

        void Reset() override
        {
            PlayerGUID = ObjectGuid::Empty;

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            Patients.clear();
            Coordinates.clear();

            Event = false;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void BeginEvent(Player* player)
        {
            PlayerGUID = player->GetGUID();

            SummonPatientTimer = 10000;
            SummonPatientCount = 0;
            PatientDiedCount = 0;
            PatientSavedCount = 0;

            switch (me->GetEntry())
            {
                case DOCTOR_ALLIANCE:
                    for (uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                        Coordinates.push_back(&AllianceCoords[i]);
                    break;
                case DOCTOR_HORDE:
                    for (uint8 i = 0; i < HORDE_COORDS; ++i)
                        Coordinates.push_back(&HordeCoords[i]);
                    break;
            }

            Event = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void PatientDied(Location* point)
        {
            Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID);
            if (player && ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
            {
                ++PatientDiedCount;

                if (PatientDiedCount > 5 && Event)
                {
                    if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6624);
                    else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(6622);

                    Reset();
                    return;
                }

                Coordinates.push_back(point);
            }
            else
                // If no player or player abandon quest in progress
                Reset();
        }

        void PatientSaved(Creature* /*soldier*/, Player* player, Location* point)
        {
            if (player && PlayerGUID == player->GetGUID())
            {
                if ((player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
                {
                    ++PatientSavedCount;

                    if (PatientSavedCount == 15)
                    {
                        if (!Patients.empty())
                        {
                            std::list<ObjectGuid>::const_iterator itr;
                            for (itr = Patients.begin(); itr != Patients.end(); ++itr)
                            {
                                if (Creature* patient = Unit::GetCreature((*me), *itr))
                                    patient->setDeathState(JUST_DIED);
                            }
                        }

                        if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6624);
                        else if (player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                            player->AreaExploredOrEventHappens(6622);

                        Reset();
                        return;
                    }

                    Coordinates.push_back(point);
                }
            }
        }

        void UpdateAI(uint32 diff) override;

        void JustEngagedWith(Unit* /*who*/) override { }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if ((quest->GetQuestId() == 6624) || (quest->GetQuestId() == 6622))
            CAST_AI(npc_doctor::npc_doctorAI, creature->AI())->BeginEvent(player);

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_doctorAI(creature);
    }
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

class npc_injured_patient : public CreatureScript
{
public:
    npc_injured_patient() : CreatureScript("npc_injured_patient") { }

    struct npc_injured_patientAI : public ScriptedAI
    {
        npc_injured_patientAI(Creature* creature) : ScriptedAI(creature) { }

        ObjectGuid DoctorGUID;
        Location* Coord;

        void Reset() override
        {
            DoctorGUID = ObjectGuid::Empty;
            Coord = NULL;

            //no select
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //no regen health
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            //to make them lay with face down
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);

            uint32 mobId = me->GetEntry();

            switch (mobId)
            {                                                   //lower max health
                case 12923:
                case 12938:                                     //Injured Soldier
                    me->SetHealth(me->CountPctFromMaxHealth(75));
                    break;
                case 12924:
                case 12936:                                     //Badly injured Soldier
                    me->SetHealth(me->CountPctFromMaxHealth(50));
                    break;
                case 12925:
                case 12937:                                     //Critically injured Soldier
                    me->SetHealth(me->CountPctFromMaxHealth(25));
                    break;
            }
        }

        void JustEngagedWith(Unit* /*who*/)override { }

        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            Player* player = caster->ToPlayer();
            if (!player || !me->IsAlive() || spell->Id != 20804)
                return;

            if (player->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                if (DoctorGUID)
                    if (Creature* doctor = Unit::GetCreature(*me, DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientSaved(me, player, Coord);

            //make not selectable
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //regen health
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

            //stand up
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);

            Talk(SAY_DOC);

            uint32 mobId = me->GetEntry();
            me->SetWalk(false);

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    me->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    me->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            //lower HP on every world tick makes it a useful counter, not officlone though
            if (me->IsAlive() && me->GetHealth() > 6)
                me->ModifyHealth(-5);

            if (me->IsAlive() && me->GetHealth() <= 6)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setDeathState(JUST_DIED);
                me->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, 32);

                if (DoctorGUID)
                    if (Creature* doctor = Unit::GetCreature((*me), DoctorGUID))
                        CAST_AI(npc_doctor::npc_doctorAI, doctor->AI())->PatientDied(Coord);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_injured_patientAI(creature);
    }
};

void npc_doctor::npc_doctorAI::UpdateAI(uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatientTimer <= diff)
        {
            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin() + rand() % Coordinates.size();
            uint32 patientEntry = 0;

            switch (me->GetEntry())
            {
                case DOCTOR_ALLIANCE:
                    patientEntry = AllianceSoldierId[rand() % 3];
                    break;
                case DOCTOR_HORDE:
                    patientEntry = HordeSoldierId[rand() % 3];
                    break;
                default:
                    TC_LOG_ERROR("scripts", "Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            if (Location* point = *itr)
            {
                if (Creature* Patient = me->SummonCreature(patientEntry, point->x, point->y, point->z, point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                {
                    //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                    Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                    Patients.push_back(Patient->GetGUID());
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->DoctorGUID = me->GetGUID();
                    CAST_AI(npc_injured_patient::npc_injured_patientAI, Patient->AI())->Coord = point;

                    Coordinates.erase(itr);
                }
            }
            SummonPatientTimer = 10000;
            ++SummonPatientCount;
        }
        else
            SummonPatientTimer -= diff;
    }
}

/*######
## npc_garments_of_quests
######*/

/// @todo get text for each NPC

enum Garments
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    // used by 12429, 12423, 12427, 12430, 12428, but signed for 12429
    SAY_THANKS              = 0,
    SAY_GOODBYE             = 1,
    SAY_HEALED              = 2,
};

class npc_garments_of_quests : public CreatureScript
{
public:
    npc_garments_of_quests() : CreatureScript("npc_garments_of_quests") { }

    struct npc_garments_of_questsAI : public npc_escortAI
    {
        npc_garments_of_questsAI(Creature* creature) : npc_escortAI(creature)
        {
            Reset();
        }

        ObjectGuid CasterGUID;

        bool IsHealed;
        bool CanRun;

        uint32 RunAwayTimer;

        void Reset() override
        {
            CasterGUID = ObjectGuid::Empty;

            IsHealed = false;
            CanRun = false;

            RunAwayTimer = 5000;

            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            // expect database to have RegenHealth=0
            me->SetHealth(me->CountPctFromMaxHealth(70));
        }

        void JustEngagedWith(Unit* /*who*/) override { }

        void SpellHit(Unit* caster, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_LESSER_HEAL_R2 || spell->Id == SPELL_FORTITUDE_R1)
            {
                //not while in combat
                if (me->IsInCombat())
                    return;

                //nothing to be done now
                if (IsHealed && CanRun)
                    return;

                if (Player* player = caster->ToPlayer())
                {
                    switch (me->GetEntry())
                    {
                        case ENTRY_SHAYA:
                            if (player->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_ROBERTS:
                            if (player->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_DOLF:
                            if (player->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_KORJA:
                            if (player->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                        case ENTRY_DG_KEL:
                            if (player->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                            {
                                if (IsHealed && !CanRun && spell->Id == SPELL_FORTITUDE_R1)
                                {
                                    Talk(SAY_THANKS, caster);
                                    CanRun = true;
                                }
                                else if (!IsHealed && spell->Id == SPELL_LESSER_HEAL_R2)
                                {
                                    CasterGUID = caster->GetGUID();
                                    me->SetStandState(UNIT_STAND_STATE_STAND);
                                    Talk(SAY_HEALED, caster);
                                    IsHealed = true;
                                }
                            }
                            break;
                    }

                    // give quest credit, not expect any special quest objectives
                    if (CanRun)
                        player->TalkedToCreature(me->GetEntry(), me->GetGUID());
                }
            }
        }

        void WaypointReached(uint32 /*waypointId*/) override
        {

        }

        void UpdateAI(uint32 diff) override
        {
            if (CanRun && !me->IsInCombat())
            {
                if (RunAwayTimer <= diff)
                {
                    if (Unit* unit = Unit::GetUnit(*me, CasterGUID))
                    {
                        switch (me->GetEntry())
                        {
                            case ENTRY_SHAYA:
                                Talk(SAY_GOODBYE, unit);
                                break;
                            case ENTRY_ROBERTS:
                                Talk(SAY_GOODBYE, unit);
                                break;
                            case ENTRY_DOLF:
                                Talk(SAY_GOODBYE, unit);
                                break;
                            case ENTRY_KORJA:
                                Talk(SAY_GOODBYE, unit);
                                break;
                            case ENTRY_DG_KEL:
                                Talk(SAY_GOODBYE, unit);
                                break;
                        }

                        Start(false, true, ObjectGuid(uint64(1)));
                    }
                    else
                        EnterEvadeMode();                       //something went wrong

                    RunAwayTimer = 30000;
                }
                else
                    RunAwayTimer -= diff;
            }

            npc_escortAI::UpdateAI(diff);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_garments_of_questsAI(creature);
    }
};

/*######
## npc_guardian
######*/

enum GuardianSpells
{
    SPELL_DEATHTOUCH            = 5
};

class npc_guardian : public CreatureScript
{
public:
    npc_guardian() : CreatureScript("npc_guardian") { }

    struct npc_guardianAI : public ScriptedAI
    {
        npc_guardianAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            if (!UpdateVictim())
                return;

            if (me->isAttackReady())
            {
                DoCastVictim(SPELL_DEATHTOUCH, true);
                me->resetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_guardianAI(creature);
    }
};

/*######
## npc_mount_vendor
######*/

class npc_mount_vendor : public CreatureScript
{
public:
    npc_mount_vendor() : CreatureScript("npc_mount_vendor") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        bool canBuy = false;
        uint8 race = player->GetRace();

        switch (creature->GetEntry())
        {
            case 384:                                           // Katie Hunter
            case 1460:                                          // Unger Statforth
            case 2357:                                          // Merideth Carlson
            case 4885:                                          // Gregor MacVince
                if (player->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                    player->SEND_GOSSIP_MENU(5855, creature->GetGUID());
                else canBuy = true;
                break;
            case 1261:                                          // Veron Amberstill
                if (player->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                    player->SEND_GOSSIP_MENU(5856, creature->GetGUID());
                else canBuy = true;
                break;
            case 3362:                                          // Ogunaro Wolfrunner
                if (player->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                    player->SEND_GOSSIP_MENU(5841, creature->GetGUID());
                else canBuy = true;
                break;
            case 3685:                                          // Harb Clawhoof
                if (player->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                    player->SEND_GOSSIP_MENU(5843, creature->GetGUID());
                else canBuy = true;
                break;
            case 4730:                                          // Lelanai
                if (player->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                    player->SEND_GOSSIP_MENU(5844, creature->GetGUID());
                else canBuy = true;
                break;
            case 4731:                                          // Zachariah Post
                if (player->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
                    player->SEND_GOSSIP_MENU(5840, creature->GetGUID());
                else canBuy = true;
                break;
            case 7952:                                          // Zjolnir
                if (player->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                    player->SEND_GOSSIP_MENU(5842, creature->GetGUID());
                else canBuy = true;
                break;
            case 7955:                                          // Milli Featherwhistle
                if (player->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                    player->SEND_GOSSIP_MENU(5857, creature->GetGUID());
                else canBuy = true;
                break;
            case 16264:                                         // Winaestra
                if (player->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                    player->SEND_GOSSIP_MENU(10305, creature->GetGUID());
                else canBuy = true;
                break;
            case 17584:                                         // Torallius the Pack Handler
                if (player->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                    player->SEND_GOSSIP_MENU(10239, creature->GetGUID());
                else canBuy = true;
                break;
            case 48510:                                         // Kall Worthaton
                if (player->GetReputationRank(1133) != REP_EXALTED && race != RACE_GOBLIN)
                    player->SEND_GOSSIP_MENU(17494, creature->GetGUID());
                else canBuy = true;
                break;
            case 55285:                                         // Astrid Langstrump
                if (player->GetReputationRank(1134) != REP_EXALTED && race != RACE_WORGEN)
                    player->SEND_GOSSIP_MENU(18429, creature->GetGUID());
                else canBuy = true;
                break;
            case 65068:                                         // Old Whitenose
                if (player->GetReputationRank(1353) != REP_EXALTED && race != RACE_PANDAREN_ALLIANCE)
                    player->SEND_GOSSIP_MENU(22258, creature->GetGUID());
                else canBuy = true;
                break;
            case 66022:                                         // Turtlemaster Odai
                if (player->GetReputationRank(1352) != REP_EXALTED && race != RACE_PANDAREN_HORDE)
                    player->SEND_GOSSIP_MENU(22257, creature->GetGUID());
                else canBuy = true;
                break;
        }

        if (canBuy)
        {
            if (creature->IsVendor())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};

/*######
## npc_rogue_trainer
######*/

#define GOSSIP_HELLO_ROGUE1 "I wish to unlearn my talents"
#define GOSSIP_HELLO_ROGUE2 "<Take the letter>"
#define GOSSIP_HELLO_ROGUE3 "Purchase a Dual Talent Specialization."
#define GOSSIP_HELLO_ROGUE4 "I wish to unlearn my specialization"

class npc_rogue_trainer : public CreatureScript
{
public:
    npc_rogue_trainer() : CreatureScript("npc_rogue_trainer") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsTrainer())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        if (player->GetClass() == CLASS_ROGUE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE1, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

        if (player->GetClass() == CLASS_ROGUE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE4, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARN_SPEC);

        if (player->GetSpecsCount() == 1 && player->GetLevel() >= sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_HELLO_ROGUE3, GOSSIP_SENDER_MAIN, GOSSIP_OPTION_LEARNDUALSPEC);

        if (player->GetClass() == CLASS_ROGUE && player->GetLevel() >= 24 && !player->HasItemCount(17126) && !player->GetQuestRewardStatus(6681))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_ROGUE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(5996, creature->GetGUID());
        } else
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, 21100, false);
                break;
            case GOSSIP_ACTION_TRAIN:
                player->GetSession()->SendTrainerList(creature->GetGUID());
                break;
            case GOSSIP_OPTION_UNLEARNTALENTS:
                player->CLOSE_GOSSIP_MENU();
                player->SendTalentWipeConfirm(creature->GetGUID(), false);
                break;
            case GOSSIP_OPTION_UNLEARN_SPEC:
                player->CLOSE_GOSSIP_MENU();
                player->SendTalentWipeConfirm(creature->GetGUID(), true);
                break;
            case GOSSIP_OPTION_LEARNDUALSPEC:
                if (player->GetSpecsCount() == 1 && !(player->GetLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL)))
                {
                    if (!player->HasEnoughMoney(uint64(10000000)))
                    {
                        player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
                        player->PlayerTalkClass->SendCloseGossip();
                        break;
                    }
                    else
                    {
                        player->ModifyMoney(int64(-10000000));

                        // Cast spells that teach dual spec
                        // Both are also ImplicitTarget self and must be cast by player
                        player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
                        player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());

                        // Should show another Gossip text with "Congratulations..."
                        player->PlayerTalkClass->SendCloseGossip();
                    }
                }
                break;
        }
        return true;
    }
};

/*######
## npc_sayge
######*/

enum Sayge
{
    SPELL_DMG      = 23768, // dmg
    SPELL_RES      = 23769, // res
    SPELL_ARM      = 23767, // arm
    SPELL_SPI      = 23738, // spi
    SPELL_INT      = 23766, // int
    SPELL_STM      = 23737, // stm
    SPELL_STR      = 23735, // str
    SPELL_AGI      = 23736, // agi
    SPELL_FORTUNE  = 23765  // faire fortune
};

#define GOSSIP_HELLO_SAYGE          "Yes"
#define GOSSIP_SENDACTION_SAYGE1    "Slay the Man"
#define GOSSIP_SENDACTION_SAYGE2    "Turn him over to liege"
#define GOSSIP_SENDACTION_SAYGE3    "Confiscate the corn"
#define GOSSIP_SENDACTION_SAYGE4    "Let him go and have the corn"
#define GOSSIP_SENDACTION_SAYGE5    "Execute your friend painfully"
#define GOSSIP_SENDACTION_SAYGE6    "Execute your friend painlessly"
#define GOSSIP_SENDACTION_SAYGE7    "Let your friend go"
#define GOSSIP_SENDACTION_SAYGE8    "Confront the diplomat"
#define GOSSIP_SENDACTION_SAYGE9    "Show not so quiet defiance"
#define GOSSIP_SENDACTION_SAYGE10   "Remain quiet"
#define GOSSIP_SENDACTION_SAYGE11   "Speak against your brother openly"
#define GOSSIP_SENDACTION_SAYGE12   "Help your brother in"
#define GOSSIP_SENDACTION_SAYGE13   "Keep your brother out without letting him know"
#define GOSSIP_SENDACTION_SAYGE14   "Take credit, keep gold"
#define GOSSIP_SENDACTION_SAYGE15   "Take credit, share the gold"
#define GOSSIP_SENDACTION_SAYGE16   "Let the knight take credit"
#define GOSSIP_SENDACTION_SAYGE17   "Thanks"

class npc_sayge : public CreatureScript
{
public:
    npc_sayge() : CreatureScript("npc_sayge") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->HasSpellCooldown(SPELL_INT) ||
            player->HasSpellCooldown(SPELL_ARM) ||
            player->HasSpellCooldown(SPELL_DMG) ||
            player->HasSpellCooldown(SPELL_RES) ||
            player->HasSpellCooldown(SPELL_STR) ||
            player->HasSpellCooldown(SPELL_AGI) ||
            player->HasSpellCooldown(SPELL_STM) ||
            player->HasSpellCooldown(SPELL_SPI))
            player->SEND_GOSSIP_MENU(7393, creature->GetGUID());
        else
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SAYGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(7339, creature->GetGUID());
        }

        return true;
    }

    void SendAction(Player* player, Creature* creature, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE1,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE2,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE3,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE4,            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(7340, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE5,            GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE6,            GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE7,            GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7341, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE8,            GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE9,            GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE10,           GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7361, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE11,           GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE12,           GOSSIP_SENDER_MAIN + 7, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE13,           GOSSIP_SENDER_MAIN + 8, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7362, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE14,           GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE15,           GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE16,           GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(7363, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SENDACTION_SAYGE17,           GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(7364, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                creature->CastSpell(player, SPELL_FORTUNE, false);
                player->SEND_GOSSIP_MENU(7365, creature->GetGUID());
                break;
        }
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (sender)
        {
            case GOSSIP_SENDER_MAIN:
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 1:
                creature->CastSpell(player, SPELL_DMG, false);
                player->AddSpellCooldown(SPELL_DMG, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 2:
                creature->CastSpell(player, SPELL_RES, false);
                player->AddSpellCooldown(SPELL_RES, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 3:
                creature->CastSpell(player, SPELL_ARM, false);
                player->AddSpellCooldown(SPELL_ARM, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 4:
                creature->CastSpell(player, SPELL_SPI, false);
                player->AddSpellCooldown(SPELL_SPI, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 5:
                creature->CastSpell(player, SPELL_INT, false);
                player->AddSpellCooldown(SPELL_INT, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 6:
                creature->CastSpell(player, SPELL_STM, false);
                player->AddSpellCooldown(SPELL_STM, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 7:
                creature->CastSpell(player, SPELL_STR, false);
                player->AddSpellCooldown(SPELL_STR, Hours(2));
                SendAction(player, creature, action);
                break;
            case GOSSIP_SENDER_MAIN + 8:
                creature->CastSpell(player, SPELL_AGI, false);
                player->AddSpellCooldown(SPELL_AGI, Hours(2));
                SendAction(player, creature, action);
                break;
        }
        return true;
    }
};

class npc_steam_tonk : public CreatureScript
{
public:
    npc_steam_tonk() : CreatureScript("npc_steam_tonk") { }

    struct npc_steam_tonkAI : public ScriptedAI
    {
        npc_steam_tonkAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override { }
        void JustEngagedWith(Unit* /*who*/) override { }

        void OnPossess(bool apply)
        {
            if (apply)
            {
                // Initialize the action bar without the melee attack command
                me->InitCharmInfo();
                me->GetCharmInfo()->InitEmptyActionBar(false);

                me->SetReactState(REACT_PASSIVE);
            }
            else
                me->SetReactState(REACT_AGGRESSIVE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_steam_tonkAI(creature);
    }
};

enum TonkMine
{
    SPELL_TONK_MINE_DETONATE    = 25099
};

class npc_tonk_mine : public CreatureScript
{
public:
    npc_tonk_mine() : CreatureScript("npc_tonk_mine") { }

    struct npc_tonk_mineAI : public ScriptedAI
    {
        npc_tonk_mineAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 ExplosionTimer;

        void Reset() override
        {
            ExplosionTimer = 3000;
        }

        void JustEngagedWith(Unit* /*who*/) override { }
        void AttackStart(Unit* /*who*/) override { }
        void MoveInLineOfSight(Unit* /*who*/) override { }


        void UpdateAI(uint32 diff) override
        {
            if (ExplosionTimer <= diff)
            {
                DoCast(me, SPELL_TONK_MINE_DETONATE, true);
                me->setDeathState(DEAD); // unsummon it
            }
            else
                ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_tonk_mineAI(creature);
    }
};

/*####
## npc_brewfest_reveler
####*/

enum BrewfestReveler
{
    SPELL_BREWFEST_TOAST = 41586
};

class npc_brewfest_reveler : public CreatureScript
{
    public:
        npc_brewfest_reveler() : CreatureScript("npc_brewfest_reveler") { }

        struct npc_brewfest_revelerAI : public ScriptedAI
        {
            npc_brewfest_revelerAI(Creature* creature) : ScriptedAI(creature) { }

            void ReceiveEmote(Player* player, uint32 emote) override
            {
                if (!IsHolidayActive(HOLIDAY_BREWFEST))
                    return;

                if (emote == TEXT_EMOTE_DANCE)
                    me->CastSpell(player, SPELL_BREWFEST_TOAST, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewfest_revelerAI(creature);
        }
};

enum TrainingDummy
{
    NPC_ADVANCED_TARGET_DUMMY                  = 2674,
    NPC_TARGET_DUMMY                           = 2673
};

class npc_training_dummy : public CreatureScript
{
public:
    npc_training_dummy() : CreatureScript("npc_training_dummy") { }

    struct npc_training_dummyAI : ScriptedAI
    {
        npc_training_dummyAI(Creature* creature) : ScriptedAI(creature)
        {
            SetCombatMovement(false);
            entry = creature->GetEntry();
        }

        uint32 entry;
        uint32 resetTimer;
        uint32 despawnTimer;
        std::unordered_map<ObjectGuid, time_t> clearAttackerCombat;

        void Reset() override
        {
            // Do not apply contorl. Stun (blizzlike is pacify but this is not working for us because Attack never happen) flag is needed only for client. At server it won't attack or rotate anyway
            me->SetFacingTo(me->GetHomePosition().GetOrientation());
            me->AddUnitState(UNIT_STATE_ROTATING | UNIT_STATE_ROOT);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);//imune to knock aways like blast wave

            resetTimer = 5000;
            despawnTimer = 15000;
            clearAttackerCombat.clear();
        }

        void EnterEvadeMode() override
        {
            if (!_EnterEvadeMode())
                return;

            Reset();
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            me->GetThreatManager().addThreat(attacker, damage);

            if (Player* owner = attacker->GetCharmerOrOwnerPlayerOrPlayerItself())
                clearAttackerCombat[owner->GetGUID()] = time(NULL) + 5;
            resetTimer = 5000;

            if (damage >= me->GetHealth())
                damage = 0;
        }

        void UpdateAI(uint32 diff) override
        {
            // Reapply in case of root fading
            me->AddUnitState(UNIT_STATE_ROTATING | UNIT_STATE_ROOT);

            if (!UpdateVictim())
                return;

            if (entry != NPC_ADVANCED_TARGET_DUMMY && entry != NPC_TARGET_DUMMY)
            {
                if (resetTimer <= diff)
                {
                    EnterEvadeMode();
                    resetTimer = 5000;
                }
                else
                    resetTimer -= diff;
                return;
            }
            else
            {
                if (despawnTimer <= diff)
                    me->DespawnOrUnsummon();
                else
                    despawnTimer -= diff;
            }

            time_t now = time(nullptr);
            for (auto itr = clearAttackerCombat.begin(); itr != clearAttackerCombat.end();)
            {
                ObjectGuid attackerGuid = itr->first;
                time_t clearTime = itr->second;
                if (now > clearTime)
                {
                    if (Unit* attacker = ObjectAccessor::GetUnit(*me, attackerGuid))
                        attacker->getHostileRefManager().deleteReference(me);

                    itr = clearAttackerCombat.erase(itr);
                }
                else
                    ++itr;
            }
        }

        void AttackStart(Unit* target) override
        {
            if (!target)
                return;

            if (me->Attack(target, true))
                DoStartNoMovement(target);
        }

        void MoveInLineOfSight(Unit* /*who*/) override { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_training_dummyAI(creature);
    }
};

/*######
# npc_wormhole
######*/

#define GOSSIP_ENGINEERING1   "Borean Tundra"
#define GOSSIP_ENGINEERING2   "Howling Fjord"
#define GOSSIP_ENGINEERING3   "Sholazar Basin"
#define GOSSIP_ENGINEERING4   "Icecrown"
#define GOSSIP_ENGINEERING5   "Storm Peaks"
#define GOSSIP_ENGINEERING6   "Underground..."

enum WormholeSpells
{
    SPELL_BOREAN_TUNDRA         = 67834,
    SPELL_SHOLAZAR_BASIN        = 67835,
    SPELL_ICECROWN              = 67836,
    SPELL_STORM_PEAKS           = 67837,
    SPELL_HOWLING_FJORD         = 67838,
    SPELL_UNDERGROUND           = 68081,

    TEXT_WORMHOLE               = 907,

    DATA_SHOW_UNDERGROUND       = 1,
};

class npc_wormhole : public CreatureScript
{
    public:
        npc_wormhole() : CreatureScript("npc_wormhole") { }

        struct npc_wormholeAI : public PassiveAI
        {
            npc_wormholeAI(Creature* creature) : PassiveAI(creature) { }

            void InitializeAI() override
            {
                _showUnderground = urand(0, 100) == 0; // Guessed value, it is really rare though
            }

            uint32 GetData(uint32 type) const override
            {
                return (type == DATA_SHOW_UNDERGROUND && _showUnderground) ? 1 : 0;
            }

        private:
            bool _showUnderground;
        };

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsSummon())
            {
                if (player == creature->ToTempSummon()->GetSummoner())
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

                    if (creature->AI()->GetData(DATA_SHOW_UNDERGROUND))
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ENGINEERING6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

                    player->PlayerTalkClass->SendGossipMenu(TEXT_WORMHOLE, creature->GetGUID());
                }
            }

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1: // Borean Tundra
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_BOREAN_TUNDRA, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2: // Howling Fjord
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_HOWLING_FJORD, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3: // Sholazar Basin
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_SHOLAZAR_BASIN, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4: // Icecrown
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_ICECROWN, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5: // Storm peaks
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_STORM_PEAKS, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 6: // Underground
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_UNDERGROUND, false);
                    break;
            }

            return true;
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wormholeAI(creature);
        }
};

/*######
## npc_pet_trainer
######*/

enum PetTrainer
{
    TEXT_ISHUNTER               = 5838,
    TEXT_NOTHUNTER              = 5839,
    TEXT_PETINFO                = 13474,
    TEXT_CONFIRM                = 7722
};

#define GOSSIP_PET1             "How do I train my pet?"
#define GOSSIP_PET2             "I wish to untrain my pet."
#define GOSSIP_PET_CONFIRM      "Yes, please do."

class npc_pet_trainer : public CreatureScript
{
public:
    npc_pet_trainer() : CreatureScript("npc_pet_trainer") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetClass() == CLASS_HUNTER)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            if (player->GetPet() && player->GetPet()->getPetType() == HUNTER_PET)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            player->PlayerTalkClass->SendGossipMenu(TEXT_ISHUNTER, creature->GetGUID());
            return true;
        }
        player->PlayerTalkClass->SendGossipMenu(TEXT_NOTHUNTER, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->PlayerTalkClass->SendGossipMenu(TEXT_PETINFO, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PET_CONFIRM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->PlayerTalkClass->SendGossipMenu(TEXT_CONFIRM, creature->GetGUID());
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                {
                    player->ResetPetTalents();
                    player->CLOSE_GOSSIP_MENU();
                }
                break;
        }
        return true;
    }
};

/*######
## npc_locksmith
######*/

enum LockSmith
{
    QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ = 10704,
    QUEST_DARK_IRON_LEGACY                = 3802,
    QUEST_THE_KEY_TO_SCHOLOMANCE_A        = 5505,
    QUEST_THE_KEY_TO_SCHOLOMANCE_H        = 5511,
    QUEST_HOTTER_THAN_HELL_A              = 10758,
    QUEST_HOTTER_THAN_HELL_H              = 10764,
    QUEST_RETURN_TO_KHAGDAR               = 9837,
    QUEST_CONTAINMENT                     = 13159,
    QUEST_ETERNAL_VIGILANCE               = 11011,
    QUEST_KEY_TO_THE_FOCUSING_IRIS        = 13372,
    QUEST_HC_KEY_TO_THE_FOCUSING_IRIS     = 13375,

    ITEM_ARCATRAZ_KEY                     = 31084,
    ITEM_SHADOWFORGE_KEY                  = 11000,
    ITEM_SKELETON_KEY                     = 13704,
    ITEM_SHATTERED_HALLS_KEY              = 28395,
    ITEM_THE_MASTERS_KEY                  = 24490,
    ITEM_VIOLET_HOLD_KEY                  = 42482,
    ITEM_ESSENCE_INFUSED_MOONSTONE        = 32449,
    ITEM_KEY_TO_THE_FOCUSING_IRIS         = 44582,
    ITEM_HC_KEY_TO_THE_FOCUSING_IRIS      = 44581,

    SPELL_ARCATRAZ_KEY                    = 54881,
    SPELL_SHADOWFORGE_KEY                 = 54882,
    SPELL_SKELETON_KEY                    = 54883,
    SPELL_SHATTERED_HALLS_KEY             = 54884,
    SPELL_THE_MASTERS_KEY                 = 54885,
    SPELL_VIOLET_HOLD_KEY                 = 67253,
    SPELL_ESSENCE_INFUSED_MOONSTONE       = 40173,
};

#define GOSSIP_LOST_ARCATRAZ_KEY                "I've lost my key to the Arcatraz."
#define GOSSIP_LOST_SHADOWFORGE_KEY             "I've lost my key to the Blackrock Depths."
#define GOSSIP_LOST_SKELETON_KEY                "I've lost my key to the Scholomance."
#define GOSSIP_LOST_SHATTERED_HALLS_KEY         "I've lost my key to the Shattered Halls."
#define GOSSIP_LOST_THE_MASTERS_KEY             "I've lost my key to the Karazhan."
#define GOSSIP_LOST_VIOLET_HOLD_KEY             "I've lost my key to the Violet Hold."
#define GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE   "I've lost my Essence-Infused Moonstone."
#define GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS    "I've lost my Key to the Focusing Iris."
#define GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS "I've lost my Heroic Key to the Focusing Iris."

class npc_locksmith : public CreatureScript
{
public:
    npc_locksmith() : CreatureScript("npc_locksmith") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        // Arcatraz Key
        if (player->GetQuestRewardStatus(QUEST_HOW_TO_BRAKE_IN_TO_THE_ARCATRAZ) && !player->HasItemCount(ITEM_ARCATRAZ_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ARCATRAZ_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        // Shadowforge Key
        if (player->GetQuestRewardStatus(QUEST_DARK_IRON_LEGACY) && !player->HasItemCount(ITEM_SHADOWFORGE_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHADOWFORGE_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        // Skeleton Key
        if ((player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_A) || player->GetQuestRewardStatus(QUEST_THE_KEY_TO_SCHOLOMANCE_H)) &&
            !player->HasItemCount(ITEM_SKELETON_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SKELETON_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        // Shatered Halls Key
        if ((player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_A) || player->GetQuestRewardStatus(QUEST_HOTTER_THAN_HELL_H)) &&
            !player->HasItemCount(ITEM_SHATTERED_HALLS_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_SHATTERED_HALLS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

        // Master's Key
        if (player->GetQuestRewardStatus(QUEST_RETURN_TO_KHAGDAR) && !player->HasItemCount(ITEM_THE_MASTERS_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_THE_MASTERS_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

        // Violet Hold Key
        if (player->GetQuestRewardStatus(QUEST_CONTAINMENT) && !player->HasItemCount(ITEM_VIOLET_HOLD_KEY, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_VIOLET_HOLD_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

        // Essence-Infused Moonstone
        if (player->GetQuestRewardStatus(QUEST_ETERNAL_VIGILANCE) && !player->HasItemCount(ITEM_ESSENCE_INFUSED_MOONSTONE, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_ESSENCE_INFUSED_MOONSTONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

        // Key to the Focusing Iris
        if (player->GetQuestRewardStatus(QUEST_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_KEY_TO_THE_FOCUSING_IRIS, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

        // Heroic Key to the Focusing Iris
        if (player->GetQuestRewardStatus(QUEST_HC_KEY_TO_THE_FOCUSING_IRIS) && !player->HasItemCount(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_HC_KEY_TO_THE_FOCUSING_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_ARCATRAZ_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SHADOWFORGE_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SKELETON_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_SHATTERED_HALLS_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_THE_MASTERS_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_VIOLET_HOLD_KEY, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->CLOSE_GOSSIP_MENU();
                player->CastSpell(player, SPELL_ESSENCE_INFUSED_MOONSTONE, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 8:
                player->CLOSE_GOSSIP_MENU();
                player->AddItem(ITEM_KEY_TO_THE_FOCUSING_IRIS, 1);
                break;
            case GOSSIP_ACTION_INFO_DEF + 9:
                player->CLOSE_GOSSIP_MENU();
                player->AddItem(ITEM_HC_KEY_TO_THE_FOCUSING_IRIS, 1);
                break;
        }
        return true;
    }
};

/*######
## npc_experience
######*/

#define EXP_COST                100000 //10 00 00 copper (10golds)
#define GOSSIP_TEXT_EXP         14736
#define GOSSIP_XP_OFF           "I no longer wish to gain experience."
#define GOSSIP_XP_ON            "I wish to start gaining experience again."

class npc_experience : public CreatureScript
{
public:
    npc_experience() : CreatureScript("npc_experience") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_OFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_XP_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_EXP, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        bool noXPGain = player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN); //PLAYER_FIELD_FLAGS_NO_XP_GAIN
        bool doSwitch = false;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1://xp off
                {
                    if (!noXPGain)//does gain xp
                        doSwitch = true;//switch to don't gain xp
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 2://xp on
                {
                    if (noXPGain)//doesn't gain xp
                        doSwitch = true;//switch to gain xp
                }
                break;
        }
        if (doSwitch)
        {
            if (!player->HasEnoughMoney(uint64(EXP_COST)))
                player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
            else if (noXPGain)
            {
                player->ModifyMoney(-int64(EXP_COST));
                player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
            else if (!noXPGain)
            {
                player->ModifyMoney(-EXP_COST);
                player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_NO_XP_GAIN);
            }
        }
        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }
};

enum Fireworks
{
    NPC_OMEN                = 15467,
    NPC_MINION_OF_OMEN      = 15466,
    NPC_FIREWORK_BLUE       = 15879,
    NPC_FIREWORK_GREEN      = 15880,
    NPC_FIREWORK_PURPLE     = 15881,
    NPC_FIREWORK_RED        = 15882,
    NPC_FIREWORK_YELLOW     = 15883,
    NPC_FIREWORK_WHITE      = 15884,
    NPC_FIREWORK_BIG_BLUE   = 15885,
    NPC_FIREWORK_BIG_GREEN  = 15886,
    NPC_FIREWORK_BIG_PURPLE = 15887,
    NPC_FIREWORK_BIG_RED    = 15888,
    NPC_FIREWORK_BIG_YELLOW = 15889,
    NPC_FIREWORK_BIG_WHITE  = 15890,

    NPC_CLUSTER_BLUE        = 15872,
    NPC_CLUSTER_RED         = 15873,
    NPC_CLUSTER_GREEN       = 15874,
    NPC_CLUSTER_PURPLE      = 15875,
    NPC_CLUSTER_WHITE       = 15876,
    NPC_CLUSTER_YELLOW      = 15877,
    NPC_CLUSTER_BIG_BLUE    = 15911,
    NPC_CLUSTER_BIG_GREEN   = 15912,
    NPC_CLUSTER_BIG_PURPLE  = 15913,
    NPC_CLUSTER_BIG_RED     = 15914,
    NPC_CLUSTER_BIG_WHITE   = 15915,
    NPC_CLUSTER_BIG_YELLOW  = 15916,
    NPC_CLUSTER_ELUNE       = 15918,

    GO_FIREWORK_LAUNCHER_1  = 180771,
    GO_FIREWORK_LAUNCHER_2  = 180868,
    GO_FIREWORK_LAUNCHER_3  = 180850,
    GO_CLUSTER_LAUNCHER_1   = 180772,
    GO_CLUSTER_LAUNCHER_2   = 180859,
    GO_CLUSTER_LAUNCHER_3   = 180869,
    GO_CLUSTER_LAUNCHER_4   = 180874,

    SPELL_ROCKET_BLUE       = 26344,
    SPELL_ROCKET_GREEN      = 26345,
    SPELL_ROCKET_PURPLE     = 26346,
    SPELL_ROCKET_RED        = 26347,
    SPELL_ROCKET_WHITE      = 26348,
    SPELL_ROCKET_YELLOW     = 26349,
    SPELL_ROCKET_BIG_BLUE   = 26351,
    SPELL_ROCKET_BIG_GREEN  = 26352,
    SPELL_ROCKET_BIG_PURPLE = 26353,
    SPELL_ROCKET_BIG_RED    = 26354,
    SPELL_ROCKET_BIG_WHITE  = 26355,
    SPELL_ROCKET_BIG_YELLOW = 26356,
    SPELL_LUNAR_FORTUNE     = 26522,

    ANIM_GO_LAUNCH_FIREWORK = 3,
    ZONE_MOONGLADE          = 493,
};

Position omenSummonPos = {7558.993f, -2839.999f, 450.0214f, 4.46f};

class npc_firework : public CreatureScript
{
public:
    npc_firework() : CreatureScript("npc_firework") { }

    struct npc_fireworkAI : public ScriptedAI
    {
        npc_fireworkAI(Creature* creature) : ScriptedAI(creature) { }

        bool isCluster()
        {
            switch (me->GetEntry())
            {
                case NPC_FIREWORK_BLUE:
                case NPC_FIREWORK_GREEN:
                case NPC_FIREWORK_PURPLE:
                case NPC_FIREWORK_RED:
                case NPC_FIREWORK_YELLOW:
                case NPC_FIREWORK_WHITE:
                case NPC_FIREWORK_BIG_BLUE:
                case NPC_FIREWORK_BIG_GREEN:
                case NPC_FIREWORK_BIG_PURPLE:
                case NPC_FIREWORK_BIG_RED:
                case NPC_FIREWORK_BIG_YELLOW:
                case NPC_FIREWORK_BIG_WHITE:
                    return false;
                case NPC_CLUSTER_BLUE:
                case NPC_CLUSTER_GREEN:
                case NPC_CLUSTER_PURPLE:
                case NPC_CLUSTER_RED:
                case NPC_CLUSTER_YELLOW:
                case NPC_CLUSTER_WHITE:
                case NPC_CLUSTER_BIG_BLUE:
                case NPC_CLUSTER_BIG_GREEN:
                case NPC_CLUSTER_BIG_PURPLE:
                case NPC_CLUSTER_BIG_RED:
                case NPC_CLUSTER_BIG_YELLOW:
                case NPC_CLUSTER_BIG_WHITE:
                case NPC_CLUSTER_ELUNE:
                default:
                    return true;
            }
        }

        GameObject* FindNearestLauncher()
        {
            GameObject* launcher = NULL;

            if (isCluster())
            {
                GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_1, 0.5f);
                GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_2, 0.5f);
                GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_3, 0.5f);
                GameObject* launcher4 = GetClosestGameObjectWithEntry(me, GO_CLUSTER_LAUNCHER_4, 0.5f);

                if (launcher1)
                    launcher = launcher1;
                else if (launcher2)
                    launcher = launcher2;
                else if (launcher3)
                    launcher = launcher3;
                else if (launcher4)
                    launcher = launcher4;
            }
            else
            {
                GameObject* launcher1 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_1, 0.5f);
                GameObject* launcher2 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_2, 0.5f);
                GameObject* launcher3 = GetClosestGameObjectWithEntry(me, GO_FIREWORK_LAUNCHER_3, 0.5f);

                if (launcher1)
                    launcher = launcher1;
                else if (launcher2)
                    launcher = launcher2;
                else if (launcher3)
                    launcher = launcher3;
            }

            return launcher;
        }

        uint32 GetFireworkSpell(uint32 entry)
        {
            switch (entry)
            {
                case NPC_FIREWORK_BLUE:
                    return SPELL_ROCKET_BLUE;
                case NPC_FIREWORK_GREEN:
                    return SPELL_ROCKET_GREEN;
                case NPC_FIREWORK_PURPLE:
                    return SPELL_ROCKET_PURPLE;
                case NPC_FIREWORK_RED:
                    return SPELL_ROCKET_RED;
                case NPC_FIREWORK_YELLOW:
                    return SPELL_ROCKET_YELLOW;
                case NPC_FIREWORK_WHITE:
                    return SPELL_ROCKET_WHITE;
                case NPC_FIREWORK_BIG_BLUE:
                    return SPELL_ROCKET_BIG_BLUE;
                case NPC_FIREWORK_BIG_GREEN:
                    return SPELL_ROCKET_BIG_GREEN;
                case NPC_FIREWORK_BIG_PURPLE:
                    return SPELL_ROCKET_BIG_PURPLE;
                case NPC_FIREWORK_BIG_RED:
                    return SPELL_ROCKET_BIG_RED;
                case NPC_FIREWORK_BIG_YELLOW:
                    return SPELL_ROCKET_BIG_YELLOW;
                case NPC_FIREWORK_BIG_WHITE:
                    return SPELL_ROCKET_BIG_WHITE;
                default:
                    return 0;
            }
        }

        uint32 GetFireworkGameObjectId()
        {
            uint32 spellId = 0;

            switch (me->GetEntry())
            {
                case NPC_CLUSTER_BLUE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BLUE);
                    break;
                case NPC_CLUSTER_GREEN:
                    spellId = GetFireworkSpell(NPC_FIREWORK_GREEN);
                    break;
                case NPC_CLUSTER_PURPLE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_PURPLE);
                    break;
                case NPC_CLUSTER_RED:
                    spellId = GetFireworkSpell(NPC_FIREWORK_RED);
                    break;
                case NPC_CLUSTER_YELLOW:
                    spellId = GetFireworkSpell(NPC_FIREWORK_YELLOW);
                    break;
                case NPC_CLUSTER_WHITE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_WHITE);
                    break;
                case NPC_CLUSTER_BIG_BLUE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_BLUE);
                    break;
                case NPC_CLUSTER_BIG_GREEN:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_GREEN);
                    break;
                case NPC_CLUSTER_BIG_PURPLE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_PURPLE);
                    break;
                case NPC_CLUSTER_BIG_RED:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_RED);
                    break;
                case NPC_CLUSTER_BIG_YELLOW:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_YELLOW);
                    break;
                case NPC_CLUSTER_BIG_WHITE:
                    spellId = GetFireworkSpell(NPC_FIREWORK_BIG_WHITE);
                    break;
                case NPC_CLUSTER_ELUNE:
                    spellId = GetFireworkSpell(urand(NPC_FIREWORK_BLUE, NPC_FIREWORK_WHITE));
                    break;
            }

            const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);

            if (spellInfo && spellInfo->Effects[0].Effect == SPELL_EFFECT_SUMMON_OBJECT_WILD)
                return spellInfo->Effects[0].MiscValue;

            return 0;
        }

        void Reset() override
        {
            if (GameObject* launcher = FindNearestLauncher())
            {
                launcher->SendCustomAnim(ANIM_GO_LAUNCH_FIREWORK);
                me->SetOrientation(launcher->GetOrientation() + M_PI/2);
            }
            else
                return;

            if (isCluster())
            {
                // Check if we are near Elune'ara lake south, if so try to summon Omen or a minion
                if (me->GetZoneId() == ZONE_MOONGLADE)
                {
                    if (!me->FindNearestCreature(NPC_OMEN, 100.0f, false) && me->GetDistance2d(omenSummonPos.GetPositionX(), omenSummonPos.GetPositionY()) <= 100.0f)
                    {
                        switch (urand(0, 9))
                        {
                            case 0:
                            case 1:
                            case 2:
                            case 3:
                                if (Creature* minion = me->SummonCreature(NPC_MINION_OF_OMEN, me->GetPositionX()+frand(-5.0f, 5.0f), me->GetPositionY()+frand(-5.0f, 5.0f), me->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
                                    minion->AI()->AttackStart(me->SelectNearestPlayer(20.0f));
                                break;
                            case 9:
                                me->SummonCreature(NPC_OMEN, omenSummonPos);
                                break;
                        }
                    }
                }
                if (me->GetEntry() == NPC_CLUSTER_ELUNE)
                    DoCast(SPELL_LUNAR_FORTUNE);

                float displacement = 0.7f;
                for (uint8 i = 0; i < 4; i++)
                    me->SummonGameObject(GetFireworkGameObjectId(), me->GetPositionX() + (i % 2 == 0 ? displacement : -displacement), me->GetPositionY() + (i > 1 ? displacement : -displacement), me->GetPositionZ() + 4.0f, me->GetOrientation(), { }, 1);
            }
            else
                //me->CastSpell(me, GetFireworkSpell(me->GetEntry()), true);
                me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), GetFireworkSpell(me->GetEntry()), true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_fireworkAI(creature);
    }
};

/*#####
# npc_spring_rabbit
#####*/

enum rabbitSpells
{
    SPELL_SPRING_FLING          = 61875,
    SPELL_SPRING_RABBIT_JUMP    = 61724,
    SPELL_SPRING_RABBIT_WANDER  = 61726,
    SPELL_SUMMON_BABY_BUNNY     = 61727,
    SPELL_SPRING_RABBIT_IN_LOVE = 61728,
    NPC_SPRING_RABBIT           = 32791
};

class npc_spring_rabbit : public CreatureScript
{
public:
    npc_spring_rabbit() : CreatureScript("npc_spring_rabbit") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_spring_rabbitAI(creature);
    }

    struct npc_spring_rabbitAI : public ScriptedAI
    {
        npc_spring_rabbitAI(Creature* creature) : ScriptedAI(creature) { }

        bool inLove;
        uint32 jumpTimer;
        uint32 bunnyTimer;
        uint32 searchTimer;
        ObjectGuid rabbitGUID;

        void Reset() override
        {
            inLove = false;
            rabbitGUID = ObjectGuid::Empty;
            jumpTimer = urand(5000, 10000);
            bunnyTimer = urand(10000, 20000);
            searchTimer = urand(5000, 10000);
            if (Unit* owner = me->GetOwner())
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        void JustEngagedWith(Unit* /*who*/) override { }

        void DoAction(int32 /*param*/) override
        {
            inLove = true;
            if (Unit* owner = me->GetOwner())
                owner->CastSpell(owner, SPELL_SPRING_FLING, true);
        }

        void UpdateAI(uint32 diff) override
        {
            if (inLove)
            {
                if (jumpTimer <= diff)
                {
                    if (Unit* rabbit = Unit::GetUnit(*me, rabbitGUID))
                        DoCast(rabbit, SPELL_SPRING_RABBIT_JUMP);
                    jumpTimer = urand(5000, 10000);
                } else jumpTimer -= diff;

                if (bunnyTimer <= diff)
                {
                    DoCast(SPELL_SUMMON_BABY_BUNNY);
                    bunnyTimer = urand(20000, 40000);
                } else bunnyTimer -= diff;
            }
            else
            {
                if (searchTimer <= diff)
                {
                    if (Creature* rabbit = me->FindNearestCreature(NPC_SPRING_RABBIT, 10.0f))
                    {
                        if (rabbit == me || rabbit->HasAura(SPELL_SPRING_RABBIT_IN_LOVE))
                            return;

                        me->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, me);
                        DoAction(1);
                        rabbit->AddAura(SPELL_SPRING_RABBIT_IN_LOVE, rabbit);
                        rabbit->AI()->DoAction(1);
                        rabbit->CastSpell(rabbit, SPELL_SPRING_RABBIT_JUMP, true);
                        rabbitGUID = rabbit->GetGUID();
                    }
                    searchTimer = urand(5000, 10000);
                } else searchTimer -= diff;
            }
        }
    };
};



struct npc_stone_statue : public ScriptedAI
{
    npc_stone_statue(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        if (uint32 spellId = me->m_spells[0])
            DoCastAOE(spellId);
    }

    void EnterEvadeMode() override { }
    bool CanAIAttack(Unit const* /*who*/) const override { return false; }
    void AttackStart(Unit* /*who*/) override { }
    void JustEngagedWith(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

enum SkyLatern
{
    SPELL_KITE_STRING = 149049,
};

// Sky Latern 73809
struct npc_sky_lantern : public ScriptedAI
{
    npc_sky_lantern(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->GetTypeId() == TYPEID_PLAYER)
        {
            me->IsInEvadeMode();
            DoCast(summoner, SPELL_KITE_STRING);
            me->ClearUnitState(UNIT_STATE_CASTING); // dirty hack
            me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }
    }

    void EnterEvadeMode() override { }
    bool CanAIAttack(Unit const* /*who*/) const override { return false; }
    void AttackStart(Unit* /*who*/) override { }
    void JustEngagedWith(Unit* /*who*/) override { }
    void UpdateAI(uint32 /*diff*/) override { }
};

enum PandariaTreasures
{
    NPC_GHOSTLY_PANDAREN_FISHERMAN   = 64004,
    NPC_GHOSTLY_PANDAREN_CRAFTSMAN   = 64191,
    NPC_GLINTING_RAPANA_WHELK        = 65552,
    NPC_JADE_WARRIOR_STATUE          = 64272,
    NPC_FROZEN_TRAIL_PACKER          = 64227,

    QUEST_GHOSTLY_PANDAREN_FISHERMAN = 31284,
    QUEST_GHOSTLY_PANDAREN_CRAFTSMAN = 31292,
    QUEST_GLINTING_RAPANA_WHELK      = 31432,
    QUEST_JADE_WARRIOR_STATUE        = 31307,
    QUEST_FROZEN_TRAIL_PACKER        = 31304,

    REW_GHOSTLY_PANDAREN_FISHERMAN   = 85973,
    REW_GHOSTLY_PANDAREN_CRAFTSMAN   = 86079,
    REW_GLINTING_RAPANA_WHELK        = 86529,
    REW_JADE_WARRIOR_STATUE          = 86199,
    REW_FROZEN_TRAIL_PACKER          = 86125,
};

std::map<uint32, std::pair<uint32, uint32>> pandariaTreasuresMap =
{
    { NPC_GHOSTLY_PANDAREN_FISHERMAN, std::make_pair(QUEST_GHOSTLY_PANDAREN_FISHERMAN, REW_GHOSTLY_PANDAREN_FISHERMAN) },
    { NPC_GHOSTLY_PANDAREN_CRAFTSMAN, std::make_pair(QUEST_GHOSTLY_PANDAREN_CRAFTSMAN, REW_GHOSTLY_PANDAREN_CRAFTSMAN) },
    { NPC_GLINTING_RAPANA_WHELK,      std::make_pair(QUEST_GLINTING_RAPANA_WHELK,      REW_GLINTING_RAPANA_WHELK)      },
    { NPC_JADE_WARRIOR_STATUE,        std::make_pair(QUEST_JADE_WARRIOR_STATUE,        REW_JADE_WARRIOR_STATUE)        },
    { NPC_FROZEN_TRAIL_PACKER,        std::make_pair(QUEST_FROZEN_TRAIL_PACKER,        REW_FROZEN_TRAIL_PACKER)        },
};

class npc_pandaria_treasures : public CreatureScript
{
    public:
        npc_pandaria_treasures() : CreatureScript("npc_pandaria_treasures") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            auto itr = pandariaTreasuresMap.find(creature->GetEntry());
            if (itr != pandariaTreasuresMap.end())
                if (player->GetQuestStatus(itr->second.first) != QUEST_STATUS_REWARDED)
                    player->ADD_GOSSIP_ITEM_DB(creature->GetGossipMenuId(), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->PlayerTalkClass->ClearMenus();

        auto itr = pandariaTreasuresMap.find(creature->GetEntry());
        if (itr != pandariaTreasuresMap.end())
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(itr->second.first))
            {
                player->RewardQuest(quest, 0, player, false);
                player->AddItem(itr->second.second, 1);
            }

        player->CLOSE_GOSSIP_MENU();
        creature->DespawnOrUnsummon();
        return true;
    }
};

struct npc_visual_sparring : public ScriptedAI
{
    npc_visual_sparring(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker->GetTypeId() != TYPEID_PLAYER && !HealthAbovePct(85))
            damage = 0;
    }
};

class npc_steam_powered_auctioneer : public CreatureScript
{
    public:
        npc_steam_powered_auctioneer() : CreatureScript("npc_steam_powered_auctioneer") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->GetZoneId() != 4395 && !player->HasSkill(SKILL_ENGINEERING))
            {
                player->SEND_GOSSIP_MENU(20943, creature->GetGUID());
                return true;
            }

            player->GetSession()->SendAuctionHello(creature->GetGUID(), creature);
            return true;
        }
};

struct npc_abominated_greench : public ScriptedAI
{
    npc_abominated_greench(Creature* creature) : ScriptedAI(creature) { }

    void DamageTaken(Unit* attacker, uint32& /*damage*/) override
    {
        if (attacker->GetTypeId() == TYPEID_PLAYER)
            if (eligiblePlayers.find(attacker->GetGUID()) == eligiblePlayers.end())
                eligiblePlayers.insert(attacker->GetGUID());
    }

    void JustDied(Unit* who) override
    {
        for (auto&& guid : eligiblePlayers)
            if (Player* player = ObjectAccessor::FindPlayer(guid))
                player->KilledMonsterCredit(15664);

        if (Creature* metzen = me->FindNearestCreature(15664, 100.0f))
        {
            if (metzen->IsAlive())
            {
                metzen->GetMotionMaster()->MovePoint(0, 217.293f, -295.882f, 151.440f);
                metzen->DespawnOrUnsummon(60000);
            }

            metzen->SummonGameObject(209506, 226.833f, -263.111f, 145.776f, 0.0f, { }, 120);
            metzen->SummonGameObject(209506, 226.441f, -280.302f, 146.928f, 0.0f, { }, 120);
            metzen->SummonGameObject(209506, 228.601f, -270.087f, 146.318f, 0.0f, { }, 120);
            metzen->SummonGameObject(209497, 225.609f, -259.333f, 145.006f, 0.0f, { }, 120);
            metzen->SummonGameObject(209497, 232.154f, -266.674f, 145.929f, 0.0f, { }, 120);
            metzen->SummonGameObject(209497, 224.170f, -278.483f, 146.675f, 0.0f, { }, 120);
        }

        std::list<Creature*> helpers;
        me->GetCreatureListWithEntryInGrid(helpers, 55003, 100.0f);
        for (auto&& helper : helpers)
        {
            if (!helper->IsAlive())
                continue;

            helper->AI()->Talk(0);
            if (helper->GetPositionY() > -296.0f)
                helper->GetMotionMaster()->MovePoint(0, 218.783f, -291.750f, 150.972f);
            else
                helper->GetMotionMaster()->MovePoint(0, 220.692f, -302.994f, 151.439f);
            helper->DespawnOrUnsummon(5000);
        }
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();

        if (me->GetDistance(me->GetPosition()) > 100.0f)
            EnterEvadeMode();
    }

private:
    std::set<ObjectGuid> eligiblePlayers;
};

struct npc_anatomical_dummy : public ScriptedAI
{
    npc_anatomical_dummy(Creature* creature) : ScriptedAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        talk_timer = 5000;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!talk_timer)
            return;

        talk_timer -= diff;
        if (talk_timer <= 0)
        {
            if (!me->IsInCombat())
            {
                talk_timer = 0;
                return;
            }
            talk_timer = urand(20000, 40000);
            Talk(0);
        }
    }

private:
    int32 talk_timer = 0;
};

enum soulMirrorsSpell
{
    SPELL_CLONE_ME            = 45204,
    SPELL_COSMETIC_50_PCT_INV = 69676,
};

// Soul Mirror 51337
struct npc_soul_mirror_artifact : public ScriptedAI
{
    npc_soul_mirror_artifact(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        summoner->CastSpell(me, SPELL_CLONE_ME, true);
        me->SetTarget(summoner->GetGUID());
    }

    void EnterEvadeMode() override { }
};

enum naaruArrivedsMisc
{
    SPELL_ARRIVA_OF_THE_NAARU      = 91752,
    SPELL_SUMMON_VISION_OF_NAARU   = 94443,
    SPELL_VISION_OF_NAARU_COSMETIC = 94444,
    NPC_VISION_OF_THE_NAARU        = 51190,
    NPC_VERY_TINY_DRANEI           = 51191,
};

// Vision of the Naaru Invisible Stalker 51200
struct npc_vision_of_the_naaru_stalker : public ScriptedAI
{
    npc_vision_of_the_naaru_stalker(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(me, SPELL_VISION_OF_NAARU_COSMETIC);
    }

    void EnterEvadeMode() override { }
};

// Vision of the Naaru 51190
struct npc_vision_of_the_naaru : public ScriptedAI
{
    npc_vision_of_the_naaru(Creature* creature) : ScriptedAI(creature) { }

    float angleFacing, x, y;
    std::vector<ObjectGuid> tinyDraneiGUIDs;

    void IsSummonedBy(Unit* summoner) override
    {
        // shoul`ve size more than default
        me->SetObjectScale(2.0f);
        x = 0.0f; y = 0.0f;
        angleFacing = Position::NormalizeOrientation(me->GetOrientation() + M_PI - M_PI / 6);
        tinyDraneiGUIDs.resize(3, ObjectGuid::Empty);

        for (uint32 i = 0; i < 3; i++)
        {
            GetPositionWithDistInOrientation(me, 6.0f, Position::NormalizeOrientation(angleFacing + i * (M_PI / 6)), x, y);

            if (Creature* tinyDranei = me->SummonCreature(NPC_VERY_TINY_DRANEI, x, y, me->GetPositionZ(), Position::NormalizeOrientation(angleFacing + i * (M_PI / 6) + M_PI), TEMPSUMMON_TIMED_DESPAWN, 8 * IN_MILLISECONDS))
            {
                GetPositionWithDistInOrientation(tinyDranei, 4.5f, tinyDranei->GetOrientation(), x, y);
                tinyDraneiGUIDs[i] = tinyDranei->GetGUID();

                // move towards the naaru
                Movement::MoveSplineInit init(tinyDranei);
                init.MoveTo(x, y, me->GetPositionZ());
                init.Launch();

                tinyDranei->m_Events.Schedule(tinyDranei->GetSplineDuration(), [this, i]()
                {
                    if (Creature* dranei = ObjectAccessor::GetCreature(*me, tinyDraneiGUIDs[i]))
                        dranei->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                });
            }
        }
    }
};

enum swordDancersMisc
{
    SPELL_SUMMON_SWORD_DANCERS = 94691,
};

// Sword Dancer Invisible Stalker 51268
struct npc_sword_dancers_stalker : public ScriptedAI
{
    npc_sword_dancers_stalker(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(me, SPELL_SUMMON_SWORD_DANCERS);
    }

    void EnterEvadeMode() override { }
};

// Sword Dancers 51269, 51270, 51271, 51272
struct npc_sword_dancer : public ScriptedAI
{
    npc_sword_dancer(Creature* creature) : ScriptedAI(creature) { }

    uint32 delay;
    float spawnedOri, x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        x = 0.0f; y = 0.0f;
        spawnedOri = me->GetOrientation();

        delay = 0;
        me->m_Events.Schedule(delay += 500, [this]()
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
        });

        me->m_Events.Schedule(delay += 2500, [this]()
        {
            me->HandleEmoteStateCommand(EMOTE_STATE_DANCE);
        });

        me->m_Events.Schedule(delay += 2500, [this]()
        {
            GetPositionWithDistInOrientation(me, 1.5f, Position::NormalizeOrientation(spawnedOri + M_PI / 2), x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), Position::NormalizeOrientation(spawnedOri + M_PI / 2));
        });

        me->m_Events.Schedule(delay += 3600, [this]()
        {
            GetPositionWithDistInOrientation(me, 1.5f, Position::NormalizeOrientation(spawnedOri - M_PI / 2), x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), Position::NormalizeOrientation(spawnedOri - M_PI / 2));
        });

        me->m_Events.Schedule(delay += 3600, [this]()
        {
            GetPositionWithDistInOrientation(me, 2.0f, spawnedOri, x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), spawnedOri);
        });

        me->m_Events.Schedule(delay += 2200, [this]()
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_APPLAUD);
        });

        me->m_Events.Schedule(delay += 1500, [this]()
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
        });
    }
};

// Mini Mana Bomb - 65903
struct npc_mini_mana_bomb : public ScriptedAI
{
    npc_mini_mana_bomb(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        ObjectGuid summonerGuid = summoner->GetGUID();
        me->AddAura(105729, me);
        me->m_Events.Schedule(1000, [this, summonerGuid]()
        {
            me->SendPlaySpellVisualKit(9715, 0, 0);
            me->SendPlaySpellVisual(965, summonerGuid, 0.0f, 0.0f, 0.0f, 0.0f, false, 0, 0);
            me->RemoveAurasDueToSpell(105729);
            me->DespawnOrUnsummon(4000);
        });
    }
};

enum HauntedMementoEnums
{
    NPC_SCOURGE_HAUNT = 29238,

    SPELL_HAUNTED                                   = 53768,
    SPELL_THROW_HAUNTED_MEMENTO                     = 54039,
    SPELL_REMOVE_HAUNTED_MEMENTO                    = 54041,
    SPELL_FORCE_PLAYER_TO_REMOVE_HAUNTED_MEMENTO    = 54042,

    EVENT_UPDATE_SUMMONER = 1,
    EVENT_CHECK_SUMMONER_AURA,
};

struct npc_scourge_haunt : public CreatureAI
{
    npc_scourge_haunt(Creature* creature) : CreatureAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetReactState(REACT_PASSIVE);
        me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, summoner->GetRelativeAngle(me));
        events.ScheduleEvent(EVENT_UPDATE_SUMMONER, 1000);
        events.ScheduleEvent(EVENT_CHECK_SUMMONER_AURA, 5000);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_UPDATE_SUMMONER:
            {
                if (TempSummon* summon = me->ToTempSummon())
                {
                    if (Unit* summoner = summon->GetSummoner())
                    {
                        if (AdjustMovementFlag(MOVEMENTFLAG_FLYING | MOVEMENTFLAG_DISABLE_GRAVITY, MOVEMENTFLAG_FLYING, summoner) ||
                            AdjustMovementFlag(MOVEMENTFLAG_WALKING, MOVEMENTFLAG_WALKING, summoner))
                            me->SendMovementFlagUpdate();

                        me->SetSpeed(GetSpeedType(), summoner->GetSpeedRate(GetSpeedType()));

                        if (!me->IsWithinDistInMap(summoner, PET_FOLLOW_DIST + 1.0f))
                            me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, summoner->GetRelativeAngle(me));

                        events.ScheduleEvent(EVENT_UPDATE_SUMMONER, 1000);
                        break;
                    }
                }
                me->DespawnOrUnsummon();
                break;
            }
            case EVENT_CHECK_SUMMONER_AURA:
            {
                if (TempSummon* summon = me->ToTempSummon())
                {
                    if (Unit* summoner = summon->GetSummoner())
                    {
                        if (summoner->HasAura(SPELL_HAUNTED))
                        {
                            events.ScheduleEvent(EVENT_CHECK_SUMMONER_AURA, 5000);
                            break;
                        }
                    }
                }
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
        }
    }

private:
    EventMap events;

    bool AdjustMovementFlag(uint32 myFlag, uint32 summonerFlag, Unit* summoner)
    {
        if ((me->HasUnitMovementFlag(myFlag) != 0) ^ (summoner->HasUnitMovementFlag(summonerFlag) != 0))
        {
            if (summoner->HasUnitMovementFlag(summonerFlag))
                me->AddUnitMovementFlag(myFlag);
            else
                me->RemoveUnitMovementFlag(myFlag);

            return true;
        }
        return false;
    }
    UnitMoveType GetSpeedType() const
    {
        if (me->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
            return MOVE_WALK;
        if (me->HasUnitMovementFlag(MOVEMENTFLAG_FLYING))
            return MOVE_FLIGHT;
        return MOVE_RUN;
    }
};

enum MusicBox
{
    SPELL_LAMENT_OF_THE_HIGHBORNE_HIGHBORNE_AURA = 37090,
};

struct npc_sylvanas_lamenter : public ScriptedAI
{
    npc_sylvanas_lamenter(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(me, SPELL_LAMENT_OF_THE_HIGHBORNE_HIGHBORNE_AURA, true);

        Position pos;
        pos.Relocate(me);
        pos.m_positionZ = summoner->GetPositionZ();

        me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());

        pos.m_positionZ += 3.5f;

        me->SetFlying(false);
        me->SetWalk(true);
        me->SendMovementFlagUpdate();
        me->GetMotionMaster()->MovePoint(0, pos);
    }
};

enum EtherealSoulTraderEnums
{
    SPELL_SUMMON_ETHEREAL_PET                       = 50049,
    SPELL_ETHEREAL_PET_AURA_ONKILL                  = 50050,
    SPELL_ETHEREAL_PET_AURA                         = 50051,
    SPELL_ETHEREAL_PET_ONSUMMON                     = 50052,
    SPELL_ETHEREAL_PET_AURA_REMOVE                  = 50055,
    SPELL_PET_ONKILL_STEAL_ESSENCE_GET_TARGET       = 50059,
    SPELL_ETHEREAL_PET_ONKILL_GIVE_TOKEN_MISSILE    = 50063,
    SPELL_ETHEREAL_PET_ONKILL_GIVE_TOKEN            = 50067,
    SPELL_ETHEREAL_PET_ONKILL_STEAL_ESSENCE         = 50101,

    SAY_SUMMON     = 0,
    SAY_STEAL      = 1,
    SAY_GIVE_TOKEN = 2,

    EVENT_SAY_SUMMON = 1,
    EVENT_STEAL_ESSENCE,
    EVENT_SAY_GIVE_TOKEN,
};

struct npc_ethereal_soul_trader : public ScriptedAI
{
    npc_ethereal_soul_trader(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(summoner, SPELL_ETHEREAL_PET_ONSUMMON, true);

        events.Reset();
        events.ScheduleEvent(EVENT_SAY_SUMMON, 3000);
        events.ScheduleEvent(EVENT_STEAL_ESSENCE, 1000);
    }

    void Unsummoned() override
    {
        DoCastAOE(SPELL_ETHEREAL_PET_AURA_REMOVE, true);
    }

    void JustDied(Unit* killer) override
    {
        events.Reset();
        DoCastAOE(SPELL_ETHEREAL_PET_AURA_REMOVE, true);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_ETHEREAL_PET_AURA_ONKILL)
        {
            DoCast(me, SPELL_PET_ONKILL_STEAL_ESSENCE_GET_TARGET, true);
            queuedTargets.push(caster->GetGUID());
        }
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_ETHEREAL_PET_ONSUMMON:
                if (target)
                    target->CastSpell(target, SPELL_ETHEREAL_PET_AURA, true);
                break;
            case SPELL_ETHEREAL_PET_AURA_REMOVE:
                if (target)
                    target->RemoveAurasDueToSpell(SPELL_ETHEREAL_PET_AURA);
                break;
            default:
                break;
        }
    }

    void AttackStart(Unit* victim) override { }

    void MoveInLineOfSight(Unit* who) override { }

    void UpdateAI(uint32 diff) override
    {
        UpdateVictim();

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
            case EVENT_SAY_SUMMON:
                Talk(SAY_SUMMON);
                break;
            case EVENT_SAY_GIVE_TOKEN:
                if (roll_chance_i(50))
                    Talk(SAY_GIVE_TOKEN);
                break;
            case EVENT_STEAL_ESSENCE:
                events.ScheduleEvent(EVENT_STEAL_ESSENCE, 1000);
                while (queuedTargets.size())
                {
                    ObjectGuid targetGuid = queuedTargets.front();
                    queuedTargets.pop();
                    if (Unit* target = ObjectAccessor::GetUnit(*me, targetGuid))
                        DoCast(target, SPELL_ETHEREAL_PET_ONKILL_STEAL_ESSENCE);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SPELL_CAST_OMNI);
                        if (roll_chance_i(25))
                        {
                            Talk(SAY_STEAL);
                            events.ScheduleEvent(EVENT_SAY_GIVE_TOKEN, 3900);
                        }
                        break;
                    }
                }

                if (TempSummon* summon = me->ToTempSummon())
                {
                    if (Unit* summoner = summon->GetSummoner())
                    {
                        if (me->IsWalking() != summoner->IsWalking())
                            me->SetWalk(summoner->IsWalking());

                        if (!me->IsWithinDistInMap(summoner, 100.0f, false))
                            me->DespawnOrUnsummon();
                    }
                }
                break;
            default:
                break;
        }
    }

private:
    EventMap events;
    std::queue<ObjectGuid> queuedTargets;
};

struct npc_sa_demolisher : public VehicleAI
{
    npc_sa_demolisher(Creature* creature) : VehicleAI(creature) { }

    void JustDied(Unit* summoner) override
    {
        me->DespawnOrUnsummon(5000);
    }
};

enum RarePandariaNpcs
{
    SPELL_SMOKED_BLADE   = 125633,
    SPELL_GRAPPLING_HOOK = 125623,
    SPELL_VANISH         = 125632,
    SPELL_VICIOUS_REND   = 125624,

    EVENT_GRAPPLING_HOOK = 1,
    EVENT_VIC_REND,
    EVENT_VANISH,
};

// Sarnak - 50782
// Salyin Warscout - 50783
// Omnis Grinlok - 50805
struct npc_rogue_rare_npc : public ScriptedAI
{
    npc_rogue_rare_npc(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    uint32 delay = 0;
    ObjectGuid targetGUID;

    void Reset() override
    {
        delay = 0;
        me->m_Events.Schedule(delay += 1000, [this]()
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                DoCast(target, SPELL_SMOKED_BLADE, true);
                return;
            }
        });

        delay = 0;
        targetGUID = ObjectGuid::Empty;
        events.Reset();
        me->setRegeneratingHealth(true);
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(0);
        me->setRegeneratingHealth(false);
        DoCast(who, SPELL_GRAPPLING_HOOK);
        events.ScheduleEvent(EVENT_VANISH, urand(13.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_VIC_REND, 4 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 15 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_VANISH:
                    DoCastVictim(SPELL_VANISH);
                    events.ScheduleEvent(EVENT_VANISH, urand(13.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                    break;
                case EVENT_VIC_REND:
                    DoCastVictim(SPELL_VICIOUS_REND);
                    events.ScheduleEvent(EVENT_VIC_REND, urand(5.5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                    break;
                case EVENT_GRAPPLING_HOOK:
                    if (Unit* target = me->GetVictim())
                        if (target->GetExactDist2d(me) > 5.0f)
                            DoCast(target, SPELL_GRAPPLING_HOOK);
                    events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

enum pandarenFireworkSpells
{
    SPELL_PANDAREN_FIREWORK_LAUNCHER = 129704,
};

// Pandaren`s Firework Launcher 65934
struct npc_pandaren_firework_launcher : public ScriptedAI
{
    npc_pandaren_firework_launcher(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->SetDisplayFromModel(1);

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            DoCast(me, SPELL_PANDAREN_FIREWORK_LAUNCHER);
        });
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

void AddSC_npcs_special()
{
    new npc_air_force_bots();
    new npc_lunaclaw_spirit();
    new npc_chicken_cluck();
    new npc_dancing_flames();
    new npc_doctor();
    new npc_injured_patient();
    new npc_garments_of_quests();
    new npc_guardian();
    new npc_mount_vendor();
    new npc_rogue_trainer();
    new npc_sayge();
    new npc_steam_tonk();
    new npc_tonk_mine();
    new npc_brewfest_reveler();
    new npc_training_dummy();
    new npc_wormhole();
    new npc_pet_trainer();
    new npc_locksmith();
    new npc_experience();
    new npc_firework();
    new npc_spring_rabbit();
    new creature_script<npc_stone_statue>("npc_stone_statue");
    new creature_script<npc_sky_lantern>("npc_sky_lantern");
    new npc_pandaria_treasures();
    new npc_steam_powered_auctioneer();
    new creature_script<npc_visual_sparring>("npc_visual_sparring");
    new creature_script<npc_abominated_greench>("npc_abominated_greench");
    new creature_script<npc_anatomical_dummy>("npc_anatomical_dummy");
    new creature_script<npc_soul_mirror_artifact>("npc_soul_mirror_artifact");
    new creature_script<npc_vision_of_the_naaru_stalker>("npc_vision_of_the_naaru_stalker");
    new creature_script<npc_vision_of_the_naaru>("npc_vision_of_the_naaru");
    new creature_script<npc_sword_dancers_stalker>("npc_sword_dancers_stalker");
    new creature_script<npc_sword_dancer>("npc_sword_dancer");
    new creature_script<npc_mini_mana_bomb>("npc_mini_mana_bomb");
    new creature_script<npc_scourge_haunt>("npc_scourge_haunt");
    new creature_script<npc_sylvanas_lamenter>("npc_sylvanas_lamenter");
    new creature_script<npc_ethereal_soul_trader>("npc_ethereal_soul_trader");
    new creature_script<npc_sa_demolisher>("npc_sa_demolisher");
    new creature_script<npc_rogue_rare_npc>("npc_rogue_rare_npc");
    new creature_script<npc_pandaren_firework_launcher>("npc_pandaren_firework_launcher");
}
