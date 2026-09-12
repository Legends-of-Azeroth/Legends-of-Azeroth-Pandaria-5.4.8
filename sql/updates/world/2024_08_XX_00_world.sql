-- gossip_menu
ALTER TABLE `gossip_menu` 
MODIFY COLUMN `VerifiedBuild` int NOT NULL DEFAULT 0 AFTER `TextID`;
ALTER TABLE `npc_text` 
MODIFY COLUMN `VerifiedBuild` int NULL DEFAULT 0 AFTER `Emote7_2`;


-- 1740 Alliance Intro Scene - Pandaria
-- 1706 Cosmetic - Stormwind - Can See Portal to Paw'Don

-- The Mission (29548) Pandria part
-- Phase 
-- 1780 Cosmetic - Ally Gunship Bombing NPCs
-- 1708 Cosmetic - Spawned Player Gyrocopter
-- 1789 Cosmetic - See Spawned Rogers on Skyfire
-- 1791 Cosmetic - Jade Forest - Strongarm Airstrip - Horde Flags
-- 1793 Cosmetic - See Taran Zhu in Honeydew
-- 1090 Cosmetic - Clouds
-- 1868 Cosmetic - See Zin'jun in Ascent
-- 1795 Cosmetic - See Teleporter @ Garroshar
-- 1685 Cosmetic - Jade Forest - Paw'don Village - Doors Closed
-- 1700 Alliance Arrival - Garrosh'ar Point - Bombing Run
-- 2108 Cosmetic - Ally Stygian Scar NPCs
DELETE FROM `phase_area` WHERE `AreaId`=5853 AND `PhaseId` IN (1685,1708,1780);
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES 
(5853, 1685, 'Cosmetic - Jade Forest - Paw\'don Village - Doors Closed'),
(5853, 1780, 'Ally Gunship Bombing NPCs'),
(5853, 1708, 'Cosmetic - Spawned Player Gyrocopter');

-- After The Mission (29548) complete 
-- Phase 1708, 1789, 1791, 1793, 1868, 1795, 1685, 1700, 2108
-- Remove 1780 Ally Gunship Bombing NPCs
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1780;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1780, 0, 0, 0, 9, 0, 29548, 0, 0, 0, 0, 0, '', 'Player has quest The Mission (29548) taken'),
(26, 1780, 0, 0, 0, 8, 0, 29548, 0, 0, 1, 0, 0, '', 'Player has not quest The Mission (29548) rewarded');

-- Remove 1708 Cosmetic - Spawned Player Gyrocopter
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1708;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1708, 0, 0, 0, 9, 0, 29548, 0, 0, 0, 0, 0, '', 'Player has quest The Mission (29548) taken'),
(26, 1708, 0, 0, 0, 8, 0, 31732, 0, 0, 1, 0, 0, '', 'Player has not quest Unleash Hell (31732) rewarded');

-- Phase 1685 Cosmetic - Jade Forest - Paw'don Village - Doors Closed
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1685;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1685, 0, 0, 0, 8, 0, 29547, 0, 0, 0, 0, 0, '', 'Player has quest The King\'s Command (29547) rewarded'),
(26, 1685, 0, 0, 0, 8, 0, 31736, 0, 0, 1, 0, 0, '', 'Player has not quest Envoy of the Alliance (31736) rewarded');

-- Unleash Hell(31732) start
-- 1787 Cosmetic - Paratroopers for Rogers Speech
-- 1708 Spawned Player Gyrocopter
-- 
-- 1702 Cosmetic - Garrosh'ar Point - Ga'trul
-- 1666 Alliance Arrival - Garrosh'ar Point - Horde Alive
-- 2117 Cosmetic - See Parachutes
-- 1698 Cosmetic - Garrosh'ar Point - Barricade 1
-- 1699 Cosmetic - Garrosh'ar Point - Barricade 2
-- 
-- 1789 Cosmetic - See Spawned Rogers on Skyfire
-- 
-- Remove 1708 Spawned Player Gyrocopter

-- Mishka
DELETE FROM `npc_text` WHERE `ID` IN (18718,18720);
INSERT INTO `npc_text` (`ID`, `Probability0`, `Probability1`, `Probability2`, `Probability3`, `Probability4`, `Probability5`, `Probability6`, `Probability7`, `BroadcastTextID0`, `BroadcastTextID1`, `BroadcastTextID2`, `BroadcastTextID3`, `BroadcastTextID4`, `BroadcastTextID5`, `BroadcastTextID6`, `BroadcastTextID7`, `VerifiedBuild`) VALUES 
(18718, 1, 1, 1, 1, 1, 1, 0, 0, 55214, 55215, 55216, 55217, 55218, 55219, 0, 0, 55960),
(18720, 1, 1, 1, 0, 0, 0, 0, 0, 55228, 55229, 55230, 0, 0, 0, 0, 0, 55960);
DELETE FROM `gossip_menu` WHERE (`MenuID`=13265 AND `TextID` IN(18718,18720));
INSERT INTO `gossip_menu` (`MenuID`, `TextID`, `VerifiedBuild`) VALUES 
(13265, 18718, 55960),
(13265, 18720, 55960);

-- Nimm Codejack
DELETE FROM `npc_text` WHERE `ID`=19264;
INSERT INTO `npc_text` (`ID`, `Probability0`, `Probability1`, `Probability2`, `Probability3`, `Probability4`, `Probability5`, `Probability6`, `Probability7`, `BroadcastTextID0`, `BroadcastTextID1`, `BroadcastTextID2`, `BroadcastTextID3`, `BroadcastTextID4`, `BroadcastTextID5`, `BroadcastTextID6`, `BroadcastTextID7`, `VerifiedBuild`) VALUES 
(19264, 1, 0, 0, 0, 0, 0, 0, 0, 58098, 0, 0, 0, 0, 0, 0, 0, 55960);
DELETE FROM `gossip_menu` WHERE (`MenuID`=13518 AND `TextID`=19264);
INSERT INTO `gossip_menu` (`MenuID`, `TextID`, `VerifiedBuild`) VALUES 
(13518, 19264, 55960); 
-- Nodd Codejack
DELETE FROM `npc_text` WHERE `ID`=18451;
INSERT INTO `npc_text` (`ID`, `Probability0`, `Probability1`, `Probability2`, `Probability3`, `Probability4`, `Probability5`, `Probability6`, `Probability7`, `BroadcastTextID0`, `BroadcastTextID1`, `BroadcastTextID2`, `BroadcastTextID3`, `BroadcastTextID4`, `BroadcastTextID5`, `BroadcastTextID6`, `BroadcastTextID7`, `VerifiedBuild`) VALUES 
(18451, 1, 0, 0, 0, 0, 0, 0, 0, 53772, 0, 0, 0, 0, 0, 0, 0, 55960);
DELETE FROM `gossip_menu` WHERE (`MenuID`=13117 AND `TextID`=18451);
INSERT INTO `gossip_menu` (`MenuID`, `TextID`, `VerifiedBuild`) VALUES 
(13117, 18451, 55960);

UPDATE `creature_template` SET `gossip_menu_id`= 13117, `npcflag`=`npcflag`|1 WHERE `entry`=54615; -- Nodd Codejack
UPDATE `creature_template` SET `gossip_menu_id`= 13265, `npcflag`=`npcflag`|1 WHERE `entry`=66527; -- Mishka
UPDATE `creature_template` SET `gossip_menu_id`= 13518, `npcflag`=`npcflag`|1 WHERE `entry`=54618; -- Nimm Codejack

-- SAI Mishka cast 61493 ([DND] First Aid) to 66480 (Skyfire Engineer) every 1 minute
-- bug
-- DELETE FROM `smart_scripts` WHERE `entryorguid` IN (66527);
-- INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
-- (66527, 0, 0, 0, 1, 0, 100, 0, 0, 0, 60000, 60000, 11, 61493, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Mishka - Out of Combat - Cast 61493 ([DND] First Aid)');

-- creature
-- delete old creatures
DELETE FROM `creature` WHERE `guid` IN (570797,570798,570795,570794,570796,570784,570792,570790,570789,570788,570787,570786,570793,570779,570780,570767,570766,504226,570768,570774,570781,570782,570783,570785,570791,570778,570771,570775,570770,570776,570772,570777,570773,570769,570765,570764,570799,570800,501127,501085,501187,582900,501159,501158,501157,501101,501164,501169,501162,501094,501207,501051,501144,501146,501185,501193,501198,570750,501192,501181,501102,570749,501123,501134,501149,501083,501184,501088,501136,501135,501150,501130,501168,582895,501126,501152,501209,501183,501132,501095,501100,501133,501171,501180,501077,501170,501153,582904,501069,501163,580176,570748,501057,501055,501093,501173,501161,501176,501108,501151,501140,501137,501117,501104,501156,501210,501186,570760,501141,501075,570747,501068,501074,501120,501124,501119,501145,501172,501175,582903,501112,501177,501078,501197,501061,501211,501139,501128,501062,501073,570751,501098,570761,501049,501115,570752,501107,501058,501200,501189,501199,501147,501125,501212,501082,501064,501195,501196,501174,501148,582902,501208,582897,501048,501052,501063,501109,501116,501050,501118,501138,501110,501105,501072,501142,501076,501203,501182,501059,501194,570745,501056,582901,501204,501178,582894,501201,501096,501113,501099,501087,501166,501160,582896,501121,501071,570762,501060,501079,501091,501188,501167,501103,570746,501155,501131,501111,501106,501066,501067,501089,570753,501090,501165,501065,501206,501053,501202,501097,501205,582898,501154,501190,501054,501179,501129,501047,501092,501070,582899,501114,501122);
DELETE FROM `creature_addon` WHERE `guid` IN (570797,570798,570795,570794,570796,570784,570792,570790,570789,570788,570787,570786,570793,570779,570780,570767,570766,504226,570768,570774,570781,570782,570783,570785,570791,570778,570771,570775,570770,570776,570772,570777,570773,570769,570765,570764,570799,570800,501127,501085,501187,582900,501159,501158,501157,501101,501164,501169,501162,501094,501207,501051,501144,501146,501185,501193,501198,570750,501192,501181,501102,570749,501123,501134,501149,501083,501184,501088,501136,501135,501150,501130,501168,582895,501126,501152,501209,501183,501132,501095,501100,501133,501171,501180,501077,501170,501153,582904,501069,501163,580176,570748,501057,501055,501093,501173,501161,501176,501108,501151,501140,501137,501117,501104,501156,501210,501186,570760,501141,501075,570747,501068,501074,501120,501124,501119,501145,501172,501175,582903,501112,501177,501078,501197,501061,501211,501139,501128,501062,501073,570751,501098,570761,501049,501115,570752,501107,501058,501200,501189,501199,501147,501125,501212,501082,501064,501195,501196,501174,501148,582902,501208,582897,501048,501052,501063,501109,501116,501050,501118,501138,501110,501105,501072,501142,501076,501203,501182,501059,501194,570745,501056,582901,501204,501178,582894,501201,501096,501113,501099,501087,501166,501160,582896,501121,501071,570762,501060,501079,501091,501188,501167,501103,570746,501155,501131,501111,501106,501066,501067,501089,570753,501090,501165,501065,501206,501053,501202,501097,501205,582898,501154,501190,501054,501179,501129,501047,501092,501070,582899,501114,501122);

-- add new creatures
SET @CGUID:=4000078;
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID+0 AND @CGUID+484;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `PhaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `VerifiedBuild`) VALUES
(@CGUID+0, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1184.0989990234375, -1447.328125, 33.29380035400390625, 4.298909187316894531, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+1, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -551.7274169921875, -1628.93408203125, 12.85113048553466796, 1.879548907279968261, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+2, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -743.0086669921875, -1676.7430419921875, 17.39997482299804687, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+3, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -675.07293701171875, -1564.88720703125, 6.757328033447265625, 1.678869485855102539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+4, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -671.22052001953125, -1561.329833984375, 5.571582794189453125, 1.659546136856079101, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+5, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -711.08160400390625, -1774.5191650390625, 0.617101132869720458, 5.080968379974365234, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+6, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -667.2689208984375, -1679.957275390625, 15.80854701995849609, 1.774110317230224609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+7, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 1, -690.91839599609375, -1487.8646240234375, 130.5379638671875, 1.765938401222229003, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+8, 66559, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1051.4600830078125, -1398.845458984375, 8.324535369873046875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+9, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -803.1475830078125, -1614.6597900390625, 3.426575183868408203, 2.318162441253662109, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+10, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -619.17364501953125, -1737.2552490234375, 10.4490060806274414, 2.346136093139648437, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+11, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -631.27362060546875, -1621.3546142578125, 15.62551403045654296, 1.611723065376281738, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+12, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -824.95501708984375, -1758.3873291015625, 8.761005401611328125, 2.825802326202392578, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+13, 40789, 870, 5785, 6533, 1, 1685, 0, 0, 0, -729.54168701171875, -1418.861083984375, 101.100555419921875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Generic Controller Bunny (CSA) (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+14, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -670.71875, -1729.9617919921875, 19.68236923217773437, 1.905510783195495605, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+15, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -668.765625, -1651.9427490234375, 18.64622116088867187, 0.113438941538333892, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+16, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -747.967041015625, -1639.5538330078125, 9.582927703857421875, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+17, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -658.453125, -1584.8472900390625, 16.22385025024414062, 1.537708640098571777, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+18, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -728.935791015625, -1477.435791015625, 149.355316162109375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 106246 - Cosmetic - Burning Tent)
(@CGUID+19, 66460, 870, 5785, 6533, 1, 1780, 0, 0, 1, -741.76043701171875, -1465.0555419921875, 149.909149169921875, 5.998937606811523437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Pilot (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+20, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -577.04791259765625, -1615.8828125, 14.25151252746582031, 4.66878366470336914, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+21, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -634.11376953125, -1604.4107666015625, 14.16986942291259765, 0.116140216588973999, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+22, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -981.5555419921875, -1610.720458984375, 9.331384658813476562, 2.769611835479736328, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+23, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+24, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -723.62200927734375, -1619.069580078125, 21.18425369262695312, 4.887689113616943359, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+25, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -769.87152099609375, -1698.23095703125, 10.44152641296386718, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+26, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -698.06134033203125, -1761.4718017578125, 1.871210098266601562, 4.215715885162353515, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+27, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -542.55731201171875, -1694.2691650390625, 7.69988250732421875, 0.195456311106681823, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+28, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -669.34027099609375, -1648.111083984375, 18.54277420043945312, 5.725718498229980468, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+29, 55370, 870, 5785, 6533, 1, 1780, 0, 0, 0, -662.15106201171875, -1466.171875, 130.3090057373046875, 1.654296040534973144, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 106246 - Cosmetic - Burning Tent)
(@CGUID+30, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -641.92364501953125, -1611.828125, 14.03620529174804687, 2.207401514053344726, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+31, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -717.97052001953125, -1421.7117919921875, 134.8536834716796875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+32, 66382, 870, 5785, 6533, 1, 1780, 0, 0, 0, -778.279541015625, -1670.23095703125, 35.75363922119140625, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+33, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -831.56494140625, -1629.0379638671875, 2.723604202270507812, 3.548845767974853515, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+34, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -721.638916015625, -1752.5521240234375, 27.85347938537597656, 5.298679828643798828, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+35, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -719.15966796875, -1671.4208984375, 16.26267433166503906, 4.412684440612792968, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+36, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -596.25177001953125, -1624.829833984375, 13.81680488586425781, 3.074210405349731445, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+37, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -754.54168701171875, -1540.220458984375, 34.37433624267578125, 2.214611768722534179, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+38, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -771.03302001953125, -1711.6336669921875, 10.19180107116699218, 1.60694289207458496, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+39, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -522.357666015625, -1668.84375, 10.27729034423828125, 0.4314214289188385, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+40, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, 0, 0, 0, 1.82674407958984375, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+41, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -724.87921142578125, -1719.723876953125, 26.69399452209472656, 4.905727386474609375, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+42, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -739.72265625, -1612.15234375, 22.43612098693847656, 2.273282527923583984, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+43, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -790.5225830078125, -1691.3367919921875, 11.08053779602050781, 2.426514148712158203, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+44, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -979.25, -1628.607666015625, 5.694042205810546875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+45, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -584.88543701171875, -1576.1024169921875, 12.15601539611816406, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+46, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -836.12847900390625, -1647.5833740234375, 4.565769195556640625, 2.336583375930786132, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+47, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -650.6319580078125, -1560.7535400390625, 6.365113735198974609, 4.623965740203857421, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+48, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -761.1805419921875, -1585.48095703125, 26.14768600463867187, 2.988919019699096679, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+49, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -537.416015625, -1693.00390625, 7.3338623046875, 1.794038176536560058, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+50, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -643.8055419921875, -1500.25, 1.116840600967407226, 1.166521549224853515, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+51, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -624.60418701171875, -1514.8941650390625, 1.155135631561279296, 1.07947397232055664, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+52, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -714.936767578125, -1618.94580078125, 17.97323036193847656, 1.159986734390258789, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+53, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -632.27081298828125, -1518.9478759765625, 2.284188270568847656, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+54, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -826.72393798828125, -1616.3992919921875, 0.980699539184570312, 1.935843229293823242, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+55, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -692.701416015625, -1496.4166259765625, 1.421829938888549804, 5.950888633728027343, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+56, 54617, 870, 5785, 6533, 1, 1780, 0, 0, 1, -680.06597900390625, -1501.439208984375, 131.42864990234375, 4.450106143951416015, 120, 0, 0, 0, 0, 0, 0, 55960), -- Rell Nightwind (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+57, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -724.42535400390625, -1463.0382080078125, 149.903411865234375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+58, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -782.5625, -1658.953125, 4.559909820556640625, 3.089388370513916015, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+59, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -760.22076416015625, -1660.578369140625, 9.400839805603027343, 3.135857820510864257, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+60, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -589.8486328125, -1742.5601806640625, 6.744942665100097656, 3.493977546691894531, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+61, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -662, -1480.0972900390625, 130.2865447998046875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 421177 - -Unknown-, 421175 - -Unknown-)
(@CGUID+62, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -574.0845947265625, -1615.4150390625, 14.25151252746582031, 4.669160366058349609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+63, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -648.57464599609375, -1682.76220703125, 13.93951225280761718, 0.488384336233139038, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+64, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -725.739013671875, -1622.1031494140625, 20.644622802734375, 4.885121822357177734, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+65, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -683.8992919921875, -1760.7847900390625, 5.662944793701171875, 4.713036537170410156, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+66, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1069.0972900390625, -1383.0660400390625, 3.80986952781677246, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+67, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -744.35589599609375, -1576.076416015625, 34.24018478393554687, 4.866236686706542968, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+68, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -751.60589599609375, -1756.40625, 27.8416595458984375, 3.950423955917358398, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+69, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -823.65972900390625, -1611.9375, 1.007074952125549316, 3.250461578369140625, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+70, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -898.66668701171875, -1663.0867919921875, 6.407142162322998046, 3.170816898345947265, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+71, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -730.6475830078125, -1551.001708984375, 34.89602279663085937, 5.433163166046142578, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+72, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -797.045166015625, -1646.59375, 3.858263492584228515, 5.583950996398925781, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+73, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -667.4425048828125, -1536.5443115234375, 3.43366098403930664, 4.594818115234375, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+74, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -549.12847900390625, -1604.857666015625, 12.76610755920410156, 3.097997665405273437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+75, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -703.6632080078125, -1468.90625, 130.611114501953125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+76, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -839.68927001953125, -1759.59033203125, 8.828516006469726562, 2.905303239822387695, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+77, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -971.91839599609375, -1643.62158203125, 5.705219268798828125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+78, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -751.02777099609375, -1539.029541015625, 33.98267364501953125, 2.123943328857421875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+79, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -551.65625, -1598.763916015625, 12.31547737121582031, 0.334768921136856079, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+80, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -838.685791015625, -1598.625, 1.345248818397521972, 0.458927631378173828, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+81, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, 0, 0, 0, 5.503710269927978515, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+82, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -554.375, -1563.7222900390625, 10.26774406433105468, 1.137903213500976562, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+83, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -662.08245849609375, -1668.509521484375, 15.6899261474609375, 4.25427103042602539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+84, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -709.2725830078125, -1523.609375, 0.809545814990997314, 2.532784700393676757, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+85, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -603.87677001953125, -1729.609375, 9.648369789123535156, 3.028424263000488281, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+86, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -727.18212890625, -1720.2657470703125, 26.33848762512207031, 5.109689235687255859, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+87, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -972.62677001953125, -1607.048583984375, 9.561455726623535156, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+88, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -693.1229248046875, -1743.1768798828125, 7.793707847595214843, 4.299345970153808593, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+89, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -670.16839599609375, -1567.826416015625, 6.009906768798828125, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+90, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -612.42486572265625, -1665.23291015625, 12.22383594512939453, 3.733853340148925781, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+91, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -725.93231201171875, -1689.1475830078125, 16.98820114135742187, 5.901762008666992187, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+92, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -699.89410400390625, -1694.349853515625, 16.89011764526367187, 5.170908927917480468, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+93, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -842.373291015625, -1589.123291015625, 3.511875867843627929, 2.248399734497070312, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+94, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -691.77606201171875, -1485.9410400390625, 130.3090057373046875, 5.058022499084472656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+95, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -673.921875, -1488.6927490234375, 1.036462187767028808, 1.343134403228759765, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+96, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -622.20489501953125, -1737.763916015625, 10.69403266906738281, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+97, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -981.00518798828125, -1599.404541015625, 10.73913097381591796, 3.299045085906982421, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+98, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -689.60394287109375, -1567.602783203125, 6.333170890808105468, 4.421268939971923828, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+99, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1078.4478759765625, -1385.954833984375, 4.380977630615234375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+100, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -621.75, -1577.4271240234375, 14.81456947326660156, 3.77436065673828125, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+101, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -971.97222900390625, -1623.96533203125, 7.022841930389404296, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+102, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -650.98614501953125, -1667.7291259765625, 14.59258174896240234, 0.106131233274936676, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+103, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -613.27081298828125, -1735.37158203125, 9.805391311645507812, 0.744170308113098144, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+104, 64858, 870, 5785, 6533, 1, 1780, 0, 0, 0, -738.63714599609375, -1456.3004150390625, 149.915283203125, 5.998937606811523437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Airman Hicks (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+105, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -663.8507080078125, -1516.7066650390625, 2.149497270584106445, 1.59213566780090332, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+106, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -784.624267578125, -1734.662841796875, 9.686534881591796875, 4.645153999328613281, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+107, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -812.58856201171875, -1761.65283203125, 8.941710472106933593, 4.92644500732421875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+108, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -701.45660400390625, -1539.1302490234375, 4.481102943420410156, 2.230078458786010742, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+109, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1069.392333984375, -1383.46875, 5.353227615356445312, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+110, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -622.06768798828125, -1698.472412109375, 13.81088447570800781, 4.778002738952636718, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+111, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1066.8160400390625, -1373.1771240234375, 3.809871196746826171, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+112, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -740.34722900390625, -1762.626708984375, 27.7734832763671875, 4.62628030776977539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+113, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -659.7783203125, -1502.4478759765625, 1.646731138229370117, 5.647500038146972656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+114, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -826.14581298828125, -1604.3541259765625, 1.141667842864990234, 2.646962165832519531, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+115, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -671.30731201171875, -1676.314208984375, 15.93360710144042968, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 130628 - Assault on Garrosh'ar Point)
(@CGUID+116, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -826.06768798828125, -1606.84033203125, 1.255111575126647949, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+117, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -755.18927001953125, -1687.3350830078125, 8.931378364562988281, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+118, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -649.36456298828125, -1736.625, 19.68236923217773437, 5.7447357177734375, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+119, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -551.86981201171875, -1712.1146240234375, 8.097507476806640625, 4.637742519378662109, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+120, 64861, 870, 5785, 6533, 1, 1780, 0, 0, 0, -720.670166015625, -1472.2447509765625, 133.105010986328125, 5.856484889984130859, 120, 0, 0, 0, 0, 0, 0, 55960), -- Captain Day (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+121, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -624.1976318359375, -1635.0931396484375, 15.62195110321044921, 2.590087890625, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+122, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -695.078125, -1721.6822509765625, 18.19254112243652343, 4.880746364593505859, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+123, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -797.30462646484375, -1655.826416015625, 3.824867963790893554, 5.008786201477050781, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+124, 66890, 870, 5785, 6533, 1, 1780, 0, 0, 0, -643.4234619140625, -1700.4234619140625, 43.99009323120117187, 2.992599725723266601, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Master (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+125, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+126, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -696.01580810546875, -1742.38232421875, 7.793707847595214843, 4.222102165222167968, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+127, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -636.59576416015625, -1715.4598388671875, 14.05414962768554687, 5.15390777587890625, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+128, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -614.35235595703125, -1667.727294921875, 12.13090038299560546, 3.782430171966552734, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+129, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1204.5660400390625, -1493.9739990234375, 14.07238960266113281, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+130, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -827.78546142578125, -1633.2137451171875, 3.10122990608215332, 3.483079910278320312, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+131, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -722.76171875, -1688.802734375, 17.90613555908203125, 4.294420242309570312, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+132, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -707.01739501953125, -1772.8489990234375, 0.607156574726104736, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+133, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 3, -728.15802001953125, -1470.6788330078125, 149.9008331298828125, 4.597786903381347656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+134, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -756.826416015625, -1560.71533203125, 34.14266204833984375, 4.567026138305664062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+135, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -774.56634521484375, -1707.531005859375, 10.23199462890625, 5.3930206298828125, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+136, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1077.40625, -1376.392333984375, 3.809872150421142578, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+137, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -824.55731201171875, -1621.78125, 3.169934988021850585, 2.794280767440795898, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+138, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -618.73956298828125, -1586.8836669921875, 14.1696624755859375, 2.751108169555664062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+139, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -803.11114501953125, -1635.861083984375, 3.837064504623413085, 2.809123516082763671, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+140, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -707.265625, -1771.5833740234375, 0.611562371253967285, 5.212006568908691406, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+141, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -625.87152099609375, -1512.907958984375, 1.010395169258117675, 1.406599640846252441, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+142, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -984.1875, -1603.967041015625, 10.22965335845947265, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+143, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -718.39239501953125, -1736.4791259765625, 27.85631561279296875, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+144, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 3, -660.3194580078125, -1461.6180419921875, 130.3944549560546875, 4.69530487060546875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+145, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -674.92681884765625, -1691.8614501953125, 16.701080322265625, 4.820263862609863281, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+146, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -784.31451416015625, -1733.4580078125, 9.720571517944335937, 4.640328407287597656, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+147, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -797.7586669921875, -1688.0052490234375, 7.020170211791992187, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+148, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1054.71875, -1391.857666015625, 7.3603057861328125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+149, 64863, 870, 5785, 6533, 1, 1780, 0, 0, 0, -730.26739501953125, -1475.0555419921875, 151.1490325927734375, 5.522500038146972656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Sky Mage Harlan (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 83810 - Cosmetic - Water Channel)
(@CGUID+150, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -687.62677001953125, -1626.982666015625, 22.59335899353027343, 3.513808727264404296, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+151, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -558.8975830078125, -1699.9271240234375, 8.546230316162109375, 3.962412357330322265, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+152, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -747.45489501953125, -1551.53125, 34.89602279663085937, 2.85747385025024414, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+153, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -825.7183837890625, -1759.567626953125, 8.765690803527832031, 3.047254323959350585, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+154, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -803.69964599609375, -1689.279541015625, 7.70287942886352539, 0.977204322814941406, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+155, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -658.7274169921875, -1529.7222900390625, 2.207141876220703125, 4.09306955337524414, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+156, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -713.68341064453125, -1625.43310546875, 17.37048912048339843, 1.161073803901672363, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+157, 40789, 870, 5785, 6533, 1, 1685, 0, 0, 0, -673.982666015625, -1476.6822509765625, 130.2828216552734375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Generic Controller Bunny (CSA) (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 132223 - FX_AllianceGunship_Idle_Main_Loop)
(@CGUID+158, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -701.2100830078125, -1678.736083984375, 15.34984493255615234, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+159, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -661.83331298828125, -1560.201416015625, 6.013905048370361328, 6.020073413848876953, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+160, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1069.451416015625, -1384.779541015625, 15.98803520202636718, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+161, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -801.45831298828125, -1637.6597900390625, 3.858106374740600585, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+162, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -578.4305419921875, -1726.611083984375, 7.87882232666015625, 5.582028388977050781, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+163, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -568.65277099609375, -1713.0052490234375, 8.380362510681152343, 3.33944106101989746, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+164, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -738.59893798828125, -1640.970458984375, 16.03557395935058593, 3.483582019805908203, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+165, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -531.7725830078125, -1629.1632080078125, 11.21038818359375, 0.424936354160308837, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+166, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+167, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -814.8782958984375, -1606.1590576171875, 0.806931018829345703, 1.842275381088256835, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+168, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -832.41143798828125, -1668.1475830078125, 4.4622955322265625, 3.94484257698059082, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+169, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -554.97393798828125, -1595.8507080078125, 12.41842365264892578, 0.745885372161865234, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+170, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -617.37164306640625, -1752.12841796875, 6.424019813537597656, 2.526853084564208984, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+171, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -567.34893798828125, -1583.2222900390625, 20.32096290588378906, 3.863297462463378906, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+172, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -883.3355712890625, -1655.8927001953125, 5.940941333770751953, 0.050056252628564834, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+173, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -721.24102783203125, -1668.4249267578125, 16.35529136657714843, 4.397888660430908203, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+174, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -593.5142822265625, -1588.1934814453125, 13.11590576171875, 4.16928720474243164, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+175, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -578.9375, -1682.751708984375, 24.30853271484375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+176, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -719.691162109375, -1688.4686279296875, 17.31457901000976562, 4.229321956634521484, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+177, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 1, -648.54168701171875, -1478.4879150390625, 130.284454345703125, 2.868044376373291015, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+178, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1158.9896240234375, -1506.326416015625, 32.06308364868164062, 2.87692427635192871, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+179, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -777.56304931640625, -1707.3905029296875, 10.23199462890625, 5.318303108215332031, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+180, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -565.5555419921875, -1608.8975830078125, 14.36054420471191406, 4.200547695159912109, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+181, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -776.2257080078125, -1652.892333984375, 8.348402976989746093, 2.992926359176635742, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+182, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -973.79864501953125, -1617.6441650390625, 5.694042205810546875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+183, 66434, 870, 5785, 6533, 1, 1685, 0, 0, 0, -757.60418701171875, -1567.03125, 34.24018478393554687, 3.515186071395874023, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Cannon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+184, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -688.05731201171875, -1707.1458740234375, 16.88533973693847656, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+185, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 1, -667.515625, -1468.0728759765625, 130.247711181640625, 2.712678909301757812, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+186, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -766.2882080078125, -1648.5885009765625, 10.34551525115966796, 3.711915493011474609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+187, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -761.84893798828125, -1591.9617919921875, 25.92687225341796875, 3.148415088653564453, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+188, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -760.63018798828125, -1633.4617919921875, 9.870668411254882812, 5.226000785827636718, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+189, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, 0, 0, 0, 5.934119701385498046, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+190, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -747.56768798828125, -1549.123291015625, 34.89602279663085937, 2.453984260559082031, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+191, 66509, 870, 5785, 6533, 1, 1780, 0, 0, 0, -682.8194580078125, -1501.0521240234375, 131.436553955078125, 4.445753097534179687, 120, 0, 0, 0, 0, 0, 0, 55960), -- Amber Kearnen (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+192, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -834.29541015625, -1760.436279296875, 8.58396148681640625, 4.355662822723388671, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+193, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -550.30206298828125, -1625.767333984375, 12.82239341735839843, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+194, 66297, 870, 5785, 6533, 1, 1708, 0, 0, 0, -653.56768798828125, -1482.46533203125, 130.2850189208984375, 5.096361160278320312, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 131025 - Gyrocopter Anim Set, Ground)
(@CGUID+195, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -716.56695556640625, -1624.6036376953125, 17.37117195129394531, 1.161139845848083496, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+196, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -871.625, -1666.3316650390625, 5.669208526611328125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+197, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -757.27972412109375, -1660.864990234375, 9.735569000244140625, 3.130904197692871093, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+198, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -740.61456298828125, -1638.0625, 15.78981971740722656, 3.39355325698852539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+199, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -738.513916015625, -1511.9478759765625, 55.05987167358398437, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+200, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -739.51214599609375, -1502.99658203125, 134.853363037109375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+201, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -573.6884765625, -1618.5960693359375, 14.07005977630615234, 4.66634988784790039, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+202, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -763.5885009765625, -1663.2720947265625, 9.051334381103515625, 3.14119124412536621, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+203, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -606.539306640625, -1696.0869140625, 11.72244453430175781, 2.758551597595214843, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+204, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -746.48089599609375, -1762.2239990234375, 27.66699981689453125, 4.106542110443115234, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+205, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -782.7955322265625, -1745.8238525390625, 9.514166831970214843, 4.584031581878662109, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+206, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -723.06060791015625, -1621.8013916015625, 20.59069061279296875, 4.931117057800292968, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+207, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -654.80035400390625, -1755.3975830078125, 19.0265350341796875, 5.034769535064697265, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+208, 54615, 870, 5785, 6533, 1, 1685, 0, 0, 2, -655.8975830078125, -1483.829833984375, 130.2856903076171875, 2.959142684936523437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Nodd Codejack (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+209, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -733.22918701171875, -1732.8367919921875, 27.84159469604492187, 0.732490181922912597, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+210, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -665.34185791015625, -1534.402587890625, 3.43366098403930664, 4.654510974884033203, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+211, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -643.32989501953125, -1503.27783203125, 140.2075042724609375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 106246 - Cosmetic - Burning Tent)
(@CGUID+212, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -691.27606201171875, -1682.7864990234375, 31.66353416442871093, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+213, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -746.38018798828125, -1667.954833984375, 10.14062690734863281, 2.459662437438964843, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+214, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -811.83331298828125, -1680.795166015625, 7.495849132537841796, 2.96669316291809082, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+215, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -718.3560791015625, -1669.53564453125, 15.97879314422607421, 4.380016326904296875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+216, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1211.05908203125, -1451.423583984375, 28.2454376220703125, 4.863817214965820312, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+217, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -664.1632080078125, -1670.670654296875, 15.6899261474609375, 4.174101829528808593, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+218, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, 0, 0, 0, 1.461476325988769531, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+219, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -723.3819580078125, -1737.0103759765625, 27.95028114318847656, 5.239404678344726562, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+220, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+221, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -628.95660400390625, -1515.935791015625, 1.524216413497924804, 1.467155098915100097, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+222, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -982.7586669921875, -1612.0069580078125, 9.206342697143554687, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+223, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -976.06768798828125, -1634.3975830078125, 5.694043636322021484, 0.126039952039718627, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+224, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -591.37847900390625, -1652.1927490234375, 12.82654762268066406, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+225, 66299, 870, 5785, 6533, 1, 1685, 0, 0, 0, 0.861486852169036865, 0, -0.37638470530509948, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Turret (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+226, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -741.76910400390625, -1564.1510009765625, 34.89602279663085937, 5.075589179992675781, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+227, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -698.75738525390625, -1691.573486328125, 16.89011764526367187, 5.301287651062011718, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+228, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -668.7274169921875, -1492.0260009765625, 130.3090057373046875, 5.058022499084472656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+229, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -793.154541015625, -1703.779541015625, 11.45113945007324218, 3.67000436782836914, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+230, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -616.34552001953125, -1581.3975830078125, 13.69006919860839843, 3.654152631759643554, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+231, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -682.34893798828125, -1488.4879150390625, 130.3090057373046875, 5.058022499084472656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+232, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -719.87200927734375, -1673.8758544921875, 15.70772361755371093, 1.259439468383789062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+233, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -608.7899169921875, -1518.6475830078125, 1.584843754768371582, 1.382902860641479492, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+234, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -665.24652099609375, -1519.87158203125, 2.149497032165527343, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+235, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -971.26910400390625, -1606.751708984375, 9.56549072265625, 5.944952964782714843, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+236, 64859, 870, 5785, 6533, 1, 1780, 0, 0, 1, -739.654541015625, -1780.05908203125, 149.907745361328125, 1.250773787498474121, 120, 0, 0, 0, 0, 0, 0, 55960), -- Sky Captain Callanan (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+237, 66510, 870, 5785, 6533, 1, 1780, 0, 0, 0, -681.34027099609375, -1501.10595703125, 131.431365966796875, 4.347804546356201171, 120, 0, 0, 0, 0, 0, 0, 55960), -- Sully "The Pickle" McLeary (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+238, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -769.545166015625, -1647.6007080078125, 10.77938270568847656, 3.380753993988037109, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+239, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -794.70611572265625, -1662.9334716796875, 4.092006206512451171, 0.040926788002252578, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+240, 66434, 870, 5785, 6533, 1, 1685, 0, 0, 0, -650.09893798828125, -1753.27783203125, 19.0265350341796875, 5.407110214233398437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Cannon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+241, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -608.27777099609375, -1724.78125, 10.51749610900878906, 4.72619485855102539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+242, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -971.3194580078125, -1625.3472900390625, 6.687335968017578125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+243, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -688.08331298828125, -1500.1771240234375, 131.4550323486328125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 106246 - Cosmetic - Burning Tent)
(@CGUID+244, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -569.7718505859375, -1582.3741455078125, 13.62040519714355468, 1.616774559020996093, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+245, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -833.328125, -1749.3524169921875, 9.303404808044433593, 2.156170845031738281, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+246, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -782.069091796875, -1732.1063232421875, 9.554168701171875, 4.688943386077880859, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+247, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -966.15802001953125, -1624.44970703125, 5.694042205810546875, 6.245493888854980468, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+248, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1069.7603759765625, -1384.109375, 3.80986952781677246, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+249, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -754.5, -1755.6146240234375, 27.57104301452636718, 3.619364738464355468, 120, 0, 0, 0, 0, 0, 0, 55960); -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1

INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `PhaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `MovementType`, `npcflag`, `unit_flags`, `unit_flags2`, `VerifiedBuild`) VALUES
(@CGUID+250, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -661.02703857421875, -1505.197021484375, 1.785750269889831542, 5.646921157836914062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+251, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -827.52777099609375, -1594.7916259765625, 0.630508899688720703, 2.121900081634521484, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+252, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -689.87152099609375, -1638.4444580078125, 22.83085250854492187, 5.183674335479736328, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+253, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -777.92120361328125, -1682.202392578125, 6.92122507095336914, 2.199449062347412109, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+254, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -964.078125, -1631.43408203125, 5.694043636322021484, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+255, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -835.52606201171875, -1608.7535400390625, 3.631232976913452148, 2.861916065216064453, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+256, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -663.98797607421875, -1535.8427734375, 3.532800197601318359, 4.647991657257080078, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+257, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -592.6475830078125, -1526.138916015625, 1.185439825057983398, 0.89901900291442871, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+258, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -705.24652099609375, -1497.4478759765625, 134.537261962890625, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 106246 - Cosmetic - Burning Tent)
(@CGUID+259, 64866, 870, 5785, 6533, 1, 1685, 0, 0, 0, -663.40277099609375, -1482.782958984375, 130.284210205078125, 2.780452966690063476, 120, 0, 0, 0, 0, 0, 0, 55960), -- Corporal Rasmussen (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 421177 - -Unknown-, 421175 - -Unknown-)
(@CGUID+260, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -789.1163330078125, -1689.3697509765625, 11.28726959228515625, 2.293546199798583984, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+261, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -600.41668701171875, -1555.453125, 11.12749576568603515, 1.034241318702697753, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+262, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -529.6649169921875, -1643.3680419921875, 11.10860633850097656, 0.179459765553474426, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+263, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, 0, 0, 0, 4.823880195617675781, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+264, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -621.59051513671875, -1633.60888671875, 15.62195110321044921, 2.617929458618164062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+265, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -666.39410400390625, -1753.951416015625, 18.93738555908203125, 5.910347938537597656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+266, 66890, 870, 5785, 6533, 1, 1780, 0, 0, 0, -535.2442626953125, -1550.7540283203125, 28.2133026123046875, 4.903226852416992187, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Master (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+267, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+268, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -756.04339599609375, -1729.3941650390625, 27.64536666870117187, 3.397314786911010742, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+269, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -731.30902099609375, -1676.4444580078125, 16.93780899047851562, 4.176340579986572265, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+270, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+271, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -692.37664794921875, -1740.4600830078125, 8.861263275146484375, 4.216427326202392578, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+272, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -600.3206787109375, -1649.9705810546875, 12.59966182708740234, 0.652945637702941894, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+273, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -830.903564453125, -1633.1259765625, 3.133801698684692382, 3.456388473510742187, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+274, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -742.9288330078125, -1567.0885009765625, 34.89602279663085937, 0.737486839294433593, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+275, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -695.8118896484375, -1692.7794189453125, 17.02403450012207031, 5.476104736328125, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+276, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -675.22393798828125, -1462.2274169921875, 130.3090057373046875, 0.536964654922485351, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+277, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -787.39581298828125, -1728.5711669921875, 9.793458938598632812, 3.917194604873657226, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+278, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1080.8541259765625, -1368.8472900390625, 3.822113752365112304, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+279, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -821.01043701171875, -1629.0225830078125, 4.481141090393066406, 2.761285305023193359, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+280, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -981.09375, -1614.7066650390625, 8.8983154296875, 2.830271244049072265, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+281, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1060.7882080078125, -1401.545166015625, 7.997231483459472656, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+282, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -667.8101806640625, -1552.28173828125, 4.884207725524902343, 2.660479545593261718, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+283, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -661.34552001953125, -1584.623291015625, 16.5813446044921875, 2.507088661193847656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+284, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -745.1961669921875, -1682.46875, 17.22924613952636718, 2.789761304855346679, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+285, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -836.484375, -1591.329833984375, 1.40793013572692871, 5.251709461212158203, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+286, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -645.263916015625, -1562.795166015625, 6.532986164093017578, 2.255315065383911132, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+287, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -592.1805419921875, -1576.3853759765625, 12.16783618927001953, 5.985514163970947265, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+288, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -676.6004638671875, -1689.7896728515625, 16.80527496337890625, 4.70403289794921875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+289, 64860, 870, 5785, 6533, 1, 1780, 0, 0, 0, -716.36114501953125, -1457.16845703125, 133.09912109375, 6.063904762268066406, 120, 0, 0, 0, 0, 0, 0, 55960), -- Captain Kerwin (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+290, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -784.83282470703125, -1735.4368896484375, 9.633149147033691406, 4.640649318695068359, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+291, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1062.939208984375, -1407.638916015625, 8.571680068969726562, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+292, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -805.15277099609375, -1622.63720703125, 2.235953569412231445, 5.519268989562988281, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+293, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -971.3975830078125, -1631.0399169921875, 5.694043636322021484, 0.034285534173250198, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+294, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -597.046875, -1678.7291259765625, 12.12304210662841796, 3.343768119812011718, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+295, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1069.4132080078125, -1388.1336669921875, 3.80986952781677246, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+296, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -629.798583984375, -1523.2998046875, 2.62810516357421875, 4.161386966705322265, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+297, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -606.1961669921875, -1543.732666015625, 6.31053018569946289, 5.74225616455078125, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+298, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -616.32989501953125, -1574.87158203125, 14.60472583770751953, 3.789583444595336914, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+299, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -720.279541015625, -1601.310791015625, 25.55076217651367187, 0.27673545479774475, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+300, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -690.2725830078125, -1503.1875, 1.759267330169677734, 2.341843128204345703, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+301, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -747.359375, -1754.470458984375, 27.84161949157714843, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+302, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -610.09375, -1739.142333984375, 8.906344413757324218, 4.775902271270751953, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+303, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -527.77777099609375, -1645.7379150390625, 11.18871021270751953, 6.136672019958496093, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+304, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -718.8524169921875, -1689.01220703125, 16.2487030029296875, 3.292572498321533203, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+305, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -964.638916015625, -1629.6597900390625, 5.694043636322021484, 0.332331597805023193, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+306, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -620.250732421875, -1635.47265625, 15.55916213989257812, 2.622378826141357421, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+307, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -515.78302001953125, -1613.6805419921875, 4.713868141174316406, 6.100655078887939453, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+308, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -680.5242919921875, -1551.529541015625, 4.410439491271972656, 1.300888299942016601, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+309, 66890, 870, 5785, 6533, 1, 1780, 0, 0, 0, -956.95550537109375, -1575.4852294921875, 32.66314697265625, 5.521714687347412109, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Master (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+310, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+311, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -849.63714599609375, -1668.9757080078125, 4.993358612060546875, 4.229179859161376953, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+312, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -779.16143798828125, -1604.0677490234375, 11.14572811126708984, 2.482956647872924804, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+313, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -695.26953125, -1739.66552734375, 8.861263275146484375, 4.141945838928222656, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+314, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -552.201416015625, -1565.0086669921875, 10.21797657012939453, 0.628063917160034179, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+315, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -775.640625, -1765.3958740234375, 10.8547821044921875, 5.346131324768066406, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+316, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -597.732666015625, -1747.8021240234375, 6.208420276641845703, 4.207509994506835937, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+317, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -974.0538330078125, -1619.578125, 18.46225357055664062, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+318, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -828.07122802734375, -1630.9539794921875, 2.861481904983520507, 3.528737783432006835, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+319, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -742.57989501953125, -1725.65625, 27.69067955017089843, 4.178181171417236328, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+320, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -671.4305419921875, -1743.4305419921875, 19.68236923217773437, 4.145617008209228515, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+321, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -970.3507080078125, -1628.236083984375, 5.694042682647705078, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+322, 64862, 870, 5785, 6533, 1, 1780, 0, 0, 0, -739.79168701171875, -1459.2552490234375, 149.9190216064453125, 5.998937606811523437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Airman Ellis (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+323, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -777.40472412109375, -1704.2984619140625, 10.32373237609863281, 5.30168914794921875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+324, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -600.703125, -1732.795166015625, 9.001134872436523437, 5.792002677917480468, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+325, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1077.2603759765625, -1384.296875, 3.80986952781677246, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+326, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -973.805419921875, -1608.384521484375, 9.34490203857421875, 0.657531917095184326, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+327, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -658.48956298828125, -1557.5538330078125, 5.892794609069824218, 4.76174783706665039, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+328, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -792.66668701171875, -1464.01220703125, -0.33544284105300903, 5.668870925903320312, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+329, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -558.11456298828125, -1615.29345703125, 13.61770820617675781, 1.719073176383972167, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+330, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -630.85589599609375, -1578.6007080078125, 16.07216835021972656, 2.097669124603271484, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+331, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -970.79864501953125, -1615.09033203125, 8.6867523193359375, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+332, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 1, -637.16668701171875, -1472.921875, 135.9058074951171875, 0.200845494866371154, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+333, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -674.58197021484375, -1691.78076171875, 16.50616836547851562, 4.696915149688720703, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+334, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -747.15802001953125, -1694.703125, 22.695281982421875, 5.588164806365966796, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+335, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -672.4288330078125, -1738.2430419921875, 19.68236923217773437, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+336, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -569.34881591796875, -1639.0255126953125, 12.94681930541992187, 0.229091942310333251, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+337, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1070.873291015625, -1396.8385009765625, 6.957218170166015625, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+338, 66460, 870, 5785, 6533, 1, 1780, 0, 0, 1, -740.89410400390625, -1462.2535400390625, 149.915313720703125, 5.998937606811523437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Pilot (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+339, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -688.701416015625, -1498.0069580078125, 1.501986503601074218, 2.007900953292846679, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+340, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -616.04144287109375, -1631.56640625, 15.19676494598388671, 5.345784187316894531, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+341, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -751.17364501953125, -1556.6978759765625, 34.89602279663085937, 2.577459573745727539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+342, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -665.91668701171875, -1519.01220703125, 2.149497270584106445, 5.014996528625488281, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+343, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -699.8524169921875, -1752.5225830078125, 3.417914152145385742, 5.154534816741943359, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+344, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -712.8681640625, -1622.6038818359375, 17.67105865478515625, 1.160496234893798828, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+345, 40789, 870, 5785, 6533, 1, 1685, 0, 0, 0, -750.875, -1498.71875, 101.100555419921875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Generic Controller Bunny (CSA) (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+346, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -542.1180419921875, -1623.8785400390625, 11.5255279541015625, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+347, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -668.1163330078125, -1518.9166259765625, 2.302931070327758789, 2.096298694610595703, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+348, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -718.34027099609375, -1695.578125, 30.41111946105957031, 2.707889318466186523, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+349, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -733.248291015625, -1672.1336669921875, 16.15409278869628906, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+350, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -602.26214599609375, -1553.4322509765625, 11.04879188537597656, 1.716076016426086425, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+351, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -763.16827392578125, -1660.291748046875, 9.06526947021484375, 3.14080214500427246, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+352, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -761.73785400390625, -1596.51220703125, 25.23956871032714843, 2.866182327270507812, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+353, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -573.8194580078125, -1719.3160400390625, 8.029664993286132812, 2.22252821922302246, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+354, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -688.34722900390625, -1472.921875, 130.292510986328125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+355, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -580.263916015625, -1728.6597900390625, 7.739507198333740234, 5.435998916625976562, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+356, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -587.00146484375, -1549.8109130859375, 9.443577766418457031, 1.876753449440002441, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+357, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -723.0111083984375, -1624.2166748046875, 20.03540420532226562, 5.032073974609375, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+358, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -726.2027587890625, -1619.2034912109375, 21.28293037414550781, 4.826416015625, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+359, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -809.14410400390625, -1685.017333984375, 7.3791351318359375, 0.883413672447204589, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+360, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -650.00177001953125, -1554.3853759765625, 6.075273990631103515, 6.253777503967285156, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+361, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -696.85589599609375, -1652.751708984375, 46.18582534790039062, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+362, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -669.5771484375, -1534.45068359375, 3.344237327575683593, 4.575816631317138671, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+363, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -720.36676025390625, -1666.5247802734375, 16.10054206848144531, 4.364813804626464843, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+364, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -976.092041015625, -1636.3541259765625, 5.69404458999633789, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+365, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -514.25177001953125, -1677.96533203125, 9.17285919189453125, 5.052550315856933593, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+366, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -731.42364501953125, -1759.3072509765625, 27.9876708984375, 4.890123844146728515, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+367, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -977.05902099609375, -1645.1632080078125, 5.702335357666015625, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+368, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -783.37677001953125, -1766.5521240234375, 11.33739662170410156, 4.904110431671142578, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+369, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -666.28558349609375, -1668.627197265625, 16.32196998596191406, 4.087627410888671875, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+370, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1210.3680419921875, -1529.8194580078125, 21.50630378723144531, 1.410326480865478515, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+371, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -787.304443359375, -1667.5703125, 4.9112548828125, 3.190003871917724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+372, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -644.15625, -1742.4114990234375, 19.0265350341796875, 5.735540390014648437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+373, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1061.5694580078125, -1400.8941650390625, 9.237018585205078125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+374, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -966.88714599609375, -1614.10595703125, 8.69875335693359375, 6.077647686004638671, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+375, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -725.89886474609375, -1716.8712158203125, 24.60250282287597656, 4.769777774810791015, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+376, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -823.89239501953125, -1637.1805419921875, 6.655963897705078125, 2.93420267105102539, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+377, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -976.279541015625, -1618.59033203125, 5.694042205810546875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+378, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -971.6719970703125, -1640.7469482421875, 5.609446048736572265, 0.731790482997894287, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+379, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -601.81768798828125, -1544.82470703125, 6.10074615478515625, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+380, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -557.97222900390625, -1564.013916015625, 11.15660476684570312, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+381, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -572.154541015625, -1547.94970703125, 10.62143325805664062, 1.159682512283325195, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+382, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -575.2554931640625, -1660.5565185546875, 10.80485248565673828, 4.070003986358642578, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+383, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1052.1754150390625, -1406.90625, 9.218511581420898437, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+384, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -582.44964599609375, -1577.611083984375, 12.55775928497314453, 3.67585301399230957, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+385, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -702.83953857421875, -1693.1439208984375, 16.75620079040527343, 4.979718208312988281, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+386, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -695.578125, -1456.8629150390625, 130.3090057373046875, 0.536964654922485351, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+387, 64864, 870, 5785, 6533, 1, 1780, 0, 0, 0, -737.57293701171875, -1453.703125, 149.9104461669921875, 5.998937606811523437, 120, 0, 0, 0, 0, 0, 0, 55960), -- Airman Clifton (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+388, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -678.015625, -1589.8385009765625, 17.00021553039550781, 2.100876569747924804, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+389, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -605.388916015625, -1519.4254150390625, 1.055689215660095214, 1.148104667663574218, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+390, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -685.98089599609375, -1733.4461669921875, 12.81303977966308593, 4.017093181610107421, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+391, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -558.921875, -1706.486083984375, 8.13785552978515625, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+392, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -715.75140380859375, -1621.773681640625, 17.67165374755859375, 1.160562753677368164, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+393, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1064.8194580078125, -1390.5850830078125, 6.75647735595703125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+394, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -655.75518798828125, -1668.185791015625, 15.24379348754882812, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+395, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -802.982666015625, -1763.6978759765625, 9.775233268737792968, 4.627320289611816406, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+396, 66434, 870, 5785, 6533, 1, 1685, 0, 0, 0, -728.2430419921875, -1544.842041015625, 34.24018478393554687, 1.240985631942749023, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Cannon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+397, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -972.765625, -1623.8819580078125, 5.694042205810546875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+398, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -647.28472900390625, -1483.4722900390625, 130.406402587890625, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+399, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -538.73956298828125, -1616.111083984375, 11.24554157257080078, 1.013408422470092773, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+400, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -760.64093017578125, -1663.55810546875, 9.386017799377441406, 3.136222362518310546, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+401, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -551.81396484375, -1675.095703125, 10.5055694580078125, 1.744890689849853515, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+402, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -724.941162109375, -1627.56298828125, 19.41633987426757812, 5.005006790161132812, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+403, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -733.50518798828125, -1761.7569580078125, 27.43190956115722656, 4.976947307586669921, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+404, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -629.51971435546875, -1537.292724609375, 5.311948776245117187, 0.40319254994392395, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+405, 62167, 870, 5785, 6533, 1, 1780, 0, 0, 0, -677.546875, -1475.9132080078125, 130.285003662109375, 6.240445613861083984, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+406, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -669.76739501953125, -1746.8021240234375, 19.68236923217773437, 4.18827676773071289, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+407, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -667.47650146484375, -1532.3089599609375, 3.344237327575683593, 4.643700599670410156, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+408, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -719.25909423828125, -1664.1375732421875, 15.8906106948852539, 4.318574905395507812, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+409, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -977.6492919921875, -1638.2257080078125, 6.742861747741699218, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+410, 66889, 870, 5785, 6533, 1, 1780, 0, 0, 0, 0, 0, 0, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+411, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -643.38055419921875, -1663.651123046875, 14.34772014617919921, 1.09088897705078125, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+412, 66889, 870, 5785, 6533, 1, 1685, 0, 0, 0, 0, 0, 0, 1.961214542388916015, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+413, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -551.248291015625, -1596.3228759765625, 12.16536903381347656, 0.555077314376831054, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+414, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -847.34722900390625, -1595.234375, 3.062809467315673828, 3.04917144775390625, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+415, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -730.71356201171875, -1681.1492919921875, 17.2212371826171875, 3.104922056198120117, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+416, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -793.0675048828125, -1656.033447265625, 3.824413776397705078, 5.167051315307617187, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+417, 66890, 870, 5785, 6533, 1, 1780, 0, 0, 0, -780.8819580078125, -1787.710693359375, 36.63570404052734375, 5.820490360260009765, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Master (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+418, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+419, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -970.5867919921875, -1644.6302490234375, 5.706992626190185546, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+420, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -967.2467041015625, -1633.7728271484375, 5.60926055908203125, 5.756136417388916015, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+421, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -577.85589599609375, -1724.71875, 7.981745719909667968, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+422, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -716.09027099609375, -1781.7274169921875, 0.578185021877288818, 3.611253976821899414, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+423, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -633.38128662109375, -1718.1287841796875, 13.83900737762451171, 5.208891868591308593, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+424, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -613.2183837890625, -1662.4169921875, 12.26436424255371093, 3.735481739044189453, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+425, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -684.74652099609375, -1459.564208984375, 130.3090057373046875, 0.536964654922485351, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+426, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1061.9722900390625, -1400.4375, 7.808002948760986328, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+427, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -557.076416015625, -1610.7535400390625, 13.77522754669189453, 3.114799022674560546, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+428, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -701.70281982421875, -1690.3675537109375, 16.75620079040527343, 5.126640796661376953, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+429, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -710.81597900390625, -1532.732666015625, 3.420492172241210937, 1.922062993049621582, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+430, 66473, 870, 5785, 6533, 1, 1780, 0, 0, 0, -702.77606201171875, -1483.1754150390625, 130.3090057373046875, 5.058022499084472656, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 81312 - Self Stun Forever)
(@CGUID+431, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -738.15972900390625, -1676.9947509765625, 17.3520660400390625, 5.784726619720458984, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+432, 66292, 870, 5785, 6533, 1, 1685, 0, 0, 0, -664.671875, -1483.295166015625, 130.28326416015625, 2.838113784790039062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Sky Admiral Rogers (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 421177 - -Unknown-, 421175 - -Unknown-)
(@CGUID+433, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -737.97918701171875, -1727.6336669921875, 27.77976226806640625, 0.527491271495819091, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+434, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1061.0225830078125, -1407.2742919921875, 8.6458282470703125, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+435, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -600.91839599609375, -1629.6163330078125, 13.93139839172363281, 2.855042934417724609, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+436, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -711.8863525390625, -1684.31884765625, 16.1476287841796875, 5.497604846954345703, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+437, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -690.1875, -1620.3802490234375, 22.73499107360839843, 3.790329694747924804, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+438, 66434, 870, 5785, 6533, 1, 1685, 0, 0, 0, -659.49652099609375, -1756.76220703125, 19.0265350341796875, 4.768757820129394531, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Cannon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+439, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -582.7117919921875, -1581.4739990234375, 12.85347175598144531, 2.255315065383911132, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+440, 66530, 870, 5785, 6533, 1, 1685, 0, 0, 0, -970.9617919921875, -1624.1458740234375, 5.694042205810546875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 1 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+441, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -747.08331298828125, -1560.2135009765625, 34.89602279663085937, 4.287228107452392578, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+442, 54618, 870, 5785, 6533, 1, 1780, 0, 0, 0, -679.982666015625, -1487.3211669921875, 130.4415740966796875, 3.458302021026611328, 120, 0, 0, 0, 0, 0, 0, 55960), -- Nimm Codejack (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+443, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 1, -645.435791015625, -1486.7396240234375, 131.961029052734375, 0.586022734642028808, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+444, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -671.14581298828125, -1490.5225830078125, 1.191768407821655273, 1.383535623550415039, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+445, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -781.83282470703125, -1731.233642578125, 9.639462471008300781, 4.701758384704589843, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+446, 66435, 870, 5785, 6533, 1, 1685, 0, 0, 0, -777.482666015625, -1749.7257080078125, 10.95946979522705078, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Explosive Barrel (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+447, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -653.74652099609375, -1534.734375, 2.67402505874633789, 3.885772466659545898, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+448, 66559, 870, 5785, 6533, 1, 1685, 0, 0, 0, -1080.73095703125, -1367.029541015625, 3.823587894439697265, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Ship 2 Explosives (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+449, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -740.6336669921875, -1632.3160400390625, 16.53662109375, 3.777277469635009765, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+450, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -559.07464599609375, -1571.671875, 11.81286239624023437, 5.8538970947265625, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+451, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -824.21636962890625, -1756.3428955078125, 8.759642601013183593, 2.844453334808349609, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+452, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -662.48516845703125, -1501.1739501953125, 1.646908402442932128, 5.655801773071289062, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+453, 66434, 870, 5785, 6533, 1, 1685, 0, 0, 0, -646.045166015625, -1748.4427490234375, 19.0265350341796875, 5.780529499053955078, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Cannon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+454, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -666.11114501953125, -1515.3541259765625, 2.167972326278686523, 1.843972086906433105, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+455, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -647.10418701171875, -1552.40625, 6.0645904541015625, 4.647157669067382812, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+456, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -580.2650146484375, -1674.2806396484375, 10.64075374603271484, 5.469912528991699218, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+457, 66527, 870, 5785, 6533, 1, 1780, 0, 0, 1, -667.13018798828125, -1467.3125, 130.2440032958984375, 4.243234157562255859, 120, 0, 0, 0, 0, 0, 0, 55960), -- Mishka (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+458, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -665.7413330078125, -1589.657958984375, 15.51859664916992187, 2.752358675003051757, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+459, 66890, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1045.6990966796875, -1445.88134765625, 25.92001724243164062, 6.267036914825439453, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Master (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+460, 65840, 870, 5785, 6533, 1, 1780, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+461, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -662.45489501953125, -1725.57470703125, 19.68236923217773437, 2.251068830490112304, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+462, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -795.29010009765625, -1658.0484619140625, 3.824413776397705078, 5.033240795135498046, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+463, 66890, 870, 5785, 6533, 1, 1780, 0, 0, 0, -1002.24566650390625, -1614.61572265625, 17.77522468566894531, 1.579473614692687988, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Gyrocopter Master (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+464, 65840, 870, 5785, 6533, 1, 1685, 0, 0, 1, 0.011486838571727275, 0, 0.073615282773971557, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Strongarm Airman (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 46598 - Ride Vehicle Hardcoded) - !!! on transport - transport template not found !!!
(@CGUID+465, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -781.53472900390625, -1608.029541015625, 11.78140735626220703, 2.42432570457458496, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+466, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -694.13677978515625, -1736.404296875, 9.878665924072265625, 4.016971111297607421, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+467, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -636.3846435546875, -1717.60205078125, 13.97934913635253906, 5.202252864837646484, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+468, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -700.5625, -1731.9322509765625, 37.25930404663085937, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+469, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -831.1593017578125, -1631.2144775390625, 2.899588584899902343, 3.498440504074096679, 120, 10, 0, 1, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1 (possible waypoints or random movement)
(@CGUID+470, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -675.5555419921875, -1588.8629150390625, 17.01610946655273437, 1.483274698257446289, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+471, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -766.69793701171875, -1694.6944580078125, 10.3967132568359375, 4.200914382934570312, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+472, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -519.35174560546875, -1655.7412109375, 9.555878639221191406, 3.43516087532043457, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+473, 66382, 870, 5785, 6533, 1, 1685, 0, 0, 0, -397.619781494140625, -1426.421875, 116.4212265014648437, 3.881088018417358398, 120, 0, 0, 0, 0, 0, 0, 55960), -- Gyrocopter Facing Bunny (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 132350 - FX_JF_SquadronBunny_Flavor)
(@CGUID+474, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 0, -723.0294189453125, -1691.1165771484375, 17.73539161682128906, 4.198885917663574218, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+475, 66480, 870, 5785, 6533, 1, 1780, 0, 0, 1, -704.296875, -1481.71533203125, 130.53790283203125, 5.626270771026611328, 120, 0, 0, 0, 0, 0, 0, 55960), -- Skyfire Engineer (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+476, 66399, 870, 5785, 6533, 1, 1685, 0, 0, 1, -738.71356201171875, -1680.310791015625, 17.4099578857421875, 6.175532817840576171, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Peon (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+477, 40789, 870, 5785, 6533, 1, 1685, 0, 0, 0, -606.08160400390625, -1454.9739990234375, 93.23944091796875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Generic Controller Bunny (CSA) (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+478, 40789, 870, 5785, 6533, 1, 1685, 0, 0, 0, -625.33160400390625, -1528.984375, 93.23944091796875, 0, 120, 0, 0, 0, 0, 0, 0, 55960), -- Generic Controller Bunny (CSA) (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop)
(@CGUID+479, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -520.71875, -1726.904296875, 2.802292823791503906, 4.89102029800415039, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+480, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -536.671875, -1736.2117919921875, 7.68720865249633789, 6.029321193695068359, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+481, 66397, 870, 5785, 6533, 1, 1685, 0, 0, 0, -556.16143798828125, -1747.032958984375, 2.416233301162719726, 5.130914688110351562, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Shredder (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+482, 66398, 870, 5785, 6533, 1, 1685, 0, 0, 1, -537.3819580078125, -1738.7274169921875, 8.125420570373535156, 5.630265712738037109, 120, 0, 0, 0, 0, 0, 0, 55960), -- Garrosh'ar Grunt (Area: The Skyfire - Difficulty: 0) CreateObject1
(@CGUID+483, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -571.2569580078125, -1658.1805419921875, 10.72843456268310546, 4.215371608734130859, 120, 0, 0, 0, 0, 0, 0, 55960), -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
(@CGUID+484, 62167, 870, 5785, 6533, 1, 1685, 0, 0, 0, -716.5867919921875, -1695.013916015625, 30.59600639343261718, 4.712605953216552734, 120, 0, 0, 0, 0, 0, 0, 55960); -- General Purpose Bunny ZTO (Area: The Skyfire - Difficulty: 0) CreateObject1 (Auras: )
DELETE FROM `creature_addon` WHERE `guid` BETWEEN @CGUID+0 AND @CGUID+484;
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvpFlags`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) VALUES
(@CGUID+0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter
(@CGUID+1, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+3, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+4, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+5, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+7, 0, 0, 0, 0, 0, 1, 0, 133, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+9, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+12, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+13, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+14, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+15, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+16, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+17, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+18, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- General Purpose Bunny ZTO - 106246 - Cosmetic - Burning Tent
(@CGUID+20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+21, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+22, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+23, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+24, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+26, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+27, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+28, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+29, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- General Purpose Bunny ZTO - 106246 - Cosmetic - Burning Tent
(@CGUID+30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+32, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+33, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+34, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+37, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+38, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+39, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+40, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+41, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+42, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+43, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+44, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+45, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+46, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+47, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+48, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+49, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+50, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+51, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+52, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+53, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+54, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+55, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+56, 0, 0, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- Rell Nightwind
(@CGUID+58, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+59, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+60, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+61, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '421177 421175'), -- General Purpose Bunny ZTO - 421177 - -Unknown-, 421175 - -Unknown-
(@CGUID+62, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+63, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+64, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+65, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+66, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+67, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+68, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+69, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+70, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+71, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+73, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+74, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+76, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+77, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+78, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+79, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+80, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+81, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+82, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+83, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+84, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+85, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+86, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+87, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+88, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+89, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+90, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+91, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+92, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+93, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+94, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+95, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+96, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+97, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+98, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+99, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+100, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+101, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+102, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+103, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+104, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Hicks
(@CGUID+105, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+106, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+107, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+108, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+109, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+110, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+111, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+112, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+113, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+114, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+115, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '130628'), -- General Purpose Bunny ZTO - 130628 - Assault on Garrosh'ar Point
(@CGUID+116, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+117, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+118, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+119, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+120, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Captain Day
(@CGUID+121, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+122, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+123, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+124, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter Master
(@CGUID+125, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+126, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+127, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+128, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+129, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+130, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+131, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+132, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+134, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+135, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+136, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+137, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+138, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+139, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+140, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+141, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+142, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+143, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+145, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+146, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+147, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+148, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+149, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, '83810'), -- Sky Mage Harlan - 83810 - Cosmetic - Water Channel
(@CGUID+150, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+151, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+152, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+153, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+154, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+155, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+156, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+157, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132223'), -- Generic Controller Bunny (CSA) - 132223 - FX_AllianceGunship_Idle_Main_Loop
(@CGUID+158, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+159, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+160, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+161, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+162, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+163, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+164, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+165, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+166, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+167, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+168, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+169, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+170, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+171, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+172, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+173, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+174, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+175, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+176, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+178, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter
(@CGUID+179, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+180, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+181, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+182, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+183, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Cannon
(@CGUID+184, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+185, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+186, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+187, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+188, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+189, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+190, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+191, 0, 0, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- Amber Kearnen
(@CGUID+192, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+193, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+194, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '131025'), -- Skyfire Gyrocopter - 131025 - Gyrocopter Anim Set, Ground
(@CGUID+195, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+196, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+197, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+198, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+199, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+201, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+202, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+203, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+204, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+205, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+206, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+207, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+209, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+210, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+211, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- General Purpose Bunny ZTO - 106246 - Cosmetic - Burning Tent
(@CGUID+212, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+213, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+214, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+215, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+216, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter
(@CGUID+217, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+218, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+219, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+220, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+221, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+222, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+223, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+224, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+225, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Gyrocopter Turret - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+226, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+227, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+228, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+229, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+230, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+231, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+232, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+233, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+234, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+235, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+236, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Sky Captain Callanan
(@CGUID+238, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+239, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+240, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Cannon
(@CGUID+241, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+242, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+243, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- General Purpose Bunny ZTO - 106246 - Cosmetic - Burning Tent
(@CGUID+244, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+245, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+246, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+247, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+248, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+249, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+250, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+251, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+252, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+254, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+255, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+256, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+257, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+258, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- General Purpose Bunny ZTO - 106246 - Cosmetic - Burning Tent
(@CGUID+259, 0, 0, 0, 0, 0, 1, 1, 483, 0, 0, 0, 0, '421177 421175'), -- Corporal Rasmussen - 421177 - -Unknown-, 421175 - -Unknown-
(@CGUID+260, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+261, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+262, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''); -- Garrosh'ar Grunt

INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvpFlags`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) VALUES
(@CGUID+263, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+264, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+265, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+266, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter Master
(@CGUID+267, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+268, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+269, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+270, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+271, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+272, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+273, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+274, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+275, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+276, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+277, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+278, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+279, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+280, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+281, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+283, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+284, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+285, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+286, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+287, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+288, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+289, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- Captain Kerwin
(@CGUID+290, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+291, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+292, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+293, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+294, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+295, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+296, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+297, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+299, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+300, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+301, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+302, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+303, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+304, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+305, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+306, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+307, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+308, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+309, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter Master
(@CGUID+310, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+311, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+312, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+313, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+314, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+315, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+316, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+317, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+318, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+319, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+320, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+321, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+322, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Ellis
(@CGUID+323, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+324, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+325, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+326, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+327, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+329, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+330, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+331, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+332, 0, 0, 0, 0, 0, 1, 0, 233, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+333, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+334, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+335, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+336, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+337, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+339, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+340, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+341, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+343, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+344, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+345, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+346, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+347, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+348, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+349, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+350, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+351, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+352, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+353, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+355, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+356, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+357, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+358, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+359, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+360, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+361, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+362, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+363, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+364, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+365, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+366, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+367, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+368, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+369, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+370, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter
(@CGUID+371, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+372, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+373, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+374, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+375, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+376, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+377, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+378, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+379, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+380, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+381, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+382, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+383, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+384, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+385, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+386, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+387, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- Airman Clifton
(@CGUID+388, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+389, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+390, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+391, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+392, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+393, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+394, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+395, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+396, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Cannon
(@CGUID+397, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+399, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+400, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+401, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+402, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+403, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+404, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+406, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+407, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+408, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+409, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+410, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+411, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+412, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- Skyfire Gyrocopter - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+413, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+414, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+415, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+416, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+417, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter Master
(@CGUID+418, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+419, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+420, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+421, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+422, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+423, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+424, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+425, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+426, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+427, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+428, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+429, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+430, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- Skyfire Gyrocopter - 81312 - Self Stun Forever
(@CGUID+431, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+432, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '421177 421175'), -- Sky Admiral Rogers - 421177 - -Unknown-, 421175 - -Unknown-
(@CGUID+433, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+434, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+435, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+436, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+437, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+438, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Cannon
(@CGUID+439, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+440, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 1 Explosives
(@CGUID+441, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+442, 0, 0, 8, 0, 0, 1, 0, 233, 0, 0, 0, 0, ''), -- Nimm Codejack
(@CGUID+443, 0, 0, 0, 0, 0, 1, 0, 233, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+444, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+445, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+446, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Explosive Barrel
(@CGUID+447, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+448, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Ship 2 Explosives
(@CGUID+449, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+450, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+451, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+452, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+453, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Cannon
(@CGUID+454, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+455, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+456, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+458, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+459, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter Master
(@CGUID+460, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+461, 0, 0, 0, 0, 0, 1, 0, 333, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+462, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+463, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Skyfire Gyrocopter Master
(@CGUID+464, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- Strongarm Airman - 46598 - Ride Vehicle Hardcoded - !!! on transport - transport template not found !!!
(@CGUID+465, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+466, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+467, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+468, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- Gyrocopter Facing Bunny
(@CGUID+469, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+470, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+471, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+472, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+473, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, '132350'), -- Gyrocopter Facing Bunny - 132350 - FX_JF_SquadronBunny_Flavor
(@CGUID+474, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+475, 0, 0, 0, 0, 0, 1, 0, 233, 0, 0, 0, 0, ''), -- Skyfire Engineer
(@CGUID+476, 0, 0, 0, 0, 0, 1, 0, 234, 0, 0, 0, 3, ''), -- Garrosh'ar Peon
(@CGUID+477, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+478, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '132221'), -- Generic Controller Bunny (CSA) - 132221 - FX_Uni_Zeppelin_Propeller_Blades_Loop
(@CGUID+479, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+480, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''), -- Garrosh'ar Grunt
(@CGUID+481, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- Garrosh'ar Shredder
(@CGUID+482, 0, 0, 0, 0, 0, 2, 0, 214, 0, 0, 0, 3, ''); -- Garrosh'ar Grunt


-- gameboject
SET @OGUID:=4000077;
DELETE FROM `gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+29;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `PhaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID+0, 213763, 870, 5785, 5853, 1, 1685, 0, -596.3819580078125, -1330.095458984375, 2.430555582046508789, 2.059484481811523437, 0, 0, 0.857166290283203125, 0.515039741992950439, 120, 255, 1, 55939), -- Campfire (Area: Garrosh'ar Point - Difficulty: 0) CreateObject1
(@OGUID+1, 213762, 870, 5785, 5853, 1, 1685, 0, -753.0711669921875, -1370.5816650390625, 3.567708253860473632, 2.408554315567016601, 0, 0, 0.933580398559570312, 0.358368009328842163, 120, 255, 1, 55939), -- Campfire (Area: Garrosh'ar Point - Difficulty: 0) CreateObject1
(@OGUID+2, 213764, 870, 5785, 5853, 1, 1685, 0, -857.8663330078125, -1431.1805419921875, 1.7537384033203125, 1.230455756187438964, 0, 0, 0.577144622802734375, 0.816641986370086669, 120, 255, 1, 55939), -- Campfire (Area: Garrosh'ar Point - Difficulty: 0) CreateObject1
(@OGUID+3, 212174, 870, 5785, 6533, 1, 1685, 0, -728.00518798828125, -1334.310791015625, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Reef Octopus Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+4, 215638, 870, 5785, 6533, 1, 1780, 0, -688.1632080078125, -1501.74658203125, 131.4707489013671875, 6.034336566925048828, -0.05271816253662109, -0.12563133239746093, -0.12777042388916015, 0.982401251792907714, 120, 255, 1, 55939), -- Orc Blade (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+5, 212174, 870, 5785, 6533, 1, 1685, 0, -689.74481201171875, -1472.5103759765625, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Reef Octopus Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+6, 215185, 870, 5785, 6533, 1, 1685, 0, -569.05902099609375, -1594.4757080078125, 14.36010646820068359, 1.749078631401062011, 0, 0, 0.767248153686523437, 0.641350388526916503, 120, 255, 1, 55939), -- Spring (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+7, 215187, 870, 5785, 6533, 1, 1685, 0, -568.875, -1591.96875, 14.44148635864257812, 2.285418987274169921, -0.5648360252380371, -0.21050739288330078, 0.791736602783203125, 0.098995335400104522, 120, 255, 1, 55939), -- Nut (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+8, 215638, 870, 5785, 6533, 1, 1780, 0, -643.029541015625, -1503.6353759765625, 140.172393798828125, 1.918493270874023437, -0.54618644714355468, -0.80658817291259765, 0.133848190307617187, 0.182155415415763854, 120, 255, 1, 55939), -- Orc Blade (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+9, 212169, 870, 5785, 6533, 1, 1685, 0, -824.19793701171875, -1582.0504150390625, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Giant Mantis Shrimp Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+10, 215133, 870, 5785, 6533, 1, 1685, 0, -504.072906494140625, -1493.80908203125, 23.78573799133300781, 2.967084884643554687, 0, 0, 0.996195793151855468, 0.08714316040277481, 120, 255, 1, 55939), -- Applebloom Cider (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+11, 215185, 870, 5785, 6533, 1, 1685, 0, -568.1180419921875, -1592.3541259765625, 14.45248603820800781, 3.315586090087890625, 0.037325859069824218, -0.81508636474609375, -0.56985950469970703, 0.097474195063114166, 120, 255, 1, 55939), -- Spring (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+12, 215133, 870, 5785, 6533, 1, 1685, 0, -550.15972900390625, -1466.5103759765625, 3.721319437026977539, 2.364506721496582031, 0.416350841522216796, 0.494716644287109375, 0.751502037048339843, 0.13096591830253601, 120, 255, 1, 55939), -- Applebloom Cider (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+13, 212174, 870, 5785, 6533, 1, 1685, 0, -782.4149169921875, -1423.84033203125, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Reef Octopus Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+14, 212169, 870, 5785, 6533, 1, 1685, 0, -810.560791015625, -1565.0103759765625, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Giant Mantis Shrimp Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+15, 209353, 870, 5785, 6533, 1, 1685, 0, -755.62152099609375, -1397.4166259765625, 4.55926513671875, 0, 0, 0, 0, 1, 120, 255, 1, 55939), -- Rain Poppy (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+16, 215187, 870, 5785, 6533, 1, 1685, 0, -569.14581298828125, -1593.0086669921875, 14.44627857208251953, 4.897857666015625, -0.05547046661376953, 0.600230216979980468, -0.30259037017822265, 0.738299250602722167, 120, 255, 1, 55939), -- Nut (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+17, 215638, 870, 5785, 6533, 1, 1780, 0, -728.248291015625, -1477.9271240234375, 150.952667236328125, 5.97043466567993164, -0.14748144149780273, 0.303347587585449218, -0.09023380279541015, 0.937063157558441162, 120, 255, 1, 55939), -- Orc Blade (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+18, 212174, 870, 5785, 6533, 1, 1685, 0, -712.68927001953125, -1500.4097900390625, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Reef Octopus Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+19, 215187, 870, 5785, 6533, 1, 1685, 0, -567.279541015625, -1592.8802490234375, 14.40350246429443359, 3.651848793029785156, 0.305171489715576171, 0.519830703735351562, -0.67652320861816406, 0.423039853572845458, 120, 255, 1, 55939), -- Nut (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+20, 215835, 870, 5785, 6533, 1, 1685, 0, -703.18927001953125, -1366.7413330078125, 2.530099153518676757, 1.208128571510314941, 0, 0, 0.567992210388183593, 0.823033928871154785, 120, 255, 0, 55939), -- Alliance Gunship Portal Effects (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+21, 200296, 870, 5785, 6533, 1, 1780, 0, -629.359375, -1482.4375, 133.246917724609375, 0, 0, 0, 0, 1, 120, 255, 1, 55939), -- Washing Tub (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+22, 215638, 870, 5785, 6533, 1, 1780, 0, -660.25177001953125, -1466.345458984375, 131.4305267333984375, 5.97043466567993164, -0.14748144149780273, 0.303347587585449218, -0.09023380279541015, 0.937063157558441162, 120, 255, 1, 55939), -- Orc Blade (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+23, 215834, 870, 5785, 6533, 1, 1685, 0, -703.170166015625, -1366.8211669921875, 2.538779497146606445, 1.208128571510314941, 0, 0, 0.567992210388183593, 0.823033928871154785, 120, 255, 1, 55939), -- Gunship Portal (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+24, 212169, 870, 5785, 6533, 1, 1685, 0, -700.123291015625, -1290.0399169921875, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Giant Mantis Shrimp Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+25, 212174, 870, 5785, 6533, 1, 1685, 0, -740.65802001953125, -1427.5208740234375, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Reef Octopus Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+26, 215133, 870, 5785, 6533, 1, 1685, 0, -531.3507080078125, -1426.1944580078125, 2.171048879623413085, 5.000845909118652343, -0.09763956069946289, -0.17023372650146484, -0.58274173736572265, 0.788605749607086181, 120, 255, 1, 55939), -- Applebloom Cider (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+27, 215133, 870, 5785, 6533, 1, 1685, 0, -472.723968505859375, -1497.107666015625, 19.85261726379394531, 5.012874603271484375, 0, 0, -0.59330272674560546, 0.804979443550109863, 120, 255, 1, 55939), -- Applebloom Cider (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+28, 212169, 870, 5785, 6533, 1, 1685, 0, -843.9757080078125, -1615.7708740234375, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939), -- Giant Mantis Shrimp Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
(@OGUID+29, 212169, 870, 5785, 6533, 1, 1685, 0, -909.9149169921875, -1467.0208740234375, 0, 3.141592741012573242, 0, 0, -1, 0, 120, 255, 1, 55939); -- Giant Mantis Shrimp Swarm (Area: The Skyfire - Difficulty: 0) CreateObject1
DELETE FROM `gameobject_addon` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+29;


UPDATE `creature_template` SET `npcflag`=81, `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=66527; -- Mishka
UPDATE `creature_template` SET `npcflag`=3, `BaseAttackTime`=2000, `unit_flags`=256, `unit_flags2`=2048 WHERE `entry`=66292; -- Sky Admiral Rogers
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64864; -- Airman Clifton
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64862; -- Airman Ellis
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64860; -- Captain Kerwin
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64866; -- Corporal Rasmussen
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=66510; -- Sully "The Pickle" McLeary
UPDATE `creature_template` SET `unit_flags2`=2048 WHERE `entry`=64859; -- Sky Captain Callanan
UPDATE `creature_template` SET `faction`=35, `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=33554944, `unit_flags2`=2048 WHERE `entry`=66299; -- Gyrocopter Turret
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048, `VehicleId`=2458 WHERE `entry`=66297; -- Skyfire Gyrocopter
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags`=768, `unit_flags2`=2048 WHERE `entry`=66509; -- Amber Kearnen
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=512, `unit_flags2`=67110912 WHERE `entry`=66434; -- Garrosh'ar Cannon
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64863; -- Sky Mage Harlan
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=512, `unit_flags2`=2048, `VehicleId`=2490 WHERE `entry`=66890; -- Skyfire Gyrocopter Master
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64861; -- Captain Day
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=64858; -- Airman Hicks
UPDATE `creature_template` SET `unit_flags`=256 WHERE `entry`=66473; -- Skyfire Gyrocopter
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=54617; -- Rell Nightwind
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=512, `unit_flags2`=67110912 WHERE `entry`=66530; -- Ship 1 Explosives
UPDATE `creature_template` SET `unit_flags2`=2048 WHERE `entry`=66382; -- Gyrocopter Facing Bunny
UPDATE `creature_template` SET `VehicleId`=2473 WHERE `entry`=55370; -- General Purpose Bunny ZTO
UPDATE `creature_template` SET `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=66397; -- Garrosh'ar Shredder
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=512, `unit_flags2`=67110912 WHERE `entry`=66559; -- Ship 2 Explosives
UPDATE `creature_template` SET `speed_walk`=0.800000011920928955, `speed_run`=0.714285731315612792, `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=66398; -- Garrosh'ar Grunt
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=512, `unit_flags2`=67110912 WHERE `entry`=66435; -- Garrosh'ar Explosive Barrel
UPDATE `creature_template` SET `speed_walk`=0.800000011920928955, `speed_run`=0.714285731315612792, `BaseAttackTime`=2000, `unit_flags2`=2048 WHERE `entry`=66399; -- Garrosh'ar Peon
UPDATE `creature_template` SET `speed_run`=1.357142806053161621, `BaseAttackTime`=2000, `unit_flags`=512, `unit_flags2`=2048, `VehicleId`=2445 WHERE `entry`=66889; -- Skyfire Gyrocopter

-- 54618
-- 55370
-- 62167
-- 65840
-- 66480
-- creature_template_addon
DELETE FROM `creature_template_addon` WHERE `entry` IN (64866 /*64866 (Corporal Rasmussen) - -Unknown-, -Unknown-*/, 64864 /*64864 (Airman Clifton)*/, 54617 /*54617 (Rell Nightwind)*/, 54618 /*54618 (Nimm Codejack)*/, 66509 /*66509 (Amber Kearnen)*/, 66299 /*66299 (Gyrocopter Turret) - Ride Vehicle Hardcoded*/, 66297 /*66297 (Skyfire Gyrocopter) - Gyrocopter Anim Set, Ground*/, 55370 /*55370 (General Purpose Bunny ZTO) - Cosmetic - Burning Tent*/, 64863 /*64863 (Sky Mage Harlan) - Cosmetic - Water Channel*/, 64860 /*64860 (Captain Kerwin)*/, 64861 /*64861 (Captain Day)*/, 66292 /*66292 (Sky Admiral Rogers) - -Unknown-, -Unknown-*/, 64858 /*64858 (Airman Hicks)*/, 64859 /*64859 (Sky Captain Callanan)*/, 65840 /*65840 (Strongarm Airman) - Ride Vehicle Hardcoded*/, 66480 /*66480 (Skyfire Engineer)*/, 66473 /*66473 (Skyfire Gyrocopter) - Self Stun Forever*/, 66559 /*66559 (Ship 2 Explosives)*/, 66530 /*66530 (Ship 1 Explosives)*/, 66382 /*66382 (Gyrocopter Facing Bunny)*/, 66399 /*66399 (Garrosh'ar Peon)*/, 66889 /*66889 (Skyfire Gyrocopter) - Ride Vehicle Hardcoded*/, 66397 /*66397 (Garrosh'ar Shredder)*/, 62167 /*62167 (General Purpose Bunny ZTO) - Cosmetic - Burning Tent*/, 66435 /*66435 (Garrosh'ar Explosive Barrel)*/, 66434 /*66434 (Garrosh'ar Cannon)*/, 66398 /*66398 (Garrosh'ar Grunt)*/, 66890 /*66890 (Skyfire Gyrocopter Master)*/);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvpFlags`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) VALUES
(64866, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, '421177 421175'), -- 64866 (Corporal Rasmussen) - -Unknown-, -Unknown-
(64864, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- 64864 (Airman Clifton)
(54617, 0, 0, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- 54617 (Rell Nightwind)
(54618, 0, 0, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- 54618 (Nimm Codejack)
(66509, 0, 0, 8, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- 66509 (Amber Kearnen)
(66299, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- 66299 (Gyrocopter Turret) - Ride Vehicle Hardcoded
(66297, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '131025'), -- 66297 (Skyfire Gyrocopter) - Gyrocopter Anim Set, Ground
-- (55370, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- 55370 (General Purpose Bunny ZTO) - Cosmetic - Burning Tent
(64863, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, '83810'), -- 64863 (Sky Mage Harlan) - Cosmetic - Water Channel
(64860, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- 64860 (Captain Kerwin)
(64861, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, ''), -- 64861 (Captain Day)
(66292, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '421177 421175'), -- 66292 (Sky Admiral Rogers) - -Unknown-, -Unknown-
(64858, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- 64858 (Airman Hicks)
(64859, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, ''), -- 64859 (Sky Captain Callanan)
(65840, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '46598'), -- 65840 (Strongarm Airman) - Ride Vehicle Hardcoded
(66480, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 0, 0, ''), -- 66480 (Skyfire Engineer)
(66473, 0, 0, 0, 3, 0, 1, 0, 0, 1267, 0, 0, 0, '81312'), -- 66473 (Skyfire Gyrocopter) - Self Stun Forever
(66559, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- 66559 (Ship 2 Explosives)
(66530, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- 66530 (Ship 1 Explosives)
(66382, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, ''), -- 66382 (Gyrocopter Facing Bunny)
(66399, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- 66399 (Garrosh'ar Peon)
(66889, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, '46598'), -- 66889 (Skyfire Gyrocopter) - Ride Vehicle Hardcoded
(66397, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- 66397 (Garrosh'ar Shredder)
-- (62167, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, '106246'), -- 62167 (General Purpose Bunny ZTO) - Cosmetic - Burning Tent
(66435, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- 66435 (Garrosh'ar Explosive Barrel)
(66434, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- 66434 (Garrosh'ar Cannon)
(66398, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 3, ''), -- 66398 (Garrosh'ar Grunt)
(66890, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 4, ''); -- 66890 (Skyfire Gyrocopter Master)


UPDATE `creature_template_addon` SET `auras`='132221' WHERE `entry`=40789; -- 40789 (Generic Controller Bunny (CSA)) - FX_Uni_Zeppelin_Propeller_Blades_Loop

UPDATE `gameobject_template_addon` SET `faction`=1732, `flags`=32 WHERE `entry`=215834; -- Gunship Portal
UPDATE `gameobject_template_addon` SET `faction`=1375, `flags`=32 WHERE `entry`=215835; -- Alliance Gunship Portal Effects
UPDATE `gameobject_template_addon` SET `flags`=4 WHERE `entry`=215133; -- Applebloom Cider

