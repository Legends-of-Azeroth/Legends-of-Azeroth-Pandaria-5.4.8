-- enlazar script a items
DELETE FROM `item_script_names` WHERE `Id` IN (110001, 110002, 110003, 110004);
INSERT INTO `item_script_names` (`Id`, `ScriptName`) VALUES
('110001','wow_token_1'),
('110002','wow_token_2'),
('110003','wow_token_5'),
('110004','wow_token_10');
