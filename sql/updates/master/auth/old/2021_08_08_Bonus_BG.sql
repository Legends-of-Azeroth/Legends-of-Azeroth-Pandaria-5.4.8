-- Eliminando bonos de BG, items: Tier 5 Hunter Test Gear Box 2 y Tier 5 Mage Test Gear
DELETE FROM `config` WHERE `name` = 'Battleground.Rewards.Enabled' AND `realmid` = '1'; 
DELETE FROM `config` WHERE `name` = 'ICore.RichPvPRewards.Enabled' AND `realmid` = '1'; 

-- Estas variables de configuración no están presentes en el archivo de configuración global ni la tabla config de la DB AUTH
insert into `config` (`category`, `name`, `value`, `default`, `realmid`, `description`, `note`) values
('Battlegrounds','Battleground.Rewards.Enabled','0','1','1','Battlegrounds bonus',NULL),
('Battlegrounds','ICore.RichPvPRewards.Enabled','0','1','1','Battlegrounds bonus',NULL);

-- En el futuro se pueden usar estos bonus para un evento y/o misiones PVP
-- actualmente los items mencionados llegan por correo y no contienen nada
