-- agregando quest = 31375 y 31373, misiones iniciales de la reputación de la Orden del Dragon Nimbo
DELETE FROM `creature_queststarter` WHERE `id` = '64533' AND `quest` = '31375'; 
DELETE FROM `creature_queststarter` WHERE `id` = '64484' AND `quest` = '31373'; 
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES('64533','31375');
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES('64484','31373');

-- reescribiendo quest = 30144
DELETE FROM `quest_poi` WHERE `questId` = '30144' AND `id` = '1'; 

DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '1'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '2'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '3'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '4'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '5'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '6'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '7'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30144' AND `id` = '1' AND `idx` = '8'; 


UPDATE `quest_objective` SET `objectId` = '74839', `type` = '1' , `amount` = '5' , `description` = 'Consigue 5 pechugas de ave silvestre' WHERE `questId` = '30144' AND `id` = '263762' AND `index` = '0'; 
UPDATE `quest_template` SET `Title` = 'El corazón de todo dragón se encuentra en su estomago' , `Objectives` = 'La mejor forma de ganarte la confianza de un dragón es mediante su estomago, alrededor de todo el valle se encuentran varias aves la cual su carne suave es un bocado fantástico para un dragón en crecimiento.' , `Details` = 'Consigue 5 pechugas de ave silvestre' , `OfferRewardText` = 'Return to Instructor Windblade at The Arboretum in The Jade Forest.' WHERE `Id` = '30144'; 

-- reescribiendo quest = 30145

UPDATE `quest_template` SET `Title` = 'Mantener la paz en el arboretum' , `Objectives` = 'Mata 10 espíritus enfadados ' , `Details` = 'Las crías de dragón han estado muy agitadas debido a la presencia de algunos espíritus malignos cerca del pueblo estos son jóvenes pandaren que terminaron siendo comidos por sus dragones debido al mal trato que estos les daban, estos espíritus regresaron de la muerte por venganza y no pueden descansar hasta lograr su cometido, necesitamos que te encargues de ellos de una vez por todas.' , `EndText` = NULL , `OfferRewardText` = 'Te agradecemos ahora las crías de dragón estarán más tranquilas y no te preocupes ese destino no te ocurrirá a ti a menos que... Olvídalo tu sigue en lo tuyo' 
WHERE `Id` = '30145'; 

UPDATE `quest_objective` SET `objectId` = '56712' , `amount` = '10'  , `description` = 'mata 10 espíritus enfadados' WHERE `questId` = '30145' AND `id` = '263816' AND `index` = '0'; 

UPDATE `quest_poi_points` SET `x` = '1908' , `y` = '-2361' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '0'; 
UPDATE `quest_poi_points` SET `x` = '1940' , `y` = '-2325' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '1'; 
UPDATE `quest_poi_points` SET `x` = '2011' , `y` = '-2286' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '2'; 
UPDATE `quest_poi_points` SET `x` = '1997' , `y` = '-2197' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '3'; 
UPDATE `quest_poi_points` SET `x` = '1893' , `y` = '-2216' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '4'; 
UPDATE `quest_poi_points` SET `x` = '1842' , `y` = '-2267' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '5'; 
UPDATE `quest_poi_points` SET `x` = '1825' , `y` = '-2340' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '6'; 
UPDATE `quest_poi_points` SET `x` = '1876' , `y` = '-2389' WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '7'; 

DELETE FROM `quest_poi_points` WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '8'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '9'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '10'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30145' AND `id` = '1' AND `idx` = '11'; 

DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '2'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '3'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '6'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '7'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '8'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '9'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '263816' AND `locale` = '10'; 

-- reescribiendo quest = 30187
UPDATE `quest_template` SET `Title` = 'El camino del dragón' , `Objectives` = 'Derrota a Liu Corazón Llameante' , `Details` = 'Tenemos grandes problemas cuando los Sha fueron liberados al instante fuimos atacados y una valiente de nuestra raza Liu Corazón Llameante dio la cara por todos nosotros y en este momento se encuentra corrompida por el Sha de la duda, debido a eso está convirtiendo a mas de nuestro pueblo en esclavos; debemos liberarla de sus cadenas de una vez por todas ella se resguarda al final del camino del dragón.$BNo te confíes ella posee un gran poder y habilidades ocultas ten cuidado se dice que puede invocar a Yulon esta bestia fue alguna vez el compañero de un gran cazador.$BLo conocían como el poderoso...' , `OfferRewardText` = 'Ha sido un largo camino que has recorrido y demostraste en todos los aspectos que eres más que digno; me enorgullece ser tu maestro.' WHERE `Id` = '30187'; 
UPDATE `quest_objective` SET `objectId` = '56732' , `amount` = '1', `description` = 'Derrota a Liu Corazón Llameante' WHERE `questId` = '30187' AND `id` = '251728' AND `index` = '0'; 

DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '2'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '3'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '4'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '5'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '6'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '7'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '8'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '9'; 
DELETE FROM `quest_poi` WHERE `questId` = '30187' AND `id` = '10'; 

DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '2' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '3' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '4' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '5' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '6' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '7' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '8' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '9' AND `idx` = '0'; 
DELETE FROM `quest_poi_points` WHERE `questId` = '30187' AND `id` = '10' AND `idx` = '0'; 

DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '506'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '507'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '508'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '509'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '510'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '511'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '512'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '513'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '514'; 
DELETE FROM `quest_objective_effects` WHERE `objectiveId` = '251728' AND `visualEffect` = '515'; 

DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '2'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '3'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '6'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '7'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '8'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '9'; 
DELETE FROM `locales_quest_objective` WHERE `id` = '251728' AND `locale` = '10'; 

UPDATE `quest_poi` SET `mapid` = '960' WHERE `questId` = '30187' AND `id` = '1'; 
UPDATE `quest_poi_points` SET `x` = '929' , `y` = '-2560' WHERE `questId` = '30187' AND `id` = '1' AND `idx` = '0'; 


-- eliminando requisitos de la quest = 31810
DELETE FROM `quest_objective` WHERE `questId` = '31810' AND `id` = '269045' AND `index` = '0'; 
-- eliminando requisitos de la quest = 30188
DELETE FROM `quest_objective` WHERE `questId` = '30188' AND `id` = '269045' AND `index` = '0'; 
-- eliminando requisitos de la quest = 31811
DELETE FROM `quest_objective` WHERE `questId` = '31811' AND `id` = '269045' AND `index` = '0'; 
