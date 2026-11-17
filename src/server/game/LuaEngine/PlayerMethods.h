/*
* Copyright (C) 2010 - 2013 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef ELUNA_PLAYERMETHODS_H
#define ELUNA_PLAYERMETHODS_H

struct lua_State;
class player;

namespace LuaPlayer
{
    int SaveToDB(lua_State* L, Player* player);

    int HasQuest(lua_State* L, Player* player);

    int HasSpell(lua_State* L, Player* player);

    int SummonPlayer(lua_State* L, Player* player);

    int Mute(lua_State* L, Player* player);

    int CreateCorpse(lua_State* L, Player* player);

    int RewardQuest(lua_State* L, Player* player);

    int HasAura(lua_State* L, Player* player);

    int IsARecruiter(lua_State* L, Player* player);

    int GetRecruiterId(lua_State* L, Player* player);

    int GetSelectedPlayer(lua_State* L, Player* player);

    int GetSelectedUnit(lua_State* L, Player* player);

    int GetLatency(lua_State* L, Player* player);

    int SendAuctionMenu(lua_State* L, Player* player);

    int SendMailMenu(lua_State* L, Player* player);

    int SendTaxiMenu(lua_State* L, Player* player);

    int SendSpiritResurrect(lua_State* L, Player* player);

    int SendTabardVendorActivate(lua_State* L, Player* player);

    int SendShowBank(lua_State* L, Player* player);

    int SendListInventory(lua_State* L, Player* player);

    int SendTrainerList(lua_State* L, Player* player);

    int SendGuildInvite(lua_State* L, Player* player);

    int LogoutPlayer(lua_State* L, Player* player);

    int GetChampioningFaction(lua_State* L, Player* player);

    int ResetAchievements(lua_State* L, Player* player);

    int HasAchieved(lua_State* L, Player* player);

    int GetOriginalSubGroup(lua_State* L, Player* player);

    int GetOriginalGroup(lua_State* L, Player* player);

    int RemoveFromBattlegroundOrBattlefieldRaid(lua_State* L, Player* player);

    int CanUninviteFromGroup(lua_State* L, Player* player);

    int GetNextRandomRaidMember(lua_State* L, Player* player);

    int GetSubGroup(lua_State* L, Player* player);

    int GetGroupInvite(lua_State* L, Player* player);

    int HasPendingBind(lua_State* L, Player* player);

    int UnbindInstance(lua_State* L, Player* player);

    int BindToInstance(lua_State* L, Player* player);

    int SetAtLoginFlag(lua_State* L, Player* player);

    int InRandomLfgDungeon(lua_State* L, Player* player);

    int IsUsingLfg(lua_State* L, Player* player);

    int HasAtLoginFlag(lua_State* L, Player* player);

    int IsVisibleForPlayer(lua_State* L, Player* player);

    int IsNeverVisible(lua_State* L, Player* player);

    int CanFlyInZone(lua_State* L, Player* player);

    int SetRestTime(lua_State* L, Player* player);

    int GetRestTime(lua_State* L, Player* player);

    int GetXPRestBonus(lua_State* L, Player* player);

    int CanSpeak(lua_State* L, Player* player);

    int IsImmuneToEnvironmentalDamage(lua_State* L, Player* player);

    int IsRested(lua_State* L, Player* player);

    int LeaveBattleground(lua_State* L, Player* player);

    int InBattlegroundQueue(lua_State* L, Player* player);

    int GetBattlegroundTypeId(lua_State* L, Player* player);

    int GetBattlegroundId(lua_State* L, Player* player);

    int IsOutdoorPvPActive(lua_State* L, Player* player);

    int InArena(lua_State* L, Player* player);

    int InBattleground(lua_State* L, Player* player);

    int CanTameExoticPets(lua_State* L, Player* player);

    int CanTitanGrip(lua_State* L, Player* player);

    int CanBlock(lua_State* L, Player* player);

    int CanParry(lua_State* L, Player* player);

    int GetDrunkValue(lua_State* L, Player* player);

    int SetDrunkValue(lua_State* L, Player* player);

    int GetSpellCooldowns(lua_State* L, Player* player);

    int ModifyArenaPoints(lua_State* L, Player* player);

    int ModifyHonorPoints(lua_State* L, Player* player);

    int GetReputationRank(lua_State* L, Player* player);

    int IsHonorOrXPTarget(lua_State* L, Player* player);

    int SetFactionForRace(lua_State* L, Player* player);

    int SetSkill(lua_State* L, Player* player);

    int HasSkill(lua_State* L, Player* player);

    int GetSkillTempBonusValue(lua_State* L, Player* player);

    int GetSkillPermBonusValue(lua_State* L, Player* player);

    int GetPureSkillValue(lua_State* L, Player* player);

    int GetBaseSkillValue(lua_State* L, Player* player);

    int GetSkillValue(lua_State* L, Player* player);

    int GetPureMaxSkillValue(lua_State* L, Player* player);

    int GetMaxSkillValue(lua_State* L, Player* player);

    int SetMovement(lua_State* L, Player* player);

    int DurabilityRepair(lua_State* L, Player* player);

    int DurabilityRepairAll(lua_State* L, Player* player);

    int DurabilityPointLossForEquipSlot(lua_State* L, Player* player);

    int DurabilityPointsLossAll(lua_State* L, Player* player);

    int DurabilityPointsLoss(lua_State* L, Player* player);

    int DurabilityLoss(lua_State* L, Player* player);

    int DurabilityLossAll(lua_State* L, Player* player);

    int KillPlayer(lua_State* L, Player* player);

    int GetManaBonusFromIntellect(lua_State* L, Player* player);

    int GetHealthBonusFromStamina(lua_State* L, Player* player);

    int GetDifficulty(lua_State* L, Player* player);

    int GetGuildRank(lua_State* L, Player* player); // TODO: Move to Guild Methods

    int SetGuildRank(lua_State* L, Player* player); // TODO: Move to Guild Methods

    int RemoveFromGroup(lua_State* L, Player* player);

    int IsGroupVisibleFor(lua_State* L, Player* player);

    int IsInSameRaidWith(lua_State* L, Player* player);

    int IsInSameGroupWith(lua_State* L, Player* player);

    int GetSpellCooldownDelay(lua_State* L, Player* player);

    int GetSpecsCount(lua_State* L, Player* player);

    int GetActiveSpec(lua_State* L, Player* player);

    int HasTalent(lua_State* L, Player* player);

    int AddTalent(lua_State* L, Player* player);

    int ResetTalentsCost(lua_State* L, Player* player);

    int ResetTalents(lua_State* L, Player* player);

    int SetFreeTalentPoints(lua_State* L, Player* player);

    int GetFreeTalentPoints(lua_State* L, Player* player);

    int GetGuildName(lua_State* L, Player* player);

    int GetReputation(lua_State* L, Player* player);

    int SetReputation(lua_State* L, Player* player);

    int RemoveSpell(lua_State* L, Player* player);

    int ClearComboPoints(lua_State* L, Player* player);

    int GainSpellComboPoints(lua_State* L, Player* player);

    int AddComboPoints(lua_State* L, Player* player);

    int GetComboTarget(lua_State* L, Player* player);

    int GetComboPoints(lua_State* L, Player* player);

    int HasReceivedQuestReward(lua_State* L, Player* player);

    int RegenerateHealth(lua_State* L, Player* player);

    int Regenerate(lua_State* L, Player* player);

    int RegenerateAll(lua_State* L, Player* player);

    int ResetPetTalents(lua_State* L, Player* player);

    int GetInGameTime(lua_State* L, Player* player);

    int TalkedToCreature(lua_State* L, Player* player);

    int KillGOCredit(lua_State* L, Player* player);

    int KilledPlayerCredit(lua_State* L, Player* player);

    int KilledMonsterCredit(lua_State* L, Player* player);

    int GroupEventHappens(lua_State* L, Player* player);

    int AreaExploredOrEventHappens(lua_State* L, Player* player);

    int CanShareQuest(lua_State* L, Player* player);

    int HasQuestForGO(lua_State* L, Player* player);

    int HasQuestForItem(lua_State* L, Player* player);

    int GetReqKillOrCastCurrentCount(lua_State* L, Player* player);

    int RemoveRewardedQuest(lua_State* L, Player* player);

    int RemoveActiveQuest(lua_State* L, Player* player);

    int SetQuestStatus(lua_State* L, Player* player);

    int GetQuestStatus(lua_State* L, Player* player);

    int GetQuestRewardStatus(lua_State* L, Player* player);

    int FailQuest(lua_State* L, Player* player);

    int IncompleteQuest(lua_State* L, Player* player);

    int CompleteQuest(lua_State* L, Player* player);

    int IsActiveQuest(lua_State* L, Player* player);

    int GetQuestLevel(lua_State* L, Player* player);

    int GetItemByEntry(lua_State* L, Player* player);

    int GetEquippedItemBySlot(lua_State* L, Player* player);

    int Whisper(lua_State* L, Player* player);

    int TextEmote(lua_State* L, Player* player);

    int Yell(lua_State* L, Player* player);

    int Say(lua_State* L, Player* player);

    int GetPhaseMaskForSpawn(lua_State* L, Player* player);

    int SummonPet(lua_State* L, Player* player);

    int RemovePet(lua_State* L, Player* player);

    int HasRestFlag(lua_State* L, Player* player);

    int SetRestFlag(lua_State* L, Player* player);

    int SetRestBonus(lua_State* L, Player* player);

    int GetRestBonus(lua_State* L, Player* player);

    int GiveLevel(lua_State* L, Player* player);

    int GiveXP(lua_State* L, Player* player);
 
    int IsGMVisible(lua_State* L, Player* player);

    int IsTaxiCheater(lua_State* L, Player* player);

    int IsGMChat(lua_State* L, Player* player);

    int IsAcceptingWhispers(lua_State* L, Player* player);

    int SetAcceptWhispers(lua_State* L, Player* player);

    int SetPvPDeath(lua_State* L, Player* player);

    int SetGMVisible(lua_State* L, Player* player);

    int SetTaxiCheat(lua_State* L, Player* player);

    int SetGMChat(lua_State* L, Player* player);

    int SetGameMaster(lua_State* L, Player* player);

    int GetChatTag(lua_State* L, Player* player);

    int IsDND(lua_State* L, Player* player);

    int IsAFK(lua_State* L, Player* player);

    int ToggleDND(lua_State* L, Player* player);

    int ToggleAFK(lua_State* L, Player* player);

    int IsFalling(lua_State* L, Player* player);

    int GetNearbyGameObject(lua_State* L, Player* player);

    int EquipItem(lua_State* L, Player* player);

    int CanEquipItem(lua_State* L, Player* player);

    int GetItemByPos(lua_State* L, Player* player);

    int GetArenaPoints(lua_State* L, Player* player);

    int SetGender(lua_State* L, Player* player);

    int GetHonorPoints(lua_State* L, Player* player);

    int GetGossipTextId(lua_State* L, Player* player);

    int GetSelection(lua_State* L, Player* player);

    int GetGMRank(lua_State* L, Player* player);

    int GetCoinage(lua_State* L, Player* player);

    int GetGuildId(lua_State* L, Player* player);

    int GetTeam(lua_State* L, Player* player);

    int GetItemCount(lua_State* L, Player* player);

    int GetLifetimeKills(lua_State* L, Player* player);

    int GetPlayerIP(lua_State* L, Player* player);

    int GetLevelPlayedTime(lua_State* L, Player* player);

    int GetTotalPlayedTime(lua_State* L, Player* player);

    int GetGuild(lua_State* L, Player* player);

    int GetGroup(lua_State* L, Player* player);

    int SetArenaPoints(lua_State* L, Player* player);

    int SetHonorPoints(lua_State* L, Player* player);

    int SetLifetimeKills(lua_State* L, Player* player);

    int SetCoinage(lua_State* L, Player* player);

    int SetBindPoint(lua_State* L, Player* player);

    int SetKnownTitle(lua_State* L, Player* player);

    int UnsetKnownTitle(lua_State* L, Player* player);

    int AdvanceSkillsToMax(lua_State* L, Player* player);

    int AdvanceAllSkills(lua_State* L, Player* player);

    int AdvanceSkill(lua_State* L, Player* player);

    int IsInGroup(lua_State* L, Player* player);

    int IsInGuild(lua_State* L, Player* player);

    int IsGM(lua_State* L, Player* player);

    int IsInArenaTeam(lua_State* L, Player* player);

    int IsHorde(lua_State* L, Player* player);

    int IsAlliance(lua_State* L, Player* player);

    int HasTitle(lua_State* L, Player* player);

    int HasItem(lua_State* L, Player* player);

    int Teleport(lua_State* L, Player* player);

    int AddLifetimeKills(lua_State* L, Player* player);

    int AddItem(lua_State* L, Player* player);

    int RemoveItem(lua_State* L, Player* player);

    int RemoveLifetimeKills(lua_State* L, Player* player);

    int ResetSpellCooldown(lua_State* L, Player* player);

    int ResetTypeCooldowns(lua_State* L, Player* player);

    int ResetAllCooldowns(lua_State* L, Player* player);

    int SendClearCooldowns(lua_State* L, Player* player);

    int SendBroadcastMessage(lua_State* L, Player* player);

    int SendAreaTriggerMessage(lua_State* L, Player* player);

    int SendNotification(lua_State* L, Player* player);

    int SendPacketToPlayer(lua_State* L, Player* player);

    int SendPacket(lua_State* L, Player* player);

    int SendPacketToGroup(lua_State* L, Player* player);

    int SendPacketToGuild(lua_State* L, Player* player);

    int SendPacketToRankedInGuild(lua_State* L, Player* player);

    int SendVendorWindow(lua_State* L, Player* player);

    int KickPlayer(lua_State* L, Player* player);

    int ModifyMoney(lua_State* L, Player* player);

    int LearnSpell(lua_State* L, Player* player);

    int ResurrectPlayer(lua_State* L, Player* player);

    int GetAccountId(lua_State* L, Player* player);

    int GetAccountName(lua_State* L, Player* player);

    int GetCorpse(lua_State* L, Player* player);

    int GossipMenuAddItem(lua_State* L, Player* player);

    int GossipComplete(lua_State* L, Player* player);

    int GossipSendMenu(lua_State* L, Player* player);

    int GossipClearMenu(lua_State* L, Player* player);

    int PlaySoundToPlayer(lua_State* L, Player* player);

    int StartTaxi(lua_State* L, Player* player);

    int SetPlayerLock(lua_State* L, Player* player);

    int GossipSendPOI(lua_State* L, Player* player);

    int GossipAddQuests(lua_State* L, Player* player);

    int SendQuestTemplate(lua_State* L, Player* player);

    int SpawnBones(lua_State* L, Player* player);

    int RemovedInsignia(lua_State* L, Player* player);

    int GetDbLocaleIndex(lua_State* L, Player* player);

    int GetDbcLocale(lua_State* L, Player* player);

    int CanUseItem(lua_State* L, Player* player);

    int HasSpellCooldown(lua_State* L, Player* player);

    int GetShieldBlockValue(lua_State* L, Player* player);

    int IsInWater(lua_State* L, Player* player);

    int SetSheath(lua_State* L, Player* player);

    int CanFly(lua_State* L, Player* player);

};
#endif