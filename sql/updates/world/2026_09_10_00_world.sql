-- Fix missing quest_template_addon entries for Worgen Starting Area quest chain
-- Issue #398: "Brothers in Arms" (26129) has no follow-up quest after completion
-- Quest 14159 "The Rebel Lord's Arsenal" is missing its addon entry, breaking the chain:
--   14154 -> 26129 -> [14159 MISSING] -> 14204 -> 14214 -> ...

-- 14159: The Rebel Lord's Arsenal
-- Prev: 26129 (Brothers In Arms), Next: 14204 (From the Shadows)
DELETE FROM `quest_template_addon` WHERE `ID` = 14159;
INSERT INTO `quest_template_addon` (`ID`, `MaxLevel`, `AllowableClasses`, `SourceSpellID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`, `RewardMailTemplateID`, `RewardMailDelay`, `RequiredSkillID`, `RequiredSkillPoints`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `ProvidedItemCount`, `SpecialFlags`, `ScriptName`)
VALUES (14159, 0, 0, 0, 26129, 14204, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

-- 14320: In Need of Ingredients (also missing addon entry, same chain area)
-- Prev: 14313 (Among Humans Again), Next: 14321 (Invasion)
DELETE FROM `quest_template_addon` WHERE `ID` = 14320;
INSERT INTO `quest_template_addon` (`ID`, `MaxLevel`, `AllowableClasses`, `SourceSpellID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`, `RewardMailTemplateID`, `RewardMailDelay`, `RequiredSkillID`, `RequiredSkillPoints`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `ProvidedItemCount`, `SpecialFlags`, `ScriptName`)
VALUES (14320, 0, 0, 0, 14313, 14321, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

-- 14099: Royal Orders (hub quest, missing addon entry - present in TC 4.3.4 with all zeros)
DELETE FROM `quest_template_addon` WHERE `ID` = 14099;
INSERT INTO `quest_template_addon` (`ID`, `MaxLevel`, `AllowableClasses`, `SourceSpellID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`, `RewardMailTemplateID`, `RewardMailDelay`, `RequiredSkillID`, `RequiredSkillPoints`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `ProvidedItemCount`, `SpecialFlags`, `ScriptName`)
VALUES (14099, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

-- 27846: The Grateful Dead (missing addon entry - present in TC 4.3.4 with all zeros)
DELETE FROM `quest_template_addon` WHERE `ID` = 27846;
INSERT INTO `quest_template_addon` (`ID`, `MaxLevel`, `AllowableClasses`, `SourceSpellID`, `PrevQuestID`, `NextQuestID`, `ExclusiveGroup`, `RewardMailTemplateID`, `RewardMailDelay`, `RequiredSkillID`, `RequiredSkillPoints`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `ProvidedItemCount`, `SpecialFlags`, `ScriptName`)
VALUES (27846, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');
