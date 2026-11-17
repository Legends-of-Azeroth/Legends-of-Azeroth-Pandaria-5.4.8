UPDATE `creature` SET `phaseId`= 1663 WHERE `guid` IN (570486,570487,570488,570489,570490,570491,570492,570493,570494,570495,570496,570497,570498,570499,570500,570501);

DELETE FROM `phase_area` WHERE `AreaId`=14 AND `PhaseId`=1663;
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES 
(14, 1663, 'Orgrimmar Gunship');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1663;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1663, 0, 0, 0, 8, 0, 29612, 0, 0, 0, 0, 0, '', 'Player has quest The Art of War (29612) rewarded'),
(26, 1663, 0, 0, 0, 8, 0, 29690, 0, 0, 1, 0, 0, '', 'Player has not quest Into the Mists (29690) rewarded');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=1074;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(29, 0, 1074, 0, 0, 8, 0, 29612, 0, 0, 0, 0, 0, '', 'Player has quest The Art of War (29612) rewarded'),
(29, 0, 1074, 0, 0, 8, 0, 29690, 0, 0, 1, 0, 0, '', 'Player has not quest Into the Mists (29690) rewarded');
