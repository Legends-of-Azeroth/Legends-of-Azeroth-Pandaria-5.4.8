-- Creature text for The Lurker Below (entry 21217)
DELETE FROM `creature_text` WHERE `CreatureID`=21217;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(21217, 0, 0, '%s takes a deep breath.', 41, 0, 100.0, 0, 0, 0, 20021, 1, 'The Lurker Below - EMOTE_SPOUT');

-- Creature text for The Gluth (entry 15932)
DELETE FROM `creature_text` WHERE `CreatureID`=15932;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(15932, 0, 0, '%s spots a zombie to devour!', 16, 0, 100.0, 0, 0, 0, 12242, 1, 'Gluth - spots one'),
(15932, 1, 0, '%s decimates all nearby flesh!', 41, 0, 100.0, 0, 0, 0, 32321, 1, 'Gluth - decimate'),
(15932, 2, 0, '%s becomes enraged!', 41, 0, 100.0, 0, 0, 0, 24144, 1, 'Gluth - enrage'),
(15932, 3, 0, '%s devours all nearby zombies!', 16, 0, 100.0, 0, 0, 0, 12348, 1, 'Gluth - devours all'),
(15932, 4, 0, '%s goes into a berserker rage!', 41, 0, 100.0, 0, 0, 0, 34057, 1, 'Gluth - berserker');

-- Creature text for Lilian Voss (entry 59200)
DELETE FROM `creature_text` WHERE `CreatureID`=59200 AND `GroupID`=12;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(59200, 12, 0, 'Lilian\'s Soul fades into the spirit realm! It can no longer be harmed!', 16, 0, 100.0, 0, 0, 0, 66157, 0, 'LILIAN_TALK_13');

-- Creature text for Dark Haze (entry 54628) 
DELETE FROM `creature_text` WHERE `CreatureID`=54628;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(54628, 0, 0, 'Spawn of the Old Gods materialize nearby!', 16, 0, 100.0, 0, 0, 0, 66157, 0, 'Dark Haze EMOTE');

-- Creature text for Explosives Barrel (entry 60095)
DELETE FROM `creature_text` WHERE `CreatureID`=60095;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(60095, 0, 0, 'Three...', 16, 0, 100.0, 0, 0, 0, 59618, 0, 'Explosives Barrel EMOTE'),
(60095, 1, 0, 'Two...', 16, 0, 100.0, 0, 0, 0, 59619, 0, 'Explosives Barrel EMOTE'),
(60095, 2, 0, 'One...', 16, 0, 100.0, 0, 0, 0, 59620, 0, 'Explosives Barrel EMOTE'),
(60095, 3, 0, 'BOOM!', 16, 0, 100.0, 0, 0, 0, 59621, 0, 'Explosives Barrel EMOTE');