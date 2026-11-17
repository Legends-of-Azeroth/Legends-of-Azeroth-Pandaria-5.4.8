-- ============================================================
-- Quest 14465 - Stout Mountain Horse (creature_template.entry = 38765).
--
-- Revert the horse's path logic from the C++ script
-- (StoutMountainHorseAI, previously removed) back to DB smart_scripts.
--
-- The DB script handles (uses guid-targeted gate opening, so no range limit):
--   * PASSENGER_BOARDED  -> WAYPOINT_START pathId 38765
--   * WP3  -> WP pause (1000 ms) + ACTIVATE_GOBJECT GO 196863 (guid 166783)
--   * WP13 -> WP pause (1000 ms) + ACTIVATE_GOBJECT GO 196864 (guid 166784)
--   * WP15 -> RemoveAurasDueToSpell 46598 (dismount passenger)
--
-- Every statement is DELETE-protected, so the file is idempotent and safe to
-- re-run on any environment.
-- ============================================================

-- 1) Waypoints (self-contained; replays the full path from the C++ migration).
DELETE FROM `waypoints` WHERE `entry` = 38765;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `point_comment`) VALUES
(38765, 1, -1870.36, 2282.73, 42.32, NULL, 0, 'Stout Mountain Horse'),
(38765, 2, -1852.05, 2302.51, 41.83, NULL, 0, 'Stout Mountain Horse'),
(38765, 3, -1829.29, 2323.74, 37.55, NULL, 0, 'Stout Mountain Horse'),
(38765, 4, -1818.32, 2332.80, 36.37, NULL, 0, 'Stout Mountain Horse'),
(38765, 5, -1801.58, 2344.71, 35.77, NULL, 0, 'Stout Mountain Horse'),
(38765, 6, -1792.48, 2352.44, 36.52, NULL, 0, 'Stout Mountain Horse'),
(38765, 7, -1787.74, 2359.60, 38.00, NULL, 0, 'Stout Mountain Horse'),
(38765, 8, -1781.81, 2383.18, 43.27, NULL, 0, 'Stout Mountain Horse'),
(38765, 9, -1777.55, 2423.56, 55.68, NULL, 0, 'Stout Mountain Horse'),
(38765, 10, -1770.01, 2447.47, 62.25, NULL, 0, 'Stout Mountain Horse'),
(38765, 11, -1754.47, 2464.13, 69.36, NULL, 0, 'Stout Mountain Horse'),
(38765, 12, -1710.82, 2466.15, 82.81, NULL, 0, 'Stout Mountain Horse'),
(38765, 13, -1691.46, 2493.66, 95.20, NULL, 0, 'Stout Mountain Horse'),
(38765, 14, -1682.62, 2507.22, 97.79, NULL, 0, 'Stout Mountain Horse'),
(38765, 15, -1669.61, 2519.56, 97.89, NULL, 0, 'Stout Mountain Horse');

-- 2) Clear any leftover smart script / C++ binding for this creature.
--    AIName must stay 'SmartAI' (SmartAI::Permissible gate) for smart_scripts to fire.
DELETE FROM `smart_scripts` WHERE `entryorguid` = 38765 AND `source_type` = 0;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 38765;

-- 2) Restore the original DB smart script (exactly the 6 backing rows).
INSERT INTO `smart_scripts`
(`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`,
 `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`,
 `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`,
 `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`,
 `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(38765, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 53, 1, 38765, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Stout Mountain Horse - On Passenger Boarded - Start WP'),
(38765, 0, 1, 0, 40, 0, 0, 0, 15, 0, 0, 0, 0, 28, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Stout Mountain Horse - On WP Reached 15 - Remove Passenger'),
(38765, 0, 2, 0, 40, 0, 0, 0, 3, 0, 0, 0, 0, 54, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Stout Mountain Horse - On WP Reached 3 - WP Pause (1000 ms)'),
(38765, 0, 3, 0, 40, 0, 0, 0, 3, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 14, 166783, 196863, 0, 0, 0, 0, 0, 0, 'Stout Mountain Horse - On WP Reached 3 - Open Gate'),
(38765, 0, 5, 0, 40, 0, 0, 0, 13, 0, 0, 0, 0, 54, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Stout Mountain Horse - On WP Reached 13 - WP Pause (1000 ms)'),
(38765, 0, 6, 0, 40, 0, 0, 0, 13, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 14, 166784, 196864, 0, 0, 0, 0, 0, 0, 'Stout Mountain Horse - On WP Reached 13 - Open Gate');