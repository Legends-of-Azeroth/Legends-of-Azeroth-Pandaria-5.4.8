-- agregando items de la temporada 12 a los vendedores de honor faltantes
DELETE FROM `npc_vendor` WHERE `entry` = '66001' AND `item` = '84711'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66001' AND `item` = '84712'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66001' AND `item` = '84713'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66001' AND `item` = '84714'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66001' AND `item` = '84715'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66001' AND `item` = '84484'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66049' AND `item` = '84711'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66049' AND `item` = '84712'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66049' AND `item` = '84713'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66049' AND `item` = '84714'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66049' AND `item` = '84715'; 
DELETE FROM `npc_vendor` WHERE `entry` = '66049' AND `item` = '84484'; 

-- honor horda: 66001
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`, `type`) 
VALUES 
('66001', '0', '84711', '0', '0', '3879', '1'),
('66001', '0', '84712', '0', '0', '3879', '1'),
('66001', '0', '84713', '0', '0', '3879', '1'),
('66001', '0', '84714', '0', '0', '3879', '1'),
('66001', '0', '84715', '0', '0', '3879', '1'),
('66001', '0', '84484', '0', '0', '3879', '1'),
-- honor alianza: 66049
('66049', '0', '84711', '0', '0', '3879', '1'),
('66049', '0', '84712', '0', '0', '3879', '1'),
('66049', '0', '84713', '0', '0', '3879', '1'),
('66049', '0', '84714', '0', '0', '3879', '1'),
('66049', '0', '84715', '0', '0', '3879', '1'),
('66049', '0', '84484', '0', '0', '3879', '1');
