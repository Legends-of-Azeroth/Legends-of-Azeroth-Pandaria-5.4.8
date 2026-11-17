-- el item id 63031 permitia al jugador abrir casi cualquier puerta dentro del juego
-- se agrego una condición para que no se pueda utilizar el spell del item a menos que el jugador este cerca del objetivo (un object)
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceGroup`=0 AND `SourceEntry`=88697 AND `SourceId`=0 AND `ElseGroup`=0 AND `ConditionTypeOrReference`=30 AND `ConditionTarget`=0 AND `ConditionValue1`=206625 AND `ConditionValue2`=5 AND `ConditionValue3`=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) 
VALUES (17, 0, 88697, 0, 0, 30, 0, 206625, 5, 0, 0, 0, 0, '', 'Slash of Tichondrius can only be casted near Firedoor');
