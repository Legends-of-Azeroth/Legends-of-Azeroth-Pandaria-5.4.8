-- Icecrown Citadel: Shadowmourne quest line - align to TrinityCore (c3ec91265a / 2015_03_30_03_world.sql)
-- NPC 37120 (Darion) no longer handles the other questgivers; 38606-38609 (Jaina/Muradin/Uther/Sylvanas)
-- are the quest starter/ender and use SmartAI with SAI timelines.
-- 5.4.8 schema adapted: smart_scripts has event_param5; creature_text uses CreatureID/GroupID/ID.

SET @JAINA         := 38606;
SET @MURADIN       := 38607;
SET @UTHER         := 38608;
SET @SYLVANAS      := 38609;
SET @DARION        := 37120;
SET @ALEXANDROS    := 38610;
SET @JAINAQUEST    := 24916;
SET @MURADINQUEST  := 24917;
SET @UTHERQUEST    := 24919;
SET @SYLVANASQUEST := 24918;
SET @DARIONQUEST   := 24915;

-- Remove the obsolete C++ script binding; SmartAI drives these NPCs now
UPDATE `creature_template` SET `AIName`='SmartAI', `ScriptName`='' WHERE `entry` IN (@JAINA, @MURADIN, @UTHER, @SYLVANAS, @ALEXANDROS);
UPDATE `creature_template` SET `AIName`='SmartAI', `ScriptName`='' WHERE `entry`=@DARION;

-- Quest starter/ender: move from Darion to the dedicated NPCs (idempotent)
DELETE FROM `creature_queststarter` WHERE `id`=@DARION AND `quest` IN (@JAINAQUEST, @MURADINQUEST, @UTHERQUEST, @SYLVANASQUEST);
DELETE FROM `creature_questender`   WHERE `id`=@DARION AND `quest` IN (@JAINAQUEST, @MURADINQUEST, @UTHERQUEST, @SYLVANASQUEST);
INSERT IGNORE INTO `creature_queststarter` (`id`, `quest`) VALUES
(@JAINA, @JAINAQUEST), (@MURADIN, @MURADINQUEST), (@UTHER, @UTHERQUEST), (@SYLVANAS, @SYLVANASQUEST);
INSERT IGNORE INTO `creature_questender` (`id`, `quest`) VALUES
(@JAINA, @JAINAQUEST), (@MURADIN, @MURADINQUEST), (@UTHER, @UTHERQUEST), (@SYLVANAS, @SYLVANASQUEST);

-- SAI timelines (5.4.8: 29 columns, event_param5 inserted after event_param4)
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (@JAINA, @JAINA*100, @MURADIN, @MURADIN*100, @UTHER, @UTHER*100, @SYLVANAS, @SYLVANAS*100, @DARION, @DARION*100+1, @ALEXANDROS) AND `source_type` IN (0,9);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@JAINA, 0, 0, 0, 20, 0, 100, 0, @JAINAQUEST, 0, 0, 0, 0, 80, @JAINA*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Quest ''Jaina''s Locket'' Finished - Run Script'),
(@JAINA*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Remove NPC Flags'),
(@JAINA*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Remove NPC Flags'),
(@JAINA*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Remove NPC Flags'),
(@JAINA*100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Remove NPC Flags'),
(@JAINA*100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Remove NPC Flags'),
(@JAINA*100, 9, 5, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Say Line 0'),
(@JAINA*100, 9, 6, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Say Line 1'),
(@JAINA*100, 9, 7, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Say Line 2'),
(@JAINA*100, 9, 8, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Say Line 3'),
(@JAINA*100, 9, 9, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Say Line 4'),
(@JAINA*100, 9, 10, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Say Line 5'),
(@JAINA*100, 9, 11, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Restore NPC Flags'),
(@JAINA*100, 9, 12, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Restore NPC Flags'),
(@JAINA*100, 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Restore NPC Flags'),
(@JAINA*100, 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Restore NPC Flags'),
(@JAINA*100, 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Lady Jaina Proudmoore - On Script - Restore NPC Flags'),
(@MURADIN, 0, 0, 0, 20, 0, 100, 0, @MURADINQUEST, 0, 0, 0, 0, 80, @MURADIN*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Quest ''Muradin''s Lament'' Finished - Run Script'),
(@MURADIN*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Remove NPC Flags'),
(@MURADIN*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Remove NPC Flags'),
(@MURADIN*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Remove NPC Flags'),
(@MURADIN*100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Remove NPC Flags'),
(@MURADIN*100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Remove NPC Flags'),
(@MURADIN*100, 9, 5, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 0'),
(@MURADIN*100, 9, 6, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 1'),
(@MURADIN*100, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 2'),
(@MURADIN*100, 9, 8, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 3'),
(@MURADIN*100, 9, 9, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 4'),
(@MURADIN*100, 9, 10, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 5'),
(@MURADIN*100, 9, 11, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 6'),
(@MURADIN*100, 9, 12, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Say Line 7'),
(@MURADIN*100, 9, 13, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Restore NPC Flags'),
(@MURADIN*100, 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Restore NPC Flags'),
(@MURADIN*100, 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Restore NPC Flags'),
(@MURADIN*100, 9, 16, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Restore NPC Flags'),
(@MURADIN*100, 9, 17, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Muradin Bronzebeard - On Script - Restore NPC Flags'),
(@UTHER, 0, 0, 0, 20, 0, 100, 0, @UTHERQUEST, 0, 0, 0, 0, 80, @UTHER*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Quest ''The Lightbringer''s Redemption'' Finished - Run Script'),
(@UTHER*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Remove NPC Flags'),
(@UTHER*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Remove NPC Flags'),
(@UTHER*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Remove NPC Flags'),
(@UTHER*100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Remove NPC Flags'),
(@UTHER*100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Remove NPC Flags'),
(@UTHER*100, 9, 5, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 0'),
(@UTHER*100, 9, 6, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 1'),
(@UTHER*100, 9, 7, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 2'),
(@UTHER*100, 9, 8, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 3'),
(@UTHER*100, 9, 9, 0, 0, 0, 100, 0, 9000, 9000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 4'),
(@UTHER*100, 9, 10, 0, 0, 0, 100, 0, 11000, 11000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 5'),
(@UTHER*100, 9, 11, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 6'),
(@UTHER*100, 9, 12, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 7'),
(@UTHER*100, 9, 13, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Say Line 8'),
(@UTHER*100, 9, 14, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Restore NPC Flags'),
(@UTHER*100, 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Restore NPC Flags'),
(@UTHER*100, 9, 16, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Restore NPC Flags'),
(@UTHER*100, 9, 17, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Restore NPC Flags'),
(@UTHER*100, 9, 18, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Uther the Lightbringer - On Script - Restore NPC Flags'),
(@SYLVANAS, 0, 0, 0, 20, 0, 100, 0, @SYLVANASQUEST, 0, 0, 0, 0, 80, @SYLVANAS*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Quest ''Sylvanas'' Vengeance'' Finished - Run Script'),
(@SYLVANAS*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Remove NPC Flags'),
(@SYLVANAS*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Remove NPC Flags'),
(@SYLVANAS*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Remove NPC Flags'),
(@SYLVANAS*100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Remove NPC Flags'),
(@SYLVANAS*100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Remove NPC Flags'),
(@SYLVANAS*100, 9, 5, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 0'),
(@SYLVANAS*100, 9, 6, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 1'),
(@SYLVANAS*100, 9, 7, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 2'),
(@SYLVANAS*100, 9, 8, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 3'),
(@SYLVANAS*100, 9, 9, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 4'),
(@SYLVANAS*100, 9, 10, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 5'),
(@SYLVANAS*100, 9, 11, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Say Line 6'),
(@SYLVANAS*100, 9, 12, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Restore NPC Flags'),
(@SYLVANAS*100, 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Restore NPC Flags'),
(@SYLVANAS*100, 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Restore NPC Flags'),
(@SYLVANAS*100, 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Restore NPC Flags'),
(@SYLVANAS*100, 9, 16, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @DARION, 0, 10, 0, 0, 0, 0, 'Lady Sylvanas Windrunner - On Script - Restore NPC Flags'),
(@DARION, 0, 7, 0, 20, 0, 100, 0, @DARIONQUEST, 0, 0, 0, 0, 80, @DARION*100+1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Quest ''Mograine''s Reunion'' Finished - Run Script'),
(@DARION*100+1, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Remove NPC Flags'),
(@DARION*100+1, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Remove NPC Flags'),
(@DARION*100+1, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Remove NPC Flags'),
(@DARION*100+1, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Remove NPC Flags'),
(@DARION*100+1, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Remove NPC Flags'),
(@DARION*100+1, 9, 5, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 0, 5, 432, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Play Emote 432'),
(@DARION*100+1, 9, 6, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 0, 11, 72468, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Cast ''Summon Alexandros'''),
(@DARION*100+1, 9, 7, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 19, @ALEXANDROS, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Set Orientation Closest Creature ''Highlord Alexandros Mograine'''),
(@DARION*100+1, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 4'),
(@DARION*100+1, 9, 9, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 0'),
(@DARION*100+1, 9, 10, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 1'),
(@DARION*100+1, 9, 11, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 5'),
(@DARION*100+1, 9, 12, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 2'),
(@DARION*100+1, 9, 13, 0, 0, 0, 100, 0, 11000, 11000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 3'),
(@DARION*100+1, 9, 14, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 4'),
(@DARION*100+1, 9, 15, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 5'),
(@DARION*100+1, 9, 16, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 9, @ALEXANDROS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 6'),
(@DARION*100+1, 9, 17, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Say Line 6'),
(@DARION*100+1, 9, 18, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Set Orientation Home Position'),
(@DARION*100+1, 9, 19, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 11, @ALEXANDROS, 10, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Despawn Highlord Alexandros'),
(@DARION*100+1, 9, 20, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Restore NPC Flags'),
(@DARION*100+1, 9, 21, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 9, @JAINA, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Restore NPC Flags'),
(@DARION*100+1, 9, 22, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @MURADIN, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Restore NPC Flags'),
(@DARION*100+1, 9, 23, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @UTHER, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Restore NPC Flags'),
(@DARION*100+1, 9, 24, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 9, @SYLVANAS, 0, 10, 0, 0, 0, 0, 'Highlord Darion Mograine - On Script - Restore NPC Flags'),
(@ALEXANDROS, 0, 0, 0, 1, 0, 100, 1, 0, 0, 0, 0, 0, 11, 72469, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Highlord Alexandros Mograine - Out of Combat - Cast ''Alexandros Spawn''');
