-- agregando al sha de la ira y modificando su tiempo de reaparición a 24 horas
DELETE FROM `creature_template` WHERE `entry`=60491;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `difficulty_entry_4`, `difficulty_entry_5`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction_A`, `faction_H`, `npcflag`, `npcflag2`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Mana_mod_extra`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `VignetteID`, `TrackingQuestID`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) 
VALUES('60491','0','0','0','0','0','0','0','41448','0','0','0','Sha de la ira',NULL,NULL,'0','90','90','4','0','14','14','0','0','1','1.14286','1','1','11092','13115','0','43992','3','2800','3000','2','64','0','0','0','0','0','0','0','0','0','4','2097260','0','60491','0','0','0','0','0','0','0','0','119626','116202','47008','10','55078','113344','31803','45524','0','2189','3600','3700','','0','1','1','600','0.009511','1','1','0','0','0','0','0','0','0','0','1','0','32099','786382847','0','boss_sha_of_anger','18414');

DELETE FROM `creature` WHERE `guid` IN (564256, 564257, 564258, 564259, 564260);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) VALUES
('564256','60491','870','5841','6415','1','1','0','0','1958.73','1634.28','482.576','0.491481','86400','0','0','0','1','0','0','0','0','0','0','0'),
('564257','60491','870','5841','6415','1','1','0','0','2361.8','595.601','494.394','0.89169','86400','0','0','0','1','0','0','0','0','0','0','0'),
('564258','60491','870','5841','6090','1','1','0','0','2737.45','1711.46','647.72','0.249445','86400','0','0','0','1','0','0','0','0','0','0','0'),
('564259','60491','870','5841','6090','1','1','0','0','2924.01','1476','648.314','3.58546','86400','0','0','0','1','0','0','0','0','0','0','0'),
('564260','60491','870','5841','6152','1','1','0','0','2955.4','407.047','507.075','1.45169','86400','0','0','0','1','0','0','0','0','0','0','0');

DELETE FROM `pool_template` WHERE `entry`=255;
INSERT INTO `pool_template` (`entry`, `max_limit`, `description`) VALUES('255','1','Sha of Anger (60491)');

DELETE FROM `pool_creature` WHERE `guid` IN (564256, 564257, 564258, 564259, 564260);
INSERT INTO `pool_creature` (`guid`, `pool_entry`, `chance`, `description`) VALUES
('564256','255','0','Sha of Anger (60491)'),
('564257','255','0','Sha of Anger (60491)'),
('564258','255','0','Sha of Anger (60491)'),
('564259','255','0','Sha of Anger (60491)'),
('564260','255','0','Sha of Anger (60491)');

-- se modificando el tiempo de reaparición a 24 horas del jefe de mundo galeon
UPDATE `creature` SET `spawntimesecs` = '86400' WHERE `guid` = '516616'; 

