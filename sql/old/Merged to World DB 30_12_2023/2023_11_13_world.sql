-- Creature text for Elaine Trias(483)
DELETE FROM `creature_text` WHERE `CreatureID`=483;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(483, 0, 0, 'Good day, Corbett.  Here\'s your cheese, fresh made this morning!  And how are things at your shop?', 12, 7, 100, 0, 0, 0, 286, 0, 'Elaine Trias'),
(483, 0, 1, 'Hi Corbett!  Here, you go!  I trust business is faring well at your clothier shop...?', 12, 7, 100, 0, 0, 0, 287, 0, 'Elaine Trias');

-- Creature text for Ozzie Togglevolt(1268)
DELETE FROM `creature_text` WHERE `CreatureID`=1268;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(1268, 0, 0, 'Hmm, I suppose it could work.  But it could really use a little more umph!', 12, 0, 100, 0, 0, 0, 39711, 0, 'Ozzie Togglevolt reply'),
(1268, 1, 0, 'Precisely.', 12, 7, 100, 273, 0, 0, 1891, 0, 'Ozzie Togglevolt'),
(1268, 1, 1, 'My thoughts exactly.', 12, 7, 100, 273, 0, 0, 1892, 0, 'Ozzie Togglevolt'),
(1268, 1, 2, 'Couldn\'t have said it better myself.', 12, 7, 100, 273, 0, 0, 1893, 0, 'Ozzie Togglevolt'),
(1268, 1, 3, 'Most definitely.', 12, 7, 100, 273, 0, 0, 1894, 0, 'Ozzie Togglevolt'),
(1268, 1, 4, 'I was thinking the same thing.', 12, 7, 100, 273, 0, 0, 1895, 0, 'Ozzie Togglevolt');

-- Creature text for Harlan Bagley(1427)
DELETE FROM `creature_text` WHERE `CreatureID`=1427;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(1427, 0, 0, 'Ah, much appreciated, Corbett.  We\'ll get these on the racks immediately.', 12, 7, 100, 0, 0, 0, 261, 0, 'Harlan Bagley'),
(1427, 0, 1, 'Ah yes, and promptly delivered.  As always, it\'s a pleasure doing business with you, Corbett.', 12, 7, 100, 0, 0, 0, 262, 0, 'Harlan Bagley');

-- SmartAI for Veraku(18554)
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (18554,1855400);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(18554, 0, 0, 0, 0, 0, 100, 0, 3000, 5000, 40000, 45000, 0, 11, 15277, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - IC - Cast Seal of Reckoning'),
(18554, 0, 1, 0, 2, 0, 100, 0, 0, 40, 15000, 20000, 0, 11, 13952, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Below 40% HP - Cast Holy Light'),
(18554, 0, 2, 3, 4, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Aggro - Say Line 0'),
(18554, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 21504, 50, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Aggro - Set Data'),
(18554, 0, 4, 0, 1, 0, 100, 0, 30000, 30000, 120000, 120000, 0, 80, 1855400, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - OOC - RunScript'),
(1855400, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 21504, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0, -2293.52, 3091.34, 152.818, 6.17546, 'Sharth Voldoun - On Script - Summon Pathaleon the Calculators Image'),
(1855400, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Set Flag Standstate \'Kneel\''),
(1855400, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 1'),
(1855400, 9, 3, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 13, 0, 0, 0, 0, 0, 19, 21504, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 13 (Pathaleon the Calculators Image)'),
(1855400, 9, 4, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Remove Flag Standstate \'Kneel\''),
(1855400, 9, 5, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 2'),
(1855400, 9, 6, 0, 0, 0, 100, 0, 14000, 14000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 3'),
(1855400, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 19, 21504, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 4 (Pathaleon the Calculators Image)'),
(1855400, 9, 8, 0, 0, 0, 100, 0, 16000, 16000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 19, 21504, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 5 (Pathaleon the Calculators Image)'),
(1855400, 9, 9, 0, 0, 0, 100, 0, 16000, 16000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 19, 21504, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 6 (Pathaleon the Calculators Image)'),
(1855400, 9, 10, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 4'),
(1855400, 9, 11, 0, 0, 0, 100, 0, 13000, 13000, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 19, 21504, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Say Line 7 (Pathaleon the Calculators Image)'),
(1855400, 9, 12, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 21504, 0, 0, 0, 0, 0, 0, 0, 'Sharth Voldoun - On Script - Set Data to Pathaleon the Calculators Image');

-- Path for Whirligig Wafflefry(25885)
UPDATE `creature_addon` SET `path_id` = 1007340 WHERE `guid` = 100734;
DELETE FROM `waypoint_data` WHERE `id`=1007340;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`) VALUES 
(1007340, 1, -1918.05, 5334.41, -12.428, 0, 60000, 0, 0, 100, 0),
(1007340, 2, -1918.05, 5334.41, -12.428, 0, 60000, 0, 0, 100, 0),
(1007340, 3, -1918.05, 5334.41, -12.428, 0, 60000, 0, 0, 100, 0),
(1007340, 4, -1918.05, 5334.41, -12.428, 0, 25000, 0, 456, 100, 0),
(1007340, 5, -1918.05, 5334.41, -12.428, 0, 25000, 0, 457, 100, 0),
(1007340, 6, -1918.05, 5334.41, -12.428, 0, 25000, 0, 458, 100, 0),
(1007340, 7, -1918.05, 5334.41, -12.428, 0, 25000, 0, 459, 100, 0),
(1007340, 8, -1918.05, 5334.41, -12.428, 0, 60000, 0, 0, 100, 0),
(1007340, 9, -1918.05, 5334.41, -12.428, 0, 60000, 0, 0, 100, 0),
(1007340, 10, -1918.05, 5334.41, -12.428, 0, 60000, 0, 0, 100, 0);

-- Remove non-exist command
DELETE FROM `command` WHERE `name` IN ('guard','isengard');

-- Remove non-billzard creature entry
DELETE FROM `creature_template` WHERE `entry` IN (99418,100290,190000,190001,190002,190003,190004,190005,190006,190007,190008,190009,190010);
DELETE FROM `creature_template_addon` WHERE `entry` IN (99418,100290,190000,190001,190002,190003,190004,190005,190006,190007,190008,190009,190010);
DELETE FROM `creature_addon` WHERE `guid` IN (SELECT `guid` FROM `creature` WHERE `id` IN (99418,100290,190000,190001,190002,190003,190004,190005,190006,190007,190008,190009,190010));
DELETE FROM `creature` WHERE `id` IN (99418,100290,190000,190001,190002,190003,190004,190005,190006,190007,190008,190009,190010);
UPDATE `creature_template` SET `ScriptName`='' WHERE `ScriptName`='npc_premium';
UPDATE `creature_template` SET `ScriptName`='' WHERE `ScriptName`='0';
UPDATE `creature_template` SET `ScriptName`='' WHERE `ScriptName`='1';

-- Remove non-billzard creature item
DELETE FROM `battle_pet_item_to_species` WHERE `itemId` IN (64439,100908);

-- Kologarn(32930) flags_extra
UPDATE `creature_template` SET `flags_extra`=524289 WHERE `entry`=32930;
-- Oracle Orphan(34519) flags_extra
UPDATE `creature_template` SET `flags_extra`=536870976 WHERE `entry`=34519;