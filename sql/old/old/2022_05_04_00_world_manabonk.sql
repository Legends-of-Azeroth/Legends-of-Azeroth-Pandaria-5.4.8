DELETE FROM `creature` WHERE `guid` IN (54462, 54527, 54528, 54531, 54532, 54553);
UPDATE `creature_template` SET `ScriptName` = "npc_minigob_manabonk" WHERE `entry` = 32838;

DELETE FROM `game_event` WHERE `eventEntry` = 118;
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `description`, `announce`) VALUES
(118, "2011-03-22 00:10:00", "2030-12-31 23:59:00", 30, 5, "Dalaran: Minigob", 2);

DELETE FROM `game_event_creature` WHERE `guid` = 44457 AND `eventEntry` = 118;
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES
(118, 44457);
