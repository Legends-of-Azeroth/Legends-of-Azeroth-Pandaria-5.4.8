UPDATE `creature_template` SET `ScriptName`='', `AIname`='SmartAI' WHERE `entry`=55685;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (55685);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(55685, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 85, 46598, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uplifting Draft - Just Spawned - Invoker Cast Ride Vehicle Hardcoded'),
(55685, 0, 1, 0, 27, 0, 100, 0, 0, 0, 0, 0, 53, 1, 55685, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uplifting Draft - Passenger Boarded - Start WP'),
(55685, 0, 2, 3, 58, 0, 100, 0, 13, 55685, 0, 0, 11, 68576, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uplifting Draft - WP Ended - Cast Eject All Passengers'),
(55685, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 11, 104490, 2, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Uplifting Draft - WP Ended - Cast Forcecast Trigger Ji Air Plateau Departure'),
(55685, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uplifting Draft - WP Ended - Despawn');
