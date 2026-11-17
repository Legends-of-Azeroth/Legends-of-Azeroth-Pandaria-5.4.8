-- Mini Mana Bomb (65903)
DELETE FROM `creature_template_addon` WHERE `entry` = 65903;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `ai_anim_kit`, `movement_anim_kit`, `melee_anim_kit`, `auras`) 
VALUES (65903, 0, 0, 0, 1, 0, 0, 0, 0, '105729');

UPDATE `creature_template` SET `ScriptName` = 'npc_toy_mini_maba_bomb' WHERE `entry` = 65903;

UPDATE `spell_proc_event` SET `procFlags` = `procFlags` & ~34816 WHERE `entry` = 108945;

-- Herbalism Flower(29888) hackfix
UPDATE `creature_template` SET `scale` = 1 WHERE `entry` = 29888;

DELETE FROM `spell_script_names` WHERE `spell_id` = 138957 AND `ScriptName` = "spell_ppm_hackfix";
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES(138957, "spell_ppm_hackfix");

DELETE FROM `spell_script_names` WHERE `spell_id` = 138786 AND (`ScriptName` = "spell_gen_remove_aura_on_remove" OR `ScriptName` = "spell_gen_remove_aura_on_remove_wushoolays_lightning");
DELETE FROM `spell_script_names` WHERE `spell_id` = 138756 AND `ScriptName` = "spell_gen_remove_aura_on_remove_blades_of_renataki";
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES(138786, "spell_gen_remove_aura_on_remove_wushoolays_lightning");
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES(138756, "spell_gen_remove_aura_on_remove_blades_of_renataki");

-- Char DB
-- DELETE FROM `character_aura` WHERE `spell` = 80326;
-- DELETE FROM `guild_challenges` WHERE `GuildId` NOT IN (SELECT `guildid` FROM `guild`); 
