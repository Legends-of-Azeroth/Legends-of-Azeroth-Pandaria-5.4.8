-- Eliminando talisman de la buena fortuna del npc vendedor 64607 - comandante lo ping
DELETE FROM `npc_vendor` WHERE `entry` = '64607' AND `item` = '697' AND `ExtendedCost` = '3932' AND `type` = '2'; 

-- actualizando estados del npc para que pueda entregar misiones1
UPDATE `creature_template` SET `npcflag` = '131' WHERE `entry` = '64607'; 

-- creando la nueva misión para obtener los talismanes de la buena fortuna
DELETE FROM `quest_template` WHERE Id=40312;
insert into `quest_template` (`Id`, `Method`, `Level`, `MinLevel`, `MaxLevel`, `ZoneOrSort`, `Type`, `SuggestedPlayers`, `LimitTime`, `RequiredClasses`, `RequiredRaces`, `RequiredSkillId`, `RequiredSkillPoints`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestIdChain`, `RewardXPId`, `RewardOrRequiredMoney`, `RewardMoneyMaxLevel`, `RewardSpell`, `RewardSpellCast`, `RewardHonor`, `RewardHonorMultiplier`, `RewardMailTemplateId`, `RewardMailDelay`, `SourceItemId`, `SourceSpellId`, `Flags`, `Flags2`, `SpecialFlags`, `MinimapTargetMark`, `RewardTitleId`, `RewardTalents`, `RewardArenaPoints`, `RewardSkillId`, `RewardSkillPoints`, `RewardReputationMask`, `QuestGiverPortrait`, `QuestTurnInPortrait`, `RewardPackageItemId`, `RewardItemId1`, `RewardItemId2`, `RewardItemId3`, `RewardItemId4`, `RewardItemCount1`, `RewardItemCount2`, `RewardItemCount3`, `RewardItemCount4`, `RewardChoiceItemId1`, `RewardChoiceItemId2`, `RewardChoiceItemId3`, `RewardChoiceItemId4`, `RewardChoiceItemId5`, `RewardChoiceItemId6`, `RewardChoiceItemCount1`, `RewardChoiceItemCount2`, `RewardChoiceItemCount3`, `RewardChoiceItemCount4`, `RewardChoiceItemCount5`, `RewardChoiceItemCount6`, `RewardFactionId1`, `RewardFactionId2`, `RewardFactionId3`, `RewardFactionId4`, `RewardFactionId5`, `RewardFactionValueId1`, `RewardFactionValueId2`, `RewardFactionValueId3`, `RewardFactionValueId4`, `RewardFactionValueId5`, `RewardFactionValueIdOverride1`, `RewardFactionValueIdOverride2`, `RewardFactionValueIdOverride3`, `RewardFactionValueIdOverride4`, `RewardFactionValueIdOverride5`, `PointMapId`, `PointX`, `PointY`, `PointOption`, `Title`, `Objectives`, `Details`, `EndText`, `OfferRewardText`, `RequestItemsText`, `CompletedText`, `RequiredSourceItemId1`, `RequiredSourceItemId2`, `RequiredSourceItemId3`, `RequiredSourceItemId4`, `RequiredSourceItemCount1`, `RequiredSourceItemCount2`, `RequiredSourceItemCount3`, `RequiredSourceItemCount4`, `RewardCurrencyId1`, `RewardCurrencyId2`, `RewardCurrencyId3`, `RewardCurrencyId4`, `RewardCurrencyCount1`, `RewardCurrencyCount2`, `RewardCurrencyCount3`, `RewardCurrencyCount4`, `QuestGiverTextWindow`, `QuestGiverTargetName`, `QuestTurnTextWindow`, `QuestTurnTargetName`, `SoundAccept`, `SoundTurnIn`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `EmoteOnIncomplete`, `EmoteOnComplete`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `WDBVerified`) values('40312','2','90','90','0','6213','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','32904','0','1','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Talismán ancestral de buena fortuna','Obten 80 Talismán inferior de buena fortuna','Saludos, $N. $BTengo el privilegio de ofrecerte una oportunidad excelente. A través de los siglos, hemos perfeccionando un método con el que se pueden manipular las mismísimas leyes de la fortuna. Con un Talismán ancestral de buena fortuna, los artefactos valiosos caerán en tus manos con mayor frecuencia; puedes  obtener una recompensa extra en las Cámaras Mogu\'shan, el Corazón del Miedo, la Veranda de la Primavera Eterna, del sha de la Ira o la banda guerrera de Salyis.',NULL,'Con esto ahora podrás obtener una recompensa extra','Regresa cuando tengas todos los talismanes','Regresa con el comandante Lo Ping','0','0','0','0','0','0','0','0','697','0','0','0','3','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414');

-- agregando objetivo de la misión
DELETE FROM `quest_objective` WHERE `questId` = '40312' AND `id` = '500282' AND `index` = '0'; 
insert into `quest_objective` (`questId`, `id`, `index`, `type`, `objectId`, `amount`, `flags`, `description`) values('40312','500282','0','4','738','80','0','');

-- agregando marcador en el mapa para entregar la misión
DELETE FROM `quest_poi` WHERE `questId` = '40312' AND `id` = '0'; 
insert into `quest_poi` (`questId`, `id`, `objIndex`, `mapid`, `WorldMapAreaId`, `FloorId`, `unk3`, `unk4`) values('40312','0','-1','870','6213','0','0','0');

-- agregando coordendas del marcador
DELETE FROM `quest_poi_points` WHERE `questId` = '40312' AND `id` = '0' AND `idx` = '0'; 
insert into `quest_poi_points` (`questId`, `id`, `idx`, `x`, `y`) values('40312','0','0','2048','4899');


-- entregando misión al jugador
delete from `creature_questender` where `id`=64607 and `quest`= 40312;
insert into `creature_questender` (`id`, `quest`) values('64607','40312');

-- completando la misión al jugador
delete from `creature_queststarter` where `id`=64607 and `quest`= 40312;
insert into `creature_queststarter` (`id`, `quest`) values('64607','40312');
