DROP TABLE IF EXISTS `rbac_account_groups`;

CREATE TABLE `rbac_account_groups` (
  `accountId` INT(10) UNSIGNED NOT NULL COMMENT 'Account id',
  `groupId` INT(10) UNSIGNED NOT NULL COMMENT 'Group id',
  `realmId` INT(11) NOT NULL DEFAULT '-1' COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`,`groupId`,`realmId`),
  KEY `fk__rbac_account_groups__rbac_groups` (`groupId`) USING BTREE
) ENGINE=MYISAM DEFAULT CHARSET=utf8 COMMENT='Account-Group relation';


DROP TABLE IF EXISTS `rbac_account_permissions`;

CREATE TABLE `rbac_account_permissions` (
  `accountId` INT(10) UNSIGNED NOT NULL COMMENT 'Account id',
  `permissionId` INT(10) UNSIGNED NOT NULL COMMENT 'Permission id',
  `granted` TINYINT(1) NOT NULL DEFAULT '1' COMMENT 'Granted = 1, Denied = 0',
  `realmId` INT(11) NOT NULL DEFAULT '-1' COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`,`permissionId`,`realmId`),
  KEY `fk__rbac_account_roles__rbac_permissions` (`permissionId`) USING BTREE
) ENGINE=MYISAM DEFAULT CHARSET=utf8 COMMENT='Account-Permission relation';