UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 53714;

UPDATE `creature` SET `spawntimesecs` = 5 WHERE `id` IN (53714,57873);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceGroup`=1 AND `SourceEntry`=108967 AND `SourceId`=0 AND `ElseGroup`=0;

UPDATE `creature` SET `position_x` = 1488.9, `position_y` = 3442.31, `position_z` = 171.245, `orientation` = 0.698132  WHERE `guid` = 563357;

DELETE FROM `creature` WHERE `guid`=563367;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseId`, `phaseGroup`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`) VALUES 
(563367, 53565, 860, 5736, 5834, 1, 1, 0, 0, 0, 0, 1492.23, 3426.46, 171.245, 6.16101, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0);

DELETE FROM `creature_text` WHERE `CreatureID`=65469;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `id`, `text`, `TextRange`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `BroadcastTextID`, `comment`) VALUES 
(65469, 0, 0, 'Such skill for a new trainee.', 0, 12, 0, 100, 511, 0, 0, 56431, 'Aspiring Trainee'),
(65469, 0, 1, 'That target didn\'t stand a chance.', 0, 12, 0, 100, 511, 0, 0, 56433, 'Aspiring Trainee'),
(65469, 0, 2, 'One day you will have to teach me your secrets.', 0, 12, 0, 100, 509, 0, 0, 56432, 'Aspiring Trainee'),
(65469, 0, 3, 'You are bound for a great things, trainee.', 0, 12, 0, 100, 508, 0, 0, 56435, 'Aspiring Trainee'),
(65469, 0, 4, 'I must train harder so I can be like you.', 0, 12, 0, 100, 1, 0, 0, 56437, 'Aspiring Trainee'),
(65469, 0, 5, 'I hope the instructors saw that!', 0, 12, 0, 100, 1, 0, 0, 56434, 'Aspiring Trainee'),
(65469, 0, 6, 'I must work as hard as you.', 0, 12, 0, 100, 1, 0, 0, 56436, 'Aspiring Trainee'),
(65469, 0, 7, 'Your form is impeccable.', 0, 12, 0, 100, 1, 0, 0, 56430, 'Aspiring Trainee');
