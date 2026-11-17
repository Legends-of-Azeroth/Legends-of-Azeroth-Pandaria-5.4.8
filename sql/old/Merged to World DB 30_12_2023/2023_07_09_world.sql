UPDATE `creature` SET `spawntimesecs`=120 WHERE `id` IN(34907, 34947);
DELETE FROM `creature_text` WHERE `creatureid` IN(34907, 34947);
INSERT INTO `creature_text` (`creatureid`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `BroadcastTextId`, `comment`) VALUES
(34907, 0, 0, 'I shall draw your soul into the mist!', 12, 0, 100, 0, 0, 0, 24846, 'Kvaldir Harpooner'),
(34907, 0, 1, 'YAAARRRGH!', 12, 0, 100, 0, 0, 0, 24847, 'Kvaldir Harpooner'),
(34907, 0, 2, 'Be consumed by the mist!', 12, 0, 100, 0, 0, 0, 24848, 'Kvaldir Harpooner'),
(34907, 0, 3, 'Look what''s lost its way...', 12, 0, 100, 0, 0, 0, 24849, 'Kvaldir Harpooner'),
(34907, 0, 4, 'I will heave your carcass into the ocean myself!', 12, 0, 100, 0, 0, 0, 24850, 'Kvaldir Harpooner'),
(34907, 0, 5, 'You''re not even fit to be fed to Leviroth.', 12, 0, 100, 0, 0, 0, 24851, 'Kvaldir Harpooner'),
(34907, 0, 6, 'You''ve strayed too far from your comfortable soil, $N.', 12, 0, 100, 0, 0, 0, 24852, 'Kvaldir Harpooner'),
(34947, 0, 0, 'I shall draw your soul into the mist!', 12, 0, 100, 0, 0, 0, 24846, 'Kvaldir Berserker'),
(34947, 0, 1, 'YAAARRRGH!', 12, 0, 100, 0, 0, 0, 24847, 'Kvaldir Berserker'),
(34947, 0, 2, 'Be consumed by the mist!', 12, 0, 100, 0, 0, 0, 24848, 'Kvaldir Berserker'),
(34947, 0, 3, 'Look what''s lost its way...', 12, 0, 100, 0, 0, 0, 24849, 'Kvaldir Berserker'),
(34947, 0, 4, 'I will heave your carcass into the ocean myself!', 12, 0, 100, 0, 0, 0, 24850, 'Kvaldir Berserker'),
(34947, 0, 5, 'You''re not even fit to be fed to Leviroth.', 12, 0, 100, 0, 0, 0, 24851, 'Kvaldir Berserker'),
(34947, 0, 6, 'You''ve strayed too far from your comfortable soil, $N.', 12, 0, 100, 0, 0, 0, 24852, 'Kvaldir Berserker');

-- Kvaldir Harpooner
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 34907;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 34907 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(34907, 0, 0, 0, 1, 0, 100, 0, 0, 0, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 11, 35098, 30, 0, 0, 0, 0, 0, 'Kvaldir Harpooner - Out Of Combat - Attack Start'),
(34907, 0, 1, 0, 1, 0, 100, 0, 0, 0, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 11, 35070, 30, 0, 0, 0, 0, 0, 'Kvaldir Harpooner - Out Of Combat - Attack Start'),
(34907, 0, 2, 0, 0, 0, 100, 0, 0, 0, 2300, 3900, 11, 66489, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Kvaldir Harpooner - In Combat - Cast Spear Throw'),
(34907, 0, 3, 0, 0, 0, 20, 1, 5000, 5000, 0, 0, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Kvaldir Harpooner - In Combat - Say Line 0');

-- Kvaldir Berserker
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 34947;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 34947 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(34947, 0, 0, 0, 1, 0, 100, 0, 0, 0, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 11, 35098, 30, 0, 0, 0, 0, 0, 'Kvaldir Harpooner - Out Of Combat - Attack Start'),
(34947, 0, 1, 0, 1, 0, 100, 0, 0, 0, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 11, 35070, 30, 0, 0, 0, 0, 0, 'Kvaldir Harpooner - Out Of Combat - Attack Start'),
(34947, 0, 2, 0, 0, 0, 100, 0, 1000, 1000, 8000, 11000, 11, 38557, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Kvaldir Berserker - In Combat - Cast Throw'),
(34947, 0, 3, 0, 9, 0, 100, 0, 0, 5, 5000, 7000, 11, 43410, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Kvaldir Berserker - Within 0-5 Range - Cast Chop'),
(34947, 0, 4, 0, 2, 0, 100, 1, 0, 30, 40000, 40000, 11, 3019, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Kvaldir Berserker - Between 0-30% Health - Cast Frenzy'),
(34947, 0, 5, 0, 0, 0, 20, 1, 5000, 5000, 0, 0, 1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Kvaldir Berserker - In Combat - Say Line 0');

-- 
-- Darthalia Ebonscorch SAI
SET @ENTRY := 32679;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Darthalia Ebonscorch - Out of Combat - Play Emote 1");

-- Archmage Tenaj SAI
SET @ENTRY := 32688;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Archmage Tenaj - Out of Combat - Play Emote 1");

-- Adorean Lew SAI
SET @ENTRY := 32689;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Adorean Lew - Out of Combat - Play Emote 1");

-- Bitty Frostflinger SAI
SET @ENTRY := 32690;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Bitty Frostflinger - Out of Combat - Play Emote 1");

-- Grindle Firespark SAI
SET @ENTRY := 32676;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Grindle Firespark - Out of Combat - Play Emote 1");

-- Fabioso the Fabulous SAI
SET @ENTRY := 32680;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Fabioso the Fabulous - Out of Combat - Play Emote 1");

-- Sabriana Sorrowgaze SAI
SET @ENTRY := 32693;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Sabriana Sorrowgaze - Out of Combat - Play Emote 1");

-- Emeline Fizzlefry SAI
SET @ENTRY := 32678;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Emeline Fizzlefry - Out of Combat - Play Emote 1");

-- Whirt the All-Knowing SAI
SET @ENTRY := 32677;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Whirt the All-Knowing - Out of Combat - Play Emote 1");

-- Mona Everspring SAI
SET @ENTRY := 32684;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Mona Everspring - Out of Combat - Play Emote 1");

-- The Magnificent Merleaux SAI
SET @ENTRY := 32681;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"The Magnificent Merleaux - Out of Combat - Play Emote 1");

-- Kitz Proudbreeze SAI
SET @ENTRY := 32685;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Kitz Proudbreeze - Out of Combat - Play Emote 1");

-- Crafticus Mindbender SAI
SET @ENTRY := 32686;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Crafticus Mindbender - Out of Combat - Play Emote 1");

-- Babagahnoosh the Grumpy SAI
SET @ENTRY := 32675;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Babagahnoosh the Grumpy - Out of Combat - Play Emote 1");

-- Magus Fansy Goodbringer SAI
SET @ENTRY := 32691;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Magus Fansy Goodbringer - Out of Combat - Play Emote 1");

-- Arcanist Alec SAI
SET @ENTRY := 32692;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Arcanist Alec - Out of Combat - Play Emote 1");

-- Linda Ann Kastinglow SAI
SET @ENTRY := 32687;
UPDATE `creature_template` SET `AIName`="SmartAI" WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,1,0,100,0,4000,8000,4000,8000,5,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Linda Ann Kastinglow - Out of Combat - Play Emote 1");

-- North Sea Kraken SAI
SET @ENTRY := 34925;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY*100 AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 6000, 9000, 7000, 9000, 0, 11, 66514, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'North Sea Kraken - IC - Cast Frost Breath'),
(@ENTRY, 0, 1, 0, 8, 0, 40, 0, 66588, 0, 0, 0, 0, 80, 3492500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'North Sea Kraken - On Spell hit Flaming Spear - ActionList'),
(@ENTRY, 0, 2, 3, 2, 0, 100, 1, 1, 6, 0, 0, 0, 11, 66994, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - On Between 1 and 6% HP - Cast Kraken Tooth Explosion'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - Linked With Previous Event - Die'),
(@ENTRY, 0, 4, 0, 1, 0, 100, 0, 30000, 30000, 30000, 30000, 0, 11, 68909, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - OOC - Cast Submerge '),
(@ENTRY, 0, 5, 0, 0, 0, 100, 0, 15000, 20000, 15000, 20000, 0, 11, 66511, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - IC - Cast Whirl'),
(@ENTRY, 0, 6, 0, 8, 0, 100, 0, 66588, 0, 0, 0, 0, 11, 66717, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'North Sea Kraken - On spell Hit - Cast Quest credit'),
(@ENTRY, 0, 7, 0, 1, 1, 100, 0, 3000, 4000, 5000, 9000, 0, 11, 66514, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'North Sea Kraken - OOC - Cast Frost Breath'),
(@ENTRY, 0, 8, 0, 1, 1, 100, 0, 10000, 15000, 10000, 15000, 0, 11, 66511, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - OOC - Cast Whirl'),
(@ENTRY, 0, 9, 0, 1, 1, 100, 1, 32000, 32000, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - OOC - Set event phase'),
(@ENTRY, 0, 10, 0, 1, 0, 100, 1, 33000, 33000, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - OOC - despawn'),
(@ENTRY, 0, 12, 0, 1, 0, 100, 1, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - OOC - Set event phase'),
(@ENTRY, 0, 13, 0, 1, 0, 100, 1, 32000, 32000, 0, 0, 0, 92, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Northsea Kraken - OOC - Inturreput cast');
