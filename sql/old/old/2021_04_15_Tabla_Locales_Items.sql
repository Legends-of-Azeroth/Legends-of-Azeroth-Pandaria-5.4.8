DROP TABLE IF EXISTS `locales_item`;
CREATE TABLE IF NOT EXISTS `locales_item` (
  `ID` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `locale` VARCHAR(4) NOT NULL,
  `Name` TEXT,
  `Description` TEXT,
  `VerifiedBuild` SMALLINT(5) DEFAULT '0',
  PRIMARY KEY (`ID`, `locale`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8;

/*
idiomas:
-- koKR
-- frFR
-- deDE
-- zhCN
-- zhTW
-- esES		, español españa
-- esMX		. español latinoamerica
-- ruRU
*/
