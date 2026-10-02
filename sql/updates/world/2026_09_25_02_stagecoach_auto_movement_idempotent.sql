-- ============================================================
-- Gilneas stagecoach: idempotent final state for moving escort
--
-- Consolidated final state. Safe to apply on a fresh DB or on top of
-- any earlier stagecoach state (it overrides all of it). Supersedes the
-- earlier incremental files (2026_09_24_00, 2026_09_25_00, 2026_09_25_01),
-- which have been removed.
--
-- Final state:
--   38755 Stagecoach Harness   VehicleId=970, no spellclick, marker only
--   43336 Stagecoach Harness   VehicleId=958, ScriptName=npc_stagecoach_harness
--   43338 Stagecoach Horse     VehicleId=0, decorative / boarded by script
--   44928 Stagecoach Carriage  VehicleId=959, ScriptName=npc_stagecoach_carriage_exodus
--
-- 46598 = hardcoded Ride Vehicle spell (SPELL_AURA_CONTROL_VEHICLE)
-- 72767 is intentionally NOT used.
-- ============================================================

-- ---------- creature_template ----------

UPDATE creature_template
SET npcflag = 0,
    unit_flags = 0,
    unit_flags2 = 2048,
    VehicleId = 970,
    AIName = '',
    ScriptName = ''
WHERE entry = 38755;

UPDATE creature_template
SET npcflag = 0,
    unit_flags = 0,
    unit_flags2 = 268437504,
    VehicleId = 958,
    AIName = '',
    ScriptName = 'npc_stagecoach_harness'
WHERE entry = 43336;

UPDATE creature_template
SET npcflag = 0,
    unit_flags = 0,
    unit_flags2 = 268437504,
    VehicleId = 0,
    AIName = '',
    ScriptName = ''
WHERE entry = 43338;

UPDATE creature_template
SET npcflag = 1,
    unit_flags = 0,
    unit_flags2 = 2048,
    VehicleId = 959,
    AIName = '',
    ScriptName = 'npc_stagecoach_carriage_exodus'
WHERE entry = 44928;

-- ---------- smart_scripts ----------

DELETE FROM smart_scripts
WHERE source_type = 0 AND entryorguid IN (43336, 44928);

-- ---------- npc_spellclick_spells ----------

DELETE FROM npc_spellclick_spells
WHERE npc_entry IN (38755, 43336, 43338, 44928);

INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags, user_type) VALUES
    (43336, 46598, 0, 0),
    (44928, 46598, 0, 0);

-- ---------- vehicle_template_accessory ----------

DELETE FROM vehicle_template_accessory
WHERE entry IN (43336, 44928);

INSERT INTO vehicle_template_accessory (entry, accessory_entry, seat_id, minion, description, summontype, summontimer) VALUES
    (43336, 43338, 0, 1, 'Stagecoach Harness - Horse', 8, 0),
    (43336, 43338, 1, 1, 'Stagecoach Harness - Horse', 8, 0),
    (43336, 44928, 2, 1, 'Stagecoach Harness - Caravan', 8, 0),
    (44928, 38853, 0, 1, 'Stagecoach Carriage', 8, 0),
    (44928, 44460, 2, 1, 'Stagecoach Carriage', 8, 0),
    (44928, 36138, 3, 1, 'Stagecoach Carriage', 8, 0),
    (44928, 43907, 4, 1, 'Stagecoach Carriage', 8, 0),
    (44928, 43907, 5, 1, 'Stagecoach Carriage', 8, 0),
    (44928, 51409, 6, 1, 'Stagecoach Carriage', 8, 0);

-- ---------- script_waypoint ----------

DELETE FROM script_waypoint WHERE entry = 43336;

INSERT INTO script_waypoint (entry, pointid, location_x, location_y, location_z, waittime, point_comment) VALUES
    (43336, 1, -1681.45, 2508.27, 97.84, 0, 'Stagecoach Harness'),
    (43336, 2, -1695.46, 2486.82, 92.64, 0, 'Stagecoach Harness'),
    (43336, 3, -1704.99, 2468.26, 84.84, 0, 'Stagecoach Harness'),
    (43336, 4, -1698.38, 2447.65, 80.77, 0, 'Stagecoach Harness'),
    (43336, 5, -1698.42, 2432.73, 76.52, 0, 'Stagecoach Harness'),
    (43336, 6, -1725.88, 2390.50, 60.80, 0, 'Stagecoach Harness'),
    (43336, 7, -1735.64, 2362.27, 63.15, 0, 'Stagecoach Harness'),
    (43336, 8, -1745.18, 2343.69, 67.34, 0, 'Stagecoach Harness'),
    (43336, 9, -1746.53, 2329.92, 69.60, 0, 'Stagecoach Harness'),
    (43336, 10, -1757.21, 2300.38, 75.77, 0, 'Stagecoach Harness'),
    (43336, 11, -1776.38, 2271.80, 82.11, 0, 'Stagecoach Harness'),
    (43336, 12, -1799.38, 2251.48, 87.64, 0, 'Stagecoach Harness'),
    (43336, 13, -1826.19, 2238.05, 89.31, 0, 'Stagecoach Harness'),
    (43336, 14, -1868.50, 2174.60, 89.31, 0, 'Stagecoach Harness'),
    (43336, 15, -1872.45, 2135.71, 89.31, 0, 'Stagecoach Harness'),
    (43336, 16, -1872.96, 2075.18, 89.31, 0, 'Stagecoach Harness'),
    (43336, 17, -1881.14, 2046.58, 89.31, 0, 'Stagecoach Harness'),
    (43336, 18, -1885.72, 2019.56, 89.31, 0, 'Stagecoach Harness'),
    (43336, 19, -1876.85, 1970.16, 89.17, 0, 'Stagecoach Harness'),
    (43336, 20, -1878.35, 1921.33, 89.13, 0, 'Stagecoach Harness'),
    (43336, 21, -1890.52, 1904.26, 89.15, 0, 'Stagecoach Harness'),
    (43336, 22, -1990.23, 1901.42, 89.28, 0, 'Stagecoach Harness'),
    (43336, 23, -2036.81, 1914.56, 83.23, 0, 'Stagecoach Harness'),
    (43336, 24, -2061.07, 1905.41, 73.95, 0, 'Stagecoach Harness'),
    (43336, 25, -2093.23, 1881.99, 53.77, 0, 'Stagecoach Harness'),
    (43336, 26, -2103.06, 1870.42, 46.52, 0, 'Stagecoach Harness'),
    (43336, 27, -2122.61, 1831.95, 29.18, 0, 'Stagecoach Harness'),
    (43336, 28, -2146.32, 1814.97, 19.03, 0, 'Stagecoach Harness'),
    (43336, 29, -2186.33, 1808.11, 12.11, 0, 'Stagecoach Harness'),
    (43336, 30, -2217.77, 1809.60, 11.78, 0, 'Stagecoach Harness'),
    (43336, 31, -2239.38, 1805.10, 11.94, 0, 'Stagecoach Harness'),
    (43336, 32, -2310.30, 1774.33, 11.05, 0, 'Stagecoach Harness'),
    (43336, 33, -2376.50, 1704.52, 11.15, 0, 'Stagecoach Harness');

-- ---------- creature spawns ----------
-- The summoned moving harness is 43336. It should not have a static spawn.
DELETE FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id = 43336);
DELETE FROM creature WHERE id = 43336;

INSERT IGNORE INTO creature (guid, id, map, spawnMask, phaseMask, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, wander_distance, MovementType) VALUES
    (219210, 38755, 654, 1, 180224, 0, -1670.155, 2532.311, 97.848, 4.387, 300, 0, 0),
    (219213, 44928, 654, 1, 180224, 0, -1669.974, 2532.950, 97.848, 4.367, 300, 0, 0),
    (421640, 43338, 654, 1, 180224, 0, -1671.675, 2530.066, 97.848, 4.391, 300, 0, 0),
    (421641, 43338, 654, 1, 180224, 0, -1670.333, 2529.619, 97.848, 4.391, 300, 0, 0);
