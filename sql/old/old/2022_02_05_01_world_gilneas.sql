UPDATE `creature_template` SET `ScriptName` = "npc_prince_liam_greymane" WHERE `entry` = 34850;

DELETE FROM `creature_text` WHERE `entry` = 34850;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(34850, 0, 0, "Stand ready, guards! We don't know how many intruders we're dealing with, but the Headlands are overrun and we're cut off from the harbor towns. Expect to be outnumbered.", "", 12, 0, 100, 0, 0, 19614, "Prince Liam Greymane"),
(34850, 1, 0, "I want the perimeter secured and the gates manned by two guards at all times. No one gets in, no one gets out.", "", 12, 0, 100, 0, 0, 19615, "Prince Liam Greymane"),
(34850, 2, 0, "We protected Gilneas from the Scourge. We protected Gilneas during the Northgate rebellion. We will protect Gilneas from whatever this new threat may be.", "", 12, 0, 100, 0, 0, 19616, "Prince Liam Greymane");

UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 55, `walk_mode` = 2 WHERE `guid` IN (220889, 220965, 219624, 219808, 220637, 220636, 220557, 220976, 220723);
DELETE FROM `creature_addon` WHERE `guid` IN (220889, 219624);
DELETE FROM `waypoint_data` WHERE `id` IN (220889, 219624);
