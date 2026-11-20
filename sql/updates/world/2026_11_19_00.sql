-- Wandering Island (Map 860) - Complete data update
-- Quest bindings, gossip menus, AI conversions, game objects

-- ============================================
-- creature_queststarter (missing quest starters)
-- ============================================
DELETE FROM `creature_queststarter` WHERE (`id`, `quest`) IN
((54786, 29408), (54568, 29414), (54734, 29421), (56159, 29776), (64543, 29786),
(56159, 29787), (56159, 29790), (56660, 29791), (54568, 29779), (54568, 29780),
(54568, 29781), (55212, 29679), (55213, 29679), (55558, 29774),
(55547, 29775), (55548, 29775), (53565, 29409), (60176, 29420), (60248, 29423),
(60253, 29423), (54631, 29523), (59496, 29775),
(57740, 29800), (54587, 29409), (65471, 29409), (54586, 29409), (60042, 30589),
(55940, 29798), (56416, 30767), (56013, 31450), (60878, 29799), (60896, 29799));

INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
-- Master Shang Xi (54786) - The Lesson of the Burning Scroll
(54786, 29408),
-- Ji Firepaw (54568) - The Way of the Tushui
(54568, 29414),
-- Master Li Fei (54734) - Only the Worthy Shall Pass
(54734, 29421),
-- Master Shang Xi Temple (56159) - Morning Breeze Village
(56159, 29776),
-- Aysa Wind Temple (64543) - Bataille Pyrotechnique
(64543, 29786),
-- Master Shang Xi After Zhao (56159) - Worthy of Passing
(56159, 29787),
-- Master Shang Xi Thousand Staff (56159) - Passing Wisdom
(56159, 29790),
-- Aisa Pre Balon Event (56660) - 29791
(56660, 29791),
-- Ji Firepaw (54568) - 29779, 29780, 29781
(54568, 29779),
(54568, 29780),
(54568, 29781),
-- Shu Pool of Reflection (55212/55213) - 29679
(55212, 29679),
(55213, 29679),
-- Water Spirit Dailo (55558) - 29774
(55558, 29774),
-- Wugou (55547/55548) - 29775
(55547, 29775),
(55548, 29775),
-- Aspiring Trainee (53565) - 29409
(53565, 29409),
-- Huojin Monk (60176) - 29420
(60176, 29420),
-- Chia-hui Autumnleaf (60248) - 29423
(60248, 29423),
-- Brewer Lin (60253) - 29423
(60253, 29423),
-- Living Air (54631) - 29523
(54631, 29523),
-- Delivery Cart Farmstead (59496) - 29775
(59496, 29775),
-- Delivery Cart Forbidden Forest (57740) - 29800
(57740, 29800),
-- Tushui Trainee (54587/65471) - 29409
(54587, 29409),
(65471, 29409),
-- Huojin Trainee (54586) - 29409
(54586, 29409),
-- Korga (60042) - Wrecking the Wreck
(60042, 30589),
-- Jojo Ironbrow (55940) - An Ancient Evil
(55940, 29798),
-- Aysa Gunship Crash (56416) - Risking It All
(56416, 30767),
-- Spirit of Master Shang Xi (56013) - A New Fate
(56013, 31450),
-- Shen Healers (60878/60896) - Healing Shen
(60878, 29799),
(60896, 29799);

-- ============================================
-- creature_questender (missing quest enders)
-- ============================================
DELETE FROM `creature_questender` WHERE (`id`, `quest`) IN
((54786, 29408), (54568, 29414), (54734, 29421), (56159, 29776), (64543, 29786),
(56274, 29786), (56159, 29787), (56159, 29790), (56660, 29791), (54568, 29779),
(54568, 29780), (54568, 29781), (55212, 29679), (55213, 29679),
(55558, 29774), (55547, 29775), (55548, 29775), (53565, 29409), (53565, 29522),
(53565, 29423), (60176, 29420), (60248, 29423), (60253, 29423), (54631, 29523),
(59496, 29775), (57740, 29800), (54587, 29409),
(65471, 29409), (54586, 29409), (60042, 30589), (55940, 29798), (56009, 29798),
(56416, 30767), (56013, 31450), (60878, 29799), (60896, 29799));

INSERT INTO `creature_questender` (`id`, `quest`) VALUES
-- Master Shang Xi (54786) - The Lesson of the Burning Scroll
(54786, 29408),
-- Ji Firepaw (54568) - The Way of the Tushui
(54568, 29414),
-- Master Li Fei (54734) - Only the Worthy Shall Pass
(54734, 29421),
-- Master Shang Xi Temple (56159) - Morning Breeze Village
(56159, 29776),
-- Aysa Wind Temple (64543) - Bataille Pyrotechnique
(64543, 29786),
-- Guardian of the Elders (56274) - Bataille Pyrotechnique
(56274, 29786),
-- Master Shang Xi After Zhao (56159) - Worthy of Passing
(56159, 29787),
-- Master Shang Xi Thousand Staff (56159) - Passing Wisdom
(56159, 29790),
-- Aisa Pre Balon Event (56660) - 29791
(56660, 29791),
-- Ji Firepaw (54568) - 29779, 29780, 29781
(54568, 29779),
(54568, 29780),
(54568, 29781),
-- Shu Pool of Reflection (55212/55213) - 29679
(55212, 29679),
(55213, 29679),
-- Water Spirit Dailo (55558) - 29774
(55558, 29774),
-- Wugou (55547/55548) - 29775
(55547, 29775),
(55548, 29775),
-- Aspiring Trainee (53565) - 29409, 29522, 29423
(53565, 29409),
(53565, 29522),
(53565, 29423),
-- Huojin Monk (60176) - 29420
(60176, 29420),
-- Chia-hui Autumnleaf (60248) - 29423
(60248, 29423),
-- Brewer Lin (60253) - 29423
(60253, 29423),
-- Living Air (54631) - 29523
(54631, 29523),
-- Delivery Cart Farmstead (59496) - 29775
(59496, 29775),
-- Delivery Cart Forbidden Forest (57740) - 29800
(57740, 29800),
-- Tushui Trainee (54587/65471) - 29409
(54587, 29409),
(65471, 29409),
-- Huojin Trainee (54586) - 29409
(54586, 29409),
-- Korga (60042) - Wrecking the Wreck
(60042, 30589),
-- Jojo Ironbrow (55940) - An Ancient Evil
(55940, 29798),
-- Vordraka (56009) - An Ancient Evil
(56009, 29798),
-- Aysa Gunship Crash (56416) - Risking It All
(56416, 30767),
-- Spirit of Master Shang Xi (56013) - A New Fate
(56013, 31450),
-- Shen Healers (60878/60896) - Healing Shen
(60878, 29799),
(60896, 29799);

-- ============================================
-- gossip_menu (fix wrong TextID pointing to Outland content)
-- ============================================
UPDATE `gossip_menu` SET `TextID` = 0 WHERE `MenuID` IN (13726, 13727, 13728);

-- ============================================
-- gossip_menu_option (gossip menus for Wandering Island)
-- ============================================
DELETE FROM `gossip_menu_option` WHERE `MenuID` IN (13726, 13727, 13728);
INSERT INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionBroadcastTextID`, `OptionType`, `OptionNpcflag`, `ActionMenuID`, `ActionPoiID`, `BoxCoded`, `BoxMoney`, `BoxText`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES
(13726, 0, 0, 'I\'m ready to decide.', 60279, 1, 0, 0, 0, 0, 0, NULL, 0, 0),
(13727, 0, 0, 'Shu, can you wake up Wuguo for me?', 54025, 1, 0, 0, 0, 0, 0, NULL, 0, 0);

-- ============================================
-- creature_template (AIName / gossip_menu_id updates)
-- ============================================
-- 60729 Aysa Cloudsinger: C++ EscortAI
UPDATE `creature_template` SET `AIName` = 'npc_aysa_gunship_crash_escortAI' WHERE `entry` = 60729;
-- 56416 Aysa Cloudsinger (quest giver): C++ ScriptedAI
UPDATE `creature_template` SET `AIName` = 'npc_aysa_gunship_crashAI' WHERE `entry` = 56416;
-- 60042 Korga Strongmane: C++ ScriptedAI
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_korgaAI' WHERE `entry` = 60042;
-- 55940 Jojo Ironbrow: C++ ScriptedAI
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_jojo_ironbrowAI' WHERE `entry` = 55940;
-- 56013 Spirit of Master Shang Xi: C++ ScriptedAI + gossip menu
UPDATE `creature_template` SET `AIName` = 'npc_shang_xi_choose_factionAI', `gossip_menu_id` = 13726 WHERE `entry` = 56013;

-- ============================================
-- smart_scripts (remove SAI replaced by C++)
-- ============================================
DELETE FROM `smart_scripts` WHERE `entryorguid` = 60729;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 60741;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 56416 AND `id` IN (12, 13);
DELETE FROM `smart_scripts` WHERE `entryorguid` = 56013;

-- ============================================
-- waypoints (Aysa Gunship Crash escort path)
-- ============================================
DELETE FROM `waypoints` WHERE `entry` = 60729 AND `pointid` IN (3, 4, 18);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES
(60729, 18, 235.042, 4007.54, 87.6689);

-- ============================================
-- spell_script_names (Wind Stone summoning Living Air)
-- ============================================
DELETE FROM `spell_script_names` WHERE `spell_id` = 106299;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (106299, 'spell_gen_summon_living_air');

-- ============================================
-- gameobject (missing instances on map 860)
-- ============================================
DELETE FROM `gameobject` WHERE `id` IN (209922, 209685) AND `map` = 860;
INSERT INTO `gameobject` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `state`) VALUES
(209922, 860, 845.89, 4372.62, 223.98, 0.00, 0.00, 0.00, 0.00, 0.00, 0),
(209685, 860, 647.49, 4224.63, 202.91, 0.00, 0.00, 0.00, 0.00, 0.00, 0);

-- ============================================
-- creature_text (Master Shang Xi Temple dialogue - NPC 64530)
-- ============================================
DELETE FROM `creature_text` WHERE `CreatureID` = 64530;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `SoundType`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(64530, 0, 0, '', 0, 0, 100, 0, 0, 0, 0, 60600, 0, 'TALK_1 - conquered every challenge'),
(64530, 0, 1, '', 0, 0, 100, 0, 0, 0, 0, 60601, 0, 'TALK_2 - larger problem Shen-zin Su'),
(64530, 0, 2, '', 0, 0, 100, 0, 0, 0, 0, 60602, 0, 'TALK_3 - four elemental spirits'),
(64530, 0, 3, '', 0, 0, 100, 0, 0, 0, 0, 60603, 0, 'TALK_4 - Ji to Dai-Lo Farmstead'),
(64530, 0, 4, '', 0, 0, 100, 0, 0, 0, 0, 60604, 0, 'TALK_5 - Aysa to Singing Pools'),
(64530, 0, 5, '', 0, 0, 100, 0, 0, 0, 0, 60605, 0, 'TALK_6 - hand that guides us all'),
(64530, 0, 6, '', 0, 0, 100, 0, 0, 0, 0, 55295, 0, 'TALK_7 - returned with spirits'),
(64530, 0, 7, '', 0, 0, 100, 0, 0, 0, 0, 55296, 0, 'TALK_8 - Wugou and Shu welcome'),
(64530, 0, 8, '', 0, 0, 100, 0, 0, 0, 0, 55297, 0, 'TALK_9 - Dafeng Dawning Span');
