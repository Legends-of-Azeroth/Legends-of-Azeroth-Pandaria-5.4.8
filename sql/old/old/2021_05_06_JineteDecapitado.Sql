-- agregando GUID del Jinete decapitado a su evento correspondiente
DELETE FROM `game_event_creature` WHERE `eventEntry` = '12' AND `guid` = '537518'; 
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES ('12', '537518'); 
