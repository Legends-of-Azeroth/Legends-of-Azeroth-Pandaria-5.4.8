-- Add FLAG_IDLE_IN_FORMATION (0x200 = 512) to all formation members
-- so LeaderStartedMoving() will trigger FormationMovementGenerator
-- for followers when the leader starts pathing.
UPDATE `creature_formations` SET `groupAI` = `groupAI` | 512 WHERE `groupAI` & 512 = 0;
