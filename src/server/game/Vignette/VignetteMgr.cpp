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

#include "VignetteMgr.h"
#include "Player.h"
#include "AreaTrigger.h"

namespace Vignette
{
    Manager::Manager(Player const* player)
    {
        m_Owner = player;
    }

    Manager::~Manager()
    {
        m_Owner = nullptr;

        for (auto&& itr : m_Vignettes)
            delete itr.second;
    }

    Entity* Manager::CreateAndAddVignette(VignetteEntry const* vignetteEntry, Map* map, uint32 const mapId, Type const vignetteType, G3D::Vector3 const position, ObjectGuid const sourceGuid)
    {
        // Check for duplicated vignettes
        for (auto&& vignette : m_Vignettes)
        {
            // Return if same vignette has already been created
            if (vignette.second->GetVignetteEntry()->Id == vignetteEntry->Id)
                return nullptr;
        }

        Entity* vignette = new Entity(vignetteEntry, mapId);
        vignette->Create(map, vignetteType, position, sourceGuid);

        m_Vignettes.insert(std::make_pair(vignette->GetGuid(), vignette));
        m_AddedVignette.insert(vignette->GetGuid());

        return vignette;
    }

    void Manager::DestroyAndRemoveVignetteByEntry(VignetteEntry const* vignetteEntry)
    {
        if (!vignetteEntry)
            return;

        for (auto&& itr = m_Vignettes.begin(); itr != m_Vignettes.end();)
        {
            if (itr->second->GetVignetteEntry()->Id == vignetteEntry->Id)
            {
                delete itr->second;
                m_RemovedVignette.insert(itr->first);
                itr = m_Vignettes.erase(itr);
                continue;
            }

            ++itr;
        }
    }

    void Manager::DestroyAndRemoveVignettes(std::function<bool(Entity* const)> lambda)
    {
        for (auto&& itr = m_Vignettes.begin(); itr != m_Vignettes.end();)
        {
            if (lambda(itr->second))
            {
                delete itr->second;
                m_RemovedVignette.insert(itr->first);
                itr = m_Vignettes.erase(itr);
                continue;
            }

            ++itr;
        }
    }

    void Manager::SendVignetteUpdateToClient()
    {
        uint32 addedVignetteCount = 0, updatedVignetteCount = 0;
        ByteBuffer playersAddBuffer, playersUpdateBuffer, playersRemoveBuffer, objectAddBuffer, objectUpdateBuffer;
        WorldPacket data(SMSG_VIGNETTE_UPDATE);

        data.WriteBits(m_RemovedVignette.size(), 24);
        data.WriteBit(false);                                 // ForceUpdate
        data.WriteBits(m_UpdatedVignette.size(), 24);
        for (auto&& vignetteGuid : m_UpdatedVignette)
        {
            data.WriteBit(vignetteGuid[4]);
            data.WriteBit(vignetteGuid[7]);
            data.WriteBit(vignetteGuid[3]);
            data.WriteBit(vignetteGuid[2]);
            data.WriteBit(vignetteGuid[6]);
            data.WriteBit(vignetteGuid[0]);
            data.WriteBit(vignetteGuid[5]);
            data.WriteBit(vignetteGuid[1]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[6]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[2]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[5]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[0]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[3]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[4]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[1]);
            playersUpdateBuffer.WriteByteSeq(vignetteGuid[7]);
        }

        for (auto&& vignetteGuid : m_RemovedVignette)
        {
            data.WriteBit(vignetteGuid[5]);
            data.WriteBit(vignetteGuid[4]);
            data.WriteBit(vignetteGuid[1]);
            data.WriteBit(vignetteGuid[7]);
            data.WriteBit(vignetteGuid[0]);
            data.WriteBit(vignetteGuid[6]);
            data.WriteBit(vignetteGuid[2]);
            data.WriteBit(vignetteGuid[3]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[4]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[0]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[6]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[7]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[5]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[3]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[1]);
            playersRemoveBuffer.WriteByteSeq(vignetteGuid[2]);
        }
        m_RemovedVignette.clear();

        data.WriteBits(m_AddedVignette.size(), 24);
        for (auto&& vignetteGuid : m_AddedVignette)
        {
            data.WriteBit(vignetteGuid[1]);
            data.WriteBit(vignetteGuid[4]);
            data.WriteBit(vignetteGuid[3]);
            data.WriteBit(vignetteGuid[6]);
            data.WriteBit(vignetteGuid[2]);
            data.WriteBit(vignetteGuid[0]);
            data.WriteBit(vignetteGuid[7]);
            data.WriteBit(vignetteGuid[5]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[7]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[1]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[0]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[6]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[2]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[3]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[4]);
            playersAddBuffer.WriteByteSeq(vignetteGuid[5]);
        }

        size_t addedVignetteCountPos = data.bitwpos();
        data.WriteBits(addedVignetteCount, 20);

        for (auto&& vignetteGuid : m_AddedVignette)
        {
            auto findResult = m_Vignettes.find(vignetteGuid);
            if (findResult == m_Vignettes.end())
                continue;

            addedVignetteCount++;

            auto vignette = findResult->second;
            ObjectGuid objectGUID = vignette->GetGuid();

            data.WriteBit(objectGUID[5]);
            data.WriteBit(objectGUID[3]);
            data.WriteBit(objectGUID[7]);
            data.WriteBit(objectGUID[4]);
            data.WriteBit(objectGUID[2]);
            data.WriteBit(objectGUID[0]);
            data.WriteBit(objectGUID[6]);
            data.WriteBit(objectGUID[1]);
            objectAddBuffer.WriteByteSeq(objectGUID[2]);
            objectAddBuffer.WriteByteSeq(objectGUID[5]);
            objectAddBuffer << float(vignette->GetPosition().z);
            objectAddBuffer << uint32(vignette->GetVignetteEntry()->Id);
            objectAddBuffer << float(vignette->GetPosition().y);
            objectAddBuffer.WriteByteSeq(objectGUID[1]);
            objectAddBuffer << float(vignette->GetPosition().x);
            objectAddBuffer.WriteByteSeq(objectGUID[6]);
            objectAddBuffer.WriteByteSeq(objectGUID[7]);
            objectAddBuffer.WriteByteSeq(objectGUID[4]);
            objectAddBuffer.WriteByteSeq(objectGUID[3]);
            objectAddBuffer.WriteByteSeq(objectGUID[0]);
        }
        m_AddedVignette.clear();

        size_t updatedVignetteCountPos = data.bitwpos();
        data.WriteBits(updatedVignetteCount, 20);

        for (auto&& vignetteGuid : m_UpdatedVignette)
        {
            auto findResult = m_Vignettes.find(vignetteGuid);
            if (findResult == m_Vignettes.end())
                continue;

            updatedVignetteCount++;

            auto vignette = findResult->second;
            ObjectGuid objectGUID = vignette->GetGuid();

            data.WriteBit(objectGUID[3]);
data.WriteBit(objectGUID[5]);
data.WriteBit(objectGUID[2]);
data.WriteBit(objectGUID[6]);
data.WriteBit(objectGUID[4]);
data.WriteBit(objectGUID[0]);
data.WriteBit(objectGUID[1]);
data.WriteBit(objectGUID[7]);
            objectUpdateBuffer.WriteByteSeq(objectGUID[5]);
objectUpdateBuffer.WriteByteSeq(objectGUID[2]);
            objectUpdateBuffer << float(vignette->GetPosition().z);
            objectUpdateBuffer << float(vignette->GetPosition().x);
            objectUpdateBuffer.WriteByteSeq(objectGUID[1]);
objectUpdateBuffer.WriteByteSeq(objectGUID[4]);
objectUpdateBuffer.WriteByteSeq(objectGUID[6]);
objectUpdateBuffer.WriteByteSeq(objectGUID[0]);
            objectUpdateBuffer << uint32(vignette->GetVignetteEntry()->Id);
            objectUpdateBuffer << float(vignette->GetPosition().y);
            objectUpdateBuffer.WriteByteSeq(objectGUID[3]);
objectUpdateBuffer.WriteByteSeq(objectGUID[7]);
        }
        m_UpdatedVignette.clear();

        data.FlushBits();

        data.PutBits(addedVignetteCountPos, addedVignetteCount, 20);
        data.PutBits(updatedVignetteCountPos, updatedVignetteCount, 20);

        if (objectUpdateBuffer.size())
            data.append(objectUpdateBuffer);
        if (playersRemoveBuffer.size())
            data.append(playersRemoveBuffer);
        if (objectAddBuffer.size())
            data.append(objectAddBuffer);
        if (playersAddBuffer.size())
            data.append(playersAddBuffer);
        if (playersUpdateBuffer.size())
            data.append(playersUpdateBuffer);

        m_Owner->GetSession()->SendPacket(&data);
    }

    void Manager::Update()
    {
        for (auto&& itr : m_Vignettes)
        {
            auto vignette = itr.second;

            // Update the position of the vignette if vignette is linked to a creature
            if (vignette->GeSourceGuid().IsUnit())
            {
                // TaskMgr::Default()->ScheduleInvocation([=]
                // {
                //     if (Creature* sourceCreature = sObjectAccessor->FindCreature(vignette->GeSourceGuid()))
                //         vignette->UpdatePosition(G3D::Vector3(sourceCreature->GetPositionX(), sourceCreature->GetPositionY(), sourceCreature->GetPositionZ()));
                // });
                if (Map* map = sMapMgr->FindBaseMap(vignette->m_Map))
                    if (Creature* sourceCreature = map->GetCreature(vignette->GeSourceGuid()))
                        vignette->UpdatePosition(G3D::Vector3(sourceCreature->GetPositionX(), sourceCreature->GetPositionY(), sourceCreature->GetPositionZ()));
            }

            if (vignette->NeedClientUpdate())
            {
                m_UpdatedVignette.insert(vignette->GetGuid());
                vignette->ResetNeedClientUpdate();
            }
        }

        // Send update to client if needed
        if (!m_AddedVignette.empty() || !m_UpdatedVignette.empty() || !m_RemovedVignette.empty())
            SendVignetteUpdateToClient();
    }

    template <class T>
    void Manager::OnWorldObjectAppear(T const* target)
    {
        VignetteEntry const* vignetteEntry = GetVignetteEntryFromWorldObject(target);
        if (!vignetteEntry)
            return;

        auto type = GetDefaultVignetteTypeFromWorldObject(target);
        auto trackingQuest = GetTrackingQuestIdFromWorldObject(target);

        if (trackingQuest)
        {
            if (m_Owner->IsQuestRewarded(trackingQuest))
                return;

            type = GetTrackingVignetteTypeFromWorldObject(target);
        }

        CreateAndAddVignette(vignetteEntry, target->GetMap(), target->GetMapId(), type, G3D::Vector3(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()), target->GetGUID());
    }

    template <class T>
    void Manager::OnWorldObjectDisappear(T const* target)
    {
        auto vignetteEntry = GetVignetteEntryFromWorldObject(target);
        if (!vignetteEntry)
            return;

        DestroyAndRemoveVignettes([target](Entity const* vignette) -> bool
        {
            if (vignette->GeSourceGuid() == target->GetGUID() && vignette->GetVignetteType() != Type::SourceScript)
                return true;

            return false;
        });
    }

    template void Manager::OnWorldObjectDisappear(Corpse const*);
    template void Manager::OnWorldObjectDisappear(Creature const*);
    template void Manager::OnWorldObjectDisappear(GameObject const*);
    template void Manager::OnWorldObjectDisappear(DynamicObject const*);
    template void Manager::OnWorldObjectDisappear(AreaTrigger const*);
    template void Manager::OnWorldObjectDisappear(WorldObject const*);
    template void Manager::OnWorldObjectDisappear(Player const*);

    template void Manager::OnWorldObjectAppear(Corpse const*);
    template void Manager::OnWorldObjectAppear(Creature const*);
    template void Manager::OnWorldObjectAppear(GameObject const*);
    template void Manager::OnWorldObjectAppear(DynamicObject const*);
    template void Manager::OnWorldObjectAppear(AreaTrigger const*);
    template void Manager::OnWorldObjectAppear(WorldObject const*);
    template void Manager::OnWorldObjectAppear(Player const*);
}
