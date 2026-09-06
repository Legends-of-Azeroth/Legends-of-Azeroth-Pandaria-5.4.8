-- =====================================================================
-- Port to TC cata_classic pure basedmg creature damage model
--
-- The per-creature damage columns are removed from `creature_template`.
-- Base damage now comes from `creature_classlevelstats.damage_exp4`
-- (per level), with max = base * 1.5. Per-creature normal-difficulty
-- tuning is preserved via a DUNGEON_NORMAL row in
-- `creature_template_difficulty` (DamageModifier = old dmg_multiplier).
--
-- Part 1 (data) MUST run before Part 2 (schema DROPs) because it reads
-- the columns that Part 2 removes.
-- =====================================================================

-- ---------------------------------------------------------------------
-- A. Populate creature_classlevelstats.damage_exp4 (MoP) from the
--    per-level mindmg of exp=4 creatures. mindmg is uniform per level,
--    so MIN() is exact. This becomes the base damage for MoP creatures.
-- ---------------------------------------------------------------------
UPDATE creature_classlevelstats cls
JOIN (
    SELECT minlevel, MIN(mindmg) AS mindmg
    FROM creature_template
    WHERE exp = 4
    GROUP BY minlevel
) m ON cls.level = m.minlevel
SET cls.damage_exp4 = m.mindmg;

-- ---------------------------------------------------------------------
-- B. Migrate per-creature dmg_multiplier into DUNGEON_NORMAL rows of
--    creature_difficulty (DamageModifier). This preserves per-creature
--    normal-difficulty damage now that the column is removed. Idempotent.
-- ---------------------------------------------------------------------
INSERT INTO creature_difficulty (id, difficulty, health_mod, damage_mod, level_min, level_max)
SELECT ct.entry, 'DUNGEON_NORMAL', 1.0, ct.dmg_multiplier, ct.minlevel, ct.maxlevel
FROM creature_template ct
WHERE ct.dmg_multiplier <> 1.0
  AND NOT EXISTS (
      SELECT 1 FROM creature_difficulty d
      WHERE d.id = ct.entry AND d.difficulty = 'DUNGEON_NORMAL'
  );

-- =====================================================================
-- Part 2: schema alignment with TC cata_classic
-- =====================================================================

-- C. Rename table + column to TC naming.
RENAME TABLE creature_difficulty TO creature_template_difficulty;
ALTER TABLE creature_template_difficulty
    CHANGE damage_mod DamageModifier float DEFAULT 1;

-- D. Drop dead per-difficulty damage columns (never read; TC has none).
ALTER TABLE creature_template_difficulty
    DROP COLUMN attackpower,
    DROP COLUMN mindmg,
    DROP COLUMN maxdmg,
    DROP COLUMN rangedattackpower,
    DROP COLUMN minrangedmg,
    DROP COLUMN maxrangedmg;

-- E. Drop the per-creature damage columns from creature_template.
ALTER TABLE creature_template
    DROP COLUMN mindmg,
    DROP COLUMN maxdmg,
    DROP COLUMN attackpower,
    DROP COLUMN minrangedmg,
    DROP COLUMN maxrangedmg,
    DROP COLUMN rangedattackpower,
    DROP COLUMN dmg_multiplier;
