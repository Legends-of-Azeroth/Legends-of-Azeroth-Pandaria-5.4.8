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

#include "Common.h"
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "GossipDef.h"
#include "UpdateMask.h"
#include "ObjectAccessor.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Pet.h"
#include "ReputationMgr.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"

#ifdef ELUNA
#include "HookMgr.h"
#endif
enum StableResultCode
{
    STABLE_ERR_MONEY        = 0x01,                         // "you don't have enough money"
    STABLE_ERR_INVALID_SLOT = 0x03,                         // "That slot is locked"
    STABLE_SUCCESS_STABLE   = 0x08,                         // stable success
    STABLE_SUCCESS_UNSTABLE = 0x09,                         // unstable/swap success
    STABLE_SUCCESS_BUY_SLOT = 0x0A,                         // buy slot success
    STABLE_ERR_EXOTIC       = 0x0B,                         // "you are unable to control exotic creatures"
    STABLE_ERR_STABLE       = 0x0C,                         // "Internal pet error"
};

void WorldSession::HandleTabardVendorActivateOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_TABARD_VENDOR_ACTIVATE");

    ObjectGuid guid;
    guid[2] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TABARDDESIGNER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleTabardVendorActivateOpcode - Unit (GUID: %u) not found or you can not interact with him.", guid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendTabardVendorActivate(guid);
}

void WorldSession::SendTabardVendorActivate(ObjectGuid guid)
{
    WorldPacket data(SMSG_TABARD_VENDOR_ACTIVATE, 8);

    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);

    SendPacket(&data);
}

void WorldSession::HandleBankerActivateOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BANKER_ACTIVATE");

    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[4]);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_BANKER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleBankerActivateOpcode - Unit (GUID: %u) not found or you can not interact with him.", guid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendShowBank(guid);
}

void WorldSession::SendShowBank(ObjectGuid guid)
{
    WorldPacket data(SMSG_SHOW_BANK, 1 + 8);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[2]);
    SendPacket(&data);
}

void WorldSession::SendShowMailBox(ObjectGuid guid)
{
    WorldPacket data(SMSG_SHOW_MAILBOX, 8);
    data << guid;
    SendPacket(&data);
}


void WorldSession::HandleTrainerListOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[0] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);

    SendTrainerList(guid);
}

void WorldSession::SendTrainerList(ObjectGuid guid)
{
    std::string str = GetTrinityString(LANG_NPC_TAINER_HELLO);
    SendTrainerList(guid, str);
}

void WorldSession::SendTrainerList(ObjectGuid guid, const std::string& strTitle, bool multiple, uint32 mTrainer)
{
    TC_LOG_DEBUG("network", "WORLD: SendTrainerList");

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: SendTrainerList - Unit (GUID: %u) not found or you can not interact with him.", guid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    CreatureTemplate const* ci = unit->GetCreatureTemplate();

    if (!ci)
    {
        TC_LOG_DEBUG("network", "WORLD: SendTrainerList - (GUID: %u) NO CREATUREINFO!", guid.GetCounter());
        return;
    }

    TrainerSpellData const* trainer_spells = multiple ? sObjectMgr->GetNpcTrainerSpells(mTrainer) : unit->GetTrainerSpells();
    if (!trainer_spells)
    {
        TC_LOG_DEBUG("network", "WORLD: SendTrainerList - Training spells not found for creature (GUID: %u Entry: %u)",
            guid.GetCounter(), unit->GetEntry());
        return;
    }

    ObjectGuid oGuid = guid;

    WorldPacket data(SMSG_TRAINER_LIST, 1 + 8 + 4 + (trainer_spells->spellList.size() * 30) + 4 + 4 + strTitle.size());
    data.WriteBit(oGuid[4]);
    data.WriteBit(oGuid[5]);

    size_t count_pos = data.bitwpos();
    data.WriteBits(0, 19);                  // placeholder

    data.WriteBits(strTitle.size(), 11);
    data.WriteBit(oGuid[6]);
    data.WriteBit(oGuid[2]);
    data.WriteBit(oGuid[7]);
    data.WriteBit(oGuid[1]);
    data.WriteBit(oGuid[3]);
    data.WriteBit(oGuid[0]);
    data.FlushBits();

    data.WriteByteSeq(oGuid[4]);

    // reputation discount
    float fDiscountMod = _player->GetReputationPriceDiscount(unit);

    uint32 count = 0;
    for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
    {
        TrainerSpell const* tSpell = &itr->second;

        bool valid = true;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (!tSpell->learnedSpell[i])
                continue;
            if (!_player->IsSpellFitByClassAndRace(tSpell->learnedSpell[i]) ||
                (tSpell->learnedSpell[i] == 40120 && _player->GetClass() != CLASS_DRUID)) // Idk why it hasn't class mask
            {
                valid = false;
                break;
            }
        }
        if (!valid)
            continue;

        data << uint8(tSpell->reqLevel);
        data << uint32(floor(tSpell->spellCost * fDiscountMod));
        data << uint32(tSpell->spell);      // learned spell (or cast-spell in profession case)

        // spells required (3 max)
        uint8 maxReq = 0;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (!tSpell->learnedSpell[i])
                continue;
            SpellsRequiringSpellMapBounds spellsRequired = sSpellMgr->GetSpellsRequiredForSpellBounds(tSpell->learnedSpell[i]);
            for (SpellsRequiringSpellMap::const_iterator itr2 = spellsRequired.first; itr2 != spellsRequired.second && maxReq < 3; ++itr2)
            {
                data << uint32(itr2->second);
                ++maxReq;
            }
            if (maxReq == 3)
                break;
        }
        while (maxReq < 3)
        {
            data << uint32(0);
            ++maxReq;
        }

        data << uint32(tSpell->reqSkill);
        data << uint32(tSpell->reqSkillValue);

        TrainerSpellState state = _player->GetTrainerSpellState(tSpell);
        data << uint8(state == TRAINER_SPELL_GREEN_DISABLED ? TRAINER_SPELL_GREEN : state);

        ++count;
    }

    data.WriteString(strTitle);
    data.WriteByteSeq(oGuid[6]);
    data.WriteByteSeq(oGuid[7]);
    data.WriteByteSeq(oGuid[1]);
    data.WriteByteSeq(oGuid[3]);
    data << unit->GetEntry();                  // TrainerID
    data.WriteByteSeq(oGuid[5]);
    data.WriteByteSeq(oGuid[0]);
    data.WriteByteSeq(oGuid[2]);
    data << uint32(trainer_spells->trainerType);

    data.PutBits(count_pos, count, 19);
    SendPacket(&data);
}

void WorldSession::HandleTrainerBuySpellOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    uint32 spellId;
    uint32 trainerId;

    recvData >> spellId >> trainerId;

    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_TRAINER_BUY_SPELL NpcGUID=%u, learn spell id is: %u", guid.GetCounter(), spellId);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_TRAINER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleTrainerBuySpellOpcode - Unit (GUID: %u) not found or you can not interact with him.", guid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // check present spell in trainer spell list
    TrainerSpellData const* trainer_spells = unit->GetTrainerSpells();
    if (!trainer_spells)
    {
        SendTrainerBuyFailed(guid, spellId, 0);
        return;
    }

    // not found, cheat?
    TrainerSpell const* trainer_spell = trainer_spells->Find(spellId);
    if (!trainer_spell)
    {
        SendTrainerBuyFailed(guid, spellId, 0);
        return;
    }

    // can't be learn, cheat? Or double learn with lags...
    if (_player->GetTrainerSpellState(trainer_spell) != TRAINER_SPELL_GREEN)
    {
        SendTrainerBuyFailed(guid, spellId, 0);
        return;
    }

    // apply reputation discount
    uint32 nSpellCost = uint32(floor(trainer_spell->spellCost * _player->GetReputationPriceDiscount(unit)));

    // check money requirement
    if (!_player->HasEnoughMoney(uint64(nSpellCost)))
    {
        SendTrainerBuyFailed(guid, spellId, 1);
        return;
    }

    _player->ModifyMoney(-int64(nSpellCost));

    unit->SendPlaySpellVisualKit(179, 0, 0);       // 53 SpellCastDirected
    _player->SendPlaySpellVisualKit(362, 1, 0);    // 113 EmoteSalute

    // learn explicitly or cast explicitly
    if (trainer_spell->IsCastable())
        _player->CastSpell(_player, trainer_spell->spell, true);
    else
        _player->LearnSpell(spellId, false);

    _player->SetSaveTimer(1);
}

void WorldSession::SendTrainerBuyFailed(ObjectGuid guid, uint32 spellId, uint32 reason)
{
    WorldPacket data(SMSG_TRAINER_BUY_FAILED, 8 + 4 + 4);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);

    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);
    data << uint32(reason);         // 1 == "Not enough money for trainer service." 0 == "Trainer service %d unavailable."
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data << uint32(spellId);        // should be same as in packet from client
    SendPacket(&data);
}

void WorldSession::HandleGossipHelloOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_GOSSIP_HELLO");

    ObjectGuid guid;
    guid[2] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[2]);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_GOSSIP); // UNIT_NPC_FLAG_NONE
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipHelloOpcode - Unit (GUID: %u) not found or you can not interact with him.", guid.GetCounter());
        return;
    }

    // set faction visible if needed
    if (FactionTemplateEntry const* factionTemplateEntry = sFactionTemplateStore.LookupEntry(unit->GetFaction()))
        _player->GetReputationMgr().SetVisible(factionTemplateEntry);

    GetPlayer()->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TALK);
    // remove fake death
    //if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
    //    GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (unit->IsArmorer() || unit->IsCivilian() || unit->IsQuestGiver() || unit->IsServiceProvider() || unit->IsGuard())
        unit->StopMoving();

    // If spiritguide, no need for gossip menu, just put player into resurrect queue
    if (unit->IsSpiritGuide())
    {
        Battleground* bg = _player->GetBattleground();
        if (bg)
        {
            bg->AddPlayerToResurrectQueue(unit->GetGUID(), _player->GetGUID());
            sBattlegroundMgr->SendAreaSpiritHealerQueryOpcode(_player, bg, unit->GetGUID());
            return;
        }
    }

    _player->PlayerTalkClass->ClearMenus();
    if (!sScriptMgr->OnGossipHello(_player, unit) && !unit->AI()->OnGossipHello(_player))
    {
        //_player->TalkedToCreature(unit->GetEntry(), unit->GetGUID());
        _player->PrepareGossipMenu(unit, unit->GetGossipMenuId(), true);
        _player->SendPreparedGossip(unit);
    }
//     if (!unit->AI()->OnGossipHello(_player)) // hack, add to support ScriptAI
//     {
// //        _player->TalkedToCreature(unit->GetEntry(), unit->GetGUID());
//         _player->PrepareGossipMenu(unit, unit->GetCreatureTemplate()->GossipMenuId, true);
//         _player->SendPreparedGossip(unit);
//     }    
}

/*void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_GOSSIP_SELECT_OPTION");

    uint32 option;
    uint32 unk;
    uint64 guid;
    std::string code = "";

    recvData >> guid >> unk >> option;

    if (_player->PlayerTalkClass->GossipOptionCoded(option))
    {
        TC_LOG_DEBUG("network", "reading string");
        recvData >> code;
        TC_LOG_DEBUG("network", "string read: %s", code.c_str());
    }

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - Unit (GUID: %u) not found or you can't interact with him.", guid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (!code.empty())
    {
        if (!Script->GossipSelectWithCode(_player, unit, _player->PlayerTalkClass->GossipOptionSender (option), _player->PlayerTalkClass->GossipOptionAction(option), code.c_str()))
            unit->OnGossipSelect (_player, option);
    }
    else
    {
        if (!Script->OnGossipSelect (_player, unit, _player->PlayerTalkClass->GossipOptionSender (option), _player->PlayerTalkClass->GossipOptionAction (option)))
           unit->OnGossipSelect (_player, option);
    }
}*/

void WorldSession::HandleSpiritHealerActivateOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SPIRIT_HEALER_ACTIVATE");

    ObjectGuid UnitGUID;

    UnitGUID[2] = recvData.ReadBit();
    UnitGUID[7] = recvData.ReadBit();
    UnitGUID[6] = recvData.ReadBit();
    UnitGUID[0] = recvData.ReadBit();
    UnitGUID[5] = recvData.ReadBit();
    UnitGUID[4] = recvData.ReadBit();
    UnitGUID[1] = recvData.ReadBit();
    UnitGUID[3] = recvData.ReadBit();

    recvData.ReadByteSeq(UnitGUID[1]);
    recvData.ReadByteSeq(UnitGUID[5]);
    recvData.ReadByteSeq(UnitGUID[6]);
    recvData.ReadByteSeq(UnitGUID[3]);
    recvData.ReadByteSeq(UnitGUID[2]);
    recvData.ReadByteSeq(UnitGUID[0]);
    recvData.ReadByteSeq(UnitGUID[7]);
    recvData.ReadByteSeq(UnitGUID[4]);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(UnitGUID, UNIT_NPC_FLAG_SPIRITHEALER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleSpiritHealerActivateOpcode - Unit (GUID: %u) not found or you can not interact with him.", UnitGUID.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);
#ifdef ELUNA
    sHookMgr->OnResurrect(GetPlayer());
#endif

    SendSpiritResurrect();
}

void WorldSession::SendSpiritResurrect()
{
    _player->ResurrectPlayer(0.5f, true);

    _player->DurabilityLossAll(0.25f, true);

    // get corpse nearest graveyard
    WorldSafeLocsEntry const* corpseGrave = NULL;
    WorldLocation corpseLocation = _player->GetCorpseLocation();
    if (_player->HasCorpse())
        corpseGrave = sObjectMgr->GetClosestGraveYard(corpseLocation, _player->GetTeam(), _player);

    // now can spawn bones
    _player->SpawnCorpseBones();

    // teleport to nearest from corpse graveyard, if different from nearest to player ghost
    if (corpseGrave)
    {
        WorldSafeLocsEntry const* ghostGrave = sObjectMgr->GetClosestGraveYard(*_player, _player->GetTeam(), _player);

        if (corpseGrave != ghostGrave)
            _player->TeleportTo(corpseGrave->map_id, corpseGrave->x, corpseGrave->y, corpseGrave->z, _player->GetOrientation());
        // or update at original position
        else
            _player->UpdateObjectVisibility();
    }
    // or update at original position
    else
        _player->UpdateObjectVisibility();
}

void WorldSession::HandleBinderActivateOpcode(WorldPacket& recvData)
{
    ObjectGuid npcGuid;

    npcGuid[0] = recvData.ReadBit();
    npcGuid[5] = recvData.ReadBit();
    npcGuid[4] = recvData.ReadBit();
    npcGuid[7] = recvData.ReadBit();
    npcGuid[6] = recvData.ReadBit();
    npcGuid[2] = recvData.ReadBit();
    npcGuid[1] = recvData.ReadBit();
    npcGuid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(npcGuid[0]);
    recvData.ReadByteSeq(npcGuid[4]);
    recvData.ReadByteSeq(npcGuid[2]);
    recvData.ReadByteSeq(npcGuid[3]);
    recvData.ReadByteSeq(npcGuid[7]);
    recvData.ReadByteSeq(npcGuid[1]);
    recvData.ReadByteSeq(npcGuid[5]);
    recvData.ReadByteSeq(npcGuid[6]);

    if (!GetPlayer()->IsInWorld() || !GetPlayer()->IsAlive())
        return;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_INNKEEPER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleBinderActivateOpcode - Unit (GUID: %u) not found or you can not interact with him.", npcGuid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendBindPoint(unit);
}

void WorldSession::SendBindPoint(Creature* npc)
{
    // prevent set homebind to instances in any case
    if (GetPlayer()->GetMap()->Instanceable())
        return;

    uint32 bindspell = 3286;

    // send spell for homebinding (3286)
    npc->CastSpell(_player, bindspell, true);
    
    _player->PlayerTalkClass->SendCloseGossip();
}

void WorldSession::HandleListPets(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recv MSG_LIST_STABLED_PETS");
    ObjectGuid guid;
    guid[0] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);

    if (!CheckStableMaster(guid))
        return;

    if (GetPlayer()->IsMounted())
        GetPlayer()->RemoveAurasByType(SPELL_AURA_MOUNTED);

    SendPetList(guid, PET_SLOT_ACTIVE_FIRST, PET_SLOT_STABLE_LAST);
}

void WorldSession::SendPetList(ObjectGuid guid, uint8 first, uint8 last)
{
    WorldPacket data(SMSG_PET_STABLE_LIST, 200);           // guess size
    ByteBuffer buff;
    data.WriteBit(guid[3]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[5]);
    size_t pos = data.bitwpos();
    data.WriteBits(0, 19);

    uint32 count = 0;

    for (auto&& itr : _player->m_petList)
    {
        if (itr.first < first && itr.first > last)
            continue;

        ++count;
        auto& petData = itr.second;
        data.WriteBits(petData.Name.size(), 8);

        // 1 = current, 2/3 = in stable (any from 4, 5, ... create problems with proper show)
        uint8 petStableState = itr.first <= PET_SLOT_ACTIVE_LAST ? 1 : 2;

        uint32 modelId = 0;

        if (CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(petData.Entry))
        {
            if (CreatureModel const* model1 = cInfo->GetModelByIdx(0))
                modelId = model1->CreatureDisplayID;
            else
            {
                CreatureModel const* model2 = cInfo->GetModelByIdx(1);
                modelId = model2->CreatureDisplayID;
            }
        }

        buff << uint32(petData.Entry);
        buff << uint32(_player->GetLevel());    // All pets have equal level
        buff << uint8(petStableState);
        buff << uint32(modelId);
        buff.WriteString(petData.Name);
        buff << uint32(petData.PetNumber);
        buff << uint32(itr.first);
    }

    data.FlushBits();
    data.PutBits(pos, count, 19);
    data.append(buff);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    SendPacket(&data);
}

void WorldSession::SendStableResult(uint8 res)
{
    WorldPacket data(SMSG_PET_STABLE_RESULT, 1);
    data << uint8(res);
    SendPacket(&data);
}

void WorldSession::HandleSetPetSlot(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_SET_PET_SLOT");

    uint32 oldPetId;
    int8 newSlot;
    recvData >> oldPetId;
    recvData >> newSlot;
    ObjectGuid npcGUID;
    npcGUID[5] = recvData.ReadBit();
    npcGUID[7] = recvData.ReadBit();
    npcGUID[3] = recvData.ReadBit();
    npcGUID[2] = recvData.ReadBit();
    npcGUID[6] = recvData.ReadBit();
    npcGUID[1] = recvData.ReadBit();
    npcGUID[0] = recvData.ReadBit();
    npcGUID[4] = recvData.ReadBit();
    recvData.ReadByteSeq(npcGUID[0]);
    recvData.ReadByteSeq(npcGUID[3]);
    recvData.ReadByteSeq(npcGUID[2]);
    recvData.ReadByteSeq(npcGUID[6]);
    recvData.ReadByteSeq(npcGUID[5]);
    recvData.ReadByteSeq(npcGUID[7]);
    recvData.ReadByteSeq(npcGUID[4]);
    recvData.ReadByteSeq(npcGUID[1]);

    if (!CheckStableMaster(npcGUID) || newSlot < 0 || newSlot > PET_SLOT_STABLE_LAST)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    int8 oldSlot = GetPlayer()->GetSlotByPetId(oldPetId);
    if (oldSlot == -1)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    bool removePet = false;

    if (oldSlot == GetPlayer()->GetCurrentPetSlot())
        removePet = newSlot >= PET_SLOT_STABLE_FIRST;
    else if (newSlot == GetPlayer()->GetCurrentPetSlot())
        removePet = oldSlot >= PET_SLOT_STABLE_FIRST;

    if (removePet)
    {
        Pet* pet = GetPlayer()->GetPet();
        // can't place in stable dead pet
        if (!pet || !pet->IsAlive() || pet->getPetType() != HUNTER_PET)
        {
            SendStableResult(STABLE_ERR_STABLE);
            return;
        }
        GetPlayer()->RemovePet(PET_REMOVE_DISMISS, PET_REMOVE_FLAG_RESET_CURRENT);
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
    stmt->setUInt8(0, newSlot);
    stmt->setUInt32(1, GetPlayer()->GetGUID().GetCounter());
    stmt->setUInt32(2, oldPetId);
    trans->Append(stmt);

    uint32 newPetId = GetPlayer()->GetPetIdBySlot(newSlot);
    if (newPetId)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, oldSlot);
        stmt->setUInt32(1, GetPlayer()->GetGUID().GetCounter());
        stmt->setUInt32(2, newPetId);
        trans->Append(stmt);
    }

    GetPlayer()->SwapPets(newSlot, oldSlot);

    CharacterDatabase.CommitTransaction(trans);
    SendStableResult(STABLE_SUCCESS_STABLE);
}

void WorldSession::HandleStablePetCallback(PreparedQueryResult result)
{
    /*if (!GetPlayer())
        return;

    uint8 freeSlot = 1;
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint8 slot = fields[1].GetUInt8();

            // slots ordered in query, and if not equal then free
            if (slot != freeSlot)
                break;

            // this slot not free, skip
            ++freeSlot;
        }
        while (result->NextRow());
    }

    if (freeSlot > 0 && freeSlot <= GetPlayer()->m_stableSlots)
    {
        _player->RemovePet(_player->GetPet(), PetSaveMode(freeSlot));
        SendStableResult(STABLE_SUCCESS_STABLE);
    }
    else
        SendStableResult(STABLE_ERR_INVALID_SLOT);*/
}

void WorldSession::HandleUnstablePet(WorldPacket& recvData)
{
    /*TC_LOG_DEBUG("network", "WORLD: Recv CMSG_UNSTABLE_PET.");
    uint64 npcGUID;
    uint32 petnumber;

    recvData >> npcGUID >> petnumber;

    if (!CheckStableMaster(npcGUID))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_ENTRY);

    stmt->setUInt32(0, _player->GetGUID().GetCounter());
    stmt->setUInt32(1, petnumber);
    stmt->setUInt8(2, PET_SAVE_FIRST_STABLE_SLOT);
    stmt->setUInt8(3, PET_SAVE_LAST_STABLE_SLOT);

    _unstablePetCallback.SetParam(petnumber);
    _unstablePetCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));*/
}

void WorldSession::HandleUnstablePetCallback(PreparedQueryResult result, uint32 petId)
{
    /*if (!GetPlayer())
        return;

    uint32 petEntry = 0;
    if (result)
    {
        Field* fields = result->Fetch();
        petEntry = fields[0].GetUInt32();
    }

    if (!petEntry)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(petEntry);
    if (!creatureInfo || !creatureInfo->IsTameable(_player->CanTameExoticPets()))
    {
        // if problem in exotic pet
        if (creatureInfo && creatureInfo->IsTameable(true))
            SendStableResult(STABLE_ERR_EXOTIC);
        else
            SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    Pet* pet = _player->GetPet();
    if (pet && pet->IsAlive())
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // delete dead pet
    if (pet)
        _player->RemovePet(pet, PET_SAVE_AS_DELETED);

    Pet* newPet = new Pet(_player, HUNTER_PET);
    if (!newPet->LoadPetFromDB(_player, petEntry, petId))
    {
        delete newPet;
        newPet = NULL;
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    SendStableResult(STABLE_SUCCESS_UNSTABLE);*/
}

void WorldSession::HandleBuyStableSlot(WorldPacket& recvData)
{
    /*TC_LOG_DEBUG("network", "WORLD: Recv CMSG_BUY_STABLE_SLOT.");
    uint64 npcGUID;

    recvData >> npcGUID;

    if (!CheckStableMaster(npcGUID))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (GetPlayer()->m_stableSlots < MAX_PET_STABLES)
    {
        /*StableSlotPricesEntry const* SlotPrice = sStableSlotPricesStore.LookupEntry(GetPlayer()->m_stableSlots+1);
        if (_player->HasEnoughMoney(SlotPrice->Price))
        {
            ++GetPlayer()->m_stableSlots;
            _player->ModifyMoney(-int32(SlotPrice->Price));
            SendStableResult(STABLE_SUCCESS_BUY_SLOT);
        }
        else
            SendStableResult(STABLE_ERR_MONEY);*/
    /*}
    else
        SendStableResult(STABLE_ERR_STABLE);*/
}

void WorldSession::HandleStableRevivePet(WorldPacket &/* recvData */)
{
    TC_LOG_DEBUG("network", "HandleStableRevivePet: Not implemented");
}

void WorldSession::HandleStableSwapPet(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_STABLE_SWAP_PET.");
    uint64 npcGUID;
    uint32 petId;

    recvData >> npcGUID >> petId;

    if (!CheckStableMaster(ObjectGuid(npcGUID)))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    Pet* pet = _player->GetPet();

    if (!pet || pet->getPetType() != HUNTER_PET)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // Find swapped pet slot in stable

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PET_SLOT_BY_ID);

    stmt->setUInt32(0, _player->GetGUID().GetCounter());
    stmt->setUInt32(1, petId);

    // _stableSwapCallback.SetParam(petId);
    // _stableSwapCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));

    _queryProcessor.AddCallback(CharacterDatabase.AsyncQuery(stmt)
        .WithPreparedCallback(std::bind(&WorldSession::HandleStableSwapPetCallback, this, std::placeholders::_1, petId )));

}

void WorldSession::HandleStableSwapPetCallback(PreparedQueryResult result, uint32 petId)
{
    /*if (!GetPlayer())
        return;

    if (!result)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    Field* fields = result->Fetch();

    uint32 slot     = fields[0].GetUInt8();
    uint32 petEntry = fields[1].GetUInt32();

    if (!petEntry)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(petEntry);
    if (!creatureInfo || !creatureInfo->IsTameable(true))
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    if (!creatureInfo->IsTameable(_player->CanTameExoticPets()))
    {
        SendStableResult(STABLE_ERR_EXOTIC);
        return;
    }

    Pet* pet = _player->GetPet();
    // The player's pet could have been removed during the delay of the DB callback
    if (!pet)
    {
        SendStableResult(STABLE_ERR_STABLE);
        return;
    }

    // move alive pet to slot or delete dead pet
    _player->RemovePet(pet, pet->IsAlive() ? PetSaveMode(slot) : PET_SAVE_AS_DELETED);

    // summon unstabled pet
    Pet* newPet = new Pet(_player);
    if (!newPet->LoadPetFromDB(_player, petEntry, petId))
    {
        delete newPet;
        SendStableResult(STABLE_ERR_STABLE);
    }
    else
        SendStableResult(STABLE_SUCCESS_UNSTABLE);*/
}

void WorldSession::HandleRepairItemOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REPAIR_ITEM");

    ObjectGuid npcGuid, itemGuid;
    bool guildBank;                                         // new in 2.3.2, bool that means from guild bank money

    itemGuid[2] = recvData.ReadBit();
    itemGuid[5] = recvData.ReadBit();
    npcGuid[3] = recvData.ReadBit();
    guildBank = recvData.ReadBit();
    npcGuid[7] = recvData.ReadBit();
    itemGuid[4] = recvData.ReadBit();
    npcGuid[2] = recvData.ReadBit();
    itemGuid[0] = recvData.ReadBit();
    itemGuid[3] = recvData.ReadBit();
    npcGuid[6] = recvData.ReadBit();
    npcGuid[1] = recvData.ReadBit();
    npcGuid[4] = recvData.ReadBit();
    itemGuid[6] = recvData.ReadBit();
    npcGuid[5] = recvData.ReadBit();
    npcGuid[0] = recvData.ReadBit();
    itemGuid[7] = recvData.ReadBit();
    itemGuid[1] = recvData.ReadBit();

    recvData.ReadByteSeq(itemGuid[2]);
    recvData.ReadByteSeq(npcGuid[1]);
    recvData.ReadByteSeq(itemGuid[1]);
    recvData.ReadByteSeq(npcGuid[4]);
    recvData.ReadByteSeq(npcGuid[7]);
    recvData.ReadByteSeq(npcGuid[3]);
    recvData.ReadByteSeq(npcGuid[2]);
    recvData.ReadByteSeq(itemGuid[7]);
    recvData.ReadByteSeq(npcGuid[5]);
    recvData.ReadByteSeq(npcGuid[0]);
    recvData.ReadByteSeq(itemGuid[5]);
    recvData.ReadByteSeq(itemGuid[3]);
    recvData.ReadByteSeq(itemGuid[4]);
    recvData.ReadByteSeq(itemGuid[6]);
    recvData.ReadByteSeq(npcGuid[6]);
    recvData.ReadByteSeq(itemGuid[0]);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_REPAIR);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleRepairItemOpcode - Unit (GUID: %u) not found or you can not interact with him.", npcGuid.GetCounter());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // reputation discount
    float discountMod = _player->GetReputationPriceDiscount(unit);

    if (itemGuid)
    {
        TC_LOG_DEBUG("network", "ITEM: Repair item, itemGUID = %u, npcGUID = %u", itemGuid.GetCounter(), npcGuid.GetCounter());

        Item* item = _player->GetItemByGuid(itemGuid);
        if (item)
            _player->DurabilityRepair(item->GetPos(), true, discountMod, guildBank);
    }
    else
    {
        TC_LOG_DEBUG("network", "ITEM: Repair all items, npcGUID = %u", npcGuid.GetCounter());
        _player->DurabilityRepairAll(true, discountMod, guildBank);
    }
}
