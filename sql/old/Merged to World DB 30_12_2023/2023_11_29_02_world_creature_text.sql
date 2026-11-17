-- Creature_text for npc_eye_of_acherus
DELETE FROM `creature_text` WHERE `CreatureID`=28511;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) 
VALUES (28511, 0, 0, 'The Eye of Acherus launches towards its destination.', 42, 0, 100, 0, 0, 0, 0, 28507, 0, 'Eye of Acherus'),
(28511, 1, 0, 'The Eye of Acherus is in your control.', 42, 0, 100, 0, 0, 0, 0, 28465, 0, 'Eye of Acherus');
