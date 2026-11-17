-- agregando los items custom de honor y el boost de profesiones a la tienda del juego
DELETE FROM `battle_pay_product_items` WHERE `id` IN ('91','92','93');
INSERT INTO `battle_pay_product_items` (`id`, `itemId`, `count`, `productId`) VALUES 
('91', '110100', '1', '91'),
('92', '110101', '1', '92'),
('93', '110005', '1', '93');

DELETE FROM `battle_pay_product` WHERE `id` IN ('91','92','93');
INSERT INTO `battle_pay_product` (`id`, `title`, `description`, `icon`, `price`, `discount`, `displayId`, `type`, `choiceType`, `flags`, `flagsInfo`) VALUES 
('91', 'Shop: Profession Boost', 'Grants you a level 600 profession', '134939', '10', '0', '0', '0', '1', '47', '0'),
('92', 'Shop: Minor Profession Boost', 'Boost your profession to the maximum available (75, 150, 225, 300, 375, 450, 525 or 600 points)', '134939', '2', '0', '0', '0', '1', '47', '0'),
('93', 'Shop: 1000 Honor Points', 'Grants you 1000 Honor points', '135884', '2', '0', '0', '0', '1', '47', '0'); 

DELETE FROM `battle_pay_entry` WHERE `id` IN ('91','92','93');
INSERT INTO `battle_pay_entry` (`id`, `productId`, `groupId`, `idx`, `title`, `description`, `icon`, `displayId`, `banner`, `flags`) VALUES 
('91', '91', '11', '0', 'Profession Boost', 'Grants you a level 600 profession', '0', '0', '2', '0'),
('92', '92', '11', '0', 'Minor Profession Boost', 'Boost your profession to the maximum available (75, 150, 225, 300, 375, 450, 525 or 600 points)', '0', '0', '2', '0'),
('93', '93', '11', '0', '1000 Honor Points', 'Grants you 1000 Honor points', '0', '0', '2', '0'); 

-- agregando traducciones
DELETE FROM `locales_battle_pay_entry` WHERE `id` IN ('91','92','93');
INSERT INTO `locales_battle_pay_entry` (`id`, `title_loc1`, `title_loc2`, `title_loc3`, `title_loc4`, `title_loc5`, `title_loc6`, `title_loc7`, `title_loc8`, `title_loc9`, `title_loc10`, `title_loc11`, `description_loc1`, `description_loc2`, `description_loc3`, `description_loc4`, `description_loc5`, `description_loc6`, `description_loc7`, `description_loc8`, `description_loc9`, `description_loc10`, `description_loc11`) VALUES 
('91', '|cffa335eeProfession Boost', NULL, NULL, NULL, NULL, '|cffa335eeImpulso de Profesión', '|cffa335eeImpulso de Profesión', NULL, NULL, NULL, NULL, 'Grants you a level 600 profession.', NULL, NULL, NULL, NULL, 'Te otorga una profesión de nivel 600.', 'Te otorga una profesión de nivel 600.', NULL, NULL, NULL, NULL), 
('92', '|cffa335eeMinor Profession Boost', NULL, NULL, NULL, NULL, '|cffa335eeImpulso de Profesión Menor', '|cffa335eeImpulso de Profesión Menor', NULL, NULL, NULL, NULL, 'Boost your profession to the maximum available (75, 150, 225, 300, 375, 450, 525 or 600 points).', NULL, NULL, NULL, NULL, 'Impulsa tu profesión hasta el máximo disponible(75, 150, 225, 300, 375, 450, 525 o 600 puntos).', 'Impulsa tu profesión hasta el máximo disponible(75, 150, 225, 300, 375, 450, 525 o 600 puntos).', NULL, NULL, NULL, NULL),
('93', '|cffa335ee1000 Honor Points', NULL, NULL, NULL, NULL, '|cffa335ee1000 Puntos de Honor', '|cffa335ee1000 Puntos de Honor', NULL, NULL, NULL, NULL, 'Grants you 1000 Honor points.', NULL, NULL, NULL, NULL, 'Te otorga 1000 puntos de honor.', 'Te otorga 1000 puntos de honor', NULL, NULL, NULL, NULL); 
