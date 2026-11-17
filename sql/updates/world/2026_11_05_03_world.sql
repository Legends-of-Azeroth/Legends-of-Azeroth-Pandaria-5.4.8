-- Exploration quests (90) show their objective twice in the quest log: quest_template.AreaDescription (imported from the Cataclysm 4.3.4 data)
-- plus a quest_objective of type 10 (area trigger) with an empty description. Move the text into the objective and empty AreaDescription,
-- so the client shows one objective line that ticks off. Only quests that have such an objective are changed.
-- Source: Cataclysm 4.3.4 data has AreaDescription as the only objective line; same double line is reported for quest 29415 on Cataclysm Classic.
UPDATE `quest_objective` o JOIN `quest_template` q ON q.`ID` = o.`QuestID`
  SET o.`Description` = q.`AreaDescription`
  WHERE o.`Type` = 10 AND o.`Description` = '' AND q.`AreaDescription` <> '' AND q.`ID` IN (62,76,201,273,287,437,455,503,578,869,870,944,984,1265,1448,1658,1699,1719,2240,2989,3505,4811,4842,5156,6025,6185,6421,8286,9160,9193,9260,9261,9262,9263,9264,9265,9400,9607,9608,9700,9701,9716,9731,9786,10269,10275,10750,10772,11652,11890,12032,12036,12263,12506,12665,12816,12817,13564,13607,13639,13816,13881,13961,14066,14389,24452,24618,25081,25325,25621,25715,25930,26258,26512,26930,26975,27007,27044,27152,27341,27349,27610,27704,28228,28635,28732,29392,29415,29536,29539);

UPDATE `quest_template` q SET q.`AreaDescription` = ''
  WHERE q.`ID` IN (62,76,201,273,287,437,455,503,578,869,870,944,984,1265,1448,1658,1699,1719,2240,2989,3505,4811,4842,5156,6025,6185,6421,8286,9160,9193,9260,9261,9262,9263,9264,9265,9400,9607,9608,9700,9701,9716,9731,9786,10269,10275,10750,10772,11652,11890,12032,12036,12263,12506,12665,12816,12817,13564,13607,13639,13816,13881,13961,14066,14389,24452,24618,25081,25325,25621,25715,25930,26258,26512,26930,26975,27007,27044,27152,27341,27349,27610,27704,28228,28635,28732,29392,29415,29536,29539) AND EXISTS (SELECT 1 FROM `quest_objective` o WHERE o.`QuestID` = q.`ID` AND o.`Type` = 10 AND o.`Description` = q.`AreaDescription`);
