
-- https://www.wowhead.com/npc=50998/stompy
DELETE FROM `creature` WHERE `id`=50998;

INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES
(507102, 50998, 870, 5841, 5841, 1, 1, 0, 0, 3774.33, 2135.97, 880.934, 233.813, 300, 0, 7, 0, 84, 0, 1, 0, 0, 0, 0, 0);
