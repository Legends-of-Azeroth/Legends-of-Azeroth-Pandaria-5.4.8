UPDATE `access_requirement` SET `completed_achievement` = 6689 WHERE `mapId` = 996 AND (`difficulty` = 'RAID_10MAN_HEROIC' OR `difficulty` = 'RAID_25MAN_HEROIC');

DELETE FROM `trinity_string` WHERE `entry` IN (15000, 15001, 15002, 15003, 15004, 15005);
INSERT INTO `trinity_string` (`entry`, `BroadcastTextID`, `content_default`) VALUES 
(15000, 0, 'Debes completar Veranda de la Primavera Normal antes de entrar a su dificultad heroica'),
(15001, 0, 'Debes completar Corazón del Miedo en dificultad normal'),
(15002, 0, 'Debes completar Corazón del Miedo en dificultad heroica'),
(15003, 0, 'Debes completar Cámaras de Mogushan en dificultad normal'),
(15004, 0, 'Debes completar Cámaras de Mogushan en dificultad heroica');