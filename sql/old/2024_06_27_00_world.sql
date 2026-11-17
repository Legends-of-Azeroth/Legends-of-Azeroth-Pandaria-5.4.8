-- Gnome - Irradiated, should be cast on player create, not on area
delete from spell_area where spell = 80653 and area = 5495;

--
-- Table structure for table `playercreateinfo_cast_spell`
--

DROP TABLE IF EXISTS `playercreateinfo_cast_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `playercreateinfo_cast_spell` (
   `raceMask` bigint unsigned NOT NULL,
   `classMask` int unsigned NOT NULL DEFAULT '0',
   `spell` int unsigned NOT NULL DEFAULT '0',
   `note` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci DEFAULT NULL,
   PRIMARY KEY (`raceMask`,`classMask`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

insert into playercreateinfo_cast_spell (raceMask, classMask, spell, note)
values  (0, 32, 48266, 'Death Knight - Frost Presence'),
        (0, 1, 2457, 'Warrior - Battle Stance'),
        (1, 4, 79597, 'Human - Hunter - Young Wolf'),
        (2, 4, 79598, 'Orc - Hunter - Young Boar'),
        (4, 4, 79593, 'Dwarf - Hunter - Young Boar'),
        (8, 4, 79602, 'Night Elf - Hunter - Young Cat'),
        (16, 4, 79600, 'Undead - Hunter - Young Widow'),
        (32, 4, 79603, 'Tauren - Hunter - Young Tallstrider'),
        (128, 4, 79599, 'Troll - Hunter - Young Raptor'),
        (256, 4, 79595, 'Goblin - Hunter - Young Crab'),
        (512, 4, 79594, 'Blood Elf - Hunter - Young Dragonhawk'),
        (1024, 4, 79601, 'Draenei - Hunter - Young Moth'),
        (2097152, 4, 79596, 'Worgen - Hunter - Young Mastiff'),
        (8388608, 4, 107924, 'Pandaren - Hunter - Wise Turtle'),
        (0, 256, 688, 'Warlock - Summon Imp'),
        (16, 925, 73523, 'Undead - Rigor Mortis'),
        (64, 256, 80653, 'Warlock - Gnome - Irradiated Aura'),
        (64, 128, 80653, 'Mage - Gnome - Irradiated Aura'),
        (64, 16, 80653, 'Priest - Gnome - Irradiated Aura'),
        (64, 8, 80653, 'Rogue - Gnome - Irradiated Aura'),
        (64, 1, 80653, 'Warrior - Gnome - Irradiated Aura'),
        (64, 512, 80653, 'Monk - Gnome - Irradiated Aura'),
        (8388608, 1, 108059, 'Pandaren - Warrior - Remove weapon'),
        (8388608, 4, 108061, 'Pandaren - Hunter - Remove weapon'),
        (8388608, 8, 108058, 'Pandaren - Rogue - Remove weapon'),
        (8388608, 16, 108057, 'Pandaren - Priest - Remove weapon'),
        (8388608, 64, 108056, 'Pandaren - Shaman - Remove weapon'),
        (8388608, 384, 108055, 'Pandaren - Mage - Remove weapon'),
        (8388608, 512, 108060, 'Pandaren - Monk - Remove weapon'),
        (8388608, 1, 107922, 'Pandaren - Warrior - Starting quest'),
        (8388608, 4, 107917, 'Pandaren - Hunter - Starting quest'),
        (8388608, 8, 107920, 'Pandaren - Rogue - Starting quest'),
        (8388608, 16, 107919, 'Pandaren - Priest - Starting quest'),
        (8388608, 64, 107921, 'Pandaren - Shaman - Starting quest'),
        (8388608, 384, 107916, 'Pandaren - Mage - Starting quest'),
        (8388608, 512, 107915, 'Pandaren - Monk - Starting quest');
