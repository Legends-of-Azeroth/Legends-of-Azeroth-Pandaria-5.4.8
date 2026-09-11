-- Quest 14222 "Last Stand" - Remove incorrect OOC attack scripts
-- Keep only Northgate Rebel (41015) attacking Frenzied Stalker (35627), matching TC 4.3.4

DELETE FROM smart_scripts
WHERE entryorguid = 35627
  AND source_type = 0
  AND event_type = 1
  AND action_type = 49
  AND target_type = 11
  AND target_param1 = 41015;

DELETE FROM smart_scripts
WHERE entryorguid = 35566
  AND source_type = 0
  AND event_type = 1
  AND action_type = 49
  AND target_type = 21
  AND target_param1 = 50;

UPDATE smart_scripts
SET link = 1,
    event_flags = 1
WHERE entryorguid = 35627
  AND source_type = 0
  AND id = 0
  AND event_type = 2
  AND action_type = 11
  AND target_type = 1;

UPDATE smart_scripts
SET link = 0,
    event_flags = 1,
    event_param2 = 30
WHERE entryorguid = 35627
  AND source_type = 0
  AND id = 1
  AND event_type = 61
  AND action_type = 1
  AND target_type = 1;

UPDATE smart_scripts
SET event_param4 = 2000
WHERE entryorguid = 41015
  AND source_type = 0
  AND id = 0
  AND event_type = 1
  AND action_type = 49
  AND target_type = 11
  AND target_param1 = 35627;
