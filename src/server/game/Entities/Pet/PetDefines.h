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

#ifndef TRINITYCORE_PET_DEFINES_H
#define TRINITYCORE_PET_DEFINES_H

enum PetType
{
    SUMMON_PET              = 0,
    HUNTER_PET              = 1,
    MAX_PET_TYPE            = 4
};

enum PetRemoveMode
{
    PET_REMOVE_DISMISS,
    PET_REMOVE_ABANDON
};

enum PetRemoveFlag
{
    PET_REMOVE_FLAG_NONE            = 0x00,
    PET_REMOVE_FLAG_RETURN_REAGENT  = 0x01,
    PET_REMOVE_FLAG_RESET_CURRENT   = 0x02,
};

#define MAX_PET_STABLES         4

// stored in character_pet.slot
enum PetSaveMode : int8
{
    PET_SAVE_AS_DELETED        = -1,                        // not saved in fact
    PET_SAVE_AS_CURRENT        =  0,                        // in current slot (with player)
    PET_SAVE_FIRST_STABLE_SLOT =  1,
    PET_SAVE_LAST_STABLE_SLOT  =  MAX_PET_STABLES,          // last in DB stable slot index (including), all higher have same meaning as PET_SAVE_NOT_IN_SLOT
    PET_SAVE_NOT_IN_SLOT       =  100                       // for avoid conflict with stable size grow will use 100
};

enum PetSpellState
{
    PETSPELL_UNCHANGED  = 0,
    PETSPELL_CHANGED    = 1,
    PETSPELL_NEW        = 2,
    PETSPELL_REMOVED    = 3
};

enum ActionFeedback
{
    FEEDBACK_NONE            = 0,
    FEEDBACK_PET_DEAD        = 1,
    FEEDBACK_NOTHING_TO_ATT  = 2,
    FEEDBACK_CANT_ATT_TARGET = 3
};

enum PetTalk
{
    PET_TALK_SPECIAL_SPELL  = 0,
    PET_TALK_ATTACK         = 1
};

// stored in character_pet.slot
enum PetSlot
{
    PET_SLOT_ACTIVE_FIRST   = 0,
    PET_SLOT_ACTIVE_LAST    = 4,
    PET_SLOT_STABLE_FIRST   = 5,
    PET_SLOT_STABLE_LAST    = 54,
};

enum PetLoadMode
{
    PET_LOAD_BY_ID,
    PET_LOAD_BY_ENTRY,
    PET_LOAD_BY_SLOT
};

enum PetTameError
{
    PET_TAME_ERROR_UNKNOWN_ERROR            = 0,
    PET_TAME_ERROR_INVALID_CREATURE         = 1,
    PET_TAME_ERROR_TOO_MANY_PETS            = 2,
    PET_TAME_ERROR_CREATURE_ALREADY_OWNED   = 3,
    PET_TAME_ERROR_NOT_TAMEABLE             = 4,
    PET_TAME_ERROR_ANOTHER_SUMMON_ACTIVE    = 5,
    PET_TAME_ERROR_YOU_CANT_TAME            = 6,
    PET_TAME_ERROR_NO_PET_AVAILABLE         = 7,
    PET_TAME_ERROR_INTERNAL_ERROR           = 8,
    PET_TAME_ERROR_TOO_HIGH_LEVEL           = 9,
    PET_TAME_ERROR_DEAD                     = 10,
    PET_TAME_ERROR_NOT_DEAD                 = 11,
    PET_TAME_ERROR_CANT_CONTROL_EXOTIC      = 12,
    PET_TAME_ERROR_INVALID_SLOT             = 13
};

#define PET_FOLLOW_DIST  1.0f
#define PET_FOLLOW_ANGLE (M_PI/2)
#define DEFAULT_FOLLOW_DISTANCE 2.5f
#define DEFAULT_FOLLOW_DISTANCE_PET 3.0f
#define DEFAULT_FOLLOW_ANGLE M_PI

// Currently used only for the hunter's pet list
// But it will be cool if I ever move all pets to the one loading to memory and avoid all asynchronous shit with DB
struct PetData
{
    uint32 PetNumber;
    uint32 Entry;
    std::string Name;
};

#endif
