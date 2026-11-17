--
DELETE FROM `spell_script_names` WHERE `spell_id` = 51858;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(51858, 'spell_chapter1_siphon_of_acherus');

-- 51904 - Summon Ghouls On Scarlet Crusade (Eye of Acherus quest 12641, action button 2)
DELETE FROM `spell_script_names` WHERE `spell_id` = 51904 AND `ScriptName` = 'spell_q12641_death_comes_from_on_high_summon_ghouls';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(51904, 'spell_q12641_death_comes_from_on_high_summon_ghouls');

-- 84421 - Loot-A-Rang (item 60854)
DELETE FROM `spell_script_names` WHERE `spell_id` = 84421;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(84421, 'spell_item_loot_a_rang');