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

#include "icecrown_citadel.h"
#include "MapManager.h"
#include "Transport.h"
#include "Vehicle.h"
#include "Group.h"
#include "MoveSplineInit.h"
#include "CombatAI.h"
#include "SpellHistory.h"

enum eAchievements
{
    IM_ON_A_BOAT_10    = 4536,
    IM_ON_A_BOAT_25    = 4612,
    TIMES_IN_A_BOAT    = 0,
};

enum Actions
{
    ACTION_INTRO_START,
    ACTION_BATTLE_EVENT,
    ACTION_MAGE_DIE,
    ACTION_MAGE_MISC_DIE,
    ACTION_SIEGE_DIE,
    ACTION_RANGED_DIE,
    ACTION_DONE,
    ACTION_FAIL,
    ACTION_MAGE_FIRST_SPAWN,
};

enum Spells
{
    SPELL_RANGED_TARGETING_A          = 70418,
    SPELL_RANGED_TARGETING_H          = 70419,
    SPELL_BURNING_PITCH_A             = 70403,
    SPELL_BURNING_PITCH_H             = 70397,
    SPELL_BURNING_PITCH_SIEGE_DMG_A   = 70383,
    SPELL_BURNING_PITCH_SIEGE_DMG_H   = 70374,
    SPELL_BURNING_PITCH_AOE_DAMAGE    = 69660,

    SPELL_WOUNDING_STRIKE             = 69651,

    SPELL_TELEPORT_VISUAL             = 64446,
    SPELL_BLADESTORM                  = 69652,
    SPELL_BLADESTORM_TRIGGER          = 69653,

    // Cannon
    SPELL_HEAT_DRAIN                  = 69470,
    SPELL_OVERHEAT                    = 69487, // Triggers spell #69488 every 0.25s. It should consume 10 Energy but does not.
    SPELL_CANNON_BLAST                = 69399,
    SPELL_INCINERATING_BLAST          = 69401,

    // Auras
    SPELL_ON_ORGRIMS_HAMMERS_DECK     = 70121,
    SPELL_ON_SKYBREAKERS_DECK         = 70120,
    SPELL_SAFE_AREA                   = 65705,

    // Rampart of Skulls NPCs Spells
    // Kor'kron Primalist
    SPELL_WRATH                       = 69968,
    SPELL_HEALING_TOUCH               = 69899,
    SPELL_REGROWTH                    = 69882,
    SPELL_REJUVENATION                = 69898,

    SPELL_SUMMON_BATTLE_STANDART_A    = 69810,
    SPELL_SUMMON_BATTLE_STANDART_H    = 69811,

    // Kor'kron Defender
    SPELL_DEVASTATE                   = 69902,
    SPELL_THUNDERCLAP                 = 69965,
    SPELL_SUNDER_ARMOR                = 57807,
    SPELL_SPELL_REFLECT               = 69901,

    // Skybreaker Vindicator
    SPELL_AVENGERS_SHIELD             = 69927,
    SPELL_CONCENTRATION               = 69930,
    SPELL_HOLY_WRATH                  = 69934,

    // Frostwyrm
    SPELL_FROST_BREATH                = 70116,
    SPELL_BLIZZARD                    = 70362,
    SPELL_FROST_CLEAVE                = 70361,

    // Muradin Bronzebeard / High Overlord Saurfang
    SPELL_CLEAVE                      = 15284,
    SPELL_RENDING_THROW               = 70309,
    SPELL_TASTE_OF_BLOOD              = 69634,
    SPELL_BATTLE_FURY                 = 69637,

    // Kor'kron Battle-mage & Skybreaker Sorcerer
    SPELL_BELOW_ZERO                  = 69705,
    SPELL_SHADOW_CHANNELING           = 43897,

    // Experience spells
    SPELL_EXPERIENCED                 = 71188,
    SPELL_VETERAN                     = 71193,
    SPELL_ELITE                       = 71195,
    SPELL_DESPERATE_RESOLVE           = 69647,

    // Kor'kron Axethrower & Skybreaker Rifleman
    SPELL_HURL_AXE                    = 70161,
    SPELL_SHOOT                       = 70162,

    // Kor'kron Rocketeer & Skybreaker Mortar Soldier
    SPELL_ROCKET_ARTILLERY_MARKER     = 71371,
    SPELL_ROCKET_ARTILLERY_TRIGGERED  = 69679,
    SPELL_ROCKET_ARTILLERY_HORDE      = 69678,
    SPELL_ROCKET_ARTILLERY_ALLIANCE   = 70609,
    SPELL_EXPLOSION                   = 69680,

    // Ship Explsion
    SPELL_SHIP_EXPLOSION              = 72137,

    // Remove Rocket Packs
    SPELL_REMOVE_ROCKET_PACK          = 70713,

    // Achievements
    SPELL_ACHIEVEMENT_CHECK           = 72959,
};

enum Events
{
    EVENT_NULL,
    EVENT_INTRO_ALLIANCE_1, // Muradin Bronzebeard yells: Fire up the engines! We got a meetin' with destiny, lads!
    EVENT_INTRO_ALLIANCE_2, // Muradin Bronzebeard yells: Hold on to yer hats!
    EVENT_INTRO_ALLIANCE_SUMMON_ORGRIMS_HAMMER,
    EVENT_INTRO_ALLIANCE_3, // Muradin Bronzebeard yells: What in the world is that? Grab me spyglass, crewman!
    EVENT_INTRO_ALLIANCE_4, // Muradin Bronzebeard yells: By me own beard! HORDE SAILIN' IN FAST 'N HOT!
    EVENT_INTRO_ALLIANCE_5, // Muradin Bronzebeard yells: EVASIVE ACTION! MAN THE GUNS!
    EVENT_INTRO_ALLIANCE_6, // Muradin Bronzebeard yells: Cowardly dogs! Ye blindsided us!
    EVENT_INTRO_ALLIANCE_7, // High Overlord Saurfang yells: This is not your battle, dwarf. Back down or we will be forced to destroy your ship.
    EVENT_INTRO_ALLIANCE_8, // Muradin Bronzebeard yells: Not me battle? I dunnae who ye think ye are, mister, but I got a score to settle with Arthas and yer not gettin' in me way! FIRE ALL GUNS! FIRE! FIRE!

    EVENT_INTRO_HORDE_1, // High Overlord Saurfang yells: Rise up, sons and daughters of the Horde! Today we battle a hated enemy of the Horde! LOK'TAR OGAR! Kor'kron, take us out!
    EVENT_INTRO_HORDE_1_1, // Kor'kron, take us out!
    EVENT_INTRO_HORDE_SUMMON_SKYBREAKER,
    EVENT_INTRO_HORDE_2, // High Overlord Saurfang yells: What is that?! Something approaching in the distance!
    EVENT_INTRO_HORDE_3, // High Overlord Saurfang yells: ALLIANCE GUNSHIP! ALL HANDS ON DECK!
    EVENT_INTRO_HORDE_4, // Muradin Bronzebeard yells: Move yer jalopy or we'll blow it out of the sky, orc! The Horde's got no business here!
    EVENT_INTRO_HORDE_5, // High Overlord Saurfang yells: You will know our business soon! KOR'KRON, ANNIHILATE THEM!

    EVENT_OUTRO_1,

    //Misc battle
    EVENT_SUMMON_PORTAL,
    EVENT_BOARDING_GUNSHIP,
    EVENT_BOARDING_TALK,
    EVENT_BURNING_PITCH,
    EVENT_WOUNDING_STRIKE,
    EVENT_BLADE_STORM,
    EVENT_FAIL,
    EVENT_RESTART_EVENT,
    EVENT_BOARDING_REAVERS_MARINE,
    EVENT_WIPE_CHECK,

    // Rampart of Skulls NPCs Events
    EVENT_WRATH,
    EVENT_HEAL,

    EVENT_SUNDER_ARMOR,
    EVENT_SPELL_REFLECT,
    EVENT_THUNDERCLAP,
    EVENT_DEVASTATE,

    EVENT_FROST_BREATH,
    EVENT_BLIZZARD,
    EVENT_CLEAVE,

    // Skybreaker Vindicator
    EVENT_AVENGERS_SHILED,
    EVENT_CONCENTRATION,
    EVENT_HOLY_WRATH,

    // First Squad Assisted
    EVENT_FIRST_SQUAD_ASSISTED_1,
    EVENT_FIRST_SQUAD_ASSISTED_2,

    // Second Squad Assisted
    EVENT_SECOND_SQUAD_ASSISTED_1,
    EVENT_SECOND_SQUAD_ASSISTED_2,
    EVENT_SECOND_SQUAD_ASSISTED_3,

    // Kor'kron Axethrower & Skybreaker Rifleman

    // Kor'kron Rocketeer & Skybreaker Mortar Soldier
    EVENT_ROCKET_ART,

    // Kor'kron Battle-Mage & Skybreaker Sorcerer
    EVENT_BELOW_ZERO,

    // Muradin Bronzebeard & High Overlord Saurfang
    EVENT_RENDING_THROW,

    // Misc (used in various NPCs)
    EVENT_RESPAWN_MAGE,
    EVENT_RESPAWN_MISC_MAGE,
    EVENT_RESPAWN_AXES_RIFLEMEN,
    EVENT_RESPAWN_ROCKETEER,
};

enum Texts
{
    // Kor'kron Primalist
    SAY_FIRST_SQUAD_RESCUED_HORDE_0  = 0,

    // Kor'kron Invoker
    SAY_FIRST_SQUAD_RESCUED_HORDE_1  = 0,

    // Kor'kron Defender
    SAY_SECOND_SQUAD_RESCUED_HORDE_0 = 0,
    SAY_SECOND_SQUAD_RESCUED_HORDE_1 = 1,
    SAY_FROSTWYRM_SUMMON_0           = 2,
    SAY_FROSTWYRM_SUMMON_1           = 3,

    // Skybreaker Vindicator
    SAY_FIRST_SQUAD_RESCUED_ALLIANCE_0  = 0,

    // Skybreaker Sorcerer
    SAY_FIRST_SQUAD_RESCUED_ALLIANCE_1  = 0,

    // Skybreaker Protector
    SAY_SECOND_SQUAD_RESCUED_ALLIANCE_0 = 0,
    SAY_SECOND_SQUAD_RESCUED_ALLIANCE_1 = 1,
    SAY_SECOND_SQUAD_RESCUED_ALLIANCE_2 = 2,

    // Muradin Bronzebeard
    SAY_INTRO_ALLIANCE_0             = 0,
    SAY_INTRO_ALLIANCE_1             = 1,
    SAY_INTRO_ALLIANCE_2             = 2,
    SAY_INTRO_ALLIANCE_3             = 3,
    SAY_INTRO_ALLIANCE_4             = 4,
    SAY_INTRO_ALLIANCE_5             = 5,
    SAY_INTRO_ALLIANCE_7             = 6,
    SAY_INTRO_HORDE_3                = 7,
    SAY_BOARDING_SKYBREAKER_1        = 8,
    SAY_BOARDING_ORGRIMS_HAMMER_0    = 9,
    SAY_NEW_RIFLEMEN_SPAWNED         = 10,
    SAY_NEW_MORTAR_TEAM_SPAWNED      = 11,
    SAY_NEW_MAGE_SPAWNED             = 12,
    SAY_ALLIANCE_VICTORY             = 13,
    SAY_ALLIANCE_DEFEAT              = 14, // How will we handle that case ? Ie. the player loses

    // High Overlord Saurfang
    SAY_INTRO_HORDE_0                = 0,
    SAY_INTRO_HORDE_0_1              = 1,
    SAY_INTRO_HORDE_1                = 2,
    SAY_INTRO_HORDE_2                = 3,
    SAY_INTRO_HORDE_4                = 4,
    SAY_BOARDING_SKYBREAKER_0        = 5,
    SAY_BOARDING_ORGRIMS_HAMMER_1    = 6,
    SAY_NEW_AXETHROWER_SPAWNED       = 7,
    SAY_NEW_ROCKETEERS_SPAWNED       = 8,
    SAY_NEW_BATTLE_MAGE_SPAWNED      = 9,
    SAY_HORDE_VICTORY                = 10,
    SAY_HORDE_DEFEAT                 = 11, // How will we handle that case ? Ie. the player loses
    SAY_INTRO_ALLIANCE_6             = 12,

    // Alliance Cannon & Horde Cannon
    SAY_OVERHEAT_ALERT               = 0,
};

enum MiscData
{
    ITEM_GOBLIN_ROCKET_PACK    = 49278,
    SPELL_CREATE_ROCKET_PACK   = 70055,

    PHASE_COMBAT               = 0,
    PHASE_INTRO                = 1,

    MUSIC_ENCOUNTER            = 17289
};

enum Points
{
    POINT_OUTRO_MIDWAY,
    POINT_OUTRO_DESPAWN,
    POINT_UPDATE_STOP,
    POINT_DEFENSE_APPROACH,
    POINT_DEFENSE_FINAL,
};

Position const FrostWyrmPosH   = {-435.429f, 2077.556f, 219.1148f, 4.767166f};
Position const FrostWyrmPosA   = {-437.409f, 2349.026f, 219.1148f, 1.483120f};

G3D::Vector3 const MuradinExitPath[] =
{
    {   8.130936f, -0.26995850f, 20.31728f },
    {   6.380936f, -0.26995850f, 20.31728f },
    {   3.507703f,  0.02986573f, 20.78463f },
    {  -2.767633f,  3.74314300f, 20.37663f },
    {  -4.017633f,  4.49314300f, 20.12663f },
    {  -7.242224f,  6.85601300f, 20.03468f },
    {  -7.742224f,  8.60601300f, 20.78468f },
    {  -7.992224f,  9.85601300f, 21.28468f },
    { -12.242220f, 23.10601000f, 21.28468f },
    { -14.884770f, 25.20844000f, 21.59985f },
};

G3D::Vector3 const SaurfangExitPath[] =
{
    { 30.43987f,   0.1475817f, 36.10674f },
    { 21.36141f,  -3.0564580f, 35.42970f },
    { 19.11141f,  -3.8064580f, 35.42970f },
    { 19.01736f,  -3.2994400f, 35.39428f },
    { 18.67470f,  -5.8628230f, 35.66611f },
    { 18.67470f,  -7.8628230f, 35.66611f },
    { 18.17470f, -17.3628200f, 35.66611f },
    { 18.17470f, -22.6128200f, 35.66611f },
    { 17.92470f, -24.3628200f, 35.41611f },
    { 17.92470f, -26.6128200f, 35.66611f },
    { 17.92470f, -27.8628200f, 35.66611f },
    { 17.92470f, -29.3628200f, 35.66611f },
    { 15.33203f, -30.4262100f, 35.93796f },
};

struct npc_gunship_spawn
{
    virtual void SetFinalPosition(float x, float y, float z, float o) { }
    virtual void StartRanks() = 0;
};

class npc_gunship_boss : public CreatureScript
{
    public:
        npc_gunship_boss() : CreatureScript("npc_gunship_boss") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance || instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE || instance->GetData(DATA_TEAM_IN_INSTANCE) != (creature->GetEntry() == NPC_GB_MURADIN_BRONZEBEARD ? ALLIANCE : HORDE))
                return true;

            if ((player->GetGroup() && !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
            {
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                return true;
            }

            return false;
        }

        struct npc_gunship_bossAI : public ScriptedAI
        {
            npc_gunship_bossAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            void InitializeAI() override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->SetFaction(AH(1802, 1801));
                me->m_SightDistance = 50.0f;
                me->m_CombatDistance = 50.0f;

                if (IsAlliance())
                    SetEquipmentSlots(false, 49775, 49774, EQUIP_NO_CHANGE);
                else
                    SetEquipmentSlots(false, 49773, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);

                MeleeCount = RAID_MODE(3, 5, 4, 6);

                events.Reset();
            }

            void AttackStart(Unit* victim) override
            {
                ScriptedAI::AttackStart(victim);
                evading = false;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->CastStop();
                me->AttackStop();
                me->StopMoving();
                if (!evading)
                {
                    evadeFacing = me->GetOrientation();
                    evading = true;
                    if (Creature* ship = MyShip())
                    {
                        if (me->GetExactDist2d(ship) > 50.0f || fabs(me->GetTransOffsetY()) > AH(25.0f, 30.0f))
                        {
                            Position dest = IsAlliance()
                                ? (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? Position { 13.51547f, -0.160213f, 20.87252f, 3.106686f } : Position { 1.344813f, -0.077413f, 20.84917f, 3.159046f })
                                : (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE ? Position { 36.40547f, 0.184604f, 36.71532f, 3.106686f } : Position { 36.4055f, 0.184604f, 36.7153f, 3.10669f });
                            float m_orientation = dest.GetOrientation();
                            MyTransport()->CalculatePassengerPosition(dest.m_positionX, dest.m_positionY, dest.m_positionZ, &m_orientation);
                            me->GetMotionMaster()->MovePoint(POINT_UPDATE_STOP, dest);
                            return;
                        }
                    }

                    me->GetMotionMaster()->MovePoint(POINT_UPDATE_STOP, *me);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                }
            }

            bool CanAIAttack(Unit const* target) const override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return false;

                if (target->GetEntry() == AH(NPC_GB_KORKRON_SERGEANT, NPC_GB_SKYBREAKER_SERGEANT) ||
                    target->GetEntry() == AH(NPC_GB_KORKRON_REAVERS, NPC_GB_SKYBREAKER_MARINE) ||
                    target->HasAura(AH(SPELL_ON_SKYBREAKERS_DECK, SPELL_ON_ORGRIMS_HAMMERS_DECK)))
                    return true;

                return false;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_INTRO_START:
                        ChangeTransportActiveState(MyTransport(), true);
                        ChangeTransportActiveState(OtherTransport(), true);
                        MyTransport()->EnableMovement(true);
                        if (IsAlliance())
                        {
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_1, 5000);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_2, 10000);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_SUMMON_ORGRIMS_HAMMER, 28000);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_3, 33000);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_4, 39000);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_5, 45000);
                        }
                        else
                        {
                            events.ScheduleEvent(EVENT_INTRO_HORDE_1, 5000);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_1_1, 16000);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_SUMMON_SKYBREAKER, 24600);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_2, 29600);
                        }
                        break;
                    case ACTION_ENEMY_GUNSHIP_TALK:
                        if (IsAlliance())
                        {
                            if (Transport* other = OtherTransport())
                            {
                                other->SummonPassenger(NPC_GB_KORKRON_BATTLE_MAGE,    {  47.29290000f,  -4.3089410f, 37.55550f, 1.570796f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 1);
                                other->SummonPassenger(NPC_GB_KORKRON_BATTLE_MAGE,    {  47.34621000f,   4.0320040f, 37.70952f, 4.817109f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 2);
                              //other->SummonPassenger(NPC_GB_KORKRON_BATTLE_MAGE,    {  13.58548000f,   0.3867192f, 34.99243f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 3);

                                other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER,     { -12.09280000f,  27.6594200f, 33.58557f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 1);
                                other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER,     {  -3.17055500f,  28.3065200f, 34.21082f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 2);
                                other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER,     {  14.92804000f,  26.1801800f, 35.47803f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 3);
                                other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER,     {  24.70331000f,  25.3658400f, 35.97845f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 4);
                                other->SummonPassenger(NPC_GB_KORKRON_ROCKETEER,      { -11.44849000f, -25.7183800f, 33.64343f, 1.518436f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 1);
                                other->SummonPassenger(NPC_GB_KORKRON_ROCKETEER,      {  12.30336000f, -25.6965300f, 35.32373f, 1.518436f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 2);
                                if (Is25ManRaid())
                                {
                                    other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER, { -16.65302000f,  27.5966800f, 33.18726f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 5);
                                    other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER, {  -8.08457200f,  28.2144800f, 33.93805f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 6);
                                    other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER, {   7.59476500f,  27.4196800f, 35.00775f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 7);
                                    other->SummonPassenger(NPC_GB_KORKRON_AXETHROWER, {  20.76339000f,  25.5821500f, 35.75287f, 1.535890f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 8);
                                    other->SummonPassenger(NPC_GB_KORKRON_ROCKETEER,  {  -0.05931854f, -25.4639900f, 34.50592f, 1.518436f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 3);
                                    other->SummonPassenger(NPC_GB_KORKRON_ROCKETEER,  {  27.62149000f, -23.4810800f, 36.12708f, 1.518436f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 4);
                                }
                            }
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_6, 0);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_7, 5000);
                            events.ScheduleEvent(EVENT_INTRO_ALLIANCE_8, 11000);
                        }
                        else
                        {
                            if (Transport* other = OtherTransport())
                            {
                                other->SummonPassenger(NPC_GB_SKYBREAKER_SORCERERS,          {   6.3859250f,   4.97900400f, 20.55417f, 4.694936f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 1);
                                other->SummonPassenger(NPC_GB_SKYBREAKER_SORCERERS,          {   6.5790410f,  -4.67480500f, 20.55060f, 1.553343f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 2);
                              //other->SummonPassenger(NPC_GB_SKYBREAKER_SORCERERS,          {  -9.4798580f,   0.05663967f, 20.77026f, 4.729842f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 3);

                                other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,           {  -0.3365479f, -18.87183000f, 20.56824f, 4.712389f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 1);
                                other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,           {  -9.1193850f, -18.79102000f, 20.58887f, 4.712389f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 2);
                                other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,           { -18.0172100f, -18.82056000f, 20.79150f, 4.747295f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 3);
                                other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,           { -29.5639000f, -17.95801000f, 20.73837f, 4.747295f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 4);
                                other->SummonPassenger(NPC_GB_SKYBREAKER_MORTAR_SOLDIER,     {  -9.3687130f,  18.75806000f, 20.65335f, 4.712389f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 1);
                                other->SummonPassenger(NPC_GB_SKYBREAKER_MORTAR_SOLDIER,     { -31.7014200f,  18.02783000f, 20.77197f, 4.712389f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 2);
                                if (Is25ManRaid())
                                {
                                    other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,       {  -4.3350220f, -18.84619000f, 20.58234f, 4.712389f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 5);
                                    other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,       { -13.6027800f, -18.74268000f, 20.59622f, 4.712389f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 6);
                                    other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,       { -23.5620100f, -18.28564000f, 20.67859f, 4.729842f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 7);
                                    other->SummonPassenger(NPC_GB_SKYBREAKER_RIFLEMAN,       { -34.7058100f, -17.67261000f, 20.51523f, 4.729842f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 8);
                                    other->SummonPassenger(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, {   0.1584473f,  18.11499000f, 20.41949f, 4.729842f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 3);
                                    other->SummonPassenger(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, { -20.4085100f,  18.40381000f, 20.50647f, 4.694936f }, TEMPSUMMON_MANUAL_DESPAWN)->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, 4);
                                }
                            }
                            events.ScheduleEvent(EVENT_INTRO_HORDE_3, 0);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_4, 4000);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_5, 11000);
                        }
                        DoAction(ACTION_BATTLE_EVENT);
                        me->GetMap()->SetZoneMusic(AREA_ICECROWN_CITADEL, MUSIC_ENCOUNTER);
                        break;
                    case ACTION_BATTLE_EVENT:
                    {
                        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == AH(ALLIANCE, HORDE))
                            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);

                        Creature* alliance = AllianceShip();
                        Creature* horde = HordeShip();
                        if (alliance && horde)
                        {
                             me->SetReactState(REACT_AGGRESSIVE);
                             me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                             horde->SetInCombatWith(alliance);
                             alliance->SetInCombatWith(horde);
                             horde->AddThreat(alliance, 0.0f);
                             alliance->AddThreat(horde, 0.0f);
                             DoZoneInCombat(OtherBoss(), 200.0f);

                             for (auto&& passenger : OtherTransport()->GetStaticPassengers())
                             {
                                 if (passenger->GetTypeId() != TYPEID_UNIT)
                                     continue;

                                 switch (passenger->GetEntry())
                                 {
                                     case NPC_GB_SKYBREAKER_RIFLEMAN:
                                     case NPC_GB_SKYBREAKER_MORTAR_SOLDIER:
                                     case NPC_GB_SKYBREAKER_SORCERERS:
                                     case NPC_GB_KORKRON_AXETHROWER:
                                     case NPC_GB_KORKRON_ROCKETEER:
                                     case NPC_GB_KORKRON_BATTLE_MAGE:
                                        if (npc_gunship_spawn* ai = dynamic_cast<npc_gunship_spawn*>(passenger->ToCreature()->AI()))
                                            ai->StartRanks();
                                        break;
                                 }
                             }

                             instance->SetBossState(DATA_GUNSHIP_EVENT, IN_PROGRESS);

                             events.ScheduleEvent(EVENT_SUMMON_PORTAL, 30000);
                             OtherBoss()->AI()->DoAction(ACTION_MAGE_FIRST_SPAWN);
                             OtherBoss()->CastSpell(OtherBoss(), SPELL_BATTLE_FURY, true);
                        }
                        else
                            DoAction(ACTION_FAIL);

                        break;
                    }
                    case ACTION_DONE:
                        if (Creature* alliance = AllianceShip())
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, alliance);
                        if (Creature* horde = HordeShip())
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, horde);

                        me->SetReactState(REACT_PASSIVE);

                        instance->SetBossState(DATA_GUNSHIP_EVENT, DONE);
                        if (Creature* zafod = me->FindNearestCreature(NPC_GB_ZAFOD_BOOMBOX, 500.0f, true))
                            zafod->CastSpell(zafod, SPELL_REMOVE_ROCKET_PACK, true);

                        me->GetMap()->SetZoneMusic(AREA_ICECROWN_CITADEL, 0);

                        Talk(AH(SAY_ALLIANCE_VICTORY, SAY_HORDE_VICTORY));

                        OtherTransport()->EnableMovement(true);
                        StopFight(MyTransport(), OtherTransport());

                        events.CancelEvent(EVENT_WIPE_CHECK);
                        events.CancelEvent(EVENT_SUMMON_PORTAL);
                        events.CancelEvent(EVENT_BOARDING_TALK);
                        events.CancelEvent(EVENT_BOARDING_GUNSHIP);
                        events.CancelEvent(EVENT_BOARDING_REAVERS_MARINE);
                        events.CancelEvent(EVENT_RENDING_THROW);
                        events.CancelEvent(EVENT_RESPAWN_MAGE);
                        events.CancelEvent(EVENT_RESPAWN_MISC_MAGE);
                        events.CancelEvent(EVENT_RESPAWN_ROCKETEER);
                        events.CancelEvent(EVENT_RESPAWN_AXES_RIFLEMEN);
                        events.ScheduleEvent(EVENT_OUTRO_1, 3500);
                        break;
                    case ACTION_FAIL:
                        if (Creature* alliance = AllianceShip())
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, alliance);
                        if (Creature* horde = HordeShip())
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, horde);

                        instance->SetBossState(DATA_GUNSHIP_EVENT, NOT_STARTED);
                        if (Creature* zafod = me->FindNearestCreature(NPC_GB_ZAFOD_BOOMBOX, 500.0f, true))
                            zafod->CastSpell(zafod, SPELL_REMOVE_ROCKET_PACK, true);

                        me->GetMap()->SetZoneMusic(AREA_ICECROWN_CITADEL, 0);

                        Talk(AH(SAY_ALLIANCE_DEFEAT, SAY_HORDE_DEFEAT));

                        StopFight(MyTransport(), OtherTransport());

                        events.CancelEvent(EVENT_WIPE_CHECK);
                        events.CancelEvent(EVENT_SUMMON_PORTAL);
                        events.CancelEvent(EVENT_BOARDING_TALK);
                        events.CancelEvent(EVENT_BOARDING_GUNSHIP);
                        events.CancelEvent(EVENT_BOARDING_REAVERS_MARINE);
                        events.CancelEvent(EVENT_RENDING_THROW);
                        events.CancelEvent(EVENT_RESPAWN_MAGE);
                        events.CancelEvent(EVENT_RESPAWN_MISC_MAGE);
                        events.CancelEvent(EVENT_RESPAWN_ROCKETEER);
                        events.CancelEvent(EVENT_RESPAWN_AXES_RIFLEMEN);
                        events.ScheduleEvent(EVENT_FAIL, 10000);
                        break;
                    case ACTION_EXIT_SHIP:
                    {
                        ChangeTransportActiveState(MyTransport(), false);
                        ChangeTransportActiveState(OtherTransport(), false);

                        //Movement::MoveSplineInit init(me);
                        //init.DisableTransportPathTransformations();
                        //init.MovebyPath(IsAlliance() ? Movement::PointsArray { std::begin(MuradinExitPath), std::end(MuradinExitPath) } : Movement::PointsArray { std::begin(SaurfangExitPath), std::end(SaurfangExitPath) });
                        //init.Launch();

                        //me->DespawnOrUnsummon(18000);

                        me->SetWalk(true);

                        G3D::Vector3 dest = AH(MuradinExitPath[2], SaurfangExitPath[4]);
                        MyTransport()->CalculatePassengerPosition(dest.x, dest.y, dest.z);
                        me->GetMotionMaster()->MovePoint(POINT_OUTRO_MIDWAY, dest.x, dest.y, dest.z);
                        break;
                    }
                    case ACTION_MAGE_DIE:
                    case ACTION_MAGE_FIRST_SPAWN:
                        if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                            events.ScheduleEvent(EVENT_RESPAWN_MAGE, action == ACTION_MAGE_FIRST_SPAWN ? 30000 : 35000);
                        break;
                    case ACTION_MAGE_MISC_DIE:
                        if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                            if (!events.IsScheduled(EVENT_RESPAWN_MISC_MAGE))
                                events.ScheduleEvent(EVENT_RESPAWN_MISC_MAGE, 30000);
                        break;
                    case ACTION_SIEGE_DIE:
                        if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                            if (!events.IsScheduled(EVENT_RESPAWN_ROCKETEER))
                                events.ScheduleEvent(EVENT_RESPAWN_ROCKETEER, 30000);
                        break;
                    case ACTION_RANGED_DIE:
                        if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                            if (!events.IsScheduled(EVENT_RESPAWN_AXES_RIFLEMEN))
                                events.ScheduleEvent(EVENT_RESPAWN_AXES_RIFLEMEN, 60000);
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HealthBelowPctDamaged(2, damage))
                {
                    damage = me->GetHealth() - 1;
                    if (instance->GetData(DATA_TEAM_IN_INSTANCE) == AH(ALLIANCE, HORDE))
                        DoAction(ACTION_FAIL);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == POINT_OUTRO_MIDWAY)
                {
                    G3D::Vector3 dest = AH(MuradinExitPath[9], SaurfangExitPath[11]);
                    MyTransport()->CalculatePassengerPosition(dest.x, dest.y, dest.z);
                    me->GetMotionMaster()->MovePoint(POINT_OUTRO_DESPAWN, dest.x, dest.y, dest.z);
                }
                else if (pointId == POINT_OUTRO_DESPAWN)
                {
                    me->Relocate(-555.59f, 2213.01f, 539.28f); // In case of any issues related to transport position update
                    me->SetPhaseMask(PHASEMASK_NORMAL | 64, false);
                    std::list<Creature*> saurfangEventNPCs;
                    GetCreatureListWithEntryInGrid(saurfangEventNPCs, me, NPC_SE_HIGH_OVERLORD_SAURFANG, 100.0f);
                    GetCreatureListWithEntryInGrid(saurfangEventNPCs, me, NPC_SE_MURADIN_BRONZEBEARD, 100.0f);
                    GetCreatureListWithEntryInGrid(saurfangEventNPCs, me, NPC_SE_KOR_KRON_REAVER, 100.0f);
                    GetCreatureListWithEntryInGrid(saurfangEventNPCs, me, NPC_SE_SKYBREAKER_MARINE, 100.0f);
                    for (auto&& npc : saurfangEventNPCs)
                        npc->SetPhaseMask(PHASEMASK_NORMAL, true);
                    me->SetPhaseMask(PHASEMASK_NORMAL, false);

                    if (Creature* boss = OtherBoss())
                        boss->AddObjectToRemoveList();
                    if (Creature* ship = OtherShip())
                        ship->AddObjectToRemoveList();
                    if (Creature* ship = MyShip())
                        ship->AddObjectToRemoveList();

                    me->AddObjectToRemoveList();
                }
                else if (pointId == POINT_UPDATE_STOP)
                    me->SetFacingTo(evadeFacing);
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                {
                    if (HealthBelowPct(75) && !me->HasAura(SPELL_TASTE_OF_BLOOD))
                        DoCast(me, SPELL_TASTE_OF_BLOOD, true);

                    if (UpdateVictim())
                    {
                        // A piece of code in Unit::SelectVictim will prevent the boss from evading if there still are unattackable attackers attacking him. Here we retify this.
                        if (!me->SelectVictim())
                            EnterEvadeMode();
                        else if (!events.IsScheduled(EVENT_RENDING_THROW))
                            events.ScheduleEvent(EVENT_RENDING_THROW, 3000);
                    }
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Common
                        case EVENT_WIPE_CHECK:
                            DoCheckFallingPlayer(me);
                            if (DoWipeCheck(MyTransport(), OtherTransport()))
                                events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                            else
                                DoAction(ACTION_FAIL);
                            break;
                        case EVENT_FAIL:
                            TeleportPlayers(instance->instance, AH(ALLIANCE, HORDE));
                            events.ScheduleEvent(EVENT_RESTART_EVENT, 2000);
                            break;
                        case EVENT_RESTART_EVENT:
                            instance->SetBossState(DATA_GUNSHIP_EVENT, FAIL);
                            MyTransport()->AddObjectToRemoveList();
                            OtherTransport()->AddObjectToRemoveList();
                            return;

                        // Alliance intro
                        case EVENT_INTRO_ALLIANCE_1:
                            Talk(SAY_INTRO_ALLIANCE_0);
                            break;
                        case EVENT_INTRO_ALLIANCE_2:
                            Talk(SAY_INTRO_ALLIANCE_1);
                            break;
                        case EVENT_INTRO_ALLIANCE_SUMMON_ORGRIMS_HAMMER:
                            sTransportMgr->CreateTransport(GO_ORGRIM_S_HAMMER_ALLIANCE_ICC, 0, me->GetMap());
                            break;
                        case EVENT_INTRO_ALLIANCE_3:
                            Talk(SAY_INTRO_ALLIANCE_2);
                            break;
                        case EVENT_INTRO_ALLIANCE_4:
                            Talk(SAY_INTRO_ALLIANCE_3);
                            break;
                        case EVENT_INTRO_ALLIANCE_5:
                            Talk(SAY_INTRO_ALLIANCE_4);
                            break;
                        case EVENT_INTRO_ALLIANCE_6:
                            Talk(SAY_INTRO_ALLIANCE_5);
                            break;
                        case EVENT_INTRO_ALLIANCE_7:
                            OtherBoss()->AI()->Talk(SAY_INTRO_ALLIANCE_6);
                            break;
                        case EVENT_INTRO_ALLIANCE_8:
                            Talk(SAY_INTRO_ALLIANCE_7);
                            break;

                        // Horde intro
                        case EVENT_INTRO_HORDE_1:
                            Talk(SAY_INTRO_HORDE_0);
                            break;
                        case EVENT_INTRO_HORDE_1_1:
                            Talk(SAY_INTRO_HORDE_0_1);
                            break;
                        case EVENT_INTRO_HORDE_SUMMON_SKYBREAKER:
                            sTransportMgr->CreateTransport(GO_THE_SKYBREAKER_HORDE_ICC, 0, me->GetMap());
                            break;
                        case EVENT_INTRO_HORDE_2:
                            Talk(SAY_INTRO_HORDE_1);
                            break;
                        case EVENT_INTRO_HORDE_3:
                            Talk(SAY_INTRO_HORDE_2);
                            break;
                        case EVENT_INTRO_HORDE_4:
                            OtherBoss()->AI()->Talk(SAY_INTRO_HORDE_3);
                            break;
                        case EVENT_INTRO_HORDE_5:
                            Talk(SAY_INTRO_HORDE_4);
                            break;

                        // Boarding
                        case EVENT_SUMMON_PORTAL:
                            if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                            {
                                if (IsAlliance())
                                {
                                       MyTransport()->SummonPassenger(NPC_GB_TELEPORT_EXIT,   { -17.557380f, -0.090421f, 21.18366f, 0 }, TEMPSUMMON_MANUAL_DESPAWN);
                                    OtherTransport()->SummonPassenger(NPC_GB_TELEPORT_PORTAL, {  47.550990f, -0.101778f, 37.61111f, 0 }, TEMPSUMMON_MANUAL_DESPAWN);
                                }
                                else
                                {
                                       MyTransport()->SummonPassenger(NPC_GB_TELEPORT_EXIT,   {   7.461699f,  0.158853f, 35.72989f, 0 }, TEMPSUMMON_MANUAL_DESPAWN);
                                    OtherTransport()->SummonPassenger(NPC_GB_TELEPORT_PORTAL, {   6.666975f,  0.013001f, 20.87888f, 0 }, TEMPSUMMON_MANUAL_DESPAWN);
                                }

                                events.ScheduleEvent(EVENT_BOARDING_TALK, 10000);
                                events.ScheduleEvent(EVENT_BOARDING_GUNSHIP, 2500); // TODO: Fix the timers
                            }
                            break;
                        case EVENT_BOARDING_TALK:
                            Talk(AH(SAY_BOARDING_SKYBREAKER_1, SAY_BOARDING_ORGRIMS_HAMMER_1));
                            break;
                        case EVENT_BOARDING_GUNSHIP:
                        {
                            OtherBoss()->AI()->Talk(AH(SAY_BOARDING_SKYBREAKER_0, SAY_BOARDING_ORGRIMS_HAMMER_0));

                            MeleeSpawnedCount = 0;

                            Position spawnPos = IsAlliance() ? Position { -17.55738f, -0.090421f, 21.18366f, 0 } : Position { 7.461699f, 0.158853f, 35.72989f, 0 };
                            if (Creature* melee = MyTransport()->SummonPassenger(AH(NPC_GB_KORKRON_SERGEANT, NPC_GB_SKYBREAKER_SERGEANT), spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                                melee->CastSpell(melee, SPELL_TELEPORT_VISUAL, true);

                            events.ScheduleEvent(EVENT_BOARDING_REAVERS_MARINE, 3000);
                            events.ScheduleEvent(EVENT_SUMMON_PORTAL, 90000);
                            break;
                        }
                        case EVENT_BOARDING_REAVERS_MARINE:
                            if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                            {
                                if (MeleeSpawnedCount <= MeleeCount)
                                {
                                    Position spawnPos = IsAlliance() ? Position { -17.55738f, -0.090421f, 21.18366f, 0 } : Position { 7.461699f, 0.158853f, 35.72989f, 0 };
                                    if (Creature* melee = MyTransport()->SummonPassenger(AH(NPC_GB_KORKRON_REAVERS, NPC_GB_SKYBREAKER_MARINE), spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        melee->CastSpell(melee, SPELL_TELEPORT_VISUAL, true);
                                        events.ScheduleEvent(EVENT_BOARDING_REAVERS_MARINE, 21000 / MeleeCount);
                                        ++MeleeSpawnedCount;
                                    }
                                }
                            }
                            break;

                        // Boss combat
                        case EVENT_RENDING_THROW:
                            if (me->GetVictim())
                                DoCastVictim(SPELL_RENDING_THROW);
                            break;

                        // Defense respawn
                        case EVENT_RESPAWN_MAGE:
                        {
                            Talk(AH(SAY_NEW_MAGE_SPAWNED, SAY_NEW_BATTLE_MAGE_SPAWNED));
                            if (IsAlliance())
                            {
                                if (Creature* mage = SpawnDefense(NPC_GB_SKYBREAKER_SORCERERS, -9.479858f, 0.05663967f, 20.77026f, 4.729842f, 0))
                                    instance->SetData64(DATA_GB_BATTLE_MAGE, mage->GetGUID());
                            }
                            else
                            {
                                if (Creature* mage = SpawnDefense(NPC_GB_KORKRON_BATTLE_MAGE, 13.58548f, 0.3867192f, 34.99243f, 1.53589f, 0))
                                    instance->SetData64(DATA_GB_BATTLE_MAGE, mage->GetGUID());
                            }
                            break;
                        }
                        case EVENT_RESPAWN_MISC_MAGE:
                        {
                            if (IsAlliance())
                            {
                                SpawnDefense(NPC_GB_SKYBREAKER_SORCERERS, 6.385925f,  4.979004f, 20.55417f, 4.694936f, 1);
                                SpawnDefense(NPC_GB_SKYBREAKER_SORCERERS, 6.579041f, -4.674805f, 20.55060f, 1.553343f, 2);
                            }
                            else
                            {
                                SpawnDefense(NPC_GB_KORKRON_BATTLE_MAGE, 47.29290f, -4.308941f, 37.55550f, 1.570796f, 1);
                                SpawnDefense(NPC_GB_KORKRON_BATTLE_MAGE, 47.34621f,  4.032004f, 37.70952f, 4.817109f, 2);
                            }
                            break;
                        }
                        case EVENT_RESPAWN_ROCKETEER:
                            Talk(AH(SAY_NEW_MORTAR_TEAM_SPAWNED, SAY_NEW_ROCKETEERS_SPAWNED));
                            if (IsAlliance())
                            {
                                SpawnDefense(NPC_GB_SKYBREAKER_MORTAR_SOLDIER,  -9.368713f, 18.75806f, 20.65335f, 4.712389f, 1);
                                SpawnDefense(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -31.701420f, 18.02783f, 20.77197f, 4.712389f, 2);
                                if (Is25ManRaid())
                                {
                                    SpawnDefense(NPC_GB_SKYBREAKER_MORTAR_SOLDIER,   0.1584473f, 18.11499f, 20.41949f, 4.729842f, 3);
                                    SpawnDefense(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -20.4085100f, 18.40381f, 20.50647f, 4.694936f, 4);
                                }
                            }
                            else
                            {
                                SpawnDefense(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.518436f, 1);
                                SpawnDefense(NPC_GB_KORKRON_ROCKETEER,  12.30336f, -25.69653f, 35.32373f, 1.518436f, 2);
                                if (Is25ManRaid())
                                {
                                    SpawnDefense(NPC_GB_KORKRON_ROCKETEER, -0.05931854f, -25.46399f, 34.50592f, 1.518436f, 3);
                                    SpawnDefense(NPC_GB_KORKRON_ROCKETEER, 27.62149000f, -23.48108f, 36.12708f, 1.518436f, 4);
                                }
                            }
                            break;
                        case EVENT_RESPAWN_AXES_RIFLEMEN:
                            Talk(AH(SAY_NEW_RIFLEMEN_SPAWNED, SAY_NEW_AXETHROWER_SPAWNED));
                            if (IsAlliance())
                            {
                                SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN,  -0.3365479f, -18.87183f, 20.56824f, 4.712389f, 1);
                                SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN,  -9.1193850f, -18.79102f, 20.58887f, 4.712389f, 2);
                                SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN, -18.0172100f, -18.82056f, 20.79150f, 4.747295f, 3);
                                SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN, -29.5639000f, -17.95801f, 20.73837f, 4.747295f, 4);
                                if (Is25ManRaid())
                                {
                                    SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN,  -4.3350220f, -18.84619f, 20.58234f, 4.712389f, 5);
                                    SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN, -13.6027800f, -18.74268f, 20.59622f, 4.712389f, 6);
                                    SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN, -23.5620100f, -18.28564f, 20.67859f, 4.729842f, 7);
                                    SpawnDefense(NPC_GB_SKYBREAKER_RIFLEMAN, -34.7058100f, -17.67261f, 20.51523f, 4.729842f, 8);
                                }
                            }
                            else
                            {
                                SpawnDefense(NPC_GB_KORKRON_AXETHROWER, -12.092800f, 27.65942f, 33.58557f, 1.53589f, 1);
                                SpawnDefense(NPC_GB_KORKRON_AXETHROWER,  -3.170555f, 28.30652f, 34.21082f, 1.53589f, 2);
                                SpawnDefense(NPC_GB_KORKRON_AXETHROWER,  14.928040f, 26.18018f, 35.47803f, 1.53589f, 3);
                                SpawnDefense(NPC_GB_KORKRON_AXETHROWER,  24.703310f, 25.36584f, 35.97845f, 1.53589f, 4);
                                if (Is25ManRaid())
                                {
                                    SpawnDefense(NPC_GB_KORKRON_AXETHROWER, -16.653020f, 27.59668f, 33.18726f, 1.53589f, 5);
                                    SpawnDefense(NPC_GB_KORKRON_AXETHROWER,  -8.084572f, 28.21448f, 33.93805f, 1.53589f, 6);
                                    SpawnDefense(NPC_GB_KORKRON_AXETHROWER,   7.594765f, 27.41968f, 35.00775f, 1.53589f, 7);
                                    SpawnDefense(NPC_GB_KORKRON_AXETHROWER,  20.763390f, 25.58215f, 35.75287f, 1.53589f, 8);
                                }
                            }
                            break;

                        // Common outro
                        case EVENT_OUTRO_1:
                            instance->DoCompleteAchievement(RAID_MODE(IM_ON_A_BOAT_10, IM_ON_A_BOAT_25, IM_ON_A_BOAT_10, IM_ON_A_BOAT_25));
                            DoCastAOE(SPELL_ACHIEVEMENT_CHECK, true);

                            MyTransport()->EnableMovement(true);
                            OtherTransport()->EnableMovement(true);
                            break;
                    }
                }

                if (me->GetVictim() && !me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_CLEAVE);

                DoMeleeAttackIfReady();
            }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                player->CLOSE_GOSSIP_MENU();
                if (sender == AH(10875, 10954) && action == 0 && me->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                {
                    DoAction(ACTION_INTRO_START);
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
                return true;
            }

        private:
            EventMap events;
            InstanceScript* instance;

            bool evading = false;
            float evadeFacing = 0.0f;
            uint32 MeleeCount = 0;
            uint32 MeleeSpawnedCount = 0;

            inline bool IsAlliance() const { return me->GetEntry() == NPC_GB_MURADIN_BRONZEBEARD; }
            inline bool IsHorde() const { return me->GetEntry() == NPC_GB_HIGH_OVERLORD_SAURFANG; }
            inline Transport* MyTransport() const { return me->GetTransport(); }
            inline Transport* OtherTransport() const { if (Creature* boss = OtherBoss()) return boss->GetTransport(); return nullptr; }
            inline Transport* Skybreaker() const { return IsAlliance() ? MyTransport() : OtherTransport(); }
            inline Transport* OrgrimsHammer() const { return IsHorde() ? MyTransport() : OtherTransport(); }
            inline Creature* AllianceShip() const { return ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SKYBREAKER_BOSS)); }
            inline Creature* HordeShip() const { return ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ORGRIMMAR_HAMMER_BOSS)); }
            inline Creature* MyShip() const { return IsAlliance() ? AllianceShip() : HordeShip(); }
            inline Creature* OtherShip() const { return IsAlliance() ? HordeShip() : AllianceShip(); }
            inline Creature* AllianceBoss() const { return IsAlliance() ? me : ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GB_MURADIN_BRONZEBEARD)); }
            inline Creature* HordeBoss() const { return IsHorde() ? me : ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GB_HIGH_OVERLORD_SAURFANG)); }
            inline Creature* OtherBoss() const { return IsAlliance() ? HordeBoss() : AllianceBoss(); }

            template<class T>
            inline T AH(T const& alliance, T const& horde) const { return IsAlliance() ? alliance : horde; }

            Creature* SpawnDefense(uint32 entry, float x, float y, float z, float o, uint32 slot) const
            {
                if (slot)
                    for (auto&& passenger : MyTransport()->GetStaticPassengers())
                        if (passenger->GetTypeId() == TYPEID_UNIT && passenger->GetEntry() == entry && passenger->ToCreature()->IsAlive() && passenger->ToCreature()->AI()->GetData(DATA_GB_PASSENGER_SPAWN_SLOT) == slot)
                            return nullptr;

                Position spawnPos = IsAlliance() ? Position { 18.01219f, -0.004024724f, 20.50269f, 4.092516f } : Position { 54.739746f, 0.093245f, 38.138222f, 3.159349f };
                if (Creature* spawn = MyTransport()->SummonPassenger(entry, spawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    spawn->AI()->SetData(DATA_GB_PASSENGER_SPAWN_SLOT, slot);

                    if (entry != NPC_GB_SKYBREAKER_SORCERERS && entry != NPC_GB_KORKRON_BATTLE_MAGE)
                    {
                        if (npc_gunship_spawn* ai = dynamic_cast<npc_gunship_spawn*>(spawn->AI()))
                            ai->SetFinalPosition(x, y, z, o);

                        y += y > 0 ? -10.0f : 10.0f;
                    }

                    MyTransport()->CalculatePassengerPosition(x, y, z);
                    spawn->GetMotionMaster()->MovePoint(POINT_DEFENSE_APPROACH, x, y, z);
                    return spawn;
                }

                return nullptr;
            }

            void SendPacketToPlayers(WorldPacket const* data) const
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        if (Player* player = itr->GetSource())
                            player->GetSession()->SendPacket(data);
            }

            void TeleportPlayers(Map* map, uint64 TeamInInstance)
            {
                for (auto&& ref : map->GetPlayers())
                {
                    if (Player* player = ref.GetSource())
                    {
                        if (player->isDead() && !player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                            player->ResurrectPlayer(1.0f);

                        if (TeamInInstance == ALLIANCE)
                            player->TeleportTo(631, -437.311493f + frand(-5.0f, 5.0f), 2390.243896f + frand(-5.0f, 5.0f), 191.233246f, M_PI / 2);
                        else
                            player->TeleportTo(631, -437.118317f + frand(-5.0f, 5.0f), 2032.588135f + frand(-5.0f, 5.0f), 191.233917f, 3 * M_PI / 2);
                    }
                }
            }

            bool DoWipeCheck(Transport* t1, Transport* t2)
            {
                return std::any_of(t1->GetPassengers().begin(), t1->GetPassengers().end(), [](WorldObject* passenger) { return passenger->GetTypeId() == TYPEID_PLAYER && passenger->ToPlayer()->IsAlive(); })
                    || std::any_of(t2->GetPassengers().begin(), t2->GetPassengers().end(), [](WorldObject* passenger) { return passenger->GetTypeId() == TYPEID_PLAYER && passenger->ToPlayer()->IsAlive(); });
            }

            void DoCheckFallingPlayer(Creature* me)
            {
                for (auto&& ref : me->GetMap()->GetPlayers())
                {
                    if (Player* player = ref.GetSource())
                    {
                        if (player->GetPositionZ() < 420.0f && player->IsWithinDistInMap(me, 300.0f))
                            player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f, me->GetOrientation());
                    }
                }
            }

            void StopFight(Transport* t1, Transport* t2)
            {
                Map* map = t1->GetMap();

                for (auto itr = t1->GetStaticPassengers().begin(); itr != t1->GetStaticPassengers().end();)
                {
                    if (WorldObject* passenger = *itr)
                    {
                        ++itr;

                        Creature* npc = passenger->ToCreature();
                        if (!npc)
                            continue;

                        switch (npc->GetEntry())
                        {
                            case NPC_GB_SKYBREAKER:
                            case NPC_GB_ORGRIMS_HAMMER:
                            case NPC_GB_HIGH_OVERLORD_SAURFANG:
                            case NPC_GB_MURADIN_BRONZEBEARD:
                            case NPC_GB_SKYBREAKER_DECKHAND:
                            case NPC_GB_ORGRIMS_HAMMER_CREW:
                            case NPC_GB_SKY_REAVER_KORM_BLACKSCAR:
                            case NPC_GB_GUNSHIP_HULL:
                                break; // Do nothing with these
                            case NPC_GB_ALLIANCE_CANNON:
                            case NPC_GB_HORDE_CANNON:
                                if (Vehicle* veh = npc->GetVehicleKit())
                                    veh->RemoveAllPassengers();
                                npc->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                break;
                            case NPC_GB_ZAFOD_BOOMBOX:
                                npc->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                                break;
                            default:
                                if (Vehicle* veh = npc->GetVehicleKit())
                                    veh->RemoveAllPassengers();
                                npc->AddObjectToRemoveList();
                                break;
                        }
                        continue;
                    }
                    ++itr;
                }

                for (auto itr = t2->GetStaticPassengers().begin(); itr != t2->GetStaticPassengers().end();)
                {
                    if (WorldObject* passenger = *itr)
                    {
                        ++itr;

                        Creature* npc = passenger->ToCreature();
                        if (!npc)
                            continue;

                        switch (npc->GetEntry())
                        {
                            case NPC_GB_SKYBREAKER:
                            case NPC_GB_ORGRIMS_HAMMER:
                            case NPC_GB_GUNSHIP_HULL:
                                break; // Do nothing with these
                            case NPC_GB_HIGH_OVERLORD_SAURFANG:
                            case NPC_GB_MURADIN_BRONZEBEARD:
                                npc->Kill(npc); // Need to keep them in world, otherwise CheckUnfriendlyShip will fail
                                break;
                            default:
                                if (Vehicle* veh = npc->GetVehicleKit())
                                    veh->RemoveAllPassengers();
                                npc->AddObjectToRemoveList();
                                break;
                        }
                        continue;
                    }
                    ++itr;
                }

                for (auto&& ref : map->GetPlayers())
                {
                    if (Player* player = ref.GetSource())
                    {
                        player->CombatStop();
                        player->getHostileRefManager().deleteReferences();
                    }
                }
            }

            void ChangeTransportActiveState(Transport* transport, bool active)
            {
                if (!transport)
                    return;

                for (auto&& passenger : transport->GetStaticPassengers())
                    passenger->setActive(active);
            }

            void SummonGameObjectWild(Map* map, uint32 entry, float x, float y, float z, float o, uint32 respawnTime)
            {
                GameObject* go = new GameObject();
                if (!go->Create(map->GenerateLowGuid<HighGuid::GameObject>(), entry, map, 65535, x, y, z, o, { }, 255, GO_STATE_READY))
                {
                    delete go;
                    return;
                }
                go->SetSpellId(1); // Just to remove it from the world entirely when it despawns
                go->SetRespawnTime(respawnTime);

                if (go->IsInWorld())
                    go->UpdateObjectVisibility(true);
                else
                    instance->instance->AddToMap(go);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunship_bossAI>(creature);
        }
};

class npc_gunship : public CreatureScript
{
    public:
        npc_gunship() : CreatureScript("npc_gunship") { }

        struct npc_gunshipAI : public ScriptedAI
        {
            npc_gunshipAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetDisplayId(11686);
                me->SetArmor(0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(false);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (Transport* t = me->GetTransport())
                    for (auto&& passenger : t->GetStaticPassengers())
                        if (Creature* creature = passenger->ToCreature())
                            if (creature->GetEntry() == NPC_GB_GUNSHIP_HULL)
                                creature->CastSpell(creature, SPELL_SHIP_EXPLOSION, true);

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (Creature* pMuradin = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GB_MURADIN_BRONZEBEARD)))
                        pMuradin->AI()->DoAction(me->GetEntry() == NPC_GB_SKYBREAKER ? ACTION_FAIL : ACTION_DONE);
                }
                else if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                {
                    if (Creature* pSaurfang = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GB_HIGH_OVERLORD_SAURFANG)))
                        pSaurfang->AI()->DoAction(me->GetEntry() == NPC_GB_SKYBREAKER ? ACTION_DONE : ACTION_FAIL);
                }
            }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunshipAI>(creature);
        }
};

class npc_gunship_safe_area : public CreatureScript
{
    public:
        npc_gunship_safe_area() : CreatureScript("npc_gunship_safe_area") { }

        struct npc_gunship_safe_areaAI : public ScriptedAI
        {
            npc_gunship_safe_areaAI(Creature *creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                if (me->m_movementInfo.transport.pos.GetExactDist(22.17635f, -11.41252f, 34.99731f) < 20.0f)
                {
                    DoCast(me, SPELL_ON_ORGRIMS_HAMMERS_DECK, true);
                }
                if (me->m_movementInfo.transport.pos.GetExactDist(-19.90114f, -11.19758f, 33.48487f) < 20.0f)
                {
                    DoCast(me, SPELL_ON_ORGRIMS_HAMMERS_DECK, true);
                    DoCast(me, SPELL_SAFE_AREA, true);
                }
                if (me->m_movementInfo.transport.pos.GetExactDist(-28.27504f, 15.594590f, 20.53790f) < 20.0f)
                {
                    DoCast(me, SPELL_ON_SKYBREAKERS_DECK, true);
                }
                if (me->m_movementInfo.transport.pos.GetExactDist(39.44747f, 0.136515f, 25.23212f) < 20.0f)
                {
                    DoCast(me, SPELL_ON_SKYBREAKERS_DECK, true);
                    DoCast(me, SPELL_SAFE_AREA, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunship_safe_areaAI>(creature);
        }
};

void SetupRankups(Creature* me, uint32 intervalExperienced, uint32 intervalVeteran = 0, uint32 intervalElite = 0)
{
    me->m_Events.Schedule(intervalExperienced + irand(-1000, 1000), [me, intervalVeteran, intervalElite]()
    {
        if (!me->IsAlive())
            return;
        me->CastSpell(me, SPELL_EXPERIENCED, true);
        if (!intervalVeteran)
            return;
        me->m_Events.Schedule(intervalVeteran + irand(-1000, 1000), [me, intervalElite]()
        {
            if (!me->IsAlive())
                return;
            me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
            me->CastSpell(me, SPELL_VETERAN, true);
            if (!intervalElite)
                return;
            me->m_Events.Schedule(intervalElite + irand(-1000, 1000), [me]()
            {
                if (!me->IsAlive())
                    return;
                me->RemoveAurasDueToSpell(SPELL_VETERAN);
                me->CastSpell(me, SPELL_ELITE, true);
            });
        });
    });

}

/* Kro'kron Axethrower */
class npc_korkron_axethrower_rifleman : public CreatureScript
{
    public:
        npc_korkron_axethrower_rifleman() : CreatureScript("npc_korkron_axethrower_rifleman") { }

        struct npc_korkron_axethrower_riflemanAI : public ScriptedAI, public npc_gunship_spawn
        {
            npc_korkron_axethrower_riflemanAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                SetCombatMovement(false);
            }

            void SetFinalPosition(float x, float y, float z, float o) override
            {
                finalPosition.Relocate(x, y, z, o);
            }

            void StartRanks() override
            {
                SetupRankups(me, 40000, 40000, 40000);
            }

            void InitializeAI() override
            {
                if (me->GetEntry() == NPC_GB_SKYBREAKER_RIFLEMAN)
                    SetEquipmentSlots(false, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE, 47267);
                else
                    SetEquipmentSlots(false, 49691, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? DATA_GB_HIGH_OVERLORD_SAURFANG : DATA_GB_MURADIN_BRONZEBEARD)))
                    boss->AI()->DoAction(ACTION_RANGED_DIE);
                me->AddObjectToRemoveList();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == POINT_DEFENSE_APPROACH)
                {
                    if (Transport* transport = me->GetTransport())
                    {
                        float m_orientation = finalPosition.GetOrientation();
                        transport->CalculatePassengerPosition(finalPosition.m_positionX, finalPosition.m_positionY, finalPosition.m_positionZ, &m_orientation);
                    }
                    me->GetMotionMaster()->MovePoint(POINT_DEFENSE_FINAL, finalPosition);
                }
                else if (pointId == POINT_DEFENSE_FINAL)
                {
                    me->SetFacingTo(finalPosition.GetOrientation());
                    StartRanks();
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_GB_PASSENGER_SPAWN_SLOT)
                    slot = data;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_GB_PASSENGER_SPAWN_SLOT)
                    return slot;
                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoCastAOE(me->GetEntry() == NPC_GB_SKYBREAKER_RIFLEMAN ? SPELL_RANGED_TARGETING_A : SPELL_RANGED_TARGETING_H);
            }

        private:
            uint32 slot = 0;
            Position finalPosition;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_korkron_axethrower_riflemanAI>(creature);
        }
};

class npc_sergeant : public CreatureScript
{
    public:
        npc_sergeant() : CreatureScript("npc_sergeant") { }

        struct npc_sergeantAI : public ScriptedAI, public npc_gunship_spawn
        {
            npc_sergeantAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            void StartRanks() override
            {
                SetupRankups(me, 40000, 40000, RAID_MODE(0, 0, 0, 40000));
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, 49689, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_AGGRESSIVE);
                events.Reset();
                events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000)); // ~61 sec
                events.ScheduleEvent(EVENT_WOUNDING_STRIKE, 5000);
                events.ScheduleEvent(EVENT_BLADE_STORM, 9000);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HealthBelowPctDamaged(25, damage) && !desperated)
                {
                    desperated = true;
                    DoCast(me, SPELL_DESPERATE_RESOLVE, true);
                }
            }

            bool CanAIAttack(Unit const* target) const override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return false;
                if (target->GetTypeId() == TYPEID_UNIT)
                    return target->GetEntry() == (me->GetEntry() == NPC_GB_SKYBREAKER_SERGEANT ? NPC_GB_HIGH_OVERLORD_SAURFANG : NPC_GB_MURADIN_BRONZEBEARD);
                return target->HasAura(me->GetEntry() == NPC_GB_SKYBREAKER_SERGEANT ? SPELL_ON_ORGRIMS_HAMMERS_DECK : SPELL_ON_SKYBREAKERS_DECK);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->AddObjectToRemoveList();
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(me->GetEntry() == NPC_GB_SKYBREAKER_SERGEANT ? DATA_GB_HIGH_OVERLORD_SAURFANG : DATA_GB_MURADIN_BRONZEBEARD)))
                    me->GetThreatManager().modifyThreatPercent(boss, -100);

                UpdateVictim();

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BURNING_PITCH:
                            DoCastAOE(me->GetEntry() == NPC_GB_KORKRON_SERGEANT ? SPELL_BURNING_PITCH_H : SPELL_BURNING_PITCH_A);
                            events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec
                            break;
                        case EVENT_WOUNDING_STRIKE:
                            if (UpdateVictim())
                                DoCastVictim(SPELL_WOUNDING_STRIKE);
                            events.ScheduleEvent(EVENT_WOUNDING_STRIKE, urand(5000, 8000));
                            break;
                        case EVENT_BLADE_STORM:
                            DoCast(me, SPELL_BLADESTORM);
                            events.ScheduleEvent(EVENT_BLADE_STORM, urand(9000, 16000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool desperated = false;
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_sergeantAI>(creature);
        }
};

class npc_marine_or_reaver : public CreatureScript
{
    public:
        npc_marine_or_reaver() : CreatureScript("npc_marine_or_reaver") { }

        struct npc_marine_or_reaverAI : public ScriptedAI, public npc_gunship_spawn
        {
            npc_marine_or_reaverAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            void StartRanks() override
            {
                SetupRankups(me, 40000, 40000, RAID_MODE(0, 0, 0, 40000));
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, 49708, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_AGGRESSIVE);
                events.Reset();
                events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HealthBelowPctDamaged(25, damage) && !desperated)
                {
                    desperated = true;
                    DoCast(me, SPELL_DESPERATE_RESOLVE, true);
                }
            }

            bool CanAIAttack(Unit const* target) const override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return false;
                if (target->GetTypeId() == TYPEID_UNIT)
                    return target->GetEntry() == (me->GetEntry() == NPC_GB_SKYBREAKER_MARINE ? NPC_GB_HIGH_OVERLORD_SAURFANG : NPC_GB_MURADIN_BRONZEBEARD);
                return target->HasAura(me->GetEntry() == NPC_GB_SKYBREAKER_MARINE ? SPELL_ON_ORGRIMS_HAMMERS_DECK : SPELL_ON_SKYBREAKERS_DECK);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->AddObjectToRemoveList();
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(me->GetEntry() == NPC_GB_SKYBREAKER_MARINE ? DATA_GB_HIGH_OVERLORD_SAURFANG : DATA_GB_MURADIN_BRONZEBEARD)))
                    me->GetThreatManager().modifyThreatPercent(boss, -100);

                UpdateVictim();

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BURNING_PITCH:
                            DoCastAOE(me->GetEntry() == NPC_GB_KORKRON_REAVERS ? SPELL_BURNING_PITCH_H : SPELL_BURNING_PITCH_A);
                            events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool desperated = false;
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_marine_or_reaverAI>(creature);
        }
};

class npc_gunship_mage : public CreatureScript
{
    public:
        npc_gunship_mage() : CreatureScript("npc_gunship_mage") { }

        struct npc_gunship_mageAI : public ScriptedAI
        {
            npc_gunship_mageAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                SetCombatMovement(false);
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, 51887, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->SetReactState(REACT_PASSIVE);
                events.Reset();
                events.ScheduleEvent(EVENT_BELOW_ZERO, urand(10000, 15000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? DATA_GB_HIGH_OVERLORD_SAURFANG : DATA_GB_MURADIN_BRONZEBEARD)))
                    boss->AI()->DoAction(me->GetGUID() == instance->GetGuidData(DATA_GB_BATTLE_MAGE) ? ACTION_MAGE_DIE : ACTION_MAGE_MISC_DIE);
                me->AddObjectToRemoveList();
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_GB_PASSENGER_SPAWN_SLOT)
                    slot = data;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_GB_PASSENGER_SPAWN_SLOT)
                    return slot;
                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetGUID() == instance->GetGuidData(DATA_GB_BATTLE_MAGE))
                {
                    events.Update(diff);

                    if (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_BELOW_ZERO:
                            {
                                std::list<Creature*> cannons;
                                GetCreatureListWithEntryInGrid(cannons, me, instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? NPC_GB_ALLIANCE_CANNON : NPC_GB_HORDE_CANNON, 500.0f);
                                for (auto&& cannon : cannons)
                                    if (Vehicle* veh = cannon->GetVehicleKit())
                                        veh->RemoveAllPassengers();

                                DoCastAOE(SPELL_BELOW_ZERO);
                                events.ScheduleEvent(EVENT_BELOW_ZERO, urand(10000, 15000));
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                else if (!me->GetCurrentSpell(CURRENT_CHANNELED_SPELL) && !me->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_ACTIVE))
                    DoCast(me, SPELL_SHADOW_CHANNELING, true);
            }

        private:
            uint32 slot = 0;
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunship_mageAI>(creature);
        }
};

/* Gunship Cannon (Horde / Ally) */
class npc_gunship_cannon : public CreatureScript
{
    public:
        npc_gunship_cannon() : CreatureScript("npc_gunship_cannon") { }

        struct npc_gunship_cannonAI : public VehicleAI
        {
            npc_gunship_cannonAI(Creature *creature) : VehicleAI(creature), instance(creature->GetInstanceScript()) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                if (!me->HasAura(SPELL_HEAT_DRAIN))
                    DoCast(me, SPELL_HEAT_DRAIN, true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_BELOW_ZERO)
                    me->GetVehicleKit()->RemoveAllPassengers();
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (Creature* boss = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetGuidData(me->GetEntry() == NPC_GB_ALLIANCE_CANNON ? DATA_SKYBREAKER_BOSS : DATA_ORGRIMMAR_HAMMER_BOSS)))
                    attacker->DealDamage(boss, damage);
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (me->HasAura(SPELL_BELOW_ZERO)/* || instance->GetBossState(DATA_GUNSHIP_BATTLE_EVENT) == DONE*/)
                {
                    me->RemoveAurasByType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    if (Vehicle* veh = me->GetVehicleKit())
                        veh->RemoveAllPassengers();
                }
                else
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunship_cannonAI>(creature);
        }
};

class npc_mortar_soldier_or_rocketeer : public CreatureScript
{
    public:
        npc_mortar_soldier_or_rocketeer() : CreatureScript("npc_mortar_soldier_or_rocketeer") { }

        struct npc_mortar_soldier_or_rocketeerAI : public ScriptedAI, public npc_gunship_spawn
        {
            npc_mortar_soldier_or_rocketeerAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                SetCombatMovement(false);
            }

            void SetFinalPosition(float x, float y, float z, float o) override
            {
                finalPosition.Relocate(x, y, z, o);
            }

            void StartRanks() override
            {
                SetupRankups(me, 60000, 60000, 60000);
            }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                events.Reset();
                events.ScheduleEvent(EVENT_ROCKET_ART, urand(10000, 15000));   // ~12 sec
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? DATA_GB_HIGH_OVERLORD_SAURFANG : DATA_GB_MURADIN_BRONZEBEARD)))
                    boss->AI()->DoAction(ACTION_SIEGE_DIE);
                me->m_Events.Schedule(10000, [this]() { me->AddObjectToRemoveList(); });
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == POINT_DEFENSE_APPROACH)
                {
                    if (Transport* transport = me->GetTransport())
                    {
                        float m_orientation = finalPosition.GetOrientation();
                        transport->CalculatePassengerPosition(finalPosition.m_positionX, finalPosition.m_positionY, finalPosition.m_positionZ, &m_orientation);
                    }
                    me->GetMotionMaster()->MovePoint(POINT_DEFENSE_FINAL, finalPosition);
                }
                else if (pointId == POINT_DEFENSE_FINAL)
                {
                    me->SetFacingTo(finalPosition.GetOrientation());
                    StartRanks();
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_GB_PASSENGER_SPAWN_SLOT)
                    slot = data;
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_GB_PASSENGER_SPAWN_SLOT)
                    return slot;
                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                   return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ROCKET_ART:
                        {
                            DoCastAOE(me->GetEntry() == NPC_GB_KORKRON_ROCKETEER ? SPELL_ROCKET_ARTILLERY_HORDE : SPELL_ROCKET_ARTILLERY_ALLIANCE, true);
                            int32 castTime = me->GetCurrentSpellCastTime(SPELL_ROCKET_ARTILLERY_TRIGGERED);
                            events.ScheduleEvent(EVENT_ROCKET_ART, urand(3000, 4000) + (castTime ? castTime : 5000));
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

        private:
            uint32 slot;
            Position finalPosition;
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_mortar_soldier_or_rocketeerAI>(creature);
        }
};

class npc_zafod_boombox : public CreatureScript
{
    public:
        npc_zafod_boombox() : CreatureScript("npc_zafod_boombox") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance || instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                return true;

            return false;
        }

        struct npc_zafod_boomboxAI : public ScriptedAI
        {
            npc_zafod_boomboxAI(Creature* creature) : ScriptedAI(creature)
            {
                DoCastAOE(SPELL_REMOVE_ROCKET_PACK, true);
            }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                player->CLOSE_GOSSIP_MENU();
                if (sender == 10885 && action == 0 && !player->HasItemCount(ITEM_GOBLIN_ROCKET_PACK, 1, true))
                    player->AddItem(ITEM_GOBLIN_ROCKET_PACK, 1);
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_zafod_boomboxAI>(creature);
        }
};

class npc_gunship_portal : public CreatureScript
{
    public:
        npc_gunship_portal() : CreatureScript("npc_gunship_portal") { }

        struct npc_gunship_portalAI : public ScriptedAI
        {
            npc_gunship_portalAI(Creature *creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
               despawnTimer = 20500;
               me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->AddObjectToRemoveList();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!despawnTimer)
                    return;

                if (despawnTimer <= diff)
                {
                    despawnTimer = 0;
                    me->AddObjectToRemoveList();
                }
                else
                    despawnTimer -= diff;
            }

        private:
            uint32 despawnTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunship_portalAI>(creature);
        }
};

class npc_gunship_trigger : public CreatureScript
{
    public:
        npc_gunship_trigger() : CreatureScript("npc_gunship_trigger") { }

        struct npc_gunship_triggerAI : public ScriptedAI
        {
            npc_gunship_triggerAI(Creature *creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(false);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 diff) override { }

        private:
            EventMap events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_gunship_triggerAI>(creature);
        }
};

/* ----------------------------------- Rampart of Skulls NPCs ----------------------------------- */

/* Kor'kron Primalist  37030*/
class npc_korkron_primalist: public CreatureScript
{
    public:
        npc_korkron_primalist() : CreatureScript("npc_korkron_primalist") { }

        struct npc_korkron_primalistAI : public ScriptedAI
        {
            npc_korkron_primalistAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                _started = instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE;
                events.Reset();
                SetEquipmentSlots(false, 45613, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                events.ScheduleEvent(EVENT_WRATH, 10000); // TODO: Fix the timers
                events.ScheduleEvent(EVENT_HEAL, 20000); // TODO: Fix the timers
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) != HORDE)
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (instance->GetData(DATA_FIRST_SQUAD_STATE) == DONE)
                    return;

                if (!_started && me->IsWithinDistInMap(who, 25.0f) && me->GetDBTableGUIDLow() == 78395) // may be wrong
                {
                    _started = true;
                    events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_1, 1000);
                    events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_2, 15000); // TODO : fix the timer
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIRST_SQUAD_ASSISTED_1:
                            Talk(SAY_FIRST_SQUAD_RESCUED_HORDE_0);
                            break;
                        case EVENT_FIRST_SQUAD_ASSISTED_2:
                            if (Creature* tempUnit = me->FindNearestCreature(NPC_KORKRON_INVOKER, 120.0f, true))
                            {
                                tempUnit->AI()->Talk(SAY_FIRST_SQUAD_RESCUED_HORDE_1);
                                tempUnit->CastSpell(tempUnit, SPELL_SUMMON_BATTLE_STANDART_H, true);
                            }
                            instance->SetData(DATA_FIRST_SQUAD_STATE, DONE);
                            break;
                        case EVENT_WRATH:
                            if (UpdateVictim())
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                    DoCast(target, SPELL_WRATH);
                            events.ScheduleEvent(EVENT_WRATH, 10000);
                            break;
                        case EVENT_HEAL:
                            if (UpdateVictim())
                            {
                                std::list<Unit*> TargetList;
                                Unit* finalTarget = me;
                                Trinity::AnyFriendlyUnitInObjectRangeCheck checker(me, me, 30.0f);
                                Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(me, TargetList, checker);
                                me->VisitNearbyObject(30.0f, searcher);
                                for (std::list<Unit*>::iterator itr = TargetList.begin(); itr != TargetList.end(); ++itr)
                                    if ((*itr)->GetHealthPct() < finalTarget->GetHealthPct())
                                        finalTarget = *itr;

                                uint32 spellId = SPELL_HEALING_TOUCH;
                                uint32 healthPct = uint32(finalTarget->GetHealthPct());
                                if (healthPct > 15 && healthPct < 20)
                                    spellId = (urand (0, 1) ? SPELL_REGROWTH : SPELL_HEALING_TOUCH);
                                else if (healthPct >= 20 && healthPct < 40)
                                    spellId = SPELL_REGROWTH;
                                else if (healthPct > 40)
                                    spellId = (urand(0, 1) ? SPELL_REJUVENATION : SPELL_REGROWTH);

                                me->CastSpell(finalTarget, spellId, false);
                                events.ScheduleEvent(EVENT_HEAL, 20000);
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool _started;
                EventMap events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_korkron_primalistAI(creature);
        }
};

/* Kor'kron Defender  37032*/
class npc_korkron_defender: public CreatureScript
{
    public:
        npc_korkron_defender() : CreatureScript("npc_korkron_defender") { }

        struct npc_korkron_defenderAI : public ScriptedAI
        {
            npc_korkron_defenderAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                SetEquipmentSlots(false, 41816, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                events.ScheduleEvent(EVENT_DEVASTATE, 10000);
                events.ScheduleEvent(EVENT_THUNDERCLAP, 20000);
                events.ScheduleEvent(EVENT_SPELL_REFLECT, 25000);
                events.ScheduleEvent(EVENT_SUNDER_ARMOR, 15000);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) != HORDE)
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (me->IsWithinDistInMap(who, 25.0f))
                {
                    if (instance->GetData(DATA_FIRST_SQUAD_STATE) == DONE && instance->GetData(DATA_SECOND_SQUAD_STATE) != DONE)
                    {
                        instance->SetData(DATA_SECOND_SQUAD_STATE, DONE);
                        events.ScheduleEvent(EVENT_SECOND_SQUAD_ASSISTED_1, 1000);
                        events.ScheduleEvent(EVENT_SECOND_SQUAD_ASSISTED_2, 15000); // TODO : fix the timer
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SECOND_SQUAD_ASSISTED_1:
                            Talk(SAY_SECOND_SQUAD_RESCUED_HORDE_0);
                            break;
                        case EVENT_SECOND_SQUAD_ASSISTED_2:
                            Talk(SAY_SECOND_SQUAD_RESCUED_HORDE_1);
                            Talk(SAY_FROSTWYRM_SUMMON_0);
                            break;
                        case EVENT_DEVASTATE:
                            if (UpdateVictim())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                   DoCast(target, SPELL_DEVASTATE);
                            }
                            events.ScheduleEvent(EVENT_DEVASTATE, 10000);
                            break;
                        case EVENT_SPELL_REFLECT:
                            if (UpdateVictim())
                            {
                                DoCast(me, SPELL_SPELL_REFLECT);
                            }
                            events.ScheduleEvent(EVENT_SPELL_REFLECT, 25000);
                            break;
                        case EVENT_THUNDERCLAP:
                            if (UpdateVictim())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                    DoCast(target, SPELL_THUNDERCLAP);
                            }
                            events.ScheduleEvent(EVENT_THUNDERCLAP, 20000);
                            break;
                        case EVENT_SUNDER_ARMOR:
                            if (UpdateVictim())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                    DoCast(target, SPELL_SUNDER_ARMOR);
                            }
                            events.ScheduleEvent(EVENT_SUNDER_ARMOR, 15000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_korkron_defenderAI(creature);
        }
};

/* Skybreaker Vindicator  37003*/
class npc_skybreaker_vindicator: public CreatureScript
{
    public:
        npc_skybreaker_vindicator() : CreatureScript("npc_skybreaker_vindicator") { }

        struct npc_skybreaker_vindicatorAI : public ScriptedAI
        {
            npc_skybreaker_vindicatorAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                _started = instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE;
                events.Reset();
                SetEquipmentSlots(false, 49708, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                events.ScheduleEvent(EVENT_AVENGERS_SHILED, 10000); // TODO: Fix the timers
                events.ScheduleEvent(EVENT_CONCENTRATION, 20000); // TODO: Fix the timers
                events.ScheduleEvent(EVENT_HOLY_WRATH, 20000); // TODO: Fix the timers
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) != ALLIANCE)
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (instance->GetData(DATA_FIRST_SQUAD_STATE) == DONE)
                    return;

                if (!_started && me->IsWithinDistInMap(who, 25.0f) && me->GetDBTableGUIDLow() == 10711) // may be wrong
                {
                    _started = true;
                    events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_1, 1000);
                    events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_2, 15000); // TODO : fix the timer
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                   return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIRST_SQUAD_ASSISTED_1:
                            Talk(SAY_FIRST_SQUAD_RESCUED_ALLIANCE_0);
                            break;
                        case EVENT_FIRST_SQUAD_ASSISTED_2:
                            if (Creature* tempUnit = me->FindNearestCreature(NPC_SKYBREAKER_SORCERER, 120.0f, true))
                            {
                                tempUnit->AI()->Talk(SAY_FIRST_SQUAD_RESCUED_ALLIANCE_1);
                                tempUnit->CastSpell(tempUnit, SPELL_SUMMON_BATTLE_STANDART_A, true);
                            }
                            instance->SetData(DATA_FIRST_SQUAD_STATE, DONE);
                            break;
                        case EVENT_AVENGERS_SHILED:
                            if (UpdateVictim())
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                    DoCast(target, SPELL_WRATH);
                            events.ScheduleEvent(EVENT_AVENGERS_SHILED, 10000);
                            break;
                        case EVENT_CONCENTRATION:
                            if (UpdateVictim())
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                    DoCast(target, SPELL_CONCENTRATION);
                            events.ScheduleEvent(EVENT_CONCENTRATION, 20000);
                            break;
                        case EVENT_HOLY_WRATH:
                            if (UpdateVictim())
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                    DoCast(target, SPELL_HOLY_WRATH);
                            events.ScheduleEvent(EVENT_HOLY_WRATH, 15000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool _started;
                EventMap events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_skybreaker_vindicatorAI(creature);
        }
};

/* Skybreaker Protector  36998*/
class npc_skybreaker_protector: public CreatureScript
{
    public:
        npc_skybreaker_protector() : CreatureScript("npc_skybreaker_protector") { }

        struct npc_skybreaker_protectorAI : public ScriptedAI
        {
            npc_skybreaker_protectorAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                SetEquipmentSlots(false, 49708, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                events.ScheduleEvent(EVENT_DEVASTATE, 10000);
                events.ScheduleEvent(EVENT_THUNDERCLAP, 20000);
                events.ScheduleEvent(EVENT_SPELL_REFLECT, 25000);
                events.ScheduleEvent(EVENT_SUNDER_ARMOR, 15000);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) != ALLIANCE)
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (me->IsWithinDistInMap(who, 25.0f))
                {
                    if (instance->GetData(DATA_FIRST_SQUAD_STATE) == DONE && instance->GetData(DATA_SECOND_SQUAD_STATE) != DONE)
                    {
                        instance->SetData(DATA_SECOND_SQUAD_STATE, DONE);
                        events.ScheduleEvent(EVENT_SECOND_SQUAD_ASSISTED_1, 1000);
                        events.ScheduleEvent(EVENT_SECOND_SQUAD_ASSISTED_2, 11000); // TODO : fix the timer
                        events.ScheduleEvent(EVENT_SECOND_SQUAD_ASSISTED_3, 17000); // TODO : fix the timer
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SECOND_SQUAD_ASSISTED_1:
                            Talk(SAY_SECOND_SQUAD_RESCUED_ALLIANCE_0);
                            break;
                        case EVENT_SECOND_SQUAD_ASSISTED_2:
                            Talk(SAY_SECOND_SQUAD_RESCUED_ALLIANCE_1);
                            break;
                        case EVENT_SECOND_SQUAD_ASSISTED_3:
                            Talk(SAY_SECOND_SQUAD_RESCUED_ALLIANCE_2);
                            Talk(SAY_FROSTWYRM_SUMMON_1);
                            break;
                        case EVENT_DEVASTATE:
                            if (UpdateVictim())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                    DoCast(target, SPELL_DEVASTATE);
                            }
                            events.ScheduleEvent(EVENT_DEVASTATE, 10000);
                            break;
                        case EVENT_SPELL_REFLECT:
                            if (UpdateVictim())
                            {
                                DoCast(me, SPELL_SPELL_REFLECT);
                            }
                            events.ScheduleEvent(EVENT_SPELL_REFLECT, 25000);
                            break;
                        case EVENT_THUNDERCLAP:
                            if (UpdateVictim())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                    DoCast(target, SPELL_THUNDERCLAP);
                            }
                            events.ScheduleEvent(EVENT_THUNDERCLAP, 20000);
                            break;
                        case EVENT_SUNDER_ARMOR:
                            if (UpdateVictim())
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                    DoCast(target, SPELL_SUNDER_ARMOR);
                            }
                            events.ScheduleEvent(EVENT_SUNDER_ARMOR, 15000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
                InstanceScript* instance;
        };
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_skybreaker_protectorAI(creature);
        }
};

/* Spire Frostwyrm  37230*/
class npc_icc_spire_frostwyrm: public CreatureScript
{
    public:
        npc_icc_spire_frostwyrm() : CreatureScript("npc_icc_spire_frostwyrm") { }

        struct npc_icc_spire_frostwyrmAI : public ScriptedAI
        {
            npc_icc_spire_frostwyrmAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                landed = false;
                events.Reset();
                me->SetFlying(true);
                me->SetReactState(REACT_AGGRESSIVE);
                events.ScheduleEvent(EVENT_FROST_BREATH, 20000);
                events.ScheduleEvent(EVENT_BLIZZARD, 25000);
                events.ScheduleEvent(EVENT_CLEAVE, 10000);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance)
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!who->isTargetableForAttack())
                    return;

                if (!landed && me->IsWithinDistInMap(who, 35.0f))
                {
                    landed = true;
                    me->SetFlying(false);
                    me->SetInCombatWith(who);
                    me->AddThreat(who, 1.0f);
                    me->GetMotionMaster()->MoveChase(who);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROST_BREATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                DoCast(target, SPELL_FROST_BREATH);
                            events.ScheduleEvent(EVENT_FROST_BREATH, 20000);
                            break;
                        case EVENT_BLIZZARD:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                DoCast(target, SPELL_BLIZZARD);
                            events.ScheduleEvent(EVENT_BLIZZARD, 25000);
                            break;
                        case EVENT_CLEAVE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 10.0f))
                                DoCast(target, SPELL_FROST_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, 10000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool landed;
                EventMap events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_icc_spire_frostwyrmAI(creature);
        }
};

class transport_icc_gunship : public TransportScript
{
    public:
        transport_icc_gunship() : TransportScript("transport_icc_gunship") { }

        void OnCreate(Transport* transport) override
        {
            InstanceScript* instance = transport->GetInstanceScript();
            if (!instance || instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                return;

            auto&& keyframes = transport->GetTransportTemplate()->keyFrames;
            auto itr = std::find_if(keyframes.begin(), keyframes.end(), [](KeyFrame const& k) { return k.Node->ArrivalEventID == 22665; });
            if (itr == keyframes.end())
                return; // Enemy transports should return here

            transport->EnableMovement(false);
            transport->SetGoState(GO_STATE_READY);
            const_cast<GameObjectValue*>(transport->GetGOValue())->Transport.PathProgress = itr->ArriveTime;
        }

        void OnAddCreaturePassenger(Transport* transport, Creature* passenger) override
        {
            passenger->Respawn();
            passenger->setActive(false, ActiveFlags::OnTransport);
        }
};

/* ---------------------------------- AreaTrigger Scripts ------------------------------------- */
class at_icc_land_frostwyrm : public AreaTriggerScript
{
    public:
        at_icc_land_frostwyrm() : AreaTriggerScript("at_icc_land_frostwyrm") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_SPIRE_FROSTWYRM_STATE) == NOT_STARTED && instance->GetData(DATA_SECOND_SQUAD_STATE) == DONE)
                {
                    if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                        player->GetMap()->SummonCreature(NPC_SPIRE_FROSTWYRM, FrostWyrmPosA);
                    else
                        player->GetMap()->SummonCreature(NPC_SPIRE_FROSTWYRM, FrostWyrmPosH);

                    instance->SetData(DATA_SPIRE_FROSTWYRM_STATE, IN_PROGRESS);
                }
            }

            return true;
        }
};

/* Remove Rocket Pack - 70713 */
class spell_icc_remove_rocket_pack : public SpellScriptLoader
{
    public:
        spell_icc_remove_rocket_pack() : SpellScriptLoader("spell_icc_remove_rocket_pack") { }

        class spell_icc_remove_rocket_pack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_remove_rocket_pack_SpellScript);

            //bool Load() override
            //{
            //    return GetOwner()->GetInstanceScript();
            //}

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                Player* hitPlr = GetHitPlayer();
                if (!hitPlr) // If player is offline
                    return;

                int32 itemId = GetEffectValue();
                uint32 itemCount = hitPlr->GetItemCount(itemId, false); // Should be 1, but just in case.
                hitPlr->DestroyItemCount(itemId, itemCount, true, false);

              if (hitPlr->HasItemCount(ITEM_GOBLIN_ROCKET_PACK, 1, true))
                hitPlr->DestroyItemCount(ITEM_GOBLIN_ROCKET_PACK, 1, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_icc_remove_rocket_pack_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icc_remove_rocket_pack_SpellScript();
        }
};

/* spell 69471 Heat Drain */
class spell_gb_heat_drain : public SpellScriptLoader
{
    public:
        spell_gb_heat_drain() : SpellScriptLoader("spell_gb_heat_drain") { }

        class spell_gb_heat_drain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_heat_drain_SpellScript);

            SpellCastResult Overheat()
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetPower(POWER_ENERGY) >= 100 && caster->GetTypeId() == TYPEID_UNIT)
                    {
                        caster->CastSpell(caster, SPELL_OVERHEAT, true);
                        if (Player* owner = caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
                            owner->GetSpellHistory()->SendCooldown(caster->ToCreature()->m_spells[0], 3000);
                            owner->GetSpellHistory()->SendCooldown(caster->ToCreature()->m_spells[1], 3000);

                            caster->ToCreature()->AddCreatureSpellCooldown(caster->ToCreature()->m_spells[0]);
                            caster->ToCreature()->AddCreatureSpellCooldown(caster->ToCreature()->m_spells[1]);

                            caster->ToCreature()->AI()->Talk(SAY_OVERHEAT_ALERT, owner);
                        }
                        return SPELL_FAILED_CASTER_AURASTATE;
                    }
                }
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gb_heat_drain_SpellScript::Overheat);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_heat_drain_SpellScript();
        }
};

/* spell 69402 Incinerating Blast */
class spell_gb_incinerating_blast : public SpellScriptLoader
{
    public:
        spell_gb_incinerating_blast() : SpellScriptLoader("spell_gb_incinerating_blast") { }

        class spell_gb_incinerating_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_incinerating_blast_SpellScript);

            void AddExtraDamage()
            {
                Unit* caster = GetCaster();
                if (!caster || !caster->GetPower(POWER_ENERGY))
                    return;

                SetHitDamage(int32(GetHitDamage() + (caster->GetPower(POWER_ENERGY) * 100.0f))); // TODO: How much should be?
                caster->SetPower(POWER_ENERGY, 0);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_gb_incinerating_blast_SpellScript::AddExtraDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_incinerating_blast_SpellScript();
        }
};

/* spell 71335, 70339 Burning Pitch */
class spell_gb_burning_pitch : public SpellScriptLoader
{
    public:
        spell_gb_burning_pitch() : SpellScriptLoader("spell_gb_burning_pitch") { }

        class spell_gb_burning_pitch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_burning_pitch_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                caster->CastSpell(caster, GetEffectValue(), true);
                target->CastSpell(target, SPELL_BURNING_PITCH_AOE_DAMAGE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gb_burning_pitch_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_burning_pitch_SpellScript();
        }
};

/* spell 70397, 70403 Burning Pitch */
class spell_gb_burning_pitch_targeting : public SpellScriptLoader
{
    public:
        spell_gb_burning_pitch_targeting() : SpellScriptLoader("spell_gb_burning_pitch_targeting") { }

        class spell_gb_burning_pitch_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_burning_pitch_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                uint32 allowedTransportEntry = GetSpellInfo()->Id == 70403 ? GO_ORGRIM_S_HAMMER_HORDE_ICC : GO_THE_SKYBREAKER_ALLIANCE_ICC;
                targets.remove_if([allowedTransportEntry](WorldObject* obj)
                {
                    if (Transport* transport = obj->GetTransport())
                        return transport->GetEntry() != allowedTransportEntry;
                    return true;
                });
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        caster->CastSpell(target, GetEffectValue());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gb_burning_pitch_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_gb_burning_pitch_targeting_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_burning_pitch_targeting_SpellScript();
        }
};

/* spell 70418, 70419 Ranged Targeting */
class spell_gb_ranged_targeting : public SpellScriptLoader
{
    public:
        spell_gb_ranged_targeting() : SpellScriptLoader("spell_gb_ranged_targeting") { }

        class spell_gb_ranged_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_ranged_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(false, GetSpellInfo()->Effects[EFFECT_0].CalcValue()));
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                _found = true;
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_1].CalcValue());
            }

            void HandleAfterCast()
            {
                if (!_found)
                    if (Unit* caster = GetCaster())
                        caster->CastSpell(caster, GetSpellInfo()->Effects[EFFECT_2].CalcValue());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gb_ranged_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_gb_ranged_targeting_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterCast += SpellCastFn(spell_gb_ranged_targeting_SpellScript::HandleAfterCast);
            }

        private:
            bool _found = false;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_ranged_targeting_SpellScript();
        }
};

/* spell 70222, 70295 Melee Targeting */
class spell_gb_melee_targeting : public SpellScriptLoader
{
    public:
        spell_gb_melee_targeting() : SpellScriptLoader("spell_gb_melee_targeting") { }

        class spell_gb_melee_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_melee_targeting_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([](WorldObject* u) { return u->GetTypeId() != TYPEID_PLAYER || u->ToPlayer()->GetVehicle(); });
                if (targets.size() > 1)
                {
                    if (Unit* caster = GetCaster())
                    {
                        for (auto&& target : targets)
                        {
                            if (target == caster->GetVictim())
                            {
                                targets.clear();
                                targets.push_back(caster->GetVictim());
                                return;
                            }
                        }
                        targets.sort(Trinity::ObjectDistanceOrderPred(caster));
                        targets.resize(1);
                    }
                }
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                _found = true;
                if (Unit* caster = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        if (caster->GetVictim() != target)
                        {
                            caster->GetAI()->AttackStart(target);
                            caster->AddThreat(target, 1000.0f);
                        }
                    }
                }
            }

            void HandleAfterCast()
            {
                if (_found)
                    return;

                if (Unit* caster = GetCaster())
                {
                    uint32 bossId = caster->GetEntry() == NPC_GB_KORKRON_SERGEANT || caster->GetEntry() == NPC_GB_KORKRON_REAVER ? NPC_GB_MURADIN_BRONZEBEARD : NPC_GB_HIGH_OVERLORD_SAURFANG;
                    if (!caster->GetVictim() || caster->GetVictim()->GetEntry() != bossId)
                        if (Creature* boss = caster->FindNearestCreature(bossId, 100.0f))
                            caster->GetAI()->AttackStart(boss);

                    uint32 spellId = caster->GetEntry() == NPC_GB_KORKRON_SERGEANT || caster->GetEntry() == NPC_GB_KORKRON_REAVER ? SPELL_BURNING_PITCH_H : SPELL_BURNING_PITCH_A;
                    if (caster->GetTypeId() == TYPEID_UNIT && !caster->ToCreature()->HasSpellCooldown(spellId))
                    {
                        caster->CastSpell(caster, spellId);
                        caster->ToCreature()->_AddCreatureSpellCooldown(spellId, time(nullptr) + 4);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gb_melee_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_gb_melee_targeting_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterCast += SpellCastFn(spell_gb_melee_targeting_SpellScript::HandleAfterCast);
            }

        private:
            bool _found = false;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_melee_targeting_SpellScript();
        }
};

/* spell 69678, 70609 Rocket Artillery */
class spell_gb_rocket_artillery : public SpellScriptLoader
{
    public:
        spell_gb_rocket_artillery() : SpellScriptLoader("spell_gb_rocket_artillery") { }

        class spell_gb_rocket_artillery_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_rocket_artillery_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        caster->CastSpell(target, GetEffectValue());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gb_rocket_artillery_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_gb_rocket_artillery_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_rocket_artillery_SpellScript();
        }
};

/* spell 69680, 69687, 69688, 69689 Explosion */
class spell_gb_rocket_artillery_explosion : public SpellScriptLoader
{
    public:
        spell_gb_rocket_artillery_explosion() : SpellScriptLoader("spell_gb_rocket_artillery_explosion") { }

        class spell_gb_rocket_artillery_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_rocket_artillery_explosion_SpellScript);

            void HandleExtraEffect()
            {
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(caster->GetEntry() == NPC_GB_SKYBREAKER_MORTAR_SOLDIER ? SPELL_BURNING_PITCH_SIEGE_DMG_A : SPELL_BURNING_PITCH_SIEGE_DMG_H, SPELLVALUE_BASE_POINT0, 10000, caster, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gb_rocket_artillery_explosion_SpellScript::HandleExtraEffect);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gb_rocket_artillery_explosion_SpellScript();
        }
};

/* spell 68645 Rocket Pack */
class spell_rocket_pack : public SpellScriptLoader
{
    public:
        spell_rocket_pack() : SpellScriptLoader("spell_rocket_pack") { }

        class spell_rocket_pack_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rocket_pack_AuraScript);

            bool Load() override
            {
                return GetOwner()->GetInstanceScript();
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), 68721, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                //GetTarget()->RemoveAurasDueToSpell(68721);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_rocket_pack_AuraScript::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_rocket_pack_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rocket_pack_AuraScript();
        }
};

/* spell 68721 Rocket Pack */
class spell_rocket_pack_periodic : public SpellScriptLoader
{
    public:
        spell_rocket_pack_periodic() : SpellScriptLoader("spell_rocket_pack_periodic") { }

        class spell_rocket_pack_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rocket_pack_periodic_AuraScript);

            bool Load() override
            {
                return GetOwner()->GetInstanceScript();
            }

            // Uncomment when SPELL_EFFECT_KNOCK_BACK_DEST hack is removed from SpellMgr.cpp for spell 68645
            /*void DummyTick(AuraEffect const* aurEff)
            {
                if (Unit* target = GetTarget())
                {
                    if (!target->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
                    {
                        Remove(AURA_REMOVE_BY_DEFAULT);
                        target->CastSpell(target, 69193, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rocket_pack_periodic_AuraScript::DummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }*/

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTargetApplication() || GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
                    return;

                if (Unit* target = GetTarget())
                    target->CastSpell(target, 69193, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_rocket_pack_periodic_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rocket_pack_periodic_AuraScript();
        }
};

class achievement_im_on_a_boat : public AchievementCriteriaScript
{
    public:
        achievement_im_on_a_boat() : AchievementCriteriaScript("achievement_im_on_a_boat") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            return target && !target->GetAI()->GetData(TIMES_IN_A_BOAT);
        }
};

void AddSC_boss_gunship_battle()
{
    new npc_gunship_boss();
    new npc_gunship();
    new npc_gunship_safe_area();
    new npc_korkron_axethrower_rifleman();
    new npc_sergeant();
    new npc_marine_or_reaver();
    new npc_gunship_mage();
    new npc_gunship_cannon();
    new npc_mortar_soldier_or_rocketeer();
    new npc_zafod_boombox();
    new npc_gunship_portal();
    new npc_gunship_trigger();
    new npc_korkron_primalist();
    new npc_korkron_defender();
    new npc_skybreaker_vindicator();
    new npc_skybreaker_protector();
    new npc_icc_spire_frostwyrm();
    new transport_icc_gunship();
    new at_icc_land_frostwyrm();
    new spell_icc_remove_rocket_pack();
    new spell_gb_heat_drain();
    new spell_gb_incinerating_blast();
    new spell_gb_burning_pitch();
    new spell_gb_burning_pitch_targeting();
    new spell_gb_ranged_targeting();
    new spell_gb_melee_targeting();
    new spell_gb_rocket_artillery();
    new spell_gb_rocket_artillery_explosion();
    new spell_rocket_pack();
    new spell_rocket_pack_periodic();
    //new achievement_im_on_a_boat(); // check if no achiv
}
