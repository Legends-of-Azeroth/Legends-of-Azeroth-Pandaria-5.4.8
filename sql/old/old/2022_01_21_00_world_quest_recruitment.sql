UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 1740;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 1740 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (1740*100, 1740*100+01) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(1740, 0, 0, 0, 19, 0, 100, 0, 26800, 0, 0, 0, 80, 1740*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Saltain - On Quest Accept (26800) - Start Script"),
(1740, 0, 1, 0, 20, 0, 100, 0, 26800, 0, 0, 0, 80, 1740*100+01, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Saltain - On Quest Reward (26800) - Start Script"),
(1740, 0, 2, 0, 20, 0, 100, 0, 25089, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 49337, 20, 0, 0, 0, 0, 0, "Saltain - On Quest Reward (25089) - Set Data 3 3 To Creature ID: 49337"),

(1740*100, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 85, 91938, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Saltain - On Script - Cast Spell To Summon"),

(1740*100+01, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 49337, 20, 0, 0, 0, 0, 0, "Saltain - On Script - Set Data 3 3 To Creature ID: 49337"),
(1740*100+01, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 11, 49340, 20, 0, 0, 0, 0, 0, "Saltain - On Script - Set Data 1 1 To Creature ID: 49340");

DELETE FROM `smart_scripts` WHERE `entryorguid` = 49340 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49340*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49340, 0, 0, 1, 75, 0, 100, 1, 0, 49337, 3, 0, 86, 91945, 2, 19, 49337, 20, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corpse - On Distance Creature (49337) - Cross Cast Spell"),
(49340, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 46598, 2, 0, 0, 0, 0, 19, 49337, 20, 0, 0, 0, 0, 0, "Corpse - On Distance Creature (49337) (Link) - Cast Spell"),
(49340, 0, 2, 0, 38, 0, 100, 1, 1, 1, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corpse - On Data Set 1 1 - Despawn Self");
