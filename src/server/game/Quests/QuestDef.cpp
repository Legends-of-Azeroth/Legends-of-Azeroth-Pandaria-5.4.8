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

#include "QuestDef.h"
#include "QuestPools.h"
#include "Player.h"
#include "World.h"
#include "ObjectMgr.h"

Quest::Quest(Field* questRecord)
{
    _id = questRecord[0].GetUInt32();
    _type = questRecord[1].GetUInt8();
    _questLevel = questRecord[2].GetInt16();
    _minLevel = questRecord[3].GetInt16();
    _packageID = questRecord[4].GetUInt32();
    _questSortID = questRecord[5].GetInt16();
    _questInfoID = questRecord[6].GetUInt16();
    _suggestedPlayers = questRecord[7].GetUInt8();
    _nextQuestInChain = questRecord[8].GetUInt32();
    _rewardXPDifficulty = questRecord[9].GetUInt8();
    _rewardMoney = questRecord[10].GetInt32();
    _rewardBonusMoney = questRecord[11].GetUInt32();
    _rewardDisplaySpell = questRecord[12].GetUInt32();
    _rewardSpell = questRecord[13].GetInt32();
    _rewardHonor = questRecord[14].GetUInt32();
    _rewardKillHonor = questRecord[15].GetUInt32();
    _sourceItemId = questRecord[16].GetUInt32();
    _flags = questRecord[17].GetUInt32();
    _flagsEx = questRecord[18].GetUInt32();

    _rewItemsCount = 0;
    _rewChoiceItemsCount = 0;
    _rewCurrencyCount = 0;

    for (uint32 i = 0; i < QUEST_ITEM_DROP_COUNT; ++i)
    {
        RewardItemId[i] = questRecord[19 + i * 4].GetUInt32();
        RewardItemCount[i] = questRecord[20 + i * 4].GetUInt32();
        ItemDrop[i] = questRecord[21 + i * 4].GetUInt32();
        ItemDropQuantity[i] = questRecord[22 + i * 4].GetUInt32();

        if (RewardItemId[i])
            ++_rewItemsCount;
    }

    for (uint32 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
    {
        RewardChoiceItemId[i] = questRecord[35 + i * 2].GetUInt32();
        RewardChoiceItemCount[i] = questRecord[36 + i * 2].GetUInt32();

        if (RewardChoiceItemId[i])
            ++_rewChoiceItemsCount;
    }

    _poiContinent = questRecord[47].GetUInt16();
    _poix = questRecord[48].GetFloat();
    _poiy = questRecord[49].GetFloat();
    _poiPriority = questRecord[50].GetUInt32();

    _rewardTitleId = questRecord[51].GetUInt16();
    _rewardArenaPoints = questRecord[52].GetUInt16();
    _rewardSkillId = questRecord[53].GetUInt16();
    _rewardSkillPoints = questRecord[54].GetUInt8();

    _questGiverPortrait = questRecord[55].GetUInt32();
    _questTurnInPortrait = questRecord[56].GetUInt32();

    for (uint32 i = 0; i < QUEST_REWARD_REPUTATIONS_COUNT; ++i)
    {
        RewardFactionId[i] = questRecord[57 + i * 3].GetUInt32();
        RewardFactionValue[i] = questRecord[58 + i * 3].GetInt32();
        RewardFactionOverride[i] = questRecord[59 + i * 3].GetInt32();
    }

    _rewardReputationMask = questRecord[72].GetUInt32();

    for (uint32 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
    {
        RewardCurrencyId[i] = questRecord[73 + i * 2].GetUInt32();
        RewardCurrencyCount[i] = questRecord[74 + i * 2].GetUInt32();

        if (RewardCurrencyId[i])
            ++_rewCurrencyCount;
    }

    _soundAccept = questRecord[81].GetUInt16();
    _soundTurnIn = questRecord[82].GetUInt16();
    _areaGroupID = questRecord[83].GetUInt8();
    _limitTime = questRecord[84].GetUInt32();
    _allowableRaces = questRecord[85].GetUInt32();

    _logTitle = questRecord[86].GetString();
    _logDescription = questRecord[87].GetString();
    _questDescription = questRecord[88].GetString();
    _areaDescription = questRecord[89].GetString();
    _portraitGiverText = questRecord[90].GetString();
    _portraitGiverName = questRecord[91].GetString();
    _portraitTurnInText = questRecord[92].GetString();
    _portraitTurnInName = questRecord[93].GetString();
    _questCompletionLog = questRecord[94].GetString();
}

void Quest::LoadRewardDisplaySpell(Field* fields)
{
    uint32 spellId = fields[1].GetUInt32();
    uint32 playerConditionId = fields[2].GetUInt32();
    uint32 type = fields[3].GetUInt32();

    if (!sSpellMgr->GetSpellInfo(spellId))
    {
        TC_LOG_ERROR("sql.sql", "Table `quest_reward_display_spell` has non-existing Spell ({}) set for quest {}. Skipped.", spellId, fields[0].GetUInt32());
        return;
    }

    if (type >= AsUnderlyingType(QuestCompleteSpellType::Max))
    {
        TC_LOG_ERROR("sql.sql", "Table `quest_reward_display_spell` invalid type value ({}) set for quest {} and spell {}. Set to 0.", type, fields[0].GetUInt32(), spellId);
        type = AsUnderlyingType(QuestCompleteSpellType::LegacyBehavior);
    }

    RewardDisplaySpell.emplace_back(spellId, playerConditionId, QuestCompleteSpellType(type));
}

void Quest::LoadQuestDetails(Field* fields)
{
    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        if (!sEmotesStore.LookupEntry(fields[1 + i].GetUInt16()))
        {
            TC_LOG_ERROR("sql.sql", "Table `quest_details` has non-existing Emote{} ({}) set for quest {}. Skipped.", 1+i, fields[1+i].GetUInt16(), fields[0].GetUInt32());
            continue;
        }

        DetailsEmote[i] = fields[1 + i].GetUInt16();
    }

    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
        DetailsEmoteDelay[i] = fields[5 + i].GetUInt32();
}

void Quest::LoadQuestRequestItems(Field* fields)
{
    _emoteOnComplete = fields[1].GetUInt16();
    _emoteOnIncomplete = fields[2].GetUInt16();

    if (!sEmotesStore.LookupEntry(_emoteOnComplete))
        TC_LOG_ERROR("sql.sql", "Table `quest_request_items` has non-existing EmoteOnComplete ({}) set for quest {}.", _emoteOnComplete, fields[0].GetUInt32());

    if (!sEmotesStore.LookupEntry(_emoteOnIncomplete))
        TC_LOG_ERROR("sql.sql", "Table `quest_request_items` has non-existing EmoteOnIncomplete ({}) set for quest {}.", _emoteOnIncomplete, fields[0].GetUInt32());

    _requestItemsText = fields[3].GetString();
}

void Quest::LoadQuestOfferReward(Field* fields)
{
    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        if (!sEmotesStore.LookupEntry(fields[1 + i].GetUInt16()))
        {
            TC_LOG_ERROR("sql.sql", "Table `quest_offer_reward` has non-existing Emote{} ({}) set for quest {}. Skipped.", 1+i, fields[1+i].GetUInt16(), fields[0].GetUInt32());
            continue;
        }

        OfferRewardEmote[i] = fields[1 + i].GetInt16();
    }

    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
        OfferRewardEmoteDelay[i] = fields[5 + i].GetUInt32();

    _offerRewardText = fields[9].GetString();
}

void Quest::LoadQuestTemplateAddon(Field* fields)
{
    _maxLevel = fields[1].GetUInt8();
    _allowableClasses = fields[2].GetUInt32();
    _sourceSpellID = fields[3].GetUInt32();
    _prevQuestID = fields[4].GetInt32();
    _nextQuestID = fields[5].GetUInt32();
    _exclusiveGroup = fields[6].GetInt32();
    _rewardMailTemplateId = fields[7].GetUInt32();
    _rewardMailDelay = fields[8].GetUInt32();
    _requiredSkillId = fields[9].GetUInt16();
    _requiredSkillPoints = fields[10].GetUInt16();
    _requiredMinRepFaction = fields[11].GetUInt16();
    _requiredMaxRepFaction = fields[12].GetUInt16();
    _requiredMinRepValue = fields[13].GetInt32();
    _requiredMaxRepValue = fields[14].GetInt32();
    _sourceItemIdCount = fields[15].GetUInt8();
    _specialFlags = fields[16].GetUInt8();
    _scriptId = sObjectMgr->GetScriptId(fields[17].GetString());

    if (_specialFlags & QUEST_SPECIAL_FLAGS_AUTO_ACCEPT)
        _flags |= QUEST_FLAGS_AUTO_ACCEPT;
}

void Quest::LoadQuestObjective(Field* fields)
{
    QuestObjective& obj = Objectives.emplace_back();
    obj.QuestID = fields[0].GetUInt32();
    obj.ID = fields[1].GetUInt32();
    obj.Type = fields[2].GetUInt8();
    obj.StorageIndex = fields[3].GetInt8();
    obj.ObjectID = fields[4].GetInt32();
    obj.Amount = fields[5].GetInt32();
    obj.Flags = fields[6].GetUInt32();
    obj.Description = fields[7].GetString();

    _usedQuestObjectiveTypes[obj.Type] = true;
}

void Quest::LoadQuestObjectiveVisualEffect(Field* fields)
{
    uint32 objID = fields[1].GetUInt32();

    for (QuestObjective& obj : Objectives)
    {
        if (obj.ID == objID)
        {
            uint8 effectIndex = fields[3].GetUInt8();
            if (effectIndex >= obj.VisualEffects.size())
                obj.VisualEffects.resize(effectIndex + 1, 0);

            obj.VisualEffects[effectIndex] = fields[4].GetInt32();
            break;
        }
    }
}

uint32 Quest::XPValue(Player const* player) const
{
    if (player)
    {
        int32 quest_level = (_questLevel == -1 ? player->GetLevel() : _questLevel);
        const QuestXPEntry* xpentry = sQuestXPStore.LookupEntry(quest_level);
        if (!xpentry)
            return 0;

        int32 diffFactor = 2 * (quest_level - player->GetLevel()) + 20;
        if (diffFactor < 1)
            diffFactor = 1;
        else if (diffFactor > 10)
            diffFactor = 10;

        uint32 xp = diffFactor * xpentry->Difficulty[_rewardXPDifficulty] / 10;
        if (xp <= 100)
            xp = 5 * ((xp + 2) / 5);
        else if (xp <= 500)
            xp = 10 * ((xp + 5) / 10);
        else if (xp <= 1000)
            xp = 25 * ((xp + 12) / 25);
        else
            xp = 50 * ((xp + 25) / 50);

        return xp;
    }

    return 0;
}

int32 Quest::GetRewOrReqMoney(Player const* player) const
{
    // RequiredMoney: the amount is the negative copper sum.
    if (_rewardMoney < 0)
        return _rewardMoney;

    // RewardMoney: the positive amount
    if (!player || !player->IsMaxLevel())
        return int32(_rewardMoney * sWorld->getRate(RATE_DROP_MONEY));
    else // At level cap, the money reward is the maximum amount between normal and bonus money reward
        return std::max(int32(GetRewMoneyMaxLevel()), int32(_rewardMoney * sWorld->getRate(RATE_DROP_MONEY)));
}

uint32 Quest::GetRewMoneyMaxLevel() const
{
    if (HasFlag(QUEST_FLAGS_NO_MONEY_FOR_XP))
        return 0;

    return _rewardBonusMoney;
}

/*static*/ bool Quest::IsTakingQuestEnabled(uint32 questId)
{
    if (!sQuestPoolMgr->IsQuestActive(questId))
        return false;

    return true;
}

bool Quest::IsAutoAccept() const
{
    return !sWorld->getBoolConfig(CONFIG_QUEST_IGNORE_AUTO_ACCEPT) && HasFlag(QUEST_FLAGS_AUTO_ACCEPT);
}

bool Quest::IsTurnIn() const
{
    return !sWorld->getBoolConfig(CONFIG_QUEST_IGNORE_AUTO_COMPLETE) && (_type == QUEST_TYPE_TURNIN || HasFlag(QUEST_FLAGS_AUTO_COMPLETE));
}

bool Quest::IsRaidQuest(Difficulty difficulty) const
{
    switch (_questInfoID)
    {
        case QUEST_INFO_RAID:
            return true;
        case QUEST_INFO_LEGENDARY:
            if (_id == 30107 || _id == 30118 || // Cluster Clutch
                _id == 32431 || _id == 32389)   // The Lion Roars
                return true;
            break;
        case QUEST_INFO_RAID_10:
            return (difficulty == RAID_DIFFICULTY_10MAN_NORMAL || difficulty == RAID_DIFFICULTY_10MAN_HEROIC);
        case QUEST_INFO_RAID_25:
            return (difficulty == RAID_DIFFICULTY_25MAN_NORMAL || difficulty == RAID_DIFFICULTY_25MAN_HEROIC || difficulty == RAID_DIFFICULTY_25MAN_LFR);
        default:
            break;
    }

    if (HasFlag(QUEST_FLAGS_RAID_GROUP_OK))
        return true;

    return false;
}

bool Quest::IsAllowedInRaid(Difficulty difficulty) const
{
    if (IsRaidQuest(difficulty))
        return true;

    return sWorld->getBoolConfig(CONFIG_QUEST_IGNORE_RAID);
}

uint32 Quest::CalculateHonorGain(uint8 level) const
{
    if (level > GT_MAX_LEVEL)
        level = GT_MAX_LEVEL;

    uint32 honor = 0;

    /*if (GetRewHonorAddition() > 0 || GetRewHonorMultiplier() > 0.0f)
    {
        // values stored from 0.. for 1...
        TeamContributionPointsEntry const* tc = sTeamContributionPointsStore.LookupEntry(level);
        if (!tc)
            return 0;

        honor = uint32(tc->value * GetRewHonorMultiplier() * 0.1f);
        honor += GetRewHonorAddition();
    }*/

    return honor;
}
