UPDATE `quest_template` SET `RewardSpellCast` = 0, `Flags` = 2621440 WHERE `Id` = 24959;
UPDATE `creature_template` SET `InhabitType` = 4, `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 49044;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49044 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49044, 0, 0, 1, 19, 0, 100, 0, 24959, 0, 0, 0, 11, 73524, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Agatha - On Quest Accept (24959) - Cast Spell 'Val'kyr Resurrection'"),
(49044, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 28, 73523, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Agatha - On Quest Accept (24959) (Link) - Remove Aura 'Rigor Mortis'");
