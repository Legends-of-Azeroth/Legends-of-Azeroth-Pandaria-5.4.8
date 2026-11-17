UPDATE `npc_trainer` SET `spell` = 33388 WHERE `spell` = 33389;
UPDATE `npc_trainer` SET `spell` = 33391 WHERE `spell` = 33392;
UPDATE `npc_trainer` SET `spell` = 34090 WHERE `spell` = 34092;
UPDATE `npc_trainer` SET `spell` = 34091 WHERE `spell` = 34093;
UPDATE `npc_trainer` SET `spell` = 54197 WHERE `spell` = 54198;
UPDATE `npc_trainer` SET `spell` = 90265 WHERE `spell` = 90266;
UPDATE `npc_trainer` SET `spell` = 90267 WHERE `spell` = 90269;
UPDATE `npc_trainer` SET `spell` = 115913 WHERE `spell` = 115916;

UPDATE `npc_trainer` SET `spellcost` = 1000 WHERE `spell` = 33388 AND `reqlevel` = 20;
UPDATE `npc_trainer` SET `spellcost` = 500000 WHERE `spell` = 33391 AND `reqlevel` = 40;
UPDATE `npc_trainer` SET `spellcost` = 2500000 WHERE `spell` = 34090 AND `reqlevel` = 60;
UPDATE `npc_trainer` SET `spellcost` = 50000000 WHERE `spell` = 90265 AND `reqlevel` = 60;
UPDATE `npc_trainer` SET `spellcost` = 2500000 WHERE `spell` = 90267 AND `reqlevel` = 60;
UPDATE `npc_trainer` SET `spellcost` = 5000000 WHERE `spell` = 34091 AND `reqlevel` = 70;
UPDATE `npc_trainer` SET `spellcost` = 5000000 WHERE `spell` = 54197 AND `reqlevel` = 68;
UPDATE `npc_trainer` SET `spellcost` = 25000000 WHERE `spell` = 115913 AND `reqlevel` = 90;
DELETE FROM `npc_trainer` WHERE `entry` = 60167 AND `spell` = 40123;