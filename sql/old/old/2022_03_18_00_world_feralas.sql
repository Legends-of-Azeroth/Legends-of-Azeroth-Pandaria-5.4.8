UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 40132;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` = 40132;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(40132,0,0,0,1,0,100,0,1000,15000,90000,120000,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Vestia Moonspear - OOC - Say Text Line 0");

DELETE FROM `creature_text` WHERE `entry` = 40132;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(40132, 0, 0, "", "Magic is a tool, some would even say a gift. But you should never forget its fundamental nature.", 12, 0, 100, 1, 0, 0, "Vestia Moonspear"),
(40132, 0, 1, "", "Arcane magic twists the boundaries of our world. It weakens the laws of this realm to allow the impossible to happen, if only for a moment.", 12, 0, 100, 1, 0, 0, "Vestia Moonspear"),
(40132, 0, 2, "", "That single moment is all a demon needs to wreak havoc. The Burning Legion has watched us for millenia, always waiting for the smallest rift to slip through, the smallest stirring to whisper from.", 12, 0, 100, 1, 0, 0, "Vestia Moonspear"),
(40132, 0, 3, "", "Remember this, young ones. Every spell you cast, no matter how minor, cuts both ways. This is why sorcery must never be used frivolously.", 12, 0, 100, 1, 0, 0, "Vestia Moonspear");

UPDATE `creature` SET `spawntimesecs` = 300, `wander_distance` = 10, `movement_type` = 1 WHERE `id` = 40079;
DELETE FROM `creature_addon` WHERE `guid` IN (257412,257413,257414,257415,257416,257453,257454,257455,257570,257746,257876,257877,257878,257879,
                                              257880,257881,257882,258102,258103,258104,258105,258106,259279,259281,259282,259287);
DELETE FROM `waypoint_data` WHERE `id` IN (257412,257413,257414,257415,257416,257453,257454,257455,257570,257746,257876,257877,257878,257879,
                                           257880,257881,257882,258102,258103,258104,258105,258106,259279,259281,259282,259287);

UPDATE `creature_template` SET `InhabitType` = 4, `AIName` = "", `ScriptName` = "npc_feralas_wisp" WHERE `entry` = 40079;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 40079 AND `source_type` = 0;

DELETE FROM `conditions` WHERE `SourceEntry` = 74738 AND `SourceTypeOrReferenceId` = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 74738, 0, 0, 31, 1, 3, 40079, 0, 0, 0, "", "Rally Wild Wisp - Target Feralas Wisp");

UPDATE `creature_template` SET `InhabitType` = 5, `AIName` = "", `ScriptName` = "npc_feralas_hippogryph" WHERE `entry` IN (5300, 5304);
UPDATE `creature_template` SET `minlevel` = 36, `maxlevel` = 37, `faction_A` = 83, `faction_H` = 83, `AIName` = "", `ScriptName` = "npc_feralas_horde_poacher" WHERE `entry` = 40069;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (5300, 5304, 40069) AND `source_type` = 0;

UPDATE `quest_template` SET `QuestGiverPortrait` = 3210, `QuestGiverTargetName` = "Frayfeather Hippogryph", `QuestGiverTextWindow` = "Frayfeather Hippogryph" WHERE `id` = 25409;

DELETE FROM `conditions` WHERE `SourceEntry` = 74674 AND `SourceTypeOrReferenceId` = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 74674, 0, 0, 31, 1, 3, 5300, 0, 0, 0, "", "Rally Wild Hippogryff - Target Frayfeather Hippogryph"),
(17, 0, 74674, 0, 1, 31, 1, 3, 5304, 0, 0, 0, "", "Rally Wild Hippogryff - Target Frayfeather Stagwing");

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_feralas_wandering_forest_walker" WHERE `entry` = 7584;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 7584 AND `source_type` = 0;

UPDATE `quest_template` SET `QuestGiverPortrait` = 9592, `QuestGiverTargetName` = "Wandering Forest Walker", `QuestGiverTextWindow` = "Wandering Forest Walker" WHERE `id` = 25410;

DELETE FROM `conditions` WHERE `SourceEntry` = 74756 AND `SourceTypeOrReferenceId` = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 74756, 0, 0, 31, 1, 3, 7584, 0, 0, 0, "", "Rally Wild Treant - Target Wandering Forest Walker");
