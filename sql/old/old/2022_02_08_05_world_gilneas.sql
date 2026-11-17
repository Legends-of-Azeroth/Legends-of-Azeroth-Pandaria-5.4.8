UPDATE `creature_template` SET `unit_flags` = 0 WHERE `entry` IN (35872, 35839);
UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` IN (35840, 35872, 35873, 35869);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (35840, 35872, 35873, 35869) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(35840, 0, 0, 0, 1, 0, 100, 0, 5000, 5000, 60000, 60000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Gwen Armstead - OOC - Say Text Line 0"),

(35872, 0, 0, 0, 0, 0, 100, 0, 1000, 5000, 1000, 5000, 11, 11538, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, "Myriam Spellwaker - In Combat  - Cast Spell 'Frostbolt'"),
(35872, 0, 1, 0, 0, 0, 100, 0, 12000, 12000, 12000, 12000, 11, 81165, 1, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, "Myriam Spellwaker - In Combat - Cast Spell 'Blizzard'"),

(35873, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 0, 0, 11, 13236, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Celestine of the Harvest - OOC - Cast Spell 'Nature Channeling'"),

(35869, 0, 0, 0, 1, 0, 100, 0, 1000, 1000, 0, 0, 11, 44163, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Vitus Darkwalker - OOC - Cast Spell 'Summon Imp'"),
(35869, 0, 1, 0, 1, 0, 100, 0, 25000, 25000, 0, 0, 11, 45104, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Vitus Darkwalker - OOC - Cast Spell 'Shadow Channelling'");

DELETE FROM `creature_text` WHERE `entry` = 35840;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `text_range`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(35840, 0, 0, "", "We must stick together to survive. Move around in groups.", 0, 12, 0, 100, 1, 0, 0, "Gwen Armstead"),
(35840, 0, 1, "", "The plan is to keep moving south. We can't go back to the Merchant Square. It's not safe.", 0, 12, 0, 100, 1, 0, 0, "Gwen Armstead"),
(35840, 0, 2, "", "The king's men will protect us. We're almost there.", 0, 12, 0, 100, 1, 0, 0, "Gwen Armstead");

UPDATE `creature_template` SET `RegenHealth` = 0 WHERE `entry` IN (47091, 35912);
UPDATE `creature` SET `curhealth` = 950 WHERE `id` = 47091;
UPDATE `creature` SET `curhealth` = 58 WHERE `id` = 35912;
UPDATE `creature` SET `wander_distance` = 10, `movement_type` = 1, `walk_mode` = 2 WHERE `id` = 35118;
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (220052, 219748, 219745, 220051, 220049, 220046, 220206, 220210, 220203, 220299,
                                                                                  220053, 219753, 220211);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 35118 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(35118, 0, 0, 1, 2, 0, 100, 1, 0, 30, 0, 0, 11, 8599, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - At 30% HP - Cast Spell 'Enrage'"),
(35118, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - At 30% HP (Link) - Say Text Line 0"),
(35118, 0, 2, 0, 8, 0, 100, 1, 100, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 3, 0, 8, 0, 100, 1, 56641, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 4, 0, 8, 0, 100, 1, 2098, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 5, 0, 8, 0, 100, 1, 122, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 6, 0, 8, 0, 100, 1, 172, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 7, 0, 8, 0, 100, 1, 589, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 8, 0, 8, 0, 100, 1, 8921, 0, 0, 0, 33, 44175, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Spellhit - Give Quest Credit"),
(35118, 0, 9, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 35232, 35, 0, 0, 0, 0, 0, "Bloodfang Worgen - On Reset - Start Attack (Creature ID: 35232, 35 Yards)"),
(35118, 0, 10, 0, 1, 0, 100, 0, 10000, 10000, 10000, 10000, 49, 0, 0, 0, 0, 0, 0, 19, 35232, 35, 0, 0, 0, 0, 0, "Bloodfang Worgen - OOC (10000 - 10000) - Start Attack (Creature ID: 35232, 35 Yards)");

DELETE FROM `creature_sparring_template` WHERE `creature_id` IN (35839, 35872);
INSERT INTO `creature_sparring_template`(`creature_id`, `health_limit_pct`, `comment`) VALUES
(35839,85,"Sergeant Cleese"),
(35872,90,"Myriam Spellwaker");
