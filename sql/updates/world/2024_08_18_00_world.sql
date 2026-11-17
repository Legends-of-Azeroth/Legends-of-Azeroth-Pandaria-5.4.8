-- Quest 12987 Hodir's Helm
-- Creature text for NPC_ICE_SPIKE_BUNNY 30215
DELETE FROM `creature_text` WHERE `CreatureID`=30215;
INSERT INTO `creature_text`(`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(30215, 1, 0, 'Sons of Hodir! I humbly present to you....', 42, 0, 100.0, 0, 0, 0, 0, 30906, 0, 'Player - Read Pronouncement'),
(30215, 2, 0, 'The Helm of Hodir!', 42, 0, 100.0, 0, 0, 0, 0, 30907, 0, 'Player - Read Pronouncement');