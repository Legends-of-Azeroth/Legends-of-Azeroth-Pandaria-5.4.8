DELETE FROM `creature` WHERE `guid` BETWEEN 10000 AND 10012;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `movement_type`) VALUES
(10000, 63331, 1, 1, 1, 10424.83, 765.6614, 1322.754, 1.998563, 120, 0, 0),
(10001, 12160, 1, 1, 1, 10313.05, 816.032, 1327.698, 3.972167, 120, 10, 1),
(10002, 12160, 1, 1, 1, 10413.19, 731.5011, 1321.063, 5.983846, 120, 10, 1),
(10003, 12160, 1, 1, 1, 10348.853, 751.136, 1325.35, 5.057269, 120, 10, 1),
(10004, 62242, 1, 1, 1, 10626.83, 927.5742, 1327.541, 2.04279, 120, 15, 1),
(10005, 62242, 1, 1, 1, 10625.32, 821.9661, 1316.752, 5.311656, 120, 15, 1),
(10006, 62242, 1, 1, 1, 10575.35, 963.194, 1325.54, 4.892617, 120, 15, 1),
(10007, 62242, 1, 1, 1, 10423.52, 919.1991, 1324.547, 5.583723, 120, 15, 1),
(10008, 62242, 1, 1, 1, 10283.21, 854.236, 1341.771, 4.222265, 120, 15, 1),
(10009, 61757, 1, 1, 1, 10690.396, 669.625, 1335.091, 5.874082, 120, 15, 1),
(10010, 2079, 1, 1, 1, 10312.7, 830.055, 1326.52, 5.41052, 120, 0, 0),
(10012, 2079, 1, 1, 1, 10312.7, 830.122, 1326.53, 2.37365, 120, 0, 0);

UPDATE `creature` SET `position_x` = 10322.9, `position_y` = 820.333, `position_z` = 1326.23, `orientation` = 5.55015 WHERE `guid` = 276859 AND `id` = 2079;
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (302852, 302853);

DELETE FROM `creature_template_addon` WHERE `entry` = 2079;
DELETE FROM `creature_addon` WHERE `guid` IN (276859, 10010, 10012);
INSERT INTO `creature_addon` (`guid`, `bytes2`, `auras`) VALUES
(276859, 1, "60921"), -- Generic Quest Invisibility 3
(10010, 1, "49414"), -- Generic Quest Invisibility 1
(10012, 1, "49415"); -- Generic Quest Invisibility 2

DELETE FROM `spell_area` WHERE `spell` IN (49417, 49416, 60922, 92549, 94566) AND `area` = 188;
DELETE FROM `spell_area` WHERE `spell` = 92237 AND `area` = 257;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(49417, 188, 0, 28713, 0, 0, 2, 1, 0, 41), -- Generic Quest Invisibility Detection 2
(49416, 188, 28713, 28714, 0, 0, 2, 1, 66, 41), -- Generic Quest Invisibility Detection 1
(60922, 188, 28714, 0, 0, 0, 2, 1, 66, 0), -- Generic Quest Invisibility Detection 3
(92549, 188, 0, 28727, 0, 0, 2, 1, 0, 43), -- See Quest Invis 4                           (?)
(94566, 188, 28727, 0, 0, 0, 2, 1, 64, 0), -- Generic Quest Invisibility Detection 5      (?)
(92237, 257, 28725, 28728, -92239, 0, 2, 1, 74, 41); -- Summon Tarindrella Aura           (?)

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (2079, 49478, 34756, 34757);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (2079, 49478, 34756, 34757, 2077) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (2079*100, 49478*100, 34757*100) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(2079,0,0,0,20,0,100,0,28713,0,0,0,80,2079*100,2,0,0,0,0,1,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 28713) Rewarded - Start Script"),
(2079,0,1,0,19,0,100,0,3118,0,0,0,1,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 3118) Accepted - Say Text Line 1"),
(2079,0,2,0,19,0,100,0,3117,0,0,0,1,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 3117) Accepted - Say Text Line 1"),
(2079,0,3,0,19,0,100,0,26841,0,0,0,1,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 26841) Accepted - Say Text Line 1"),
(2079,0,4,0,19,0,100,0,3119,0,0,0,1,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 3119) Accepted - Say Text Line 1"),
(2079,0,5,0,19,0,100,0,3116,0,0,0,1,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 3116) Accepted - Say Text Line 1"),
(2079,0,6,0,19,0,100,0,3120,0,0,0,1,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Quest (ID: 3120) Accepted - Say Text Line 1"),

(2079*100,9,0,0,0,0,100,0,0,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Script - Say Text Line 0"),
(2079*100,9,1,0,0,0,100,0,0,0,0,0,64,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Ilthalaine - On Script - Store Target (Invoker)"),
(2079*100,9,2,0,0,0,100,0,6000,6000,0,0,1,0,0,0,0,0,0,19,49477,10,0,0,0,0,0,"Ilthalaine - On Script - Say Text Line 0 (Creature ID: 49477)"),
(2079*100,9,3,0,0,0,100,0,1000,1000,0,0,100,1,0,0,0,0,0,19,49478,10,0,0,0,0,0,"Ilthalaine - On Script - Send Target (Creature ID: 49478)"),
(2079*100,9,4,0,0,0,100,0,6000,6000,0,0,45,1,1,0,0,0,0,19,49478,10,0,0,0,0,0,"Ilthalaine - On Script - Set Data 1 1 (Creature ID: 49478)"),

(49478,0,0,0,38,0,100,0,1,1,0,0,80,49478*100,2,0,0,0,0,1,0,0,0,0,0,0,0,"Dentaria Silverglade - On Data Set 1 1 - Start Script"),

(49478*100,9,0,0,0,0,100,0,0,0,0,0,1,0,0,0,0,0,0,12,1,0,0,0,0,0,0,"Dentaria Silverglade - On Script - Say Text Line 0"),
(49478*100,9,1,0,0,0,100,0,6000,6000,0,0,1,1,0,0,0,0,0,12,1,0,0,0,0,0,0,"Dentaria Silverglade - On Script - Say Text Line 1"),

(34757,0,0,0,10,0,100,0,1,25,25000,25000,80,34757*100,0,0,0,0,0,1,0,0,0,0,0,0,0,"Doranel Amberleaf - OOC LOS - Start Script"),

(34757*100,9,0,0,0,0,100,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Doranel Amberleaf - On Script - Say Text Line 0"),
(34757*100,9,1,0,0,0,100,0,4000,4000,0,0,1,0,0,0,0,0,0,19,34756,15,0,0,0,0,0,"Doranel Amberleaf - On Script - Say Text Line 0 (Creature ID: 34756)"),
(34757*100,9,2,0,0,0,100,0,2000,2000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,2.795327,"Doranel Amberleaf - On Script - Set Orientation (2.795327)"),
(34757*100,9,3,0,0,0,100,0,5000,5000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,5.3058,"Doranel Amberleaf - On Script - Set Orientation (5.3058)"),

(2077,0,0,0,1,0,70,0,2000,4000,7000,9000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Melithar Forteramure - OOC - Play Emote Talk"),
(2077,0,1,0,19,0,100,0,3118,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (3118) - Say Text Line 0"),
(2077,0,2,0,19,0,100,0,3117,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (3117) - Say Text Line 0"),
(2077,0,3,0,19,0,100,0,26841,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (26841) - Say Text Line 0"),
(2077,0,4,0,19,0,100,0,3119,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (3119) - Say Text Line 0"),
(2077,0,5,0,19,0,100,0,3116,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (3116) - Say Text Line 0"),
(2077,0,6,0,19,0,100,0,3120,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (3120) - Say Text Line 0"),
(2077,0,7,0,19,0,100,0,31168,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"Melithar Forteramure - On Quest Accept (31168) - Say Text Line 0");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceGroup`= 1 AND `SourceEntry` = 34757;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(22,1,34757,0,31,0,4,0,0,0,0,"","SAI event require a player");

DELETE FROM `creature_text` WHERE `entry` IN (2079, 49478, 34756, 34757, 2077);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text_female`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(2079,0,0,"I'm sorry, Huntress. None of our new $cs are ready. They still have much to learn.","",12,0,100,1,0,0,"Ilthalaine"),
(2079,1,0,"You'll find the trainer inside of Aldrassil, $n.","",12,0,100,1,0,0,"Ilthalaine"),

(49478,0,0,"","We will leave you to your training, Ilthalaine. This is your area of expertise.",12,0,100,1,0,0,"Dentaria Silverglade"),
(49478,1,0,"","Study well, $n. We may need you fighting by our side before the next moon fills.",12,0,100,1,0,0,"Dentaria Silverglade"),

(34756,0,0,"","Shh! Someone's here.",12,0,100,1,0,0,"Moriana Dawnlight"),

(34757,0,0,"Don't get me wrong... I'm grateful to have Shando Stormrage back, but this is all really suspicious. What happened to Fandral?","",12,0,100,6,0,0,"Doranel Amberleaf"),

(2077,0,0,"You'll find the trainer inside of Aldrassil, $n.","",12,0,100,1,0,0,"Melithar Staghelm");

UPDATE `creature_template` SET `npcflag` = 0, `gossip_menu_id` = 0 WHERE `entry` = 8583;

UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 2 WHERE `guid` IN (277063, 276892, 276628, 277061, 276632);
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `id` IN (34756, 34757);

DELETE FROM `creature_addon` WHERE `guid` IN (277063, 276892, 276628, 277061, 276632);
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`, `auras`) VALUES
(277063, 277063, 1, "18950"),
(276892, 276892, 1, "18950"),
(276628, 276628, 1, "18950"),
(277061, 277061, 1, "18950"),
(276632, 276632, 1, "18950");

DELETE FROM `waypoint_data` WHERE `id` IN (277063, 276892, 276628, 277061, 276632);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(277063, 1, 10397.1, 883.697, 1321.38, 0, 0, 0, 0, 100, 0, 12160),
(277063, 2, 10365.1, 866.052, 1324.96, 0, 0, 0, 0, 100, 0, 12160),
(277063, 3, 10355.6, 852.585, 1325.15, 0, 0, 0, 0, 100, 0, 12160),
(277063, 4, 10323.4, 827.04, 1326.37, 0, 0, 0, 0, 100, 0, 12160),
(277063, 5, 10368, 747.071, 1321.5, 0, 0, 0, 0, 100, 0, 12160),
(277063, 6, 10323.4, 827.04, 1326.37, 0, 0, 0, 0, 100, 0, 12160),
(277063, 7, 10353.7, 850.765, 1325.19, 0, 0, 0, 0, 100, 0, 12160),
(277063, 8, 10366.6, 867.413, 1324.95, 0, 0, 0, 0, 100, 0, 12160),
(277063, 9, 10383.3, 875.205, 1323.82, 0, 0, 0, 0, 100, 0, 12160),
(277063, 10, 10395.7, 884.819, 1321.46, 0, 0, 0, 0, 100, 0, 12160),
(277063, 11, 10417.3, 886.186, 1318.98, 0, 0, 0, 0, 100, 0, 12160),

(276892, 1, 10502.5, 835.888, 1361.22, 0, 0, 0, 0, 100, 0, 12160),
(276892, 2, 10512.7, 820.212, 1354.79, 0, 0, 0, 0, 100, 0, 12160),
(276892, 3, 10520.4, 825.945, 1354.79, 0, 0, 0, 0, 100, 0, 12160),
(276892, 4, 10510.2, 848.159, 1348.96, 0, 0, 0, 0, 100, 0, 12160),
(276892, 5, 10488.9, 856.99, 1342.79, 0, 0, 0, 0, 100, 0, 12160),
(276892, 6, 10462.4, 858.067, 1339, 0, 0, 0, 0, 100, 0, 12160),
(276892, 7, 10419.6, 885.502, 1319.2, 0, 0, 0, 0, 100, 0, 12160),
(276892, 8, 10460.5, 858.739, 1338.22, 0, 0, 0, 0, 100, 0, 12160),
(276892, 9, 10497.2, 855.959, 1344.87, 0, 0, 0, 0, 100, 0, 12160),
(276892, 10, 10514.3, 843.018, 1350.85, 0, 0, 0, 0, 100, 0, 12160),
(276892, 11, 10521.8, 823.973, 1354.79, 0, 0, 0, 0, 100, 0, 12160),
(276892, 12, 10513.1, 821.36, 1354.79, 0, 0, 0, 0, 100, 0, 12160),
(276892, 13, 10502.5, 835.888, 1361.22, 0, 0, 0, 0, 100, 0, 12160),
(276892, 14, 10492.3, 845.633, 1366.7, 0, 0, 0, 0, 100, 0, 12160),
(276892, 15, 10469.2, 849.412, 1375.61, 0, 0, 0, 0, 100, 0, 12160),
(276892, 16, 10450.9, 843.149, 1380.85, 0, 0, 0, 0, 100, 0, 12160),
(276892, 17, 10483.7, 849.269, 1370.22, 0, 0, 0, 0, 100, 0, 12160),

(276628, 1, 10501.6, 787.122, 1397.8, 0, 0, 0, 0, 100, 0, 12160),
(276628, 2, 10484.2, 786.734, 1395.33, 0, 0, 0, 0, 100, 0, 12160),
(276628, 3, 10461.4, 798.532, 1388.93, 0, 0, 0, 0, 100, 0, 12160),
(276628, 4, 10444.6, 821.521, 1382.36, 0, 0, 0, 0, 100, 0, 12160),
(276628, 5, 10444.9, 834.345, 1380.91, 0, 0, 0, 0, 100, 0, 12160),
(276628, 6, 10449.3, 841.089, 1381.17, 0, 0, 0, 0, 100, 0, 12160),
(276628, 7, 10444.9, 834.345, 1380.91, 0, 0, 0, 0, 100, 0, 12160),
(276628, 8, 10444.6, 821.521, 1382.36, 0, 0, 0, 0, 100, 0, 12160),
(276628, 9, 10461.4, 798.532, 1388.93, 0, 0, 0, 0, 100, 0, 12160),
(276628, 10, 10484.2, 786.734, 1395.33, 0, 0, 0, 0, 100, 0, 12160),
(276628, 11, 10501.6, 787.122, 1397.8, 0, 0, 0, 0, 100, 0, 12160),

(277061, 1, 10484.9, 800.648, 1326.81, 0, 0, 0, 0, 100, 0, 12160),
(277061, 2, 10495.4, 797.863, 1328.54, 0, 0, 0, 0, 100, 0, 12160),
(277061, 3, 10504.5, 796.76, 1330.43, 0, 0, 0, 0, 100, 0, 12160),
(277061, 4, 10522.4, 782.14, 1329.6, 0, 0, 0, 0, 100, 0, 12160),
(277061, 5, 10502.7, 796.839, 1330.35, 0, 0, 0, 0, 100, 0, 12160),
(277061, 6, 10490.3, 799.39, 1326.82, 0, 0, 0, 0, 100, 0, 12160),
(277061, 7, 10484.9, 800.648, 1326.81, 0, 0, 0, 0, 100, 0, 12160),
(277061, 8, 10473.1, 799.122, 1322.74, 0, 0, 0, 0, 100, 0, 12160),
(277061, 9, 10460.3, 799.294, 1322.59, 0, 0, 0, 0, 100, 0, 12160),
(277061, 10, 10402.6, 764.818, 1322.69, 0, 0, 0, 0, 100, 0, 12160),
(277061, 11, 10461.5, 799.607, 1322.67, 0, 0, 0, 0, 100, 0, 12160),
(277061, 12, 10476.7, 799.978, 1324.27, 0, 0, 0, 0, 100, 0, 12160),
(277061, 13, 10484.9, 800.648, 1326.81, 0, 0, 0, 0, 100, 0, 12160),

(276632, 1, 10427.9, 771.281, 1335.5, 0, 0, 0, 0, 100, 0, 12160),
(276632, 2, 10435.2, 776.668, 1337.29, 0, 0, 0, 0, 100, 0, 12160),
(276632, 3, 10428.3, 786.768, 1337.29, 0, 0, 0, 0, 100, 0, 12160),
(276632, 4, 10442.5, 797.24, 1345.63, 0, 0, 0, 0, 100, 0, 12160),
(276632, 5, 10452.2, 797.576, 1345.65, 0, 0, 0, 0, 100, 0, 12160),
(276632, 6, 10442.4, 796.852, 1345.63, 0, 0, 0, 0, 100, 0, 12160),
(276632, 7, 10428.6, 787.004, 1337.29, 0, 0, 0, 0, 100, 0, 12160),
(276632, 8, 10434.9, 776.611, 1337.29, 0, 0, 0, 0, 100, 0, 12160),
(276632, 9, 10421.2, 767.289, 1331.54, 0, 0, 0, 0, 100, 0, 12160),
(276632, 10, 10404.1, 759.106, 1322.68, 0, 0, 0, 0, 100, 0, 12160),
(276632, 11, 10418.7, 765.651, 1329.76, 0, 0, 0, 0, 100, 0, 12160);

UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` IN (48624, 48623, 62242);
UPDATE `creature_template` SET `RegenHealth` = 0 WHERE `entry` = 44617;
UPDATE `creature` SET `curhealth` = 650 WHERE `id` = 44617;

DELETE FROM `creature` WHERE `guid` = 308260 AND `id` = 14432;
DELETE FROM `creature` WHERE `guid` = 302860 AND `id` = 14429;
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 15 WHERE `id` IN (1984, 2031, 1989, 2032, 1985, 1986, 1988);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 7 WHERE `id` IN (2042, 1998, 1995, 2022, 2007, 2006, 2008, 1996, 1999, 2043, 1997, 2000, 2001, 2034, 7235);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 5 WHERE guid IN (277643, 278482, 278481, 278478, 278219, 278185, 278183, 278080, 278063, 278046, 278483);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 5 WHERE `id` IN (2029, 2030);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 3 WHERE `id` IN (2003, 2002, 2004, 2005, 2017, 2015, 2018, 2019, 2020, 2021, 2010, 2011, 2009, 2152);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 2 WHERE `id` IN (7319, 2162, 1993);

DELETE FROM `creature_addon` WHERE `guid` IN (277954, 277611, 278587, 278301, 277866, 278300, 278374, 278023, 277994, 277906, 277903, 277828, 277667, 277540,
                                              277995, 278386, 277993, 277830, 277827, 277542, 277537, 277502, 277385, 278388, 277384, 278389, 278379, 278317,
                                              277536, 277997, 277996, 277992, 277539, 277541, 277798, 277668, 277700, 278641, 278635, 278616, 278612, 278611,
                                              278309, 278643, 278583, 278546, 278652, 277943, 277947, 278057, 278520, 278607, 278154, 278527, 278549, 278358,
                                              277711, 278458, 277822, 278505, 277705, 278162, 278210, 278472, 277723, 278676, 278673, 278138, 278787, 278781,
                                              278776, 278743, 278804, 278044, 278745, 278809);
DELETE FROM `waypoint_data` WHERE `id` IN (277954, 277611, 278587, 278301, 277866, 278300, 278374, 278023, 277994, 277906, 277903, 277828, 277667, 277540,
                                           277995, 278386, 277993, 277830, 277827, 277542, 277537, 277502, 277385, 278388, 277384, 278389, 278379, 278317,
                                           277536, 277997, 277996, 277992, 277539, 277541, 277798, 277668, 277700, 278641, 278635, 278616, 278612, 278611,
                                           278309, 278643, 278583, 278546, 278652, 277943, 277947, 278057, 278520, 278607, 278154, 278527, 278549, 278358,
                                           277711, 278458, 277822, 278505, 277705, 278162, 278210, 278472, 277723, 278676, 278673, 278138, 278787, 278781,
                                           278776, 278743, 278804, 278044, 278745, 278809);
