-- command table
DELETE FROM `command` WHERE `name` IN ('guard delete','guard global','guard handle','guard player');
DELETE FROM `command` WHERE `name` IN ('reload achievement_reward_locale');
INSERT INTO `command` (`name`, `security`, `help`) VALUES 
('reload achievement_reward_locale', 5, 'Syntax: .reload achievement_reward_locale\nReload achievement_reward_locale table.');

-- creature_template table
UPDATE `creature_template` SET `flags_extra`=0 WHERE `entry`=37921;
UPDATE `creature_template` SET `unit_class`=1 WHERE `entry`=59637;

-- creature_addon table
DELETE FROM `creature_addon` WHERE `guid` IN (77232,136675);

-- conditions table
DELETE FROM `conditions` WHERE `SourceGroup`=190001;
