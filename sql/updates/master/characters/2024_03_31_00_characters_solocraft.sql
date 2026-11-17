-- custom_solocraft_character_stats
DROP TABLE IF EXISTS `custom_solocraft_character_stats`;
CREATE TABLE `custom_solocraft_character_stats` (
  `guid` int unsigned NOT NULL,
  `Difficulty` float NOT NULL,
  `GroupSize` int NOT NULL,
  `SpellPower` int unsigned NOT NULL DEFAULT '0',
  `Stats` float NOT NULL DEFAULT '100',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;
