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

#ifndef TradeData_h__
#define TradeData_h__

enum TradeSlots
{
    TRADE_SLOT_COUNT          = 7,
    TRADE_SLOT_TRADED_COUNT   = 6,
    TRADE_SLOT_NONTRADED      = 6,
    TRADE_SLOT_INVALID        = -1
};

class Item;
class Player;

class TC_GAME_API TradeData
{
public:                                                 // constructors
    TradeData(Player* player, Player* trader, Player* initiator) :
        m_player(player), m_trader(trader), m_initiator(initiator), m_accepted(false), m_acceptProccess(false),
        m_money(0), m_spell(0), m_spellCastItem()
    {
        //memset(m_items, 0, TRADE_SLOT_COUNT * sizeof(uint64));
    }

    Player* GetTrader() const { return m_trader; }
    TradeData* GetTraderData() const;

    Item* GetItem(TradeSlots slot) const;
    bool HasItem(ObjectGuid itemGuid) const;
    TradeSlots GetTradeSlotForItem(ObjectGuid itemGuid) const;
    void SetItem(TradeSlots slot, Item* item, bool update = false);

    uint32 GetSpell() const { return m_spell; }
    void SetSpell(uint32 spell_id, Item* castItem = NULL);

    Item*  GetSpellCastItem() const;
    bool HasSpellCastItem() const { return !m_spellCastItem.IsEmpty(); }

    uint64 GetMoney() const { return m_money; }
    void SetMoney(uint64 money);

    bool IsAccepted() const { return m_accepted; }
    void SetAccepted(bool state, bool crosssend = false);

    bool IsInAcceptProcess() const { return m_acceptProccess; }
    void SetInAcceptProcess(bool state) { m_acceptProccess = state; }

    bool IsInitiator(Player const* player) const { return m_initiator == player; }

private:                                                // internal functions

    void Update(bool for_trader = true);

    private:                                                // fields

    Player*    m_player;                                // Player who own of this TradeData
    Player*    m_trader;                                // Player who trade with m_player
    Player*    m_initiator;                             // Player who first initiated this trade

    bool       m_accepted;                              // m_player press accept for trade list
    bool       m_acceptProccess;                        // one from player/trader press accept and this processed

    uint64     m_money;                                 // m_player place money to trade

    uint32     m_spell;                                 // m_player apply spell to non-traded slot item
    ObjectGuid m_spellCastItem;                         // applied spell casted by item use

    ObjectGuid m_items[TRADE_SLOT_COUNT];               // traded items from m_player side including non-traded slot
};

#endif // TradeData_h__