UPDATE `creature` SET `wander_distance` = 15, `movement_type` = 1 WHERE `id` = 35463;
DELETE FROM `creature_template_addon` WHERE `entry` = 35463;
DELETE FROM `creature_addon` WHERE `guid` IN (221071, 221163, 221305, 221314, 221318, 221319, 221326, 221339);
DELETE FROM `waypoint_data` WHERE `id` IN (221163, 221326);

UPDATE `creature_template` SET `ScriptName` = "", `AIName` = "SmartAI" WHERE `entry` = 35463;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 35463 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(35463, 0, 0, 1, 2, 0, 100, 0, 0, 30, 0, 0, 11, 8599, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Lurker - At HP 0-30% - Cast Spell 'Enrage'"),
(35463, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Lurker - At HP 0-30% (Link) - Say Text Line 0"),
(35463, 0, 2, 4, 11, 0, 100, 0, 0, 0, 0, 0, 11, 5916, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Lurker - On Respawn - Cast Spell 'Shadowstalker Stealth'"),
(35463, 0, 3, 4, 7, 0, 100, 0, 0, 0, 0, 0, 11, 5916, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Lurker - On Evade - Cast Spell 'Shadowstalker Stealth'"),
(35463, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Lurker - Link - Set React State Passive");
