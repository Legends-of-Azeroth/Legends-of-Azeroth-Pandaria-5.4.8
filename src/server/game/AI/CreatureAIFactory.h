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

#ifndef TRINITY_CREATUREAIFACTORY_H
#define TRINITY_CREATUREAIFACTORY_H

//#include "Policies/Singleton.h"
#include "ObjectRegistry.h"
#include "FactoryHolder.h"
#include "GameObjectAI.h"

struct SelectableAI : public FactoryHolder<CreatureAI, Creature>, public Permissible<Creature>
{
    SelectableAI(const char* id) : FactoryHolder<CreatureAI, Creature>(id) { }
};

template<class REAL_AI>
struct CreatureAIFactory : public SelectableAI
{
    CreatureAIFactory(const char* name) : SelectableAI(name) { }

    CreatureAI* Create(Creature* c) const override
    {
        return new REAL_AI(c);
    }

    int Permit(const Creature* c) const { return REAL_AI::Permissible(c); }
};

typedef FactoryHolder<CreatureAI, Creature> CreatureAICreator;
typedef FactoryHolder<CreatureAI, Creature>::FactoryHolderRegistry CreatureAIRegistry;
typedef FactoryHolder<CreatureAI, Creature>::FactoryHolderRepository CreatureAIRepository;

//GO
struct SelectableGameObjectAI : public FactoryHolder<GameObjectAI, GameObject>, public Permissible<GameObject>
{
    SelectableGameObjectAI(const char* id) : FactoryHolder<GameObjectAI, GameObject>(id) { }
};

template<class REAL_GO_AI>
struct GameObjectAIFactory : public SelectableGameObjectAI
{
    GameObjectAIFactory(const char* name) : SelectableGameObjectAI(name) { }

    GameObjectAI* Create(GameObject* go) const override
    {
        return new REAL_GO_AI(go);
    }

    int Permit(const GameObject* g) const { return REAL_GO_AI::Permissible(g); }
};

typedef FactoryHolder<GameObjectAI, GameObject> GameObjectAICreator;
typedef FactoryHolder<GameObjectAI, GameObject>::FactoryHolderRegistry GameObjectAIRegistry;
typedef FactoryHolder<GameObjectAI, GameObject>::FactoryHolderRepository GameObjectAIRepository;
#endif
