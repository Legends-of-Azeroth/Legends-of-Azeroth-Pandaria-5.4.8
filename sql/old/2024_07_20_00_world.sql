UPDATE `creature` SET `phaseId`= 1686 WHERE `guid` IN (
570539,570540,570541,570542,570543,570544,570545,570546,570547,570548,570549,570550,570551,570552,570553,570554,570555,570556,570557,570558,570559,570560,570561,570562,570563,570564,570565,570566,570567,570568,570569,570570,
570571,570572,570573,570574,570575,570576,570577,570578,570579,570580,570581,570582,570583,570584,570585,570586,570587,570588,570589,570590,570591,570592,570593,570594,570595,570596,570597,570598,570599,570600,570601,570602,
570603,570604,570605,570606,570607,570608,570609,570610,570611);

DELETE FROM `phase_area` WHERE `AreaId`=1519 AND `PhaseId`=1686;
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES 
(1519, 1686, 'Stormwind - 5.0 Phased Terrain Master');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=26 AND `SourceGroup`=1686;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 1686, 0, 0, 0, 8, 0, 29547, 0, 0, 0, 0, 0, '', 'Player has quest The King\'s Command (29547) rewarded'),
(26, 1686, 0, 0, 0, 8, 0, 29548, 0, 0, 1, 0, 0, '', 'Player has not quest The Mission (29548) rewarded');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=29 AND `SourceEntry`=1066;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(29, 0, 1066, 0, 0, 8, 0, 29547, 0, 0, 0, 0, 0, '', 'Player has quest The King\'s Command (29547) rewarded'),
(29, 0, 1066, 0, 0, 8, 0, 29548, 0, 0, 1, 0, 0, '', 'Player has not quest The Mission (29548) rewarded');
