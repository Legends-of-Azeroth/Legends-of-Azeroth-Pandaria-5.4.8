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

#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "Item.h"
#include "Map.h"
#include "Transport.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "SpellInfo.h"

using namespace Trinity;

void VisibleNotifier::SendToSelf()
{
    // at this moment i_clientGUIDs have guids that not iterate at grid level checks
    // but exist one case when this possible and object not out of range: transports
    if (Transport* transport = i_player.GetTransport())
    {
        for (auto&& it : transport->GetPassengers())
        {
            if (vis_guids.find(it->GetGUID()) != vis_guids.end())
            {
                vis_guids.erase(it->GetGUID());

                switch (it->GetTypeId())
                {
                    case TYPEID_GAMEOBJECT:
                        i_player.UpdateVisibilityOf(it->ToGameObject(), i_data, i_visibleNow);
                        break;
                    case TYPEID_PLAYER:
                        i_player.UpdateVisibilityOf(it->ToPlayer(), i_data, i_visibleNow);

                        it->ToPlayer()->UpdateVisibilityOf(&i_player);
                        break;
                    case TYPEID_UNIT:
                        i_player.UpdateVisibilityOf(it->ToCreature(), i_data, i_visibleNow);
                        break;
                    case TYPEID_DYNAMICOBJECT:
                        i_player.UpdateVisibilityOf(it->ToDynObject(), i_data, i_visibleNow);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    for (Player::ClientGUIDs::const_iterator it = vis_guids.begin();it != vis_guids.end(); ++it)
    {
        i_player.m_clientGUIDs.erase(*it);
        i_data.AddOutOfRangeGUID(*it);

        if ((*it).IsPlayer())
        {
            Player* player = ObjectAccessor::FindPlayer(*it);
            if (player && player->IsInWorld())
                player->UpdateVisibilityOf(&i_player);
        }
    }

    if (!i_data.HasData())
        return;

    WorldPacket packet;
    i_data.BuildPacket(&packet);
    i_player.GetSession()->SendPacket(&packet);

    for (std::set<Unit*>::const_iterator it = i_visibleNow.begin(); it != i_visibleNow.end(); ++it)
        i_player.SendInitialVisiblePackets(*it);
}

void VisibleChangesNotifier::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        if (iter->GetSource() == &i_object)
            continue;

        iter->GetSource()->UpdateVisibilityOf(&i_object);

        if (iter->GetSource()->HasSharedVision())
        {
            for (SharedVisionList::const_iterator i = iter->GetSource()->GetSharedVisionList().begin();
                i != iter->GetSource()->GetSharedVisionList().end(); ++i)
            {
                if ((*i)->m_seer == iter->GetSource())
                    (*i)->UpdateVisibilityOf(&i_object);
            }
        }
    }
}

void VisibleChangesNotifier::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->GetSource()->HasSharedVision())
            for (SharedVisionList::const_iterator i = iter->GetSource()->GetSharedVisionList().begin();
                i != iter->GetSource()->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == iter->GetSource())
                    (*i)->UpdateVisibilityOf(&i_object);
}

void VisibleChangesNotifier::Visit(DynamicObjectMapType &m)
{
    for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (Unit* caster = iter->GetSource()->GetCaster())
            if (Player* player = caster->ToPlayer())
                if (player->m_seer == iter->GetSource())
                    player->UpdateVisibilityOf(&i_object);
}

inline void CreatureUnitRelocationWorker(Creature* c, Unit* u)
{
    if (!u->IsAlive() || !c->IsAlive() || c == u || u->IsInFlight())
        return;

    if (c->HasReactState(REACT_AGGRESSIVE) && !c->HasUnitState(UNIT_STATE_SIGHTLESS))
        if (c->IsAIEnabled && c->CanSeeOrDetect(u, false, true))
            c->AI()->MoveInLineOfSight_Safe(u);
}

void AIRelocationNotifier::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* c = iter->GetSource();
        CreatureUnitRelocationWorker(c, &i_unit);
        if (isCreature)
            CreatureUnitRelocationWorker((Creature*)&i_unit, c);
    }
}

void MessageDistDeliverer::Visit(PlayerMapType &m)
{
    for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Player* target = iter->GetSource();
        if (!target->InSamePhase(i_phaseMask))
            continue;

        if (required3dDist)
        {
            if (target->GetExactDistSq(i_source) > i_distSq)
                continue;
        }
        else
        {
            if (target->GetExactDist2dSq(i_source) > i_distSq)
                continue;
        }        

        // Send packet to all who are sharing the player's vision
        if (target->HasSharedVision())
        {
            SharedVisionList::const_iterator i = target->GetSharedVisionList().begin();
            for (; i != target->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == target)
                    SendPacket(*i);
        }

        if (target->m_seer == target || target->GetVehicle())
            SendPacket(target);
    }
}

void MessageDistDeliverer::Visit(CreatureMapType &m)
{
    for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        Creature* target = iter->GetSource();
        if (!target->InSamePhase(i_phaseMask))
            continue;

        if (required3dDist)
        {
            if (target->GetExactDistSq(i_source) > i_distSq)
                continue;
        }
        else
        {
            if (target->GetExactDist2dSq(i_source) > i_distSq)
                continue;
        }

        // Send packet to all who are sharing the creature's vision
        if (target->HasSharedVision())
        {
            SharedVisionList::const_iterator i = target->GetSharedVisionList().begin();
            for (; i != target->GetSharedVisionList().end(); ++i)
                if ((*i)->m_seer == target)
                    SendPacket(*i);
        }
    }
}

void MessageDistDeliverer::Visit(DynamicObjectMapType &m)
{
    for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        DynamicObject* target = iter->GetSource();
        if (!target->InSamePhase(i_phaseMask))
            continue;

        if (required3dDist)
        {
            if (target->GetExactDistSq(i_source) > i_distSq)
                continue;
        }
        else
        {
            if (target->GetExactDist2dSq(i_source) > i_distSq)
                continue;
        }

        if (Unit* caster = target->GetCaster())
        {
            // Send packet back to the caster if the caster has vision of dynamic object
            Player* player = caster->ToPlayer();
            if (player && player->m_seer == target)
                SendPacket(player);
        }
    }
}

/*
void
MessageDistDeliverer::VisitObject(Player* player)
{
    if (!i_ownTeamOnly || (i_source.GetTypeId() == TYPEID_PLAYER && player->GetTeam() == ((Player&)i_source).GetTeam()))
    {
        SendPacket(player);
    }
}
*/

template<class T>
void ObjectUpdater::Visit(GridRefManager<T> &m)
{
    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
        if (iter->GetSource()->IsInWorld())
            iter->GetSource()->Update(i_timeDiff);
}

bool AnyDeadUnitObjectInRangeCheck::operator()(Player* u)
{
    return !u->IsAlive() && !u->HasAuraType(SPELL_AURA_GHOST) && i_searchObj->IsWithinDistInMap(u, i_range);
}

bool AnyDeadUnitObjectInRangeCheck::operator()(Corpse* u)
{
    return u->GetType() != CORPSE_BONES && i_searchObj->IsWithinDistInMap(u, i_range);
}

bool AnyDeadUnitObjectInRangeCheck::operator()(Creature* u)
{
    return !u->IsAlive() && i_searchObj->IsWithinDistInMap(u, i_range);
}

bool AnyDeadUnitSpellTargetInRangeCheck::operator()(Player* u)
{
    return AnyDeadUnitObjectInRangeCheck::operator()(u) && i_check(u);
}

bool AnyDeadUnitSpellTargetInRangeCheck::operator()(Corpse* u)
{
    return AnyDeadUnitObjectInRangeCheck::operator()(u) && i_check(u);
}

bool AnyDeadUnitSpellTargetInRangeCheck::operator()(Creature* u)
{
    return AnyDeadUnitObjectInRangeCheck::operator()(u) && i_check(u);
}

template void ObjectUpdater::Visit<Creature>(CreatureMapType&);
template void ObjectUpdater::Visit<GameObject>(GameObjectMapType&);
template void ObjectUpdater::Visit<DynamicObject>(DynamicObjectMapType&);
template void ObjectUpdater::Visit<AreaTrigger>(AreaTriggerMapType &);
