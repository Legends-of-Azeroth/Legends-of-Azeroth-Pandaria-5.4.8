
DELETE FROM `spell_script_names` WHERE `spell_id` IN (125883, 125893);

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(125883, "spell_monk_zen_flight_check"),
(125893, "aura_monk_glyph_of_zen_flight");
