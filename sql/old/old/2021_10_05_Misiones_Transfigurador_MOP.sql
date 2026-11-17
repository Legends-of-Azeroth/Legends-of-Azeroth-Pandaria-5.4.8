-- primera parte de la cadena de misiones para desbloquear el transfigurador global de items

-- modificando npc existentes para que puedan entregar misiones
UPDATE `creature_template` SET `npcflag` = '3' WHERE `entry` = '57801'; 
UPDATE `creature_template` SET `npcflag` = '3' WHERE `entry` = '57800'; 

-- cadenas de misiones inicial para desbloquear la cadena de misiones para el nuevo transfigurador custom de items
DELETE FROM `quest_template` WHERE Id BETWEEN 40314 AND 40323;
insert into `quest_template` (`Id`, `Method`, `Level`, `MinLevel`, `MaxLevel`, `ZoneOrSort`, `Type`, `SuggestedPlayers`, `LimitTime`, `RequiredClasses`, `RequiredRaces`, `RequiredSkillId`, `RequiredSkillPoints`, `RequiredMinRepFaction`, `RequiredMaxRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepValue`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestIdChain`, `RewardXPId`, `RewardOrRequiredMoney`, `RewardMoneyMaxLevel`, `RewardSpell`, `RewardSpellCast`, `RewardHonor`, `RewardHonorMultiplier`, `RewardMailTemplateId`, `RewardMailDelay`, `SourceItemId`, `SourceSpellId`, `Flags`, `Flags2`, `SpecialFlags`, `MinimapTargetMark`, `RewardTitleId`, `RewardTalents`, `RewardArenaPoints`, `RewardSkillId`, `RewardSkillPoints`, `RewardReputationMask`, `QuestGiverPortrait`, `QuestTurnInPortrait`, `RewardPackageItemId`, `RewardItemId1`, `RewardItemId2`, `RewardItemId3`, `RewardItemId4`, `RewardItemCount1`, `RewardItemCount2`, `RewardItemCount3`, `RewardItemCount4`, `RewardChoiceItemId1`, `RewardChoiceItemId2`, `RewardChoiceItemId3`, `RewardChoiceItemId4`, `RewardChoiceItemId5`, `RewardChoiceItemId6`, `RewardChoiceItemCount1`, `RewardChoiceItemCount2`, `RewardChoiceItemCount3`, `RewardChoiceItemCount4`, `RewardChoiceItemCount5`, `RewardChoiceItemCount6`, `RewardFactionId1`, `RewardFactionId2`, `RewardFactionId3`, `RewardFactionId4`, `RewardFactionId5`, `RewardFactionValueId1`, `RewardFactionValueId2`, `RewardFactionValueId3`, `RewardFactionValueId4`, `RewardFactionValueId5`, `RewardFactionValueIdOverride1`, `RewardFactionValueIdOverride2`, `RewardFactionValueIdOverride3`, `RewardFactionValueIdOverride4`, `RewardFactionValueIdOverride5`, `PointMapId`, `PointX`, `PointY`, `PointOption`, `Title`, `Objectives`, `Details`, `EndText`, `OfferRewardText`, `RequestItemsText`, `CompletedText`, `RequiredSourceItemId1`, `RequiredSourceItemId2`, `RequiredSourceItemId3`, `RequiredSourceItemId4`, `RequiredSourceItemCount1`, `RequiredSourceItemCount2`, `RequiredSourceItemCount3`, `RequiredSourceItemCount4`, `RewardCurrencyId1`, `RewardCurrencyId2`, `RewardCurrencyId3`, `RewardCurrencyId4`, `RewardCurrencyCount1`, `RewardCurrencyCount2`, `RewardCurrencyCount3`, `RewardCurrencyCount4`, `QuestGiverTextWindow`, `QuestGiverTargetName`, `QuestTurnTextWindow`, `QuestTurnTargetName`, `SoundAccept`, `SoundTurnIn`, `DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`, `EmoteOnIncomplete`, `EmoteOnComplete`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`, `OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`, `WDBVerified`) 
values
('40314','2','90','90','0','-344','83','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Un nombre perdido en el vacío abisal','Encuentra el Señor Tejedistorsiones Vaal´akir  en Dalaran','Cuando Dimensius, señor del vacío atacó a Káresh surgió una facción que quiso usar los poderes oscuros que éste emanaba para hacerle frente, por estas prácticas luego de perder nuestra carne aquellos quienes manipulaban las energías oscuras nos parecen aberrantes, ellos por voluntad propia se exiliaron y vagaron de planeta en planeta como nosotros, entre aquellos desterrados de nuestro pueblo se encontraba un retorcedor de la realidad que usaba aquella energía para poder deformar aquellas cosas materiales sin cambiar su esencia, su nombre era… Vaal´akir, un descendiente oculto del rey de K´aresh que al ver su pueblo desgarrado por la energía arcana del vacío y la oscuridad de Dimensius no dudo en usar la energía de este para hacerle frente, informantes del protectorado han rastreado su energía hasta el Salón púrpura en Dalaran, habla con él, será un valioso aliado en un futuro',NULL,'¿eh?.. ¿Qué quieres ser de carne? no tengo tiempo para hablar...',NULL,'Señor tejedistorsiones Vaal´akir Salón Púrpura Dalaran','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40315','2','90','90','0','-344','83','3','0','0','0','0','0','0','0','0','0','40314','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','El poder en bruto ','Asesina a Arcanon en el Gelidar y consigue su matriz arcana','Vine aquí por una razón, el Kirin Tor me ha brindado estudios mágicos de gran ayuda, he abandonado el camino de las sombras para usar el poder en bruto de las líneas Ley, el poder arcano que emana es infinito y es lo que me ayudará a tejer distorsiones de un nivel superior, Ser de carne, aquí en el norte de este mundo se puede sentir la energía arcana muy fuerte, ve al Gelidar y consigue un poco de ella por mi, te aseguro que no te arrepentiras',NULL,'Ser de carne, perfecto, esto refinar mi técnica aquí tienes una recompensa.','Tienes lo que te pedi?','Regresa con Vaal´akir en Dalaran','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40316','2','90','90','0','-344','83','0','0','0','0','0','0','0','0','0','0','40315','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Un martillo para un forjador de distorsiones ','Tráele al teje distorsiones.$B20 Barras de Elementium encantado$B7 barras de Trillium$B5 cristal de la vorágine$B5 cristal de Sha','Para poder conjurar mis hechizos de distorsión primero necesito de objeto en un plano material para poder transformarlo, algo así como un martillo, yo puedo tomar los materiales y forjarlo, tráeme esta lista, verás de lo que es capaz este Teje distorsiones.',NULL,'Gracias ser de carne, estoy casi listo, la hora de volver a mis artes se acerca ','Traes todo?','Regresa con Vaal´akir en Dalaran','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40317','2','90','90','0','-344','83','0','0','0','0','0','0','0','0','0','0','40316','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','El poder llama','Encuentra al Tejedistorsiones Vaal´akir en Pandaria','Aquí en el Norte ya no hay nada que me interese, ser de carne, nos veremos en la tribuna Mogu´shan, siento que ese lugar rebosa de poder, un poder que me llama.',NULL,'Oh, llegaste, rápido, hay trabajo que hacer','Traes todo?','Encuentra al Tejedistorsiones Vaal´akir en Pandaria','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40318','2','90','90','0','-344','62','0','0','0','0','0','0','0','0','0','0','40317','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Los secretos Mogu´shan','Consigue el elemento Arcano X32 en el encuentro con los reyes espíritu en las cámaras de Mogu´shan ','Esa cámara, siento como el poder arcano palpita dentro de ella, tráeme algo para fines científicos… $B<Vaal´akir suelta una risita macabra>$BHey no me mires así, algunos preferimos tener poder que dejarlo olvidado y almacenado en una cámara.',NULL,'Gracias ser de carne, esto servirá para mis investigaciones, pronto te daré más tareas, estoy muy cerca.','Lo tienes?','Regresa con Vaal´akir en Pandaria','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40319','2','90','90','0','-344','83','0','0','0','0','0','0','0','0','0','0','40318','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Magia del vacío en el continente','Consigue un fragmento de Ira del Sha de la ira$BConsigue un fragmento de la duda del Sha de la duda$BConsigue un fragmento de Violencia del Sha de la violencia','Cada esquina de este continente está afectada por poderes etéreos, se puede sentir el flujo de esa energía maligna… Ser de Carne, traeme un fragmento de esa magia Eterea, de alguna manera con el elemento X32 podría darle forma al influjo caótico que esta emana.',NULL,'Ser de carne como lo hiciste… no importa, la misma realidad se retuerce con la esencia de este poder, excelente trabajo, pronto estará listo el yunque, pronto, hay pocas tareas por completar','Traes todo?','Regresa con Vaal´akir en Pandaria','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40320','2','90','90','0','-344','83','0','0','0','0','0','0','0','0','0','0','40319','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Al final del espinazo','Recolecta la esencia Camina realidades','Algunos colegas tejedores de ilusiones me han informado que llevan meses tras una criatura con un poder extraño de saltar de realidad en realidad distorsionando y doblando las dimensiones en cada salto, su última ubicación fue en las pequeñas piedras al final de la muralla Espinazo de dragón, ve y tráeme su esencia.',NULL,'Su esencia es muy Volátil, es perfecto, estamos muy cerca.','Lo tienes?','Regresa con Vaal´akir en Pandaria','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40321','2','90','90','0','-344','83','0','0','0','0','0','0','0','0','0','0','40320','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','El elemento para crear ','Recolecta la esencia de Aullaescarcha','Al tejer ilusiones el martillo forjador se sobre calienta, se necesita de la esencia elemental de hielo pura para poder hacerlo de una manera efectiva, hay un elemental con una esencia muy fuerte ubicado en la perdición del explorador en el Monte Nieverest.',NULL,'Excelente ser de carne, estoy muy agradecido contigo.','Lo obtuviste?','Regresa con Vaal´akir en Pandaria','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40322','2','90','90','0','-344','83','0','0','0','18875469','0','0','0','0','0','0','40321','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Un nuevo aliado','Encuentra al Tejedistorciones en el Santuario de las Siete Estrellas','Has servido bien a mis fines de estudio y la creación de mis objetos, es hora de que te ayude en algo, nos veremos en tu base principal, me encontrarás allí siempre que me necesites, me has servido bien ser de carne así que te será de utilidad a ti. ',NULL,'Interesante ubicación para una base',NULL,'Regresa con Vaal´akir en el Santuario de las siete estrellas','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('40323','2','90','90','0','-344','83','0','0','0','33555378','0','0','0','0','0','0','40321','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','Un nuevo aliado','Encuentra al Tejedistorciones en el Santuario de las Dos Lunas','Has servido bien a mis fines de estudio y la creación de mis objetos, es hora de que te ayude en algo, nos veremos en tu base principal, me encontrarás allí siempre que me necesites, me has servido bien ser de carne así que te será de utilidad a ti. ',NULL,'Interesante ubicación para una base',NULL,'Regresa con Vaal´akir en el Santuario de las dos Lunas','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','','','','890','878','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414');

-- Agregando objetivos en el mapa para las misiones
DELETE FROM `quest_poi` WHERE questId BETWEEN 40314 AND 40323;
insert into `quest_poi` (`questId`, `id`, `objIndex`, `mapid`, `WorldMapAreaId`, `FloorId`, `unk3`, `unk4`) values
('40314','0','-1','571','504','0','0','0'),
('40315','0','-1','571','504','0','0','0'),
('40315','1','0','571','486','0','0','0'),
('40315','2','1','571','486','0','0','0'),
('40316','0','-1','571','504','0','0','0'),
('40317','0','-1','870','809','0','0','0'),
('40318','0','-1','870','809','0','0','0'),
('40318','1','1','1008','896','0','0','0'),
('40319','0','-1','870','809','0','0','0'),
('40320','0','-1','870','809','0','0','0'),
('40320','1','1','870','858','0','0','0'),
('40321','0','-1','870','809','0','0','0'),
('40321','1','1','870','809','0','0','0'),
('40322','0','-1','870','95','0','0','0'),
('40323','0','-1','870','903','0','0','0');

-- Agregando puntos en el mapa  para los objetivos de las misiones
DELETE FROM `quest_poi_points` WHERE questId BETWEEN 40314 AND 40323;
insert into `quest_poi_points` (`questId`, `id`, `idx`, `x`, `y`) values
('40314','0','0','5988','641'),
('40314','0','1','5933','689'),
('40314','0','2','5915','640'),
('40315','0','0','5977','667'),
('40315','1','0','4095','6724'),
('40315','1','1','4149','6695'),
('40315','1','2','4182','6753'),
('40315','1','3','4130','6783'),
('40315','2','0','4137','6741'),
('40316','0','0','5977','667'),
('40317','0','0','3999','1086'),
('40318','0','0','3999','1086'),
('40318','1','0','4253','1565'),
('40318','1','1','4258','1618'),
('40318','1','2','4217','1636'),
('40318','1','3','4192','1613'),
('40318','1','4','4204','1561'),
('40319','0','0','3999','1086'),
('40320','0','0','3999','1086'),
('40320','1','0','-1705','2809'),
('40321','0','0','3999','1086'),
('40321','1','0','3557','2147'),
('40322','0','0','731','236'),
('40323','0','0','1776','885');

-- agregando objetivos a cumplir en las misiones
DELETE FROM `quest_objective` WHERE questId BETWEEN 40314 AND 40323;
insert into `quest_objective` (`questId`, `id`, `index`, `type`, `objectId`, `amount`, `flags`, `description`) values
('40315','500284','0','0','300011','1','0',NULL),
('40316','500286','0','1','17771','20','0',NULL),
('40315','500285','1','1','129923','1','0',NULL),
('40316','500287','1','1','72095','7','0',NULL),
('40316','500288','2','1','52722','5','0',NULL),
('40316','500289','3','1','74248','5','0',NULL),
('40318','500290','0','1','129924','1','0',NULL),
('40319','500291','0','1','129925','1','0',NULL),
('40319','500292','1','1','129926','1','0',NULL),
('40319','500293','2','1','129927','1','0',NULL),
('40320','500294','0','1','129928','1','0',NULL),
('40321','500295','0','1','129929','1','0',NULL);

-- indicando las criaturas que inician las misiones
DELETE FROM `creature_queststarter` WHERE quest BETWEEN 40314 AND 40323;
insert into `creature_queststarter` (`id`, `quest`) values
('57800','40314'),('57801','40314'),
('300010','40315'),('300010','40316'),
('300010','40317'),('300012','40318'),
('300012','40319'),('300012','40320'),
('300012','40321'),('300012','40322'),
('300012','40323');

-- indicando las criaturas que finalizan las misiones
DELETE FROM `creature_questender` WHERE quest BETWEEN 40314 AND 40323;
insert into `creature_questender` (`id`, `quest`) values
('300010','40314'),('300010','40315'),
('300010','40316'),('300012','40317'),
('300012','40318'),('300012','40319'),
('300012','40320'),('300012','40321'),
('300015','40322'),('300015','40323');

-- agregando nuevos npc usados en la cadena de misiones
DELETE FROM `creature_template` WHERE entry IN (300010, 300011, 300012, 300013, 300014, 300015);
insert into `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `difficulty_entry_4`, `difficulty_entry_5`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_unk`, `faction_A`, `faction_H`, `npcflag`, `npcflag2`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Mana_mod_extra`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `VignetteID`, `TrackingQuestID`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) 
values
('300010','0','0','0','0','0','0','0','18660','0','0','0','Vaal´akir','Señor Tejedistorsiones','','0','91','91','4','0','35','35','3','0','1','1','1','4','3796','8289','0','42296','2','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','10','1','1','1','0','0','0','0','0','0','0','0','1','156','0','0','0','','15595'),
('300011','0','0','0','0','0','0','0','19003','0','0','0','Arcanon','','','0','91','91','4','0','14','14','0','0','1','2','2','4','3796','8289','0','42296','2','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','7','2097224','0','300011','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','10','1','1','1','0','0','0','0','0','0','0','0','1','156','0','143330899','0','npc_yorik_sharpeye','15595'),
('300012','0','0','0','0','0','0','0','18660','0','0','0','Vaal´akir','Señor Tejedistorsiones','','0','91','91','4','0','35','35','3','0','1','1','1','4','3796','8289','0','42296','2','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','10','1','1','1','0','0','0','0','0','0','0','0','1','156','0','0','0','','15595'),
('300013','0','0','0','0','0','0','0','27390','0','0','0','Camina realidades','','','0','91','91','4','0','14','14','0','0','1','2','2','4','3796','8289','0','42296','2','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','7','2097224','0','300013','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','10','1','1','1','0','0','0','0','0','0','0','0','1','156','0','143330899','0','npc_yorik_sharpeye','15595'),
('300014','0','0','0','0','0','0','0','26177','0','0','0','Aullaescarcha','','','0','91','91','4','0','14','14','0','0','1','2','2','4','3796','8289','0','42296','2','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','7','2097224','0','300014','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','10','1','1','1','0','0','0','0','0','0','0','0','1','156','0','143330899','0','npc_yorik_sharpeye','15595'),
('300015','0','0','0','0','0','0','0','18660','0','0','0','Vaal´akir','Señor Tejedistorsiones','','0','91','91','4','0','35','35','3','0','1','1','1','4','3796','8289','0','42296','2','2000','2000','1','0','2048','0','0','0','0','0','0','0','0','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','10','1','1','1','0','0','0','0','0','0','0','0','1','156','0','0','0','','15595');

-- agregando nuevos npc en el mundo
DELETE FROM `creature` WHERE id IN (300010, 300011, 300012, 300013, 300014, 300015);
insert into `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`) 
values
('582646','300010','571','0','0','1','4294967295','0','0','5976.39','666.622','641.505','2.91077','300','0','0','0','4075320','0','0','0','0','0','0','0'),
('582647','300011','571','0','0','1','4','0','0','4137.88','6741.41','249.792','2.59709','300','0','0','0','4075320','0','0','0','0','0','0','0'),
('582648','300012','870','0','0','1','1','0','0','3998.25','1088.37','461.015','5.43996','300','0','0','0','4075320','0','0','0','0','0','0','0'),
('582649','300013','870','0','0','1','4','0','0','-1707.89','2837.32','14.5892','0.920729','300','0','0','0','4075320','0','0','0','0','0','0','0'),
('582650','300014','870','0','0','1','4','0','0','3557.38','2147.87','1028.67','5.08206','300','0','0','0','4075320','0','0','0','0','0','0','0'),
('582651','300015','870','0','0','1','1','0','0','731.672','236.646','519.744','3.74989','300','0','0','0','4075320','0','0','0','0','0','0','0'),
('582652','300015','870','0','0','1','1','0','0','1776.86','885.505','487.216','1.75019','300','0','0','0','4075320','0','0','0','0','0','0','0');

-- agregando nuevos items objetivos de misiones
DELETE FROM `item_template` WHERE entry IN (129923, 129924, 129925, 129926, 129927, 129928, 129929);
insert into `item_template` (`entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `Flags3`, `Unk430_1`, `Unk430_2`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_unk1_1`, `stat_unk2_1`, `stat_type2`, `stat_value2`, `stat_unk1_2`, `stat_unk2_2`, `stat_type3`, `stat_value3`, `stat_unk1_3`, `stat_unk2_3`, `stat_type4`, `stat_value4`, `stat_unk1_4`, `stat_unk2_4`, `stat_type5`, `stat_value5`, `stat_unk1_5`, `stat_unk2_5`, `stat_type6`, `stat_value6`, `stat_unk1_6`, `stat_unk2_6`, `stat_type7`, `stat_value7`, `stat_unk1_7`, `stat_unk2_7`, `stat_type8`, `stat_value8`, `stat_unk1_8`, `stat_unk2_8`, `stat_type9`, `stat_value9`, `stat_unk1_9`, `stat_unk2_9`, `stat_type10`, `stat_value10`, `stat_unk1_10`, `stat_unk2_10`, `ScalingStatDistribution`, `ScalingStatValue`, `DamageType`, `delay`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `ArmorDamageModifier`, `duration`, `ItemLimitCategory`, `HolidayId`, `StatScalingFactor`, `CurrencySubstitutionId`, `CurrencySubstitutionCount`, `RequiredDisenchantSkill`, `flagsCustom`, `VerifiedBuild`) 
values
('129923','12','0','-1','Arcane Matrix','24730','1','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('129924','12','0','-1','Arcane Element X32','70333','1','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('129925','12','0','-1','Shard of Wrath of Wrath','116772','5','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('129926','12','0','-1','Shard of Doubt of Doubt','116770','5','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('129927','12','0','-1','Fragment of Violence from the Shah of Violence','120954','5','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('129928','12','0','-1','Essence of Realities Walk','55243','1','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414'),
('129929','12','0','-1','Essence of Frosthow','60021','1','2048','8192','0','0.9613','1','1','0','0','0','-1','-1','1','0','0','0','0','0','0','0','0','0','20','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','0','0','0','-1','0','-1','4','','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','18414');

-- agregando loot a npc objetivos de misiones
DELETE FROM `creature_loot_template` WHERE entry IN (300011, 300013, 300014);
insert into `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) 
values
('300011','129923','-100','','0','1','1'),
('300013','129928','-100','','0','1','1'),
('300014','129929','-100','','0','1','1');

-- agregando nuevas fases intermedias para interactuar con los NPC objetivos de misiones
DELETE FROM `spell_area` WHERE quest_start IN (40315, 40320, 40321);
insert into `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) 
values
('59074','4024','40315','40316','0','0','2','1','8','1'),
('59074','6138','40320','40321','0','0','2','1','8','1'),
('59074','6175','40321','40322','0','0','2','1','8','1');

/* modificando el loot de criaturas existentes para que entreguen items objetivos de misiones */

-- Camaras Mogushan - Los Reyes Espíritu
DELETE FROM `creature_loot_template` WHERE `entry` = '60709' AND `item` = '129924' AND `lootmode` = 'RAID_25MAN_HEROIC'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '60709' AND `item` = '129924' AND `lootmode` = 'RAID_10MAN_HEROIC'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '60709' AND `item` = '129924' AND `lootmode` = 'RAID_25MAN_NORMAL'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '60709' AND `item` = '129924' AND `lootmode` = 'RAID_10MAN_NORMAL'; 

INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
('60709', '129924', '-100', 'RAID_10MAN_NORMAL', '0', '1', '1'),
('60709', '129924', '-100', 'RAID_25MAN_NORMAL', '0', '1', '1'),
('60709', '129924', '-100', 'RAID_10MAN_HEROIC', '0', '1', '1'), 
('60709', '129924', '-100', 'RAID_25MAN_HEROIC', '0', '1', '1'); 

-- Sha de la ira
DELETE FROM `creature_loot_template` WHERE `entry` = '60491' AND `item` = '129925' AND `lootmode` = ''; 
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES
('60491', '129925', '-100', '', '0', '1', '1');

-- Sha de la duda
DELETE FROM `creature_loot_template` WHERE `entry` = '56439' AND `item` = '129926' AND `lootmode` = 'DUNGEON_NORMAL'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '56439' AND `item` = '129926' AND `lootmode` = 'DUNGEON_HEROIC'; 
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
('56439', '129926', '-100', 'DUNGEON_NORMAL', '0', '1', '1'),
('56439', '129926', '-100', 'DUNGEON_HEROIC', '0', '1', '1'); 

-- Sha de la violencia
DELETE FROM `creature_loot_template` WHERE `entry` = '56719' AND `item` = '129927' AND `lootmode` = 'DUNGEON_NORMAL'; 
DELETE FROM `creature_loot_template` WHERE `entry` = '56719' AND `item` = '129927' AND `lootmode` = 'DUNGEON_HEROIC'; 
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
('56719', '129927', '-100', 'DUNGEON_NORMAL', '1', '1', '1'),
('56719', '129927', '-100', 'DUNGEON_HEROIC', '1', '1', '1'); 
