-- Fixes rogue learning spell at level three 
DELETE FROM `spell_learn_spell` WHERE `entry`=2098 AND `SpellID`=65535;
