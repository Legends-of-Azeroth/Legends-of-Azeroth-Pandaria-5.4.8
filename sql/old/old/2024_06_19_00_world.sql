ALTER TABLE `creature`
	ADD COLUMN `phaseId` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `phaseMask`,
	ADD COLUMN `phaseGroup` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `phaseId`;
ALTER TABLE `gameobject`
	ADD COLUMN `phaseGroup` INT(10) UNSIGNED NOT NULL DEFAULT '0' AFTER `phaseId`;

DROP TABLE IF EXISTS `phase_area`;
CREATE TABLE `phase_area` (
  `AreaId` int(10) unsigned NOT NULL,
  `PhaseId` int(10) unsigned NOT NULL,
  `Comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`AreaId`,`PhaseId`)
);

DELETE FROM `phase_area` WHERE `AreaId`=5834 AND `PhaseId` IN (592,593,594,595,596,597,598);
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES 
(5834, 592, 'Pandaren starting zone - Cosmetic - Warrior Weapon Racks'),
(5834, 593, 'Pandaren starting zone - Cosmetic - Mage Weapon Racks'),
(5834, 594, 'Pandaren starting zone - Cosmetic - Hunter Weapon Racks'),
(5834, 595, 'Pandaren starting zone - Cosmetic - Priest Weapon Racks'),
(5834, 596, 'Pandaren starting zone - Cosmetic - Rogue Weapon Racks'),
(5834, 597, 'Pandaren starting zone - Cosmetic - Shaman Weapon Racks'),
(5834, 598, 'Pandaren starting zone - Cosmetic - Monk Weapon Racks');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup` IN (592,593,594,595,596,597,598) AND `SourceEntry`=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 592, 0, 0, 1, 15, 0, 1, 0, 0, 0, 0, 0, '', 'Phase 592 in area 5834 if player is Warrior'),
(26, 593, 0, 0, 1, 15, 0, 128, 0, 0, 0, 0, 0, '', 'Phase 593 in area 5834 if player is Mage'),
(26, 594, 0, 0, 1, 15, 0, 4, 0, 0, 0, 0, 0, '', 'Phase 594 in area 5834 if player is Hunter'),
(26, 595, 0, 0, 1, 15, 0, 16, 0, 0, 0, 0, 0, '', 'Phase 595 in area 5834 if player is Priest'),
(26, 596, 0, 0, 1, 15, 0, 8, 0, 0, 0, 0, 0, '', 'Phase 596 in area 5834 if player is Rogue'),
(26, 597, 0, 0, 1, 15, 0, 64, 0, 0, 0, 0, 0, '', 'Phase 597 in area 5834 if player is Shaman'),
(26, 598, 0, 0, 1, 15, 0, 512, 0, 0, 0, 0, 0, '', 'Phase 598 in area 5834 if player is Monk');

DELETE FROM `gameobject_template` WHERE entry IN (210005, 210015, 210016, 210017, 210018, 210019, 210020);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `unkInt32`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES
(210005, 3, 10721, 'Weapon Rack', '', '', '', 1, 77279, 77278, 0, 0, 0, 0, 57, 40856, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13360, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414),
(210015, 3, 10721, 'Weapon Rack', '', '', '', 1, 76392, 76390, 0, 0, 0, 0, 57, 40859, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13361, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414),
(210016, 3, 10721, 'Weapon Rack', '', '', '', 1, 73211, 0, 0, 0, 0, 0,     57, 40860, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13362, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414),
(210017, 3, 10721, 'Weapon Rack', '', '', '', 1, 73207, 76393, 0, 0, 0, 0, 57, 40861, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13363, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414),
(210018, 3, 10721, 'Weapon Rack', '', '', '', 1, 73208, 73212, 0, 0, 0, 0, 57, 40862, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13365, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414),
(210019, 3, 10721, 'Weapon Rack', '', '', '', 1, 76391, 73213, 0, 0, 0, 0, 57, 40863, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13366, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414),
(210020, 3, 10721, 'Weapon Rack', '', '', '', 1, 73210, 0, 0, 0, 0, 0,     57, 40864, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13364, 0, 0, 0, 0, 0, 0, 0, 0, 128680, 0, 0, 0, 0, 0, 0, '', '', 18414);

DELETE FROM `gameobject_loot_template` WHERE `Entry` IN(210005, 40856,40859,40860,40861,40862,40863,40864);
INSERT INTO `gameobject_loot_template` (`Entry`,`Item`,`ChanceOrQuestChance`) VALUES
(40856, 73209, -100),
(40859, 76392, -100),
(40859, 76390, -100),
(40860, 73211, -100),
(40861, 73207, -100),
(40861, 76393, -100),
(40862, 73208, -100),
(40862, 73212, -100),
(40863, 76391, -100),
(40863, 73213, -100),
(40864, 73210, -100);

SET @OGUID := 4000000;
DELETE FROM `gameobject` WHERE `id` IN (210005,210015,210016,210017,210018,210019,210020);
DELETE FROM `gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+69;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseId`, `phaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `ScriptName`) VALUES 
(@OGUID+0,  210005, 860, 5736, 5834, 1, 598, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+1,  210005, 860, 5736, 5834, 1, 598, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+2,  210005, 860, 5736, 5834, 1, 598, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+3,  210005, 860, 5736, 5834, 1, 598, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+4,  210005, 860, 5736, 5834, 1, 598, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, ''),
(@OGUID+5,  210005, 860, 5736, 5834, 1, 598, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+6,  210005, 860, 5736, 5834, 1, 598, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+7,  210005, 860, 5736, 5834, 1, 598, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+8,  210005, 860, 5736, 5834, 1, 598, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+9,  210005, 860, 5736, 5834, 1, 598, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+10, 210015, 860, 5736, 5834, 1, 593, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+11, 210015, 860, 5736, 5834, 1, 593, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+12, 210015, 860, 5736, 5834, 1, 593, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+13, 210015, 860, 5736, 5834, 1, 593, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+14, 210015, 860, 5736, 5834, 1, 593, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+15, 210015, 860, 5736, 5834, 1, 593, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+16, 210015, 860, 5736, 5834, 1, 593, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+17, 210015, 860, 5736, 5834, 1, 593, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+18, 210015, 860, 5736, 5834, 1, 593, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+19, 210015, 860, 5736, 5834, 1, 593, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, ''),
(@OGUID+20, 210016, 860, 5736, 5834, 1, 594, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+21, 210016, 860, 5736, 5834, 1, 594, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+22, 210016, 860, 5736, 5834, 1, 594, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+23, 210016, 860, 5736, 5834, 1, 594, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+24, 210016, 860, 5736, 5834, 1, 594, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+25, 210016, 860, 5736, 5834, 1, 594, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+26, 210016, 860, 5736, 5834, 1, 594, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+27, 210016, 860, 5736, 5834, 1, 594, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+28, 210016, 860, 5736, 5834, 1, 594, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+29, 210016, 860, 5736, 5834, 1, 594, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, ''),
(@OGUID+30, 210017, 860, 5736, 5834, 1, 595, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+31, 210017, 860, 5736, 5834, 1, 595, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+32, 210017, 860, 5736, 5834, 1, 595, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+33, 210017, 860, 5736, 5834, 1, 595, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+34, 210017, 860, 5736, 5834, 1, 595, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+35, 210017, 860, 5736, 5834, 1, 595, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+36, 210017, 860, 5736, 5834, 1, 595, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+37, 210017, 860, 5736, 5834, 1, 595, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+38, 210017, 860, 5736, 5834, 1, 595, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+39, 210017, 860, 5736, 5834, 1, 595, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, ''),
(@OGUID+40, 210018, 860, 5736, 5834, 1, 596, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+41, 210018, 860, 5736, 5834, 1, 596, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+42, 210018, 860, 5736, 5834, 1, 596, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+43, 210018, 860, 5736, 5834, 1, 596, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+44, 210018, 860, 5736, 5834, 1, 596, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+45, 210018, 860, 5736, 5834, 1, 596, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+46, 210018, 860, 5736, 5834, 1, 596, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+47, 210018, 860, 5736, 5834, 1, 596, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+48, 210018, 860, 5736, 5834, 1, 596, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+49, 210018, 860, 5736, 5834, 1, 596, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, ''),
(@OGUID+50, 210019, 860, 5736, 5834, 1, 597, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+51, 210019, 860, 5736, 5834, 1, 597, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+52, 210019, 860, 5736, 5834, 1, 597, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+53, 210019, 860, 5736, 5834, 1, 597, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+54, 210019, 860, 5736, 5834, 1, 597, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+55, 210019, 860, 5736, 5834, 1, 597, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+56, 210019, 860, 5736, 5834, 1, 597, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+57, 210019, 860, 5736, 5834, 1, 597, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+58, 210019, 860, 5736, 5834, 1, 597, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+59, 210019, 860, 5736, 5834, 1, 597, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, ''),
(@OGUID+60, 210020, 860, 5736, 5834, 1, 592, 0, 1420.94, 3457.37, 171.185, 0.279252, 0, 0, 0.139173, 0.990268, 0, 255, 1, ''),
(@OGUID+61, 210020, 860, 5736, 5834, 1, 592, 0, 1443.33, 3450.59, 171.162, 4.11898, 0, 0, -0.882947, 0.469473, 0, 255, 1, ''),
(@OGUID+62, 210020, 860, 5736, 5834, 1, 592, 0, 1487.7, 3450.5, 171.175, 4.20625, 0, 0, -0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+63, 210020, 860, 5736, 5834, 1, 592, 0, 1487.86, 3407.09, 171.193, 2.07694, 0, 0, 0.861628, 0.507539, 0, 255, 1, ''),
(@OGUID+64, 210020, 860, 5736, 5834, 1, 592, 0, 1428.59, 3433.96, 171.162, 0.523598, 0, 0, 0.258819, 0.965926, 0, 255, 1, ''),
(@OGUID+65, 210020, 860, 5736, 5834, 1, 592, 0, 1430.43, 3429.18, 171.14, 0.401425, 0, 0, 0.199367, 0.979925, 0, 255, 1, ''),
(@OGUID+66, 210020, 860, 5736, 5834, 1, 592, 0, 1472.45, 3441.16, 171.183, 4.7822, 0, 0, -0.681998, 0.731354, 0, 255, 1, ''),
(@OGUID+67, 210020, 860, 5736, 5834, 1, 592, 0, 1435.92, 3460.55, 171.177, 3.56047, 0, 0, -0.978148, 0.207912, 0, 255, 1, ''),
(@OGUID+68, 210020, 860, 5736, 5834, 1, 592, 0, 1495, 3412.24, 171.231, 2.79252, 0, 0, 0.984807, 0.173652, 0, 255, 1, ''),
(@OGUID+69, 210020, 860, 5736, 5834, 1, 592, 0, 1478.45, 3398.19, 171.242, 2.04204, 0, 0, 0.85264, 0.522499, 0, 255, 1, '');

DELETE FROM `trinity_string` WHERE `entry` IN (12531,12532);
INSERT INTO `trinity_string` (`entry`,`content_default`) VALUES
(12531,'It consist of PhaseIDs:'),
(12532,'PhaseID: %u, PhaseGroup: %u');

DELETE FROM `command` WHERE `name` IN ('gobject set phaseid','modify phaseid','npc set phaseid','npc set phasegroup');
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('gobject set phaseid', 5, 'Syntax: .gobject set phase #guid #phaseid\r\n\r\nGameobject with DB guid #guid phaseid changed to #phaseid with related world vision update for players. Gameobject state saved to DB and persistent.'),
('modify phaseid', 2, 'Syntax: .modify phase #phaseid\r\n\r\nSelected character phaseid changed to #phaseid with related world vision update. Change active until in game phase changed, or GM-mode enable/disable, or re-login. Character pts pasemask update to same value.'),
('npc set phaseid', 5, 'Syntax: .npc set phase #phaseid\r\n\r\nSelected unit or pet phaseid changed to #phaseid with related world vision update for players. In creature case state saved to DB and persistent. In pet case change active until in game phase changed for owner, owner re-login, or GM-mode enable/disable..'),
('npc set phasegroup', 5, 'Syntax: .npc set phasegroup #phasegroup\r\n\r\nSelected unit or pet phasegroup changed to #phasegroup with related world vision update for players. In creature case state saved to DB and persistent. In pet case change active until in game phase changed for owner, owner re-login, or GM-mode enable/disable..');

DELETE FROM `phase_area` WHERE `AreaId`=1519 AND `PhaseId` IN (1687,1706);
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES 
(1519, 1687, 'See Rell Nightwind'),
(1519, 1706, 'Cosmetic - Stormwind - Can See Portal to Paw\'Don');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1687 AND `SourceEntry`=1519;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1687, 1519, 0, 0, 28, 0, 29547, 0, 0, 0, 0, 0, '', ''),
(26, 1687, 1519, 0, 1, 8, 0, 29547, 0, 0, 0, 0, 0, '', ''),
(26, 1687, 1519, 0, 1, 8, 0, 29548, 0, 0, 1, 0, 0, '', '');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1706 AND `SourceEntry`=1519;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1706, 1519, 0, 0, 28, 0, 29547, 0, 0, 0, 0, 0, '', ''),
(26, 1706, 1519, 0, 1, 8, 0, 29547, 0, 0, 0, 0, 0, '', '');

UPDATE `creature_template_addon` SET `auras` = '' WHERE `entry` = 55789;

UPDATE `creature` SET `phaseId` = 1687 WHERE `id` = 55789;
UPDATE `gameobject` SET `phaseId` = 1706  WHERE `id` = 215457;

UPDATE `gameobject_template` SET `Scriptname` = '' WHERE `Scriptname` = 'go_wandering_weapon_rack';
