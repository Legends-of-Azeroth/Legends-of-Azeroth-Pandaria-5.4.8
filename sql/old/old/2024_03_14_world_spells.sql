-- Durid Spell Celestial Alignment(112071), Astral Communion(127663)
DELETE FROM `spell_script_names` WHERE `spell_id` IN (112071, 127663);
INSERT INTO `spell_script_names` VALUES 
(112071, 'spell_druid_celestial_alignment'),
(127663, 'spell_druid_astral_communion');
