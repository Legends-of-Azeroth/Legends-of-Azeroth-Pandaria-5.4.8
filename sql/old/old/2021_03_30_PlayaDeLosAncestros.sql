-- restringir los spell 52410 y 52418 para el mapa 607 (bg playa de los ancestros)
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`='17' AND `SourceGroup`='0' AND `SourceEntry`='52410' AND `SourceId`='0' AND `ElseGroup`='0' AND 
`ConditionTypeOrReference`='22' AND `ConditionTarget`='0' AND `ConditionValue1`='607';
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`='17' AND `SourceGroup`='0' AND `SourceEntry`='52418' AND `SourceId`='0' AND `ElseGroup`='0' AND 
`ConditionTypeOrReference`='22' AND `ConditionTarget`='0' AND `ConditionValue1`='607';

INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) 
VALUES
('17','0','52410','0','0','22','0','607','0','0','0','0','0','0','solo usar 52410 en playa de los ancestros - map 607'),
('17','0','52418','0','0','22','0','607','0','0','0','0','0','0','solo usar 52418 en playa de los ancestros - map 607');
