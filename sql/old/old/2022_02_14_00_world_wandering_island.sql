-- Trader Feng
UPDATE `creature` SET `position_x` = 685.9648, `position_y` = 3457.286, `position_z` = 115.2643, `orientation` = 4.438516, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562782;

DELETE FROM `creature_template_addon` WHERE `entry` = 65087;
DELETE FROM `creature_addon` WHERE `guid` = 562782;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(562782, 562782, 1);

DELETE FROM `waypoint_data` WHERE `id` = 562782;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562782, 1, 682.5261, 3445.052, 111.3957, 0, 0, 0, 0, 100, 0, 65087),
(562782, 2, 677.9879, 3435.248, 108.3949, 0, 0, 0, 0, 100, 0, 65087),
(562782, 3, 672.0121, 3428.091, 105.7101, 0, 0, 0, 0, 100, 0, 65087),
(562782, 4, 665.7396, 3421.781, 103.6911, 0, 0, 0, 0, 100, 0, 65087),
(562782, 5, 672.0121, 3428.091, 105.7101, 0, 0, 0, 0, 100, 0, 65087),
(562782, 6, 677.9879, 3435.248, 108.3949, 0, 0, 0, 0, 100, 0, 65087),
(562782, 7, 682.5261, 3445.052, 111.3957, 0, 0, 0, 0, 100, 0, 65087),
(562782, 8, 685.9618, 3457.276, 115.4463, 0, 0, 0, 0, 100, 0, 65087),
(562782, 9, 688.4932, 3464.862, 117.3893, 0, 0, 0, 0, 100, 0, 65087),
(562782, 10, 695.3524, 3474.267, 118.2684, 0, 0, 0, 0, 100, 0, 65087),
(562782, 11, 707.7864, 3480.49, 119.6443, 0, 0, 0, 0, 100, 0, 65087),
(562782, 12, 718.7917, 3484.793, 123.47, 0, 0, 0, 0, 100, 0, 65087),
(562782, 13, 729.8662, 3489.422, 128.9295, 0, 0, 0, 0, 100, 0, 65087),
(562782, 14, 736.9288, 3492.556, 131.9822, 0, 0, 0, 0, 100, 0, 65087),
(562782, 15, 744.9202, 3499.066, 135.8553, 0, 0, 0, 0, 100, 0, 65087),
(562782, 16, 748.9774, 3506.485, 137.2696, 0, 0, 0, 0, 100, 0, 65087),
(562782, 17, 752.2049, 3515.733, 138.4052, 0, 0, 0, 0, 100, 0, 65087),
(562782, 18, 752.684, 3525.537, 138.9453, 0, 0, 0, 0, 100, 0, 65087),
(562782, 19, 750.6389, 3533.383, 139.3126, 0, 0, 0, 0, 100, 0, 65087),
(562782, 20, 746.3143, 3539.372, 139.5674, 0, 0, 0, 0, 100, 0, 65087),
(562782, 21, 741.3802, 3548.038, 140.4095, 0, 0, 0, 0, 100, 0, 65087),
(562782, 22, 739.0538, 3557.195, 140.6876, 0, 0, 0, 0, 100, 0, 65087),
(562782, 23, 737.0833, 3565.836, 140.6876, 0, 0, 0, 0, 100, 0, 65087),
(562782, 24, 735.7222, 3575.628, 140.6348, 0, 0, 0, 0, 100, 0, 65087),
(562782, 25, 735.6476, 3587.222, 140.6348, 0, 0, 0, 0, 100, 0, 65087),
(562782, 26, 734.0833, 3597.866, 140.6348, 0, 0, 0, 0, 100, 0, 65087),
(562782, 27, 731.5486, 3610.281, 140.7456, 0, 0, 0, 0, 100, 0, 65087),
(562782, 28, 734.8965, 3622.396, 140.6299, 0, 0, 0, 0, 100, 0, 65087),
(562782, 29, 734.934, 3627.23, 142.631, 0, 0, 0, 0, 100, 0, 65087),
(562782, 30, 736.5643, 3634.82, 142.6308, 0, 0, 0, 0, 100, 0, 65087),
(562782, 31, 737.8351, 3642.159, 142.6355, 0, 0, 0, 0, 100, 0, 65087),
(562782, 32, 736.5643, 3634.82, 142.6308, 0, 0, 0, 0, 100, 0, 65087),
(562782, 33, 734.934, 3627.23, 142.631, 0, 0, 0, 0, 100, 0, 65087),
(562782, 34, 734.375, 3621.875, 140.6299, 0, 0, 0, 0, 100, 0, 65087),
(562782, 35, 731.5486, 3610.281, 140.7456, 0, 0, 0, 0, 100, 0, 65087),
(562782, 36, 734.0833, 3597.866, 140.6348, 0, 0, 0, 0, 100, 0, 65087),
(562782, 37, 735.6476, 3587.222, 140.6348, 0, 0, 0, 0, 100, 0, 65087),
(562782, 38, 735.7222, 3575.628, 140.6348, 0, 0, 0, 0, 100, 0, 65087),
(562782, 39, 737.0833, 3565.836, 140.6876, 0, 0, 0, 0, 100, 0, 65087),
(562782, 40, 739.0538, 3557.195, 140.6876, 0, 0, 0, 0, 100, 0, 65087),
(562782, 41, 741.3802, 3548.038, 140.4095, 0, 0, 0, 0, 100, 0, 65087),
(562782, 42, 746.3143, 3539.372, 139.5674, 0, 0, 0, 0, 100, 0, 65087),
(562782, 43, 750.6389, 3533.383, 139.3126, 0, 0, 0, 0, 100, 0, 65087),
(562782, 44, 752.684, 3525.537, 138.9453, 0, 0, 0, 0, 100, 0, 65087),
(562782, 45, 752.2049, 3515.733, 138.4052, 0, 0, 0, 0, 100, 0, 65087),
(562782, 46, 748.9774, 3506.485, 137.2696, 0, 0, 0, 0, 100, 0, 65087),
(562782, 47, 744.9202, 3499.066, 135.8553, 0, 0, 0, 0, 100, 0, 65087),
(562782, 48, 736.9288, 3492.556, 131.9822, 0, 0, 0, 0, 100, 0, 65087),
(562782, 49, 730.1268, 3489.528, 129.2023, 0, 0, 0, 0, 100, 0, 65087),
(562782, 50, 718.7917, 3484.793, 123.47, 0, 0, 0, 0, 100, 0, 65087),
(562782, 51, 707.7864, 3480.49, 119.6443, 0, 0, 0, 0, 100, 0, 65087),
(562782, 52, 695.3524, 3474.267, 118.2684, 0, 0, 0, 0, 100, 0, 65087),
(562782, 53, 688.4965, 3464.872, 117.3337, 0, 0, 0, 0, 100, 0, 65087),
(562782, 54, 685.9648, 3457.286, 115.2643, 0, 0, 0, 0, 100, 0, 65087);

-- Groundskeeper Amalia
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562783;

DELETE FROM `creature_template_addon` WHERE `entry` = 65077;
DELETE FROM `creature_addon` WHERE `guid` = 562783;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(562783, 562783, 1);

DELETE FROM `waypoint_data` WHERE `id` = 562783;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562783, 1, 720.3629, 3517.311, 141.2257, 0, 0, 0, 0, 100, 0, 65077),
(562783, 2, 720.3629, 3517.311, 141.2257, 0, 4000, 0, 0, 100, 0, 65077),
(562783, 3, 735.2761, 3517.652, 140.2504, 0, 0, 0, 0, 100, 0, 65077),
(562783, 4, 739.3906, 3522.052, 140.4548, 0, 0, 0, 0, 100, 0, 65077),
(562783, 5, 739.8472, 3527.497, 140.6105, 0, 0, 0, 0, 100, 0, 65077),
(562783, 6, 736.4809, 3530.385, 140.7048, 0, 4000, 0, 0, 100, 0, 65077),
(562783, 7, 736.6268, 3537.718, 140.6876, 0, 0, 0, 0, 100, 0, 65077),
(562783, 8, 730.2899, 3540.156, 140.9006, 0, 5000, 0, 0, 100, 0, 65077),
(562783, 9, 722.0608, 3544.668, 141.283, 0, 0, 0, 0, 100, 0, 65077),
(562783, 10, 719.0677, 3543.64, 141.4743, 0, 4000, 0, 0, 100, 0, 65077),
(562783, 11, 716.3507, 3532.825, 141.6007, 0, 0, 0, 0, 100, 0, 65077),
(562783, 12, 713.0677, 3527.756, 141.1189, 0, 4000, 0, 0, 100, 0, 65077);

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 65077;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65077 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65077*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(65077, 0, 0, 0, 34, 0, 100, 0, 2, 1, 0, 0, 80, 65077*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Groundskeeper Amalia - MI 1 - Start Script"),
(65077, 0, 1, 0, 34, 0, 100, 0, 2, 5, 0, 0, 80, 65077*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Groundskeeper Amalia - MI 5 - Start Script"),
(65077, 0, 2, 0, 34, 0, 100, 0, 2, 7, 0, 0, 80, 65077*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Groundskeeper Amalia - MI 7 - Start Script"),
(65077, 0, 3, 0, 34, 0, 100, 0, 2, 9, 0, 0, 80, 65077*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Groundskeeper Amalia - MI 9 - Start Script"),
(65077, 0, 4, 0, 34, 0, 100, 0, 2, 11, 0, 0, 80, 65077*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Groundskeeper Amalia - MI 11 - Start Script"),

(65077*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 381, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Groundskeeper Amalia - On Script - Play Emote");

-- Jin
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 65085;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65085 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(65085, 0, 0, 0, 1, 0, 100, 0, 0, 0, 3000, 10000, 10, 5, 273, 396, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Jin - Update OOC - Play Random Emote (5, 273, 396)");

-- Da Na
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 65086;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65086 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(65086, 0, 0, 0, 1, 0, 100, 0, 3000, 5000, 3000, 10000, 10, 5, 273, 396, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Da Na - Update OOC - Play Random Emote (5, 273, 396)");

-- Gan Darkcask
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 65044;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65044 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(65044, 0, 0, 0, 1, 0, 100, 0, 0, 0, 3000, 10000, 10, 5, 273, 396, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Gan Darkcask - Update OOC - Play Random Emote (5, 273, 396)");

-- Gao Longwagon
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 65045;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65045 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(65045, 0, 0, 0, 1, 0, 100, 0, 3000, 5000, 3000, 10000, 10, 5, 273, 396, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Gao Longwagon - Update OOC - Play Random Emote (5, 273, 396)");

-- Lorewalker Amai
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (64875, 64881, 64880, 64879, 64876);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (64875, 64881, 64880, 64879, 64876) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (6487500, 6487600) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(64875, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 326000, 326000, 80, 6487500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Update OOC - Start Script"),

(6487500, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 4, 1, 0, 0, 0, 0, 10, 562813, 64876, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 4 1"),
(6487500, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 0"),
(6487500, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 1"),
(6487500, 9, 3, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 1"),
(6487500, 9, 4, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 2, 1, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 1"),
(6487500, 9, 5, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 2"),
(6487500, 9, 6, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 3"),
(6487500, 9, 7, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 4"),
(6487500, 9, 8, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 3, 1, 0, 0, 0, 0, 10, 562810, 64879, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 3 1"),
(6487500, 9, 9, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 5"),
(6487500, 9, 10, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 6"),
(6487500, 9, 11, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 7"),
(6487500, 9, 12, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 2, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 2"),
(6487500, 9, 13, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 5, 11, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Play Emote (11)"),
(6487500, 9, 14, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 8"),
(6487500, 9, 15, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 2"),
(6487500, 9, 16, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 3, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 3"),
(6487500, 9, 17, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 9, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 9"),
(6487500, 9, 18, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 4, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 4"),
(6487500, 9, 19, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 10"),
(6487500, 9, 20, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 11, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 11"),
(6487500, 9, 21, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 2, 3, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 3"),
(6487500, 9, 22, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 45, 1, 5, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 5"),
(6487500, 9, 23, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 1, 12, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 12"),
(6487500, 9, 24, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 13, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 13"),
(6487500, 9, 25, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 6, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 6"),
(6487500, 9, 26, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 45, 2, 4, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 4"),
(6487500, 9, 27, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 1, 14, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 14"),
(6487500, 9, 28, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 15, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 15"),
(6487500, 9, 29, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 3, 2, 0, 0, 0, 0, 10, 562810, 64879, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 3 2"),
(6487500, 9, 30, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 16, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 16"),
(6487500, 9, 31, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 17, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 17"),
(6487500, 9, 32, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 562810, 64879, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 3 3"),
(6487500, 9, 33, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 18, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 18"),
(6487500, 9, 34, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 19, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 19"),
(6487500, 9, 35, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 2, 5, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 5"),
(6487500, 9, 36, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 20"),
(6487500, 9, 37, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 7, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 7"),
(6487500, 9, 38, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 21, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 21"),
(6487500, 9, 39, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 3, 4, 0, 0, 0, 0, 10, 562810, 64879, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 3 4"),
(6487500, 9, 40, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 22, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 22"),
(6487500, 9, 41, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 2, 6, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 6"),
(6487500, 9, 42, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 23, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 23"),
(6487500, 9, 43, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 8, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 8"),
(6487500, 9, 44, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 45, 2, 7, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 7"),
(6487500, 9, 45, 0, 0, 0, 100, 0, 2500, 2500, 0, 0, 45, 1, 9, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 9"),
(6487500, 9, 46, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 45, 3, 5, 0, 0, 0, 0, 10, 562810, 64879, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 3 5"),
(6487500, 9, 47, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 24, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 24"),
(6487500, 9, 48, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 25"),
(6487500, 9, 49, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 26, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 26"),
(6487500, 9, 50, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 27, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 27"),
(6487500, 9, 51, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 28, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 28"),
(6487500, 9, 52, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 29, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 29"),
(6487500, 9, 53, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 45, 4, 2, 0, 0, 0, 0, 10, 562813, 64876, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 4 2"),
(6487500, 9, 54, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 30"),
(6487500, 9, 55, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 31, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 31"),
(6487500, 9, 56, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 10, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 10"),
(6487500, 9, 57, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 45, 2, 8, 0, 0, 0, 0, 10, 562815, 64880, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 2 8"),
(6487500, 9, 58, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 32, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 32"),
(6487500, 9, 59, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 11, 0, 0, 0, 0, 10, 562807, 64881, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 1 11"),
(6487500, 9, 60, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 33, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 33"),
(6487500, 9, 61, 0, 0, 0, 100, 0, 4500, 4500, 0, 0, 1, 34, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 34"),
(6487500, 9, 62, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 45, 3, 6, 0, 0, 0, 0, 10, 562810, 64879, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 3 6"),
(6487500, 9, 63, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 35"),
(6487500, 9, 64, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 36"),
(6487500, 9, 65, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 1, 37, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 37"),
(6487500, 9, 66, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 4.380776, "Lorewalker Amai - Set Orientation (4.380776)"),
(6487500, 9, 67, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 38, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 38"),
(6487500, 9, 68, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 39, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Amai - Say Text Line 39"),
(6487500, 9, 69, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 45, 4, 3, 0, 0, 0, 0, 10, 562813, 64876, 0, 0, 0, 0, 0, "Lorewalker Amai - Set Data 4 3"),
(6487500, 9, 70, 0, 0, 0, 100, 0, 1500, 1500, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.821526, "Lorewalker Amai - Set Orientation (5.821526)"),
-- Hao
(64881, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 1 - Say Text Line 0"),
(64881, 0, 1, 0, 38, 0, 100, 0, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 2 - Say Text Line 1"),
(64881, 0, 2, 0, 38, 0, 100, 0, 1, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 3 - Say Text Line 2"),
(64881, 0, 3, 0, 38, 0, 100, 0, 1, 4, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 4 - Say Text Line 3"),
(64881, 0, 4, 0, 38, 0, 100, 0, 1, 5, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 5 - Say Text Line 4"),
(64881, 0, 5, 0, 38, 0, 100, 0, 1, 6, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 6 - Say Text Line 5"),
(64881, 0, 6, 0, 38, 0, 100, 0, 1, 7, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 7 - Say Text Line 6"),
(64881, 0, 7, 0, 38, 0, 100, 0, 1, 8, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 8 - Say Text Line 7"),
(64881, 0, 8, 0, 38, 0, 100, 0, 1, 9, 0, 0, 1, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 9 - Say Text Line 8"),
(64881, 0, 9, 0, 38, 0, 100, 0, 1, 10, 0, 0, 1, 9, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 10 - Say Text Line 9"),
(64881, 0, 10, 0, 38, 0, 100, 0, 1, 11, 0, 0, 1, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Hao - On Data Set 1 11 - Say Text Line 10"),
-- Nan
(64880, 0, 0, 0, 38, 0, 100, 0, 2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 1 - Say Text Line 0"),
(64880, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 2 - Say Text Line 1"),
(64880, 0, 2, 0, 38, 0, 100, 0, 2, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 3 - Say Text Line 2"),
(64880, 0, 3, 0, 38, 0, 100, 0, 2, 4, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 4 - Say Text Line 3"),
(64880, 0, 4, 0, 38, 0, 100, 0, 2, 5, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 5 - Say Text Line 4"),
(64880, 0, 5, 0, 38, 0, 100, 0, 2, 6, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 6 - Say Text Line 5"),
(64880, 0, 6, 0, 38, 0, 100, 0, 2, 7, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 7 - Say Text Line 6"),
(64880, 0, 7, 0, 38, 0, 100, 0, 2, 8, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Nan - On Data Set 2 8 - Say Text Line 7"),
-- Yin
(64879, 0, 0, 0, 38, 0, 100, 0, 3, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Yin - On Data Set 3 1 - Say Text Line 0"),
(64879, 0, 1, 0, 38, 0, 100, 0, 3, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Yin - On Data Set 3 2 - Say Text Line 1"),
(64879, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Yin - On Data Set 3 3 - Say Text Line 2"),
(64879, 0, 3, 0, 38, 0, 100, 0, 3, 4, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Yin - On Data Set 3 4 - Say Text Line 3"),
(64879, 0, 4, 0, 38, 0, 100, 0, 3, 5, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Yin - On Data Set 3 5 - Say Text Line 4"),
(64879, 0, 5, 0, 38, 0, 100, 0, 3, 6, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Yin - On Data Set 3 6 - Say Text Line 5"),
-- Lorewalker Ruolin
(64876, 0, 0, 1, 38, 0, 100, 0, 4, 1, 0, 0, 11, 126804, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Ruolin - On Data Set 4 1 - Cast Spell 'Singing'"),
(64876, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 6487600, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Ruolin - On Data Set 4 1 (Link) - Start Script"),
(64876, 0, 2, 0, 38, 0, 100, 0, 4, 2, 0, 0, 28, 126804, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Ruolin - On Data Set 4 2 - Remove Aura"),
(64876, 0, 3, 0, 38, 0, 100, 0, 4, 3, 0, 0, 5, 273, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Ruolin - On Data Set 4 3 - Play Emote (273)"),

(6487600, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 4, 30627, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lorewalker Ruolin - Play Sound (30627)");

DELETE FROM `creature_text` WHERE `entry` IN (64875, 64881, 64880, 64879);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(64875, 0, 0, "", "This is the Song of Liu Lang, the first pandaren explorer.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 1, 0, "", "The song is written in the old tongue, the language of emperors and scholars. Hardly anyone speaks it anymore.", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),
(64875, 2, 0, "", "It is all about his adventures.", 12, 0, 100, 273, 0, 0, "Lorewalker Amai"),
(64875, 3, 0, "", "Brave Liu Lang set out to explore the world on the back of a sea turtle.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 4, 0, "", "Does anyone remember the turtle's name? Yin?", 12, 0, 100, 25, 0, 0, "Lorewalker Amai"),
(64875, 5, 0, "", "That's right, the turtle was called Shen-zin Su. At first, he was only big enough for Liu Lang to sit on.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 6, 0, "", "He grew and grew and grew, SO big, that now some people call him \"The Wandering Isle.\"", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 7, 0, "", "Question, Hao?", 12, 0, 100, 25, 0, 0, "Lorewalker Amai"),
(64875, 8, 0, "", "Yes, Hao - our home is on Shen-zin Su's back.", 12, 0, 100, 273, 0, 0, "Lorewalker Amai"),
(64875, 9, 0, "", "Liu Lang discovered many things as he explored the world - oh - yes, Hao?", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),
(64875, 10, 0, "", "That's a good question. Nobody has spoken to Shen-zin Su for many generations. He only ever spoke to Liu Lang.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 11, 0, "", "But I am certain that Shen-zin Su knows we are here. And he cares for every one of us!", 12, 0, 100, 66, 0, 0, "Lorewalker Amai"),
(64875, 12, 0, "", "Especially Hao! Liu Lang once said, \"Noble is he who always asks questions.\"", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),
(64875, 13, 0, "", "\"We should all be like children, for the world is our elder, and has many things to teach us.\"", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 14, 0, "", "Students! Students! Pay attention now.", 12, 0, 100, 22, 0, 0, "Lorewalker Amai"),
(64875, 15, 0, "", "Does anyone know how often Liu Lang went back to Pandaria?", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 16, 0, "", "Haha, no Yin. Liu Lang would eat wherever he explored.", 12, 0, 100, 11, 0, 0, "Lorewalker Amai"),
(64875, 17, 0, "", "He returned to the main continent of Pandaria once every five years.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 18, 0, "", "You are right, Yin. It IS hidden away, behind a cloak of mists.", 12, 0, 100, 274, 0, 0, "Lorewalker Amai"),
(64875, 19, 0, "", "Does anyone remember how Liu Lang was always able to find it? Nan?", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),
(64875, 20, 0, "", "That's right! Liu Lang always had a way back home. Every five years, he would return, to pick up more explorers.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 21, 0, "", "Yes Hao! Only the BRAVEST pandaren joined Liu Lang on his turtle to explore the world.", 12, 0, 100, 273, 0, 0, "Lorewalker Amai"),
(64875, 22, 0, "", "Shen-zin Su hasn't made landfall for a long time. But that hasn't stopped some pandaren from exploring, anyway.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 23, 0, "", "Yes, like Chen and Li Li! They're out, exploring the world. Maybe someday you will, too.", 12, 0, 100, 21, 0, 0, "Lorewalker Amai"),
(64875, 24, 0, "", "I'm glad you asked, Yin. Liu Lang explored the world his entire life, and raised many children here on the Wandering Isle.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 25, 0, "", "Eventually he grew very old, and visited Pandaria one final time.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 26, 0, "", "But nobody else wanted to go exploring with him. So he left.", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),
(64875, 27, 0, "", "Shen-zin Su has never returned to Pandaria.", 12, 0, 100, 274, 0, 0, "Lorewalker Amai"),
(64875, 28, 0, "", "Tired now, Liu Lang said goodbye to Shen-zin Su, his oldest friend.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 29, 0, "", "Then, Liu Lang went up to the Wood of Staves, carrying with him a bamboo umbrella that he always took on his adventures.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 30, 0, "", "He opened his umbrella, planted it in the ground, and sat underneath its cool shade.", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),
(64875, 31, 0, "", "He closed his eyes, and became one with the land. And then - do you know what happened?", 12, 0, 100, 2, 0, 0, "Lorewalker Amai"),
(64875, 32, 0, "", "His umbrella... sprouted! It grew roots, flowered, and became a tree!", 12, 0, 100, 5, 0, 0, "Lorewalker Amai"),
(64875, 33, 0, "", "Well, it's true. If you ever go to the Wood of Staves, you can see it.", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 34, 0, "", "Along with the budding staves of all the elders who came before us, growing now into giant trees.", 12, 0, 100, 274, 0, 0, "Lorewalker Amai"),
(64875, 35, 0, "", "It's not a sad story! Liu Lang himself said, \"Never mourn a life well-lived.\"", 12, 0, 100, 1, 0, 0, "Lorewalker Amai"),
(64875, 36, 0, "", "And I think he lived a very very good life. We owe everything we have to Liu Lang, the first pandaren explorer.", 12, 0, 100, 273, 0, 0, "Lorewalker Amai"),
(64875, 37, 0, "", "We tell his story, so that he will always be remembered.", 12, 0, 100, 2, 0, 0, "Lorewalker Amai"),
(64875, 38, 0, "", "Thank you, Ruolin! That was beautiful.", 12, 0, 100, 21, 0, 0, "Lorewalker Amai"),
(64875, 39, 0, "", "Could you sing it once more, for the students who just arrived?", 12, 0, 100, 6, 0, 0, "Lorewalker Amai"),

(64881, 0, 0, "Why can't I understand any of the words?", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 1, 0, "We live on the back of a big turtle?!", "", 12, 0, 100, 5, 0, 0, "Hao"),
(64881, 2, 0, "I just thought it was weird that the mountains had flippers!", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 3, 0, "Does the turtle know we're here?", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 4, 0, "Hey!", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 5, 0, "Yeah, you should all be more like me.", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 6, 0, "Does that mean our moms and dads and grandparents were all explorers, too?", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 7, 0, "I want to explore the world!", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 8, 0, "Oh. Well, forget it then.", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 9, 0, "What?", "", 12, 0, 100, 1, 0, 0, "Hao"),
(64881, 10, 0, "That's un-possible!", "", 12, 0, 100, 1, 0, 0, "Hao"),

(64880, 0, 0, "", "What is the song about?", 12, 0, 100, 1, 0, 0, "Nan"),
(64880, 1, 0, "", "Sheesh, Hao. Haven't you ever paid attention?", 12, 0, 100, 1, 0, 0, "Nan"),
(64880, 2, 0, "", "Even Hao?", 12, 0, 100, 6, 0, 0, "Nan"),
(64880, 3, 0, "", "Blerch! I'd rather jump off this bridge.", 12, 0, 100, 1, 0, 0, "Nan"),
(64880, 4, 0, "", "Because sea turtles always return to the beach where they were born!", 12, 0, 100, 1, 0, 0, "Nan"),
(64880, 5, 0, "", "Like Chen and Li Li Stormstout!", 12, 0, 100, 1, 0, 0, "Nan"),
(64880, 6, 0, "", "No you don't. It's full of trolls.", 12, 0, 100, 1, 0, 0, "Nan"),
(64880, 7, 0, "", "What? What?", 12, 0, 100, 1, 0, 0, "Nan"),

(64879, 0, 0, "", "Shen-zin Su!", 12, 0, 100, 1, 0, 0, "Yin"),
(64879, 1, 0, "", "...Whenever he got hungry?", 12, 0, 100, 1, 0, 0, "Yin"),
(64879, 2, 0, "", "But isn't Pandaria hidden somewhere? Nobody can find it!", 12, 0, 100, 1, 0, 0, "Yin"),
(64879, 3, 0, "", "But we don't explore very much anymore, do we?", 12, 0, 100, 1, 0, 0, "Yin"),
(64879, 4, 0, "", "Lorewalker! What ever happened to Liu Lang?", 12, 0, 100, 1, 0, 0, "Yin"),
(64879, 5, 0, "", "That's a sad story!", 12, 0, 100, 1, 0, 0, "Yin");

-- Vision of Dafeng & Vision of Zhao-Ren
UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` IN (65559, 65560);
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 563687;

DELETE FROM `creature_template_addon` WHERE `entry` IN (65559, 65560);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `bytes1`, `bytes2`) VALUES
(65559, 563687, 50331648, 1),
(65560, 0, 50331648, 1);

DELETE FROM `waypoint_data` WHERE `id` = 563687;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(563687, 1, 1163.323, 4094.89, 196.7238, 0, 0, 1, 0, 100, 0, 65560),
(563687, 2, 1160.189, 4088.899, 196.8717, 0, 0, 1, 0, 100, 0, 65560),
(563687, 3, 1156.356, 4080.826, 199.6824, 0, 0, 1, 0, 100, 0, 65560),
(563687, 4, 1157.795, 4074.997, 201.5452, 0, 0, 1, 0, 100, 0, 65560),
(563687, 5, 1162.868, 4072.484, 203.9529, 0, 0, 1, 0, 100, 0, 65560),
(563687, 6, 1170.455, 4076.58, 206.3303, 0, 0, 1, 0, 100, 0, 65560),
(563687, 7, 1173.443, 4088.29, 209.1731, 0, 0, 1, 0, 100, 0, 65560),
(563687, 8, 1171.333, 4097.327, 209.1731, 0, 0, 1, 0, 100, 0, 65560),
(563687, 9, 1170.729, 4107.088, 208.22, 0, 0, 1, 0, 100, 0, 65560),
(563687, 10, 1177.141, 4113.896, 207.1179, 0, 0, 1, 0, 100, 0, 65560),
(563687, 11, 1185.208, 4111.542, 205.3374, 0, 0, 1, 0, 100, 0, 65560),
(563687, 12, 1186.818, 4103.221, 202.7245, 0, 0, 1, 0, 100, 0, 65560),
(563687, 13, 1177.809, 4099.19, 202.0769, 0, 0, 1, 0, 100, 0, 65560),
(563687, 14, 1169.811, 4098.712, 199.5409, 0, 0, 1, 0, 100, 0, 65560);

DELETE FROM `creature_formations` WHERE `leaderGUID` = 563687;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`, `point_1`, `point_2`) VALUES
(563687, 563687, 0, 0, 2, 0, 0),
(563687, 563683, 10, 0, 2, 0, 0);
