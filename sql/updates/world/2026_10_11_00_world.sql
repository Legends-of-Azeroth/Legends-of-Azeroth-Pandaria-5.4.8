-- Transmogrifier: the confirmation shown before an item is used as the new appearance, as a gossip menu option, so that every
-- language has its own text (gossip_menu_option_locale) and nothing is hard-coded in the script
DELETE FROM `gossip_menu_option` WHERE `MenuID` = 900200 AND `OptionID` = 0;
INSERT INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionBroadcastTextID`, `OptionType`, `OptionNpcflag`, `ActionMenuID`, `ActionPoiID`, `BoxCoded`, `BoxMoney`, `BoxText`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES
(900200, 0, 6, 'Use this item for transmogrification', 0, 1, 1, 0, 0, 0, 0, 'Using this item for transmogrify will bind it to you and make it non-refundable and non-tradeable.\nDo you wish to continue?', 0, 0);

DELETE FROM `gossip_menu_option_locale` WHERE `MenuID` = 900200 AND `OptionID` = 0;
INSERT INTO `gossip_menu_option_locale` (`MenuID`, `OptionID`, `Locale`, `OptionText`, `BoxText`) VALUES
(900200, 0, 'zhCN', '使用这件物品进行幻化', '是否幻化成这个造型，这会使这个物品变成绑定状态并不享受售卖退还服务。下面将显示物品和需要的金币数量！\n\n是否仍然继续？');
