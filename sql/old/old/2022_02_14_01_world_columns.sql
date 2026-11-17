ALTER TABLE `creature_addon`
  ADD `ai_anim_kit` SMALLINT NOT NULL DEFAULT 0 AFTER `emote`,
  ADD `movement_anim_kit` SMALLINT NOT NULL DEFAULT 0 AFTER `ai_anim_kit`,
  ADD `melee_anim_kit` SMALLINT NOT NULL DEFAULT 0 AFTER `movement_anim_kit`;
ALTER TABLE `creature_template_addon`
  ADD `ai_anim_kit` SMALLINT NOT NULL DEFAULT 0 AFTER `emote`,
  ADD `movement_anim_kit` SMALLINT NOT NULL DEFAULT 0 AFTER `ai_anim_kit`,
  ADD `melee_anim_kit` SMALLINT NOT NULL DEFAULT 0 AFTER `movement_anim_kit`;
