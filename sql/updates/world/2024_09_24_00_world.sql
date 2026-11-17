-- Fix Paladin Judgement power gain. Thanks Sjenkie from discord for report.
DELETE FROM `spell_script_names` WHERE `spell_id`=20271;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (20271, 'spell_pal_judgment');
