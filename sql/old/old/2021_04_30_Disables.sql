-- desactivando laz mazmorras de scholomance y cámaras escarlata en modo normal y heroico
DELETE FROM `disables` WHERE `sourceType` = '2' AND `entry` = '1007'; 
DELETE FROM `disables` WHERE `sourceType` = '2' AND `entry` = '1001'; 
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) 
VALUES 
('2', '1007', '3', '0', '0', 'Disabled Scholomance'),
('2', '1001', '3', '0', '0', 'Disabled Scarlet Halls');

-- agregando requisito de nivel de equipo para apuntar al buscdor de bandas, ahora necesitan nivel 600
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '527'; -- old requiredItemLevel: 460 , guardianes de mogushan
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '528'; -- old requiredItemLevel: 460 , la camara de los misterios
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '529'; -- old requiredItemLevel: 470 , un enfoque aterrador
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '530'; -- old requiredItemLevel: 470 , pesadilla de shekzeer
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '526'; -- old requiredItemLevel: 470 , veranda de la primavera eterna
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '610'; -- old requiredItemLevel: 480 , la carga de la brigaba zandalari
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '611'; -- old requiredItemLevel: 480 , avismo de la desidia
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '612'; -- old requiredItemLevel: 480 , camaras del modelado de carne
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '613'; -- old requiredItemLevel: 480 , pinaculos de la tormenta
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '716'; -- old requiredItemLevel: 496 , valle de la pena eterna
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '717'; -- old requiredItemLevel: 496 , las puertas de la venganza
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '724'; -- old requiredItemLevel: 496 , El bunker
UPDATE `lfg_dungeon_template` SET `requiredItemLevel` = '600' WHERE `dungeonId` = '725'; -- old requiredItemLevel: 496 , El ocaso
