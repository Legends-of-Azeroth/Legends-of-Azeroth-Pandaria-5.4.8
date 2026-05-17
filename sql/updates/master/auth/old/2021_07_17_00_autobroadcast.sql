
DROP TABLE IF EXISTS `autobroadcast`;
SET character_set_client = utf8mb4;

CREATE TABLE `autobroadcast` (
  `realmid` INT(11) NOT NULL DEFAULT '-1',
  `id` TINYINT(3) UNSIGNED NOT NULL AUTO_INCREMENT,
  `weight` TINYINT(3) UNSIGNED DEFAULT '1',
  `text` LONGTEXT NOT NULL,
  PRIMARY KEY (`id`,`realmid`)
) ENGINE=INNODB DEFAULT CHARSET=utf8;
