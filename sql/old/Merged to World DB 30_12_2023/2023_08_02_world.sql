-- Patchfix Quest 8327
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`) IN 
((14,6570,7785,0,0,47,0,8327,74,0),
(14,6570,7868,0,0,47,0,8327,1,0),
(14,6570,7869,0,0,8,0,8335,0,0));
INSERT INTO `conditions` (SourceTypeOrReferenceId,SourceGroup,SourceEntry,SourceId,ElseGroup,ConditionTypeOrReference,ConditionTarget,ConditionValue1,ConditionValue2,ConditionValue3,NegativeCondition,ErrorType,ErrorTextId,ScriptName,Comment) VALUES
(14,6570,7785,0,0,47,0,8327,74,0,0,0,0,'',"Group 0: Show gossip text 7785 if quest 'Report to Lanthan Perilon' is taken or completed or rewarded"),
(14,6570,7868,0,0,47,0,8327,1,0,0,0,0,'',"Group 0: Show gossip text 7868 if quest 'Report to Lanthan Perilon' is not taken"),
(14,6570,7869,0,0,8,0,8335,0,0,0,0,0,'',"Group 0: Show gossip text 7869 if quest 'Felendren the Banished' is rewarded");