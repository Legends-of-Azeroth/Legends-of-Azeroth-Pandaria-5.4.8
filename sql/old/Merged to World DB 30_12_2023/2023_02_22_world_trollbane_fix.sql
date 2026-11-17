-- Trollbane SAI
DELETE FROM `smart_scripts` WHERE (`entryorguid`, `source_type`, `id`, `link`) IN ((16819, 0, 0, 1),(16819, 0, 1, 0),(16819, 0, 2, 0),(16819, 0, 3, 0));
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(16819, 0, 0, 1, 20, 0, 100, 0, 10254, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Commander Danath Trollbane - On Quest Rewarded - Say 0'),
(16819, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 6245, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Commander Danath Trollbane - On Quest Rewarded - Cast Force Target - Salute'),
(16819, 0, 2, 0, 20, 0, 100, 0, 11002, 0, 0, 0, 0, 1, 1, 7000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Force Commander Danath Trollbane - On Quest Reward 11002 - Say Line 1'),
(16819, 0, 3, 0, 52, 0, 100, 0, 1, 16819, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Force Commander Danath Trollbane - On Text Over Line 1 - Say Line 2');

-- Remove old quest Revered Among Honor Hold
DELETE FROM `creature_queststarter` WHERE `quest`=10558;

-- Update trollbane AI
UPDATE `creature_template` SET `AIName`='SmartAI',`ScriptName`='' WHERE `entry`=16819;