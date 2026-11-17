-- quest con requerimiento de PJ femenino, anteriormente solo los pj masculinos podian obtener estas misiones pero dichas misiones eran para los PJ femeninos
UPDATE `conditions` SET `ConditionValue1` = '1' WHERE `SourceTypeOrReferenceId` = '19' AND `SourceEntry` = '14153' AND `ConditionTypeOrReference` = '20' AND `ConditionValue1` = '0' AND `Comment` = 'Only female'; 
UPDATE `conditions` SET `ConditionValue1` = '1' WHERE `SourceTypeOrReferenceId` = '19' AND `SourceEntry` = '14474' AND `ConditionTypeOrReference` = '20' AND `ConditionValue1` = '0' AND `Comment` = 'Only female'; 
UPDATE `conditions` SET `ConditionValue1` = '1' WHERE `SourceTypeOrReferenceId` = '19' AND `SourceEntry` = '25202' AND `ConditionTypeOrReference` = '20' AND `ConditionValue1` = '0' AND `Comment` = 'Only female'; 
UPDATE `conditions` SET `ConditionValue1` = '1' WHERE `SourceTypeOrReferenceId` = '19' AND `SourceEntry` = '25244' AND `ConditionTypeOrReference` = '20' AND `ConditionValue1` = '0' AND `Comment` = 'Only female'; 
UPDATE `conditions` SET `ConditionValue1` = '1' WHERE `SourceTypeOrReferenceId` = '19' AND `SourceEntry` = '26711' AND `ConditionTypeOrReference` = '20' AND `ConditionValue1` = '0' AND `Comment` = 'Only female'; 

-- bolsa de camorrista, se cambio el % de botín en una tercera parte
UPDATE `item_loot_template` SET `ChanceOrQuestChance` = '33' WHERE `entry` = '92718' AND `item` = '92916' AND `lootmode` = ''; 
UPDATE `item_loot_template` SET `ChanceOrQuestChance` = '33' WHERE `entry` = '92718' AND `item` = '92927' AND `lootmode` = ''; 
