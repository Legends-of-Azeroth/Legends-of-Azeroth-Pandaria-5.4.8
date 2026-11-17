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

#include "ScenarioMgr.h"
#include "WorldSession.h"

void WorldSession::HandleQueryScenarioPOI(WorldPackets::Scene::QueryScenarioPOI& packet)
{
    // Read criteria tree ids and add the in a unordered_set so we don't send POIs for the same criteria tree multiple times
    std::unordered_set<int32> criteriaTreeIds;
    for (uint32 criteriaTreeId : packet.CriteriaTreeIds)
        criteriaTreeIds.insert(criteriaTreeId); // CriteriaTreeID

    uint32 count = 0;
    ByteBuffer dataBuff;
    WorldPacket data(SMSG_SCENARIO_POIS);

    size_t writePos = data.bitwpos();
    data.WriteBits(count, 21);

    for (auto&& criteriaTreeId : criteriaTreeIds)
    {
        std::vector<ScenarioPOI> const* ScenarioPOIs = sScenarioMgr->GetScenarioPOIs(criteriaTreeId);
        if (!ScenarioPOIs)
            continue;

        data.WriteBits(ScenarioPOIs->size(), 19);

        for (ScenarioPOI const& scenarioPOI : *ScenarioPOIs)
        {
            data.WriteBits(scenarioPOI.Points.size(), 21);

            dataBuff << int32(scenarioPOI.BlobIndex);
            dataBuff << int32(scenarioPOI.Floor);
            dataBuff << int32(scenarioPOI.WorldEffectID);

            for (ScenarioPOIPoint const& scenarioPOIBlobPoint : scenarioPOI.Points)
            {
                dataBuff << int32(scenarioPOIBlobPoint.X);
                dataBuff << int32(scenarioPOIBlobPoint.Y);
            }

            dataBuff << int32(0);
            dataBuff << int32(scenarioPOI.PlayerConditionID);
            dataBuff << int32(scenarioPOI.MapID);
            dataBuff << int32(0);
            dataBuff << int32(scenarioPOI.WorldMapAreaID);
        }
        dataBuff << int32(criteriaTreeId);

        count++;
    }

    data.FlushBits();
    data.PutBits(writePos, count, 21);
    data.append(dataBuff);

    SendPacket(&data);
}
