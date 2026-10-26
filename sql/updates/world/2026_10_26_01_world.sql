-- Hostile quest targets and rares that respawned after 1-6 seconds
-- The world dump gives these spawns a respawn delay of 1-6 seconds (plus the 60 second corpse decay, halved once looted), so named
-- quest targets came back within 30-60 seconds and rares were up almost permanently. TDB 4.3.4 and AzerothCore use 300 seconds for
-- all of them (1800 for the rare Gug Fatcandle, 60 for Felendren the Banished); spawns without an older counterpart get the delay of
-- the other spawns of the same creature or of their neighbours. Scripted event, escort and vehicle creatures that rely on a short
-- delay, and the Dead Scar battle on the Isle of Quel'Danas, keep theirs.

-- Tirisfal Glades: Nissa Agamand (1655), Devlin Agamand (1657), Captain Perrine (1662), Scarlet Lieutenant Gebler (39002)
-- Deathknell: Karrel Grayves (1918)
UPDATE `creature` SET `spawntimesecs` = 300 WHERE `guid` IN (194142, 194144, 192609, 194288, 194116, 192562);
-- Elwynn Forest: Princess (330); Northshire: Kurtok the Slayer (42938); Durotar: Lieutenant Palliter (39269)
UPDATE `creature` SET `spawntimesecs` = 300 WHERE `guid` IN (180100, 178468, 252854);
-- Sunstrider Isle: Felendren the Banished (15367)
UPDATE `creature` SET `spawntimesecs` = 60 WHERE `guid` = 54969;
-- Blade's Edge Mountains: Consortium Engineer (19673), the only one of its spawns with 6 seconds
UPDATE `creature` SET `spawntimesecs` = 300 WHERE `guid` = 102294;

-- Rares: Gug Fatcandle (62), Tormented Spirit (1533), Fellicent's Shade (10358)
UPDATE `creature` SET `spawntimesecs` = 1800 WHERE `guid` IN (302867, 18298, 194297, 16737, 193453);

-- The Wandering Isle: Fe-Feng Wiseman (55601), like the other Fe-Feng hozen in the village
UPDATE `creature` SET `spawntimesecs` = 300 WHERE `guid` IN (562795, 562796, 562797, 562798, 562800, 562801, 562802, 562803, 562804, 562808,
562819, 562820, 562826, 562827, 562828, 562830, 562831, 562832, 562834, 562835, 562838, 562839, 562844, 562859, 562872, 562878, 562890);
-- The Wandering Isle: Amberleaf Scamp (54130) and Corsac Fox (53704, 57797), like their other spawns
UPDATE `creature` SET `spawntimesecs` = 600 WHERE `guid` IN (562056, 562057, 562058, 562061, 562062, 562065, 562066, 562067, 562069, 562074,
562076, 562078, 562079, 562082, 562083, 562084, 562085, 562086, 562087, 562090, 562095, 562098, 562099, 562100, 562101, 562102, 562105,
562106, 562107, 562108, 562110, 562115, 562116, 562117, 562118, 562119, 562120, 563539);
UPDATE `creature` SET `spawntimesecs` = 600 WHERE `guid` IN (562063, 562064, 562080, 562091, 563535, 562075, 562088);
