
DELETE FROM `disables` WHERE `sourceType`=11 AND `entry` IN (1283,1277,1275,1276,1282,1273,1278,1280,1279,1281);
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES 
(11, 1283, 0, '', '', 'Granjero Fung'),
(11, 1277, 0, '', '', 'Chee Chee'),
(11, 1275, 0, '', '', 'Ella'),
(11, 1276, 0, '', '', 'Viejo Zarpa Collado'),
(11, 1282, 0, '', '', 'Pez Junco Talado'),
(11, 1273, 0, '', '', 'Jogu el Ebrio'),
(11, 1278, 0, '', '', 'Sho'),
(11, 1280, 0, '', '', 'Tina Zarpa Fangosa'),
(11, 1279, 0, '', '', 'Haohan Zarpa Fangosa'),
(11, 1281, 0, '', '', '\r\nGina Zarpa Fangosa');

-- 514096 guid de la creatura eliminada
-- 516066,516067 de la creatura id 58646
DELETE FROM `creature` WHERE entry IN (64231,58646);


