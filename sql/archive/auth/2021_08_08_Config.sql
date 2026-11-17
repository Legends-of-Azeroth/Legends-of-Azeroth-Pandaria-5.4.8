-- agregando variables de configuración faltantes en el servidor

-- estos datos fueron proporcionados por un tercero(anterior dueño del core)
DELETE from `config`;

insert into `config` (`category`, `name`, `value`, `default`, `realmid`, `description`, `note`) values
('AntiCheat','Anticheat.DetectionsEnabled','27','31','1','Anticheat hack',NULL),
('AntiCheat','Anticheat.ReportsForIngameWarnings','70','1','1','Anticheat hack',NULL),
('Dungeon Finder','DungeonFinder.DPSNeeded','1','1','1','Specifies how many players have to select dps role in order for the party to be eligible for a dungeon.\nDefault:     1',NULL),
('Dungeon Finder','DungeonFinder.TanksNeeded','1','1','1','Specifies how many players have to select tank role in order for the party to be eligible for a dungeon.\nDefault:     1',NULL),
('Dungeon Finder','DungeonFinder.CastDeserter','0','1','1','Cast Deserter spell at players who leave DF-parties while the dungeons is in progress.\nDefault:     1',NULL),
('Dungeon Finder','DungeonFinder.HealersNeeded','1','1','1','Specifies how many players have to select healer role in order for the party to be eligible for a dungeon.\nDefault:     1',NULL),
('Dungeon Finder','DungeonFinder.ShortagePercent','50','50','1','Specifies at which percent of the ideal role representation ratio in a queue should it grant a Call to Arms reward\nDefault:     50',NULL),
('Dungeon Finder','DungeonFinder.KeepQueuesInDungeon','true','1','1','Specifies how many players have to select healer role in order for the party to be eligible for a dungeon.\nDefault:     1',NULL),
('Dungeon Finder','DungeonFinder.OverrideRolesRequired','true','0','1','If enabled all LFG role checks will use values from DungeonFinder.TanksNeeded, DungeonFinder.HealersNeeded and DungeonFinder.DPSNeeded to determine the number of players required in order for the party to be eligible for a dungeon. Overrides both group and raid dungeon values.\nDefault:     0',NULL),
('Dungeon Finder','DungeonFinder.ShortageCheckInterval','60','60','1','Specifies the interval in seconds at which shortage roles for a queue should be updated.\nDefault:     5',NULL),
('LFR','LFR.LootChance','100','20','-1','Chance to get personal loot from any boss',NULL),
('Misc','VengeanceMultipier','1','1','-1',NULL,NULL),
('Performance','Visibility.AINotifyDelay','1000','1000','1','Description: delay (in milliseconds) between action and nearby AI reaction. Lower values may have\n             performance impact.\nDefault:     1000',NULL),
('Performance','Visibility.RelocationLowerLimit','10','20','1','Description: distance in yards. unit should pass that distance to trigger visibility update.\nDefault:     10',NULL),
('Players','AllowTwoSide.Interaction.Chat','0','0','-1','Allow chat assembling between factions.\nDefault:     0',NULL),
('Players','AllowTwoSide.Interaction.Auction','0','0','1','Merge all auction houses for players from different teams\nDefault: 0 (Not allowed)\n         1 (Allowed)',NULL),
('Rate','XP.Kill.Difference','10','0','1','',NULL),
('Rate','BonusRoll.LootChance','31','31','1','Bonus Roll',NULL),
('Rate','XP.Gather.Difference','10','0','1','',NULL),
('Rate','TargetPosRecalculateRange','5','5','1','Range attack ',NULL);
