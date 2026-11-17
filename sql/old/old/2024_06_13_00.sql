-- Fe-Feng Hozen SAI, Thanks Kaliga
SET @ENTRY := 54131;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,11,0,100,0,0,0,0,0,8,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Fe-Feng Hozen - On Respawn - Set React State"),
(@ENTRY,0,1,0,4,0,100,0,0,0,0,0,41,0,0,0,0,0,0,11,56739,1,0,0,0,0,0,"Fe-Feng Hozen - On Aggro - Despawn Vehicle"),
(@ENTRY,0,2,0,7,0,100,0,0,0,0,0,41,2000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Fe-Feng Hozen - On Evade - Despawn"),
(@ENTRY,0,3,0,0,0,100,0,2000,8000,4000,8000,11,121484,0,0,0,0,0,1,0,0,0,0,0,0,0,"Fe-Feng Hozen - IC - Cast Ook'em"),
(@ENTRY,0,4,0,0,0,50,0,5000,10000,5000,10000,11,115006,0,0,0,0,0,1,0,0,0,0,0,0,0,"Fe-Feng Hozen - IC - Cast Hozen Rage");

-- This line maybe cause some error , missing model id , flags extra mismatch
-- DELETE FROM `creature_template` WHERE `id`=56739;
-- INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `name`, `femaleName`, `subname`, `TitleAlt`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `HealthScalingExpansion`, `RequiredExpansion`, `VignetteID`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `FadeRegionRadius`, `WidgetSetID`, `WidgetSetUnitConditionID`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES 
-- (56739,0,0,0,0,0,'Hozen Hanging Bunny','','',NULL,'','0','1','1','4','0','0','35','16777216','1','1.14286','1','0','0','2000','2000','1','1','1','33555200','0','0','0','0','0','0','0','10','1048576','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','1898','0','0','','0','4','1','1','1','1','1','1','1','1','0','0','0','0','0','1','0','0','','35662');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=56739;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(56739, 46598,'0','0');

-- fix item drop tooltip for quest 24859 - Naga Hide, Thanks jasondove
SET @ENTRY := 38359; -- Vashj'elan Warrior
SET @ITEM := 50437; -- Intact Naga Hide
UPDATE `creature_template` SET `questItem1`=@ITEM WHERE `entry`=@ENTRY;
