
/*Data for the table `battle_pay_entry` */

DELETE FROM `battle_pay_entry`;
INSERT  INTO `battle_pay_entry`(`id`,`productId`,`groupId`,`idx`,`title`,`description`,`icon`,`displayId`,`banner`,`flags`) VALUES 
(12,83,11,0,'Boost 90','Level Up 90',614740,0,2,0),
(13,100,5,0,'Thunderfury, Blessed Blade of the Windseeker','Legendary One-Handed Sword',135349,0,2,0),
(4,3,4,0,'Tyrael\'s Hilt','Teaches you how to summon this companion.',135277,0,2,0),
(5,2,2,0,'Sky Golem','Flying Mount',894222,0,2,0),
(11,9,10,0,'WoW Token - 01','01 point for the WoW shop',939375,0,3,0),
(10,8,9,0,'Recipe: Vial of the Sands','Teaches you how to make the Vial of the Sands.',134940,0,2,0),
(9,7,8,0,'Puzzle Box of Yogg-Saron','Extremely difficult to open',458238,0,2,0),
(8,6,7,0,'Shrimp Dumplings','Grants 250 Spirit',651593,0,2,0),
(7,5,6,0,'Gnomish X-Ray Specs','Allows you TO see players without clothing AND armor.',667398,0,2,0),
(6,4,3,0,'Embersilk Bag','22 slot bag',348522,0,2,0),
(14,10,10,0,'WoW Token - 02','02 point for the WoW shop',939375,0,3,0),
(2,9,1,0,'WoW Token - 01','1 point for the WoW shop',939375,0,3,0),
(3,1,2,0,'Heart of the Aspects','Flying Mount',576138,0,2,0),
(15,11,10,0,'WoW Token - 05','05 point for the WoW shop',939375,0,3,0),
(16,12,10,0,'WoW Token - 10','10 point for the WoW shop',939375,0,3,0);

/*Data for the table `battle_pay_group` */

DELETE FROM `battle_pay_group`;
INSERT  INTO `battle_pay_group`(`id`,`idx`,`name`,`icon`,`type`) VALUES 
(1,1,'Featured',939376,1),
(4,4,'Pets',939380,0),
(2,2,'Mounts',939379,0),
(11,11,'Services',939382,0),
(5,5,'Weapons',940868,0),
(3,3,'Bags',940857,0),
(6,6,'Armor',940856,0),
(7,7,'Food',940861,0),
(8,8,'Toys',940867,0),
(9,9,'Scroll',940860,0),
(10,10,'WoW',940869,0);

/*Data for the table `battle_pay_product` */

DELETE FROM `battle_pay_product`;
INSERT  INTO `battle_pay_product`(`id`,`title`,`description`,`icon`,`price`,`discount`,`displayId`,`type`,`choiceType`,`flags`,`flagsInfo`) VALUES 
(1,'Heart of the Aspects','Flying Mount',576138,20,10,0,0,1,47,0),
(83,'Boost 90','Level Up 90',614740,2000,15,0,2,1,975,0),
(100,'Thunderfury, Blessed Blade of the Windseeker','Legendary One-Handed Sword',135349,35,55,0,0,1,47,0),
(2,'Sky Golem','Flying Mount',894222,20,10,0,0,1,47,0),
(3,'Tyrael\'s Hilt','Teaches you how to summon this companion.',135277,20,10,0,0,1,47,0),
(5,'Gnomish X-Ray Specs','Allows you TO see players without clothing AND armor.',667398,8,0,0,0,1,47,0),
(4,'Shop: Embersilk Bag','Thank you for supporting the project, enjoy your bag of 22 slots',348522,5,0,0,0,1,47,0),
(6,'Shrimp Dumplings','Grants 250 Spirit',651593,5,0,0,0,1,47,0),
(7,'Puzzle Box of Yogg-Saron','Extremely difficult to open',458238,25,0,0,0,1,47,0),
(8,'Recipe: Vial of the Sands','Teaches you how to make the Vial of the Sands.',134940,25,0,0,0,1,47,0),
(9,'WoW Token - 01','1 point for the WoW shop',939375,2,20,0,0,1,47,0),
(10,'WoW Token - 02','1 point for the WoW shop',939375,3,20,0,0,1,47,0),
(11,'WoW Token - 05','1 point for the WoW shop',939375,7,20,0,0,1,47,0),
(12,'WoW Token - 10','1 point for the WoW shop',939375,13,10,0,0,1,47,0);

/*Data for the table `battle_pay_product_items` */

DELETE FROM `battle_pay_product_items`;
INSERT  INTO `battle_pay_product_items`(`id`,`itemId`,`count`,`productId`) VALUES 
(1,78924,1,1),
(83,19019,1,83),
(100,19019,1,100),
(2,95416,1,2),
(3,39656,1,3),
(4,54443,1,4),
(5,40895,1,5),
(6,74651,1,6),
(7,64482,1,7),
(8,67538,1,8),
(9,110001,1,9),
(10,110002,1,10),
(11,110003,1,11),
(12,110004,1,12);

/*Data for the table `locales_battle_pay_entry` */

DELETE FROM `locales_battle_pay_entry`;
INSERT  INTO `locales_battle_pay_entry`(`id`,`title_loc1`,`title_loc2`,`title_loc3`,`title_loc4`,`title_loc5`,`title_loc6`,`title_loc7`,`title_loc8`,`title_loc9`,`title_loc10`,`title_loc11`,`description_loc1`,`description_loc2`,`description_loc3`,`description_loc4`,`description_loc5`,`description_loc6`,`description_loc7`,`description_loc8`,`description_loc9`,`description_loc10`,`description_loc11`) VALUES 
(6,'Embersilk Bag',NULL,NULL,NULL,NULL,'Bolsa de seda de ascuas','Bolsa de seda de ascuas',NULL,NULL,NULL,NULL,'Bag of 22 slots',NULL,NULL,NULL,NULL,'Bolsa de 22 casillas','Bolsa de 22 casillas',NULL,NULL,NULL,NULL),
(2,'WoW Token - 01',NULL,NULL,NULL,NULL,'WoW Token - 01','WoW Token - 01',NULL,NULL,NULL,NULL,'1 point for the WoW shop',NULL,NULL,NULL,NULL,'1 punto para la tienda de WoW.','1 punto para la tienda de WoW.',NULL,NULL,NULL,NULL),
(3,'Heart of the Aspects',NULL,NULL,NULL,NULL,'Corazón de los Aspectos','Corazón de los Aspectos',NULL,NULL,NULL,NULL,'Flying Mount',NULL,NULL,NULL,NULL,'Montura Voladora','Montura Voladora',NULL,NULL,NULL,NULL),
(4,'Tyrael\'s Hilt',NULL,NULL,NULL,NULL,'Empeñadura de Tyrael','Empeñadura de Tyrael',NULL,NULL,NULL,NULL,'Teaches you how to summon this companion.',NULL,NULL,NULL,NULL,'Te enseña a convocar a este compañero','Te enseña a convocar a este compañero',NULL,NULL,NULL,NULL),
(5,'Sky Golem',NULL,NULL,NULL,NULL,'Gólem Aéreo','Gólem Aéreo',NULL,NULL,NULL,NULL,'Flying Mount',NULL,NULL,NULL,NULL,'Montura Voladora','Montura Voladora',NULL,NULL,NULL,NULL),
(7,'Gnomish X-Ray Specs',NULL,NULL,NULL,NULL,'Gafas de rayos X gnómicas','Gafas de rayos X gnómicas',NULL,NULL,NULL,NULL,'Allows you TO see players without clothing AND armor.',NULL,NULL,NULL,NULL,'Te permite ver a los jugadores sin ropa ni armadura.','Te permite ver a los jugadores sin ropa ni armadura.',NULL,NULL,NULL,NULL),
(8,'Shrimp Dumplings',NULL,NULL,NULL,NULL,'Empanadillas de gambas','Empanadillas de gambas',NULL,NULL,NULL,NULL,'Grants 250 Spirit',NULL,NULL,NULL,NULL,'Otorga 250 de espíritu','Otorga 250 de espíritu',NULL,NULL,NULL,NULL),
(9,'Puzzle Box of Yogg-Saron',NULL,NULL,NULL,NULL,'Caja de puzles de Yogg-Saron','Caja de puzles de Yogg-Saron',NULL,NULL,NULL,NULL,'Extremely difficult to open',NULL,NULL,NULL,NULL,'Extremadamente difícil de abrir','Extremadamente difícil de abrir',NULL,NULL,NULL,NULL),
(10,'Recipe: Vial of the Sands',NULL,NULL,NULL,NULL,'Receta del vial de las arenas','Receta del vial de las arenas',NULL,NULL,NULL,NULL,'Teaches you how to make the Vial of the Sands.',NULL,NULL,NULL,NULL,'Te enseña la receta del vial de las arenas.','Te enseña la receta del vial de las arenas.',NULL,NULL,NULL,NULL),
(11,'WoW Token - 01',NULL,NULL,NULL,NULL,'WoW Token - 01','WoW Token - 01',NULL,NULL,NULL,NULL,'01 point for the WoW shop',NULL,NULL,NULL,NULL,'01 punto para la tienda WoW','01 punto para la tienda WoW',NULL,NULL,NULL,NULL),
(12,'Boost 90',NULL,NULL,NULL,NULL,'Boost 90','Boost 90',NULL,NULL,NULL,NULL,'Level Up 90',NULL,NULL,NULL,NULL,'Sube de nivel al 90!','Sube de nivel al 90!',NULL,NULL,NULL,NULL),
(13,'Thunderfury, Blessed Blade of the Windseeker',NULL,NULL,NULL,NULL,'Trueno Furioso, Espada Bendita del Hijo del Viento','Trueno Furioso, Espada Bendita del Hijo del Viento',NULL,NULL,NULL,NULL,'Legendary One-Handed Sword',NULL,NULL,NULL,NULL,'Espada legendaria de una mano','Espada legendaria de una mano',NULL,NULL,NULL,NULL),
(14,'WoW Token - 02',NULL,NULL,NULL,NULL,'WoW Token - 02','WoW Token - 02',NULL,NULL,NULL,NULL,'02 points for the WoW shop',NULL,NULL,NULL,NULL,'02 puntos para la tienda WoW','02 puntos para la tienda WoW',NULL,NULL,NULL,NULL),
(15,'WoW Token - 05',NULL,NULL,NULL,NULL,'WoW Token - 05','WoW Token - 05',NULL,NULL,NULL,NULL,'05 points for the WoW shop',NULL,NULL,NULL,NULL,'05 puntos para la tienda WoW','05 puntos para la tienda WoW',NULL,NULL,NULL,NULL),
(16,'WoW Token - 10',NULL,NULL,NULL,NULL,'WoW Token - 10','WoW Token - 10',NULL,NULL,NULL,NULL,'10 points for the WoW shop',NULL,NULL,NULL,NULL,'10 puntos para la tienda WoW','10 puntos para la tienda WoW',NULL,NULL,NULL,NULL);

/*Data for the table `locales_battle_pay_group` */

DELETE FROM `locales_battle_pay_group`;
INSERT  INTO `locales_battle_pay_group`(`id`,`name_loc1`,`name_loc2`,`name_loc3`,`name_loc4`,`name_loc5`,`name_loc6`,`name_loc7`,`name_loc8`,`name_loc9`,`name_loc10`,`name_loc11`) VALUES 
(1,'Featured',NULL,NULL,NULL,NULL,'Destacados','Destacados',NULL,NULL,NULL,NULL),
(2,'Mounts',NULL,NULL,NULL,NULL,'Monturas','Monturas',NULL,NULL,NULL,NULL),
(3,'Bags',NULL,NULL,NULL,NULL,'Bolsas','Bolsas',NULL,NULL,NULL,NULL),
(4,'Pets',NULL,NULL,NULL,NULL,'Mascotas','Mascotas',NULL,NULL,NULL,NULL),
(7,'Food',NULL,NULL,NULL,NULL,'Comida','Comida',NULL,NULL,NULL,NULL),
(5,'Weapons',NULL,NULL,NULL,NULL,'Armas','Armas',NULL,NULL,NULL,NULL),
(6,'Armor',NULL,NULL,NULL,NULL,'Armaduras','Armaduras',NULL,NULL,NULL,NULL),
(8,'Toys',NULL,NULL,NULL,NULL,'Juguetes','Juguetes',NULL,NULL,NULL,NULL),
(9,'Scroll',NULL,NULL,NULL,NULL,'Recetas','Recetas',NULL,NULL,NULL,NULL),
(11,'Services',NULL,NULL,NULL,NULL,'Servicios','Servicios',NULL,NULL,NULL,NULL),
(10,'WoW',NULL,NULL,NULL,NULL,'Fichas de Juego','Fichas de Juego',NULL,NULL,NULL,NULL);

/*Data for the table `locales_battle_pay_product` */

DELETE FROM `locales_battle_pay_product`;
INSERT  INTO `locales_battle_pay_product`(`id`,`title_loc1`,`title_loc2`,`title_loc3`,`title_loc4`,`title_loc5`,`title_loc6`,`title_loc7`,`title_loc8`,`title_loc9`,`title_loc10`,`title_loc11`,`description_loc1`,`description_loc2`,`description_loc3`,`description_loc4`,`description_loc5`,`description_loc6`,`description_loc7`,`description_loc8`,`description_loc9`,`description_loc10`,`description_loc11`) VALUES 
(1,'Shop: Heart of the Aspects',NULL,NULL,NULL,NULL,'Tienda: Corazón de los Aspectos','Tienda: Corazón de los Aspectos',NULL,NULL,NULL,NULL,'Thank you for supporting the project, enjoy your flying Mount',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, distruta tu nueva montura moladora','Gracias por apoyar el proyecto, distruta tu nueva montura moladora',NULL,NULL,NULL,NULL),
(4,'Shop: Embersilk Bag',NULL,NULL,NULL,NULL,'Tienda: Bolsa de seda de ascuas','Tienda: Bolsa de seda de ascuas',NULL,NULL,NULL,NULL,'Thank you for supporting the project, enjoy your bag of 22 slots',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu bolsa de 22 casillas','Gracias por apoyar el proyecto, disfruta de tu bolsa de 22 casillas',NULL,NULL,NULL,NULL),
(2,'Shop: Sky Golem',NULL,NULL,NULL,NULL,'Tienda: Gólem Aéreo','Tienda: Gólem Aéreo',NULL,NULL,NULL,NULL,'Thank you for supporting the project, enjoy your flying Mount',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, distruta tu nueva montura moladora','Gracias por apoyar el proyecto, distruta tu nueva montura moladora',NULL,NULL,NULL,NULL),
(3,'Shop: Tyrael\'s Hilt',NULL,NULL,NULL,NULL,'Tienda: Empeñadura de Tyrael','Tienda: Empeñadura de Tyrael',NULL,NULL,NULL,NULL,'Thank you for supporting the project, enjoy your new companion.',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, distruta de tu nuevo compañero.','Gracias por apoyar el proyecto, distruta de tu nuevo compañero.',NULL,NULL,NULL,NULL),
(100,'Shop: Thunderfury, Blessed Blade of the Windseeker',NULL,NULL,NULL,NULL,'Tienda: Trueno Furioso, Espada Bendita del Hijo del Viento','Tienda: Trueno Furioso, Espada Bendita del Hijo del Viento',NULL,NULL,NULL,NULL,'Thank you for supporting the project, enjoy your legendary One-Handed Sword',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, distruta de tu nuevo item Legendario.','Gracias por apoyar el proyecto, distruta de tu nuevo item Legendario.',NULL,NULL,NULL,NULL),
(5,'Shop: Gnomish X-Ray Specs',NULL,NULL,NULL,NULL,'Tienda: Gafas de rayos X gnómicas','Tienda: Gafas de rayos X gnómicas',NULL,NULL,NULL,NULL,'Allows you to see players without clothing and armor.',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, distruta de tu nuevo item que permite ver a los jugadores sin ropa ni armadura.','Gracias por apoyar el proyecto, distruta de tu nuevo item que permite ver a los jugadores sin ropa ni armadura.',NULL,NULL,NULL,NULL),
(6,'Shop: Shrimp Dumplings',NULL,NULL,NULL,NULL,'Tienda: Empanadillas de gambas','Tienda: Empanadillas de gambas',NULL,NULL,NULL,NULL,'Thank you for supporting the project.',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nuevo item.','Gracias por apoyar el proyecto, disfruta de tu nuevo item.',NULL,NULL,NULL,NULL),
(7,'Shop: Puzzle Box of Yogg-Saron',NULL,NULL,NULL,NULL,'Tienda: Caja de puzles de Yogg-Saron','Tienda: Caja de puzles de Yogg-Saron',NULL,NULL,NULL,NULL,'Thank you for supporting the project.',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nuevo item.','Gracias por apoyar el proyecto, disfruta de tu nuevo item.',NULL,NULL,NULL,NULL),
(8,'Shop: Recipe: Vial of the Sands',NULL,NULL,NULL,NULL,'Tienda: Receta del vial de las arenas','Tienda: Receta del vial de las arenas',NULL,NULL,NULL,NULL,'Thank you for supporting the project, enjoy your Vial de las Arenas recipe',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nueva receta del Vial de las arenas.','Gracias por apoyar el proyecto, disfruta de tu nueva receta del Vial de las arenas.',NULL,NULL,NULL,NULL),
(9,'Shop: WoW Token - 01',NULL,NULL,NULL,NULL,'Tienda: WoW Token - 01','Tienda: WoW Token - 01',NULL,NULL,NULL,NULL,'Thanks for supporting the project, enjoy your new Item!',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nuevo Item!','Gracias por apoyar el proyecto, disfruta de tu nuevo Item!',NULL,NULL,NULL,NULL),
(10,'Shop: WoW Token - 02',NULL,NULL,NULL,NULL,'Tienda: WoW Token - 02','Tienda: WoW Token - 02',NULL,NULL,NULL,NULL,'Thanks for supporting the project, enjoy your new Item!',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nuevo Item!','Gracias por apoyar el proyecto, disfruta de tu nuevo Item!',NULL,NULL,NULL,NULL),
(11,'Shop: WoW Token - 05',NULL,NULL,NULL,NULL,'Tienda: WoW Token - 05','Tienda: WoW Token - 05',NULL,NULL,NULL,NULL,'Thanks for supporting the project, enjoy your new Item!',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nuevo Item!','Gracias por apoyar el proyecto, disfruta de tu nuevo Item!',NULL,NULL,NULL,NULL),
(12,'Shop: WoW Token - 10',NULL,NULL,NULL,NULL,'Tienda: WoW Token - 10','Tienda: WoW Token - 10',NULL,NULL,NULL,NULL,'Thanks for supporting the project, enjoy your new Item!',NULL,NULL,NULL,NULL,'Gracias por apoyar el proyecto, disfruta de tu nuevo Item!','Gracias por apoyar el proyecto, disfruta de tu nuevo Item!',NULL,NULL,NULL,NULL);
