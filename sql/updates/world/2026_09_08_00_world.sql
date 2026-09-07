-- Remove old Incanter's Absorption AuraScript bindings that reference
-- non-existent MoP spell IDs (44394/44413).
-- The correct MoP implementation uses:
--   1463   -> spell_mage_incanters_ward          (absorb + mana restore + trigger 116267)
--   118859 -> spell_mage_incanters_ward_cooldown (passive activation/deactivation)
DELETE FROM `spell_script_names` WHERE `spell_id` = 1463   AND `ScriptName` = 'spell_mage_incanters_absorbtion_manashield';
DELETE FROM `spell_script_names` WHERE `spell_id` = 11426  AND `ScriptName` = 'spell_mage_incanters_absorbtion_absorb';
DELETE FROM `spell_script_names` WHERE `spell_id` = 144318 AND `ScriptName` = 'spell_mage_incanters_absorbtion_manashield';
