DELETE FROM `smart_scripts` WHERE `entryorguid`=66292 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, 
`target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(66292, 0, 0, 1, 62, 0, 100, 0, 51000, 0, 0, 0, 62, 870, 0, 0, 0, 0, 0, 7, 0, 0, 0, -668.4260, -1482.3900, 130.2999, 0, ''),
(66292, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 208, 29548, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(66292, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 201, 94, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(66292, 0, 3, 0, 19, 0, 100, 0, 31732, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 54615, 15, 0, 0, 0, 0, 0, '');

update `creature_template` set `AIName`= 'SmartAI' where `entry`= 54615;

DELETE FROM `smart_scripts` WHERE `entryorguid`=54615 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(54615, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 1, 2, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '');



update `creature_template` set `AIName`= 'SmartAI', `npcflag`= 16777216 where `entry`= 66297;
update `creature_template` set `AIName`= 'SmartAI' where `entry`= 66295;

INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(66297, 130323, 1, 0);


DELETE FROM `smart_scripts` WHERE `entryorguid`=66297 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(66297, 0, 0, 1, 73, 0, 100, 0, 0, 0, 0, 0, 33, 66398, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(66297, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 33, 66397, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(66297, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 33, 66400, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(66297, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 66401, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '');
 
DELETE FROM `smart_scripts` WHERE `entryorguid`=66295 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(66295, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');


update `creature_template` set `faction_A`= 2361, `faction_H`= 2361 where `entry`= 66327;
update `creature_template` set `faction_A`= 35, `faction_H`= 35 where `entry`= 66339;


INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582894, 66290, 870, 5785, 5853, 1, 1, -667.432, -1569.37, 6.1631, 1.65613  , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582895, 66290, 870, 5785, 5853, 1, 1, -665.463, -1592.38, 16.0288, 3.63219 , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582896, 66290, 870, 5785, 5853, 1, 1, -652.103, -1547.49, 5.08312, 0.289532, 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582897, 66290, 870, 5785, 5853, 1, 1, -562.988, -1692.01, 9.81827, 1.32099 , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582898, 66290, 870, 5785, 5853, 1, 1, -660.203, -1527.69, 2.06662, 1.80692 , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582899, 66290, 870, 5785, 5853, 1, 1, -705.448, -1774, 0.570535, 0.332969  , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582900, 66290, 870, 5785, 5853, 1, 1, -728.182, -1732.61, 27.8416, 4.68323 , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582901, 66290, 870, 5785, 5853, 1, 1, -589.297, -1744.49, 6.31247, 4.68323 , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582902, 66290, 870, 5785, 5853, 1, 1, -578.613, -1685.2, 9.99234, 4.92417  , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582903, 66290, 870, 5785, 5853, 1, 1, -575.438, -1648.23, 12.2619, 4.68323 , 60);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES (582904, 66290, 870, 5785, 5853, 1, 1, -626.477, -1694.04, 14.704, 4.68323  , 60);

DELETE FROM `smart_scripts` WHERE `entryorguid`=66424 AND `source_type`=0 AND `id`=1 AND `link`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES (66424, 0, 1, 0, 6, 0, 100, 0, 0, 0, 0, 0, 28, 131031, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, '');



/*

*/
