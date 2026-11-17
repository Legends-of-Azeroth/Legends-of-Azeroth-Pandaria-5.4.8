DROP TABLE IF EXISTS `terrain_phase_info`;
CREATE TABLE `terrain_phase_info`(  
  Id INT(10) unsigned NOT NULL,
  TerrainSwapMap INT(10) unsigned NOT NULL,
  Comment VARCHAR(255),
  PRIMARY KEY (Id, TerrainSwapMap)
);

DROP TABLE IF EXISTS `terrain_swap_defaults`;
CREATE TABLE `terrain_swap_defaults`(  
  MapId INT(10) unsigned NOT NULL,
  TerrainSwapMap INT(10) unsigned NOT NULL,
  Comment VARCHAR(255),
  PRIMARY KEY (MapId, TerrainSwapMap)
);

DROP TABLE IF EXISTS `terrain_worldmap`;
CREATE TABLE `terrain_worldmap`(  
  TerrainSwapMap INT(10) unsigned NOT NULL,
  WorldMapArea INT(10) unsigned NOT NULL,
  Comment VARCHAR(255),
  PRIMARY KEY (TerrainSwapMap, WorldMapArea)
);

DELETE FROM `terrain_swap_defaults`;
INSERT INTO `terrain_swap_defaults` (`MapId`, `TerrainSwapMap`, `Comment`) VALUES 
(0, 736, 'Twilight Highlands - Dragonmaw Port'),
(0, 1066, 'Stormwind Gunship Pandaria Start'),
(1, 719, 'Mount Hyjal default terrain'),
(1, 1074, 'Orgrimmar Gunship Pandaria Start'),
(870, 971, 'Jade Forest Alliance Starting Terrain'),
(870, 1076, 'Jade Forest Horde Starting Terrain'),
(870, 972, 'Jade Forest Battlefield Phase'),
(870, 1061, 'Krasarang Wilds - Domination Point'),
(870, 1062, 'Krasarang Wilds - Lion\'s Landing'),
(860, 975, 'Wandering Island - Turtle Hurted'),
(860, 976, 'Wandering Island - Turtle Healed'),
(1064, 1120, 'Thunder King Horde Hub'),
(1064, 1121, 'Thunder Island Alliance Hub'),
(654, 638, 'Gilneas default terrain'),
(654, 655, 'Gilneas - Duskmist Shore broken'),
(654, 656, 'Gilneas - terrain3'),
(648, 661, 'Lost Isles - terrain1'),
(648, 659, 'Lost Isles - terrain2');

DELETE FROM `terrain_worldmap`;
INSERT INTO `terrain_worldmap` (`TerrainSwapMap`, `WorldMapArea`, `Comment`) VALUES 
(638, 545, 'Gilneas'),
(655, 678, 'Gilneas_terrain1'),
(656, 679, 'Gilneas_terrain2'),
(719, 683, 'Hyjal_terrain1'),
(1061, 910, 'KrasarangHorde'),
(1062, 910, 'KrasarangAlliance');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=719;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=975;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=976;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=1120;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=1121;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=1061;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=1062;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=638;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=655;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=656;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=661;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=659;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(29, 0, 719, 0, 0, 8, 0, 25372, 0, 0, 1, 0, 0, '', ''),
(29, 0, 975, 0, 0, 8, 0, 29799, 0, 0, 1, 0, 0, '', ''),
(29, 0, 975, 0, 0, 8, 0, 30767, 0, 0, 0, 0, 0, '', ''),
(29, 0, 975, 0, 1, 28, 0, 30767, 0, 0, 0, 0, 0, '', ''),
(29, 0, 976, 0, 0, 8, 0, 29799, 0, 0, 0, 0, 0, '', ''),
(29, 0, 1120, 0, 0, 8, 0, 32212, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1120, 0, 0, 8, 0, 32644, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1120, 0, 0, 28, 0, 32212, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1120, 0, 0, 28, 0, 32644, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1121, 0, 0, 8, 0, 32212, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1121, 0, 0, 8, 0, 32644, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1121, 0, 0, 28, 0, 32212, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1121, 0, 0, 28, 0, 32644, 0, 0, 1, 0, 0, '', ''),
(29, 0, 1061, 0, 0, 8, 0, 32108, 0, 0, 0, 0, 0, '', ''),
(29, 0, 1061, 0, 1, 8, 0, 32109, 0, 0, 0, 0, 0, '', ''),
(29, 0, 1062, 0, 0, 8, 0, 32108, 0, 0, 0, 0, 0, '', ''),
(29, 0, 1062, 0, 1, 8, 0, 32109, 0, 0, 0, 0, 0, '', ''),
(29, 0, 638, 0, 0, 8, 0, 14386, 0, 0, 1, 0, 0, '', ''),
(29, 0, 655, 0, 0, 8, 0, 14386, 0, 0, 0, 0, 0, '', ''),
(29, 0, 655, 0, 0, 8, 0, 14466, 0, 0, 1, 0, 0, '', ''),
(29, 0, 656, 0, 0, 8, 0, 14466, 0, 0, 0, 0, 0, '', ''),
(29, 0, 661, 0, 0, 28, 0, 14245, 0, 0, 0, 0, 0, '', ''),
(29, 0, 661, 0, 1, 8, 0, 14245, 0, 0, 0, 0, 0, '', ''),
(29, 0, 661, 0, 1, 8, 0, 24958, 0, 0, 1, 0, 0, '', ''),
(29, 0, 659, 0, 0, 8, 0, 24958, 0, 0, 0, 0, 0, '', '');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=616 AND `SourceEntry`=1;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=1519 AND `SourceEntry`=6;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=1637 AND `SourceEntry`=6;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=5736 AND `SourceEntry` IN (1,2);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=6507 AND `SourceEntry` IN (1,2);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=5785 AND `SourceEntry` IN (5,6);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=5805 AND `SourceEntry` IN (1,2);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=6134 AND `SourceEntry` IN (1,2);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=25 AND `SourceGroup`=6138 AND `SourceEntry` IN (1,2);
DELETE FROM `phase_definitions` WHERE `zoneId`=12 AND `entry`=1;
DELETE FROM `phase_definitions` WHERE `zoneId`=14 AND `entry`=1;
DELETE FROM `phase_definitions` WHERE `zoneId`=16 AND `entry`=1;
DELETE FROM `phase_definitions` WHERE `zoneId`=616 AND `entry`=1;
DELETE FROM `phase_definitions` WHERE `zoneId`=1519 AND `entry`=7;
DELETE FROM `phase_definitions` WHERE `zoneId`=1637 AND `entry`=7;
DELETE FROM `phase_definitions` WHERE `zoneId`=5736 AND `entry` IN (1,2);
DELETE FROM `phase_definitions` WHERE `zoneId`=5785 AND `entry` IN (2,3);
DELETE FROM `phase_definitions` WHERE `zoneId`=6507 AND `entry` IN (1,2);
DELETE FROM `phase_definitions` WHERE `zoneId`=5785 AND `entry` IN (5,6);
DELETE FROM `phase_definitions` WHERE `zoneId`=5805 AND `entry` IN (1,2);
DELETE FROM `phase_definitions` WHERE `zoneId`=6134 AND `entry` IN (1,2);
DELETE FROM `phase_definitions` WHERE `zoneId`=6138 AND `entry` IN (1,2);

UPDATE `phase_definitions` SET `terrainswapmap` = 0;
