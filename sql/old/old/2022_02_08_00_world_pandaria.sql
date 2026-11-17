UPDATE `creature_template` SET `InhabitType` = 7 WHERE `entry` = 54567;
UPDATE `creature_template` SET `minlevel` = 2, `maxlevel` = 2, `faction_A` = 2102, `faction_H` = 2102, `npcflag` = 0, `speed_walk` = 1, `speed_run` = 1.142857, `baseattacktime` = 2000, `rangeattacktime` = 2000, `unit_class` = 0, `unit_flags` = 32768, `unit_flags2` = 2048, `dynamicflags` = 0, `VehicleId` = 0, `HoverHeight` = 1 WHERE `entry` = 59637;

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` ="" WHERE `entry` = 55694;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 55694 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(55694,0,0,0,54,0,100,0,0,0,0,0,62,860,0,0,0,0,0,1,0,0,0,909.137,3610.38,252.175,0,"Ji Firepaw - On Summoned - Teleport Creature"),
(55694,0,1,0,10,0,100,0,1,10,12000,12000,1,0,2000,0,0,0,0,21,10,0,0,0,0,0,0,"Ji Firepaw - OOC LoS - Say Text Line 0"),
(55694,0,2,0,52,0,100,0,0,55694,0,0,53,1,55694,0,0,0,0,1,0,0,0,0,0,0,0,"Ji Firepaw - On Text Over 0 - Waypoint Start"),
(55694,0,3,0,58,0,100,0,41,55694,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Ji Firepaw - On Waypoint Ended - Despawn Self");

DELETE FROM `waypoints` WHERE `pointid` IN (35, 36, 37, 38, 39, 40, 41) AND `entry` = 55694;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
(55694, 35, 1008.55, 4153.77, 202.52, "Ji Firepaw"),
(55694, 36, 1035.22, 4153.14, 205.30, "Ji Firepaw"),
(55694, 37, 1048.20, 4176.42, 204.94, "Ji Firepaw"),
(55694, 38, 1078.19, 4169.25, 205.67, "Ji Firepaw"),
(55694, 39, 1103.67, 4172.46, 197.90, "Ji Firepaw"),
(55694, 40, 1104.01, 4173.42, 197.73, "Ji Firepaw"),
(55694, 41, 1131.81, 4151.84, 190.62, "Ji Firepaw");

UPDATE `creature` SET `spawntimesecs` = 300 WHERE `id` = 55633 AND `zoneid` = 5736; 
UPDATE `creature` SET `spawntimesecs` = 300 WHERE `id` = 57466 AND `zoneid` = 5736; 
UPDATE `creature` SET `spawntimesecs` = 300 WHERE `id` = 55632 AND `zoneid` = 5736; 
