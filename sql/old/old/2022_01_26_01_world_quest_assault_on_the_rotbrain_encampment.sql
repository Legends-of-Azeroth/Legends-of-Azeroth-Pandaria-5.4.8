UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (1569, 49426);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (1569, 49426) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49426*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(1569, 0, 0, 1, 20, 0, 100, 0, 24971, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Shadow Priest Sarvis - On Quest Reward (24971) - Say Text Line 0"),

(49426, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 80, 49426*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Just Summoned - Start Script"),

(49426*100, 9, 0, 0, 0, 0, 100, 0, 100, 100, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1843.147095, 1631.313599, 96.93, 0, "Deathguard Darnell - On Script - Move To Position"),
(49426*100, 9, 1, 0, 0, 0, 100, 0, 11000, 11000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 21, 100, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Say Text Line 0"),
(49426*100, 9, 2, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Say Text Line 1"),
(49426*100, 9, 3, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Say Text Line 2"),
(49426*100, 9, 4, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Say Text Line 3"),
(49426*100, 9, 5, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 21, 100, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Say Text Line 4"),
(49426*100, 9, 6, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 5, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Play Emote (3)"),
(49426*100, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Deathguard Darnell - On Script - Despawn Self");

DELETE FROM `creature_text` WHERE `entry` IN (1569, 49426);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(1569, 0, 0, "Well, look who's here!", "", 12, 0, 100, 5, 0, 0, "Shadow Priest Sarvis"),

(49426, 0, 0, "Hi, $n! What do you think about my new gear?", "", 12, 0, 100, 23, 0, 0, "Deathguard Darnell"),
(49426, 1, 0, "They're making me into a Deathguard. Can you believe it?", "", 12, 0, 100, 1, 0, 0, "Deathguard Darnell"),
(49426, 2, 0, "I'll probably be staying here in Deathknell. If things work out though... who knows? Maybe I'll get to meet the Dark Lady herself.", "", 12, 0, 100, 6, 0, 0, "Deathguard Darnell"),
(49426, 3, 0, "How about you? I've heard that Sarvis is going to send you off to Calston Estate. Big assignment, eh?", "", 12, 0, 100, 1, 0, 0, "Deathguard Darnell"),
(49426, 4, 0, "Well, I won't hold you up. It's been a pleasure. I hope our paths cross again soon.", "", 12, 0, 100, 66, 0, 0, "Deathguard Darnell");
