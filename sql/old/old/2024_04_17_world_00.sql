
-- Add condition for quest 14368  
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceEntry` IN (68596,68597,68598));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 36289, 68596, 0, 0, 9, 0, 14368, 0, 0, 0, 0, 0, '', 'Required quest active for spellclick'),
(18, 36287, 68597, 0, 0, 9, 0, 14368, 0, 0, 0, 0, 0, '', 'Required quest active for spellclick'),
(18, 36288, 68598, 0, 0, 9, 0, 14368, 0, 0, 0, 0, 0, '', 'Required quest active for spellclick');

-- Delete duplicated spawn for entry 36289(James)
DELETE FROM `creature` WHERE `guid` IN (221768,221772);
