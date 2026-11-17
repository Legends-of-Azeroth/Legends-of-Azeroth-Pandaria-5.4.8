-- =====================================================================
-- Remove orphaned playerbots command rows from `command` table
--
-- The `npcbot` command is registered by mod_playerbots, which is built
-- with PLAYERBOTS=OFF. Leftover rows in the `command` table make the
-- worldserver log at startup:
--   Table `command` have not existed command 'npcbot', skip.
-- =====================================================================

DELETE FROM command WHERE name LIKE 'npcbot%' OR name LIKE 'pmon%';
