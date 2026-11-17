-- fix wrong equipment_id
UPDATE `creature` SET `equipment_id`=0 WHERE `guid`=17;

-- Add Portal to Moonglade (Area: The Eastern Earthshrine)
SET @OGUID:=32;
DELETE FROM `gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+0;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `ScriptName`) VALUES 
(@OGUID+0, 206110, 0, 1519, 5428, 1, 1, 0, -8339.8837890625, 288.7882080078125, 156.832550048828125, 5.037568092346191406, 0, 0, -0.58331871032714843, 0.812243342399597167, 120, 255, 1, ''); -- Portal to Moonglade (Area: The Eastern Earthshrine - Difficulty: 0) CreateObject1

-- Add Baradin Guard(48253) WP
SET @CGUID := 235384;
SET @PATH := @CGUID * 10;
DELETE FROM `waypoint_data` WHERE `id`= @PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`) VALUES
(@PATH, 0, -367.4618, 1060.483, 21.78117, 0, 0),
(@PATH, 1, -363.8108, 1065.535, 21.7774, 0, 0),
(@PATH, 2, -358.8576, 1068.865, 21.7958, 0, 0),
(@PATH, 3, -363.8108, 1065.535, 21.7774, 0, 0),
(@PATH, 4, -367.4618, 1060.483, 21.78117, 0, 0),
(@PATH, 5, -366.533, 1056.254, 21.88419, 0, 0),
(@PATH, 6, -363.1406, 1049.899, 22.00154, 0, 0),
(@PATH, 7, -359.6632, 1045.901, 21.83958, 0, 0),
(@PATH, 8, -354.3264, 1043.92, 21.88429, 0, 0),
(@PATH, 9, -344.3264, 1042.731, 21.55309, 0, 0),
(@PATH, 10, -336.3333, 1045.595, 21.77679, 0, 0),
(@PATH, 11, -332.5243, 1054.642, 21.90986, 0, 0),
(@PATH, 12, -334.0365, 1060.42, 21.77668, 0, 0),
(@PATH, 13, -338.7743, 1068.306, 21.41007, 0, 0),
(@PATH, 14, -343.7083, 1073.292, 21.3137, 0, 0),
(@PATH, 15, -350.6337, 1072.78, 21.48847, 0, 0),
(@PATH, 16, -358.6962, 1069.132, 21.79697, 0, 0),
(@PATH, 17, -350.6337, 1072.78, 21.48847, 0, 0),
(@PATH, 18, -343.7083, 1073.292, 21.3137, 0, 0),
(@PATH, 19, -338.7743, 1068.306, 21.41007, 0, 0),
(@PATH, 20, -334.0365, 1060.42, 21.77668, 0, 0),
(@PATH, 21, -332.5243, 1054.642, 21.90986, 0, 0),
(@PATH, 22, -336.3333, 1045.595, 21.77679, 0, 0),
(@PATH, 23, -344.3264, 1042.731, 21.55309, 0, 0),
(@PATH, 24, -354.3264, 1043.92, 21.88429, 0, 0),
(@PATH, 25, -359.6632, 1045.901, 21.83958, 0, 0),
(@PATH, 26, -363.1406, 1049.899, 22.00154, 0, 0),
(@PATH, 27, -366.533, 1056.254, 21.88419, 0, 0),
(@PATH, 28, -367.4618, 1060.483, 21.78117, 0, 0),
(@PATH, 29, -363.8108, 1065.535, 21.7774, 0, 0),
(@PATH, 30, -358.8576, 1068.865, 21.7958, 0, 0),
(@PATH, 31, -363.8108, 1065.535, 21.7774, 0, 0),
(@PATH, 32, -367.4618, 1060.483, 21.78117, 0, 0),
(@PATH, 33, -366.533, 1056.254, 21.88419, 0, 0),
(@PATH, 34, -363.1406, 1049.899, 22.00154, 0, 0),
(@PATH, 35, -359.6632, 1045.901, 21.83958, 0, 0),
(@PATH, 36, -354.3264, 1043.92, 21.88429, 0, 0),
(@PATH, 37, -344.3264, 1042.731, 21.55309, 0, 0),
(@PATH, 38, -336.3333, 1045.595, 21.77679, 0, 0),
(@PATH, 39, -332.5243, 1054.642, 21.90986, 0, 0),
(@PATH, 40, -334.0365, 1060.42, 21.77668, 0, 0),
(@PATH, 41, -338.7743, 1068.306, 21.41007, 0, 0),
(@PATH, 42, -334.0365, 1060.42, 21.77668, 0, 0),
(@PATH, 43, -338.7743, 1068.306, 21.41007, 0, 0),
(@PATH, 44, -343.7083, 1073.292, 21.3137, 0, 0),
(@PATH, 45, -350.6337, 1072.78, 21.48847, 0, 0),
(@PATH, 46, -358.6962, 1069.132, 21.79697, 0, 0),
(@PATH, 47, -350.6337, 1072.78, 21.48847, 0, 0),
(@PATH, 48, -343.7083, 1073.292, 21.3137, 0, 0),
(@PATH, 49, -338.7743, 1068.306, 21.41007, 0, 0),
(@PATH, 50, -334.0365, 1060.42, 21.77668, 0, 0),
(@PATH, 51, -332.5243, 1054.642, 21.90986, 0, 0),
(@PATH, 52, -336.3333, 1045.595, 21.77679, 0, 0),
(@PATH, 53, -344.3264, 1042.731, 21.55309, 0, 0),
(@PATH, 54, -354.3264, 1043.92, 21.88429, 0, 0),
(@PATH, 55, -359.6632, 1045.901, 21.83958, 0, 0),
(@PATH, 56, -363.1406, 1049.899, 22.00154, 0, 0),
(@PATH, 57, -366.533, 1056.254, 21.88419, 0, 0),
(@PATH, 58, -367.4618, 1060.483, 21.78117, 0, 0),
(@PATH, 59, -363.8108, 1065.535, 21.7774, 0, 0),
(@PATH, 60, -358.8576, 1068.865, 21.7958, 0, 0),
(@PATH, 61, -363.8108, 1065.535, 21.7774, 0, 0),
(@PATH, 62, -367.4618, 1060.483, 21.78117, 0, 0),
(@PATH, 63, -366.533, 1056.254, 21.88419, 0, 0),
(@PATH, 64, -363.1406, 1049.899, 22.00154, 0, 0),
(@PATH, 65, -359.6632, 1045.901, 21.83958, 0, 0),
(@PATH, 66, -354.3264, 1043.92, 21.88429, 0, 0),
(@PATH, 67, -344.3264, 1042.731, 21.55309, 0, 0),
(@PATH, 68, -336.3333, 1045.595, 21.77679, 0, 0),
(@PATH, 69, -332.5243, 1054.642, 21.90986, 0, 0);

UPDATE `creature` SET `position_x`= -367.4618, `position_y`= 1060.483, `position_z`= 21.78117, `orientation`= 0, `wander_distance`= 0, `movement_type`= 2 WHERE `guid`= @CGUID;
DELETE FROM `creature_addon` WHERE `guid`= @CGUID;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(@CGUID, @PATH, 1);

-- Edna Mullby (1286)
DELETE FROM `creature_text` WHERE `CreatureID`=1286;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(1286, 0, 1, 'Welcome.', 12, 7, 100, 3, 0, 0, 0, 32936, 0, 'Edna Mullby - Random Say on Aggro'),
(1286, 0, 2, 'Welcome. May I help you find something?', 12, 7, 100, 3, 0, 0, 0, 43336, 0, 'Edna Mullby - Random Say on Aggro'),
(1286, 0, 3, 'Greetings! Please have a look around.', 12, 7, 100, 3, 0, 0, 0, 43333, 0, 'Edna Mullby - Random Say on Aggro'),
(1286, 0, 4, 'Greetings.', 12, 7, 100, 3, 0, 0, 0, 43337, 0, 'Edna Mullby - Random Say on Aggro'),
(1286, 0, 5, 'Greetings, $c.', 12, 7, 100, 3, 0, 0, 0, 43330, 0, 'Edna Mullby - Random Say on Aggro'),
(1286, 0, 6, 'Let me know if you need help finding anything, $c.', 12, 7, 100, 3, 0, 0, 0, 43335, 0, 'Edna Mullby - Random Say on Aggro'),
(1286, 1, 1, 'Hello there, $n.  Happy Pilgrim\'s Bounty!', 12, 7, 100, 3, 0, 0, 0, 49904, 0, 'Edna Mullby - Random Say on Aggro'), -- Pilgrim's Bounty Text
(1286, 1, 2, 'Are you enjoying Pilgrim\'s Bounty, $n?  Let me know if you need help finding anything.', 12, 7, 100, 3, 0, 0, 0, 49902, 0, 'Edna Mullby - Random Say on Aggro'); -- Pilgrim's Bounty Text

DELETE FROM `smart_scripts` WHERE `entryorguid`=1286;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(1286, 0, 0, 0, 1, 0, 100, 0, 10000, 15000, 40000, 60000, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Edna Mullby - Out of Combat - Say Line 0');

UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=1286;

-- Marda Weller (1287)
DELETE FROM `creature_text` WHERE `CreatureID`=1287;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(1287, 0, 1, 'Welcome.', 12, 7, 100, 3, 0, 0, 0, 32936, 0, 'Marda Weller - Random Say on Aggro'),
(1287, 0, 2, 'Welcome. May I help you find something?', 12, 7, 100, 3, 0, 0, 0, 43336, 0, 'Marda Weller - Random Say on Aggro'),
(1287, 0, 3, 'Greetings! Please have a look around.', 12, 7, 100, 3, 0, 0, 0, 43333, 0, 'Marda Weller - Random Say on Aggro'),
(1287, 0, 4, 'Greetings.', 12, 7, 100, 3, 0, 0, 0, 43337, 0, 'Marda Weller - Random Say on Aggro'),
(1287, 0, 5, 'Greetings, $c.', 12, 7, 100, 3, 0, 0, 0, 43330, 0, 'Marda Weller - Random Say on Aggro'),
(1287, 0, 6, 'Let me know if you need help finding anything, $c.', 12, 7, 100, 3, 0, 0, 0, 43335, 0, 'Marda Weller - Random Say on Aggro'),
(1287, 1, 1, 'Hello there, $n.  Happy Pilgrim\'s Bounty!', 12, 7, 100, 3, 0, 0, 0, 49904, 0, 'Marda Weller - Random Say on Aggro'), -- Pilgrim's Bounty Text
(1287, 1, 2, 'Has Pilgrim\'s Bounty got you in the shopping spirit, $c?  Well, you have come to the right place!', 12, 7, 100, 3, 0, 0, 0, 49912, 0, 'Marda Weller - Random Say on Aggro'), -- Pilgrim's Bounty Text
(1287, 1, 3, 'Are you enjoying Pilgrim\'s Bounty, $n?  Let me know if you need help finding anything.', 12, 7, 100, 3, 0, 0, 0, 49902, 0, 'Marda Weller - Random Say on Aggro'); -- Pilgrim's Bounty Text

DELETE FROM `smart_scripts` WHERE `entryorguid`=1287;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(1287, 0, 0, 0, 1, 0, 100, 0, 10000, 15000, 40000, 60000, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Edna Mullby - Out of Combat - Say Line 0');

UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=1287;

