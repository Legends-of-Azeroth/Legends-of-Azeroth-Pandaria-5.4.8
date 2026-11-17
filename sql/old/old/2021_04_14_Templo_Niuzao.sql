-- modificando el npc que inicia la batalla para que ofresca textos y el npc que tiene mucho daño cuerpo a cuerpo
UPDATE `creature_template` SET `npcflag` = '1' WHERE `entry` = '61620'; 
UPDATE `creature_template` SET `mindmg` = '450' , `maxdmg` = '450' , `attackpower` = '45' WHERE `entry` = '62794'; 

-- textos faltantes 
DELETE FROM `creature_text` WHERE `entry` = '61620' AND `groupid` = '9' AND `id` = '0'; 
DELETE FROM `creature_text` WHERE `entry` = '61620' AND `groupid` = '11' AND `id` = '0'; 
DELETE FROM `creature_text` WHERE `entry` = '61620' AND `groupid` = '13' AND `id` = '0'; 
DELETE FROM `creature_text` WHERE `entry` = '61620' AND `groupid` = '16' AND `id` = '0'; 
DELETE FROM `creature_text` WHERE `entry` = '61620' AND `groupid` = '15' AND `id` = '0'; 
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `TextRange`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `BroadcastTextId`, `comment`) 
VALUES 
('61620','9','0','Look out there. Too many for us to take head on.','','0','12','0','100','0','0','31506','62738','Yang Ironclaw - EMOTE_WAVE_ONE'),
('61620','11','0','Let\'s give these mantid a taste of their own medicine!','','0','12','0','100','0','0','31508','62738','Yang Ironclaw - EMOTE_WAVE_TWO'),
('61620','13','0','Let\'s get this party started, shall we?','','0','12','0','100','0','0','31510','62738','Yang Ironclaw - EMOTE_WAVE_THREE'),
('61620','16','0','Bring it, uglies!','','0','14','0','100','0','0','31513','62738','Yang Ironclaw - EMOTE_BOMBARD'),
('61620','15','0','Warriors coming in! Focus your fire!','','0','12','0','100','0','0','31512','62738','Yang Ironclaw - EMOTE_WAVE_FOUR');
