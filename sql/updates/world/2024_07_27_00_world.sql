-- rename creature_addon aiAnimKit movementAnimKit meleeAnimKit
ALTER TABLE `creature_addon` 
CHANGE COLUMN `ai_anim_kit` `aiAnimKit` smallint NOT NULL DEFAULT 0 AFTER `emote`,
CHANGE COLUMN `movement_anim_kit` `movementAnimKit` smallint NOT NULL DEFAULT 0 AFTER `aiAnimKit`,
CHANGE COLUMN `melee_anim_kit` `meleeAnimKit` smallint NOT NULL DEFAULT 0 AFTER `movementAnimKit`;

-- rename creature_template_addon
ALTER TABLE `creature_template_addon` 
CHANGE COLUMN `ai_anim_kit` `aiAnimKit` smallint NOT NULL DEFAULT 0 AFTER `emote`,
CHANGE COLUMN `movement_anim_kit` `movementAnimKit` smallint NOT NULL DEFAULT 0 AFTER `aiAnimKit`,
CHANGE COLUMN `melee_anim_kit` `meleeAnimKit` smallint NOT NULL DEFAULT 0 AFTER `movementAnimKit`;
