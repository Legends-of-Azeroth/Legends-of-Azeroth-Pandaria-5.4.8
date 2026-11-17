-- Temple of the Jade Serpent: Liu Flameheart (56732) has a static spawn (guid 539885) at z 243.6, about 64 yd above the courtyard (ground z ~180)
-- and outside the navmesh. The instance script summons the real Liu at LiuSpawnPosition after the courtyard trash is dead, so the static spawn is a
-- duplicate that can never be reached. Remove it.
DELETE FROM `creature` WHERE `guid` = 539885 AND `id` = 56732 AND `map` = 960 AND `position_z` > 240;
