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

#include "BattlePetPackets.h"

namespace
{
    constexpr uint8 BATTLE_PET_MAX_DECLINED_NAMES = 5;
}

void WorldPackets::BattlePet::BattlePetSetBattleSlot::Read()
{
    _worldPacket >> Slot;

    PetEntry[4] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();
    PetEntry[3] = _worldPacket.ReadBit();
    PetEntry[1] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();
    PetEntry[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetEntry[5]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    _worldPacket.ReadByteSeq(PetEntry[6]);
    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
}

void WorldPackets::BattlePet::BattlePetSetFlags::Read()
{
    _worldPacket >> Flag;

    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();
    Mode = _worldPacket.ReadBits(2);
    PetEntry[1] = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    PetEntry[3] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetEntry[6]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
    _worldPacket.ReadByteSeq(PetEntry[5]);
}

void WorldPackets::BattlePet::BattlePetSummon::Read()
{
    PetEntry[3] = _worldPacket.ReadBit();
    PetEntry[2] = _worldPacket.ReadBit();
    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();
    PetEntry[1] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetEntry[6]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetEntry[5]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
}

void WorldPackets::BattlePet::QueryBattlePetName::Read()
{
    PetGuid[2] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    PetEntry[3] = _worldPacket.ReadBit();
    PetGuid[3] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();
    PetGuid[4] = _worldPacket.ReadBit();
    PetGuid[1] = _worldPacket.ReadBit();
    PetGuid[0] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();
    PetGuid[7] = _worldPacket.ReadBit();
    PetGuid[5] = _worldPacket.ReadBit();
    PetGuid[6] = _worldPacket.ReadBit();
    PetEntry[1] = _worldPacket.ReadBit();
    PetEntry[2] = _worldPacket.ReadBit();
    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetGuid[5]);
    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetGuid[0]);
    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetGuid[3]);
    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetGuid[1]);
    _worldPacket.ReadByteSeq(PetGuid[6]);
    _worldPacket.ReadByteSeq(PetEntry[6]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
    _worldPacket.ReadByteSeq(PetGuid[7]);
    _worldPacket.ReadByteSeq(PetGuid[2]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    _worldPacket.ReadByteSeq(PetGuid[4]);
    _worldPacket.ReadByteSeq(PetEntry[5]);
}

void WorldPackets::BattlePet::BattlePetDeletePet::Read()
{
    PetEntry[3] = _worldPacket.ReadBit();
    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    PetEntry[2] = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();
    PetEntry[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetEntry[5]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetEntry[6]);
}

void WorldPackets::BattlePet::BattlePetLearn::Read()
{
    ItemGuid[7] = _worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();
    ItemGuid[5] = _worldPacket.ReadBit();
    ItemGuid[4] = _worldPacket.ReadBit();
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    ItemGuid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(ItemGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);
    _worldPacket.ReadByteSeq(ItemGuid[7]);
    _worldPacket.ReadByteSeq(ItemGuid[0]);
    _worldPacket.ReadByteSeq(ItemGuid[6]);
}

void WorldPackets::BattlePet::BattlePetModifyName::Read()
{
    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();
    PetEntry[3] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    NicknameLen = _worldPacket.ReadBits(7);
    PetEntry[2] = _worldPacket.ReadBit();
    PetEntry[1] = _worldPacket.ReadBit();
    HasDeclinedNames = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();

    DeclinedNameLens.resize(BATTLE_PET_MAX_DECLINED_NAMES);
    if (HasDeclinedNames)
        for (uint8 i = 0; i < BATTLE_PET_MAX_DECLINED_NAMES; i++)
            DeclinedNameLens[i] = _worldPacket.ReadBits(7);

    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
    _worldPacket.ReadByteSeq(PetEntry[6]);
    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetEntry[5]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    Nickname = _worldPacket.ReadString(NicknameLen);

    DeclinedNames.resize(BATTLE_PET_MAX_DECLINED_NAMES);
    if (HasDeclinedNames)
        for (uint8 i = 0; i < BATTLE_PET_MAX_DECLINED_NAMES; i++)
            DeclinedNames[i] = _worldPacket.ReadString(DeclinedNameLens[i]);
}

void WorldPackets::BattlePet::CageBattlePet::Read()
{
    PetEntry[5] = _worldPacket.ReadBit();
    PetEntry[1] = _worldPacket.ReadBit();
    PetEntry[4] = _worldPacket.ReadBit();
    PetEntry[3] = _worldPacket.ReadBit();
    PetEntry[0] = _worldPacket.ReadBit();
    PetEntry[6] = _worldPacket.ReadBit();
    PetEntry[2] = _worldPacket.ReadBit();
    PetEntry[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetEntry[5]);
    _worldPacket.ReadByteSeq(PetEntry[4]);
    _worldPacket.ReadByteSeq(PetEntry[2]);
    _worldPacket.ReadByteSeq(PetEntry[7]);
    _worldPacket.ReadByteSeq(PetEntry[1]);
    _worldPacket.ReadByteSeq(PetEntry[3]);
    _worldPacket.ReadByteSeq(PetEntry[6]);
    _worldPacket.ReadByteSeq(PetEntry[0]);
}

void WorldPackets::BattlePet::PetBattleInput::Read()
{
    bool hasAbilityId = !_worldPacket.ReadBit();
    bool hasNewFrontPet = !_worldPacket.ReadBit();
    bool hasUnk19 = !_worldPacket.ReadBit();
    bool hasIgnoreAbandonPenalty = _worldPacket.ReadBit();
    bool hasUnk18 = !_worldPacket.ReadBit();
    bool hasRound = !_worldPacket.ReadBit();
    bool hasMoveType = !_worldPacket.ReadBit();

    uint32 abilityId = 0;
    uint8 newFrontPet = 0;
    uint8 unk19 = 0;
    uint8 unk18 = 0;
    uint32 round = 0;
    uint8 moveType = 0;

    if (hasMoveType)
        _worldPacket >> moveType;
    if (hasUnk19)
        _worldPacket >> unk19;
    if (hasRound)
        _worldPacket >> round;
    if (hasUnk18)
        _worldPacket >> unk18;
    if (hasAbilityId)
        _worldPacket >> abilityId;
    if (hasNewFrontPet)
        _worldPacket >> newFrontPet;

    HasAbilityId = hasAbilityId;
    HasNewFrontPet = hasNewFrontPet;
    AbilityId = abilityId;
    NewFrontPet = newFrontPet;
    MoveType = moveType;
}

void WorldPackets::BattlePet::PetBattleRequestWild::Read()
{
    ObjectGuid guid;

    for (uint8 i = 0; i < 2; i++)
        _worldPacket >> TeamPositions[i][0] >> TeamPositions[i][2] >> TeamPositions[i][1];

    _worldPacket >> BattleOrigin[2] >> BattleOrigin[1] >> BattleOrigin[0];

    guid[0] = _worldPacket.ReadBit();
    bool hasBattleFacing = !_worldPacket.ReadBit();
    guid[6] = _worldPacket.ReadBit();
    guid[3] = _worldPacket.ReadBit();
    guid[5] = _worldPacket.ReadBit();
    guid[2] = _worldPacket.ReadBit();
    guid[7] = _worldPacket.ReadBit();
    guid[1] = _worldPacket.ReadBit();
    guid[4] = _worldPacket.ReadBit();

    bool hasLocationResult = !_worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(guid[3]);
    _worldPacket.ReadByteSeq(guid[6]);
    _worldPacket.ReadByteSeq(guid[5]);
    _worldPacket.ReadByteSeq(guid[2]);
    _worldPacket.ReadByteSeq(guid[7]);
    _worldPacket.ReadByteSeq(guid[1]);
    _worldPacket.ReadByteSeq(guid[0]);
    _worldPacket.ReadByteSeq(guid[4]);

    OpponentGuid = guid;

    if (hasBattleFacing)
        _worldPacket >> BattleFacing;

    if (hasLocationResult)
        _worldPacket >> LocationResult;
}

void WorldPackets::BattlePet::PetBattleSetFrontPet::Read()
{
    PetNum = _worldPacket.read<uint8>();
}
