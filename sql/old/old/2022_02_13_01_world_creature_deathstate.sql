/* fix Creatures Pandaria #18 */

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 60223;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 60223;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 59156;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 59156;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 59157;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 59157;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 65816;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 65816;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 58460;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 58460;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 58412;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 58412;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 60225;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 60225;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 59158;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 59158;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 65824;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 65824;

UPDATE `creature_template` SET `unit_flags`  = 32768  WHERE `entry` = 60225;
UPDATE `creature_template_addon` SET bytes1 = 0 WHERE entry = 60225;
