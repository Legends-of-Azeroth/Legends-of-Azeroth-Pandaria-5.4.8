-- Kira Songshine creature text
DELETE FROM `creature_text` WHERE `CreatureID`=3937 AND `GroupID`=0;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(3937, 0, 0, 'Fresh bread for sale!', 12, 7, 100.0, 0, 0, 0, 4013, 0, 'Kira Songshine'),
(3937, 0, 1, 'Fresh bread, baked this very morning.', 12, 7, 100.0, 0, 0, 0, 4014, 0, 'Kira Songshine'),
(3937, 0, 2, 'Come get yer fresh bread!', 12, 7, 100.0, 0, 0, 0, 4015, 0, 'Kira Songshine');

-- Echo of Archimonde creature text
DELETE FROM `creature_text` WHERE `CreatureID`=13083 AND `GroupID`=0;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES 
(13083, 0, 0, 'All of your efforts have been in vain, for the draining of the World Tree has already begun!  Soon the heart of your world will beat no more!', 14, 0, 100.0, 0, 0, 10986, 20432, 0, 'Echo of Archimonde');

