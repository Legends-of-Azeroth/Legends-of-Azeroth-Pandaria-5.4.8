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

#include "ScriptMgrMacros.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "OutdoorPvPMgr.h"
#include "ScriptSystem.h"
#include "Transport.h"
#include "Vehicle.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "GossipDef.h"
#include "CreatureAIImpl.h"
#include "Player.h"
#include "WorldPacket.h"
#include "Chat.h"
#include "AreaTrigger.h"
#include "GameEventMgr.h"
#ifdef ELUNA
#include "LuaEngine.h"
#include "HookMgr.h"
#endif

UnusedScriptContainer UnusedScripts;
UnusedScriptNamesContainer UnusedScriptNames;

// Utility macros to refer to the script registry.
#define SCR_REG_MAP(T) ScriptRegistry<T>::ScriptMap
#define SCR_REG_ITR(T) ScriptRegistry<T>::ScriptMapIterator
#define SCR_REG_LST(T) ScriptRegistry<T>::ScriptPointerList

// Utility macros for looping over scripts.
#define FOR_SCRIPTS(T, C, E) \
    if (SCR_REG_LST(T).empty()) \
        return; \
    for (SCR_REG_ITR(T) C = SCR_REG_LST(T).begin(); \
        C != SCR_REG_LST(T).end(); ++C)
#define FOR_SCRIPTS_RET(T, C, E, R) \
    if (SCR_REG_LST(T).empty()) \
        return R; \
    for (SCR_REG_ITR(T) C = SCR_REG_LST(T).begin(); \
        C != SCR_REG_LST(T).end(); ++C)
#define FOREACH_SCRIPT(T) \
    FOR_SCRIPTS(T, itr, end) \
    itr->second

// Utility macros for finding specific scripts.
#define GET_SCRIPT_NO_RET(T, I, V) \
    T* V = ScriptRegistry<T>::GetScriptById(I);
#define GET_SCRIPT(T, I, V) \
    T* V = ScriptRegistry<T>::GetScriptById(I); \
    if (!V) \
        return;
#define GET_SCRIPT_RET(T, I, V, R) \
    T* V = ScriptRegistry<T>::GetScriptById(I); \
    if (!V) \
        return R;

struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} *SpellSummary;

ScriptObject::ScriptObject(char const* name) : _name(name)
{
    sScriptMgr->IncreaseScriptCount();
}

ScriptObject::~ScriptObject()
{
    sScriptMgr->DecreaseScriptCount();
}

ScriptMgr::ScriptMgr()
    : _scriptCount(0), _scheduledScripts(0) { }

ScriptMgr::~ScriptMgr() { }

ScriptMgr* ScriptMgr::instance()
{
    static ScriptMgr instance;
    return &instance;
}

//#ifdef ELUNA
//extern void StartEluna(bool restart);
//#endif
void ScriptMgr::Initialize()
{
    uint32 oldMSTime = getMSTime();

    LoadDatabase();

    TC_LOG_INFO("server.loading", "Loading C++ scripts");
    FillSpellSummary();
    _scriptLoader();

#ifdef SCRIPTS
    for (std::string const& scriptName : UnusedScriptNames)
    {
        TC_LOG_ERROR("sql.sql", "ScriptName '%s' exists in database, but no core script found!", scriptName.c_str());
    }
#endif

    UnloadUnusedScripts();

    TC_LOG_INFO("server.loading", ">> Loaded %u C++ scripts in %u ms", GetScriptCount(), GetMSTimeDiffToNow(oldMSTime));
    /* Eluna [Lua Engine] */
//#ifdef ELUNA
//    StartEluna(false);
//#endif
}

void ScriptMgr::Unload()
{
    #define SCR_CLEAR(T) \
        for (SCR_REG_ITR(T) itr = SCR_REG_LST(T).begin(); itr != SCR_REG_LST(T).end(); ++itr) \
            delete itr->second; \
        SCR_REG_LST(T).clear();

    // Clear scripts for every script type.
    SCR_CLEAR(SpellScriptLoader);
    SCR_CLEAR(ServerScript);
    SCR_CLEAR(WorldScript);
    SCR_CLEAR(FormulaScript);
    SCR_CLEAR(WorldMapScript);
    SCR_CLEAR(InstanceMapScript);
    SCR_CLEAR(BattlegroundMapScript);
    SCR_CLEAR(ItemScript);
    SCR_CLEAR(CreatureScript);
    SCR_CLEAR(GameObjectScript);
    SCR_CLEAR(AreaTriggerScript);
    SCR_CLEAR(BattlegroundScript);
    SCR_CLEAR(OutdoorPvPScript);
    SCR_CLEAR(CommandScript);
    SCR_CLEAR(WeatherScript);
    SCR_CLEAR(AuctionHouseScript);
    SCR_CLEAR(ConditionScript);
    SCR_CLEAR(VehicleScript);
    SCR_CLEAR(DynamicObjectScript);
    SCR_CLEAR(TransportScript);
    SCR_CLEAR(AchievementCriteriaScript);
    SCR_CLEAR(PlayerScript);
    SCR_CLEAR(GuildScript);
    SCR_CLEAR(GroupScript);
    SCR_CLEAR(GameEventScript);
    SCR_CLEAR(UnitScript);
    SCR_CLEAR(SceneScript);
    SCR_CLEAR(QuestScript);

    #undef SCR_CLEAR

    for (ExampleScriptContainer::iterator itr = ExampleScripts.begin(); itr != ExampleScripts.end(); ++itr)
        delete *itr;
    ExampleScripts.clear();

    UnloadUnusedScripts();

    delete [] SpellSummary;
    delete [] UnitAI::AISpellInfo;
}

void ScriptMgr::UnloadUnusedScripts()
{
    for (size_t i = 0; i < UnusedScripts.size(); ++i)
        delete UnusedScripts[i];

    UnusedScripts.clear();
}

void ScriptMgr::LoadDatabase()
{
    sScriptSystemMgr->LoadScriptWaypoints();
}

void ScriptMgr::FillSpellSummary()
{
    UnitAI::FillAISpellInfo();

    SpellSummary = new TSpellSummary[sSpellMgr->GetSpellInfoStoreSize()];

    SpellInfo const* pTempSpell;

    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = sSpellMgr->GetSpellInfo(i);
        // This spell doesn't exist.
        if (!pTempSpell)
            continue;

        for (uint32 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            // Spell targets self.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF-1);

            // Spell targets a single enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY-1);

            // Spell targets AoE at enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY-1);

            // Spell targets an enemy.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY-1);

            // Spell targets a single friend (or self).
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND-1);

            // Spell targets AoE friends.
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_LASTTARGET_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND-1);

            // Spell targets any friend (or self).
            if (pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_UNIT_LASTTARGET_AREA_PARTY ||
                pTempSpell->Effects[j].TargetA.GetTarget() == TARGET_SRC_CASTER)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND-1);

            // Make sure that this spell includes a damage effect.
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_SCHOOL_DAMAGE ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_INSTAKILL ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE-1);

            // Make sure that this spell includes a healing effect (or an apply aura with a periodic heal).
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                pTempSpell->Effects[j].Effect == SPELL_EFFECT_HEAL_MECHANICAL ||
                (pTempSpell->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA  && pTempSpell->Effects[j].ApplyAuraName == 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING-1);

            // Make sure that this spell applies an aura.
            if (pTempSpell->Effects[j].Effect == SPELL_EFFECT_APPLY_AURA)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA-1);
        }
    }
}

void ScriptMgr::CreateSpellScripts(uint32 spellId, std::list<SpellScript*>& scriptVector)
{
    SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);

    for (SpellScriptsContainer::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellScriptLoader* tmpscript = ScriptRegistry<SpellScriptLoader>::GetScriptById(itr->second);
        if (!tmpscript)
            continue;

        SpellScript* script = tmpscript->GetSpellScript();

        if (!script)
            continue;

        script->_Init(&tmpscript->GetName(), spellId);

        scriptVector.push_back(script);
    }
}

void ScriptMgr::CreateAuraScripts(uint32 spellId, std::list<AuraScript*>& scriptVector)
{
    SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);

    for (SpellScriptsContainer::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellScriptLoader* tmpscript = ScriptRegistry<SpellScriptLoader>::GetScriptById(itr->second);
        if (!tmpscript)
            continue;

        AuraScript* script = tmpscript->GetAuraScript();

        if (!script)
            continue;

        script->_Init(&tmpscript->GetName(), spellId);

        scriptVector.push_back(script);
    }
}

void ScriptMgr::CreateSpellScriptLoaders(uint32 spellId, std::vector<std::pair<SpellScriptLoader*, SpellScriptsContainer::iterator> >& scriptVector)
{
    SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);
    scriptVector.reserve(std::distance(bounds.first, bounds.second));

    for (SpellScriptsContainer::iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellScriptLoader* tmpscript = ScriptRegistry<SpellScriptLoader>::GetScriptById(itr->second);
        if (!tmpscript)
            continue;

        scriptVector.push_back(std::make_pair(tmpscript, itr));
    }
}

void ScriptMgr::OnNetworkStart()
{
    FOREACH_SCRIPT(ServerScript)->OnNetworkStart();
}

void ScriptMgr::OnNetworkStop()
{
    FOREACH_SCRIPT(ServerScript)->OnNetworkStop();
}

void ScriptMgr::OnSocketOpen(std::shared_ptr<WorldSocket> socket)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnSocketOpen(socket);
}

void ScriptMgr::OnSocketClose(std::shared_ptr<WorldSocket> socket)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnSocketClose(socket);
}

void ScriptMgr::OnPacketReceive(WorldSession* session, WorldPacket packet)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnPacketReceive(session, packet);
}

void ScriptMgr::OnPacketSend(WorldSession* session, WorldPacket packet)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnPacketSend(session, packet);
}

void ScriptMgr::OnUnknownPacketReceive(WorldSocket* socket, WorldPacket packet)
{
    ASSERT(socket);

    FOREACH_SCRIPT(ServerScript)->OnUnknownPacketReceive(socket, packet);
}

void ScriptMgr::OnOpenStateChange(bool open)
{
    FOREACH_SCRIPT(WorldScript)->OnOpenStateChange(open);
}

void ScriptMgr::OnConfigLoad(bool reload)
{
    FOREACH_SCRIPT(WorldScript)->OnConfigLoad(reload);
}

void ScriptMgr::OnMotdChange(std::string& newMotd)
{
    FOREACH_SCRIPT(WorldScript)->OnMotdChange(newMotd);
}

void ScriptMgr::OnShutdownInitiate(ShutdownExitCode code, ShutdownMask mask)
{
    FOREACH_SCRIPT(WorldScript)->OnShutdownInitiate(code, mask);
}

void ScriptMgr::OnShutdownCancel()
{
    FOREACH_SCRIPT(WorldScript)->OnShutdownCancel();
}

void ScriptMgr::OnWorldUpdate(uint32 diff)
{
    FOREACH_SCRIPT(WorldScript)->OnUpdate(diff);
}

void ScriptMgr::OnHonorCalculation(float& honor, uint8 level, float multiplier)
{
    FOREACH_SCRIPT(FormulaScript)->OnHonorCalculation(honor, level, multiplier);
}

void ScriptMgr::OnGrayLevelCalculation(uint8& grayLevel, uint8 playerLevel)
{
    FOREACH_SCRIPT(FormulaScript)->OnGrayLevelCalculation(grayLevel, playerLevel);
}

void ScriptMgr::OnColorCodeCalculation(XPColorChar& color, uint8 playerLevel, uint8 mobLevel)
{
    FOREACH_SCRIPT(FormulaScript)->OnColorCodeCalculation(color, playerLevel, mobLevel);
}

void ScriptMgr::OnZeroDifferenceCalculation(uint8& diff, uint8 playerLevel)
{
    FOREACH_SCRIPT(FormulaScript)->OnZeroDifferenceCalculation(diff, playerLevel);
}

void ScriptMgr::OnBaseGainCalculation(uint32& gain, uint8 playerLevel, uint8 mobLevel, ContentLevels content)
{
    FOREACH_SCRIPT(FormulaScript)->OnBaseGainCalculation(gain, playerLevel, mobLevel, content);
}

void ScriptMgr::OnGainCalculation(uint32& gain, Player* player, Unit* unit)
{
    ASSERT(player);
    ASSERT(unit);

    FOREACH_SCRIPT(FormulaScript)->OnGainCalculation(gain, player, unit);
}

void ScriptMgr::OnGroupRateCalculation(float& rate, uint32 count, bool isRaid)
{
    FOREACH_SCRIPT(FormulaScript)->OnGroupRateCalculation(rate, count, isRaid);
}

#define SCR_MAP_BGN(M, V, I, E, C, T) \
    if (V->GetEntry() && V->GetEntry()->T()) \
    { \
        FOR_SCRIPTS(M, I, E) \
        { \
            MapEntry const* C = I->second->GetEntry(); \
            if (!C) \
                continue; \
            if (C->MapID == V->GetId()) \
            {

#define SCR_MAP_END \
                return; \
            } \
        } \
    }

void ScriptMgr::OnCreateMap(Map* map)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnCreate(map);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnCreate((InstanceMap*)map);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnCreate((BattlegroundMap*)map);
    SCR_MAP_END;
}

void ScriptMgr::OnDestroyMap(Map* map)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnDestroy(map);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnDestroy((InstanceMap*)map);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnDestroy((BattlegroundMap*)map);
    SCR_MAP_END;
}

void ScriptMgr::OnLoadGridMap(Map* map, GridMap* gmap, uint32 gx, uint32 gy)
{
    ASSERT(map);
    ASSERT(gmap);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnLoadGridMap(map, gmap, gx, gy);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnLoadGridMap((InstanceMap*)map, gmap, gx, gy);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnLoadGridMap((BattlegroundMap*)map, gmap, gx, gy);
    SCR_MAP_END;
}

void ScriptMgr::OnUnloadGridMap(Map* map, GridMap* gmap, uint32 gx, uint32 gy)
{
    ASSERT(map);
    ASSERT(gmap);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnUnloadGridMap(map, gmap, gx, gy);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnUnloadGridMap((InstanceMap*)map, gmap, gx, gy);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnUnloadGridMap((BattlegroundMap*)map, gmap, gx, gy);
    SCR_MAP_END;
}

void ScriptMgr::OnPlayerEnterMap(Map* map, Player* player)
{
    ASSERT(map);
    ASSERT(player);

    FOREACH_SCRIPT(PlayerScript)->OnMapChanged(player);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnPlayerEnter(map, player);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnPlayerEnter((InstanceMap*)map, player);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnPlayerEnter((BattlegroundMap*)map, player);
    SCR_MAP_END;
}

void ScriptMgr::OnPlayerLeaveMap(Map* map, Player* player)
{
    ASSERT(map);
    ASSERT(player);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnPlayerLeave(map, player);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnPlayerLeave((InstanceMap*)map, player);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnPlayerLeave((BattlegroundMap*)map, player);
    SCR_MAP_END;
}

void ScriptMgr::OnMapUpdate(Map* map, uint32 diff)
{
    ASSERT(map);

    SCR_MAP_BGN(WorldMapScript, map, itr, end, entry, IsWorldMap);
        itr->second->OnUpdate(map, diff);
    SCR_MAP_END;

    SCR_MAP_BGN(InstanceMapScript, map, itr, end, entry, IsDungeon);
        itr->second->OnUpdate((InstanceMap*)map, diff);
    SCR_MAP_END;

    SCR_MAP_BGN(BattlegroundMapScript, map, itr, end, entry, IsBattleground);
        itr->second->OnUpdate((BattlegroundMap*)map, diff);
    SCR_MAP_END;
}

#undef SCR_MAP_BGN
#undef SCR_MAP_END

InstanceScript* ScriptMgr::CreateInstanceData(InstanceMap* map)
{
    ASSERT(map);

    GET_SCRIPT_RET(InstanceMapScript, map->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetInstanceScript(map);
}

bool ScriptMgr::OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Item* target)
{
    ASSERT(caster);
    ASSERT(target);
#ifdef ELUNA
    if (sHookMgr->OnDummyEffect(caster, spellId, effIndex, target))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, target->GetScriptId(), tmpscript, false);
    return tmpscript->OnDummyEffect(caster, spellId, effIndex, target);
}

bool ScriptMgr::OnQuestAccept(Player* player, Item* item, Quest const* quest)
{
    ASSERT(player);
    ASSERT(item);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestAccept(player, item, quest))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, item->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, item, quest);
}

bool ScriptMgr::OnItemUse(Player* player, Item* item, SpellCastTargets const& targets)
{
    ASSERT(player);
    ASSERT(item);
#ifdef ELUNA
    if (sHookMgr->OnUse(player, item, targets))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, item->GetScriptId(), tmpscript, false);
    return tmpscript->OnUse(player, item, targets);
}

bool ScriptMgr::OnItemExpire(Player* player, ItemTemplate const* proto)
{
    ASSERT(player);
    ASSERT(proto);
#ifdef ELUNA
    if (sHookMgr->OnExpire(player, proto))
        return true;
#endif

    GET_SCRIPT_RET(ItemScript, proto->ScriptId, tmpscript, false);
    return tmpscript->OnExpire(player, proto);
}

//bool ScriptMgr::OnItemRemove(Player * player, Item * item)
//{
//    ASSERT(player);
//    ASSERT(item);
//#ifdef ELUNA
//    if (sHookMgr->OnRemove(player, item))
//        return false;
//#endif
//    GET_SCRIPT_RET(ItemScript, item->GetScriptId(), tmpscript, false);
//    return tmpscript->OnRemove(player, item);
//
//}

bool ScriptMgr::OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target)
{
    ASSERT(caster);
    ASSERT(target);
#ifdef ELUNA
    if (sHookMgr->OnDummyEffect(caster, spellId, effIndex, target))
        return true;
#endif

    GET_SCRIPT_RET(CreatureScript, target->GetScriptId(), tmpscript, false);
    return tmpscript->OnDummyEffect(caster, spellId, effIndex, target);
}

bool ScriptMgr::OnItemPickup(Player* player, Item* item, ItemPickupSourceType sourceType, uint32 sourceId)
{
    ASSERT(player);
    ASSERT(item);
//#ifdef ELUNA
//    if (sHookMgr->OnPickup(player, item, sourceType, sourceId))
//        return true;
//#endif

    GET_SCRIPT_RET(ItemScript, item->GetScriptId(), tmpscript, false);
    return tmpscript->OnPickup(player, item, sourceType, sourceId);
}

void ScriptMgr::OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(item);
#ifdef ELUNA
    sHookMgr->HandleGossipSelectOption(player, item, sender, action, "");
#endif
    GET_SCRIPT(ItemScript, item->GetScriptId(), tmpscript);
    tmpscript->OnGossipSelect(player, item, sender, action);
}

void ScriptMgr::OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(item);
#ifdef ELUNA
    sHookMgr->HandleGossipSelectOption(player, item, sender, action, code);
#endif
    GET_SCRIPT(ItemScript, item->GetScriptId(), tmpscript);
    tmpscript->OnGossipSelectCode(player, item, sender, action, code);
}
void ScriptMgr::OnGossipSelect(Player* player, uint32 menu_id, uint32 sender, uint32 action)
{
#ifdef ELUNA
    sHookMgr->HandleGossipSelectOption(player, menu_id, sender, action, "");
#endif
    FOREACH_SCRIPT(PlayerScript)->OnGossipSelect(player, menu_id, sender, action);
}

void ScriptMgr::OnGossipSelectCode(Player* player, uint32 menu_id, uint32 sender, uint32 action, const char* code)
{
#ifdef ELUNA
    sHookMgr->HandleGossipSelectOption(player, menu_id, sender, action, code);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnGossipSelectCode(player, menu_id, sender, action, code);
}

bool ScriptMgr::OnGossipHello(Player* player, Creature* creature)
{
    ASSERT(player);
    ASSERT(creature);
#ifdef ELUNA
    if (sHookMgr->OnGossipHello(player, creature))
        return true;
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipHello(player, creature);
}

bool ScriptMgr::OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(creature);
#ifdef ELUNA
    if (sHookMgr->OnGossipSelect(player, creature, sender, action))
        return true;
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelect(player, creature, sender, action);
}

bool ScriptMgr::OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(code);
#ifdef ELUNA
    if (sHookMgr->OnGossipSelectCode(player, creature, sender, action, code))
        return true;
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelectCode(player, creature, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestAccept(player, creature, quest))
    {
        player->PlayerTalkClass->ClearMenus();
        return true;
    }
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, creature, quest);
}

bool ScriptMgr::OnQuestSelect(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestSelect(player, creature, quest))
    {
        player->PlayerTalkClass->ClearMenus();
        return true;
    }
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestSelect(player, creature, quest);
}

bool ScriptMgr::OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestComplete(player, creature, quest))
    {
        player->PlayerTalkClass->ClearMenus();
        return true;
    }
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestComplete(player, creature, quest);
}

bool ScriptMgr::OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt)
{
    ASSERT(player);
    ASSERT(creature);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestReward(player, creature, quest))
    {
        player->PlayerTalkClass->ClearMenus();
        return true;
    }
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestReward(player, creature, quest, opt);
}

Optional<QuestGiverStatus> ScriptMgr::GetDialogStatus(Player* player, Creature* creature)
{
    ASSERT(player);
    ASSERT(creature);
#ifdef ELUNA
    if (Optional<QuestGiverStatus> status = sHookMgr->GetDialogStatus(player, creature))
    {
        player->PlayerTalkClass->ClearMenus();
        return *status;
    }
#endif

    GET_SCRIPT_RET(CreatureScript, creature->GetScriptId(), tmpscript, std::nullopt);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->GetDialogStatus(player, creature);
}

CreatureAI* ScriptMgr::GetCreatureAI(Creature* creature)
{
    ASSERT(creature);
#ifdef ELUNA
    if (CreatureAI* luaAI = sHookMgr->GetAI(creature))
        return luaAI;
#endif

    GET_SCRIPT_NO_RET(CreatureScript, creature->GetScriptId(), tmpscript);
    GET_SCRIPT_NO_RET(VehicleScript, creature->GetScriptId(), tmpVehiclescript);

    if (tmpscript)
        return tmpscript->GetAI(creature);
    else if (tmpVehiclescript)
        return tmpVehiclescript->GetAI(creature);
    else
        return NULL;
}

GameObjectAI* ScriptMgr::GetGameObjectAI(GameObject* gameobject)
{
    ASSERT(gameobject);
#ifdef ELUNA
    if (GameObjectAI* luaAI = sHookMgr->GetAI(gameobject))
        return luaAI;
#endif

    GET_SCRIPT_RET(GameObjectScript, gameobject->GetScriptId(), tmpscript, NULL);
    return tmpscript->GetAI(gameobject);
}

bool ScriptMgr::OnGossipHello(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);
#ifdef ELUNA
    if (sHookMgr->OnGossipHello(player, go))
        return true;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnGossipHello(player, go);
}

bool ScriptMgr::OnReportUse(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);
//#ifdef ELUNA
//    if (sHookMgr->OnReportUse(player, go))
//        return true;
//#endif
    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnReportUse(player, go);
}

bool ScriptMgr::OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
{
    ASSERT(player);
    ASSERT(go);
#ifdef ELUNA
    if (sHookMgr->OnGossipSelect(player, go, sender, action))
        return true;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelect(player, go, sender, action);
}

bool ScriptMgr::OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(code);
#ifdef ELUNA
    if (sHookMgr->OnGossipSelectCode(player, go, sender, action, code))
        return true;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    return tmpscript->OnGossipSelectCode(player, go, sender, action, code);
}

bool ScriptMgr::OnQuestAccept(Player* player, GameObject* go, Quest const* quest)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestAccept(player, go, quest))
        return true;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestAccept(player, go, quest);
}

bool ScriptMgr::OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt)
{
    ASSERT(player);
    ASSERT(go);
    ASSERT(quest);
#ifdef ELUNA
    if (sHookMgr->OnQuestReward(player, go, quest))
        return true;
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, false);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->OnQuestReward(player, go, quest, opt);
}

Optional<QuestGiverStatus> ScriptMgr::GetDialogStatus(Player* player, GameObject* go)
{
    ASSERT(player);
    ASSERT(go);
#ifdef ELUNA
    if (Optional<QuestGiverStatus> status = sHookMgr->GetDialogStatus(player, go))
    {
        player->PlayerTalkClass->ClearMenus();
        return *status;
    }
#endif

    GET_SCRIPT_RET(GameObjectScript, go->GetScriptId(), tmpscript, std::nullopt);
    player->PlayerTalkClass->ClearMenus();
    return tmpscript->GetDialogStatus(player, go);
}

void ScriptMgr::OnGameObjectDestroyed(GameObject* go, Player* player)
{
    ASSERT(go);
#ifdef ELUNA
    sHookMgr->OnDestroyed(go, player);
#endif

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnDestroyed(go, player);
}

void ScriptMgr::OnGameObjectDamaged(GameObject* go, Player* player)
{
    ASSERT(go);
#ifdef ELUNA
    sHookMgr->OnDamaged(go, player);
#endif

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnDamaged(go, player);
}

void ScriptMgr::OnGameObjectLootStateChanged(GameObject* go, uint32 state, Unit* unit)
{
    ASSERT(go);
#ifdef ELUNA
    sHookMgr->OnGameObjectStateChanged(go, state);
#endif

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnLootStateChanged(go, state, unit);
}

void ScriptMgr::OnGameObjectStateChanged(GameObject* go, uint32 state)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnGameObjectStateChanged(go, state);
}

void ScriptMgr::OnGameObjectUpdate(GameObject* go, uint32 diff)
{
    ASSERT(go);

    GET_SCRIPT(GameObjectScript, go->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(go, diff);
}

bool ScriptMgr::OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, GameObject* target)
{
    ASSERT(caster);
    ASSERT(target);

#ifdef ELUNA
    if (sHookMgr->OnDummyEffect(caster, spellId, effIndex, target))
        return true;
#endif
    GET_SCRIPT_RET(GameObjectScript, target->GetScriptId(), tmpscript, false);
    return tmpscript->OnDummyEffect(caster, spellId, effIndex, target);
}

bool ScriptMgr::OnAreaTrigger(Player* player, AreaTriggerEntry const* trigger, bool entered)
{
    ASSERT(player);
    ASSERT(trigger);
#ifdef ELUNA
    if (sHookMgr->OnAreaTrigger(player, trigger, entered))
        return true;
#endif

    GET_SCRIPT_RET(AreaTriggerScript, sObjectMgr->GetAreaTriggerScriptId(trigger->ID), tmpscript, false);
    return entered ? tmpscript->OnTrigger(player, trigger) : tmpscript->OnExit(player, trigger);
}

IAreaTrigger* ScriptMgr::CreateAreaTriggerInterface(uint32 entry)
{
    AreaTriggerTemplate const* atTemplate = sObjectMgr->GetAreaTriggerTemplate(entry);
    if (!atTemplate)
        return NULL;

    GET_SCRIPT_RET(SpellAreaTriggerScript, atTemplate->ScriptId, tmpscript, NULL);
    IAreaTrigger* result = tmpscript->GetInterface();
    result->m_range = atTemplate->Radius;
    return result;
}

Battleground* ScriptMgr::CreateBattleground(BattlegroundTypeId /*typeId*/)
{
    /// @todo Implement script-side battlegrounds.
    ASSERT(false);
    return NULL;
}

OutdoorPvP* ScriptMgr::CreateOutdoorPvP(OutdoorPvPData const* data)
{
    ASSERT(data);

    GET_SCRIPT_RET(OutdoorPvPScript, data->ScriptId, tmpscript, NULL);
    return tmpscript->GetOutdoorPvP();
}

std::vector<ChatCommand> ScriptMgr::GetChatCommands()
{
    std::vector<ChatCommand> table;

    FOR_SCRIPTS_RET(CommandScript, itr, end, table)
    {
        std::vector<ChatCommand> cmds = itr->second->GetCommands();
        //table.insert(table.end(), cmds.begin(), cmds.end());
        std::move(cmds.begin(), cmds.end(), std::back_inserter(table));
    }

    // FOR_SCRIPTS(CommandScript, itr, end)
    // {
    //     std::vector<ChatCommand> cmds = itr->second->GetCommands();
    //     std::move(cmds.begin(), cmds.end(), std::back_inserter(table));
    // }

    return table;
}

void ScriptMgr::OnWeatherChange(Weather* weather, WeatherState state, float grade)
{
    ASSERT(weather);
#ifdef ELUNA
    sHookMgr->OnChange(weather, state, grade);
#endif

    GET_SCRIPT(WeatherScript, weather->GetScriptId(), tmpscript);
    tmpscript->OnChange(weather, state, grade);
}

void ScriptMgr::OnWeatherUpdate(Weather* weather, uint32 diff)
{
    ASSERT(weather);

    GET_SCRIPT(WeatherScript, weather->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(weather, diff);
}

void ScriptMgr::OnAuctionAdd(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);
#ifdef ELUNA
    sHookMgr->OnAdd(ah);
#endif

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionAdd(ah, entry);
}

void ScriptMgr::OnAuctionRemove(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);
#ifdef ELUNA
    sHookMgr->OnRemove(ah);
#endif

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionRemove(ah, entry);
}

void ScriptMgr::OnAuctionSuccessful(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);
#ifdef ELUNA
    sHookMgr->OnSuccessful(ah);
#endif

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionSuccessful(ah, entry);
}

void ScriptMgr::OnAuctionExpire(AuctionHouseObject* ah, AuctionEntry* entry)
{
    ASSERT(ah);
    ASSERT(entry);
#ifdef ELUNA
    sHookMgr->OnExpire(ah);
#endif

    FOREACH_SCRIPT(AuctionHouseScript)->OnAuctionExpire(ah, entry);
}

bool ScriptMgr::OnConditionCheck(const Condition* condition, ConditionSourceInfo& sourceInfo)
{
    ASSERT(condition);
#ifdef ELUNA
    if (sHookMgr->OnConditionCheck(condition, sourceInfo))
        return true;
#endif

    GET_SCRIPT_RET(ConditionScript, condition->ScriptId, tmpscript, true);
    return tmpscript->OnConditionCheck(condition, sourceInfo);
}

void ScriptMgr::OnInstall(Vehicle* veh)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
#ifdef ELUNA
    sHookMgr->OnInstall(veh);
#endif

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnInstall(veh);
}

void ScriptMgr::OnUninstall(Vehicle* veh)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
#ifdef ELUNA
    sHookMgr->OnUninstall(veh);
#endif

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnUninstall(veh);
}

void ScriptMgr::OnReset(Vehicle* veh)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
#ifdef ELUNA
    sHookMgr->OnReset(veh);
#endif

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnReset(veh);
}

void ScriptMgr::OnInstallAccessory(Vehicle* veh, Creature* accessory)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
    ASSERT(accessory);
#ifdef ELUNA
    sHookMgr->OnInstallAccessory(veh, accessory);
#endif

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnInstallAccessory(veh, accessory);
}

void ScriptMgr::OnAddPassenger(Vehicle* veh, Unit* passenger, int8 seatId)
{
    ASSERT(veh);	
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
    ASSERT(passenger);
#ifdef ELUNA
    sHookMgr->OnAddPassenger(veh, passenger, seatId);
#endif

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnAddPassenger(veh, passenger, seatId);	
}

void ScriptMgr:: BeforeAddPassenger(Vehicle* veh, Unit* passenger){
	ASSERT(veh);
	ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
	ASSERT(passenger);


	GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
	tmpscript->BeforeAddPassenger(veh, passenger);
}

void ScriptMgr::OnRemovePassenger(Vehicle* veh, Unit* passenger)
{
    ASSERT(veh);
    ASSERT(veh->GetBase()->GetTypeId() == TYPEID_UNIT);
    ASSERT(passenger);
#ifdef ELUNA
    sHookMgr->OnRemovePassenger(veh, passenger);
#endif

    GET_SCRIPT(VehicleScript, veh->GetBase()->ToCreature()->GetScriptId(), tmpscript);
    tmpscript->OnRemovePassenger(veh, passenger);
}

void ScriptMgr::OnDynamicObjectUpdate(DynamicObject* dynobj, uint32 diff)
{
    ASSERT(dynobj);

    FOR_SCRIPTS(DynamicObjectScript, itr, end)
        itr->second->OnUpdate(dynobj, diff);
}

void ScriptMgr::OnTransportCreate(Transport* transport)
{
    ASSERT(transport);
//#ifdef ELUNA
//    sHookMgr->OnTransportCreate(transport);
//#endif

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnCreate(transport);
}

void ScriptMgr::OnAddPassenger(Transport* transport, Player* player)
{
    ASSERT(transport);
    ASSERT(player);
#ifdef ELUNA
    sHookMgr->OnAddPassenger(transport, player);
#endif

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnAddPassenger(transport, player);
}

void ScriptMgr::OnAddCreaturePassenger(Transport* transport, Creature* creature)
{
    ASSERT(transport);
    ASSERT(creature);
#ifdef ELUNA
    sHookMgr->OnAddCreaturePassenger(transport, creature);
#endif

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnAddCreaturePassenger(transport, creature);
}

void ScriptMgr::OnRemovePassenger(Transport* transport, Player* player)
{
    ASSERT(transport);
    ASSERT(player);
#ifdef ELUNA
    sHookMgr->OnRemovePassenger(transport, player);
#endif

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnRemovePassenger(transport, player);
}

void ScriptMgr::OnTransportUpdate(Transport* transport, uint32 diff)
{
    ASSERT(transport);

    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnUpdate(transport, diff);
}

void ScriptMgr::OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
{
#ifdef ELUNA
    sHookMgr->OnRelocate(transport, waypointId, mapId, x, y, z);
#endif
    GET_SCRIPT(TransportScript, transport->GetScriptId(), tmpscript);
    tmpscript->OnRelocate(transport, waypointId, mapId, x, y, z);
}

void ScriptMgr::OnStartup()
{
    FOREACH_SCRIPT(WorldScript)->OnStartup();
}

void ScriptMgr::OnShutdown()
{
    FOREACH_SCRIPT(WorldScript)->OnShutdown();
}

bool ScriptMgr::OnCriteriaCheck(uint32 scriptId, Player* source, Unit* target)
{
    ASSERT(source);
    // target can be NULL.

    GET_SCRIPT_RET(AchievementCriteriaScript, scriptId, tmpscript, false);
    return tmpscript->OnCheck(source, target);
}

// Player
void ScriptMgr::OnPVPKill(Player* killer, Player* killed)
{
#ifdef ELUNA
    sHookMgr->OnPVPKill(killer, killed);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnPVPKill(killer, killed);
}

void ScriptMgr::OnCreatureKill(Player* killer, Creature* killed)
{
#ifdef ELUNA
    sHookMgr->OnCreatureKill(killer, killed);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnCreatureKill(killer, killed);
}

void ScriptMgr::OnPlayerKilledByCreature(Creature* killer, Player* killed)
{
#ifdef ELUNA
    sHookMgr->OnPlayerKilledByCreature(killer, killed);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnPlayerKilledByCreature(killer, killed);
}

void ScriptMgr::OnPlayerLevelChanged(Player* player, uint8 oldLevel)
{
#ifdef ELUNA
    sHookMgr->OnLevelChanged(player, oldLevel);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnLevelChanged(player, oldLevel);
}

void ScriptMgr::OnPlayerEquipChanged(Player* player, uint32 item)
{
    FOREACH_SCRIPT(PlayerScript)->OnEquipChanged(player, item);
}

void ScriptMgr::OnPlayerFreeTalentPointsChanged(Player* player, uint32 points)
{
#ifdef ELUNA
    sHookMgr->OnFreeTalentPointsChanged(player, points);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnFreeTalentPointsChanged(player, points);
}

void ScriptMgr::OnPlayerTalentsReset(Player* player, bool noCost)
{
#ifdef ELUNA
    sHookMgr->OnTalentsReset(player, noCost);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnTalentsReset(player, noCost);
}

void ScriptMgr::OnPlayerMoneyChanged(Player* player, int64& amount)
{
#ifdef ELUNA
    sHookMgr->OnMoneyChanged(player, amount);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnMoneyChanged(player, amount);
}

void ScriptMgr::OnGivePlayerXP(Player* player, uint32& amount, Unit* victim)
{
#ifdef ELUNA
    sHookMgr->OnGiveXP(player, amount, victim);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnGiveXP(player, amount, victim);
}

void ScriptMgr::OnPlayerReputationChange(Player* player, uint32 factionID, float& standing, bool incremental)
{
//#ifdef ELUNA
//    sHookMgr->OnReputationChange(player, factionID, standing, incremental);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnReputationChange(player, factionID, standing, incremental);
}

void ScriptMgr::OnPlayerQuestAdded(Player* player, const Quest* quest)
{
//#ifdef ELUNA
//    sHookMgr->OnQuestAdded(player, quest);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnQuestAdded(player, quest);
}

void ScriptMgr::OnPlayerQuestAbandoned(Player* player, const Quest* quest)
{
//#ifdef ELUNA
//    sHookMgr->OnQuestAbandoned(player, quest);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnQuestAbandoned(player, quest);
}

void ScriptMgr::OnPlayerQuestCompleted(Player* player, const Quest* quest)
{
//#ifdef ELUNA
//    sHookMgr->OnQuestCompleted(player, quest);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnQuestCompleted(player, quest);
}

void ScriptMgr::OnPlayerQuestFailed(Player* player, const Quest* quest)
{
//#ifdef ELUNA
//    sHookMgr->OnQuestFailed(player, quest);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnQuestFailed(player, quest);
}

void ScriptMgr::OnPlayerQuestRewarded(Player* player, const Quest* quest)
{
//#ifdef ELUNA
//    sHookMgr->OnQuestRewarded(player, quest);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnQuestRewarded(player, quest);
}

void ScriptMgr::OnPlayerDuelRequest(Player* target, Player* challenger)
{
#ifdef ELUNA
    sHookMgr->OnDuelRequest(target, challenger);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnDuelRequest(target, challenger);
}

void ScriptMgr::OnPlayerDuelStart(Player* player1, Player* player2)
{
#ifdef ELUNA
    sHookMgr->OnDuelStart(player1, player2);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnDuelStart(player1, player2);
}

void ScriptMgr::OnPlayerDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
{
#ifdef ELUNA
    sHookMgr->OnDuelEnd(winner, loser, type);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnDuelEnd(winner, loser, type);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, receiver);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, group);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, guild);
}

void ScriptMgr::OnPlayerChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
{
    FOREACH_SCRIPT(PlayerScript)->OnChat(player, type, lang, msg, channel);
}

void ScriptMgr::OnPlayerEmote(Player* player, uint32 emote)
{
#ifdef ELUNA
    sHookMgr->OnEmote(player, emote);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnEmote(player, emote);
}

void ScriptMgr::OnPlayerTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid)
{
#ifdef ELUNA
    sHookMgr->OnTextEmote(player, textEmote, emoteNum, guid);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnTextEmote(player, textEmote, emoteNum, guid);
}

void ScriptMgr::OnPlayerSpellCast(Player* player, Spell* spell, bool skipCheck)
{
#ifdef ELUNA
    sHookMgr->OnSpellCast(player, spell, skipCheck);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnSpellCast(player, spell, skipCheck);
}

void ScriptMgr::OnPlayerLogin(Player* player)
{
#ifdef ELUNA
    sHookMgr->OnLogin(player);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnLogin(player);
}

void ScriptMgr::OnPlayerLogout(Player* player)
{
#ifdef ELUNA
    sHookMgr->OnLogout(player);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnLogout(player);
}

void ScriptMgr::OnPlayerCreate(Player* player)
{
#ifdef ELUNA
    sHookMgr->OnCreate(player);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnCreate(player);
}

void ScriptMgr::OnPlayerDelete(ObjectGuid guid)
{
#ifdef ELUNA
    sHookMgr->OnDelete(guid.GetCounter());
#endif
    FOREACH_SCRIPT(PlayerScript)->OnDelete(guid);
}

void ScriptMgr::OnPlayerSave(Player* player)
{
//#ifdef ELUNA
//    sHookMgr->OnPlayerSave(Player* player);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnSave(player);
}

void ScriptMgr::OnPlayerBindToInstance(Player* player, Difficulty difficulty, uint32 mapid, bool permanent)
{
#ifdef ELUNA
    sHookMgr->OnBindToInstance(player, difficulty, mapid, permanent);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnBindToInstance(player, difficulty, mapid, permanent);
}

void ScriptMgr::OnUpdate(Player* player, uint32 diff)
{
//#ifdef ELUNA
//    sHookMgr->OnUpdate(player, diff);
//#endif
    FOREACH_SCRIPT(PlayerScript)->OnUpdate(player, diff);
}

void ScriptMgr::OnPlayerUpdateZone(Player* player, uint32 newZone, uint32 newArea)
{
#ifdef ELUNA
    sHookMgr->OnUpdateZone(player, newZone, newArea);
#endif
    FOREACH_SCRIPT(PlayerScript)->OnUpdateZone(player, newZone, newArea);
}


// Guild
void ScriptMgr::OnGuildAddMember(Guild* guild, Player* player, uint8& plRank)
{
#ifdef ELUNA
    sHookMgr->OnAddMember(guild, player, plRank);
#endif
    FOREACH_SCRIPT(GuildScript)->OnAddMember(guild, player, plRank);
}

void ScriptMgr::OnGuildRemoveMember(Guild* guild, Player* player, bool isDisbanding, bool isKicked)
{
#ifdef ELUNA
    sHookMgr->OnRemoveMember(guild, player, isDisbanding, isKicked);
#endif
    FOREACH_SCRIPT(GuildScript)->OnRemoveMember(guild, player, isDisbanding, isKicked);
}

void ScriptMgr::OnGuildMOTDChanged(Guild* guild, const std::string& newMotd)
{
#ifdef ELUNA
    sHookMgr->OnMOTDChanged(guild, newMotd);
#endif
    FOREACH_SCRIPT(GuildScript)->OnMOTDChanged(guild, newMotd);
}

void ScriptMgr::OnGuildInfoChanged(Guild* guild, const std::string& newInfo)
{
#ifdef ELUNA
    sHookMgr->OnInfoChanged(guild, newInfo);
#endif
    FOREACH_SCRIPT(GuildScript)->OnInfoChanged(guild, newInfo);
}

void ScriptMgr::OnGuildCreate(Guild* guild, Player* leader, const std::string& name)
{
#ifdef ELUNA
    sHookMgr->OnCreate(guild, leader, name);
#endif
    FOREACH_SCRIPT(GuildScript)->OnCreate(guild, leader, name);
}

void ScriptMgr::OnGuildDisband(Guild* guild)
{
#ifdef ELUNA
    sHookMgr->OnDisband(guild);
#endif
    FOREACH_SCRIPT(GuildScript)->OnDisband(guild);
}

void ScriptMgr::OnGuildMemberWitdrawMoney(Guild* guild, Player* player, uint64 &amount, bool isRepair)
{
#ifdef ELUNA
    sHookMgr->OnMemberWitdrawMoney(guild, player, amount, isRepair);
#endif
    FOREACH_SCRIPT(GuildScript)->OnMemberWitdrawMoney(guild, player, amount, isRepair);
}

void ScriptMgr::OnGuildMemberDepositMoney(Guild* guild, Player* player, uint64 &amount)
{
#ifdef ELUNA
    sHookMgr->OnMemberDepositMoney(guild, player, amount);
#endif
    FOREACH_SCRIPT(GuildScript)->OnMemberDepositMoney(guild, player, amount);
}

void ScriptMgr::OnGuildItemMove(Guild* guild, Player* player, Item* pItem, bool isSrcBank, uint8 srcContainer, uint8 srcSlotId,
            bool isDestBank, uint8 destContainer, uint8 destSlotId)
{
#ifdef ELUNA
    sHookMgr->OnItemMove(guild, player, pItem, isSrcBank, srcContainer, srcSlotId, isDestBank, destContainer, destSlotId);
#endif
    FOREACH_SCRIPT(GuildScript)->OnItemMove(guild, player, pItem, isSrcBank, srcContainer, srcSlotId, isDestBank, destContainer, destSlotId);
}

void ScriptMgr::OnGuildEvent(Guild* guild, uint8 eventType, uint32 playerGuid1, uint32 playerGuid2, uint8 newRank)
{
#ifdef ELUNA
    sHookMgr->OnEvent(guild, eventType, playerGuid1, playerGuid2, newRank);
#endif
    FOREACH_SCRIPT(GuildScript)->OnEvent(guild, eventType, playerGuid1, playerGuid2, newRank);
}

void ScriptMgr::OnGuildBankEvent(Guild* guild, uint8 eventType, uint8 tabId, uint32 playerGuid, uint32 itemOrMoney, uint16 itemStackCount, uint8 destTabId)
{
#ifdef ELUNA
    sHookMgr->OnBankEvent(guild, eventType, tabId, playerGuid, itemOrMoney, itemStackCount, destTabId);
#endif
    FOREACH_SCRIPT(GuildScript)->OnBankEvent(guild, eventType, tabId, playerGuid, itemOrMoney, itemStackCount, destTabId);
}

// Group
void ScriptMgr::OnGroupAddMember(Group* group, ObjectGuid guid)
{
    ASSERT(group);
#ifdef ELUNA
    sHookMgr->OnAddMember(group, guid);
#endif
    FOREACH_SCRIPT(GroupScript)->OnAddMember(group, guid);
}

void ScriptMgr::OnGroupInviteMember(Group* group, ObjectGuid guid)
{
    ASSERT(group);
#ifdef ELUNA
    sHookMgr->OnInviteMember(group, guid);
#endif
    FOREACH_SCRIPT(GroupScript)->OnInviteMember(group, guid);
}

void ScriptMgr::OnGroupRemoveMember(Group* group, ObjectGuid guid, RemoveMethod method, ObjectGuid kicker, const char* reason)
{
    ASSERT(group);
#ifdef ELUNA
    sHookMgr->OnRemoveMember(group, guid, method, kicker, reason);
#endif
    FOREACH_SCRIPT(GroupScript)->OnRemoveMember(group, guid, method, kicker, reason);
}

void ScriptMgr::OnGroupChangeLeader(Group* group, ObjectGuid newLeaderGuid, ObjectGuid oldLeaderGuid)
{
    ASSERT(group);
#ifdef ELUNA
    sHookMgr->OnChangeLeader(group, newLeaderGuid, oldLeaderGuid);
#endif
    FOREACH_SCRIPT(GroupScript)->OnChangeLeader(group, newLeaderGuid, oldLeaderGuid);
}

void ScriptMgr::OnGroupDisband(Group* group)
{
    ASSERT(group);
#ifdef ELUNA
    sHookMgr->OnDisband(group);
#endif
    FOREACH_SCRIPT(GroupScript)->OnDisband(group);
}

// GameEvent
bool ScriptMgr::CanGameEventStart(GameEventData const& event, bool shouldStart)
{
    GET_SCRIPT_RET(GameEventScript, event.ScriptID, tmpscript, shouldStart);
    return tmpscript->CanStart(shouldStart);
}

void ScriptMgr::OnGameEventStart(GameEventData const& event)
{
    GET_SCRIPT(GameEventScript, event.ScriptID, tmpscript);
    tmpscript->OnStart();
}

void ScriptMgr::OnGameEventStarted(GameEventData const& event)
{
    GET_SCRIPT(GameEventScript, event.ScriptID, tmpscript);
    tmpscript->OnStarted();
}

void ScriptMgr::OnGameEventStop(GameEventData const& event)
{
    GET_SCRIPT(GameEventScript, event.ScriptID, tmpscript);
    tmpscript->OnStop();
}

void ScriptMgr::OnGameEventStopped(GameEventData const& event)
{
    GET_SCRIPT(GameEventScript, event.ScriptID, tmpscript);
    tmpscript->OnStopped();
}

uint32 ScriptMgr::GetGameEventUpdateInterval(GameEventData const& event)
{
    GET_SCRIPT_RET(GameEventScript, event.ScriptID, tmpscript, 0);
    return tmpscript->GetUpdateInterval();
}

void ScriptMgr::OnGameEventUpdate(GameEventData const& event, uint32 diff)
{
    GET_SCRIPT(GameEventScript, event.ScriptID, tmpscript);
    tmpscript->OnUpdate(diff);
}

void ScriptMgr::FillGameEventWorldStates(GameEventData const& event, Player* player, WorldStateBuilder& builder)
{
    GET_SCRIPT(GameEventScript, event.ScriptID, tmpscript);
    tmpscript->FillWorldStates(player, builder);
}

void ScriptMgr::OnGlobalItemDelFromDB(CharacterDatabaseTransaction trans, ObjectGuid itemGuid)
{
    ASSERT(trans);
    ASSERT(itemGuid);

    FOREACH_SCRIPT(GlobalScript)->OnItemDelFromDB(trans, itemGuid);
}

void ScriptMgr::OnGlobalMirrorImageDisplayItem(const Item *item, uint32 &display)
{
    FOREACH_SCRIPT(GlobalScript)->OnMirrorImageDisplayItem(item, display);
}

// Unit
void ScriptMgr::OnHeal(Unit* healer, Unit* reciever, uint32& gain)
{
    FOREACH_SCRIPT(UnitScript)->OnHeal(healer, reciever, gain);
}

void ScriptMgr::OnDamage(Unit* attacker, Unit* victim, uint32& damage)
{
    FOREACH_SCRIPT(UnitScript)->OnDamage(attacker, victim, damage);
}

void ScriptMgr::ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, int32& damage)
{
    FOREACH_SCRIPT(UnitScript)->ModifyPeriodicDamageAurasTick(target, attacker, damage);
}

void ScriptMgr::ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage)
{
    FOREACH_SCRIPT(UnitScript)->ModifyMeleeDamage(target, attacker, damage);
}

void ScriptMgr::ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage)
{
    FOREACH_SCRIPT(UnitScript)->ModifySpellDamageTaken(target, attacker, damage);
}

// Scene
void ScriptMgr::OnSceneStart(Player* player, uint32 sceneInstanceID, SceneTemplate const* sceneTemplate)
{
    ASSERT(player);
    ASSERT(sceneTemplate);

    GET_SCRIPT(SceneScript, sceneTemplate->ScriptId, tmpscript);
    tmpscript->OnSceneStart(player, sceneInstanceID, sceneTemplate);
}

void ScriptMgr::OnSceneTrigger(Player* player, uint32 sceneInstanceID, SceneTemplate const* sceneTemplate, std::string const& triggerName)
{
    ASSERT(player);
    ASSERT(sceneTemplate);

    GET_SCRIPT(SceneScript, sceneTemplate->ScriptId, tmpscript);
    tmpscript->OnSceneTriggerEvent(player, sceneInstanceID, sceneTemplate, triggerName);
}

void ScriptMgr::OnSceneCancel(Player* player, uint32 sceneInstanceID, SceneTemplate const* sceneTemplate)
{
    ASSERT(player);
    ASSERT(sceneTemplate);

    GET_SCRIPT(SceneScript, sceneTemplate->ScriptId, tmpscript);
    tmpscript->OnSceneCancel(player, sceneInstanceID, sceneTemplate);
}

void ScriptMgr::OnSceneComplete(Player* player, uint32 sceneInstanceID, SceneTemplate const* sceneTemplate)
{
    ASSERT(player);
    ASSERT(sceneTemplate);

    GET_SCRIPT(SceneScript, sceneTemplate->ScriptId, tmpscript);
    tmpscript->OnSceneComplete(player, sceneInstanceID, sceneTemplate);
}

void ScriptMgr::OnQuestStatusChange(Player* player, Quest const* quest, QuestStatus oldStatus, QuestStatus newStatus)
{
    ASSERT(player);
    ASSERT(quest);

    GET_SCRIPT(QuestScript, quest->GetScriptId(), tmpscript);
    tmpscript->OnQuestStatusChange(player, quest, oldStatus, newStatus);
}

void ScriptMgr::OnQuestObjectiveChange(Player* player, Quest const* quest, QuestObjective const* objective, int32 oldAmount, int32 newAmount)
{
    ASSERT(player);
    ASSERT(quest);

    GET_SCRIPT(QuestScript, quest->GetScriptId(), tmpscript);
    tmpscript->OnQuestObjectiveChange(player, quest, objective, oldAmount, newAmount);
}

bool ScriptMgr::CanHavePetAI(Creature* creature)
{
    return creature->GetCharmInfo() != nullptr;
}

SpellScriptLoader::SpellScriptLoader(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<SpellScriptLoader>::Instance()->AddScript(this);
}

ServerScript::ServerScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<ServerScript>::Instance()->AddScript(this);
}

WorldScript::WorldScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<WorldScript>::Instance()->AddScript(this);
}

FormulaScript::FormulaScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<FormulaScript>::Instance()->AddScript(this);
}

UnitScript::UnitScript(const char* name, bool addToScripts)
    : ScriptObject(name)
{
    if (addToScripts)
        ScriptRegistry<UnitScript>::Instance()->AddScript(this);
}

WorldMapScript::WorldMapScript(const char* name, uint32 mapId)
    : ScriptObject(name), MapScript<Map>(mapId)
{
    if (GetEntry() && !GetEntry()->IsWorldMap())
        TC_LOG_ERROR("scripts", "WorldMapScript for map %u is invalid.", mapId);

    ScriptRegistry<WorldMapScript>::Instance()->AddScript(this);
}

InstanceMapScript::InstanceMapScript(const char* name, uint32 mapId)
    : ScriptObject(name), MapScript<InstanceMap>(mapId)
{
    if (GetEntry() && !GetEntry()->IsDungeon() && !GetEntry()->IsScenario())
        TC_LOG_ERROR("scripts", "InstanceMapScript for map %u is invalid.", mapId);

    ScriptRegistry<InstanceMapScript>::Instance()->AddScript(this);
}

BattlegroundMapScript::BattlegroundMapScript(const char* name, uint32 mapId)
    : ScriptObject(name), MapScript<BattlegroundMap>(mapId)
{
    if (GetEntry() && !GetEntry()->IsBattleground())
        TC_LOG_ERROR("scripts", "BattlegroundMapScript for map %u is invalid.", mapId);

    ScriptRegistry<BattlegroundMapScript>::Instance()->AddScript(this);
}

ItemScript::ItemScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<ItemScript>::Instance()->AddScript(this);
}

CreatureScript::CreatureScript(const char* name)
    : ScriptObject(name)
    //: UnitScript(name, false)
{
    ScriptRegistry<CreatureScript>::Instance()->AddScript(this);
}

GameObjectScript::GameObjectScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<GameObjectScript>::Instance()->AddScript(this);
}

AreaTriggerScript::AreaTriggerScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<AreaTriggerScript>::Instance()->AddScript(this);
}

SpellAreaTriggerScript::SpellAreaTriggerScript(const char* name)
: ScriptObject(name)
{
    ScriptRegistry<SpellAreaTriggerScript>::Instance()->AddScript(this);
}

BattlegroundScript::BattlegroundScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<BattlegroundScript>::Instance()->AddScript(this);
}

OutdoorPvPScript::OutdoorPvPScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<OutdoorPvPScript>::Instance()->AddScript(this);
}

CommandScript::CommandScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<CommandScript>::Instance()->AddScript(this);
}

WeatherScript::WeatherScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<WeatherScript>::Instance()->AddScript(this);
}

AuctionHouseScript::AuctionHouseScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<AuctionHouseScript>::Instance()->AddScript(this);
}

ConditionScript::ConditionScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<ConditionScript>::Instance()->AddScript(this);
}

VehicleScript::VehicleScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<VehicleScript>::Instance()->AddScript(this);
}
DynamicObjectScript::DynamicObjectScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<DynamicObjectScript>::Instance()->AddScript(this);
}

TransportScript::TransportScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<TransportScript>::Instance()->AddScript(this);
}

AchievementCriteriaScript::AchievementCriteriaScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<AchievementCriteriaScript>::Instance()->AddScript(this);
}

PlayerScript::PlayerScript(const char* name)
    : UnitScript(name, false)
{
    ScriptRegistry<PlayerScript>::Instance()->AddScript(this);
}

GuildScript::GuildScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<GuildScript>::Instance()->AddScript(this);
}

GroupScript::GroupScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<GroupScript>::Instance()->AddScript(this);
}

SceneScript::SceneScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<SceneScript>::Instance()->AddScript(this);
}

QuestScript::QuestScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<QuestScript>::Instance()->AddScript(this);
}

GameEventScript::GameEventScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<GameEventScript>::Instance()->AddScript(this);
}

GlobalScript::GlobalScript(const char* name)
    : ScriptObject(name)
{
    ScriptRegistry<GlobalScript>::Instance()->AddScript(this);
}

// Instantiate static members of ScriptRegistry.
template<class TScript> std::map<uint32, TScript*> ScriptRegistry<TScript>::ScriptPointerList;
template<class TScript> uint32 ScriptRegistry<TScript>::_scriptIdCounter = 100000000;

// Specialize for each script type class like so:
template class ScriptRegistry<SpellScriptLoader>;
template class ScriptRegistry<ServerScript>;
template class ScriptRegistry<WorldScript>;
template class ScriptRegistry<FormulaScript>;
template class ScriptRegistry<WorldMapScript>;
template class ScriptRegistry<InstanceMapScript>;
template class ScriptRegistry<BattlegroundMapScript>;
template class ScriptRegistry<ItemScript>;
template class ScriptRegistry<CreatureScript>;
template class ScriptRegistry<GameObjectScript>;
template class ScriptRegistry<AreaTriggerScript>;
template class ScriptRegistry<BattlegroundScript>;
template class ScriptRegistry<OutdoorPvPScript>;
template class ScriptRegistry<CommandScript>;
template class ScriptRegistry<WeatherScript>;
template class ScriptRegistry<AuctionHouseScript>;
template class ScriptRegistry<ConditionScript>;
template class ScriptRegistry<VehicleScript>;
template class ScriptRegistry<DynamicObjectScript>;
template class ScriptRegistry<TransportScript>;
template class ScriptRegistry<AchievementCriteriaScript>;
template class ScriptRegistry<PlayerScript>;
template class ScriptRegistry<GuildScript>;
template class ScriptRegistry<GroupScript>;
template class ScriptRegistry<GameEventScript>;
template class ScriptRegistry<UnitScript>;
template class ScriptRegistry<SceneScript>;
template class ScriptRegistry<QuestScript>;

// Undefine utility macros.
#undef GET_SCRIPT_RET
#undef GET_SCRIPT
#undef FOREACH_SCRIPT
#undef FOR_SCRIPTS_RET
#undef FOR_SCRIPTS
#undef SCR_REG_LST
#undef SCR_REG_ITR
#undef SCR_REG_MAP
