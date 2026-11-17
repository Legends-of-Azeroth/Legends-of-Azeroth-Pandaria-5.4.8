-- Modify game_event table `start_time` and `end_time` default value
ALTER TABLE `game_event` 
MODIFY COLUMN `start_time` timestamp NOT NULL DEFAULT '2000-01-01 00:00:00'  COMMENT 'Absolute start date, the event will never start before' AFTER `eventEntry`,
MODIFY COLUMN `end_time` timestamp NOT NULL DEFAULT '2038-01-01 00:00:00'  COMMENT 'Absolute end date, the event will never start afler' AFTER `start_time`;
