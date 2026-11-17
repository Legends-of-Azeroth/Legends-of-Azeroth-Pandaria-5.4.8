-- se elimino las monturas de temporada anterior y se agrego una nueva
-- tambien se agregaron items custom a la sección de servicios

-- eliminando antiguas monturas de la temporada 2.0 de la tienda in-game
DELETE FROM `battle_pay_product_items` WHERE `id` IN (86,87,88,89,90); 
DELETE FROM `battle_pay_product` WHERE `id` IN (86,87,88,89,90); 
DELETE FROM `battle_pay_entry` WHERE `id` IN (86,87,88,89,90); 
DELETE FROM `locales_battle_pay_entry` WHERE `id` IN (86,87,88,89,90); 

-- agregando nueva categoria y eliminando la anterior
DELETE FROM `battle_pay_group` WHERE `id` = '12'; 
INSERT INTO `battle_pay_group` (`id`, `idx`, `name`, `icon`, `type`) VALUES ('12', '12', 'Patch 2.0', '939381', '0'); 

DELETE FROM `locales_battle_pay_group` WHERE `id` = '12'; 
INSERT INTO `locales_battle_pay_group` (`id`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_loc8`, `name_loc9`, `name_loc10`, `name_loc11`) 
VALUES ('12', 'Patch 2.0', NULL, NULL, NULL, NULL, 'Parche 2.0', 'Parche 2.0', NULL, NULL, NULL, NULL); 

-- agregando nuevas monturas del Parche 2.0 a la tienda
DELETE FROM `battle_pay_product_items` WHERE `id` IN (190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210);
INSERT INTO `battle_pay_product_items` (`id`, `itemId`, `count`, `productId`) 
VALUES 
('190', '33809', '1', '190'),
('191', '49096', '1', '191'),
('192', '49098', '1', '192'),
('193', '49282', '1', '193'),
('194', '49283', '1', '194'),
('195', '49284', '1', '195'),
('196', '54069', '1', '196'),
('197', '68008', '1', '197'),
('198', '68825', '1', '198'),
('199', '69846', '1', '199'),
('200', '71718', '1', '200'),
('201', '72582', '1', '201'),
('202', '76755', '1', '202'),
('203', '78924', '1', '203'),
('204', '85870', '1', '204'),
('205', '92724', '1', '205'),
('206', '93671', '1', '206'),
('207', '95341', '1', '207'),
('208', '97989', '1', '208'),
('209', '104011', '1', '209'),
('210', '107951', '1', '210');

DELETE FROM `battle_pay_product` WHERE `id` IN (190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210);
INSERT INTO `battle_pay_product` (`id`, `title`, `description`, `icon`, `price`, `discount`, `displayId`, `type`, `choiceType`, `flags`, `flagsInfo`) 
VALUES 
('190', 'Shop: Amani War Bear', 'Terrestrial mount', '132117', '15', '0','22464', '0', '1', '47', '0'), 
('191', 'Shop: Crusader\'s White Warhorse', 'Terrestrial mount', '132261', '25', '0','29937', '0', '1', '47', '0'), 
('192', 'Shop: Crusader\'s Black Warhorse', 'Terrestrial mount', '132251', '25', '0','29938', '0', '1', '47', '0'), 
('193', 'Shop: Big Battle Bear', 'Terrestrial mount', '132117', '25', '0','25335', '0', '1', '47', '0'), 
('194', 'Shop: Reins of the Spectral Tiger', 'Terrestrial mount', '298591', '40', '0','21973', '0', '1', '47', '0'), 
('195', 'Shop: Reins of the Swift Spectral Tiger', 'Terrestrial mount', '298591', '40', '0','21974', '0', '1', '47', '0'), 
('196', 'Shop: Blazing Hippogryph', 'Flying mount', '132265', '25', '0','31803', '0', '1', '47', '0'), 
('197', 'Shop: Mottled Drake', 'Flying mount', '236232', '25', '0','37231', '0', '1', '47', '0'), 
('198', 'Shop: Amani Dragonhawk', 'Flying mount', '132188', '25', '0','37800', '0', '1', '47', '0'), 
('199', 'Shop: Winged Guardian', 'Flying mount', '526356', '55', '0','38260', '0', '1', '47', '0'), 
('200', 'Shop: Swift Shorestrider', 'Terrestrial mount', '132198', '10', '0','17011', '0', '1', '47', '0'), 
('201', 'Shop: Corrupted Hippogryph', 'Flying mount', '132265', '25', '0','38972', '0', '1', '47', '0'), 
('202', 'Shop: Tyrael\'s Charger', 'Flying mount', '577318', '55', '0','39530', '0', '1', '47', '0'), 
('203', 'Shop: Heart of the Aspects', 'Flying mount', '576138', '55', '0','40029', '0', '1', '47', '0'), 
('204', 'Shop: Imperial Quilen', 'Flying mount', '644374', '40', '0','43254', '0', '1', '47', '0'), 
('205', 'Shop: Swift Windsteed', 'Terrestrial mount', '773250', '40', '0','46729', '0', '1', '47', '0'), 
('206', 'Shop: Ghastly Charger\'s Skull', 'Flying mount', '796703', '25', '0','48014', '0', '1', '47', '0'), 
('207', 'Shop: Armored Bloodwing', 'Flying mount', '804461', '55', '0','48020', '0', '1', '47', '0'), 
('208', 'Shop: Enchanted Fey Dragon', 'Flying mount', '841541', '55', '0','48714', '0', '1', '47', '0'), 
('209', 'Shop: Stormcrow', 'Flying mount', '897087', '55', '0','51323', '0', '1', '47', '0'), 
('210', 'Shop: Iron Skyreaver', 'Flying mount', '943739', '55', '0','53038', '0', '1', '47', '0');

DELETE FROM `battle_pay_entry` WHERE `id` IN (190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210);
INSERT INTO `battle_pay_entry` (`id`, `productId`, `groupId`, `idx`, `title`, `description`, `icon`, `displayId`, `banner`, `flags`) 
VALUES 
('190', '190', '12', '0','Amani War Bear', 'Terrestrial mount', '0', '0', '2', '0'),
('191', '191', '12', '0','Crusader\'s White Warhorse', 'Terrestrial mount', '0', '0', '2', '0'),
('192', '192', '12', '0','Crusader\'s Black Warhorse', 'Terrestrial mount', '0', '0', '2', '0'),
('193', '193', '12', '0','Big Battle Bear', 'Terrestrial mount', '0', '0', '2', '0'),
('194', '194', '12', '0','Reins of the Spectral Tiger', 'Terrestrial mount', '0', '0', '2', '0'),
('195', '195', '12', '0','Reins of the Swift Spectral Tiger', 'Terrestrial mount', '0', '0', '2', '0'),
('196', '196', '12', '0','Blazing Hippogryph', 'Flying mount', '0', '0', '2', '0'),
('197', '197', '12', '0','Mottled Drake', 'Flying mount', '0', '0', '2', '0'),
('198', '198', '12', '0','Amani Dragonhawk', 'Flying mount', '0', '0', '2', '0'),
('199', '199', '12', '0','Winged Guardian', 'Flying mount', '0', '0', '2', '0'),
('200', '200', '12', '0','Swift Shorestrider', 'Terrestrial mount', '0', '0', '2', '0'),
('201', '201', '12', '0','Corrupted Hippogryph', 'Flying mount', '0', '0', '2', '0'),
('202', '202', '12', '0','Tyrael\'s Charger', 'Flying mount', '0', '0', '2', '0'),
('203', '203', '12', '0','Heart of the Aspects', 'Flying mount', '0', '0', '2', '0'),
('204', '204', '12', '0','Imperial Quilen', 'Flying mount', '0', '0', '2', '0'),
('205', '205', '12', '0','Swift Windsteed', 'Terrestrial mount', '0', '0', '2', '0'),
('206', '206', '12', '0','Ghastly Charger\'s Skull', 'Flying mount', '0', '0', '2', '0'),
('207', '207', '12', '0','Armored Bloodwing', 'Flying mount', '0', '0', '2', '0'),
('208', '208', '12', '0','Enchanted Fey Dragon', 'Flying mount', '0', '0', '2', '0'),
('209', '209', '12', '0','Stormcrow', 'Flying mount', '0', '0', '2', '0'),
('210', '210', '12', '0','Iron Skyreaver', 'Flying mount', '0', '0', '2', '0');

DELETE FROM `locales_battle_pay_entry` WHERE `id` IN (190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210);
INSERT INTO `locales_battle_pay_entry` (`id`, `title_loc1`, `title_loc6`, `title_loc7`, `description_loc1`, `description_loc6`, `description_loc7`) 
VALUES 
('190', 'Amani War Bear', 'Oso de guerra Amani', 'Oso de guerra Amani', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('191', 'Crusader\'s White Warhorse', 'Caballo de guerra blanco de cruzado', 'Caballo de guerra blanco de cruzado', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('192', 'Crusader\'s Black Warhorse', 'Caballo de guerra negro de cruzado', 'Caballo de guerra negro de cruzado', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('193', 'Big Battle Bear', 'Oso de batalla grande', 'Oso de batalla grande', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('194', 'Reins of the Spectral Tiger', 'Riendas del tigre espectral', 'Riendas del tigre espectral', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('195', 'Reins of the Swift Spectral Tiger', 'Riendas del tigre espectral presto', 'Riendas del tigre espectral presto', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('196', 'Blazing Hippogryph', 'Hipogrifo llameante', 'Hipogrifo llameante', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('197', 'Mottled Drake', 'Draco jaspeado', 'Draco jaspeado', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('198', 'Amani Dragonhawk', 'Dracohalcón Amani', 'Dracohalcón Amani', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('199', 'Winged Guardian', 'Guardián alado', 'Guardián alado', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('200', 'Swift Shorestrider', 'Zancaorillas presto', 'Zancaorillas presto', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('201', 'Corrupted Hippogryph', 'Hipogrifo corrupto', 'Hipogrifo corrupto', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('202', 'Tyrael\'s Charger', 'Destrero de Tyrael', 'Destrero de Tyrael', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('203', 'Heart of the Aspects', 'Corazón de los Aspectos', 'Corazón de los Aspectos', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('204', 'Imperial Quilen', 'Quilen imperial', 'Quilen imperial', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('205', 'Swift Windsteed', 'Corcel del viento presto', 'Corcel del viento presto', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('206', 'Ghastly Charger\'s Skull', 'Calavera de destrero espantoso', 'Calavera de destrero espantoso', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('207', 'Armored Bloodwing', 'Alasangre acorazado', 'Alasangre acorazado', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('208', 'Enchanted Fey Dragon', 'Dragón hada encantado', 'Dragón hada encantado', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('209', 'Stormcrow', 'Cuervo tormentoso', 'Cuervo tormentoso', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('210', 'Iron Skyreaver', 'Atracacielos férreo', 'Atracacielos férreo', 'Flying mount', 'Montura Voladora', 'Montura Voladora');



-- agregando los items custom de justicia, valor y conquista a la tienda del juego
DELETE FROM `battle_pay_product_items` WHERE `id` IN ('94','95','96');
INSERT INTO `battle_pay_product_items` (`id`, `itemId`, `count`, `productId`) VALUES 
('94', '110006', '1', '94'),
('95', '110007', '1', '95'),
('96', '110008', '1', '96');

DELETE FROM `battle_pay_product` WHERE `id` IN ('94','95','96');
INSERT INTO `battle_pay_product` (`id`, `title`, `description`, `icon`, `price`, `discount`, `displayId`, `type`, `choiceType`, `flags`, `flagsInfo`) VALUES 
('94', 'Shop: 1000 justice points', 'Grants you 1000 justice points', '135884', '5', '0', '0', '0', '1', '47', '0'),
('95', 'Shop: 1000 valor points', 'Grants you 1000 valor points', '135884', '10', '0', '0', '0', '1', '47', '0'),
('96', 'Shop: 1000 conquest points', 'Grants you 1000 conquest points', '135884', '10', '0', '0', '0', '1', '47', '0'); 

DELETE FROM `battle_pay_entry` WHERE `id` IN ('94','95','96');
INSERT INTO `battle_pay_entry` (`id`, `productId`, `groupId`, `idx`, `title`, `description`, `icon`, `displayId`, `banner`, `flags`) VALUES 
('94', '94', '11', '0', '1000 justice points', 'Grants you 1000 justice points', '0', '0', '2', '0'),
('95', '95', '11', '0', '1000 valor points', 'Grants you 1000 valor points', '0', '0', '2', '0'),
('96', '96', '11', '0', '1000 conquest points', 'Grants you 1000 conquest points', '0', '0', '2', '0'); 

-- agregando traducciones
DELETE FROM `locales_battle_pay_entry` WHERE `id` IN ('94','95','96');
INSERT INTO `locales_battle_pay_entry` (`id`, `title_loc1`, `title_loc2`, `title_loc3`, `title_loc4`, `title_loc5`, `title_loc6`, `title_loc7`, `title_loc8`, `title_loc9`, `title_loc10`, `title_loc11`, `description_loc1`, `description_loc2`, `description_loc3`, `description_loc4`, `description_loc5`, `description_loc6`, `description_loc7`, `description_loc8`, `description_loc9`, `description_loc10`, `description_loc11`) VALUES 
('94', '|cffa335ee1000 justice points', NULL, NULL, NULL, NULL, '|cffa335ee1000 Puntos de justicia', '|cffa335ee1000 Puntos de justicia', NULL, NULL, NULL, NULL, 'Grants you 1000 justice points.', NULL, NULL, NULL, NULL, 'Te otorga 1000 puntos de justicia.', 'Te otorga 1000 puntos de justicia.', NULL, NULL, NULL, NULL), 
('95', '|cffa335ee1000 valor points', NULL, NULL, NULL, NULL, '|cffa335ee1000 Puntos de valor', '|cffa335ee1000 Puntos de valor', NULL, NULL, NULL, NULL, 'Grants you 1000 valor points.', NULL, NULL, NULL, NULL, 'Te otorga 1000 puntos de valor.', 'Te otorga 1000 puntos de valor.', NULL, NULL, NULL, NULL),
('96', '|cffa335ee1000 conquest points', NULL, NULL, NULL, NULL, '|cffa335ee1000 Puntos de conquista', '|cffa335ee1000 Puntos de conquista', NULL, NULL, NULL, NULL, 'Grants you 1000 conquest points.', NULL, NULL, NULL, NULL, 'Te otorga 1000 puntos de conquista.', 'Te otorga 1000 puntos de conquista.', NULL, NULL, NULL, NULL); 

-- actualizando el precio del item custom de honor
UPDATE `battle_pay_product` SET `price` = '5' WHERE `id` = '93'; 
