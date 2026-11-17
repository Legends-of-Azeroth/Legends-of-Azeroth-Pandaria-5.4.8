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

#ifndef CHARACTER_BOOST_H
#define CHARACTER_BOOST_H

enum CharBoostMisc
{
    // Items
    ITEM_HEARTHSTONE                         = 6948,
    ITEM_EMBERSILK_BAG                       = 54443,
    ITEM_LEMON_FLAVOUR_PUDING                = 108920,
    // Spells
    SPELL_SWIFT_PURPLE_WIND_RIDER            = 32297,
    SPELL_SWIFT_PURPLE_GRYPGON               = 32292,
    // Misc
    MAP_VALE_OF_ETERNAL_BLOSSOMS             = 870,
    MAIL_CHARRACTER_BOOST_EQUIPED_ITEMS_BODY = 403,
    // Profession spells
    SPELL_FIRST_AID                          = 110406,
    SPELL_COOKING                            = 104381,
    SPELL_FISHING                            = 110410,
    SPELL_ARCHAEOLOGY                        = 110393,
    SPELL_TAILORING                          = 110426,
    SPELL_ENGINEERING                        = 110403,
    SPELL_ALCHEMY                            = 105206,
    SPELL_SKINNING                           = 102216,
    SPELL_MINING                             = 102161,
    SPELL_HERBALISM                          = 110413,
    SPELL_INSCRIPTION                        = 110417,
    SPELL_JEWELCRAFTING                      = 110420,
    SPELL_BLACKSMITHING                      = 110396,
    SPELL_LEATHERWORKING                     = 110423,
    SPELL_ENCHANTING                         = 110400,
    // Riding spells
    SPELL_ARTISAN_RIDING                     = 34091,
    SPELL_COLD_WHEATHER_FLYING               = 54197,
    SPELL_FLIGHT_MASTER_LICENSE              = 90267,
    SPELL_WISDOM_OF_FOUR_WINDS               = 115913,
};

float const startPosition[2][4] =
{
    { 1605.908f, 921.2222f, 470.6227f, 0.124413f }, // horde
    { 880.6965f, 296.6945f, 503.1162f, 3.779655f }  // alliance
};

struct CharacterBoostData
{
    CharacterBoostData() : charGuid(ObjectGuid::Empty), action(0), specialization(0), allianceFaction(false) { }

    ObjectGuid charGuid;
    uint32 action;
    uint32 specialization;
    bool allianceFaction;
};

struct BoostItems
{
    BoostItems() : spec(0), slot(0), itemId(0) { }

    uint32 spec;
    uint32 slot;
    uint32 itemId;
};
typedef std::vector<BoostItems*> BoostItemsVector;

typedef std::map<uint8 /*slot*/, uint32 /*ItemId*/> PreparedItemsMap;

void LoadBoostItems();
void SetBoosting(WorldSession* session, uint32 accountId, bool boost);

static BoostItemsVector mBoostItemsMap;

class CharacterBooster
{
    public:
        CharacterBooster(WorldSession* session);

        uint32 GetCurrentAction() const { return m_charBoostInfo.action; }
        ObjectGuid::LowType GetGuidLow() const { return m_charBoostInfo.charGuid.GetCounter(); }
        void HandleCharacterBoost();
        bool IsBoosting(ObjectGuid::LowType lowGuid) const { return m_boosting && (m_charBoostInfo.charGuid.GetCounter() == lowGuid); }
        void SetBoostedCharInfo(ObjectGuid guid, uint32 action, uint32 specialization, bool allianceFaction);
        void Update(uint32 diff);
        void SendCharBoostPacket(PreparedItemsMap items) const;

    private:
        void _GetCharBoostItems(PreparedItemsMap& itemsToMail, PreparedItemsMap& itemsToEquip) const;
        std::string _EquipItems(CharacterDatabaseTransaction trans, PreparedItemsMap itemsToEquip) const;
        void _GetBoostedCharacterData(uint8& raceId, uint8& classId, uint8& level) const;
        void _HandleCharacterBoost() const;
        void _LearnSpells(CharacterDatabaseTransaction trans) const;
        void _PrepareInventory(CharacterDatabaseTransaction trans) const;
        uint32 _PrepareMail(CharacterDatabaseTransaction trans, std::string const subject, std::string const body) const;
        std::string _SetSpecialization(CharacterDatabaseTransaction trans, uint8 const classId) const;
        void _SaveBoostedChar(CharacterDatabaseTransaction trans, std::string items, uint8 const raceId, uint8 const classId) const;
        void _SendMail(CharacterDatabaseTransaction trans, PreparedItemsMap items) const;
        void _LearnVeteranBonuses(CharacterDatabaseTransaction trans, uint8 const classId) const;
        void LearnNonExistedSpell(CharacterDatabaseTransaction trans, uint32 spell) const;
        void LearnNonExistedSkill(CharacterDatabaseTransaction trans, uint32 skill) const;
        WorldSession* GetSession() const { return m_session; }

        CharacterBoostData m_charBoostInfo;
        WorldSession* m_session;
        uint32 m_timer;
        bool m_boosting;
        bool m_sendPacket;
};

#endif
