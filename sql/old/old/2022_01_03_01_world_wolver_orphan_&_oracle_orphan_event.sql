UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` IN (34520, 34519);
UPDATE `creature_template` SET `flags_extra` = `flags_extra`|536870912 WHERE `entry` = 34519;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (34520, 34519, -315778, -315779, -315780) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(-315778,0,0,1,1,0,100,0,15000,20000,15000,20000,11,65611,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Cast 'Throw Ragdoll'"),
(-315778,0,1,0,61,0,100,0,0,0,0,0,28,65617,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Remove Aura 'Hold Ragdoll State'"),
(-315778,0,2,4,8,0,100,0,65614,0,0,0,75,65617,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - On Spellhit (Throw Ragdoll) - Cast 'Hold Ragdoll State'"),
(-315778,0,3,0,1,0,100,0,5000,20000,15000,30000,10,53,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Random Emote"),
(-315778,0,4,0,61,0,100,0,0,0,0,0,45,1,1,0,0,0,0,19,34519,0,0,0,0,0,0,"Wolver Orphan - On Spellhit (Throw Ragdoll) - Set Data 1 1"),

(-315779,0,0,1,1,0,100,0,15000,20000,15000,20000,11,65613,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Cast 'Throw Ragdoll'"),
(-315779,0,1,0,61,0,100,0,0,0,0,0,28,65617,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Remove Aura 'Hold Ragdoll State'"),
(-315779,0,2,4,8,0,100,0,65611,0,0,0,75,65617,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - On Spellhit (Throw Ragdoll) - Cast 'Hold Ragdoll State'"),
(-315779,0,3,0,1,0,100,0,5000,20000,15000,30000,10,53,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Random Emote"),
(-315779,0,4,0,61,0,100,0,0,0,0,0,45,2,2,0,0,0,0,19,34519,0,0,0,0,0,0,"Wolver Orphan - On Spellhit (Throw Ragdoll) - Set Data 2 2"),

(-315780,0,0,1,1,0,100,0,15000,20000,15000,20000,11,65614,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Cast 'Throw Ragdoll'"),
(-315780,0,1,0,61,0,100,0,0,0,0,0,28,65617,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Remove Aura 'Hold Ragdoll State'"),
(-315780,0,2,4,8,0,100,0,65613,0,0,0,75,65617,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - On Spellhit (Throw Ragdoll) - Cast 'Hold Ragdoll State'"),
(-315780,0,3,0,1,0,100,0,5000,20000,15000,30000,10,53,0,0,0,0,0,1,0,0,0,0,0,0,0,"Wolver Orphan - OOC - Random Emote"),
(-315780,0,4,0,61,0,100,0,0,0,0,0,45,3,3,0,0,0,0,19,34519,0,0,0,0,0,0,"Wolver Orphan - On Spellhit (Throw Ragdoll) - Set Data 3 3"),

(34519,0,0,0,1,0,100,0,5000,20000,45000,90000,1,0,8000,0,0,0,0,19,34520,0,0,0,0,0,0,"Oracle Orphan - OOC - Say Text Line 0"),
(34519,0,1,0,52,0,100,0,0,34520,0,0,1,0,8000,0,0,0,0,1,0,0,0,0,0,0,0,"Oracle Orphan - On Text Over Line 0 (Wolvar) - Say Text Line 0"),
(34519,0,2,0,52,0,100,0,0,34519,0,0,1,1,7000,0,0,0,0,1,0,0,0,0,0,0,0,"Oracle Orphan - On Text Over Line 0 - Say Text Line 1"),
(34519,0,3,0,52,0,100,0,1,34519,0,0,1,2,7000,0,0,0,0,1,0,0,0,0,0,0,0,"Oracle Orphan - On Text Over Line 1 - Say Text Line 2"),
(34519,0,4,0,52,0,100,0,2,34519,0,0,1,0,7000,0,0,0,0,19,34365,0,0,0,0,0,0,"Oracle Orphan - On Text Over Line 2 - Say Text Line 0 (Matron)"),
(34519,0,5,0,38,0,100,0,1,1,0,0,69,1,0,0,1,0,0,10,315778,34520,0,0,0,0,0,"Oracle Orphan - On Data Set 1 1 - Move To Position"),
(34519,0,6,0,38,0,100,0,2,2,0,0,69,2,0,0,1,0,0,10,315779,34520,0,0,0,0,0,"Oracle Orphan - On Data Set 2 2 - Move To Position"),
(34519,0,7,0,38,0,100,0,3,3,0,0,69,3,0,0,1,0,0,10,315780,34520,0,0,0,0,0,"Oracle Orphan - On Data Set 3 3 - Move To Position");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` IN (13, 17) AND `SourceEntry` IN (65611, 65613, 65614);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 65611, 0, 0, 31, 0, 3, 34520, 315779, 0, 0, 0, "", "Wolvar Orphan"),
(13, 1, 65613, 0, 0, 31, 0, 3, 34520, 315780, 0, 0, 0, "", "Wolvar Orphan"),
(13, 1, 65614, 0, 0, 31, 0, 3, 34520, 315778, 0, 0, 0, "", "Wolvar Orphan");

DELETE FROM `creature_text` WHERE `entry` IN (34519, 34520, 34365);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`) VALUES 
(34519, 0, 0, "Give toy back!", "", 12, 0, 100, 1, 0, 0, 0, "Oracle Orphan"),
(34519, 1, 0, "Stop!", "", 12, 0, 100, 1, 0, 0, 0, "Oracle Orphan"),
(34519, 2, 0, "We tell orphan matron on you!", "", 12, 0, 100, 1, 0, 0, 0, "Oracle Orphan"),

(34520, 0, 0, "If you want, go get, big-tongue baby!", "", 12, 0, 100, 0, 0, 0, 0, "Wolvar Orphan"),

(34365, 0, 0, "", "Children, please behave yourselves! No one will want to adopt you if you keep acting like that!", 12, 0, 100, 5, 0, 0, 0, "Orphan Matron Aria");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` IN (34520, -315778, -315779, -315780);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, -315778, 0, 0, 1, 1, 65617, 0, 0, 0, 0, 0, "", "Wolver Orphan only run SAI if has aura 'Hold Ragdoll State'"),
(22, 1, -315779, 0, 0, 1, 1, 65617, 0, 0, 0, 0, 0, "", "Wolver Orphan only run SAI if has aura 'Hold Ragdoll State'"),
(22, 1, -315780, 0, 0, 1, 1, 65617, 0, 0, 0, 0, 0, "", "Wolver Orphan only run SAI if has aura 'Hold Ragdoll State'");

DELETE FROM `game_event_creature` WHERE `eventEntry` = 10 AND `guid` BETWEEN 315778 AND 315781;
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES 
(10, 315778),
(10, 315779),
(10, 315780),
(10, 315781);

DELETE FROM `disables` WHERE `entry` = 34519 AND `sourceType` = 10;
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES
(10, 34519, 1, "", "", "Oracle Orphan - MMAP");
