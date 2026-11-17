-- eliminando botín normal de los jefes de mundo Sha de la Ira y Galeon
DELETE FROM `creature_loot_template` WHERE `entry` = '62346'; -- Galeón
DELETE FROM `creature_loot_template` WHERE `entry` = '60491'; -- sha de la ira

-- Se modifica el tiempo de estos jefes de mundo a 4 horas

-- sha de la ira
UPDATE `creature` SET `spawntimesecs` = '14400' WHERE `guid` = '564256';
UPDATE `creature` SET `spawntimesecs` = '14400' WHERE `guid` = '564257';
UPDATE `creature` SET `spawntimesecs` = '14400' WHERE `guid` = '564258';
UPDATE `creature` SET `spawntimesecs` = '14400' WHERE `guid` = '564259';
UPDATE `creatuRE` SET `spawntimesecs` = '14400' WHERE `guid` = '564260';

-- galeon
UPDATE `creature` SET `spawntimesecs` = '14400' WHERE `guid` = '516616'; 
