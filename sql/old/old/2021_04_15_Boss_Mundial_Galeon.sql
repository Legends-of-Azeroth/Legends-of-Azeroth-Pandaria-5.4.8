-- anteriormente se habia borrado por un problema con el loot
-- se reparo los problemas con el loot y se modifico su tiempo de reaparición a 8 horas
DELETE FROM `ofi_world`.`creature` WHERE `guid` = '516616'; 
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) 
VALUES('516616','62346','870','5805','6387','1','1','0','0','-572.945','-102.899','156.181','1.61634','28800','0','0','0','0','0','0','0','0','0','0','0');
