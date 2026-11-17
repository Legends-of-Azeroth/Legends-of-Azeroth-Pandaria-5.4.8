ALTER TABLE `quest_template`
    CHANGE `WDBVerified` `VerifiedBuild` int NOT NULL DEFAULT '0',
    CHANGE `Id` `ID` int unsigned NOT NULL DEFAULT '0',
    CHANGE `Method` `QuestType` tinyint unsigned NOT NULL DEFAULT '2' AFTER `ID`,
    CHANGE `Level` `QuestLevel` SMALLINT(5) NOT NULL DEFAULT '1' AFTER `QuestType`,
    CHANGE `MinLevel` `MinLevel` SMALLINT(5) NOT NULL DEFAULT '0' AFTER `QuestLevel`,
    CHANGE `RewardPackageItemId` `QuestPackageID` int unsigned NOT NULL DEFAULT '0' AFTER `MinLevel`,
    CHANGE `ZoneOrSort` `QuestSortID` smallint NOT NULL DEFAULT '0' AFTER `QuestPackageID`,
    CHANGE `Type` `QuestInfoID` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' AFTER `QuestSortID`,
    CHANGE `SuggestedPlayers` `SuggestedGroupNum` tinyint unsigned NOT NULL DEFAULT '0' AFTER `QuestInfoID`,
    CHANGE `NextQuestIdChain` `RewardNextQuest` int unsigned NOT NULL DEFAULT '0' AFTER `SuggestedGroupNum`,
    CHANGE `RewardXPId` `RewardXPDifficulty` int unsigned NOT NULL DEFAULT '0' AFTER `RewardNextQuest`,
    CHANGE `RewardOrRequiredMoney` `RewardMoney` int NOT NULL DEFAULT '0' AFTER `RewardXPDifficulty`,
    CHANGE `RewardMoneyMaxLevel` `RewardBonusMoney` int unsigned NOT NULL DEFAULT '0' AFTER `RewardMoney`,
    CHANGE `RewardSpell` `RewardDisplaySpell` int unsigned NOT NULL DEFAULT '0' AFTER `RewardBonusMoney`,
    CHANGE `RewardSpellCast` `RewardSpell` int unsigned NOT NULL DEFAULT '0' AFTER `RewardDisplaySpell`,
    CHANGE `RewardHonor` `RewardHonor` int unsigned NOT NULL DEFAULT '0' AFTER `RewardSpell`,
    CHANGE `RewardHonorMultiplier` `RewardKillHonor` int unsigned NOT NULL DEFAULT '0' AFTER `RewardHonor`,
    CHANGE `SourceItemId` `StartItem` int unsigned NOT NULL DEFAULT '0' AFTER `RewardKillHonor`,
    CHANGE `Flags` `Flags` int unsigned NOT NULL DEFAULT '0' AFTER `StartItem`,
    CHANGE `Flags2` `FlagsEx` int unsigned NOT NULL DEFAULT '0' AFTER `Flags`,

    CHANGE `RewardItemId1` `RewardItem1` int unsigned NOT NULL DEFAULT '0' AFTER `FlagsEx`,
    CHANGE `RewardItemCount1` `RewardAmount1` int unsigned NOT NULL DEFAULT '0' AFTER `RewardItem1`,
    CHANGE `RewardItemId2` `RewardItem2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardAmount1`,
    CHANGE `RewardItemCount2` `RewardAmount2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardItem2`,
    CHANGE `RewardItemId3` `RewardItem3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardAmount2`,
    CHANGE `RewardItemCount3` `RewardAmount3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardItem3`,
    CHANGE `RewardItemId4` `RewardItem4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardAmount3`,
    CHANGE `RewardItemCount4` `RewardAmount4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardItem4`,

    CHANGE `RequiredSourceItemId1` `ItemDrop1` int unsigned NOT NULL DEFAULT '0' AFTER `RewardAmount4`,
    CHANGE `RequiredSourceItemCount1` `ItemDropQuantity1` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDrop1`,
    CHANGE `RequiredSourceItemId2` `ItemDrop2` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDropQuantity1`,
    CHANGE `RequiredSourceItemCount2` `ItemDropQuantity2` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDrop2`,
    CHANGE `RequiredSourceItemId3` `ItemDrop3` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDropQuantity2`,
    CHANGE `RequiredSourceItemCount3` `ItemDropQuantity3` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDrop3`,
    CHANGE `RequiredSourceItemId4` `ItemDrop4` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDropQuantity3`,
    CHANGE `RequiredSourceItemCount4` `ItemDropQuantity4` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDrop4`,

    CHANGE `RewardChoiceItemId1` `RewardChoiceItemID1` int unsigned NOT NULL DEFAULT '0' AFTER `ItemDropQuantity4`,
    CHANGE `RewardChoiceItemCount1` `RewardChoiceItemQuantity1` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemID1`,
    CHANGE `RewardChoiceItemId2` `RewardChoiceItemID2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemQuantity1`,
    CHANGE `RewardChoiceItemCount2` `RewardChoiceItemQuantity2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemID2`,
    CHANGE `RewardChoiceItemId3` `RewardChoiceItemID3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemQuantity2`,
    CHANGE `RewardChoiceItemCount3` `RewardChoiceItemQuantity3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemID3`,
    CHANGE `RewardChoiceItemId4` `RewardChoiceItemID4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemQuantity3`,
    CHANGE `RewardChoiceItemCount4` `RewardChoiceItemQuantity4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemID4`,
    CHANGE `RewardChoiceItemId5` `RewardChoiceItemID5` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemQuantity4`,
    CHANGE `RewardChoiceItemCount5` `RewardChoiceItemQuantity5` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemID5`,
    CHANGE `RewardChoiceItemId6` `RewardChoiceItemID6` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemQuantity5`,
    CHANGE `RewardChoiceItemCount6` `RewardChoiceItemQuantity6` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemID6`,

    CHANGE `PointMapId` `POIContinent` int unsigned NOT NULL DEFAULT '0' AFTER `RewardChoiceItemQuantity6`,
    CHANGE `PointX` `POIx` float NOT NULL DEFAULT '0' AFTER `POIContinent`,
    CHANGE `PointY` `POIy` float NOT NULL DEFAULT '0' AFTER `POIx`,
    CHANGE `PointOption` `POIPriority` int NOT NULL DEFAULT '0' AFTER `POIy`,

    CHANGE `RewardTitleId` `RewardTitle` int unsigned NOT NULL DEFAULT '0' AFTER `POIPriority`,
    CHANGE `RewardArenaPoints` `RewardArenaPoints` int unsigned NOT NULL DEFAULT '0' AFTER `RewardTitle`,
    CHANGE `RewardSkillId` `RewardSkillLineID` int unsigned NOT NULL DEFAULT '0' AFTER `RewardArenaPoints`,
    CHANGE `RewardSkillPoints` `RewardNumSkillUps` int unsigned NOT NULL DEFAULT '0' AFTER `RewardSkillLineID`,

    CHANGE `QuestGiverPortrait` `PortraitGiver` int unsigned NOT NULL DEFAULT '0' AFTER `RewardNumSkillUps`,
    CHANGE `QuestTurnInPortrait` `PortraitTurnIn` int unsigned NOT NULL DEFAULT '0' AFTER `PortraitGiver`,

    CHANGE `RewardFactionId1` `RewardFactionID1` int unsigned NOT NULL DEFAULT '0' AFTER `PortraitTurnIn`,
    CHANGE `RewardFactionId2` `RewardFactionID2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardFactionID1`,
    CHANGE `RewardFactionId3` `RewardFactionID3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardFactionID2`,
    CHANGE `RewardFactionId4` `RewardFactionID4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardFactionID3`,
    CHANGE `RewardFactionId5` `RewardFactionID5` int unsigned NOT NULL DEFAULT '0' AFTER `RewardFactionID4`,

    CHANGE `RewardCurrencyId1` `RewardCurrencyID1` int unsigned NOT NULL DEFAULT '0' AFTER `RewardFactionID5`,
    CHANGE `RewardCurrencyCount1` `RewardCurrencyQty1` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyID1`,
    CHANGE `RewardCurrencyId2` `RewardCurrencyID2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyQty1`,
    CHANGE `RewardCurrencyCount2` `RewardCurrencyQty2` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyID2`,
    CHANGE `RewardCurrencyId3` `RewardCurrencyID3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyQty2`,
    CHANGE `RewardCurrencyCount3` `RewardCurrencyQty3` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyID3`,
    CHANGE `RewardCurrencyId4` `RewardCurrencyID4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyQty3`,
    CHANGE `RewardCurrencyCount4` `RewardCurrencyQty4` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyID4`,

    CHANGE `SoundAccept` `AcceptedSoundKitID` int unsigned NOT NULL DEFAULT '0' AFTER `RewardCurrencyQty4`,
    CHANGE `SoundTurnIn` `CompleteSoundKitID` int unsigned NOT NULL DEFAULT '0' AFTER `AcceptedSoundKitID`,

    CHANGE `MinimapTargetMark` `AreaGroupID` int unsigned NOT NULL DEFAULT '0' AFTER `CompleteSoundKitID`,
    CHANGE `LimitTime` `TimeAllowed` bigint NOT NULL DEFAULT '0' AFTER `AreaGroupID`,
    CHANGE `RequiredRaces` `AllowableRaces` bigint unsigned DEFAULT '0' AFTER `TimeAllowed`,

    CHANGE `Title` `LogTitle` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `AllowableRaces`,
    CHANGE `Objectives` `LogDescription` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `LogTitle`,
    CHANGE `Details` `QuestDescription` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `LogDescription`,
    CHANGE `EndText` `AreaDescription` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `QuestDescription`,

    CHANGE `QuestGiverTextWindow` `PortraitGiverText` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `AreaDescription`,
    CHANGE `QuestGiverTargetName` `PortraitGiverName` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `PortraitGiverText`,
    CHANGE `QuestTurnTextWindow` `PortraitTurnInText` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `PortraitGiverName`,
    CHANGE `QuestTurnTargetName` `PortraitTurnInName` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `PortraitTurnInText`,

    CHANGE `CompletedText` `QuestCompletionLog` mediumtext CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci AFTER `PortraitTurnInName`
;

-- quest_details
DROP TABLE IF EXISTS `quest_details`;
CREATE TABLE `quest_details`
(
    `ID`            MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
    `Emote1`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `Emote2`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `Emote3`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `Emote4`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `EmoteDelay1`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `EmoteDelay2`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `EmoteDelay3`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `EmoteDelay4`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `VerifiedBuild` SMALLINT(5)           NOT NULL DEFAULT '0',
    PRIMARY KEY (`ID`)
) ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

INSERT INTO `quest_details` (`ID`, `Emote1`, `Emote2`, `Emote3`, `Emote4`, `EmoteDelay1`, `EmoteDelay2`, `EmoteDelay3`,
                             `EmoteDelay4`, `VerifiedBuild`)
SELECT `ID`,
       `DetailsEmote1`,
       `DetailsEmote2`,
       `DetailsEmote3`,
       `DetailsEmote4`,
       `DetailsEmoteDelay1`,
       `DetailsEmoteDelay2`,
       `DetailsEmoteDelay3`,
       `DetailsEmoteDelay4`,
       `VerifiedBuild`
FROM `quest_template`
WHERE `DetailsEmote1` != 0
   OR `DetailsEmote2` != 0
   OR `DetailsEmote3` != 0
   OR `DetailsEmote4` != 0
   OR `DetailsEmoteDelay1` != 0
   OR `DetailsEmoteDelay2` != 0
   OR `DetailsEmoteDelay3` != 0
   OR `DetailsEmoteDelay4` != 0;


-- quest_request_items
DROP TABLE IF EXISTS `quest_request_items`;
CREATE TABLE `quest_request_items`
(
    `ID`                MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
    `EmoteOnComplete`   SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `EmoteOnIncomplete` SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `CompletionText`    TEXT,
    `VerifiedBuild`     SMALLINT(5)           NOT NULL DEFAULT '0',
    PRIMARY KEY (`ID`)
) ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

INSERT INTO `quest_request_items` (`ID`, `EmoteOnComplete`, `EmoteOnIncomplete`, `CompletionText`, `VerifiedBuild`)
SELECT `ID`, `EmoteOnComplete`, `EmoteOnIncomplete`, `RequestItemsText`, `VerifiedBuild`
FROM `quest_template`
WHERE `EmoteOnComplete` != 0
   OR `EmoteOnIncomplete` != 0
   OR `RequestItemsText` != '';

-- quest_offer_reward
DROP TABLE IF EXISTS `quest_offer_reward`;
CREATE TABLE `quest_offer_reward`
(
    `ID`            MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
    `Emote1`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `Emote2`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `Emote3`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `Emote4`        SMALLINT(5) UNSIGNED  NOT NULL DEFAULT '0',
    `EmoteDelay1`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `EmoteDelay2`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `EmoteDelay3`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `EmoteDelay4`   INT(10) UNSIGNED      NOT NULL DEFAULT '0',
    `RewardText`    TEXT,
    `VerifiedBuild` SMALLINT(5)           NOT NULL DEFAULT '0',
    PRIMARY KEY (`ID`)
) ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

INSERT INTO `quest_offer_reward` (`ID`, `Emote1`, `Emote2`, `Emote3`, `Emote4`, `EmoteDelay1`, `EmoteDelay2`,
                                  `EmoteDelay3`, `EmoteDelay4`, `RewardText`, `VerifiedBuild`)
SELECT `ID`,
       `OfferRewardEmote1`,
       `OfferRewardEmote2`,
       `OfferRewardEmote3`,
       `OfferRewardEmote4`,
       `OfferRewardEmoteDelay1`,
       `OfferRewardEmoteDelay2`,
       `OfferRewardEmoteDelay3`,
       `OfferRewardEmoteDelay4`,
       `OfferRewardText`,
       `VerifiedBuild`
FROM `quest_template`
WHERE `OfferRewardEmote1` != 0
   OR `OfferRewardEmote2` != 0
   OR `OfferRewardEmote3` != 0
   OR `OfferRewardEmote4` != 0
   OR `OfferRewardEmoteDelay1` != 0
   OR `OfferRewardEmoteDelay2` != 0
   OR `OfferRewardEmoteDelay3` != 0
   OR `OfferRewardEmoteDelay4` != 0
   OR `OfferRewardText` != '';

-- delete old fields
ALTER TABLE `quest_template`
    DROP `DetailsEmote1`,
    DROP `DetailsEmote2`,
    DROP `DetailsEmote3`,
    DROP `DetailsEmote4`,
    DROP `DetailsEmoteDelay1`,
    DROP `DetailsEmoteDelay2`,
    DROP `DetailsEmoteDelay3`,
    DROP `DetailsEmoteDelay4`,
    DROP `EmoteOnIncomplete`,
    DROP `EmoteOnComplete`,
    DROP `RequestItemsText`,
    DROP `OfferRewardEmote1`,
    DROP `OfferRewardEmote2`,
    DROP `OfferRewardEmote3`,
    DROP `OfferRewardEmote4`,
    DROP `OfferRewardEmoteDelay1`,
    DROP `OfferRewardEmoteDelay2`,
    DROP `OfferRewardEmoteDelay3`,
    DROP `OfferRewardEmoteDelay4`,
    DROP `OfferRewardText`;

-- RewardFactionID(s), RewardFactionValue(s) and RewardFactionOverride(s) [5x3 fields]
ALTER TABLE `quest_template`
    CHANGE COLUMN `RewardFactionValueId1` `RewardFactionValue1` int NOT NULL DEFAULT '0' AFTER `RewardFactionID1`,
    CHANGE COLUMN `RewardFactionValueIdOverride1` `RewardFactionOverride1` int NOT NULL DEFAULT '0' AFTER `RewardFactionValue1`,
    CHANGE COLUMN `RewardFactionValueId2` `RewardFactionValue2` int NOT NULL DEFAULT '0' AFTER `RewardFactionID2`,
    CHANGE COLUMN `RewardFactionValueIdOverride2` `RewardFactionOverride2` int NOT NULL DEFAULT '0' AFTER `RewardFactionValue2`,
    CHANGE COLUMN `RewardFactionValueId3` `RewardFactionValue3` int NOT NULL DEFAULT '0' AFTER `RewardFactionID3`,
    CHANGE COLUMN `RewardFactionValueIdOverride3` `RewardFactionOverride3` int NOT NULL DEFAULT '0' AFTER `RewardFactionValue3`,
    CHANGE COLUMN `RewardFactionValueId4` `RewardFactionValue4` int NOT NULL DEFAULT '0' AFTER `RewardFactionID4`,
    CHANGE COLUMN `RewardFactionValueIdOverride4` `RewardFactionOverride4` int NOT NULL DEFAULT '0' AFTER `RewardFactionValue4`,
    CHANGE COLUMN `RewardFactionValueId5` `RewardFactionValue5` int NOT NULL DEFAULT '0' AFTER `RewardFactionID5`,
    CHANGE COLUMN `RewardFactionValueIdOverride5` `RewardFactionOverride5` int NOT NULL DEFAULT '0' AFTER `RewardFactionValue5`,
    CHANGE COLUMN `RewardReputationMask` `RewardFactionFlags` int unsigned NOT NULL DEFAULT '0' AFTER `RewardFactionOverride5`;

DROP TABLE IF EXISTS `quest_template_addon`;
CREATE TABLE `quest_template_addon`
(
    `ID`                    int unsigned                                                 NOT NULL DEFAULT '0',
    `MaxLevel`              tinyint unsigned                                             NOT NULL DEFAULT '0',
    `AllowableClasses`      int unsigned                                                 NOT NULL DEFAULT '0',
    `SourceSpellID`         int unsigned                                                 NOT NULL DEFAULT '0',
    `PrevQuestID`           int                                                          NOT NULL DEFAULT '0',
    `NextQuestID`           int unsigned                                                 NOT NULL DEFAULT '0',
    `ExclusiveGroup`        int                                                          NOT NULL DEFAULT '0',
    `RewardMailTemplateID`  int unsigned                                                 NOT NULL DEFAULT '0',
    `RewardMailDelay`       int unsigned                                                 NOT NULL DEFAULT '0',
    `RequiredSkillID`       smallint unsigned                                            NOT NULL DEFAULT '0',
    `RequiredSkillPoints`   smallint unsigned                                            NOT NULL DEFAULT '0',
    `RequiredMinRepFaction` smallint unsigned                                            NOT NULL DEFAULT '0',
    `RequiredMaxRepFaction` smallint unsigned                                            NOT NULL DEFAULT '0',
    `RequiredMinRepValue`   int                                                          NOT NULL DEFAULT '0',
    `RequiredMaxRepValue`   int                                                          NOT NULL DEFAULT '0',
    `ProvidedItemCount`     tinyint unsigned                                             NOT NULL DEFAULT '0',
    `SpecialFlags`          int unsigned                                                 NOT NULL DEFAULT '0',
    `ScriptName`            varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '',
    PRIMARY KEY (`ID`)
) ENGINE = InnoDB
  DEFAULT CHARSET = utf8mb4
  COLLATE = utf8mb4_unicode_ci;

INSERT INTO `quest_template_addon` (ID, MaxLevel, AllowableClasses, SourceSpellID, PrevQuestID, NextQuestID,
                                    ExclusiveGroup, RewardMailTemplateID, RewardMailDelay, RequiredSkillID,
                                    RequiredSkillPoints, RequiredMinRepFaction, RequiredMaxRepFaction,
                                    RequiredMinRepValue, RequiredMaxRepValue, ProvidedItemCount, SpecialFlags,
                                    ScriptName)
SELECT `ID`,
       `MaxLevel`,
       `RequiredClasses`,
       `SourceSpellId`,
       `PrevQuestId`,
       `NextQuestId`,
       `ExclusiveGroup`,
       `RewardMailTemplateId`,
       `RewardMailDelay`,
       `RequiredSkillId`,
       `RequiredSkillPoints`,
       `RequiredMinRepFaction`,
       `RequiredMaxRepFaction`,
       `RequiredMinRepValue`,
       `RequiredMaxRepValue`,
       IF(`StartItem` > 0, 1, 0),
       `SpecialFlags`,
       ''
FROM `quest_template`
WHERE MaxLevel != 0
   OR RequiredClasses != 0
   OR SourceSpellId != 0
   OR `PrevQuestId` != 0
   OR `NextQuestId` != 0
   OR `ExclusiveGroup` != 0
   OR `RewardMailTemplateId` != 0
   OR `RewardMailDelay` != 0
   OR `RequiredSkillId` != 0
   OR `RequiredSkillPoints` != 0
   OR `RequiredMinRepFaction` != 0
   OR `RequiredMaxRepFaction` != 0
   OR `RequiredMinRepValue` != 0
   OR `RequiredMaxRepValue` != 0
   OR `StartItem` != 0
   OR `SpecialFlags` != 0;

-- delete old fields
ALTER TABLE `quest_template`
    DROP `MaxLevel`,
    DROP `RequiredClasses`,
    DROP `SourceSpellId`,
    DROP `PrevQuestId`,
    DROP `NextQuestId`,
    DROP `ExclusiveGroup`,
    DROP `RewardMailTemplateId`,
    DROP `RewardMailDelay`,
    DROP `RequiredSkillId`,
    DROP `RequiredSkillPoints`,
    DROP `RequiredMinRepFaction`,
    DROP `RequiredMaxRepFaction`,
    DROP `RequiredMinRepValue`,
    DROP `RequiredMaxRepValue`,
    DROP `SpecialFlags`,
    DROP `RewardTalents`; -- this is the only field that is deleted without renaming, but it is only used by quest #1 which isn't in game
