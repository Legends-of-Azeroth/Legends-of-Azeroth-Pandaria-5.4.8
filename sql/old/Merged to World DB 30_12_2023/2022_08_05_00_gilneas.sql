UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (221827, 221824, 221825, 221822, 221826, 221820, 221821, 221828, 221823);
DELETE FROM `creature` WHERE `guid` IN (371554, 371564, 371569, 371573, 371581, 371583, 371592, 371593, 371604, 371615, 371618, 371639) AND `id` = 36555;
DELETE FROM `creature_addon` WHERE `guid` IN (371554, 371564, 371569, 371573, 371581, 371583, 371592, 371593, 371604, 371615, 371618, 371639);
UPDATE `creature` SET `phaseMask` = 16384, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 219766 AND `id` = 36528;

DELETE FROM `creature_addon` WHERE `guid` = 219766;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(219766, 219766, 4097);

DELETE FROM `waypoint_data` WHERE `id` = 219766;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `entry`) VALUES
(219766, 1, -2191.43, 2210.01, 16.8541, 0, 0, 0, 36528),
(219766, 2, -2192.77, 2240.55, 10.3578, 0, 0, 0, 36528),
(219766, 3, -2191.43, 2210.01, 16.8541, 0, 0, 0, 36528),
(219766, 4, -2180.43, 2185.91, 17.1765, 0, 0, 0, 36528),
(219766, 5, -2143.8, 2179.52, 21.9216, 0, 0, 0, 36528),
(219766, 6, -2180.43, 2185.91, 17.1765, 0, 0, 0, 36528);

UPDATE `creature_template` SET `npcflag` = 1, `spell1` = 68903, `AIName` = "", `ScriptName` = "npc_mountain_horse" WHERE `entry` = 36540;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 36540 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 36540*100 AND `source_type` = 9;
UPDATE `creature_template` SET `speed_walk` = 1, `speed_run` = 1.71429, `AIName` = "", `ScriptName` = "npc_mountain_horse_summoned" WHERE `entry` = 36555;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 36540;
