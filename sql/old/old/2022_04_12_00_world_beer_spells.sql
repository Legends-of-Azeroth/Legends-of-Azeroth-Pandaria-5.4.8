DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (42254, 42255, 42256, 42257, 42258, 42259, 42260, 42261, 42263, 42264, 43959, 43961, -49738, -50098, -49864, 49962);
UPDATE `spell_script_names` SET `ScriptName` = "spell_brewfest_botm_teach_language" WHERE `ScriptName` = "spell_item_teach_language";

DELETE FROM `spell_script_names` WHERE `spell_id` IN (42254, 42255, 42256, 42257, 42258, 42259, 42260, 42261, 42263, 42264, 43959, 43961, 51694, 49738, 49822, 50098, 49864, 49962);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(42254,"spell_brewfest_botm_weak_alcohol"),
(42255,"spell_brewfest_botm_weak_alcohol"),
(42256,"spell_brewfest_botm_weak_alcohol"),
(42257,"spell_brewfest_botm_weak_alcohol"),
(42258,"spell_brewfest_botm_weak_alcohol"),
(42259,"spell_brewfest_botm_weak_alcohol"),
(42260,"spell_brewfest_botm_weak_alcohol"),
(42261,"spell_brewfest_botm_weak_alcohol"),
(42263,"spell_brewfest_botm_weak_alcohol"),
(42264,"spell_brewfest_botm_weak_alcohol"),
(43959,"spell_brewfest_botm_weak_alcohol"),
(43961,"spell_brewfest_botm_weak_alcohol"),
(51694,"spell_brewfest_botm_empty_bottle_throw_resolve"),
(49738,"spell_brewfest_botm_internal_combustion"),
(49822,"spell_brewfest_botm_bloated"),
(50098,"spell_brewfest_botm_the_beast_within"),
(49864,"spell_brewfest_botm_gassy"),
(49962,"spell_brewfest_botm_jungle_madness");

UPDATE `creature_template` SET `scale` = 1, `AIName` = "SmartAI" WHERE `entry` = 27870;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 27870 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 27870*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(27870,0,0,0,11,0,100,0,0,0,0,0,80,27870*100,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wild Flower - On Spawn - Start Script"),

(27870*100,9,0,0,0,0,100,0,3000,3000,0,0,11,49801,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wild Flower - On Script - Cast 'BOTM - Flowery Brew - Grow'");

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 27908;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 27908 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (27908*100, 27908*100+01) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(27908,0,0,0,11,0,100,0,0,0,0,0,80,27908*100,0,0,0,0,0,1,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Spawn - Start Script"),
(27908,0,1,0,34,0,100,0,8,1,0,0,80,27908*100+01,0,0,0,0,0,1,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Reached Point 1 - Start Script"),

(27908*100,9,0,0,0,0,100,0,0,0,0,0,11,50008,0,0,0,0,0,1,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Script - Cast 'BOTM - Jungle Brew - Jungle Invisibility Aura'"),
(27908*100,9,1,0,0,0,100,0,0,0,0,0,66,0,0,0,0,0,0,23,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Script - Set Orientation Summoner"),
(27908*100,9,2,0,0,0,100,0,2000,2000,0,0,69,1,0,0,3,0,0,23,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Script - Move To Summoner"),

(27908*100+01,9,0,0,0,0,100,0,1000,1000,0,0,11,49965,0,0,0,0,0,23,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Script - Cast 'BOTM - Jungle Brew - Jungle Madness Scared'"),
(27908*100+01,9,1,0,0,0,100,0,3000,3000,0,0,11,50048,0,0,0,0,0,1,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Script - Cast 'BOTM - Jungle Brew - Jungle Gnome Vanish Aura'"),
(27908*100+01,9,2,0,0,0,100,0,1000,1000,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Angry Jungle Gnome - On Script - Despawn Self");
