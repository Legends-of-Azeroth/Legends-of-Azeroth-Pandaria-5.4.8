-- Quest Twilight Shores (28238) Part 1
-- Add missing spawns Fargo Flintlocke (44806) Quest(28238)
SET @CGUID := 17;
DELETE FROM `creature` WHERE `guid` IN (@CGUID);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawntimesecs_max`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `movement_type`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `dynamicflags`, `ScriptName`, `walk_mode`) VALUES 
(@CGUID+0, 44806, 0, 1519, 4411, 1, 1, 0, 0, -8543.591796875, 1268.9930419921875, 4.626535415649414062, 5.323254108428955078, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0); -- Fargo Flintlocke (Area: Stormwind Harbor - Difficulty: 0) CreateObject1 (Auras: 93308 - Fargo Flintlocke Quest Invis B)

DELETE FROM `creature_addon` WHERE `guid` IN (@CGUID);
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `ai_anim_kit`, `movement_anim_kit`, `melee_anim_kit`, `auras`) VALUES 
(@CGUID+0, 0, 0, 0, 1, 0, 0, 0, 0, '93308'); -- Fargo Flintlocke

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=50262 AND `spell_id`=93320;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(50262, 93320, 3, 0);

-- Spell 100616 (Quest Invisibility Detection 27)
DELETE FROM `spell_area` WHERE `spell`=100616 AND `area`=4411 AND `quest_start`=28238;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(100616, 4411, 28238, 0, 0, 0, 2, 1, 64, 11);

-- Spellclick 
UPDATE `creature_template` SET `npcflag` = `npcflag` | 16777216 WHERE `entry`=50262;
