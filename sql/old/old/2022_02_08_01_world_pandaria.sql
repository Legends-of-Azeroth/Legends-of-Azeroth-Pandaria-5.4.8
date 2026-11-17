UPDATE `creature_template` SET `AIName` = "" WHERE `entry` = 56663; 
UPDATE `creature_template` SET `AIName` = "" WHERE `entry` = 56660; 
UPDATE `creature_template` SET `AIName` = "" WHERE `entry` = 56661; 
UPDATE `creature_template` SET `AIName` = "" WHERE `entry` = 56676; 
UPDATE `creature_template` SET `AIName` = "" WHERE `entry` = 55918; 
UPDATE `creature_template` SET `AIName` = "" WHERE `entry` = 57769;

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "mob_aisa_pre_balon_event" WHERE `entry` = 56662; 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 56662;
UPDATE `creature_template` SET `IconName` = "vehichleCursor", `unit_flags` = 33536, `npcflag` = 16777216, `VehicleId` = 1887 WHERE `entry` = 55918;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 55649;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(55649, 46598, 1, 0);

UPDATE `creature_template_addon` SET `auras` = 49414 WHERE `entry` = 56663;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 55918;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(55918, 46598, 1, 0),
(55918, 128815, 1, 0),
(55918, 95247, 1, 0);

DELETE FROM `spell_script_names` WHERE `spell_id` IN (95247, 128815);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(95247, "spell_grab_air_balloon"), 
(128815, "spell_grab_air_balloon"); 

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 18 AND `SourceGroup` = 55918;
REPLACE INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ConditionTypeOrReference`, `ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`NegativeCondition`,`Comment`)VALUES 
(18, 55918, 128815, 9, 0, 29791, 0, 0, "Required quest active for spellclick"),
(18, 55918, 128815, 31, 0, 4, 0, 0, "Spellclick for player"),
(18, 55918, 46598, 31, 0, 3, 0, 0, "Spellclick for npc");

UPDATE `creature_template` SET `VehicleId` = 1820, `InhabitType` = 4, `ScriptName` = "mop_air_balloon" WHERE `entry` = 55649;

DELETE FROM `vehicle_template_accessory` WHERE `entry` IN (56739, 57464, 57690, 55918, 65476, 65477, 55649, 56661, 56662);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(56739, 54131, 0, 0, "Fe-Feng Hozen", 7, 0),
(57464, 57465, 0, 0, "Fe-Feng Ruffian", 7, 0),
(57690, 57691, 0, 1, "Tiger Pillar", 8, 0),
(55918, 56662, 0, 1, "Aysa", 8, 0),
(55918, 56663, 1, 1, "Ji Firepaw", 8, 0),
(55918, 65476, 2, 1, "Balloon Air Spirit Bunny", 8, 0),
(55918, 65477, 3, 1, "Balloon Fire Spirit Bunny", 8, 0),
(65476, 65102, 0, 1, "Dafeng", 8, 0),
(65477, 65107, 0, 1, "Huo", 8, 0),
(55649, 56661, 1, 1, "Aysa", 8, 0),
(55649, 56660, 2, 1, "Ji Firepaw", 8, 0),
(55649, 65476, 3, 1, "Balloon Air Spirit Bunny", 8, 0),
(55649, 65477, 4, 1, "Balloon Fire Spirit Bunny", 8, 0);

UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` IN (56676, 57769);

DELETE FROM `conditions` WHERE SourceEntry IN (114888, 114898, 106759, 118571, 118572);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 1, 114888, 0, 0, 31, 0, 3, 57769, 0, 0, 0, "", "Cast - ON - 57769"),
(13, 3, 114898, 0, 0, 31, 0, 3, 57769, 0, 0, 0, "", "Cast - ON - 57769"),
(13, 1, 106759, 0, 0, 31, 0, 3, 56676, 0, 0, 0, "", "Cast - ON - 56676"),
(13, 1, 118571, 0, 0, 31, 0, 3, 57769, 0, 0, 0, "", "Cast - ON - 57769"),
(13, 1, 118572, 0, 0, 31, 0, 3, 57769, 0, 0, 0, "", "Cast - ON - 57769");

DELETE FROM `spell_target_position` WHERE `id` = 105002 AND `target_map` = 860;
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(105002, 860, 915.559, 4563.667, 231.0833, 2.298086);

UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry` = 56676;

DELETE FROM `script_waypoint` WHERE `entry` = 55649;
INSERT INTO `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `point_comment`) VALUES
(55649, 1, 934.2396, 4571.044, 233.6516, "Shang Xi's Hot Air Balloon"),
(55649, 2, 1015.715, 4604.329, 216.5495, "Shang Xi's Hot Air Balloon"),
(55649, 3, 1056.448, 4658.655, 187.7374, "Shang Xi's Hot Air Balloon"),
(55649, 4, 1081.418, 4796.375, 157.6622, "Shang Xi's Hot Air Balloon"),
(55649, 5, 1091.849, 4926.616, 137.9268, "Shang Xi's Hot Air Balloon"),
(55649, 6, 1026.328, 5137.097, 138.0555, "Shang Xi's Hot Air Balloon"),
(55649, 7, 818.2934, 5216.31, 134.6396, "Shang Xi's Hot Air Balloon"),
(55649, 8, 621.7153, 5132.557, 142.15, "Shang Xi's Hot Air Balloon"),
(55649, 9, 547.2552, 5035.906, 130.4939, "Shang Xi's Hot Air Balloon"),
(55649, 10, 440.7517, 4867.261, 117.5795, "Shang Xi's Hot Air Balloon"),
(55649, 11, 403.5208, 4650.163, 81.13639, "Shang Xi's Hot Air Balloon"),
(55649, 12, 341.3038, 4475.935, 75.50136, "Shang Xi's Hot Air Balloon"),
(55649, 13, 222.5573, 4369.272, 93.21405, "Shang Xi's Hot Air Balloon"),
(55649, 14, 164.7587, 4297.126, 112.2426, "Shang Xi's Hot Air Balloon"),
(55649, 15, 131.7517, 4156.708, 124.6377, "Shang Xi's Hot Air Balloon"), -- Quest Credit
(55649, 16, 123.9514, 4109.052, 124.6377, "Shang Xi's Hot Air Balloon"),
(55649, 17, 112.0469, 3931.905, 128.3417, "Shang Xi's Hot Air Balloon"),
(55649, 18, 218.5, 3824.769, 137.5752, "Shang Xi's Hot Air Balloon"),
(55649, 19, 388.9236, 3765.605, 159.4655, "Shang Xi's Hot Air Balloon"),
(55649, 20, 596.5712, 3689.253, 183.0423, "Shang Xi's Hot Air Balloon"),
(55649, 21, 745.1858, 3664.532, 194.0496, "Shang Xi's Hot Air Balloon"); -- Despawn

DELETE FROM `vehicle_accessory` WHERE `guid` = 562981 AND `accessory_entry` = 55649;
INSERT INTO `vehicle_accessory` (`guid`,`accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(562981, 55649, 0, 0, "", 8, 0);
