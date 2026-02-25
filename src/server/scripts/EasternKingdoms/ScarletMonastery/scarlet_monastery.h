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

#ifndef SCARLET_MONASTERY_H_
#define SCARLET_MONASTERY_H_

#define DataHeader "SM"
#define scarlet_monastery_scriptname "instance_scarlet_monastery"

uint32 const encounternumber = 4;

#include "SpellScript.h"
#include "Map.h"
#include "Creature.h"
#include "CreatureAIImpl.h"

#define LFG_DUNGEON_HEADLESS_HORSEMAN 285

enum eData
{
    BOSS_THALNOS_THE_SOULRENDER     = 0,
    BOSS_BROTHER_KORLOFF            = 1,
    BOSS_WHITEMANE                  = 2,
    BOSS_DURAND                     = 3,
};

enum eObjects
{
    GO_THALNOS_EXIT                 = 211844,
    GO_KORLOFF_EXIT                 = 210564,
    GO_WHITEMANE_ENTRANCE           = 210563,
    GO_CHALLENGE_DOOR               = 211989,
};

enum eSpells
{
    SPELL_SUMMON_WHITEMANE_SOUL_TARGET = 126791,
    SPELL_SUMMON_UNQUENCHABLE          = 126802,
    SPELL_SUMMON_HAND_OF_PROVIDENCE    = 126832,
    SPELL_SUMMON_SOUL_MISSILE          = 126795,
    SPELL_WHITEMANE_CORPSE_SPELL_FOCUS = 126789,
    SPELL_WHITEMANE_KILL_CREDIT        = 132022, // wrong spell
};

enum Actions
{
    ACTION_REMOVE_RAISE = 0,
    ACTION_FIRESTORM    = 1,
};

enum Worldstate
{
    WORLDSTATE_EMPOWERED_SPIRITUALIST = 12253,
    WORLDSTATE_BURNING_MAN            = 12259,
    WORLDSTATE_AND_STAY_DEAD          = 12546,
};

enum eCreature
{
    NPC_BROTHER_KORLOFF             = 59223,
    NPC_COMMANDER_DURAND            = 60040,
    NPC_HIGH_INQUISITOR_WHITEMANE   = 3977,
    NPC_THALNOS_THE_SOULRENDER      = 59789,
    NPC_SCARLET_FANATIC             = 58555, // trash
    NPC_SCARLET_CENTURION           = 59746,
    NPC_SCARLET_FLAMETHOWER         = 59705,
    NPC_SCARLET_JUDICATOR           = 58605,
    NPC_SCARLET_ZEALOT              = 58590,
    NPC_SCARLET_PURIFIER            = 58569,
    NPC_SCARLET_INITIATE            = 58783,
    NPC_FALLEN_CRUSADER             = 59884, // Thalnos
    NPC_EVICTED_SOUL                = 59974,
    NPC_EMPOWERING_SPIRIT           = 59893,
    NPC_EMPOWERED_ZOMBIE            = 59930,
    NPC_SCORCHED_EARTH_TRIGGER      = 59507,
    NPC_ZOMBIFIED_CORPSE            = 59771,
    NPC_HOODED_CRUSADER_INTO        = 64838,
    NPC_HOODED_CRUSADER_OUTRO       = 64842,
    NPC_FUEL_BARREL                 = 59706,
    NPC_TRAINING_DUMMY              = 60197,
    NPC_SPIRIT_OF_REDEMPTION        = 58592,
    NPC_PILE_OF_CORPSES             = 59722,
    NPC_FRENZIED_SPIRIT             = 60033,
    NPC_TRAINING_DUMMY_2            = 64446,
    NPC_Q31514_KILL_CREDIT          = 64835, // "Unto Dust Thou Shalt Return" Kill Credit
    NPC_Q31516_KILL_CREDIT          = 64840, // "Unto Dust Thou Shalt Return" Kill Credit
};

enum InstanceScenarioData
{
    SCENARIO_ID                     = 53,
    // Bosses data
    CRITERIA_THALNOS_THE_SOULRENDER = 19270,
    CRITERIA_BROTHER_KORLOFF        = 19271,
    CRITERIA_DURAND                 = 533,
    CRITERIA_ENEMIES                = 20044,
    ENEMIES_COUNT                   = 40,
};

#define MAX_TYPES 4
#endif // SCARLET_MONASTERY_H_