DROP TABLE IF EXISTS `locales_creature`;
CREATE TABLE IF NOT EXISTS `locales_creature` (
  `entry` MEDIUMINT(8) UNSIGNED NOT NULL DEFAULT '0',
  `locale` VARCHAR(4) NOT NULL,
  `Name` TEXT,
  `SubName` TEXT,
  `VerifiedBuild` SMALLINT(5) DEFAULT '0',
  PRIMARY KEY (`entry`,`locale`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8;
