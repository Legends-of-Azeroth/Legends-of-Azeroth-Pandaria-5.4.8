DROP TABLE IF EXISTS `creature_sparring_template`;
CREATE TABLE `creature_sparring_template` (
  `creature_id` mediumint(8) unsigned NOT NULL,
  `health_limit_pct` float DEFAULT "0",
  `comment` varchar(255) DEFAULT "",
  PRIMARY KEY (`creature_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DELETE FROM `creature_sparring_template` WHERE `creature_id` IN (49422, 49423, 49428);
INSERT INTO `creature_sparring_template`(`creature_id`, `health_limit_pct`, `comment`) VALUES
(49422,85,"Rotbrain Berserker"),
(49423,85,"Rotbrain Magus"),
(49428,85,"Deathguard Protector");
