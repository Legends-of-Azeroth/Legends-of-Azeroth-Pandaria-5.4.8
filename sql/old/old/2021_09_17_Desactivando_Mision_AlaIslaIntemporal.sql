-- desactivando misión de teletransporte a la isla intemporal
DELETE FROM `disables` WHERE `sourceType` = '1' AND `entry` = '33231'; 
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) 
VALUES ('1', '33231', '0', '', '', 'quest: Viaje a la Isla Intemporal'); 
