-- 
UPDATE `creature` SET `position_x` = 10547.014648, `position_y` = 875.161499, `position_z` = 1309.603149, `orientation` = 1.639151 WHERE `guid` = 276862;

UPDATE `creature_template_addon` SET `bytes1` = 65536 WHERE `entry` = 49479;
UPDATE `creature_template_addon` SET `bytes1` = 3 WHERE `entry` = 8584;
DELETE FROM `creature_addon` WHERE `guid` = 276856;
INSERT INTO `creature_addon` (`guid`, `bytes1`) VALUES
(276856, 8);

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 49479;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49479 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49479*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49479, 0, 0, 0, 20, 0, 100, 512, 28724, 0, 0, 0, 0, 80, 4947900, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - action list'),
(49479, 0, 1, 0, 19, 0, 100, 0, 28725, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Say text 2'),
(49479, 0, 2, 0, 19, 0, 100, 0, 28729, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Say text 3'),
(49479, 0, 3, 0, 19, 0, 100, 0, 28730, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Say text 4'),
(4947900, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Close gossip'),
(4947900, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 83, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Remove npc flag'),
(4947900, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 11, 87071, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Cast Alchemy'),
(4947900, 9, 3, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 11, 92388, 0, 0, 0, 0, 0, 19, 8584, 15, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Cast Curing Ivveron'),
(4947900, 9, 4, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Say text 1'),(4947900, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 82, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Dentaria Silverglade - on quest rewarded - Add npc flag');

DELETE FROM `conditions` WHERE `SourceEntry` = 92388 AND `SourceGroup` = 1 AND `SourceTypeOrReferenceId` = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 1, 92388, 0, 0, 31, 0, 3, 8584, 0, 0, 0, 0, "", "Spell ID: 92388 target Iverron");

DELETE FROM `creature_text` WHERE `CreatureID` = 49479;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(49479, 0, 0, 'Iverron\'s poison is cured, but it will take some time for him to recover.', 12, 0, 100, 0, 0, 0, 0, 49668, 0, 'Dentaria Silverglade'),
(49479, 1, 0, 'Shadowthread Cave lies to the north. Be careful, it\'s dangerous there of late.', 12, 0, 100, 0, 0, 0, 0, 49665, 0, 'Dentaria Silverglade'),
(49479, 2, 0, 'The moonwell is to the northeast, on the other side of the pool and up the hill.', 12, 0, 100, 0, 0, 0, 0, 49666, 0, 'Dentaria Silverglade'),
(49479, 3, 0, 'The ramp up to Aldrassil is just in sight over there. Circle around and find Tenaron up top.', 12, 0, 100, 0, 0, 0, 0, 49667, 0, 'Dentaria Silverglade');

UPDATE `quest_template` SET `OfferRewardText` = "You are a quick study in our ways." WHERE `Id` = 26947;