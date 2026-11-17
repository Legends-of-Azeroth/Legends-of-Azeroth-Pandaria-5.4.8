-- Instructor Qun
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 57748;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 57748 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (5774800, 5774801, 5774802, 5774803) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(57748, 0, 0, 0, 1, 0, 100, 0, 0, 0, 5000, 5000, 87, 5774800, 5774801, 5774802, 5774803, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - OOC - Start Random Script"),

(5774800, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 10, 507, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Emote (507)"),
(5774800, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Random Sound (33643, 33645, 33646)"),
(5774800, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 1, 0, 0, 0, 0, 11, 0, 15, 0, 0, 0, 0, 0, "Instructor Qun - Set Data 3 1"),
(5774801, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 10, 509, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Emote"),
(5774801, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Random Sound (33643, 33645, 33646)"),
(5774801, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 2, 0, 0, 0, 0, 11, 0, 15, 0, 0, 0, 0, 0, "Instructor Qun - Set Data 3 2"),
(5774802, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 10, 511, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Emote"),
(5774802, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Random Sound (33643, 33645, 33646)"),
(5774802, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 11, 0, 15, 0, 0, 0, 0, 0, "Instructor Qun - Set Data 3 3"),
(5774803, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 10, 543, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Emote"),
(5774803, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Instructor Qun - Play Random Sound (33643, 33645, 33646)"),
(5774803, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 4, 0, 0, 0, 0, 11, 0, 15, 0, 0, 0, 0, 0, "Instructor Qun - Set Data 3 4");

-- Aspiring Trainee
UPDATE `creature` SET `position_x` = 1491.057129, `position_y` = 3439.181152, `position_z` = 171.162048, `orientation` = 6.186045 WHERE `guid` = 563357;

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` IN (53565, 65469);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (53565, 65469, -563382, -563383) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (5356500, 5356501, 5356502, 5356503, 5356504) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(53565, 0, 0, 1, 38, 0, 100, 0, 1, 1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 1 1 - Set Phase 1"),
(53565, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 17, 510, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 1 1 (Link) - Set Emote State (510)"),
(53565, 0, 2, 3, 60, 1, 100, 0, 1000, 1000, 5000, 5000, 11, 108967, 0, 0, 0, 0, 0, 11, 0, 4, 0, 0, 0, 0, 0, "Aspiring Trainee - On Update - Cast Spell 'Jab'"),
(53565, 0, 3, 4, 61, 1, 100, 0, 0, 0, 0, 0, 10, 507, 509, 511, 543, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Update (Link) - Play Random Emote (507, 509, 511, 543)"),
(53565, 0, 4, 0, 61, 1, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Update (Link) - Play Random Sound (33643, 33645, 33646)"),
(53565, 0, 5, 6, 38, 0, 100, 0, 2, 2, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 2 2 - Set Phase 2"),
(53565, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 17, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 2 2 (Link) - Clear Emote"),
(53565, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 2 2 (Link) - Say Text Line 0"),
(53565, 0, 8, 0, 38, 0, 100, 0, 3, 1, 0, 0, 80, 5356500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 1 - Start Script"),
(53565, 0, 9, 0, 38, 0, 100, 0, 3, 2, 0, 0, 80, 5356502, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 2 - Start Script"),
(53565, 0, 10, 0, 38, 0, 100, 0, 3, 3, 0, 0, 80, 5356503, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 3 - Start Script"),
(53565, 0, 11, 0, 38, 0, 100, 0, 3, 4, 0, 0, 80, 5356504, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 4 - Start Script"),

(5356500, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 507, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Emote (507)"),
(5356500, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Random Sound (33643, 33645, 33646)"),

(5356501, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 508, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Emote (508)"),
(5356501, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Random Sound (33643, 33645, 33646)"),

(5356502, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 509, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Emote (509)"),
(5356502, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Random Sound (33643, 33645, 33646)"),

(5356503, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 511, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Emote (511)"),
(5356503, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Random Sound (33643, 33645, 33646)"),

(5356504, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 5, 543, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Emote (543)"),
(5356504, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - Play Random Sound (33643, 33645, 33646)"),

(65469, 0, 0, 1, 38, 0, 100, 0, 1, 1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 1 1 - Set Phase 1"),
(65469, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 17, 510, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 1 1 (Link) - Set Emote State (510)"),
(65469, 0, 2, 3, 60, 1, 100, 0, 1000, 1000, 5000, 5000, 11, 108967, 0, 0, 0, 0, 0, 11, 0, 4, 0, 0, 0, 0, 0, "Aspiring Trainee - On Update - Cast Spell 'Jab'"),
(65469, 0, 3, 4, 61, 1, 100, 0, 0, 0, 0, 0, 10, 507, 509, 511, 543, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Update (Link) - Play Random Emote (507, 509, 511, 543)"),
(65469, 0, 4, 0, 61, 1, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Update (Link) - Play Random Sound (33643, 33645, 33646)"),
(65469, 0, 5, 6, 38, 0, 100, 0, 2, 2, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 2 2 - Set Phase 2"),
(65469, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 17, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 2 2 (Link) - Clear Emote"),
(65469, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 2 2 (Link) - Say Text Line 0"),
(65469, 0, 8, 0, 38, 0, 100, 0, 3, 1, 0, 0, 80, 5356500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 1 - Start Script"),
(65469, 0, 9, 0, 38, 0, 100, 0, 3, 2, 0, 0, 80, 5356502, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 2 - Start Script"),
(65469, 0, 10, 0, 38, 0, 100, 0, 3, 3, 0, 0, 80, 5356503, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 3 - Start Script"),
(65469, 0, 11, 0, 38, 0, 100, 0, 3, 4, 0, 0, 80, 5356504, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Data Set 3 4 - Start Script"),

(-563382, 0, 0, 0, 1, 0, 100, 0, 20000, 20000, 120000, 120000, 1, 1, 16000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - OOC - Say Text Line 1"),
(-563382, 0, 1, 0, 52, 0, 100, 0, 1, 53565, 0, 0, 1, 2, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 1 Over - Say Text Line 2"),
(-563382, 0, 2, 0, 52, 0, 100, 0, 2, 53565, 0, 0, 1, 3, 11000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 2 Over - Say Text Line 3"),
(-563382, 0, 3, 0, 52, 0, 100, 0, 3, 53565, 0, 0, 1, 4, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 3 Over - Say Text Line 4"),
(-563382, 0, 4, 0, 52, 0, 100, 0, 4, 53565, 0, 0, 45, 1, 1, 0, 0, 0, 0, 11, 53565, 3, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 4 Over - Set Data 1 1"),

(-563383, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 1, 5, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 1 Over - Say Text Line 5"),
(-563383, 0, 1, 0, 52, 0, 100, 0, 5, 53565, 0, 0, 1, 6, 15000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 5 Over - Say Text Line 6"),
(-563383, 0, 2, 0, 52, 0, 100, 0, 6, 53565, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Aspiring Trainee - On Text Line 6 Over - Say Text Line 7");

DELETE FROM `creature_addon` WHERE `guid` IN (563382, 563383);
INSERT INTO `creature_addon` (`guid`, `bytes2`) VALUES
(563382, 1),
(563383, 1);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceEntry` = 108967;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 1, 108967, 0, 0, 31, 0, 3, 53714, 0, 0, 0, 0, "", "Jab target only Training Target"),
(13, 1, 108967, 0, 1, 31, 0, 3, 57873, 0, 0, 0, 0, "", "Jab target only Training Target");

DELETE FROM `creature_text` WHERE `entry` = 53565;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(53565, 0, 0, "Such skill for a new trainee.", "Such skill for a new trainee.", 12, 0, 100, 511, 0, 0, "Aspiring Trainee"),
(53565, 0, 1, "That target didn't stand a chance.", "That target didn't stand a chance.", 12, 0, 100, 511, 0, 0, "Aspiring Trainee"),
(53565, 0, 2, "One day you will have to teach me your secrets.", "One day you will have to teach me your secrets.", 12, 0, 100, 509, 0, 0, "Aspiring Trainee"),
(53565, 0, 3, "You are bound for a great things, trainee.", "You are bound for a great things, trainee.", 12, 0, 100, 508, 0, 0, "Aspiring Trainee"),
(53565, 0, 4, "I must train harder so I can be like you.", "I must train harder so I can be like you.", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 0, 5, "I hope the instructors saw that!", "I hope the instructors saw that!", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 0, 6, "I must work as hard as you.", "I must work as hard as you.", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 0, 7, "Your form is impeccable.", "Your form is impeccable.", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 1, 0, "My money's on Lam. Did you see him take down Chu during training last week?", "", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 2, 0, "His stamina is nothing against Lam's strength.", "", 12, 0, 100, 0, 0, 0, "Aspiring Trainee"),
(53565, 3, 0, "He has the legs of an ox! Have you ever been kicked by him in training?", "", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 4, 0, "Raise you a keg of Stormshout Gold. It'll be your loss.", "", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 5, 0, "That's where you're mistaken, friend. Zhou is as sturdy as an oak.", "", 12, 0, 100, 1, 0, 0, "Aspiring Trainee"),
(53565, 6, 0, "Zhou has the stamina of three men, so he can out-stay most opponents.", "", 12, 0, 100, 0, 0, 0, "Aspiring Trainee"),
(53565, 7, 0, "Four coppers on Zhou.", "", 12, 0, 100, 0, 0, 0, "Aspiring Trainee");

-- Quiet Lam
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 57752;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 57752 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(57752, 0, 0, 1, 1, 0, 100, 0, 0, 0, 5000, 5000, 10, 507, 509, 511, 543, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Quiet Lam - OOC - Play Random Emote (507, 509, 511, 543)"),
(57752, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Quiet Lam - Play Random Sound (33643, 33645, 33646)");

-- Ironfist Zhou
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 57753;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 57753 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(57753, 0, 0, 1, 1, 0, 100, 0, 0, 0, 5000, 5000, 10, 507, 509, 511, 543, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Ironfist Zhou - OOC - Play Random Emote (507, 509, 511, 543)"),
(57753, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 115, 33643, 33645, 33646, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, "Ironfist Zhou - Play Random Sound (33643, 33645, 33646)");
