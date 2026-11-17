-- Fix for quest Smoke Before Fire (31743) , update Imps Credit(66301) position, Imps Credit maybe a dymatic spawn. Thanks Terragor
-- This quest need rewrite
UPDATE `creature` SET `position_x` = -132.0905, `position_y` = -1564.7851, `position_z` = 72.8406 WHERE `id` = 66301;
