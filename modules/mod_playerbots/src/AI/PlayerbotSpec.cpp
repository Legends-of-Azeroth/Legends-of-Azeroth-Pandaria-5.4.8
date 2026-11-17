#include "PlayerbotSpec.h"

#include "AiFactory.h"
#include "Playerbots.h"
#include "PlayerbotAI.h"
#include "Player.h"

unsigned int PlayerBotSpec::GetSpectab(Player* player)
{
    const Specializations& spec = player->GetSpecialization();
    switch (spec)
    {
        case SPEC_HUNTER_BEAST_MASTERY:
        case SPEC_DEATH_KNIGHT_BLOOD:
        case SPEC_DRUID_BALANCE:
        case SPEC_WARRIOR_ARMS:
        case SPEC_PALADIN_HOLY:
        case SPEC_MAGE_ARCANE:
        case SPEC_PRIEST_DISCIPLINE:
        case SPEC_ROGUE_ASSASSINATION:
        case SPEC_SHAMAN_ELEMENTAL:
        case SPEC_WARLOCK_AFFLICTION:
        case SPEC_MONK_BREWMASTER:
        {
            return static_cast<unsigned int>(0);
        }

        case SPEC_HUNTER_MARKSMANSHIP:
        case SPEC_DEATH_KNIGHT_FROST:
        case SPEC_DRUID_FERAL:
        case SPEC_WARRIOR_FURY:
        case SPEC_PALADIN_PROTECTION:
        case SPEC_MAGE_FIRE:
        case SPEC_PRIEST_HOLY:
        case SPEC_ROGUE_COMBAT:
        case SPEC_SHAMAN_ENHANCEMENT:
        case SPEC_WARLOCK_DEMONOLOGY:
        case SPEC_MONK_WINDWALKER:
        {
            return static_cast<unsigned int>(1);
        }

        case SPEC_HUNTER_SURVIVAL:
        case SPEC_DEATH_KNIGHT_UNHOLY:
        case SPEC_DRUID_GUARDIAN:
        case SPEC_WARRIOR_PROTECTION:
        case SPEC_PALADIN_RETRIBUTION:
        case SPEC_MAGE_FROST:
        case SPEC_PRIEST_SHADOW:
        case SPEC_ROGUE_SUBTLETY:
        case SPEC_SHAMAN_RESTORATION:
        case SPEC_WARLOCK_DESTRUCTION:
        case SPEC_MONK_MISTWEAVER:
        {
            return static_cast<unsigned int>(2);
        }
        case SPEC_DRUID_RESTORATION:
        {
            return static_cast<unsigned int>(3);
        }
    }
    return 99;
}

bool PlayerBotSpec::IsRanged(Player* player, bool bySpec)
{
    PlayerbotAI* botAi = GET_PLAYERBOT_AI(player);
    if (!bySpec && botAi)
        return botAi->ContainsStrategy(STRATEGY_TYPE_RANGED);

    const Specializations& spec = player->GetSpecialization();
    switch (player->GetClass())
    {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
        case CLASS_ROGUE:
        case CLASS_MONK:
        {
            return false;
        }
        case CLASS_DRUID:
        {
            if (spec == Specializations::SPEC_DRUID_FERAL || spec == Specializations::SPEC_DRUID_GUARDIAN)
            {
                return false;
            }
            break;
        }
        case CLASS_PALADIN:
        {
            if (spec != Specializations::SPEC_DEATH_KNIGHT_UNHOLY)
            {
                return false;
            }
            break;
        }
        case CLASS_SHAMAN:
        {
            if (spec == Specializations::SPEC_SHAMAN_ENHANCEMENT)
            {
                return false;
            }
            break;
        }
    }
    return true;
}

bool PlayerBotSpec::IsMelee(Player* player, bool bySpec)
{
    return !IsRanged(player, bySpec);
}

bool PlayerBotSpec::IsCaster(Player* player, bool bySpec)
{
    return IsRanged(player, bySpec) && player->GetClass() != CLASS_HUNTER;
}

bool PlayerBotSpec::IsCombo(Player* player, bool bySpec)
{
    // int tab = AiFactory::GetPlayerSpecTab(player);
    return player->GetClass() == CLASS_ROGUE ||
        (player->GetClass() == CLASS_DRUID && player->HasAura(768));  // cat druid
}

bool PlayerBotSpec::IsRangedDps(Player* player, bool bySpec)
{
    return IsRanged(player, bySpec) && IsDps(player, bySpec);
}

bool PlayerBotSpec::IsTank(Player* player, bool bySpec)
{
    PlayerbotAI* botAi = GET_PLAYERBOT_AI(player);
    if (!bySpec && botAi)
        return botAi->ContainsStrategy(STRATEGY_TYPE_TANK);

    if (player->GetSpecialization() == Specializations::SPEC_PALADIN_PROTECTION ||
        player->GetSpecialization() == Specializations::SPEC_WARRIOR_PROTECTION ||
        player->GetSpecialization() == Specializations::SPEC_MONK_BREWMASTER ||
        player->GetSpecialization() == Specializations::SPEC_DRUID_GUARDIAN ||
        player->GetSpecialization() == Specializations::SPEC_DEATH_KNIGHT_BLOOD)
        return true;
    return false;
}

bool PlayerBotSpec::IsHeal(Player* player, bool bySpec)
{
    PlayerbotAI* botAi = GET_PLAYERBOT_AI(player);
    if (!bySpec && botAi)
        return botAi->ContainsStrategy(STRATEGY_TYPE_HEAL);

    if (player->GetSpecialization() == Specializations::SPEC_PALADIN_HOLY ||
        player->GetSpecialization() == Specializations::SPEC_PRIEST_DISCIPLINE||
        player->GetSpecialization() == Specializations::SPEC_PRIEST_HOLY ||
        player->GetSpecialization() == Specializations::SPEC_DRUID_RESTORATION ||
        player->GetSpecialization() == Specializations::SPEC_SHAMAN_RESTORATION ||
        player->GetSpecialization() == Specializations::SPEC_MONK_MISTWEAVER)
        return true;

    return false;
}

bool PlayerBotSpec::IsDps(Player* player, bool bySpec)
{
    PlayerbotAI* botAi = GET_PLAYERBOT_AI(player);
    if (!bySpec && botAi)
        return botAi->ContainsStrategy(STRATEGY_TYPE_DPS);

    return (!IsTank(player) && !IsHeal(player));
}

uint32 PlayerBotSpec::GetGroupTankNum(Player* player)
{
    Group* group = player->GetGroup();
    if (!group)
    {
        return 0;
    }
    uint32 result = 0;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (IsTank(member) && member->IsAlive())
        {
            result++;
        }
    }
    return result;
}

bool PlayerBotSpec::IsMainTank(Player* player)
{
    Group* group = player->GetGroup();
    if (!group)
    {
        return false;
    }
    ObjectGuid mainTank = ObjectGuid();
    Group::MemberSlotList const& slots = group->GetMemberSlots();
    for (Group::member_citerator itr = slots.begin(); itr != slots.end(); ++itr)
    {
        if (itr->flags & MEMBER_FLAG_MAINTANK)
        {
            mainTank = itr->guid;
            break;
        }
    }
    if (mainTank != ObjectGuid::Empty)
    {
        return player->GetGUID() == mainTank;
    }
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (IsTank(member) && member->IsAlive())
        {
            return player->GetGUID() == member->GetGUID();
        }
    }
    return false;
}
bool PlayerBotSpec::IsAssistTank(Player* player) { return IsTank(player) && !IsMainTank(player); }

bool PlayerBotSpec::IsAssistTankOfIndex(Player* bot, Player* player, int index)
{
    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }
    Group::MemberSlotList const& slots = group->GetMemberSlots();
    int counter = 0;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (group->IsAssistant(member->GetGUID()) && IsAssistTank(member))
        {
            if (index == counter)
            {
                return player == member;
            }
            counter++;
        }
    }
    // not enough
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!group->IsAssistant(member->GetGUID()) && IsAssistTank(member))
        {
            if (index == counter)
            {
                return player == member;
            }
            counter++;
        }
    }
    return false;
}