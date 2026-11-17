/*ida a las zonas cataclysmo ambas facciones con misiones de spawn de portales*/
DELETE FROM `object_visibility_state`;
INSERT INTO `object_visibility_state` (`type`, `entryorguid`, `visibilityQuestID`, `visibilityQuestState`) VALUES
("GameObject", "207688", "25316", "6"), /*monte hijal*/
("GameObject", "207692", "25316", "6"), /*monte hijal*/
("GameObject", "207693", "27123", "6"), /*infralar alianza*/
("GameObject", "207689", "27123", "6"), /*infralar horda*/
("GameObject", "200005", "27722", "6"), /*Maelstrom horda*/
("GameObject", "205268", "27727", "6"), /*Maelstrom alianza*/
("GameObject", "207695", "28558", "6"), /*uldum alianza*/
("GameObject", "207687", "28557", "6"), /*uldum horda*/
("GameObject", "207686", "28717", "6"), /*tierras altas horda*/
("GameObject", "207694", "28716", "6"), /*tierras altas alianza*/
("GameObject", "207691", "14482", "6"), /*vashjir alianza*/
("GameObject", "207690", "25924", "6"); /*vashjir horda*/

/* portal custom maelstrom */
DELETE FROM `gameobject_template` WHERE `entry` = 200005;
INSERT INTO `gameobject_template`(`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `unkInt32`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES
("200005", "22", "5231", "Portal to the Maelstrom", "", "", "", "2", "0", "0", "0", "0", "0", "0", "84464", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "", "", "15595");
UPDATE `gameobject` SET `id` = 200005 WHERE `guid` = 9954532;

/*delete spawn npc*/
DELETE FROM `creature` WHERE `guid` in (372275,316786,316785,372514,315882,315854,315851,315850,316424,315824,315825);

/*npc 45005 efecto visual*/
UPDATE `creature_template` SET `MovementType` = 1, `InhabitType` = 7 WHERE `entry` = 45005;

UPDATE `creature_template` SET `ScriptName` = "npc_deepholm_wyvern" WHERE `entry` = 45005;
UPDATE `creature_template` SET `ScriptName` = "npc_maelstorm_aggra" WHERE `entry` = 45006;
UPDATE `creature_template` SET `ScriptName` = "npc_deepholm_wyvern_ride" WHERE `entry` = 45024;
