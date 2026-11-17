-- Add PVP S12 - S15

-- Delte old events
DELETE FROM `game_event` WHERE `eventEntry` = 124;
DELETE FROM `game_event` WHERE `eventEntry` = 125;
DELETE FROM `game_event` WHERE `eventEntry` = 126;
DELETE FROM `game_event` WHERE `eventEntry` = 127;
DELETE FROM `game_event` WHERE `eventEntry` = 128;
DELETE FROM `game_event` WHERE `eventEntry` = 129;
DELETE FROM `game_event` WHERE `eventEntry` = 130;
DELETE FROM `game_event` WHERE `eventEntry` = 131;
DELETE FROM `game_event` WHERE `eventEntry` = 132;
DELETE FROM `game_event` WHERE `eventEntry` = 133;
DELETE FROM `game_event` WHERE `eventEntry` = 134;

-- Add new events
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (103, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 1', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (104, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 2', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (105, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 3', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (106, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 4', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (107, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 5', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (108, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 6', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (109, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 7', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (110, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 8', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (111, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 9', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (112, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 10', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (113, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 11', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (114, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 12', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (115, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 13', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (116, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 14', 0, '', 2, 0);
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`, `world_event`, `ScriptName`, `announce`, `world_state`) VALUES (117, '0000-00-00 00:00:00', '0000-00-00 00:00:00', 5184000, 2592000, 0, 'Arena Season 15', 0, '', 2, 0);

-- Add new game_event_arena_seasons and delete old 
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 124;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 125;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 126;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 127;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 128;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 129;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 130;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 131;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 132;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 133;
DELETE FROM `game_event_arena_seasons` WHERE `eventEntry` = 134;
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (103, 1);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (104, 2);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (105, 3);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (106, 4);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (107, 5);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (108, 6);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (109, 7);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (110, 8);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (111, 9);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (112, 10);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (113, 11);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (114, 12);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (115, 13);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (116, 14);
INSERT INTO `game_event_arena_seasons` (`eventEntry`, `season`) VALUES (117, 15);

-- Spawn Vendors for S15

INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES (582671, 73146, 870, 0, 0, 1, 1, 0, 0, 2136.37, 2618.08, 543.117, 0.524664, 300, 0, 0, 0, 2757587, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES (582666, 73141, 870, 0, 0, 1, 1, 0, 0, 2138.91, 2625.85, 543.116, 5.27552, 300, 0, 0, 0, 2757587, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES (582667, 73144, 870, 0, 0, 1, 1, 0, 0, 194.663, 2200.33, 272.414, 0.491716, 300, 0, 0, 0, 2757587, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES (582668, 73147, 870, 0, 0, 1, 1, 0, 0, 202.531, 2209.23, 272.373, 4.41634, 300, 0, 0, 0, 2852724, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES (582669, 73143, 870, 0, 0, 1, 1, 0, 0, 203.379, 2196.93, 272.373, 1.98056, 300, 0, 0, 0, 2757587, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES (582670, 73145, 870, 0, 0, 1, 1, 0, 0, 2147.97, 2619.14, 543.092, 2.76306, 300, 0, 0, 0, 2757587, 0, 0, 0, 0, 0, 0, 0);

-- Link Spawn to Event
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES (117, 582666);
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES (117, 582667);
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES (117, 582668);
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES (117, 582669);
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES (117, 582670);
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES (117, 582671);
