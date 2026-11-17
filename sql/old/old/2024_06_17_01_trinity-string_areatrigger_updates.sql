DELETE FROM `trinity_string` WHERE `entry` IN (1999, 1204);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(1999, 'You have left areatrigger %u.'),
(1204, 'You have entered areatrigger %u.');