-- Priest Glyph of Confession
DELETE FROM `spell_script_names` WHERE `spell_id`=126152;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(126152, 'spell_pri_glyph_of_confession');
