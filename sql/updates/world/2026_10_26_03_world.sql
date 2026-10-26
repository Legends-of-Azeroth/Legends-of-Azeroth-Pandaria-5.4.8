-- Quest turn-in scripts that still started on the pre-Cataclysm version of their quest, so they never ran

-- Sarkoth (25129, was 790): Hana'zua (3287), lying wounded, sits up, kneels and sends the player off, then lies back down
UPDATE `smart_scripts` SET `event_param1` = 25129, `comment` = 'Hana''zua - On Quest ''Sarkoth'' Finished - Remove Bytes 1 Dead'
WHERE `entryorguid` = 3287 AND `source_type` = 0 AND `id` = 0;

-- Samophlange (29024, was 902) and Nugget Slugs (29027, was 3922): Sputtervalve (3442) and Wenikee Boltbucket (9316) try to fix the
-- Samophlange and break it. Their actionlists also respawned the Ratchet Samophlange (now a copper vein's guid); the ones at their
-- camps are always spawned, and setting a respawn time on those would despawn them, so that step is gone.
UPDATE `smart_scripts` SET `event_param1` = 29024, `comment` = 'Sputtervalve - On Quest ''Samophlange'' Finished - Run Script'
WHERE `entryorguid` = 3442 AND `source_type` = 0 AND `id` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 344200 AND `source_type` = 9 AND `id` = 1;
UPDATE `smart_scripts` SET `event_param1` = 2000, `event_param2` = 2000 WHERE `entryorguid` = 344200 AND `source_type` = 9 AND `id` = 2;

UPDATE `smart_scripts` SET `event_param1` = 29027, `comment` = 'Wenikee Boltbucket - On Quest ''Nugget Slugs'' Finished - Run Script'
WHERE `entryorguid` = 9316 AND `source_type` = 0 AND `id` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 931600 AND `source_type` = 9 AND `id` = 1;
-- He opened with his closing line ("... oops.") instead of "Ok, let's get started."
UPDATE `smart_scripts` SET `action_param1` = 0, `comment` = 'Wenikee Boltbucket - On Script - Say Line 0'
WHERE `entryorguid` = 931600 AND `source_type` = 9 AND `id` = 0;
UPDATE `smart_scripts` SET `event_param1` = 2000, `event_param2` = 2000 WHERE `entryorguid` = 931600 AND `source_type` = 9 AND `id` = 2;
