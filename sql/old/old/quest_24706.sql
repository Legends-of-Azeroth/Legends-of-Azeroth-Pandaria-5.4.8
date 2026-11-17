/* https://www.wowhead.com/quest=24706/the-spirits-of-golakka-hot-springs */
DELETE FROM `smart_scripts` WHERE `entryorguid` = 38237;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
("38237", "0", "0", "0", "19", "0", "100", "0", "24706", "0", "0", "0", "85", "71458", "0", "0", "0", "0", "0", "7", "0", "0", "0", "0", "0", "0", "0", "on quest accept - summon maxximilian"),
("38237", "0", "1", "0", "19", "0", "100", "0", "24705", "0", "0", "0", "85", "71458", "0", "0", "0", "0", "0", "7", "0", "0", "0", "0", "0", "0", "0", "on quest accept - summon maxximilian");

UPDATE `quest_objective` SET `objectId` = 38254 WHERE `questid` = 24706;

DELETE FROM `creature_template_addon` WHERE `entry` = 38255;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `ai_anim_kit`, `movement_anim_kit`, `melee_anim_kit`, `auras`) VALUES
("38255", "0", "2410", "0", "0", "0", "0", "0", "0", '');