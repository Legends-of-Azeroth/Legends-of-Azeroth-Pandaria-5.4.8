-- agregando nueva categoría para las monturas de temporada
DELETE FROM `battle_pay_group` WHERE `id` = '12'; 
INSERT INTO `battle_pay_group` (`id`, `idx`, `name`, `icon`, `type`) VALUES ('12', '12', 'Season 2', '939381', '0'); 

-- agregando items
DELETE FROM `battle_pay_product_items` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `battle_pay_product_items` (`id`, `itemId`, `count`, `productId`) 
VALUES 
('86', '85870', '1', '86'),
('87', '92724', '1', '87'),
('88', '19872', '1', '88'), 
('89', '54068', '1', '89'), 
('90', '49285', '1', '90'); 

DELETE FROM `battle_pay_product` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `battle_pay_product` (`id`, `title`, `description`, `icon`, `price`, `discount`, `displayId`, `type`, `choiceType`, `flags`, `flagsInfo`) 
VALUES 
('86', 'Shop: Imperial Quilen', 'Flying mount', '644374', '55', '0', '43254', '0', '1', '47', '0'), 
('87', 'Shop: Swift Windsteed', 'Flying mount', '773250', '55', '0', '46729', '0', '1', '47', '0'), 
('88', 'Shop: Swift Razzashi Raptor', 'Terrestrial mount', '132253', '27', '0', '15289', '0', '1', '47', '0'), 
('89', 'Shop: Wooly White Rhino', 'Terrestrial mount', '236194', '27', '0', '31721', '0', '1', '47', '0'),
('90', 'Shop: X-51 Nether-Rocket', 'Flying mount', '134282', '30', '0', '23656', '0', '1', '47', '0'); 

DELETE FROM `battle_pay_entry` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `battle_pay_entry` (`id`, `productId`, `groupId`, `idx`, `title`, `description`, `icon`, `displayId`, `banner`, `flags`) 
VALUES 
('86', '86', '12', '0', 'Imperial Quilen', 'Flying mount', '0', '0', '2', '0'),
('87', '87', '12', '0', 'Swift Windsteed', 'Flying mount', '0', '0', '2', '0'),
('88', '88', '12', '0', 'Swift Razzashi Raptor', 'Terrestrial mount', '0', '0', '2', '0'),
('89', '89', '12', '0', 'Wooly White Rhino', 'Terrestrial mount', '0', '0', '2', '0'),
('90', '90', '12', '0', 'X-51 Nether-Rocket', 'Flying mount', '0', '0', '2', '0');

-- traducciones
DELETE FROM `locales_battle_pay_group` WHERE `id` = '12'; 
INSERT INTO `locales_battle_pay_group` (`id`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_loc8`, `name_loc9`, `name_loc10`, `name_loc11`) 
VALUES ('12', 'Season 02', NULL, NULL, NULL, NULL, 'Temporada 02', 'Temporada 02', NULL, NULL, NULL, NULL); 

DELETE FROM `locales_battle_pay_entry` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `locales_battle_pay_entry` (`id`, `title_loc1`, `title_loc6`, `title_loc7`, `description_loc1`, `description_loc6`, `description_loc7`) 
VALUES 
('86', 'Imperial Quilen', 'Quilen imperial', 'Quilen imperial', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('87', 'Swift Windsteed', 'Corcel del viento presto', 'Corcel del viento presto', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('88', 'Swift Razzashi Raptor', 'Raptor Razzashi presto', 'Raptor Razzashi presto', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('89', 'Wooly White Rhino', 'Rinoceronte blanco lanudo', 'Rinoceronte blanco lanudo', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('90', 'X-51 Nether-Rocket', 'Cohete abisal X-51', 'Cohete abisal X-51', 'Flying mount', 'Montura Voladora', 'Montura Voladora'); 
