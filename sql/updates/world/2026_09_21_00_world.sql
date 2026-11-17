-- ============================================================
-- Quest 14465 - Stout Mountain Horse (creature_template.entry = 38765).
--
-- Migrate the horse's path logic from DB smart_scripts to the C++ script
-- StoutMountainHorseAI (src/server/scripts/Pandaria/zone_isle_of_giants.cpp,
-- registered via RegisterCreatureAI in AddSC_isle_of_giants).
--
-- The C++ AI handles:
--   * PassengerBoarded -> StartPath(true, 38765)
--   * WP3  -> PausePath(1000) + open gate GO 196863
--   * WP13 -> PausePath(1000) + open gate GO 196864
--   * WP15 -> RemoveAurasDueToSpell(46598)  (dismount passenger)
--
-- Idempotent: safe to re-run on any environment.
-- ============================================================

-- 1) Ensure the 15 path waypoints exist.
REPLACE INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `point_comment`) VALUES
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

-- 2) Point the creature at the C++ script.
UPDATE `creature_template` SET `ScriptName` = 'StoutMountainHorseAI' WHERE `entry` = 38765;

-- 3) Remove the old DB smart script so it does not duplicate the C++ logic.
DELETE FROM `smart_scripts` WHERE `entryorguid` = 38765;
