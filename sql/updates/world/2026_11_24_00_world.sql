-- npc_tarindrella (Gnarlpine Corruption Totem, 49598) - quest 28727 (Vile Touch)
-- Attach script and creature texts

UPDATE `creature_template` SET `AIName`='npc_tarindrellaAI', `ScriptName`='' WHERE `entry`=49598;

DELETE FROM `creature_text` WHERE `CreatureID`=49598;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(49598, 0, 0, 'This totem has been corrupting the eggs! It seems a greater threat looms. The Gnarlpine remain tainted by something most foul.', 14, 0, 100, 0, 0, 0, 0, 49697, 0, 'Gnarlpine Corruption Totem'),
(49598, 1, 0, 'You\'ve come to help, $c? Let us stay together for a while.', 14, 0, 100, 0, 0, 0, 0, 49550, 0, 'Gnarlpine Corruption Totem'),
(49598, 2, 0, 'My dear friends... I\'m so sorry...', 14, 0, 100, 0, 0, 0, 0, 49671, 0, 'Gnarlpine Corruption Totem');
