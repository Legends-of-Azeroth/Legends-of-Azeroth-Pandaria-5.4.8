-- DB/Quest: Fix: Loss Reduction (25179)

SET @NPC_INJURED_RAZOR_HILL_GRUNT                       := 39270;
SET @QUEST_LOSS_REDUCTION                               := 25179;
SET @SPELL_INJURED_RAZOR_HILL_GRUNT_CANCEL_FEIGN_DEATH  := 73705;
SET @SPELL_PERMANENT_FEIGN_DEATH                        := 75511;

UPDATE `creature_template` SET `faction_A`=29, `faction_H`=29, `npcflag`=16777218, `AIName`='SmartAI', `ScriptName`='' WHERE `entry`=@NPC_INJURED_RAZOR_HILL_GRUNT;
UPDATE `creature` SET `MovementType`=0 WHERE `id`=@NPC_INJURED_RAZOR_HILL_GRUNT;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=22 AND `SourceGroup`=3 AND `SourceEntry`=@NPC_INJURED_RAZOR_HILL_GRUNT;
INSERT INTO `conditions`(`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(22, 3, @NPC_INJURED_RAZOR_HILL_GRUNT, 0, 0, 9, 0, @QUEST_LOSS_REDUCTION, 0, 0, 0, 0, '', 'Start SAI only if player has quest 25179 taken');

DELETE FROM `creature_template_addon` WHERE `entry`=@NPC_INJURED_RAZOR_HILL_GRUNT;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 0, 0, 1, 0, '76143'); -- Low Health (Aura)

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=@NPC_INJURED_RAZOR_HILL_GRUNT;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(@NPC_INJURED_RAZOR_HILL_GRUNT, @SPELL_INJURED_RAZOR_HILL_GRUNT_CANCEL_FEIGN_DEATH, 1, 0);

DELETE FROM `creature_text` WHERE `entry`=@NPC_INJURED_RAZOR_HILL_GRUNT;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
-- While lying on the ground, they injured grunts say:
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 0, "I can't fight any longer...", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (ground)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 1, "Somebody... anybody...", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (ground)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 2, "I'll never see my wife again...", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (ground)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 3, "I'll never see my husband again...", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (ground)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 4, "I can't fight any longer...", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (ground)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 5, "H-help me", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (ground)'),
-- When you rescue one of them, they say:
(@NPC_INJURED_RAZOR_HILL_GRUNT, 1, 0, "Thank you, $R... I was close to death. May I stay by your side for a moment?", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (rescue)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 1, 1, "You've saved my life, $C. Protect me for a few more moments while I recover.", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (rescue)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 1, 2, "For the Warchief You lead, I will follow.", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (rescue)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 1, 3, "I was supposed to die in battle... ah well, another day. I will follow you until I recover.", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (rescue)'),
-- When they depart again for Razor Hill, they say:
(@NPC_INJURED_RAZOR_HILL_GRUNT, 2, 0, "Thank you again. I will return to Razor Hill now.", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (return to Razor Hill)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 2, 1, "I feel much better now. For the Horde", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (return to Razor Hill)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 2, 2, "It was an honor to watch you fight, $N.", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (return to Razor Hill)'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 2, 3, "You fight well. I will leave you now. Lok'tar ogar", 12, 0, 100, 0, 0, 0, 'Injured Razor Hill Grunt (return to Razor Hill)');

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (@NPC_INJURED_RAZOR_HILL_GRUNT, @NPC_INJURED_RAZOR_HILL_GRUNT*100);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 11, @SPELL_PERMANENT_FEIGN_DEATH, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Reset - Cast Permanent Feign Death'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 1, 0, 10, 0, 100, 0, 1, 5, 300000, 300000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - In Range - Say Random Text'),
(@NPC_INJURED_RAZOR_HILL_GRUNT, 0, 2, 0, 73, 0, 100, 1, @NPC_INJURED_RAZOR_HILL_GRUNT, 0, 0, 0, 80, @NPC_INJURED_RAZOR_HILL_GRUNT*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Spellclick - Start Script'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 28, @SPELL_PERMANENT_FEIGN_DEATH, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Remove Aura Permanent Feign Death'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'injured Razor Hill Grunt - On Script - Set npcflag 0'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 33, @NPC_INJURED_RAZOR_HILL_GRUNT, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Give Kill Credit'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 3, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Say Random Text'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 4, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 17, 333, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Set Emote STATE_READY1H'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 5, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Follow'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 6, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Say Random Text'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 7, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Start Run'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 8, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 46, 40, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Move 40 yard'),
(@NPC_INJURED_RAZOR_HILL_GRUNT*100, 9, 9, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 4000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Razor Hill Grunt - On Script - Force Despawn');
