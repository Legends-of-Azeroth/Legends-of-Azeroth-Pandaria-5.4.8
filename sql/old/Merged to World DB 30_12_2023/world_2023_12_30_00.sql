-- Add missing Thunder Falls spawns
SET @CGUID := 1;
DELETE FROM `creature` WHERE `guid` IN (@CGUID,@CGUID+1,@CGUID+2,@CGUID+3,@CGUID+4,@CGUID+5,@CGUID+6,@CGUID+7,@CGUID+8,@CGUID+9,@CGUID+10);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`) VALUES 
(@CGUID+0, 116, 0, 12, 92, 1, 1, 0, 1, -9294.96875, 637.0548095703125, 130.6758270263671875, 0.249209791421890258, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+1, 116, 0, 12, 92, 1, 1, 0, 1, -9288.8837890625, 658.26959228515625, 131.2381744384765625, 2.419910192489624023, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+3, 116, 0, 12, 92, 1, 1, 0, 1, -9298.201171875, 625.1895751953125, 130.763397216796875, 5.201081275939941406, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+4, 116, 0, 12, 92, 1, 1, 0, 1, -9291.0830078125, 677.8447265625, 131.856231689453125, 3.438298702239990234, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+5, 474, 0, 12, 92, 1, 1, 0, 1, -9277.080078125, 675.76708984375, 132.896148681640625, 0.33456045389175415, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+6, 116, 0, 12, 92, 1, 1, 0, 1, -9299.6943359375, 681.3992919921875, 132.268890380859375, 4.945234298706054687, 120, 0, 3, 0, 176, 0, 1, 0, 0, 0, 0, 0, '', 0),
(@CGUID+7, 116, 0, 12, 0, 1, 1, 0, 1, -9290.412109375, 689.0948486328125, 132.760101318359375, 3.735004663467407226, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+8, 116, 0, 12, 61, 1, 1, 0, 1, -9304.0361328125, 707.4471435546875, 131.0206146240234375, 4.171336650848388671, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+9, 116, 0, 12, 61, 1, 1, 0, 1, -9305.9482421875, 713.344970703125, 131.017242431640625, 2.321287870407104492, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0),
(@CGUID+10, 116, 0, 12, 92, 1, 1, 0, 1, -9288.033203125, 667.16851806640625, 131.6483917236328125, 3.490658521652221679, 120, 0, 0, 0, 176, 0, 0, 0, 0, 0, 0, 0, '', 0);

DELETE FROM `creature_addon` WHERE `guid` IN (@CGUID+5);
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `ai_anim_kit`, `movement_anim_kit`, `melee_anim_kit`, `auras`) VALUES 
(@CGUID+5, 0, 0, 0, 1, 0, 0, 0, 0, NULL); -- Rogue Wizard

UPDATE `creature` SET `modelid`=0 WHERE `id`=116; 

-- Wyrmrest Protector (58193)
DELETE FROM `creature` WHERE `guid` IN (303581,303582,318636,318637);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`) VALUES 
(303581, 58193, 1, 1637, 5171, 1, 1, 0, 0, 2018.91, -4271.49, 95.5316, 0.314159, 300, 0, 0, 0, 2074850, 0, 0, 0, 0, 0, 0, 0, '', 0),
(303582, 58193, 1, 1637, 5171, 1, 1, 0, 0, 2061.06, -4265.48, 95.5776, 4.39823, 300, 0, 0, 0, 2074850, 0, 0, 0, 0, 0, 0, 0, '', 0),
(318636, 58193, 0, 1519, 5398, 1, 1, 0, 0, -8114.24, 382.674, 116.149, 2.86234, 300, 0, 0, 0, 2074850, 0, 0, 0, 0, 0, 0, 0, '', 0),
(318637, 58193, 0, 1519, 5398, 1, 1, 0, 0, -8099.84, 421.083, 116.183, 3.90954, 300, 0, 0, 0, 2074850, 0, 0, 0, 0, 0, 0, 0, '', 0);

-- King Varian Wrynn (29611)
UPDATE `creature_template_addon` SET `emote`=0 WHERE `entry`=29611;

-- Greatfather Winter's Helper (15745)
UPDATE `creature` SET `wander_distance`=0, `movement_type`=0 WHERE `id`=15745;

-- Quest The Reason for the Season (7062)
UPDATE `creature_queststarter` SET `id`=1362 WHERE `quest`=7062;

-- Moni Widdlesprock (61836) Stormwind Harbor Guard around Commoander Sharp (29712)
DELETE FROM `creature` WHERE `guid` IN (@CGUID+11,@CGUID+12,@CGUID+13,@CGUID+14,@CGUID+15,@CGUID+16,@CGUID+17);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`) VALUES 
(@CGUID+11, 61836, 0, 1519, 0, 1, 1, 0, 1, -8797.326171875, 599.467041015625, 97.829498291015625, 4.605575084686279296, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0), -- Moni Widdlesprock (Area: 0 - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+12, 29712, 0, 1519, 4411, 1, 1, 0, 1, -8331.21875, 1263.7847900390625, 5.242923736572265625, 2.076941728591918945, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0), -- Stormwind Harbor Guard (Area: Stormwind Harbor - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+13, 29712, 0, 1519, 4411, 1, 1, 0, 1, -8343.2236328125, 1258.2379150390625, 5.260245800018310546, 0.92502450942993164, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0), -- Stormwind Harbor Guard (Area: Stormwind Harbor - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+14, 29712, 0, 1519, 4411, 1, 1, 0, 1, -8341.0107421875, 1264.045166015625, 5.24453592300415039, 1.134464025497436523, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0), -- Stormwind Harbor Guard (Area: Stormwind Harbor - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+15, 29712, 0, 1519, 4411, 1, 1, 0, 1, -8329.7705078125, 1258.4375, 5.245781898498535156, 1.919862151145935058, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0); -- Stormwind Harbor Guard (Area: Stormwind Harbor - Difficulty: 0) CreateObject1 (Auras: )

-- Stormwind Harbor Guard around Commoander Sharp (29712)
DELETE FROM `creature_addon` WHERE `guid` IN (@CGUID+12,@CGUID+13,@CGUID+14,@CGUID+15);
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `ai_anim_kit`, `movement_anim_kit`, `melee_anim_kit`, `auras`) VALUES 
(@CGUID+12, 0, 0, 0, 1, 333, 0, 0, 0, ''), -- Stormwind Harbor Guard
(@CGUID+13, 0, 0, 0, 1, 333, 0, 0, 0, ''), -- Stormwind Harbor Guard
(@CGUID+14, 0, 0, 0, 1, 333, 0, 0, 0, ''), -- Stormwind Harbor Guard
(@CGUID+15, 0, 0, 0, 1, 333, 0, 0, 0, ''); -- Stormwind Harbor Guard
