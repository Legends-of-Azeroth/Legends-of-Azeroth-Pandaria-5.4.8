-- fix timestamp default
ALTER TABLE game_event 
MODIFY COLUMN start_time timestamp NOT NULL COMMENT 'Absolute start date, the event will never start before' AFTER eventEntry,
MODIFY COLUMN end_time timestamp NOT NULL COMMENT 'Absolute end date, the event will never start afler' AFTER start_time,
MODIFY COLUMN ScriptName varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' AFTER world_event,
ENGINE = InnoDB, CHARACTER SET = utf8mb4, COLLATE = utf8mb4_bin;
