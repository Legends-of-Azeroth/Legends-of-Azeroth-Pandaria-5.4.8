-- drop db_script_string
UPDATE `waypoint_scripts` SET `dataint`=0 WHERE `dataint` >= 2000000000;  
DROP TABLE IF EXISTS `db_script_string`;
