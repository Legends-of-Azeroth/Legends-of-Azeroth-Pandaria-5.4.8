DELETE FROM `creature_template` WHERE `entry` = 3871204;

DELETE FROM `disables` WHERE `entry` IN (35170, 35167, 35456, 35188) AND `sourceType` = 10;
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES
(10, 35170, 1, "", "", "Worgen Alpha - MMAP"),
(10, 35167, 1, "", "", "Worgen Alpha - MMAP"),
(10, 35456, 1, "", "", "Worgen Runt - MMAP"),
(10, 35188, 1, "", "", "Worgen Runt - MMAP");

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "", `mindmg` = 28, `maxdmg` = 46, `attackpower` = 12, `dmg_multiplier` = 1 WHERE `entry` = 35077;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "" WHERE `entry` = 35081;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (35077, 3871204, 35170, 35188, 35456, 35167) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (3507700, 3507701, 3507702, 1415400, 1415402) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(35077, 0, 0, 0, 19, 0, 100, 0, 14154, 0, 0, 0, 11, 66914, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Lord Darius Crowley - On Quest (14154) Accept - Cast Spell 'Gilneas - Quest - Gilneas Prison Periodic Forcecast'"),
(35077, 0, 1, 0, 37, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Lord Darius Crowley - On AI Init - Disable Combat Movement"),
(35077, 0, 2, 0, 0, 0, 100, 0, 6000, 6000, 15000, 17000, 207, 67825, 61044, 67827, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Lord Darius Crowley - In Combat - Cast Random Spell (Left Hook, Demoralizing Shout, Snap Kick)");

DELETE FROM `waypoints` WHERE `entry` IN (3517000, 3516700);

DELETE FROM `spell_script_names` WHERE `spell_id` = 66853;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(66853, "spell_gen_gilneas_prison_periodic_dummy");

DELETE FROM `spell_target_position` WHERE `id` IN (66925, 66836);
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`) VALUES
(66925, 0, 654, -1636.292, 1481.836, 70.94804),
(66836, 0, 654, -1620.981, 1509.193, 67.10413);

UPDATE `creature_template` SET `AIName` = "", `ScriptName`= "npc_worgen_runt", `flags_extra` = 64, `MovementType` = 0 WHERE `entry` IN (35456, 35188, 35167, 35170);
