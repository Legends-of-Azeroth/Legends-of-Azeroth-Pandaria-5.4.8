UPDATE `creature_template` SET `InhabitType` = 4, `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 49044;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49044 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49044, 0, 0, 1, 19, 0, 100, 0, 24959, 0, 0, 0, 86, 73524, 0, 7, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Agatha - On Quest Accept (24959) - Cast Spell"),
(49044, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 28, 73523, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Agatha - On Quest Accept (24959) (Link) - Remove Aura");

DELETE FROM `creature_template_addon` WHERE `entry` = 49044;
INSERT INTO `creature_template_addon` (`entry`, `bytes1`, `bytes2`, `auras`) VALUES
(49044, 50397184, 4097, "49414");

UPDATE `quest_template` SET `DetailsEmote1` = 1, `DetailsEmote2` = 1 WHERE `Id` = 24959;

UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` = 49129;
UPDATE `creature` SET `MovementType` = 2, `spawndist` = 0 WHERE `guid` = 192757 AND `id` = 49129;
UPDATE `creature_addon` SET `path_id` = 192757, `bytes2` = 1 WHERE `guid` = 192757;
DELETE FROM `waypoint_data` WHERE `id` = 192757;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `entry`) VALUES
(192757, 1, 1725.01, 1667.63, 137.94, 0, 0, 1, 49129),
(192757, 2, 1710.03, 1656.84, 140.835, 0, 0, 1, 49129),
(192757, 3, 1698.03, 1652.92, 146.397, 0, 0, 1, 49129),
(192757, 4, 1702.94, 1634.51, 140.377, 0, 0, 1, 49129),
(192757, 5, 1716.34, 1617.19, 134.848, 0, 0, 1, 49129),
(192757, 6, 1731.1, 1618.59, 133.88, 0, 0, 1, 49129),
(192757, 7, 1747.64, 1639.03, 128.976, 0, 0, 1, 49129),
(192757, 8, 1746.05, 1657.5, 131.738, 0, 0, 1, 49129),
(192757, 9, 1744.97, 1676.7, 139.807, 0, 0, 1, 49129),
(192757, 10, 1737.38, 1689.39, 142.346, 0, 0, 1, 49129),
(192757, 11, 1721.84, 1699.9, 145.727, 0, 0, 1, 49129),
(192757, 12, 1702.88, 1701.67, 147.952, 0, 0, 1, 49129),
(192757, 13, 1684.23, 1697.14, 151.702, 0, 0, 1, 49129),
(192757, 14, 1679.76, 1677.61, 157.186, 0, 0, 1, 49129),
(192757, 15, 1688.75, 1665.62, 152.315, 0, 0, 1, 49129),
(192757, 16, 1706.31, 1666.57, 144.594, 0, 0, 1, 49129),
(192757, 17, 1721.01, 1675.69, 138.709, 0, 0, 1, 49129);

UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` = 49128;
UPDATE `creature` SET `MovementType` = 2, `spawndist` = 0 WHERE `guid` = 192760 AND `id` = 49128;
DELETE FROM `creature_addon` WHERE `guid` = 192760;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes1`, `bytes2`) VALUES
(192760, 192760, 50331648, 1);
DELETE FROM `waypoint_data` WHERE `id` = 192760;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `entry`) VALUES
(192760, 1, 1757.78, 1593.55, 116.917, 0, 0, 1, 49128),
(192760, 2, 1758.89, 1604.89, 127.626, 0, 0, 1, 49128),
(192760, 3, 1754.94, 1620.19, 131.002, 0, 0, 1, 49128),
(192760, 4, 1741.96, 1635.04, 141.067, 0, 0, 1, 49128),
(192760, 5, 1722.93, 1649.94, 144.151, 0, 0, 1, 49128),
(192760, 6, 1703.4, 1655.41, 156.222, 0, 0, 1, 49128),
(192760, 7, 1683.45, 1647.61, 158.232, 0, 0, 1, 49128),
(192760, 8, 1678.86, 1635.12, 154.974, 0, 0, 1, 49128),
(192760, 9, 1681.15, 1625.05, 153.188, 0, 0, 1, 49128),
(192760, 10, 1699.46, 1616.94, 146.098, 0, 0, 1, 49128),
(192760, 11, 1701.61, 1601.05, 142.797, 0, 0, 1, 49128),
(192760, 12, 1697.91, 1578.1, 144.346, 0, 0, 1, 49128),
(192760, 13, 1705.4, 1557.76, 143.995, 0, 0, 1, 49128),
(192760, 14, 1719.17, 1551.01, 136.678, 0, 0, 1, 49128),
(192760, 15, 1736.93, 1555, 129.218, 0, 0, 1, 49128),
(192760, 16, 1746.94, 1565.07, 123.072, 0, 0, 1, 49128),
(192760, 17, 1755.16, 1577.36, 119.816, 0, 0, 1, 49128);

UPDATE `creature` SET `MovementType` = 2, `spawndist` = 0, `position_x` = 1674.95, `position_y` = 1683.32, `position_z` = 139.203 WHERE `guid` = 192759 AND `id` = 50372;
UPDATE `creature_template` SET `InhabitType` = 4, `AIName` = "", `ScriptName` = "npc_aradne" WHERE `entry` = 50372;

DELETE FROM `creature_addon` WHERE `guid` = 192759;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes1`, `bytes2`) VALUES
(192759, 192759, 50331648, 1);

DELETE FROM `waypoint_data` WHERE `id` = 192759;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `entry`) VALUES
(192759, 1, 1674.95, 1683.32, 143.203, 0, 0, 0, 50372),
(192759, 2, 1676.98, 1681.87, 142.799, 0, 0, 0, 50372),
(192759, 3, 1679.82, 1681.69, 142.267, 0, 0, 0, 50372),
(192759, 4, 1681.72, 1683.03, 141.959, 0, 0, 0, 50372),
(192759, 5, 1683.93, 1685.34, 141.606, 0, 10000, 0, 50372),
(192759, 6, 1686.35, 1686.91, 141.154, 0, 0, 0, 50372),
(192759, 7, 1688.19, 1690.18, 140.926, 0, 0, 0, 50372),
(192759, 8, 1689.24, 1693.53, 140.947, 0, 0, 0, 50372),
(192759, 9, 1691.95, 1695.14, 140.673, 0, 0, 0, 50372),
(192759, 10, 1695.06, 1695.14, 140.008, 0, 0, 0, 50372),
(192759, 11, 1697.34, 1693.19, 139.244, 0, 0, 0, 50372),
(192759, 12, 1698, 1691.2, 139.049, 0, 0, 0, 50372),
(192759, 13, 1698.51, 1688.87, 138.864, 0, 10000, 0, 50372),
(192759, 14, 1698.55, 1685.78, 138.671, 0, 0, 0, 50372),
(192759, 15, 1698.47, 1683.06, 138.616, 0, 0, 0, 50372),
(192759, 16, 1697.66, 1680.8, 138.579, 0, 10000, 0, 50372),
(192759, 17, 1699.37, 1684.32, 138.532, 0, 0, 0, 50372),
(192759, 18, 1702.7, 1690.31, 138.334, 0, 0, 0, 50372),
(192759, 19, 1704.96, 1695.01, 138.167, 0, 0, 0, 50372),
(192759, 20, 1706.95, 1696.84, 137.934, 0, 0, 0, 50372),
(192759, 21, 1707.11, 1700.6, 138.225, 0, 10000, 0, 50372),
(192759, 22, 1710.43, 1700.99, 137.2, 0, 0, 0, 50372),
(192759, 23, 1716.15, 1701.67, 135.765, 0, 0, 0, 50372),
(192759, 24, 1721.19, 1702.27, 134.121, 0, 0, 0, 50372),
(192759, 25, 1726.76, 1703.41, 132.53, 0, 0, 0, 50372),
(192759, 26, 1730.24, 1703.47, 132.115, 0, 0, 0, 50372),
(192759, 27, 1733.31, 1703.32, 132.178, 0, 10000, 0, 50372),
(192759, 28, 1726.35, 1702.37, 132.503, 0, 0, 0, 50372),
(192759, 29, 1719.89, 1698.69, 134.855, 0, 0, 0, 50372),
(192759, 30, 1710.81, 1691.48, 137.626, 0, 0, 0, 50372),
(192759, 31, 1703.63, 1685.48, 138.024, 0, 0, 0, 50372),
(192759, 32, 1697.91, 1681.32, 138.569, 0, 10000, 0, 50372),
(192759, 33, 1698.1, 1683.77, 138.69, 0, 0, 0, 50372),
(192759, 34, 1698.15, 1688.19, 138.898, 0, 10000, 0, 50372),
(192759, 35, 1699.09, 1691.26, 138.879, 0, 0, 0, 50372),
(192759, 36, 1701.96, 1695.36, 138.619, 0, 0, 0, 50372),
(192759, 37, 1705.86, 1696.63, 138.068, 0, 0, 0, 50372),
(192759, 38, 1706.74, 1700.46, 138.263, 0, 10000, 0, 50372),
(192759, 39, 1700.76, 1698.66, 139.131, 0, 0, 0, 50372),
(192759, 40, 1696.39, 1697.44, 139.9, 0, 0, 0, 50372),
(192759, 41, 1691.82, 1695.35, 140.723, 0, 0, 0, 50372),
(192759, 42, 1687.56, 1692.08, 141.147, 0, 0, 0, 50372),
(192759, 43, 1686.83, 1690.55, 141.241, 0, 0, 0, 50372);

DELETE FROM `creature_text` WHERE `entry` = 50372;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(50372, 0, 0, "", "Rise from the grave and serve the Dark Lady!", 12, 0, 100, 1, 0, 0, "Aradne"),
(50372, 0, 1, "", "In the name of Sylvanas, the Banshee Queen, I bestow this gift upon you!", 12, 0, 100, 1, 0, 0, "Aradne"),
(50372, 0, 2, "", "Rise from death's slumber and join your brothers!", 12, 0, 100, 1, 0, 0, "Aradne"),
(50372, 0, 3, "", "Waken, sleeper. Your new life awaits.", 12, 0, 100, 1, 0, 0, "Aradne"),
(50372, 0, 4, "", "You are returned to life. The Banshee Queen asks for your service.", 12, 0, 100, 1, 0, 0, "Aradne");

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_deathknell_grave_target" WHERE `entry` = 50373;

DELETE FROM `conditions` WHERE `SourceEntry` = 93446 AND `SourceTypeOrReferenceId` = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 1, 93446, 0, 0, 31, 0, 3, 50373, 0, 0, 0, "", "Raise Undead target Deathknell Grave Target");

UPDATE `creature` SET `MovementType` = 0, `spawndist` = 0 WHERE `id` = 50373;

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_risen_dead" WHERE `entry` = 50374;
DELETE FROM `creature_text` WHERE `entry` = 50374;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(50374, 0, 0, "What... what's happening to me? Why have you done this to me?", "", 12, 0, 100, 6, 0, 0, "Risen Dead"),
(50374, 1, 0, "I should be dead and to death I will return!", "", 12, 0, 100, 5, 0, 0, "Risen Dead"),
(50374, 2, 0, "I never asked for this! Leave me alone!", "", 12, 0, 100, 5, 0, 0, "Risen Dead"),
(50374, 3, 0, "What magic is this that turns back the hand of death?", "", 12, 0, 100, 6, 0, 0, "Risen Dead"),
(50374, 4, 0, "If the Banshee Queen has offered me this chance, I will gladly serve.", "", 12, 0, 100, 1, 0, 0, "Risen Dead"),
(50374, 5, 0, "This is Lady Sylvanas's doing?", "", 12, 0, 100, 6, 0, 0, "Risen Dead"),
(50374, 6, 0, "What use has the Dark Lady for me?", "", 12, 0, 100, 6, 0, 0, "Risen Dead"),
(50374, 7, 0, "I... I don't remember... Why have you done this?", "", 12, 0, 100, 6, 0, 0, "Risen Dead"),
(50374, 8, 0, "%s commits suicide.", "", 16, 0, 100, 0, 0, 0, "Risen Dead"),
(50374, 9, 0, "%s enrages and transforms into a Mindless Zombie and runs away.", "", 16, 0, 100, 0, 0, 0, "Risen Dead"),
(50374, 10, 0, "%s walks off towards the barracks.", "", 16, 0, 100, 0, 0, 0, "Risen Dead");

DELETE FROM `creature` WHERE `guid` IN (193857, 193848, 193854, 193855, 193850);
DELETE FROM `creature_addon` WHERE `guid` IN (193857, 193848, 193854, 193855, 193850);

DELETE FROM `waypoint_data` WHERE `id` IN (50374*100, 50374*100+01);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`) VALUES
(50374*100, 1, 1702, 1677.55, 134.298, 0, 0, 1),
(50374*100, 2, 1693.66, 1670.64, 134.032, 0, 0, 1),
(50374*100, 3, 1686.21, 1661.74, 133.171, 0, 0, 1),
(50374*100, 4, 1681.66, 1655.15, 136.316, 0, 0, 1),
(50374*100, 5, 1676.76, 1643.16, 139.253, 0, 0, 1),
(50374*100, 6, 1669.11, 1631.08, 139.495, 0, 0, 1),
(50374*100, 7, 1664.203, 1620.63, 140.526, 0, 0, 1),

(50374*100+01, 1, 1695.44, 1672.29, 133.945, 0, 0, 0),
(50374*100+01, 2, 1691.08, 1660.22, 131.406, 0, 0, 0),
(50374*100+01, 3, 1709.56, 1643.45, 124.713, 0, 0, 0),
(50374*100+01, 4, 1723.48, 1633.31, 120.426, 0, 0, 0);

UPDATE `creature_template` SET `ScriptName` = "npc_mindless_zombie" WHERE `entry` = 1501;
UPDATE `creature` SET `spawndist` = 5, `MovementType` = 1 WHERE `id` = 1501;

UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` = 20725;

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_undertaker_mordo" WHERE `entry` = 1568;

DELETE FROM `waypoint_data` WHERE `id` IN (50414*100, 50414*100+01);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `entry`) VALUES
(50414*100, 1, 1694.16, 1677.08, 134.786, 0, 0, 0, 50414),
(50414*100, 2, 1691.29, 1675.84, 135.295, 0, 0, 0, 50414),

(50414*100+01, 1, 1691.29, 1675.85, 135.295, 0, 0, 0, 50414),
(50414*100+01, 2, 1690.59, 1661.87, 131.92, 0, 0, 0, 50414),
(50414*100+01, 3, 1719.38, 1633.98, 121.065, 0, 0, 0, 50414);

DELETE FROM `creature_text` WHERE `entry` IN (1568, 50414);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES 
(1568, 0, 0, "Let's see, I just saw a corpse with a jaw that would fit you...", "", 12, 0, 100, 0, 0, 0, "Undertaker Mordo"),
(1568, 1, 0, "That should do the job. Come back right away if it falls off again.", "", 12, 0, 100, 0, 0, 0, "Undertaker Mordo"),

(50414, 0, 0, "T-thank you, Under-t-taker.", "", 12, 0, 100, 0, 0, 0, "Risen Recruit");

UPDATE `creature_template` SET `ScriptName` = "npc_darnell" WHERE `entry` = 49141;

DELETE FROM `creature_text` WHERE `entry` = 49141;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(49141, 0, 0, "Greetings, $n.", "", 12, 0, 100, 3, 0, 0, "Darnell"),
(49141, 1, 0, "The Shadow Grave is this way.  Follow me, $n.", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 2, 0, "This way!", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 3, 0, "Now, where could those supplies be?", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 4, 0, "Maybe they're over here?", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 5, 0, "Hmm...", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 6, 0, "No, not over here.", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 7, 0, "Hey, give me a hand, $n!  I can't find the supplies that Mordo needed!", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 8, 0, "Let's see now... where could they be...", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 9, 0, "Nice work!  You've found them.  Let's bring these back to Mordo.", "", 12, 0, 100, 1, 0, 0, "Darnell"),
(49141, 10, 0, "I saw someone up there whose jaw fell off.  I wonder if Mordo can fix him up?", "", 12, 0, 100, 1, 0, 0, "Darnell");

-- Marshal Redpath
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_marshal_redpath", `gossip_menu_id` = 12485 WHERE `entry` = 49230;
UPDATE `creature` SET `orientation` = 5.8555, `spawntimesecs` = 60 WHERE `id` = 49230;

DELETE FROM `creature_text` WHERE `entry` = 49230;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(49230, 0, 0, "BLEEAAAGGHHH! I'm a monster, don't look at me!", "", 12, 0, 100, 1, 0, 0, "Marshal Redpath"),
(49230, 0, 1, "Who are you calling a monster? You're the monster! I'm just a man who died.", "", 12, 0, 100, 1, 0, 0, "Marshal Redpath");

DELETE FROM `smart_scripts` WHERE `entryorguid` = 49230 AND `source_type` = 0;

DELETE FROM `waypoint_data` WHERE `id` = 49230*100;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`) VALUES
(49230*100, 1, 1752.72, 1626.94, 116.001, 0, 0, 1),
(49230*100, 2, 1758.17, 1604.42, 110.959, 0, 0, 1),
(49230*100, 3, 1756.88, 1585.49, 111.543, 0, 0, 1),
(49230*100, 4, 1751.61, 1564.8, 113.489, 0, 0, 1),
(49230*100, 5, 1752.27, 1548.56, 113.92, 0, 0, 1),
(49230*100, 6, 1763.14, 1538.02, 114.346, 0, 0, 1);

UPDATE `gossip_menu_option` SET `action_menu_id` = 12486 WHERE `menu_id` = 12485;
UPDATE `gossip_menu_option` SET `npc_option_npcflag` = 1 WHERE `menu_id` = 12486;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 12485;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15,12485,0,0,9,24960,0,0,0,"","Show gossip if player has quest: The wakening");

-- Lilian Voss
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_lilian_voss", `gossip_menu_id` = 12483 WHERE `entry` = 38895;
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `id` = 38895;

DELETE FROM `creature_text` WHERE `entry` = 38895;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(38895, 0, 0, "", "No. You're lying! My father will protect me!", 12, 0, 100, 1, 0, 0, "Lilian Voss"),
(38895, 0, 1, "", "You don't understand... I CAN'T be undead! Not me, not now...", 12, 0, 100, 22, 0, 0, "Lilian Voss");

DELETE FROM `creature_template_addon` WHERE `entry` = 38895;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`, `emote`) VALUES
(38895, 4097, 431);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 38895 AND `source_type` = 0;

DELETE FROM `waypoint_data` WHERE `id` = 38895*100;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`) VALUES
(38895*100, 1, 1752.21, 1652.93, 119.551, 0, 0, 1),
(38895*100, 2, 1752.72, 1635.44, 116.883, 0, 0, 1),
(38895*100, 3, 1755.51, 1611.9, 112.498, 0, 0, 1),
(38895*100, 4, 1756.03, 1591.45, 111.872, 0, 0, 1),
(38895*100, 5, 1755.69, 1570.22, 112.78, 0, 0, 1),
(38895*100, 6, 1751.78, 1550.02, 114.082, 0, 0, 1),
(38895*100, 7, 1762.13, 1539.01, 114.289, 0, 0, 1);

UPDATE `gossip_menu_option` SET `action_menu_id` = 12484 WHERE `menu_id` = 12483;
UPDATE `gossip_menu_option` SET `npc_option_npcflag` = 1 WHERE `menu_id` = 12484;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 12483;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15,12483,0,0,9,24960,0,0,0,"","Show gossip if player has quest: The wakening");

-- Valdred Moray
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_valdred_moray", `gossip_menu_id` = 12487 WHERE `entry` = 49231;
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `id` = 49231;

DELETE FROM `creature_text` WHERE `entry` = 49231;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(49231, 0, 0, "Valdred Moray, reporting for duty, sir!", "", 14, 0, 100, 66, 0, 0, "Valdred Moray"),
(49231, 0, 1, "I see. Well then, let's get to work, $n! The Dark Lady needs us, right?", "", 12, 0, 100, 1, 0, 0, "Valdred Moray");

DELETE FROM `creature_template_addon` WHERE `entry` = 49231;
INSERT INTO `creature_template_addon` (`entry`, `bytes1`, `bytes2`) VALUES
(49231, 8, 4097);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 49231 AND `source_type` = 0;

DELETE FROM `waypoint_data` WHERE `id` = 49231*100;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`) VALUES
(49231*100, 1, 1728.97, 1696.68, 127.857, 0, 0, 0),
(49231*100, 2, 1716.91, 1690.64, 132.728, 0, 0, 0),
(49231*100, 3, 1707.91, 1685.12, 133.644, 0, 0, 0),
(49231*100, 4, 1695.44, 1677.7, 134.632, 0, 0, 0);

UPDATE `gossip_menu_option` SET `action_menu_id` = 12488 WHERE `menu_id` = 12487;
UPDATE `gossip_menu_option` SET `action_menu_id` = 12489 WHERE `menu_id` = 12488;
UPDATE `gossip_menu_option` SET `npc_option_npcflag` = 1 WHERE `menu_id` IN (12488, 12489);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 12487;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15,12487,0,0,9,24960,0,0,0,"","Show gossip if player has quest: The wakening");

UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_vile_fin_puddlejumper" WHERE `entry` = 1543;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_vile_fin_minor_oracle" WHERE `entry` = 1544;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_sedrick_calston" WHERE `entry` = 38925;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_captured_vile_fin_puddlejumper" WHERE `entry` = 38923;
UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_captured_vile_fin_minor_oracle" WHERE `entry` = 39078;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (1543, 1544) AND `source_type` = 0;
DELETE FROM `spell_script_names` WHERE `spell_id` = 73108;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(73108, "spell_murloc_leash");

DELETE FROM `creature_text` WHERE `entry` IN (1543, 1544);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(1543, 0, 0, "The %s is ready to be captured!", "", 16, 0, 100, 0, 0, 0, "Vile Fin Puddlejumper"),

(1544, 0, 0, "The %s is ready to be captured!", "", 16, 0, 100, 0, 0, 0, "Vile Fin Minor Oracle");
