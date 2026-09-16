-- Reiniciando el registro de las arenas jugadas
UPDATE `arena_game_id` SET `game_id` = 1 WHERE `realm_id` = 1;
DELETE FROM `arena_games`;
