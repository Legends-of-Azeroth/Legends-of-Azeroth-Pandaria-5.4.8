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

#include "SpellMgr.h"
#include "SpellInfo.h"
#include "ObjectMgr.h"
#include "SpellAuras.h"
#include "SpellAuraDefines.h"
#include "SharedDefines.h"
#include "DBCStores.h"
#include "World.h"
#include "Chat.h"
#include "Spell.h"
#include "BattlegroundMgr.h"
#include "MapManager.h"
#include "BattlefieldWG.h"
#include "BattlefieldMgr.h"
#include "Player.h"
#include <fstream>

bool IsPrimaryProfessionSkill(uint32 skill)
{
    SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
    return pSkill && pSkill->CategoryID == SKILL_CATEGORY_PROFESSION;
}

bool IsWeaponSkill(uint32 skill)
{
    SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
    return pSkill && pSkill->CategoryID == SKILL_CATEGORY_WEAPON;
}

bool IsPartOfSkillLine(uint32 skillId, uint32 spellId)
{
    SkillLineAbilityMapBounds skillBounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellId);
    for (SkillLineAbilityMap::const_iterator itr = skillBounds.first; itr != skillBounds.second; ++itr)
        if (itr->second->SkillLine == skillId)
            return true;

    return false;
}

DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellInfo const* spellproto)
{
    if (spellproto->IsPositive())
        return DIMINISHING_NONE;

    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (spellproto->Effects[i].ApplyAuraName == SPELL_AURA_MOD_TAUNT)
            return DIMINISHING_TAUNT;
    }

    // Explicit Diminishing Groups
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            // Frost Tomb
            if (spellproto->Id == 48400)
                return DIMINISHING_NONE;
            // ToC Icehowl Arctic Breath
            else if (spellproto->SpellVisual[0] == 14153)
                return DIMINISHING_NONE;
            // Black Plague
            else if (spellproto->Id == 64155)
                return DIMINISHING_NONE;
            // Earthquake (Trash, Ulduar)
            else if (spellproto->Id == 64697)
                return DIMINISHING_NONE;
            break;
        }
        // Event spells
        case SPELLFAMILY_UNK1:
            return DIMINISHING_NONE;
        case SPELLFAMILY_WARRIOR:
        {
            // Hamstring - limit duration to 10s in PvP
            if (spellproto->SpellFamilyFlags[0] & 0x2)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Curses/etc
            if ((spellproto->SpellFamilyFlags[0] & 0x408000 || spellproto->SpellFamilyFlags[1] & 0x200) &&
                !(spellproto->SpellFamilyFlags[3] & 0x04000000)) // except AoE
                return DIMINISHING_LIMITONLY;
            // Sin and Punishment (Priest spell, don't ask)
            else if (spellproto->SpellIconID == 1869)
                return DIMINISHING_NONE;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Faerie Fire and Faerie Swarm
            if (spellproto->SpellFamilyFlags[0] & 0x500)
                return DIMINISHING_LIMITONLY;
            // Faerie Swarm snare
            if (spellproto->Id == 102354)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Crippling poison - Limit to 10 seconds in PvP (No SpellFamilyFlags)
            if (spellproto->SpellIconID == 163)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Hunter's Mark
            if ((spellproto->SpellFamilyFlags[0] & 0x400) && spellproto->SpellIconID == 538)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Judgement of Justice - limit duration to 10s in PvP
            if (spellproto->SpellFamilyFlags[0] & 0x100000)
                return DIMINISHING_LIMITONLY;
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Mark of Blood
            if ((spellproto->SpellFamilyFlags[0] & 0x10000000) && spellproto->SpellIconID == 2285)
                return DIMINISHING_LIMITONLY;
            break;
        }
        default:
            break;
    }

    switch (spellproto->Id)
    {
        case 710:       // Banish
        case 3355:      // Freezing Trap
        case 82691:     // Ring of Frost
        case 76780:     // Bind Elemental
        case 107079:    // Quaking Palm
            return DIMINISHING_MESMERIZES;
        case 99:        // Disorienting Roar (Talent)
        case 19503:     // Scatter Shot
        case 31661:     // Dragon's Breath
        case 88625:     // Holy Word: Chastise
        case 123393:    // Breath of Fire (Glyphed)
            return DIMINISHING_MESMERIZES_SHORT;
        case 90337:     // Bad Manner(Monkey pet)
        case 126246:    // Lullaby (Crane pet)
        case 126355:    // Paralyzing Quill (Porcupine pet)
        case 126423:    // Petrifying Gaze (Basilisk pet)
            return DIMINISHING_STUN;
        case 7922:      // Charge
        case 77505:     // Earthquake
        case 113953:    // Paralysis (Stun effect of Paralytic Poison)
        case 118000:    // Dragon Roar
        case 118895:    // Dragon Roar (Talent)
            return DIMINISHING_STUN_SHORT;
        //case 108194:    // Asphyxiate (If target is immune to stun)
        //    return DIMINISHING_SILENCE;
        case 2094:      // Blind
        case 6358:      // Seduction (Succubus)
        case 10326:     // Turn Evil
        case 104045:    // Sleep (Metamorphosis)
        case 105421:    // Blinding Light
        case 115268:    // Mesmerize (Shivarra)
        case 145067:    // Turn Evil (Evil is a Point of View)
            return DIMINISHING_FEAR;
        case 64695:     // Earthgrab Totem
        case 111340:    // Ice Ward
        case 123407:    // Spinning Fire Blossom
        case 19185:     // Entrapment
        case 64803:     // Entrapment
            return DIMINISHING_ROOT_SHORT;
        case 33786:     // Cyclone
        case 113506:    // Cyclone (Symbiosis)
            return DIMINISHING_CYCLONE;
        case 116:       // Frost Bolt
        case 8680:      // Wound Poison
        case 12323:     // Piercing Howl
        case 18223:     // Curse of Exhaustion
        case 58180:     // Infected Wounds
        case 82654:     // Widow Venom
        case 116095:    // Disable
        case 116330:    // Dizzying Haze
            return DIMINISHING_LIMITONLY;
        case 131510:    // Uncontrolled Banish
        case 137669:    // Storm cloud
        case 137664:    // Frozen blood
        case 136615:    // Electrified
        case 108920:    // Void Tendrils (Initial root)
        case 139843:    // Arctic Freeze
        case 142945:    // Eerie Fog
            return DIMINISHING_NONE;
        default:
            break;
    }

    // Lastly - Set diminishing depending on mechanic
    uint32 mechanic = spellproto->GetAllEffectsMechanicMask();
    if (mechanic & (1 << MECHANIC_CHARM))
        return DIMINISHING_MIND_CONTROL;
    if (mechanic & (1 << MECHANIC_SILENCE)) // Must be before MECHANIC_ROOT
        return DIMINISHING_SILENCE;
    if (mechanic & ((1 << MECHANIC_SLEEP) | (1 << MECHANIC_SAPPED) | (1 << MECHANIC_POLYMORPH) | (1 << MECHANIC_SHACKLE)
        | (1 << MECHANIC_DISORIENTED) | (1 << MECHANIC_KNOCKOUT))) // Not shure about it.
        return DIMINISHING_MESMERIZES;
    if (mechanic & (1 << MECHANIC_DISARM))
        return DIMINISHING_DISARM;
    if (mechanic & (1 << MECHANIC_FEAR))
        return DIMINISHING_FEAR;
    if (mechanic & (1 << MECHANIC_STUN))
        return DIMINISHING_STUN;
    if (mechanic & (1 << MECHANIC_BANISH))
        return DIMINISHING_BANISH;
    if (mechanic & (1 << MECHANIC_ROOT))
        return DIMINISHING_ROOT;
    if (mechanic & (1 << MECHANIC_HORROR))
        return DIMINISHING_HORROR;

    return DIMINISHING_NONE;
}

DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_TAUNT:
        case DIMINISHING_STUN:
        case DIMINISHING_STUN_SHORT:
        case DIMINISHING_CYCLONE:
            return DRTYPE_ALL;
        case DIMINISHING_LIMITONLY:
        case DIMINISHING_NONE:
            return DRTYPE_NONE;
        default:
            return DRTYPE_PLAYER;
    }
}

DiminishingLevels GetDiminishingReturnsMaxLevel(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_TAUNT:
            return DIMINISHING_LEVEL_TAUNT_IMMUNE;
        default:
            return DIMINISHING_LEVEL_IMMUNE;
    }
}

int32 GetDiminishingReturnsLimitDuration(DiminishingGroup group, SpellInfo const* spellproto)
{
    if (!IsDiminishingReturnsGroupDurationLimited(group))
        return 0;

    // Explicit diminishing duration
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_DRUID:
        {
            // Faerie Fire, Faerie Swarm - limit to 20 seconds in PvP
            if (spellproto->SpellFamilyFlags[0] & 0x500)
                return 20 * IN_MILLISECONDS;
            if (spellproto->Id == 33786)
                return 6 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Wyvern Sting
            if (spellproto->SpellFamilyFlags[1] & 0x1000)
                return 6 * IN_MILLISECONDS;
            // Hunter's Mark
            if (spellproto->SpellFamilyFlags[0] & 0x400)
                return 20 * IN_MILLISECONDS;
            switch (spellproto->Id)
            {
                case 82654:  // Widow Venom
                    return 10 * IN_MILLISECONDS;
                case 117526: // Binding Shot
                    return 3 * IN_MILLISECONDS;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Repentance - limit to 6 seconds in PvP
            if (spellproto->SpellFamilyFlags[0] & 0x4)
                return 6 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Curse of Enfeeblement - limit to 12 seconds in PvP
            if (spellproto->SpellFamilyFlags[0] & 0x8000)
                return 12 * IN_MILLISECONDS;
            // Curse of Elements - limit to 120 seconds in PvP
            else if (spellproto->SpellFamilyFlags[1] & 0x200)
               return 120 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_MONK:
        {
            if (spellproto->Id == 116706) // Disable (root)
                return 3 * IN_MILLISECONDS;
            if (spellproto->Id == 115078) // Paralysis
                return 4 * IN_MILLISECONDS;
            break;
        }
        default:
            break;
    }

    return 8 * IN_MILLISECONDS;
}

bool IsDiminishingReturnsGroupDurationLimited(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_BANISH:
        case DIMINISHING_CYCLONE:
        case DIMINISHING_MESMERIZES:
        case DIMINISHING_MESMERIZES_SHORT:
        case DIMINISHING_FEAR:
        case DIMINISHING_HORROR:
        case DIMINISHING_MIND_CONTROL:
        case DIMINISHING_ROOT:
        case DIMINISHING_ROOT_SHORT:
        case DIMINISHING_STUN:
        case DIMINISHING_STUN_SHORT:
        case DIMINISHING_LIMITONLY:
            return true;
        default:
            return false;
    }
}

SpellMgr::SpellMgr() { }

SpellMgr::~SpellMgr()
{
    UnloadSpellInfoStore();
}

SpellMgr* SpellMgr::instance()
{
    static SpellMgr _instance;
    return &_instance;
}

/// Some checks for spells, to prevent adding deprecated/broken spells for trainers, spell book, etc
bool SpellMgr::IsSpellValid(SpellInfo const* spellInfo, Player* player, bool msg)
{
    // not exist
    if (!spellInfo)
        return false;

    bool needCheckReagents = false;

    // check effects
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        switch (spellInfo->Effects[i].Effect)
        {
            case 0:
                continue;

            // craft spell for crafting non-existed item (break client recipes list show)
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_CREATE_ITEM_2:
            {
                if (spellInfo->Effects[i].ItemType == 0)
                {
                    // skip auto-loot crafting spells, its not need explicit item info (but have special fake items sometime)
                    if (!spellInfo->IsLootCrafting())
                    {
                        if (msg)
                        {
                            if (player)
                                ChatHandler(player->GetSession()).PSendSysMessage("Craft spell %u not have create item entry.", spellInfo->Id);
                            else
                                TC_LOG_ERROR("sql.sql", "Craft spell %u not have create item entry.", spellInfo->Id);
                        }
                        return false;
                    }

                }
                // also possible IsLootCrafting case but fake item must exist anyway
                else if (!sObjectMgr->GetItemTemplate(spellInfo->Effects[i].ItemType))
                {
                    if (msg)
                    {
                        if (player)
                            ChatHandler(player->GetSession()).PSendSysMessage("Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Effects[i].ItemType);
                        else
                            TC_LOG_ERROR("sql.sql", "Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Effects[i].ItemType);
                    }
                    return false;
                }

                needCheckReagents = true;
                break;
            }
            case SPELL_EFFECT_LEARN_SPELL:
            {
                SpellInfo const* spellInfo2 = sSpellMgr->GetSpellInfo(spellInfo->Effects[i].TriggerSpell);
                if (!IsSpellValid(spellInfo2, player, msg))
                {
                    if (msg)
                    {
                        if (player)
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell %u learn to broken spell %u, and then...", spellInfo->Id, spellInfo->Effects[i].TriggerSpell);
                        else
                            TC_LOG_ERROR("sql.sql", "Spell %u learn to invalid spell %u, and then...", spellInfo->Id, spellInfo->Effects[i].TriggerSpell);
                    }
                    return false;
                }
                break;
            }
        }
    }

    if (needCheckReagents)
    {
        for (uint8 j = 0; j < MAX_SPELL_REAGENTS; ++j)
        {
            if (spellInfo->Reagent[j] > 0 && !sObjectMgr->GetItemTemplate(spellInfo->Reagent[j]))
            {
                if (msg)
                {
                    if (player)
                        ChatHandler(player->GetSession()).PSendSysMessage("Craft spell %u have not-exist reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                    else
                        TC_LOG_ERROR("sql.sql", "Craft spell %u have not-exist reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                }
                return false;
            }
        }
    }

    return true;
}

uint32 SpellMgr::GetSpellIdForDifficulty(uint32 spell, Unit const* caster) const
{
    if (!spell || !caster || !caster->GetMap()->IsDungeon())
        return spell;
    if (SpellInfo const* info = GetSpellInfo(spell, caster->GetMap()->GetDifficulty()))
        return info->Id;
    return spell;
}

SpellInfo const* SpellMgr::GetSpellForDifficultyFromSpell(SpellInfo const* spell, Unit const* caster) const
{
    if (!spell || !caster || !caster->GetMap()->IsDungeon())
        return spell;
    return GetSpellInfo(spell->Id, caster->GetMap()->GetDifficulty());
}

SpellChainNode const* SpellMgr::GetSpellChainNode(uint32 spell_id) const
{
    SpellChainMap::const_iterator itr = mSpellChains.find(spell_id);
    if (itr == mSpellChains.end())
        return NULL;

    return &itr->second;
}

uint32 SpellMgr::GetFirstSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        return node->first->Id;

    return spell_id;
}

uint32 SpellMgr::GetLastSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        return node->last->Id;

    return spell_id;
}

uint32 SpellMgr::GetNextSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        if (node->next)
            return node->next->Id;

    return 0;
}

uint32 SpellMgr::GetPrevSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        if (node->prev)
            return node->prev->Id;

    return 0;
}

uint8 SpellMgr::GetSpellRank(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        return node->rank;

    return 0;
}

uint32 SpellMgr::GetSpellWithRank(uint32 spell_id, uint32 rank, bool strict) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
    {
        if (rank != node->rank)
            return GetSpellWithRank(node->rank < rank ? node->next->Id : node->prev->Id, rank, strict);
    }
    else if (strict && rank > 1)
        return 0;
    return spell_id;
}

SpellRequiredMapBounds SpellMgr::GetSpellsRequiredForSpellBounds(uint32 spell_id) const
{
    return mSpellReq.equal_range(spell_id);
}

SpellsRequiringSpellMapBounds SpellMgr::GetSpellsRequiringSpellBounds(uint32 spell_id) const
{
    return mSpellsReqSpell.equal_range(spell_id);
}

bool SpellMgr::IsSpellRequiringSpell(uint32 spellid, uint32 req_spellid) const
{
    SpellsRequiringSpellMapBounds spellsRequiringSpell = GetSpellsRequiringSpellBounds(req_spellid);
    for (SpellsRequiringSpellMap::const_iterator itr = spellsRequiringSpell.first; itr != spellsRequiringSpell.second; ++itr)
    {
        if (itr->second == spellid)
            return true;
    }
    return false;
}

SpellLearnSkillNode const* SpellMgr::GetSpellLearnSkill(uint32 spell_id) const
{
    SpellLearnSkillMap::const_iterator itr = mSpellLearnSkills.find(spell_id);
    if (itr != mSpellLearnSkills.end())
        return &itr->second;
    else
        return NULL;
}

SpellLearnSpellMapBounds SpellMgr::GetSpellLearnSpellMapBounds(uint32 spell_id) const
{
    return mSpellLearnSpells.equal_range(spell_id);
}

bool SpellMgr::IsSpellLearnSpell(uint32 spell_id) const
{
    return mSpellLearnSpells.find(spell_id) != mSpellLearnSpells.end();
}

bool SpellMgr::IsSpellLearnToSpell(uint32 spell_id1, uint32 spell_id2) const
{
    SpellLearnSpellMapBounds bounds = GetSpellLearnSpellMapBounds(spell_id1);
    for (SpellLearnSpellMap::const_iterator i = bounds.first; i != bounds.second; ++i)
        if (i->second.spell == spell_id2)
            return true;
    return false;
}

SpellTargetPosition const* SpellMgr::GetSpellTargetPosition(uint32 spell_id, SpellEffIndex effIndex) const
{
    SpellTargetPositionMap::const_iterator itr = mSpellTargetPositions.find(std::make_pair(spell_id, effIndex));
    if (itr != mSpellTargetPositions.end())
        return &itr->second;
    return NULL;
}

SpellSpellGroupMapBounds SpellMgr::GetSpellSpellGroupMapBounds(uint32 spell_id) const
{
    spell_id = GetFirstSpellInChain(spell_id);
    return mSpellSpellGroup.equal_range(spell_id);
}

bool SpellMgr::IsSpellMemberOfSpellGroup(uint32 spellid, SpellGroup groupid) const
{
    SpellSpellGroupMapBounds spellGroup = GetSpellSpellGroupMapBounds(spellid);
    for (SpellSpellGroupMap::const_iterator itr = spellGroup.first; itr != spellGroup.second; ++itr)
    {
        if (itr->second == groupid)
            return true;
    }
    return false;
}

SpellGroupSpellMapBounds SpellMgr::GetSpellGroupSpellMapBounds(SpellGroup group_id) const
{
    return mSpellGroupSpell.equal_range(group_id);
}

void SpellMgr::GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells) const
{
    std::set<SpellGroup> usedGroups;
    GetSetOfSpellsInSpellGroup(group_id, foundSpells, usedGroups);
}

void SpellMgr::GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells, std::set<SpellGroup>& usedGroups) const
{
    if (usedGroups.find(group_id) != usedGroups.end())
        return;
    usedGroups.insert(group_id);

    SpellGroupSpellMapBounds groupSpell = GetSpellGroupSpellMapBounds(group_id);
    for (SpellGroupSpellMap::const_iterator itr = groupSpell.first; itr != groupSpell.second; ++itr)
    {
        if (itr->second < 0)
        {
            SpellGroup currGroup = (SpellGroup)abs(itr->second);
            GetSetOfSpellsInSpellGroup(currGroup, foundSpells, usedGroups);
        }
        else
        {
            foundSpells.insert(itr->second);
        }
    }
}

bool SpellMgr::AddSameEffectStackRuleSpellGroups(SpellInfo const* spellInfo, int32 amount, std::map<SpellGroup, int32>& groups) const
{
    uint32 spellId = spellInfo->GetFirstRankSpell()->Id;
    SpellSpellGroupMapBounds spellGroup = GetSpellSpellGroupMapBounds(spellId);
    // Find group with SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT if it belongs to one
    for (SpellSpellGroupMap::const_iterator itr = spellGroup.first; itr != spellGroup.second; ++itr)
    {
        SpellGroup group = itr->second;
        SpellGroupStackMap::const_iterator found = mSpellGroupStack.find(group);
        if (found != mSpellGroupStack.end())
        {
            if (found->second == SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT)
            {
                // Put the highest amount in the map
                if (groups.find(group) == groups.end())
                    groups[group] = amount;
                else
                {
                    int32 curr_amount = groups[group];
                    // Take absolute value because this also counts for the highest negative aura
                    if (abs(curr_amount) < abs(amount))
                        groups[group] = amount;
                }
                // return because a spell should be in only one SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT group
                return true;
            }
        }
    }
    // Not in a SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT group, so return false
    return false;
}

SpellGroupStackRule SpellMgr::CheckSpellGroupStackRules(SpellInfo const* spellInfo1, SpellInfo const* spellInfo2) const
{
    uint32 spellid_1 = spellInfo1->GetFirstRankSpell()->Id;
    uint32 spellid_2 = spellInfo2->GetFirstRankSpell()->Id;

    // find SpellGroups which are common for both spells
    SpellSpellGroupMapBounds spellGroup1 = GetSpellSpellGroupMapBounds(spellid_1);
    std::set<SpellGroup> groups;
    for (SpellSpellGroupMap::const_iterator itr = spellGroup1.first; itr != spellGroup1.second; ++itr)
    {
        if (IsSpellMemberOfSpellGroup(spellid_2, itr->second))
        {
            bool add = true;
            SpellGroupSpellMapBounds groupSpell = GetSpellGroupSpellMapBounds(itr->second);
            for (SpellGroupSpellMap::const_iterator itr2 = groupSpell.first; itr2 != groupSpell.second; ++itr2)
            {
                if (itr2->second < 0)
                {
                    SpellGroup currGroup = (SpellGroup)abs(itr2->second);
                    if (IsSpellMemberOfSpellGroup(spellid_1, currGroup) && IsSpellMemberOfSpellGroup(spellid_2, currGroup))
                    {
                        add = false;
                        break;
                    }
                }
            }
            if (add)
                groups.insert(itr->second);
        }
    }

    SpellGroupStackRule rule = SPELL_GROUP_STACK_RULE_DEFAULT;

    for (std::set<SpellGroup>::iterator itr = groups.begin(); itr!= groups.end(); ++itr)
    {
        SpellGroupStackMap::const_iterator found = mSpellGroupStack.find(*itr);
        if (found != mSpellGroupStack.end())
            rule = found->second;
        if (rule)
            break;
    }
    return rule;
}

SpellProcEventEntry const* SpellMgr::GetSpellProcEvent(uint32 spellId) const
{
    SpellProcEventMap::const_iterator itr = mSpellProcEventMap.find(spellId);
    if (itr != mSpellProcEventMap.end())
        return &itr->second;
    return NULL;
}

bool SpellMgr::IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellInfo const* procSpell, uint32 procFlags, uint32 procExtra, bool active) const
{
    // No extra req need
    uint32 procEvent_procEx = PROC_EX_NONE;

    // check prockFlags for condition
    if ((procFlags & EventProcFlag) == 0)
        return false;

    bool hasFamilyMask = false;

    /* Check Periodic Auras

    *Dots can trigger if spell has no PROC_FLAG_SUCCESSFUL_NEGATIVE_MAGIC_SPELL
        nor PROC_FLAG_TAKEN_POSITIVE_MAGIC_SPELL

    *Only Hots can trigger if spell has PROC_FLAG_TAKEN_POSITIVE_MAGIC_SPELL

    *Only dots can trigger if spell has both positivity flags or PROC_FLAG_SUCCESSFUL_NEGATIVE_MAGIC_SPELL

    *Aura has to have PROC_FLAG_TAKEN_POSITIVE_MAGIC_SPELL or spellfamily specified to trigger from Hot

    */

    if (procFlags & PROC_FLAG_DONE_PERIODIC)
    {
        if (procExtra & PROC_EX_INTERNAL_DOT && EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS && !(procFlags & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG))
            if (!(EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG))
                return false;
        if (procExtra & PROC_EX_INTERNAL_HOT && EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG && !(procFlags & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS))
            if (!(EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS))
                return false;
    }

    if (procFlags & PROC_FLAG_TAKEN_PERIODIC)
    {
        if (procExtra & PROC_EX_INTERNAL_DOT && EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS && !(EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG))
            if (!(EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG))
                return false;
        if (procExtra & PROC_EX_INTERNAL_HOT && EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG && !(EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS))
            if (!(EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS))
                return false;
    }
    // Trap casts are active by default
    if (procFlags & PROC_FLAG_DONE_TRAP_ACTIVATION)
        active = true;

    // Always trigger for this
    if (procFlags & (PROC_FLAG_KILLED | PROC_FLAG_DEATH))
        return true;

    if (spellProcEvent)     // Exist event data
    {
        // Store extra req
        procEvent_procEx = spellProcEvent->procEx;

        // For melee triggers
        if (procSpell == NULL)
        {
            // Check (if set) for school (melee attack have Normal school)
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
                return false;
        }
        else // For spells need check school/spell family/family mask
        {
            // Check (if set) for school
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & procSpell->SchoolMask) == 0)
                return false;

            // Check (if set) for spellFamilyName
            if (spellProcEvent->spellFamilyName && (spellProcEvent->spellFamilyName != procSpell->SpellFamilyName))
                return false;

            // spellFamilyName is Ok need check for spellFamilyMask if present
            if (spellProcEvent->spellFamilyMask)
            {
                if (!(spellProcEvent->spellFamilyMask & procSpell->SpellFamilyFlags))
                    return false;
                hasFamilyMask = true;
                // Some spells are not considered as active even with have spellfamilyflags
                if (!(procEvent_procEx & PROC_EX_ONLY_ACTIVE_SPELL))
                    active = true;
            }

            if (procEvent_procEx & PROC_EX_MELEE_RANGE_ONLY)
                if (!procSpell->RangeEntry || !(procSpell->RangeEntry->type & SPELL_RANGE_MELEE))
                    if (procSpell->Id != 121473 && procSpell->Id != 121474) // Shadow Blade, Shadow Blade Off-hand
                        return false;
        }
    }

    if (procFlags & PROC_FLAG_KILL)
        return true;

    if (procExtra & (PROC_EX_INTERNAL_REQ_FAMILY))
    {
        if (!hasFamilyMask)
            return false;
    }

    // Check for extra req (if none) and hit/crit
    if (procEvent_procEx == PROC_EX_NONE)
    {
        // No extra req, so can trigger only for hit/crit - spell has to be active
        if ((procExtra & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) && active)
            return true;
        if (procExtra & PROC_EX_REFLECT && active)
            return true;
    }
    else // Passive spells hits here only if resist/reflect/immune/evade
    {
        if (procExtra & AURA_SPELL_PROC_EX_MASK)
        {
            // if spell marked as procing only from not active spells
            if (active && procEvent_procEx & PROC_EX_NOT_ACTIVE_SPELL)
                return false;
            // if spell marked as procing only from active spells
            if (!active && procEvent_procEx & PROC_EX_ONLY_ACTIVE_SPELL)
                return false;
            // Exist req for PROC_EX_EX_TRIGGER_ALWAYS
            if (procEvent_procEx & PROC_EX_EX_TRIGGER_ALWAYS)
                return true;
            // PROC_EX_NOT_ACTIVE_SPELL and PROC_EX_ONLY_ACTIVE_SPELL flags handle: if passed checks before
            if ((procExtra & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) && ((procEvent_procEx & (AURA_SPELL_PROC_EX_MASK)) == 0))
                return true;
        }
        // Check Extra Requirement like (hit/crit/miss/resist/parry/dodge/block/immune/reflect/absorb and other)
        if (procEvent_procEx & procExtra)
            return true;
    }
    return false;
}

SpellProcEntry const* SpellMgr::GetSpellProcEntry(uint32 spellId) const
{
    SpellProcMap::const_iterator itr = mSpellProcMap.find(spellId);
    if (itr != mSpellProcMap.end())
        return &itr->second;
    return NULL;
}

bool SpellMgr::CanSpellTriggerProcOnEvent(SpellProcEntry const& procEntry, ProcEventInfo& eventInfo) const
{
    // proc type doesn't match
    if (!(eventInfo.GetTypeMask() & procEntry.typeMask))
        return false;

    // check XP or honor target requirement
    if (procEntry.attributesMask & PROC_ATTR_REQ_EXP_OR_HONOR)
        if (Player* actor = eventInfo.GetActor()->ToPlayer())
            if (eventInfo.GetActionTarget() && !actor->IsHonorOrXPTarget(eventInfo.GetActionTarget()))
                return false;

    // always trigger for these types
    if (eventInfo.GetTypeMask() & (PROC_FLAG_KILLED | PROC_FLAG_KILL | PROC_FLAG_DEATH))
        return true;

    // check school mask (if set) for other trigger types
    if (procEntry.schoolMask && !(eventInfo.GetSchoolMask() & procEntry.schoolMask))
        return false;

    // check spell family name/flags (if set) for spells
    if (eventInfo.GetTypeMask() & (PERIODIC_PROC_FLAG_MASK | SPELL_PROC_FLAG_MASK | PROC_FLAG_DONE_TRAP_ACTIVATION))
    {
        if (procEntry.spellFamilyName && (procEntry.spellFamilyName != eventInfo.GetSpellInfo()->SpellFamilyName))
            return false;

        if (procEntry.spellFamilyMask && !(procEntry.spellFamilyMask & eventInfo.GetSpellInfo()->SpellFamilyFlags))
            return false;
    }

    // check spell type mask (if set)
    if (eventInfo.GetTypeMask() & (SPELL_PROC_FLAG_MASK | PERIODIC_PROC_FLAG_MASK))
    {
        if (procEntry.spellTypeMask && !(eventInfo.GetSpellTypeMask() & procEntry.spellTypeMask))
            return false;
    }

    // check spell phase mask
    if (eventInfo.GetTypeMask() & REQ_SPELL_PHASE_PROC_FLAG_MASK)
    {
        if (!(eventInfo.GetSpellPhaseMask() & procEntry.spellPhaseMask))
            return false;
    }

    // check hit mask (on taken hit or on done hit, but not on spell cast phase)
    if ((eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK) || ((eventInfo.GetTypeMask() & DONE_HIT_PROC_FLAG_MASK) && !(eventInfo.GetSpellPhaseMask() & PROC_SPELL_PHASE_CAST)))
    {
        uint32 hitMask = procEntry.hitMask;
        // get default values if hit mask not set
        if (!hitMask)
        {
            // for taken procs allow normal + critical hits by default
            if (eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK)
                hitMask |= PROC_HIT_NORMAL | PROC_HIT_CRITICAL;
            // for done procs allow normal + critical + absorbs by default
            else
                hitMask |= PROC_HIT_NORMAL | PROC_HIT_CRITICAL | PROC_HIT_ABSORB;
        }
        if (!(eventInfo.GetHitMask() & hitMask))
            return false;
    }

    return true;
}

SpellBonusEntry const* SpellMgr::GetSpellBonusData(uint32 spellId) const
{
    // Lookup data
    SpellBonusMap::const_iterator itr = mSpellBonusMap.find(spellId);
    if (itr != mSpellBonusMap.end())
        return &itr->second;
    // Not found, try lookup for 1 spell rank if exist
    if (uint32 rank_1 = GetFirstSpellInChain(spellId))
    {
        SpellBonusMap::const_iterator itr2 = mSpellBonusMap.find(rank_1);
        if (itr2 != mSpellBonusMap.end())
            return &itr2->second;
    }
    return NULL;
}

SpellThreatEntry const* SpellMgr::GetSpellThreatEntry(uint32 spellID) const
{
    SpellThreatMap::const_iterator itr = mSpellThreatMap.find(spellID);
    if (itr != mSpellThreatMap.end())
        return &itr->second;
    else
    {
        uint32 firstSpell = GetFirstSpellInChain(spellID);
        itr = mSpellThreatMap.find(firstSpell);
        if (itr != mSpellThreatMap.end())
            return &itr->second;
    }
    return NULL;
}

SkillLineAbilityMapBounds SpellMgr::GetSkillLineAbilityMapBounds(uint32 spell_id) const
{
    return mSkillLineAbilityMap.equal_range(spell_id);
}

PetAura const* SpellMgr::GetPetAura(uint32 spell_id, uint8 eff) const
{
    SpellPetAuraMap::const_iterator itr = mSpellPetAuraMap.find((spell_id<<8) + eff);
    if (itr != mSpellPetAuraMap.end())
        return &itr->second;
    else
        return NULL;
}

SpellEnchantProcEntry const* SpellMgr::GetSpellEnchantProcEvent(uint32 enchId) const
{
    SpellEnchantProcEventMap::const_iterator itr = mSpellEnchantProcEventMap.find(enchId);
    if (itr != mSpellEnchantProcEventMap.end())
        return &itr->second;
    return NULL;
}

bool SpellMgr::IsArenaAllowedEnchancment(uint32 ench_id) const
{
    return mEnchantCustomAttr[ench_id];
}

const std::vector<int32>* SpellMgr::GetSpellLinked(int32 spell_id) const
{
    SpellLinkedMap::const_iterator itr = mSpellLinkedMap.find(spell_id);
    return itr != mSpellLinkedMap.end() ? &(itr->second) : NULL;
}

PetLevelupSpellSet const* SpellMgr::GetPetSpellList(uint32 petFamily) const
{
    PetLevelupSpellMap::const_iterator itr = mPetSpellMap.find(petFamily);
    if (itr != mPetSpellMap.end())
        return &itr->second;
    else
        return NULL;
}

SpellAreaMapBounds SpellMgr::GetSpellAreaMapBounds(uint32 spell_id) const
{
    return mSpellAreaMap.equal_range(spell_id);
}

SpellAreaForQuestMapBounds SpellMgr::GetSpellAreaForQuestMapBounds(uint32 quest_id) const
{
    return mSpellAreaForQuestMap.equal_range(quest_id);
}

SpellAreaForQuestMapBounds SpellMgr::GetSpellAreaForQuestEndMapBounds(uint32 quest_id) const
{
    return mSpellAreaForQuestEndMap.equal_range(quest_id);
}

SpellAreaForAuraMapBounds SpellMgr::GetSpellAreaForAuraMapBounds(uint32 spell_id) const
{
    return mSpellAreaForAuraMap.equal_range(spell_id);
}

SpellAreaForAreaMapBounds SpellMgr::GetSpellAreaForAreaMapBounds(uint32 area_id) const
{
    return mSpellAreaForAreaMap.equal_range(area_id);
}

bool SpellArea::IsFitToRequirements(Player const* player, uint32 newZone, uint32 newArea) const
{
    if (gender != GENDER_NONE)                   // not in expected gender
        if (!player || gender != player->GetGender())
            return false;

    if (raceMask)                                // not in expected race
        if (!player || !(raceMask & player->GetRaceMask()))
            return false;

    if (areaId)                                  // not in expected zone
        if (newZone != areaId && newArea != areaId)
            return false;

    if (questStart)                              // not in expected required quest state
    {
        Quest const* qInfo = sObjectMgr->GetQuestTemplate(questStart);
        if (qInfo && qInfo->IsRepeatable() && (questStartStatus & 0x40)) // QUEST_STATUS_REWARDED
        {
            if (!const_cast<Player*>(player)->IsDailyQuestDone(questStart))
                return false;
        }
        else if (!player || (((1 << player->GetQuestStatus(questStart)) & questStartStatus) == 0))
            return false;
    }

    if (questEnd)                                // not in expected forbidden quest state
    {
        Quest const* qInfo = sObjectMgr->GetQuestTemplate(questEnd);
        if (qInfo && qInfo->IsRepeatable() && (questEndStatus & 0x40)) // QUEST_STATUS_REWARDED
        {
            if (!const_cast<Player*>(player)->IsDailyQuestDone(questEnd))
                return false;
        }
        else if (!player || (((1 << player->GetQuestStatus(questEnd)) & questEndStatus) == 0))
            return false;
    }

    if (auraSpell)                               // not have expected aura
        if (!player || (auraSpell > 0 && !player->HasAura(auraSpell)) || (auraSpell < 0 && player->HasAura(-auraSpell)))
            return false;

    if (player)
    {
        if (Battleground* bg = player->GetBattleground())
            return bg->IsSpellAllowed(spellId, player);
    }

    // Extra conditions
    switch (spellId)
    {
        case 91604: // No fly Zone - Wintergrasp
        {
            if (!player)
                return false;

            Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId());
            if (!Bf || Bf->CanFlyIn() || (!player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) && !player->HasAuraType(SPELL_AURA_FLY)))
                return false;
            break;
        }
        case 56618: // Horde Controls Factory Phase Shift
        case 56617: // Alliance Controls Factory Phase Shift
        {
            if (!player)
                return false;

            Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId());

            if (!bf || bf->GetTypeId() != BATTLEFIELD_WG)
                return false;

            // team that controls the workshop in the specified area
            uint32 team = bf->GetData(newArea);

            if (team == TEAM_HORDE)
                return spellId == 56618;
            else if (team == TEAM_ALLIANCE)
                return spellId == 56617;
            break;
        }
        case 57940: // Essence of Wintergrasp - Northrend
        case 58045: // Essence of Wintergrasp - Wintergrasp
        {
            if (!player)
                return false;

            if (Battlefield* battlefieldWG = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_WG))
                return battlefieldWG->IsEnabled() && (player->GetTeamId() == battlefieldWG->GetDefenderTeam()) && !battlefieldWG->IsWarTime();
            break;
        }
        case 74411: // Battleground - Dampening
        {
            if (!player)
                return false;

            if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId()))
                return bf->IsWarTime();
            break;
        }

    }

    return true;
}

SpellInfo const* SpellMgr::GetSpellInfo(uint32 spellId, Difficulty difficulty) const
{
    if (spellId < GetSpellInfoStoreSize())
    {
        if (mSpellInfoMap[difficulty][spellId])
            return mSpellInfoMap[difficulty][spellId];

        if (difficulty == RAID_DIFFICULTY_25MAN_HEROIC && mSpellInfoMap[RAID_DIFFICULTY_25MAN_NORMAL][spellId])
            return mSpellInfoMap[RAID_DIFFICULTY_25MAN_NORMAL][spellId];
        return mSpellInfoMap[REGULAR_DIFFICULTY][spellId];
    }

    return nullptr;
}

void SpellMgr::UnloadSpellInfoChains()
{
    for (auto&& it : mSpellChains)
        for (auto&& spellInfoMap : mSpellInfoMap)
            spellInfoMap[it.first]->ChainEntry = nullptr;

    mSpellChains.clear();
}

void SpellMgr::LoadSpellRanks()
{
    // cleanup data and load spell ranks for talents from dbc
    uint32 oldMSTime = getMSTime();

    //                                                     0             1      2
    QueryResult result = WorldDatabase.Query("SELECT first_spell_id, spell_id, `rank` from spell_ranks ORDER BY first_spell_id, `rank`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell rank records. DB table `spell_ranks` is empty.");
        return;
    }

    uint32 count = 0;
    bool finished = false;

    do
    {
                        // spellid, rank
        std::list < std::pair < int32, int32 > > rankChain;
        int32 currentSpell = -1;
        int32 lastSpell = -1;

        // fill one chain
        while (currentSpell == lastSpell && !finished)
        {
            Field* fields = result->Fetch();

            currentSpell = fields[0].GetUInt32();
            if (lastSpell == -1)
                lastSpell = currentSpell;
            uint32 spell_id = fields[1].GetUInt32();
            uint32 rank = fields[2].GetUInt8();

            // don't drop the row if we're moving to the next rank
            if (currentSpell == lastSpell)
            {
                rankChain.push_back(std::make_pair(spell_id, rank));
                if (!result->NextRow())
                    finished = true;
            }
            else
                break;
        }
        // check if chain is made with valid first spell
        SpellInfo const* first = GetSpellInfo(lastSpell);
        if (!first)
        {
            TC_LOG_ERROR("sql.sql", "Spell rank identifier(first_spell_id) %u listed in `spell_ranks` does not exist!", lastSpell);
            continue;
        }
        // check if chain is long enough
        if (rankChain.size() < 2)
        {
            TC_LOG_ERROR("sql.sql", "There is only 1 spell rank for identifier(first_spell_id) %u in `spell_ranks`, entry is not needed!", lastSpell);
            continue;
        }
        int32 curRank = 0;
        bool valid = true;
        // check spells in chain
        for (std::list<std::pair<int32, int32> >::iterator itr = rankChain.begin(); itr!= rankChain.end(); ++itr)
        {
            SpellInfo const* spell = GetSpellInfo(itr->first);
            if (!spell)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u (rank %u) listed in `spell_ranks` for chain %u does not exist!", itr->first, itr->second, lastSpell);
                valid = false;
                break;
            }
            ++curRank;
            if (itr->second != curRank)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u (rank %u) listed in `spell_ranks` for chain %u does not have proper rank value(should be %u)!", itr->first, itr->second, lastSpell, curRank);
                valid = false;
                break;
            }
        }
        if (!valid)
            continue;
        int32 prevRank = 0;
        // insert the chain
        std::list<std::pair<int32, int32> >::iterator itr = rankChain.begin();
        do
        {
            ++count;
            int32 addedSpell = itr->first;

            for (auto&& spellInfoMap : mSpellInfoMap)
            {
                if (spellInfoMap[addedSpell])
                {
                    if (spellInfoMap[addedSpell]->ChainEntry)
                    {
                        TC_LOG_ERROR("sql.sql", "Spell %u (rank: %u, first: %u) listed in `spell_ranks` has already ChainEntry from dbc.", addedSpell, itr->second, lastSpell);
                    }
                }
            }

            mSpellChains[addedSpell].first = GetSpellInfo(lastSpell);
            mSpellChains[addedSpell].last = GetSpellInfo(rankChain.back().first);
            mSpellChains[addedSpell].rank = itr->second;
            mSpellChains[addedSpell].prev = GetSpellInfo(prevRank);
            for (auto&& spellInfoMap : mSpellInfoMap)
                if (spellInfoMap[addedSpell])
                    spellInfoMap[addedSpell]->ChainEntry = &mSpellChains[addedSpell];
            prevRank = addedSpell;
            ++itr;

            if (itr == rankChain.end())
            {
                mSpellChains[addedSpell].next = NULL;
                break;
            }
            else
                mSpellChains[addedSpell].next = GetSpellInfo(itr->first);
        }
        while (true);
    }
    while (!finished);

    TC_LOG_INFO("server.loading", ">> Loaded %u spell rank records in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellRequired()
{
    uint32 oldMSTime = getMSTime();

    mSpellsReqSpell.clear();                                   // need for reload case
    mSpellReq.clear();                                         // need for reload case

    //                                                   0        1
    QueryResult result = WorldDatabase.Query("SELECT spell_id, req_spell from spell_required");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell required records. DB table `spell_required` is empty.");

        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell_id = fields[0].GetUInt32();
        uint32 spell_req = fields[1].GetUInt32();

        // check if chain is made with valid first spell
        SpellInfo const* spell = GetSpellInfo(spell_id);
        if (!spell)
        {
            TC_LOG_ERROR("sql.sql", "spell_id %u in `spell_required` table is not found in dbcs, skipped", spell_id);
            continue;
        }

        SpellInfo const* reqSpell = GetSpellInfo(spell_req);
        if (!reqSpell)
        {
            TC_LOG_ERROR("sql.sql", "req_spell %u in `spell_required` table is not found in dbcs, skipped", spell_req);
            continue;
        }

        if (spell->IsRankOf(reqSpell))
        {
            TC_LOG_ERROR("sql.sql", "req_spell %u and spell_id %u in `spell_required` table are ranks of the same spell, entry not needed, skipped", spell_req, spell_id);
            continue;
        }

        if (IsSpellRequiringSpell(spell_id, spell_req))
        {
            TC_LOG_ERROR("sql.sql", "duplicated entry of req_spell %u and spell_id %u in `spell_required`, skipped", spell_req, spell_id);
            continue;
        }

        mSpellReq.insert (std::pair<uint32, uint32>(spell_id, spell_req));
        mSpellsReqSpell.insert (std::pair<uint32, uint32>(spell_req, spell_id));
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u spell required records in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

}

void SpellMgr::LoadSpellLearnSkills()
{
    uint32 oldMSTime = getMSTime();

    mSpellLearnSkills.clear();                              // need for reload case

    // search auto-learned skills and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < GetSpellInfoStoreSize(); ++spell)
    {
        SpellInfo const* entry = GetSpellInfo(spell);

        if (!entry)
            continue;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (entry->Effects[i].Effect == SPELL_EFFECT_SKILL)
            {
                SpellLearnSkillNode dbcNode;
                dbcNode.skill = entry->Effects[i].MiscValue;
                dbcNode.step  = entry->Effects[i].CalcValue();
                if (dbcNode.skill == SKILL_RIDING)
                    dbcNode.value = dbcNode.step * 75;
                else if (dbcNode.skill >= SKILL_WAY_OF_THE_GRILL && dbcNode.skill <= SKILL_WAY_OF_THE_BREW)
                {
                    dbcNode.value = 525;
                    if (dbcNode.skill == SKILL_WAY_OF_THE_BREW) // Error in dbc?
                        dbcNode.step = 1;
                }
                else
                    dbcNode.value = 1;

                auto entry = sDBCManager.GetSkillRaceClassInfo(dbcNode.skill, 0, 0);
                if (entry)
                {
                    auto skillTier = sSkillTiersStore.LookupEntry(entry->SkillTierID);
                    if (skillTier)
                        dbcNode.maxvalue = skillTier->MaxSkillValue[dbcNode.step - 1];
                    else
                    {
                        TC_LOG_ERROR("server.loading", "SpellMgr::LoadSpellLearnSkills skill tier (%u) not found for skill %u", entry->SkillTierID, dbcNode.skill);
                        dbcNode.maxvalue = dbcNode.step * 75;
                    }
                }
                else
                {
                    TC_LOG_ERROR("server.loading", "SpellMgr::LoadSpellLearnSkills skill (%u) not found in SkillRaceClassInfo.dbc", dbcNode.skill);
                    dbcNode.maxvalue = dbcNode.step * 75;
                }

                mSpellLearnSkills[entry->ID] = dbcNode;
                ++dbc_count;
                break;
            }
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u Spell Learn Skills from DBC in %u ms", dbc_count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellLearnSpells()
{
    uint32 oldMSTime = getMSTime();

    mSpellLearnSpells.clear();                              // need for reload case

    //                                                  0      1        2
    QueryResult result = WorldDatabase.Query("SELECT entry, SpellID, Active FROM spell_learn_spell");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell learn spells. DB table `spell_learn_spell` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell_id = fields[0].GetUInt32();

        SpellLearnSpellNode node;
        node.spell       = fields[1].GetUInt32();
        node.active      = fields[2].GetBool();
        node.autoLearned = false;

        if (!GetSpellInfo(spell_id))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_learn_spell` does not exist", spell_id);
            continue;
        }

        if (!GetSpellInfo(node.spell))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_learn_spell` learning not existed spell %u", spell_id, node.spell);
            continue;
        }

        if (sDBCManager.GetTalentSpellCost(node.spell))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_learn_spell` attempt learning talent spell %u, skipped", spell_id, node.spell);
            continue;
        }

        mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell_id, node));

        ++count;
    } while (result->NextRow());

    // copy state loaded from db
    SpellLearnSpellMap dbSpellLearnSpells = mSpellLearnSpells;

    // search auto-learned spells and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < GetSpellInfoStoreSize(); ++spell)
    {
        SpellInfo const* entry = GetSpellInfo(spell);

        if (!entry)
            continue;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (entry->Effects[i].Effect == SPELL_EFFECT_LEARN_SPELL)
            {
                SpellLearnSpellNode dbc_node;
                dbc_node.spell = entry->Effects[i].TriggerSpell;
                dbc_node.active = true;                     // all dbc based learned spells is active (show in spell book or hide by client itself)

                // ignore learning not existed spells (broken/outdated/or generic learnig spell 483
                if (!GetSpellInfo(dbc_node.spell))
                    continue;

                // talent or passive spells or skill-step spells auto-casted and not need dependent learning,
                // pet teaching spells must not be dependent learning (casted)
                // other required explicit dependent learning
                dbc_node.autoLearned = entry->Effects[i].TargetA.GetTarget() == TARGET_UNIT_PET || sDBCManager.GetTalentSpellCost(spell) > 0 || entry->IsPassive() || entry->HasEffect(SPELL_EFFECT_SKILL_STEP);

                SpellLearnSpellMapBounds db_node_bounds = dbSpellLearnSpells.equal_range(spell);

                bool found = false;
                for (SpellLearnSpellMap::const_iterator itr = db_node_bounds.first; itr != db_node_bounds.second; ++itr)
                {
                    if (itr->second.spell == dbc_node.spell)
                    {
                        TC_LOG_ERROR("sql.sql", "Spell %u auto-learn spell %u in spell.dbc then the record in `spell_learn_spell` is redundant, please fix DB.",
                            spell, dbc_node.spell);
                        found = true;
                        break;
                    }
                }

                if (!found)                                  // add new spell-spell pair if not found
                {
                    mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell, dbc_node));
                    ++dbc_count;
                }
            }
        }
    }
    /*
    uint32 mastery_count = 0;
    for (uint32 i = 0; i < sTalentTabStore.GetNumRows(); ++i)
    {
        TalentTabEntry const* talentTab = sTalentTabStore.LookupEntry(i);
        if (!talentTab)
            continue;

        for (uint32 c = CLASS_WARRIOR; c < MAX_CLASSES; ++c)
        {
            if (!(talentTab->ClassMask & (1 << (c - 1))))
                continue;

            uint32 masteryMainSpell = MasterySpells[c];

            for (uint32 m = 0; m < MAX_MASTERY_SPELLS; ++m)
            {
                uint32 mastery = talentTab->MasterySpellId[m];
                if (!mastery)
                    continue;

                SpellLearnSpellMapBounds db_node_bounds = dbSpellLearnSpells.equal_range(masteryMainSpell);
                bool found = false;
                for (SpellLearnSpellMap::const_iterator itr = db_node_bounds.first; itr != db_node_bounds.second; ++itr)
                {
                    if (itr->second.spell == mastery)
                    {
                        TC_LOG_ERROR("sql.sql", "Found redundant record (entry: %u, SpellID: %u) in `spell_learn_spell`, spell added automatically as mastery learned spell from TalentTab.dbc", masteryMainSpell, mastery);
                        found = true;
                        break;
                    }
                }

                if (found)
                    continue;

                // Check if it is already found in Spell.dbc, ignore silently if yes
                SpellLearnSpellMapBounds dbc_node_bounds = GetSpellLearnSpellMapBounds(masteryMainSpell);
                found = false;
                for (SpellLearnSpellMap::const_iterator itr = dbc_node_bounds.first; itr != dbc_node_bounds.second; ++itr)
                {
                    if (itr->second.spell == mastery)
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                    continue;

                SpellLearnSpellNode masteryNode;
                masteryNode.spell       = mastery;
                masteryNode.active      = true;
                masteryNode.autoLearned = false;

                mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(masteryMainSpell, masteryNode));
                ++mastery_count;
            }
        }
    }
    */
    TC_LOG_INFO("server.loading", ">> Loaded %u spell learn spells, %u found in Spell.dbc and %u from TalentTab.dbc in %u ms", count, dbc_count, 0, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellTargetPositions()
{
    uint32 oldMSTime = getMSTime();

    mSpellTargetPositions.clear();                                // need for reload case

    //                                                0      1          2             3                  4                  5                   6
    QueryResult result = WorldDatabase.Query("SELECT id, effIndex, target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM spell_target_position");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell target coordinates. DB table `spell_target_position` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 Spell_ID = fields[0].GetUInt32();
        SpellEffIndex effIndex = SpellEffIndex(fields[1].GetUInt8());

        SpellTargetPosition st;

        st.target_mapId       = fields[2].GetUInt16();
        st.target_X           = fields[3].GetFloat();
        st.target_Y           = fields[4].GetFloat();
        st.target_Z           = fields[5].GetFloat();
        st.target_Orientation = fields[6].GetFloat();

        MapEntry const* mapEntry = sMapStore.LookupEntry(st.target_mapId);
        if (!mapEntry)
        {
            TC_LOG_ERROR("sql.sql", "Spell (Id: %u, effIndex: %u) target map (ID: %u) does not exist in `Map.dbc`.", Spell_ID, effIndex, st.target_mapId);
            continue;
        }

        if (st.target_X==0 && st.target_Y==0 && st.target_Z==0)
        {
            TC_LOG_ERROR("sql.sql", "Spell (Id: %u, effIndex: %u) target coordinates not provided.", Spell_ID, effIndex);
            continue;
        }

        SpellInfo const* spellInfo = GetSpellInfo(Spell_ID);
        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Spell (Id: %u) listed in `spell_target_position` does not exist.", Spell_ID);
            continue;
        }

        if (spellInfo->Effects[effIndex].TargetA.GetTarget() == TARGET_DEST_DB || spellInfo->Effects[effIndex].TargetB.GetTarget() == TARGET_DEST_DB)
        {
            std::pair<uint32, SpellEffIndex> key = std::make_pair(Spell_ID, effIndex);
            mSpellTargetPositions[key] = st;
            ++count;
        }
        else
        {
            TC_LOG_ERROR("sql.sql", "Spell (Id: %u, effIndex: %u) listed in `spell_target_position` does not have target TARGET_DEST_DB (17).", Spell_ID, effIndex);
            continue;
        }

    } while (result->NextRow());

    /*
    // Check all spells
    for (uint32 i = 1; i < GetSpellInfoStoreSize; ++i)
    {
        SpellInfo const* spellInfo = GetSpellInfo(i);
        if (!spellInfo)
            continue;

        bool found = false;
        for (int j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            switch (spellInfo->Effects[j].TargetA)
            {
                case TARGET_DEST_DB:
                    found = true;
                    break;
            }
            if (found)
                break;
            switch (spellInfo->Effects[j].TargetB)
            {
                case TARGET_DEST_DB:
                    found = true;
                    break;
            }
            if (found)
                break;
        }
        if (found)
        {
            if (!sSpellMgr->GetSpellTargetPosition(i))
                TC_LOG_DEBUG("spells", "Spell (ID: %u) does not have record in `spell_target_position`", i);
        }
    }*/

    TC_LOG_INFO("server.loading", ">> Loaded %u spell teleport coordinates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellGroups()
{
    uint32 oldMSTime = getMSTime();

    mSpellSpellGroup.clear();                                  // need for reload case
    mSpellGroupSpell.clear();

    //                                                0     1
    QueryResult result = WorldDatabase.Query("SELECT id, spell_id FROM spell_group");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell group definitions. DB table `spell_group` is empty.");
        return;
    }

    std::set<uint32> groups;
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 group_id = fields[0].GetUInt32();
        if (group_id <= SPELL_GROUP_DB_RANGE_MIN && group_id >= SPELL_GROUP_CORE_RANGE_MAX)
        {
            TC_LOG_ERROR("sql.sql", "SpellGroup id %u listed in `spell_group` is in core range, but is not defined in core!", group_id);
            continue;
        }
        int32 spell_id = fields[1].GetInt32();

        groups.insert(std::set<uint32>::value_type(group_id));
        mSpellGroupSpell.insert(SpellGroupSpellMap::value_type((SpellGroup)group_id, spell_id));

    } while (result->NextRow());

    for (SpellGroupSpellMap::iterator itr = mSpellGroupSpell.begin(); itr!= mSpellGroupSpell.end();)
    {
        if (itr->second < 0)
        {
            if (groups.find(abs(itr->second)) == groups.end())
            {
                TC_LOG_ERROR("sql.sql", "SpellGroup id %u listed in `spell_group` does not exist", abs(itr->second));
                mSpellGroupSpell.erase(itr++);
            }
            else
                ++itr;
        }
        else
        {
            SpellInfo const* spellInfo = GetSpellInfo(itr->second);

            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_group` does not exist", itr->second);
                mSpellGroupSpell.erase(itr++);
            }
            else if (spellInfo->GetRank() > 1)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_group` is not first rank of spell", itr->second);
                mSpellGroupSpell.erase(itr++);
            }
            else
                ++itr;
        }
    }

    for (std::set<uint32>::iterator groupItr = groups.begin(); groupItr != groups.end(); ++groupItr)
    {
        std::set<uint32> spells;
        GetSetOfSpellsInSpellGroup(SpellGroup(*groupItr), spells);

        for (std::set<uint32>::iterator spellItr = spells.begin(); spellItr != spells.end(); ++spellItr)
        {
            ++count;
            mSpellSpellGroup.insert(SpellSpellGroupMap::value_type(*spellItr, SpellGroup(*groupItr)));
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u spell group definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellGroupStackRules()
{
    uint32 oldMSTime = getMSTime();

    mSpellGroupStack.clear();                                  // need for reload case

    //                                                       0         1
    QueryResult result = WorldDatabase.Query("SELECT group_id, stack_rule FROM spell_group_stack_rules");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell group stack rules. DB table `spell_group_stack_rules` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 group_id = fields[0].GetUInt32();
        uint8 stack_rule = fields[1].GetInt8();
        if (stack_rule >= SPELL_GROUP_STACK_RULE_MAX)
        {
            TC_LOG_ERROR("sql.sql", "SpellGroupStackRule %u listed in `spell_group_stack_rules` does not exist", stack_rule);
            continue;
        }

        SpellGroupSpellMapBounds spellGroup = GetSpellGroupSpellMapBounds((SpellGroup)group_id);

        if (spellGroup.first == spellGroup.second)
        {
            TC_LOG_ERROR("sql.sql", "SpellGroup id %u listed in `spell_group_stack_rules` does not exist", group_id);
            continue;
        }

        mSpellGroupStack[(SpellGroup)group_id] = (SpellGroupStackRule)stack_rule;

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u spell group stack rules in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellProcEvents()
{
    uint32 oldMSTime = getMSTime();

    mSpellProcEventMap.clear();                             // need for reload case

    //                                                0      1           2                3                 4                 5                6                 7          8        9       10            11         12
    QueryResult result = WorldDatabase.Query("SELECT entry, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, SpellFamilyMask3, procFlags, procEx, ppmRate, CustomChance, Cooldown, phase+0 FROM spell_proc_event");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell proc event conditions. DB table `spell_proc_event` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        int i = 0;

        int32 spellId = fields[i++].GetInt32();

        bool allRanks = false;
        if (spellId < 0)
        {
            allRanks = true;
            spellId = -spellId;
        }

        SpellInfo const* spellInfo = GetSpellInfo(spellId);
        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc_event` does not exist", spellId);
            continue;
        }

        if (allRanks)
        {
            if (!spellInfo->IsRanked())
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc_event` with all ranks, but spell has no ranks.", spellId);

            if (spellInfo->GetFirstRankSpell()->Id != uint32(spellId))
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc_event` is not first rank of spell.", spellId);
                continue;
            }
        }

        SpellProcEventEntry spellProcEvent;

        spellProcEvent.schoolMask         = fields[i++].GetInt8();
        spellProcEvent.spellFamilyName    = fields[i++].GetUInt16();
        spellProcEvent.spellFamilyMask[0] = fields[i++].GetUInt32();
        spellProcEvent.spellFamilyMask[1] = fields[i++].GetUInt32();
        spellProcEvent.spellFamilyMask[2] = fields[i++].GetUInt32();
        spellProcEvent.spellFamilyMask[3] = fields[i++].GetUInt32();
        spellProcEvent.procFlags          = fields[i++].GetUInt32();
        spellProcEvent.procEx             = fields[i++].GetUInt32();
        spellProcEvent.ppmRate            = fields[i++].GetFloat();
        spellProcEvent.customChance       = fields[i++].GetFloat();
        spellProcEvent.cooldown           = fields[i++].GetUInt32();
        spellProcEvent.phaseMask          = ProcPhaseMask(fields[i++].GetUInt32());

        while (spellInfo)
        {
            if (mSpellProcEventMap.find(spellInfo->Id) != mSpellProcEventMap.end())
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc_event` already has its first rank in table.", spellInfo->Id);
                break;
            }

            if (!spellInfo->ProcFlags && !spellProcEvent.procFlags)
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc_event` probably not triggered spell", spellInfo->Id);

            mSpellProcEventMap[spellInfo->Id] = spellProcEvent;

            if (allRanks)
                spellInfo = spellInfo->GetNextRankSpell();
            else
                break;
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u extra spell proc event conditions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellProcs()
{
    uint32 oldMSTime = getMSTime();

    mSpellProcMap.clear();                             // need for reload case

    //                                                 0        1           2                3                 4                 5                 6         7              8               9        10              11             12      13        14
    QueryResult result = WorldDatabase.Query("SELECT spellId, schoolMask, spellFamilyName, spellFamilyMask0, spellFamilyMask1, spellFamilyMask2, typeMask, spellTypeMask, spellPhaseMask, hitMask, attributesMask, ratePerMinute, chance, cooldown, charges FROM spell_proc");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell proc conditions and data. DB table `spell_proc` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 spellId = fields[0].GetInt32();

        bool allRanks = false;
        if (spellId < 0)
        {
            allRanks = true;
            spellId = -spellId;
        }

        SpellInfo const* spellInfo = GetSpellInfo(spellId);
        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc` does not exist", spellId);
            continue;
        }

        if (allRanks)
        {
            if (!spellInfo->IsRanked())
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc` with all ranks, but spell has no ranks.", spellId);

            if (spellInfo->GetFirstRankSpell()->Id != uint32(spellId))
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc` is not first rank of spell.", spellId);
                continue;
            }
        }

        SpellProcEntry baseProcEntry;

        baseProcEntry.schoolMask      = fields[1].GetInt8();
        baseProcEntry.spellFamilyName = fields[2].GetUInt16();
        baseProcEntry.spellFamilyMask[0] = fields[3].GetUInt32();
        baseProcEntry.spellFamilyMask[1] = fields[4].GetUInt32();
        baseProcEntry.spellFamilyMask[2] = fields[5].GetUInt32();
        baseProcEntry.typeMask        = fields[6].GetUInt32();
        baseProcEntry.spellTypeMask   = fields[7].GetUInt32();
        baseProcEntry.spellPhaseMask  = fields[8].GetUInt32();
        baseProcEntry.hitMask         = fields[9].GetUInt32();
        baseProcEntry.attributesMask  = fields[10].GetUInt32();
        baseProcEntry.ratePerMinute   = fields[11].GetFloat();
        baseProcEntry.chance          = fields[12].GetFloat();
        float cooldown                = fields[13].GetFloat();
        baseProcEntry.cooldown        = uint32(cooldown);
        baseProcEntry.charges         = fields[14].GetUInt32();

        while (spellInfo)
        {
            if (mSpellProcMap.find(spellInfo->Id) != mSpellProcMap.end())
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_proc` already has its first rank in table.", spellInfo->Id);
                break;
            }

            SpellProcEntry procEntry = SpellProcEntry(baseProcEntry);

            // take defaults from dbcs
            if (!procEntry.typeMask)
                procEntry.typeMask = spellInfo->ProcFlags;
            if (!procEntry.charges)
                procEntry.charges = spellInfo->ProcCharges;
            if (!procEntry.chance && !procEntry.ratePerMinute)
                procEntry.chance = float(spellInfo->ProcChance);

            // validate data
            if (procEntry.schoolMask & ~SPELL_SCHOOL_MASK_ALL)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has wrong `schoolMask` set: %u", spellInfo->Id, procEntry.schoolMask);
            if (procEntry.spellFamilyName && (procEntry.spellFamilyName < 3 || procEntry.spellFamilyName > 17 || procEntry.spellFamilyName == 14 || procEntry.spellFamilyName == 16))
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has wrong `spellFamilyName` set: %u", spellInfo->Id, procEntry.spellFamilyName);
            if (procEntry.chance < 0)
            {
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has negative value in `chance` field", spellInfo->Id);
                procEntry.chance = 0;
            }
            if (procEntry.ratePerMinute < 0)
            {
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has negative value in `ratePerMinute` field", spellInfo->Id);
                procEntry.ratePerMinute = 0;
            }
            if (cooldown < 0)
            {
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has negative value in `cooldown` field", spellInfo->Id);
                procEntry.cooldown = 0;
            }
            if (procEntry.chance == 0 && procEntry.ratePerMinute == 0)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u doesn't have `chance` and `ratePerMinute` values defined, proc will not be triggered", spellInfo->Id);
            if (procEntry.charges > 99)
            {
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has too big value in `charges` field", spellInfo->Id);
                procEntry.charges = 99;
            }
            if (!procEntry.typeMask)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u doesn't have `typeMask` value defined, proc will not be triggered", spellInfo->Id);
            if (procEntry.spellTypeMask & ~PROC_SPELL_TYPE_MASK_ALL)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has wrong `spellTypeMask` set: %u", spellInfo->Id, procEntry.spellTypeMask);
            if (procEntry.spellTypeMask && !(procEntry.typeMask & (SPELL_PROC_FLAG_MASK | PERIODIC_PROC_FLAG_MASK)))
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has `spellTypeMask` value defined, but it won't be used for defined `typeMask` value", spellInfo->Id);
            if (!procEntry.spellPhaseMask && procEntry.typeMask & REQ_SPELL_PHASE_PROC_FLAG_MASK)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u doesn't have `spellPhaseMask` value defined, but it's required for defined `typeMask` value, proc will not be triggered", spellInfo->Id);
            if (procEntry.spellPhaseMask & ~PROC_SPELL_PHASE_MASK_ALL)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has wrong `spellPhaseMask` set: %u", spellInfo->Id, procEntry.spellPhaseMask);
            if (procEntry.spellPhaseMask && !(procEntry.typeMask & REQ_SPELL_PHASE_PROC_FLAG_MASK))
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has `spellPhaseMask` value defined, but it won't be used for defined `typeMask` value", spellInfo->Id);
            if (procEntry.hitMask & ~PROC_HIT_MASK_ALL)
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has wrong `hitMask` set: %u", spellInfo->Id, procEntry.hitMask);
            if (procEntry.hitMask && !(procEntry.typeMask & TAKEN_HIT_PROC_FLAG_MASK || (procEntry.typeMask & DONE_HIT_PROC_FLAG_MASK && (!procEntry.spellPhaseMask || procEntry.spellPhaseMask & (PROC_SPELL_PHASE_HIT | PROC_SPELL_PHASE_FINISH)))))
                TC_LOG_ERROR("sql.sql", "`spell_proc` table entry for spellId %u has `hitMask` value defined, but it won't be used for defined `typeMask` and `spellPhaseMask` values", spellInfo->Id);

            mSpellProcMap[spellInfo->Id] = procEntry;

            if (allRanks)
                spellInfo = spellInfo->GetNextRankSpell();
            else
                break;
        }
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u spell proc conditions and data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellBonusess()
{
    uint32 oldMSTime = getMSTime();

    mSpellBonusMap.clear();                             // need for reload case

    //                                                0      1             2          3         4
    QueryResult result = WorldDatabase.Query("SELECT entry, direct_bonus, dot_bonus, ap_bonus, ap_dot_bonus FROM spell_bonus_data");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell bonus data. DB table `spell_bonus_data` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();

        SpellInfo const* spell = GetSpellInfo(entry);
        if (!spell)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_bonus_data` does not exist", entry);
            continue;
        }

        SpellBonusEntry& sbe = mSpellBonusMap[entry];
        sbe.direct_damage = fields[1].GetFloat();
        sbe.dot_damage    = fields[2].GetFloat();
        sbe.ap_bonus      = fields[3].GetFloat();
        sbe.ap_dot_bonus   = fields[4].GetFloat();

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u extra spell bonus data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellThreats()
{
    uint32 oldMSTime = getMSTime();

    mSpellThreatMap.clear();                                // need for reload case

    //                                                0      1        2       3
    QueryResult result = WorldDatabase.Query("SELECT entry, flatMod, pctMod, apPctMod FROM spell_threat");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 aggro generating spells. DB table `spell_threat` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetSpellInfo(entry))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_threat` does not exist", entry);
            continue;
        }

        SpellThreatEntry ste;
        ste.flatMod  = fields[1].GetInt32();
        ste.pctMod   = fields[2].GetFloat();
        ste.apPctMod = fields[3].GetFloat();

        mSpellThreatMap[entry] = ste;
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u SpellThreatEntries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSkillLineAbilityMap()
{
    uint32 oldMSTime = getMSTime();

    mSkillLineAbilityMap.clear();

    uint32 count = 0;

    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); ++i)
    {
        SkillLineAbilityEntry const* SkillInfo = sSkillLineAbilityStore.LookupEntry(i);
        if (!SkillInfo)
            continue;

        mSkillLineAbilityMap.insert(SkillLineAbilityMap::value_type(SkillInfo->Spell, SkillInfo));
        ++count;
    }

    for (uint32 i = 0; i < sSpellEffectStore.GetNumRows(); ++i)
    {
        auto entry = sSpellEffectStore.LookupEntry(i);
        if (!entry || entry->Effect != SPELL_EFFECT_APPLY_GLYPH)
            continue;

        bool found = false;
        auto bounds = sSpellMgr->GetSkillLineAbilityMapBounds(entry->EffectSpellId);
        for (auto it = bounds.first; it != bounds.second && !found; ++it)
        {
            for (uint32 c = CLASS_WARRIOR; c < MAX_CLASSES; ++c)
            {
                if (it->second->ClassMask & (1 << (c - 1)))
                {
                    mGlyphSpells[c].push_back(entry->EffectSpellId);
                    found = true;
                    break;
                }
            }
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u SkillLineAbility MultiMap Data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellPetAuras()
{
    uint32 oldMSTime = getMSTime();

    mSpellPetAuraMap.clear();                                  // need for reload case

    //                                                  0       1       2    3
    QueryResult result = WorldDatabase.Query("SELECT spell, effectId, pet, aura FROM spell_pet_auras");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell pet auras. DB table `spell_pet_auras` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell = fields[0].GetUInt32();
        uint8 eff = fields[1].GetUInt8();
        uint32 pet = fields[2].GetUInt32();
        uint32 aura = fields[3].GetUInt32();

        SpellPetAuraMap::iterator itr = mSpellPetAuraMap.find((spell<<8) + eff);
        if (itr != mSpellPetAuraMap.end())
            itr->second.AddAura(pet, aura);
        else
        {
            SpellInfo const* spellInfo = GetSpellInfo(spell);
            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_pet_auras` does not exist", spell);
                continue;
            }
            if (spellInfo->Effects[eff].Effect != SPELL_EFFECT_DUMMY &&
               (spellInfo->Effects[eff].Effect != SPELL_EFFECT_APPLY_AURA ||
                spellInfo->Effects[eff].ApplyAuraName != SPELL_AURA_DUMMY))
            {
                TC_LOG_ERROR("spells", "Spell %u listed in `spell_pet_auras` does not have dummy aura or dummy effect", spell);
                continue;
            }

            SpellInfo const* spellInfo2 = GetSpellInfo(aura);
            if (!spellInfo2)
            {
                TC_LOG_ERROR("sql.sql", "Aura %u listed in `spell_pet_auras` does not exist", aura);
                continue;
            }

            PetAura pa(pet, aura, spellInfo->Effects[eff].TargetA.GetTarget() == TARGET_UNIT_PET, spellInfo->Effects[eff].CalcValue());
            mSpellPetAuraMap[(spell<<8) + eff] = pa;
        }

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u spell pet auras in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

// Fill custom data about enchancments
void SpellMgr::LoadEnchantCustomAttr()
{
    uint32 oldMSTime = getMSTime();

    uint32 size = sSpellItemEnchantmentStore.GetNumRows();
    mEnchantCustomAttr.resize(size);

    for (uint32 i = 0; i < size; ++i)
       mEnchantCustomAttr[i] = 0;

    uint32 count = 0;
    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellInfo = GetSpellInfo(i);
        if (!spellInfo)
            continue;

        /// @todo find a better check
        if (!(spellInfo->AttributesEx2 & SPELL_ATTR2_PRESERVE_ENCHANT_IN_ARENA) || !(spellInfo->Attributes & SPELL_ATTR0_NOT_SHAPESHIFT))
            continue;

        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (spellInfo->Effects[j].Effect == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY)
            {
                uint32 enchId = spellInfo->Effects[j].MiscValue;
                SpellItemEnchantmentEntry const* ench = sSpellItemEnchantmentStore.LookupEntry(enchId);
                if (!ench)
                    continue;
                mEnchantCustomAttr[enchId] = true;
                ++count;
                break;
            }
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u custom enchant attributes in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellEnchantProcData()
{
    uint32 oldMSTime = getMSTime();

    mSpellEnchantProcEventMap.clear();                             // need for reload case

    //                                                  0         1           2         3
    QueryResult result = WorldDatabase.Query("SELECT entry, customChance, PPMChance, procEx FROM spell_enchant_proc_data");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell enchant proc event conditions. DB table `spell_enchant_proc_data` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 enchantId = fields[0].GetUInt32();

        SpellItemEnchantmentEntry const* ench = sSpellItemEnchantmentStore.LookupEntry(enchantId);
        if (!ench)
        {
            TC_LOG_ERROR("sql.sql", "Enchancment %u listed in `spell_enchant_proc_data` does not exist", enchantId);
            continue;
        }

        SpellEnchantProcEntry spe;

        spe.customChance = fields[1].GetUInt32();
        spe.PPMChance = fields[2].GetFloat();
        spe.procEx = fields[3].GetUInt32();

        mSpellEnchantProcEventMap[enchantId] = spe;

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u enchant proc data definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellLinked()
{
    uint32 oldMSTime = getMSTime();

    mSpellLinkedMap.clear();    // need for reload case

    //                                                0              1             2
    QueryResult result = WorldDatabase.Query("SELECT spell_trigger, spell_effect, type FROM spell_linked_spell");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 linked spells. DB table `spell_linked_spell` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 trigger = fields[0].GetInt32();
        int32 effect = fields[1].GetInt32();
        int32 type = fields[2].GetUInt8();

        SpellInfo const* spellInfo = GetSpellInfo(abs(trigger));
        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_linked_spell` does not exist", abs(trigger));
            continue;
        }
        spellInfo = GetSpellInfo(abs(effect));
        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_linked_spell` does not exist", abs(effect));
            continue;
        }

        if (type) //we will find a better way when more types are needed
        {
            if (trigger > 0)
                trigger += SPELL_LINKED_MAX_SPELLS * type;
            else
                trigger -= SPELL_LINKED_MAX_SPELLS * type;
        }
        mSpellLinkedMap[trigger].push_back(effect);

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u linked spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadPetSpellMap()
{
    uint32 oldMSTime = getMSTime();

    mPetSpellMap.clear();                                   // need for reload case

    uint32 count = 0;
    uint32 family_count = 0;

    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); ++i)
    {
        SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(i);
        if (!skillLine)
            continue;

        for (uint32 j = 0; j < sCreatureFamilyStore.GetNumRows(); ++j)
        {
            CreatureFamilyEntry const* creatureFamily = sCreatureFamilyStore.LookupEntry(j);
            if (!creatureFamily)
                continue;

            for (auto&& skillId : creatureFamily->skillLine)
            {
                if (!skillId)
                    continue;

                if (skillLine->SkillLine != skillId)
                    continue;

                if (skillLine->AcquireMethod != ABILITY_LEARNED_ON_GET_RACE_OR_CLASS_SKILL)
                    continue;

                SpellInfo const* spell = GetSpellInfo(skillLine->Spell);
                if (!spell) // not exist or triggered or talent
                    continue;

                if (skillLine->SkillLine == SKILL_PET_GHOUL && spell->AttributesEx4 & SPELL_ATTR4_UNK15) // It is spell triggered from main ability and it hasn't to display in action bar.
                    continue;

                PetLevelupSpellSet& spellSet = mPetSpellMap[creatureFamily->ID];
                if (spellSet.empty())
                    ++family_count;

                spellSet.insert(PetLevelupSpellSet::value_type(spell->SpellLevel, spell->Id));
                ++count;
            }
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u pet levelup and default spells for %u families in %u ms", count, family_count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellAreas()
{
    uint32 oldMSTime = getMSTime();

    mSpellAreaMap.clear();                                  // need for reload case
    mSpellAreaForQuestMap.clear();
    mSpellAreaForQuestEndMap.clear();
    mSpellAreaForAuraMap.clear();

    //                                                  0     1         2              3               4                 5          6          7       8         9
    QueryResult result = WorldDatabase.Query("SELECT spell, area, quest_start, quest_start_status, quest_end_status, quest_end, aura_spell, racemask, gender, autocast FROM spell_area");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell area requirements. DB table `spell_area` is empty.");

        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell = fields[0].GetUInt32();
        SpellArea spellArea;
        spellArea.spellId             = spell;
        spellArea.areaId              = fields[1].GetUInt32();
        spellArea.questStart          = fields[2].GetUInt32();
        spellArea.questStartStatus    = fields[3].GetUInt32();
        spellArea.questEndStatus      = fields[4].GetUInt32();
        spellArea.questEnd            = fields[5].GetUInt32();
        spellArea.auraSpell           = fields[6].GetInt32();
        spellArea.raceMask            = fields[7].GetUInt32();
        spellArea.gender              = Gender(fields[8].GetUInt8());
        spellArea.autocast            = fields[9].GetBool();

        if (SpellInfo const* spellInfo = GetSpellInfo(spell))
        {
            if (spellArea.autocast)
                const_cast<SpellInfo*>(spellInfo)->Attributes |= SPELL_ATTR0_CANT_CANCEL;
        }
        else
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` does not exist", spell);
            continue;
        }

        {
            bool ok = true;
            SpellAreaMapBounds sa_bounds = GetSpellAreaMapBounds(spellArea.spellId);
            for (SpellAreaMap::const_iterator itr = sa_bounds.first; itr != sa_bounds.second; ++itr)
            {
                if (spellArea.spellId != itr->second.spellId)
                    continue;
                if (spellArea.areaId != itr->second.areaId)
                    continue;
                if (spellArea.questStart != itr->second.questStart)
                    continue;
                if (spellArea.auraSpell != itr->second.auraSpell)
                    continue;
                if ((spellArea.raceMask & itr->second.raceMask) == 0)
                    continue;
                if (spellArea.gender != itr->second.gender)
                    continue;

                // duplicate by requirements
                ok = false;
                break;
            }

            if (!ok)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` already listed with similar requirements.", spell);
                continue;
            }
        }

        if (spellArea.areaId && !sAreaTableStore.LookupEntry(spellArea.areaId))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have wrong area (%u) requirement", spell, spellArea.areaId);
            continue;
        }

        if (spellArea.questStart && !sObjectMgr->GetQuestTemplate(spellArea.questStart))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have wrong start quest (%u) requirement", spell, spellArea.questStart);
            continue;
        }

        if (spellArea.questEnd)
        {
            if (!sObjectMgr->GetQuestTemplate(spellArea.questEnd))
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have wrong end quest (%u) requirement", spell, spellArea.questEnd);
                continue;
            }
        }

        if (spellArea.auraSpell)
        {
            SpellInfo const* spellInfo = GetSpellInfo(abs(spellArea.auraSpell));
            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have wrong aura spell (%u) requirement", spell, abs(spellArea.auraSpell));
                continue;
            }

            if (uint32(abs(spellArea.auraSpell)) == spellArea.spellId)
            {
                TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have aura spell (%u) requirement for itself", spell, abs(spellArea.auraSpell));
                continue;
            }

            // not allow autocast chains by auraSpell field (but allow use as alternative if not present)
            if (spellArea.autocast && spellArea.auraSpell > 0)
            {
                bool chain = false;
                SpellAreaForAuraMapBounds saBound = GetSpellAreaForAuraMapBounds(spellArea.spellId);
                for (SpellAreaForAuraMap::const_iterator itr = saBound.first; itr != saBound.second; ++itr)
                {
                    if (itr->second->autocast && itr->second->auraSpell > 0)
                    {
                        chain = true;
                        break;
                    }
                }

                if (chain)
                {
                    TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have aura spell (%u) requirement that itself autocast from aura", spell, spellArea.auraSpell);
                    continue;
                }

                SpellAreaMapBounds saBound2 = GetSpellAreaMapBounds(spellArea.auraSpell);
                for (SpellAreaMap::const_iterator itr2 = saBound2.first; itr2 != saBound2.second; ++itr2)
                {
                    if (itr2->second.autocast && itr2->second.auraSpell > 0)
                    {
                        chain = true;
                        break;
                    }
                }

                if (chain)
                {
                    TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have aura spell (%u) requirement that itself autocast from aura", spell, spellArea.auraSpell);
                    continue;
                }
            }
        }

        if (spellArea.raceMask && (spellArea.raceMask & RACEMASK_ALL_PLAYABLE) == 0)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have wrong race mask (%u) requirement", spell, spellArea.raceMask);
            continue;
        }

        if (spellArea.gender != GENDER_NONE && spellArea.gender != GENDER_FEMALE && spellArea.gender != GENDER_MALE)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u listed in `spell_area` have wrong gender (%u) requirement", spell, spellArea.gender);
            continue;
        }

        SpellArea const* sa = &mSpellAreaMap.insert(SpellAreaMap::value_type(spell, spellArea))->second;

        // for search by current zone/subzone at zone/subzone change
        if (spellArea.areaId)
            mSpellAreaForAreaMap.insert(SpellAreaForAreaMap::value_type(spellArea.areaId, sa));

        // for search at quest start/reward
        if (spellArea.questStart)
            mSpellAreaForQuestMap.insert(SpellAreaForQuestMap::value_type(spellArea.questStart, sa));

        // for search at quest start/reward
        if (spellArea.questEnd)
            mSpellAreaForQuestEndMap.insert(SpellAreaForQuestMap::value_type(spellArea.questEnd, sa));

        // for search at aura apply
        if (spellArea.auraSpell)
            mSpellAreaForAuraMap.insert(SpellAreaForAuraMap::value_type(abs(spellArea.auraSpell), sa));

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u spell area requirements in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

// Temporary structure to hold spell effect entries for faster loading
struct SpellEffectArray
{
    SpellEffectArray()
    {
        for (int i = 0; i < MAX_SPELL_EFFECTS; i++)
            effects[i] = NULL;
    }

    SpellEffectEntry const* effects[MAX_SPELL_EFFECTS];
};

void SpellMgr::LoadSpellInfoStore()
{
    uint32 oldMSTime = getMSTime();

    std::map<uint32, std::set<Difficulty>> spellDifficultyList;
    std::multimap<uint32, SpellTargetRestrictionsEntry const*> spellTargetRestrictions;

    for (uint32 i = 0; i < sSpellEffectStore.GetNumRows(); ++i)
        if (SpellEffectEntry const* spellEffect = sSpellEffectStore.LookupEntry(i))
            spellDifficultyList[spellEffect->EffectSpellId].insert(Difficulty(spellEffect->EffectDifficulty));

    for (uint32 i = 0; i < sSpellTargetRestrictionsStore.GetNumRows(); ++i)
    {
        if (SpellTargetRestrictionsEntry const* targetRestrictions = sSpellTargetRestrictionsStore.LookupEntry(i))
        {
            spellDifficultyList[targetRestrictions->SpellId].insert(Difficulty(targetRestrictions->Difficulty));
            spellTargetRestrictions.emplace(targetRestrictions->SpellId, targetRestrictions);
        }
    }

    UnloadSpellInfoStore();
    for (uint32 i = 0; i < MAX_DIFFICULTY; ++i)
        mSpellInfoMap[i].resize(sSpellStore.GetNumRows());

    auto getTargetRestrictionsId = [&](SpellEntry const* spell, Difficulty difficulty)
    {
        uint32 targetRestrictions = 0;
        auto bound = spellTargetRestrictions.equal_range(spell->Id);
        for (auto itr = bound.first; itr != bound.second; ++itr)
            if (difficulty == itr->second->Difficulty)
                targetRestrictions = itr->second->Id;

        if (!targetRestrictions && difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            for (auto itr = bound.first; itr != bound.second; ++itr)
                if (itr->second->Difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
                    targetRestrictions = itr->second->Id;

        }
        return targetRestrictions ? targetRestrictions : spell->SpellTargetRestrictionsId;
    };

    for (uint32 i = 0; i < sSpellStore.GetNumRows(); ++i)
        if (SpellEntry const* spellEntry = sSpellStore.LookupEntry(i))
            for (auto&& difficulty : spellDifficultyList[i])
                mSpellInfoMap[difficulty][i] = new SpellInfo(spellEntry, difficulty, getTargetRestrictionsId(spellEntry, difficulty));

    for (uint32 i = 0; i < sTalentStore.GetNumRows(); i++)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        for (auto&& map : mSpellInfoMap)
        {
            if (SpellInfo* spellInfo = map[talentInfo->SpellID])
                spellInfo->TalentId = talentInfo->SpellID;
            if (SpellInfo* spellInfo = map[talentInfo->ReplacesSpell])
                spellInfo->OverrideSpellList.push_back(talentInfo->SpellID);
        }
    }

    auto AddSpellOverride = [=](uint32 spell, uint32 with)
    {
        for (auto&& map : mSpellInfoMap)
            if (SpellInfo* spellInfo = map[spell])
                spellInfo->OverrideSpellList.push_back(with);
    };
    AddSpellOverride(688,    112866); // Summon Imp -> Summon Fel Imp
    AddSpellOverride(697,    112867); // Summon Voidwalker -> Summon Voidlord
    AddSpellOverride(691,    112869); // Summon Felhunter -> Summon Observer
    AddSpellOverride(712,    112868); // Summon Succubus -> Summon Shivarra
    AddSpellOverride(30146,  112870); // Summon Felguard -> Summon Wrathguard
    AddSpellOverride(18540,  112927); // Summon Doomguard -> Summon Terrorguard
    AddSpellOverride(1122,   112921); // Summon Infernal -> Summon Abyssal
    AddSpellOverride(26573,  116467); // Consecration -> Consecration to location
    AddSpellOverride(105174, 123194); // Handl of Gul'dan -> Handl of Gul'dan to location
    AddSpellOverride(105361, 31801 ); // Seal of Command -> Seal of Truth
    AddSpellOverride(686,    112092); // Shadow Bolt -> Glyphed Shadow Bolt (3 hits)

    TC_LOG_INFO("server.loading", ">> Loaded SpellInfo store in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::UnloadSpellInfoStore()
{
    for (auto&& spells : mSpellInfoMap)
    {
        for (auto&& it : spells)
            delete it;
        spells.clear();
    }
}

void SpellMgr::UnloadSpellInfoImplicitTargetConditionLists()
{
    for (auto&& spells : mSpellInfoMap)
    {
        for (auto&& it : spells)
            if (it)
                it->_UnloadImplicitTargetConditionLists();
    }
}

void SpellMgr::LoadSpellInfoCustomAttributes()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT spell, attribute + 0 FROM spell_custom_attribute");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 spellId = fields[0].GetUInt32();
            if (!GetSpellInfo(spellId))
            {
                TC_LOG_ERROR("sql.sql", "SpellMgr::LoadSpellInfoCustomAttributesFromDB - Non-existing spell %u", spellId);
                continue;
            }
            uint32 bit = fields[1].GetUInt32();
            if (!bit || bit > 32) // MySQL store enums as 1, 2, 3...
            {
                TC_LOG_ERROR("sql.sql", "SpellMgr::LoadSpellInfoCustomAttributesFromDB - Invalid attribute (%u) for spell %u", bit, spellId);
                continue;
            }
            for (auto&& store : mSpellInfoMap)
                if (auto spellInfo = store[spellId])
                    spellInfo->AttributesCu |= 1 << (bit - 1);
        } while (result->NextRow());
    }

    SpellInfo* spellInfo = nullptr;
    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        for (uint32 difficulty = 0; difficulty < MAX_DIFFICULTY; ++difficulty)
        {
            spellInfo = mSpellInfoMap[difficulty][i];
            if (!spellInfo)
                continue;

            for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
            {
                switch (spellInfo->Effects[j].ApplyAuraName)
                {
                    case SPELL_AURA_MOD_POSSESS:
                    case SPELL_AURA_MOD_CONFUSE:
                    case SPELL_AURA_MOD_CHARM:
                    case SPELL_AURA_AOE_CHARM:
                    case SPELL_AURA_MOD_FEAR:
                    case SPELL_AURA_MOD_FEAR_2:
                    case SPELL_AURA_MOD_STUN:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_AURA_CC;
                        break;
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                    case SPELL_AURA_PERIODIC_LEECH:
                    case SPELL_AURA_PERIODIC_MANA_LEECH:
                    case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
                    case SPELL_AURA_PERIODIC_ENERGIZE:
                    case SPELL_AURA_OBS_MOD_HEALTH:
                    case SPELL_AURA_OBS_MOD_POWER:
                    case SPELL_AURA_POWER_BURN:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_NO_INITIAL_THREAT;
                        break;
                    case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS:
                    case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2:
                        spellInfo->AttributesEx8 |= SPELL_ATTR8_AURA_SEND_AMOUNT;
                        break;
                }

                switch (spellInfo->Effects[j].Effect)
                {
                    case SPELL_EFFECT_POWER_DRAIN:
                    case SPELL_EFFECT_POWER_BURN:
                    case SPELL_EFFECT_HEAL_MAX_HEALTH:
                    case SPELL_EFFECT_HEALTH_LEECH:
                    case SPELL_EFFECT_HEAL_PCT:
                    case SPELL_EFFECT_ENERGIZE_PCT:
                    case SPELL_EFFECT_ENERGIZE:
                    case SPELL_EFFECT_HEAL_MECHANICAL:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_NO_INITIAL_THREAT;
                        break;
                    case SPELL_EFFECT_CHARGE:
                    case SPELL_EFFECT_CHARGE_DEST:
                    case SPELL_EFFECT_JUMP:
                    case SPELL_EFFECT_JUMP_DEST:
                    case SPELL_EFFECT_LEAP_BACK:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_CHARGE;
                        break;
                    case SPELL_EFFECT_PICKPOCKET:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_PICKPOCKET;
                        break;
                    case SPELL_EFFECT_ENCHANT_ITEM:
                    case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
                    case SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC:
                    case SPELL_EFFECT_ENCHANT_HELD_ITEM:
                    {
                        // only enchanting profession enchantments procs can stack
                        if (IsPartOfSkillLine(SKILL_ENCHANTING, i))
                        {
                            uint32 enchantId = spellInfo->Effects[j].MiscValue;
                            SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
                            if (!enchant)
                                break;

                            for (uint8 s = 0; s < MAX_ITEM_ENCHANTMENT_EFFECTS; ++s)
                            {
                                if (enchant->Type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                                    continue;

                                SpellInfo* procInfo = (SpellInfo*)GetSpellInfo(enchant->SpellID[s]);
                                if (!procInfo)
                                    continue;

                                // if proced directly from enchantment, not via proc aura
                                // NOTE: Enchant Weapon - Blade Ward also has proc aura spell and is proced directly
                                // however its not expected to stack so this check is good
                                if (procInfo->HasAura(SPELL_AURA_PROC_TRIGGER_SPELL))
                                    continue;

                                procInfo->AttributesCu |= SPELL_ATTR0_CU_ENCHANT_PROC;
                            }
                        }
                        break;
                    }
                }
            }

            if (spellInfo->Id >= 203751) // I dunno why 203751 is the first in spell_dbc
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SERVERSIDE_ONLY;

            if (spellInfo->HasEffect(SPELL_EFFECT_KNOCK_BACK) && !(spellInfo->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS))
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_ALWAYS_CHECK_LOS;

            switch (spellInfo->Id)
            {
                case 12540: // Gouge
                case 13579:
                case 24698:
                case 28456:
                case 29425:
                case 34940:
                case 36862:
                case 38863:
                case 52743: // Head Smack
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_REQ_TARGET_FACING_CASTER;
                    break;
                case 53: // Backstab
                case 5221: // Shred
                case 6785: // Ravage
                case 21987: // Lash of Pain
                case 58563: // Assassinate Restless Lookout
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_REQ_CASTER_BEHIND_TARGET;
                    break;
                case 26029: // Dark Glare
                case 37433: // Spout
                case 43140: // Flame Breath
                case 43215: // Flame Breath
                case 70461: // Coldflame Trap
                case 72133: // Pain and Suffering
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CONE_LINE;
                    break;
                case 24340:  // Meteor
                case 26558:  // Meteor
                case 28884:  // Meteor
                case 36837:  // Meteor
                case 38903:  // Meteor
                case 41276:  // Meteor
                case 57467:  // Meteor
                case 26789:  // Shard of the Fallen Star
                case 31436:  // Malevolent Cleave
                case 35181:  // Dive Bomb
                case 40810:  // Saber Lash
                case 43267:  // Saber Lash
                case 43268:  // Saber Lash
                case 42384:  // Brutal Swipe
                case 45150:  // Meteor Slash
                case 64688:  // Sonic Screech
                case 72373:  // Shared Suffering
                case 70492:  // Ooze Eruption
                case 81280:  // Blood Burst
                case 98474:  // Flame Scythe
                case 145944: // Sha Smash
                case 106375: // Unstable Twilight
                case 107439: // Twilight Barrage
                case 106401: // Twilight Onslaught
                case 103414: // Stomp
                case 135703: // Static shock tr ef dmg
                case 116364: // Arcane Velocity
                case 116018: // Epicenter
                case 116157: // Lightning fists
                case 116374: // Lightning fists (trigger dmg)
                case 136324: // Rising Anger
                case 136220: // Acidic explosion tr ef dmg
                    // ONLY SPELLS WITH SPELLFAMILY_GENERIC and EFFECT_SCHOOL_DAMAGE
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                    break;
                case 18500: // Wing Buffet
                case 33086: // Wild Bite
                case 49749: // Piercing Blow
                case 52890: // Penetrating Strike
                case 53454: // Impale
                case 59446: // Impale
                case 62383: // Shatter
                case 64777: // Machine Gun
                case 65239: // Machine Gun
                case 65919: // Impale
                case 74439: // Machine Gun
                case 63278: // Mark of the Faceless (General Vezax)
                case 62544: // Thrust (Argent Tournament)
                case 64588: // Thrust (Argent Tournament)
                case 66479: // Thrust (Argent Tournament)
                case 68505: // Thrust (Argent Tournament)
                case 62709: // Counterattack! (Argent Tournament)
                case 62626: // Break-Shield (Argent Tournament, Player)
                case 64590: // Break-Shield (Argent Tournament, Player)
                case 64342: // Break-Shield (Argent Tournament, NPC)
                case 64686: // Break-Shield (Argent Tournament, NPC)
                case 65147: // Break-Shield (Argent Tournament, NPC)
                case 68504: // Break-Shield (Argent Tournament, NPC)
                case 62874: // Charge (Argent Tournament, Player)
                case 68498: // Charge (Argent Tournament, Player)
                case 64591: // Charge (Argent Tournament, Player)
                case 63003: // Charge (Argent Tournament, NPC)
                case 63010: // Charge (Argent Tournament, NPC)
                case 68321: // Charge (Argent Tournament, NPC)
                case 72255: // Mark of the Fallen Champion (Deathbringer Saurfang)
                case 118000:// Dragon Roar
                case 122786:// Broken Leg (Garalon)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 64422: // Sonic Screech (Auriaya)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 72293: // Mark of the Fallen Champion (Deathbringer Saurfang)
                case 140023:// Ring of Peace (target aura)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 44413: // Incanter's Absorption - Considering the minimal duration and problems with aura stacking we skip saving this aura
                case 40075: // Fel Flak Fire
                case 55849: // Power Spark
                case 67483: // Eclipse Marker - When a druid logins, he doesnt have either eclipse power, nor the marker auras, nor the eclipse buffs. Dont save them.
                case 67484: // Eclipse Marker
                case 48517: // Eclipse(Solar)
                case 48518: // Eclipse(Lunar)
                case 65266: // Gear Scaling
                case 65635:
                case 65636:
                case 66666:
                case 66667:
                case 66668:
                case 108446:// Soul Link
                case 110310:// Dampening
                case 115556:// Master Demonologist
                case 113901:// Demonic Gateway
                case 116014:// Rune of Power
                case 137639:// Storm, Earth, and Fire
                case 138080:
                case 138081:
                case 138083:
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB;
                    break;
                case 3600:   // Earthbind Totem
                case 5857:   // Hellfire Effect
                case 8187:   // Magma Totem
                case 8349:   // Fire Nova
                case 50622:  // Bladestorm
                case 95738:  // Bladestorm Off-Hand
                case 44949:  // Whirlwind Off-Hand.
                case 32375:  // Mass Dispel (Allies)
                case 32592:  // Mass Dispel (Enemies)
                case 39897:  // Mass Dispel (Enemies) (Mechanic)
                case 33395:  // Freeze
                case 49821:  // Mind Sear
                case 48721:  // Blood Boil
                case 44461:  // Living Bomb
                case 42208:  // Blizzard
                case 42223:  // Rain of Fire
                case 42231:  // Hurricane
                case 42669:  // Smash (Ingvar the Plunderer)
                case 50286:  // Starfall
                case 59706:  // Smash (Ingvar the Plunderer) (Heroic)
                case 42723:  // Dark Smash (Ingvar the Plunderer)
                case 59709:  // Dark Smash (Ingvar the Plunderer) (Heroic)
                case 106998: // Astral Storm
                case 110745: // Divine Star
                case 122128: // Divine Star
                case 120361: // Barrage
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_ALWAYS_CHECK_LOS;
                    break;
                case 131538:  // Stampede
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_PERSISTENT;
                    break;
                case 71904: // Chaos Bane (Item - Shadowmourne Legendary)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_ALWAYS_CHECK_LOS;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                    break;
                case 120032: // Dancing Steel
                case 142530: // Bloody Dancing Steel
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_ENCHANT_PROC;
                    break;
                case 110914: // Dark Bargain
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_RESILIENCE;
                    break;
                case 81022:  // Stampede
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_PERSISTENT;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB;
                    break;
                default:
                    break;
            }

            if ((spellInfo->HasEffect(SPELL_EFFECT_APPLY_AREA_AURA_OWNER) && spellInfo->Id != 119904 ) || //  Override Command Demon while Grimoire of Sacrifice casted by warlock
                spellInfo->HasEffect(SPELL_EFFECT_APPLY_AREA_AURA_PET) ||
                spellInfo->HasAura(SPELL_AURA_SET_VEHICLE_ID) ||
                spellInfo->HasAura(SPELL_AURA_AREA_TIRGGER) ||
                spellInfo->IsChanneled())
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB;

            // I don't sure what this can't be used as exploit by the logout during bossfight
            if (spellInfo->HasAuraInterruptFlag(AURA_INTERRUPT_FLAG_CHANGE_MAP))
                if (spellInfo->GetDuration() == -1 || spellInfo->GetDuration() > 2 * MINUTE * IN_MILLISECONDS)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB;

            if (spellInfo->AttributesEx4 & SPELL_ATTR4_FIXED_DAMAGE)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_RESILIENCE;

            if (spellInfo->SpellVisual[0] || spellInfo->SpellVisual[1] || spellInfo->IsChanneled() || spellInfo->Speed > 0.0f ||
                spellInfo->CategoryRecoveryTime || spellInfo->RecoveryTime ||
                spellInfo->HasAttribute(SPELL_ATTR8_AURA_SEND_AMOUNT) || spellInfo->HasAttribute(SPELL_ATTR4_TRIGGERED))
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_ALWAYS_SEND_TO_CLIENT;

            for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                if (!spellInfo->_IsPositiveEffect(i, false))
                    spellInfo->NegativeEffectMask |= (1 << i);

            if (spellInfo->NegativeEffectMask)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_HAS_NEGATIVE_EFFECT;

            switch (spellInfo->SpellFamilyName)
            {
                case SPELLFAMILY_WARRIOR:
                    // Shout
                    if (spellInfo->SpellFamilyFlags[0] & 0x20000 || spellInfo->SpellFamilyFlags[1] & 0x20)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_AURA_CC;
                    break;
                case SPELLFAMILY_DRUID:
                    // Roar
                    if (spellInfo->SpellFamilyFlags[0] & 0x8)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_AURA_CC;
                    break;
                default:
                    break;
            }

            spellInfo->_InitializeExplicitTargetMask();
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded SpellInfo custom attributes in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellInfoCorrections()
{
    uint32 oldMSTime = getMSTime();

    SpellInfo* spellInfo = NULL;
    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        for (auto&& spellInfoMap : mSpellInfoMap)
        {
            spellInfo = spellInfoMap[i];
            if (!spellInfo)
                continue;

            if (spellInfo->ActiveIconID == 2158)  // flight
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;

            switch (spellInfo->Id)
            {
                case 42730: // Woe Strike
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 42739;
                    break;
                case 59735: // Woe Strike
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 59736;
                    break;
                case 131197: // Summon Zorlok
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_DEST_DB);
                    break;
                case 127253: // Lightning Sweep [Geodata Issue, pull throw all map]
                    spellInfo->Effects[EFFECT_1].MiscValue = 25;
                    break;
                case 65580: // Snared Net
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 65601: // Dragging a Razormane
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB;
                    break;
                case 52611: // Summon Skeletons
                case 52612: // Summon Skeletons
                    spellInfo->Effects[EFFECT_0].MiscValueB = 64;
                    break;
                case 40244: // Simon Game Visual
                case 40245: // Simon Game Visual
                case 40246: // Simon Game Visual
                case 40247: // Simon Game Visual
                case 42835: // Spout, remove damage effect, only anim is needed
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 30657: // Quake
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 30571;
                    break;
                case 30541: // Blaze (needs conditions entry)
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_TARGET_ENEMY);
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo();
                    break;
                case 63665: // Charge (Argent Tournament emote on riders)
                case 31298: // Sleep (needs target selection script)
                case 51904: // Summon Ghouls On Scarlet Crusade (this should use conditions table, script for this spell needs to be fixed)
                case 68933: // Wrath of Air Totem rank 2 (Aura)
                case 29200: // Purify Helboar Meat
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_CASTER);
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo();
                    break;
                case 31344: // Howl of Azgalor
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS); // 100yards instead of 50000?!
                    break;
                case 42818: // Headless Horseman - Wisp Flight Port
                case 42821: // Headless Horseman - Wisp Flight Missile
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); // 100 yards
                    break;
                case 36350: // They Must Burn Bomb Aura (self)
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 36325; // They Must Burn Bomb Drop (DND)
                    break;
                case 49838: // Stop Time
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 89909: // Water Spout
                    spellInfo->Attributes &= ~SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                    break;
                case 61407: // Energize Cores
                case 62136: // Energize Cores
                case 54069: // Energize Cores
                case 56251: // Energize Cores
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENTRY);
                    break;
                case 50785: // Energize Cores
                case 59372: // Energize Cores
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENEMY);
                    break;
                case 63320: // Glyph of Life Tap
                case 53232: // Rapid Killing (Rank 2)
                    // Entries were not updated after spell effect change, we have to do that manually :/
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                    break;
                case 31347: // Doom
                case 39365: // Thundering Storm
                case 41071: // Raise Dead (HACK)
                case 42442: // Vengeance Landing Cannonfire
                case 42611: // Shoot
                case 44978: // Wild Magic
                case 45001: // Wild Magic
                case 45002: // Wild Magic
                case 45004: // Wild Magic
                case 45006: // Wild Magic
                case 45010: // Wild Magic
                case 45761: // Shoot Gun
                case 45863: // Cosmetic - Incinerate to Random Target
                case 48246: // Ball of Flame
                case 41635: // Prayer of Mending
                case 44869: // Spectral Blast
                case 45027: // Revitalize
                case 45976: // Muru Portal Channel
                case 52124: // Sky Darkener Assault
                case 53096: // Quetz'lun's Judgment
                case 52479: // Gift of the Harvester
                case 61588: // Blazing Harpoon
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 36384: // Skartax Purple Beam
                    spellInfo->MaxAffectedTargets = 2;
                    break;
                case 28542: // Life Drain - Sapphiron
                case 29213: // Curse of the Plaguebringer - Noth
                case 37790: // Spread Shot
                case 39992: // Needle Spine
                case 41303: // Soul Drain
                case 41376: // Spite
                case 45248: // Shadow Blades
                case 46771: // Flame Sear
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 42005: // Bloodboil
                case 38296: // Spitfire Totem
                case 37676: // Insidious Whisper
                case 46008: // Negative Energy
                case 45641: // Fire Bloom
                case 55665: // Life Drain - Sapphiron (H)
                case 28796: // Poison Bolt Volly - Faerlina
                    spellInfo->MaxAffectedTargets = 5;
                    break;
                case 54835: // Curse of the Plaguebringer - Noth (H)
                case 54098: // Poison Bolt Volly - Faerlina (H)
                    spellInfo->MaxAffectedTargets = 10;
                    break;
                case 50312: // Unholy Frenzy
                    spellInfo->MaxAffectedTargets = 15;
                    break;
                case 33711: // Murmur's Touch
                case 38794: // Murmur's Touch
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 33760;
                    break;
                case 17941: // Shadow Trance
                case 22008: // Netherwind Focus
                case 23920: // Spell Reflection
                case 34936: // Backlash
                case 140076:// Backlash
                case 51124: // Killing Machine
                case 57934: // Tricks of the Trade
                case 64823: // Item - Druid T8 Balance 4P Bonus
                case 69369: // Predator Swiftness
                case 74434: // Soulburn
                case 81292: // Glyph of Mind Spike
                case 93400: // Shooting Stars
                case 104242:// Unending Breath
                case 114250:// Selfless Healer
                case 124430:// Divine Insight
                case 122510:// Ultimatum
                case 132158:// Nature's Swiftness
                    spellInfo->ProcCharges = 1;
                    break;
                case 28200: // Ascendance (Talisman of Ascendance trinket)
                    spellInfo->ProcCharges = 6;
                    break;
                case 51852: // The Eye of Acherus (no spawn in phase 2 in db)
                    spellInfo->Effects[EFFECT_0].MiscValue |= 1;
                    break;
                case 51912: // Crafty's Ultra-Advanced Proto-Typical Shortening Blaster
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 3000;
                    break;
                // Master Shapeshifter: missing stance data for forms other than bear - bear version has correct data
                // To prevent aura staying on target after talent unlearned
                case 24900: // Heart of the Wild - Cat Effect
                    spellInfo->Stances = 1 << (FORM_CAT - 1);
                    break;
                case 24899: // Heart of the Wild - Bear Effect
                    spellInfo->Stances = 1 << (FORM_BEAR - 1);
                    break;
                case 51470: // Elemental Oath (Rank 2)
                    spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_ADD_FLAT_MODIFIER;
                    spellInfo->Effects[EFFECT_1].MiscValue = SPELLMOD_EFFECT2;
                    spellInfo->Effects[EFFECT_1].SpellClassMask = flag128(0x00000000, 0x00004000, 0x00000000, 0x00000000);
                    break;
                case 64904: // Hymn of Hope
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT;
                    break;
                case 30421: // Nether Portal - Perseverence
                    spellInfo->Effects[EFFECT_2].BasePoints += 30000;
                    break;
                case 41913: // Parasitic Shadowfiend Passive
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY; // proc debuff, and summon infinite fiends
                    break;
                case 27892: // To Anchor 1
                case 27928: // To Anchor 1
                case 27935: // To Anchor 1
                case 27915: // Anchor to Skulls
                case 27931: // Anchor to Skulls
                case 27937: // Anchor to Skulls
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    break;
                // target allys instead of enemies, target A is src_caster, spells with effect like that have ally target
                // this is the only known exception, probably just wrong data
                case 29214: // Wrath of the Plaguebringer
                case 54836: // Wrath of the Plaguebringer
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ALLY);
                    spellInfo->Effects[EFFECT_1].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ALLY);
                    break;
                case 8145: // Tremor Totem (instant pulse)
                case 6474: // Earthbind Totem (instant pulse)
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                    break;
                case 70728: // Exploit Weakness (needs target selection script)
                case 70840: // Devious Minds (needs target selection script)
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_CASTER);
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_PET);
                    break;
                case 70893: // Culling The Herd (needs target selection script)
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_CASTER);
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_MASTER);
                    break;
                case 45602: // Ride Carpet
                    spellInfo->Effects[EFFECT_0].BasePoints = 0; // force seat 0, vehicle doesn't have the required seat flags for "no seat specified (-1)"
                    break;
                case 61719: // Easter Lay Noblegarden Egg Aura - Interrupt flags copied from aura which this aura is linked with
                    spellInfo->AuraInterruptFlags[0] = AURA_INTERRUPT_FLAG_HITBYSPELL | AURA_INTERRUPT_FLAG_TAKE_DAMAGE;
                    break;
                case 71838: // Drain Life - Bryntroll Normal
                case 71839: // Drain Life - Bryntroll Heroic
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 34471:  // The Beast Within
                case 108201: // Desecrated Ground
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_CONFUSED | SPELL_ATTR5_USABLE_WHILE_FEARED | SPELL_ATTR5_USABLE_WHILE_STUNNED;
                    break;
                case 117902: // Gather Steam Aura - Quest 30792
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 2000;
                    break;
                case 128407: // Poisoned Barb
                    spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                    break;
                // ULDUAR SPELLS
                //
                case 64014: // Expedition Base Camp Teleport
                case 64032: // Formation Grounds Teleport
                case 64028: // Colossal Forge Teleport
                case 64031: // Scrapyard Teleport
                case 64030: // Antechamber Teleport
                case 64029: // Shattered Walkway Teleport
                case 64024: // Conservatory Teleport
                case 64025: // Halls of Invention Teleport
                case 64027: // Descent into Madness Teleport
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_DEST_DB);
                    break;
                case 62374: // Pursued (Flame Leviathan)
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS); // 50000yd
                    break;
                case 62383: // Shatter (Ignis)
                    spellInfo->SpellVisual[0] = 12639;
                    break;
                case 62716: // Growth of Nature (Freya)
                case 65584: // Growth of Nature (Freya)
                case 64381: // Strength of the Pack (Auriaya)
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 63018: // Searing Light (XT-002)
                case 65121: // Searing Light (25m) (XT-002)
                case 63024: // Gravity Bomb (XT-002)
                case 64234: // Gravity Bomb (25m) (XT-002)
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 62834: // Boom (XT-002)
                            // This hack is here because we suspect our implementation of spell effect execution on targets
                            // is done in the wrong order. We suspect that EFFECT_0 needs to be applied on all targets,
                            // then EFFECT_1, etc - instead of applying each effect on target1, then target2, etc.
                            // The above situation causes the visual for this spell to be bugged, so we remove the instakill
                            // effect and implement a script hack for that.
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 64321: // Potent Pheromones (Freya)
                            // spell should dispel area aura, but doesn't have the attribute
                            // may be db data bug, or blizz may keep reapplying area auras every update with checking immunity
                            // that will be clear if we get more spells with problem like this
                    spellInfo->AttributesEx |= SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY;
                    break;
                case 61791: // Ride Vehicle (Yogg-Saron)
                            // TODO: remove this when basepoints of all Ride Vehicle auras are calculated correctly
                    spellInfo->Effects[EFFECT_0].BasePoints = 1;
                    break;
                case 62301: // Cosmic Smash (Algalon the Observer)
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 64598: // Cosmic Smash (Algalon the Observer)
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 62293: // Cosmic Smash (Algalon the Observer)
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_DEST_CASTER);
                    break;
                case 62311: // Cosmic Smash (Algalon the Observer)
                case 64596: // Cosmic Smash (Algalon the Observer)
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); // 100yd
                    break;
                case 62907: // Freya's Ward
                case 62947:
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(0);
                    break;
                case 63631: // Shock Blast (Mimiron)
                    spellInfo->InterruptFlags = 0;
                    break;
                case 63355: // Crunch Armor 10 (Kologarn)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(18); // 20 seconds
                    spellInfo->StackAmount = 4;
                    break;
                case 62038: // Bitting Cold (Hodir)
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 5000;
                    break;
                case 62997: // Plasma Blast
                case 64529: // Plasma Blast
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_IGNORE_RESISTANCES;
                    break;
                case 64389: // Sentinel Blast
                    spellInfo->ChannelInterruptFlags[0] &= ~AURA_INTERRUPT_FLAG_TAKE_DAMAGE;
                    break;
                case 61975: // Rune of Power
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); // 10 secs
                    break;
                case 62634: // Mortar
                case 62306: // Hurl Boulder
                case 62490: // Hurl Pyrite Barrel
                case 62358: // Fire Cannon
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(174); // 1000yds
                    break;
                case 62507: // Touch of Dominion
                case 62565: // Touch of Dominion
                    spellInfo->AttributesEx |= SPELL_ATTR1_NO_THREAT;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 65509: // Constellation Phase Effect
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // ENDOF ULDUAR SPELLS
                //
                // TRIAL OF THE CRUSADER SPELLS
                //
                case 66258: // Infernal Eruption (10N)
                    // increase duration from 15 to 18 seconds because caster is already
                    // unsummoned when spell missile hits the ground so nothing happen in result
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(85);
                    break;
                case 67870:
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                // ENDOF TRIAL OF THE CRUSADER SPELLS
                //
                // HALLS OF REFLECTIONS SPELLS
                //
                case 69818: // Summon Raging Ghoul
                    spellInfo->AuraInterruptFlags = { 0, 0 };
                    break;
                // ENDOF HALLS OF REFLECTIONS SPELLS
                //
                // ICECROWN CITADEL SPELLS
                case 70781: // Light's Hammer Teleport
                case 70856: // Oratory of the Damned Teleport
                case 70857: // Rampart of Skulls Teleport
                case 70858: // Deathbringer's Rise Teleport
                case 70859: // Upper Spire Teleport
                case 70860: // Frozen Throne Teleport
                case 70861: // Sindragosa's Lair Teleport
                    spellInfo->Effects[0].TargetA = TARGET_DEST_DB;
                    break;
                case 69055: // Bone Slice (Lord Marrowgar)
                case 70814: // Bone Slice (Lord Marrowgar)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_8_YARDS);    // 8yd
                    break;
                case 69075: // Bone Storm (Lord Marrowgar)
                case 70834: // Bone Storm (Lord Marrowgar)
                case 70835: // Bone Storm (Lord Marrowgar)
                case 70836: // Bone Storm (Lord Marrowgar)
                            //    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                            //    spellInfo->EffectRadiusIndex[0] = EFFECT_RADIUS_100_YARDS;   // 100yd
                            //    break;
                case 71160: // Plague Stench (Stinky)
                case 71161: // Plague Stench (Stinky)
                case 71123: // Decimate (Stinky & Precious)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);   // 100yd
                    break;
                case 72378: // Blood Nova (Deathbringer Saurfang)
                case 73058: // Blood Nova (Deathbringer Saurfang)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);
                    break;
                case 72769: // Scent of Blood (Deathbringer Saurfang)
                            //spellInfo->Effect[0] = 0;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break; // no break
                case 72771: // Scent of Blood (Deathbringer Saurfang)
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    break;
                case 72723: // Resistant Skin (Deathbringer Saurfang adds)
                            // this spell initially granted Shadow damage immunity, however it was removed but the data was left in client
                    spellInfo->Effects[2].Effect = 0;
                    break;
                case 70460: // Coldflame Jets (Traps after Saurfang)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1);   // 10 seconds
                    break;
                case 70530: // Volatile Ooze Beam Protection (Professor Putricide)
                    spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA; // for an unknown reason this was SPELL_EFFECT_APPLY_AREA_AURA_RAID
                    break;
                case 71159: // Awaken Plagued Zombies
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                    break;
                case 57055: // Mini (Amanitar)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);
                    break;
                    // This is here because cooldown on creature procs is not implemented
                case 71604: // Mutated Strength (Professor Putricide)
                case 72673: // Mutated Strength (Professor Putricide)
                case 72674: // Mutated Strength (Professor Putricide)
                case 72675: // Mutated Strength (Professor Putricide)
                    spellInfo->Effects[1].Effect = 0;
                    break;
                case 72454: // Mutated Plague (Professor Putricide)
                case 72464: // Mutated Plague (Professor Putricide)
                case 72506: // Mutated Plague (Professor Putricide)
                case 72507: // Mutated Plague (Professor Putricide)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    break;
                case 70911: // Unbound Plague (Professor Putricide)
                case 72854: // Unbound Plague (Professor Putricide)
                case 72855: // Unbound Plague (Professor Putricide)
                case 72856: // Unbound Plague (Professor Putricide)
                    spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                    break;
                    // Mutated Transformation - crit does not seems to inflict additional damage, but it occurs wrong in combat-log.
                case 70405:
                case 72508:
                case 72509:
                case 72510:
                    // SPELL_MALLEABLE_GOO missiles - Spell-id's are picked from spellwork_cs
                    // They can crit with base crit chance since they are not casted directly by Professor Prutricide, thus not ignoring it
                case 70853:
                case 72458:
                case 72873:
                case 72874:
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 71518: // Unholy Infusion Quest Credit (Professor Putricide)
                case 72934: // Blood Infusion Quest Credit (Blood-Queen Lana'thel)
                case 72289: // Frost Infusion Quest Credit (Sindragosa)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // another missing radius
                    break;
                case 71708: // Empowered Flare (Blood Prince Council)
                case 72785: // Empowered Flare (Blood Prince Council)
                case 72786: // Empowered Flare (Blood Prince Council)
                case 72787: // Empowered Flare (Blood Prince Council)
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 71266: // Swarming Shadows
                case 72890: // Swarming Shadows
                    spellInfo->AreaGroupId = 0; // originally, these require area 4522, which is... outside of Icecrown Citadel
                    break;
                case 70588: // Suppression
                case 70602: // Corruption
                case 48278: // Paralyze
                    spellInfo->AttributesEx |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 70715: // Column of Frost (visual marker)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(32); // 6 seconds (missing)
                    break;
                case 71085: // Mana Void (periodic aura)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(9); // 30 seconds (missing)
                    break;
                case 72015: // Frostbolt Volley (only heroic)
                case 72016: // Frostbolt Volley (only heroic)
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_40_YARDS);
                    break;
                case 70936: // Summon Suppressor
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[0].TargetB = 0;
                    break;
                case 72706: // Achievement Check (Valithria Dreamwalker)
                case 71357: // Order Whelp
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    break;
                case 70598: // Sindragosa's Fury
                    spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                    break;
                case 69846: // Frost Bomb (Sindragosa - effect summon NPC off, summoned over script)
                    spellInfo->Speed = 0.0f;    // This spell's summon happens instantly
                    break;
                case 66545: // Summon Memory (TrialOfTheChampion)
                    spellInfo->Effects[0].TargetB = TARGET_UNIT_CASTER;
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 71614: // Ice Lock
                    spellInfo->Mechanic = MECHANIC_STUN;
                    break;
                case 72762: // Defile
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(559); // 53 seconds
                    break;
                case 72743: // Defile
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(22); // 45 seconds
                    break;
                case 72754: // Defile
                case 73708: // Defile
                case 73709: // Defile
                case 73710: // Defile
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    break;
                case 69030: // Val'kyr Target Search
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    break;
                case 69198: // Raging Spirit Visual
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13);             // 50000yd
                    break;
                case 73654: // Harvest Souls
                case 74295: // Harvest Souls
                case 74296: // Harvest Souls
                case 74297: // Harvest Souls
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    break;
                case 72546: // Harvest Soul
                case 72597: // Harvest Soul
                case 72608: // Harvest Soul
                case 73655: // Harvest Soul
                    spellInfo->AttributesEx7 |= SPELL_ATTR7_ZONE_TELEPORT;
                    break;
                case 73540: // Summon Shadow Trap
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(23);          // 90 seconds
                    break;
                case 73530: // Shadow Trap (visual)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28);          // 5 seconds
                    break;
                case 56303: // mark
                case 1130:
                case 14323:
                case 14324:
                case 14325:
                case 53338:
                    //spellInfo->Effect[0] = 0;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 72262: // Quake (icc)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);   // 200yd
                    break;
                case 68981: // Remorseless Winter
                case 74270: // Remorseless Winter
                case 74271: // Remorseless Winter
                case 74272: // Remorseless Winter
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);   // 200yd
                    break;
                case 46014: // Knocked Up (Nitro Boosts explode aggro exploit)
                            //spellInfo->EffectMiscValue[0] = 50; //knock
                    spellInfo->Effects[0].BasePoints = 370;
                    break;
                    //case 70664: //Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
                    //    //spellInfo->EffectMiscValue[0] = 50; //knock
                    //    spellInfo->EffectBasePoints[0] = 20;
                    //    break;
                    //case 2836: // Knocked Up (Nitro Boosts explode aggro exploit)
                    //    //spellInfo->EffectMiscValue[0] = 50; //knock
                    //    spellInfo->EffectBasePoints[0] = 200;
                    //    break;
                    //case 63623: // Avoidance shadow fiend
                    //case 62137: // Avoidance summoned ghoul
                    //case 32233: // Avoidance warlock pet
                    //case 65220: // Avoidance hunter pet
                    //    spellInfo->EffectApplyAuraName[0] = SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE;
                    //    break;
                case 73529: // Shadow Trap (icc) - use full range
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);   // 10yd
                    break;
                case 74282: // Shadow Trap (searcher)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_3_YARDS);   // 3yd
                    break;
                case 72595: // Restore Soul
                case 73650: // Restore Soul
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    break;
                case 74086: // Destroy Soul
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REDIRECTED | SPELL_ATTR1_CANT_BE_REFLECTED;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_UNTARGETABLE;
                    break;
                case 74302: // Summon Spirit Bomb
                case 74342: // Summon Spirit Bomb
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 74341: // Summon Spirit Bomb
                case 74343: // Summon Spirit Bomb
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_200_YARDS);   // 200yd
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 72350: // Fury of Frostmourne
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    break;
                case 75127: // Kill Frostmourne Players
                case 72351: // Fury of Frostmourne
                case 72431: // Jump (removes Fury of Frostmourne debuff)
                case 72429: // Mass Resurrection
                case 73159: // Play Movie
                case 73582: // Trigger Vile Spirit (Inside, Heroic)
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    break;
                case 72376: // Raise Dead
                    spellInfo->MaxAffectedTargets = 3;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);   // 50000yd
                    break;
                case 71809: // Jump
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); // 100yds
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);   // 10yd
                    break;
                case 72405: // Broken Frostmourne
                    spellInfo->AttributesEx |= SPELL_ATTR1_NO_THREAT;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_20_YARDS);   // 20yd
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_20_YARDS);   // 20yd
                    break;
                case 71289: // Dominate Mind
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_MOD_HEALING_DONE_PERCENT; // Fix heal done bonus under control
                    break;
                case 71412: // Green Ooze Summon
                case 71415: // Orange Ooze Summon
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 71189: // Dreamwalker's Rage
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    break;
                case 69099: // Ice Pulse
                case 73776: // Ice Pulse
                case 73777: // Ice Pulse
                case 73778: // Ice Pulse
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 72272: // Vile Gas
                case 72273: // Vile Gas
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_1].Mechanic = MECHANIC_NONE;
                    spellInfo->Effects[EFFECT_2].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                    break;
                case 52207: // Devour Humanoid
                case 52632: // Devour Humanoid
                case 69503: // Devour Humanoid
                case 71163: // Devour Humanoid
                    spellInfo->ChannelInterruptFlags = { 0, 0 }; // Workaround to prevent interrupting it during jump. Ideally such trigger spells should only execute when motion ends
                    break;
                case 70485: // Leap to a Random Location
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); // 100yds
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13); // 10yds, data from 4.3.4
                    break;
                case 71169: // Shadow's Fate
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 72864: // Death Plague
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(14); // 8yds, data from 4.3.4
                    break;
                case 72867: // Death Plague
                case 72879: // Death Plague
                case 72929: // Death Plague
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12); // 100yds, data from 4.3.4
                    break;
                case 72255: // Mark of the Fallen Champion
                case 72444: // Mark of the Fallen Champion
                case 72445: // Mark of the Fallen Champion
                case 72446: // Mark of the Fallen Champion
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                    break;
                case 72257: // Remove Marks of the Fallen Champion
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                    break;
                case 69146: // Coldflame
                case 70823: // Coldflame
                case 70824: // Coldflame
                case 70825: // Coldflame
                    spellInfo->AttributesEx4 &= ~SPELL_ATTR4_IGNORE_RESISTANCES;
                    break;
                // ENDOF ICECROWN CITADEL SPELLS
                //
                // RUBY SANCTUM SPELLS
                //
                case 74769: // Twilight Cutter
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS); // 100yd
                    break;
                case 75509: // Twilight Mending
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 75888: // Awaken Flames
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                // ENDOF RUBY SANCTUM SPELLS
                //
                // EYE OF ETERNITY SPELLS
                // All spells below work even without these changes. The LOS attribute is due to problem
                // from collision between maps & gos with active destroyed state.
                case 57473: // Arcane Storm bonus explicit visual spell
                case 57431: // Summon Static Field
                case 56091: // Flame Spike (Wyrmrest Skytalon)
                case 56092: // Engulf in Flames (Wyrmrest Skytalon)
                case 57090: // Revivify (Wyrmrest Skytalon)
                case 57143: // Life Burst (Wyrmrest Skytalon)
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                // This would never crit on retail and it has attribute for SPELL_ATTR3_NO_DONE_BONUS because is handled from player,
                // until someone figures how to make scions not critting without hack and without making them main casters this should stay here.
                case 63934: // Arcane Barrage (casted by players and NONMELEEDAMAGELOG with caster Scion of Eternity (original caster)).
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                // ENDOF EYE OF ETERNITY SPELLS
                //
                case 40055: // Introspection
                case 40165: // Introspection
                case 40166: // Introspection
                case 40167: // Introspection
                    spellInfo->Attributes |= SPELL_ATTR0_NEGATIVE_1;
                    break;
                // Halls Of Origination spells
                // Temple Guardian Anhuur
                case 76606: // Disable Beacon Beams L
                case 76608: // Disable Beacon Beams R
                    // Little hack, Increase the radius so it can hit the Cave In Stalkers in the platform.
                    spellInfo->Effects[EFFECT_0].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_45_YARDS);
                    break;
                case 75323: // Reverberating Hymn
                    // Aura is refreshed at 3 seconds, and the tick should happen at the fourth.
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER;
                    break;
                case 24314: // Threatening Gaze
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_CAST | AURA_INTERRUPT_FLAG_MOVE | AURA_INTERRUPT_FLAG_JUMP;
                    break;
                case 5420: // Tree of Life (Passive)
                    spellInfo->Stances = 1 << (FORM_TREE - 1);
                    break;
                case 49376: // Feral Charge (Cat Form)
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                    break;
                case 96942:  // Gaze of Occu'thar
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    break;
                case 75463: // Cosmetic Burrow
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39);
                    break;
                case 77106: // Omega Stance Summon Bunny
                    spellInfo->Effects[EFFECT_0].TargetA = 0;
                    break;
                // ISLE OF CONQUEST SPELLS
                //
                case 66551: // Teleport
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 50000yd
                    break;
                // ENDOF ISLE OF CONQUEST SPELLS
                //
                // OCULUS
                //
                case 49345: // Call Emerald Drake
                case 49461: // Call Amber Drake
                case 49462: // Call Ruby Drake
                    spellInfo->Speed = 0; // Effects are executed on launch on retail, hacking it this way is easier than changing effect handlers for only a bunch of spells
                    break;
                // ENDOF OCULUS SPELLS
                //
                // UTGARDE KEEP
                //
                case 48642: // Launch Harpoon
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                // ENDOF UTGARDE KEEP SPELLS
                //
                // World Bosses
                //
                // Akma'hat
                case 93578: // Sands of Time
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 119489: // Unleashed Wrath
                    spellInfo->MaxAffectedTargets = 10;
                    break;
                case 136340: // Storm Cloud
                case 136339: // Lightning Tether
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                //
                // THRONE OF THE TIDES SPELLS
                //
                // Lady Nazjar
                case 90479: // Waterspout dmg
                    spellInfo->Effects[0].Effect = 0;
                    break;
                case 75690: // Waterspout knock
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    break;
                case 75700: // Geyser Errupt
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 94046: // Geyser Errupt knock
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 75813: // Chain Lightning
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 75993: // Lightning Surge dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 80564: // Fungal Spores dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Commander Ulthok
                case 76085: // Dark Fissure dmg
                case 91375:
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                    break;
                case 76047:
                    spellInfo->Effects[0].TargetB = 0;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                    break;
                // Erunak Stonespeaker
                case 84945: // Earth Shards dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Mindbender Ghursha
                case 76341: // Unrelenting Agony dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Ozumat
                case 83463: // Entangling Grasp
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 83915: // Brain Spike
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(23);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(23);
                    break;
                case 83561: // Blight of Ozumat
                case 83608:
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER;
                    break;
                // ENDOF THRONE OF THE TIDES SPELLS
                //
                // DEADMINES SPELLS
                //
                // Admiral Ripsnarl
                case 88840: // Vanish
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(125);
                    break;
                case 95647: // Ripsnarl Achievement Aura
                    spellInfo->AttributesEx3 = SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 89268: // Throw Food Targeting
                case 89740:
                case 90561:
                case 90562:
                case 90582:
                case 90583:
                case 90563:
                case 90584:
                case 90564:
                case 90585:
                case 90565:
                case 90586:
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // Vanessa Vancleef
                case 92620: // Backslash targeting
                    spellInfo->MaxAffectedTargets =1;
                    break;
                // ENDOF DEADMINES
                //
                // BLACKROCK CAVERNS SPELLS
                //
                // Rom'ogg Bonecrusher
                case 75272: // Quake
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(23);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(23);
                    break;
                case 82189: // Chains of Woe
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82192: // Chains of Woe Root Aura
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(9);
                    break;
                // Corla
                case 75677: // Nether Beam
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                // Ascendant Lord Obsidius
                case 76186: // Thinderclap
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 76164: // Shadow of Obsidius
                    spellInfo->Effects[0].BasePoints = 10000000;
                    break;
                // ENDOF BLACKROCK CAVERNS SPELLS
                //
                // THE VORTEX PINNACLE SPELLS
                //
                // Grand Vizier Ertan
                case 86292: // Cyclone Shield dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    break;
                // Asaad
                case 87551: // Supremacy of the Storm dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 87622: // Chain Lightning
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 87618: // Static Cling
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 86911: // Unstable Grounding Field
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->InterruptFlags = 0;
                    break;
                case 87517: // Grounding Field Visual Beams
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                // Altairus
                case 88276: // Call of Wind Dummy
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 88282: // Upwind of Altairus
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    break;
                // Trash
                case 87854: // Arcane Barrage
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 88073: // Starfall
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 87761: // Rally
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10);
                    break;
                case 87933: // Air Nova
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 85159: // Howling Gale dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 85084: // Howling Gale
                    spellInfo->Effects[0].TriggerSpell = 85159;
                    break;
                case 87765: // Lightning Lash Dummy
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 87768: // Lightning Nova
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 88170: // Cloudburst
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 89522: // Catch Fall
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                    break;
                case 89526: // Catch Fall
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                    break;
                // ENDOF THE VORTEX PINNACLE SPELLS
                //
                // BARADIN HOLD SPELLS
                //
                // Occu'thar
                case 96913: // Searing Shadow
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 96872: // Focused Fire
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 96883: // Focused Fire dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(17);
                    break;
                case 96920: // Eye of Occu'thar
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 96931: // Eye of Occu'thar script
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[0].TargetB = 0;
                    break;
                case 96968: // Occu'thar's Destruction
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 96946: // Eye of Occu'thar dmg
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                    break;
                // Alizabal
                case 105065: // Seething Hate Dummy
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 105069: // Seething Hate dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                    break;
                case 106248: // Blade Dance Dummy
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 104994: // Blade Dance dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(17);
                    break;
                case 105726: // Blade Dance charge
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 105784: // Blade Dance aura 1
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                // ENDOF BARADIN HOLD SPELLS
                //
                // THE STONECORE SPELLS
                //
                // Corborus
                case 82415: // Dumpening Wave
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 92122: // Crystal Shard dmg
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Slabhide
                case 80800: // Eruption
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 80801: // Eruption aura
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Ozruk
                case 78807: // Shatter
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    break;
                case 92426: // Paralyze
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // High Priestess Azil
                case 79251: // Gratity Well aura
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 79249: // Gravity Well aura dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_0].BasePoints = 3000;
                    break;
                case 79050: // Arcane Shield dmg
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // ENDOF STONECORE SPELLS
                //
                // HALLS OF ORIGINATION
                //
                // Trash
                case 77262: // Raging Inferno dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 77273: // Lava Eruption
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Temple Guardian Anhuur
                case 75515: // Searing Flame
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 74938: // Shield of Light
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                // Earthrager Ptah
                case 75540: // Flame Bolt dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 83650: // Raging Smash
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 75547: // Quicksand AOE
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 75339: // Earthspike dmg
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 94974: // Earthspike
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                // Anraphet
                case 76094: // Alpha Beams aoe
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(30);
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 76956: // Alpha Beams dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 75609: // Crumbling Ruin
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Isiset
                case 74374: // Arcane Barrage
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // Setesh
                case 76146: // Shadow Bolt Volley
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Ammunae
                case 75702: // Noxious Spore
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 75790: // Rampant Growth
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Rajh
                case 73872: // Sun Strike
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 73874: // Sun Strike dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetA = 0;
                    break;
                case 76355: // Blessing of the Sun aura
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 74108: // Solar Winds dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    break;
                case 89133: // Solar Fire dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    break;
                // ENDOF HALLS OF ORIGINATION SPELLS
                //
                // LOST CITY OF THE TOL'VIR
                //
                case 83644: // Mystic Trap N
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 83112: // Land Mine Player Search Effect
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    spellInfo->AttributesEx3 = SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 91263: // Detonate Traps
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    break;
                case 84799: // Paralytic Blow Dart N
                    spellInfo->Attributes = 0;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].TargetB = 0;
                    spellInfo->Targets = TARGET_FLAG_UNIT;
                    break;
                case 83131: // Summon Shockwave Target N
                case 83132: // Summon Shockwave Target S
                case 83133: // Summon Shockwave Target W
                case 83134: // Summon Shockwave Target E
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Targets = TARGET_FLAG_DEST_LOCATION;
                    break;
                case 83454: // Shockwave N
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 81690: // Scent of Blood N
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 82263: // Merged Souls
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].TargetB = 0;
                    spellInfo->Targets = TARGET_FLAG_UNIT;
                    break;
                case 82430: // Repentance
                    spellInfo->Effects[EFFECT_0].MiscValue = 250;
                    break;
                case 91196: // Blaze of the Heavens
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    break;
                case 88814: // Hallowed Ground N
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(32);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(32);
                    break;
                case 81942: // Heaven's Fury N
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 82622: // Plague of Ages N
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->AttributesEx = SPELL_ATTR1_CANT_TARGET_SELF;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS | SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 82637: // Plague of Ages N
                    spellInfo->ExcludeTargetAuraSpell = 82622;
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Attributes &= ~SPELL_ATTR0_NEGATIVE_1;
                    spellInfo->AttributesEx = SPELL_ATTR1_CANT_TARGET_SELF;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS | SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 82640: // Plague of Ages N
                    spellInfo->ExcludeTargetAuraSpell = 82622;
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Attributes &= ~SPELL_ATTR0_NEGATIVE_1;
                    spellInfo->AttributesEx = SPELL_ATTR1_CANT_TARGET_SELF;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS | SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 82139: // Repentance
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_DEATH_PERSISTENT;
                    break;
                case 82425: // Wail of Darkness N
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(32);
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 84546: // Static Shock
                case 84555: // Static Shock
                case 84556: // Static Shock
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Targets = TARGET_FLAG_DEST_LOCATION;
                    break;
                case 84956: // Call of Sky
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Targets = TARGET_FLAG_DEST_LOCATION;
                    break;
                case 83455: // Chain Lightning N
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 83790: // Cloud Burst
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST_RANDOM;
                    spellInfo->Targets = TARGET_FLAG_DEST_LOCATION;
                    break;
                case 83051: // Cloud Burst
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    break;
                case 83566: // Wailing Winds
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].TargetB = 0;
                    break;
                case 83094: // Wailing Winds N
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_TRIGGERED;
                    break;
                case 83151: // Absorb Storms
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_CASTER;
                    break;
                case 84126: // Call Scorpid
                case 82791: // Call Crocolisk
                    spellInfo->Attributes = 0;
                    break;
                case 82224: // Soul Fragment
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 84589: // Deflecting Winds
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                // ENDOF LOST CITY OF THE TOL'VIR
                // SHADOWFANG KEEP SPELLS
                // Baron Ashbury
                case 93720: // Wracking Pain
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 93810: // Calamity dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 93423: // Asphyxiate
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 93468: // Stay of Execution
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 93706: // Stay of Execution t
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Baron Silverlaine
                case 93956: // Cursed Veil
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    break;
                // Commander Springvale
                case 93722: // Shield of Perfidious dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                    break;
                // Lord Valden
                case 93697: // Conjure Poisonous Mixture
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    break;
                case 93505: // Conjure Frost Mixture
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    break;
                case 93617: // Toxic Coagulant dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 93689: // Toxic Catalyst dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 93527: // Ice Shards
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_DONT_DISPLAY_IN_AURA_BAR;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                // ENDOF SHADOWFANG SPELLS
                //
                // GRIM BATOL SPELLS
                //
                // Trash
                case 76517: // Eruption Fire
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 76782: // Rock Smash dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 76786: // Fissure dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 76101: // Lightning Strike dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 76392: // Arcane Slash
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 76370: // Warped Twilight
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 76620: // Azure Blast
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 76404: // Crimson Charge
                    spellInfo->EquippedItemClass = -1;
                    spellInfo->EquippedItemSubClassMask = -1;
                    break;
                case 76409: // Crimson Shockwave
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 76327: // Blaze
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 76693: // Empowering Twilight dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 76627: // Mortal Strike
                    spellInfo->EquippedItemClass = -1;
                    spellInfo->EquippedItemSubClassMask = -1;
                    break;
                case 76603: // Earth Spike
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 76411: // Meat Grinder
                    spellInfo->EquippedItemClass = -1;
                    spellInfo->EquippedItemSubClassMask = -1;
                    break;
                case 76413: // Meat Grinder dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                    break;
                case 76578: // Chain Lightning
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // General Umbriss
                case 74675: // Blitz dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 74837: // Modgud Malady
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 90170: // Modgud Malice aura
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[2].Effect = 0;
                    break;
                // Forgemaster Throngus
                case 74976: // Disorenting Roar
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 90754: // Lava Patch dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 74986: // Cave In dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 74984: // Mighty Stomp
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Drahga Shadowburner
                case 75238: // Supernova
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 75245: // Burning Shadowbolt
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 75317: // Seeping Twilight dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                // ENDOF GRIM BATOL SPELLS
                //
                // THRONE OF THE FOUR WINDS SPELLS
                //
                case 85269: // Catch Fall
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                    break;
                // Anshal
                case 85425: // Nurture summon trigger
                case 85428: // Nurture dummy
                case 85429: // Nurture summon
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                    break;
                case 86282: // Toxic Spores dmg
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                // Nezir
                case 86367: // Sleet Storm dmg
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                    break;
                case 84644: // Sleet Storm
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 85483: // Wind Blast
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CONE_ENEMY_104;
                    break;
                // Al'Akir
                case 88214: // Lightning Strike
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 89668: // Lightning Rod
                case 89628: // Lightning Clouds
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 88427: // Electrocute
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13);
                    break;
                // ENDOF THRONE OF THE FOUR WINDS SPELLS
                //
                // BLACKWING DESCENT SPELLS
                //
                // Trash
                case 79604: // Thunderclap
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 80035: // Vengeful Rage
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 79589: // Constricting Chains
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(27);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(27);
                    break;
                case 80336: // Frost Burn
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    break;
                case 80878: // Bestowal of Angerforge
                case 80871: // Bestowal of Thaurissan
                case 80875: // Bestowal of Ironstar
                case 80872: // Bestowal of Burningeye
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 80638: // Stormbolt
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 80646: // Chain Lightning
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 91849: // Grip of Death
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 92048: // Shadow Infusion
                    spellInfo->TargetAuraSpell = 0;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 92051: // Shadow Conductor
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 92023: // Encasing Shadows
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->TargetAuraSpell = 0;
                    break;
                case 92153: // Blazing Inferno missile
                    spellInfo->Speed = 6;
                    break;
                case 92154: // Blazing Inferno dmg
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 92173: // Shadow Breath
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Magmaw
                case 89773: // Mangle
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 78359: // Magma Split 1
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 78068: // Magma Split 2
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                // Omnotron Defence System
                case 78697: // Recharging blue
                case 78698: // Recharging orange
                case 78699: // Recharging purple
                case 78700: // Recharging green
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 79629: // Power Generator aoe
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                    break;
                case 91858: // Overcharged Power Generator aoe
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 91879: // Arcane Blowback
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 79912: // Static Shock dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 79035: // Inseneration Security Missure
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 79501: // Acquiring Target
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                    break;
                case 79504: // Flamethrower
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                    break;
                case 79617: // Backdraft
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 80092: // Poison Bomb
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 80164: // Chemical Cloud aoe a
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    break;
                case 80161: // Chemocal Cloud aoe b
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    break;
                case 80097: // Poison Pubble aoe
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                // Maloriak
                case 89798: // Master Adventurer Award
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 78194: // Magma Jets summon
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                    break;
                case 77569: // Release Aberrations
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_GAMEOBJECT_SRC_AREA;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 77991: // Release All
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_GAMEOBJECT_SRC_AREA;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 77699: // Flash Freeze dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 77763: // Biting Chill dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 77615: // Debilitating Slime
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 77948: // Debilitating Slime knock back
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(10);
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(10);
                    break;
                case 77908: // Arcane Storm dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(31);
                    break;
                case 78095: // Magma Jets dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 78225: // Acid Nova
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 77987: // Grown Catalyst
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 78208: // Absolute Zero
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 77715: // Shatter
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 77925: // Throw Red Bottle
                case 77928: // Throw Red Bottle
                case 77932: // Throw Blue Bottle
                case 77934: // Throw Blue Bottle
                case 77937: // Throw Green Bottle
                case 77938: // Throw Green Bottle
                case 92831: // Throw Black Bottle
                case 92837: // Throw Black Bottle
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_GAMEOBJECT_TARGET;
                    break;
                // Chimaeron
                case 82848: // Massacre
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 88872: // Feud
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 82881: // Break
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 91304: // Shadow Whip
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                // Atramedes
                case 77611: // Resonating Clash interrupt
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 77709: // Resonating Clash energize
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    spellInfo->Effects[EFFECT_2].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    break;
                case 77782: // Sonic Flames dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 77672: // Sinar Pulse target
                    spellInfo->MaxAffectedTargets = 3;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 77612: // Modulation
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 77675: // Sonar Pulse dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 77982: // Searing Flame dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 78115: // Sonar Fireball
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 92553: // Sonar Bomb dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 77966: // Searing Flame Missile
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(23);
                    break;
                case 78353: // Roaring Flame dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 78023: // Roaring Flame aura dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 78875: // Devastation
                    spellInfo->TargetAuraSpell = 0;
                    break;
                case 78868: // Devastation dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // Nefarian
                case 78684: //  Hail of Bones
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                    break;
                case 77826: // Shadowflame Breath
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CONE_ENTRY;
                    break;
                // ENDOF BLACKWING DESCENT SPELLS
                //
                // FIRELANDS SPELLS
                //
                // Trash
                case 101093:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 99692: // Terrifying Roar
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(33);
                    break;
                case 97552: // Lava Shower dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    break;
                case 99993: // Fiery Blood
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(18);
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 100273: // Shell Spin dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 100799: // Fire Torment dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 99530: // Flame Stomp
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 99555: // Summon Lava Jest
                    spellInfo->MaxAffectedTargets = 4;
                    break;
                // Shannox
                case 100002: // Hurl Spear dmg
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 100031: // Hurl Spear aoe
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 100495: // Magma Flare
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(27);
                    break;
                case 99945: // Face Rage jump
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 99947: // Face Rage aura
                    spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 100415: // Rage
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 99937: // Jagged Tear
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                // Bethtilac
                case 99052: // Smoldering Devastation
                    spellInfo->InterruptFlags = 0;
                    break;
                case 98471: // Burning Acid
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 97202: // Fiery Web Spin
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->ChannelInterruptFlags[0] |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 98997: // Clear Spiderweb Filament
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 99304: // Consume (drone)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 99476: // The Widow Kiss
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 99506;
                    break;
                case 99526: // Fixate
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 99999: // Fixate (broodling)
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                // Alysrazor
                case 102111: // Cataclysm
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_SCRIPT_EFFECT;
                    break;
                case 99208: // Ritual of the Flame dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99558: // Meteoric Impact
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99274: // Molten Meteor dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99844: // Blazing Claw dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_104;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CONE_ENEMY_104;
                    break;
                case 99794: // Fiery Vortex dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 100640: // Harsh Winds
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99816: // Fiery Tornado dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99747: // Blazing Buffet dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99461: // Blazing Power dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 99427: // Incindiary Cloud dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 98885: // Brashfire
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99919: // Ignition
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 99529: // Incindiary Cloud sum
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 99528: // Blazing Power sum
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 99605: // Firestorm
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_2].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 99606: // Firestorm dot
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 101410: // Molten Feather bar
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 100744: // Firestorm heroic
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 100745: // Firestorm heroic dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                // Lord Rhyolith
                case 98843: // Ride Vehicle
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 98010: // Volcanic Birth
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 98493: // Heated Volcano
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 98255: // Molten Armor
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = 0;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = 0;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].TargetB = 0;
                    break;
                case 97230: // Magma Flow area
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); // 10 seconds
                    break;
                case 99875: // Fuse
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].TargetB = 0;
                    break;
                case 98135: // Summon Fragment of Rhyolith
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 98553: // Summon Spark of Rhyolith
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 98136: // Summon Fragment of Rhyolith sum
                case 98552:
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                    break;
                case 98649: // Meltdown dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                // Baleroc
                case 99351: // Inferno Blade
                    spellInfo->Attributes &= ~SPELL_ATTR0_ON_NEXT_SWING;
                    break;
                case 99262: // Vital Spark
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 99515: // Final Countdown
                    spellInfo->MaxAffectedTargets = 2;
                    break;
                case 99254: // Torment visual
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 99256: // Torment
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                // Majordomo Staghelm
                case 98565: // Burning Orbs summon
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 98229: // Concentration
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                // Ragnaros
                case 98159: // Magma Trap aoe
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 98710: // Sulfuras Smash
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR5_DONT_TURN_DURING_CAST;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 98708: // Sulfuras Smash
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 101088: // Lavalogged
                    spellInfo->ExcludeTargetAuraSpell = 101008;
                    break;
                    spellInfo->ExcludeTargetAuraSpell = 101102;
                    break;
                case 99012: // Splitting Blow script
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 98497: // Molten Seed aoe 2
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 98498: // Molten Seed dmg
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    break;
                case 100158: // Molten Power
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 99125: // Blazing Heat dummy
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 99129: // Blazing Heat summon
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 99267: // Living Meteor aoe 1
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 100171: // World in Flame
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(566); // 8 seconds
                    break;
                case 98981: // Lava Bolt
                    spellInfo->MaxAffectedTargets = 4;
                    break;
                case 100476: // Breadth of Frost summon
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(23);
                    break;
                case 100567: // Breadth of Frost dmg
                    spellInfo->ExcludeTargetAuraSpell = 100567;
                    break;
                case 100679: // Dreadflame summon
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 100714: // Cloudburst missile
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 100644: // Entrapping Roots summon
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(23);
                    break;
                case 100653: // Entrapping Roots dmg
                    spellInfo->ExcludeTargetAuraSpell = 100653;
                    break;
                    spellInfo->ExcludeTargetAuraSpell = 101237;
                    break;
                case 100777: // Magma Geyser
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 100941: // Dreadflame dmg
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                // ENDOF FIRELANDS
                //
                // BASTION OF TWILIGHT SPELLS
                //
                // Trash
                case 85647: // Mind Sear dmg
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 85697:
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                    break;
                // Halfus Wyrmbreaker
                case 87683: // Dragon Vengeance
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 87609: // Atrophic Poison
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 83710: // Furious Roar
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 83719: // Fireball Barrage aoe
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 83855: // Scorching Breath dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 83734: // Fireball Barrage dmg0
                case 83721: // Fireball Barrage dmg1
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    break;
                // Theralion & Valiona
                case 86380: // Dazzling Destruction Summon Missile
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 86369: // Twilight Blast
                    spellInfo->Speed = 15;
                    break;
                case 86371: // Twilight Blast dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 86505: // Fabolous Flames dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(32);
                    break;
                case 86607: // Engulfing Magic
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 86825: // Blackout dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 86013: // Twilight Meteorite
                    spellInfo->Speed = 15;
                    break;
                case 86014: // Twilight Meteorite dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 88518: // Twilight Meteorite Mark
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 86199: // Twilight Flame dmg1
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[1].Effect = 0;
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 86228: // Twilight Flame dmg2
                    spellInfo->TargetAuraSpell = 0;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 86305: // Unstable Twilight dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 86202: // Twilight Shift Aura 1
                case 88436: // Twilight Shift Aura 2
                    spellInfo->Effects[0].MiscValue = 16;
                    break;
                case 86210: // Twilight Zone
                    spellInfo->Effects[0].Effect = 0;
                    break;
                case 86214: // Twilight Zone dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 93019: // Rift Blast
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 93055: // Shifting Reality
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                // Cho'gall
                case 91303: // Conversion
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 81571: // Unleashed Shadows dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 81538: // Blaze dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 81713: // Depravity
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    break;
                case 81761: // Spilled Blood of the Old God dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 82919: // Sprayed Corruption
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(9);
                    break;
                case 82299: // Fester Blood
                    spellInfo->Effects[0].TargetA = 1;
                    spellInfo->Effects[0].TargetB = 0;
                    spellInfo->Effects[0].Effect = 3;
                    spellInfo->Effects[1].Effect = 0;
                    break;
                case 82630: // Comsume Blood of the Old God
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82414: // Darkened Creations
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82433: // Darkened Creations sum
                    spellInfo->Effects[0].TargetA = 1;
                    break;
                case 82361: // Corruption of the Old God
                    spellInfo->Effects[1].Effect = 0;
                    break;
                case 82363: // Corruption of the Old God dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82151: // Shadow Bolt
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 91317: // Worshipping
                    spellInfo->Effects[2].Effect = 0;
                case 91331: // Twisted Devotion
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 93103: // Corrupted Blood aura
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->ExcludeTargetAuraSpell = 93103;
                    break;
                // Sinestra
                case 95822: // Twilight Flame dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 95855: // Call of Flames
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 89435: // Wrack aoe
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 89421: // Wrack dmg
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                    break;
                case 90028: // Unleash Essence
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 90045: // Indomitable dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 87655: // Twilight Infusion dummy
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 87231: // Fiery Barrier aura
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10);
                    break;
                case 88146: // Twilight Essence dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(22);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(22);
                    break;
                case 92852: // Twilight Slicer dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 95564: // Twilight Infusion missile
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 89299: // Twilight Spit dmg
                    spellInfo->AttributesEx3  |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 92958: // Twilight Pulse dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Council of Ascendents
                case 82746: // Glaciate
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 92548: // Glaciate 2
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82699: // Water Bomb
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82762: // Waterlogged
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                    break;
                case 77347: // Water Bomb Summon
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 82666: // Frost Imbued
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 82859: // Inferno Rush
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 82856: // Inferno Jump
                    spellInfo->Effects[1].Effect = 0;
                    break;
                case 82663: // Flame Imbued
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 83067: // Thundershock
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 83099: // Lightning Rod
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 83300: // Chain Lightning dummy
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 83282: // Chain Lightning
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 83070: // Lightning Blast
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[0].TargetB = 0;
                    spellInfo->Effects[1].Effect = 0;
                    break;
                case 83078: // Disperse 1
                    spellInfo->Effects[0].Effect = 0;
                    break;
                case 83565: // Quake
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 84915: // Liquid Ice dmg
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                case 84948: // Gravity Crush
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                case 84913: // Lava Seed
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(28);
                    break;
                // ENDOF BASTION OF TWILIGHT SPELLS
                //
                // ZUL'GURUB SPELLS
                //
                // Venoxis
                case 96489: // Toxic Explosion
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                    break;
                case 96560: // Word of Hethiss
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 96842: // Bloodvenom
                    spellInfo->MaxAffectedTargets = 3;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(41);
                    break;
                case 96638: // Bloodvenom dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(26);
                    break;
                case 96685: // Venomous Infusion dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    break;
                case 96475: // Toxic Link
                    spellInfo->MaxAffectedTargets = 2;
                    break;
                case 96476: // Toxic Link dummy
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->TargetAuraSpell = 96477;
                    break;
                // Zanzil
                case 96319:
                case 96316:
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 96342: // Pursuit
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 96914: // Zanzil Fire
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_STUN;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                // Kilnara
                case 96457: // Wall of Agony
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    break;
                case 96909: // Wail of Sorrow
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 96422: // Tears of Blood dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(32);
                    break;
                case 97355: // Gaping Wound jump
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 97357: // Gaping Wound dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 98238: // Rat Lure
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(10);
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                    break;
                // Jindo The Godbreaker
                case 96689: // Spirit World aura
                    spellInfo->Effects[EFFECT_1].MiscValue = 2;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 97022: // Hakkar's Chains
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->ChannelInterruptFlags = { 0, 0 };
                    break;
                case 97172: // Shadows of Hakkar
                case 97198: // Body Slam
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 97597: // Spirit Warrior's Gaze
                case 97158:
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 97152: // Summon Spirit target
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(33);
                    break;
                case 96964: // Sunder Rift
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(18);
                    break;
                case 96970: // Sunder Rift:
                    spellInfo->ProcChance = 100;
                    break;
                case 97320: // Sunder Rift
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    spellInfo->InterruptFlags = 0;
                    break;
                // ENDOF ZUL'GURUB SPELLS
                //
                // ZUL'AMAN SPELLS
                //
                // Akil'zon
                case 43648: // Electrical Storm
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_STUN;
                    break;
                // Janalai
               case 42471: // Hatch Eggs
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(10);
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 42630: // Fire Bomb dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(15);
                    break;
                case 43140: // Flame Breath
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    spellInfo->ChannelInterruptFlags[0] &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                // Halazzi
                case 97505: // Refreshing Stream heal
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                    break;
                // Hex Lord Malacrass
                case 44132: // Drain Power
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 43095: // Creeping Paralysis
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(12);
                    break;
                case 43121: // Feather Storm
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                    break;
                case 43550:
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                    break;
                // ENDOF ZUL'AMAN SPELLS
                //
                // END TIME SPELLS
                //
                // Trash
                case 109952: // Cadaver Toss
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 102066: // Flesh Rip
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                // Echo of Tyrande
                case 102242: // Tears of Elune script
                    spellInfo->MaxAffectedTargets = 2;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 102244: // Tears of Elune dmg
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_2_YARDS);
                    break;
                case 102542: // Achievement
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 102002: // Shrink
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 102193: // Moonbolt
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 102149: // Moonlance dmg
                    spellInfo->ExcludeTargetAuraSpell = 102149;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 102414: // Dark Moonlight
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 102173: // Stardust
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 102151: // Moonlance summon 1
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_5_YARDS);
                    break;
                // Echo of Sylvanas
                case 101397: // Death Grip aoe
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 101200: // Summon Ghoul
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break;
                case 100865: // Wracking Pain target aura
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break;
                case 101257: // Wracking Pain dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break;
                case 101547: // Blighted Arrows
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_DEST_AREA_ENTRY;
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13);
                    break;
                case 101401: // Blighted Arrows
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = 4;
                    break;
                // Echo of Baine
                case 101619: // Magma
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 101840: // Magma Blast
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 101601: // Throw Totem
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36);
                    break;
                case 101625: // Pulverize aoe
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 101627: // Pulverize triggered
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_TRIGGERED;
                    break;
                case 101815: // Pulverize gobject
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_TRIGGERED;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_20_YARDS);
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // Echo of Jaina
                case 101812: // Blink
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                // Murozond
                case 102364: // Blessing of Bronze Dragons
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                // ENDOF END TIME SPELLS
                //
                // HOUR OF TWILIGHT
                //
                case 102475: // thrall lava burst
                case 104130: // thrall lightning chain
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 103905: // Molten Fury
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_UNTARGETABLE;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                // ENDOF HOUR OF TWILIGHT
                //
                // WELL OF_ ETERNITY SPELLS
                //
                // Trash
                case 108474: // Crushing Leap
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 105720: // Demon Grip
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 102937: // Demon Grip Root
                    spellInfo->Effects[EFFECT_0].BasePoints = -99;
                    break;
                // Perotharn
                case 105496: // Track player
                    spellInfo->Speed = SPEED_CHARGE;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 107536: // Punishing Flame dmg
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                // Queen Azshara
                case 102478: // Ice Fling
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 103241: // Total Obedience
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_INSTAKILL;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = 0;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = 0;
                    break;
                // Mannoroth
                case 104648: // Nether Portal
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_3_YARDS);
                    break;
                case 104678: // Debilitating Flay
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 105073: // Wrath of Elune
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 105054: // Summon Felhound
                case 105058: // Summon Felguard
                case 105059: // Summon Devastator
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 105523: // Magistrike Arc
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                // ENDOF WELL OF ETERNITY SPELLS
                //
                // HOUR OF TWILIGHT SPELLS
                //
                // Arcurion
                case 102480: // Icy Boulder aoe
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // Archbishop Benedictus
                case 103600: // Purifying Light targeting
                case 103768: // Corrupting Twilight targeting
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 103648: // Purifying Blast
                case 103776: // Twilight Bolt
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 102915: // Shroud of Luminosity
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                // ENDOF HOUR OF TWILIGHT SPELLS
                //
                // DRAGON SOUL SPELLS
                //
                // Flood aoe
                case 107796:
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // Flood channel
                case 107791:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                // Boulder Smash aoe
                case 107596:
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 108220: // Deep Corruption
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 108183: // Twilight Submission
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 108096: // Twilight Portal Beam
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                // Morchok
                case 103821: // Earthen Vortex
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_STUN;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_60_YARDS);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_60_YARDS);
                    spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_60_YARDS);
                    break;
                case 103178: // Earths Vengeance dmg
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(618);
                    break;
                case 103534: // Danger
                case 103536: // Warning
                case 103541: // Safe
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 103687: // Crush Armor
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 103785: // Black Blood of the Earth dmg
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                // Yor'sahj The Unsleeping
                case 105420: // Color Combine 1
                case 105435: // Color Combine 2
                case 105436: // Color Combine 3
                case 105437: // Color Combine 4
                case 105439: // Color Combine 5
                case 105440: // Color Combine 6
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENTRY;
                    break;
                case 105636: // Corrupted Minions aura
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 105534: // Mana Void dummy
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 109390: // Deep Corruption dmg (trash)
                case 105173: // Deep Corruption dmg
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 108228: // Mana Diffusion
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                // Warlord Zon'ozz
                case 103527: // Void Diffusion dmg
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                    break;
                case 104031: // Void Diffusion debuff
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 109197: // Tentacle Toss aoe 1
                case 109237: // Tentacle Toss aoe 2
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 109240: // Tentacle Toss jump
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 104347: // Shadow Gaze
                    spellInfo->AttributesEx4 &= ~SPELL_ATTR4_IGNORE_RESISTANCES;
                    break;
                case 104378: // Black Blood of Go'rath
                    spellInfo->AttributesEx4 &= ~SPELL_ATTR4_IGNORE_RESISTANCES;
                    break;
                case 104377: // Black Blood of Go'rath
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    spellInfo->AttributesEx4 &= ~SPELL_ATTR4_IGNORE_RESISTANCES;
                    break;
                // Hagara the Stormbinder
                case 109393:
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 109305: // Frozen Grasp
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break;
                case 109307: // Frozen Grasp grip
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_JUMP_DEST;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 109315: // Icicle
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 105311: // Crystalline Tether
                case 105482:
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 105316: // Ice Lance dmg
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 105256: // Frozen Tempest
                    spellInfo->Mechanic = 0;
                    break;
                case 105367: // Lightning Conduit dummy 1
                    //spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 2 secs
                    //spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_10_YARDS);
                    //spellInfo->Effects[EFFECT_0].ChainTarget = 25;
                    //spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 1 secs
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                    spellInfo->Effects[EFFECT_1].ApplyAuraTickCount = 1000;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 105371: // Lightning Conduit dummy 2
                    //spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 1 secs
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 105369: // Lightning Conduit dmg
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 1 secs
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER;
                    break;
                case 107850: // Focused Assault dmg
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 50000
                    spellInfo->Effects[EFFECT_0].BasePoints = 100;
                    break;
                case 109325: // Frostflake
                    spellInfo->StackAmount = 9;
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 105289: // Shattered Ice
                    spellInfo->InterruptFlags = 0;
                    break;
                case 105409: // Water Shield
                    spellInfo->Effects[EFFECT_2].ApplyAuraTickCount = 8000;
                    break;
                case 109557: // Storm Pillars
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                // Ultraxion
                case 106374: // Unstable Twilight aoe 1
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 109176: // Unstable Twilight aoe 2
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 105900: // Essence of Dreams
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 106371: // Hour of Twilight
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 103327;
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 106174;
                    break;
                case 108491: // Charging Up (Kalecgos)
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_CAST;
                    break;
                // Warmaster Blackhorn
                case 107558: // Degeneration
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 107567: // Brutal Strike
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 107588: // Twilight Onslaught
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 108038: // Harpoon
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 5000 yards
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 107518: // Detonate
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 108046: // Shockwave
                case 110137:
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_MOVEMENT;
                    break;
                case 108041: // Artillery Barrage
                    spellInfo->Effects[EFFECT_0].BasePoints = 0;
                    break;
                // Spine of Deathwing
                case 105490: // Fiery Grip
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 105241: // Absorb Blood
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(26); // 4 yards
                    break;
                case 105845: // Nuclear Blast
                    spellInfo->InterruptFlags = 0;
                    break;
                case 105479: // Searing Plasma
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 105777: // Roll Control
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); // 3 seconds
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 105847: // Seal Armor Breach
                case 105848:
                    spellInfo->InterruptFlags = 0;
                    break;
                case 106199: // Blood Corruption: Death
                case 106200: // Blood Corruption: Earth
                    spellInfo->StackAmount = 1;
                    break;
                // Madness of Deathwing
                case 106663: // Carrying Winds
                case 106668:
                case 106670:
                case 106672:
                case 106674:
                case 106676:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->ExcludeCasterAuraSpell = 0;
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); // 1 second
                    break;
                case 106400: // Implale
                case 108649: // Corrupting Parasite
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    spellInfo->AttributesEx |= SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE;
                    break;
                case 108813: // Unstable Corruption
                    spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                    break;
                case 105445: // Blistering Heat dmg
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 106444: // Impale
                    spellInfo->AttributesEx |= SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 107029: // Impale Aspect
                case 106548:
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 105841: // Degenerative Bite dmg
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_STACK_FROM_DIFF_CASTERS;
                    break;
                case 105937: // Regenerative Blood heal
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 105799: // Time Zone missile
                case 107055: // Time Zone missile 2
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 106940: // Trigger Concentration
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 105830: // Time Zone aura (debuff)
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = 0;
                    break;
                case 106242: // Elementium Meteor target
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 109102: // Congealing Blood
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 106043: // Spellweave
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_CANT_MISS;
                    break;
                case 106791: // Shrapnel
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_IGNORE_RESISTANCES;
                    break;
                case 106794: // Shrapnel (visual)
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL | SPELL_ATTR0_NEGATIVE_1;
                    break;
                case 106814: // Base Visual
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                // Aspects auras
                // Increase duration up to 10 secs
                case 106028:
                case 106027:
                case 106457:
                case 106464:
                case 106029:
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); // 10 secs
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 106040: // Spellweaving
                    spellInfo->ProcChance = 10;
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); // 10 secs
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                // Increase amplitude
                case 105825:
                case 105823:
                case 106456:
                case 106463:
                case 106026:
                case 106039:
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 7000;
                    break;
                case 106527: // Cataclysm screen
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); // 10 secs
                    break;
                // Expose Weakness
                case 106588:
                case 106600:
                case 106613:
                case 106624:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                // ENDOF DRAGON SOUL SPELLS
                //
                // TIMELESS ISLE
                //
                case 144539: // Jadefire Wall Damage
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 144545: // Jadefire Bolt Selector
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 144640: // Leap
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                // ENDOF TIMELESS ISLE
                //
                // SCARLET HALLS
                //
                case 113364: // Book Burner
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->AttributesEx2 &= ~SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 113366: // Book Burner Cosmetic
                    spellInfo->MaxAffectedTargets = 1;
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    break;
                case 113641: // Dragon Breath
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNEL_TRACK_TARGET;
                    break;
                case 113653: // Dragon Breath eff
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 125492: // Shield Breaker [temp till vehicle on players doesn`t work...]
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 112955: // Light Blades
                    spellInfo->Effects[EFFECT_1].Mechanic = MECHANIC_NONE;
                    break;
                case 113719: // Cannon Blast
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 111894: // Dog food
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                // ENDOF SCARLET HALLS
                //
                // SCARLET MONASTERY
                case 114488: // Flying Kick
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_CANT_MISS;
                    break;
                case 114952: // Fuel Barrel
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 125844: // Burning Man
                case 125852:
                    spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                    break;
                case 115150: // Summon Vile Spirits
                    spellInfo->Effects[0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_TARGET_ANY);
                    break;
                case 114487: // Flying Kick Eff
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 113134: // Mass Resurrection
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                    break;
                // ENDOF SCARLET MONASTERY
                //
                // SCHOLOMANCE
                //
                case 114062: // Wondrous Rapidity
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 114061;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 122449: // Ice wall
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 120027: // Burn
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    break;
                case 114874: // Potion of Brutish Force
                    spellInfo->Attributes &= ~SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                    break;
                case 111585: // Dark Blaze
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 113312: // Explosive Pain
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENTRY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_DEST_AREA_ENTRY;
                    break;
                case 114048: // Whirl of Illusion
                    spellInfo->Effects[0].ApplyAuraTickCount = 1500;
                    break;
                case 113866:
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED | SPELL_ATTR1_CANT_BE_REDIRECTED;
                    break;
                case 113996: // Bone Armor
                    spellInfo->ProcChance = 0;
                    spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 111669: // Summon Books (textures fix)
                    spellInfo->AttributesEx2 &= ~SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                // ENDOF SCHOLOMANCE
                //
                // GATE OF THE SETTING SUN
                //
                case 118787: // Resin Burst
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                    break;
                case 111671: // Battering Headbutt
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 124656: // Great wall corner B
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_20_YARDS);
                    break;
                case 107078: // Viscous Fluid
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 1500;
                    break;
                case 107215: // Mution Explosion
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 113645: // Sabotage
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                // ENDOF GATE OF THE SETTING SUN
                //
                // SHADOPAN MONASTERY
                //
                case 106332: // Tornado Slam
                    spellInfo->Effects[1].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                    break;
                case 128245: // Flip Out
                    spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 106827: // Smoke blades
                    spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 112060: // Apparitions
                    spellInfo->Effects[0].TargetB = 0;
                    break;
                case 106992: // Fire Arrow
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 1500;
                    break;
                case 106871: //  Sha Spike (unk_138 don`t implemented)
                    spellInfo->Effects[0].TargetA = TARGET_DEST_TARGET_ENEMY;
                    spellInfo->Effects[1].TargetA = TARGET_DEST_TARGET_ENEMY;
                    break;
                case 106979: // Fire Arrow Hit
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                // ENDOF SHADOPAN MONASTERY
                //
                // SIEGE OF NIUZAO TEMPLE
                //
                case 119990: // Summon Sappling
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 120095: // Detonate Visual
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 120270: // Drain Barrel (Top)
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENTRY);
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(0);
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    spellInfo->Effects[EFFECT_1].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENTRY);
                    spellInfo->Effects[EFFECT_1].TargetB = SpellImplicitTargetInfo(0);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    spellInfo->ExplicitTargetMask = TARGET_FLAG_UNIT_MASK;
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(285); // 1s
                    break;
                case 124277: // Blade Rush Summon forcecast
                case 120561: // Bombard
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 121442: // Caustic Pitch
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_DEST_DEST);
                    spellInfo->ExplicitTargetMask = TARGET_FLAG_DEST_LOCATION;
                    break;
                case 121448: // Encased in Resin
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_STUN;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->ExplicitTargetMask = TARGET_FLAG_UNIT_MASK;
                    break;
                // ENDOF SIEGE OF NIUZAO TEMPLE
                //
                // MOGU'SHAN PALACE
                //
                case 119922: //Shockwave
                case 119929:
                case 119931:
                case 119932:
                case 119933:
                    spellInfo->Speed = 5.f;
                    break;
                case 119930:
                    spellInfo->Speed = 5.f;
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 119684: // Ground Slam
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                    spellInfo->Effects[0].TargetB = 0;
                    spellInfo->Effects[1].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                    spellInfo->Effects[1].TargetB = 0;
                    break;
                case 123654: // Lightning bolt
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 120499: // Shockwave pick targets
                    spellInfo->Effects[EFFECT_0].BasePoints = 0;
                    break;
                // ENDOF MOGU'SHAN PALACE
                //
                // STORMSTOUT BREWERY
                //
                case 106847: // Barrel Toss
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_DEST_AREA_ENEMY;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(16);
                    break;
                case 106614: // Barrel Ride
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 106768: // Brew Barrel Periodic Trigger
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 94465: // Brew Barrel Aura Canceller
                case 122376: // Barrel Drop
                case 106647: // Rolling Barrel Cosmetic
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CASTABLE_WHILE_ON_VEHICLE;
                    break;
                case 106808: // Ground Pound
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                    break;
                // STORMSTOUT BREWERY ENDOF
                //
                // TEMPLE OF THE JADE SERPENT
                //
                case 106112: // Release Doubt
                {
                    const SpellRadiusEntry* radius = sSpellRadiusStore.LookupEntry(22);
                    if (!radius)
                        break;
                    spellInfo->Effects[0].RadiusEntry = radius; //200yards.
                    break;
                }
                case 106267: // Hydrolance Pulse
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(16);
                    break;
                case 106113: // Touch of Nothingness
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 106736: // Wither Will
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 115167: // Corrupted Waters
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 113315: // Intensity
                {
                    SpellInfo const* spell = sSpellMgr->GetSpellInfo(113379);
                    if (!spell)
                        break;
                    spellInfo->DurationEntry = spell->DurationEntry;
                    break;
                }
                case 106909: // Jade Soul
                {
                    SpellInfo const* spell = sSpellMgr->GetSpellInfo(113379);
                    if (!spell)
                        break;
                    spellInfo->DurationEntry = spell->DurationEntry;
                    break;
                }
                case 107223: // Sunfire Rays
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 106762: // Summon Corrupt Droplet
                case 113394: // Intensity
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 106334: // Wash Away
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    spellInfo->AttributesEx3 &= ~ SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 106331:
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CLIENT_UI_TARGET_EFFECTS;
                    break;
                case 106959: // Serpent Wave Periodic
                    spellInfo->Effects[0].ApplyAuraTickCount = 400;
                    break;
                case 106105: // Hydrolance Pulse
                case 106104: // Hydrolance Pulse
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(16);
                    break;
                // ENDOF TEMPLE OF THE JADE SERPENT
                //
                // SCENARIOS:
                //
                case 131641: // Rain of Sha
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    break;
                case 125510: // Speed of Jinja
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                    break;
                case 121886: // Throw Keg in Cauldron
                case 141812: // Cannon Launch
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 121872: // Cleaver Toss
                    spellInfo->Attributes &= ~SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                    break;
                case 133556: // Bombard
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 133557: // Bombard damage
                    spellInfo->Attributes |= SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                    break;
                case 122142: // Earth Shattering
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 120349: // Fire Trap
                case 123800: // Lightning Trap
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 134985: // Meet the scout (Show`s like DB_TARGET, not nearby entry in sniff data)
                case 134996: // Meet the scout
                case 134089: // Meet the scout (Horde)
                case 136170: // Meet the scout
                    spellInfo->Effects[0].TargetA = TARGET_DEST_DB;
                    break;
                case 142139: // Stone Rain
                    spellInfo->Effects[EFFECT_0].BasePoints = 32500;
                    break;
                case 139186: // Eternal Banishment (seems like not summoner, static spawn)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 134210: // Memory of Relicvary
                    spellInfo->AreaGroupId = 0; // wrong section, shouldn`t require black temple map
                    break;
                case 125564: // Whirlpool
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 132776: // Summon Rope Swing
                case 132787: // Summon Net Climber
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER; // should have same like 141133 for corrent work
                    break;
                case 144374: // Invoke Lava
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 142629: // Banana Storm Eff
                case 142191: // Amber Globule Detonation
                case 133156: // Electric Field
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 142238: // Heal Illusion
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                // ENDOF SCENARIOS:
                //
                // BRAWLERS GUILD
                //
                case 133290: // Flames of fallen glory
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 133292;
                    break;
                case 133332: // Shadowy Blast
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 124860: // Rain Dance
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 134789: // Fallen kin
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28); // 5s
                    break;
                case 136194: // Feral Spirit
                    spellInfo->Attributes &= ~SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                    break;
                case 133346: // Dark Zone (Missed trigger data for this)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 133465: // Summon Ghost
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                    break;
                case 140954: // Luckydoom
                case 140955: // Good Luckydo (required not existed target aura)
                    spellInfo->TargetAuraSpell = 132633;
                    break;
                case 141059: // Overcharged (same, required not existed spell)
                case 141335: // Lightning Charge Visual
                    spellInfo->TargetAuraSpell = 0;
                    break;
                // ENDOF BRAWLERS GUILD
                //
                // MOGU'SHAN VAULT
                //
                case 116227: // Life's Fragile Thread
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 116000: // Voodoo Dolls
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 130774:// Amethyst Pool - Periodic damage
                    spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 118566: // Lifestream
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 116161: // Crossed Over
                    spellInfo->Effects[EFFECT_1].MiscValue = 2; // Set Phase to 2
                    spellInfo->Effects[EFFECT_3].Effect    = 0; // No need to summon
                    break;
                case 116272: // Banishment
                    spellInfo->Effects[EFFECT_0].MiscValue = 6; // Set Phase to 6
                    break;
                case 116525: // Focused Assault
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    break;
                case 117921: // Massive Attacks
                    spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_IGNORE_RESISTANCES;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 116829: // Focalised Energy
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 118303: // Fixate
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 15850: // Chilled
                case 16927: // Chilled
                case 20005: // Chilled
                    spellInfo->Mechanic = MECHANIC_SNARE;
                    break;
                case 89640: // Water Spirit
                    spellInfo->Effects[EFFECT_0].BasePoints = 5000;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 130649: // Life Spirit
                    spellInfo->Effects[EFFECT_0].BasePoints = 20000;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 116711: // Draw Flame
                    spellInfo->ChannelInterruptFlags = { 0, 0 };
                    spellInfo->AuraInterruptFlags = { 0, 0 };
                    break;
                case 118443: // Instantly Death
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[0].TargetB = 0;
                    break;
                case 116989: // Overloaded Missile
                case 117220: // Overloaded Triggered
                case 118430: // Core Beam
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 129724: // Grasping Energy Tendrils
                    spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                    break;
                case 117960: // Celestial Breathe
                    spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                    break;
                case 118018: // Draw Power (Summons)
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[2].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[3].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[4].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[5].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 116661: // Draw Power (lightning damage for activated focus)
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 117558:
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13); // 10 yards
                    break;
                case 117829: // Cowardice (DoT)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 117833: // Crazy Thought
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 117708: // Maddening Shout
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                    break;
                case 117222: // Sweeping kick
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER_FRONT;
                    break;
                case 125092: // Petrification
                    spellInfo->StackAmount = 100; // max
                    break;
                case 116980: // Fortify Stone Aura
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_20_YARDS);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_20_YARDS);
                    spellInfo->ExplicitTargetMask = TARGET_FLAG_UNIT_MASK;
                    break;
                case 121247: // Impale
                    spellInfo->Attributes &= ~SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                    break;
                case 115877: // fully petrified
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28); // 5s
                    break;
                case 117918: // Overhand Strike
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_5_YARDS);
                    break;
                case 116835: // Devastating Arc Players aura
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 116815: // Magnetize Armor
                case 117193:
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 116818: // Magnetize
                case 117195:
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 116561: // Energized Tiles
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_1_YARD);
                    break;
                case 116510: // Explosion
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    break;
                case 117961: // Impervious Shield
                    spellInfo->AttributesEx &= ~SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY;
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_NOT_STEALABLE;
                    spellInfo->InterruptFlags = 0;
                    break;
                case 118047: // Pillage
                    spellInfo->Speed = SPEED_CHARGE;
                    break;
                case 116803: // Titan Gas
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 116365: // Arcane Velocity
                case 116223: // Jade Shards
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 117203: // Siphoning Shield [issue with geodata, without it shield could`ve pull from area, it result - no effect/remove mechanic]
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                    break;
                case 125206: // Rend Flesh
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 126816: // Dampening Field
                    spellInfo->Effects[EFFECT_0].BasePoints = -10;
                    spellInfo->Effects[EFFECT_1].BasePoints = -10;
                    break;
                // ENDOF MOGU'SHAN VAULT
                //
                // HEART OF FEAR
                //
                case 122336: // Sonic Ring
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_0_5_YARDS);
                    spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_0_5_YARDS);
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 124673: // Sonic Pulse
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_0_5_YARDS);
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 123100: // Pheromones Switch
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_3_5_YARDS);
                    break;
                case 122761: // exhale
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 121898: // Whirling blade
                    spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_8_YARDS);
                    break;
                case 123735: // Dread Scretch
                case 125464: // Sha Energy
                    spellInfo->MaxAffectedTargets = 2;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 123471: // intensify
                case 123470:
                case 132254:
                    spellInfo->AttributesEx |= SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY;
                    break;
                case 124033: // Storm Unleashed Jump
                    spellInfo->Speed = SPEED_CHARGE;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 123597: // Storm Unleashed stalker east/west
                case 123639:
                case 123640:
                case 123643:
                case 123644:
                case 123645:
                    spellInfo->Effects[EFFECT_0].MiscValue = 63278;
                    break;
                case 121949: // Parasitic Growth
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 122224: // Impaling Spear
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 122064: // Corrosive Resin
                    spellInfo->StackAmount = 5; // max
                    break;
                case 123792: // Cry of Fear
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 123596: // Dissonance Field
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_4_YARDS);
                    break;
                case 124845: // Calamity
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 124258: // Storm Unleashed Ride
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_3_5_YARDS);
                    break;
                case 123430: // Ride vehicle legs
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->Effects[EFFECT_0].BasePoints = 1;
                    break;
                case 123431:
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->Effects[EFFECT_0].BasePoints = 2;
                    break;
                case 123432:
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->Effects[EFFECT_0].BasePoints = 3;
                    break;
                case 123433:
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->Effects[EFFECT_0].BasePoints = 4;
                    break;
                case 125098: // Retreat
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(427); // ~10min, doesn`t matter, just need more 2m 45s
                    break;
                case 123707: // Eyes of the Empress
                    spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 124844: // Amassing Darkness dmg
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 124843: // Amazing Darkness eff
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 123459: // Wind Step Effect
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    break;
                case 122370: // Reshape Life
                    spellInfo->Effects[EFFECT_9].MiscValue = 16;
                    break;
                case 123175: // Windstep
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 123963: // Korthik Strike
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 122185: // Mending Ex aura
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27);
                    break;
                case 122147: // Mending
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 123504:
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                    break;
                case 122551: // Draw Power Visual
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 142189: // Summon amber globule (12s duration, blizzard, u srsly?)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(427);
                    break;
                case 122839: // Tempest Slash [only dbm trigger]
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 124777: // Poison Bomb
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 133042: // Fixate
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 126841: // Klaxxi Resonance
                    spellInfo->Effects[EFFECT_0].BasePoints = -10;
                    spellInfo->Effects[EFFECT_1].BasePoints = -10;
                    break;
                // ENDOF HEART OF FEAR
                //
                // TERRACE OF ENDLESS SPRING
                //
                case 125706:
                case 130022:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 118312: // Water Bolt
                case 117187: // Lightning Bolt
                case 122881: // Unstable Bolt
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 117230: // Overpowered
                    spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                    break;
                case 123461: //Get Away!
                    /*spellInfo->Effects[2].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[2].ApplyAuraName = SPELL_AURA_APPLY_CREATURE_IMMUNITIES;
                    spellInfo->Effects[2].MiscValue = 1614;
                    spellInfo->Effects[2].MiscValueB = 0;
                    spellInfo->Effects[2].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[2].TargetB = 0;*/
                    break;
                case 119414: // Breath of Fear
                    spellInfo->Effects[2].Effect = 0;
                    break;
                case 119312: // Conjure Terror Spawn (01)
                case 119370: // Conjure Terror Spawn (02)
                case 119371: // Conjure Terror Spawn (03)
                case 119372: // Conjure Terror Spawn (04)
                    spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[0].TargetB = 0;
                    break;
                case 119841: // Fearless
                    spellInfo->Effects[0].TargetB = TARGET_DEST_DB;
                    spellInfo->Effects[1].TargetB = TARGET_DEST_DB;
                    break;
                case 126848: // Ritual of Purification
                    spellInfo->Effects[0].BasePoints = -10;
                    spellInfo->Effects[1].BasePoints = -10;
                    break;
                case 107145: // Wall of Light activation
                    spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                    break;
                case 125713: // Channeling Corruption (triggered)
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 125736: // Night Terrors (missile)
                    spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                    break;
                case 117988: // Defiled Ground
                    spellInfo->Effects[0].Effect = 0;
                    spellInfo->Effects[1].Mechanic = MECHANIC_NONE;
                    break;
                case 117052: // Corruption Sha
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[0].TargetB = 0;
                    spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[1].TargetB = 0;
                    spellInfo->Effects[2].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[2].TargetB = 0;
                    break;
                case 127731: // Corruption Sha (triggered)
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[0].TargetB = 0;
                    break;
                case 123244: // Hide
                    spellInfo->Effects[0].Effect = 0;
                    spellInfo->Effects[2].Effect = 0;
                    break;
                case 123620: // Clouded Reflection
                    spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                    break;
                case 117163: // Water bolt
                case 117519: // Touch of Sha
                case 120629: // Huddle In Terror
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 120285: // Transfer Light
                    spellInfo->TargetAuraState = 0;
                    break;
                case 117999: // Light of Wall Buff
                case 117866: // Champion of the Light
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_SAVE_AURA_TO_DB;
                    break;
                case 120453: // Conjure Dread Spawn [ we haven`t sniffs from heroic]
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_DEST_DEST);
                    break;
                case 122777: // Nightmares [idk, seems like not 8]
                    spellInfo->Effects[EFFECT_0].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_7_YARDS);
                    spellInfo->Effects[EFFECT_1].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_7_YARDS);
                    break;
                case 122718: // Force and Ferve
                    spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_IGNORE_RESISTANCES;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                // ENDOF TERRACE OF ENDLESS SPRING
                //
                // ISLE OF THUNDER
                //
                case 140244: // Lightning Crack
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 137263: // Jump on pterrorwing
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 140639: // Infiltrating Stormsea
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 139221: // Blood Drip
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); // 10 secs
                    break;
                case 137889: // Siphon Essence (conditions doesn`t work anyway)
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_SUMMONER;
                    break;
                case 140001: // Event Cleanup (unk effect 426)
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                // ENDOF ISLE OF THUNDER
                //
                // THRONE OF THUNDER
                //
                case 136991: // Biting Cold (Damage)
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 138389: // Static Wound Damage
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 138732: // Ionization
                    spellInfo->Effects[EFFECT_0].TargetA = 0;
                    break;
                case 140774: // Lightning Storm summ
                    spellInfo->Effects[EFFECT_0].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50_YARDS);
                    spellInfo->Effects[EFFECT_0].TargetA = 0;
                    break;
                case 138349: // Static Wound
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0; // this spell try to trigger not existed spell 52477
                    break;
                case 136992: // Biting Cold
                    spellInfo->StackAmount = 1;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 137261: // Lightning Storm(damage)
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_SRC_CASTER);
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENEMY);
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_500_YARDS);
                    break;
                case 137162: // Static Burst
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 40 yds
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 137389: // Focused Lightning Speed
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 1000;
                    break;
                case 137676: // Focused Lightning Grow
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 500;
                    break;
                case 138652: // Eruption
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENEMY);
                    break;
                case 137107: // Reckless Charge
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 138658: // Eruption Damage
                    spellInfo->Effects[EFFECT_0].TargetB = SpellImplicitTargetInfo(TARGET_UNIT_SRC_AREA_ENEMY);
                    break;
                case 137390: // Shadowed Loa Spirit
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break;
                case 140636: // Conjure Elementals [missed implict]
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_CASTER);
                    break;
                case 139852: // Teleport all to depths
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 134920: // Quake Stomp
                    spellInfo->Effects[EFFECT_2].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_CASTER);
                    break;
                case 139834: // Cinders
                    spellInfo->Effects[0].TargetA = SpellImplicitTargetInfo(TARGET_DEST_DEST);
                    break;
                case 139832: // Submerged
                    spellInfo->AuraInterruptFlags = { 0, 0 };
                    break;
                case 139848: // Acid Rain (missile)
                    spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_TARGET_ANY);
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 139866: // Torrent of Ice.
                    spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 139504: // Rampage Poison Cast.
                case 139513: // Rampage Arcane Cast.
                case 139433: // Rampage Flame Cast.
                case 139440: // Rampage Frost Cast.
                    spellInfo->Effects[0].Effect = SPELL_EFFECT_TRIGGER_MISSILE;
                    spellInfo->Effects[0].TargetA = TARGET_DEST_TARGET_ENEMY;
                    break;
                case 134339: // Ji kun wings stuff
                case 140014: // Daedalian Wings
                case 140013: // Flight
                case 133755: // Flight
                    spellInfo->AuraInterruptFlags = { 0, 0 };
                    spellInfo->AreaGroupId = 0;
                    break;
                case 89428: // Jump visual spin
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_LANDING;
                    break;
                case 133740: // Yellow Beam
                case 133734: // Red Beam
                case 133672: // Red Beam
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 139364: // Spirit Lantern
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 140814: // Spray Water
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 138328: // Draw Vita
                case 138327: // Draw Anima
                case 137422:
                    spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 138323: // Summon essence of Anima
                case 138324: // Summon essence of Vita (no data about triggers etc.)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 138178: // Thunder Trap
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 134912: // Decapitate (wrong, not 5y)
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6);
                    break;
                case 134916: // Decapitate Debuff
                case 139900: // Storm Cloud
                case 136922: // Frostbite
                case 137359: // Shadow Loa Marker
                case 139857: // Torrent of Ice
                case 133768: // Arterial Cut
                case 133731: // Spectrum Ann
                case 133737:
                case 133675:
                case 134755: // Eye Sore
                case 136326: // Overcharge eff
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 135098: // Thunderstruck (5s too low, set 10)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1);
                    break;
                case 139316: // Putrify
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 137611: // Conduit Level
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_UNTARGETABLE;
                    break;
                case 138254: // Invoke Celestials
                case 138189:
                case 138267:
                case 138264:
                    spellInfo->Effects[EFFECT_2].MiscValue = 2;
                    break;
                case 136591: // Rushing Wind pull
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    spellInfo->Effects[EFFECT_2].Effect = 0; // seems like wrong eff (video data haven`t stun eff at pull)
                    break;
                case 138435: // Arcing Lightning (wrong target define?)
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    break;
                case 136948: // Displace
                    spellInfo->Effects[EFFECT_2].TriggerSpell = 0; // wrong synch
                    break;
                case 138613: // Matter Swap (unk_138)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 138486: // Crimson Wake Fixate
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 138359: // Jump to Platform (no data in sniffs, only dest)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 137528: // Feed Young (should`ve 2.5s in heroic too!)
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 2500;
                    break;
                case 134321: // Eat
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 134347: // Incubated Low (should`ve 5s)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28); // 5s
                    break;
                case 136413: // Force Will
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_110;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 112844: // Cocoon
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(8);
                    break;
                case 140600: // Sonic Call
                    spellInfo->Effects[EFFECT_0].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_15_YARDS);
                    break;
                case 136232: // Maze
                case 136233:
                case 136236:
                case 136237:
                case 136238:
                case 136239:
                case 136242:
                case 136243:
                case 136244:
                case 136234:
                case 136235:
                case 136240:
                case 136241:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 138795: // ice Comet Selector
                    spellInfo->Speed = 40.0f;
                    break;
                case 136654: // Rending Charge
                case 134691: // Impale
                case 137641: // Soul Fragment
                case 136937: // Frostbite
                case 136587: // Venom Bolt Volley
                case 138659: // Touch of the Animus
                case 136178: // Mutation
                case 140508: // Volatile Mutation
                case 143576: // Canned Heat Eff
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 137408: // Fan of Flames
                    spellInfo->AttributesEx2 |= SPELL_ATTR4_FIXED_DAMAGE;
                    break;
                case 137052: // Primordius Proximity
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 136192: // Arcing Lightning
                    spellInfo->Effects[EFFECT_4].BasePoints = 4000000;
                    break;
                case 140640: // Screech
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28); // 5s
                    break;
                case 139870: // Torrent of Ice
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(25); // 180s
                    break;
                case 137412: // Flames Of Passion
                case 137415:
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_CHARGE;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 139485: // Dark Winds
                    spellInfo->AreaGroupId = 3576;
                    break;
                case 138300: // Fortitude of the Ox (core issue)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 136962: // Anima Ring Eff
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 140688: // Tortos Knock Away
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 136147: // Fist Smash
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 133798: // life Drain [TARGET_UNK_130]
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_110;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CONE_ENEMY_110;
                    break;
                case 139202: // Blue Ray Tracking
                case 139204: // Infared Tracking
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                // ENDOF THRONE OF THUNDER
                //
                // SIEGE OF ORGRIMMAR
                //
                case 143412: // Swirl
                case 143413:
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 500;
                    break;
                case 144639: // Corruption
                case 146325: // Cutter Laser Fixate
                case 144918: // Cutter Laser
                case 144774: // Reaching Attack
                case 143579: // Swelling Corruption
                case 142931: // Expoised Veins
                case 143275: // Hewn
                case 143278: // Genetic Alteration
                case 143279: // Genetic Alteration aur
                case 142929: // Tenderizing Strikes
                case 142315: // Caustic Blood
                case 143339: // Injection
                case 142797: // Noxios Vapours
                case 143240: // Sonic Pulse
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 143574: // Swelling Corruption
                    spellInfo->ProcCooldown = 100; // set very very low but it should have a personal inner CD
                    break;
                case 143424: // Sha Shear
                case 143423:
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    spellInfo->ChannelInterruptFlags[0] |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 147638: // Mighty Stomp
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 143210: // Instance Poison
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 143840: // Mark of Anguish
                    spellInfo->AttributesEx2 &= ~SPELL_ATTR2_CAN_TARGET_DEAD;
                    break;
                case 143292: // Fixate
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 144396: // Vengeful Strikes
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 0 to 40
                    break;
                case 143544: // Calamity Overpower
                case 145605: // Corrupted Brew Overpower
                case 149280: // Crimson Acid
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                    break;
                case 145655: // Shared Tornment
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                    break;
                case 143961: // Defiled Ground
                case 144657: // Piercing Corruption
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 0 to 40
                    break;
                case 145735: // Icy Fear
                case 147082: // Burst of Anger
                case 145073: // Residual Corruption
                    spellInfo->TargetAuraSpell = 0; // req 145178, but this doesn`t exist in any DBC
                    break;
                case 145212: // Unleashed Anger
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 144482: // Tear Reality
                case 144628: // Titanic Smash
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 144421: // Corruption
                    spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA; // unk 202, possible same like SPELL_AREA_AURA_ENTRY
                    break;
                case 144363: // Gift of the Titans Link
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    spellInfo->TargetAuraState = 0;
                    break;
                case 146037: // Sha Vortex
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                    break;
                case 144574: // Corrupted Prison
                case 144636:
                case 144683:
                case 144684:
                    spellInfo->AttributesEx2 &= ~SPELL_ATTR2_CAN_TARGET_DEAD;
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_DEATH_PERSISTENT;
                    break;
                case 147068: // Flames of Galakrond
                case 143500: // Heroic Shockwave
                case 143266: // Launch Sawblade
                case 144330: // Iron Prison
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 146782: // Blizzard (unk_141)
                case 148849: // Flamestrike (unk_141)
                case 144692: // Pool of Fire (unk_125)
                case 146255: // Path of Blossoms (unk 138)
                case 143850: // Essence of Y`shaarj (unk 138)
                case 144553: // Mortar Barrage Missle (unk 138)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 149188: // Tidal Wave
                case 147820:
                    spellInfo->Effects[EFFECT_0].TargetB = 0; // handle calculate angle - unk134
                    break;
                case 146027: // Mortar Blast
                    spellInfo->Speed = 1.6f; // 2.0 seems so fast
                    break;
                case 147897: // The most complicated bomb in the world (shouldn`t interrupt by damage taken)
                case 147916:
                    spellInfo->ChannelInterruptFlags = { 0, 0 };
                    break;
                case 144209: // Borer Drill
                case 144673: // Crawler Mine
                    spellInfo->Effects[EFFECT_0].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    break;
                case 148962: // Blade Salvo
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                    break;
                case 145730: // Resistance Totem Eff
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DYNOBJ_ALLY;
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                    break;
                case 144090: // Foul Stream
                case 146765: // Flame Arrows
                case 147404: // Set to Blow
                case 145987: // Set to Blow Button
                case 142913: // Displaced Energy
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED | SPELL_ATTR1_CANT_BE_REDIRECTED;
                    break;
                case 143987: // Falling Ash
                    spellInfo->Effects[EFFECT_1].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_500_YARDS);
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED | SPELL_ATTR1_CANT_BE_REDIRECTED;
                    break;
                case 143480: // Assassins Mark
                case 143882: // Hunter`s Mark
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 143593: // self-proc, not 1s for each
                    spellInfo->ProcCooldown = 250;
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 143990: // Foul Geyser
                case 146488: // Anti Air Cannon
                case 144208: // Death from Above
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 143503: // Warsong
                case 143343: // Defeaning Screech
                case 144331: // Iron Prison
                case 144334: // Iron Tomb
                case 144327: // Ricochet
                case 141189: // Blind Strike
                case 141192: // Blind Cleave
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case 146815: // Supernova (creature died - no dmg eff to not near players)
                    spellInfo->Effects[EFFECT_1].Effect = 0;
                    break;
                case 142898: // Arcing Smash Preparation (jump_dest not will work immediatly right, so override in MovementInForm)
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 142826: // Arcing Smash Controll (missed target implicit)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 143846: // Essence of Y`shaarj (should trigger each 3s AFTER spawn)
                    spellInfo->Effects[EFFECT_2].ApplyAuraTickCount = 6000;
                    break;
                case 143430: // Clump Check
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER; // unk 140
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                    spellInfo->TargetAuraSpell = 0;
                    break;
                case 143445: // Fixate
                case 143828: // Locked On
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 146365: // Set to Blow At (unk 138)
                case 145460: // Mogu Rune of Power (unk 138)
                case 148302: // Bombard
                case 147459: // Magnetic Pull Jump
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 142722: // Unstable Defense System Teleport
                    spellInfo->Attributes |= SPELL_ATTR0_CASTABLE_WHILE_DEAD;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 145864: // Defense System class Armageddon
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                    break;
                case 145125: // Lever Eff
                case 146820: // Unstable Spark Summon
                case 143838: // Laser Turret Targeting
                case 143280: // Bloodletting
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 143917: // Unlocking
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 146180: // Gusting Crane Kick
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 1000;
                    break;
                case 144115: // Flame Coating
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 142416: // Insane Calculation:: Fiery Edge
                    spellInfo->TargetAuraSpell = 0; // miss value
                    break;
                case 143526: // Deactivated Weapons
                case 143263:
                case 143936:
                case 144012:
                case 146381:
                case 143938: // Activated Weapons
                case 144011:
                case 143711:
                case 146380:
                case 146418:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 143385: // Electrostatic Charge
                    spellInfo->AttributesEx |= SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE;
                    break;
                case 143487: // Magnetic Crush
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 148243: // Fixate
                case 147665: // Fixate
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                    break;
                case 143798: // Pipe Transfert Jump
                    spellInfo->Effects[EFFECT_0].ValueMultiplier = 2.5f;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 145223: // Pipe Transfert main teleport
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                    break;
                case 143758: // Pipe Transfert to conveyor
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                    spellInfo->Effects[EFFECT_1].BasePoints = 100; // it`s joke ? +1million % dmg from fall?
                    break;
                case 143833: // Laser Turret Targeting
                    spellInfo->TargetAuraSpell = 0;
                    break;
                case 147010: // Death from Above (unk 125)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 143830: // Laser Ground Eff
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST; // unk 125
                    break;
                case 144094: // Sonic Resonance
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 143696: // Create Amber Bomb
                case 141795: // Summon Paragons Type
                case 141801:
                case 141796:
                case 141797:
                case 141798:
                case 141802:
                case 141800:
                case 141799:
                case 141803:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 143505: // Point Blank Shot
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 143974: // Shield Bash
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 0 to 40
                    break;
                case 142811: // Fiery Edge
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28);
                    break;
                case 143287: // Bloodletting Missle
                case 143288:
                case 143289:
                case 143284:
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 5000 yards
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 142735: // Reaction: Blue (unk 138)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ANY;
                    break;
                case 143740: // Caustic Amber
                    spellInfo->Effects[EFFECT_0].TargetB = 0; // unk138 not req
                    break;
                case 143939: // Gouge
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 0 to 40
                    break;
                case 143941: // Mutilate
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 0 to 40
                    spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                    break;
                case 144584: // Chain lightning
                case 144583: // Chain Heal
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 144585: // Ancestral Fury
                case 148681: // Reave
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 144994: // Whirling Corruption
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 144762: // Desecrated
                case 144817: // Desecrated Empower
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 147831: // Attack Thrall (ok, u should hit to thrall and has attribute SPELL_ATTR3_ONLY_TARGET_PLAYERS...)
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                    break;
                case 145246: // Phase Three Transform
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 144969: // Annihilate
                    spellInfo->PreventionType = SPELL_PREVENTION_TYPE_NONE;
                    break;
                case 145065: // Touch of Y`shaarj
                case 145071:
                    spellInfo->Effects[EFFECT_8].Effect = 0;
                    spellInfo->Effects[EFFECT_9].Effect = 0;
                    spellInfo->Effects[EFFECT_10].Effect = 0;
                    spellInfo->Effects[EFFECT_11].Effect = 0;
                    break;
                case 145171: // Empowered Touch of Y`shaarj
                case 145175:
                    spellInfo->Effects[EFFECT_8].Effect = 0;
                    spellInfo->Effects[EFFECT_9].Effect = 0;
                    spellInfo->Effects[EFFECT_10].Effect = 0;
                    spellInfo->Effects[EFFECT_11].Effect = 0;
                    spellInfo->Effects[EFFECT_12].Effect = 0;
                    break;
                case 144956: // Jump to Ground (only distance exist for it, no any creature in sniffs)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                    break;
                case 145599: // Touch of Y`shaarj
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5); // 0 to 40
                    break;
                case 147126: // Clump Check (unk 127)
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                    spellInfo->Effects[EFFECT_0].MaxRadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_50000_YARDS);
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_100_YARDS);
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 146987: // Phase Four Transform (not need inc total heal pct)
                    spellInfo->Effects[EFFECT_1].Effect = 0; // wrong model for it
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 147011: // Manifest Rage (shouldn`t be interruptable)
                    spellInfo->ChannelInterruptFlags = { 0, 0 };
                    break;
                case 143340: // Injection Initialize
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                    break;
                case 144191: // Explosive Ring Visual
                case 144192:
                case 144193:
                case 144195:
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                    break;
                // ENDOF SIEGE OF ORGRIMMAR
                case 44544: // Fingers of Frost
                    spellInfo->Effects[0].SpellClassMask[0] |= 0x00020000;  // Ice Lance.
                    break;
                case 2944:  // Devouring Plague - I dunno why is so.
                    spellInfo->Effects[0].BonusMultiplier = spellInfo->Effects[3].BonusMultiplier;
                    spellInfo->Effects[1].BonusMultiplier = spellInfo->Effects[4].BonusMultiplier;
                    spellInfo->Effects[3].Effect = 0;
                    spellInfo->Effects[4].Effect = 0;
                    break;
                case 686:   // Shadow Bolt - same shit
                    spellInfo->Effects[EFFECT_0].BonusMultiplier = spellInfo->Effects[EFFECT_2].BonusMultiplier;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 90647:  // Clear Glyph (Rank 2)
                case 127649: // Remove Talent (Rank 2)
                    spellInfo->SpellLevel = 81;
                    break;
                case 111621: // Clear Glyph (Rank 3)
                case 113873: // Remove Talent (Rank 3)
                    spellInfo->SpellLevel = 86;
                    break;
                case 104315: // Demonic Fury - 109145 applies on learn.
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    break;
                case 22570:  // Miam
                    spellInfo->Effects[EFFECT_2].BasePoints = 300;  // Roughly. Bonus damage from EFFECT_1 must be: fixed_base + weapon_damage / 2 + cp_damage * cp_count.
                    break;                                          // This damage adds to the weapon damage and we take 200%. So let's just add another 100% of weapon damage.
                case 79147:  // Sanguinary Vein
                    spellInfo->Effects[EFFECT_1].Effect = 0;    // Handled in scripts.
                    break;
                case 84617:  // Revealing Strike - Adds 35% damage to Rupture, Eviscerate and Crimson Tempest.
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_FROM_CASTER;
                    spellInfo->Effects[EFFECT_2].SpellClassMask = flag128(0x120000, 0, 0x2000, 0);
                    break;
                case 1949:   // Hellfire
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_MOVEMENT; // Strongly hints that for channeled spells we need to use ChannelInterruptFlags and 0x8 is movement
                    break;
                case 104025: // Immolation Aura - May be not blizzlike.
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 5857;
                    break;
                case 5782:   // Fear
                    spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;   // Only to remove Fear Ward. Very hacky.
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    break;
                case 75206: // Flamebreaker, Flameseer's Staff, Flamebreaker quest
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                    break;
                case 74723: // Summon Unbound Flamesparks, Flameseer's Staff, Flamebreaker quest
                    spellInfo->Effects[EFFECT_0].MiscValue = 40065;
                    break;
                // Nexus Phase Shift
                case 99488:
                    spellInfo->Effects[EFFECT_0].MiscValue = 256;
                    break;
                // Twilight Prison
                case 99561:
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = 0;
                    break;
                // Open Fire Portal
                case 100528:
                case 101067:
                case 101068:
                case 101069:
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                    break;
                // Summon Kalecgos
                case 98069:
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    spellInfo->Effects[EFFECT_1].MiscValue = 256;
                    break;
                case 5484:   // Howl of Terror
                    spellInfo->Effects[EFFECT_1].Effect = 0; // NO_TARGET not working for SPELL_EFFECT_TRIGGER_SPELL
                    break;
                case 22482: // Blade Flurry
                    spellInfo->AttributesEx2 &= ~SPELL_ATTR2_CAN_TARGET_DEAD;
                    break;
                case 45182: // Cheating Death
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
                    break;
                    // HACKALERT By this way it comes to Mastery: Elemental Overload but it filtered in script
                case 45284: // Lighting Bolt (Mastery proc)
                    spellInfo->SpellFamilyFlags[0] |= 0x1;
                    break;
                case 45297: // Chain Lighting (Mastery proc)
                    spellInfo->SpellFamilyFlags[0] |= 0x2;
                    break;
                    // HACKALERT END
                case 5672:   // Healing Stream
                case 114941: // Healing Tide
                    spellInfo->AttributesEx5 |= SPELL_ATTR5_HASTE_AFFECT_DURATION;
                    break;
                case 16213: // Purification
                    spellInfo->Effects[EFFECT_0].SpellClassMask[2] |= 0x40; // Earth Shield (heal)
                    break;
                case 76857: // Mastery: Critical Block - Hacked in Unit::isBlockCritical
                case 131116: // Raging Blow! - Handled by Raging Blow hit
                    spellInfo->ProcFlags = 0;
                    break;
                case 31801:  ///< Seal of Truth
                    spellInfo->ProcFlags |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
                    break;
                case 32379: // Shadow Word: Death
                    std::swap(spellInfo->Effects[EFFECT_0].TargetA, spellInfo->Effects[EFFECT_1].TargetA);
                    std::swap(spellInfo->Effects[EFFECT_0].TargetB, spellInfo->Effects[EFFECT_1].TargetB);
                    break;
                case 113344: // Bloodbath
                    spellInfo->SpellScalingId = 0;
                    spellInfo->Effects[EFFECT_0].ScalingMultiplier = 1.0;
                    break;
                case 108505: // Archimonde's Darkness
                case 146659: // Glyph of Rapid Displacement
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_AURA_SEND_AMOUNT;
                    break;
                case 58423:  // Relentless Strikes
                    spellInfo->Effects[EFFECT_0].SpellClassMask[1] |= 0x8;  // Envenom.
                    break;
                case 116943: // Earthgrab
                    spellInfo->Effects[EFFECT_0].TargetB = 0;
                    break;
                case 29725:  // Sudden Death
                    spellInfo->ProcChance = 100; // Handled in script.
                    break;
                case 52437:  // Sudden Death
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_ADD_PCT_MODIFIER;
                    spellInfo->ProcCharges = 1;
                    break;
                case 114175: // Demonic Slash
                    spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE;
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 104314;
                    spellInfo->Effects[EFFECT_1].BasePoints = 60;
                    break;
                case 115129: // Expel Harm
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                    break;
                case 116023: // Sparring
                case 123332: // Crackling Jade Lightning Chi Proc Driver
                    spellInfo->AuraInterruptFlags[0] &= ~AURA_INTERRUPT_FLAG_CAST;
                    break;
                case 108212: // Burst of Speed
                    spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                    break;
                case 65409: // Into the Nightmare (quest 13940)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 6203:  // Soulstone
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                    break;
                case 348:   // Immolate - 50% damage for Rain of Fire
                    spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_FROM_CASTER;
                    spellInfo->Effects[EFFECT_2].SpellClassMask = flag128(0, 0, 0x10000000, 0);
                    spellInfo->Effects[EFFECT_2].BasePoints = 50;
                    break;
                case 128988: // Protection of the Celestials
                case 138728: // Change of Tactics
                    spellInfo->CastTimeEntry = nullptr;
                    spellInfo->CastTimeMax = 0;
                    spellInfo->CastTimeMin = 0;
                    break;
                case 109212: // Spirit Bond. I dunno wtf is this.
                    spellInfo->Effects[EFFECT_0].Effect = 0;
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                    break;
                case 147364: // Rapid Rolling
                    spellInfo->StackAmount = 0;
                    break;
                case 122235: // Clash
                case 126450: // Clash (symbiosis)
                    spellInfo->Speed = SPEED_CHARGE;
                    break;
                case 2094:   // Blind
                case 5374:   // Mutilate (main-hand damage, spell hit rolled by main spell)
                case 141004: // Lightning Strike (Ranged. Melee version has this attribute)
                    spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                    break;
                case 2812:   // Denounce
                    spellInfo->FacingCasterFlags |= SPELL_FACING_FLAG_INFRONT;
                    break;
                case 29838:  // Second Wind
                    spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                    spellInfo->Effects[EFFECT_0].ApplyAuraTickCount = 500;
                    break;
                case 16931:  // Thick Hide
                    spellInfo->Effects[EFFECT_3].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 31224:  // Cloak of Shadows
                case 39666:  // Cloak of Shadows (?)
                case 65961:  // Cloak of Shadows (?)
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 81549;
                    break;
                case 110788: // Cloak of Shadows
                    spellInfo->Effects[EFFECT_1].TriggerSpell = 110790;
                    break;
                case 3355:   // Freezing Trap
                case 13812:  // Explosive Trap
                case 45145:  // Snake Trap
                case 57879:  // Snake Trap
                case 63487:  // Ice Trap
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                    break;
                case 20164:  // Seal of Justice
                    spellInfo->ProcCooldown = 0;    // Dunno why
                    break;
                case 26023:  // Pursuit of Justice
                    spellInfo->Effects[EFFECT_5].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 34299:  // Leader of the Pack
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_HEAL_PCT;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 42463: // Seal of Truth
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 35546:  // Combat Potency
                case 49560:  // Death Grip
                    spellInfo->Mechanic = MECHANIC_NONE;
                    break;
                case 42223:  // Rain of Fire
                    spellInfo->AttributesEx3 &= ~SPELL_ATTR3_NO_PUSHBACK_ON_DAMAGE;
                    break;
                case 42992:
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 53652:  // Beacon of Light
                    spellInfo->AttributesEx6 &= ~SPELL_ATTR6_IGNORE_CASTER_AURAS;
                    break;
                case 55533:  // Glyph of Healing Wave
                case 83077:  // Improved Serpent Sting
                case 88767:  // Fulmination
                case 86678:  // Light of the Ancient Kings
                case 101024: // Glyph of Ferocious Bite
                case 122233: // Crimson Tempest
                case 146585: // Execution Sentence
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                    break;
                case 114916: // Execution Sentence
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_HEALING_BONUS;
                    break;
                case 68285:  // Leader of the Pack
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_ENERGIZE_PCT;
                    break;
                case 52610:  // Savage Roar
                case 127538: // Savage Roar
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER;
                    break;
                case 54957: // Glyph of Flash of Light
                    spellInfo->Effects[EFFECT_0].SpellClassMask[2] |= 0x02000000; // Eternal Flame
                    break;
                case 57761:  // Brain Freeze
                    spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                    spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_ABILITY_IGNORE_AURASTATE;
                    spellInfo->Effects[EFFECT_2].SpellClassMask[1] |= 0x00001000; // Frostfire Bolt
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 57840:  // Killing Spree. Possible this is the blizzlike bug and it mustn't be reproduced every time
                    spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                    break;
                case 58673:  // Glyph of Icebound Fortitude
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 61249:  // Improved Recuperate
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                    break;
                case 57841:  // Killing Spree
                case 57842:  // Killing Spree Off-Hand
                case 81269:  // Efflorescence
                case 108503: // Grimoire of Sacrifice
                case 117667: // Legacy of the Emperor (single target)
                case 118253: // Serpent Sting
                case 120699: // Lynx Rush
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13);
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 61851:  // Killing Spree
                case 81549:  // Cloak of Shadows
                case 110790: // Cloak of Shadows
                    spellInfo->AttributesEx |= SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY;
                    break;
                case 119607: // Renewing Mist TODO: RadiusEntry > MaxRadiusEntry may be fix this in another way?
                    spellInfo->Effects[EFFECT_1].RadiusEntry = spellInfo->Effects[EFFECT_1].MaxRadiusEntry;
                    break;
                case 80240:  // Havoc
                    spellInfo->Effects[EFFECT_1].SpellClassMask[1] |= 0x200;    // Needs for Curse of the Elements
                    break;
                case 82691:  // Ring of Frost
                    spellInfo->AttributesEx |= (SPELL_ATTR1_CANT_BE_REFLECTED | SPELL_ATTR1_CANT_BE_REDIRECTED);
                    break;
                case 86213:  // Soul Swap Exhale
                    spellInfo->SpellPowers.clear(); // WTF?
                    break;
                case 1160:   // Demoralizing Shout
                case 29341:  // Shadowburn
                case 53563:  // Beacon of Light
                case 77758:  // Thrash
                case 106830: // Thrash
                case 108686: // Immolate
                case 114893: // Stone Bulwark
                    spellInfo->Effects[EFFECT_1].RealPointsPerLevel = 888;
                    spellInfo->Effects[EFFECT_1].BasePoints = 300;
                    spellInfo->Effects[EFFECT_1].ScalingMultiplier = 5.0;
                case 132464: // Chi Wave
                case 132467: // Chi Wave
                case 137619: // Marked for Death
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 89485:  // Inner Focus
                    spellInfo->Effects[EFFECT_1].SpellClassMask = spellInfo->Effects[EFFECT_0].SpellClassMask;
                    spellInfo->ProcCharges = 1;
                    break;
                case 81022:  // Stampede
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                    break;
                case 88611:  // Smoke Bomb
                    spellInfo->Attributes    |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 91724:  // Spell Warding
                    spellInfo->AuraInterruptFlags[0] &= ~AURA_INTERRUPT_FLAG_CHANGE_MAP;
                    break;
                case 100977: // Harmony
                    spellInfo->Effects[EFFECT_0].SpellClassMask[3] |= 0x00000400; // Cenarion Ward
                    break;
                case 108371: // Harvest Life
                    spellInfo->Effects[EFFECT_0].MiscValue = SPELLMOD_DOT;
                    break;
                case 20066:  // Repentance
                case 31687:  // Summon Water Elemental
                case 51963:  // Gargoyle Strike
                case 113792: // Psychic Terror
                    spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                    break;
                case 115611: // Temporal Ripples
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    spellInfo->Effects[EFFECT_0].RealPointsPerLevel = 0.0f;
                    break;
                case 116330: // Dizzying Haze
                    spellInfo->Effects[EFFECT_1].Mechanic = MECHANIC_NONE;
                    break;
                case 120755: // Glaive Toss
                case 120756: // Glaive Toss
                    spellInfo->Effects[EFFECT_0].ChainTarget = 2;
                    break;
                case 122016: // Incite
                    spellInfo->ProcCharges = 3;
                    spellInfo->StackAmount = 0;
                    break;
                case 47753:  // Divine Aegis
                case 53271:  // Master's Call
                case 54114:  // Heart of the Phoenix
                case 102791: // Wild Mushroom: Bloom
                case 102792: // Wild Mushroom: Bloom
                case 113896: // Demonic Gateway
                case 120729: // Demonic Gateway
                case 117666: // Legacy of the Emperor
                case 121414: // Glaive Toss
                case 120761: // Glaive Toss
                case 121148: // Cascade
                case 127629: // Cascade
                case 125950: // Soothing Mist (statue)
                case 125962: // Soothing Mist <Statue> Trigger
                case 124280: // Touch of Karma
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 120840: // Cascade (cooldown)
                case 127631: // Cascade (cooldown)
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    spellInfo->Attributes    |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    break;
                case 124974: // Nature's Vigil
                    spellInfo->AttributesEx7 &= ~SPELL_ATTR7_ALLIANCE_ONLY;
                    break;
                case 124988: // Nature's Vigil
                case 125321: // Bananarang
                case 145153: // Dream of Cenarius
                    spellInfo->Effects[EFFECT_0].RealPointsPerLevel = 0.0f;
                    break;
                case 125883: // Zen Flight
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_MELEE_ATTACK;
                    spellInfo->AuraInterruptFlags[0] &= ~AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION;
                    break;
                case 55001:  // Parachute
                case 126389: // Goblin Glider
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_LANDING;
                    break;
                case 128997: // Spirit Beast Blessing
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER; // By default TARGET_NO_TARGET, need to check auto cast
                    break;
                case 134563: // Healing Elixirs
                    spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                    break;
                case 137323: // Lucidity
                    spellInfo->Effects[EFFECT_0].SpellClassMask[2] |= 0x00400000;
                    break;
                case 146199: // Spirit of Chi - Ji
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                    break;
                case 146557: // Frozen Thoughts
                    spellInfo->Effects[EFFECT_0].SpellClassMask[0] |= 0x00020220;
                    spellInfo->Effects[EFFECT_0].SpellClassMask[1] |= 0x00001000;
                    break;
                case 203780: // Server-side aura for Leap
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_LANDING;
                    break;
                case 88330: // Enchanted Imp Sack
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                    break;
                case 33384: // Mass Charm
                    spellInfo->Effects[EFFECT_0].BasePoints = 9999;
                    break;
                case 66655: // Airship Cannon (Isle of Conquest)
                case 67436: // Airship Cannon (Isle of Conquest)
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(174); // 1000yds
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                    break;
                case 49872: case 51673: // Rocket Blast (Strand of the Ancients)
                case 67452: case 67453: // Rocket Blast (Isle of Conquest)
                case 68170: case 67454: // Rocket Blast (Isle of Conquest) (Level 80)
                case 66195: case 67444: // Napalm (Isle of Conquest)
                case 67458: case 67459: // Fire Cannon, Cannon (Isle of Conquest)
                case 66541: case 68169: // Incendiary Rocket (Isle of Conquest)
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 50000yds
                    break;
                case 60682: // Ride Vehicle
                case 60683: // Ride Vehicle
                case 60944: // Ride Shredder
                case 60962: // Ride Tower Cannon
                case 60968: // Ride Wintergrasp Vehicle
                case 62309: // Ride Vehicle (Scales w/ Gear)
                case 65030: // Ride Vehicle (Scales w/ Gear)
                case 65031: // Ride Vehicle (Scales w/ Gear)
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_1].TargetB = 0;
                    break;
                case 65266: // Gear Scaling
                case 65635: // Gear Scaling
                case 65636: // Gear Scaling
                case 66666: // Gear Scaling
                case 66667: // Gear Scaling
                case 66668: // Gear Scaling
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 37897: // Parachute
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_LANDING;
                    break;
                case 136605: // Subdue Pterodactyl
                case 136789: // Periodic Abort Trigger
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 148009: // Spirit of Chi-Ji
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                    break;
                case 45391: // Summon Demonic Vapor
                case 45680: // Shadow Bolt
                case 45188: // Dawnblade Attack
                case 45892: // Sinister Reflection
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 44866: // Spectral Blast
                    spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                    break;
                case 45833: // Power of the Blue Flight
                    spellInfo->Speed = 0; // We need instant cast becase the spell is cast by GO-summoned trigger which lives only for 1ms
                    break;
                case 136529: // Hitch Devilsaur
                case 138494: // Star Beam
                    spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                    break;
                case 46145:  // Cold Slap (Ahune)
                    spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                    spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                    break;
                case 80265: // Potion of Illusion
                case 80396:
                    spellInfo->AttributesEx4 |= SPELL_ATTR4_NOT_USABLE_IN_ARENA_OR_RATED_BG;
                    spellInfo->AttributesEx8 |= SPELL_ATTR8_NOT_IN_BG_OR_ARENA;
                    spellInfo->AttributesEx9 |= SPELL_ATTR9_NOT_USABLE_IN_ARENA;
                    break;
                case 55088: // Barrel o' Fun
                case 57415: // The Reckoning
                case 57544: // Ashbringer
                    spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS; // These should target GameObjects at destination, when cast from various angles dest might end up being inside their model and the cast will fail
                    break;
                case 147500: // Stomp
                case 147512: // Stomp
                    spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(EFFECT_RADIUS_10_YARDS);
                    break;
                case 148429: // Fire-Watcher's Oath
                    spellInfo->ProcChance = 100; // 5.4.8 hotfix
                    spellInfo->AuraInterruptFlags[0] &= ~AURA_INTERRUPT_FLAG_MOUNT;
                    break;
                case 42132: // Headless Horseman - Start Fire
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 50000yds
                    break;
                case 32312: // Move 1
                case 37388: // Move 2
                     spellInfo->MaxAffectedTargets = 1;
                     spellInfo->AttributesCu &= ~SPELL_ATTR0_CU_NEGATIVE;
                     break;
                case 39338: // Karazhan - Chess, Medivh CHEAT: Hand of Medivh, Target Horde
                case 39342: // Karazhan - Chess, Medivh CHEAT: Hand of Medivh, Target Alliance
                    spellInfo->MaxAffectedTargets = 1;
                    break;
                case 39341: // Karazhan - Chess, Medivh CHEAT: Fury of Medivh, Target Horde
                case 39344: // Karazhan - Chess, Medivh CHEAT: Fury of Medivh, Target Alliance
                    spellInfo->MaxAffectedTargets = 3;
                    break;
                case 30532: // In Game
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(13); // 50000yds
                    break;
                case 39331: // Game In Session
                    spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AREA_AURA_FRIEND; // Hack, because blizzlike _PARTY does not affect players when cast by an NPC
                    break;
                case 30236:
                case 30239:
                case 30240:
                case 30241: // TODO: check wtf with custom spelleffect_dbc
                    spellInfo->Effects[EFFECT_0].MiscValueB = 64;
                    break;
                case 142910: // Ironbound Wraithcharger
                    spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(16);
                    break;
                case 50553: // Parachute (hack for royale event, revert if necessary)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                    break;
                case 100117: // AFD Royale - Drop
                    spellInfo->AuraInterruptFlags[0] |= AURA_INTERRUPT_FLAG_LANDING;
                    break;
                case 88343: //
                    spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(4);
                    spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(1);
                    break;
                default:
                    break;
            }

            switch (spellInfo->SpellFamilyName)
            {
                case SPELLFAMILY_PALADIN:
                    // Seals of the Pure should affect Seal of Righteousness
                    if (spellInfo->SpellIconID == 25 && spellInfo->Attributes & SPELL_ATTR0_PASSIVE)
                        spellInfo->Effects[EFFECT_0].SpellClassMask[1] |= 0x20000000;
                    break;
                case SPELLFAMILY_DEATHKNIGHT:
                    // Icy Touch - extend FamilyFlags (unused value) for Sigil of the Frozen Conscience to use
                    if (spellInfo->SpellIconID == 2721 && spellInfo->SpellFamilyFlags[0] & 0x2)
                        spellInfo->SpellFamilyFlags[0] |= 0x40;
                    break;
            }

            for (auto&& effect : spellInfo->Effects)
                if (effect.ApplyAuraName == SPELL_AURA_REFLECT_SPELLS || effect.ApplyAuraName == SPELL_AURA_REFLECT_SPELLS_SCHOOL)
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_SPELL_REFLECT;

            // Many of this DoT's haven't SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER but must have it.
            if (spellInfo->AttributesEx10 & SPELL_ATTR10_CUMULATIVE_PERIODIC && spellInfo->DurationEntry)
                spellInfo->AttributesEx8 |= SPELL_ATTR8_DONT_RESET_PERIODIC_TIMER;
        }
    }

    if (SummonPropertiesEntry* properties = const_cast<SummonPropertiesEntry*>(sSummonPropertiesStore.LookupEntry(121)))
        properties->Type = SUMMON_TYPE_TOTEM;
    if (SummonPropertiesEntry* properties = const_cast<SummonPropertiesEntry*>(sSummonPropertiesStore.LookupEntry(647))) // 52893
        properties->Type = SUMMON_TYPE_TOTEM;

    TC_LOG_INFO("server.loading", ">> Loaded SpellInfo corrections in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadItemSpellsCorrections()
{
    for (auto&& itr : *sObjectMgr->GetItemTemplateStore())
    {
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            if (itr.second.Spells[i].SpellId && (
                itr.second.Spells[i].SpellTrigger == ITEM_SPELLTRIGGER_ON_EQUIP ||
                itr.second.Spells[i].SpellTrigger == ITEM_SPELLTRIGGER_ON_NO_DELAY_USE))
            {
                for (auto&& store : mSpellInfoMap)
                    if (SpellInfo* spellInfo = store[itr.second.Spells[i].SpellId])
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_PERSISTENT;
            }
        }
    }

    for (uint32 i = 0; i < sItemSetStore.GetNumRows(); ++i)
    {
        if (ItemSetEntry const* set = sItemSetStore.LookupEntry(i))
        {
            for (auto&& spellId : set->spells)
                for (auto&& store : mSpellInfoMap)
                    if (SpellInfo* spellInfo = store[spellId])
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_PERSISTENT;
        }
    }
}
