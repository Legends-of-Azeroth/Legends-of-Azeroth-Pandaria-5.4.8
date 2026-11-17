-- Starting zone Training Dummies: use npc_training_dummy like the other Training Dummies.
-- Their SmartAI gave the spell practice credit but never left combat (it only evaded while the attacker was 20-25 yards away),
-- so attackers stayed in combat. npc_training_dummy drops combat with each attacker 5 seconds after their last hit and now gives
-- the same spell practice credit.
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_training_dummy' WHERE `entry` IN (44171, 44389, 44548, 44614, 44703, 44794, 44820, 44848, 44937, 48304);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (44171, 44389, 44548, 44614, 44703, 44794, 44820, 44848, 44937, 48304) AND `source_type` = 0;

-- The Archery Targets (5202) in Durotar were spawned with random movement and wandered around. They are target dummies and stand
-- still, like the other Archery Target spawns.
UPDATE `creature` SET `MovementType` = 0, `wander_distance` = 0 WHERE `guid` IN (255194, 255197, 255272, 255290);
