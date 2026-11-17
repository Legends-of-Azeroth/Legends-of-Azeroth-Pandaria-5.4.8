-- Ancient Teleport: Dalaran
-- https://www.wowhead.com/spell=120145/ancient-teleport-dalaran
-- https://www.wowhead.com/spell=221237/ancient-portal-dalaran (legion)
-- https://www.wowhead.com/spell=121848/ancient-portal-dalaran (pandria)
DELETE FROM `spell_target_position` WHERE (`id`,`effIndex`) IN ((120145,0),(121848,0));
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(120145, 0, 0, 303.22, 347.21, 125.53, 1.721941947937011718),
(121848, 0, 0, 303.22, 347.21, 125.53, 1.721941947937011718);

-- Update wrong name and spellid of ancient-portal-dalaran
UPDATE `gameobject_template` SET `name`='Portal to Dalaran',`data0`=121848 WHERE `entry`=211835;

-- Add locale text
DELETE FROM `gameobject_template_locale` WHERE `entry`=211835 AND `locale` IN ('deDE','esES','esMX','frFR','koKR','ruRU','zhCN','zhTW');
INSERT INTO `gameobject_template_locale` (`entry`, `locale`, `name`, `castBarCaption`, `VerifiedBuild`) VALUES
(211835, 'deDE', 'Portal nach Dalaran', '', 18019),
(211835, 'esES', 'Portal a Dalaran', '', 18019),
(211835, 'esMX', 'Portal a Dalaran', '', 18019),
(211835, 'frFR', 'Portail vers Dalaran', '', 18019),
(211835, 'koKR', '달라란으로 통하는 차원문', '', 18019),
(211835, 'ruRU', 'Портал в Даларан', '', 18019),
(211835, 'zhCN', '通往达拉然的传送门', '', 18019),
(211835, 'zhTW', '達拉然傳送門', '', 18019);

