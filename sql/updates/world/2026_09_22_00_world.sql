-- ============================================================
-- Quest 24438 - Exodus (Ogre Ambusher cutscene).
--
-- Point the Ogre Ambusher (creature_template.entry = 38762) at the
-- C++ script npc_ogre_ambusher_exodusAI
-- (src/server/scripts/EasternKingdoms/zone_gilneas.cpp,
-- registered via RegisterCreatureAI in AddSC_gilneas).
--
-- The C++ AI drives the stand-in-place ranged rock-throwing attack:
--   * UpdateAI -> UpdateVictim + range check + AttackerStateUpdate
--
-- The ambush itself (summon at WP24, cleanup at WP25) is handled by
-- the stagecoach harness AI; the ogres are summoned at runtime.
--
-- Idempotent: safe to re-run on any environment.
-- ============================================================

-- Point the creature at the C++ script.
UPDATE `creature_template` SET `ScriptName` = 'npc_ogre_ambusher_exodusAI' WHERE `entry` = 38762;

-- Rename the summoned harness script to match the RegisterCreatureAI registration
-- (npc_stagecoach_harness -> npc_stagecoach_harnessAI).
UPDATE `creature_template` SET `ScriptName` = 'npc_stagecoach_harnessAI' WHERE `entry` = 43336;
