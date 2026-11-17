-- Elimando el botín de los patrones de diseño que no corresponden al parche actual
/* 
 Patrón: paño celestial y sus usos - item = 100863
 Diseño: lingote de trillium equilibrado y sus usos - item = 100865
 Patrón: pellejo magnífico endurecido y sus usos item = 100864

 Estos pàtrones de Diseño Corresponden al Parche 5.4
Botín mundial
insert into `world_drop_loot_template` (`item`, `chance`, `expansion`, `level_min`, `level_max`, `group`, `info`) 
values
('100863','3','4','85','92','0',''),
('100864','3','4','85','92','0',''),
('100865','3','4','85','92','0','');

Botin de regular de los npc de la isla del trueno
insert into `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) values
('67473','100864','0.00191418','REGULAR','0','1','1'),
('69228','100864','0.00533362','REGULAR','0','1','1');
insert into `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) values
('67473','100865','0.00306268','REGULAR','0','1','1'),
('69216','100865','0.00162372','REGULAR','0','1','1'),
('69236','100865','0.00608445','REGULAR','0','1','1'),
('69461','100865','0.0239808','REGULAR','0','1','1'),
('70069','100865','0.00140652','REGULAR','0','1','1'),
('70588','100865','0.0347283','REGULAR','0','1','1');
insert into `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) values
('58778','100863','0.0162','','0','1','1'),
('69171','100863','0.00352526','REGULAR','0','1','1'),
('69226','100863','0.00678979','REGULAR','0','1','1'),
('69235','100863','0.00244587','REGULAR','0','1','1'),
('69264','100863','0.00223469','REGULAR','0','1','1'),
('70069','100863','0.00281304','REGULAR','0','1','1');
*/

DELETE FROM `world_drop_loot_template` WHERE `item` IN (100863, 100864, 100865);
DELETE FROM `creature_loot_template` WHERE `entry` = '67473' AND `item` = '100864' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69228' AND `item` = '100864' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '67473' AND `item` = '100865' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69216' AND `item` = '100865' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69236' AND `item` = '100865' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69461' AND `item` = '100865' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '70069' AND `item` = '100865' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '70588' AND `item` = '100865' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '58778' AND `item` = '100863' AND `lootmode` = ''; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69171' AND `item` = '100863' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69226' AND `item` = '100863' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69235' AND `item` = '100863' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '69264' AND `item` = '100863' AND `lootmode` = 'REGULAR'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '70069' AND `item` = '100863' AND `lootmode` = 'REGULAR'; 

