-- Script/Quest: Update: Lazy Peon (25134)

UPDATE `creature_template` SET `npcflag`=0, `AIName`='', `ScriptName`='npc_lazy_peon' WHERE `entry`=10556;
UPDATE `creature_template_addon` SET `auras`="" WHERE `entry`=10556;
DELETE FROM `creature_questrelation` WHERE `id`=10556 AND `quest`=25134;

DELETE FROM `creature_text` WHERE `entry`=10556;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(10556, 0, 0, 'Just was resting eyes! Back to work now!', 12, 0, 100, 0, 0, 0, 'Lazy Peon Say 0'),
(10556, 1, 0, 'OK boss, I get back to tree-hitting.', 12, 0, 100, 0, 0, 0, 'Lazy Peon Say 1'),
(10556, 2, 0, "Ow! OK, I'll get back to work, $P!", 12, 0, 100, 0, 0, 0, 'Lazy Peon Say 2'),
(10556, 3, 0, 'Sleepy... so sleepy...', 12, 0, 100, 0, 0, 0, 'Lazy Peon Say 3');
