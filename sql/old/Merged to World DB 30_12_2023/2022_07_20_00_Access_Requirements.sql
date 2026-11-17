UPDATE `access_requirement` SET `completed_achievement` = 6689 WHERE `mapId` = 996 AND (`difficulty` = "RAID_10MAN_HEROIC" OR `difficulty` = "RAID_25MAN_HEROIC");

DELETE FROM `trinity_string` WHERE `entry` IN (15000, 15001, 15002, 15003, 15004, 15005);
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc8`) VALUES 
(15000, "You must clear Terrace of Endless Spring on normal before progressing to Heroic difficulty.", "Вы должны пройти «Терасса Вечной Весны» в обычном режиме, прежде чем переходить на героический уровень сложности."),
(15001, "You must complete Heart of Fear in normal difficulty.", "Вы должны пройти «Сердце Страха» в обычном режиме."),
(15002, "You must complete Heart of Fear on heroic difficulty.", "Вы должны пройти «Сердце Страха» в героическом режиме."),
(15003, "You must complete Mogu'shan Vaults in normal difficulty.", "Вы должны пройти «Подземелья Могу'шан» в обычном режиме."),
(15004, "You must complete Mogu'shan Vaults on heroic difficulty.", "Вы должны пройти «Подземелья Могу'шан» в героическом режиме.");
