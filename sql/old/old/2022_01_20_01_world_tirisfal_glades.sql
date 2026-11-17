UPDATE `creature_template` SET `faction_A` = 14, `faction_H` = 14 WHERE `entry` = 49422;

DELETE FROM `creature_addon` WHERE `guid` = 193968;
DELETE FROM `waypoint_data` WHERE `id` = 193968;

UPDATE `creature` SET `wander_distance` = 3, `movement_type` = 1 WHERE `id` IN (49422, 49423);
UPDATE `creature_template` SET `ScriptName` = "", `AIName` = "SmartAI" WHERE `entry` IN (49422, 49423);
UPDATE `creature_template` SET `unit_flags` = 33280, `ScriptName` = "", `AIName` = "SmartAI" WHERE `entry` = 49424;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (49422, 49423, 49424) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49422, 0, 0, 0, 4, 0, 100, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Berserker - On Aggro - Say Text Line 0 (No Repeat)"),
(49422, 0, 1, 2, 2, 0, 100, 1, 0, 30, 0, 0, 11, 63227, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Berserker - 0-30% HP - Cast Spell 'Enrage' (No Repeat)"),
(49422, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Berserker - 0-30% HP (Link) - Say Text Line 1 (No Repeat)"),
(49422, 0, 3, 0, 0, 0, 100, 0, 4000, 7000, 15000, 18000, 11, 79881, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Berserker - In Combat - Cast Spell 'Slam'"),
(49422, 0, 4, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 11, 49428, 50, 0, 0, 0, 0, 0, "Rotbrain Berserker - On Reset - Start Attack (Creature ID: 49428, 50 Yards)"),
(49422, 0, 5, 0, 1, 0, 100, 0, 10000, 10000, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 11, 49428, 50, 0, 0, 0, 0, 0, "Rotbrain Berserker - OOC (10000 - 10000) - Start Attack (Creature ID: 49428, 50 Yards)"),

(49423, 0, 0, 0, 4, 0, 100, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Magus - On Aggro - Say Text Line 0 (No Repeat)"),
(49423, 0, 1, 0, 0, 0, 100, 0, 0, 0, 3400, 4700, 11, 9053, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Magus - In Combat - Cast Spell 'Fireball'"),
(49423, 0, 2, 0, 0, 0, 100, 0, 7000, 9000, 18000, 25000, 11, 11962, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Rotbrain Magus - In Combat - Cast Spell 'Immolate'"),
(49423, 0, 3, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 11, 49428, 50, 0, 0, 0, 0, 0, "Rotbrain Magus - On Reset - Start Attack (Creature ID: 49428, 50 Yards)"),
(49423, 0, 4, 0, 1, 0, 100, 0, 10000, 10000, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 11, 49428, 50, 0, 0, 0, 0, 0, "Rotbrain Magus - OOC (10000 - 10000) - Start Attack (Creature ID: 49428, 50 Yards)"),

(49424, 0, 0, 1, 4, 0, 100, 1, 0, 0, 0, 0, 11, 83015, 0, 0, 0, 0, 0, 21, 25, 0, 0, 0, 0, 0, 0, "Marshal Redpath - On Aggro - Cast Spell 'Heroic Leap' (No Repeat)"),
(49424, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Marshal Redpath - On Aggro (Link) - Say Text Line 0 (No Repeat)"),
(49424, 0, 2, 0, 0, 0, 100, 1, 5000, 9000, 6000, 10000, 11, 33239, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Marshal Redpath - In Combat - Cast Spell 'Whirlwind'");

DELETE FROM `creature_text` WHERE `entry` IN (49422, 49423, 49424);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(49422, 0, 0, "I'll kill you!", "I'll kill you!", 12, 0, 15, 0, 0, 0, "Rotbrain Berserker"),
(49422, 0, 1, "Look what they've done to me!", "Look what they've done to me!", 12, 0, 15, 0, 0, 0, "Rotbrain Berserker"),
(49422, 0, 2, "Who am I?", "Who am I?", 12, 0, 15, 0, 0, 0, "Rotbrain Berserker"),
(49422, 1, 0, "%s becomes enraged!", "%s becomes enraged!", 16, 0, 100, 0, 0, 0, "Rotbrain Berserker"),

(49423, 0, 0, "I'll kill you!", "I'll kill you!", 12, 0, 15, 0, 0, 0, "Rotbrain Magus"),
(49423, 0, 1, "Look what they've done to me!", "Look what they've done to me!", 12, 0, 15, 0, 0, 0, "Rotbrain Magus"),
(49423, 0, 2, "Who am I?", "Who am I?", 12, 0, 15, 0, 0, 0, "Rotbrain Magus"),

(49424, 0, 0, "BLEEAAAGGHHH! I'm a monster, don't look at me!", "", 14, 0, 100, 0, 0, 0, "Rotbrain Magus");
