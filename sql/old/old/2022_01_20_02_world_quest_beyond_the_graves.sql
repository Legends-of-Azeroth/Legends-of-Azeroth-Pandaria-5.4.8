UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 2307;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 2307 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(2307, 0, 0, 0, 20, 0, 100, 0, 25089, 0, 0, 0, 85, 91938, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Caretaker Caice - On Quest Reward (25089) - Cast Spell To Summon");

UPDATE `creature` SET `position_x` = 1816.2699, `position_y` = 1589.7817, `position_z` = 96.536, `orientation` = 6.1166 WHERE `guid` = 192714;
UPDATE `creature` SET `position_x` = 1685.6896, `position_y` = 1647.7761, `position_z` = 137.354, `orientation` = 1.6925 WHERE `guid` = 192713;
UPDATE `creature` SET `position_x` = 1708.0999, `position_y` = 1644.4699, `position_z` = 126.151, `orientation` = 5.5768 WHERE `guid` = 192770;
UPDATE `creature` SET `position_x` = 1689.6483, `position_y` = 1674.8232, `position_z` = 135.612, `orientation` = 0.1570 WHERE `guid` = 192771;

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (-192770, -192714);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(-192770, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 1000, 1000, 45, 1, 1, 0, 0, 0, 0, 11, 49337, 8, 0, 0, 0, 0, 0, "Generic Bunny - PRK - On Update OOC (1000 - 1000) - Set Data 1 1 To Creature ID: 49337"),
(-192714, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 1000, 1000, 45, 2, 2, 0, 0, 0, 0, 11, 49337, 5, 0, 0, 0, 0, 0, "Generic Bunny - PRK - On Update OOC (1000 - 1000) - Set Data 2 2 To Creature ID: 49337");

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 49337;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49337 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (49337*100, 49337*100+01, 49337*100+02) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(49337, 0, 0, 4, 54, 0, 100, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Just Summoned - Say Text Line 0 (No Repeat)"),
(49337, 0, 1, 0, 38, 0, 100, 1, 1, 1, 0, 0, 80, 49337*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Data Set 1 1 - Start Script (No Repeat)"),
(49337, 0, 2, 0, 38, 0, 100, 1, 2, 2, 0, 0, 80, 49337*100+01, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Data Set 2 2 - Start Script (No Repeat)"),
(49337, 0, 3, 4, 54, 0, 100, 1, 0, 0, 0, 0, 80, 49337*100+02, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Just Summoned - Start Script (No Repeat)"),
(49337, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Just Summoned (Link) - Set React State (Passive)"),
(49337, 0, 5, 0, 8, 0, 100, 0, 46598, 0, 0, 0, 11, 91935, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Spell Hit (46598) - Cast Spell Self (91935)"),
(49337, 0, 6, 0, 38, 0, 100, 1, 3, 3, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Data Set 3 3 - Despawn Self"),

(49337*100, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Say Text Line 1"),
(49337*100, 9, 1, 0, 0, 0, 100, 0, 100, 100, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Set Run On"),
(49337*100, 9, 2, 0, 0, 0, 100, 0, 500, 500, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1783.75, 1595.59, 105.206, 0, "Darnell - On Script - Move To Position"),
(49337*100, 9, 3, 0, 0, 0, 100, 0, 12000, 12000, 0, 0, 29, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Set Follow Owner"),

(49337*100+01, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Say Text Line 2"),
(49337*100+01, 9, 1, 0, 0, 0, 100, 0, 100, 100, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Set Run On"),
(49337*100+01, 9, 2, 0, 0, 0, 100, 0, 500, 500, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1861.56, 1603.44, 98.11, 0, "Darnell - On Script - Move To Position"),
(49337*100+01, 9, 3, 0, 0, 0, 100, 0, 2500, 2500, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Say Text Line 3"),
(49337*100+01, 9, 4, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 29, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Set Follow Owner"),

(49337*100+02, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Say Text Line 4"),
(49337*100+02, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 5, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "Darnell - On Script - Say Text Line 5");

DELETE FROM `creature_text` WHERE `entry` = 49337;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(49337, 0, 0, "Hello again..", "", 12, 0, 100, 3, 0, 0, "Darnell"),
(49337, 1, 0, "I know the way to Deathknell. Come with me!", "", 12, 0, 100, 273, 0, 0, "Darnell"),
(49337, 2, 0, "Good, you're still here. Now, where's Deathguard Saltain?", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49337, 3, 0, "Ah, here he is.", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49337, 4, 0, "We make our way, $N. Saltain said that we can find a few dead bodies up here.", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49337, 5, 0, "I think I see some corpses up ahead. Let's go, $N! You do the searching and fighting. I'll do the lifting.", "", 12, 0, 100, 1, 0, 0, "Darnell");

DELETE FROM `conditions` WHERE `SourceEntry` = 49337 AND `SourceTypeOrReferenceId` = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(22, 1, 49337, 0, 0, 9, 0, 26800, 0, 0, 1, 0, 0, "", "SAI only when player has not take quest (26800)"),
(22, 4, 49337, 0, 0, 9, 0, 26800, 0, 0, 0, 0, 0, "", "SAI only when player has take quest (26800)");
