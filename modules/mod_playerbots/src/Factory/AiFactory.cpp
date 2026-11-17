/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "AiFactory.h"

#include "BotAIObjectContext.h"

#include "Engine.h"
#include "Group.h"

#include "Item.h"

#include "PlayerbotAI.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"

#include "SharedDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

#include "DKAiObjectContext.h"
#include "DruidAiObjectContext.h"
#include "HunterAiObjectContext.h"
#include "PaladinAiObjectContext.h"
#include "PriestAiObjectContext.h"
#include "RogueAiObjectContext.h"
#include "ShamanAiObjectContext.h"
#include "MageAiObjectContext.h"
#include "WarriorAIObjectContext.h"
#include "WarlockAiObjectContext.h"

AiObjectContext* AiFactory::createAiObjectContext(Player* player, PlayerbotAI* botAI)
{
    switch (player->GetClass())
    {
        case CLASS_PRIEST:
            return new PriestAiObjectContext(botAI);
        case CLASS_MAGE:
            return new MageAiObjectContext(botAI);
        case CLASS_WARLOCK:
            return new WarlockAiObjectContext(botAI);
        case CLASS_WARRIOR:
            return new WarriorAiObjectContext(botAI);
        case CLASS_SHAMAN:
            return new ShamanAiObjectContext(botAI);
        case CLASS_PALADIN:
            return new PaladinAiObjectContext(botAI);
        case CLASS_DRUID:
            return new DruidAiObjectContext(botAI);
        case CLASS_HUNTER:
            return new HunterAiObjectContext(botAI);
        case CLASS_ROGUE:
            return new RogueAiObjectContext(botAI);
        case CLASS_DEATH_KNIGHT:
            return new DKAiObjectContext(botAI);
        //case CLASS_MONK:
            //return new MonkAiObjectContext(botAI);
    }

    return new BotAiObjectContext(botAI);
}

Specializations AiFactory::GetPlayerSpecTab(Player* bot)
{
    return bot->GetSpecialization();
}

BotRoles AiFactory::GetPlayerRoles(Player* player)
{
    BotRoles role = BOT_ROLE_NONE;
    const Specializations spec = player->GetSpecialization();

    switch (player->GetClass())
    {
        case CLASS_PRIEST:
            if (spec == Specializations::SPEC_PRIEST_SHADOW)
                role = BOT_ROLE_DPS;
            else
                role = BOT_ROLE_HEALER;
            break;
        case CLASS_SHAMAN:
            if (spec == Specializations::SPEC_DRUID_RESTORATION)
                role = BOT_ROLE_HEALER;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_WARRIOR:
            if (spec == Specializations::SPEC_WARRIOR_PROTECTION)
                role = BOT_ROLE_TANK;
            else
                role = BOT_ROLE_DPS;
            break;
        case CLASS_PALADIN:
            if (spec == Specializations::SPEC_PALADIN_HOLY)
                role = BOT_ROLE_HEALER;
            else if (spec == Specializations::SPEC_PALADIN_PROTECTION)
                role = BOT_ROLE_TANK;
            else if (spec == 2)
                role = BOT_ROLE_DPS;
            break;
        case CLASS_DRUID:
            if (spec == Specializations::SPEC_DRUID_FERAL || spec == Specializations::SPEC_DRUID_BALANCE)
                role = BOT_ROLE_DPS;
            else if (spec == Specializations::SPEC_DRUID_GUARDIAN)
                role = BOT_ROLE_TANK;
            else if (spec == Specializations::SPEC_DRUID_RESTORATION)
                role = BOT_ROLE_HEALER;
            break;
        case CLASS_MONK:
            if (spec == Specializations::SPEC_MONK_BREWMASTER)
                role = BOT_ROLE_TANK;
            else if (spec == Specializations::SPEC_MONK_MISTWEAVER)
                role = BOT_ROLE_HEALER;
            else if (spec == Specializations::SPEC_MONK_WINDWALKER)
                role = BOT_ROLE_DPS;
            break;
        default:
            role = BOT_ROLE_DPS;
            break;
    }

    return role;
}

std::string AiFactory::GetPlayerSpecName(Player* player)
{
    std::string specName;
    const Specializations spec = GetPlayerSpecTab(player);
    switch (player->GetClass())
    {
        case CLASS_PRIEST:
            if (spec == Specializations::SPEC_PRIEST_SHADOW)
                specName = "shadow";
            else if (spec == Specializations::SPEC_PRIEST_HOLY)
                specName = "holy";
            else
                specName = "disc";
            break;
        case CLASS_SHAMAN:
            if (spec == Specializations::SPEC_SHAMAN_RESTORATION)
                specName = "resto";
            else if (spec == Specializations::SPEC_SHAMAN_ENHANCEMENT)
                specName = "enhance";
            else
                specName = "elem";
            break;
        case CLASS_WARRIOR:
            if (spec == Specializations::SPEC_WARRIOR_PROTECTION)
                specName = "prot";
            else if (spec == Specializations::SPEC_WARRIOR_FURY)
                specName = "fury";
            else
                specName = "arms";
            break;
        case CLASS_PALADIN:
            if (spec == Specializations::SPEC_PALADIN_HOLY)
                specName = "holy";
            else if (spec == Specializations::SPEC_PALADIN_PROTECTION)
                specName = "prot";
            else
                specName = "retrib";
            break;
        case CLASS_DRUID:
            if (spec == Specializations::SPEC_DRUID_FERAL)
                specName = "feraldps";
            else if (spec == Specializations::SPEC_DRUID_GUARDIAN)
                specName = "guardiantank";
            else if (spec == Specializations::SPEC_DRUID_RESTORATION)
                specName = "resto";
            else
                specName = "balance";
            break;
        case CLASS_ROGUE:
            if (spec == Specializations::SPEC_ROGUE_ASSASSINATION)
                specName = "assas";
            else if (spec == Specializations::SPEC_ROGUE_COMBAT)
                specName = "combat";
            else
                specName = "subtle";
            break;
        case CLASS_HUNTER:
            if (spec == Specializations::SPEC_HUNTER_BEAST_MASTERY)
                specName = "beast";
            else if (spec == Specializations::SPEC_HUNTER_MARKSMANSHIP)
                specName = "marks";
            else
                specName = "surv";
            break;
        case CLASS_DEATH_KNIGHT:
            if (spec == Specializations::SPEC_DEATH_KNIGHT_FROST)
                specName = "frostdps";
            else if (spec == Specializations::SPEC_DEATH_KNIGHT_UNHOLY)
                specName = "unholydps";
            else
                specName = "blooddps";
            break;
        case CLASS_MAGE:
            if (spec == Specializations::SPEC_MAGE_ARCANE)
                specName = "arcane";
            else if (spec == Specializations::SPEC_MAGE_FIRE)
                specName = "fire";
            else
                specName = "frost";
            break;
        case CLASS_WARLOCK:
            if (spec == Specializations::SPEC_WARLOCK_DESTRUCTION)
                specName = "destru";
            else if (spec == Specializations::SPEC_WARLOCK_DEMONOLOGY)
                specName = "demono";
            else
                specName = "affli";
            break;
        case CLASS_MONK:
            if (spec == Specializations::SPEC_MONK_BREWMASTER)
                specName = "brewmaster";
            else if (spec == Specializations::SPEC_MONK_MISTWEAVER)
                specName = "mistweaver";
            else
                specName = "windwalker";
            break;
        default:
            break;
    }

    return specName;
}

void AiFactory::AddDefaultCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* engine)
{
    if (!player->InBattleground())
    {
        engine->addStrategiesNoInit("racials", "chat", "default", "cast time", "duel", "boost", nullptr);
    }
    if (sPlayerbotAIConfig->autoAvoidAoe && facade->HasRealPlayerMaster())
    {
        engine->addStrategy("avoid aoe", false);
    }
    engine->addStrategy("formation", false);

    const Specializations& spec = player->GetSpecialization();
    switch (player->GetClass())
    {
        case CLASS_PRIEST:
        {
            if (spec == Specializations::SPEC_PRIEST_SHADOW)
            {
                engine->addStrategiesNoInit("dps", "shadow debuff", "shadow aoe", nullptr);
            }
            else if (spec == Specializations::SPEC_PRIEST_DISCIPLINE)
            {
                engine->addStrategiesNoInit("heal", nullptr);
            }
            else
            {
                engine->addStrategiesNoInit("holy heal", nullptr);
            }

            engine->addStrategiesNoInit("dps assist", "cure", nullptr);
            break;
        }
        case CLASS_MAGE:
        {
            if (spec == Specializations::SPEC_MAGE_ARCANE)
                engine->addStrategiesNoInit("arcane", "arcane aoe", nullptr);
            else if (spec == Specializations::SPEC_MAGE_FIRE)
                engine->addStrategiesNoInit("fire", "fire aoe", nullptr);
            else
                engine->addStrategiesNoInit("frost", "frost aoe", nullptr);

            engine->addStrategiesNoInit("dps", "dps assist", "cure", nullptr);
            break;
        }
        case CLASS_WARRIOR:
        {
            if (spec == Specializations::SPEC_WARRIOR_PROTECTION)
                engine->addStrategiesNoInit("tank", "tank assist", "aoe", nullptr);
            else if (player->GetLevel() < 10 || spec == Specializations::SPEC_WARRIOR_ARMS)
                engine->addStrategiesNoInit("arms", "aoe", "dps assist", /*"behind",*/ nullptr);
            else
                engine->addStrategiesNoInit("fury", "aoe", "dps assist", /*"behind",*/ nullptr);
            break;
        }
        case CLASS_SHAMAN:
        {
            if (spec == Specializations::SPEC_SHAMAN_ELEMENTAL)
                engine->addStrategiesNoInit("caster", "caster aoe", "bmana", nullptr);
            else if (spec == Specializations::SPEC_SHAMAN_RESTORATION)
                engine->addStrategiesNoInit("heal", "bmana", nullptr);
            else
                engine->addStrategiesNoInit("melee", "melee aoe", "bdps", nullptr);

            engine->addStrategiesNoInit("dps assist", "cure", "totems", nullptr);
            break;
        }
        case CLASS_PALADIN:
        {
            if (spec == Specializations::SPEC_PALADIN_PROTECTION)
                engine->addStrategiesNoInit("tank", "tank assist", "bthreat", "barmor", "cure", nullptr);
            else if (spec == Specializations::SPEC_PALADIN_HOLY)
                engine->addStrategiesNoInit("heal", "dps assist", "cure", "bcast", nullptr);
            else
                engine->addStrategiesNoInit("dps", "dps assist", "cure", "baoe", nullptr);
            break;
        }
        case CLASS_DRUID:
        {
            if (spec == Specializations::SPEC_DRUID_BALANCE)
            {
                engine->addStrategiesNoInit("caster", "cure", "caster aoe", "dps assist", nullptr);
                engine->addStrategy("caster debuff", false);
            }
            else if (spec == Specializations::SPEC_DRUID_RESTORATION)
                engine->addStrategiesNoInit("heal", "cure", "dps assist", nullptr);
            else if (spec == Specializations::SPEC_DRUID_FERAL)
            {
                if (player->GetLevel() >= 20 && !player->HasAura(16931) /*thick hide*/)
                {
                    engine->addStrategiesNoInit("cat", "dps assist", nullptr);
                }
                else
                {
                    engine->addStrategiesNoInit("bear", "tank assist", nullptr);
                }
            }
            else if (spec == Specializations::SPEC_DRUID_GUARDIAN)
            {
                engine->addStrategiesNoInit("bear", "tank assist", nullptr);
            }
            break;
        }
        case CLASS_HUNTER:
        {
            engine->addStrategiesNoInit("dps", "aoe", "bdps", "dps assist", nullptr);
            engine->addStrategy("dps debuff", false);
            break;
        }
        case CLASS_ROGUE:
        {
            if (spec == Specializations::SPEC_ROGUE_ASSASSINATION)
            {
                engine->addStrategiesNoInit("melee", "dps assist", "aoe", /*"behind",*/ nullptr);
            }
            else
            {
                engine->addStrategiesNoInit("dps", "dps assist", "aoe", /*"behind",*/ nullptr);
            }
            break;
        }
        case CLASS_WARLOCK:
        {
            if (spec == Specializations::SPEC_WARLOCK_DEMONOLOGY)
                engine->addStrategy("demono", false);
            else if (spec == Specializations::SPEC_WARLOCK_DESTRUCTION)
                engine->addStrategy("destru", false);
            else
                engine->addStrategy("affli", false);
            
            engine->addStrategiesNoInit("dps assist", "dps", "aoe", nullptr);
            break;
        }
        case CLASS_DEATH_KNIGHT:
        {
            if (spec == Specializations::SPEC_DEATH_KNIGHT_BLOOD)
                engine->addStrategiesNoInit("blood", "tank assist", nullptr);
            else if (spec == Specializations::SPEC_DEATH_KNIGHT_FROST)
                engine->addStrategiesNoInit("frost", "frost aoe", "dps assist", nullptr);
            else
                engine->addStrategiesNoInit("unholy", "unholy aoe", "dps assist", nullptr);
            break;
        }
        case CLASS_MONK:
        {
            if (spec == Specializations::SPEC_MONK_BREWMASTER)
                engine->addStrategiesNoInit("tank", "tank assist", "aoe", nullptr);
            else if (spec == Specializations::SPEC_MONK_MISTWEAVER)
                engine->addStrategiesNoInit("heal", nullptr);
            else
                engine->addStrategiesNoInit("melee", "melee aoe", "dps assist", nullptr);

            engine->addStrategiesNoInit("dps assist", "cure", nullptr);
        }
        break;
    }

    if (PlayerBotSpec::IsTank(player, true))
        engine->addStrategy("tank face", false);
    if (PlayerBotSpec::IsMelee(player, true) && PlayerBotSpec::IsDps(player, true))
        engine->addStrategy("behind", false);
    if (PlayerBotSpec::IsHeal(player, true))
    {
        //if (sPlayerbotAIConfig->autoSaveMana)
            //engine->addStrategy("save mana", false);
        //engine->addStrategy("healer dps", false);
    }

    if (facade->IsRealPlayer() || sRandomPlayerbotMgr->IsRandomBot(player))
    {
        if (!player->GetGroup())
        {
            // change for heal spec
            engine->addStrategy("boost", false);
            engine->addStrategy("dps assist", false);
            engine->removeStrategy("threat", false);
            // engine-
            switch (player->GetClass())
            {
            case CLASS_PRIEST:
            {
                if (spec != Specializations::SPEC_PRIEST_SHADOW)
                {
                    engine->addStrategiesNoInit("holy dps", "shadow debuff", "shadow aoe", nullptr);
                }
                break;
            }
            case CLASS_DRUID:
            {
                if (spec == Specializations::SPEC_DRUID_RESTORATION)
                {
                    engine->addStrategiesNoInit("caster", "caster aoe", nullptr);
                    engine->addStrategy("caster debuff", false);
                }
                break;
            }
            case CLASS_SHAMAN:
            {
                if (spec == Specializations::SPEC_SHAMAN_RESTORATION)
                {
                    engine->addStrategiesNoInit("caster", "caster aoe", "bmana", nullptr);
                }
                break;
            }
            case CLASS_PALADIN:
            {
                if (spec == Specializations::SPEC_PALADIN_HOLY)
                {
                    engine->addStrategiesNoInit("dps", "dps assist", "baoe", nullptr);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    else
    {
        engine->ChangeStrategy(sPlayerbotAIConfig->combatStrategies);
    }

    // temporary
    //engine->addStrategy("ranged");
    //engine->addStrategy("melee");
    //engine->addStrategy("say hello");
}

Engine* AiFactory::createCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* aiObjectContext)
{
    Engine* engine = new Engine(facade, aiObjectContext);
    AddDefaultCombatStrategies(player, facade, engine);
    engine->Init();
    return engine;
}

void AiFactory::AddDefaultNonCombatStrategies(Player* player, PlayerbotAI* const facade, Engine* nonCombatEngine)
{
    const Specializations tab = GetPlayerSpecTab(player);

    switch (player->GetClass())
    {
        case CLASS_PRIEST:
            nonCombatEngine->addStrategiesNoInit("dps assist", "cure", nullptr);
            break;
        case CLASS_PALADIN:
            if (tab == Specializations::SPEC_PALADIN_PROTECTION)
            {
                nonCombatEngine->addStrategiesNoInit("bthreat", "tank assist", "barmor", nullptr);
                if (player->GetLevel() >= 20)
                {
                    nonCombatEngine->addStrategy("bstats", false);
                }
                else
                {
                    nonCombatEngine->addStrategy("bdps", false);
                }
            }
            else if (tab == Specializations::SPEC_PALADIN_HOLY)
                nonCombatEngine->addStrategiesNoInit("dps assist", "bmana", "bcast", nullptr);
            else
                nonCombatEngine->addStrategiesNoInit("dps assist", "bdps", "baoe", nullptr);

            nonCombatEngine->addStrategiesNoInit("cure", nullptr);
            break;
        case CLASS_HUNTER:
            nonCombatEngine->addStrategiesNoInit("bdps", "dps assist", "pet", nullptr);
            break;
        case CLASS_SHAMAN:
            if (tab == Specializations::SPEC_SHAMAN_ELEMENTAL || tab == Specializations::SPEC_SHAMAN_RESTORATION)
                nonCombatEngine->addStrategy("bmana", false);
            else
                nonCombatEngine->addStrategy("bdps", false);

            nonCombatEngine->addStrategiesNoInit("dps assist", "cure", nullptr);
            break;
        case CLASS_MAGE:
            if (tab == Specializations::SPEC_MAGE_ARCANE || tab == Specializations::SPEC_MAGE_FIRE)
                nonCombatEngine->addStrategy("bdps", false);
            else
                nonCombatEngine->addStrategy("bmana", false);

            nonCombatEngine->addStrategiesNoInit("dps assist", "cure", nullptr);
            break;
        case CLASS_DRUID:
            if (tab == Specializations::SPEC_DRUID_FERAL)
            {
                if (player->GetLevel() >= 20 && !player->HasAura(16931) /*thick hide*/)
                {
                    nonCombatEngine->addStrategy("dps assist", false);
                }
                else
                {
                    nonCombatEngine->addStrategy("tank assist", false);
                }
            }
            else if (tab == Specializations::SPEC_DRUID_GUARDIAN)
            {
                nonCombatEngine->addStrategy("tank assist", false);
            }
            else
                nonCombatEngine->addStrategiesNoInit("dps assist", "cure", nullptr);
            break;
        case CLASS_WARRIOR:
            if (tab == Specializations::SPEC_WARRIOR_PROTECTION)
                nonCombatEngine->addStrategy("tank assist", false);
            else
                nonCombatEngine->addStrategy("dps assist", false);
            break;
        case CLASS_WARLOCK:
            if (tab == Specializations::SPEC_WARLOCK_AFFLICTION)
            {
                nonCombatEngine->addStrategiesNoInit("bmana", nullptr);
            }
            else if (tab == Specializations::SPEC_WARLOCK_DEMONOLOGY)
            {
                nonCombatEngine->addStrategiesNoInit("bdps", nullptr);
            }
            else if (tab == Specializations::SPEC_WARLOCK_DESTRUCTION)
            {
                nonCombatEngine->addStrategiesNoInit("bhealth", nullptr);
            }
            nonCombatEngine->addStrategiesNoInit("dps assist", nullptr);
            break;
        case CLASS_DEATH_KNIGHT:
            if (tab == Specializations::SPEC_DEATH_KNIGHT_BLOOD)
                nonCombatEngine->addStrategy("tank assist", false);
            else
                nonCombatEngine->addStrategy("dps assist", false);
            break;
        default:
            nonCombatEngine->addStrategy("dps assist", false);
            break;
    }
    if (!player->InBattleground())
    {
        nonCombatEngine->addStrategiesNoInit("nc", "food", "chat", "follow", "default", "quest", "loot", "gather", "duel",
            "buff", "mount", "emote", nullptr);
    }
    nonCombatEngine->addStrategy("say hello");
}

Engine* AiFactory::createNonCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* aiObjectContext)
{
    Engine* nonCombatEngine = new Engine(facade, aiObjectContext);

    AddDefaultNonCombatStrategies(player, facade, nonCombatEngine);
    nonCombatEngine->Init();
    return nonCombatEngine;
}

void AiFactory::AddDefaultDeadStrategies(Player* player, PlayerbotAI* const facade, Engine* deadEngine)
{
    (void)facade;  // unused and remove warning
    deadEngine->addStrategiesNoInit("dead", "stay", "chat", "default", "follow", nullptr);

    if (sRandomPlayerbotMgr->IsRandomBot(player) && !player->GetGroup())
    {
        deadEngine->removeStrategy("follow", false);
    }
}

Engine* AiFactory::createDeadEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext)
{
    Engine* deadEngine = new Engine(facade, AiObjectContext);
    AddDefaultDeadStrategies(player, facade, deadEngine);
    deadEngine->Init();
    return deadEngine;
}
