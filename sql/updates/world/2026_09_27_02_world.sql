-- Korean creature names from TrinityCore TDB 1210.26091 (2026-09-09).
-- Copy only entries whose current English name matches the source DB exactly.
-- Preserve existing Korean fields and all other locales.
DROP TEMPORARY TABLE IF EXISTS `_koKR_stage_creature_template_locale`;
CREATE TEMPORARY TABLE `_koKR_stage_creature_template_locale` (
    `entry` INT UNSIGNED NOT NULL,
    `SourceName` VARCHAR(100) NOT NULL,
    `Name` TEXT NULL,
    `FemaleName` TEXT NULL,
    `Title` TEXT NULL,
    PRIMARY KEY (`entry`)
) ENGINE=InnoDB;

INSERT INTO `_koKR_stage_creature_template_locale` (`entry`, `SourceName`, `Name`, `FemaleName`, `Title`) VALUES
(33651, 'VX-001', NULL, NULL, '대인 공격포'),
(42488, 'Chief Engineer Yoon', '선임기술자 윤', NULL, '기술용품 상인'),
(56128, 'Hozen Chieftain', '호젠 족장', NULL, NULL),
(68976, 'Ishi', '이시', NULL, NULL);

UPDATE `creature_template_locale` AS `l`
INNER JOIN `_koKR_stage_creature_template_locale` AS `s` ON `l`.`entry` = `s`.`entry` AND `l`.`locale` = 'koKR'
INNER JOIN `creature_template` AS `p` ON `p`.`entry` = `s`.`entry` AND BINARY `p`.`name` = BINARY `s`.`SourceName`
SET `l`.`Name` = IF(`s`.`Name` IS NOT NULL AND (`l`.`Name` IS NULL OR `l`.`Name` NOT REGEXP '[가-힣ㄱ-ㅎㅏ-ㅣ]'), `s`.`Name`, `l`.`Name`),
    `l`.`FemaleName` = IF(`s`.`FemaleName` IS NOT NULL AND (`l`.`FemaleName` IS NULL OR `l`.`FemaleName` NOT REGEXP '[가-힣ㄱ-ㅎㅏ-ㅣ]'), `s`.`FemaleName`, `l`.`FemaleName`),
    `l`.`Title` = IF(`s`.`Title` IS NOT NULL AND (`l`.`Title` IS NULL OR `l`.`Title` NOT REGEXP '[가-힣ㄱ-ㅎㅏ-ㅣ]'), `s`.`Title`, `l`.`Title`);

INSERT INTO `creature_template_locale` (`entry`, `locale`, `Name`, `FemaleName`, `Title`)
SELECT `s`.`entry`, 'koKR', `s`.`Name`, `s`.`FemaleName`, `s`.`Title`
FROM `_koKR_stage_creature_template_locale` AS `s`
INNER JOIN `creature_template` AS `p` ON `p`.`entry` = `s`.`entry` AND BINARY `p`.`name` = BINARY `s`.`SourceName`
LEFT JOIN `creature_template_locale` AS `l` ON `l`.`entry` = `s`.`entry` AND `l`.`locale` = 'koKR'
WHERE `l`.`entry` IS NULL;

DROP TEMPORARY TABLE `_koKR_stage_creature_template_locale`;
