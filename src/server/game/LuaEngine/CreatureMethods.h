/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef LUA_CREATUREMETHODS_H
#define LUA_CREATUREMETHODS_H

struct lua_State;
class Creature;

namespace LuaCreature
{
    int IsDamageEnoughForLootingAndReward(lua_State* L, Creature* creature);
    int IsReputationGainDisabled(lua_State* L, Creature* creature);
    int SetDisableReputationGain(lua_State* L, Creature* creature);
    int GetCurrentWaypointId(lua_State* L, Creature* creature);
    int GetWaypointPath(lua_State* L, Creature* creature);
    int GetTransportHomePosition(lua_State* L, Creature* creature);
    int IsRegeneratingHealth(lua_State* L, Creature* creature);
    int HasInvolvedQuest(lua_State* L, Creature* creature);
    int SetInCombatWithZone(lua_State* L, Creature* creature);
    int SetRespawnRadius(lua_State* L, Creature* creature);
    int GetRespawnRadius(lua_State* L, Creature* creature);
    int Respawn(lua_State* L, Creature* creature);
    int SetRespawnDelay(lua_State* L, Creature* creature);
    int GetRespawnDelay(lua_State* L, Creature* creature);
    int DespawnOrUnsummon(lua_State* L, Creature* creature);
    int RemoveCorpse(lua_State* L, Creature* creature);
    int SetDefaultMovementType(lua_State* L, Creature* creature);
    int GetDefaultMovementType(lua_State* L, Creature* creature);
    int IsTargetAcceptable(lua_State* L, Creature* creature);
    int CanAssistTo(lua_State* L, Creature* creature);
    int HasSearchedAssistance(lua_State* L, Creature* creature);
    int SetNoSearchAssistance(lua_State* L, Creature* creature);
    int SetNoCallAssistance(lua_State* L, Creature* creature);
    int CallAssistance(lua_State* L, Creature* creature);
    int CallForHelp(lua_State* L, Creature* creature);
    int FleeToGetAssistance(lua_State* L, Creature* creature);
    int GetAggroRange(lua_State* L, Creature* creature);
    int GetAttackDistance(lua_State* L, Creature* creature);
    int CanStartAttack(lua_State* L, Creature* creature); // TODO: Implement core side
    int ResetLootMode(lua_State* L, Creature* creature); // TODO: Implement LootMode features
    int RemoveLootMode(lua_State* L, Creature* creature); // TODO: Implement LootMode features
    int AddLootMode(lua_State* L, Creature* creature); // TODO: Implement LootMode features
    int SetLootMode(lua_State* L, Creature* creature); // TODO: Implement LootMode features
    int HasLootMode(lua_State* L, Creature* creature); // TODO: Implement LootMode features
    int GetLootMode(lua_State* L, Creature* creature); // TODO: Implement LootMode features
    int IsTappedBy(lua_State* L, Creature* creature);
    int HasLootRecipient(lua_State* L, Creature* creature);
    int GetLootRecipientGroup(lua_State* L, Creature* creature);
    int GetLootRecipient(lua_State* L, Creature* creature);
    int SetDeathState(lua_State* L, Creature* creature);
    int SetWalk(lua_State* L, Creature* creature); // TODO: Move same to Player ?
    int SetReactState(lua_State* L, Creature* creature);
    int GetReactState(lua_State* L, Creature* creature);
    int HasReactState(lua_State* L, Creature* creature);
    int CanSwim(lua_State* L, Creature* creature);
    int CanWalk(lua_State* L, Creature* creature);
    int SetDisableGravity(lua_State* L, Creature* creature);
    int SetHover(lua_State* L, Creature* creature);
    int IsInEvadeMode(lua_State* L, Creature* creature);
    int IsElite(lua_State* L, Creature* creature);
    int IsGuard(lua_State* L, Creature* creature);
    int IsTrigger(lua_State* L, Creature* creature);
    int IsCivilian(lua_State* L, Creature* creature);
    int IsRacialLeader(lua_State* L, Creature* creature);
    int HasCategoryCooldown(lua_State* L, Creature* creature);
    int GetScriptName(lua_State* L, Creature* creature);
    int GetAIName(lua_State* L, Creature* creature);
    int GetScriptId(lua_State* L, Creature* creature);
    int GetCreatureSpellCooldownDelay(lua_State* L, Creature* creature);
    int GetCorpseDelay(lua_State* L, Creature* creature);
    int GetHomePosition(lua_State* L, Creature* creature);
    int AttackStart(lua_State* L, Creature* creature);
    int GetNearestTargetInAttackDistance(lua_State* L, Creature* creature);
    int GetNearestTarget(lua_State* L, Creature* creature);
    int GetNearestHostileTargetInAggroRange(lua_State* L, Creature* creature);
    int Despawn(lua_State* L, Creature* creature);
    int SendCreatureTalk(lua_State* L, Creature* creature);
    int GetAITarget(lua_State* L, Creature* creature);
    int GetAITargets(lua_State* L, Creature* creature);
    int GetAITargetsCount(lua_State* L, Creature* creature);
    int IsDungeonBoss(lua_State* L, Creature* creature);
    int IsWorldBoss(lua_State* L, Creature* creature);
    int GetNPCFlags(lua_State* L, Creature* creature);
    int SetNPCFlags(lua_State* L, Creature* creature);
    int HasSpell(lua_State* L, Creature* creature);
    int HasQuest(lua_State* L, Creature* creature);
    int SaveToDB(lua_State* L, Creature* creature);
    int HasSpellCooldown(lua_State* L, Creature* creature);
    int GetShieldBlockValue(lua_State* L, Creature* creature);
    int CanFly(lua_State* L, Creature* creature);
    int SelectVictim(lua_State* L, Creature* creature);
};
#endif