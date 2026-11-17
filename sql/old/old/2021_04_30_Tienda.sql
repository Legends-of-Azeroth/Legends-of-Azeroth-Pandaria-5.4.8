-- agregando nueva categoría para las monturas de temporada
DELETE FROM `battle_pay_group` WHERE `id` = '12'; 
INSERT INTO `battle_pay_group` (`id`, `idx`, `name`, `icon`, `type`) VALUES ('12', '12', 'Season 1', '939381', '0'); 

-- agregando items
DELETE FROM `battle_pay_product_items` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `battle_pay_product_items` (`id`, `itemId`, `count`, `productId`) 
VALUES 
('86', '97989', '1', '86'),
('87', '107951', '1', '87'),
('88', '19902', '1', '88'), 
('89', '49290', '1', '89'), 
('90', '54811', '1', '90'); 

DELETE FROM `battle_pay_product` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `battle_pay_product` (`id`, `title`, `description`, `icon`, `price`, `discount`, `displayId`, `type`, `choiceType`, `flags`, `flagsInfo`) 
VALUES 
('86', 'Shop: Enchanted Fey Dragon', 'Flying mount', '841541', '55', '0', '48714', '0', '1', '47', '0'), 
('87', 'Shop: Iron Skyreaver', 'Flying mount', '943739', '55', '0', '53038', '0', '1', '47', '0'), 
('88', 'Shop: Swift Zulian Tiger', 'Terrestrial mount', '132242', '33', '0', '15290', '0', '1', '47', '0'), 
('89', 'Shop: Magic Rooster', 'Terrestrial mount', '132834', '33', '0', '29344', '0', '1', '47', '0'),
('90', 'Shop: Celestial Steed', 'Terrestrial mount', '369225', '36', '0', '31958', '0', '1', '47', '0'); 

DELETE FROM `battle_pay_entry` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `battle_pay_entry` (`id`, `productId`, `groupId`, `idx`, `title`, `description`, `icon`, `displayId`, `banner`, `flags`) 
VALUES 
('86', '86', '12', '0', 'Enchanted Fey Dragon', 'Flying mount', '0', '0', '2', '0'),
('87', '87', '12', '0', 'Iron Skyreaver', 'Flying mount', '0', '0', '2', '0'),
('88', '88', '12', '0', 'Swift Zulian Tiger', 'Terrestrial mount', '0', '0', '2', '0'),
('89', '89', '12', '0', 'Magic Rooster', 'Terrestrial mount', '0', '0', '2', '0'),
('90', '90', '12', '0', 'Celestial Steed', 'Terrestrial mount', '0', '0', '2', '0'); 

-- traducciones
DELETE FROM `locales_battle_pay_group` WHERE `id` = '12'; 
INSERT INTO `locales_battle_pay_group` (`id`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_loc8`, `name_loc9`, `name_loc10`, `name_loc11`) 
VALUES ('12', 'Season 01', NULL, NULL, NULL, NULL, 'Temporada 01', 'Temporada 01', NULL, NULL, NULL, NULL); 

DELETE FROM `locales_battle_pay_entry` WHERE `id` IN (86,87,88,89,90); 
INSERT INTO `locales_battle_pay_entry` (`id`, `title_loc1`, `title_loc6`, `title_loc7`, `description_loc1`, `description_loc6`, `description_loc7`) 
VALUES 
('86', 'Enchanted Fey Dragon', 'Dragón hada encantado', 'Dragón hada encantado', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('87', 'Iron Skyreaver', 'Atracacielos férreo', 'Atracacielos férreo', 'Flying mount', 'Montura Voladora', 'Montura Voladora'),
('88', 'Swift Zulian Tiger', 'Tigre Zulian presto', 'Tigre Zulian presto', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('89', 'Magic Rooster', 'Huevo de gallo mágico', 'Huevo de gallo mágico', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'),
('90', 'Celestial Steed', 'Corcel celestial', 'Corcel celestial', 'Terrestrial mount', 'Montura Terrestre', 'Montura Terrestre'); 
