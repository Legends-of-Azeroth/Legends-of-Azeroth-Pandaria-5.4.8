-- Creature Galeon - Id : 89783 , se corrigio el botín de este jefe de mundo para que solo de 1 por PJ
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 1 WHERE `entry` = 62346;
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 8 WHERE `entry` = 62346 AND item=89783;

-- agregando las traducciones de los tokens en la nueva tabla de traducción de items
DELETE FROM `locales_item` WHERE `ID` = '110001' AND `locale` = 'esMX'; 
DELETE FROM `locales_item` WHERE `ID` = '110001' AND `locale` = 'esES'; 
DELETE FROM `locales_item` WHERE `ID` = '110002' AND `Locale` = 'esES'; 
DELETE FROM `locales_item` WHERE `ID` = '110002' AND `locale` = 'esMX'; 
DELETE FROM `locales_item` WHERE `ID` = '110003' AND `locale` = 'esMX'; 
DELETE FROM `locales_item` WHERE `ID` = '110003' AND `locale` = 'esES'; 
DELETE FROM `locales_item` WHERE `ID` = '110004' AND `locale` = 'esES'; 
DELETE FROM `locales_item` WHERE `ID` = '110004' AND `locale` = 'esMX'; 
INSERT INTO `locales_item` (`ID`, `locale`, `Name`, `Description`, `VerifiedBuild`) 
VALUES
('110001','esMX','Coins 01','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 01 Torghast Coins)','1'),
('110001','esES','Coins 01','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 01 Torghast Coins)','1'),
('110002','esES','Coins 02','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 02 Torghast Coins)','1'),
('110002','esMX','Coins 02','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 02 Torghast Coins)','1'),
('110003','esMX','Coins 05','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 05 Torghast Coins)','1'),
('110003','esES','Coins 05','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 05 Torghast Coins)','1'),
('110004','esES','Coins 10','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 10 Torghast Coins)','1'),
('110004','esMX','Coins 10','Desde que Saitamapaz cayó en la locura, y quedó calvo, ha estado buscando cada uno de sus pelos alrededor del mundo. Tú acabas de hacerte con un puñado de ellos, ¿te atreverás a devolverlos? (valor: 10 Torghast Coins)','1');

-- cambiando el bonus de mundo de pandaria, el item Monster Bag - bolsa de 36 casillas por una bolsa de 22 casillas - Bolsa de seda de ascuas
UPDATE `world_drop_loot_template` SET `item` = '54443' WHERE `item` = '1100' AND `level_min` = '92' AND `level_max` = '92'; 
