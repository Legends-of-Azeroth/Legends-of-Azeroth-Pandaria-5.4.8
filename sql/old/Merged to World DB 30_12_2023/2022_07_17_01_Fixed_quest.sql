-- Problemas en la gesta la daga en la oscuridad
update `creature_template` set `npcflag`=3 where `entry` = 67414;
update `creature_template` set `faction_H`= 15 where `entry` in (67779,67792);
update `creature_template` set `faction_A`= 15 where `entry` in (67779,67792);

-- quest 32247
update `creature_template` set `AIName`= 'SmartAI', `npcflag`= 1 where `entry` in (68312,68331);

DELETE FROM `smart_scripts` WHERE `entryorguid`=68331 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(68331, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 33, 68331, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'kc');
DELETE FROM `smart_scripts` WHERE `entryorguid`=68312 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(68312, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 33, 68312, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'kc');

-- misc
update `creature_template` set `npcflag`=1 where `entry` in (67927,67926);




-- 30834 
update `creature_template` set `AIName`= 'SmartAI' where `entry`= 59272;

DELETE FROM `smart_scripts` WHERE `entryorguid`=59272 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(59272, 0, 0, 0, 19, 0, 100, 0, 30834, 0, 0, 0, 33, 60982, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '');


-- 30491 
update `creature_template` set `AIName`= 'SmartAI' where `entry` in (61693,59354);

DELETE FROM `smart_scripts` WHERE `entryorguid`=61693 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(61693, 0, 0, 1, 19, 0, 100, 0, 30491, 0, 0, 0, 33, 59319, 0, 6, 6, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(61693, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 208, 30491, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '');

DELETE FROM `smart_scripts` WHERE `entryorguid`=59354 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(59354, 0, 0, 0, 19, 0, 100, 0, 30492, 0, 0, 0, 85, 116473, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On quest accept - Summon yaks');



-- 30692 
update `creature_template` set `AIName`= 'SmartAI' where `entry`= 59703;

DELETE FROM `smart_scripts` WHERE `entryorguid`=59703 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(59703, 0, 0, 0, 19, 0, 100, 0, 30692, 0, 0, 0, 33, 59749, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On quest accept - Summon yaks');


-- 30610
update `creature_template` set `AIName`= 'SmartAI' where `entry`= 59894;

DELETE FROM `smart_scripts` WHERE `entryorguid`=59894 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(59894, 0, 0, 1, 19, 0, 100, 0, 30610, 0, 0, 0, 33, 60120, 0, 5, 5, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ''),
(59894, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 208, 30610, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, '');
 
