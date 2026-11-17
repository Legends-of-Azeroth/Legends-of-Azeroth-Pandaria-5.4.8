DELETE FROM `gameobject_addon` WHERE `guid` IN (181397, 207807);
INSERT INTO `gameobject_addon` (`guid`, `invisibilityType`, `invisibilityValue`)
VALUES
    (181397, 18, 1000), -- orgrimmar
    (207807, 18, 1000); -- stormwind

DELETE FROM `spell_area` WHERE `area` IN (5428, 5429)  AND `spell` = 84481;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`)
VALUES
    (84481, 5429, 27203, 27123, 0, 0, 2, 1, 74, 11), -- orgrimmar
    (84481, 5428, 27203, 27123, 0, 0, 2, 1, 74, 11); -- stormwind

-- this is no longer needed with proper invisibility setup above
DELETE FROM `object_visibility_state` WHERE `entryorguid` = 205268;
