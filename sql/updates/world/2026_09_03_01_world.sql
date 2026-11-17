
DELETE FROM `phase_area` WHERE (`AreaId`, `PhaseId`) IN
((108, 226), (4714, 105), (4861, 194), (4979, 195), (4989, 194), (4990, 194),
(5013, 194), (5014, 194), (5016, 194), (5018, 194), (5019, 194), (5020, 194),
(5022, 194), (5140, 169), (5140, 361), (5424, 169), (5424, 361), (5602, 312),
(5602, 313), (5602, 315), (5602, 324), (5602, 325), (5700, 313), (5700, 315),
(5700, 324));

INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES
-- Sentinel Hill (Westfall)
(108,   226, 'Sentinel Hill - Rise of the Brotherhood'),
-- Gilneas
(4714,  105, 'Gilneas - Phase 105'),
-- Mount Hyjal - Phase 194 (The Regrowth / Regrowth phase)
(4861,  194, 'Mount Hyjal - The Regrowth - Phase 194'),
(4989,  194, 'Mount Hyjal - Grove of Aessina - Phase 194'),
(4990,  194, 'Mount Hyjal - Shrine of Aviana - Phase 194'),
(5013,  194, 'Mount Hyjal - Blackhorn''s Penance - Phase 194'),
(5014,  194, 'Mount Hyjal - The Inferno - Phase 194'),
(5016,  194, 'Mount Hyjal - Whistling Grove - Phase 194'),
(5018,  194, 'Mount Hyjal - Ashen Lake - Phase 194'),
(5019,  194, 'Mount Hyjal - Sanctuary of Malorne - Phase 194'),
(5020,  194, 'Mount Hyjal - The Flamewake - Phase 194'),
(5022,  194, 'Mount Hyjal - Rim of the World - Phase 194'),
-- Mount Hyjal - Phase 195 (Shrine of Goldrinn)
(4979,  195, 'Mount Hyjal - Shrine of Goldrinn - Phase 195'),
-- Twilight Highlands - Highbank (Quest 28598)
(5140,  169, 'Highbank phase after quest 28598 complete'),
(5140,  361, 'Highbank phase before quest 28598 complete'),
-- Twilight Highlands - Obsidian Breakers (Quest 28598)
(5424,  169, 'Obsidian Breakers phase after quest 28598 complete'),
(5424,  361, 'Obsidian Breakers phase before quest 28598 complete'),
-- Uldum - Schnottz''s Landing (Quests 27950, 27990, 28002)
(5602,  312, 'Schnottz''s Landing phase while quest 27950 taken'),
(5602,  313, 'Schnottz''s Landing phase up to quest 27950 rewarded'),
(5602,  315, 'Schnottz''s Landing phase after quest 27950 rewarded'),
(5602,  324, 'Schnottz''s Landing phase after quest 28002 rewarded'),
(5602,  325, 'Schnottz''s Landing phase after quest 27990 Completed'),
-- Uldum - Schnottz''s Hostel (Quests 27950, 28002)
(5700,  313, 'Schnottz''s Hostel phase up to quest 27950 rewarded'),
(5700,  315, 'Schnottz''s Hostel phase after quest 27950 rewarded'),
(5700,  324, 'Schnottz''s Hostel phase after quest 28002 rewarded');


-- 2a. High Chief Bristlelimb (17702) - Add phase actions to existing SmartAI
--     Phase 170 on reset, Phase 169 on Data Set 1 1
--     Events 0-6 exist; we append starting at id=7
DELETE FROM `smart_scripts` WHERE `entryorguid` = 17702 AND `source_type` = 0 AND `id` IN (7, 8);
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Reset: Set Phase 170 (bitmask = visible in phase 170)
(17702, 0, 7, 0, 25, 0, 100, 512, 0, 0, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'High Chief Bristlelimb - On Reset - Set Phasemask 2'),
-- On Data Set 1 1: Set Phase 169 (bitmask = visible in phase 169)
(17702, 0, 8, 0, 38, 0, 100, 512, 1, 1, 0, 0, 0,
 44, 169, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'High Chief Bristlelimb - On Data Set - Set Phasemask 1');

-- 2b. Halfdan the Ice-Hearted (23671) - Add phase actions to existing SmartAI
--     Phase 371 on just summoned, Phase 169 on Val'kyr Observer, Phase 170 OOC
--     Events 0-13 exist; append at id=14
DELETE FROM `smart_scripts` WHERE `entryorguid` = 23671 AND `source_type` = 0 AND `id` IN (14, 15, 16, 17);
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Just Summoned: Set Phase 371 (self)
(23671, 0, 14, 0, 54, 0, 100, 512, 0, 0, 0, 0, 0,
 44, 371, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Halfdan the Ice-Hearted - On Just Summoned - Set Phasemask 3'),
-- On Just Summoned: Change Phasemask of Val''kyr Observer to 169 (nearby, range 90)
(23671, 0, 15, 0, 54, 0, 100, 512, 0, 0, 0, 0, 0,
 44, 169, 0, 0, 0, 0, 0, 9, 24118, 0, 90, 0, 0, 0, 0, 0,
 'Halfdan the Ice-Hearted - On Just Summoned - Change Phasemask of Val''kyr Observer'),
-- OOC: Change Phasemask of Val''kyr Observer to 170 (nearby, range 90, repeatable)
(23671, 0, 16, 0, 1, 0, 100, 1, 30000, 30000, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 9, 24118, 0, 90, 0, 0, 0, 0, 0,
 'Halfdan the Ice-Hearted - Ooc - Change Phasemask of Val''kyr Observer'),
-- On Death: Set Data 1 1 to Val''kyr Observer (nearby, range 90)
(23671, 0, 17, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0,
 45, 1, 1, 0, 0, 0, 0, 9, 24118, 0, 90, 0, 0, 0, 0, 0,
 'Halfdan the Ice-Hearted - On Death - Set Data 1 1 to Val''kyr Observer');

-- 2c. Raelorasz (26117) - Add phase action on quest rewarded
--     Quest 11967 "Mustering the Reds" -> Set Phase 169 on Keristrasza (27588)
--     Events 0-1 exist; append at id=2
DELETE FROM `smart_scripts` WHERE `entryorguid` = 26117 AND `source_type` = 0 AND `id` = 2;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Quest 11967 Rewarded: Set Phase 169 on Keristrasza (creature 27588, respawn range 20)
(26117, 0, 2, 0, 20, 0, 100, 0, 11967, 0, 0, 0, 0,
 44, 169, 0, 0, 0, 0, 0, 10, 27588, 26206, 0, 0, 0, 0, 0, 0,
 'Raelorasz - On Quest Mustering the Reds Rewarded - Set Phase 169 on Keristrasza');

-- 2d. Brann's Flying Machine (30134) - Create SmartAI from scratch
--     Full SmartAI script for Brann's Flying Machine escort in Ulduar
DELETE FROM `smart_scripts` WHERE `entryorguid` = 30134 AND `source_type` = 0;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Passenger Boarded: Run action list 3013400
(30134, 0, 0, 0, 27, 0, 100, 512, 0, 0, 0, 0, 0,
 80, 3013400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Machine - On Passenger Boarded - Run Script'),
-- On Waypoint 2: Talk to Brann (30107)
(30134, 0, 1, 0, 40, 0, 100, 512, 2, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 4: Talk
(30134, 0, 2, 0, 40, 0, 100, 512, 4, 0, 0, 0, 0,
 1, 1, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 7: Talk
(30134, 0, 3, 0, 40, 0, 100, 512, 7, 0, 0, 0, 0,
 1, 2, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 22: Talk
(30134, 0, 4, 0, 40, 0, 100, 512, 22, 0, 0, 0, 0,
 1, 3, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 27: Talk
(30134, 0, 5, 0, 40, 0, 100, 512, 27, 0, 0, 0, 0,
 1, 4, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 31: Talk
(30134, 0, 6, 0, 40, 0, 100, 512, 31, 0, 0, 0, 0,
 1, 5, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 35: Talk
(30134, 0, 7, 0, 40, 0, 100, 512, 35, 0, 0, 0, 0,
 1, 6, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 36: Talk
(30134, 0, 8, 0, 40, 0, 100, 512, 36, 0, 0, 0, 0,
 1, 7, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 41: Talk
(30134, 0, 9, 0, 40, 0, 100, 512, 41, 0, 0, 0, 0,
 1, 8, 0, 0, 0, 0, 0, 19, 30107, 10, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Talk'),
-- On Waypoint 42: Eject Passengers
(30134, 0, 10, 11, 40, 0, 100, 512, 42, 0, 0, 0, 0,
 11, 68576, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Eject Passengers'),
-- On Waypoint 42: Invoker Cast to Spawn Brann
(30134, 0, 11, 12, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 85, 56675, 0, 0, 0, 0, 0, 21, 20, 0, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Invoker Cast to Spawn Brann'),
-- On Waypoint 42: Despawn
(30134, 0, 12, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Machine - On Waypoint - Despawn'),
-- OOC: Cross Cast (repeatable)
(30134, 0, 13, 0, 1, 0, 100, 512, 3000, 3000, 3000, 3000, 0,
 86, 56622, 1, 9, 30136, 5, 50, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Machine - OOC - Cross Cast'),
-- OOC: Cross Cast (repeatable)
(30134, 0, 14, 0, 1, 0, 100, 512, 3000, 3000, 3000, 3000, 0,
 86, 55089, 1, 19, 30136, 40, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Machine - OOC - Cross Cast'),
-- Just Created: Set Phase 371 (self)
(30134, 0, 16, 0, 63, 0, 100, 512, 0, 0, 0, 0, 0,
 44, 371, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Machine - Just Created - Set Phase 371');

-- 2e. Crusader Olakin Sainrith (31428) - Create SmartAI from scratch
DELETE FROM `smart_scripts` WHERE `entryorguid` = 31428 AND `source_type` = 0;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Data Set 1 1: Set Faction 1770 (hostile)
(31428, 0, 0, 1, 38, 0, 100, 512, 1, 1, 0, 0, 0,
 2, 1770, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Data Set 1 1 - Set Faction'),
-- On Data Set 1 1: Set Hostile
(31428, 0, 1, 2, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Data Set 1 1 - Set Hostile'),
-- On Data Set 1 1: Attack Morbidus (30698)
(31428, 0, 2, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 49, 0, 0, 0, 0, 0, 0, 19, 30698, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Data Set 1 1 - Attack Morbidus'),
-- On Evade: Set Faction 2070 (friendly)
(31428, 0, 3, 4, 7, 0, 100, 512, 1, 1, 0, 0, 0,
 2, 2070, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Evade - Set Faction'),
-- On Evade: Set Passive
(31428, 0, 4, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Evade - Set Passive'),
-- On Just Summoned: Equip Items (40598, 42543)
(31428, 0, 5, 6, 54, 0, 100, 512, 0, 0, 0, 0, 0,
 71, 0, 0, 40598, 42543, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Just Summoned - Equip Items'),
-- On Just Summoned: Set Ingame Phase 175 (self)
(31428, 0, 6, 7, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 44, 175, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Just Summoned - Set Ingame Phase 175'),
-- On Just Summoned: Dismount
(31428, 0, 7, 8, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 43, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Just Summoned - Dismount'),
-- On Just Summoned: Say Line 1
(31428, 0, 8, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Just Summoned - Say Line 1'),
-- On Data Set 2 2: Cast Ride Ghostwing (59091) on Ghostwing (31432)
(31428, 0, 9, 0, 38, 0, 100, 512, 2, 2, 0, 0, 0,
 11, 59091, 2, 0, 0, 0, 0, 19, 31432, 0, 0, 0, 0, 0, 0, 0,
 'Crusader Olakin Sainrith - On Data Set - Cast Ride Ghostwing');

-- 2f. Ghostwing (31432) - Create SmartAI from scratch
DELETE FROM `smart_scripts` WHERE `entryorguid` = 31432 AND `source_type` = 0;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Just Summoned: Set Ingame Phase 175 (self)
(31432, 0, 0, 1, 54, 0, 100, 512, 0, 0, 0, 0, 0,
 44, 175, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Just Summoned - Set Ingame Phase 175'),
-- On Just Summoned: Start Waypoints
(31432, 0, 1, 2, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Just Summoned - Start WP'),
-- On Just Summoned: Set Active
(31432, 0, 2, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 53, 1, 31432, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Just Summoned - Set Active'),
-- On Reached WP 31432: Pause Waypoints 3000ms
(31432, 0, 3, 4, 40, 0, 100, 512, 1, 31432, 0, 0, 0,
 54, 3000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Reached WP1 - Pause WP'),
-- On Reached WP1: Set Data 2 2 on Crusader Olakin Sainrith (31428)
(31432, 0, 4, 5, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 45, 2, 2, 0, 0, 0, 0, 19, 31428, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Reached WP1 - Set Data on Crusader Olakin Sainrith'),
-- On Reached WP1: Say Line 0
(31432, 0, 5, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Reached WP1 - Say Line 0'),
-- On Reached WP 31432 (second): Say Line 0 on Darkrider Arly (30631)
(31432, 0, 6, 7, 40, 0, 100, 512, 2, 31432, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 19, 30631, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Reached WP2 - Say Line 0 on Darkrider Arly'),
-- On Reached WP2: Despawn Crusader Olakin Sainrith (31428)
(31432, 0, 7, 8, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 41, 0, 0, 0, 0, 0, 0, 19, 31432, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Reached WP2 - Despawn Crusader Olakin Sainrith'),
-- On Reached WP2: Despawn Self
(31432, 0, 8, 0, 61, 0, 100, 512, 0, 0, 0, 0, 0,
 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Ghostwing - On Reached WP2 - Despawn');

-- ============================================================
-- 3. SmartAI Phase Actions for GameObjects
-- ============================================================

-- 3a. Banner of Provocation (181058) - GO SmartAI with phase action
--     GO (source_type=1) runs random TAL 18105800/18105801, summons 16059,
--     and sets phase 170 on GO 181059 (range 90)
DELETE FROM `smart_scripts` WHERE `entryorguid` = 181058 AND `source_type` = 1;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Counter Set: Run Random Action List (18105800, 18105801)
(181058, 1, 0, 0, 63, 0, 100, 0, 0, 0, 0, 0, 0,
 88, 18105800, 18105801, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Banner of Provocation - Action list - Random Action list'),
-- On Counter Set: Summon unit 16059 (6 sec duration)
(181058, 1, 1, 2, 77, 0, 100, 1, 1, 4, 0, 0, 0,
 12, 16059, 6, 120000, 0, 0, 0, 8, 0, 0, 0, 0, 606.113, -199.239, -53.4413, 0,
 'Banner of Provocation - On counter set - Summon unit'),
-- On Counter Set: Set phase 170 on GO 181059 (range 90)
(181058, 1, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 20, 181059, 90, 0, 0, 0, 0, 0, 0,
 'Banner of Provocation - On counter set - Set to phase 2');

-- Timed Action List 18105800 (source_type=9) - Summon adds
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18105800 AND `source_type` = 9;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
(18105800, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16053, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 581.312, -198.222, -53.5695, 0.534245,
 'Banner of Provocation - Action list - Summon unit'),
(18105800, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16050, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 595.331, -207.563, -53.4271, 1.51207,
 'Banner of Provocation - Action list - Summon unit'),
(18105800, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16052, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 609.828, -206.809, -53.3796, 2.17573,
 'Banner of Provocation - Action list - Summon unit'),
(18105800, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16055, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 618.483, -196.314, -53.2406, 2.92657,
 'Banner of Provocation - Action list - Summon unit');

-- Timed Action List 18105801 (source_type=9) - Summon adds
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18105801 AND `source_type` = 9;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
(18105801, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16051, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 581.312, -198.222, -53.5695, 0.534245,
 'Banner of Provocation - Action list - Summon unit'),
(18105801, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16049, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 595.331, -207.563, -53.4271, 1.51207,
 'Banner of Provocation - Action list - Summon unit'),
(18105801, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16054, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 609.828, -206.809, -53.3796, 2.17573,
 'Banner of Provocation - Action list - Summon unit'),
(18105801, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 12, 16058, 6, 240000, 0, 0, 0, 8, 0, 0, 0, 0, 618.483, -196.314, -53.2406, 2.92657,
 'Banner of Provocation - Action list - Summon unit');

-- 3b. Razorthorn Dirt Mound (187073) - Add SmartAI with phase action
DELETE FROM `smart_scripts` WHERE `entryorguid` = 187073 AND `source_type` = 1;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Data Set 1 1: Run Script (action list 18707300)
(187073, 1, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 0,
 80, 18707300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Razorthorn Dirt Mound - On Data Set 1 1 - Run Script');

-- Create Timed Action List 18707300 (source_type=9)
DELETE FROM `smart_scripts` WHERE `entryorguid` = 18707300 AND `source_type` = 9;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Script: Set Phase 170 (self, after 10ms)
(18707300, 9, 0, 0, 0, 0, 100, 0, 10, 10, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'On Script - Set Phase 170'),
-- On Script: Set Phase 169 (self, after 120s)
(18707300, 9, 1, 0, 0, 0, 100, 0, 120000, 120000, 0, 0, 0,
 44, 169, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'On Script - Set Phase 169');

-- 3c. Nerubian Crater (190555) - Add SmartAI with phase action
DELETE FROM `smart_scripts` WHERE `entryorguid` = 190555 AND `source_type` = 1;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Data Set 1 1: Run Script (action list 19055500)
(190555, 1, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 0,
 80, 19055500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Nerubian Crater - On Data Set 1 1 - Run Script');

-- Create Timed Action List 19055500 (source_type=9)
DELETE FROM `smart_scripts` WHERE `entryorguid` = 19055500 AND `source_type` = 9;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Script: Set Phase 170 (self)
(19055500, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'On Script - Set Phase 170'),
-- On Script: Set Phase 169 (self, after 180s)
(19055500, 9, 1, 0, 0, 0, 100, 0, 180000, 180000, 0, 0, 0,
 44, 169, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'On Script - Set Phase 169');

-- 3d. Val'kyr Observer (24118) - Instance SAI + Timed Action List
--     In 4.3.4 the SAI is attached to guid -42888 (second observer spawn).
--     In 5.4.8 the second observer spawn is guid 127835.
--     Triggered by Halfdan (23671) on death: Set Data 1 1 on 24118 -> Run TAL 2411800
DELETE FROM `smart_scripts` WHERE `entryorguid` = -127835 AND `source_type` = 0;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- On Data 1 1 Set: Call Action List 2411800
(-127835, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 0,
 80, 2411800, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Data 1 1 Set - Call Actionlist');

-- Timed Action List 2411800 (source_type=9)
DELETE FROM `smart_scripts` WHERE `entryorguid` = 2411800 AND `source_type` = 9;
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`,
 `comment`)
VALUES
-- Talk 0
(2411800, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Talk 0'),
-- Talk 1 (after 10s)
(2411800, 9, 1, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 0,
 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Talk 1'),
-- Cast 'Stop the Ascension!: Halfdan\'s Soul Destruction' (43092) on 24119 (after 3s)
(2411800, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0,
 11, 43092, 2, 0, 0, 0, 0, 19, 24119, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Cast Halfdan\'s Soul Destruction'),
-- Despawn Halfdan\'s Soul (24119) (after 10s)
(2411800, 9, 3, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 0,
 41, 0, 0, 0, 0, 0, 0, 19, 24119, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Despawn Halfdan\'s Soul'),
-- Despawn Halfdan the Ice-Hearted (23671)
(2411800, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 41, 0, 0, 0, 0, 0, 0, 19, 23671, 0, 1, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Despawn Halfdan the Ice-Hearted'),
-- Set Phase 170 to nearby Val'kyr Observer (range 90)
(2411800, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 9, 24118, 0, 90, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Set Phase 170 to nearby Val\'kyr Observer'),
-- Set Phase 170 to Self
(2411800, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 44, 170, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Set Phase 170 to Self'),
-- Interrupt Current Cast
(2411800, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0,
 92, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
 'Val\'kyr Observer - On Script - Interrupt Current Cast');
