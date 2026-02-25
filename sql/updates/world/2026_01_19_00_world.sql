
-- Phasing
DELETE FROM `phase_area` WHERE `AreaId` = 6384 AND `PhaseId` = 1443 OR `AreaId` = 6386 AND `PhaseId` = 1443 OR `AreaId` = 6384 AND `PhaseId` = 1445 OR `AreaId` = 6386 AND `PhaseId` = 1445;
INSERT INTO `phase_area` (`AreaId`, `PhaseId`, `Comment`) VALUES
(6384, 1443, 'Cosmetic - Scarlet Monastery Quest Phase'),
(6386, 1443, 'Cosmetic - Scarlet Monastery Quest Phase'),
(6384, 1445, 'Cosmetic - Scarlet Monastery Quest HC Phase'),
(6386, 1445, 'Cosmetic - Scarlet Monastery Quest HC Phase');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 26 AND `SourceGroup` = 1443 AND `SourceEntry` = 6384) OR (`SourceTypeOrReferenceId` = 26 AND `SourceGroup` = 1443 AND `SourceEntry` = 6386) OR (`SourceTypeOrReferenceId` = 26 AND `SourceGroup` = 1445 AND `SourceEntry` = 6386) OR (`SourceTypeOrReferenceId` = 26 AND `SourceGroup` = 1445 AND `SourceEntry` = 6384);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES
(26, 1443, 6384, 0, 0, 8, 0, 31514, 0, 0, 1, 'Allow Phase 1443 if Quest (31514) IS NOT rewarded'),
(26, 1443, 6386, 0, 0, 8, 0, 31514, 0, 0, 1, 'Allow Phase 1443 if Quest (31514) IS NOT rewarded'),
(26, 1445, 6384, 0, 0, 8, 0, 31516, 0, 0, 1, 'Allow Phase 1443 if Quest (31516) IS NOT rewarded'),
(26, 1445, 6386, 0, 0, 8, 0, 31516, 0, 0, 1, 'Allow Phase 1443 if Quest (31516) IS NOT rewarded');

DELETE FROM `quest_request_items` WHERE `ID` IN(31516 /*Unto Dust Thou Shalt Return*/, 31515 /*Blades of the Anointed*/, 31514 /*Unto Dust Thou Shalt Return*/, 31513 /*Blades of the Anointed*/);
INSERT INTO `quest_request_items` (`ID`, `EmoteOnComplete`, `EmoteOnIncomplete`, `CompletionText`, `VerifiedBuild`) VALUES
(31516, 0, 0, '<The deed is not yet done. There can be no end to the Scarlet Crusade while Whitemane can come back from the dead.>', 18414), -- Unto Dust Thou Shalt Return
(31515, 0, 0, '<With one blade already in your possession, you move to take the other.>$B$B<An inscription on the statue behind the sword reads, \"The Hand of Providence\".>', 18414), -- Blades of the Anointed
(31514, 0, 0, '<The deed is not yet done. There can be no end to the Scarlet Crusade while Whitemane can come back from the dead.>', 18414), -- Unto Dust Thou Shalt Return
(31513, 0, 0, '<With one blade already in your possession, you move to take the other.>$B$B<An inscription on the statue behind the sword reads, \"The Hand of Providence\".>', 18414); -- Blades of the Anointed

-- Creature Text
-- Hooded Crusader 64842
-- RP event https://wowpedia.fandom.com/wiki/Unto_Dust_Thou_Shalt_Return
-- Hooded Crusader says: It is done. You did it, <name>!
-- The Hooded Crusader leaps from where she appeared to Whitemane's corpse.
-- The Hooded Crusader gazes down upon the high inquisitor's blade-skewered corpse.
-- Hooded Crusader says: And now enough of this deception.
-- The Hooded Crusader sheds her disguise.
-- Lilian Voss says: Hello, my name's Lilian. You don't mind if I take those blades, do you?
-- The blades transform as she pulls them from Whitemane's corpse.
-- Lilian Voss says: Yes, these will do quite nicely. They feel almost as if they were meant for me all along.
DELETE FROM `creature_text` WHERE `CreatureID`=64842;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(64842, 0, 0, 'It is done. You did it, $p!', 12, 0, 100, 0, 0, 0, 0, 65470, 0, 'Hooded Crusader'),
(64842, 1, 0, 'The Hooded Crusader gazes down upon the high inquisitor\'s blade-skewered corpse.', 16, 0, 100, 0, 0, 0, 0, 65473, 0, 'Hooded Crusader'),
(64842, 2, 0, 'And now enough of this deception.', 12, 0, 100, 0, 0, 0, 0, 65471, 0, 'Hooded Crusader'),
(64842, 3, 0, 'Hello, my name\'s Lilian. You don\'t mind if I take those blades, do you?', 12, 0, 100, 0, 0, 0, 0, 65472, 0, 'Hooded Crusader'),
(64842, 4, 0, 'The blades transform as she pulls them from Whitemane\'s corpse.', 16, 0, 100, 0, 0, 0, 0, 65567, 0, 'Hooded Crusader'),
(64842, 5, 0, 'Yes, these will do quite nicely. They feel almost as if they were meant for me all along.', 12, 0, 100, 0, 0, 0, 0, 65518, 0, 'Hooded Crusader');

-- Creature
UPDATE `creature_template` SET `ScriptName` = 'npc_hooded_crusader_c64842' WHERE `entry` = 64842;