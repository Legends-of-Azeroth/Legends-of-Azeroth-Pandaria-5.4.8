DELETE FROM conditions WHERE SourceTypeOrReferenceId=17 AND SourceEntry=15119;
INSERT INTO conditions (SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName, Comment) VALUES
(17,0,15119,0,0,29,0,9684,100,0,1,0,0,"","");

DELETE FROM creature WHERE id=9684;

DELETE FROM event_scripts WHERE id = 3981;
INSERT INTO event_scripts (id, delay, command, datalong, datalong2, dataint, x, y, z, o) VALUES
(3981,0,10,9684,3000000 ,0,-6976.77,-2060.63,-271.289,3.7);
