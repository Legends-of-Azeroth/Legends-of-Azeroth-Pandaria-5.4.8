
-- 1. Remove wrongly-added night equipment templates (ID=2)
DELETE FROM creature_equip_template WHERE CreatureID IN
  (1756, 14304, 29712, 35322, 42218, 67965, 73600) AND ID = 2;

-- 2. Remove wrongly-added game_event_model_equip mappings (event 29)
DELETE FROM game_event_model_equip
WHERE eventEntry = 29
  AND guid IN (SELECT guid FROM creature WHERE id IN (1756, 29712));
