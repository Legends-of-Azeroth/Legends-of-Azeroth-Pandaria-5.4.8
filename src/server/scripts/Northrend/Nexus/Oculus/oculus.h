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

#ifndef OCULUS_H_
#define OCULUS_H_

#define OculusScriptName "instance_oculus"

uint32 const EncounterCount = 4;

enum DataTypes
{
    // Encounter States/Boss GUIDs
    DATA_DRAKOS                             = 0,
    DATA_VAROS                              = 1,
    DATA_UROM                               = 2,
    DATA_EREGOS                             = 3,

    DATA_CENTRIFUGUE_CONSTRUCTS             = 4
};

enum CreatureIds
{
    NPC_DRAKOS                              = 27654,
    NPC_VAROS                               = 27447,
    NPC_UROM                                = 27655,
    NPC_EREGOS                              = 27656,

    NPC_AZURE_RING_GUARDIAN                 = 28236,
    NPC_AZURE_RING_GUARDIAN_2               = 27638,
    NPC_CENTRIFUGE_CONSTRUCT                = 27641,
    NPC_RUBY_DRAKE_VEHICLE                  = 27756,
    NPC_EMERALD_DRAKE_VEHICLE               = 27692,
    NPC_AMBER_DRAKE_VEHICLE                 = 27755,
    NPC_VERDISA                             = 27657,
    NPC_BELGARISTRASZ                       = 27658,
    NPC_ETERNOS                             = 27659,
    NPC_GREATER_WHELP                       = 28276,
    NPC_IMAGE_OF_BELGARISTRASZ              = 28012,
};

enum GameObjectIds
{
    GO_DRAGON_CAGE_DOOR                     = 193995,
    GO_EREGOS_CACHE_N                       = 191349,
    GO_EREGOS_CACHE_H                       = 193603,
    GO_SPOTLIGHT                            = 191351,
};

enum SpellEvents
{
    EVENT_CALL_DRAGON                       = 12229
};

enum CreatureActions
{
    ACTION_CALL_DRAGON_EVENT                = 1
};

enum OculusWorldStates
{
    WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW   = 3524,
    WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT = 3486
};

enum OculusSpells
{
    SPELL_CENTRIFUGE_SHIELD                 = 50053,
    SPELL_DEATH_SPELL                       = 50415
};

enum InstanceTexts
{
    SAY_EREGOS_INTRO_TEXT                   = 0,
    SAY_VAROS_INTRO_TEXT                    = 4
};

enum InstanceEvents
{
    EVENT_VAROS_INTRO                       = 1,
    EVENT_EREGOS_INTRO
};

enum Misc
{
    POINT_MOVE_OUT                          = 1
};

enum Worldstates
{
    WORLDSTATE_RUBY_VOID    = 6334,
    WORLDSTATE_EMERALD_VOID = 6335,
    WORLDSTATE_AMBER_VOID   = 6336,
};

struct CacheEligibilityCheckAccessor
{
    virtual bool IsPlayerEligibleForCache(ObjectGuid playerGuid) const = 0;
};

template<class AI>
AI* GetOculusAI(Creature* creature)
{
    if (InstanceMap* instance = creature->GetMap()->ToInstanceMap())
        if (instance->GetInstanceScript())
            if (instance->GetScriptId() == sObjectMgr->GetScriptId(OculusScriptName))
                return new AI(creature);
    return NULL;
}

#endif // OCULUS_H_
