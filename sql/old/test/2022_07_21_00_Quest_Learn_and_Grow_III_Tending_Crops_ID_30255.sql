
UPDATE `creature_template` SET `npcflag` = 16777216 WHERE `entry` = 59987;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=59987;
INSERT INTO `npc_spellclick_spells`(`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES (59987, 111108, 1, 0);

-- Parched EZ-Gro Green Cabbage SAI
SET @ENTRY := 59987;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,73,0,100,1,0,0,0,0,28,115824,0,0,0,0,0,1,0,0,0,0,0,0,0,"On SpellClick Remove Aura "),
(@ENTRY,0,1,0,73,0,100,1,0,0,0,0,33,59987,0,0,0,0,0,7,0,0,0,0,0,0,0,"On SpellClick Kill Credit"),
(@ENTRY,0,2,0,73,0,100,0,0,0,0,0,41,10000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Despawn 10s");

-- Farmer Yoon SAI
SET @ENTRY := 58646;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,1,8,0,100,0,126987,0,0,0,33,64942,0,0,0,0,0,7,0,0,0,0,0,0,0,"On spellhit 126987 - kill credit 64942"),
(@ENTRY,0,1,0,61,0,100,0,0,0,0,0,1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,"On spellhit 126987 - talk 0"),
(@ENTRY,0,2,0,64,0,100,0,0,0,0,0,33,70454,0,0,0,0,0,17,0,100,0,0,0,0,0,"[Kill]OnSpeak"),
(@ENTRY,0,3,4,19,0,100,0,30254,0,0,0,33,59985,2,2,0,0,0,7,0,0,0,0,0,0,0,""),
(@ENTRY,0,4,0,61,0,100,0,0,0,0,0,33,59990,2,2,0,0,0,7,0,0,0,0,0,0,0,""),
(@ENTRY,0,5,0,19,0,100,0,30255,0,0,0,12,59987,7,0,0,0,0,8,0,0,0,-161.232,637.367,165.409,0,"On Quest Accept Summon Green Cabbage"),
(@ENTRY,0,6,0,19,0,100,0,30255,0,0,0,12,59987,7,0,0,0,0,8,0,0,0,-161.734,641.861,165.409,0,"On Quest Accept Summon Green Cabbage");

UPDATE `quest_template` SET `RequiredSourceItemId1` = 79104 WHERE `Id` = 30255;
UPDATE `quest_template` SET `RequiredSourceItemCount1` = 1 WHERE `Id` = 30255;

DELETE FROM conditions WHERE SourceTypeOrReferenceId IN (18) AND SourceGroup IN (59987) AND `SourceEntry` IN (111108);
INSERT INTO `conditions`(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 59987, 111108, 0, 0, 2, 0, 79104, 1, 0, 0, 0, 0, '', 'Spellclick Only Work With Item 79104');
