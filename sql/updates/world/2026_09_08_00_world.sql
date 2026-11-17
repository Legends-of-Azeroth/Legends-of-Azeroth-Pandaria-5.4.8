-- Remove old Incanter's Absorption AuraScript bindings that reference
-- non-existent MoP spell IDs (44394/44413).
-- The correct MoP implementation uses:
--   1463   -> spell_mage_incanters_ward          (absorb + mana restore + trigger 116267)
--   118859 -> spell_mage_incanters_ward_cooldown (passive activation/deactivation)
DELETE FROM `spell_script_names` WHERE `spell_id` = 1463   AND `ScriptName` = 'spell_mage_incanters_absorbtion_manashield';
DELETE FROM `spell_script_names` WHERE `spell_id` = 11426  AND `ScriptName` = 'spell_mage_incanters_absorbtion_absorb';
DELETE FROM `spell_script_names` WHERE `spell_id` = 144318 AND `ScriptName` = 'spell_mage_incanters_absorbtion_manashield';

-- Remove old Divine Storm script that references non-existent MoP spell 54171.
-- MoP Divine Storm (53385) has no built-in heal; the heal comes from
-- Glyph of Divine Storm (handled by spell_pal_glyph_of_divine_storm).
DELETE FROM `spell_script_names` WHERE `spell_id` = 53385 AND `ScriptName` = 'spell_pal_divine_storm';

-- Remove spell_mage_mirror_image bindings from Summon Images spells (88091/88092).
-- These are TRIGGER_SPELL effects (the targets cast BY the glyph script),
-- not the source spell. Only 63093 (Glyph of Mirror Image) needs the script.
DELETE FROM `spell_script_names` WHERE `spell_id` = 88091 AND `ScriptName` = 'spell_mage_mirror_image';
DELETE FROM `spell_script_names` WHERE `spell_id` = 88092 AND `ScriptName` = 'spell_mage_mirror_image';

-- Remove spell_warr_last_stand binding: triggered spell 12976 does not exist in MoP.
-- MoP Last Stand (12975) is a self-contained aura (PERIODIC_HEAL), no script needed.
DELETE FROM `spell_script_names` WHERE `spell_id` = 12975 AND `ScriptName` = 'spell_warr_last_stand';

-- Remove spell_rog_vanish bindings from 18461: spell does not exist in MoP Spell.dbc.
DELETE FROM `spell_script_names` WHERE `spell_id` = 18461 AND `ScriptName` = 'spell_rog_vanish';
DELETE FROM `spell_script_names` WHERE `spell_id` = 18461 AND `ScriptName` = 'spell_rog_vanish_trigger';

-- Remove spell_dk_deaths_advance binding from 124285: aura positions are swapped
-- compared to 96268 (eff0=305,eff1=171 vs eff0=171,eff1=305). Script matches 96268.
DELETE FROM `spell_script_names` WHERE `spell_id` = 124285 AND `ScriptName` = 'spell_dk_deaths_advance';

-- Remove spell_dk_soul_reaper bindings from 114866/130735/130736: these have
-- Aura=226 (PERIODIC_DUMMY) at eff1, but script expects Aura=3 (PERIODIC_DAMAGE).
-- Only 69409 has the correct aura type.
DELETE FROM `spell_script_names` WHERE `spell_id` = 114866 AND `ScriptName` = 'spell_dk_soul_reaper';
DELETE FROM `spell_script_names` WHERE `spell_id` = 130735 AND `ScriptName` = 'spell_dk_soul_reaper';
DELETE FROM `spell_script_names` WHERE `spell_id` = 130736 AND `ScriptName` = 'spell_dk_soul_reaper';

-- Remove spell_rog_marked_for_death binding from 140149: only has eff0 (Aura=226),
-- no EFFECT_1 for the script's DUMMY hook. 137619 has correct eff1=Dummy.
DELETE FROM `spell_script_names` WHERE `spell_id` = 140149 AND `ScriptName` = 'spell_rog_marked_for_death';

-- Remove spell_timeless_isle_cauterize binding from 147997: DBC has Fx136/A89,
-- no periodic dummy aura for the script to hook.
DELETE FROM `spell_script_names` WHERE `spell_id` = 147997 AND `ScriptName` = 'spell_timeless_isle_cauterize';

-- Remove bindings where DBC effect type is completely different from script expectation:
-- 122853 (inhale): DBC has Fx2 (SCHOOL_DAMAGE), script expects SCRIPT_EFFECT
DELETE FROM `spell_script_names` WHERE `spell_id` = 122853 AND `ScriptName` = 'spell_inhale';
-- 128815 (grab_air_balloon): DBC has Fx160, script expects APPLY_AURA
DELETE FROM `spell_script_names` WHERE `spell_id` = 128815 AND `ScriptName` = 'spell_grab_air_balloon';
-- 122169 (ook_ook_barrel_ride): DBC has Fx140 (FORCE_CAST), script expects CONTROL_VEHICLE aura
DELETE FROM `spell_script_names` WHERE `spell_id` = 122169 AND `ScriptName` = 'spell_ook_ook_barrel_ride';

-- Remove AuraScript bindings where DBC has Fx3 (DUMMY effect, not an aura) or wrong aura type:
DELETE FROM `spell_script_names` WHERE `spell_id` = 117006 AND `ScriptName` = 'spell_devastating_arc';
DELETE FROM `spell_script_names` WHERE `spell_id` = 86529  AND `ScriptName` = 'spell_sha_mail_specialization';
DELETE FROM `spell_script_names` WHERE `spell_id` = 122982 AND `ScriptName` = 'spell_unseen_strike';
DELETE FROM `spell_script_names` WHERE `spell_id` = 123600 AND `ScriptName` = 'spell_su_dummy';
DELETE FROM `spell_script_names` WHERE `spell_id` = 123616 AND `ScriptName` = 'spell_su_dumaura';
DELETE FROM `spell_script_names` WHERE `spell_id` = 124018 AND `ScriptName` = 'spell_pheromones_of_zeal';
DELETE FROM `spell_script_names` WHERE `spell_id` = 137526 AND `ScriptName` = 'spell_incubate_visual';
DELETE FROM `spell_script_names` WHERE `spell_id` = 137587 AND `ScriptName` = 'spell_warl_kil_jaedens_cunning';
DELETE FROM `spell_script_names` WHERE `spell_id` = 138002 AND `ScriptName` = 'spell_fluidity';

-- More bindings with mismatched aura/effect types:
DELETE FROM `spell_script_names` WHERE `spell_id` = 20243  AND `ScriptName` = 'spell_warr_sword_and_board';
DELETE FROM `spell_script_names` WHERE `spell_id` = 23218  AND `ScriptName` = 'spell_dru_shapeshift_move_speed';
DELETE FROM `spell_script_names` WHERE `spell_id` = 110700 AND `ScriptName` = 'spell_pal_divine_shield';
DELETE FROM `spell_script_names` WHERE `spell_id` = 115399 AND `ScriptName` = 'spell_monk_healing_elixirs';
DELETE FROM `spell_script_names` WHERE `spell_id` = 75428  AND `ScriptName` = 'spell_romogg_skullcracker_eff';
DELETE FROM `spell_script_names` WHERE `spell_id` = 108939 AND `ScriptName` = 'spell_pri_glyph_of_levitate';

-- Fix old _stats script names: code renamed them without _stats suffix.
-- Replace old bindings with correct script names.
DELETE FROM `spell_script_names` WHERE `spell_id` = 1126  AND `ScriptName` = 'spell_dru_mark_of_the_wild_stats';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (1126, 'spell_dru_mark_of_the_wild');
DELETE FROM `spell_script_names` WHERE `spell_id` = 1459  AND `ScriptName` = 'spell_mage_arcane_brilliance_stats';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (1459, 'spell_mage_arcane_brilliance');
DELETE FROM `spell_script_names` WHERE `spell_id` = 21562 AND `ScriptName` = 'spell_pri_power_word_fortitude_stats';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (21562, 'spell_pri_power_word_fortitude');

-- Bind Mastery: Discipline (Shield) AuraScript to shield spells without built-in
-- mastery scaling. 114908 (Spirit Shell) and 47753 (Divine Aegis shield) are
-- excluded: their absorb amounts already include mastery via
-- spell_pri_spirit_shell / spell_pri_divine_aegis.
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (17, 'spell_mastery_shield_discipline');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (123258, 'spell_mastery_shield_discipline');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (114214, 'spell_mastery_shield_discipline');

-- Bind Templar's Verdict damage scaling (Holy Power / Divine Purpose).
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (85256, 'spell_pal_templar_s_verdict');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (138165, 'spell_pal_templar_s_verdict');
