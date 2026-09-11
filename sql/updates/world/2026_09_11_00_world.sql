-- Quest 14222 "Last Stand" - Fix friendly NPCs not attacking Frenzied Stalkers
-- Frenzied Stalkers (35627) and nearby friendly NPCs should auto-attack each other

-- ============================================
-- 1. Frenzied Stalker (35627) - Add AIName and SmartAI
-- ============================================

UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 35627;

DELETE FROM smart_scripts WHERE entryorguid = 35627 AND source_type = 0;

-- Enrage at 0-30% health (from TC 4.3.4)
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment)
VALUES
(35627, 0, 0, 1, 2, 0, 100, 1, 0, 30, 0, 0, 0, 11, 8599, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Frenzied Stalker - Between 0-30% Health - Cast Enrage (No Repeat)'),
(35627, 0, 1, 0, 61, 0, 100, 1, 0, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Frenzied Stalker - Between 0-30% Health - Say Line 0 (No Repeat)');

-- ============================================
-- 2. Northgate Rebel (41015) - Add AIName and SmartAI
-- ============================================

UPDATE creature_template SET AIName = 'SmartAI' WHERE entry = 41015;

DELETE FROM smart_scripts WHERE entryorguid = 41015 AND source_type = 0;

-- Out of combat - Attack Frenzied Stalker (35627) (from TC 4.3.4)
INSERT INTO smart_scripts (entryorguid, source_type, id, link, event_type, event_phase_mask, event_chance, event_flags, event_param1, event_param2, event_param3, event_param4, event_param5, action_type, action_param1, action_param2, action_param3, action_param4, action_param5, action_param6, target_type, target_param1, target_param2, target_param3, target_param4, target_x, target_y, target_z, target_o, comment)
VALUES
(41015, 0, 0, 0, 1, 0, 100, 0, 0, 0, 2000, 2000, 0, 49, 0, 0, 0, 0, 0, 0, 11, 35627, 0, 0, 0, 0, 0, 0, 0, 'Northgate Rebel - Out of Combat - Attack Frenzied Stalker (35627)');
