-- Silverpine Forest: fix the typo "Crowly" in the first line of creature 44882 (should be "Crowley").
-- Source: broadcast_text 44951: "We haven't got much time. Crowley will be arriving shortly. Let's go!"
UPDATE `creature_text` SET `Text` = REPLACE(`Text`, 'Crowly', 'Crowley') WHERE `CreatureID` = 44882 AND `GroupID` = 0 AND `ID` = 0 AND `Text` LIKE '%Crowly%';
