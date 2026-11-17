UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 38217;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (38217, -251585) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(-251585, 0, 0, 0, 0, 0, 100, 0, 0, 0, 2300, 3900, 11, 95826, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Darkspear Watcher - In Combat - Cast 'Shoot'"),
(-251585, 0, 1, 0, 1, 0, 100, 0, 1000, 3000, 2000, 4000, 5, 37, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darkspear Watcher - OOC - Play Emote (37)");

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 37951;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 37951 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 37951*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(37951, 0, 0, 1, 19, 0, 100, 0, 24750, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 1, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 2, 3, 19, 0, 100, 0, 24758, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 3, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 4, 5, 19, 0, 100, 0, 24770, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 5, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 6, 7, 19, 0, 100, 0, 24776, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 7, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 8, 9, 19, 0, 100, 0, 24782, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 9, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 10, 11, 19, 0, 100, 0, 24607, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 11, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 12, 13, 19, 0, 100, 0, 24764, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 13, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 14, 15, 19, 0, 100, 0, 26272, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 15, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 16, 17, 19, 0, 100, 0, 31159, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept - Say Text Line 0"),
(37951, 0, 17, 18, 61, 0, 100, 0, 0, 0, 0, 0, 12, 37988, 8, 0, 0, 0, 0, 8, 0, 0, 0, -1173.453, -5266.401, 0.9423928, 0.2298513, "Jin'thala - On Quest Accept (Link) - Summon Zuni"),

(37951, 0, 18, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 37951*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Quest Accept (Link) - Start Script"),

(37951*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Script - Remove Npc Flag Quest Giver"),
(37951*100, 9, 1, 0, 0, 0, 100, 0, 16000, 16000, 0, 0, 81, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Jin'thala - On Script - Add Npc Flag Quest Giver");

DELETE FROM `creature_text` WHERE `entry` = 37951;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(37951, 0, 0, "$n. Zuni. Ya'll find ya trainer in the trainin' grounds to the east. Bring pride to the Darkspear.", "", 12, 0, 100, 1, 0, 0, "Jin'thala");

DELETE FROM `waypoints` WHERE `entry` = 37988*10;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES
(37988*10, 1, -1167.87, -5281.1, 1.15112, "Zuni Point 1"),
(37988*10, 2, -1171.67, -5302.84, 6.65308, "Zuni Point 2"),
(37988*10, 3, -1179.91, -5314.7, 11.0816, "Zuni Point 3"),
(37988*10, 4, -1190.13, -5326.58, 13.3516, "Zuni Point 4"),
(37988*10, 5, -1207.07, -5354.74, 13.2643, "Zuni Point 5"),
(37988*10, 6, -1207.28, -5373.21, 12.4616, "Zuni Point 6"),
(37988*10, 7, -1181.83, -5379.26, 11.951, "Zuni Point 7"),
(37988*10, 8, -1162.04, -5391.74, 12.06027, "Zuni Point 8");

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 37988;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 37988 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (37988*100, 37988*100+01) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(37988, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 80, 37988*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Zuni - On Just Summon - Start Script"),
(37988, 0, 1, 0, 40, 0, 100, 0, 8, 0, 0, 0, 80, 37988*100+01, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Zuni - On WP 8 Reached - Start Script"),

(37988*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.2298513, "Zuni - On Script - Set Orientation (0.2298513)"),
(37988*100, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1.655128, "Zuni - On Script - Set Orientation (1.655128)"),
(37988*100, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Zuni - On Script - Say Text Line 0"),
(37988*100, 9, 3, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 53, 1, 37988*10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Zuni - On Script - Start WP"),

(37988*100+01, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.490659, "Zuni - On Script - Set Orientation (3.490659)"),
(37988*100+01, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Zuni - On Script - Say Text Line 1"),
(37988*100+01, 9, 2, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Zuni - On Script - Despawn Self");

DELETE FROM `creature_text` WHERE `entry` = 37988;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(37988, 0, 0, "Ya, mon. Let's crack some tiki target skulls!", "", 12, 0, 100, 5, 0, 21366, "Zuni"),
(37988, 1, 0, "Ya trainer should be somewhere in the grounds 'ere. I'll catch you lata, mon.", "", 12, 0, 100, 1, 0, 21367, "Zuni");

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (38038, 38278, 38279, 42619, 38281, 38268, 38282, 38272, 38280) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (3803800, 3803801, 3803802) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
-- Tiki Target
(38038, 0, 0, 1, 11, 0, 100, 0, 0, 0, 0, 0, 87, 3803800, 3803801, 3803802, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On Respawn - Run Random Script"),
(38038, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - Linked To Id 0 - Set Health Regen 0"),
(38038, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - Linked To Id 0 - Set Health Regen 0"),
(38038, 0, 3, 0, 1, 0, 100, 0, 1000, 1000, 1000, 1000, 203, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - In Combat - Combat Stop"),
(38038, 0, 4, 0, 8, 0, 100, 0, 100, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Charge - Quest Credit"),
(38038, 0, 5, 0, 8, 0, 100, 0, 20271, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Judgment - Quest Credit"),
(38038, 0, 6, 0, 8, 0, 100, 0, 56641, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Steady Shot - Quest Credit"),
(38038, 0, 7, 0, 8, 0, 100, 0, 2098, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Run Through - Quest Credit"),
(38038, 0, 8, 0, 8, 0, 100, 0, 73899, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Primal Strike - Quest Credit"),
(38038, 0, 9, 0, 8, 0, 100, 0, 122, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Frost Nova - Quest Credit"),
(38038, 0, 10, 0, 8, 0, 100, 0, 172, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Corruption - Quest Credit"),
(38038, 0, 11, 0, 8, 0, 100, 0, 589, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Shadow Word: Pain - Quest Credit"),
(38038, 0, 12, 0, 8, 0, 100, 0, 8921, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Moonfire - Quest Credit"),
(38038, 0, 13, 0, 8, 0, 100, 0, 100787, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On SpellHit Tiger Palm - Quest Credit"),
(38038, 0, 14, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 71240, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On Death - Cast Spell 'Tiki Target Death'"),
-- Tiki Target Action List 00
(3803800, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 11, 71064, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Training Dummy - On Action List - Cast Visual 1"),
(3803800, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38281, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38268, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 42619, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38272, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38282, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38278, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38279, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803800, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38280, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
-- Tiki Target Action List 01
(3803801, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 11, 71065, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On Script - Cast Tiki Target Visual 2"),
(3803801, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38281, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38268, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 42619, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38272, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38282, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38278, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38279, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803801, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38280, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
-- Tiki Target Action List 02
(3803802, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 11, 71066, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tiki Target - On Script - Cast Tiki Target Visual 3"),
(3803802, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38281, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38268, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 42619, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38272, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38282, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38278, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38279, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
(3803802, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 38280, 50, 0, 0, 0, 0, 0, "Tiki Target - On Script - Set Data 0 1"),
-- Novice Darkspear Priest
(38278, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 11, 9734, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Priest - On Data Set 0 1 - Attack Start"),
(38278, 0, 1, 0, 60, 0, 100, 0, 3000, 6000, 3000, 6000, 11, 9734, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Priest - In Combat - Cast Spell 'Holy Smite'"),
-- Novice Darkspear Mage
(38279, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 11, 20797, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Mage - On Data Set 0 1 - Attack Start"),
(38279, 0, 1, 0, 60, 0, 100, 0, 3500, 6500, 3500, 6500, 11, 20797, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Mage - In Combat - Cast Spell 'Fireball'"),
-- Novice Darkspear Warlock
(42619, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 11, 20791, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Warlock - On Data Set 0 1 - Attack Start"),
(42619, 0, 1, 0, 60, 0, 100, 0, 3500, 6500, 3500, 6500, 11, 20791, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Warlock - In Combat - Cast Spell 'Shadow Bolt'"),
-- Novice Darkspear Shaman
(38281, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 11, 20802, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Shaman - On Data Set 0 1 - Attack Start"),
(38281, 0, 1, 0, 60, 0, 100, 0, 3500, 6500, 3500, 6500, 11, 20802, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Shaman - In Combat - Cast Spell 'Lightning Bolt'"),
-- Novice Darkspear Warrior
(38268, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Warrior - On Data Set 0 1 - Attack Start"),
(38268, 0, 1, 0, 60, 0, 100, 0, 3000, 6000, 3000, 6000, 11, 25710, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Novice Darkspear Warrior - In Combat - Cast Spell 'Heroic Strike'"),
-- Novice Darkspear Hunter
(38282, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 11, 6660, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Hunter - On Data Set 0 1 - Attack Start"),
(38282, 0, 1, 0, 60, 0, 100, 0, 1000, 4000, 1000, 4000, 11, 6660, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Hunter - In Combat - Cast Spell 'Shoot'"),
-- Novice Darkspear Rogue
(38272, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Rogue - On Data Set 0 1 - Attack Start"),
(38272, 0, 1, 0, 60, 0, 100, 0, 3000, 6000, 3000, 6000, 11, 60195, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Novice Darkspear Rogue - In Combat - Cast Spell 'Sinister Strike'"),
-- Novice Darkspear Druid
(38280, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 11, 9739, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Druid - On Data Set 0 1 - Attack Start"),
(38280, 0, 1, 0, 60, 0, 100, 0, 2500, 2500, 2500, 2500, 11, 9739, 0, 0, 0, 0, 0, 19, 38038, 20, 0, 0, 0, 0, 0, "Novice Darkspear Druid - In Combat - Cast Spell 'Wrath'");

DELETE FROM `creature_sparring_template` WHERE `creature_id` = 38038;
INSERT INTO `creature_sparring_template`(`creature_id`, `health_limit_pct`, `comment`) VALUES
(38038,85,"Tiki Target");
