-- Tirisfal Glades: creature abilities
-- Most of the zone's hostile creatures had no AI and only melee'd. Their abilities are ported from the Cataclysm data (Tirisfal is
-- the Cataclysm revamp, e.g. Tirisfal Farmer's Dirt Toss), and from the classic data where Cataclysm has none (Bleeding Horror,
-- Rotting Ancestor and the Scarlet Monastery entrance mobs). Spells that 5.4.8 no longer has (old player ranks) are left out.

SET @ENTRIES := '1520,1523,1525,1526,1527,1528,1529,1530,1531,1532,1533,1534,1535,1536,1537,1538,1539,1540,1545,1547,1548,1549,1655,1657,1662,1664,1675,1688,1753,1890,1910,1934,1935,1936,1941,4280,4281,4282,4283,4284,4285,10358,10359,39049';

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE FIND_IN_SET(`entry`, @ENTRIES);
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND FIND_IN_SET(`entryorguid`, @ENTRIES);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `target_type`, `comment`) VALUES
-- Scourge
(1520, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 7000, 7000, 11, 84282, 0, 2, 'Rattlecage Soldier - In Combat - Cast ''Slice of the Scourge'''),
(1523, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 5000, 5000, 11, 84282, 0, 2, 'Cracked Skull Soldier - In Combat - Cast ''Slice of the Scourge'''),
(1525, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 8000, 8000, 11, 3234, 0, 2, 'Rotting Dead - In Combat - Cast ''Disease Touch'''),
(1526, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 7000, 7000, 11, 3234, 0, 2, 'Ravaged Corpse - In Combat - Cast ''Disease Touch'''),
(1527, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 5000, 5000, 11, 3234, 0, 2, 'Hungering Dead - In Combat - Cast ''Disease Touch'''),
(1528, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 8000, 8000, 11, 3234, 0, 2, 'Shambling Horror - In Combat - Cast ''Disease Touch'''),
(1529, 0, 0, 0, 0, 0, 100, 0, 2700, 3400, 9400, 11600, 11, 3322, 0, 2, 'Bleeding Horror - In Combat - Cast ''Rancid Blood'''),
(1530, 0, 0, 0, 0, 0, 100, 0, 2700, 3400, 9800, 12100, 11, 3322, 0, 2, 'Rotting Ancestor - In Combat - Cast ''Rancid Blood'''),
(1531, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 8000, 8000, 11, 7713, 0, 2, 'Lost Soul - In Combat - Cast ''Wailing Dead'''),
(1532, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 13000, 13000, 11, 7713, 0, 2, 'Wandering Spirit - In Combat - Cast ''Wailing Dead'''),
(1533, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 13000, 13000, 11, 7713, 0, 2, 'Tormented Spirit - In Combat - Cast ''Wailing Dead'''),
-- Wailing Ancestor cast it only 12 seconds into the fight and 75% of the time, so it rarely lived long enough to cast
-- it. Use the timing of the other Wailing Dead casters.
(1534, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 13000, 13000, 11, 7713, 0, 2, 'Wailing Ancestor - In Combat - Cast ''Wailing Dead'''),
(1890, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 15000, 15000, 11, 81219, 0, 1, 'Rattlecage Skeleton - In Combat - Cast ''Battle Shout'''),
(1655, 0, 0, 0, 0, 0, 100, 0, 2000, 2000, 3000, 3000, 11, 28993, 64, 2, 'Nissa Agamand - In Combat CMC - Cast ''Banshee Wail'''),
(1655, 0, 1, 0, 0, 0, 100, 0, 4000, 4000, 17000, 17000, 11, 51897, 0, 2, 'Nissa Agamand - In Combat - Cast ''Banshee Screech'''),
(1657, 0, 0, 0, 9, 0, 100, 0, 0, 5, 35000, 40000, 11, 3148, 0, 2, 'Devlin Agamand - Within 0-5 Range - Cast ''Head Crack'''),
(1657, 0, 1, 0, 0, 0, 100, 0, 4000, 4000, 4000, 4000, 11, 34447, 0, 2, 'Devlin Agamand - In Combat - Cast ''Arcane Missiles'''),
(1657, 0, 2, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 7, 'Devlin Agamand - On Aggro - Say Line 0'),
(1657, 0, 3, 0, 1, 0, 100, 0, 1000, 10000, 15000, 25000, 1, 1, 0, 1, 'Devlin Agamand - Out of Combat - Say Line 1'),
(10358, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 5000, 5000, 11, 13901, 64, 2, 'Fellicent''s Shade - In Combat CMC - Cast ''Arcane Bolt'''),
(10358, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 7000, 7000, 11, 11975, 0, 2, 'Fellicent''s Shade - In Combat - Cast ''Arcane Explosion'''),
(10358, 0, 2, 0, 0, 0, 100, 0, 1000, 1000, 10000, 10000, 11, 7068, 0, 2, 'Fellicent''s Shade - In Combat - Cast ''Veil of Shadow'''),
-- Scarlet Crusade
(1535, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 30000, 60000, 1, 0, 0, 1, 'Scarlet Warrior - In Combat - Say Line 0'),
(1535, 0, 1, 0, 0, 0, 100, 0, 3000, 5000, 45000, 50000, 11, 3238, 0, 1, 'Scarlet Warrior - In Combat - Cast ''Nimble Reflexes'''),
(1535, 0, 2, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 7, 'Scarlet Warrior - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1535, 0, 3, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 11, 57846, 0, 2, 'Scarlet Warrior - In Combat - Cast ''Heroic Strike'''),
(1536, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 1, 'Scarlet Missionary - On Aggro - Say Line 0'),
(1536, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 4000, 4000, 11, 9053, 64, 2, 'Scarlet Missionary - In Combat CMC - Cast ''Fireball'''),
(1537, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 1, 'Scarlet Zealot - On Aggro - Say Line 0'),
(1537, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 5000, 5000, 11, 84535, 0, 2, 'Scarlet Zealot - In Combat - Cast ''Zealotry'''),
(1538, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 1, 'Scarlet Friar - On Aggro - Say Line 0'),
(1538, 0, 1, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 'Scarlet Friar - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1538, 0, 2, 0, 0, 0, 100, 0, 3000, 4000, 20000, 25000, 11, 75967, 0, 1, 'Scarlet Friar - In Combat - Cast ''Whirlwind'''),
(1539, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 1800000, 1800000, 11, 12544, 1, 1, 'Scarlet Neophyte - Out of Combat - Cast ''Frost Armor'''),
(1539, 0, 1, 0, 0, 0, 100, 0, 0, 0, 2400, 3800, 11, 13322, 64, 2, 'Scarlet Neophyte - In Combat CMC - Cast ''Frostbolt'''),
(1539, 0, 2, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 1, 'Scarlet Neophyte - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1539, 0, 3, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 1, 'Scarlet Neophyte - On Aggro - Say Line 0'),
(1540, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 1, 'Scarlet Vanguard - On Aggro - Say Line 0'),
(1540, 0, 1, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 'Scarlet Vanguard - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1540, 0, 2, 0, 4, 0, 100, 0, 0, 0, 0, 0, 75, 7164, 0, 1, 'Scarlet Vanguard - On Aggro - Add Aura ''Defensive Stance'''),
(1540, 0, 3, 0, 0, 0, 100, 0, 3000, 6000, 3000, 6000, 11, 11972, 0, 2, 'Scarlet Vanguard - In Combat - Cast ''Shield Bash'''),
(1662, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 7000, 7000, 11, 12169, 0, 1, 'Captain Perrine - In Combat - Cast ''Shield Block'''),
(1662, 0, 1, 0, 0, 0, 100, 0, 15000, 15000, 15000, 15000, 11, 3019, 0, 1, 'Captain Perrine - In Combat - Cast ''Frenzy'''),
(1664, 0, 0, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 'Captain Vachon - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1664, 0, 1, 0, 0, 0, 100, 0, 4000, 4000, 6000, 6000, 11, 79732, 0, 2, 'Captain Vachon - In Combat - Cast ''Shield Bash'''),
(1664, 0, 2, 0, 0, 0, 100, 0, 3000, 6000, 8000, 12000, 11, 12169, 0, 1, 'Captain Vachon - In Combat - Cast ''Shield Block'''),
(1934, 0, 0, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 'Tirisfal Farmer - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1934, 0, 1, 0, 0, 0, 100, 0, 4000, 4500, 12000, 20000, 11, 80382, 0, 2, 'Tirisfal Farmer - In Combat - Cast ''Dirt Toss'''),
(1934, 0, 2, 0, 4, 0, 10, 0, 0, 0, 0, 0, 1, 0, 0, 7, 'Tirisfal Farmer - On Aggro - Say Line 0'),
(1935, 0, 0, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 'Tirisfal Farmhand - Between 0-15% Health - Flee For Assist (No Repeat)'),
(1935, 0, 1, 0, 4, 0, 10, 0, 0, 0, 0, 0, 1, 0, 0, 7, 'Tirisfal Farmhand - On Aggro - Say Line 0'),
(1936, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 5000, 5000, 11, 11976, 0, 2, 'Farmer Solliden - In Combat - Cast ''Strike'''),
(4280, 0, 0, 0, 0, 0, 100, 0, 2100, 3300, 8200, 9300, 11, 13953, 0, 2, 'Scarlet Preserver - In Combat - Cast ''Holy Strike'''),
(4280, 0, 1, 0, 2, 0, 100, 1, 5, 30, 0, 0, 11, 13952, 0, 1, 'Scarlet Preserver - Between 5-30% Health - Cast ''Holy Light'' (No Repeat)'),
(4281, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 6979, 0, 2, 'Scarlet Scout - On Aggro - Cast ''Fire Shot'''),
(4281, 0, 1, 0, 0, 0, 100, 0, 1700, 2300, 6800, 7900, 11, 6660, 0, 2, 'Scarlet Scout - In Combat - Cast ''Shoot'''),
(4282, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 4000, 4000, 11, 9053, 64, 2, 'Scarlet Magician - In Combat CMC - Cast ''Fireball'''),
(4283, 0, 0, 0, 2, 0, 100, 1, 20, 80, 0, 0, 11, 3639, 0, 1, 'Scarlet Sentry - Between 20-80% Health - Cast ''Improved Blocking'' (No Repeat)'),
(4284, 0, 0, 0, 0, 0, 100, 0, 2100, 3200, 8700, 9600, 11, 9613, 64, 2, 'Scarlet Augur - In Combat CMC - Cast ''Shadow Bolt'''),
(4285, 0, 0, 0, 0, 0, 100, 0, 4000, 7000, 8000, 12000, 11, 9734, 64, 2, 'Scarlet Disciple - In Combat CMC - Cast ''Holy Smite'''),
(4285, 0, 1, 0, 2, 0, 100, 1, 25, 50, 0, 0, 11, 11640, 0, 1, 'Scarlet Disciple - Between 25-50% Health - Cast ''Renew'' (No Repeat)'),
(4285, 0, 2, 0, 2, 0, 100, 1, 0, 10, 0, 0, 11, 11642, 0, 1, 'Scarlet Disciple - Between 0-10% Health - Cast ''Heal'' (No Repeat)'),
-- Gnolls, murlocs and beasts
(1675, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 120000, 120000, 11, 3237, 0, 2, 'Rot Hide Mongrel - In Combat - Cast ''Curse of Thule'''),
(1753, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 100000, 100000, 11, 3237, 0, 2, 'Maggot Eye - In Combat - Cast ''Curse of Thule'''),
(1753, 0, 1, 0, 0, 0, 100, 0, 3000, 3000, 7000, 7000, 11, 3243, 0, 2, 'Maggot Eye - In Combat - Cast ''Life Harvest'''),
(1941, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 120000, 120000, 11, 3234, 0, 2, 'Rot Hide Graverobber - In Combat - Cast ''Disease Touch'''),
(1545, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 8000, 8000, 11, 7159, 0, 2, 'Vile Fin Muckdweller - In Combat - Cast ''Backstab'''),
(1910, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 6000, 6000, 11, 11824, 0, 2, 'Muad - In Combat - Cast ''Shock'''),
(1547, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 6000, 6000, 11, 82797, 0, 2, 'Decrepit Darkhound - In Combat - Cast ''Leaping Bite'''),
(1547, 0, 1, 0, 0, 0, 100, 0, 3000, 3000, 4000, 4000, 11, 17253, 0, 2, 'Decrepit Darkhound - In Combat - Cast ''Bite'''),
(1548, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 6000, 6000, 11, 82797, 0, 2, 'Cursed Darkhound - In Combat - Cast ''Leaping Bite'''),
(1548, 0, 1, 0, 0, 0, 100, 0, 3000, 3000, 4000, 4000, 11, 17253, 0, 2, 'Cursed Darkhound - In Combat - Cast ''Bite'''),
(1549, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 13000, 13000, 11, 82797, 0, 2, 'Ravenous Darkhound - In Combat - Cast ''Leaping Bite'''),
(1549, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 7000, 7000, 11, 17253, 0, 2, 'Ravenous Darkhound - In Combat - Cast ''Bite'''),
(1688, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 15000, 15000, 11, 11918, 0, 2, 'Night Web Matriarch - In Combat - Cast ''Poison'''),
(10359, 0, 0, 0, 0, 0, 100, 0, 4000, 4000, 15000, 15000, 11, 3583, 0, 2, 'Sri''skulk - In Combat - Cast ''Deadly Poison'''),
(39049, 0, 0, 0, 0, 0, 100, 0, 1000, 1000, 6000, 6000, 11, 16827, 0, 2,'Plagued Bruin - In Combat - Cast ''Claw'''),
(39049, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 11000, 11000, 11, 3242, 0, 2, 'Plagued Bruin - In Combat - Cast ''Ravage''');

-- Deathknell: Deathguard abilities
-- Deathguard Elite and Deathguard Randolph had no AI and only melee'd. Port their abilities from the Cataclysm data. Both already
-- have Invisibility and Stealth Detection as a template aura, so its cast on reset is left out.

SET @ENTRIES := '1736,7980';

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE FIND_IN_SET(`entry`, @ENTRIES);
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND FIND_IN_SET(`entryorguid`, @ENTRIES);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `target_type`, `comment`) VALUES
(1736, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 2000, 3000, 11, 95826, 64, 2, 'Deathguard Randolph - In Combat - Cast ''Shoot'''),
(1736, 0, 1, 0, 25, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 1, 'Deathguard Randolph - On Reset - Set Reactstate Aggressive'),
(7980, 0, 0, 0, 0, 0, 100, 0, 5000, 7000, 7000, 10000, 11, 14516, 0, 2, 'Deathguard Elite - In Combat - Cast ''Strike'''),
(7980, 0, 1, 0, 9, 0, 100, 0, 0, 5, 12000, 16000, 11, 9080, 0, 2, 'Deathguard Elite - Within 0-5 Range - Cast ''Hamstring''');

-- Recruitment (26800): pointing Darnell (49337) at a Scarlet Corpse (49340)

-- Right-clicking a corpse makes the player cast Select Scarlet Corpse (91942) on it (the corpse was the caster and the player the
-- target). spell_select_scarlet_corpse makes the player point and sends their Darnell to the corpse.
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 49340;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(49340, 91942, 1, 0);

DELETE FROM `spell_script_names` WHERE `spell_id` = 91942;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(91942, 'spell_select_scarlet_corpse');

-- Only while Recruitment is in the quest log, and a corpse is only picked up by a Darnell that was sent to it
-- (the selection is Select Scarlet Corpse's aura on the corpse)
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 18 AND `SourceGroup` = 49340 AND `SourceEntry` = 91942;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceGroup` = 1 AND `SourceEntry` = 49340 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(18, 49340, 91942, 0, 0, 9, 0, 26800, 0, 0, 0, 0, 0, '', 'Scarlet Corpse - Spellclick only while quest 26800 is taken'),
(22, 1, 49340, 0, 0, 1, 1, 91942, 1, 0, 0, 0, 0, '', 'Scarlet Corpse - Only picked up when selected by the player');

-- Scarlet Corpse: Darnell is a personal summon, which "closest creature" lookups skip, so the corpse finds him with a range lookup.
-- Darnell casts Reverse Cast Ride Vehicle on it, which gives the kill credit to his summoner, and spell_reverse_cast_ride_vehicle_darnell
-- makes the corpse ride on him (his vehicle has a seat for each of the 6 corpses). Saltain despawns the corpses on turn-in.
UPDATE `smart_scripts` SET `link` = 0, `action_param3` = 9, `action_param4` = 49337, `action_param5` = 0, `action_param6` = 5,
`comment` = 'Scarlet Corpse - On Darnell In Range - Darnell Cast Reverse Cast Ride Vehicle'
WHERE `entryorguid` = 49340 AND `source_type` = 0 AND `id` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49340 AND `source_type` = 0 AND `id` = 1;

DELETE FROM `spell_script_names` WHERE `spell_id` = 91945;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(91945, 'spell_reverse_cast_ride_vehicle_darnell');

-- Darnell: once at the corpse, wait a moment and follow the player again
DELETE FROM `smart_scripts` WHERE `entryorguid` = 49337 AND `source_type` = 0 AND `id` = 7;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 4933703 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(49337, 0, 7, 0, 34, 0, 100, 0, 8, 49340, 0, 0, 80, 4933703, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Darnell - On Point 49340 Reached (Scarlet Corpse) - Start Script'),
(4933703, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 29, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Darnell - On Script - Set Follow Owner');

-- Deathguard Saltain: on turning in Beyond the Graves or Recruitment, Darnell goes away. He is a personal summon that "closest
-- creature" cannot find, so use a range lookup (this also reaches other players' Darnells within 20 yards).
UPDATE `smart_scripts` SET `target_type` = 9, `target_param1` = 49337, `target_param2` = 0, `target_param3` = 20
WHERE (`entryorguid` = 1740 AND `source_type` = 0 AND `id` = 2) OR (`entryorguid` = 174001 AND `source_type` = 9 AND `id` = 0);

-- Summon Darnell (91938): spell_area casts it again on login or area change while Beyond the Graves or Recruitment is in the quest
-- log (the aura is no longer saved, so Darnell comes back after a relog) and removes it on an area change once the quest is not.
-- spell_summon_darnell unsummons Darnell when the aura goes away. Deathknell is its own zone (6454) inside Tirisfal Glades (85), and
-- the quests go between both; without a fitting row the summon on quest accept fails with "wrong zone".
DELETE FROM `spell_area` WHERE `spell` = 91938;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES
(91938, 85, 25089, 0, -8326, 0, 2, 1, (1 << 1) | (1 << 3), 0),
(91938, 85, 26800, 0, -8326, 0, 2, 1, (1 << 1) | (1 << 3), 0),
(91938, 6454, 25089, 0, -8326, 0, 2, 1, (1 << 1) | (1 << 3), 0),
(91938, 6454, 26800, 0, -8326, 0, 2, 1, (1 << 1) | (1 << 3), 0);

DELETE FROM `spell_script_names` WHERE `spell_id` = 91938;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(91938, 'spell_summon_darnell');

-- Scarlet Corpses are no quest givers (Recruitment is started by Deathguard Saltain)
DELETE FROM `creature_queststarter` WHERE `id` = 49340 AND `quest` = 26800;
UPDATE `creature_template` SET `npcflag` = 0x01000000 WHERE `entry` = 49340;

-- Speak with Sevren (25005): Coleman Farthing offered it straight away, but it hands in the Agamand remains collected in Deaths in
-- the Family (25029), as the pre-Cataclysm 355 followed 354
UPDATE `quest_template_addon` SET `PrevQuestID` = 25029 WHERE `ID` = 25005;

-- Johaan's Experiment (24977): feeding the Laced Pumpkin to the Captured Scarlet Zealot (1931)
-- His script still started on the pre-Cataclysm quest Fields of Grief (407) and walked him around the Gallow's End cellar. Since
-- Cataclysm he is held outside by the gazebo and the quest is 24977: on turn-in he speaks, turns into a ghoul (Ghoul Form, 3287),
-- staggers around, rots and dies, then despawns and respawns quickly for the next player.
UPDATE `creature` SET `spawntimesecs` = 1 WHERE `guid` = 193727;

DELETE FROM `smart_scripts` WHERE `entryorguid` = 1931 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(1931, 0, 0, 0, 20, 0, 100, 0, 24977, 0, 0, 0, 0, 80, 193100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Quest \'Johaan\'s Experiment\' Finished - Run Script'),
(1931, 0, 1, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 41, 3000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Just Died - Despawn In 3000 ms');

-- No turn-ins to him while he is turning; the quest giver flag comes back when he respawns
DELETE FROM `smart_scripts` WHERE `entryorguid` = 193100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(193100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 83, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Script - Remove Npc Flag Questgiver'),
(193100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Script - Say Line 0'),
(193100, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 11, 3287, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Script - Cast \'Ghoul Form\''),
(193100, 9, 3, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Script - Say Line 1'),
(193100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 89, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Script - Start Random Movement'),
(193100, 9, 5, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Captured Scarlet Zealot - On Script - Kill Self');

-- Night Elf Mohawk camp in Tirisfal Glades: put the pre-Cataclysm spawns back on the lowered terrain
UPDATE `creature` SET `position_z` = 34.299 WHERE `guid` = 62020; -- Night Elf Mohawk
UPDATE `creature` SET `position_z` = 33.745 WHERE `guid` = 83023; -- Mighty Frostsaber
UPDATE `gameobject` SET `position_z` = 34.596 WHERE `guid` = 9035; -- Night Elf Stool
UPDATE `gameobject` SET `position_z` = 34.000 WHERE `guid` = 9792; -- Night Elf Tent 02 x.8, sunk into the slope so its downhill edge touches the ground

-- Lieutenant Sanders hangs from his noose (vehicle 659) instead of standing beside it

-- The noose hangs still, as sniffed (the world dump gave it random movement)
UPDATE `creature` SET `wander_distance` = 0, `MovementType` = 0 WHERE `guid` = 192577;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 38936;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(38936, 46598, 1, 0);

-- Sanders is a minion accessory: the accessories are installed again on the noose's first update, and a non-minion one is not
-- unsummoned when it is put off, so it was left standing beside the tree as a second Sanders
DELETE FROM `vehicle_template_accessory` WHERE `entry` = 38936;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(38936, 13158, 0, 1, 'Lieutenant Sanders'' Noose - Lieutenant Sanders', 8, 0);

-- The chain: the Noose Focus above the noose chains Sanders by the foot, to a second, also invisible Noose Focus at his foot (the
-- old standalone Sanders spawn's guid). The sniffed Sanders Foot Noose (73444) is a chain of fixed length that falls short of
-- him, so this uses a Base -> Base chain beam instead (Ongolongo - Chain Visual). Both foci hang still, as sniffed.
DELETE FROM `creature` WHERE `guid` = 192578 AND `id` IN (13158, 39093);
DELETE FROM `creature_addon` WHERE `guid` = 192578;
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `position_x`, `position_y`, `position_z`,
`orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES
(192578, 39093, 0, 85, 4918, 1, 1, 11686, 2533.23, -920.179, 61.0012, 4.53786, 300, 0, 0, 42, 0, 0);
UPDATE `creature` SET `wander_distance` = 0, `MovementType` = 0 WHERE `guid` = 192579;
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 39093;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (39093, -192579) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `target_type`, `target_param1`, `target_param2`, `comment`) VALUES
(-192579, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 5000, 5000, 11, 79807, 34, 10, 192578, 39093, 'Lieutenant Sanders'' Noose Focus - Out of Combat - Cast ''Ongolongo - Chain Visual'' on the Noose Focus at his foot (Aura Not Present)');

-- A hanging prisoner holds no weapon
DELETE FROM `creature_equip_template` WHERE `CreatureID` = 13158;

-- A Daughter's Embrace (25046): Summon Lilian Voss (Guardian) (73306) is cast again on login or on entering the Scarlet
-- Watch Post, and spell_summon_lilian_voss_guardian only summons her when she is not still following the player.
-- Only while the quest is incomplete: once High Priest Voss is dealt with she has left and must not return to replay it.
DELETE FROM `spell_area` WHERE `spell` = 73306;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES
(73306, 459, 25046, 0, 0, 0, 2, 1, (1 << 3), 0);

DELETE FROM `spell_script_names` WHERE `spell_id` = 73306;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(73306, 'spell_summon_lilian_voss_guardian');

-- Practice Target (5674) beside Helena Atwood in the Undercity was spawned with random movement and wandered around. It is a
-- target dummy and stands still, like the Archery Target spawns.
UPDATE `creature` SET `MovementType` = 0, `wander_distance` = 0 WHERE `guid` = 192550;
