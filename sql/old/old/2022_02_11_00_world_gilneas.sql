UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (219662, 219664);
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_josiah_avery" WHERE `entry` = 35369;
UPDATE `creature_template` SET `unit_flags` = 33024, `AIName` = "", `ScriptName` = "npc_josiah_avery_worgen_form" WHERE `entry` = 35370;

UPDATE `creature_template` SET `AIName` = "SmartAI", `flags_extra` = 128 WHERE `entry` = 50415;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 50415 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(50415, 0, 1, 0, 1, 0, 100, 1, 800, 800, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Josiah Event Trigger - OOC - Say Text Line 0 (No Repeat)");

DELETE FROM `creature_text` WHERE `entry` IN (50415, 35369);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(50415, 0, 0, "You've been bitten by a worgen. It's probably nothing, but it sure stings a little.$B$B|TInterface\\Icons\\INV_Misc_monsterfang_02.blp:32|t", "You've been bitten by a worgen. It's probably nothing, but it sure stings a little.$B$B|TInterface\\Icons\\INV_Misc_monsterfang_02.blp:32|t", 42, 0, 100, 0, 0, 0, "Josiah Event Trigger"),

(35369, 0, 0, "What's wrong with me??!", "", 14, 0, 100, 0, 0, 0, "Josiah Avery"),
(35369, 1, 0, "My face! What's wrong with my face!", "", 14, 0, 100, 0, 0, 0, "Josiah Avery"),
(35369, 2, 0, "My hands... don't look at my hands!", "", 14, 0, 100, 0, 0, 0, "Josiah Avery"),
(35369, 3, 0, "The pain is unbearable!", "", 14, 0, 100, 0, 0, 0, "Josiah Avery"),
(35369, 4, 0, "Make it stop!", "", 14, 0, 100, 0, 0, 0, "Josiah Avery"),
(35369, 5, 0, "I can't fight it!", "", 14, 0, 100, 0, 0, 0, "Josiah Avery");

DELETE FROM `conditions` WHERE `SourceEntry`= 67350 AND `SourceTypeOrReferenceId`= 13;
INSERT INTO conditions (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ScriptName`, `Comment`) VALUES
(13, 1, 67350, 0, 0, 31, 0, 3, 35374, 0, 0, 0, "", "Summon Josiah - Target Lab Trigger"),
(13, 4, 67350, 0, 0, 31, 0, 3, 35374, 0, 0, 0, "", "Summon Josiah - Target Lab Trigger");

DELETE FROM `spell_script_names` WHERE `spell_id` = 67357;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(67357, "spell_gilneas_pull_to");
