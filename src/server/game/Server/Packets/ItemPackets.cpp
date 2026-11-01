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

#include "ItemPackets.h"

#include <vector>

void WorldPackets::Item::SplitItem::Read()
{
    _worldPacket >> SrcBag;
    _worldPacket >> Count;
    _worldPacket >> DstBag >> SrcSlot >> DstSlot;
}

void WorldPackets::Item::SwapInvItem::Read()
{
    _worldPacket >> SrcSlot >> DstSlot;

    Size = _worldPacket.ReadBits(2);
    if (Size != 2)
        return;

    HasDstSlot = _worldPacket.ReadBit();
    HasDstBag = _worldPacket.ReadBit();
    HasSrcSlot = _worldPacket.ReadBit();
    HasSrcBag = _worldPacket.ReadBit();

    if (!HasDstSlot)
        _worldPacket >> CheckDstSlot;
    if (!HasDstBag)
        _worldPacket >> DstBag;
    if (!HasSrcSlot)
        _worldPacket >> CheckSrcSlot;
    if (!HasSrcBag)
        _worldPacket >> SrcBag;
}

void WorldPackets::Item::AutoEquipItemSlot::Read()
{
    _worldPacket >> Slot;
    ItemGuid[6] = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();
    uint32 length = _worldPacket.ReadBits(2);
    ItemGuid[7] = _worldPacket.ReadBit();

    struct Unk14
    {
        uint8 Unk0;
        uint8 Unk1;
    };

    std::vector<Unk14> unk14;
    for (uint32 i = 0; i < length; ++i)
    {
        Unk14 u;
        u.Unk1 = _worldPacket.ReadBit();
        u.Unk0 = _worldPacket.ReadBit();
        unk14.push_back(u);
    }
    ItemGuid[5] = _worldPacket.ReadBit();
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();
    ItemGuid[4] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(ItemGuid[6]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);
    _worldPacket.ReadByteSeq(ItemGuid[7]);
    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[0]);
    _worldPacket.ReadByteSeq(ItemGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);

    for (auto&& itr : unk14)
    {
        if (itr.Unk1)
            itr.Unk1 = _worldPacket.read<uint8>();
        if (itr.Unk0)
            itr.Unk0 = _worldPacket.read<uint8>();
    }
}

void WorldPackets::Item::SwapItem::Read()
{
    _worldPacket >> SrcSlotAlt >> SrcBagAlt >> DstBagAlt >> DstSlotAlt;

    Count = _worldPacket.ReadBits(2);
    if (Count != 2)
        return;

    for (uint8 i = 0; i < Count; i++)
    {
        HasSlot[i] = !_worldPacket.ReadBit();
        HasBag[i] = !_worldPacket.ReadBit();
    }

    DstBag = HasBag[0] ? _worldPacket.read<uint8>() : DstBagAlt;
    DstSlot = HasSlot[0] ? _worldPacket.read<uint8>() : DstSlotAlt;
    SrcBag = HasBag[1] ? _worldPacket.read<uint8>() : SrcBagAlt;
    SrcSlot = HasSlot[1] ? _worldPacket.read<uint8>() : SrcSlotAlt;
}

void WorldPackets::Item::AutoEquipItem::Read()
{
    _worldPacket >> SrcSlot;
    _worldPacket >> SrcBag;
}

void WorldPackets::Item::DestroyItem::Read()
{
    _worldPacket >> Count;
    _worldPacket >> Slot >> Bag;
}

void WorldPackets::Item::ReadItem::Read()
{
    _worldPacket >> Bag >> Slot;
}

void WorldPackets::Item::SellItem::Read()
{
    _worldPacket >> Count;

    ItemGuid[4] = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    ItemGuid[7] = _worldPacket.ReadBit();
    VendorGuid[6] = _worldPacket.ReadBit();
    VendorGuid[5] = _worldPacket.ReadBit();
    VendorGuid[1] = _worldPacket.ReadBit();
    ItemGuid[5] = _worldPacket.ReadBit();
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();
    VendorGuid[2] = _worldPacket.ReadBit();
    ItemGuid[6] = _worldPacket.ReadBit();
    VendorGuid[4] = _worldPacket.ReadBit();
    VendorGuid[0] = _worldPacket.ReadBit();
    VendorGuid[7] = _worldPacket.ReadBit();
    VendorGuid[3] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(VendorGuid[6]);
    _worldPacket.ReadByteSeq(VendorGuid[3]);
    _worldPacket.ReadByteSeq(VendorGuid[1]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);
    _worldPacket.ReadByteSeq(VendorGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[7]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);
    _worldPacket.ReadByteSeq(VendorGuid[7]);
    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(VendorGuid[0]);
    _worldPacket.ReadByteSeq(VendorGuid[5]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(ItemGuid[6]);
    _worldPacket.ReadByteSeq(VendorGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[0]);
}

void WorldPackets::Item::BuyBackItem::Read()
{
    _worldPacket >> Slot;

    VendorGuid[2] = _worldPacket.ReadBit();
    VendorGuid[3] = _worldPacket.ReadBit();
    VendorGuid[0] = _worldPacket.ReadBit();
    VendorGuid[4] = _worldPacket.ReadBit();
    VendorGuid[1] = _worldPacket.ReadBit();
    VendorGuid[7] = _worldPacket.ReadBit();
    VendorGuid[5] = _worldPacket.ReadBit();
    VendorGuid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(VendorGuid[0]);
    _worldPacket.ReadByteSeq(VendorGuid[6]);
    _worldPacket.ReadByteSeq(VendorGuid[1]);
    _worldPacket.ReadByteSeq(VendorGuid[7]);
    _worldPacket.ReadByteSeq(VendorGuid[5]);
    _worldPacket.ReadByteSeq(VendorGuid[2]);
    _worldPacket.ReadByteSeq(VendorGuid[3]);
    _worldPacket.ReadByteSeq(VendorGuid[4]);
}

void WorldPackets::Item::BuyItem::Read()
{
    _worldPacket >> BagSlot >> Count >> Item >> Slot;

    VendorGuid[6] = _worldPacket.ReadBit();
    BagGuid[6] = _worldPacket.ReadBit();
    BagGuid[4] = _worldPacket.ReadBit();
    VendorGuid[4] = _worldPacket.ReadBit();
    ItemType = _worldPacket.ReadBits(2);
    VendorGuid[0] = _worldPacket.ReadBit();
    VendorGuid[3] = _worldPacket.ReadBit();
    BagGuid[3] = _worldPacket.ReadBit();
    VendorGuid[7] = _worldPacket.ReadBit();
    VendorGuid[5] = _worldPacket.ReadBit();
    BagGuid[2] = _worldPacket.ReadBit();
    VendorGuid[1] = _worldPacket.ReadBit();
    BagGuid[7] = _worldPacket.ReadBit();
    VendorGuid[2] = _worldPacket.ReadBit();
    BagGuid[1] = _worldPacket.ReadBit();
    BagGuid[0] = _worldPacket.ReadBit();
    BagGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(VendorGuid[5]);
    _worldPacket.ReadByteSeq(VendorGuid[0]);
    _worldPacket.ReadByteSeq(BagGuid[3]);
    _worldPacket.ReadByteSeq(BagGuid[1]);
    _worldPacket.ReadByteSeq(BagGuid[6]);
    _worldPacket.ReadByteSeq(VendorGuid[2]);
    _worldPacket.ReadByteSeq(VendorGuid[7]);
    _worldPacket.ReadByteSeq(VendorGuid[6]);
    _worldPacket.ReadByteSeq(BagGuid[0]);
    _worldPacket.ReadByteSeq(BagGuid[5]);
    _worldPacket.ReadByteSeq(VendorGuid[4]);
    _worldPacket.ReadByteSeq(BagGuid[2]);
    _worldPacket.ReadByteSeq(VendorGuid[3]);
    _worldPacket.ReadByteSeq(BagGuid[7]);
    _worldPacket.ReadByteSeq(VendorGuid[1]);
    _worldPacket.ReadByteSeq(BagGuid[4]);
}

void WorldPackets::Item::AutoStoreBagItem::Read()
{
    _worldPacket >> SrcSlot >> SrcBag >> DstBag;
}

void WorldPackets::Item::WrapItem::Read()
{
    Size = _worldPacket.ReadBits(2);
    if (Size != 2)
        return;

    HasGiftSlot = !_worldPacket.ReadBit();
    HasGiftBag = !_worldPacket.ReadBit();
    HasItemSlot = !_worldPacket.ReadBit();
    HasItemBag = !_worldPacket.ReadBit();

    if (HasGiftSlot)
        _worldPacket >> GiftSlot;
    if (HasGiftBag)
        _worldPacket >> GiftBag;
    if (HasItemSlot)
        _worldPacket >> ItemSlot;
    if (HasItemBag)
        _worldPacket >> ItemBag;
}

void WorldPackets::Item::SocketGems::Read()
{
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][4] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][0] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][6] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][2] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][1] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][7] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][3] = _worldPacket.ReadBit();
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        GemGuids[i][5] = _worldPacket.ReadBit();

    ItemGuid[5] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();
    ItemGuid[6] = _worldPacket.ReadBit();
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    ItemGuid[4] = _worldPacket.ReadBit();
    ItemGuid[7] = _worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(ItemGuid[7]);
    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[6]);

    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][6]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][4]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][3]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][2]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][0]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][1]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][7]);
    for (int i = 0; i < MAX_GEM_SOCKETS; ++i)
        _worldPacket.ReadByteSeq(GemGuids[i][5]);

    _worldPacket.ReadByteSeq(ItemGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);
    _worldPacket.ReadByteSeq(ItemGuid[0]);
}

void WorldPackets::Item::GetItemPurchaseData::Read()
{
    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[2]);
}

void WorldPackets::Item::ItemPurchaseRefund::Read()
{
    Guid[2] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Item::ReforgeItem::Read()
{
    _worldPacket >> ReforgeEntry >> Bag >> Slot;

    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[5]);
}

void WorldPackets::Item::UpgradeItem::Read()
{
    _worldPacket >> ItemSlot >> ItemBag >> UpgradeEntry;

    ItemGuid[5] = _worldPacket.ReadBit();
    NpcGuid[6] = _worldPacket.ReadBit();
    ItemGuid[6] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();
    ItemGuid[7] = _worldPacket.ReadBit();
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    NpcGuid[5] = _worldPacket.ReadBit();
    ItemGuid[4] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(ItemGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(ItemGuid[6]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);
    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[0]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[5]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(ItemGuid[4]);
}

void WorldPackets::Item::OpenItem::Read()
{
    _worldPacket >> BagIndex >> Slot;
}

void WorldPackets::Item::RepairItem::Read()
{
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[5] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();
    GuildBank = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();
    ItemGuid[4] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    NpcGuid[6] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();
    ItemGuid[6] = _worldPacket.ReadBit();
    NpcGuid[5] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();
    ItemGuid[7] = _worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[5]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(ItemGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[6]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(ItemGuid[0]);
}

void WorldPackets::Item::ItemTextQuery::Read()
{
    _worldPacket >> ItemGuid;
}

void WorldPackets::Item::SetLootSpecialization::Read()
{
    _worldPacket >> LootSpecialization;
}
