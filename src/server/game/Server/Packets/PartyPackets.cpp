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

#include "PartyPackets.h"

WorldPacket const* WorldPackets::Party::SendRaidTargetUpdateAll::Write()
{
    _worldPacket.WriteBits(TargetIcons.size(), 23);

    ByteBuffer dataBuffer;

    for (auto itr = TargetIcons.begin(); itr != TargetIcons.end(); ++itr)
    {
        ObjectGuid guid = itr->second;

        _worldPacket.WriteBit(guid[2]);
        _worldPacket.WriteBit(guid[1]);
        _worldPacket.WriteBit(guid[3]);
        _worldPacket.WriteBit(guid[7]);
        _worldPacket.WriteBit(guid[6]);
        _worldPacket.WriteBit(guid[4]);
        _worldPacket.WriteBit(guid[0]);
        _worldPacket.WriteBit(guid[5]);

        dataBuffer.WriteByteSeq(guid[4]);
        dataBuffer.WriteByteSeq(guid[7]);
        dataBuffer.WriteByteSeq(guid[1]);
        dataBuffer.WriteByteSeq(guid[0]);
        dataBuffer.WriteByteSeq(guid[6]);
        dataBuffer.WriteByteSeq(guid[5]);
        dataBuffer.WriteByteSeq(guid[3]);
        dataBuffer << (uint8)itr->first;
        dataBuffer.WriteByteSeq(guid[2]);
    }

    _worldPacket.append(dataBuffer);
    _worldPacket << (uint8)PartyIndex; // 0 = home(local) group, 1 = away(LFG etc.) group

    return &_worldPacket;
}

void WorldPackets::Party::PartyInviteClient::Read()
{
    _worldPacket.read_skip<uint32>();
    _worldPacket.read_skip<uint8>();
    _worldPacket.read_skip<uint32>();

    CrossRealmGuid[7] = _worldPacket.ReadBit();
    uint8 realmLen = _worldPacket.ReadBits(9);
    CrossRealmGuid[3] = _worldPacket.ReadBit();
    uint8 nameLen = _worldPacket.ReadBits(9);
    CrossRealmGuid[2] = _worldPacket.ReadBit();
    CrossRealmGuid[5] = _worldPacket.ReadBit();
    CrossRealmGuid[4] = _worldPacket.ReadBit();
    CrossRealmGuid[0] = _worldPacket.ReadBit();
    CrossRealmGuid[1] = _worldPacket.ReadBit();
    CrossRealmGuid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(CrossRealmGuid[7]);
    _worldPacket.ReadByteSeq(CrossRealmGuid[6]);
    _worldPacket.ReadByteSeq(CrossRealmGuid[0]);
    _worldPacket.ReadByteSeq(CrossRealmGuid[4]);
    RealmName = _worldPacket.ReadString(realmLen);
    _worldPacket.ReadByteSeq(CrossRealmGuid[1]);
    _worldPacket.ReadByteSeq(CrossRealmGuid[2]);
    _worldPacket.ReadByteSeq(CrossRealmGuid[3]);
    MemberName = _worldPacket.ReadString(nameLen);
    _worldPacket.ReadByteSeq(CrossRealmGuid[5]);
}

void WorldPackets::Party::GroupInviteResponse::Read()
{
    _worldPacket.read_skip<uint8>();
    Unknown = _worldPacket.ReadBit();
    Accept = _worldPacket.ReadBit();
}

void WorldPackets::Party::GroupUninviteGuid::Read()
{
    _worldPacket.read_skip<uint8>();

    Guid[6] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    uint8 reasonLen = _worldPacket.ReadBits(8);
    Reason = _worldPacket.ReadString(reasonLen);

    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[0]);
}

void WorldPackets::Party::GroupSetLeader::Read()
{
    _worldPacket.read_skip<uint8>();

    Guid[1] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Party::GroupSetRoles::Read()
{
    _worldPacket.read_skip<uint8>();
    _worldPacket >> NewRole;

    TargetGuid[2] = _worldPacket.ReadBit();
    TargetGuid[0] = _worldPacket.ReadBit();
    TargetGuid[7] = _worldPacket.ReadBit();
    TargetGuid[4] = _worldPacket.ReadBit();
    TargetGuid[1] = _worldPacket.ReadBit();
    TargetGuid[3] = _worldPacket.ReadBit();
    TargetGuid[6] = _worldPacket.ReadBit();
    TargetGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGuid[1]);
    _worldPacket.ReadByteSeq(TargetGuid[5]);
    _worldPacket.ReadByteSeq(TargetGuid[2]);
    _worldPacket.ReadByteSeq(TargetGuid[6]);
    _worldPacket.ReadByteSeq(TargetGuid[7]);
    _worldPacket.ReadByteSeq(TargetGuid[0]);
    _worldPacket.ReadByteSeq(TargetGuid[4]);
    _worldPacket.ReadByteSeq(TargetGuid[3]);
}

void WorldPackets::Party::GroupDisband::Read()
{
    _worldPacket.read_skip<uint8>();
}

void WorldPackets::Party::LootMethod::Read()
{
    _worldPacket.read_skip<uint8>();
    _worldPacket >> Method;
    _worldPacket >> LootThreshold;

    LootMaster[7] = _worldPacket.ReadBit();
    LootMaster[1] = _worldPacket.ReadBit();
    LootMaster[2] = _worldPacket.ReadBit();
    LootMaster[0] = _worldPacket.ReadBit();
    LootMaster[4] = _worldPacket.ReadBit();
    LootMaster[5] = _worldPacket.ReadBit();
    LootMaster[6] = _worldPacket.ReadBit();
    LootMaster[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(LootMaster[7]);
    _worldPacket.ReadByteSeq(LootMaster[1]);
    _worldPacket.ReadByteSeq(LootMaster[3]);
    _worldPacket.ReadByteSeq(LootMaster[4]);
    _worldPacket.ReadByteSeq(LootMaster[6]);
    _worldPacket.ReadByteSeq(LootMaster[5]);
    _worldPacket.ReadByteSeq(LootMaster[0]);
    _worldPacket.ReadByteSeq(LootMaster[2]);
}

void WorldPackets::Party::MinimapPingClient::Read()
{
    _worldPacket >> Y;
    _worldPacket >> X;
    _worldPacket.read_skip<uint8>();
}

void WorldPackets::Party::RandomRollClient::Read()
{
    _worldPacket >> Maximum;
    _worldPacket >> Minimum;
    _worldPacket.read_skip<uint8>();
}

void WorldPackets::Party::GroupRaidConvert::Read()
{
    ToRaid = _worldPacket.ReadBit();
}

void WorldPackets::Party::GroupChangeSubGroup::Read()
{
    _worldPacket >> GroupNr >> GroupIdx;

    Guid[1] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
}

void WorldPackets::Party::GroupAssistantLeader::Read()
{
    _worldPacket >> Unk;

    Guid[2] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Apply = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Party::SetEveryoneIsAssistant::Read()
{
    _worldPacket.read_skip<uint8>();
    Apply = _worldPacket.ReadBit();
}

void WorldPackets::Party::RaidReadyCheck::Read()
{
    _worldPacket >> PartyIndex;
}

void WorldPackets::Party::RaidReadyCheckConfirm::Read()
{
    _worldPacket.read_skip<uint8>();

    Guid[2] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Status = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[6]);
}

void WorldPackets::Party::RequestPartyMemberStats::Read()
{
    _worldPacket.read_skip<uint8>();

    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
}

void WorldPackets::Party::OptOutOfLoot::Read()
{
    PassOnLoot = _worldPacket.ReadBit();
}

void WorldPackets::Party::RolePollBegin::Read()
{
    _worldPacket >> Index;
}

void WorldPackets::Party::GroupRequestJoinUpdates::Read()
{
    _worldPacket >> PartyIndex;
}

void WorldPackets::Party::ClearRaidMarker::Read()
{
    _worldPacket >> MarkerId;
}
