-- Quest 8487 (Corrupted Soil): Tainted Soil Sample (180921) is a chest whose data7 links the green glow, Tainted Soil Trap (181583).
-- Linked traps of chests are not summoned by the core, so the trap is a separate static spawn next to every sample and stayed
-- visible after the sample was looted. go_tainted_soil hides the trap together with the looted sample and shows it again when the
-- sample respawns. The trap respawn time is set to the sample's (120 s) so that both come back together.
UPDATE `gameobject_template` SET `ScriptName` = 'go_tainted_soil' WHERE `entry` = 180921 AND `ScriptName` = '';
UPDATE `gameobject` SET `spawntimesecs` = 120 WHERE `id` = 181583;
