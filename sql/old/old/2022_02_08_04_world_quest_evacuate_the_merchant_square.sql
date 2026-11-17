DELETE FROM `smart_scripts` WHERE `entryorguid` = 195327 AND `source_type` = 1;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (34981, 35836, 35660) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (1953270, 1953271, 3566000) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(195327, 1, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 87, 1953270, 1953271, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Merchant Square Door - Start Random Script"),

(34981, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frightened Citizen - On Just Summoned - Set Run On"),
(34981, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 19, 35830, 10, 0, 0, 0, 0, 0, "Frightened Citizen - On Just Summoned (Link) - Move To Position (1)"),
(34981, 0, 2, 0, 34, 0, 100, 0, 0, 1, 0, 0, 1, 0, 2000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frightened Citizen - MI 1 - Say Text Line 0 (2000 ms duration)"),
(34981, 0, 3, 0, 52, 0, 100, 0, 0, 34981, 0, 0, 69, 2, 0, 0, 0, 0, 0, 19, 35010, 20, 0, 0, 0, 0, 0, "Frightened Citizen - On Text 0 Over - Move To Position (2)"),
(34981, 0, 4, 0, 34, 0, 100, 0, 0, 2, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frightened Citizen - MI 2 - Despawn (1000 ms)"),

(35836, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frightened Citizen - On Just Summoned - Set Run On"),
(35836, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 19, 35830, 10, 0, 0, 0, 0, 0, "Frightened Citizen - On Just Summoned (Link) - Move To Position (1)"),
(35836, 0, 2, 0, 34, 0, 100, 0, 0, 1, 0, 0, 1, 0, 2000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frightened Citizen - MI 1 - Say Text Line 0 (2000 ms duration)"),
(35836, 0, 3, 0, 52, 0, 100, 0, 0, 35836, 0, 0, 69, 2, 0, 0, 0, 0, 0, 19, 35010, 20, 0, 0, 0, 0, 0, "Frightened Citizen - On Text 0 Over - Move To Position (2)"),
(35836, 0, 4, 0, 34, 0, 100, 0, 0, 2, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Frightened Citizen - MI 2 - Despawn (1000 ms)"),

(35660, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 80, 3566000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rampaging Worgen - On Just Summoned - Start Script"),

(1953270, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 85, 68087, 64, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Merchant Square Door - Summon Frightened Citizen"),
(1953270, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 33, 35830, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Merchant Square Door - Add Kill Credit"),

(1953271, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 85, 68070, 64, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Merchant Square Door - Summon Frightened Citizen and Rampaging Worgen"),
(1953271, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 33, 35830, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Merchant Square Door - Add Kill Credit"),

(3566000, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Rampaging Worgen - On Script - Start Attack Invoker");
