-- extendiendo el soporte para la recompensa de títulos en misiones -> el rango anterior solo llegaba hasta 255 por lo cual no se podian otorgar títulos con un Id superior a 255
ALTER TABLE `quest_template` CHANGE `RewardTitleId` `RewardTitleId` SMALLINT(5) UNSIGNED DEFAULT 0 NOT NULL; 
