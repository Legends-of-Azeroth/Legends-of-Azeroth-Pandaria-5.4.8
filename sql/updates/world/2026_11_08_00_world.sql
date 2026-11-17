-- Eversong Woods, "The Dwarven Spy" (8483): Prospector Anvilward (15420) turns hostile at waypoint 7 but never attacks the
-- player who lured him; he waits until he is hit. His faction and gossip flag are also not restored if the fight is abandoned.
-- Cause: waypoint 7 only runs Set Faction 24 and nothing starts combat. Waypoint events have no action invoker, so the
-- player who picked the gossip option is stored (Store Target List, var 1) and attacked at waypoint 7.
-- Only the exact original rows are touched. If the script differs from the original one, nothing is changed.

-- Move the "Out of Combat - Say Line 2" row from id 8 to id 9 to make room for the attack row.
UPDATE `smart_scripts` SET `id` = 9 WHERE `entryorguid` = 15420 AND `source_type` = 0 AND `id` = 8 AND `link` = 0
  AND `event_type` = 1 AND `action_type` = 1 AND `action_param1` = 2
  AND NOT EXISTS (SELECT 1 FROM (SELECT DISTINCT 1 FROM `smart_scripts` WHERE `entryorguid` = 15420 AND `source_type` = 0 AND `id` = 9) AS `t`);
-- Only when the move above happened: chain the new rows.
UPDATE `smart_scripts` SET `link` = 12 WHERE `entryorguid` = 15420 AND `source_type` = 0 AND `id` = 2 AND `link` = 0 AND `action_type` = 83
  AND EXISTS (SELECT 1 FROM (SELECT DISTINCT 1 FROM `smart_scripts` WHERE `entryorguid` = 15420 AND `source_type` = 0 AND `id` = 9 AND `event_type` = 1) AS `t`);
UPDATE `smart_scripts` SET `link` = 8 WHERE `entryorguid` = 15420 AND `source_type` = 0 AND `id` = 7 AND `link` = 0 AND `action_type` = 2 AND `action_param1` = 24
  AND EXISTS (SELECT 1 FROM (SELECT DISTINCT 1 FROM `smart_scripts` WHERE `entryorguid` = 15420 AND `source_type` = 0 AND `id` = 9 AND `event_type` = 1) AS `t`);

INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`)
SELECT * FROM (
SELECT 15420 AS `entryorguid`, 0 AS `source_type`, 8 AS `id`, 0 AS `link`, 61 AS `event_type`, 0 AS `event_phase_mask`, 100 AS `event_chance`, 0 AS `event_flags`, 0 AS `event_param1`, 0 AS `event_param2`, 0 AS `event_param3`, 0 AS `event_param4`, 0 AS `event_param5`, 49 AS `action_type`, 0 AS `action_param1`, 0 AS `action_param2`, 0 AS `action_param3`, 0 AS `action_param4`, 0 AS `action_param5`, 0 AS `action_param6`, 12 AS `target_type`, 1 AS `target_param1`, 0 AS `target_param2`, 0 AS `target_param3`, 0 AS `target_param4`, 0 AS `target_x`, 0 AS `target_y`, 0 AS `target_z`, 0 AS `target_o`, 'Prospector Anvilward - On Waypoint 7 Reached - Start Attacking Stored Target' AS `comment`
UNION ALL
SELECT 15420, 0, 10, 11, 25, 0, 100, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Prospector Anvilward - On Reset - Set Faction Default'
UNION ALL
SELECT 15420, 0, 11, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 82, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Prospector Anvilward - On Reset - Add Npc Flag Gossip'
UNION ALL
SELECT 15420, 0, 12, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 'Prospector Anvilward - On Gossip Option 0 Selected - Store Target List (invoker)'
UNION ALL
SELECT 15420, 0, 13, 14, 7, 0, 100, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Prospector Anvilward - On Evade - Set Faction Default'
UNION ALL
SELECT 15420, 0, 14, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 82, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Prospector Anvilward - On Evade - Add Npc Flag Gossip'
) AS `new_rows`
WHERE EXISTS (SELECT 1 FROM `smart_scripts` AS `s` WHERE `s`.`entryorguid` = 15420 AND `s`.`source_type` = 0 AND `s`.`id` = 7 AND `s`.`link` = 8)
  AND EXISTS (SELECT 1 FROM `smart_scripts` AS `s` WHERE `s`.`entryorguid` = 15420 AND `s`.`source_type` = 0 AND `s`.`id` = 2 AND `s`.`link` = 12)
  AND NOT EXISTS (SELECT 1 FROM `smart_scripts` AS `s` WHERE `s`.`entryorguid` = 15420 AND `s`.`source_type` = 0 AND `s`.`id` IN (8, 10, 11, 12, 13, 14));
