UPDATE `creature_template` SET `AIName` = "", `ScriptName` = "npc_archmage_landalock" WHERE `entry` = 20735;
DELETE FROM `creature` WHERE `id` IN (37849, 37850, 37851, 37853, 37854, 37855, 37856, 37858, 37859, 37861, 37862, 37864, 31619);
UPDATE `creature_template` SET `unit_flags` = `unit_flags`|33555200, `InhabitType` = 4 WHERE `entry` IN (37849, 37850, 37851, 37853, 37854, 37855, 37856, 37858, 37859, 37861, 37862, 37864);
