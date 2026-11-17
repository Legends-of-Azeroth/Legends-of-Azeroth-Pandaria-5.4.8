-- Ruul the Darkener: one stationary spawn on his drake over Shadowmoon Valley, as in AzerothCore. 73715 was set to waypoint movement
-- without a path ("doesn't have waypoint path id: 0"), and 77079 was a second copy of the rare at -4148, 590.
UPDATE `creature_template` SET `MovementType` = 0 WHERE `entry` = 21315;

DELETE FROM `creature` WHERE `guid` = 77079 AND `id` = 21315;
DELETE FROM `creature_addon` WHERE `guid` IN (73715, 77079);
UPDATE `creature` SET `position_x` = -3761.74, `position_y` = 1081.67, `position_z` = 125.316, `orientation` = 4.5238, `spawntimesecs` = 300,
`wander_distance` = 0, `currentwaypoint` = 0, `MovementType` = 0 WHERE `guid` = 73715;
