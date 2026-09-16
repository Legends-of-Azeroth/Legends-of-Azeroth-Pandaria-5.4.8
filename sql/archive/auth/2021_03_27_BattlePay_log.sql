
/*Table structure for table `battlepay_log` */

DROP TABLE IF EXISTS `battlepay_log`;

CREATE TABLE `battlepay_log` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `accountId` int(11) unsigned NOT NULL,
  `characterGuid` int(10) NOT NULL DEFAULT 0,
  `realm` int(5) unsigned NOT NULL,
  `item` int(10) NOT NULL DEFAULT 0,
  `price` int(5) unsigned NOT NULL,
  `date` timestamp NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
