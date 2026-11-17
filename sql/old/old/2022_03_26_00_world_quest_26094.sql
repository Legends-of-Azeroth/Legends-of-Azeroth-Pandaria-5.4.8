-- Fix Quest Striking Back
-- Add npc_spellclick_spells for Repaired Bomber
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES (42092, 78510, 3, 0);

-- Update npcflag from Repaired Bomber
UPDATE creature_template SET npcflag = 16777216 WHERE entry =42092;