-- Charging Ox Wave: the line filter belongs to the stun (119392), not to the visual (125084)
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'spell_monk_charging_ox_wave';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(119392, 'spell_monk_charging_ox_wave');
