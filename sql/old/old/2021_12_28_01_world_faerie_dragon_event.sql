-- Faerie Dragon Event (Full)
UPDATE `creature` SET `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318974 AND `id` = 51966;

UPDATE `creature` SET `position_x` = 1765.84, `position_y` = 2205.46, `position_z` = 148.782, `orientation` = 0.78, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318968;
UPDATE `creature` SET `position_x` = 1733.86, `position_y` = 2239.33, `position_z` = 150.096, `orientation` = 0.01, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318969;
UPDATE `creature` SET `position_x` = 1756.01, `position_y` = 2279.27, `position_z` = 149.163, `orientation` = 5.5, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318970;
UPDATE `creature` SET `position_x` = 1821.38, `position_y` = 2298.49, `position_z` = 144.901, `orientation` = 4.18, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318976;
UPDATE `creature` SET `position_x` = 1865.11, `position_y` = 2269.06, `position_z` = 145.072, `orientation` = 3.63, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318977;
UPDATE `creature` SET `position_x` = 1857.66, `position_y` = 2210.51, `position_z` = 143.898, `orientation` = 2.72, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318978;
UPDATE `creature` SET `position_x` = 1813.91, `position_y` = 2174.47, `position_z` = 143.531, `orientation` = 1.73, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 318979;

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (51963, 51966);
UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry` = 51966;

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (51966, 51963, -318968, -318969, -318970, -318976, -318977, -318978, -318979) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (51966*100, 51963*100, 51963*100+01, 51963*100+02, 51963*100+03, 51963*100+04, 51963*100+05, 51963*100+06) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(51966, 0, 0, 0, 1, 0, 100, 0, 10000, 10000, 4510000, 4510000, 80, 51966*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - OOC (Every 75 min) - Start Script"),
(51966, 0, 1, 0, 54, 0, 100, 0, 0, 0, 0, 0, 11, 96186, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Just Summoned - Cast Spell 'Faerie Circle Wind Chimes'"),

(-318968, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318968, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318968, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1784.42, 2225.54, 149.007, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318968, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318968, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318968, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318968, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96187, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Faerie Dragon's Song'"),
(-318968, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1765.84, 2205.46, 148.782, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(-318969, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318969, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318969, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1781.42, 2242.3, 148.941, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318969, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100+01, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318969, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318969, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318969, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1733.86, 2239.33, 150.096, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(-318970, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318970, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318970, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1787.99, 2255.03, 149.103, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318970, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100+02, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318970, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318970, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318970, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1756.01, 2279.27, 149.163, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(-318976, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318976, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318976, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1808.14, 2259.5, 148.599, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318976, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100+03, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318976, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318976, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318976, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1821.38, 2298.49, 144.901, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(-318977, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318977, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318977, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1821.41, 2247.84, 148.731, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318977, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100+04, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318977, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318977, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318977, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1865.11, 2269.06, 145.072, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(-318978, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318978, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318978, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1818.87, 2229.71, 148.816, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318978, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100+05, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318978, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318978, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318978, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1857.66, 2210.51, 143.898, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(-318979, 0, 0, 0, 60, 0, 100, 1, 1000, 1000, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Update - Set Visiblity (0) (No Repeat)"),
(-318979, 0, 1, 2, 38, 0, 100, 0, 1, 1, 0, 0, 47, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 1 1 - Set Visiblity (1)"),
(-318979, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1803.93, 2220.34, 148.872, 0, "Fey-Drunk Darter - On Data Set 1 1 (Link) - Move To Position"),
(-318979, 0, 3, 0, 38, 0, 100, 0, 2, 2, 0, 0, 80, 51963*100+06, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 2 2 - Start Script"),
(-318979, 0, 4, 5, 38, 0, 100, 0, 3, 3, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Data Set 3 3 - Set Visiblity (0)"),
(-318979, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 92, 0, 96184, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Data Set 3 3 (Link) - Interrupt Spell 'Gift of the Heart'"),
(-318979, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1813.91, 2174.47, 143.531, 0, "Fey-Drunk Darter - On Data Set 3 3 (Link) - Move To Position"),

(51966*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318968, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318968)"),
(51966*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318969, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318969)"),
(51966*100, 9, 2, 0, 0, 0, 100, 0, 360000, 360000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318970, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318970) (36 sec)"), -- 6 min
(51966*100, 9, 3, 0, 0, 0, 100, 0, 900000, 900000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318976, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318976) (1.5 min)"), -- 15 min
(51966*100, 9, 4, 0, 0, 0, 100, 0, 480000, 480000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318977, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318977) (48 sec)"), -- 8 min
(51966*100, 9, 5, 0, 0, 0, 100, 0, 120000, 120000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318978, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318978) (12 sec)"), -- 2 min
(51966*100, 9, 6, 0, 0, 0, 100, 0, 960000, 960000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 318979, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 1 1 (Creature GUID: 318979) (1.6 min)"), -- 16 min
(51966*100, 9, 7, 0, 0, 0, 100, 0, 1440000, 1440000, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318968, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318968) (1.73 min)"), -- 24 min
(51966*100, 9, 8, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318969, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318969)"),
(51966*100, 9, 9, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318970, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318970)"),
(51966*100, 9, 10, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318976, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318976)"),
(51966*100, 9, 11, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318977, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318977)"),
(51966*100, 9, 12, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318978, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318978)"),
(51966*100, 9, 13, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 2, 2, 0, 0, 0, 0, 10, 318979, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 2 2 (Creature GUID: 318979)"),
(51966*100, 9, 14, 0, 0, 0, 100, 0, 500, 500, 0, 0, 11, 96181, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Cast Spell 'Faerie Circle Base'"),
(51966*100, 9, 15, 0, 0, 0, 100, 0, 138000, 138000, 0, 0, 92, 0, 96181, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Interrupt Spell 'Faerie Circle Base' (2.3 min)"),
(51966*100, 9, 16, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318968, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318968)"),
(51966*100, 9, 17, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318969, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318969)"),
(51966*100, 9, 18, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318970, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318970)"),
(51966*100, 9, 19, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318976, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318976)"),
(51966*100, 9, 20, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318977, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318977)"),
(51966*100, 9, 21, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318978, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318978)"),
(51966*100, 9, 22, 0, 0, 0, 100, 0, 200, 200, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 318979, 51963, 0, 0, 0, 0, 0, "Faerie Circle - On Script - Set Data 3 3 (Creature GUID: 318979)"),

(51963*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96187, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Faerie Dragon's Song'"),
(51963*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 51966, 3, 138000, 0, 0, 0, 8, 0, 0, 0, 1802.609, 2239.78, 148.9559, 0.2094, "Fey-Drunk Darter - On Script - Summon Creature"),
(51963*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)"),

(51963*100+01, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)"),
(51963*100+02, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)"),
(51963*100+03, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)"),
(51963*100+04, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)"),
(51963*100+05, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)"),
(51963*100+06, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 96184, 0, 0, 0, 0, 0, 10, 318974, 51966, 0, 0, 0, 0, 0, "Fey-Drunk Darter - On Script - Cast Spell 'Gift of the Heart' (Creature GUID: 318974)");

DELETE FROM `conditions` WHERE `SourceEntry` = 96184 AND `SourceTypeOrReferenceId` = 13 AND `SourceGroup` = 1;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 1, 96184, 0, 0, 31, 0, 3, 51966, 0, 0, 0, "", "Spell 'Gift of the Heart' target 'Faerie Circle'");
