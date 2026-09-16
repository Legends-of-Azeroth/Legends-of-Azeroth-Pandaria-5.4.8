SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account`  (
  `id` int unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL DEFAULT '',
  `battlenet_account` varchar(32) NOT NULL DEFAULT '',
  `sha_pass_hash` varchar(40) NOT NULL DEFAULT '',
  `sessionkey` varchar(80) NOT NULL DEFAULT '',
  `v` varchar(64) NOT NULL DEFAULT '',
  `s` varchar(64) NOT NULL DEFAULT '',
  `token_key` varchar(100) NOT NULL DEFAULT '',
  `totp_secret` varbinary(128) DEFAULT NULL,
  `email` varchar(255) NOT NULL DEFAULT '',
  `reg_mail` varchar(255) NOT NULL DEFAULT '',
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `failed_logins` int unsigned NOT NULL DEFAULT '0',
  `locked` tinyint unsigned NOT NULL DEFAULT '0',
  `lock_country` varchar(2) NOT NULL DEFAULT '00',
  `last_login` timestamp NULL DEFAULT NULL,
  `online` tinyint unsigned NOT NULL DEFAULT '0',
  `expansion` tinyint unsigned NOT NULL DEFAULT '4',
  `mutetime` bigint NOT NULL DEFAULT '0',
  `mutereason` varchar(255) NOT NULL DEFAULT '',
  `muteby` varchar(50) NOT NULL DEFAULT '',
  `locale` tinyint unsigned NOT NULL DEFAULT '0',
  `os` varchar(4) NOT NULL DEFAULT '',
  `recruiter` int unsigned NOT NULL DEFAULT '0',
  `project_member_id` int unsigned NOT NULL DEFAULT '0',
  `rank` int DEFAULT NULL,
  `staff_id` int DEFAULT NULL,
  `vp` int DEFAULT NULL,
  `dp` int NOT NULL DEFAULT '0',
  `isactive` varchar(50) DEFAULT NULL,
  `activation` varchar(256) DEFAULT NULL,
  `invited_by` varchar(32) NOT NULL DEFAULT '',
  `inv_friend_acc` varchar(32) NOT NULL DEFAULT '',
  `rewarded` int NOT NULL DEFAULT '0',
  `flags` int NOT NULL DEFAULT '0',
  `gmlevel` tinyint unsigned NOT NULL DEFAULT '0',
  `active_realm_id` int unsigned NOT NULL DEFAULT '0',
  `online_mute_timer` bigint unsigned NOT NULL DEFAULT '0',
  `active_mute_id` int unsigned NOT NULL DEFAULT '0',
  `project_verified` tinyint(1) NOT NULL DEFAULT '0',
  `cash` int NOT NULL DEFAULT '0',
  `project_is_free` tinyint(1) NOT NULL DEFAULT '0',
  `project_is_temp` tinyint(1) NOT NULL DEFAULT '0',
  `project_unban_count` tinyint NOT NULL DEFAULT '0',
  `project_antierror` int unsigned DEFAULT NULL,
  `project_attached` int unsigned DEFAULT NULL,
  `project_passchange` int unsigned NOT NULL DEFAULT '0',
  `project_vote_time` bigint NOT NULL DEFAULT '0',
  `project_hwid` varchar(40) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `idx_username`(`username` ASC) USING BTREE,
  INDEX `idx_id`(`id` ASC) USING BTREE,
  INDEX `idx_sha`(`sha_pass_hash` ASC) USING BTREE,
  INDEX `idx_session`(`sessionkey` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Account System' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account
-- ----------------------------
INSERT INTO `account` (`id`, `username`, `battlenet_account`, `sha_pass_hash`, `sessionkey`, `v`, `s`, `token_key`, `totp_secret`, `email`, `reg_mail`, `joindate`, `last_ip`, `failed_logins`, `locked`, `lock_country`, `last_login`, `online`, `expansion`, `mutetime`, `mutereason`, `muteby`, `locale`, `os`, `recruiter`, `project_member_id`, `rank`, `staff_id`, `vp`, `dp`, `isactive`, `activation`, `invited_by`, `inv_friend_acc`, `rewarded`, `flags`, `gmlevel`, `active_realm_id`, `online_mute_timer`, `active_mute_id`, `project_verified`, `cash`, `project_is_free`, `project_is_temp`, `project_unban_count`, `project_antierror`, `project_attached`, `project_passchange`, `project_vote_time`, `project_hwid`) 
VALUES (1, 'TEST', '', '3D0D99423E31FCC67A6745EC89D70D700344BC76', 'A4BEAD4BBD62A0C1F5C1BA3CE857C7E587B1AFD1EBF2543F4EAFFC87EC09DE8EF51AA142D04ED259', '77733268AF0D167253BC9097A6BDDAA4E55B66EBFBC5432350DF549084F29D75', 'CC28EADCB56862623B8211A43B96F5FD1B8667464349816AE225487CC0425AB1', '', NULL, '', '', '2021-11-03 17:57:54', '25.90.193.232', 0, 0, '00', '2021-11-04 20:22:34', 0, 4, 0, '', '', 6, 'Win', 0, 0, NULL, NULL, NULL, 0, NULL, NULL, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, '');

-- ----------------------------
-- Table structure for account_access
-- ----------------------------
DROP TABLE IF EXISTS `account_access`;
CREATE TABLE `account_access`  (
  `id` int UNSIGNED NOT NULL,
  `gmlevel` tinyint UNSIGNED NOT NULL,
  `RealmID` int NOT NULL DEFAULT -1,
  `name` varchar(255) NULL DEFAULT NULL,
  PRIMARY KEY (`id`, `RealmID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_access
-- ----------------------------
INSERT INTO `account_access` VALUES (1, 100, -1, NULL);

-- ----------------------------
-- Table structure for account_banned
-- ----------------------------
DROP TABLE IF EXISTS `account_banned`;
CREATE TABLE `account_banned`  (
  `id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Account id',
  `realm` int NOT NULL,
  `bandate` int UNSIGNED NOT NULL DEFAULT 0,
  `unbandate` int UNSIGNED NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`, `bandate`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Ban List' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_banned
-- ----------------------------

-- ----------------------------
-- Table structure for account_boost
-- ----------------------------
DROP TABLE IF EXISTS `account_boost`;
CREATE TABLE `account_boost`  (
  `id` int NOT NULL DEFAULT 0,
  `realmid` int UNSIGNED NOT NULL DEFAULT 1,
  `counter` int UNSIGNED NOT NULL DEFAULT 0
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_boost
-- ----------------------------

-- ----------------------------
-- Table structure for account_muted
-- ----------------------------
DROP TABLE IF EXISTS `account_muted`;
CREATE TABLE `account_muted`  (
  `id` int NOT NULL,
  `realmid` int NOT NULL DEFAULT 0,
  `acc_id` int NOT NULL,
  `char_id` int NOT NULL,
  `mute_acc` varchar(32) NOT NULL,
  `mute_name` varchar(50) NOT NULL,
  `mute_date` bigint NOT NULL,
  `muted_by` varchar(50) NOT NULL,
  `mute_time` bigint NOT NULL,
  `mute_reason` varchar(255) NOT NULL,
  `public_channels_only` tinyint NOT NULL,
  PRIMARY KEY (`realmid`, `id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of account_muted
-- ----------------------------

-- ----------------------------
-- Table structure for arena_game_id
-- ----------------------------
DROP TABLE IF EXISTS `arena_game_id`;
CREATE TABLE `arena_game_id`  (
  `game_id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `realm_id` tinyint UNSIGNED NOT NULL,
  PRIMARY KEY (`game_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of arena_game_id
-- ----------------------------

-- ----------------------------
-- Table structure for arena_games
-- ----------------------------
DROP TABLE IF EXISTS `arena_games`;
CREATE TABLE `arena_games`  (
  `gameid` bigint NOT NULL DEFAULT 0,
  `teamid` bigint NOT NULL DEFAULT 0,
  `guid` bigint NOT NULL DEFAULT 0,
  `changeType` int NOT NULL,
  `ratingChange` int NOT NULL,
  `teamRating` int NOT NULL,
  `matchMakerRating` smallint UNSIGNED NULL DEFAULT NULL,
  `damageDone` int NOT NULL,
  `deaths` int NOT NULL,
  `healingDone` int NOT NULL,
  `damageTaken` int NOT NULL,
  `healingTaken` int NOT NULL,
  `killingBlows` int NOT NULL,
  `damageAbsorbed` int UNSIGNED NOT NULL,
  `timeControlled` int UNSIGNED NOT NULL,
  `aurasDispelled` int UNSIGNED NOT NULL,
  `aurasStolen` int UNSIGNED NOT NULL,
  `highLatencyTimes` int UNSIGNED NOT NULL,
  `spellsPrecast` int UNSIGNED NOT NULL,
  `mapId` int NOT NULL,
  `start` int NOT NULL,
  `end` int NOT NULL,
  `class` tinyint UNSIGNED NULL DEFAULT NULL,
  `season` smallint UNSIGNED NULL DEFAULT NULL,
  `type` tinyint UNSIGNED NULL DEFAULT NULL,
  `realmid` tinyint UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`gameid`, `teamid`, `guid`) USING BTREE,
  INDEX `idx__teamid`(`teamid` ASC) USING BTREE,
  INDEX `idx__season__class__type`(`season` ASC, `class` ASC, `type` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'WoWArmory Game Chart' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of arena_games
-- ----------------------------

-- ----------------------------
-- Table structure for arena_match_stat
-- ----------------------------
DROP TABLE IF EXISTS `arena_match_stat`;
CREATE TABLE `arena_match_stat`  (
  `realm` tinyint UNSIGNED NOT NULL,
  `teamGuid` int UNSIGNED NOT NULL,
  `teamName` text NOT NULL,
  `type` tinyint UNSIGNED NOT NULL,
  `teamRating` smallint UNSIGNED NOT NULL,
  `player` int UNSIGNED NOT NULL,
  `class` tinyint UNSIGNED NOT NULL,
  `name` text NOT NULL,
  `time` int UNSIGNED NOT NULL,
  `mapID` smallint UNSIGNED NOT NULL,
  `instanceID` int UNSIGNED NOT NULL,
  `status` tinyint UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`teamGuid`, `realm`, `player`, `instanceID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of arena_match_stat
-- ----------------------------

-- ----------------------------
-- Table structure for arena_team
-- ----------------------------
DROP TABLE IF EXISTS `arena_team`;
CREATE TABLE `arena_team`  (
  `arenaTeamId` int UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(24) NOT NULL,
  `captainGuid` int UNSIGNED NOT NULL DEFAULT 0,
  `realmNumber` int UNSIGNED NOT NULL DEFAULT 1,
  `type` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `rating` smallint UNSIGNED NOT NULL DEFAULT 0,
  `matchMakerRating` smallint UNSIGNED NOT NULL DEFAULT 1500,
  `seasonGames` smallint UNSIGNED NOT NULL DEFAULT 0,
  `seasonWins` smallint UNSIGNED NOT NULL DEFAULT 0,
  `weekGames` smallint UNSIGNED NOT NULL DEFAULT 0,
  `weekWins` smallint UNSIGNED NOT NULL DEFAULT 0,
  `rank` int UNSIGNED NOT NULL DEFAULT 0,
  `backgroundColor` int UNSIGNED NOT NULL DEFAULT 0,
  `emblemStyle` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `emblemColor` int UNSIGNED NOT NULL DEFAULT 0,
  `borderStyle` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `borderColor` int UNSIGNED NOT NULL DEFAULT 0,
  `season` int UNSIGNED NOT NULL DEFAULT 0,
  `created` int UNSIGNED NOT NULL DEFAULT 0,
  `deleted` int UNSIGNED NOT NULL DEFAULT 0,
  `realmid` tinyint UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`arenaTeamId`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of arena_team
-- ----------------------------

-- ----------------------------
-- Table structure for arena_team_member
-- ----------------------------
DROP TABLE IF EXISTS `arena_team_member`;
CREATE TABLE `arena_team_member`  (
  `arenaTeamId` int UNSIGNED NOT NULL DEFAULT 0,
  `guid` int UNSIGNED NOT NULL DEFAULT 0,
  `realmid` tinyint UNSIGNED NOT NULL DEFAULT 1,
  `personalRating` smallint NOT NULL DEFAULT 0,
  `matchMakerRating` smallint UNSIGNED NOT NULL DEFAULT 1500,
  `weekGames` smallint UNSIGNED NOT NULL DEFAULT 0,
  `weekWins` smallint UNSIGNED NOT NULL DEFAULT 0,
  `seasonGames` smallint UNSIGNED NOT NULL DEFAULT 0,
  `seasonWins` smallint UNSIGNED NOT NULL DEFAULT 0,
  `name` varchar(12) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `class` tinyint UNSIGNED NOT NULL,
  `joined` int UNSIGNED NOT NULL DEFAULT 0,
  `removed` int UNSIGNED NOT NULL DEFAULT 0,
  `itemLevel` smallint UNSIGNED NOT NULL DEFAULT 0,
  `lastILvlCheck` int UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`arenaTeamId`, `guid`, `realmid`) USING BTREE,
  INDEX `guid`(`guid` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of arena_team_member
-- ----------------------------

-- ----------------------------
-- Table structure for armory_game_chart
-- ----------------------------
DROP TABLE IF EXISTS `armory_game_chart`;
CREATE TABLE `armory_game_chart`  (
  `gameid` int NOT NULL,
  `realmid` tinyint UNSIGNED NOT NULL DEFAULT 1,
  `teamid` int NOT NULL,
  `guid` int NOT NULL,
  `changeType` int NOT NULL,
  `ratingChange` int NOT NULL,
  `teamRating` int NOT NULL,
  `damageDone` int NOT NULL,
  `deaths` int NOT NULL,
  `healingDone` int NOT NULL,
  `damageTaken` int NOT NULL,
  `healingTaken` int NOT NULL,
  `killingBlows` int NOT NULL,
  `mapId` int NOT NULL,
  `start` int NOT NULL,
  `end` int NOT NULL,
  `class` tinyint UNSIGNED NULL DEFAULT NULL,
  `season` smallint UNSIGNED NULL DEFAULT NULL,
  `type` tinyint UNSIGNED NULL DEFAULT NULL
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of armory_game_chart
-- ----------------------------

-- ----------------------------
-- Table structure for autobroadcast
-- ----------------------------
DROP TABLE IF EXISTS `autobroadcast`;
CREATE TABLE `autobroadcast`  (
  `realmid` int NOT NULL DEFAULT -1,
  `id` tinyint UNSIGNED NOT NULL AUTO_INCREMENT,
  `weight` tinyint UNSIGNED NULL DEFAULT 1,
  `text` longtext NOT NULL,
  PRIMARY KEY (`id`, `realmid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of autobroadcast
-- ----------------------------

-- ----------------------------
-- Table structure for battleground_games
-- ----------------------------
DROP TABLE IF EXISTS `battleground_games`;
CREATE TABLE `battleground_games`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `season` int UNSIGNED NOT NULL,
  `realm_id` int UNSIGNED NOT NULL,
  `map_id` int UNSIGNED NOT NULL,
  `instance_id` int UNSIGNED NOT NULL,
  `is_random_bg` tinyint UNSIGNED NOT NULL,
  `winner` enum('H','A','N') NOT NULL,
  `start_time` int UNSIGNED NOT NULL,
  `duration` int UNSIGNED NOT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `realm_id`(`realm_id` ASC) USING BTREE,
  INDEX `map_id`(`map_id` ASC) USING BTREE,
  INDEX `season`(`season` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battleground_games
-- ----------------------------

-- ----------------------------
-- Table structure for battleground_ladder_criteria
-- ----------------------------
DROP TABLE IF EXISTS `battleground_ladder_criteria`;
CREATE TABLE `battleground_ladder_criteria`  (
  `criteria` enum('Win','Loss','FastWin','Kills','ObjectiveCaptures','ObjectiveDefenses','DailyWins','DailyKills','SeasonKills','SeasonWinsAV','SeasonWinsWG','SeasonWinsAB','SeasonWinsEotS','SeasonWinsSotA','SeasonWinsIoC','TotalWins','TotalDraws','TotalLosses','TotalKills','TotalLeavesBeforeGame','TotalLeavesDuringGame') NOT NULL COMMENT 'Name of the column in `battleground_ladder_progress` if `type` is \'Daily\', \'Season\' or \'Statistic\'',
  `type` enum('Statistic','Season','Daily','Alterac Valley','Warsong Gulch','Arathi Basin','Eye of the Storm','Strand of the Ancients','Isle of Conquest') NOT NULL COMMENT 'If not set - affects all battlegrounds, if set - overrides global setting only for the specified battleground. Only valid for battleground-specific `type`s',
  `param` int NOT NULL DEFAULT 0,
  `name` tinytext NOT NULL,
  `cap` int UNSIGNED NOT NULL COMMENT 'Maximum count of progress units a player can get',
  `ladder_points_per_progress` int NOT NULL DEFAULT 0 COMMENT 'Repeatable ladder points reward for each unit of progress in this criteria',
  `ladder_points_for_cap` int NOT NULL DEFAULT 0 COMMENT 'One-time ladder points reward for reaching progress cap in this criteria',
  `group_penalty_size` int UNSIGNED NOT NULL DEFAULT 3 COMMENT 'Count of group members at which ladder points penalty from `group_penalty_percent` kicks in',
  `group_penalty_percent` int NOT NULL DEFAULT 0 COMMENT 'Percentage modifier of ladder points for each player in the group above or equal to `group_penalty_size`',
  PRIMARY KEY (`criteria`, `type`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battleground_ladder_criteria
-- ----------------------------

-- ----------------------------
-- Table structure for battleground_ladder_rewards
-- ----------------------------
DROP TABLE IF EXISTS `battleground_ladder_rewards`;
CREATE TABLE `battleground_ladder_rewards`  (
  `season` int UNSIGNED NOT NULL COMMENT 'Battleground season ID',
  `id` int UNSIGNED NOT NULL COMMENT 'Incrementing number identifying this reward set',
  `top` float UNSIGNED NOT NULL COMMENT 'How many players will receive the reward. Depends on `type`',
  `type` enum('Players','Percents') NOT NULL DEFAULT 'Players' COMMENT 'Determines whether the `top` number or `top` percentage of players will receive the reward',
  `money_reward` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Amount in copper',
  `item_reward` tinytext NOT NULL COMMENT 'Format: itemid:count itemid:count ...',
  `loyalty_reward` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Number of Orbs of Loyalty',
  `premium_reward` tinytext NOT NULL COMMENT 'Duration in timestring format (e.g. \"30d5h42m10s\")',
  `title_reward` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Title ID',
  `mail_subject` tinytext NOT NULL,
  `mail_text` text NOT NULL,
  PRIMARY KEY (`season`, `id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battleground_ladder_rewards
-- ----------------------------

-- ----------------------------
-- Table structure for battleground_scores
-- ----------------------------
DROP TABLE IF EXISTS `battleground_scores`;
CREATE TABLE `battleground_scores`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Unique identifier for each player that participated in the battleground',
  `game_id` int UNSIGNED NOT NULL,
  `team` tinyint UNSIGNED NOT NULL,
  `guid` int UNSIGNED NOT NULL,
  `realm_id` int UNSIGNED NOT NULL,
  `group_index` int UNSIGNED NOT NULL,
  `ladder_points` int NOT NULL,
  `killing_blows` int UNSIGNED NOT NULL,
  `deaths` int UNSIGNED NOT NULL,
  `honorable_kills` int UNSIGNED NOT NULL,
  `damage_done` int UNSIGNED NOT NULL,
  `healing_done` int UNSIGNED NOT NULL,
  `damage_taken` int UNSIGNED NOT NULL,
  `healing_taken` int UNSIGNED NOT NULL,
  `bonus_honor` int UNSIGNED NOT NULL,
  `graveyards_assaulted` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `graveyards_defended` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `towers_assaulted` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `towers_defended` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `mines_captured` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `leaders_killed` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `secondary_objective` int UNSIGNED NULL DEFAULT NULL COMMENT 'Alterac Valley',
  `flag_captures` int UNSIGNED NULL DEFAULT NULL COMMENT 'Warsong Gulch, Eye of the Storm',
  `flag_returns` int UNSIGNED NULL DEFAULT NULL COMMENT 'Warsong Gulch',
  `bases_assaulted` int UNSIGNED NULL DEFAULT NULL COMMENT 'Arathi Basin, Isle of Conquest',
  `bases_defended` int UNSIGNED NULL DEFAULT NULL COMMENT 'Arathi Basin, Isle of Conquest',
  `demolishers_destroyed` int UNSIGNED NULL DEFAULT NULL COMMENT 'Strand of the Ancients',
  `gates_destroyed` int UNSIGNED NULL DEFAULT NULL COMMENT 'Strand of the Ancients',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `game_id`(`game_id` ASC) USING BTREE,
  INDEX `guid`(`guid` ASC) USING BTREE,
  INDEX `realm_id`(`realm_id` ASC) USING BTREE,
  CONSTRAINT `FK_battleground_scores_battleground_games` FOREIGN KEY (`game_id`) REFERENCES `battleground_games` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battleground_scores
-- ----------------------------

-- ----------------------------
-- Table structure for battleground_seasons
-- ----------------------------
DROP TABLE IF EXISTS `battleground_seasons`;
CREATE TABLE `battleground_seasons`  (
  `id` tinyint NOT NULL AUTO_INCREMENT,
  `begin` int UNSIGNED NOT NULL,
  `end` int UNSIGNED NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battleground_seasons
-- ----------------------------

-- ----------------------------
-- Table structure for battlenet_account_bans
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_account_bans`;
CREATE TABLE `battlenet_account_bans`  (
  `id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Account id',
  `bandate` int UNSIGNED NOT NULL DEFAULT 0,
  `unbandate` int UNSIGNED NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  PRIMARY KEY (`id`, `bandate`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Ban List' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battlenet_account_bans
-- ----------------------------

-- ----------------------------
-- Table structure for battlenet_accounts
-- ----------------------------
DROP TABLE IF EXISTS `battlenet_accounts`;
CREATE TABLE `battlenet_accounts`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `email` varchar(320) NOT NULL,
  `sha_pass_hash` varchar(64) NOT NULL DEFAULT '',
  `v` varchar(256) NOT NULL DEFAULT '',
  `s` varchar(64) NOT NULL DEFAULT '',
  `sessionKey` varchar(128) NOT NULL DEFAULT '',
  `locked` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `last_login` timestamp NULL DEFAULT NULL,
  `online` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `last_ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `failed_logins` int UNSIGNED NOT NULL DEFAULT 0,
  `project_member_id` int UNSIGNED NULL DEFAULT NULL,
  `project_is_temp` tinyint(1) NULL DEFAULT 0 COMMENT 'nighthold',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Account System' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battlenet_accounts
-- ----------------------------

-- ----------------------------
-- Table structure for battlepay_log
-- ----------------------------
DROP TABLE IF EXISTS `battlepay_log`;
CREATE TABLE `battlepay_log`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `accountId` int UNSIGNED NOT NULL,
  `characterGuid` int NOT NULL DEFAULT 0,
  `realm` int UNSIGNED NOT NULL,
  `item` int NOT NULL DEFAULT 0,
  `price` int UNSIGNED NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of battlepay_log
-- ----------------------------

-- ----------------------------
-- Table structure for bonus_rates
-- ----------------------------
DROP TABLE IF EXISTS `bonus_rates`;
CREATE TABLE `bonus_rates`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Autoincrementable ID',
  `realmid` int NOT NULL DEFAULT -1 COMMENT 'RealmID for which the rates would be active. -1 for all realms',
  `active` tinyint UNSIGNED NOT NULL DEFAULT 1 COMMENT 'If set to 0 - this bonus would not be loaded',
  `schedule` varchar(50) NOT NULL DEFAULT '* * * * *' COMMENT 'Cron-style schedule defining the time for the bonus rates period. Multiple periods can be specified with a semicolon separated list',
  `multiplier` float NOT NULL DEFAULT 2 COMMENT 'Rate multiplier (i.e. 2 would change the rates to be twice their usual value during the bonus rates period)',
  `rates` text NOT NULL COMMENT 'Space separated list of rate names as used in config (i.e. \"Rate.XP.Kill Rate.Honor\")',
  `start_announcement` tinytext NULL COMMENT 'Announcement displayed in chat for all online players when the bonus rate period starts',
  `end_announcement` tinytext NULL COMMENT 'Announcement displayed in chat for all online players when the bonus rate period ends',
  `active_announcement` tinytext NULL COMMENT 'Announcement displayed in chat for all players logging in whenever the bonus rate period is active',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of bonus_rates
-- ----------------------------

-- ----------------------------
-- Table structure for boost_promotion_executed
-- ----------------------------
DROP TABLE IF EXISTS `boost_promotion_executed`;
CREATE TABLE `boost_promotion_executed`  (
  `member_id` int NOT NULL
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of boost_promotion_executed
-- ----------------------------

-- ----------------------------
-- Table structure for ip_banned
-- ----------------------------
DROP TABLE IF EXISTS `ip_banned`;
CREATE TABLE `ip_banned`  (
  `ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `bandate` int UNSIGNED NOT NULL,
  `unbandate` int UNSIGNED NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(255) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`ip`, `bandate`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Banned IPs' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of ip_banned
-- ----------------------------

-- ----------------------------
-- Table structure for logs
-- ----------------------------
DROP TABLE IF EXISTS `logs`;
CREATE TABLE `logs`  (
  `time` int UNSIGNED NOT NULL,
  `realm` int UNSIGNED NOT NULL,
  `type` tinyint UNSIGNED NOT NULL,
  `level` tinyint UNSIGNED NOT NULL DEFAULT 0,
  `string` text NULL
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of logs
-- ----------------------------

-- ----------------------------
-- Table structure for mute_active
-- ----------------------------
DROP TABLE IF EXISTS `mute_active`;
CREATE TABLE `mute_active`  (
  `realmid` tinyint NOT NULL,
  `account` int NOT NULL,
  `mute_id` int NOT NULL,
  `mute_timer` int NOT NULL,
  PRIMARY KEY (`realmid`, `account`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of mute_active
-- ----------------------------

-- ----------------------------
-- Table structure for pay_history
-- ----------------------------
DROP TABLE IF EXISTS `pay_history`;
CREATE TABLE `pay_history`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `orderNo` varchar(255) NULL DEFAULT NULL,
  `synType` varchar(16) NULL DEFAULT NULL,
  `status` varchar(64) NULL DEFAULT NULL,
  `price` float(10, 2) NULL DEFAULT NULL,
  `time` varchar(32) NULL DEFAULT NULL,
  `cpparam` varchar(256) NULL DEFAULT NULL,
  `username` varchar(128) NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'not used, don\'t know how to make payment work.' ROW_FORMAT = COMPACT;

-- ----------------------------
-- Records of pay_history
-- ----------------------------

-- ----------------------------
-- Table structure for promocodes
-- ----------------------------
DROP TABLE IF EXISTS `promocodes`;
CREATE TABLE `promocodes`  (
  `code` varchar(50) NOT NULL COMMENT 'Latin letters, digits and dash symbol are allowed, case insensitive',
  `realmid` int NOT NULL DEFAULT -1 COMMENT 'Realm the code can be redeemed on or -1 for any realm',
  `start_time` int UNSIGNED NOT NULL COMMENT 'Activation UNIX timestamp, 0 if always active',
  `end_time` int UNSIGNED NOT NULL COMMENT 'Expiration UNIX timestamp, 0 if never expires',
  `money` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Copper',
  `items` tinytext NOT NULL COMMENT 'Format: itemid:count itemid:count ...',
  `premium` tinytext NOT NULL COMMENT 'Duration in timestring format (e.g. \"30d5h42m10s\")',
  `redeemed` tinyint UNSIGNED NOT NULL DEFAULT 0 COMMENT '1 if the code was redeemed, 0 otherwise',
  `redeemer_guid` int UNSIGNED NOT NULL COMMENT 'Character GUID that redeemed the code',
  `redeemer_realmid` int UNSIGNED NOT NULL COMMENT 'Character\'s realm',
  `redeemer_account` int UNSIGNED NOT NULL COMMENT 'Character\'s account',
  `redeemer_member` int UNSIGNED NOT NULL COMMENT 'Character\'s nighthold member',
  PRIMARY KEY (`code`) USING BTREE,
  INDEX `realmid`(`realmid` ASC) USING BTREE,
  INDEX `redeemed`(`redeemed` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'used for custom\r\ntodo: make this implement into blizzcms' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of promocodes
-- ----------------------------

-- ----------------------------
-- Table structure for promotion_auras
-- ----------------------------
DROP TABLE IF EXISTS `promotion_auras`;
CREATE TABLE `promotion_auras`  (
  `entry` int UNSIGNED NOT NULL,
  `start_date` int UNSIGNED NOT NULL,
  `lenght` int UNSIGNED NOT NULL COMMENT 'Lenght in minutes',
  `active` tinyint UNSIGNED NOT NULL,
  `comment` text NOT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of promotion_auras
-- ----------------------------

-- ----------------------------
-- Table structure for rbac_account_groups
-- ----------------------------
DROP TABLE IF EXISTS `rbac_account_groups`;
CREATE TABLE `rbac_account_groups`  (
  `accountId` int UNSIGNED NOT NULL COMMENT 'Account id',
  `groupId` int UNSIGNED NOT NULL COMMENT 'Group id',
  `realmId` int NOT NULL DEFAULT -1 COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`, `groupId`, `realmId`) USING BTREE,
  INDEX `fk__rbac_account_groups__rbac_groups`(`groupId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Account-Group relation' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of rbac_account_groups
-- ----------------------------

-- ----------------------------
-- Table structure for rbac_account_permissions
-- ----------------------------
DROP TABLE IF EXISTS `rbac_account_permissions`;
CREATE TABLE `rbac_account_permissions`  (
  `accountId` int UNSIGNED NOT NULL COMMENT 'Account id',
  `permissionId` int UNSIGNED NOT NULL COMMENT 'Permission id',
  `granted` tinyint(1) NOT NULL DEFAULT 1 COMMENT 'Granted = 1, Denied = 0',
  `realmId` int NOT NULL DEFAULT -1 COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`, `permissionId`, `realmId`) USING BTREE,
  INDEX `fk__rbac_account_roles__rbac_permissions`(`permissionId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Account-Permission relation' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of rbac_account_permissions
-- ----------------------------

-- ----------------------------
-- Table structure for rbac_default_permissions
-- ----------------------------
DROP TABLE IF EXISTS `rbac_default_permissions`;
CREATE TABLE `rbac_default_permissions`  (
  `secId` int UNSIGNED NOT NULL COMMENT 'Security Level id',
  `permissionId` int UNSIGNED NOT NULL COMMENT 'permission id',
  `realmId` int NOT NULL DEFAULT -1 COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`secId`, `permissionId`, `realmId`) USING BTREE,
  INDEX `fk__rbac_default_permissions__rbac_permissions`(`permissionId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Default permission to assign to different account security levels' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of rbac_default_permissions
-- ----------------------------
REPLACE INTO `rbac_default_permissions` (`secId`, `permissionId`, `realmId`) VALUES
	(3, 192, -1),
	(2, 193, -1),
	(1, 194, -1),
	(0, 195, -1);

-- ----------------------------
-- Table structure for rbac_linked_permissions
-- ----------------------------
DROP TABLE IF EXISTS `rbac_linked_permissions`;
CREATE TABLE `rbac_linked_permissions`  (
  `id` int UNSIGNED NOT NULL COMMENT 'Permission id',
  `linkedId` int UNSIGNED NOT NULL COMMENT 'Linked Permission id',
  PRIMARY KEY (`id`, `linkedId`) USING BTREE,
  INDEX `fk__rbac_linked_permissions__rbac_permissions1`(`id`) USING BTREE,
  INDEX `fk__rbac_linked_permissions__rbac_permissions2`(`linkedId`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Permission - Linked Permission relation' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of rbac_linked_permissions
-- ----------------------------
REPLACE INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
	(192, 7),
	(192, 21),
	(192, 42),
	(192, 43),
	(192, 193),
	(192, 196),
	(192, 778),
	(192, 779),
	(192, 780),
	(192, 781),
	(192, 782),
	(192, 783),
	(192, 784),
	(192, 785),
	(192, 786),
	(192, 787),
	(192, 788),
	(192, 789),
	(192, 790),
	(192, 791),
	(192, 792),
	(192, 793),
	(192, 794),
	(192, 795),
	(192, 796),
	(192, 835),
	(192, 1042),
	(192, 1043),
	(192, 1044),
	(193, 48),
	(193, 52),
	(193, 53),
	(193, 194),
	(193, 197),
	(194, 1),
	(194, 2),
	(194, 9),
	(194, 11),
	(194, 13),
	(194, 14),
	(194, 15),
	(194, 16),
	(194, 17),
	(194, 18),
	(194, 19),
	(194, 20),
	(194, 22),
	(194, 23),
	(194, 25),
	(194, 26),
	(194, 27),
	(194, 28),
	(194, 29),
	(194, 30),
	(194, 31),
	(194, 32),
	(194, 33),
	(194, 34),
	(194, 35),
	(194, 36),
	(194, 37),
	(194, 38),
	(194, 39),
	(194, 40),
	(194, 41),
	(194, 44),
	(194, 46),
	(194, 47),
	(194, 51),
	(194, 195),
	(194, 198),
	(194, 632),
	(194, 798),
	(195, 3),
	(195, 4),
	(195, 5),
	(195, 6),
	(195, 24),
	(195, 49),
	(195, 199),
	(196, 202),
	(196, 203),
	(196, 204),
	(196, 205),
	(196, 206),
	(196, 226),
	(196, 227),
	(196, 230),
	(196, 231),
	(196, 233),
	(196, 234),
	(196, 235),
	(196, 238),
	(196, 239),
	(196, 240),
	(196, 241),
	(196, 242),
	(196, 243),
	(196, 244),
	(196, 245),
	(196, 246),
	(196, 247),
	(196, 248),
	(196, 249),
	(196, 250),
	(196, 251),
	(196, 252),
	(196, 253),
	(196, 254),
	(196, 255),
	(196, 256),
	(196, 257),
	(196, 258),
	(196, 259),
	(196, 260),
	(196, 261),
	(196, 262),
	(196, 264),
	(196, 265),
	(196, 266),
	(196, 267),
	(196, 268),
	(196, 269),
	(196, 270),
	(196, 271),
	(196, 272),
	(196, 279),
	(196, 280),
	(196, 283),
	(196, 287),
	(196, 288),
	(196, 289),
	(196, 290),
	(196, 291),
	(196, 292),
	(196, 293),
	(196, 294),
	(196, 295),
	(196, 296),
	(196, 297),
	(196, 298),
	(196, 299),
	(196, 302),
	(196, 303),
	(196, 304),
	(196, 305),
	(196, 306),
	(196, 307),
	(196, 308),
	(196, 309),
	(196, 310),
	(196, 313),
	(196, 314),
	(196, 319),
	(196, 320),
	(196, 321),
	(196, 322),
	(196, 323),
	(196, 324),
	(196, 325),
	(196, 326),
	(196, 327),
	(196, 328),
	(196, 329),
	(196, 330),
	(196, 331),
	(196, 332),
	(196, 333),
	(196, 334),
	(196, 335),
	(196, 336),
	(196, 337),
	(196, 338),
	(196, 339),
	(196, 340),
	(196, 341),
	(196, 342),
	(196, 343),
	(196, 344),
	(196, 345),
	(196, 346),
	(196, 347),
	(196, 348),
	(196, 349),
	(196, 350),
	(196, 351),
	(196, 352),
	(196, 353),
	(196, 354),
	(196, 355),
	(196, 356),
	(196, 357),
	(196, 358),
	(196, 359),
	(196, 360),
	(196, 361),
	(196, 362),
	(196, 363),
	(196, 364),
	(196, 365),
	(196, 366),
	(196, 373),
	(196, 375),
	(196, 400),
	(196, 401),
	(196, 402),
	(196, 403),
	(196, 404),
	(196, 405),
	(196, 406),
	(196, 407),
	(196, 417),
	(196, 418),
	(196, 419),
	(196, 420),
	(196, 421),
	(196, 422),
	(196, 423),
	(196, 424),
	(196, 425),
	(196, 426),
	(196, 427),
	(196, 428),
	(196, 429),
	(196, 434),
	(196, 435),
	(196, 436),
	(196, 437),
	(196, 438),
	(196, 439),
	(196, 440),
	(196, 441),
	(196, 442),
	(196, 443),
	(196, 444),
	(196, 445),
	(196, 446),
	(196, 447),
	(196, 448),
	(196, 449),
	(196, 450),
	(196, 451),
	(196, 452),
	(196, 453),
	(196, 454),
	(196, 455),
	(196, 456),
	(196, 457),
	(196, 458),
	(196, 459),
	(196, 461),
	(196, 463),
	(196, 464),
	(196, 465),
	(196, 472),
	(196, 474),
	(196, 475),
	(196, 476),
	(196, 477),
	(196, 478),
	(196, 488),
	(196, 489),
	(196, 491),
	(196, 492),
	(196, 493),
	(196, 495),
	(196, 497),
	(196, 498),
	(196, 499),
	(196, 500),
	(196, 502),
	(196, 503),
	(196, 505),
	(196, 508),
	(196, 511),
	(196, 513),
	(196, 514),
	(196, 516),
	(196, 519),
	(196, 522),
	(196, 523),
	(196, 526),
	(196, 527),
	(196, 529),
	(196, 530),
	(196, 533),
	(196, 535),
	(196, 536),
	(196, 537),
	(196, 538),
	(196, 539),
	(196, 540),
	(196, 541),
	(196, 556),
	(196, 581),
	(196, 582),
	(196, 592),
	(196, 593),
	(196, 596),
	(196, 602),
	(196, 603),
	(196, 604),
	(196, 605),
	(196, 606),
	(196, 607),
	(196, 608),
	(196, 609),
	(196, 610),
	(196, 611),
	(196, 612),
	(196, 613),
	(196, 614),
	(196, 615),
	(196, 616),
	(196, 617),
	(196, 618),
	(196, 619),
	(196, 620),
	(196, 621),
	(196, 622),
	(196, 623),
	(196, 624),
	(196, 625),
	(196, 626),
	(196, 627),
	(196, 628),
	(196, 629),
	(196, 630),
	(196, 631),
	(196, 633),
	(196, 634),
	(196, 635),
	(196, 636),
	(196, 637),
	(196, 638),
	(196, 639),
	(196, 640),
	(196, 641),
	(196, 642),
	(196, 643),
	(196, 644),
	(196, 645),
	(196, 646),
	(196, 647),
	(196, 648),
	(196, 649),
	(196, 650),
	(196, 651),
	(196, 652),
	(196, 653),
	(196, 654),
	(196, 655),
	(196, 656),
	(196, 657),
	(196, 658),
	(196, 659),
	(196, 660),
	(196, 661),
	(196, 662),
	(196, 663),
	(196, 664),
	(196, 665),
	(196, 666),
	(196, 667),
	(196, 668),
	(196, 669),
	(196, 670),
	(196, 671),
	(196, 672),
	(196, 673),
	(196, 674),
	(196, 675),
	(196, 676),
	(196, 677),
	(196, 678),
	(196, 679),
	(196, 680),
	(196, 681),
	(196, 682),
	(196, 683),
	(196, 684),
	(196, 685),
	(196, 686),
	(196, 687),
	(196, 688),
	(196, 689),
	(196, 690),
	(196, 691),
	(196, 692),
	(196, 693),
	(196, 694),
	(196, 695),
	(196, 696),
	(196, 697),
	(196, 698),
	(196, 699),
	(196, 701),
	(196, 702),
	(196, 703),
	(196, 704),
	(196, 706),
	(196, 707),
	(196, 708),
	(196, 709),
	(196, 710),
	(196, 711),
	(196, 712),
	(196, 713),
	(196, 714),
	(196, 715),
	(196, 716),
	(196, 717),
	(196, 718),
	(196, 719),
	(196, 721),
	(196, 722),
	(196, 723),
	(196, 724),
	(196, 725),
	(196, 726),
	(196, 727),
	(196, 728),
	(196, 729),
	(196, 730),
	(196, 733),
	(196, 734),
	(196, 735),
	(196, 736),
	(196, 738),
	(196, 739),
	(196, 748),
	(196, 753),
	(196, 757),
	(196, 773),
	(196, 777),
	(196, 836),
	(196, 837),
	(196, 838),
	(196, 839),
	(196, 840),
	(196, 841),
	(196, 843),
	(196, 852),
	(196, 866),
	(196, 867),
	(196, 870),
	(196, 871),
	(196, 872),
	(196, 873),
	(196, 875),
	(196, 876),
	(196, 877),
	(196, 878),
	(196, 879),
	(196, 881),
	(196, 1009),
	(196, 1010),
	(196, 1011),
	(196, 1012),
	(196, 1013),
	(196, 1014),
	(196, 1015),
	(196, 1016),
	(196, 1037),
	(196, 1038),
	(196, 1039),
	(196, 1040),
	(196, 1041),
	(197, 232),
	(197, 236),
	(197, 237),
	(197, 273),
	(197, 274),
	(197, 275),
	(197, 276),
	(197, 277),
	(197, 284),
	(197, 285),
	(197, 286),
	(197, 301),
	(197, 311),
	(197, 387),
	(197, 388),
	(197, 389),
	(197, 390),
	(197, 391),
	(197, 392),
	(197, 393),
	(197, 394),
	(197, 395),
	(197, 396),
	(197, 397),
	(197, 398),
	(197, 399),
	(197, 473),
	(197, 479),
	(197, 480),
	(197, 481),
	(197, 482),
	(197, 485),
	(197, 486),
	(197, 487),
	(197, 494),
	(197, 501),
	(197, 506),
	(197, 509),
	(197, 510),
	(197, 517),
	(197, 518),
	(197, 521),
	(197, 542),
	(197, 543),
	(197, 550),
	(197, 558),
	(197, 568),
	(197, 571),
	(197, 572),
	(197, 573),
	(197, 574),
	(197, 575),
	(197, 576),
	(197, 577),
	(197, 578),
	(197, 579),
	(197, 580),
	(197, 583),
	(197, 584),
	(197, 585),
	(197, 586),
	(197, 587),
	(197, 588),
	(197, 589),
	(197, 590),
	(197, 591),
	(197, 594),
	(197, 595),
	(197, 601),
	(197, 743),
	(197, 750),
	(197, 758),
	(197, 761),
	(197, 762),
	(197, 763),
	(197, 764),
	(197, 765),
	(197, 766),
	(197, 767),
	(197, 768),
	(197, 769),
	(197, 770),
	(197, 771),
	(197, 772),
	(197, 774),
	(197, 856),
	(197, 857),
	(197, 858),
	(197, 859),
	(197, 860),
	(197, 861),
	(197, 862),
	(197, 863),
	(197, 864),
	(197, 865),
	(197, 884),
	(197, 885),
	(198, 218),
	(198, 300),
	(198, 312),
	(198, 315),
	(198, 316),
	(198, 317),
	(198, 318),
	(198, 367),
	(198, 368),
	(198, 369),
	(198, 370),
	(198, 371),
	(198, 372),
	(198, 374),
	(198, 376),
	(198, 377),
	(198, 408),
	(198, 409),
	(198, 410),
	(198, 411),
	(198, 412),
	(198, 413),
	(198, 414),
	(198, 415),
	(198, 416),
	(198, 430),
	(198, 431),
	(198, 432),
	(198, 433),
	(198, 462),
	(198, 466),
	(198, 467),
	(198, 468),
	(198, 469),
	(198, 470),
	(198, 471),
	(198, 483),
	(198, 484),
	(198, 490),
	(198, 504),
	(198, 512),
	(198, 515),
	(198, 520),
	(198, 524),
	(198, 528),
	(198, 531),
	(198, 532),
	(198, 544),
	(198, 545),
	(198, 546),
	(198, 547),
	(198, 548),
	(198, 549),
	(198, 551),
	(198, 552),
	(198, 553),
	(198, 554),
	(198, 555),
	(198, 557),
	(198, 559),
	(198, 560),
	(198, 561),
	(198, 562),
	(198, 563),
	(198, 564),
	(198, 565),
	(198, 566),
	(198, 567),
	(198, 569),
	(198, 570),
	(198, 597),
	(198, 598),
	(198, 599),
	(198, 600),
	(198, 737),
	(198, 740),
	(198, 741),
	(198, 742),
	(198, 744),
	(198, 745),
	(198, 746),
	(198, 747),
	(198, 749),
	(198, 751),
	(198, 752),
	(198, 754),
	(198, 755),
	(198, 756),
	(198, 759),
	(198, 760),
	(198, 855),
	(198, 1000),
	(198, 1001),
	(198, 1002),
	(198, 1003),
	(198, 1004),
	(198, 1005),
	(198, 1006),
	(198, 1007),
	(198, 1008),
	(198, 1017),
	(198, 1018),
	(198, 1019),
	(198, 1020),
	(198, 1021),
	(198, 1022),
	(198, 1023),
	(198, 1024),
	(198, 1025),
	(198, 1026),
	(198, 1027),
	(198, 1028),
	(198, 1029),
	(198, 1030),
	(198, 1031),
	(198, 1032),
	(198, 1033),
	(198, 1034),
	(198, 1035),
	(198, 1036),
	(199, 217),
	(199, 221),
	(199, 222),
	(199, 223),
	(199, 225),
	(199, 263),
	(199, 378),
	(199, 379),
	(199, 380),
	(199, 496),
	(199, 507),
	(199, 525),
	(199, 534),
	(199, 797);

-- ----------------------------
-- Table structure for rbac_permissions
-- ----------------------------
DROP TABLE IF EXISTS `rbac_permissions`;
CREATE TABLE `rbac_permissions`  (
  `id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Permission id',
  `name` varchar(100) NOT NULL COMMENT 'Permission name',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Permission List' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of rbac_permissions
-- ----------------------------
REPLACE INTO `rbac_permissions` (`id`, `name`) VALUES
	(1,'Instant logout'),
	(2,'Skip Queue'),
	(3,'Join Normal Battleground'),
	(4,'Join Random Battleground'),
	(5,'Join Arenas'),
	(6,'Join Dungeon Finder'),
	(7,'Skip idle connection check'),
	(8,'Cannot earn achievements'),
	(9,'Cannot earn realm first achievements'),
	(11,'Log GM trades'),
	(13,'Skip Instance required bosses check'),
	(14,'Skip character creation team mask check'),
	(15,'Skip character creation class mask check'),
	(16,'Skip character creation race mask check'),
	(17,'Skip character creation reserved name check'),
	(18,'Skip character creation death knight min level check'),
	(19,'Skip needed requirements to use channel check'),
	(20,'Skip disable map check'),
	(21,'Skip reset talents when used more than allowed check'),
	(22,'Skip spam chat check'),
	(23,'Skip over-speed ping check'),
	(24,'Two side faction characters on the same account'),
	(25,'Allow say chat between factions'),
	(26,'Allow channel chat between factions'),
	(27,'Two side mail interaction'),
	(28,'See two side who list'),
	(29,'Add friends of other faction'),
	(30,'Save character without delay with .save command'),
	(31,'Use params with .unstuck command'),
	(32,'Can be assigned tickets with .assign ticket command'),
	(33,'Notify if a command was not found'),
	(34,'Check if should appear in list using .gm ingame command'),
	(35,'See all security levels with who command'),
	(36,'Filter whispers'),
	(37,'Use staff badge in chat'),
	(38,'Resurrect with full Health Points'),
	(39,'Restore saved gm setting states'),
	(40,'Allows to add a gm to friend list'),
	(41,'Use Config option START_GM_LEVEL to assign new character level'),
	(42,'Allows to use CMSG_WORLD_TELEPORT opcode'),
	(43,'Allows to use CMSG_WHOIS opcode'),
	(44,'Receive global GM messages/texts'),
	(45,'Join channels without announce'),
	(46,'Change channel settings without being channel moderator'),
	(47,'Can ignore non-strong lower security checks if it\'s disabled in config'),
	(48,'Enable IP, Last Login and EMail output in pinfo'),
	(49,'Forces to enter the email for confirmation on password change'),
	(50,'Allow user to check his own email with .account'),
	(51,'Allow trading between factions'),
	(52,'No battleground deserter debuff'),
	(53,'Can be AFK on the battleground'),
	(192,'Role: Sec Level Administrator'),
	(193,'Role: Sec Level Gamemaster'),
	(194,'Role: Sec Level Moderator'),
	(195,'Role: Sec Level Player'),
	(196,'Role: Administrator Commands'),
	(197,'Role: Gamemaster Commands'),
	(198,'Role: Moderator Commands'),
	(199,'Role: Player Commands'),
	(200,'Command: rbac'),
	(201,'Command: rbac account'),
	(202,'Command: rbac account list'),
	(203,'Command: rbac account grant'),
	(204,'Command: rbac account deny'),
	(205,'Command: rbac account revoke'),
	(206,'Command: rbac list'),
	(217,'Command: account'),
	(218,'Command: account addon'),
	(219,'Command: account create'),
	(220,'Command: account delete'),
	(221,'Command: account lock'),
	(222,'Command: account lock country'),
	(223,'Command: account lock ip'),
	(224,'Command: account onlinelist'),
	(225,'Command: account password'),
	(226,'Command: account set'),
	(227,'Command: account set addon'),
	(228,'Command: account set gmlevel'),
	(229,'Command: account set password'),
	(230,'Command: achievement'),
	(231,'Command: achievement add'),
	(232,'Command: arena'),
	(233,'Command: arena captain'),
	(234,'Command: arena create'),
	(235,'Command: arena disband'),
	(236,'Command: arena info'),
	(237,'Command: arena lookup'),
	(238,'Command: arena rename'),
	(239,'Command: ban'),
	(240,'Command: ban account'),
	(241,'Command: ban character'),
	(242,'Command: ban ip'),
	(243,'Command: ban playeraccount'),
	(244,'Command: baninfo'),
	(245,'Command: baninfo account'),
	(246,'Command: baninfo character'),
	(247,'Command: baninfo ip'),
	(248,'Command: banlist'),
	(249,'Command: banlist account'),
	(250,'Command: banlist character'),
	(251,'Command: banlist ip'),
	(252,'Command: unban'),
	(253,'Command: unban account'),
	(254,'Command: unban character'),
	(255,'Command: unban ip'),
	(256,'Command: unban playeraccount'),
	(257,'Command: bf'),
	(258,'Command: bf start'),
	(259,'Command: bf stop'),
	(260,'Command: bf switch'),
	(261,'Command: bf timer'),
	(262,'Command: bf enable'),
	(263,'Command: account email'),
	(264,'Command: account set sec'),
	(265,'Command: account set sec email'),
	(266,'Command: account set sec regmail'),
	(267,'Command: cast'),
	(268,'Command: cast back'),
	(269,'Command: cast dist'),
	(270,'Command: cast self'),
	(271,'Command: cast target'),
	(272,'Command: cast dest'),
	(273,'Command: character'),
	(274,'Command: character customize'),
	(275,'Command: character changefaction'),
	(276,'Command: character changerace'),
	(277,'Command: character deleted'),
	(279,'Command: character deleted list'),
	(280,'Command: character deleted restore'),
	(283,'Command: character level'),
	(284,'Command: character rename'),
	(285,'Command: character reputation'),
	(286,'Command: character titles'),
	(287,'Command: levelup'),
	(288,'Command: pdump'),
	(289,'Command: pdump load'),
	(290,'Command: pdump write'),
	(291,'Command: cheat'),
	(292,'Command: cheat casttime'),
	(293,'Command: cheat cooldown'),
	(294,'Command: cheat explore'),
	(295,'Command: cheat god'),
	(296,'Command: cheat power'),
	(297,'Command: cheat status'),
	(298,'Command: cheat taxi'),
	(299,'Command: cheat waterwalk'),
	(300,'Command: debug'),
	(301,'Command: debug anim'),
	(302,'Command: debug areatriggers'),
	(303,'Command: debug arena'),
	(304,'Command: debug bg'),
	(305,'Command: debug entervehicle'),
	(306,'Command: debug getitemstate'),
	(307,'Command: debug getitemvalue'),
	(308,'Command: debug getvalue'),
	(309,'Command: debug combat'),
	(310,'Command: debug itemexpire'),
	(311,'Command: debug lootrecipient'),
	(312,'Command: debug los'),
	(313,'Command: debug mod32value'),
	(314,'Command: debug moveflags'),
	(315,'Command: debug play'),
	(316,'Command: debug play cinematics'),
	(317,'Command: debug play movie'),
	(318,'Command: debug play sound'),
	(319,'Command: debug send'),
	(320,'Command: debug send buyerror'),
	(321,'Command: debug send channelnotify'),
	(322,'Command: debug send chatmessage'),
	(323,'Command: debug send equiperror'),
	(324,'Command: debug send largepacket'),
	(325,'Command: debug send opcode'),
	(326,'Command: debug send qinvalidmsg'),
	(327,'Command: debug send qpartymsg'),
	(328,'Command: debug send sellerror'),
	(329,'Command: debug send setphaseshift'),
	(330,'Command: debug send spellfail'),
	(331,'Command: debug setaurastate'),
	(332,'Command: debug setbit'),
	(333,'Command: debug setitemvalue'),
	(334,'Command: debug setvalue'),
	(335,'Command: debug setvid'),
	(336,'Command: debug spawnvehicle'),
	(337,'Command: debug threat'),
	(338,'Command: debug update'),
	(339,'Command: debug worldstate'),
	(340,'Command: wpgps'),
	(341,'Command: deserter'),
	(342,'Command: deserter bg'),
	(343,'Command: deserter bg add'),
	(344,'Command: deserter bg remove'),
	(345,'Command: deserter instance'),
	(346,'Command: deserter instance add'),
	(347,'Command: deserter instance remove'),
	(348,'Command: disable'),
	(349,'Command: disable add'),
	(350,'Command: disable add achievement_criteria'),
	(351,'Command: disable add battleground'),
	(352,'Command: disable add map'),
	(353,'Command: disable add mmap'),
	(354,'Command: disable add outdoorpvp'),
	(355,'Command: disable add quest'),
	(356,'Command: disable add spell'),
	(357,'Command: disable add vmap'),
	(358,'Command: disable remove'),
	(359,'Command: disable remove achievement_criteria'),
	(360,'Command: disable remove battleground'),
	(361,'Command: disable remove map'),
	(362,'Command: disable remove mmap'),
	(363,'Command: disable remove outdoorpvp'),
	(364,'Command: disable remove quest'),
	(365,'Command: disable remove spell'),
	(366,'Command: disable remove vmap'),
	(367,'Command: event'),
	(368,'Command: event activelist'),
	(369,'Command: event start'),
	(370,'Command: event stop'),
	(371,'Command: gm'),
	(372,'Command: gm chat'),
	(373,'Command: gm fly'),
	(374,'Command: gm ingame'),
	(375,'Command: gm list'),
	(376,'Command: gm visible'),
	(377,'Command: go'),
	(378,'Command: account 2fa'),
	(379,'Command: account 2fa setup'),
	(380,'Command: account 2fa remove'),
	(381,'Command: account set 2fa'),
	(387,'Command: gobject'),
	(388,'Command: gobject activate'),
	(389,'Command: gobject add'),
	(390,'Command: gobject add temp'),
	(391,'Command: gobject delete'),
	(392,'Command: gobject info'),
	(393,'Command: gobject move'),
	(394,'Command: gobject near'),
	(395,'Command: gobject set'),
	(396,'Command: gobject set phase'),
	(397,'Command: gobject set state'),
	(398,'Command: gobject target'),
	(399,'Command: gobject turn'),
	(400,'Command: debug transport'),
	(401,'Command: guild'),
	(402,'Command: guild create'),
	(403,'Command: guild delete'),
	(404,'Command: guild invite'),
	(405,'Command: guild uninvite'),
	(406,'Command: guild rank'),
	(407,'Command: guild rename'),
	(408,'Command: honor'),
	(409,'Command: honor add'),
	(410,'Command: honor add kill'),
	(411,'Command: honor update'),
	(412,'Command: instance'),
	(413,'Command: instance listbinds'),
	(414,'Command: instance unbind'),
	(415,'Command: instance stats'),
	(416,'Command: instance savedata'),
	(417,'Command: learn'),
	(418,'Command: learn all'),
	(419,'Command: learn all my'),
	(420,'Command: learn all my class'),
	(421,'Command: learn all my pettalents'),
	(422,'Command: learn all my spells'),
	(423,'Command: learn all my talents'),
	(424,'Command: learn all gm'),
	(425,'Command: learn all crafts'),
	(426,'Command: learn all default'),
	(427,'Command: learn all lang'),
	(428,'Command: learn all recipes'),
	(429,'Command: unlearn'),
	(430,'Command: lfg'),
	(431,'Command: lfg player'),
	(432,'Command: lfg group'),
	(433,'Command: lfg queue'),
	(434,'Command: lfg clean'),
	(435,'Command: lfg options'),
	(436,'Command: list'),
	(437,'Command: list creature'),
	(438,'Command: list item'),
	(439,'Command: list object'),
	(440,'Command: list auras'),
	(441,'Command: list mail'),
	(442,'Command: lookup'),
	(443,'Command: lookup area'),
	(444,'Command: lookup creature'),
	(445,'Command: lookup event'),
	(446,'Command: lookup faction'),
	(447,'Command: lookup item'),
	(448,'Command: lookup itemset'),
	(449,'Command: lookup object'),
	(450,'Command: lookup quest'),
	(451,'Command: lookup player'),
	(452,'Command: lookup player ip'),
	(453,'Command: lookup player account'),
	(454,'Command: lookup player email'),
	(455,'Command: lookup skill'),
	(456,'Command: lookup spell'),
	(457,'Command: lookup spell id'),
	(458,'Command: lookup taxinode'),
	(459,'Command: lookup tele'),
	(460,'Command: lookup title'),
	(461,'Command: lookup map'),
	(462,'Command: announce'),
	(463,'Command: channel'),
	(464,'Command: channel set'),
	(465,'Command: channel set ownership'),
	(466,'Command: gmannounce'),
	(467,'Command: gmnameannounce'),
	(468,'Command: gmnotify'),
	(469,'Command: nameannounce'),
	(470,'Command: notify'),
	(471,'Command: whispers'),
	(472,'Command: group'),
	(473,'Command: group leader'),
	(474,'Command: group disband'),
	(475,'Command: group remove'),
	(476,'Command: group join'),
	(477,'Command: group list'),
	(478,'Command: group summon'),
	(479,'Command: pet'),
	(480,'Command: pet create'),
	(481,'Command: pet learn'),
	(482,'Command: pet unlearn'),
	(483,'Command: send'),
	(484,'Command: send items'),
	(485,'Command: send mail'),
	(486,'Command: send message'),
	(487,'Command: send money'),
	(488,'Command: additem'),
	(489,'Command: additemset'),
	(490,'Command: appear'),
	(491,'Command: aura'),
	(492,'Command: bank'),
	(493,'Command: bindsight'),
	(494,'Command: combatstop'),
	(495,'Command: cometome'),
	(496,'Command: commands'),
	(497,'Command: cooldown'),
	(498,'Command: damage'),
	(499,'Command: dev'),
	(500,'Command: die'),
	(501,'Command: dismount'),
	(502,'Command: distance'),
	(503,'Command: flusharenapoints'),
	(504,'Command: freeze'),
	(505,'Command: gps'),
	(506,'Command: guid'),
	(507,'Command: help'),
	(508,'Command: hidearea'),
	(509,'Command: itemmove'),
	(510,'Command: kick'),
	(511,'Command: linkgrave'),
	(512,'Command: listfreeze'),
	(513,'Command: maxskill'),
	(514,'Command: movegens'),
	(515,'Command: mute'),
	(516,'Command: neargrave'),
	(517,'Command: pinfo'),
	(518,'Command: playall'),
	(519,'Command: possess'),
	(520,'Command: recall'),
	(521,'Command: repairitems'),
	(522,'Command: respawn'),
	(523,'Command: revive'),
	(524,'Command: saveall'),
	(525,'Command: save'),
	(526,'Command: setskill'),
	(527,'Command: showarea'),
	(528,'Command: summon'),
	(529,'Command: unaura'),
	(530,'Command: unbindsight'),
	(531,'Command: unfreeze'),
	(532,'Command: unmute'),
	(533,'Command: unpossess'),
	(534,'Command: unstuck'),
	(535,'Command: wchange'),
	(536,'Command: mmap'),
	(537,'Command: mmap loadedtiles'),
	(538,'Command: mmap loc'),
	(539,'Command: mmap path'),
	(540,'Command: mmap stats'),
	(541,'Command: mmap testarea'),
	(542,'Command: morph'),
	(543,'Command: demorph'),
	(544,'Command: modify'),
	(545,'Command: modify arenapoints'),
	(546,'Command: modify bit'),
	(547,'Command: modify drunk'),
	(548,'Command: modify energy'),
	(549,'Command: modify faction'),
	(550,'Command: modify gender'),
	(551,'Command: modify honor'),
	(552,'Command: modify hp'),
	(553,'Command: modify mana'),
	(554,'Command: modify money'),
	(555,'Command: modify mount'),
	(556,'Command: modify phase'),
	(557,'Command: modify rage'),
	(558,'Command: modify reputation'),
	(559,'Command: modify runicpower'),
	(560,'Command: modify scale'),
	(561,'Command: modify speed'),
	(562,'Command: modify speed all'),
	(563,'Command: modify speed backwalk'),
	(564,'Command: modify speed fly'),
	(565,'Command: modify speed walk'),
	(566,'Command: modify speed swim'),
	(567,'Command: modify spell'),
	(568,'Command: modify standstate'),
	(569,'Command: modify talentpoints'),
	(570,'Command: npc'),
	(571,'Command: npc add'),
	(572,'Command: npc add formation'),
	(573,'Command: npc add item'),
	(574,'Command: npc add move'),
	(575,'Command: npc add temp'),
	(576,'Command: npc add delete'),
	(577,'Command: npc add delete item'),
	(578,'Command: npc add follow'),
	(579,'Command: npc add follow stop'),
	(580,'Command: npc set'),
	(581,'Command: npc set allowmove'),
	(582,'Command: npc set entry'),
	(583,'Command: npc set factionid'),
	(584,'Command: npc set flag'),
	(585,'Command: npc set level'),
	(586,'Command: npc set link'),
	(587,'Command: npc set model'),
	(588,'Command: npc set movetype'),
	(589,'Command: npc set phase'),
	(590,'Command: npc set spawndist'),
	(591,'Command: npc set spawntime'),
	(592,'Command: npc set data'),
	(593,'Command: npc info'),
	(594,'Command: npc near'),
	(595,'Command: npc move'),
	(596,'Command: npc playemote'),
	(597,'Command: npc say'),
	(598,'Command: npc textemote'),
	(599,'Command: npc whisper'),
	(600,'Command: npc yell'),
	(601,'Command: npc tame'),
	(602,'Command: quest'),
	(603,'Command: quest add'),
	(604,'Command: quest complete'),
	(605,'Command: quest remove'),
	(606,'Command: quest reward'),
	(607,'Command: reload'),
	(608,'Command: reload access_requirement'),
	(609,'Command: reload achievement_criteria_data'),
	(610,'Command: reload achievement_reward'),
	(611,'Command: reload all'),
	(612,'Command: reload all achievement'),
	(613,'Command: reload all area'),
	(614,'Command: broadcast_text'),
	(615,'Command: reload all gossips'),
	(616,'Command: reload all item'),
	(617,'Command: reload all locales'),
	(618,'Command: reload all loot'),
	(619,'Command: reload all npc'),
	(620,'Command: reload all quest'),
	(621,'Command: reload all scripts'),
	(622,'Command: reload all spell'),
	(623,'Command: reload areatrigger_involvedrelation'),
	(624,'Command: reload areatrigger_tavern'),
	(625,'Command: reload areatrigger_teleport'),
	(626,'Command: reload auctions'),
	(627,'Command: reload autobroadcast'),
	(628,'Command: reload command'),
	(629,'Command: reload conditions'),
	(630,'Command: reload config'),
	(631,'Command: reload battleground_template'),
	(632,'Command: .mutehistory'),
	(633,'Command: reload creature_linked_respawn'),
	(634,'Command: reload creature_loot_template'),
	(635,'Command: reload creature_onkill_reputation'),
	(636,'Command: reload creature_questender'),
	(637,'Command: reload creature_queststarter'),
	(638,'Command: reload creature_summon_groups'),
	(639,'Command: reload creature_template'),
	(640,'Command: reload creature_text'),
	(641,'Command: reload disables'),
	(642,'Command: reload disenchant_loot_template'),
	(643,'Command: reload event_scripts'),
	(644,'Command: reload fishing_loot_template'),
	(645,'Command: reload graveyard_zone'),
	(646,'Command: reload game_tele'),
	(647,'Command: reload gameobject_questender'),
	(648,'Command: reload gameobject_loot_template'),
	(649,'Command: reload gameobject_queststarter'),
	(650,'Command: reload gm_tickets'),
	(651,'Command: reload gossip_menu'),
	(652,'Command: reload gossip_menu_option'),
	(653,'Command: reload item_enchantment_template'),
	(654,'Command: reload item_loot_template'),
	(655,'Command: reload item_set_names'),
	(656,'Command: reload lfg_dungeon_rewards'),
	(657,'Command: reload locales_achievement_reward'),
	(658,'Command: reload locales_creature'),
	(659,'Command: reload locales_creature_text'),
	(660,'Command: reload locales_gameobject'),
	(661,'Command: reload locales_gossip_menu_option'),
	(662,'Command: reload locales_item'),
	(663,'Command: reload locales_item_set_name'),
	(664,'Command: reload locales_npc_text'),
	(665,'Command: reload locales_page_text'),
	(666,'Command: reload locales_points_of_interest'),
	(667,'Command: reload locales_quest'),
	(668,'Command: reload mail_level_reward'),
	(669,'Command: reload mail_loot_template'),
	(670,'Command: reload milling_loot_template'),
	(671,'Command: reload npc_spellclick_spells'),
	(672,'Command: reload trainer'),
	(673,'Command: reload npc_vendor'),
	(674,'Command: reload page_text'),
	(675,'Command: reload pickpocketing_loot_template'),
	(676,'Command: reload points_of_interest'),
	(677,'Command: reload prospecting_loot_template'),
	(678,'Command: reload quest_poi'),
	(679,'Command: reload quest_template'),
	(680,'Command: reload rbac'),
	(681,'Command: reload reference_loot_template'),
	(682,'Command: reload reserved_name'),
	(683,'Command: reload reputation_reward_rate'),
	(684,'Command: reload reputation_spillover_template'),
	(685,'Command: reload skill_discovery_template'),
	(686,'Command: reload skill_extra_item_template'),
	(687,'Command: reload skill_fishing_base_level'),
	(688,'Command: reload skinning_loot_template'),
	(689,'Command: reload smart_scripts'),
	(690,'Command: reload spell_required'),
	(691,'Command: reload spell_area'),
	(692,'Command: reload spell_bonus_data'),
	(693,'Command: reload spell_group'),
	(694,'Command: reload spell_learn_spell'),
	(695,'Command: reload spell_loot_template'),
	(696,'Command: reload spell_linked_spell'),
	(697,'Command: reload spell_pet_auras'),
	(698,'Command: character changeaccount'),
	(699,'Command: reload spell_proc'),
	(701,'Command: reload spell_target_position'),
	(702,'Command: reload spell_threats'),
	(703,'Command: reload spell_group_stack_rules'),
	(704,'Command: reload trinity_string'),
	(706,'Command: reload waypoint_scripts'),
	(707,'Command: reload waypoint_data'),
	(708,'Command: reload vehicle_accessory'),
	(709,'Command: reload vehicle_template_accessory'),
	(710,'Command: reset'),
	(711,'Command: reset achievements'),
	(712,'Command: reset honor'),
	(713,'Command: reset level'),
	(714,'Command: reset spells'),
	(715,'Command: reset stats'),
	(716,'Command: reset talents'),
	(717,'Command: reset all'),
	(718,'Command: server'),
	(719,'Command: server corpses'),
	(720,'Command: server exit'),
	(721,'Command: server idlerestart'),
	(722,'Command: server idlerestart cancel'),
	(723,'Command: server idleshutdown'),
	(724,'Command: server idleshutdown cancel'),
	(725,'Command: server info'),
	(726,'Command: server plimit'),
	(727,'Command: server restart'),
	(728,'Command: server restart cancel'),
	(729,'Command: server set'),
	(730,'Command: server set closed'),
	(731,'Command: server set difftime'),
	(732,'Command: server set loglevel'),
	(733,'Command: server set motd'),
	(734,'Command: server shutdown'),
	(735,'Command: server shutdown cancel'),
	(736,'Command: server motd'),
	(737,'Command: tele'),
	(738,'Command: tele add'),
	(739,'Command: tele del'),
	(740,'Command: tele name'),
	(741,'Command: tele group'),
	(742,'Command: ticket'),
	(743,'Command: ticket assign'),
	(744,'Command: ticket close'),
	(745,'Command: ticket closedlist'),
	(746,'Command: ticket comment'),
	(747,'Command: ticket complete'),
	(748,'Command: ticket delete'),
	(749,'Command: ticket escalate'),
	(750,'Command: ticket escalatedlist'),
	(751,'Command: ticket list'),
	(752,'Command: ticket onlinelist'),
	(753,'Command: ticket reset'),
	(754,'Command: ticket response'),
	(755,'Command: ticket response append'),
	(756,'Command: ticket response appendln'),
	(757,'Command: ticket togglesystem'),
	(758,'Command: ticket unassign'),
	(759,'Command: ticket viewid'),
	(760,'Command: ticket viewname'),
	(761,'Command: titles'),
	(762,'Command: titles add'),
	(763,'Command: titles current'),
	(764,'Command: titles remove'),
	(765,'Command: titles set'),
	(766,'Command: titles set mask'),
	(767,'Command: wp'),
	(768,'Command: wp add'),
	(769,'Command: wp event'),
	(770,'Command: wp load'),
	(771,'Command: wp modify'),
	(772,'Command: wp unload'),
	(773,'Command: wp reload'),
	(774,'Command: wp show'),
	(777,'Command: mailbox'),
	(778,'Command: ahbot'),
	(779,'Command: ahbot items'),
	(780,'Command: ahbot items gray'),
	(781,'Command: ahbot items white'),
	(782,'Command: ahbot items green'),
	(783,'Command: ahbot items blue'),
	(784,'Command: ahbot items purple'),
	(785,'Command: ahbot items orange'),
	(786,'Command: ahbot items yellow'),
	(787,'Command: ahbot ratio'),
	(788,'Command: ahbot ratio alliance'),
	(789,'Command: ahbot ratio horde'),
	(790,'Command: ahbot ratio neutral'),
	(791,'Command: ahbot rebuild'),
	(792,'Command: ahbot reload'),
	(793,'Command: ahbot status'),
	(794,'Command: guild info'),
	(795,'Command: instance setbossstate'),
	(796,'Command: instance getbossstate'),
	(797,'Command: pvpstats'),
	(798,'Command: mod xp'),
	(835,'Command: debug loadcells'),
	(836,'Command: debug boundary'),
	(837,'Command: npc evade'),
	(838,'Command: pet level'),
	(839,'Command: server shutdown force'),
	(840,'Command: server restart force'),
	(841,'Command: debug neargraveyard'),
	(843,'Command: reload quest_greeting'),
	(852,'Command: debug dummy'),
	(855,'Command: debug play music'),
	(856,'Command: npc spawngroup'),
	(857,'Command: npc despawngroup'),
	(858,'Command: gobject spawngroup'),
	(859,'Command: gobject despawngroup'),
	(860,'Command: list respawns'),
	(861,'Command: group set'),
	(862,'Command: group set assistant'),
	(863,'Command: group set maintank'),
	(864,'Command: group set mainassist'),
	(865,'Command: npc showloot'),
	(866,'Command: list spawnpoints'),
	(867,'Command: reload quest_greeting_locale'),
	(870,'Command: debug threatinfo'),
	(871,'Command: debug instancespawn'),
	(872,'Command: server debug'),
	(873,'Command: reload creature_movement_override'),
	(874,'Command: debug asan'),
	(875,'Command: lookup map id'),
	(876,'Command: lookup item id'),
	(877,'Command: lookup quest id'),
	(878,'Command: debug questreset'),
	(879,'Command: debug poolstatus'),
	(880,'Command: pdump copy'),
	(881,'Command: reload vehicle_template'),
	(884,'Command: bg start'),
	(885,'Command: bg stop'),
	(1000,'Command: go creature'),
	(1001,'Command: go graveyard'),
	(1002,'Command: go grid'),
	(1003,'Command: go object'),
	(1004,'Command: go taxinode'),
	(1005,'Command: go ticket'),
	(1006,'Command: go trigger'),
	(1007,'Command: go xyz'),
	(1008,'Command: go zonexy'),
	(1009,'Command: reload all eventai'),
	(1010,'Command: reload creature_ai_scripts'),
	(1011,'Command: reload creature_ai_texts'),
	(1012,'Command: reload game_graveyard_zone'),
	(1013,'Command: reload npc_trainer'),
	(1014,'Command: reload spell_proc_event'),
	(1015,'Command: reload spell_scripts'),
	(1016,'Command: reload warden_action'),
	(1017,'Command: .ticket bug'),
	(1018,'Command: .ticket bug assign'),
	(1019,'Command: .ticket bug close'),
	(1020,'Command: .ticket bug closedlist'),
	(1021,'Command: .ticket bug comment'),
	(1022,'Command: .ticket bug delete'),
	(1023,'Command: .ticket bug list'),
	(1024,'Command: .ticket bug unassign'),
	(1025,'Command: .ticket bug view'),
	(1026,'Command: .ticket bug reset'),
	(1027,'Command: .ticket suggest'),
	(1028,'Command: .ticket suggest assign'),
	(1029,'Command: .ticket suggest close'),
	(1030,'Command: .ticket suggest closedlist'),
	(1031,'Command: .ticket suggest comment'),
	(1032,'Command: .ticket suggest delete'),
	(1033,'Command: .ticket suggest list'),
	(1034,'Command: .ticket suggest unassign'),
	(1035,'Command: .ticket suggest view'),
	(1036,'Command: .ticket suggest reset'),
	(1037,'Command: .reload quest_objective'),
	(1038,'Command: .reload quest_objective_effects'),
	(1039,'Command: .reload locales_quest_objective'),
	(1040,'Command: .reload bmauctions'),
	(1041,'Command: .reload blackmarket_template'),
	(1042,'Command: .account boost'),
	(1043,'Command: .account boost add'),
	(1044,'Command: .account boost delete');

-- ----------------------------
-- View structure for vw_rbac
-- ----------------------------
DROP VIEW IF EXISTS `vw_rbac`;
CREATE ALGORITHM=UNDEFINED SQL SECURITY INVOKER VIEW `vw_rbac` AS
select `t1`.`linkedId` AS `Permission ID`,`t1`.`id` AS `Permission Group`,ifnull(`t2`.`secId`,'linked') AS `Security Level`,`t3`.`name` AS `Permission`
from ((`rbac_linked_permissions` `t1` left join `rbac_default_permissions` `t2` on((`t1`.`id` = `t2`.`permissionId`))) left join `rbac_permissions` `t3` on((`t1`.`linkedId` = `t3`.`id`)));

-- ----------------------------
-- Table structure for realm_classes
-- ----------------------------
DROP TABLE IF EXISTS `realm_classes`;
CREATE TABLE `realm_classes`  (
  `realmId` int NOT NULL,
  `class` tinyint NOT NULL COMMENT 'Class Id',
  `expansion` tinyint NOT NULL COMMENT 'Expansion for class activation',
  PRIMARY KEY (`realmId`, `class`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of realm_classes
-- ----------------------------
INSERT INTO `realm_classes` VALUES (1, 1, 0);
INSERT INTO `realm_classes` VALUES (1, 2, 0);
INSERT INTO `realm_classes` VALUES (1, 3, 0);
INSERT INTO `realm_classes` VALUES (1, 4, 0);
INSERT INTO `realm_classes` VALUES (1, 5, 0);
INSERT INTO `realm_classes` VALUES (1, 6, 2);
INSERT INTO `realm_classes` VALUES (1, 7, 0);
INSERT INTO `realm_classes` VALUES (1, 8, 0);
INSERT INTO `realm_classes` VALUES (1, 9, 0);
INSERT INTO `realm_classes` VALUES (1, 10, 4);
INSERT INTO `realm_classes` VALUES (1, 11, 0);

-- ----------------------------
-- Table structure for realm_diff_stats
-- ----------------------------
DROP TABLE IF EXISTS `realm_diff_stats`;
CREATE TABLE `realm_diff_stats`  (
  `realm_id` tinyint UNSIGNED NOT NULL,
  `diff` mediumint UNSIGNED NULL DEFAULT NULL,
  `min` mediumint UNSIGNED NULL DEFAULT NULL,
  `max` mediumint UNSIGNED NULL DEFAULT NULL,
  `unixtime` int NOT NULL,
  PRIMARY KEY (`realm_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of realm_diff_stats
-- ----------------------------
INSERT INTO `realm_diff_stats` VALUES (1, 25, 14, 129, 1636139221);

-- ----------------------------
-- Table structure for realm_races
-- ----------------------------
DROP TABLE IF EXISTS `realm_races`;
CREATE TABLE `realm_races`  (
  `realmId` int NOT NULL,
  `race` tinyint NOT NULL COMMENT 'Race Id',
  `expansion` tinyint NOT NULL COMMENT 'Expansion for race activation',
  PRIMARY KEY (`realmId`, `race`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of realm_races
-- ----------------------------
INSERT INTO `realm_races` VALUES (1, 1, 0);
INSERT INTO `realm_races` VALUES (1, 2, 0);
INSERT INTO `realm_races` VALUES (1, 3, 0);
INSERT INTO `realm_races` VALUES (1, 4, 0);
INSERT INTO `realm_races` VALUES (1, 5, 0);
INSERT INTO `realm_races` VALUES (1, 6, 0);
INSERT INTO `realm_races` VALUES (1, 7, 0);
INSERT INTO `realm_races` VALUES (1, 8, 0);
INSERT INTO `realm_races` VALUES (1, 9, 3);
INSERT INTO `realm_races` VALUES (1, 10, 1);
INSERT INTO `realm_races` VALUES (1, 11, 1);
INSERT INTO `realm_races` VALUES (1, 22, 3);
INSERT INTO `realm_races` VALUES (1, 24, 4);
INSERT INTO `realm_races` VALUES (1, 25, 4);
INSERT INTO `realm_races` VALUES (1, 26, 4);

-- ----------------------------
-- Table structure for realmcharacters
-- ----------------------------
DROP TABLE IF EXISTS `realmcharacters`;
CREATE TABLE `realmcharacters`  (
  `realmid` int UNSIGNED NOT NULL DEFAULT 0,
  `acctid` int UNSIGNED NOT NULL,
  `numchars` tinyint UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`realmid`, `acctid`) USING BTREE,
  INDEX `acctid`(`acctid` ASC) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'How many characters accounts have' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of realmcharacters
-- ----------------------------
INSERT INTO `realmcharacters` VALUES (1, 1, 4);
INSERT INTO `realmcharacters` VALUES (1, 2, 1);
INSERT INTO `realmcharacters` VALUES (2, 1, 0);
INSERT INTO `realmcharacters` VALUES (2, 2, 0);

-- ----------------------------
-- Table structure for realmlist
-- ----------------------------
DROP TABLE IF EXISTS `realmlist`;
CREATE TABLE `realmlist` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `address` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `port` smallint NOT NULL DEFAULT '8085',
  `localAddress` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `localSubnetMask` varchar(255) NOT NULL DEFAULT '255.255.255.0',
  `icon` tinyint unsigned NOT NULL DEFAULT '0',
  `flag` tinyint NOT NULL,
  `timezone` tinyint unsigned NOT NULL DEFAULT '0',
  `allowedSecurityLevel` tinyint unsigned NOT NULL DEFAULT '0',
  `population` float unsigned NOT NULL DEFAULT '0',
  `gamebuild` int unsigned NOT NULL DEFAULT '12340',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT=DYNAMIC COMMENT='Realm System';

-- ----------------------------
-- Records of realmlist
-- ----------------------------
INSERT INTO `realmlist` (`id`, `name`, `address`, `port`, `localAddress`, `localSubnetMask`, `icon`, `flag`, `timezone`, `allowedSecurityLevel`, `population`, `gamebuild`) VALUES 
(1, 'MoP', '25.81.18.30', 8085, '127.0.0.1', '255.255.255.0', 0, 0, 14, 0, 0, 18414);


-- ----------------------------
-- Table structure for build_info
-- ----------------------------
DROP TABLE IF EXISTS `build_info`;
CREATE TABLE `build_info`  (
  `build` int NOT NULL,
  `majorVersion` int NULL DEFAULT NULL,
  `minorVersion` int NULL DEFAULT NULL,
  `bugfixVersion` int NULL DEFAULT NULL,
  `hotfixVersion` varchar(3) NULL DEFAULT NULL,
  `winAuthSeed` varchar(32) NULL DEFAULT NULL,
  `win64AuthSeed` varchar(32) NULL DEFAULT NULL,
  `mac64AuthSeed` varchar(32) NULL DEFAULT NULL,
  `winChecksumSeed` varchar(40) NULL DEFAULT NULL,
  `macChecksumSeed` varchar(40) NULL DEFAULT NULL,
  PRIMARY KEY (`build`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for build_info
-- ----------------------------
INSERT INTO `build_info` VALUES 
(5875, 1, 12, 1, NULL, NULL, NULL, NULL, '95EDB27C7823B363CBDDAB56A392E7CB73FCCA20', '8D173CC381961EEBABF336F5E6675B101BB513E5'),
(6005, 1, 12, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(6141, 1, 12, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(8606, 2, 4, 3, NULL, NULL, NULL, NULL, '319AFAA3F2559682F9FF658BE01456255F456FB1', 'D8B0ECFE534BC1131E19BAD1D4C0E813EEE4994F'),
(9947, 3, 1, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(10505, 3, 2, 2, 'a', NULL, NULL, NULL, NULL, NULL),
(11159, 3, 3, 0, 'a', NULL, NULL, NULL, NULL, NULL),
(11403, 3, 3, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(11723, 3, 3, 3, 'a', NULL, NULL, NULL, NULL, NULL),
(12340, 3, 3, 5, 'a', NULL, NULL, NULL, 'CDCBBD5188315E6B4D19449D492DBCFAF156A347', 'B706D13FF2F4018839729461E3F8A0E2B5FDC034'),
(13623, 4, 0, 6, 'a', NULL, NULL, NULL, NULL, NULL),
(13930, 3, 3, 5, 'a', NULL, NULL, NULL, NULL, NULL),
(14545, 4, 2, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(15595, 4, 3, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(18414, 5, 4, 8, NULL, NULL, NULL, NULL, NULL, NULL),
(19116, 6, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(19243, 6, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(19342, 6, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(19702, 6, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(19802, 6, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(19831, 6, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(19865, 6, 1, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(20182, 6, 2, 0, 'a', NULL, NULL, NULL, NULL, NULL),
(20201, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20216, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20253, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20338, 6, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(20444, 6, 2, 2, NULL, NULL, NULL, NULL, NULL, NULL),
(20490, 6, 2, 2, 'a', NULL, NULL, NULL, NULL, NULL),
(20574, 6, 2, 2, 'a', NULL, NULL, NULL, NULL, NULL),
(20726, 6, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(20779, 6, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(20886, 6, 2, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(21355, 6, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(21463, 6, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(21742, 6, 2, 4, NULL, NULL, NULL, NULL, NULL, NULL),
(22248, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22293, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22345, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22410, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22423, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22498, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22522, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22566, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22594, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22624, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22747, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22810, 7, 0, 3, NULL, NULL, NULL, NULL, NULL, NULL),
(22900, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22908, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22950, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22995, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(22996, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23171, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23222, 7, 1, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23360, 7, 1, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(23420, 7, 1, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(23911, 7, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(23937, 7, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(24015, 7, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL),
(24330, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24367, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24415, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24430, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24461, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(24742, 7, 2, 5, NULL, NULL, NULL, NULL, NULL, NULL),
(25549, 7, 3, 2, NULL, 'FE594FC35E7F9AFF86D99D8A364AB297', '1252624ED8CBD6FAC7D33F5D67A535F3', '66FC5E09B8706126795F140308C8C1D8', NULL, NULL),
(25996, 7, 3, 5, NULL, '23C59C5963CBEF5B728D13A50878DFCB', 'C7FF932D6A2174A3D538CA7212136D2B', '210B970149D6F56CAC9BADF2AAC91E8E', NULL, NULL),
(26124, 7, 3, 5, NULL, 'F8C05AE372DECA1D6C81DA7A8D1C5C39', '46DF06D0147BA67BA49AF553435E093F', 'C9CA997AB8EDE1C65465CB2920869C4E', NULL, NULL),
(26365, 7, 3, 5, NULL, '2AAC82C80E829E2CA902D70CFA1A833A', '59A53F307288454B419B13E694DF503C', 'DBE7F860276D6B400AAA86B35D51A417', NULL, NULL),
(26654, 7, 3, 5, NULL, 'FAC2D693E702B9EC9F750F17245696D8', 'A752640E8B99FE5B57C1320BC492895A', '9234C1BD5E9687ADBD19F764F2E0E811', NULL, NULL),
(26822, 7, 3, 5, NULL, '283E8D77ECF7060BE6347BE4EB99C7C7', '2B05F6D746C0C6CC7EF79450B309E595', '91003668C245D14ECD8DF094E065E06B', NULL, NULL),
(26899, 7, 3, 5, NULL, 'F462CD2FE4EA3EADF875308FDBB18C99', '3551EF0028B51E92170559BD25644B03', '8368EFC2021329110A16339D298200D4', NULL, NULL),
(26972, 7, 3, 5, NULL, '797ECC19662DCBD5090A4481173F1D26', '6E212DEF6A0124A3D9AD07F5E322F7AE', '341CFEFE3D72ACA9A4407DC535DED66A', NULL, NULL),
(28153, 8, 0, 1, NULL, NULL, 'DD626517CC6D31932B479934CCDC0ABF', NULL, NULL, NULL),
(30706, 8, 1, 5, NULL, NULL, 'BB6D9866FE4A19A568015198783003FC', NULL, NULL, NULL),
(30993, 8, 2, 0, NULL, NULL, '2BAD61655ABC2FC3D04893B536403A91', NULL, NULL, NULL),
(31229, 8, 2, 0, NULL, NULL, '8A46F23670309F2AAE85C9A47276382B', NULL, NULL, NULL),
(31429, 8, 2, 0, NULL, NULL, '7795A507AF9DC3525EFF724FEE17E70C', NULL, NULL, NULL),
(31478, 8, 2, 0, NULL, NULL, '7973A8D54BDB8B798D9297B096E771EF', NULL, NULL, NULL),
(32305, 8, 2, 5, NULL, NULL, '21F5A6FC7AD89FBF411FDA8B8738186A', NULL, NULL, NULL),
(32494, 8, 2, 5, NULL, NULL, '58984ACE04919401835C61309A848F8A', NULL, NULL, NULL),
(32580, 8, 2, 5, NULL, NULL, '87C2FAA0D7931BF016299025C0DDCA14', NULL, NULL, NULL),
(32638, 8, 2, 5, NULL, NULL, '5D07ECE7D4A867DDDE615DAD22B76D4E', NULL, NULL, NULL),
(32722, 8, 2, 5, NULL, NULL, '1A09BE1D38A122586B4931BECCEAD4AA', NULL, NULL, NULL);

-- ----------------------------
-- Table structure for sql_update
-- ----------------------------
DROP TABLE IF EXISTS `sql_update`;
CREATE TABLE `sql_update`  (
  `file` varchar(50) NOT NULL,
  `realmid` tinyint NOT NULL DEFAULT -1,
  `date` datetime NULL DEFAULT NULL,
  `result` longtext NULL,
  PRIMARY KEY (`file`, `realmid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of sql_update
-- ----------------------------

-- ----------------------------
-- Table structure for uptime
-- ----------------------------
DROP TABLE IF EXISTS `uptime`;
CREATE TABLE `uptime`  (
  `realmid` int UNSIGNED NOT NULL,
  `starttime` int UNSIGNED NOT NULL DEFAULT 0,
  `uptime` int UNSIGNED NOT NULL DEFAULT 0,
  `maxplayers` smallint UNSIGNED NOT NULL DEFAULT 0,
  `revision` varchar(255) NOT NULL DEFAULT 'SkyFire',
  PRIMARY KEY (`realmid`, `starttime`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin COMMENT = 'Uptime system' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of uptime
-- ----------------------------
INSERT INTO `uptime` VALUES (1, 1635979141, 600, 0, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1635980112, 0, 0, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636049637, 601, 0, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636050723, 1200, 0, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636057343, 4202, 0, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636067032, 603, 1, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636067868, 3000, 1, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636071143, 600, 1, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636072221, 601, 1, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636073025, 1200, 1, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636074441, 3600, 1, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');
INSERT INTO `uptime` VALUES (1, 1636138018, 1203, 0, 'SkyFire 5.x.xRev: 0 Release Hash: Archive (Win64, little-endian)');

-- ----------------------------
-- Table structure for wow_token
-- ----------------------------
DROP TABLE IF EXISTS `wow_token`;
CREATE TABLE `wow_token`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `accountId` int UNSIGNED NOT NULL,
  `characterGuid` int NOT NULL DEFAULT 0,
  `realm` int UNSIGNED NOT NULL,
  `coins` int UNSIGNED NOT NULL,
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of wow_token
-- ----------------------------

-- ----------------------------
-- Table structure for updates
-- ----------------------------
DROP TABLE IF EXISTS `updates`;
CREATE TABLE IF NOT EXISTS `updates` (
  `name` varchar(200) NOT NULL COMMENT 'filename with extension of the update.',
  `hash` char(40) DEFAULT '' COMMENT 'sha1 hash of the sql file.',
  `state` enum('RELEASED','ARCHIVED','CUSTOM') NOT NULL DEFAULT 'RELEASED' COMMENT 'defines if an update is released or archived.',
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'timestamp when the query was applied.',
  `speed` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'time the query takes to apply in ms.',
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='List of all applied updates in this database.';

-- ----------------------------
-- Records of updates
-- ----------------------------
INSERT INTO `updates` (`name`, `hash`, `state`, `speed`) VALUES
('2021_03_27_BattlePay_DP.sql', '702ef317df44edb4837cf7d39d9e9311c6977d19', 'ARCHIVED', 0),
('2021_03_27_BattlePay_log.sql', '59da241c9b1db34b1263a684e4a623b0ee8193dc', 'ARCHIVED', 0),
('2021_03_27_WowTokens_log.sql', '1628733c3b7d08c9cccd42068d2a3f52f3f59c9f', 'ARCHIVED', 0),
('2021_05_15_Reinicio_de_Arenas.sql', 'da9cac057a9a81390fae1254b95346812dbd0356', 'ARCHIVED', 0),
('2021_07_17_00_autobroadcast.sql', '3ac1b3144101ef032da2a260c2c87e80073a05d6', 'ARCHIVED', 0),
('2021_08_08_Bonus_BG.sql', 'b247424b065bc9a76cfc0bc084ca837a3a1ff980', 'ARCHIVED', 0),
('2021_08_08_Config.sql', 'fd38520e205ba4b41bf25b80b2db208fb1ae29b6', 'ARCHIVED', 0),
('2023_11_03_auth.sql', '3e453104907e0da5fe9397f1387c5ceb20c3d1eb', 'ARCHIVED', 0),
('2023_11_09_auth.sql', '59eb7c9cda10821b385558efbb63b006b0a395c4', 'ARCHIVED', 0),
('2023_11_10_auth.sql', '0e24f41caa65e29b97b23b6426758aa0215ec853', 'ARCHIVED', 0),
('2023_11_26_auth.sql', '58913771f0fbe4f336f2d7ef005a09570385ff06', 'ARCHIVED', 0),
('2023_11_27_auth.sql', 'c0068d70e6c6b9624ff75d9b4ef79ebf00b844f7', 'ARCHIVED', 0),
('2023_12_04_auth.sql', '97736498bfe1e4b0ca1dc5efd412450646a00705', 'ARCHIVED', 0),
('fusion-cms.sql', '2e3dd26cd3257982cb89dd21588d3a4384e5807a', 'ARCHIVED', 0);

-- ----------------------------
-- Table structure for updates_include
-- ----------------------------
DROP TABLE IF EXISTS `updates_include`;
CREATE TABLE IF NOT EXISTS `updates_include` (
  `path` varchar(200) NOT NULL COMMENT 'directory to include. $ means relative to the source directory.',
  `state` enum('RELEASED','ARCHIVED') NOT NULL DEFAULT 'RELEASED' COMMENT 'defines if the directory contains released or archived updates.',
  PRIMARY KEY (`path`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='List of directories where we want to include sql updates.';

-- ----------------------------
-- Records of updates_include
-- ----------------------------
INSERT INTO `updates_include` (`path`, `state`) VALUES
('$/sql/updates/master/auth', 'RELEASED'),
('$/sql/archive/auth', 'ARCHIVED');

SET FOREIGN_KEY_CHECKS = 1;
