UPDATE `creature_template` SET `ScriptName` = "", `AIName` = "SmartAI" WHERE `entry` = 25462;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 25462 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 25462*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(25462, 0, 0, 1, 25, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Reset (Link) - Set Phase 1"),
(25462, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 53274, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Reset - Cast Spell 'Icebound Visage'"),
(25462, 0, 2, 0, 19, 1, 100, 0, 12593, 0, 0, 0, 80, 25462*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Quest Accept (In Service Of The Lich King ID: 12593) - Start Script (Phase 1)"),

(25462*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Set Phase 0"),
(25462*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 58024, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Cast 'Icecrown Airship - A - Attack - 02b Cannon Explosion, Shield' (Phase 0)"),
(25462*100, 9, 2, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Say Text Line 0 (Phase 0)"),
(25462*100, 9, 3, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 5, 396, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Play Emote (396) (Phase 0)"),
(25462*100, 9, 4, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 5, 396, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Play Emote (396) (Phase 0)"),
(25462*100, 9, 5, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Say Text Line 1 (Phase 0)"),
(25462*100, 9, 6, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 5, 396, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Play Emote (396) (Phase 0)"),
(25462*100, 9, 7, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 5, 396, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Play Emote (396) (Phase 0)"),
(25462*100, 9, 8, 0, 0, 0, 100, 0, 9500, 9500, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Say Text Line 2 (Phase 0)"),
(25462*100, 9, 9, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 5, 396, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Play Emote (396) (Phase 0)"),
(25462*100, 9, 10, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "The Lich King - On Script - Set Phase 1 (Phase 0)");

DELETE FROM `creature_text` WHERE `entry` = 25462;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`) VALUES
(25462, 0, 0, "All that I am: anger, cruelty, vengeance - I bestow upon you, my chosen knight. I have granted you immortality so that you may herald in a new, dark age for the Scourge.", 12, 0, 100, 396, 0, 14734, 0, "The Lich King"),
(25462, 1, 0, "Gaze now upon the lands below us. The Scarlet Crusade scurries to undo my work, while Light's Hope stands defiantly against us - a blemish upon these Plaguelands. They must all be shown the price of their defiance.", 12, 0, 100, 397, 0, 14735, 0, "The Lich King"),
(25462, 2, 0, "You will become my force of retribution. Where you tread, doom will follow. Go now and claim your destiny, death knight.", 12, 0, 100, 396, 0, 14736, 0, "The Lich King");
