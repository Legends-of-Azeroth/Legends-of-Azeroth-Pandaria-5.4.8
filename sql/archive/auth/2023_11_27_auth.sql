-- Change account table struct
ALTER TABLE `account` 
ADD COLUMN `lock_country` varchar(2) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '00' AFTER `locked`;
ALTER TABLE `account` 
ADD COLUMN `totp_secret` varbinary(128) NULL DEFAULT NULL AFTER `token_key`;