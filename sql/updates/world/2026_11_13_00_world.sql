-- Anachronos (15192) gossip moved from hardcoded C++ to database configuration.
-- creature_template.gossip_menu_id is already 6539. This fully defines both gossip
-- options for menu 6539 and gates each by quest state via conditions, matching the
-- previous C++ logic (show option while its quest is in progress).
--
--   OptionID 16  "Yes, Anachronos..."  -> shown while quest 29134 (A Wrinkle in Time) is in progress
--   OptionID 17  "Yes, Anachronos..." -> shown while quest 29193 (On a Wing and a Prayer) is in progress
--
-- Both options reuse the existing vanilla broadcast text 51305.
-- The OptionID values must match the enum AnachronosGossipOption in
-- src/server/scripts/World/tarecgosa_scripts.cpp.

-- Bind the Tarecgosa AI scripts to their creatures. Each RegisterCreatureAI(ai_name) in
-- src/server/scripts/World/tarecgosa_scripts.cpp registers a CreatureScript named after the
-- struct, bound to the creature via creature_template.ScriptName.
UPDATE `creature_template` SET `ScriptName` = 'npc_anachronos_15192' WHERE `entry` = 15192;
UPDATE `creature_template` SET `ScriptName` = 'npc_tarecgosa_52835' WHERE `entry` = 52835;
UPDATE `creature_template` SET `ScriptName` = 'npc_tarecgosa_53098' WHERE `entry` = 53098;
UPDATE `creature_template` SET `ScriptName` = 'npc_kalecgos_53149' WHERE `entry` = 53149;
UPDATE `creature_template` SET `ScriptName` = 'npc_thyrinar' WHERE `entry` = 53472;
UPDATE `creature_template` SET `ScriptName` = 'npc_kalecgos_53349' WHERE `entry` = 53349;
-- npc_hallegosa handles both the Horde (53371) and Alliance (53466) Hallegosa via GetMapId() == 1.
UPDATE `creature_template` SET `ScriptName` = 'npc_hallegosa' WHERE `entry` IN (53371, 53466);
UPDATE `creature_template` SET `ScriptName` = 'npc_kalecgos_53392' WHERE `entry` = 53392;
UPDATE `creature_template` SET `ScriptName` = 'npc_tarecgosa_53396' WHERE `entry` = 53396;
UPDATE `creature_template` SET `ScriptName` = 'npc_blue_dragon_53394' WHERE `entry` = 53394;
UPDATE `creature_template` SET `ScriptName` = 'npc_stalker_53422' WHERE `entry` = 53422;

-- Bind the Mini Mana Bomb toy AI script (RegisterCreatureAI(npc_toy_mini_maba_bomb) in
-- src/server/scripts/Toys/npc_toy.cpp).
UPDATE `creature_template` SET `ScriptName` = 'npc_toy_mini_maba_bomb' WHERE `entry` = 65903;

-- Bind the Fangs of Father scripts (registered in AddSC_fangs_of_father_scripts() in
-- src/server/scripts/World/fangs_of_father_scripts.cpp).
UPDATE `creature_template` SET `ScriptName` = 'npc_mostrasz_56314' WHERE `entry` = 56314;
UPDATE `creature_template` SET `ScriptName` = 'npc_wrathion_57777' WHERE `entry` = 57777;
UPDATE `creature_template` SET `ScriptName` = 'npc_lord_hiram_creed' WHERE `entry` = 57802;

-- Bind the pet scaling spell script (spell_gen_pet_calculate, a SpellScriptLoader registered in
-- AddSC_pet_spell_scripts() in src/server/scripts/Spells/spell_pet.cpp). It handles these
-- pet-scaling spell IDs via a switch in its AuraScript::Register().
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'spell_gen_pet_calculate' AND `spell_id` IN (19591, 35695, 61013, 61017, 61697);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(19591, 'spell_gen_pet_calculate'),
(35695, 'spell_gen_pet_calculate'),
(61013, 'spell_gen_pet_calculate'),
(61017, 'spell_gen_pet_calculate'),
(61697, 'spell_gen_pet_calculate');

-- Gossip options for menu 6539.
DELETE FROM `gossip_menu_option` WHERE `MenuID` = 6539 AND `OptionID` IN (16, 17);
INSERT INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionBroadcastTextID`, `OptionType`, `OptionNpcflag`, `ActionMenuID`, `ActionPoiID`, `BoxCoded`, `BoxMoney`, `BoxText`, `BoxBroadcastTextID`, `VerifiedBuild`)
VALUES
(6539, 16, 0, 'Yes, Anachronos. I am ready to witness your vision of the future.', 51305, 1, 0, 0, 0, 0, 0, NULL, 0, 0),
(6539, 17, 0, 'Yes, Anachronos. I am ready to witness your vision of the future.', 51305, 1, 0, 0, 0, 0, 0, NULL, 0, 0);

-- Gate each option by quest state (CONDITION_QUESTTAKEN = 9: true while quest is in progress).
-- SourceTypeOrReferenceId 15 = CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION, SourceGroup = MenuID, SourceEntry = OptionID.
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 6539 AND `SourceEntry` IN (16, 17);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`)
VALUES
(15, 6539, 16, 0, 0, 9, 0, 29134, 0, 0, 0, 0, 0, '', 'Anachronos (15192) gossip 6539 opt 16: show while quest 29134 (A Wrinkle in Time) is in progress'),
(15, 6539, 17, 0, 0, 9, 0, 29193, 0, 0, 0, 0, 0, '', 'Anachronos (15192) gossip 6539 opt 17: show while quest 29193 (On a Wing and a Prayer) is in progress');
