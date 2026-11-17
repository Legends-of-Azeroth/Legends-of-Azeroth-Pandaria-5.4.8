/*World of Warcraft Quest Guide Great Vessel of Salvation  ID 31082*/

/*
ID - 122153 Apoyo de Rikkitun
ID - 122165 Levitar semilla madre
ID - 122162 Motherseed Visual Aura
ID - 122171 Canto Rikkitun
*/

UPDATE `creature_template` SET `npcflag` = 1 WHERE `entry` = 62601;
UPDATE `creature_template` SET `unit_flags` = 33288 WHERE `entry` = 62587;

-- Motherseed SAI
SET @ENTRY := 62601;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,1000,1000,1000,1000,59,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Always Run"),
(@ENTRY,0,1,0,64,0,100,0,0,0,0,0,45,0,1,0,0,0,0,11,62587,20,0,0,0,0,0,"Set Data On Gossip Hello"),
(@ENTRY,0,2,0,8,0,100,1,122171,0,0,0,33,62601,0,0,0,0,0,21,10,0,0,0,0,0,0,"Kill Credit"),
(@ENTRY,0,3,0,8,0,100,0,122171,0,0,0,83,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Remove Flag"),
(@ENTRY,0,4,0,8,0,100,0,122171,0,0,0,69,0,0,0,0,0,0,8,0,0,0,796.15,4166.28,166.21,0,"Move To Position On Spell Hit"),
(@ENTRY,0,5,0,8,0,100,0,122171,0,0,0,41,30000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Despawn On 30s");


-- Rikkitun Chanter SAI
SET @ENTRY := 62587;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,1000,1000,1000,1000,59,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Always Run"),
(@ENTRY,0,1,0,38,0,100,0,0,1,0,0,11,122171,0,0,0,0,0,11,62601,20,0,0,0,0,0,"On Data Set Cast Spell"),
(@ENTRY,0,2,0,38,0,100,0,0,1,0,0,69,0,0,0,0,0,0,8,0,0,0,796.15,4166.28,166.21,0,"On Data Set Move To Position"),
(@ENTRY,0,3,0,1,0,100,0,1000,1000,0,0,41,60000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Despawn On 30s");












