
-- 1. Add missing 3rd-slot crossbow (2551) to night equip of 68 & 1976
UPDATE creature_equip_template
SET ItemID3 = 2551
WHERE CreatureID IN (68, 1976) AND ID = 2;

-- 2. Add day + night equipment template for 37944 (elite patroller)
DELETE FROM creature_equip_template WHERE CreatureID = 37944 AND ID IN (1, 2);
INSERT INTO creature_equip_template (CreatureID, ID, ItemID1, ItemID2, ItemID3, VerifiedBuild) VALUES
  (37944, 1, 1899, 143, 0, 0),
  (37944, 2, 2715, 143, 2551, 0);

-- 3. Map 37944's 2 spawns to Nights event (29) so the night equip triggers.
--    (4.3.4 mapped these as modelid=0, equipment_id=2 in event 25)
DELETE FROM game_event_model_equip WHERE guid IN (110111, 110112) AND eventEntry = 29;
INSERT INTO game_event_model_equip (eventEntry, guid, modelid, equipment_id) VALUES
  (29, 110111, 0, 2),
  (29, 110112, 0, 2);
