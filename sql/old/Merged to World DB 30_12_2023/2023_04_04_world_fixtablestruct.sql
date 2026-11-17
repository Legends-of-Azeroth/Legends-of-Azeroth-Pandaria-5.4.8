-- fix timestamp default
ALTER TABLE `hotfix_data` 
MODIFY COLUMN `hotfixDate` datetime NOT NULL AFTER `type`,
ENGINE = InnoDB, CHARACTER SET = utf8mb4, COLLATE = utf8mb4_bin;