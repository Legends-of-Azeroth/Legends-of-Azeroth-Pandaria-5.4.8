
/*Table structure for table `wow_token` */

DROP TABLE IF EXISTS `wow_token`;

CREATE TABLE `wow_token` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `accountId` int(11) unsigned NOT NULL,
  `characterGuid` int(10) NOT NULL DEFAULT 0,
  `realm` int(5) unsigned NOT NULL,
  `coins` int(5) unsigned NOT NULL,
  `date` timestamp NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
