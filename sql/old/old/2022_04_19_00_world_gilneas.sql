UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (221718, 221717, 221348, 221716, 221352, 219776, 221137, 221141, 221139, 221140, 221138,
                                                                                  219357, 219356, 219360, 219359, 219358, 219362);
DELETE FROM `creature_addon` WHERE `guid` IN (221718, 221717, 221348, 221716, 221352);

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_james" WHERE `entry` = 36289;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_ashley" WHERE `entry` = 36288;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_cynthia" WHERE `entry` = 36287;

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (36287, 36288, 36289) AND `source_type` = 0;

DELETE FROM `creature_text` WHERE `entry` IN (36287, 36288, 36289);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(36287, 0, 0, "", "You are scary! I just want my mommy!", 12, 0, 100, 0, 0, 0, "Cynthia"),

(36288, 0, 0, "", "Are you one of the good worgen, $gmister:miss;? Did you see Cynthia hiding in the sheds outside?", 12, 0, 100, 0, 0, 0, "Ashley"),

(36289, 0, 0, "Don't hurt me! I was just looking for my sisters! I think Ashley's inside that house!", "", 12, 0, 100, 0, 0, 0, "James");
