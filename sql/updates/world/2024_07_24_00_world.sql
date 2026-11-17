-- 
ALTER TABLE `creature_template_addon`   
	ADD COLUMN `StandState` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `MountCreatureID`,
	ADD COLUMN `AnimTier` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `StandState`,
	ADD COLUMN `VisFlags` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `AnimTier`,
	ADD COLUMN `SheathState` TINYINT UNSIGNED DEFAULT 1 NOT NULL AFTER `VisFlags`,
	ADD COLUMN `PvPFlags` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `SheathState`;

UPDATE `creature_template_addon` SET `StandState`=`bytes1` & 0xFF;
UPDATE `creature_template_addon` SET `AnimTier`=(`bytes1` >> 24) & 0xFF;
UPDATE `creature_template_addon` SET `VisFlags`=(`bytes1` >> 16) & 0xFF;
UPDATE `creature_template_addon` SET `SheathState`=`bytes2` & 0xFF;
UPDATE `creature_template_addon` SET `PvPFlags`=(`bytes2` >> 8) & 0xFF;

ALTER TABLE `creature_addon`   
	ADD COLUMN `StandState` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `MountCreatureID`,
	ADD COLUMN `AnimTier` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `StandState`,
	ADD COLUMN `VisFlags` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `AnimTier`,
	ADD COLUMN `SheathState` TINYINT UNSIGNED DEFAULT 1 NOT NULL AFTER `VisFlags`,
	ADD COLUMN `PvPFlags` TINYINT UNSIGNED DEFAULT 0 NOT NULL AFTER `SheathState`;

UPDATE `creature_addon` SET `StandState`=`bytes1` & 0xFF;
UPDATE `creature_addon` SET `AnimTier`=(`bytes1` >> 24) & 0xFF;
UPDATE `creature_addon` SET `VisFlags`=(`bytes1` >> 16) & 0xFF;
UPDATE `creature_addon` SET `SheathState`=`bytes2` & 0xFF;
UPDATE `creature_addon` SET `PvPFlags`=(`bytes2` >> 8) & 0xFF;

-- Conversion done, drop old columns
ALTER TABLE `creature_template_addon`  
	DROP COLUMN `bytes1`, 
	DROP COLUMN `bytes2`;

ALTER TABLE `creature_addon`  
	DROP COLUMN `bytes1`, 
	DROP COLUMN `bytes2`;

-- Add visibilityDistanceType
ALTER TABLE `creature_addon`
	ADD COLUMN `visibilityDistanceType` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `emote`;

ALTER TABLE `creature_template_addon`
	ADD COLUMN `visibilityDistanceType` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `emote`;

-- Set right visibility for creatures
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 90;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 392;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 1284;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 1493;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 1768;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 1853;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 1908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 2120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 2726;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 2748;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 2754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3653;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3654;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3669;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3670;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3671;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3914;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3936;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3943;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 3977;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4274;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4275;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4278;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4279;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4715;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4718;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4719;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4829;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4830;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4831;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4854;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 4968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 5359;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 5709;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 5710;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 5711;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 5775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6109;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6193;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6235;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6906;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6907;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 6910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7023;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 7024;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7079;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7206;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7228;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7267;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7271;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7273;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7275;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7291;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7361;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7795;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7797;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7800;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7846;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7977;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 7999;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8127;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8197;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8198;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8443;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8923;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8929;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8933;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 8983;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9016;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9017;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9018;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9019;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9024;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9025;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9027;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9028;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9029;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9031;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9032;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9033;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9156;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9319;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9499;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9502;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9568;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9659;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9736;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9816;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 9938;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10076;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10162;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10220;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10339;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10363;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10415;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10429;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10430;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10435;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10436;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10437;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10438;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10439;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10440;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10502;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10505;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10508;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10516;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10584;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10596;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10808;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10809;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10811;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10812;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10813;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10826;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10901;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10942;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 10997;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11032;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11261;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11262;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11380;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11486;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11487;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11488;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11489;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11490;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11496;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11501;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11502;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11517;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11519;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11520;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11583;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11622;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11658;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11659;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11660;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11661;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11662;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11666;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11667;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11668;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11669;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11671;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11897;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11978;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11981;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11982;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11983;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 11988;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12017;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12018;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12056;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12057;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12076;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12098;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12100;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12101;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12118;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12129;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12142;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12203;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12206;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12225;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12258;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12259;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12265;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12397;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12422;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12435;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12457;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12458;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12459;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12460;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12461;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12463;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12464;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12465;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12468;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12557;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12804;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 12806;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13020;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 13116;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 13117;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13161;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13178;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13221;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13280;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13282;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13419;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13596;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13738;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13976;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 13996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14020;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14261;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14262;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14263;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14265;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 14269;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 14272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14302;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14321;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14322;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14323;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14324;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14325;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14326;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14327;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14349;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14353;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14354;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14401;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14435;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14456;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14507;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14509;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14515;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14517;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14605;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14661;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 14824;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14834;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14861;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14888;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14889;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14943;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14944;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14945;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14946;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14947;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 14948;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15122;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15185;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15192;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15203;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15204;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15205;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15223;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15226;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15227;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15228;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15231;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15232;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15233;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15234;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15235;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15238;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15239;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15240;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15241;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15245;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15247;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15248;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15249;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15250;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15251;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15252;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15253;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15254;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15255;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15257;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15258;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15259;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15262;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15263;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15275;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15276;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15277;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15300;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15302;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15305;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15311;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15312;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15318;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15319;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15320;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15322;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15323;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15324;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15325;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15326;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15327;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15329;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15330;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15331;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15333;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15334;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15335;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15336;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15337;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15338;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15339;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15340;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15341;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15343;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15344;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15345;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15346;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15347;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15348;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15355;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15369;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15370;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15385;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15386;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15387;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15388;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15389;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15390;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15391;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15392;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15393;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15428;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15471;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15473;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15481;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15507;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15509;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15511;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15516;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15517;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15521;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15527;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15538;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15550;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15555;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15589;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15628;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15667;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15712;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15718;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15725;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15726;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15727;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15728;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15740;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15741;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15742;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15743;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15744;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15773;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15776;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15800;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15802;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15809;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15896;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15904;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15928;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15929;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15930;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15931;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15932;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15934;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15936;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15952;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15953;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15954;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15956;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15962;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15963;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15974;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15975;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15976;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15977;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15978;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15979;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15980;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15981;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15984;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15989;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 15990;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16011;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16017;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16018;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16020;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16021;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16022;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16024;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16025;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16026;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16027;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16028;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16029;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16034;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16039;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16040;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16059;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16060;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16061;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16062;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16063;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16065;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16067;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16097;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16098;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16101;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16102;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16104;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16125;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16126;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 16128;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16136;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16137;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16142;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16145;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16146;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16151;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16152;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16154;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16156;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16157;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16158;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16163;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16164;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16165;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16167;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16172;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16179;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16180;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16193;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16194;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16218;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16290;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16363;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16375;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16386;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16398;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16401;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16421;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16431;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16440;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16446;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16452;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16457;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16477;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16488;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16505;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16506;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16524;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16531;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16573;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16598;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16604;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16697;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16776;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16777;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16800;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16801;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16802;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16803;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16807;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16808;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16809;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16861;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 16939;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17007;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17075;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17076;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17225;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17257;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17258;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17306;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17307;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17308;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17309;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17367;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17368;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17369;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17377;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17380;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17381;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17388;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17427;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17435;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17454;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17461;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17462;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17468;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17470;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17521;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17534;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17536;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17547;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17592;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17602;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17603;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17611;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17621;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17622;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17623;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17645;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17646;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17650;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17652;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17653;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17711;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17719;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17720;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17725;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17767;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17770;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17796;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17797;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17798;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17808;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17818;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17826;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17830;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17839;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17848;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17852;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17862;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17864;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17871;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17876;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17880;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17881;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17888;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17895;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17897;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17898;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17905;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17906;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17907;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17916;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17918;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17941;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17942;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17951;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17965;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17975;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17976;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17977;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17978;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17980;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 17991;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18040;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18058;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18061;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18096;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18100;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18101;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18102;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18107;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18192;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18238;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18257;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18258;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18259;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18287;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18338;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18341;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18343;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18344;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18371;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18373;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18399;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18409;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18411;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18432;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18433;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18434;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18435;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18436;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18473;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18478;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18479;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18528;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18607;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18620;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18621;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18625;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18667;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18707;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18708;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18726;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18728;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18729;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18732;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 18733;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18757;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18759;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18769;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18770;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18780;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18793;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18794;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18796;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18806;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18829;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18831;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18834;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18835;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18836;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18847;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18848;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18928;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18932;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18945;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 18946;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19218;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19219;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19220;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19224;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19260;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19291;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19292;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19336;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19358;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19359;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19382;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19389;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19397;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19398;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19399;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19400;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19427;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19433;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19516;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19551;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19554;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19565;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19568;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19577;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19622;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19710;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19851;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19870;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19873;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19874;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19876;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19878;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19894;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19895;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19918;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19922;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19927;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19934;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 19951;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 20000;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20031;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20032;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20033;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20034;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20036;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20037;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20039;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20040;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20043;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20045;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20046;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20047;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20048;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20049;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20050;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20051;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20052;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20060;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20062;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20063;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20075;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20129;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20132;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20142;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20169;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20170;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20182;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20183;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20188;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20266;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20267;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20303;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20304;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20306;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20307;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20318;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20319;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20454;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20521;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20522;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20531;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20550;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20555;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20568;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20569;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20575;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20578;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20579;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20581;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20592;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20596;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20597;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20600;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20630;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20632;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20633;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20636;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20637;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20645;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20652;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20653;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20654;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20657;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20706;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20737;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20738;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20745;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20746;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20769;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20776;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20870;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20885;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20898;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20900;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20904;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20905;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20909;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20911;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20912;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20923;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20977;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 20993;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21075;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21091;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21101;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21102;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21104;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21122;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21140;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21148;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21153;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21154;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21159;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21170;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21174;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21187;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21212;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21213;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21219;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21222;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21224;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21225;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21226;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21227;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21228;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21251;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21269;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21270;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21271;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21273;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21274;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21315;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21338;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21350;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21362;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21364;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21404;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21456;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21525;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21526;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21551;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21581;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21582;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21588;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21589;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21590;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21598;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21599;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21600;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21602;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21616;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21617;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21618;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21620;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21621;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21622;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21624;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21626;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21657;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21685;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21686;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21697;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21698;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21712;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21785;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21801;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21806;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21812;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21845;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21862;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21865;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21873;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21874;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21913;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21915;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21918;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21919;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21920;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21931;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21932;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21933;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21958;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21964;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21965;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21966;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21976;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21984;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 21988;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22009;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22036;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22055;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22056;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22067;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22077;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22089;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22112;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22122;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22128;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22129;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22140;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22146;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22167;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22170;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22171;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22172;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22207;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22210;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22274;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22290;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22293;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22295;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22300;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22335;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22360;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22389;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22404;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22405;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22406;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22409;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22418;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22422;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 22441;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22509;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22517;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 22522;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22525;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 22526;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22556;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 22558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22563;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22564;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22566;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22570;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22593;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22594;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22627;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22686;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22841;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22844;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22845;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22846;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22847;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22848;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22849;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22852;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22853;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22854;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22855;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22856;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22859;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22869;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22871;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22873;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22874;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22876;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22878;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22880;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22881;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22883;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22884;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22885;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22898;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22917;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22942;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22945;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22947;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22948;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22949;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22951;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22952;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22953;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22954;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22956;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22960;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22961;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22962;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22964;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22980;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22988;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 22997;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23018;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23047;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23049;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23051;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23054;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23059;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23061;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23062;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23069;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23076;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23118;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23126;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23147;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23188;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23195;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23197;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23222;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23223;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23226;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23235;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23238;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23239;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23260;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23261;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23281;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23282;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23283;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23292;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23293;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23294;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23295;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23297;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23298;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23304;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23307;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23314;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23318;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23321;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23323;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23328;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23330;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23337;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23339;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23340;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23342;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23344;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23345;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23346;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23348;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23374;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23375;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23378;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23387;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23394;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23397;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23399;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23400;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23401;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23402;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23403;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23404;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23409;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23418;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23419;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23420;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23421;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23436;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23440;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23441;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23468;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23500;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23512;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23516;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23517;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23519;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23523;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23524;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23538;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23539;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23556;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23557;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23574;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23575;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23576;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23577;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23578;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23746;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23755;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23767;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23771;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23803;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23810;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23812;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23817;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23863;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23867;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23878;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23880;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23884;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23885;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23889;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23915;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23916;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23917;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23944;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23947;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23953;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23954;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23969;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23972;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 23980;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24000;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24008;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 24019;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24025;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24027;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24060;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24072;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24083;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24110;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24130;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24132;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24165;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24167;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24169;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24182;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24183;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24185;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24193;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24194;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24222;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24239;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24240;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24241;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24245;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24247;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24248;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24289;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24290;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24312;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24363;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24364;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24371;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24381;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24382;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24383;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24384;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24386;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24440;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24446;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24468;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24512;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24515;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24527;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 24538;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24549;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24553;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24554;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24555;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24556;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24557;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24560;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24561;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 24646;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 24647;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24652;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24675;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24682;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24701;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24707;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24712;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24721;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24744;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24745;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24747;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24762;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24769;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24770;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24771;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24781;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24786;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24787;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24795;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24797;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 24812;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24844;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24849;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24850;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24859;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24861;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24862;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24865;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24891;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24892;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24895;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24911;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24912;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24914;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24915;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 24925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24983;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24985;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24986;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 24987;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25006;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25031;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25033;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25158;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25160;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25161;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25164;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25165;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25175;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25289;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25315;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25319;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25330;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25334;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25349;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25356;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25364;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25383;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25386;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25387;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25431;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25436;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25462;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25465;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25471;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25473;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25490;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25492;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25493;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25508;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25525;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25541;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25549;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25550;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25552;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25555;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25556;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25557;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25560;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25562;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25567;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25573;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25574;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25578;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25579;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25588;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25594;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25640;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25666;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25708;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25724;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25727;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25732;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25733;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25735;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25740;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25741;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25744;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25745;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25748;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25750;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25755;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25756;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25757;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25760;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25762;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25764;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25765;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25771;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25791;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25817;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25824;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25840;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25845;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25846;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25847;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25855;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25860;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25864;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25952;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25954;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25956;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25958;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25959;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25960;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25964;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25965;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25966;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25971;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25972;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 25973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26046;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26057;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26093;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26094;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26117;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26121;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26127;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26129;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26130;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26174;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26175;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26193;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26206;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26231;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26247;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26260;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26261;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26265;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26266;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26276;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26277;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 26287;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26290;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26297;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26298;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 26310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26341;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26346;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26370;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26373;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26406;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26417;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26420;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26444;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26475;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26483;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26495;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26498;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26499;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26528;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26529;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26572;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26578;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26579;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26591;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26612;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26626;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26650;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26651;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26654;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26668;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26692;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26700;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26712;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26716;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26736;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26761;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26763;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26783;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26788;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26794;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26796;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26798;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26799;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26800;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26801;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26802;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26803;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26807;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26808;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26836;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26840;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26841;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26855;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26856;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26858;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26861;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26867;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26917;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26918;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26926;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26927;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26933;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 26967;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27036;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27104;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27220;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27241;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27249;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27255;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27257;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27270;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27285;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27292;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27326;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27327;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27338;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27389;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27390;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27396;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27397;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27413;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27439;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27455;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27480;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27513;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27587;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27605;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27611;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27619;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27631;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27638;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27641;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27654;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27655;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27660;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27671;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27679;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27682;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27702;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27709;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27710;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27753;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27766;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27767;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27768;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27769;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27770;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27771;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27773;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27774;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27776;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27777;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27779;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27780;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27781;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27782;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27801;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27821;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27837;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27843;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27847;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27850;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27852;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27862;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27869;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27880;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27881;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27883;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27888;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27894;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27905;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27921;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27929;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27931;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27939;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27947;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27949;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27956;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27969;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27978;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27979;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27980;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27981;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27982;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27992;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 27993;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 27996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28006;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28007;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28008;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28015;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28018;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28021;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28063;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28068;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 28094;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28135;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28137;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28151;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28157;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28160;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28174;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28198;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28207;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28208;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28211;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28212;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28240;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 28243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28248;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28255;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28274;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28276;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28292;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28293;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28294;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28295;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28304;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28305;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 28312;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28316;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 28319;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28330;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 28332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28356;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28357;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 28366;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28377;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28401;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28409;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28416;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28439;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28442;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28444;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28445;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28469;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28478;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28492;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28494;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28495;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28496;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28499;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28520;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28523;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28525;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28529;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28536;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28539;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28540;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28542;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28560;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28561;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28565;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28576;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28577;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 28586;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28587;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28591;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28594;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28614;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28616;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28617;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28631;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28636;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28639;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28642;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28647;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28653;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28654;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28658;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28660;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28662;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28670;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28671;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28672;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28684;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28717;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28729;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28730;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28738;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28739;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28740;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28741;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28751;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28761;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28762;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28765;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28777;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28780;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 28781;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28785;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28786;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28788;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28793;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28804;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28823;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28848;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28849;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28856;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 28859;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28860;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28873;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28874;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28884;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28907;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28909;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28911;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28919;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28920;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28921;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28922;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28923;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28924;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28928;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28929;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28931;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28932;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28952;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28960;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 28996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29048;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29050;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29051;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29062;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29063;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29066;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29077;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29078;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29079;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29080;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29096;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29097;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29098;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29100;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29102;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29104;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29107;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29108;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29109;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29110;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29112;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29113;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29114;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29117;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29118;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29128;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29150;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29153;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29173;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29175;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29176;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29177;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29178;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29179;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29180;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29182;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29183;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29199;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29204;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29213;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29227;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29228;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 29240;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29241;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29247;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29248;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29249;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 29263;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29266;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29273;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29274;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29278;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29306;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 29308;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29311;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29312;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29313;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29314;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29316;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29324;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29333;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29347;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29349;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29350;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29353;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29354;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29355;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29356;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29357;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29358;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29359;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29362;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29363;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29371;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29373;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29375;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29379;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29388;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29394;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29417;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29424;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29445;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29446;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29458;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29460;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29469;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29485;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29498;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29500;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29545;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29573;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29574;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29575;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29576;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29592;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29597;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29599;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29611;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29615;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29625;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29639;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29645;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29647;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29679;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29701;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29708;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29709;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29718;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29734;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 29747;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29748;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29753;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29755;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29756;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29773;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 29789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 29790;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29798;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29800;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29809;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29810;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29815;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29821;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29823;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29824;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29825;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29828;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29831;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29833;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29835;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29836;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29837;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29852;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29863;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29870;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29871;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29878;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29883;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29884;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29894;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29895;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29896;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29897;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29898;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29900;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29913;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29914;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29931;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29933;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29934;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29935;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29936;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29939;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29940;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29941;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29974;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29982;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29983;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29985;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29986;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 29991;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30014;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30017;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30019;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30020;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30021;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30022;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30023;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30024;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30025;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30026;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30043;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30045;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30050;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30052;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30055;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30061;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30068;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30071;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30075;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30082;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30084;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30087;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30096;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30097;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30118;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30121;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30122;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30123;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30124;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30125;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30126;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30127;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30138;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30139;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30141;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30142;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30145;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30150;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30151;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30161;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30163;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30169;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30188;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30204;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30205;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30209;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30210;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30211;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30212;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30228;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30235;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30252;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30260;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30262;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30275;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30288;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30294;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30295;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30298;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30300;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30302;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30320;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30327;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30328;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30330;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30331;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30333;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 30342;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 30343;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30366;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30389;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30390;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30393;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30394;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30396;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30397;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30398;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30399;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30403;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30406;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30407;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30415;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30420;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30421;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30425;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30426;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30427;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30428;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30442;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30443;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30445;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30452;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30455;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30456;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30457;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30459;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30461;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30462;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30476;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30482;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30486;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30493;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30495;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30496;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30498;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30500;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30502;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30508;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30509;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30521;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30522;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30529;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30532;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30540;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30545;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30549;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30564;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30575;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30576;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30588;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30589;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30592;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30593;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30598;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30599;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30600;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30602;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30603;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30614;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30616;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30639;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30640;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30646;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30649;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30651;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30660;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30670;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30675;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30677;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30679;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30680;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30681;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30682;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30695;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30699;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30719;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30742;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30744;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30745;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30748;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30749;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30752;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30753;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30755;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30774;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30788;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30794;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30807;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30810;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30819;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30822;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30823;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30837;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30858;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30861;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30866;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30867;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30881;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30883;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30892;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30903;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30905;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30934;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30935;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30936;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30972;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 30973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30988;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30991;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30995;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30998;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 30999;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31000;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31001;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31006;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31011;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31014;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31016;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31040;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31047;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31050;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31056;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31070;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31083;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31084;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31088;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31089;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31092;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31125;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31135;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31137;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31139;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31191;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31192;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31193;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31194;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31195;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31198;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31204;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31210;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31211;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31212;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31218;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31219;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31222;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31243;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31249;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 31253;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31255;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31271;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31277;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31280;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31293;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31301;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31311;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31312;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31333;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31334;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31359;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31364;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31367;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31368;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31373;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31386;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31388;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31390;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31391;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31393;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31394;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31399;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31400;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31406;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31419;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31437;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31464;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31465;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31469;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31501;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31502;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31504;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31508;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31509;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31511;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31512;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31520;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31526;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31534;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31536;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31538;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31539;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31540;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31541;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31547;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31560;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31561;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31565;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31583;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31585;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31586;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31588;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31589;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31590;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31591;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31592;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31593;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31594;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31596;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31597;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31598;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31599;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31600;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31602;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31603;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31604;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31607;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31610;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31611;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31612;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31615;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31616;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31617;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31630;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31640;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31650;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31651;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31657;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31669;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31676;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31679;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31680;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31684;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 31734;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31743;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31752;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31765;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31767;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31773;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31782;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31787;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31811;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31814;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31817;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31821;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31822;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31831;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31844;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 31869;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31880;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31892;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 31917;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 31920;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32002;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32004;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32010;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32023;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32056;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32136;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32138;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32154;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32155;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32159;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32160;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32167;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32173;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32174;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32180;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32185;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32186;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32187;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32195;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32197;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32199;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32202;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32221;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32231;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32232;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32234;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32235;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32245;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32254;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32266;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32273;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32277;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32292;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32295;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32305;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32313;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32314;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32318;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32319;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32344;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32347;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32348;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32353;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32365;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32368;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32376;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32378;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32400;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32422;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32431;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32442;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32443;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32445;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32446;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32473;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32479;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32480;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32482;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32483;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32485;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32486;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32487;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32491;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32492;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32495;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32501;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32511;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32512;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32513;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32520;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32521;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32522;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32523;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32524;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32534;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32540;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32551;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32552;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32553;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32554;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32555;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32569;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32576;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32577;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32578;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32579;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32580;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32594;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32596;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32597;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32598;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32600;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32601;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32602;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32627;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32630;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32662;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32725;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32742;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32768;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32795;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 32796;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32845;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32846;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32865;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32867;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32871;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32873;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32892;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32897;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32898;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32900;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32901;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32906;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32913;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32914;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32915;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32927;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32930;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 32933;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32934;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32941;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32946;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32948;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32953;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 32958;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33045;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33051;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33059;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33060;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33061;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33062;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33063;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33067;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33068;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33070;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33080;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33081;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33087;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33090;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33108;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33109;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33113;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33114;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33118;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33121;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33134;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33136;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33138;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33139;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33140;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33141;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33142;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33146;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33147;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33148;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33149;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33154;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33167;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33174;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33186;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33191;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33192;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33212;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33213;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33218;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33221;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33234;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33241;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33259;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33271;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33280;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33282;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33287;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33288;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33293;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33303;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33325;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33326;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33327;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33328;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33329;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33330;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33331;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33333;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33337;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33343;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33344;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33346;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33350;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33354;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33355;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33360;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33364;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33365;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33366;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33367;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33369;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33370;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33377;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33378;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33387;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33388;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33391;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33392;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33393;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33410;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33411;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33412;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33413;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33425;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33426;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33430;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33431;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33432;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33433;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33441;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33488;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33500;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33515;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33516;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33524;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33525;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33526;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33527;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33528;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33567;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33572;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33575;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33576;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33579;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33651;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33666;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33667;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33668;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33670;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33692;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33695;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33697;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33699;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33700;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33705;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33716;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33717;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33718;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33719;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33720;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33724;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33729;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33730;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33732;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33733;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33734;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33735;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33737;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33741;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33755;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33757;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33758;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33768;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33773;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33779;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33806;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33815;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33816;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33818;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33819;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33820;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33822;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33823;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33824;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33827;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33828;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33829;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33830;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33831;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33835;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33836;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33839;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33846;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33850;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33851;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33852;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33883;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 33885;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33888;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33909;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 33910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33911;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33954;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33966;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33967;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33983;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33984;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33985;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33986;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33988;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33989;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33990;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33993;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33994;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33995;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33998;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 33999;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34000;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34001;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34003;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34014;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34015;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34016;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34034;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34045;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34049;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34054;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34068;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34069;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34071;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34086;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34106;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34108;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34109;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34111;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34113;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34145;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34152;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34159;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34161;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34162;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34164;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34165;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34169;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34171;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34175;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34185;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34186;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34192;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34194;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34197;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34198;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34199;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34218;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34226;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34234;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34235;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34237;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34245;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34247;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34255;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34257;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34275;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34276;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34277;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34295;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34297;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34313;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34362;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34434;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34441;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34442;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34443;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34444;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34445;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34450;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34454;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34455;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34456;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34458;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34459;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34460;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34461;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34463;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34465;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34468;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34469;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34470;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34471;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34473;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34474;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34475;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34496;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34515;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34564;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34566;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34567;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34568;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34606;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34607;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34628;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34630;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34649;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34657;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34660;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34701;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34702;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34703;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34704;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34705;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34720;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34735;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34737;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34738;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34739;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34740;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34741;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34775;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34776;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34777;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34778;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34780;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34793;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34796;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34797;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34799;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34802;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34810;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34813;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34815;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34825;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34826;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34862;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34921;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34922;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34924;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 34925;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34928;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34929;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34935;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34942;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 34944;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34990;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34992;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34994;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34995;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 34996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35013;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35028;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35029;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35031;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35032;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35033;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35034;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35036;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35037;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35039;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35040;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35043;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35045;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35046;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35047;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35048;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35049;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35050;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35051;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35052;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35055;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35069;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35089;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35092;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35106;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35145;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35150;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35163;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35176;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35242;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35262;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35263;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35265;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35266;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35267;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35268;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35269;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35270;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35271;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35273;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35278;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35279;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35280;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35297;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35320;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35321;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35335;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35336;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35339;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35347;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35348;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35349;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35350;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35353;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35354;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35355;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35356;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35357;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35358;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35359;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35360;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35368;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35372;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35373;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35377;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35403;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35405;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35410;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35413;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35415;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35417;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35419;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35421;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35427;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35429;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35431;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35433;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35436;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35438;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35439;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35440;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35447;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35448;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35459;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35470;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35474;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35476;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35482;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35490;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35493;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35511;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35512;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35513;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35514;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35515;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35516;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35517;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35519;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35520;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35521;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35522;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35523;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35524;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35525;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35527;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35528;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35529;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35531;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35532;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35534;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35536;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35537;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35538;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35539;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35540;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35541;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35542;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35545;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35547;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35557;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35559;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35560;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35564;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35568;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35569;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35570;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35572;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35590;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35615;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35616;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35617;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35651;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35655;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35662;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35666;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35667;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35668;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35669;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35670;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35671;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35672;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35675;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35676;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35680;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35681;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35682;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35683;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35684;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35685;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35686;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35692;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35695;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35696;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35697;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35699;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35700;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35701;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35702;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35703;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35704;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35705;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35706;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35707;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35708;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35709;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35710;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35711;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35712;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35713;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35714;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35715;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35716;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35717;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35718;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35719;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35720;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35721;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35724;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35725;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35726;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35728;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35729;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35730;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35732;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35733;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35734;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35735;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35736;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35737;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35738;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35739;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35740;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35741;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35742;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35743;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35744;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35745;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35746;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35747;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35748;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35749;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 35819;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 35827;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 35877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36065;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36066;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36067;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36082;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36083;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36084;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36086;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36087;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36088;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36089;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36090;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36091;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36128;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36140;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36155;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36165;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36171;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36231;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36272;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36283;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36355;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36356;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36357;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36358;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36449;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36476;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36477;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36494;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36498;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36502;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36504;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36508;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36538;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36561;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36565;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36566;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36572;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36597;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36598;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36612;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36616;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36626;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36627;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36633;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36658;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36661;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36678;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36701;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36725;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36735;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36737;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36788;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36791;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36794;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36804;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36807;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36808;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36811;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36818;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36819;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36820;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36821;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36823;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36824;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36829;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36838;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36839;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36840;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36841;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36844;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36853;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36855;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36881;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36892;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36896;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36897;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36907;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36913;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36938;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36940;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36941;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36944;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36945;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36946;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36947;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36954;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 36955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36960;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36961;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 36966;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36967;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36969;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36970;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36971;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36978;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36982;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36990;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36993;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 36998;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37003;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37004;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37007;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37011;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37012;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37014;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37016;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37017;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37021;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37022;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37023;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37025;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37026;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37027;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37028;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37029;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37031;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37032;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37033;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37034;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37035;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37037;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37065;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37069;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37098;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37116;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37117;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37119;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37122;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37123;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37124;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37125;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37126;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37129;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37146;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37148;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37149;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37186;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37188;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37193;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37194;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37203;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37221;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37223;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37225;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37226;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37233;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37321;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37323;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37329;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37341;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37343;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37363;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37377;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37464;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37490;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37491;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37493;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37494;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37495;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37504;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37505;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37506;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37528;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37533;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37534;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37535;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37540;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37545;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37549;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37550;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37551;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37554;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37562;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37571;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37574;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37593;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37596;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37597;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37611;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37612;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37613;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37622;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37627;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37636;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37637;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37639;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37640;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37641;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37642;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37644;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37662;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37663;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37665;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37666;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37672;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37677;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37678;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37695;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37697;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37698;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37702;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37704;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37712;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37713;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37720;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37728;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37729;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37730;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37755;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37799;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37801;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37804;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37813;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37814;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37826;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37827;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37829;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37830;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37871;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37878;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37882;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37894;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37898;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37902;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37903;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37904;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37920;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37928;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37930;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37932;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37934;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37935;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37936;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37949;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37952;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37957;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37958;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 37959;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37965;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37967;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37970;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37972;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 37973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 37996;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38000;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38001;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38004;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38008;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38009;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38010;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38019;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38020;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38021;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38025;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38026;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38031;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38052;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38058;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38059;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38061;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38062;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38063;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38072;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38073;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38074;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38075;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38076;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38077;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38078;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38079;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38080;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38081;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38082;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38083;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38084;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38086;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38087;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38088;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38089;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38090;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38091;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38092;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38093;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38094;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38095;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38096;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38097;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38098;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38099;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38100;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38101;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38102;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38103;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38106;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38108;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38121;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38123;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38128;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38129;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38135;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38136;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38138;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38150;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38153;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38154;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38160;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38161;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38162;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38169;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38170;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38174;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38185;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38188;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38191;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38194;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38216;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38219;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38220;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38226;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38230;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38236;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38248;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38256;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38257;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38258;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38261;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38262;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38264;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38265;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38266;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38267;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38285;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38289;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38291;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38292;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38297;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38298;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38303;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38304;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38308;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38309;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38332;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38349;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38350;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38351;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38352;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38367;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38377;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38386;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38388;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38390;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38393;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38394;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38395;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38396;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38397;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38398;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38399;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38400;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38401;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38402;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38403;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38404;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38405;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38406;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38407;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38408;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38418;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38431;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38433;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38434;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38435;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38436;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38444;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38445;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38446;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38451;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38462;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38479;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38480;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38481;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38482;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38483;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38485;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38487;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38490;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38493;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38494;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38500;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38505;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38527;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38528;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38529;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38547;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38549;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38550;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38557;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38569;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38572;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38579;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38582;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38583;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38584;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38585;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38586;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38587;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38588;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38589;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38590;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38602;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38604;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38605;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38606;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38610;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38625;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38626;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38628;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38629;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38630;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38631;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38632;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38633;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38634;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38635;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38641;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38675;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38676;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38677;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38678;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38679;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38680;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38681;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38682;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38683;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38684;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38685;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38686;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38688;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38689;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38690;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38692;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38699;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38700;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38701;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38702;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38721;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38724;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38733;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38734;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38751;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38757;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38758;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38759;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38760;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38761;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38769;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38770;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38771;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38773;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38774;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38784;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38785;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38786;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38787;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38788;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38790;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38821;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 38857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38883;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 38917;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 38995;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39000;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39001;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39057;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39091;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39092;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39120;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39121;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39122;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39123;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39125;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39137;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39166;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39167;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39190;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39231;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39232;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39233;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 39234;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39271;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39273;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39284;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39285;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39286;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39287;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39288;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39289;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39296;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39300;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39301;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39302;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39303;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39304;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39305;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39306;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39307;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39309;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39310;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39311;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39372;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39420;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39654;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39735;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39743;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39744;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39746;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39747;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39751;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39759;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39799;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39800;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39801;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39802;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39803;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39805;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39814;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39815;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39820;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39823;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39837;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39841;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39859;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39863;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39864;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39901;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39903;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39908;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39913;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39919;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39920;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39922;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39944;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 39945;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 39964;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40001;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40029;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40043;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40044;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40055;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40083;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40091;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40100;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40135;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40142;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40144;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40145;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40146;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40178;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40188;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40189;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40192;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40218;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40219;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40282;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40301;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40309;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40340;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40341;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40416;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40417;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40418;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40419;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40420;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40421;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40422;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40423;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40424;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40429;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40466;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40468;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40469;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40484;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40499;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40506;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40626;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40627;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40628;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 40655;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40670;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40671;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40672;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40673;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40675;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40681;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40682;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40683;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40684;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40704;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 40728;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 40870;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40942;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 40987;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 40995;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 40999;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 41178;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41417;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41418;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41504;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 41550;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41570;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41630;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41632;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41634;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41641;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41642;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41894;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41955;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41987;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41988;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41997;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 41998;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 42015;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 42098;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 42173;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 42196;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42215;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 42229;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42314;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42317;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42347;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42389;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42419;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42471;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42846;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42847;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42848;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 42849;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 42851;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 43094;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 43217;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 43244;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 43280;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 43281;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 43282;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 43359;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 43370;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 43641;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 43718;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44058;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 44140;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44365;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44367;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44403;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44453;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44467;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44471;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 44566;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 44581;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44592;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44593;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44610;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44615;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44641;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44645;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44650;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44652;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44687;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44731;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44737;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44764;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44797;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44835;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 44842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44885;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44887;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44888;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44889;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44890;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44891;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 44923;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 44951;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45030;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45181;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45186;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45213;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45291;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 45314;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45316;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45423;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45475;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45492;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45511;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45561;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45681;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45693;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45790;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45799;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45870;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45871;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 45872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45886;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45893;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 45895;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46003;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46041;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46042;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46134;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46136;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46277;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46321;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46345;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46361;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46401;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46402;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46416;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46418;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46560;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46603;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46644;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46646;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46692;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46717;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46753;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46835;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46877;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46878;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 46880;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46931;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 46965;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47079;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47210;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47221;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47222;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47287;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47452;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47455;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47484;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47510;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47546;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47594;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47625;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47643;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47656;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47684;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47732;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47763;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47766;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47772;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47777;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47790;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47791;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47792;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47793;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47829;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 47930;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48012;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48028;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 48092;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 48097;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48168;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 48185;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48214;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48501;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48564;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 48606;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48697;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 48858;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 48977;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 48983;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 49111;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 49132;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 49197;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 49454;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 49526;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 49529;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 49530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 49531;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 49552;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 49910;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 49941;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50038;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50055;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50056;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50057;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50369;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50416;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50470;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50472;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 50939;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 51060;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 51065;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 51127;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 51131;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 51340;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 51346;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 51609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 51624;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 52151;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 52176;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 52409;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 52530;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 52558;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 52659;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53102;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53115;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53154;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53158;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53369;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 53394;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 53396;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 53518;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53647;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 53649;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53680;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53681;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53691;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53694;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53695;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53698;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 53723;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 53738;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53786;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53791;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53872;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53876;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53896;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53899;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53900;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 53966;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 53986;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54020;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 54043;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54143;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54299;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 54313;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 54421;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 54432;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54486;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54488;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54489;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 54499;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54508;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54513;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54544;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 54548;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 54969;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 55078;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 55085;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 55091;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 55265;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 55415;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 55541;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 55542;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 55543;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 55971;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 56087;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 56126;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 56479;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 56622;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 57201;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 57769;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 58632;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 58633;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 58662;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 58664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 58722;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 58875;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59080;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59113;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59150;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59153;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59155;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59184;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59198;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59209;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59223;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59304;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59480;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59503;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59613;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 59789;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 60040;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 68553;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 72647;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 72674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 72677;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 72785;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 72793;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 73595;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 73857;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 76380;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 77370;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 77376;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 77488;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 77617;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 79228;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 79274;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 79285;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 79603;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 80026;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 81406;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 81492;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 82378;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 82417;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 85312;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 89978;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 92879;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 93105;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 93628;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 93759;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 94580;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 94960;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 94968;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 95049;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 95674;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 95675;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 95676;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 95832;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 95833;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 95834;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 95842;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 95843;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 96159;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 96574;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 96608;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 96609;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 96640;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 96648;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 96664;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 96677;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 96754;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 96756;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 97068;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 97081;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 97083;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 97084;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 97087;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 97097;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 97163;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 97197;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 97200;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 97202;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 97219;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 97795;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 98291;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 98482;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 98483;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 98484;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 98486;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 98497;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 98905;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 98907;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 98973;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 99804;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 101064;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 101638;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 101640;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 101704;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 101748;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 101950;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 101951;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 101976;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 101995;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 3 WHERE `entry` = 102246;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 102267;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 102279;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 102281;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 102326;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` = 103115;
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 4 WHERE `entry` = 108525;

-- creature_template_movement
DROP TABLE IF EXISTS `creature_template_movement`;
CREATE TABLE `creature_template_movement` (
  `CreatureId` int unsigned NOT NULL DEFAULT '0',
  `Ground` tinyint unsigned DEFAULT NULL,
  `Swim` tinyint unsigned DEFAULT NULL,
  `Flight` tinyint unsigned DEFAULT NULL,
  `Rooted` tinyint unsigned DEFAULT NULL,
  `Chase` tinyint unsigned DEFAULT NULL,
  `Random` tinyint unsigned DEFAULT NULL,
  `InteractionPauseTimer` int unsigned DEFAULT NULL COMMENT 'Time (in milliseconds) during which creature will not move after interaction with player',
  PRIMARY KEY (`CreatureId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS `creature_movement_override`;
CREATE TABLE `creature_movement_override` (
  `SpawnId` int unsigned NOT NULL DEFAULT '0',
  `Ground` tinyint unsigned DEFAULT NULL,
  `Swim` tinyint unsigned DEFAULT NULL,
  `Flight` tinyint unsigned DEFAULT NULL,
  `Rooted` tinyint unsigned DEFAULT NULL,
  `Chase` tinyint unsigned DEFAULT NULL,
  `Random` tinyint unsigned DEFAULT NULL,
  `InteractionPauseTimer` int unsigned DEFAULT NULL COMMENT 'Time (in milliseconds) during which creature will not move after interaction with player',
  PRIMARY KEY (`SpawnId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

INSERT IGNORE INTO `creature_template_movement`(`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) SELECT `entry`,0,0,0,0 FROM `creature_template` WHERE `InhabitType`!=3;
UPDATE `creature_template_movement` SET `Ground`=1 WHERE `CreatureId` IN (SELECT `entry` FROM `creature_template` WHERE `InhabitType` & 1);
UPDATE `creature_template_movement` SET `Swim`=1 WHERE `CreatureId` IN (SELECT `entry` FROM `creature_template` WHERE `InhabitType` & 2);
UPDATE `creature_template_movement` SET `Flight`=1 WHERE `CreatureId` IN (SELECT `entry` FROM `creature_template` WHERE (`InhabitType` & 5) = 4);
UPDATE `creature_template_movement` SET `Flight`=2 WHERE `CreatureId` IN (SELECT `entry` FROM `creature_template` WHERE (`InhabitType` & 5) = 5);
UPDATE `creature_template_movement` SET `Rooted`=1 WHERE `CreatureId` IN (SELECT `entry` FROM `creature_template` WHERE `InhabitType` & 8);

ALTER TABLE `creature_template` DROP `InhabitType`;

DELETE FROM `trinity_string` WHERE `entry`=11008;
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `content_loc9`, `content_loc10`, `content_loc11`, `content_female`, `content_female_loc1`, `content_female_loc2`, `content_female_loc3`, `content_female_loc4`, `content_female_loc5`, `content_female_loc6`, `content_female_loc7`, `content_female_loc8`, `content_female_loc9`, `content_female_loc10`, `content_female_loc11`) 
VALUES (11008, 'Movement type: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

DELETE FROM `command` WHERE `name`='reload creature_movement_override';
INSERT INTO `command` (`name`,`security`,`help`) VALUES
('reload creature_movement_override',5,'Syntax: .reload creature_movement_override\nReload creature_movement_override table.');

-- More data for creature_template_movement
INSERT IGNORE INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) 
VALUES 
(1, 0, 0, 1, 0, 0, 0, NULL),
(30, 1, 0, 0, 0, 0, 0, NULL),
(43, 1, 0, 0, 0, 0, 0, NULL),
(67, 1, 1, 0, 0, 0, 0, NULL),
(69, 1, 0, 0, 0, 0, 0, NULL),
(70, 1, 1, 0, 0, 0, 0, NULL),
(72, 1, 1, 0, 0, 0, 0, NULL),
(75, 1, 1, 0, 0, 0, 0, NULL),
(81, 1, 1, 0, 0, 0, 0, NULL),
(113, 1, 0, 0, 0, 0, 0, NULL),
(118, 1, 0, 0, 0, 0, 0, NULL),
(119, 1, 0, 0, 0, 0, 0, NULL),
(149, 1, 0, 0, 0, 0, 0, NULL),
(150, 1, 1, 0, 0, 0, 0, NULL),
(157, 1, 1, 0, 0, 0, 0, NULL),
(161, 1, 1, 0, 0, 0, 0, NULL),
(163, 1, 1, 0, 0, 0, 0, NULL),
(165, 1, 1, 0, 0, 0, 0, NULL),
(193, 1, 0, 0, 0, 0, 0, NULL),
(200, 1, 0, 0, 0, 0, 0, NULL),
(204, 1, 0, 0, 0, 0, 0, NULL),
(207, 1, 1, 0, 0, 0, 0, NULL),
(208, 1, 1, 0, 0, 0, 0, NULL),
(209, 1, 1, 0, 0, 0, 0, NULL),
(213, 1, 0, 0, 0, 0, 0, NULL),
(217, 1, 1, 0, 0, 0, 0, NULL),
(219, 1, 1, 0, 0, 0, 0, NULL),
(220, 1, 1, 0, 0, 0, 0, NULL),
(243, 1, 1, 0, 0, 0, 0, NULL),
(260, 1, 1, 0, 0, 0, 0, NULL),
(296, 1, 1, 0, 0, 0, 0, NULL),
(298, 1, 1, 0, 0, 0, 0, NULL),
(299, 1, 0, 0, 0, 0, 0, NULL),
(301, 1, 1, 0, 0, 0, 0, NULL),
(318, 1, 1, 0, 0, 0, 0, NULL),
(319, 1, 1, 0, 0, 0, 0, NULL),
(320, 1, 1, 0, 0, 0, 0, NULL),
(321, 1, 1, 0, 0, 0, 0, NULL),
(322, 1, 1, 0, 0, 0, 0, NULL),
(323, 1, 1, 0, 0, 0, 0, NULL),
(324, 1, 1, 0, 0, 0, 0, NULL),
(326, 1, 1, 0, 0, 0, 0, NULL),
(330, 1, 0, 0, 0, 0, 0, NULL),
(333, 1, 1, 0, 0, 0, 0, NULL),
(336, 1, 1, 0, 0, 0, 0, NULL),
(339, 1, 1, 0, 0, 0, 0, NULL),
(345, 1, 0, 0, 0, 0, 0, NULL),
(354, 1, 1, 0, 0, 0, 0, NULL),
(356, 1, 0, 0, 0, 0, 0, NULL),
(359, 1, 0, 0, 0, 0, 0, NULL),
(380, 1, 1, 0, 0, 0, 0, NULL),
(386, 1, 1, 0, 0, 0, 0, NULL),
(390, 1, 0, 0, 0, 0, 0, NULL),
(417, 1, 0, 0, 0, 0, 0, NULL),
(428, 0, 0, 1, 0, 0, 0, NULL),
(441, 1, 0, 0, 0, 0, 0, NULL),
(442, 1, 0, 0, 0, 0, 0, NULL),
(444, 1, 0, 0, 0, 0, 0, NULL),
(454, 1, 1, 0, 0, 0, 0, NULL),
(470, 1, 1, 0, 0, 0, 0, NULL),
(471, 1, 1, 0, 0, 0, 0, NULL),
(496, 1, 1, 0, 0, 0, 0, NULL),
(497, 1, 1, 0, 0, 0, 0, NULL),
(498, 1, 1, 0, 0, 0, 0, NULL),
(505, 1, 0, 0, 0, 0, 0, NULL),
(509, 1, 1, 0, 0, 0, 0, NULL),
(516, 1, 1, 0, 0, 0, 0, NULL),
(521, 1, 1, 0, 0, 0, 0, NULL),
(524, 1, 0, 0, 0, 0, 0, NULL),
(525, 1, 0, 0, 0, 0, 0, NULL),
(535, 1, 1, 0, 0, 0, 0, NULL),
(536, 1, 1, 0, 0, 0, 0, NULL),
(538, 1, 1, 0, 0, 0, 0, NULL),
(539, 1, 0, 0, 0, 0, 0, NULL),
(547, 1, 0, 0, 0, 0, 0, NULL),
(565, 1, 0, 0, 0, 0, 0, NULL),
(569, 1, 0, 0, 0, 0, 0, NULL),
(574, 1, 1, 0, 0, 0, 0, NULL),
(586, 1, 1, 0, 0, 0, 0, NULL),
(592, 1, 1, 0, 0, 0, 0, NULL),
(605, 1, 1, 0, 0, 0, 0, NULL),
(606, 1, 1, 0, 0, 0, 0, NULL),
(607, 1, 1, 0, 0, 0, 0, NULL),
(609, 1, 1, 0, 0, 0, 0, NULL),
(610, 1, 1, 0, 0, 0, 0, NULL),
(611, 1, 1, 0, 0, 0, 0, NULL),
(612, 1, 1, 0, 0, 0, 0, NULL),
(613, 1, 1, 0, 0, 0, 0, NULL),
(614, 1, 1, 0, 0, 0, 0, NULL),
(616, 1, 0, 0, 0, 0, 0, NULL),
(620, 1, 0, 0, 0, 0, 0, NULL),
(628, 1, 1, 0, 0, 0, 0, NULL),
(681, 1, 0, 0, 0, 0, 0, NULL),
(682, 1, 0, 0, 0, 0, 0, NULL),
(683, 1, 0, 0, 0, 0, 0, NULL),
(684, 1, 0, 0, 0, 0, 0, NULL),
(685, 1, 0, 0, 0, 0, 0, NULL),
(686, 1, 0, 0, 0, 0, 0, NULL),
(687, 1, 0, 0, 0, 0, 0, NULL),
(698, 1, 0, 0, 0, 0, 0, NULL),
(704, 1, 0, 0, 0, 0, 0, NULL),
(705, 1, 0, 0, 0, 0, 0, NULL),
(708, 1, 0, 0, 0, 0, 0, NULL),
(721, 1, 0, 0, 0, 0, 0, NULL),
(725, 1, 0, 0, 0, 0, 0, NULL),
(728, 1, 0, 0, 0, 0, 0, NULL),
(729, 1, 0, 0, 0, 0, 0, NULL),
(730, 1, 0, 0, 0, 0, 0, NULL),
(731, 1, 0, 0, 0, 0, 0, NULL),
(736, 1, 0, 0, 0, 0, 0, NULL),
(753, 1, 1, 0, 0, 0, 0, NULL),
(756, 1, 0, 0, 0, 0, 0, NULL),
(758, 1, 0, 0, 0, 0, 0, NULL),
(767, 1, 0, 0, 0, 0, 0, NULL),
(768, 1, 0, 0, 0, 0, 0, NULL),
(769, 1, 0, 0, 0, 0, 0, NULL),
(772, 1, 0, 0, 0, 0, 0, NULL),
(809, 1, 1, 0, 0, 0, 0, NULL),
(822, 1, 0, 0, 0, 0, 0, NULL),
(832, 1, 0, 0, 0, 0, 0, NULL),
(833, 1, 1, 0, 0, 0, 0, NULL),
(834, 1, 1, 0, 0, 0, 0, NULL),
(854, 1, 0, 0, 0, 0, 0, NULL),
(855, 1, 0, 0, 0, 0, 0, NULL),
(856, 1, 0, 0, 0, 0, 0, NULL),
(858, 1, 1, 0, 0, 0, 0, NULL),
(883, 1, 0, 0, 0, 0, 0, NULL),
(890, 1, 0, 0, 0, 0, 0, NULL),
(897, 1, 1, 0, 0, 0, 0, NULL),
(904, 1, 1, 0, 0, 0, 0, NULL),
(905, 0, 1, 0, 0, 0, 0, NULL),
(919, 1, 1, 0, 0, 0, 0, NULL),
(923, 1, 0, 0, 0, 0, 0, NULL),
(924, 1, 0, 0, 0, 0, 0, NULL),
(930, 1, 1, 0, 0, 0, 0, NULL),
(949, 1, 0, 0, 0, 0, 0, NULL),
(976, 1, 0, 0, 0, 0, 0, NULL),
(977, 1, 0, 0, 0, 0, 0, NULL),
(1015, 1, 0, 0, 0, 0, 0, NULL),
(1016, 1, 1, 0, 0, 0, 0, NULL),
(1017, 1, 0, 0, 0, 0, 0, NULL),
(1018, 1, 0, 0, 0, 0, 0, NULL),
(1019, 1, 1, 0, 0, 0, 0, NULL),
(1020, 1, 0, 0, 0, 0, 0, NULL),
(1021, 1, 0, 0, 0, 0, 0, NULL),
(1022, 1, 0, 0, 0, 0, 0, NULL),
(1023, 1, 0, 0, 0, 0, 0, NULL),
(1058, 1, 1, 0, 0, 0, 0, NULL),
(1085, 1, 0, 0, 0, 0, 0, NULL),
(1111, 1, 1, 0, 0, 0, 0, NULL),
(1112, 1, 1, 0, 0, 0, 0, NULL),
(1126, 1, 0, 0, 0, 0, 0, NULL),
(1127, 1, 0, 0, 0, 0, 0, NULL),
(1128, 1, 0, 0, 0, 0, 0, NULL),
(1129, 1, 0, 0, 0, 0, 0, NULL),
(1130, 1, 0, 0, 0, 0, 0, NULL),
(1131, 1, 0, 0, 0, 0, 0, NULL),
(1132, 1, 0, 0, 0, 0, 0, NULL),
(1133, 1, 0, 0, 0, 0, 0, NULL),
(1138, 1, 0, 0, 0, 0, 0, NULL),
(1140, 1, 1, 0, 0, 0, 0, NULL),
(1182, 1, 1, 0, 0, 0, 0, NULL),
(1184, 1, 0, 0, 0, 0, 0, NULL),
(1185, 1, 0, 0, 0, 0, 0, NULL),
(1186, 1, 1, 0, 0, 0, 0, NULL),
(1188, 1, 0, 0, 0, 0, 0, NULL),
(1189, 1, 0, 0, 0, 0, 0, NULL),
(1190, 1, 0, 0, 0, 0, 0, NULL),
(1191, 1, 0, 0, 0, 0, 0, NULL),
(1192, 1, 0, 0, 0, 0, 0, NULL),
(1193, 0, 1, 0, 0, 0, 0, NULL),
(1195, 1, 1, 0, 0, 0, 0, NULL),
(1196, 1, 0, 0, 0, 0, 0, NULL),
(1199, 1, 0, 0, 0, 0, 0, NULL),
(1201, 1, 0, 0, 0, 0, 0, NULL),
(1224, 0, 1, 0, 0, 0, 0, NULL),
(1225, 1, 1, 0, 0, 0, 0, NULL),
(1230, 1, 1, 0, 0, 0, 0, NULL),
(1235, 1, 1, 0, 0, 0, 0, NULL),
(1258, 1, 0, 0, 0, 0, 0, NULL),
(1288, 1, 1, 0, 0, 0, 0, NULL),
(1290, 1, 1, 0, 0, 0, 0, NULL),
(1306, 1, 1, 0, 0, 0, 0, NULL),
(1322, 1, 1, 0, 0, 0, 0, NULL),
(1334, 1, 1, 0, 0, 0, 0, NULL),
(1353, 1, 0, 0, 0, 0, 0, NULL),
(1361, 1, 1, 0, 0, 0, 0, NULL),
(1412, 1, 0, 0, 0, 0, 0, NULL),
(1455, 1, 0, 0, 0, 0, 0, NULL),
(1467, 1, 1, 0, 0, 0, 0, NULL),
(1485, 1, 1, 0, 0, 0, 0, NULL),
(1504, 1, 0, 0, 0, 0, 0, NULL),
(1505, 1, 0, 0, 0, 0, 0, NULL),
(1508, 1, 0, 0, 0, 0, 0, NULL),
(1509, 1, 0, 0, 0, 0, 0, NULL),
(1512, 0, 0, 1, 0, 0, 0, NULL),
(1546, 1, 1, 0, 0, 0, 0, NULL),
(1547, 1, 0, 0, 0, 0, 0, NULL),
(1548, 1, 0, 0, 0, 0, 0, NULL),
(1549, 1, 0, 0, 0, 0, 0, NULL),
(1555, 1, 0, 0, 0, 0, 0, NULL),
(1567, 1, 1, 0, 0, 0, 0, NULL),
(1643, 1, 1, 0, 0, 0, 0, NULL),
(1644, 1, 1, 0, 0, 0, 0, NULL),
(1649, 1, 1, 0, 0, 0, 0, NULL),
(1659, 1, 1, 0, 0, 0, 0, NULL),
(1677, 1, 1, 0, 0, 0, 0, NULL),
(1688, 1, 0, 0, 0, 0, 0, NULL),
(1689, 1, 0, 0, 0, 0, 0, NULL),
(1695, 1, 1, 0, 0, 0, 0, NULL),
(1713, 1, 0, 0, 0, 0, 0, NULL),
(1765, 1, 1, 0, 0, 0, 0, NULL),
(1766, 1, 1, 0, 0, 0, 0, NULL),
(1768, 1, 1, 0, 0, NULL, 2, NULL),
(1778, 1, 1, 0, 0, 0, 0, NULL),
(1780, 1, 1, 0, 0, 0, 0, NULL),
(1781, 1, 0, 0, 0, 0, 0, NULL),
(1797, 1, 1, 0, 0, 0, 0, NULL),
(1815, 1, 1, 0, 0, 0, 0, NULL),
(1816, 1, 0, 0, 0, 0, 0, NULL),
(1817, 1, 1, 0, 0, 0, 0, NULL),
(1821, 1, 0, 0, 0, 0, 0, NULL),
(1822, 1, 1, 0, 0, 0, 0, NULL),
(1823, 1, 0, 0, 0, 0, 0, NULL),
(1824, 1, 0, 0, 0, 0, 0, NULL),
(1825, 1, 0, 0, 0, 0, 0, NULL),
(1859, 1, 1, 0, 0, 0, 0, NULL),
(1860, 1, 0, 0, 0, 0, 0, NULL),
(1863, 1, 0, 0, 0, 0, 0, NULL),
(1908, 1, 1, 0, 0, NULL, 2, NULL),
(1921, 1, 1, 0, 1, 0, 0, NULL),
(1922, 1, 0, 0, 0, 0, 0, NULL),
(1923, 1, 0, 0, 0, 0, 0, NULL),
(1933, 1, 0, 0, 0, 0, 0, NULL),
(1961, 1, 1, 0, 0, 0, 0, NULL),
(1984, 1, 0, 0, 0, 0, 0, NULL),
(1985, 1, 0, 0, 0, 0, 0, NULL),
(1986, 1, 0, 0, 0, 0, 0, NULL),
(1987, 1, 0, 0, 0, 0, 0, NULL),
(1988, 1, 0, 0, 0, 0, 0, NULL),
(1989, 1, 0, 0, 0, 0, 0, NULL),
(1994, 1, 0, 0, 0, 0, 0, NULL),
(1995, 0, 0, 1, 0, 0, 0, NULL),
(1996, 0, 0, 1, 0, 0, 0, NULL),
(1998, 1, 0, 0, 0, 0, 0, NULL),
(1999, 1, 0, 0, 0, 0, 0, NULL),
(2000, 1, 0, 0, 0, 0, 0, NULL),
(2001, 1, 0, 0, 0, 0, 0, NULL),
(2031, 1, 0, 0, 0, 0, 0, NULL),
(2032, 1, 0, 0, 0, 0, 0, NULL),
(2033, 1, 0, 0, 0, 0, 0, NULL),
(2034, 1, 0, 0, 0, 0, 0, NULL),
(2041, 1, 0, 0, 0, 0, 0, NULL),
(2042, 1, 0, 0, 0, 0, 0, NULL),
(2043, 1, 0, 0, 0, 0, 0, NULL),
(2052, 1, 0, 0, 0, 0, 0, NULL),
(2069, 1, 0, 0, 0, 0, 0, NULL),
(2070, 1, 0, 0, 0, 0, 0, NULL),
(2071, 1, 0, 0, 0, 0, 0, NULL),
(2084, 1, 1, 0, 0, 0, 0, NULL),
(2087, 1, 1, 0, 0, 0, 0, NULL),
(2098, 1, 0, 0, 0, 0, 0, NULL),
(2110, 1, 0, 0, 0, 0, 0, NULL),
(2133, 1, 1, 0, 0, 0, 0, NULL),
(2163, 1, 1, 0, 0, 0, 0, NULL),
(2164, 1, 0, 0, 0, 0, 0, NULL),
(2165, 1, 0, 0, 0, 0, 0, NULL),
(2173, 0, 1, 0, 0, 0, 0, NULL),
(2174, 0, 1, 0, 0, 0, 0, NULL),
(2175, 1, 0, 0, 0, 0, 0, NULL),
(2185, 0, 1, 0, 0, 0, 0, NULL),
(2187, 0, 1, 0, 0, 0, 0, NULL),
(2188, 0, 1, 0, 0, 0, 0, NULL),
(2197, 1, 1, 0, 0, 0, 0, NULL),
(2199, 1, 1, 0, 0, 0, 0, NULL),
(2200, 1, 1, 0, 0, 0, 0, NULL),
(2220, 1, 0, 0, 0, 0, 0, NULL),
(2223, 1, 1, 0, 0, 0, 0, NULL),
(2237, 1, 0, 0, 0, 0, 0, NULL),
(2275, 1, 0, 0, 0, 0, 0, NULL),
(2293, 1, 1, 0, 0, 0, 0, NULL),
(2295, 1, 1, 0, 0, 0, 0, NULL),
(2296, 1, 1, 0, 0, 0, 0, NULL),
(2297, 1, 1, 0, 0, 0, 0, NULL),
(2300, 1, 1, 0, 0, 0, 0, NULL),
(2301, 1, 1, 0, 0, 0, 0, NULL),
(2303, 1, 1, 0, 0, 0, 0, NULL),
(2312, 1, 1, 0, 0, 0, 0, NULL),
(2313, 1, 1, 0, 0, 0, 0, NULL),
(2334, 0, 0, 1, 0, 0, 0, NULL),
(2348, 1, 0, 0, 0, 0, 0, NULL),
(2349, 1, 1, 0, 0, 0, 0, NULL),
(2350, 1, 0, 0, 0, 0, 0, NULL),
(2351, 1, 1, 0, 0, 0, 0, NULL),
(2354, 1, 0, 0, 0, 0, 0, NULL),
(2356, 1, 0, 0, 0, 0, 0, NULL),
(2384, 1, 0, 0, 0, 0, 0, NULL),
(2385, 1, 1, 0, 0, 0, 0, NULL),
(2406, 1, 0, 0, 0, 0, 0, NULL),
(2407, 1, 0, 0, 0, 0, 0, NULL),
(2441, 1, 1, 0, 0, 0, 0, NULL),
(2442, 1, 0, 0, 0, 0, 0, NULL),
(2522, 1, 0, 0, 0, 0, 0, NULL),
(2559, 1, 0, 0, 0, 0, 0, NULL),
(2560, 1, 0, 0, 0, 0, 0, NULL),
(2561, 1, 0, 0, 0, 0, 0, NULL),
(2563, 1, 0, 0, 0, 0, 0, NULL),
(2565, 1, 0, 0, 0, 0, 0, NULL),
(2614, 0, 1, 1, 0, 0, 0, NULL),
(2615, 0, 1, 1, 0, 0, 0, NULL),
(2617, 1, 1, 0, 0, 0, 0, NULL),
(2620, 1, 0, 0, 0, 0, 0, NULL),
(2630, 1, 0, 0, 0, 0, 0, NULL),
(2667, 1, 1, 0, 1, 0, 0, NULL),
(2673, 1, 0, 0, 1, 0, 0, NULL),
(2674, 1, 0, 0, 1, 0, 0, NULL),
(2676, 1, 0, 0, 0, 0, 0, NULL),
(2680, 1, 0, 0, 0, 0, 0, NULL),
(2681, 1, 0, 0, 0, 0, 0, NULL),
(2686, 1, 0, 0, 0, 0, 0, NULL),
(2727, 1, 0, 0, 0, 0, 0, NULL),
(2728, 1, 0, 0, 0, 0, 0, NULL),
(2729, 1, 0, 0, 0, 0, 0, NULL),
(2730, 1, 0, 0, 0, 0, 0, NULL),
(2731, 1, 0, 0, 0, 0, 0, NULL),
(2732, 1, 0, 0, 0, 0, 0, NULL),
(2734, 1, 0, 0, 0, 0, 0, NULL),
(2753, 1, 1, 0, 0, 0, 0, NULL),
(2756, 1, 1, 0, 0, 0, 0, NULL),
(2798, 1, 1, 0, 0, 0, 0, NULL),
(2813, 1, 1, 0, 0, 0, 0, NULL),
(2815, 1, 1, 0, 0, 0, 0, NULL),
(2850, 1, 1, 0, 0, 0, 0, NULL),
(2853, 0, 0, 1, 0, 0, 0, NULL),
(2872, 1, 1, 0, 0, 0, 0, NULL),
(2883, 1, 1, 0, 0, 0, 0, NULL),
(2885, 1, 1, 0, 0, 0, 0, NULL),
(2923, 1, 0, 0, 0, 0, 0, NULL),
(2924, 1, 0, 0, 0, 0, 0, NULL),
(2925, 1, 0, 0, 0, 0, 0, NULL),
(2926, 1, 0, 0, 0, 0, 0, NULL),
(2940, 1, 1, 0, 0, 0, 0, NULL),
(2947, 1, 1, 0, 0, 0, 0, NULL),
(2948, 1, 1, 0, 0, 0, 0, NULL),
(2949, 1, 1, 0, 0, 0, 0, NULL),
(2950, 1, 1, 0, 0, 0, 0, NULL),
(2951, 1, 1, 0, 0, 0, 0, NULL),
(2952, 1, 1, 0, 0, 0, 0, NULL),
(2953, 1, 1, 0, 0, 0, 0, NULL),
(2954, 1, 0, 0, 0, 0, 0, NULL),
(2955, 1, 1, 0, 0, 0, 0, NULL),
(2956, 1, 1, 0, 0, 0, 0, NULL),
(2957, 1, 1, 0, 0, 0, 0, NULL),
(2958, 1, 1, 0, 0, 0, 0, NULL),
(2959, 1, 1, 0, 0, 0, 0, NULL),
(2960, 1, 1, 0, 0, 0, 0, NULL),
(2961, 1, 0, 0, 0, 0, 0, NULL),
(2962, 1, 1, 0, 0, 0, 0, NULL),
(2963, 1, 1, 0, 0, 0, 0, NULL),
(2964, 1, 1, 0, 0, 0, 0, NULL),
(2965, 1, 1, 0, 0, 0, 0, NULL),
(2966, 1, 1, 0, 0, 0, 0, NULL),
(2967, 1, 1, 0, 0, 0, 0, NULL),
(2968, 1, 1, 0, 0, 0, 0, NULL),
(2969, 1, 1, 0, 0, 0, 0, NULL),
(2970, 1, 1, 0, 0, 0, 0, NULL),
(2971, 1, 1, 0, 0, 0, 0, NULL),
(2972, 1, 0, 0, 0, 0, 0, NULL),
(2973, 1, 0, 0, 0, 0, 0, NULL),
(2974, 1, 0, 0, 0, 0, 0, NULL),
(2975, 1, 1, 0, 0, 0, 0, NULL),
(2976, 1, 1, 0, 0, 0, 0, NULL),
(2977, 1, 1, 0, 0, 0, 0, NULL),
(2978, 1, 1, 0, 0, 0, 0, NULL),
(2979, 1, 1, 0, 0, 0, 0, NULL),
(2980, 1, 1, 0, 0, 0, 0, NULL),
(2981, 1, 1, 0, 0, 0, 0, NULL),
(2982, 1, 1, 0, 0, 0, 0, NULL),
(2984, 1, 1, 0, 0, 0, 0, NULL),
(2985, 1, 1, 0, 0, 0, 0, NULL),
(2986, 1, 1, 0, 0, 0, 0, NULL),
(2987, 1, 1, 0, 0, 0, 0, NULL),
(2988, 1, 1, 0, 0, 0, 0, NULL),
(2989, 1, 1, 0, 0, 0, 0, NULL),
(2990, 1, 1, 0, 0, 0, 0, NULL),
(2991, 1, 1, 0, 0, 0, 0, NULL),
(2993, 1, 1, 0, 0, 0, 0, NULL),
(2994, 1, 1, 0, 0, 0, 0, NULL),
(2995, 1, 1, 0, 0, 0, 0, NULL),
(2996, 1, 1, 0, 0, 0, 0, NULL),
(2997, 1, 1, 0, 0, 0, 0, NULL),
(2998, 1, 1, 0, 0, 0, 0, NULL),
(2999, 1, 1, 0, 0, 0, 0, NULL),
(3001, 1, 1, 0, 0, 0, 0, NULL),
(3002, 1, 1, 0, 0, 0, 0, NULL),
(3003, 1, 1, 0, 0, 0, 0, NULL),
(3004, 1, 1, 0, 0, 0, 0, NULL),
(3005, 1, 1, 0, 0, 0, 0, NULL),
(3007, 1, 1, 0, 0, 0, 0, NULL),
(3008, 1, 1, 0, 0, 0, 0, NULL),
(3009, 1, 1, 0, 0, 0, 0, NULL),
(3010, 1, 1, 0, 0, 0, 0, NULL),
(3011, 1, 1, 0, 0, 0, 0, NULL),
(3012, 1, 1, 0, 0, 0, 0, NULL),
(3013, 1, 1, 0, 0, 0, 0, NULL),
(3014, 1, 1, 0, 0, 0, 0, NULL),
(3015, 1, 1, 0, 0, 0, 0, NULL),
(3016, 1, 1, 0, 0, 0, 0, NULL),
(3017, 1, 1, 0, 0, 0, 0, NULL),
(3018, 1, 1, 0, 0, 0, 0, NULL),
(3019, 1, 1, 0, 0, 0, 0, NULL),
(3020, 1, 1, 0, 0, 0, 0, NULL),
(3021, 1, 1, 0, 0, 0, 0, NULL),
(3022, 1, 1, 0, 0, 0, 0, NULL),
(3023, 1, 1, 0, 0, 0, 0, NULL),
(3024, 1, 1, 0, 0, 0, 0, NULL),
(3025, 1, 1, 0, 0, 0, 0, NULL),
(3026, 1, 1, 0, 0, 0, 0, NULL),
(3027, 1, 1, 0, 0, 0, 0, NULL),
(3028, 1, 1, 0, 0, 0, 0, NULL),
(3029, 1, 1, 0, 0, 0, 0, NULL),
(3030, 1, 1, 0, 0, 0, 0, NULL),
(3031, 1, 1, 0, 0, 0, 0, NULL),
(3032, 1, 1, 0, 0, 0, 0, NULL),
(3033, 1, 1, 0, 0, 0, 0, NULL),
(3034, 1, 1, 0, 0, 0, 0, NULL),
(3035, 1, 1, 0, 0, 0, 0, NULL),
(3036, 1, 1, 0, 0, 0, 0, NULL),
(3037, 1, 1, 0, 0, 0, 0, NULL),
(3038, 1, 1, 0, 0, 0, 0, NULL),
(3039, 1, 1, 0, 0, 0, 0, NULL),
(3040, 1, 1, 0, 0, 0, 0, NULL),
(3041, 1, 1, 0, 0, 0, 0, NULL),
(3042, 1, 1, 0, 0, 0, 0, NULL),
(3043, 1, 1, 0, 0, 0, 0, NULL),
(3044, 1, 1, 0, 0, 0, 0, NULL),
(3045, 1, 1, 0, 0, 0, 0, NULL),
(3046, 1, 1, 0, 0, 0, 0, NULL),
(3047, 1, 1, 0, 0, 0, 0, NULL),
(3048, 1, 1, 0, 0, 0, 0, NULL),
(3049, 1, 1, 0, 0, 0, 0, NULL),
(3050, 1, 1, 0, 0, 0, 0, NULL),
(3051, 1, 1, 0, 0, 0, 0, NULL),
(3052, 1, 1, 0, 0, 0, 0, NULL),
(3053, 1, 1, 0, 0, 0, 0, NULL),
(3054, 1, 1, 0, 0, 0, 0, NULL),
(3055, 1, 1, 0, 0, 0, 0, NULL),
(3057, 1, 1, 0, 0, 0, 0, NULL),
(3058, 1, 1, 0, 0, 0, 0, NULL),
(3059, 1, 1, 0, 0, 0, 0, NULL),
(3060, 1, 1, 0, 0, 0, 0, NULL),
(3061, 1, 1, 0, 0, 0, 0, NULL),
(3062, 1, 1, 0, 0, 0, 0, NULL),
(3063, 1, 1, 0, 0, 0, 0, NULL),
(3064, 1, 1, 0, 0, 0, 0, NULL),
(3065, 1, 1, 0, 0, 0, 0, NULL),
(3066, 1, 1, 0, 0, 0, 0, NULL),
(3067, 1, 1, 0, 0, 0, 0, NULL),
(3068, 1, 1, 0, 0, 0, 0, NULL),
(3069, 1, 1, 0, 0, 0, 0, NULL),
(3070, 1, 1, 0, 0, 0, 0, NULL),
(3071, 1, 1, 0, 0, 0, 0, NULL),
(3072, 1, 1, 0, 0, 0, 0, NULL),
(3073, 1, 1, 0, 0, 0, 0, NULL),
(3074, 1, 1, 0, 0, 0, 0, NULL),
(3075, 1, 1, 0, 0, 0, 0, NULL),
(3076, 1, 1, 0, 0, 0, 0, NULL),
(3077, 1, 1, 0, 0, 0, 0, NULL),
(3078, 1, 1, 0, 0, 0, 0, NULL),
(3079, 1, 1, 0, 0, 0, 0, NULL),
(3080, 1, 1, 0, 0, 0, 0, NULL),
(3081, 1, 1, 0, 0, 0, 0, NULL),
(3082, 1, 1, 0, 0, 0, 0, NULL),
(3083, 1, 1, 0, 0, 0, 0, NULL),
(3084, 1, 1, 0, 0, 0, 0, NULL),
(3092, 1, 1, 0, 0, 0, 0, NULL),
(3093, 1, 1, 0, 0, 0, 0, NULL),
(3095, 1, 1, 0, 0, 0, 0, NULL),
(3098, 1, 1, 0, 0, 0, 0, NULL),
(3099, 1, 1, 0, 0, 0, 0, NULL),
(3100, 1, 1, 0, 0, 0, 0, NULL),
(3102, 1, 1, 0, 0, 0, 0, NULL),
(3121, 1, 0, 0, 0, 0, 0, NULL),
(3122, 1, 1, 0, 0, 0, 0, NULL),
(3123, 1, 1, 0, 0, 0, 0, NULL),
(3148, 1, 1, 0, 0, 0, 0, NULL),
(3176, 1, 1, 0, 0, 0, 0, NULL),
(3202, 1, 1, 0, 0, 0, 0, NULL),
(3209, 1, 1, 0, 0, 0, 0, NULL),
(3210, 1, 1, 0, 0, 0, 0, NULL),
(3211, 1, 1, 0, 0, 0, 0, NULL),
(3212, 1, 1, 0, 0, 0, 0, NULL),
(3213, 1, 1, 0, 0, 0, 0, NULL),
(3214, 1, 1, 0, 0, 0, 0, NULL),
(3215, 1, 1, 0, 0, 0, 0, NULL),
(3217, 1, 1, 0, 0, 0, 0, NULL),
(3218, 1, 1, 0, 0, 0, 0, NULL),
(3219, 1, 1, 0, 0, 0, 0, NULL),
(3220, 1, 1, 0, 0, 0, 0, NULL),
(3221, 1, 1, 0, 0, 0, 0, NULL),
(3222, 1, 1, 0, 0, 0, 0, NULL),
(3223, 1, 1, 0, 0, 0, 0, NULL),
(3224, 1, 1, 0, 0, 0, 0, NULL),
(3225, 1, 0, 0, 0, 0, 0, NULL),
(3227, 1, 0, 0, 0, 0, 0, NULL),
(3229, 1, 1, 0, 0, 0, 0, NULL),
(3231, 1, 1, 0, 0, 0, 0, NULL),
(3232, 1, 1, 0, 0, 0, 0, NULL),
(3233, 1, 1, 0, 0, 0, 0, NULL),
(3234, 1, 0, 0, 0, 0, 0, NULL),
(3235, 1, 0, 0, 0, 0, 0, NULL),
(3236, 1, 0, 0, 0, 0, 0, NULL),
(3237, 1, 0, 0, 0, 0, 0, NULL),
(3238, 1, 0, 0, 0, 0, 0, NULL),
(3239, 1, 0, 0, 0, 0, 0, NULL),
(3240, 1, 0, 0, 0, 0, 0, NULL),
(3241, 1, 0, 0, 0, 0, 0, NULL),
(3242, 1, 0, 0, 0, 0, 0, NULL),
(3243, 1, 0, 0, 0, 0, 0, NULL),
(3244, 1, 0, 0, 0, 0, 0, NULL),
(3245, 1, 0, 0, 0, 0, 0, NULL),
(3246, 1, 0, 0, 0, 0, 0, NULL),
(3247, 1, 0, 0, 0, 0, 0, NULL),
(3248, 1, 0, 0, 0, 0, 0, NULL),
(3249, 1, 0, 0, 0, 0, 0, NULL),
(3250, 1, 0, 0, 0, 0, 0, NULL),
(3251, 1, 0, 0, 0, 0, 0, NULL),
(3252, 1, 0, 0, 0, 0, 0, NULL),
(3254, 1, 0, 0, 0, 0, 0, NULL),
(3255, 1, 0, 0, 0, 0, 0, NULL),
(3256, 1, 0, 0, 0, 0, 0, NULL),
(3257, 1, 0, 0, 0, 0, 0, NULL),
(3258, 1, 1, 0, 0, 0, 0, NULL),
(3260, 1, 1, 0, 0, 0, 0, NULL),
(3261, 1, 1, 0, 0, 0, 0, NULL),
(3263, 1, 1, 0, 0, 0, 0, NULL),
(3265, 1, 1, 0, 0, 0, 0, NULL),
(3266, 1, 1, 0, 0, 0, 0, NULL),
(3267, 1, 1, 0, 0, 0, 0, NULL),
(3268, 1, 1, 0, 0, 0, 0, NULL),
(3269, 1, 1, 0, 0, 0, 0, NULL),
(3270, 1, 1, 0, 0, 0, 0, NULL),
(3271, 1, 1, 0, 0, 0, 0, NULL),
(3272, 1, 1, 0, 0, 0, 0, NULL),
(3273, 1, 1, 0, 0, 0, 0, NULL),
(3274, 1, 1, 0, 0, 0, 0, NULL),
(3275, 1, 1, 0, 0, 0, 0, NULL),
(3276, 1, 1, 0, 0, 0, 0, NULL),
(3277, 1, 1, 0, 0, 0, 0, NULL),
(3278, 1, 1, 0, 0, 0, 0, NULL),
(3279, 1, 1, 0, 0, 0, 0, NULL),
(3280, 1, 1, 0, 0, 0, 0, NULL),
(3282, 1, 1, 0, 0, 0, 0, NULL),
(3283, 1, 1, 0, 0, 0, 0, NULL),
(3284, 1, 1, 0, 0, 0, 0, NULL),
(3285, 1, 1, 0, 0, 0, 0, NULL),
(3286, 1, 1, 0, 0, 0, 0, NULL),
(3292, 1, 1, 0, 0, 0, 0, NULL),
(3296, 1, 1, 0, 0, 0, 0, NULL),
(3299, 1, 1, 0, 0, 0, 0, NULL),
(3300, 1, 1, 0, 0, 0, 0, NULL),
(3302, 1, 1, 0, 0, 0, 0, NULL),
(3303, 1, 1, 0, 0, 0, 0, NULL),
(3307, 1, 1, 0, 0, 0, 0, NULL),
(3337, 1, 1, 0, 0, 0, 0, NULL),
(3338, 1, 1, 0, 0, 0, 0, NULL),
(3339, 1, 1, 0, 0, 0, 0, NULL),
(3341, 1, 1, 0, 0, 0, 0, NULL),
(3374, 1, 1, 0, 0, 0, 0, NULL),
(3375, 1, 1, 0, 0, 0, 0, NULL),
(3376, 1, 1, 0, 0, 0, 0, NULL),
(3377, 1, 1, 0, 0, 0, 0, NULL),
(3378, 1, 1, 0, 0, 0, 0, NULL),
(3379, 1, 1, 0, 0, 0, 0, NULL),
(3380, 1, 1, 0, 0, 0, 0, NULL),
(3381, 1, 1, 0, 0, 0, 0, NULL),
(3382, 1, 1, 0, 0, 0, 0, NULL),
(3383, 1, 1, 0, 0, 0, 0, NULL),
(3384, 1, 1, 0, 0, 0, 0, NULL),
(3385, 1, 1, 0, 0, 0, 0, NULL),
(3386, 1, 1, 0, 0, 0, 0, NULL),
(3387, 1, 1, 0, 0, 0, 0, NULL),
(3388, 1, 1, 0, 0, 0, 0, NULL),
(3389, 1, 1, 0, 0, 0, 0, NULL),
(3390, 1, 1, 0, 0, 0, 0, NULL),
(3391, 1, 1, 0, 0, 0, 0, NULL),
(3392, 1, 1, 0, 0, 0, 0, NULL),
(3393, 1, 1, 0, 0, 0, 0, NULL),
(3394, 1, 1, 0, 0, 0, 0, NULL),
(3396, 1, 1, 0, 0, 0, 0, NULL),
(3397, 1, 1, 0, 0, 0, 0, NULL),
(3411, 1, 1, 0, 0, 0, 0, NULL),
(3414, 1, 1, 0, 0, 0, 0, NULL),
(3415, 1, 0, 0, 0, 0, 0, NULL),
(3416, 1, 0, 0, 0, 0, 0, NULL),
(3418, 1, 1, 0, 0, 0, 0, NULL),
(3419, 1, 1, 0, 0, 0, 0, NULL),
(3420, 1, 1, 0, 0, 0, 0, NULL),
(3421, 1, 1, 0, 0, 0, 0, NULL),
(3424, 1, 0, 0, 0, 0, 0, NULL),
(3425, 1, 0, 0, 0, 0, 0, NULL),
(3426, 1, 0, 0, 0, 0, 0, NULL),
(3427, 1, 1, 0, 0, 0, 0, NULL),
(3428, 1, 1, 0, 0, 0, 0, NULL),
(3429, 1, 1, 0, 0, 0, 0, NULL),
(3430, 1, 1, 0, 0, 0, 0, NULL),
(3431, 1, 1, 0, 0, 0, 0, NULL),
(3432, 1, 1, 0, 0, 0, 0, NULL),
(3433, 1, 1, 0, 0, 0, 0, NULL),
(3434, 1, 1, 0, 0, 0, 0, NULL),
(3435, 1, 1, 0, 0, 0, 0, NULL),
(3436, 1, 1, 0, 0, 0, 0, NULL),
(3438, 1, 1, 0, 0, 0, 0, NULL),
(3439, 1, 1, 0, 0, 0, 0, NULL),
(3440, 1, 1, 0, 0, 0, 0, NULL),
(3441, 1, 1, 0, 0, 0, 0, NULL),
(3442, 1, 1, 0, 0, 0, 0, NULL),
(3443, 1, 1, 0, 0, 0, 0, NULL),
(3444, 1, 0, 0, 0, 0, 0, NULL),
(3445, 1, 1, 0, 0, 0, 0, NULL),
(3446, 1, 1, 0, 0, 0, 0, NULL),
(3447, 1, 1, 0, 0, 0, 0, NULL),
(3448, 1, 1, 0, 0, 0, 0, NULL),
(3449, 1, 1, 0, 0, 0, 0, NULL),
(3452, 1, 1, 0, 0, 0, 0, NULL),
(3453, 1, 1, 0, 0, 0, 0, NULL),
(3456, 1, 1, 0, 0, 0, 0, NULL),
(3457, 1, 1, 0, 0, 0, 0, NULL),
(3458, 1, 1, 0, 0, 0, 0, NULL),
(3459, 1, 1, 0, 0, 0, 0, NULL),
(3463, 1, 0, 0, 0, 0, 0, NULL),
(3464, 1, 1, 0, 0, 0, 0, NULL),
(3465, 1, 1, 0, 0, 0, 0, NULL),
(3466, 1, 0, 0, 0, 0, 0, NULL),
(3467, 1, 1, 0, 0, 0, 0, NULL),
(3470, 1, 1, 0, 0, 0, 0, NULL),
(3471, 1, 1, 0, 0, 0, 0, NULL),
(3472, 1, 0, 0, 0, 0, 0, NULL),
(3473, 1, 0, 0, 0, 0, 0, NULL),
(3474, 1, 0, 0, 0, 0, 0, NULL),
(3475, 1, 0, 0, 0, 0, 0, NULL),
(3476, 0, 1, 0, 0, 0, 0, NULL),
(3477, 1, 1, 0, 0, 0, 0, NULL),
(3478, 1, 1, 0, 0, 0, 0, NULL),
(3479, 1, 1, 0, 0, 0, 0, NULL),
(3480, 1, 1, 0, 0, 0, 0, NULL),
(3481, 1, 1, 0, 0, 0, 0, NULL),
(3482, 1, 1, 0, 0, 0, 0, NULL),
(3483, 1, 1, 0, 0, 0, 0, NULL),
(3484, 1, 1, 0, 0, 0, 0, NULL),
(3485, 1, 1, 0, 0, 0, 0, NULL),
(3486, 1, 1, 0, 0, 0, 0, NULL),
(3487, 1, 1, 0, 0, 0, 0, NULL),
(3488, 1, 1, 0, 0, 0, 0, NULL),
(3489, 1, 1, 0, 0, 0, 0, NULL),
(3490, 1, 1, 0, 0, 0, 0, NULL),
(3491, 1, 1, 0, 0, 0, 0, NULL),
(3492, 1, 1, 0, 0, 0, 0, NULL),
(3493, 1, 1, 0, 0, 0, 0, NULL),
(3494, 1, 1, 0, 0, 0, 0, NULL),
(3495, 1, 1, 0, 0, 0, 0, NULL),
(3496, 1, 1, 0, 0, 0, 0, NULL),
(3497, 1, 1, 0, 0, 0, 0, NULL),
(3498, 1, 1, 0, 0, 0, 0, NULL),
(3499, 1, 1, 0, 0, 0, 0, NULL),
(3501, 1, 1, 0, 0, 0, 0, NULL),
(3502, 1, 1, 0, 0, 0, 0, NULL),
(3503, 1, 0, 0, 0, 0, 0, NULL),
(3521, 1, 1, 0, 0, 0, 0, NULL),
(3525, 1, 1, 0, 0, 0, 0, NULL),
(3527, 1, 0, 0, 0, 0, 0, NULL),
(3558, 1, 0, 0, 0, 0, 0, NULL),
(3560, 1, 0, 0, 0, 0, 0, NULL),
(3565, 1, 0, 0, 0, 0, 0, NULL),
(3566, 1, 1, 0, 0, 0, 0, NULL),
(3572, 1, 1, 0, 0, 0, 0, NULL),
(3615, 1, 1, 0, 0, 0, 0, NULL),
(3619, 1, 0, 0, 0, 0, 0, NULL),
(3623, 1, 1, 0, 0, 0, 0, NULL),
(3630, 1, 0, 0, 0, 0, 0, NULL),
(3631, 1, 0, 0, 0, 0, 0, NULL),
(3632, 1, 0, 0, 0, 0, 0, NULL),
(3633, 1, 0, 0, 0, 0, 0, NULL),
(3634, 1, 0, 0, 0, 0, 0, NULL),
(3636, 1, 1, 0, 0, 0, 0, NULL),
(3637, 1, 1, 0, 0, 0, 0, NULL),
(3638, 1, 0, 0, 0, 0, 0, NULL),
(3641, 0, 1, 0, 0, 0, 0, NULL),
(3642, 0, 1, 0, 0, 0, 0, NULL),
(3651, 1, 1, 0, 0, 0, 0, NULL),
(3655, 1, 1, 0, 0, 0, 0, NULL),
(3658, 1, 1, 0, 0, 0, 0, NULL),
(3659, 1, 1, 0, 0, 0, 0, NULL),
(3665, 1, 1, 0, 0, 0, 0, NULL),
(3681, 0, 0, 1, 0, 0, 0, NULL),
(3682, 1, 1, 0, 0, 0, 0, NULL),
(3683, 1, 1, 0, 0, 0, 0, NULL),
(3684, 1, 1, 0, 0, 0, 0, NULL),
(3685, 1, 1, 0, 0, 0, 0, NULL),
(3688, 1, 1, 0, 0, 0, 0, NULL),
(3689, 1, 1, 0, 0, 0, 0, NULL),
(3690, 1, 1, 0, 0, 0, 0, NULL),
(3703, 1, 1, 0, 0, 0, 0, NULL),
(3704, 1, 1, 0, 0, 0, 0, NULL),
(3705, 1, 1, 0, 0, 0, 0, NULL),
(3718, 1, 1, 0, 0, 0, 0, NULL),
(3774, 1, 1, 0, 0, 0, 0, NULL),
(3779, 1, 1, 0, 0, 0, 0, NULL),
(3809, 1, 1, 0, 0, 0, 0, NULL),
(3810, 1, 1, 0, 0, 0, 0, NULL),
(3811, 1, 0, 0, 0, 0, 0, NULL),
(3819, 1, 1, 0, 0, 0, 0, NULL),
(3820, 1, 1, 0, 0, 0, 0, NULL),
(3821, 1, 1, 0, 0, 0, 0, NULL),
(3823, 1, 1, 0, 0, 0, 0, NULL),
(3824, 1, 0, 0, 0, 0, 0, NULL),
(3825, 1, 1, 0, 0, 0, 0, NULL),
(3831, 1, 1, 0, 0, 0, 0, NULL),
(3844, 1, 0, 0, 0, 0, 0, NULL),
(3861, 1, 1, 0, 0, 0, 0, NULL),
(3862, 1, 1, 0, 0, 0, 0, NULL),
(3869, 1, 0, 2, 0, 0, 0, NULL),
(3870, 1, 0, 2, 0, 0, 0, NULL),
(3883, 1, 1, 0, 0, 0, 0, NULL),
(3884, 1, 1, 0, 0, 0, 0, NULL),
(3911, 1, 0, 0, 0, 0, 0, NULL),
(3927, 1, 1, 0, 0, 0, 0, NULL),
(3928, 1, 1, 0, 0, 0, 0, NULL),
(3934, 1, 1, 0, 0, 0, 0, NULL),
(3939, 1, 0, 0, 0, 0, 0, NULL),
(3978, 1, 1, 0, 0, 0, 0, NULL),
(3986, 1, 1, 0, 0, 0, 0, NULL),
(3992, 1, 1, 0, 0, 0, 0, NULL),
(3993, 1, 1, 0, 0, 0, 0, NULL),
(3994, 1, 1, 0, 0, 0, 0, NULL),
(3995, 1, 1, 0, 0, 0, 0, NULL),
(4005, 1, 0, 0, 0, 0, 0, NULL),
(4006, 1, 0, 0, 0, 0, 0, NULL),
(4007, 1, 0, 0, 0, 0, 0, NULL),
(4008, 1, 0, 0, 0, 0, 0, NULL),
(4009, 1, 0, 0, 0, 0, 0, NULL),
(4012, 1, 0, 0, 0, 0, 0, NULL),
(4013, 1, 0, 0, 0, 0, 0, NULL),
(4014, 1, 0, 0, 0, 0, 0, NULL),
(4015, 1, 0, 0, 0, 0, 0, NULL),
(4016, 1, 0, 0, 0, 0, 0, NULL),
(4017, 1, 0, 0, 0, 0, 0, NULL),
(4018, 1, 0, 0, 0, 0, 0, NULL),
(4019, 1, 0, 0, 0, 0, 0, NULL),
(4020, 1, 0, 0, 0, 0, 0, NULL),
(4021, 1, 0, 0, 0, 0, 0, NULL),
(4022, 1, 1, 0, 0, 0, 0, NULL),
(4023, 1, 1, 0, 0, 0, 0, NULL),
(4024, 1, 1, 0, 0, 0, 0, NULL),
(4025, 1, 1, 0, 0, 0, 0, NULL),
(4026, 1, 1, 0, 0, 0, 0, NULL),
(4027, 1, 1, 0, 0, 0, 0, NULL),
(4028, 1, 0, 0, 0, 0, 0, NULL),
(4029, 1, 0, 0, 0, 0, 0, NULL),
(4031, 1, 0, 0, 0, 0, 0, NULL),
(4032, 1, 0, 0, 0, 0, 0, NULL),
(4034, 1, 0, 0, 0, 0, 0, NULL),
(4035, 1, 0, 0, 0, 0, 0, NULL),
(4036, 1, 0, 0, 0, 0, 0, NULL),
(4037, 1, 0, 0, 0, 0, 0, NULL),
(4038, 1, 0, 0, 0, 0, 0, NULL),
(4040, 1, 1, 0, 0, 0, 0, NULL),
(4041, 1, 0, 0, 0, 0, 0, NULL),
(4042, 1, 0, 0, 0, 0, 0, NULL),
(4044, 1, 0, 0, 0, 0, 0, NULL),
(4045, 1, 1, 0, 0, 0, 0, NULL),
(4046, 1, 1, 0, 0, 0, 0, NULL),
(4048, 1, 1, 0, 0, 0, 0, NULL),
(4050, 1, 1, 0, 0, 0, 0, NULL),
(4051, 1, 1, 0, 0, 0, 0, NULL),
(4052, 1, 1, 0, 0, 0, 0, NULL),
(4053, 1, 1, 0, 0, 0, 0, NULL),
(4056, 1, 1, 0, 0, 0, 0, NULL),
(4057, 1, 1, 0, 0, 0, 0, NULL),
(4059, 0, 0, 1, 0, 0, 0, NULL),
(4061, 1, 1, 0, 0, 0, 0, NULL),
(4066, 1, 1, 0, 0, 0, 0, NULL),
(4067, 1, 0, 0, 0, 0, 0, NULL),
(4070, 1, 1, 0, 0, 0, 0, NULL),
(4072, 1, 1, 0, 0, 0, 0, NULL),
(4073, 1, 0, 0, 0, 0, 0, NULL),
(4074, 1, 0, 0, 0, 0, 0, NULL),
(4075, 1, 0, 0, 0, 0, 0, NULL),
(4076, 1, 0, 0, 0, 0, 0, NULL),
(4077, 1, 1, 0, 0, 0, 0, NULL),
(4080, 1, 1, 0, 0, 0, 0, NULL),
(4082, 1, 1, 0, 0, 0, 0, NULL),
(4083, 1, 1, 0, 0, 0, 0, NULL),
(4084, 1, 1, 0, 0, 0, 0, NULL),
(4085, 1, 1, 0, 0, 0, 0, NULL),
(4086, 1, 1, 0, 0, 0, 0, NULL),
(4093, 1, 1, 0, 0, 0, 0, NULL),
(4094, 1, 1, 0, 0, 0, 0, NULL),
(4095, 1, 1, 0, 0, 0, 0, NULL),
(4096, 1, 1, 0, 0, 0, 0, NULL),
(4097, 1, 1, 0, 0, 0, 0, NULL),
(4099, 1, 1, 0, 0, 0, 0, NULL),
(4100, 1, 1, 0, 0, 0, 0, NULL),
(4101, 1, 1, 0, 0, 0, 0, NULL),
(4104, 1, 1, 0, 0, 0, 0, NULL),
(4107, 1, 0, 0, 0, 0, 0, NULL),
(4109, 1, 0, 0, 0, 0, 0, NULL),
(4110, 1, 0, 0, 0, 0, 0, NULL),
(4111, 1, 1, 0, 0, 0, 0, NULL),
(4112, 1, 1, 0, 0, 0, 0, NULL),
(4114, 1, 1, 0, 0, 0, 0, NULL),
(4115, 1, 1, 0, 0, 0, 0, NULL),
(4116, 1, 1, 0, 0, 0, 0, NULL),
(4117, 1, 0, 0, 0, 0, 0, NULL),
(4118, 1, 0, 0, 0, 0, 0, NULL),
(4119, 1, 0, 0, 0, 0, 0, NULL),
(4120, 1, 0, 0, 0, 0, 0, NULL),
(4124, 1, 0, 0, 0, 0, 0, NULL),
(4126, 1, 0, 0, 0, 0, 0, NULL),
(4127, 1, 0, 0, 0, 0, 0, NULL),
(4128, 1, 0, 0, 0, 0, 0, NULL),
(4129, 1, 0, 0, 0, 0, 0, NULL),
(4130, 1, 0, 0, 0, 0, 0, NULL),
(4131, 1, 0, 0, 0, 0, 0, NULL),
(4132, 1, 0, 0, 0, 0, 0, NULL),
(4133, 1, 0, 0, 0, 0, 0, NULL),
(4139, 1, 0, 0, 0, 0, 0, NULL),
(4140, 1, 0, 0, 0, 0, 0, NULL),
(4150, 1, 0, 0, 0, 0, 0, NULL),
(4151, 1, 0, 0, 0, 0, 0, NULL),
(4154, 1, 0, 0, 0, 0, 0, NULL),
(4158, 1, 0, 0, 0, 0, 0, NULL),
(4166, 1, 0, 0, 0, 0, 0, NULL),
(4188, 1, 1, 0, 0, 0, 0, NULL),
(4196, 1, 0, 0, 0, 0, 0, NULL),
(4197, 1, 1, 0, 0, 0, 0, NULL),
(4198, 1, 1, 0, 0, 0, 0, NULL),
(4201, 1, 1, 0, 0, 0, 0, NULL),
(4202, 1, 1, 0, 0, 0, 0, NULL),
(4248, 1, 0, 0, 0, 0, 0, NULL),
(4249, 1, 0, 0, 0, 0, 0, NULL),
(4250, 1, 0, 0, 0, 0, 0, NULL),
(4251, 1, 0, 0, 0, 0, 0, NULL),
(4252, 1, 0, 0, 0, 0, 0, NULL),
(4263, 1, 0, 0, 0, 0, 0, NULL),
(4264, 1, 0, 0, 0, 0, 0, NULL),
(4276, 1, 1, 0, 0, 0, 0, NULL),
(4277, 1, 0, 0, 0, 0, 0, NULL),
(4304, 1, 0, 0, 0, 0, 0, NULL),
(4309, 1, 1, 0, 0, 0, 0, NULL),
(4310, 1, 1, 0, 0, 0, 0, NULL),
(4312, 1, 1, 0, 0, 0, 0, NULL),
(4313, 1, 1, 0, 0, 0, 0, NULL),
(4315, 1, 1, 0, 0, 0, 0, NULL),
(4316, 1, 0, 0, 0, 0, 0, NULL),
(4317, 1, 1, 0, 0, 0, 0, NULL),
(4318, 1, 1, 0, 0, 0, 0, NULL),
(4319, 1, 1, 0, 0, 0, 0, NULL),
(4351, 1, 0, 0, 0, 0, 0, NULL),
(4352, 1, 0, 0, 0, 0, 0, NULL),
(4355, 1, 0, 0, 0, 0, 0, NULL),
(4356, 1, 0, 0, 0, 0, 0, NULL),
(4357, 1, 0, 0, 0, 0, 0, NULL),
(4376, 1, 0, 0, 0, 0, 0, NULL),
(4377, 1, 0, 0, 0, 0, 0, NULL),
(4378, 1, 0, 0, 0, 0, 0, NULL),
(4379, 1, 0, 0, 0, 0, 0, NULL),
(4380, 1, 0, 0, 0, 0, 0, NULL),
(4388, 0, 1, 0, 0, 0, 0, NULL),
(4389, 0, 1, 0, 0, 0, 0, NULL),
(4390, 0, 1, 0, 0, 0, 0, NULL),
(4407, 1, 1, 0, 0, 0, 0, NULL),
(4409, 1, 1, 0, 0, 0, 0, NULL),
(4411, 1, 0, 0, 0, 0, 0, NULL),
(4412, 1, 0, 0, 0, 0, 0, NULL),
(4413, 1, 0, 0, 0, 0, 0, NULL),
(4414, 1, 0, 0, 0, 0, 0, NULL),
(4415, 1, 0, 0, 0, 0, 0, NULL),
(4419, 1, 1, 0, 0, 0, 0, NULL),
(4423, 1, 0, 0, 0, 0, 0, NULL),
(4429, 1, 1, 0, 0, 0, 0, NULL),
(4430, 1, 1, 0, 0, 0, 0, NULL),
(4439, 1, 1, 0, 0, 0, 0, NULL),
(4451, 1, 1, 0, 0, 0, 0, NULL),
(4452, 1, 1, 0, 0, 0, 0, NULL),
(4453, 1, 1, 0, 0, 0, 0, NULL),
(4454, 1, 1, 0, 0, 0, 0, NULL),
(4483, 1, 1, 0, 0, 0, 0, NULL),
(4489, 1, 1, 0, 0, 0, 0, NULL),
(4495, 1, 1, 0, 0, 0, 0, NULL),
(4496, 1, 1, 0, 0, 0, 0, NULL),
(4498, 1, 1, 0, 0, 0, 0, NULL),
(4499, 1, 0, 0, 0, 0, 0, NULL),
(4507, 1, 1, 0, 0, 0, 0, NULL),
(4511, 1, 1, 0, 0, 0, 0, NULL),
(4512, 1, 0, 0, 0, 0, 0, NULL),
(4514, 1, 0, 0, 0, 0, 0, NULL),
(4534, 1, 1, 0, 0, 0, 0, NULL),
(4535, 1, 1, 0, 0, 0, 0, NULL),
(4545, 1, 1, 0, 0, 0, 0, NULL),
(4546, 1, 1, 0, 0, 0, 0, NULL),
(4547, 1, 1, 0, 0, 0, 0, NULL),
(4548, 1, 0, 0, 0, 0, 0, NULL),
(4579, 1, 1, 0, 0, 0, 0, NULL),
(4620, 1, 1, 0, 0, 0, 0, NULL),
(4627, 1, 1, 0, 0, 0, 0, NULL),
(4629, 1, 1, 0, 0, 0, 0, NULL),
(4630, 1, 1, 0, 0, 0, 0, NULL),
(4632, 1, 1, 0, 0, 0, 0, NULL),
(4633, 1, 1, 0, 0, 0, 0, NULL),
(4634, 1, 1, 0, 0, 0, 0, NULL),
(4635, 1, 1, 0, 0, 0, 0, NULL),
(4636, 1, 1, 0, 0, 0, 0, NULL),
(4637, 1, 1, 0, 0, 0, 0, NULL),
(4638, 1, 1, 0, 0, 0, 0, NULL),
(4639, 1, 1, 0, 0, 0, 0, NULL),
(4640, 1, 1, 0, 0, 0, 0, NULL),
(4641, 1, 1, 0, 0, 0, 0, NULL),
(4642, 1, 1, 0, 0, 0, 0, NULL),
(4643, 1, 1, 0, 0, 0, 0, NULL),
(4644, 1, 1, 0, 0, 0, 0, NULL),
(4645, 1, 1, 0, 0, 0, 0, NULL),
(4646, 1, 1, 0, 0, 0, 0, NULL),
(4647, 1, 1, 0, 0, 0, 0, NULL),
(4648, 1, 1, 0, 0, 0, 0, NULL),
(4649, 1, 1, 0, 0, 0, 0, NULL),
(4651, 1, 1, 0, 0, 0, 0, NULL),
(4652, 1, 1, 0, 0, 0, 0, NULL),
(4653, 1, 1, 0, 0, 0, 0, NULL),
(4654, 1, 1, 0, 0, 0, 0, NULL),
(4655, 1, 1, 0, 0, 0, 0, NULL),
(4656, 1, 1, 0, 0, 0, 0, NULL),
(4657, 1, 1, 0, 0, 0, 0, NULL),
(4658, 1, 1, 0, 0, 0, 0, NULL),
(4659, 1, 1, 0, 0, 0, 0, NULL),
(4660, 1, 0, 0, 0, 0, 0, NULL),
(4661, 1, 0, 0, 0, 0, 0, NULL),
(4662, 1, 0, 0, 0, 0, 0, NULL),
(4663, 1, 1, 0, 0, 0, 0, NULL),
(4664, 1, 1, 0, 0, 0, 0, NULL),
(4665, 1, 1, 0, 0, 0, 0, NULL),
(4666, 1, 1, 0, 0, 0, 0, NULL),
(4667, 1, 1, 0, 0, 0, 0, NULL),
(4668, 1, 1, 0, 0, 0, 0, NULL),
(4670, 1, 0, 0, 0, 0, 0, NULL),
(4671, 1, 0, 0, 0, 0, 0, NULL),
(4672, 1, 0, 0, 0, 0, 0, NULL),
(4673, 1, 0, 0, 0, 0, 0, NULL),
(4674, 1, 0, 0, 0, 0, 0, NULL),
(4675, 1, 0, 0, 0, 0, 0, NULL),
(4676, 1, 0, 0, 0, 0, 0, NULL),
(4677, 1, 1, 0, 0, 0, 0, NULL),
(4678, 1, 0, 0, 0, 0, 0, NULL),
(4679, 1, 1, 0, 0, 0, 0, NULL),
(4680, 1, 1, 0, 0, 0, 0, NULL),
(4681, 1, 0, 0, 0, 0, 0, NULL),
(4682, 1, 1, 0, 0, 0, 0, NULL),
(4684, 1, 1, 0, 0, 0, 0, NULL),
(4685, 1, 0, 0, 0, 0, 0, NULL),
(4686, 1, 1, 0, 0, 0, 0, NULL),
(4687, 1, 1, 0, 0, 0, 0, NULL),
(4688, 1, 0, 0, 0, 0, 0, NULL),
(4689, 1, 0, 0, 0, 0, 0, NULL),
(4690, 1, 0, 0, 0, 0, 0, NULL),
(4692, 1, 0, 0, 0, 0, 0, NULL),
(4693, 1, 0, 0, 0, 0, 0, NULL),
(4694, 1, 0, 0, 0, 0, 0, NULL),
(4695, 1, 0, 0, 0, 0, 0, NULL),
(4696, 1, 1, 0, 0, 0, 0, NULL),
(4697, 1, 0, 0, 0, 0, 0, NULL),
(4699, 1, 0, 0, 0, 0, 0, NULL),
(4700, 1, 0, 0, 0, 0, 0, NULL),
(4701, 1, 0, 0, 0, 0, 0, NULL),
(4702, 1, 0, 0, 0, 0, 0, NULL),
(4705, 1, 1, 0, 0, 0, 0, NULL),
(4706, 1, 1, 0, 0, 0, 0, NULL),
(4707, 1, 1, 0, 0, 0, 0, NULL),
(4708, 1, 1, 0, 0, 0, 0, NULL),
(4709, 1, 1, 0, 0, 0, 0, NULL),
(4720, 1, 1, 0, 0, 0, 0, NULL),
(4721, 1, 1, 0, 0, 0, 0, NULL),
(4722, 1, 1, 0, 0, 0, 0, NULL),
(4726, 1, 1, 0, 0, 0, 0, NULL),
(4727, 1, 0, 0, 0, 0, 0, NULL),
(4729, 1, 0, 0, 0, 0, 0, NULL),
(4779, 1, 0, 0, 0, 0, 0, NULL),
(4827, 0, 1, 0, 0, 0, 0, NULL),
(4830, 0, 1, 0, 0, 0, 0, NULL),
(4875, 1, 1, 0, 0, 0, 0, NULL),
(4876, 1, 1, 0, 0, 0, 0, NULL),
(4877, 1, 1, 0, 0, 0, 0, NULL),
(4878, 1, 1, 0, 0, 0, 0, NULL),
(4882, 1, 1, 0, 0, 0, 0, NULL),
(4943, 1, 1, 0, 0, 0, 0, NULL),
(4945, 1, 0, 0, 0, 0, 0, NULL),
(4946, 1, 0, 0, 0, 0, 0, NULL),
(4950, 1, 0, 0, 0, 0, 0, NULL),
(5050, 1, 1, 0, 0, 0, 0, NULL),
(5051, 1, 1, 0, 0, 0, 0, NULL),
(5054, 1, 1, 0, 0, 0, 0, NULL),
(5056, 1, 1, 0, 0, 0, 0, NULL),
(5058, 1, 1, 0, 0, 0, 0, NULL),
(5098, 1, 1, 0, 0, 0, 0, NULL),
(5131, 1, 1, 0, 0, 0, 0, NULL),
(5185, 0, 1, 0, 0, 0, 0, NULL),
(5186, 0, 1, 0, 0, 0, 0, NULL),
(5189, 1, 1, 0, 0, 0, 0, NULL),
(5192, 1, 1, 0, 0, 0, 0, NULL),
(5198, 1, 0, 0, 0, 0, 0, NULL),
(5202, 1, 1, 0, 1, 0, 0, NULL),
(5268, 1, 1, 0, 0, 0, 0, NULL),
(5272, 1, 0, 0, 0, 0, 0, NULL),
(5274, 1, 0, 0, 0, 0, 0, NULL),
(5286, 1, 1, 0, 0, 0, 0, NULL),
(5287, 1, 0, 0, 0, 0, 0, NULL),
(5288, 1, 0, 0, 0, 0, 0, NULL),
(5291, 1, 1, 0, 0, 0, 0, NULL),
(5307, 1, 1, 0, 0, 0, 0, NULL),
(5308, 1, 0, 0, 0, 0, 0, NULL),
(5349, 1, 1, 0, 0, 0, 0, NULL),
(5352, 1, 1, 0, 0, 0, 0, NULL),
(5356, 1, 1, 0, 0, 0, 0, NULL),
(5389, 1, 1, 0, 0, 0, 0, NULL),
(5395, 1, 1, 0, 0, 0, 0, NULL),
(5396, 1, 1, 0, 0, 0, 0, NULL),
(5397, 1, 1, 0, 0, 0, 0, NULL),
(5398, 1, 1, 0, 0, 0, 0, NULL),
(5403, 1, 0, 0, 0, 0, 0, NULL),
(5406, 1, 0, 0, 0, 0, 0, NULL),
(5411, 1, 1, 0, 0, 0, 0, NULL),
(5412, 1, 1, 0, 0, 0, 0, NULL),
(5422, 1, 0, 0, 0, 0, 0, NULL),
(5423, 1, 0, 0, 0, 0, 0, NULL),
(5424, 1, 0, 0, 0, 0, 0, NULL),
(5425, 1, 0, 0, 0, 0, 0, NULL),
(5426, 1, 1, 0, 0, 0, 0, NULL),
(5427, 1, 1, 0, 0, 0, 0, NULL),
(5428, 1, 0, 0, 0, 0, 0, NULL),
(5429, 1, 1, 0, 0, 0, 0, NULL),
(5430, 1, 1, 0, 0, 0, 0, NULL),
(5433, 1, 0, 0, 0, 0, 0, NULL),
(5434, 0, 1, 0, 0, 0, 0, NULL),
(5435, 0, 1, 0, 0, 0, 0, NULL),
(5437, 1, 0, 0, 0, 0, 0, NULL),
(5441, 1, 0, 0, 0, 0, 0, NULL),
(5444, 1, 0, 0, 0, 0, 0, NULL),
(5446, 1, 0, 0, 0, 0, 0, NULL),
(5449, 1, 0, 0, 0, 0, 0, NULL),
(5450, 1, 1, 0, 0, 0, 0, NULL),
(5451, 1, 1, 0, 0, 0, 0, NULL),
(5452, 1, 1, 0, 0, 0, 0, NULL),
(5453, 1, 0, 0, 0, 0, 0, NULL),
(5454, 1, 1, 0, 0, 0, 0, NULL),
(5455, 1, 1, 0, 0, 0, 0, NULL),
(5456, 1, 0, 0, 0, 0, 0, NULL),
(5457, 1, 0, 0, 0, 0, 0, NULL),
(5458, 1, 1, 0, 0, 0, 0, NULL),
(5459, 1, 1, 0, 0, 0, 0, NULL),
(5460, 1, 1, 0, 0, 0, 0, NULL),
(5465, 1, 1, 0, 0, 0, 0, NULL),
(5467, 1, 1, 0, 0, 0, 0, NULL),
(5469, 1, 0, 0, 0, 0, 0, NULL),
(5471, 1, 1, 0, 0, 0, 0, NULL),
(5472, 1, 1, 0, 0, 0, 0, NULL),
(5473, 1, 1, 0, 0, 0, 0, NULL),
(5474, 1, 1, 0, 0, 0, 0, NULL),
(5475, 1, 1, 0, 0, 0, 0, NULL),
(5523, 1, 0, 0, 0, 0, 0, NULL),
(5543, 1, 1, 0, 0, 0, 0, NULL),
(5544, 1, 1, 0, 0, 0, 0, NULL),
(5594, 1, 1, 0, 0, 0, 0, NULL),
(5599, 1, 1, 0, 0, 0, 0, NULL),
(5600, 1, 1, 0, 0, 0, 0, NULL),
(5601, 1, 1, 0, 0, 0, 0, NULL),
(5602, 1, 1, 0, 0, 0, 0, NULL),
(5604, 1, 1, 0, 0, 0, 0, NULL),
(5615, 1, 1, 0, 0, 0, 0, NULL),
(5616, 1, 1, 0, 0, 0, 0, NULL),
(5617, 1, 1, 0, 0, 0, 0, NULL),
(5618, 1, 1, 0, 0, 0, 0, NULL),
(5623, 1, 1, 0, 0, 0, 0, NULL),
(5638, 1, 1, 0, 0, 0, 0, NULL),
(5641, 1, 1, 0, 0, 0, 0, NULL),
(5642, 1, 1, 0, 0, 0, 0, NULL),
(5643, 1, 1, 0, 0, 0, 0, NULL),
(5644, 1, 1, 0, 0, 0, 0, NULL),
(5645, 1, 1, 0, 0, 0, 0, NULL),
(5646, 1, 1, 0, 0, 0, 0, NULL),
(5647, 1, 1, 0, 0, 0, 0, NULL),
(5674, 1, 1, 0, 1, 0, 0, NULL),
(5678, 1, 1, 0, 0, 0, 0, NULL),
(5708, 1, 0, 0, 0, 0, 0, NULL),
(5726, 1, 0, 0, 0, 0, 0, NULL),
(5727, 1, 0, 0, 0, 0, 0, NULL),
(5728, 1, 1, 0, 0, 0, 0, NULL),
(5729, 1, 0, 0, 0, 0, 0, NULL),
(5730, 1, 0, 0, 0, 0, 0, NULL),
(5752, 1, 1, 0, 0, 0, 0, NULL),
(5756, 1, 1, 0, 0, 0, 0, NULL),
(5760, 1, 0, 0, 0, 0, 0, NULL),
(5764, 1, 0, 0, 0, 0, 0, NULL),
(5766, 1, 0, 0, 0, 0, 0, NULL),
(5767, 1, 1, 0, 0, 0, 0, NULL),
(5768, 1, 1, 0, 0, 0, 0, NULL),
(5769, 1, 1, 0, 0, 0, 0, NULL),
(5770, 1, 1, 0, 0, 0, 0, NULL),
(5771, 1, 1, 0, 0, 0, 0, NULL),
(5774, 1, 0, 0, 0, 0, 0, NULL),
(5783, 1, 1, 0, 0, 0, 0, NULL),
(5784, 1, 1, 0, 0, 0, 0, NULL),
(5785, 1, 1, 0, 0, 0, 0, NULL),
(5786, 1, 1, 0, 0, 0, 0, NULL),
(5787, 1, 1, 0, 0, 0, 0, NULL),
(5792, 1, 1, 0, 0, 0, 0, NULL),
(5797, 1, 1, 0, 0, 0, 0, NULL),
(5798, 1, 1, 0, 0, 0, 0, NULL),
(5799, 1, 1, 0, 0, 0, 0, NULL),
(5800, 1, 1, 0, 0, 0, 0, NULL),
(5806, 1, 0, 0, 0, 0, 0, NULL),
(5807, 1, 1, 0, 0, 0, 0, NULL),
(5808, 1, 1, 0, 0, 0, 0, NULL),
(5810, 1, 1, 0, 0, 0, 0, NULL),
(5813, 1, 1, 0, 0, 0, 0, NULL),
(5818, 1, 1, 0, 0, 0, 0, NULL),
(5828, 1, 0, 0, 0, 0, 0, NULL),
(5829, 1, 0, 0, 0, 0, 0, NULL),
(5830, 1, 1, 0, 0, 0, 0, NULL),
(5831, 1, 1, 0, 0, 0, 0, NULL),
(5834, 1, 0, 0, 0, 0, 0, NULL),
(5836, 1, 1, 0, 0, 0, 0, NULL),
(5837, 1, 1, 0, 0, 0, 0, NULL),
(5838, 1, 1, 0, 0, 0, 0, NULL),
(5841, 1, 1, 0, 0, 0, 0, NULL),
(5842, 1, 0, 0, 0, 0, 0, NULL),
(5849, 1, 1, 0, 0, 0, 0, NULL),
(5850, 1, 0, 0, 0, 0, 0, NULL),
(5854, 1, 0, 0, 0, 0, 0, NULL),
(5856, 1, 0, 0, 0, 0, 0, NULL),
(5857, 1, 0, 0, 0, 0, 0, NULL),
(5858, 1, 0, 0, 0, 0, 0, NULL),
(5859, 1, 1, 0, 0, 0, 0, NULL),
(5863, 1, 1, 0, 0, 0, 0, NULL),
(5865, 1, 0, 0, 0, 0, 0, NULL),
(5870, 1, 1, 0, 0, 0, 0, NULL),
(5871, 1, 1, 0, 0, 0, 0, NULL),
(5876, 1, 1, 0, 0, 0, 0, NULL),
(5877, 1, 1, 0, 0, 0, 0, NULL),
(5878, 1, 1, 0, 0, 0, 0, NULL),
(5888, 1, 1, 0, 0, 0, 0, NULL),
(5899, 1, 1, 0, 0, 0, 0, NULL),
(5901, 1, 1, 0, 0, 0, 0, NULL),
(5904, 1, 1, 0, 0, 0, 0, NULL),
(5905, 1, 1, 0, 0, 0, 0, NULL),
(5906, 1, 1, 0, 0, 0, 0, NULL),
(5907, 1, 1, 0, 0, 0, 0, NULL),
(5908, 1, 1, 0, 0, 0, 0, NULL),
(5911, 1, 1, 0, 0, 0, 0, NULL),
(5915, 1, 1, 0, 0, 0, 0, NULL),
(5919, 1, 0, 0, 0, 0, 0, NULL),
(5930, 1, 1, 0, 0, 0, 0, NULL),
(5932, 1, 1, 0, 0, 0, 0, NULL),
(5933, 1, 1, 0, 0, 0, 0, NULL),
(5936, 0, 1, 0, 0, 0, 0, NULL),
(5938, 1, 1, 0, 0, 0, 0, NULL),
(5939, 1, 1, 0, 0, 0, 0, NULL),
(5940, 1, 1, 0, 0, 0, 0, NULL),
(5944, 1, 1, 0, 0, 0, 0, NULL),
(5955, 1, 1, 0, 0, 0, 0, NULL),
(5957, 1, 1, 0, 0, 0, 0, NULL),
(5984, 1, 0, 0, 0, 0, 0, NULL),
(5985, 1, 1, 0, 0, 0, 0, NULL),
(5992, 1, 1, 0, 0, 0, 0, NULL),
(6015, 1, 1, 0, 0, 0, 0, NULL),
(6019, 1, 1, 0, 0, 0, 0, NULL),
(6032, 1, 1, 0, 0, 0, 0, NULL),
(6033, 0, 1, 0, 0, 0, 0, NULL),
(6046, 1, 1, 0, 0, 0, 0, NULL),
(6066, 1, 1, 0, 1, 0, 0, NULL),
(6067, 1, 1, 0, 0, 0, 0, NULL),
(6068, 1, 1, 0, 0, 0, 0, NULL),
(6072, 1, 1, 0, 0, 0, 0, NULL),
(6073, 1, 1, 0, 0, 0, 0, NULL),
(6115, 1, 1, 0, 0, 0, 0, NULL),
(6132, 1, 1, 0, 0, 0, 0, NULL),
(6139, 1, 0, 0, 0, 0, 0, NULL),
(6141, 1, 0, 0, 0, 0, 0, NULL),
(6145, 0, 1, 0, 0, 0, 0, NULL),
(6167, 1, 0, 0, 0, 0, 0, NULL),
(6183, 1, 1, 0, 0, 0, 0, NULL),
(6236, 1, 1, 0, 0, 0, 0, NULL),
(6244, 1, 1, 0, 0, 0, 0, NULL),
(6247, 1, 1, 0, 0, 0, 0, NULL),
(6248, 1, 1, 0, 0, 0, 0, NULL),
(6249, 1, 1, 0, 0, 0, 0, NULL),
(6251, 1, 1, 0, 0, 0, 0, NULL),
(6252, 1, 1, 0, 0, 0, 0, NULL),
(6253, 1, 1, 0, 0, 0, 0, NULL),
(6254, 1, 1, 0, 0, 0, 0, NULL),
(6266, 1, 1, 0, 0, 0, 0, NULL),
(6271, 1, 0, 0, 0, 0, 0, NULL),
(6290, 1, 1, 0, 0, 0, 0, NULL),
(6368, 1, 0, 0, 0, 0, 0, NULL),
(6387, 1, 1, 0, 0, 0, 0, NULL),
(6393, 1, 1, 0, 0, 0, 0, NULL),
(6394, 1, 1, 0, 0, 0, 0, NULL),
(6410, 1, 1, 0, 0, 0, 0, NULL),
(6491, 1, 1, 0, 0, 0, 0, NULL),
(6494, 1, 1, 0, 0, 0, 0, NULL),
(6495, 1, 1, 0, 0, 0, 0, NULL),
(6496, 1, 1, 0, 0, 0, 0, NULL),
(6548, 1, 1, 0, 0, 0, 0, NULL),
(6568, 1, 1, 0, 0, 0, 0, NULL),
(6576, 1, 1, 0, 0, 0, 0, NULL),
(6606, 1, 1, 0, 0, 0, 0, NULL),
(6626, 1, 1, 0, 0, 0, 0, NULL),
(6668, 1, 1, 0, 0, 0, 0, NULL),
(6706, 1, 1, 0, 0, 0, 0, NULL),
(6726, 1, 1, 0, 0, 0, 0, NULL),
(6730, 1, 1, 0, 0, 0, 0, NULL),
(6746, 1, 1, 0, 0, 0, 0, NULL),
(6747, 1, 1, 0, 0, 0, 0, NULL),
(6769, 1, 1, 0, 0, 0, 0, NULL),
(6770, 1, 1, 0, 0, 0, 0, NULL),
(6775, 1, 1, 0, 0, 0, 0, NULL),
(6776, 1, 1, 0, 0, 0, 0, NULL),
(6783, 1, 1, 0, 0, 0, 0, NULL),
(6788, 1, 0, 0, 0, 0, 0, NULL),
(6789, 1, 0, 0, 0, 0, 0, NULL),
(6791, 1, 1, 0, 0, 0, 0, NULL),
(6827, 1, 1, 0, 0, 0, 0, NULL),
(6867, 1, 0, 0, 0, 0, 0, NULL),
(7044, 1, 0, 0, 0, 0, 0, NULL),
(7045, 1, 0, 0, 0, 0, 0, NULL),
(7046, 1, 0, 0, 0, 0, 0, NULL),
(7055, 1, 0, 0, 0, 0, 0, NULL),
(7067, 1, 1, 0, 0, 0, 0, NULL),
(7089, 1, 1, 0, 0, 0, 0, NULL),
(7125, 1, 0, 0, 0, 0, 0, NULL),
(7126, 1, 0, 0, 0, 0, 0, NULL),
(7161, 1, 1, 0, 0, 0, 0, NULL),
(7166, 1, 0, 0, 0, 0, 0, NULL),
(7233, 1, 1, 0, 0, 0, 0, NULL),
(7268, 1, 1, 0, 0, 0, 0, NULL),
(7287, 1, 1, 0, 0, 0, 0, NULL),
(7288, 1, 1, 0, 0, 0, 0, NULL),
(7307, 1, 1, 0, 0, 0, 0, NULL),
(7308, 1, 1, 0, 0, 0, 0, NULL),
(7310, 1, 1, 0, 0, 0, 0, NULL),
(7319, 1, 0, 0, 0, 0, 0, NULL),
(7323, 1, 1, 0, 0, 0, 0, NULL),
(7376, 0, 0, 1, 0, 0, 0, NULL),
(7404, 1, 1, 0, 0, 0, 0, NULL),
(7407, 1, 1, 0, 0, 0, 0, NULL),
(7408, 1, 1, 0, 0, 0, 0, NULL),
(7428, 1, 0, 0, 0, 0, 0, NULL),
(7429, 1, 0, 0, 0, 0, 0, NULL),
(7430, 1, 0, 0, 0, 0, 0, NULL),
(7431, 1, 0, 0, 0, 0, 0, NULL),
(7432, 1, 0, 0, 0, 0, 0, NULL),
(7433, 1, 0, 0, 0, 0, 0, NULL),
(7434, 1, 0, 0, 0, 0, 0, NULL),
(7435, 1, 0, 0, 0, 0, 0, NULL),
(7436, 1, 0, 0, 0, 0, 0, NULL),
(7437, 1, 0, 0, 0, 0, 0, NULL),
(7438, 1, 1, 0, 0, 0, 0, NULL),
(7439, 1, 1, 0, 0, 0, 0, NULL),
(7440, 1, 1, 0, 0, 0, 0, NULL),
(7441, 1, 1, 0, 0, 0, 0, NULL),
(7442, 1, 1, 0, 0, 0, 0, NULL),
(7443, 1, 0, 0, 0, 0, 0, NULL),
(7444, 1, 0, 0, 0, 0, 0, NULL),
(7445, 1, 0, 0, 0, 0, 0, NULL),
(7446, 1, 0, 0, 0, 0, 0, NULL),
(7447, 1, 0, 0, 0, 0, 0, NULL),
(7448, 1, 0, 0, 0, 0, 0, NULL),
(7449, 1, 0, 0, 0, 0, 0, NULL),
(7450, 1, 1, 0, 0, 0, 0, NULL),
(7451, 1, 1, 0, 0, 0, 0, NULL),
(7452, 1, 1, 0, 0, 0, 0, NULL),
(7453, 1, 1, 0, 0, 0, 0, NULL),
(7454, 1, 1, 0, 0, 0, 0, NULL),
(7455, 1, 0, 0, 0, 0, 0, NULL),
(7456, 1, 0, 0, 0, 0, 0, NULL),
(7457, 1, 1, 0, 0, 0, 0, NULL),
(7458, 1, 1, 0, 0, 0, 0, NULL),
(7459, 1, 1, 0, 0, 0, 0, NULL),
(7460, 1, 1, 0, 0, 0, 0, NULL),
(7461, 1, 1, 0, 0, 0, 0, NULL),
(7462, 1, 0, 0, 0, 0, 0, NULL),
(7463, 1, 1, 0, 0, 0, 0, NULL),
(7523, 1, 0, 0, 0, 0, 0, NULL),
(7524, 1, 0, 0, 0, 0, 0, NULL),
(7527, 1, 1, 0, 1, 0, 0, NULL),
(7553, 0, 0, 1, 0, 0, 0, NULL),
(7555, 0, 0, 1, 0, 0, 0, NULL),
(7564, 1, 1, 0, 0, 0, 0, NULL),
(7583, 1, 1, 0, 0, 0, 0, NULL),
(7714, 1, 1, 0, 0, 0, 0, NULL),
(7724, 1, 1, 0, 0, 0, 0, NULL),
(7730, 1, 1, 0, 0, 0, 0, NULL),
(7731, 1, 1, 0, 0, 0, 0, NULL),
(7733, 1, 1, 0, 0, 0, 0, NULL),
(7763, 1, 1, 0, 0, 0, 0, NULL),
(7767, 1, 0, 0, 0, 0, 0, NULL),
(7769, 1, 0, 0, 0, 0, 0, NULL),
(7770, 1, 1, 0, 0, 0, 0, NULL),
(7771, 1, 1, 0, 0, 0, 0, NULL),
(7784, 1, 0, 0, 0, 0, 0, NULL),
(7799, 1, 1, 0, 0, 0, 0, NULL),
(7804, 1, 1, 0, 0, 0, 0, NULL),
(7823, 1, 1, 0, 0, 0, 0, NULL),
(7824, 1, 1, 0, 0, 0, 0, NULL),
(7847, 1, 1, 0, 0, 0, 0, NULL),
(7855, 1, 1, 0, 0, 0, 0, NULL),
(7856, 1, 1, 0, 0, 0, 0, NULL),
(7857, 1, 1, 0, 0, 0, 0, NULL),
(7858, 1, 1, 0, 0, 0, 0, NULL),
(7870, 1, 1, 0, 0, 0, 0, NULL),
(7872, 1, 1, 0, 0, 0, 0, NULL),
(7873, 1, 1, 0, 0, 0, 0, NULL),
(7874, 1, 1, 0, 0, 0, 0, NULL),
(7876, 1, 1, 0, 0, 0, 0, NULL),
(7881, 1, 1, 0, 0, 0, 0, NULL),
(7882, 1, 1, 0, 0, 0, 0, NULL),
(7883, 1, 1, 0, 0, 0, 0, NULL),
(7975, 1, 1, 0, 0, 0, 0, NULL),
(8016, 1, 1, 0, 0, 0, 0, NULL),
(8035, 1, 1, 0, 1, 0, 0, NULL),
(8075, 0, 1, 1, 0, 0, 0, NULL),
(8119, 1, 1, 0, 0, 0, 0, NULL),
(8124, 1, 1, 0, 0, 0, 0, NULL),
(8125, 1, 1, 0, 0, 0, 0, NULL),
(8126, 1, 1, 0, 0, 0, 0, NULL),
(8128, 1, 1, 0, 0, 0, 0, NULL),
(8129, 1, 1, 0, 0, 0, 0, NULL),
(8131, 1, 1, 0, 0, 0, 0, NULL),
(8137, 1, 1, 0, 0, 0, 0, NULL),
(8139, 1, 1, 0, 0, 0, 0, NULL),
(8150, 1, 1, 0, 0, 0, 0, NULL),
(8151, 1, 1, 0, 0, 0, 0, NULL),
(8152, 1, 1, 0, 0, 0, 0, NULL),
(8153, 1, 1, 0, 0, 0, 0, NULL),
(8154, 1, 1, 0, 0, 0, 0, NULL),
(8196, 1, 0, 0, 0, 0, 0, NULL),
(8197, 1, 0, 0, 0, 0, 0, NULL),
(8198, 1, 0, 0, 0, 0, 0, NULL),
(8208, 1, 0, 0, 0, 0, 0, NULL),
(8211, 1, 0, 0, 0, 0, 0, NULL),
(8236, 0, 1, 0, 0, 0, 0, NULL),
(8276, 0, 0, 1, 0, 0, 0, NULL),
(8277, 1, 1, 0, 0, 0, 0, NULL),
(8300, 1, 1, 0, 0, 0, 0, NULL),
(8303, 1, 1, 0, 0, 0, 0, NULL),
(8306, 1, 1, 0, 0, 0, 0, NULL),
(8307, 1, 1, 0, 0, 0, 0, NULL),
(8336, 1, 1, 0, 0, 0, 0, NULL),
(8356, 1, 1, 0, 0, 0, 0, NULL),
(8357, 1, 1, 0, 0, 0, 0, NULL),
(8358, 1, 1, 0, 0, 0, 0, NULL),
(8359, 1, 1, 0, 0, 0, 0, NULL),
(8360, 1, 1, 0, 0, 0, 0, NULL),
(8361, 1, 1, 0, 0, 0, 0, NULL),
(8362, 1, 1, 0, 0, 0, 0, NULL),
(8363, 1, 1, 0, 0, 0, 0, NULL),
(8364, 1, 1, 0, 0, 0, 0, NULL),
(8398, 1, 1, 0, 0, 0, 0, NULL),
(8401, 1, 1, 0, 0, 0, 0, NULL),
(8418, 1, 1, 0, 0, 0, 0, NULL),
(8496, 1, 1, 0, 0, 0, 0, NULL),
(8506, 0, 0, 1, 0, 0, 0, NULL),
(8518, 1, 1, 0, 0, 0, 0, NULL),
(8549, 1, 1, 0, 0, 0, 0, NULL),
(8554, 1, 1, 0, 0, 0, 0, NULL),
(8579, 1, 1, 0, 0, 0, 0, NULL),
(8582, 1, 1, 0, 0, 0, 0, NULL),
(8588, 1, 1, 0, 0, 0, 0, NULL),
(8617, 1, 1, 0, 0, 0, 0, NULL),
(8656, 1, 0, 0, 0, 0, 0, NULL),
(8657, 1, 0, 0, 0, 0, 0, NULL),
(8661, 1, 1, 0, 0, 0, 0, NULL),
(8662, 0, 0, 1, 0, 0, 0, NULL),
(8664, 1, 1, 0, 0, 0, 0, NULL),
(8667, 1, 1, 0, 0, 0, 0, NULL),
(8674, 1, 1, 0, 0, 0, 0, NULL),
(8722, 1, 1, 0, 0, 0, 0, NULL),
(8736, 1, 1, 0, 0, 0, 0, NULL),
(8738, 1, 1, 0, 0, 0, 0, NULL),
(8762, 1, 1, 0, 0, 0, 0, NULL),
(8764, 1, 1, 2, 0, 0, 0, NULL),
(8878, 1, 1, 0, 0, 0, 0, NULL),
(8881, 1, 0, 0, 0, 0, 0, NULL),
(8882, 1, 0, 0, 0, 0, 0, NULL),
(8883, 1, 0, 0, 0, 0, 0, NULL),
(8884, 1, 0, 0, 0, 0, 0, NULL),
(8885, 1, 0, 0, 0, 0, 0, NULL),
(8933, 1, 1, 0, 0, 0, 0, NULL),
(8956, 1, 0, 0, 0, 0, 0, NULL),
(8957, 1, 0, 0, 0, 0, 0, NULL),
(8958, 1, 0, 0, 0, 0, 0, NULL),
(8959, 1, 1, 0, 0, 0, 0, NULL),
(8960, 1, 0, 0, 0, 0, 0, NULL),
(8961, 1, 0, 0, 0, 0, 0, NULL),
(8964, 1, 0, 1, 0, 0, 0, NULL),
(8996, 1, 1, 0, 0, 0, 0, NULL),
(9076, 1, 1, 0, 0, 0, 0, NULL),
(9087, 1, 1, 0, 0, 0, 0, NULL),
(9165, 1, 0, 0, 0, 0, 0, NULL),
(9238, 1, 1, 0, 0, 0, 0, NULL),
(9298, 1, 1, 0, 0, 0, 0, NULL),
(9316, 1, 1, 0, 0, 0, 0, NULL),
(9336, 1, 1, 0, 0, 0, 0, NULL),
(9377, 1, 0, 0, 0, 0, 0, NULL),
(9397, 1, 1, 0, 0, 0, 0, NULL),
(9416, 1, 1, 0, 0, 0, 0, NULL),
(9461, 1, 0, 1, 0, 0, 0, NULL),
(9496, 1, 1, 0, 1, 0, 0, NULL),
(9525, 1, 1, 0, 0, 0, 0, NULL),
(9549, 1, 1, 0, 0, 0, 0, NULL),
(9551, 1, 1, 0, 0, 0, 0, NULL),
(9557, 1, 1, 0, 0, 0, 0, NULL),
(9558, 1, 1, 0, 0, 0, 0, NULL),
(9577, 1, 0, 0, 0, 0, 0, NULL),
(9617, 1, 1, 0, 0, 0, 0, NULL),
(9636, 1, 1, 0, 0, 0, 0, NULL),
(9690, 1, 0, 0, 0, 0, 0, NULL),
(9694, 1, 0, 0, 0, 0, 0, NULL),
(9696, 1, 1, 0, 0, 0, 0, NULL),
(9697, 1, 0, 0, 0, 0, 0, NULL),
(9706, 1, 1, 0, 0, 0, 0, NULL),
(9820, 1, 1, 0, 0, 0, 0, NULL),
(9857, 1, 1, 0, 0, 0, 0, NULL),
(9981, 1, 1, 0, 0, 0, 0, NULL),
(9983, 1, 1, 0, 0, 0, 0, NULL),
(9985, 1, 1, 0, 0, 0, 0, NULL),
(9990, 1, 1, 0, 0, 0, 0, NULL),
(10016, 1, 0, 0, 0, 0, 0, NULL),
(10017, 1, 0, 0, 0, 0, 0, NULL),
(10048, 1, 1, 0, 0, 0, 0, NULL),
(10050, 1, 1, 0, 0, 0, 0, NULL),
(10054, 1, 1, 0, 0, 0, 0, NULL),
(10063, 1, 1, 0, 0, 0, 0, NULL),
(10077, 1, 0, 0, 0, 0, 0, NULL),
(10079, 1, 1, 0, 0, 0, 0, NULL),
(10086, 1, 1, 0, 0, 0, 0, NULL),
(10182, 1, 1, 0, 0, 0, 0, NULL),
(10183, 1, 0, 0, 0, 0, 0, NULL),
(10184, 1, 0, 0, 0, 0, 0, NULL),
(10197, 1, 1, 0, 0, 0, 0, NULL),
(10198, 1, 0, 0, 0, 0, 0, NULL),
(10199, 1, 1, 0, 0, 0, 0, NULL),
(10200, 1, 0, 0, 0, 0, 0, NULL),
(10201, 1, 1, 0, 0, 0, 0, NULL),
(10221, 1, 0, 0, 0, 0, 0, NULL),
(10256, 1, 1, 0, 0, 0, 0, NULL),
(10265, 1, 0, 0, 0, 0, 0, NULL),
(10278, 1, 1, 0, 0, 0, 0, NULL),
(10300, 1, 1, 0, 0, 0, 0, NULL),
(10301, 1, 1, 0, 0, 0, 0, NULL),
(10303, 1, 1, 0, 0, 0, 0, NULL),
(10305, 1, 1, 0, 0, 0, 0, NULL),
(10307, 1, 1, 0, 0, 0, 0, NULL),
(10359, 1, 0, 0, 0, 0, 0, NULL),
(10362, 1, 1, 0, 0, 0, 0, NULL),
(10365, 1, 1, 0, 0, 0, 0, NULL),
(10368, 1, 1, 0, 0, 0, 0, NULL),
(10374, 1, 1, 0, 0, 0, 0, NULL),
(10375, 1, 1, 0, 0, 0, 0, NULL),
(10377, 1, 1, 0, 0, 0, 0, NULL),
(10378, 1, 1, 0, 0, 0, 0, NULL),
(10380, 1, 1, 0, 0, 0, 0, NULL),
(10401, 1, 1, 0, 0, 0, 0, NULL),
(10402, 1, 1, 0, 0, 0, 0, NULL),
(10403, 1, 1, 0, 0, 0, 0, NULL),
(10404, 1, 1, 0, 0, 0, 0, NULL),
(10408, 0, 1, 1, 0, 0, 0, NULL),
(10415, 0, 0, 1, 0, 0, 0, NULL),
(10427, 1, 1, 0, 0, 0, 0, NULL),
(10428, 1, 1, 0, 0, 0, 0, NULL),
(10431, 1, 1, 0, 0, 0, 0, NULL),
(10443, 1, 1, 0, 0, 0, 0, NULL),
(10444, 1, 1, 0, 0, 0, 0, NULL),
(10446, 1, 1, 0, 0, 0, 0, NULL),
(10448, 1, 1, 0, 0, 0, 0, NULL),
(10450, 1, 1, 0, 0, 0, 0, NULL),
(10460, 1, 1, 0, 0, 0, 0, NULL),
(10468, 1, 1, 0, 0, 0, 0, NULL),
(10506, 1, 0, 1, 0, 0, 0, NULL),
(10537, 1, 1, 0, 0, 0, 0, NULL),
(10539, 1, 1, 0, 0, 0, 0, NULL),
(10541, 0, 0, 1, 0, 0, 0, NULL),
(10581, 1, 0, 0, 0, 0, 0, NULL),
(10582, 1, 0, 0, 0, 0, 0, NULL),
(10599, 1, 1, 0, 0, 0, 0, NULL),
(10600, 1, 1, 0, 0, 0, 0, NULL),
(10607, 1, 0, 0, 0, 0, 0, NULL),
(10612, 1, 1, 0, 0, 0, 0, NULL),
(10617, 1, 1, 0, 0, 0, 0, NULL),
(10618, 1, 1, 0, 0, 0, 0, NULL),
(10619, 1, 1, 0, 0, 0, 0, NULL),
(10636, 1, 0, 0, 0, 0, 0, NULL),
(10637, 1, 1, 0, 0, 0, 0, NULL),
(10638, 1, 1, 0, 0, 0, 0, NULL),
(10644, 1, 0, 0, 0, 0, 0, NULL),
(10645, 1, 1, 0, 0, 0, 0, NULL),
(10646, 1, 1, 0, 0, 0, 0, NULL),
(10656, 1, 0, 0, 0, 0, 0, NULL),
(10657, 1, 0, 0, 0, 0, 0, NULL),
(10659, 1, 0, 0, 0, 0, 0, NULL),
(10660, 1, 0, 0, 0, 0, 0, NULL),
(10661, 1, 0, 0, 0, 0, 0, NULL),
(10662, 1, 0, 0, 0, 0, 0, NULL),
(10663, 1, 0, 0, 0, 0, 0, NULL),
(10664, 1, 0, 0, 0, 0, 0, NULL),
(10668, 1, 1, 0, 0, 0, 0, NULL),
(10684, 1, 0, 0, 0, 0, 0, NULL),
(10685, 1, 0, 0, 0, 0, 0, NULL),
(10721, 1, 1, 0, 0, 0, 0, NULL),
(10737, 1, 0, 0, 0, 0, 0, NULL),
(10738, 1, 1, 0, 0, 0, 0, NULL),
(10741, 1, 0, 0, 0, 0, 0, NULL),
(10758, 1, 1, 0, 0, 0, 0, NULL),
(10759, 1, 1, 0, 0, 0, 0, NULL),
(10760, 1, 1, 0, 0, 0, 0, NULL),
(10761, 1, 1, 0, 0, 0, 0, NULL),
(10779, 1, 0, 0, 0, 0, 0, NULL),
(10780, 1, 0, 0, 0, 0, 0, NULL),
(10806, 1, 0, 0, 0, 0, 0, NULL),
(10807, 1, 0, 0, 0, 0, 0, NULL),
(10810, 1, 0, 0, 0, 0, 0, NULL),
(10881, 1, 1, 0, 0, 0, 0, NULL),
(10882, 1, 0, 0, 0, 0, 0, NULL),
(10896, 1, 1, 0, 0, 0, 0, NULL),
(10916, 1, 1, 0, 0, 0, 0, NULL),
(10918, 1, 1, 0, 0, 0, 0, NULL),
(10928, 1, 0, 0, 0, 0, 0, NULL),
(10929, 1, 1, 0, 0, 0, 0, NULL),
(10941, 1, 1, 0, 0, 0, 0, NULL),
(10942, 0, 1, 0, 0, 0, 0, NULL),
(10956, 0, 1, 0, 0, 0, 0, NULL),
(10978, 1, 1, 0, 0, 0, 0, NULL),
(10979, 1, 0, 0, 0, 0, 0, NULL),
(10981, 1, 0, 0, 0, 0, 0, NULL),
(10992, 1, 0, 0, 0, 0, 0, NULL),
(10993, 1, 1, 0, 0, 0, 0, NULL),
(11024, 1, 0, 0, 0, 0, 0, NULL),
(11047, 1, 1, 0, 0, 0, 0, NULL),
(11051, 1, 1, 0, 0, 0, 0, NULL),
(11071, 1, 1, 0, 0, 0, 0, NULL),
(11074, 1, 1, 0, 0, 0, 0, NULL),
(11079, 1, 1, 0, 0, 0, 0, NULL),
(11084, 1, 1, 0, 0, 0, 0, NULL),
(11103, 1, 1, 0, 0, 0, 0, NULL),
(11104, 1, 1, 0, 0, 0, 0, NULL),
(11105, 1, 1, 0, 0, 0, 0, NULL),
(11106, 1, 1, 0, 0, 0, 0, NULL),
(11116, 1, 1, 0, 0, 0, 0, NULL),
(11117, 1, 1, 0, 0, 0, 0, NULL),
(11118, 1, 1, 0, 0, 0, 0, NULL),
(11119, 1, 1, 0, 0, 0, 0, NULL),
(11138, 1, 1, 0, 0, 0, 0, NULL),
(11139, 1, 1, 0, 0, 0, 0, NULL),
(11179, 0, 0, 1, 0, 0, 0, NULL),
(11182, 1, 1, 0, 0, 0, 0, NULL),
(11183, 1, 1, 0, 0, 0, 0, NULL),
(11184, 1, 1, 0, 0, 0, 0, NULL),
(11185, 1, 1, 0, 0, 0, 0, NULL),
(11186, 1, 1, 0, 0, 0, 0, NULL),
(11187, 1, 1, 0, 0, 0, 0, NULL),
(11188, 1, 1, 0, 0, 0, 0, NULL),
(11189, 1, 1, 0, 0, 0, 0, NULL),
(11190, 1, 1, 0, 0, 0, 0, NULL),
(11191, 1, 1, 0, 0, 0, 0, NULL),
(11192, 1, 1, 0, 0, 0, 0, NULL),
(11193, 1, 1, 0, 0, 0, 0, NULL),
(11259, 1, 1, 0, 0, 0, 0, NULL),
(11317, 1, 1, 0, 0, 0, 0, NULL),
(11318, 1, 1, 0, 0, 0, 0, NULL),
(11319, 1, 1, 0, 0, 0, 0, NULL),
(11337, 1, 1, 0, 0, 0, 0, NULL),
(11338, 1, 1, 0, 0, 0, 0, NULL),
(11339, 1, 1, 0, 0, 0, 0, NULL),
(11340, 1, 1, 0, 0, 0, 0, NULL),
(11341, 1, 1, 0, 0, 0, 0, NULL),
(11342, 1, 1, 0, 0, 0, 0, NULL),
(11343, 1, 1, 0, 0, 0, 0, NULL),
(11344, 1, 1, 0, 0, 0, 0, NULL),
(11345, 1, 1, 0, 0, 0, 0, NULL),
(11347, 1, 1, 0, 0, 0, 0, NULL),
(11348, 1, 1, 0, 0, 0, 0, NULL),
(11349, 1, 1, 0, 0, 0, 0, NULL),
(11350, 1, 1, 0, 0, 0, 0, NULL),
(11351, 1, 1, 0, 0, 0, 0, NULL),
(11352, 1, 1, 0, 0, 0, 0, NULL),
(11353, 1, 1, 0, 0, 0, 0, NULL),
(11354, 1, 1, 0, 0, 0, 0, NULL),
(11356, 1, 1, 0, 0, 0, 0, NULL),
(11357, 1, 0, 0, 0, 0, 0, NULL),
(11358, 1, 0, 0, 0, 0, 0, NULL),
(11359, 1, 0, 0, 0, 0, 0, NULL),
(11360, 1, 1, 0, 0, 0, 0, NULL),
(11361, 1, 1, 0, 0, 0, 0, NULL),
(11364, 1, 0, 0, 0, 0, 0, NULL),
(11365, 1, 1, 0, 0, 0, 0, NULL),
(11366, 1, 0, 0, 0, 0, 0, NULL),
(11367, 1, 1, 0, 0, 0, 0, NULL),
(11368, 1, 0, 1, 0, 0, 0, NULL),
(11369, 1, 0, 0, 0, 0, 0, NULL),
(11370, 1, 0, 0, 0, 0, 0, NULL),
(11371, 1, 1, 0, 0, 0, 0, NULL),
(11372, 1, 1, 0, 0, 0, 0, NULL),
(11373, 1, 1, 0, 0, 0, 0, NULL),
(11374, 0, 1, 0, 0, 0, 0, NULL),
(11375, 1, 0, 0, 0, 0, 0, NULL),
(11376, 1, 0, 0, 0, 0, 0, NULL),
(11377, 1, 0, 0, 0, 0, 0, NULL),
(11379, 1, 0, 0, 0, 0, 0, NULL),
(11380, 1, 1, 0, 0, 0, 0, NULL),
(11382, 1, 1, 0, 0, 0, 0, NULL),
(11384, 1, 1, 0, 0, 0, 0, NULL),
(11385, 1, 0, 0, 0, 0, 0, NULL),
(11386, 1, 0, 0, 0, 0, 0, NULL),
(11387, 1, 1, 0, 0, 0, 0, NULL),
(11388, 1, 1, 0, 0, 0, 0, NULL),
(11389, 1, 1, 0, 0, 0, 0, NULL),
(11390, 1, 1, 0, 0, 0, 0, NULL),
(11391, 1, 1, 0, 0, 0, 0, NULL),
(11407, 1, 1, 0, 0, 0, 0, NULL),
(11438, 1, 1, 0, 0, 0, 0, NULL),
(11449, 1, 1, 0, 0, 0, 0, NULL),
(11463, 1, 0, 0, 0, 0, 0, NULL),
(11468, 1, 1, 0, 0, 0, 0, NULL),
(11478, 1, 0, 0, 0, 0, 0, NULL),
(11481, 1, 0, 0, 0, 0, 0, NULL),
(11493, 1, 0, 0, 0, 0, 0, NULL),
(11494, 0, 0, 1, 0, 0, 0, NULL),
(11495, 1, 0, 0, 0, 0, 0, NULL),
(11499, 1, 1, 0, 0, 0, 0, NULL),
(11500, 1, 1, 0, 0, 0, 0, NULL),
(11516, 1, 1, 0, 0, 0, 0, NULL),
(11546, 1, 1, 0, 0, 0, 0, NULL),
(11556, 1, 1, 0, 0, 0, 0, NULL),
(11559, 1, 1, 0, 0, 0, 0, NULL),
(11561, 1, 0, 0, 0, 0, 0, NULL),
(11564, 1, 0, 0, 0, 0, 0, NULL),
(11576, 1, 0, 0, 0, 0, 0, NULL),
(11577, 1, 1, 0, 0, 0, 0, NULL),
(11578, 1, 0, 0, 0, 0, 0, NULL),
(11583, 1, 1, 1, 0, 0, 0, NULL),
(11596, 1, 1, 0, 0, 0, 0, NULL),
(11599, 1, 1, 0, 0, 0, 0, NULL),
(11601, 1, 1, 0, 0, 0, 0, NULL),
(11614, 1, 0, 0, 0, 0, 0, NULL),
(11624, 1, 1, 0, 0, 0, 0, NULL),
(11625, 1, 1, 0, 0, 0, 0, NULL),
(11626, 1, 1, 0, 0, 0, 0, NULL),
(11629, 1, 1, 0, 0, 0, 0, NULL),
(11660, 1, 0, 0, 0, 0, 0, NULL),
(11670, 1, 0, 0, 0, 0, 0, NULL),
(11680, 1, 1, 0, 0, 0, 0, NULL),
(11684, 1, 0, 0, 0, 0, 0, NULL),
(11685, 1, 1, 0, 0, 0, 0, NULL),
(11686, 1, 1, 0, 0, 0, 0, NULL),
(11687, 1, 1, 0, 0, 0, 0, NULL),
(11696, 1, 1, 0, 0, 0, 0, NULL),
(11697, 1, 1, 0, 0, 0, 0, NULL),
(11715, 1, 1, 0, 0, 0, 0, NULL),
(11718, 1, 1, 0, 0, 0, 0, NULL),
(11738, 1, 0, 0, 0, 0, 0, NULL),
(11739, 1, 0, 0, 0, 0, 0, NULL),
(11751, 1, 1, 0, 0, 0, 0, NULL),
(11753, 1, 1, 0, 0, 0, 0, NULL),
(11754, 1, 1, 0, 0, 0, 0, NULL),
(11755, 1, 1, 0, 0, 0, 0, NULL),
(11756, 1, 1, 0, 0, 0, 0, NULL),
(11757, 1, 1, 0, 0, 0, 0, NULL),
(11758, 1, 1, 0, 0, 0, 0, NULL),
(11777, 1, 0, 0, 0, 0, 0, NULL),
(11778, 1, 0, 0, 0, 0, 0, NULL),
(11781, 1, 0, 0, 0, 0, 0, NULL),
(11782, 1, 0, 0, 0, 0, 0, NULL),
(11787, 1, 0, 0, 0, 0, 0, NULL),
(11788, 1, 0, 0, 0, 0, 0, NULL),
(11808, 1, 1, 0, 0, 0, 0, NULL),
(11811, 1, 1, 0, 0, 0, 0, NULL),
(11821, 1, 1, 0, 0, 0, 0, NULL),
(11822, 1, 1, 0, 0, 0, 0, NULL),
(11823, 1, 1, 0, 0, 0, 0, NULL),
(11826, 1, 1, 0, 0, 0, 0, NULL),
(11830, 1, 1, 0, 0, 0, 0, NULL),
(11831, 1, 1, 0, 0, 0, 0, NULL),
(11833, 1, 1, 0, 0, 0, 0, NULL),
(11836, 1, 0, 0, 0, 0, 0, NULL),
(11856, 1, 1, 0, 0, 0, 0, NULL),
(11858, 1, 1, 0, 0, 0, 0, NULL),
(11860, 1, 1, 0, 0, 0, 0, NULL),
(11861, 1, 1, 0, 0, 0, 0, NULL),
(11862, 1, 0, 0, 0, 0, 0, NULL),
(11863, 1, 1, 0, 0, 0, 0, NULL),
(11864, 1, 1, 0, 0, 0, 0, NULL),
(11869, 1, 1, 0, 0, 0, 0, NULL),
(11871, 1, 0, 0, 0, 0, 0, NULL),
(11877, 1, 1, 0, 0, 0, 0, NULL),
(11910, 1, 1, 0, 0, 0, 0, NULL),
(11911, 1, 1, 0, 0, 0, 0, NULL),
(11912, 1, 1, 0, 0, 0, 0, NULL),
(11913, 1, 1, 0, 0, 0, 0, NULL),
(11914, 1, 1, 0, 0, 0, 0, NULL),
(11915, 1, 1, 0, 0, 0, 0, NULL),
(11917, 1, 1, 0, 0, 0, 0, NULL),
(11918, 1, 1, 0, 0, 0, 0, NULL),
(11921, 1, 0, 0, 0, 0, 0, NULL),
(11944, 1, 1, 0, 0, 0, 0, NULL),
(11957, 1, 0, 0, 0, 0, 0, NULL),
(11959, 1, 0, 0, 0, 0, 0, NULL),
(12027, 1, 1, 0, 0, 0, 0, NULL),
(12028, 1, 1, 0, 0, 0, 0, NULL),
(12030, 1, 1, 0, 0, 0, 0, NULL),
(12031, 1, 1, 0, 0, 0, 0, NULL),
(12032, 1, 1, 0, 0, 0, 0, NULL),
(12033, 1, 1, 0, 0, 0, 0, NULL),
(12034, 1, 1, 0, 0, 0, 0, NULL),
(12037, 1, 1, 0, 0, 0, 0, NULL),
(12038, 1, 1, 0, 0, 0, 0, NULL),
(12043, 1, 1, 0, 0, 0, 0, NULL),
(12045, 1, 1, 0, 0, 0, 0, NULL),
(12046, 1, 1, 0, 0, 0, 0, NULL),
(12120, 1, 1, 0, 0, 0, 2, NULL),
(12123, 0, 1, 0, 0, 0, 0, NULL),
(12124, 0, 1, 0, 0, 0, 0, NULL),
(12125, 0, 1, 0, 0, 0, 0, NULL),
(12138, 1, 1, 0, 0, 0, 0, NULL),
(12144, 1, 0, 0, 0, 0, 0, NULL),
(12150, 1, 0, 0, 0, 0, 0, NULL),
(12239, 1, 0, 0, 0, 0, 0, NULL),
(12240, 1, 0, 0, 0, 0, 0, NULL),
(12241, 1, 0, 0, 0, 0, 0, NULL),
(12245, 1, 1, 0, 0, 0, 0, NULL),
(12246, 1, 1, 0, 0, 0, 0, NULL),
(12277, 1, 1, 0, 0, 0, 0, NULL),
(12296, 1, 0, 0, 0, 0, 0, NULL),
(12338, 1, 1, 0, 0, 0, 0, NULL),
(12340, 1, 1, 0, 0, 0, 0, NULL),
(12354, 1, 0, 0, 0, 0, 0, NULL),
(12355, 1, 0, 0, 0, 0, 0, NULL),
(12356, 1, 0, 0, 0, 0, 0, NULL),
(12357, 1, 0, 0, 0, 0, 0, NULL),
(12383, 1, 0, 0, 0, 0, 0, NULL),
(12426, 1, 0, 0, 1, 0, 0, NULL),
(12431, 1, 1, 0, 0, 0, 0, NULL),
(12432, 1, 0, 0, 0, 0, 0, NULL),
(12433, 1, 1, 0, 0, 0, 0, NULL),
(12576, 1, 1, 0, 0, 0, 0, NULL),
(12579, 1, 1, 0, 0, 0, 0, NULL),
(12758, 0, 0, 1, 0, 0, 0, NULL),
(12816, 1, 1, 0, 0, 0, 0, NULL),
(12865, 1, 1, 0, 0, 0, 0, NULL),
(12866, 1, 1, 0, 0, 0, 0, NULL),
(12922, 1, 0, 0, 0, 0, 0, NULL),
(12960, 1, 1, 0, 0, 0, 0, NULL),
(12999, 0, 1, 1, 0, 0, 0, NULL),
(13019, 1, 1, 0, 0, 0, 0, NULL),
(13321, 1, 1, 0, 0, 0, 0, NULL),
(13596, 0, 1, 0, 0, 0, 0, NULL),
(13656, 1, 1, 0, 0, 0, 0, NULL),
(13697, 1, 1, 0, 0, 0, 0, NULL),
(13698, 1, 1, 0, 0, 0, 0, NULL),
(13699, 1, 1, 0, 0, 0, 0, NULL),
(13717, 1, 1, 0, 0, 0, 0, NULL),
(13718, 1, 1, 0, 0, 0, 0, NULL),
(13737, 1, 1, 0, 0, 0, 0, NULL),
(13836, 1, 0, 0, 0, 0, 0, NULL),
(13837, 1, 0, 0, 0, 0, 0, NULL),
(13842, 1, 1, 0, 0, 0, 0, NULL),
(13917, 1, 1, 0, 0, 0, 0, NULL),
(14081, 1, 1, 0, 1, 0, 0, NULL),
(14201, 1, 1, 0, 0, 0, 0, NULL),
(14222, 1, 0, 0, 0, 0, 0, NULL),
(14228, 1, 1, 0, 0, 0, 0, NULL),
(14232, 1, 1, 0, 0, 0, 0, NULL),
(14234, 1, 1, 0, 0, 0, 0, NULL),
(14266, 1, 1, 0, 0, 0, 0, NULL),
(14272, 0, 0, 1, 0, 0, 0, NULL),
(14279, 1, 0, 0, 0, 0, 0, NULL),
(14280, 1, 0, 0, 0, 0, 0, NULL),
(14282, 1, 0, 0, 0, 0, 0, NULL),
(14339, 1, 0, 0, 0, 0, 0, NULL),
(14344, 1, 0, 0, 0, 0, 0, NULL),
(14348, 1, 1, 0, 0, 0, 0, NULL),
(14356, 0, 1, 0, 0, 0, 0, NULL),
(14357, 0, 1, 0, 0, 0, 0, NULL),
(14362, 1, 1, 0, 1, 0, 0, NULL),
(14388, 1, 0, 0, 0, 0, 0, NULL),
(14389, 1, 1, 0, 0, 0, 0, NULL),
(14422, 0, 0, 1, 0, 0, 0, NULL),
(14426, 1, 1, 0, 0, 0, 0, NULL),
(14430, 1, 0, 0, 0, 0, 0, NULL),
(14440, 1, 1, 0, 0, 0, 0, NULL),
(14441, 1, 1, 0, 0, 0, 0, NULL),
(14442, 1, 1, 0, 0, 0, 0, NULL),
(14449, 0, 0, 1, 0, 0, 0, NULL),
(14458, 1, 0, 0, 0, 0, 0, NULL),
(14472, 1, 0, 0, 0, 0, 0, NULL),
(14490, 0, 1, 0, 0, 0, 0, NULL),
(14495, 0, 0, 1, 0, 0, 0, NULL),
(14507, 1, 1, 0, 0, 0, 0, NULL),
(14508, 1, 1, 0, 0, 0, 0, 0),
(14509, 1, 1, 0, 0, 0, 0, NULL),
(14510, 1, 1, 0, 0, 0, 0, NULL),
(14517, 1, 1, 0, 0, 0, 0, NULL),
(14531, 1, 1, 0, 0, 0, 0, NULL),
(14532, 1, 0, 0, 0, 0, 0, NULL),
(14542, 1, 0, 0, 0, 0, 0, NULL),
(14549, 1, 0, 0, 0, 0, 0, NULL),
(14550, 1, 0, 0, 0, 0, 0, NULL),
(14567, 1, 1, 0, 0, 0, 0, NULL),
(14606, 0, 0, 1, 0, 0, 0, NULL),
(14697, 1, 1, 0, 0, 0, 0, NULL),
(14699, 1, 0, 0, 0, 0, 0, NULL),
(14706, 1, 0, 0, 0, 0, 0, NULL),
(14711, 1, 0, 0, 0, 0, 0, NULL),
(14717, 1, 1, 0, 0, 0, 0, NULL),
(14718, 1, 1, 0, 0, 0, 0, NULL),
(14728, 1, 1, 0, 0, 0, 0, NULL),
(14742, 1, 1, 0, 0, 0, 0, NULL),
(14743, 1, 1, 0, 0, 0, 0, NULL),
(14750, 1, 0, 0, 0, 0, 0, NULL),
(14754, 1, 1, 0, 0, 0, 0, NULL),
(14781, 1, 1, 0, 0, 0, 0, NULL),
(14821, 1, 0, 0, 0, 0, 0, NULL),
(14823, 1, 1, 0, 0, 0, 0, 20000),
(14825, 1, 1, 0, 0, 0, 0, NULL),
(14826, 1, 1, 0, 0, 0, 0, NULL),
(14834, 1, 0, 0, 0, 0, 0, NULL),
(14850, 1, 1, 0, 0, 0, 0, NULL),
(14857, 1, 1, 0, 0, 0, 0, NULL),
(14859, 1, 1, 0, 0, 0, 0, NULL),
(14865, 1, 0, 0, 0, 0, 0, NULL),
(14872, 1, 1, 0, 0, 0, 0, NULL),
(14873, 1, 1, 0, 0, 0, 0, NULL),
(14874, 1, 1, 0, 0, 0, 0, NULL),
(14880, 1, 0, 0, 0, 0, 0, NULL),
(14881, 1, 1, 0, 0, 0, 0, NULL),
(14882, 1, 1, 0, 0, 0, 0, NULL),
(14883, 1, 1, 0, 0, 0, 0, NULL),
(14892, 1, 1, 0, 0, 0, 0, NULL),
(14893, 1, 1, 0, 0, 0, 0, NULL),
(14894, 1, 0, 0, 0, 0, 0, NULL),
(14901, 1, 1, 0, 0, 0, 0, NULL),
(14908, 1, 1, 0, 0, 0, 0, NULL),
(14909, 1, 1, 0, 0, 0, 0, NULL),
(14964, 1, 1, 0, 0, 0, 0, NULL),
(14965, 1, 0, 1, 0, 0, 0, NULL),
(14988, 1, 1, 0, 0, 0, 0, NULL),
(15041, 1, 0, 0, 0, 0, 0, NULL),
(15042, 1, 1, 0, 0, 0, 0, NULL),
(15047, 1, 1, 0, 1, 0, 0, NULL),
(15067, 1, 1, 0, 0, 0, 0, NULL),
(15068, 1, 1, 0, 0, 0, 0, NULL),
(15101, 1, 1, 0, 0, 0, 0, NULL),
(15111, 1, 1, 0, 0, 0, 0, NULL),
(15118, 0, 1, 0, 0, 0, 0, NULL),
(15165, 1, 1, 0, 0, 0, 0, NULL),
(15185, 1, 1, 1, 0, 0, 0, NULL),
(15192, 1, 0, 0, 0, 0, 0, NULL),
(15210, 1, 0, 0, 0, 0, 0, NULL),
(15214, 0, 0, 1, 0, 0, 0, NULL),
(15218, 0, 0, 1, 0, 0, 0, NULL),
(15221, 0, 0, 1, 0, 0, 0, NULL),
(15222, 0, 0, 1, 0, 0, 0, NULL),
(15223, 1, 1, 0, 0, 0, 0, NULL),
(15226, 1, 0, 0, 0, 0, 0, NULL),
(15227, 1, 0, 0, 0, 0, 0, NULL),
(15228, 1, 0, 0, 0, 0, 0, NULL),
(15231, 1, 0, 0, 0, 0, 0, NULL),
(15232, 1, 0, 0, 0, 0, 0, NULL),
(15234, 1, 0, 0, 0, 0, 0, NULL),
(15235, 1, 1, 0, 0, 0, 0, NULL),
(15236, 1, 1, 0, 0, 0, 0, NULL),
(15237, 1, 0, 0, 0, 0, 0, NULL),
(15238, 1, 0, 0, 0, 0, 0, NULL),
(15239, 1, 0, 0, 0, 0, 0, NULL),
(15241, 1, 0, 1, 0, 0, 0, NULL),
(15242, 1, 0, 1, 0, 0, 0, NULL),
(15243, 1, 0, 0, 0, 0, 0, NULL),
(15244, 1, 0, 0, 0, 0, 0, NULL),
(15245, 1, 0, 0, 0, 0, 0, NULL),
(15248, 1, 0, 0, 0, 0, 0, NULL),
(15249, 1, 1, 0, 0, 0, 0, NULL),
(15251, 1, 0, 0, 0, 0, 0, NULL),
(15253, 1, 0, 0, 0, 0, 0, NULL),
(15254, 1, 0, 0, 0, 0, 0, NULL),
(15255, 1, 0, 0, 0, 0, 0, NULL),
(15256, 1, 0, 0, 0, 0, 0, NULL),
(15257, 1, 0, 0, 0, 0, 0, NULL),
(15258, 1, 0, 0, 0, 0, 0, NULL),
(15259, 1, 0, 0, 0, 0, 0, NULL),
(15322, 1, 0, 0, 0, 0, 0, NULL),
(15326, 1, 0, 0, 0, 0, 0, NULL),
(15329, 1, 0, 0, 0, 0, 0, NULL),
(15330, 1, 0, 0, 0, 0, 0, NULL),
(15331, 1, 0, 0, 0, 0, 0, NULL),
(15332, 1, 0, 0, 0, 0, 0, NULL),
(15337, 1, 0, 0, 0, 0, 0, NULL),
(15342, 1, 0, 0, 0, 0, 0, NULL),
(15345, 1, 0, 0, 0, 0, 0, NULL),
(15346, 1, 0, 0, 0, 0, 0, NULL),
(15347, 1, 0, 0, 0, 0, 0, NULL),
(15350, 1, 1, 0, 0, 0, 0, NULL),
(15366, 1, 0, 0, 0, 0, 0, NULL),
(15372, 1, 0, 0, 0, 0, 0, NULL),
(15384, 0, 0, 1, 0, 0, 0, NULL),
(15393, 1, 0, 0, 0, 0, 0, NULL),
(15426, 0, 0, 1, 0, 0, 0, NULL),
(15472, 1, 1, 0, 0, 0, 0, NULL),
(15491, 0, 1, 1, 0, 0, 0, NULL),
(15505, 0, 1, 0, 0, 0, 0, NULL),
(15520, 1, 0, 0, 0, 0, 0, NULL),
(15526, 0, 1, 0, 0, 0, 0, NULL),
(15554, 1, 0, 0, 0, 0, 0, NULL),
(15571, 0, 1, 0, 0, 0, 0, NULL),
(15590, 0, 0, 1, 0, 0, 0, NULL),
(15631, 0, 0, 1, 0, 0, 0, NULL),
(15646, 1, 0, 0, 0, 0, 0, NULL),
(15651, 1, 0, 0, 0, 0, 0, NULL),
(15652, 1, 0, 0, 0, 0, 0, NULL),
(15661, 0, 1, 0, 0, 0, 0, NULL),
(15695, 0, 0, 1, 0, 0, 0, NULL),
(15717, 0, 0, 1, 0, 0, 0, NULL),
(15767, 1, 1, 0, 0, 0, 0, NULL),
(15800, 0, 0, 1, 0, 0, 0, NULL),
(15922, 0, 0, 1, 0, 0, 0, NULL),
(15963, 1, 1, 1, 0, 0, 0, NULL),
(15964, 0, 0, 1, 0, 0, 0, NULL),
(15974, 1, 1, 0, 0, 0, 0, NULL),
(15975, 1, 1, 0, 0, 0, 0, NULL),
(15976, 1, 1, 0, 0, 0, 0, NULL),
(15977, 1, 0, 0, 0, 0, 0, NULL),
(16014, 1, 1, 0, 0, 0, 0, NULL),
(16015, 1, 1, 0, 0, 0, 0, NULL),
(16085, 0, 0, 1, 0, 0, 0, NULL),
(16111, 1, 0, 0, 1, 0, 0, NULL),
(16117, 1, 0, 0, 0, 0, 0, NULL),
(16129, 0, 0, 1, 0, 0, 0, NULL),
(16137, 1, 1, 1, 0, 0, 0, NULL),
(16138, 1, 0, 0, 0, 0, 0, NULL),
(16140, 1, 0, 0, 0, 0, 0, NULL),
(16166, 1, 0, 0, 0, 0, 0, NULL),
(16170, 1, 0, 0, 0, 0, 0, NULL),
(16171, 1, 0, 0, 0, 0, 0, NULL),
(16179, 1, 0, 0, 0, 0, 0, NULL),
(16188, 1, 0, 0, 0, 0, 0, NULL),
(16218, 0, 0, 1, 0, 0, 0, NULL),
(16227, 1, 1, 0, 0, 0, 0, NULL),
(16236, 1, 1, 0, 1, 0, 0, NULL),
(16296, 0, 0, 1, 0, 0, 0, NULL),
(16347, 1, 0, 0, 0, 0, 0, NULL),
(16348, 1, 0, 0, 0, 0, 0, NULL),
(16349, 1, 0, 0, 0, 0, 0, NULL),
(16350, 1, 0, 0, 0, 0, 0, NULL),
(16351, 1, 0, 0, 0, 0, 0, NULL),
(16352, 1, 0, 0, 0, 0, 0, NULL),
(16363, 0, 0, 1, 0, 0, 0, NULL),
(16364, 1, 1, 0, 1, 0, 0, NULL),
(16371, 1, 0, 0, 0, 0, 0, NULL),
(16372, 1, 0, 0, 0, 0, 0, NULL),
(16377, 1, 0, 0, 0, 0, 0, NULL),
(16378, 1, 0, 2, 0, 0, 0, NULL),
(16400, 1, 1, 0, 0, 0, 0, NULL),
(16404, 0, 1, 0, 0, 0, 0, NULL),
(16405, 0, 1, 0, 0, 0, 0, NULL),
(16416, 1, 1, 0, 0, 0, 0, NULL),
(16417, 1, 1, 0, 0, 0, 0, NULL),
(16418, 1, 1, 0, 0, 0, 0, NULL),
(16453, 1, 0, 0, 0, 0, 0, NULL),
(16458, 1, 1, 0, 0, 0, 0, NULL),
(16474, 1, 0, 0, 0, 0, 0, NULL),
(16550, 0, 1, 1, 0, 0, 0, NULL),
(16556, 1, 1, 0, 0, 0, 0, NULL),
(16557, 1, 1, 0, 0, 0, 0, NULL),
(16560, 1, 1, 0, 0, 0, 0, NULL),
(16561, 1, 0, 0, 0, 0, 0, NULL),
(16562, 1, 0, 0, 0, 0, 0, NULL),
(16563, 1, 1, 0, 0, 0, 0, NULL),
(16567, 1, 1, 0, 0, 0, 0, NULL),
(16569, 1, 0, 0, 0, 0, 0, NULL),
(16570, 1, 1, 0, 0, 0, 0, NULL),
(16571, 1, 1, 0, 0, 0, 0, NULL),
(16572, 1, 1, 0, 0, 0, 0, NULL),
(16573, 1, 1, 0, 0, 0, 0, NULL),
(16581, 1, 1, 0, 0, 0, 0, NULL),
(16592, 0, 0, 1, 0, 0, 0, NULL),
(16598, 0, 0, 1, 0, 0, 0, NULL),
(16779, 1, 0, 0, 0, 0, 0, NULL),
(16781, 1, 0, 0, 0, 0, 0, NULL),
(16887, 0, 0, 1, 0, 0, 0, NULL),
(16897, 0, 0, 1, 1, 0, 0, NULL),
(16898, 0, 0, 1, 0, 0, 0, NULL),
(16899, 0, 0, 1, 0, 0, 0, NULL),
(16922, 0, 0, 1, 0, 0, 0, NULL),
(16934, 1, 0, 0, 0, 0, 0, NULL),
(16972, 1, 0, 0, 0, 0, 0, NULL),
(17035, 1, 0, 0, 0, 0, 0, NULL),
(17047, 0, 0, 1, 0, 0, 0, NULL),
(17053, 1, 0, 0, 0, 0, 0, NULL),
(17066, 1, 0, 0, 0, 0, 0, NULL),
(17093, 1, 1, 0, 0, 0, 0, NULL),
(17107, 0, 0, 1, 0, 0, 0, NULL),
(17111, 1, 0, 0, 0, 0, 0, NULL),
(17112, 1, 0, 0, 0, 0, 0, NULL),
(17123, 1, 1, 0, 0, 0, 0, 0),
(17178, 1, 1, 0, 0, 0, 0, NULL),
(17202, 1, 0, 0, 0, 0, 0, NULL),
(17203, 1, 0, 0, 0, 0, 0, NULL),
(17225, 1, 1, 1, 0, 0, 0, NULL),
(17233, 0, 1, 1, 0, 0, 0, NULL),
(17236, 1, 0, 0, 0, 0, 0, NULL),
(17253, 0, 0, 1, 0, 0, 0, NULL),
(17280, 1, 0, 0, 0, 0, 0, NULL),
(17290, 1, 1, 0, 0, 0, 0, NULL),
(17296, 1, 1, 0, 0, 0, 0, NULL),
(17307, 1, 1, 1, 0, 0, 0, NULL),
(17312, 1, 1, 0, 0, 0, 0, NULL),
(17316, 0, 1, 1, 0, 0, 0, NULL),
(17345, 1, 0, 0, 0, 0, 0, NULL),
(17347, 1, 0, 0, 0, 0, 0, NULL),
(17348, 1, 0, 0, 0, 0, 0, NULL),
(17376, 0, 0, 1, 0, 0, 0, NULL),
(17436, 0, 0, 1, 0, 0, 0, NULL),
(17463, 1, 1, 0, 0, 0, 0, NULL),
(17474, 1, 0, 0, 0, 0, 0, NULL),
(17522, 1, 0, 0, 0, 0, 0, NULL),
(17523, 1, 0, 0, 0, 0, 0, NULL),
(17536, 1, 1, 1, 0, 0, 0, NULL),
(17548, 1, 0, 0, 0, 0, 0, NULL),
(17552, 0, 0, 1, 0, 0, 0, NULL),
(17578, 1, 0, 0, 1, 0, 0, NULL),
(17592, 0, 0, 1, 0, 0, 0, NULL),
(17644, 0, 0, 1, 0, 0, 0, NULL),
(17645, 0, 0, 1, 0, 0, 0, NULL),
(17652, 0, 0, 1, 0, 0, 0, NULL),
(17661, 1, 0, 0, 0, 0, 0, NULL),
(17669, 1, 0, 0, 0, 0, 0, NULL),
(17673, 0, 1, 0, 0, 0, 0, NULL),
(17683, 1, 0, 0, 0, 0, 0, NULL),
(17733, 1, 0, 0, 0, 0, 0, NULL),
(17813, 1, 1, 0, 0, 0, 0, NULL),
(17814, 1, 1, 0, 0, 0, 0, NULL),
(17815, 1, 1, 0, 0, 0, 0, NULL),
(17828, 1, 0, 0, 0, 0, 0, NULL),
(17867, 0, 0, 1, 0, 0, 0, NULL),
(17883, 0, 0, 1, 0, 0, 0, NULL),
(17886, 0, 1, 1, 0, 0, 0, NULL),
(17888, 1, 1, 0, 0, 0, 0, NULL),
(17889, 0, 0, 1, 0, 0, 0, NULL),
(17906, 0, 0, 1, 0, 0, 0, NULL),
(17907, 0, 0, 1, 0, 0, 0, NULL),
(17915, 0, 1, 0, 0, 0, 0, NULL),
(17939, 1, 1, 0, 0, 0, 0, NULL),
(17947, 0, 0, 1, 0, 0, 0, NULL),
(17950, 0, 1, 1, 0, 0, 0, NULL),
(17965, 1, 0, 0, 0, 0, 0, NULL),
(17966, 1, 1, 0, 0, 0, 0, NULL),
(17974, 0, 0, 1, 0, 0, 0, NULL),
(17984, 0, 0, 1, 0, 0, 0, NULL),
(17988, 0, 0, 1, 0, 0, 0, NULL),
(17992, 0, 0, 1, 0, 0, 0, NULL),
(18033, 1, 0, 0, 0, 0, 0, NULL),
(18059, 1, 0, 0, 0, 0, 0, NULL),
(18108, 0, 0, 1, 0, 0, 0, NULL),
(18138, 0, 1, 0, 0, 0, 0, NULL),
(18173, 0, 0, 1, 0, 0, 0, NULL),
(18176, 1, 1, 0, 1, 0, 0, NULL),
(18208, 0, 0, 1, 0, 0, 0, NULL),
(18212, 0, 1, 0, 0, 0, 0, NULL),
(18225, 1, 0, 1, 0, 0, 0, NULL),
(18242, 0, 0, 1, 0, 0, 0, NULL),
(18263, 0, 0, 1, 0, 0, 0, NULL),
(18264, 0, 0, 1, 0, 0, 0, NULL),
(18275, 0, 1, 0, 0, 0, 0, NULL),
(18284, 0, 1, 0, 0, 0, 0, NULL),
(18318, 1, 0, 0, 0, 0, 0, NULL),
(18319, 1, 0, 0, 0, 0, 0, NULL),
(18320, 1, 0, 0, 0, 0, 0, NULL),
(18321, 1, 0, 0, 0, 0, 0, NULL),
(18322, 1, 0, 0, 0, 0, 0, NULL),
(18323, 1, 0, 0, 0, 0, 0, NULL),
(18325, 1, 0, 0, 0, 0, 0, NULL),
(18326, 1, 0, 0, 0, 0, 0, NULL),
(18327, 1, 0, 0, 0, 0, 0, NULL),
(18328, 1, 0, 0, 0, 0, 0, NULL),
(18329, 1, 0, 0, 0, 0, 0, NULL),
(18355, 1, 0, 0, 0, 0, 0, NULL),
(18366, 1, 0, 0, 0, 0, 0, NULL),
(18367, 1, 0, 0, 0, 0, 0, NULL),
(18371, 1, 1, 0, 0, 0, 0, NULL),
(18372, 1, 1, 0, 1, 0, 0, NULL),
(18373, 1, 1, 0, 0, 0, 0, NULL),
(18388, 0, 0, 1, 0, 0, 0, NULL),
(18393, 0, 0, 1, 0, 0, 0, NULL),
(18395, 0, 0, 1, 0, 0, 0, NULL),
(18409, 1, 1, 1, 0, 0, 0, NULL),
(18432, 1, 1, 1, 0, 0, 0, NULL),
(18435, 1, 1, 1, 0, 0, 0, NULL),
(18444, 0, 0, 1, 0, 0, 0, NULL),
(18466, 1, 1, 0, 0, 0, 0, NULL),
(18467, 1, 1, 0, 0, 0, 0, NULL),
(18472, 1, 1, 0, 0, 0, 0, NULL),
(18473, 1, 1, 0, 0, 0, 0, NULL),
(18476, 1, 1, 0, 0, 0, 0, NULL),
(18477, 1, 1, 0, 0, 0, 0, NULL),
(18493, 1, 1, 0, 0, 0, 0, NULL),
(18495, 1, 1, 0, 0, 0, 0, NULL),
(18497, 1, 1, 0, 0, 0, 0, NULL),
(18521, 1, 1, 0, 0, 0, 0, NULL),
(18524, 1, 1, 0, 0, 0, 0, NULL),
(18532, 0, 0, 1, 0, 0, 0, NULL),
(18542, 1, 1, 0, 0, 0, 0, NULL),
(18560, 0, 0, 1, 0, 0, 0, NULL),
(18563, 0, 0, 1, 0, 0, 0, NULL),
(18602, 0, 0, 1, 0, 0, 0, NULL),
(18647, 1, 1, 0, 0, 0, 0, NULL),
(18662, 0, 0, 1, 0, 0, 0, NULL),
(18670, 1, 1, 0, 0, 0, 0, NULL),
(18695, 1, 1, 0, 0, 0, 0, NULL),
(18699, 1, 1, 0, 0, 0, 0, NULL),
(18700, 1, 0, 0, 0, 0, 0, NULL),
(18702, 1, 1, 0, 0, 0, 0, NULL),
(18721, 0, 0, 1, 0, 0, 0, NULL),
(18726, 0, 0, 1, 0, 0, 0, NULL),
(18729, 0, 0, 1, 0, 0, 0, NULL),
(18734, 1, 1, 0, 1, 0, 0, NULL),
(18735, 1, 1, 0, 1, 0, 0, NULL),
(18736, 1, 1, 0, 1, 0, 0, NULL),
(18737, 1, 1, 0, 1, 0, 0, NULL),
(18738, 1, 1, 0, 1, 0, 0, NULL),
(18750, 0, 1, 0, 0, 0, 0, NULL),
(18759, 0, 0, 1, 0, 0, 0, NULL),
(18766, 0, 0, 1, 0, 0, 0, NULL),
(18778, 0, 0, 1, 0, 0, 0, NULL),
(18793, 0, 1, 1, 0, 0, 0, NULL),
(18814, 0, 0, 1, 0, 0, 0, NULL),
(18818, 0, 1, 1, 0, 0, 0, NULL),
(18828, 0, 0, 1, 0, 0, 0, NULL),
(18849, 0, 1, 1, 0, 0, 0, NULL),
(18879, 1, 1, 1, 0, 0, 0, NULL),
(18883, 0, 1, 1, 0, 0, 0, NULL),
(18896, 0, 0, 1, 0, 0, 0, NULL),
(18932, 0, 0, 1, 0, 0, 0, NULL),
(18963, 1, 1, 0, 0, 0, 0, NULL),
(18982, 1, 1, 0, 0, 0, 0, NULL),
(18983, 1, 1, 0, 0, 0, 0, NULL),
(18996, 1, 1, 0, 0, 0, 0, NULL),
(19008, 0, 1, 1, 0, 0, 0, NULL),
(19009, 0, 1, 1, 0, 0, 0, NULL),
(19022, 1, 1, 0, 0, 0, 0, NULL),
(19023, 1, 1, 0, 0, 0, 0, NULL),
(19024, 1, 1, 0, 0, 0, 0, NULL),
(19025, 1, 1, 0, 0, 0, 0, NULL),
(19026, 1, 1, 0, 0, 0, 0, NULL),
(19027, 1, 1, 0, 0, 0, 0, NULL),
(19030, 1, 1, 0, 0, 0, 0, NULL),
(19055, 0, 0, 1, 0, 0, 0, NULL),
(19211, 0, 0, 1, 0, 0, 0, NULL),
(19212, 0, 0, 1, 0, 0, 0, NULL),
(19297, 0, 0, 1, 0, 0, 0, NULL),
(19300, 0, 0, 1, 0, 0, 0, NULL),
(19301, 0, 0, 1, 0, 0, 0, NULL),
(19302, 0, 0, 1, 0, 0, 0, NULL),
(19303, 0, 0, 1, 0, 0, 0, NULL),
(19304, 0, 0, 1, 0, 0, 0, NULL),
(19382, 1, 1, 1, 0, 0, 0, NULL),
(19387, 0, 0, 1, 0, 0, 0, NULL),
(19388, 0, 0, 1, 0, 0, 0, NULL),
(19393, 0, 0, 1, 0, 0, 0, NULL),
(19399, 1, 1, 0, 1, 0, 0, NULL),
(19412, 0, 0, 1, 0, 0, 0, NULL),
(19421, 0, 1, 1, 0, 0, 0, NULL),
(19423, 1, 0, 0, 0, 0, 0, NULL),
(19428, 1, 0, 0, 0, 0, 0, NULL),
(19429, 1, 0, 0, 0, 0, 0, NULL),
(19437, 0, 0, 1, 0, 0, 0, NULL),
(19458, 1, 1, 0, 0, 0, 0, NULL),
(19459, 1, 1, 0, 0, 0, 0, NULL),
(19483, 0, 0, 1, 0, 0, 0, NULL),
(19484, 0, 0, 1, 0, 0, 0, NULL),
(19514, 0, 1, 1, 0, 0, 0, NULL),
(19555, 0, 0, 1, 0, 0, 0, NULL),
(19568, 0, 1, 1, 0, 0, 0, NULL),
(19590, 0, 0, 1, 0, 0, 0, NULL),
(19616, 1, 1, 0, 0, 0, 0, NULL),
(19640, 1, 0, 0, 0, 0, 0, NULL),
(19646, 0, 0, 1, 0, 0, 0, NULL),
(19655, 0, 0, 1, 0, 0, 0, NULL),
(19656, 0, 1, 1, 0, 0, 0, NULL),
(19660, 0, 0, 1, 0, 0, 0, NULL),
(19677, 0, 0, 1, 0, 0, 0, NULL),
(19711, 1, 0, 0, 0, 0, 0, NULL),
(19714, 1, 0, 0, 0, 0, 0, NULL),
(19859, 1, 1, 0, 0, 0, 0, NULL),
(19860, 1, 1, 0, 0, 0, 0, NULL),
(19861, 1, 1, 0, 0, 0, 0, NULL),
(19866, 0, 1, 1, 0, 0, 0, NULL),
(19867, 0, 1, 1, 0, 0, 0, NULL),
(19868, 0, 1, 1, 0, 0, 0, NULL),
(19871, 0, 0, 1, 0, 0, 0, NULL),
(19896, 1, 1, 0, 0, 0, 0, NULL),
(19897, 1, 1, 0, 1, 0, 0, NULL),
(19910, 1, 1, 0, 0, 0, 0, NULL),
(19918, 0, 0, 1, 0, 0, 0, NULL),
(19932, 1, 1, 0, 0, 0, 0, NULL),
(19933, 1, 1, 0, 0, 0, 0, NULL),
(19934, 1, 0, 0, 0, 0, 0, NULL),
(19935, 1, 1, 0, 0, 0, 0, NULL),
(19936, 1, 0, 0, 0, 0, 0, NULL),
(19938, 1, 1, 1, 0, 0, 0, NULL),
(19950, 1, 1, 0, 0, 0, 0, NULL),
(19951, 1, 0, 0, 0, 0, 0, NULL),
(19959, 0, 1, 1, 0, 0, 0, NULL),
(20001, 1, 0, 0, 0, 0, 0, NULL),
(20021, 1, 1, 0, 0, 0, 0, NULL),
(20025, 1, 1, 0, 0, 0, 0, NULL),
(20026, 1, 1, 0, 0, 0, 0, NULL),
(20027, 1, 1, 0, 0, 0, 0, NULL),
(20051, 1, 0, 0, 0, 0, 0, NULL),
(20053, 1, 1, 0, 0, 0, 0, NULL),
(20054, 1, 1, 0, 0, 0, 0, NULL),
(20055, 1, 1, 0, 0, 0, 0, NULL),
(20058, 1, 1, 0, 0, 0, 0, NULL),
(20061, 1, 1, 0, 1, 0, 0, NULL),
(20064, 1, 0, 0, 0, 0, 0, NULL),
(20069, 1, 0, 0, 0, 0, 0, NULL),
(20075, 1, 1, 0, 0, 0, 0, NULL),
(20080, 1, 1, 0, 0, 0, 0, NULL),
(20081, 1, 1, 0, 0, 0, 0, NULL),
(20082, 1, 0, 0, 0, 0, 0, NULL),
(20102, 1, 1, 0, 0, 0, 0, NULL),
(20127, 0, 0, 1, 0, 0, 0, NULL),
(20129, 1, 1, 0, 0, 0, 0, NULL),
(20130, 1, 0, 0, 0, 0, 0, NULL),
(20131, 1, 0, 0, 0, 0, 0, NULL),
(20142, 1, 0, 0, 0, 0, 0, NULL),
(20155, 0, 0, 1, 0, 0, 0, NULL),
(20156, 0, 0, 1, 0, 0, 0, NULL),
(20166, 0, 0, 1, 0, 0, 0, NULL),
(20167, 0, 0, 1, 0, 0, 0, NULL),
(20170, 1, 1, 1, 0, 0, 0, NULL),
(20172, 1, 0, 0, 0, 0, 0, NULL),
(20209, 0, 0, 1, 0, 0, 0, NULL),
(20212, 0, 0, 1, 0, 0, 0, NULL),
(20213, 0, 0, 1, 0, 0, 0, NULL),
(20226, 0, 0, 1, 0, 0, 0, NULL),
(20230, 0, 0, 1, 0, 0, 0, NULL),
(20237, 1, 1, 0, 0, 0, 0, NULL),
(20251, 1, 1, 1, 0, 0, 0, NULL),
(20290, 0, 1, 0, 0, 0, 0, NULL),
(20298, 1, 1, 0, 0, 0, 0, NULL),
(20299, 1, 1, 0, 0, 0, 0, NULL),
(20300, 1, 1, 0, 0, 0, 0, NULL),
(20301, 1, 1, 0, 0, 0, 0, NULL),
(20302, 1, 1, 0, 0, 0, 0, NULL),
(20304, 0, 0, 1, 0, 0, 0, NULL),
(20306, 1, 1, 0, 0, 0, 0, NULL),
(20307, 0, 0, 1, 0, 0, 0, NULL),
(20314, 0, 0, 1, 0, 0, 0, NULL),
(20315, 1, 1, 0, 0, 0, 0, NULL),
(20317, 1, 0, 0, 0, 0, 0, NULL),
(20318, 1, 1, 0, 0, 0, 0, NULL),
(20330, 1, 1, 0, 0, 0, 0, NULL),
(20392, 0, 1, 1, 0, 0, 0, NULL),
(20417, 0, 0, 1, 0, 0, 0, NULL),
(20418, 0, 0, 1, 0, 0, 0, NULL),
(20424, 1, 0, 0, 0, 0, 0, NULL),
(20431, 0, 0, 1, 0, 0, 0, NULL),
(20440, 0, 0, 1, 0, 0, 0, NULL),
(20454, 0, 1, 1, 0, 0, 0, NULL),
(20465, 0, 1, 0, 0, 0, 0, NULL),
(20499, 1, 1, 0, 0, 0, 0, NULL),
(20536, 1, 1, 0, 0, 0, 0, NULL),
(20537, 1, 1, 0, 0, 0, 0, NULL),
(20538, 1, 1, 0, 0, 0, 0, NULL),
(20539, 1, 1, 0, 0, 0, 0, NULL),
(20553, 0, 0, 1, 0, 0, 0, NULL),
(20561, 1, 1, 0, 0, 0, 0, NULL),
(20564, 0, 0, 1, 0, 0, 0, NULL),
(20574, 1, 0, 0, 0, 0, 0, NULL),
(20611, 1, 1, 1, 0, 0, 0, NULL),
(20617, 0, 0, 1, 0, 0, 0, NULL),
(20635, 1, 1, 1, 0, 0, 0, NULL),
(20654, 0, 1, 1, 0, 0, 0, NULL),
(20666, 0, 0, 1, 0, 0, 0, NULL),
(20667, 0, 1, 0, 0, 0, 0, NULL),
(20670, 0, 0, 1, 0, 0, 0, NULL),
(20671, 1, 1, 0, 0, 0, 0, NULL),
(20673, 1, 1, 0, 0, 0, 0, NULL),
(20686, 1, 0, 0, 0, 0, 0, NULL),
(20688, 1, 0, 0, 0, 0, 0, NULL),
(20690, 1, 1, 0, 0, 0, 0, NULL),
(20691, 1, 0, 0, 0, 0, 0, NULL),
(20692, 1, 0, 0, 0, 0, 0, NULL),
(20693, 1, 0, 0, 0, 0, 0, NULL),
(20694, 1, 0, 0, 0, 0, 0, NULL),
(20695, 1, 0, 0, 0, 0, 0, NULL),
(20696, 1, 0, 0, 0, 0, 0, NULL),
(20697, 1, 0, 0, 0, 0, 0, NULL),
(20698, 1, 0, 0, 0, 0, 0, NULL),
(20699, 1, 0, 0, 0, 0, 0, NULL),
(20701, 1, 0, 0, 0, 0, 0, NULL),
(20706, 1, 1, 0, 0, 0, 0, NULL),
(20713, 1, 0, 0, 0, 0, 0, NULL),
(20714, 1, 1, 0, 0, 0, 0, NULL),
(20728, 1, 1, 0, 0, 0, 0, NULL),
(20729, 1, 1, 0, 0, 0, 0, NULL),
(20736, 0, 0, 1, 0, 0, 0, NULL),
(20739, 0, 1, 1, 0, 0, 0, NULL),
(20748, 1, 1, 0, 0, 0, 0, NULL),
(20749, 1, 1, 0, 0, 0, 0, NULL),
(20751, 1, 1, 0, 0, 0, 0, NULL),
(20755, 0, 0, 1, 0, 0, 0, NULL),
(20764, 0, 0, 1, 0, 0, 0, NULL),
(20767, 0, 0, 1, 0, 0, 0, NULL),
(20769, 0, 0, 1, 0, 0, 0, NULL),
(20771, 0, 0, 1, 0, 0, 0, NULL),
(20781, 0, 0, 1, 0, 0, 0, NULL),
(20782, 0, 0, 1, 0, 0, 0, NULL),
(20797, 1, 0, 0, 0, 0, 0, NULL),
(20804, 0, 1, 1, 0, 0, 0, NULL),
(20809, 0, 0, 1, 0, 0, 0, NULL),
(20813, 0, 0, 1, 0, 0, 0, NULL),
(20814, 0, 0, 1, 0, 0, 0, NULL),
(20815, 0, 0, 1, 0, 0, 0, NULL),
(20816, 0, 0, 1, 0, 0, 0, NULL),
(20845, 0, 0, 1, 0, 0, 0, NULL),
(20851, 0, 0, 1, 0, 0, 0, NULL),
(20852, 0, 0, 1, 0, 0, 0, NULL),
(20853, 0, 0, 1, 0, 0, 0, NULL),
(20855, 0, 0, 1, 0, 0, 0, NULL),
(20856, 0, 0, 1, 0, 0, 0, NULL),
(20858, 0, 0, 1, 0, 0, 0, NULL),
(20899, 0, 0, 1, 0, 0, 0, NULL),
(20903, 1, 0, 0, 0, 0, 0, NULL),
(20926, 0, 0, 1, 0, 0, 0, NULL),
(20933, 0, 0, 1, 0, 0, 0, NULL),
(20998, 1, 1, 0, 0, 0, 0, NULL),
(21004, 1, 1, 0, 0, 0, 0, NULL),
(21022, 1, 1, 0, 0, 0, 0, NULL),
(21025, 0, 0, 1, 0, 0, 0, NULL),
(21029, 0, 1, 1, 0, 0, 0, NULL),
(21030, 0, 0, 1, 0, 0, 0, NULL),
(21033, 0, 1, 1, 0, 0, 0, NULL),
(21039, 0, 0, 1, 0, 0, 0, NULL),
(21041, 0, 0, 1, 0, 0, 0, NULL),
(21051, 0, 0, 1, 0, 0, 0, NULL),
(21052, 0, 0, 1, 0, 0, 0, NULL),
(21053, 0, 0, 1, 0, 0, 0, NULL),
(21054, 0, 0, 1, 0, 0, 0, NULL),
(21064, 1, 0, 0, 0, 0, 0, NULL),
(21071, 1, 1, 0, 1, 0, 0, NULL),
(21075, 0, 0, 1, 0, 0, 0, NULL),
(21090, 0, 0, 1, 0, 0, 0, NULL),
(21092, 0, 0, 1, 0, 0, 0, NULL),
(21094, 0, 0, 1, 0, 0, 0, NULL),
(21096, 0, 0, 1, 0, 0, 0, NULL),
(21122, 0, 0, 1, 0, 0, 0, NULL),
(21123, 1, 1, 0, 0, 0, 0, NULL),
(21124, 1, 1, 0, 0, 0, 0, NULL),
(21142, 0, 0, 1, 0, 0, 0, NULL),
(21153, 1, 1, 1, 0, 0, 0, NULL),
(21159, 0, 1, 1, 0, 0, 0, NULL),
(21170, 0, 0, 1, 0, 0, 0, NULL),
(21173, 0, 0, 1, 0, 0, 0, NULL),
(21182, 0, 0, 1, 0, 0, 0, NULL),
(21186, 0, 0, 1, 0, 0, 0, NULL),
(21210, 0, 1, 1, 0, 0, 0, NULL),
(21211, 0, 1, 1, 0, 0, 0, NULL),
(21212, 1, 1, 0, 0, 0, 0, NULL),
(21217, 0, 0, 1, 1, 0, 0, NULL),
(21219, 1, 1, 0, 0, 0, 0, NULL),
(21223, 1, 1, 0, 0, 0, 0, NULL),
(21233, 1, 1, 0, 1, 0, 0, NULL),
(21236, 0, 1, 1, 0, 0, 0, NULL),
(21237, 0, 1, 1, 0, 0, 0, NULL),
(21252, 0, 0, 1, 0, 0, 0, NULL),
(21261, 0, 0, 1, 0, 0, 0, NULL),
(21262, 0, 0, 1, 0, 0, 0, NULL),
(21292, 0, 1, 1, 0, 0, 0, NULL),
(21310, 0, 0, 1, 0, 0, 0, NULL),
(21315, 0, 1, 1, 0, 0, 0, NULL),
(21316, 0, 1, 1, 0, 0, 0, NULL),
(21322, 1, 1, 0, 1, 0, 0, NULL),
(21340, 1, 1, 0, 0, 0, 0, NULL),
(21348, 0, 1, 1, 0, 0, 0, NULL),
(21349, 0, 1, 1, 0, 0, 0, NULL),
(21352, 0, 0, 1, 0, 0, 0, NULL),
(21353, 0, 0, 1, 0, 0, 0, NULL),
(21363, 0, 0, 1, 0, 0, 0, NULL),
(21366, 0, 0, 1, 0, 0, 0, NULL),
(21369, 0, 0, 1, 0, 0, 0, NULL),
(21378, 1, 1, 0, 0, 0, 0, NULL),
(21393, 0, 0, 1, 0, 0, 0, NULL),
(21394, 0, 0, 1, 0, 0, 0, NULL),
(21403, 0, 1, 1, 0, 0, 0, NULL),
(21413, 0, 1, 1, 0, 0, 0, NULL),
(21417, 0, 1, 1, 0, 0, 0, NULL),
(21429, 0, 0, 1, 0, 0, 0, NULL),
(21436, 0, 0, 1, 0, 0, 0, NULL),
(21437, 0, 0, 1, 0, 0, 0, NULL),
(21438, 0, 0, 1, 0, 0, 0, NULL),
(21439, 0, 0, 1, 0, 0, 0, NULL),
(21440, 0, 0, 1, 0, 0, 0, NULL),
(21443, 0, 0, 1, 0, 0, 0, NULL),
(21445, 0, 0, 1, 0, 0, 0, NULL),
(21448, 1, 1, 0, 0, 0, 0, NULL),
(21451, 0, 0, 1, 0, 0, 0, NULL),
(21463, 0, 0, 1, 0, 0, 0, NULL),
(21489, 0, 0, 1, 0, 0, 0, NULL),
(21497, 0, 0, 1, 0, 0, 0, NULL),
(21508, 0, 1, 0, 0, 0, 0, NULL),
(21529, 1, 0, 0, 0, 0, 0, NULL),
(21530, 1, 0, 0, 0, 0, 0, NULL),
(21625, 0, 0, 1, 0, 0, 0, NULL),
(21643, 1, 1, 0, 0, 0, 0, NULL),
(21648, 1, 1, 1, 0, 0, 0, NULL),
(21657, 0, 1, 1, 0, 0, 0, NULL),
(21659, 0, 0, 1, 0, 0, 0, NULL),
(21719, 0, 1, 1, 0, 0, 0, NULL),
(21722, 1, 1, 1, 0, 0, 0, NULL),
(21723, 1, 1, 0, 0, 0, 0, NULL),
(21724, 1, 1, 0, 0, 0, 0, NULL),
(21757, 0, 1, 1, 0, 0, 0, NULL),
(21785, 1, 0, 0, 0, 0, 0, NULL),
(21801, 0, 0, 1, 0, 0, 0, NULL),
(21817, 1, 1, 0, 0, 0, 0, NULL),
(21820, 1, 1, 0, 0, 0, 0, NULL),
(21821, 1, 1, 0, 0, 0, 0, NULL),
(21823, 1, 1, 1, 0, 0, 0, NULL),
(21840, 0, 1, 1, 0, 0, 0, NULL),
(21876, 0, 0, 1, 0, 0, 0, NULL),
(21879, 1, 1, 1, 0, 0, 0, NULL),
(21891, 1, 0, 0, 0, 0, 0, NULL),
(21893, 0, 0, 1, 0, 0, 0, NULL),
(21898, 0, 0, 1, 0, 0, 0, NULL),
(21899, 0, 0, 1, 0, 0, 0, NULL),
(21903, 0, 0, 1, 0, 0, 0, NULL),
(21904, 1, 0, 0, 0, 0, 0, NULL),
(21930, 0, 0, 1, 0, 0, 0, NULL),
(21931, 0, 0, 1, 0, 0, 0, NULL),
(21933, 0, 0, 1, 0, 0, 0, NULL),
(21935, 0, 0, 1, 0, 0, 0, NULL),
(21942, 0, 0, 1, 0, 0, 0, NULL),
(21943, 0, 1, 0, 0, 0, 0, NULL),
(21944, 0, 0, 1, 0, 0, 0, NULL),
(21952, 1, 0, 0, 0, 0, 0, NULL),
(21956, 1, 1, 0, 0, 0, 0, NULL),
(21974, 0, 1, 1, 0, 0, 0, NULL),
(21976, 1, 0, 1, 0, 0, 0, NULL),
(21987, 0, 0, 1, 0, 0, 0, NULL),
(21988, 0, 0, 1, 0, 0, 0, NULL),
(21989, 1, 0, 0, 0, 0, 0, NULL),
(21990, 1, 0, 0, 0, 0, 0, NULL),
(21993, 0, 1, 1, 0, 0, 0, NULL),
(21996, 0, 1, 1, 0, 0, 0, NULL),
(21997, 0, 1, 1, 0, 0, 0, NULL),
(21999, 0, 0, 1, 0, 0, 0, NULL),
(22000, 1, 1, 1, 0, 0, 0, NULL),
(22001, 0, 0, 1, 0, 0, 0, NULL),
(22002, 0, 0, 1, 0, 0, 0, NULL),
(22003, 0, 0, 1, 0, 0, 0, NULL),
(22005, 1, 0, 0, 0, 0, 0, NULL),
(22006, 1, 1, 1, 0, 0, 0, NULL),
(22023, 0, 0, 1, 0, 0, 0, NULL),
(22027, 1, 0, 0, 0, 0, 0, NULL),
(22039, 0, 1, 1, 0, 0, 0, NULL),
(22040, 0, 0, 1, 0, 0, 0, NULL),
(22044, 1, 1, 0, 0, 0, 0, NULL),
(22052, 1, 1, 0, 0, 0, 0, NULL),
(22058, 0, 0, 1, 0, 0, 0, NULL),
(22063, 0, 0, 1, 0, 0, 0, NULL),
(22064, 1, 0, 1, 0, 0, 0, NULL),
(22065, 0, 1, 1, 0, 0, 0, NULL),
(22066, 0, 1, 1, 0, 0, 0, NULL),
(22067, 1, 0, 1, 0, 0, 0, NULL),
(22068, 0, 0, 1, 0, 0, 0, NULL),
(22069, 0, 1, 1, 0, 0, 0, NULL),
(22070, 0, 0, 1, 0, 0, 0, NULL),
(22071, 0, 1, 1, 0, 0, 0, NULL),
(22073, 0, 1, 1, 0, 0, 0, NULL),
(22077, 1, 0, 1, 0, 0, 0, NULL),
(22078, 0, 1, 1, 0, 0, 0, NULL),
(22079, 0, 1, 1, 0, 0, 0, NULL),
(22080, 0, 0, 1, 0, 0, 0, NULL),
(22085, 1, 0, 1, 0, 0, 0, NULL),
(22086, 0, 1, 1, 0, 0, 0, NULL),
(22087, 0, 1, 1, 0, 0, 0, NULL),
(22088, 0, 0, 1, 0, 0, 0, NULL),
(22089, 1, 0, 1, 0, 0, 0, NULL),
(22090, 0, 1, 1, 0, 0, 0, NULL),
(22096, 0, 0, 1, 0, 0, 0, NULL),
(22116, 0, 0, 1, 0, 0, 0, NULL),
(22122, 1, 0, 1, 0, 0, 0, NULL),
(22124, 0, 1, 1, 0, 0, 0, NULL),
(22125, 0, 1, 1, 0, 0, 0, NULL),
(22126, 0, 0, 1, 0, 0, 0, NULL),
(22132, 1, 1, 0, 0, 0, 0, NULL),
(22136, 1, 0, 0, 0, 0, 0, NULL),
(22139, 0, 1, 1, 0, 0, 0, NULL),
(22140, 1, 1, 1, 0, 0, 0, NULL),
(22162, 1, 1, 0, 0, 0, 0, NULL),
(22173, 1, 1, 0, 0, 0, 0, NULL),
(22177, 0, 1, 1, 0, 0, 0, NULL),
(22181, 1, 1, 1, 0, 0, 0, NULL),
(22207, 0, 0, 1, 0, 0, 0, NULL),
(22268, 0, 1, 1, 0, 0, 0, NULL),
(22274, 1, 1, 1, 0, 0, 0, NULL),
(22288, 0, 1, 1, 0, 0, 0, NULL),
(22293, 1, 0, 0, 0, 0, 0, 0),
(22315, 1, 1, 0, 1, 0, 0, NULL),
(22317, 0, 0, 1, 0, 0, 0, NULL),
(22326, 0, 0, 1, 0, 0, 0, NULL),
(22331, 1, 1, 0, 1, 0, 0, NULL),
(22336, 1, 1, 0, 1, 0, 0, NULL),
(22339, 1, 0, 1, 0, 0, 0, NULL),
(22340, 0, 1, 1, 0, 0, 0, NULL),
(22348, 0, 0, 1, 0, 0, 0, NULL),
(22350, 0, 0, 1, 0, 0, 0, NULL),
(22351, 0, 0, 1, 0, 0, 0, NULL),
(22358, 0, 0, 1, 0, 0, 0, NULL),
(22380, 0, 0, 1, 0, 0, 0, NULL),
(22395, 0, 1, 1, 0, 0, 0, NULL),
(22400, 0, 1, 1, 0, 0, 0, NULL),
(22401, 0, 0, 1, 0, 0, 0, NULL),
(22402, 0, 0, 1, 0, 0, 0, NULL),
(22403, 0, 0, 1, 0, 0, 0, NULL),
(22404, 0, 0, 1, 0, 0, 0, NULL),
(22405, 0, 0, 1, 0, 0, 0, NULL),
(22406, 0, 0, 1, 0, 0, 0, NULL),
(22423, 0, 0, 1, 0, 0, 0, NULL),
(22432, 0, 1, 1, 0, 0, 0, NULL),
(22448, 1, 1, 1, 0, 0, 0, NULL),
(22451, 1, 1, 0, 1, 0, 0, NULL),
(22457, 0, 1, 1, 0, 0, 0, NULL),
(22460, 0, 0, 1, 0, 0, 0, NULL),
(22461, 1, 1, 0, 1, 0, 0, NULL),
(22467, 0, 0, 1, 0, 0, 0, NULL),
(22495, 0, 0, 1, 0, 0, 0, NULL),
(22500, 0, 1, 1, 0, 0, 0, NULL),
(22515, 0, 0, 1, 0, 0, 0, NULL),
(22517, 0, 0, 1, 0, 0, 0, NULL),
(22581, 0, 0, 1, 0, 0, 0, NULL),
(22582, 0, 0, 1, 0, 0, 0, NULL),
(22583, 0, 0, 1, 0, 0, 0, NULL),
(22584, 0, 0, 1, 0, 0, 0, NULL),
(22585, 0, 0, 1, 0, 0, 0, NULL),
(22586, 0, 0, 1, 0, 0, 0, NULL),
(22737, 1, 0, 0, 0, 0, 0, NULL),
(22738, 1, 0, 0, 0, 0, 0, NULL),
(22740, 1, 1, 0, 0, 0, 0, NULL),
(22742, 1, 1, 0, 0, 0, 0, NULL),
(22798, 0, 0, 1, 0, 0, 0, NULL),
(22799, 0, 0, 1, 0, 0, 0, NULL),
(22800, 0, 0, 1, 0, 0, 0, NULL),
(22801, 0, 0, 1, 0, 0, 0, NULL),
(22829, 1, 1, 0, 0, 0, 0, NULL),
(22831, 1, 1, 0, 0, 0, 0, NULL),
(22835, 1, 1, 0, 0, 0, 0, NULL),
(22838, 1, 1, 0, 0, 0, 0, NULL),
(22839, 1, 1, 0, 0, 0, 0, NULL),
(22842, 0, 0, 1, 0, 0, 0, NULL),
(22843, 0, 0, 1, 0, 0, 0, NULL),
(22850, 0, 0, 1, 0, 0, 0, NULL),
(22851, 1, 1, 0, 0, 0, 0, NULL),
(22852, 1, 1, 0, 0, 0, 0, NULL),
(22854, 1, 1, 0, 0, 0, 0, NULL),
(22866, 1, 1, 0, 0, 0, 0, NULL),
(22870, 0, 1, 1, 0, 0, 0, NULL),
(22872, 1, 1, 0, 0, 0, 0, NULL),
(22886, 1, 1, 1, 0, 0, 0, NULL),
(22889, 1, 0, 0, 0, 0, 0, NULL),
(22902, 1, 0, 0, 0, 0, 0, NULL),
(22905, 1, 1, 0, 0, 0, 0, NULL),
(22918, 0, 0, 1, 0, 0, 0, NULL),
(22944, 1, 1, 0, 0, 0, 0, NULL),
(22961, 1, 1, 0, 0, 0, 0, NULL),
(22972, 1, 1, 1, 0, 0, 0, NULL),
(22974, 0, 0, 1, 0, 0, 0, NULL),
(22979, 0, 0, 1, 0, 0, 0, NULL),
(22980, 1, 1, 1, 0, 0, 0, NULL),
(22984, 0, 0, 1, 0, 0, 0, NULL),
(22991, 0, 0, 1, 0, 0, 0, NULL),
(23030, 1, 1, 1, 0, 0, 0, NULL),
(23033, 0, 0, 1, 0, 0, 0, NULL),
(23035, 1, 1, 0, 0, 0, 0, NULL),
(23036, 0, 0, 1, 0, 0, 0, NULL),
(23037, 0, 1, 1, 0, 0, 0, NULL),
(23046, 1, 1, 1, 0, 0, 0, NULL),
(23051, 1, 0, 1, 0, 0, 0, NULL),
(23054, 0, 0, 1, 0, 0, 0, NULL),
(23057, 1, 1, 1, 0, 0, 0, NULL),
(23058, 1, 1, 1, 0, 0, 0, NULL),
(23061, 1, 1, 1, 0, 0, 0, NULL),
(23076, 1, 1, 0, 1, 0, 0, NULL),
(23077, 1, 1, 1, 1, 0, 0, NULL),
(23082, 0, 0, 1, 0, 0, 0, NULL),
(23084, 0, 0, 1, 0, 0, 0, NULL),
(23095, 0, 0, 1, 0, 0, 0, NULL),
(23104, 0, 1, 1, 0, 0, 0, NULL),
(23123, 0, 0, 1, 0, 0, 0, NULL),
(23126, 1, 1, 0, 0, 0, 0, NULL),
(23188, 0, 1, 1, 0, 0, 0, NULL),
(23210, 0, 0, 1, 0, 0, 0, NULL),
(23225, 0, 0, 1, 0, 0, 0, NULL),
(23238, 1, 1, 0, 0, 0, 0, NULL),
(23253, 1, 1, 1, 0, 0, 0, NULL),
(23261, 1, 1, 1, 0, 0, 0, NULL),
(23281, 1, 1, 1, 0, 0, 0, NULL),
(23282, 0, 0, 1, 0, 0, 0, NULL),
(23283, 0, 0, 1, 0, 0, 0, NULL),
(23288, 0, 0, 1, 0, 0, 0, NULL),
(23292, 0, 0, 1, 0, 0, 0, NULL),
(23293, 0, 0, 1, 0, 0, 0, NULL),
(23294, 0, 0, 1, 0, 0, 0, NULL),
(23295, 0, 0, 1, 0, 0, 0, NULL),
(23296, 0, 0, 1, 0, 0, 0, NULL),
(23297, 0, 0, 1, 0, 0, 0, NULL),
(23298, 0, 0, 1, 0, 0, 0, NULL),
(23299, 0, 0, 1, 0, 0, 0, NULL),
(23304, 0, 0, 1, 0, 0, 0, NULL),
(23330, 1, 1, 1, 0, 0, 0, NULL),
(23351, 0, 0, 1, 0, 0, 0, NULL),
(23377, 1, 1, 1, 0, 0, 0, NULL),
(23412, 0, 0, 1, 0, 0, 0, NULL),
(23416, 0, 0, 1, 0, 0, 0, NULL),
(23417, 0, 0, 1, 0, 0, 0, NULL),
(23426, 0, 0, 1, 0, 0, 0, NULL),
(23440, 1, 1, 1, 0, 0, 0, NULL),
(23444, 0, 0, 1, 0, 0, 0, NULL),
(23448, 0, 0, 1, 0, 0, 0, NULL),
(23450, 0, 1, 1, 0, 0, 0, NULL),
(23472, 0, 0, 1, 0, 0, 0, NULL),
(23488, 0, 0, 1, 0, 0, 0, NULL),
(23491, 0, 0, 1, 0, 0, 0, NULL),
(23496, 0, 0, 1, 0, 0, 0, NULL),
(23499, 0, 0, 1, 0, 0, 0, NULL),
(23502, 0, 0, 1, 0, 0, 0, NULL),
(23534, 1, 1, 0, 0, 0, 0, NULL),
(23535, 1, 1, 0, 0, 0, 0, NULL),
(23536, 1, 1, 0, 0, 0, 0, NULL),
(23537, 0, 0, 1, 0, 0, 0, NULL),
(23543, 0, 0, 1, 0, 0, 0, NULL),
(23557, 0, 1, 1, 0, 0, 0, NULL),
(23567, 1, 1, 0, 0, 0, 0, NULL),
(23585, 0, 0, 1, 0, 0, 0, NULL),
(23616, 1, 0, 0, 0, 0, 0, NULL),
(23618, 1, 0, 0, 0, 0, 0, NULL),
(23619, 1, 1, 0, 0, 0, 0, NULL),
(23622, 1, 0, 0, 0, 0, 0, NULL),
(23623, 1, 1, 0, 0, 0, 0, NULL),
(23624, 1, 1, 0, 0, 0, 0, NULL),
(23625, 1, 1, 0, 0, 0, 0, NULL),
(23626, 1, 1, 0, 0, 0, 0, NULL),
(23665, 1, 1, 0, 0, 0, 0, NULL),
(23680, 1, 1, 1, 0, 0, 0, NULL),
(23687, 1, 0, 1, 0, 0, 0, NULL),
(23689, 0, 0, 1, 0, 0, 0, NULL),
(23693, 1, 1, 1, 0, 0, 0, NULL),
(23706, 0, 0, 1, 1, 0, 0, NULL),
(23727, 0, 0, 1, 0, 0, 0, NULL),
(23746, 0, 0, 1, 0, 0, 0, NULL),
(23751, 0, 0, 1, 0, 0, 0, NULL),
(23752, 0, 1, 1, 0, 0, 0, NULL),
(23753, 0, 1, 1, 0, 0, 0, NULL),
(23771, 1, 1, 0, 1, 0, 0, NULL),
(23772, 0, 0, 1, 0, 0, 0, NULL),
(23785, 0, 1, 0, 0, 0, 0, NULL),
(23803, 0, 0, 1, 0, 0, 0, NULL),
(23805, 0, 0, 1, 0, 0, 0, NULL),
(23815, 0, 0, 1, 0, 0, 0, NULL),
(23830, 0, 0, 1, 0, 0, 0, NULL),
(23837, 0, 0, 1, 0, 0, 0, NULL),
(23864, 1, 1, 1, 0, 0, 0, NULL),
(23867, 0, 0, 1, 0, 0, 0, NULL),
(23868, 0, 0, 1, 0, 0, 0, NULL),
(23869, 0, 0, 1, 0, 0, 0, NULL),
(23876, 1, 1, 0, 1, 0, 0, NULL),
(23884, 0, 0, 1, 0, 0, 0, NULL),
(23885, 0, 0, 1, 0, 0, 0, NULL),
(23899, 1, 1, 0, 0, 0, 0, NULL),
(23901, 0, 0, 1, 0, 0, 0, NULL),
(23907, 0, 0, 1, 0, 0, 0, NULL),
(23915, 0, 0, 1, 0, 0, 0, NULL),
(23916, 0, 0, 1, 0, 0, 0, NULL),
(23917, 0, 0, 1, 0, 0, 0, NULL),
(23928, 0, 1, 0, 0, 0, 0, NULL),
(23935, 0, 1, 1, 0, 0, 0, NULL),
(23945, 0, 1, 1, 0, 0, 0, NULL),
(23947, 0, 0, 1, 0, 0, 0, NULL),
(23957, 0, 0, 1, 0, 0, 0, NULL),
(23959, 0, 0, 1, 0, 0, 0, NULL),
(23968, 0, 0, 1, 0, 0, 0, NULL),
(23969, 1, 0, 1, 0, 0, 0, NULL),
(23972, 0, 0, 1, 0, 0, 0, NULL),
(23996, 1, 1, 0, 0, 0, 0, NULL),
(24013, 1, 1, 1, 0, 0, 0, NULL),
(24019, 0, 0, 1, 0, 0, 0, NULL),
(24021, 0, 0, 1, 0, 0, 0, NULL),
(24035, 1, 1, 0, 0, 0, 0, NULL),
(24042, 0, 0, 1, 0, 0, 0, NULL),
(24068, 0, 0, 1, 0, 0, 0, NULL),
(24083, 1, 1, 1, 0, 0, 0, NULL),
(24110, 0, 0, 1, 0, 0, 0, NULL),
(24118, 0, 1, 1, 0, 0, 0, NULL),
(24119, 0, 1, 1, 0, 0, 0, NULL),
(24132, 0, 0, 1, 0, 0, 0, NULL),
(24168, 0, 1, 1, 0, 0, 0, NULL),
(24169, 1, 1, 1, 0, 0, 0, NULL),
(24172, 1, 1, 1, 0, 0, 0, NULL),
(24187, 0, 1, 1, 0, 0, 0, NULL),
(24210, 1, 1, 0, 1, 0, 0, NULL),
(24221, 0, 1, 1, 0, 0, 0, NULL),
(24222, 1, 0, 1, 0, 0, 0, NULL),
(24223, 0, 0, 1, 0, 0, 0, NULL),
(24235, 0, 1, 1, 0, 0, 0, NULL),
(24238, 1, 1, 0, 0, 0, 0, NULL),
(24258, 0, 1, 1, 0, 0, 0, NULL),
(24260, 0, 1, 1, 0, 0, 0, NULL),
(24272, 1, 1, 1, 0, 0, 0, NULL),
(24285, 0, 1, 0, 0, 0, 0, NULL),
(24324, 1, 0, 0, 0, 0, 0, NULL),
(24325, 0, 0, 1, 0, 0, 0, NULL),
(24327, 0, 1, 1, 0, 0, 0, NULL),
(24336, 0, 0, 1, 0, 0, 0, NULL),
(24372, 0, 0, 1, 1, 0, 0, NULL),
(24373, 0, 0, 1, 1, 0, 0, NULL),
(24418, 1, 0, 0, 0, 0, 0, NULL),
(24440, 1, 1, 1, 0, 0, 0, NULL),
(24500, 0, 1, 0, 0, 0, 0, NULL),
(24514, 0, 0, 1, 0, 0, 0, NULL),
(24518, 0, 0, 1, 0, 0, 0, NULL),
(24551, 0, 0, 1, 0, 0, 0, NULL),
(24553, 1, 1, 0, 0, 0, 0, NULL),
(24554, 1, 1, 0, 0, 0, 0, NULL),
(24555, 1, 1, 0, 0, 0, 0, NULL),
(24556, 1, 1, 0, 0, 0, 0, NULL),
(24557, 1, 1, 0, 0, 0, 0, NULL),
(24558, 1, 1, 0, 0, 0, 0, NULL),
(24559, 1, 1, 0, 0, 0, 0, NULL),
(24560, 1, 1, 0, 0, 0, 0, NULL),
(24561, 1, 1, 0, 0, 0, 0, NULL),
(24563, 1, 1, 0, 0, 0, 0, NULL),
(24637, 0, 1, 0, 0, 0, 0, NULL),
(24649, 1, 0, 0, 0, 0, 0, NULL),
(24650, 1, 1, 0, 0, 0, 0, NULL),
(24656, 1, 1, 0, 0, 0, 0, NULL),
(24664, 1, 1, 0, 0, 0, 0, NULL),
(24666, 0, 0, 1, 0, 0, 0, NULL),
(24673, 1, 1, 1, 0, 0, 0, NULL),
(24674, 1, 1, 0, 0, 0, 0, NULL),
(24675, 1, 1, 0, 0, 0, 0, NULL),
(24683, 1, 1, 0, 0, 0, 0, NULL),
(24684, 1, 1, 0, 0, 0, 0, NULL),
(24685, 1, 1, 0, 0, 0, 0, NULL),
(24686, 1, 1, 0, 0, 0, 0, NULL),
(24687, 1, 1, 0, 0, 0, 0, NULL),
(24688, 1, 1, 0, 0, 0, 0, NULL),
(24689, 1, 1, 0, 0, 0, 0, NULL),
(24690, 1, 1, 0, 0, 0, 0, NULL),
(24692, 1, 0, 0, 0, 0, 0, NULL),
(24693, 1, 0, 0, 0, 0, 0, NULL),
(24695, 1, 0, 0, 0, 0, 0, NULL),
(24696, 1, 1, 0, 0, 0, 0, NULL),
(24697, 1, 1, 0, 0, 0, 0, NULL),
(24698, 1, 1, 0, 0, 0, 0, NULL),
(24699, 1, 0, 0, 0, 0, 0, NULL),
(24707, 0, 1, 1, 0, 0, 0, NULL),
(24708, 0, 0, 1, 0, 0, 0, NULL),
(24715, 1, 1, 0, 0, 0, 0, NULL),
(24722, 1, 1, 0, 0, 0, 0, NULL),
(24723, 1, 1, 0, 0, 0, 0, NULL),
(24744, 1, 1, 0, 0, 0, 0, NULL),
(24745, 1, 0, 0, 0, 0, 0, NULL),
(24747, 1, 1, 1, 0, 0, 0, NULL),
(24761, 1, 0, 1, 0, 0, 0, NULL),
(24762, 1, 1, 0, 0, 0, 0, NULL),
(24767, 0, 0, 1, 0, 0, 0, NULL),
(24769, 0, 1, 1, 0, 0, 0, NULL),
(24770, 0, 1, 1, 0, 0, 0, NULL),
(24777, 1, 1, 0, 0, 0, 0, NULL),
(24781, 1, 0, 0, 0, 0, 0, NULL),
(24783, 0, 1, 1, 0, 0, 0, NULL),
(24787, 1, 0, 1, 0, 0, 0, NULL),
(24792, 1, 1, 0, 1, 0, 0, NULL),
(24806, 1, 1, 1, 0, 0, 0, NULL),
(24808, 1, 1, 0, 0, 0, 0, NULL),
(24809, 1, 0, 0, 0, 0, 0, NULL),
(24813, 1, 1, 0, 0, 0, 0, NULL),
(24815, 1, 0, 0, 0, 0, 0, NULL),
(24822, 1, 1, 0, 0, 0, 0, NULL),
(24844, 1, 0, 1, 0, 0, 0, NULL),
(24848, 1, 0, 0, 0, 0, 0, NULL),
(24849, 0, 1, 1, 0, 0, 0, NULL),
(24850, 1, 1, 0, 0, 0, 0, NULL),
(24852, 0, 0, 1, 0, 0, 0, NULL),
(24854, 1, 0, 0, 0, 0, 0, NULL),
(24855, 1, 0, 0, 0, 0, 0, NULL),
(24857, 1, 1, 0, 0, 0, 0, NULL),
(24866, 1, 1, 0, 0, 0, 0, NULL),
(24867, 1, 0, 0, 0, 0, 0, NULL),
(24875, 0, 0, 1, 0, 0, 0, NULL),
(24876, 0, 0, 1, 0, 0, 0, NULL),
(24877, 0, 0, 1, 0, 0, 0, NULL),
(24881, 1, 1, 0, 0, 0, 0, NULL),
(24882, 1, 0, 0, 0, 0, 0, NULL),
(24883, 0, 0, 1, 0, 0, 0, NULL),
(24885, 1, 1, 0, 0, 0, 0, NULL),
(24886, 1, 1, 0, 0, 0, 0, NULL),
(24891, 1, 1, 0, 0, 0, 0, NULL),
(24892, 1, 1, 0, 0, 0, 0, NULL),
(24893, 0, 0, 1, 0, 0, 0, NULL),
(24895, 1, 1, 0, 0, 0, 0, NULL),
(24899, 0, 1, 0, 0, 0, 0, NULL),
(24903, 1, 0, 0, 0, 0, 0, NULL),
(24904, 1, 0, 0, 0, 0, 0, NULL),
(24907, 1, 0, 0, 0, 0, 0, NULL),
(24908, 1, 0, 0, 0, 0, 0, NULL),
(24916, 0, 0, 1, 0, 0, 0, NULL),
(24917, 1, 1, 0, 0, 0, 0, NULL),
(24918, 1, 1, 0, 0, 0, 0, NULL),
(24919, 1, 0, 0, 0, 0, 0, NULL),
(24920, 1, 1, 0, 0, 0, 0, NULL),
(24921, 0, 0, 1, 0, 0, 0, NULL),
(24922, 1, 0, 0, 0, 0, 0, NULL),
(24923, 1, 1, 0, 0, 0, 0, NULL),
(24928, 0, 0, 1, 0, 0, 0, NULL),
(24932, 1, 1, 0, 0, 0, 0, NULL),
(24933, 0, 1, 1, 0, 0, 0, NULL),
(24936, 1, 0, 0, 0, 0, 0, NULL),
(24937, 1, 1, 0, 0, 0, 0, NULL),
(24938, 1, 1, 0, 0, 0, 0, NULL),
(24939, 1, 1, 0, 0, 0, 0, NULL),
(24940, 1, 1, 0, 0, 0, 0, NULL),
(24941, 1, 1, 0, 0, 0, 0, NULL),
(24942, 1, 1, 0, 0, 0, 0, NULL),
(24943, 1, 1, 0, 0, 0, 0, NULL),
(24944, 1, 1, 0, 0, 0, 0, NULL),
(24945, 1, 1, 0, 0, 0, 0, NULL),
(24946, 1, 1, 0, 0, 0, 0, NULL),
(24947, 1, 1, 0, 0, 0, 0, NULL),
(24948, 1, 1, 0, 0, 0, 0, NULL),
(24949, 1, 1, 0, 0, 0, 0, NULL),
(24950, 1, 1, 0, 0, 0, 0, NULL),
(24951, 1, 1, 0, 0, 0, 0, NULL),
(24952, 1, 1, 0, 0, 0, 0, NULL),
(24953, 1, 1, 0, 0, 0, 0, NULL),
(24955, 1, 1, 0, 0, 0, 0, NULL),
(24958, 0, 0, 1, 0, 0, 0, NULL),
(24959, 0, 0, 1, 0, 0, 0, NULL),
(24960, 1, 1, 0, 0, 0, 0, NULL),
(24961, 1, 0, 0, 0, 0, 0, NULL),
(24962, 1, 1, 0, 0, 0, 0, NULL),
(24963, 1, 1, 0, 0, 0, 0, NULL),
(24964, 1, 1, 0, 0, 0, 0, NULL),
(24965, 1, 1, 0, 0, 0, 0, NULL),
(24966, 1, 1, 0, 0, 0, 0, NULL),
(24967, 1, 1, 0, 0, 0, 0, NULL),
(24972, 1, 0, 0, 0, 0, 0, NULL),
(24973, 0, 0, 1, 0, 0, 0, NULL),
(24976, 1, 1, 0, 0, 0, 0, NULL),
(24978, 1, 1, 0, 0, 0, 0, NULL),
(24979, 1, 1, 0, 0, 0, 0, NULL),
(24980, 0, 0, 1, 0, 0, 0, NULL),
(24981, 1, 0, 0, 0, 0, 0, NULL),
(24991, 0, 0, 1, 0, 0, 0, NULL),
(24994, 1, 1, 0, 0, 0, 0, NULL),
(24999, 1, 1, 0, 0, 0, 0, NULL),
(25000, 1, 1, 0, 0, 0, 0, NULL),
(25001, 1, 1, 0, 0, 0, 0, NULL),
(25002, 1, 1, 0, 0, 0, 0, NULL),
(25003, 1, 0, 0, 0, 0, 0, NULL),
(25004, 1, 0, 0, 0, 0, 0, NULL),
(25005, 1, 0, 0, 0, 0, 0, NULL),
(25008, 1, 1, 0, 0, 0, 0, NULL),
(25027, 1, 1, 0, 0, 0, 0, NULL),
(25028, 1, 1, 0, 0, 0, 0, NULL),
(25030, 1, 0, 0, 0, 0, 0, NULL),
(25031, 1, 0, 0, 0, 0, 0, NULL),
(25032, 1, 1, 0, 0, 0, 0, NULL),
(25033, 1, 0, 0, 0, 0, 0, NULL),
(25034, 1, 1, 0, 0, 0, 0, NULL),
(25035, 1, 1, 0, 0, 0, 0, NULL),
(25038, 1, 0, 1, 0, 0, 0, NULL),
(25039, 1, 1, 0, 0, 0, 0, NULL),
(25041, 1, 1, 0, 0, 0, 0, NULL),
(25042, 0, 0, 1, 0, 0, 0, NULL),
(25044, 1, 0, 0, 0, 0, 0, NULL),
(25046, 1, 1, 0, 0, 0, 0, NULL),
(25047, 0, 1, 1, 0, 0, 0, NULL),
(25049, 1, 0, 0, 0, 0, 0, NULL),
(25057, 1, 1, 0, 0, 0, 0, NULL),
(25059, 1, 1, 0, 0, 0, 0, NULL),
(25061, 1, 1, 0, 0, 0, 0, NULL),
(25063, 1, 1, 1, 0, 0, 0, NULL),
(25064, 1, 1, 0, 0, 0, 0, NULL),
(25065, 0, 0, 1, 0, 0, 0, NULL),
(25066, 0, 0, 1, 0, 0, 0, NULL),
(25067, 0, 0, 1, 0, 0, 0, NULL),
(25068, 1, 0, 0, 0, 0, 0, NULL),
(25073, 1, 1, 0, 0, 0, 0, NULL),
(25086, 1, 0, 0, 0, 0, 0, NULL),
(25087, 1, 1, 0, 0, 0, 0, NULL),
(25088, 1, 1, 0, 0, 0, 0, NULL),
(25090, 0, 0, 1, 0, 0, 0, NULL),
(25091, 0, 0, 1, 0, 0, 0, NULL),
(25092, 0, 0, 1, 0, 0, 0, NULL),
(25108, 1, 1, 0, 0, 0, 0, NULL),
(25114, 1, 0, 0, 0, 0, 0, NULL),
(25115, 1, 1, 0, 0, 0, 0, NULL),
(25116, 1, 1, 0, 0, 0, 0, NULL),
(25117, 1, 1, 0, 0, 0, 0, NULL),
(25118, 1, 1, 0, 0, 0, 0, NULL),
(25119, 1, 1, 0, 0, 0, 0, NULL),
(25120, 1, 1, 0, 0, 0, 0, NULL),
(25121, 1, 1, 0, 0, 0, 0, NULL),
(25122, 1, 1, 0, 0, 0, 0, NULL),
(25123, 1, 1, 0, 0, 0, 0, NULL),
(25124, 1, 1, 0, 0, 0, 0, NULL),
(25125, 1, 1, 0, 0, 0, 0, NULL),
(25126, 1, 1, 0, 0, 0, 0, NULL),
(25127, 1, 1, 0, 0, 0, 0, NULL),
(25128, 1, 1, 0, 0, 0, 0, NULL),
(25129, 1, 1, 0, 0, 0, 0, NULL),
(25130, 1, 1, 0, 0, 0, 0, NULL),
(25131, 1, 1, 0, 0, 0, 0, NULL),
(25132, 1, 1, 0, 0, 0, 0, NULL),
(25133, 1, 1, 0, 0, 0, 0, NULL),
(25134, 1, 0, 0, 0, 0, 0, NULL),
(25135, 1, 0, 0, 0, 0, 0, NULL),
(25136, 1, 0, 0, 0, 0, 0, NULL),
(25137, 1, 0, 0, 0, 0, 0, NULL),
(25138, 1, 0, 0, 0, 0, 0, NULL),
(25140, 1, 0, 0, 0, 0, 0, NULL),
(25141, 1, 0, 0, 0, 0, 0, NULL),
(25142, 1, 0, 0, 0, 0, 0, NULL),
(25143, 1, 0, 0, 0, 0, 0, NULL),
(25144, 1, 1, 1, 0, 0, 0, NULL),
(25153, 1, 0, 0, 0, 0, 0, NULL),
(25154, 0, 0, 1, 0, 0, 0, NULL),
(25155, 1, 0, 0, 0, 0, 0, NULL),
(25156, 0, 0, 1, 0, 0, 0, NULL),
(25157, 0, 0, 1, 0, 0, 0, NULL),
(25158, 1, 0, 0, 0, 0, 0, NULL),
(25160, 0, 1, 1, 0, 0, 0, NULL),
(25161, 1, 1, 0, 0, 0, 0, NULL),
(25162, 1, 1, 0, 0, 0, 0, NULL),
(25164, 1, 1, 0, 0, 0, 0, NULL),
(25165, 1, 1, 0, 0, 0, 0, NULL),
(25166, 1, 1, 0, 0, 0, 0, NULL),
(25167, 1, 0, 0, 0, 0, 0, NULL),
(25169, 1, 0, 0, 0, 0, 0, NULL),
(25170, 1, 0, 0, 0, 0, 0, NULL),
(25171, 0, 0, 1, 0, 0, 0, NULL),
(25172, 1, 0, 0, 0, 0, 0, NULL),
(25173, 0, 0, 1, 0, 0, 0, NULL),
(25174, 1, 0, 0, 0, 0, 0, NULL),
(25175, 1, 0, 0, 0, 0, 0, NULL),
(25178, 1, 1, 0, 0, 0, 0, NULL),
(25192, 0, 0, 1, 0, 0, 0, NULL),
(25203, 0, 1, 0, 0, 0, 0, NULL),
(25204, 0, 1, 0, 0, 0, 0, NULL),
(25213, 1, 0, 0, 0, 0, 0, NULL),
(25214, 1, 1, 0, 0, 0, 0, NULL),
(25218, 1, 0, 0, 0, 0, 0, NULL),
(25219, 0, 0, 1, 0, 0, 0, NULL),
(25223, 1, 0, 0, 0, 0, 0, NULL),
(25225, 1, 0, 0, 0, 0, 0, NULL),
(25226, 0, 1, 0, 0, 0, 0, NULL),
(25229, 0, 0, 1, 0, 0, 0, NULL),
(25230, 0, 0, 1, 0, 0, 0, NULL),
(25231, 0, 0, 1, 0, 0, 0, NULL),
(25232, 0, 0, 1, 0, 0, 0, NULL),
(25236, 1, 0, 0, 0, 0, 0, NULL),
(25246, 1, 1, 0, 0, 0, 0, NULL),
(25265, 1, 0, 0, 0, 0, 0, NULL),
(25267, 1, 0, 0, 0, 0, 0, NULL),
(25268, 1, 1, 0, 0, 0, 0, NULL),
(25284, 1, 1, 0, 1, 0, 0, NULL),
(25286, 0, 1, 1, 0, 0, 0, NULL),
(25303, 1, 0, 0, 0, 0, 0, NULL),
(25305, 1, 0, 1, 1, 0, 0, NULL),
(25308, 0, 0, 1, 0, 0, 0, NULL),
(25309, 0, 0, 1, 0, 0, 0, NULL),
(25310, 0, 0, 1, 0, 0, 0, NULL),
(25315, 1, 0, 0, 1, 0, 0, NULL),
(25319, 1, 1, 0, 0, 0, 0, NULL),
(25324, 1, 0, 0, 0, 0, 0, NULL),
(25325, 1, 0, 0, 0, 0, 0, NULL),
(25344, 1, 0, 0, 0, 0, 0, NULL),
(25345, 1, 0, 0, 0, 0, 0, NULL),
(25346, 1, 0, 0, 0, 0, 0, NULL),
(25347, 1, 0, 0, 0, 0, 0, NULL),
(25348, 1, 0, 0, 0, 0, 0, NULL),
(25349, 1, 1, 1, 0, 0, 0, NULL),
(25354, 1, 0, 0, 0, 0, 0, NULL),
(25357, 0, 0, 1, 0, 0, 0, NULL),
(25358, 0, 0, 1, 0, 0, 0, NULL),
(25360, 1, 0, 0, 0, 0, 0, NULL),
(25363, 1, 1, 0, 0, 0, 0, NULL),
(25364, 0, 1, 1, 0, 0, 0, NULL),
(25367, 1, 1, 0, 0, 0, 0, NULL),
(25368, 1, 1, 0, 0, 0, 0, NULL),
(25369, 1, 1, 0, 0, 0, 0, NULL),
(25370, 1, 1, 0, 0, 0, 0, NULL),
(25371, 1, 1, 0, 0, 0, 0, NULL),
(25372, 1, 1, 0, 0, 0, 0, NULL),
(25373, 1, 1, 0, 0, 0, 0, NULL),
(25387, 1, 1, 1, 0, 0, 0, NULL),
(25397, 0, 1, 1, 0, 0, 0, NULL),
(25398, 0, 1, 1, 0, 0, 0, NULL),
(25399, 0, 1, 1, 0, 0, 0, NULL),
(25445, 1, 0, 1, 0, 0, 0, NULL),
(25451, 2, 1, 1, 0, 0, 0, NULL),
(25453, 2, 1, 1, 0, 0, 0, NULL),
(25460, 1, 0, 1, 0, 0, 0, NULL),
(25464, 1, 1, 1, 0, 0, 0, NULL),
(25466, 0, 0, 1, 0, 0, 0, NULL),
(25479, 1, 1, 1, 0, 0, 0, NULL),
(25483, 1, 1, 0, 0, 0, 0, NULL),
(25484, 1, 1, 0, 0, 0, 0, NULL),
(25485, 1, 1, 0, 0, 0, 0, NULL),
(25486, 1, 1, 0, 0, 0, 0, NULL),
(25491, 1, 0, 0, 0, 0, 0, NULL),
(25499, 1, 0, 0, 0, 0, 0, NULL),
(25500, 1, 0, 0, 0, 0, 0, NULL),
(25502, 1, 0, 0, 0, 0, 0, NULL),
(25505, 0, 0, 1, 0, 0, 0, NULL),
(25506, 1, 1, 0, 0, 0, 0, NULL),
(25507, 1, 1, 0, 0, 0, 0, NULL),
(25508, 1, 1, 0, 0, 0, 0, NULL),
(25509, 1, 1, 0, 0, 0, 0, NULL),
(25510, 0, 0, 1, 0, 0, 0, NULL),
(25511, 0, 0, 1, 0, 0, 0, NULL),
(25512, 0, 0, 1, 0, 0, 0, NULL),
(25513, 0, 0, 1, 0, 0, 0, NULL),
(25515, 1, 0, 0, 0, 0, 0, NULL),
(25518, 1, 1, 0, 0, 0, 0, NULL),
(25525, 0, 0, 1, 0, 0, 0, NULL),
(25534, 1, 1, 0, 1, 0, 0, NULL),
(25535, 0, 0, 1, 0, 0, 0, NULL),
(25536, 0, 0, 1, 0, 0, 0, NULL),
(25537, 1, 0, 0, 0, 0, 0, NULL),
(25538, 1, 1, 0, 0, 0, 0, NULL),
(25539, 1, 1, 0, 0, 0, 0, NULL),
(25540, 1, 1, 0, 0, 0, 0, NULL),
(25541, 1, 1, 0, 0, 0, 0, NULL),
(25542, 1, 0, 0, 0, 0, 0, NULL),
(25543, 0, 0, 1, 0, 0, 0, NULL),
(25544, 1, 0, 0, 0, 0, 0, NULL),
(25545, 1, 0, 1, 0, 0, 0, NULL),
(25546, 1, 1, 0, 0, 0, 0, NULL),
(25547, 1, 1, 0, 0, 0, 0, NULL),
(25548, 1, 1, 0, 0, 0, 0, NULL),
(25549, 1, 1, 0, 0, 0, 0, NULL),
(25550, 1, 1, 0, 0, 0, 0, NULL),
(25551, 1, 1, 0, 0, 0, 0, NULL),
(25552, 1, 1, 0, 0, 0, 0, NULL),
(25553, 1, 1, 0, 0, 0, 0, NULL),
(25554, 0, 0, 1, 0, 0, 0, NULL),
(25555, 1, 1, 0, 0, 0, 0, NULL),
(25556, 1, 1, 0, 0, 0, 0, NULL),
(25557, 1, 0, 0, 0, 0, 0, NULL),
(25558, 1, 0, 0, 0, 0, 0, NULL),
(25559, 1, 0, 0, 0, 0, 0, NULL),
(25560, 1, 1, 0, 0, 0, 0, NULL),
(25561, 1, 0, 0, 0, 0, 0, NULL),
(25562, 1, 1, 0, 0, 0, 0, NULL),
(25563, 1, 1, 0, 0, 0, 0, NULL),
(25565, 1, 1, 0, 0, 0, 0, NULL),
(25566, 1, 0, 0, 0, 0, 0, NULL),
(25567, 1, 1, 0, 0, 0, 0, NULL),
(25568, 1, 1, 0, 0, 0, 0, NULL),
(25569, 1, 1, 0, 0, 0, 0, NULL),
(25570, 1, 1, 0, 0, 0, 0, NULL),
(25571, 1, 1, 0, 0, 0, 0, NULL),
(25572, 1, 1, 0, 0, 0, 0, NULL),
(25573, 1, 1, 0, 0, 0, 0, NULL),
(25574, 1, 1, 0, 0, 0, 0, NULL),
(25575, 1, 1, 0, 0, 0, 0, NULL),
(25576, 1, 1, 0, 0, 0, 0, NULL),
(25577, 1, 1, 0, 0, 0, 0, NULL),
(25578, 1, 1, 0, 0, 0, 0, NULL),
(25579, 1, 1, 0, 0, 0, 0, NULL),
(25580, 1, 0, 0, 0, 0, 0, NULL),
(25581, 0, 0, 1, 0, 0, 0, NULL),
(25582, 0, 0, 1, 0, 0, 0, NULL),
(25584, 0, 0, 1, 0, 0, 0, NULL),
(25588, 1, 1, 0, 0, 0, 0, NULL),
(25591, 1, 1, 0, 0, 0, 0, NULL),
(25592, 1, 1, 0, 0, 0, 0, NULL),
(25593, 1, 1, 0, 0, 0, 0, NULL),
(25594, 0, 0, 1, 0, 0, 0, NULL),
(25595, 1, 1, 0, 0, 0, 0, NULL),
(25597, 1, 1, 0, 0, 0, 0, NULL),
(25598, 1, 1, 0, 0, 0, 0, NULL),
(25599, 1, 1, 0, 0, 0, 0, NULL),
(25603, 1, 0, 0, 0, 0, 0, NULL),
(25608, 1, 0, 0, 0, 0, 0, NULL),
(25613, 1, 1, 0, 0, 0, 0, NULL),
(25633, 1, 1, 0, 0, 0, 0, NULL),
(25634, 1, 1, 0, 0, 0, 0, NULL),
(25640, 1, 0, 0, 0, 0, 0, NULL),
(25653, 1, 1, 0, 0, 0, 0, NULL),
(25654, 0, 0, 1, 0, 0, 0, NULL),
(25662, 1, 1, 0, 0, 0, 0, NULL),
(25663, 1, 1, 0, 0, 0, 0, NULL),
(25664, 0, 0, 1, 0, 0, 0, NULL),
(25665, 0, 0, 1, 0, 0, 0, NULL),
(25666, 0, 0, 1, 0, 0, 0, NULL),
(25683, 1, 0, 0, 0, 0, 0, NULL),
(25695, 1, 0, 0, 0, 0, 0, NULL),
(25697, 1, 0, 0, 0, 0, 0, NULL),
(25703, 1, 0, 0, 0, 0, 0, NULL),
(25704, 1, 0, 0, 0, 0, 0, NULL),
(25706, 1, 0, 0, 0, 0, 0, NULL),
(25708, 1, 1, 0, 0, 0, 0, NULL),
(25710, 1, 0, 0, 0, 0, 0, NULL),
(25711, 0, 1, 1, 0, 0, 0, NULL),
(25721, 0, 1, 1, 0, 0, 0, NULL),
(25727, 0, 0, 1, 0, 0, 0, NULL),
(25731, 1, 0, 0, 0, 0, 0, NULL),
(25732, 0, 0, 1, 0, 0, 0, NULL),
(25733, 0, 0, 1, 0, 0, 0, NULL),
(25735, 1, 0, 0, 0, 0, 0, NULL),
(25740, 1, 0, 0, 0, 0, 0, NULL),
(25741, 1, 0, 0, 0, 0, 0, NULL),
(25744, 1, 0, 0, 0, 0, 0, NULL),
(25745, 1, 0, 0, 0, 0, 0, NULL),
(25746, 1, 0, 0, 0, 0, 0, NULL),
(25754, 1, 0, 0, 0, 0, 0, NULL),
(25755, 1, 0, 0, 0, 0, 0, NULL),
(25756, 1, 0, 0, 0, 0, 0, NULL),
(25757, 1, 0, 0, 0, 0, 0, NULL),
(25762, 0, 0, 1, 0, 0, 0, NULL),
(25765, 1, 1, 1, 0, 0, 0, NULL),
(25770, 0, 0, 1, 0, 0, 0, NULL),
(25772, 1, 0, 0, 0, 0, 0, NULL),
(25782, 1, 0, 0, 0, 0, 0, NULL),
(25795, 0, 0, 1, 0, 0, 0, NULL),
(25796, 0, 0, 1, 0, 0, 0, NULL),
(25798, 1, 1, 0, 0, 0, 0, NULL),
(25799, 1, 1, 0, 0, 0, 0, NULL),
(25815, 0, 0, 1, 0, 0, 0, NULL),
(25819, 0, 1, 1, 0, 0, 0, NULL),
(25824, 1, 0, 0, 0, 0, 0, NULL),
(25837, 1, 1, 0, 0, 0, 0, NULL),
(25840, 1, 0, 0, 0, 0, 0, NULL),
(25848, 0, 0, 1, 0, 0, 0, NULL),
(25851, 1, 1, 0, 0, 0, 0, NULL),
(25852, 1, 0, 0, 0, 0, 0, NULL),
(25853, 1, 0, 0, 0, 0, 0, NULL),
(25854, 1, 0, 0, 0, 0, 0, NULL),
(25855, 1, 0, 0, 0, 0, 0, NULL),
(25856, 1, 0, 0, 0, 0, 0, NULL),
(25857, 1, 0, 0, 0, 0, 0, NULL),
(25858, 1, 0, 0, 0, 0, 0, NULL),
(25859, 1, 0, 0, 0, 0, 0, NULL),
(25860, 1, 0, 0, 0, 0, 0, NULL),
(25863, 1, 1, 0, 0, 0, 0, NULL),
(25864, 1, 1, 0, 0, 0, 0, NULL),
(25865, 1, 0, 0, 0, 0, 0, NULL),
(25866, 1, 1, 0, 0, 0, 0, NULL),
(25867, 1, 1, 0, 0, 0, 0, NULL),
(25868, 1, 0, 0, 0, 0, 0, NULL),
(25869, 1, 0, 0, 0, 0, 0, NULL),
(25870, 1, 0, 0, 0, 0, 0, NULL),
(25871, 1, 0, 0, 0, 0, 0, NULL),
(25872, 1, 0, 0, 0, 0, 0, NULL),
(25873, 1, 0, 0, 0, 0, 0, NULL),
(25874, 1, 0, 0, 0, 0, 0, NULL),
(25875, 1, 0, 0, 0, 0, 0, NULL),
(25876, 1, 0, 0, 0, 0, 0, NULL),
(25877, 1, 0, 0, 0, 0, 0, NULL),
(25878, 1, 0, 0, 0, 0, 0, NULL),
(25879, 0, 0, 1, 0, 0, 0, NULL),
(25882, 1, 0, 0, 0, 0, 0, NULL),
(25883, 1, 0, 0, 0, 0, 0, NULL),
(25884, 1, 0, 0, 0, 0, 0, NULL),
(25885, 1, 0, 0, 0, 0, 0, NULL),
(25886, 1, 0, 0, 0, 0, 0, NULL),
(25887, 1, 0, 0, 0, 0, 0, NULL),
(25888, 1, 0, 0, 0, 0, 0, NULL),
(25889, 1, 0, 0, 0, 0, 0, NULL),
(25890, 1, 0, 0, 0, 0, 0, NULL),
(25891, 1, 0, 0, 0, 0, 0, NULL),
(25892, 1, 0, 0, 0, 0, 0, NULL),
(25893, 1, 0, 0, 0, 0, 0, NULL),
(25894, 1, 0, 0, 0, 0, 0, NULL),
(25895, 1, 0, 0, 0, 0, 0, NULL),
(25896, 1, 0, 0, 0, 0, 0, NULL),
(25897, 1, 0, 0, 0, 0, 0, NULL),
(25898, 1, 0, 0, 0, 0, 0, NULL),
(25899, 1, 0, 0, 0, 0, 0, NULL),
(25900, 1, 0, 0, 0, 0, 0, NULL),
(25901, 1, 0, 0, 0, 0, 0, NULL),
(25902, 1, 0, 0, 0, 0, 0, NULL),
(25903, 1, 0, 0, 0, 0, 0, NULL),
(25904, 1, 0, 0, 0, 0, 0, NULL),
(25905, 1, 0, 0, 0, 0, 0, NULL),
(25906, 1, 0, 0, 0, 0, 0, NULL),
(25907, 1, 0, 0, 0, 0, 0, NULL),
(25908, 1, 0, 0, 0, 0, 0, NULL),
(25909, 1, 0, 0, 0, 0, 0, NULL),
(25910, 1, 0, 0, 0, 0, 0, NULL),
(25911, 1, 0, 0, 0, 0, 0, NULL),
(25912, 1, 0, 0, 0, 0, 0, NULL),
(25913, 1, 0, 0, 0, 0, 0, NULL),
(25914, 1, 0, 0, 0, 0, 0, NULL),
(25915, 1, 0, 0, 0, 0, 0, NULL),
(25916, 1, 0, 0, 0, 0, 0, NULL),
(25917, 1, 0, 0, 0, 0, 0, NULL),
(25918, 1, 0, 0, 0, 0, 0, NULL),
(25919, 1, 0, 0, 0, 0, 0, NULL),
(25920, 1, 0, 0, 0, 0, 0, NULL),
(25921, 1, 0, 0, 0, 0, 0, NULL),
(25922, 1, 0, 0, 0, 0, 0, NULL),
(25923, 1, 0, 0, 0, 0, 0, NULL),
(25924, 1, 1, 0, 0, 0, 0, NULL),
(25925, 1, 0, 0, 0, 0, 0, NULL),
(25926, 1, 0, 0, 0, 0, 0, NULL),
(25927, 1, 0, 0, 0, 0, 0, NULL),
(25928, 1, 0, 0, 0, 0, 0, NULL),
(25929, 1, 0, 0, 0, 0, 0, NULL),
(25930, 1, 0, 0, 0, 0, 0, NULL),
(25931, 1, 0, 0, 0, 0, 0, NULL),
(25932, 1, 0, 0, 0, 0, 0, NULL),
(25933, 1, 0, 0, 0, 0, 0, NULL),
(25934, 1, 0, 0, 0, 0, 0, NULL),
(25935, 1, 0, 0, 0, 0, 0, NULL),
(25936, 1, 0, 0, 0, 0, 0, NULL),
(25937, 1, 0, 0, 0, 0, 0, NULL),
(25938, 1, 0, 0, 0, 0, 0, NULL),
(25939, 1, 0, 0, 0, 0, 0, NULL),
(25940, 1, 0, 0, 0, 0, 0, NULL),
(25941, 1, 0, 0, 0, 0, 0, NULL),
(25942, 1, 0, 0, 0, 0, 0, NULL),
(25943, 1, 0, 0, 0, 0, 0, NULL),
(25944, 1, 0, 0, 0, 0, 0, NULL),
(25945, 1, 0, 0, 0, 0, 0, NULL),
(25946, 1, 0, 0, 0, 0, 0, NULL),
(25947, 1, 0, 0, 0, 0, 0, NULL),
(25948, 1, 1, 0, 0, 0, 0, NULL),
(25949, 1, 1, 0, 0, 0, 0, NULL),
(25951, 1, 1, 0, 0, 0, 0, NULL),
(25952, 1, 0, 0, 0, 0, 0, NULL),
(25953, 0, 0, 1, 0, 0, 0, NULL),
(25954, 1, 1, 0, 0, 0, 0, NULL),
(25955, 1, 1, 0, 0, 0, 0, NULL),
(25956, 1, 1, 0, 0, 0, 0, NULL),
(25957, 1, 1, 0, 0, 0, 0, NULL),
(25958, 1, 1, 0, 0, 0, 0, NULL),
(25959, 1, 1, 0, 0, 0, 0, NULL),
(25960, 1, 0, 0, 0, 0, 0, NULL),
(25961, 1, 0, 0, 0, 0, 0, NULL),
(25962, 1, 0, 0, 0, 0, 0, NULL),
(25964, 1, 1, 1, 0, 0, 0, NULL),
(25965, 1, 1, 1, 0, 0, 0, NULL),
(25966, 1, 1, 1, 0, 0, 0, NULL),
(25967, 1, 0, 0, 0, 0, 0, NULL),
(25970, 1, 0, 0, 0, 0, 0, NULL),
(25971, 1, 0, 0, 0, 0, 0, NULL),
(25972, 1, 0, 0, 0, 0, 0, NULL),
(25973, 1, 0, 0, 0, 0, 0, NULL),
(25974, 1, 0, 0, 0, 0, 0, NULL),
(25975, 1, 0, 0, 0, 0, 0, NULL),
(25976, 1, 0, 0, 0, 0, 0, NULL),
(25977, 1, 0, 0, 0, 0, 0, NULL),
(25980, 1, 1, 0, 0, 0, 0, NULL),
(25985, 1, 0, 0, 0, 0, 0, NULL),
(25991, 1, 1, 0, 0, 0, 0, NULL),
(25992, 1, 1, 0, 0, 0, 0, NULL),
(25994, 1, 0, 0, 0, 0, 0, NULL),
(26007, 1, 0, 0, 0, 0, 0, NULL),
(26012, 1, 0, 0, 0, 0, 0, NULL),
(26016, 1, 0, 0, 0, 0, 0, NULL),
(26017, 1, 0, 0, 0, 0, 0, NULL),
(26019, 1, 0, 0, 0, 0, 0, NULL),
(26020, 1, 0, 0, 0, 0, 0, NULL),
(26021, 1, 0, 0, 0, 0, 0, NULL),
(26024, 1, 0, 0, 0, 0, 0, NULL),
(26027, 1, 0, 0, 0, 0, 0, NULL),
(26028, 1, 0, 0, 0, 0, 0, NULL),
(26029, 1, 0, 0, 0, 0, 0, NULL),
(26030, 1, 0, 0, 0, 0, 0, NULL),
(26031, 1, 0, 0, 0, 0, 0, NULL),
(26032, 1, 0, 0, 0, 0, 0, NULL),
(26033, 1, 0, 0, 0, 0, 0, NULL),
(26037, 1, 0, 0, 0, 0, 0, NULL),
(26038, 1, 0, 0, 0, 0, 0, NULL),
(26042, 1, 1, 0, 0, 0, 0, NULL),
(26043, 0, 0, 1, 0, 0, 0, NULL),
(26046, 1, 0, 1, 0, 0, 0, NULL),
(26050, 1, 1, 0, 0, 0, 0, NULL),
(26056, 1, 1, 0, 0, 0, 0, NULL),
(26057, 1, 1, 1, 0, 0, 0, NULL),
(26073, 1, 1, 0, 0, 0, 0, NULL),
(26075, 1, 0, 0, 0, 0, 0, NULL),
(26082, 0, 0, 1, 0, 0, 0, NULL),
(26090, 1, 1, 0, 0, 0, 0, NULL),
(26091, 1, 1, 0, 0, 0, 0, NULL),
(26092, 1, 1, 0, 0, 0, 0, NULL),
(26101, 1, 1, 0, 0, 0, 0, NULL),
(26102, 1, 1, 0, 0, 0, 0, NULL),
(26111, 1, 0, 0, 0, 0, 0, NULL),
(26113, 1, 0, 0, 0, 0, 0, NULL),
(26114, 0, 0, 1, 0, 0, 0, NULL),
(26116, 1, 1, 0, 0, 0, 0, NULL),
(26119, 1, 0, 0, 0, 0, 0, NULL),
(26120, 0, 0, 1, 0, 0, 0, NULL),
(26121, 1, 0, 0, 0, 0, 0, NULL),
(26123, 1, 0, 0, 0, 0, 0, NULL),
(26124, 1, 0, 0, 0, 0, 0, NULL),
(26127, 0, 1, 1, 0, 0, 0, NULL),
(26131, 1, 0, 0, 0, 0, 0, NULL),
(26161, 0, 0, 1, 0, 0, 0, NULL),
(26164, 1, 0, 0, 0, 0, 0, NULL),
(26166, 1, 1, 0, 0, 0, 0, NULL),
(26174, 1, 1, 1, 0, 0, 0, NULL),
(26176, 1, 1, 0, 0, 0, 0, NULL),
(26177, 1, 0, 0, 0, 0, 0, NULL),
(26178, 1, 0, 0, 0, 0, 0, NULL),
(26188, 0, 0, 1, 0, 0, 0, NULL),
(26190, 1, 1, 1, 0, 0, 0, NULL),
(26192, 1, 0, 0, 0, 0, 0, NULL),
(26204, 1, 1, 0, 0, 0, 0, NULL),
(26207, 1, 0, 0, 0, 0, 0, NULL),
(26214, 1, 0, 0, 0, 0, 0, NULL),
(26215, 1, 0, 0, 0, 0, 0, NULL),
(26216, 1, 0, 0, 0, 0, 0, NULL),
(26221, 1, 0, 0, 0, 0, 0, NULL),
(26222, 1, 1, 0, 0, 0, 0, NULL),
(26223, 1, 1, 0, 0, 0, 0, NULL),
(26227, 0, 0, 1, 0, 0, 0, NULL),
(26230, 1, 0, 0, 0, 0, 0, NULL),
(26237, 1, 0, 1, 0, 0, 0, NULL),
(26238, 1, 0, 0, 0, 0, 0, NULL),
(26239, 1, 0, 0, 0, 0, 0, NULL),
(26240, 1, 0, 0, 0, 0, 0, NULL),
(26241, 1, 0, 0, 0, 0, 0, NULL),
(26242, 1, 0, 0, 0, 0, 0, NULL),
(26243, 1, 0, 0, 0, 0, 0, NULL),
(26244, 1, 0, 0, 0, 0, 0, NULL),
(26246, 1, 1, 0, 0, 0, 0, NULL),
(26247, 1, 1, 0, 0, 0, 0, NULL),
(26248, 0, 0, 1, 0, 0, 0, NULL),
(26249, 0, 0, 1, 0, 0, 0, NULL),
(26251, 1, 0, 0, 0, 0, 0, NULL),
(26253, 1, 1, 0, 0, 0, 0, NULL),
(26254, 1, 0, 0, 0, 0, 0, NULL),
(26255, 1, 0, 0, 0, 0, 0, NULL),
(26256, 0, 0, 1, 0, 0, 0, NULL),
(26258, 0, 0, 1, 0, 0, 0, NULL),
(26259, 1, 1, 0, 0, 0, 0, NULL),
(26262, 1, 0, 0, 0, 0, 0, NULL),
(26267, 1, 0, 0, 0, 0, 0, NULL),
(26276, 1, 1, 1, 0, 0, 0, NULL),
(26277, 1, 1, 1, 0, 0, 0, NULL),
(26286, 0, 1, 1, 0, 0, 0, NULL),
(26288, 0, 1, 1, 0, 0, 0, NULL),
(26289, 1, 0, 0, 0, 0, 0, NULL),
(26298, 0, 0, 1, 0, 0, 0, NULL),
(26300, 1, 0, 0, 0, 0, 0, NULL),
(26301, 1, 0, 0, 0, 0, 0, NULL),
(26302, 1, 0, 0, 0, 0, 0, NULL),
(26303, 1, 0, 0, 0, 0, 0, NULL),
(26304, 1, 0, 0, 0, 0, 0, NULL),
(26305, 1, 0, 0, 0, 0, 0, NULL),
(26306, 1, 0, 0, 0, 0, 0, NULL),
(26307, 1, 0, 0, 0, 0, 0, NULL),
(26308, 1, 0, 0, 0, 0, 0, NULL),
(26309, 1, 0, 0, 0, 0, 0, NULL),
(26310, 0, 1, 1, 0, 0, 0, NULL),
(26323, 1, 0, 0, 0, 0, 0, NULL),
(26324, 1, 0, 0, 0, 0, 0, NULL),
(26325, 1, 0, 0, 0, 0, 0, NULL),
(26326, 1, 0, 0, 0, 0, 0, NULL),
(26327, 1, 0, 0, 0, 0, 0, NULL),
(26328, 1, 0, 0, 0, 0, 0, NULL),
(26329, 1, 0, 0, 0, 0, 0, NULL),
(26330, 1, 0, 0, 0, 0, 0, NULL),
(26331, 1, 0, 0, 0, 0, 0, NULL),
(26332, 1, 0, 0, 0, 0, 0, NULL),
(26337, 1, 0, 0, 0, 0, 0, NULL),
(26338, 1, 0, 0, 0, 0, 0, NULL),
(26339, 1, 0, 0, 0, 0, 0, NULL),
(26340, 1, 0, 0, 0, 0, 0, NULL),
(26341, 1, 0, 0, 0, 0, 0, NULL),
(26342, 1, 0, 0, 0, 0, 0, NULL),
(26346, 1, 0, 0, 0, 0, 0, NULL),
(26350, 1, 0, 0, 0, 0, 0, NULL),
(26351, 1, 0, 0, 0, 0, 0, NULL),
(26352, 1, 1, 0, 0, 0, 0, NULL),
(26353, 1, 0, 0, 0, 0, 0, NULL),
(26354, 1, 0, 0, 0, 0, 0, NULL),
(26355, 1, 0, 0, 0, 0, 0, NULL),
(26367, 1, 0, 0, 0, 0, 0, NULL),
(26369, 1, 1, 1, 0, 0, 0, NULL),
(26378, 1, 1, 0, 0, 0, 0, NULL),
(26383, 1, 1, 0, 0, 0, 0, NULL),
(26384, 1, 1, 0, 0, 0, 0, NULL),
(26391, 1, 0, 0, 0, 0, 0, NULL),
(26393, 1, 1, 0, 0, 0, 0, NULL),
(26394, 1, 1, 0, 0, 0, 0, NULL),
(26395, 1, 1, 0, 0, 0, 0, NULL),
(26396, 1, 1, 0, 0, 0, 0, NULL),
(26397, 1, 1, 0, 0, 0, 0, NULL),
(26398, 1, 1, 0, 0, 0, 0, NULL),
(26400, 0, 0, 1, 0, 0, 0, NULL),
(26401, 1, 0, 0, 0, 0, 0, NULL),
(26403, 0, 0, 1, 0, 0, 0, NULL),
(26435, 0, 0, 1, 0, 0, 0, NULL),
(26438, 1, 0, 0, 0, 0, 0, NULL),
(26439, 1, 0, 0, 0, 0, 0, NULL),
(26452, 0, 1, 0, 0, 0, 0, NULL),
(26464, 1, 0, 0, 0, 0, 0, NULL),
(26465, 1, 0, 0, 0, 0, 0, NULL),
(26466, 1, 0, 0, 0, 0, 0, NULL),
(26483, 1, 1, 1, 0, 0, 0, NULL),
(26502, 1, 0, 0, 0, 0, 0, NULL),
(26510, 0, 1, 0, 0, 0, 0, NULL),
(26511, 0, 1, 0, 0, 0, 0, NULL),
(26520, 1, 0, 0, 0, 0, 0, NULL),
(26522, 0, 1, 0, 0, 0, 0, NULL),
(26534, 1, 0, 0, 0, 0, 0, NULL),
(26560, 1, 1, 0, 0, 0, 0, NULL),
(26575, 0, 1, 1, 0, 0, 0, NULL),
(26576, 1, 1, 0, 0, 0, 0, NULL),
(26577, 0, 1, 1, 0, 0, 0, NULL),
(26578, 0, 1, 1, 0, 0, 0, NULL),
(26579, 0, 0, 1, 0, 0, 0, NULL),
(26607, 0, 1, 1, 0, 0, 0, NULL),
(26638, 0, 0, 1, 0, 0, 0, NULL),
(26648, 0, 0, 1, 0, 0, 0, NULL),
(26650, 1, 1, 0, 0, 0, 0, NULL),
(26651, 1, 1, 0, 0, 0, 0, NULL),
(26668, 2, 0, 1, 0, 0, 0, NULL),
(26677, 0, 0, 1, 0, 0, 0, NULL),
(26678, 1, 0, 0, 1, 0, 0, NULL),
(26688, 0, 0, 1, 0, 0, 0, NULL),
(26698, 1, 0, 0, 0, 0, 0, NULL),
(26712, 0, 0, 1, 0, 0, 0, NULL),
(26724, 1, 0, 0, 0, 0, 0, NULL),
(26736, 0, 1, 1, 0, 0, 0, NULL),
(26738, 1, 0, 0, 0, 0, 0, NULL),
(26739, 1, 0, 0, 0, 0, 0, NULL),
(26740, 1, 0, 0, 0, 0, 0, NULL),
(26741, 1, 0, 0, 0, 0, 0, NULL),
(26742, 1, 0, 0, 0, 0, 0, NULL),
(26743, 1, 0, 0, 0, 0, 0, NULL),
(26744, 1, 0, 0, 0, 0, 0, NULL),
(26745, 1, 0, 0, 0, 0, 0, NULL),
(26747, 1, 0, 0, 0, 0, 0, NULL),
(26748, 1, 0, 0, 0, 0, 0, NULL),
(26749, 1, 0, 0, 0, 0, 0, NULL),
(26750, 1, 0, 0, 0, 0, 0, NULL),
(26751, 1, 0, 0, 0, 0, 0, NULL),
(26752, 1, 0, 0, 0, 0, 0, NULL),
(26753, 1, 0, 0, 0, 0, 0, NULL),
(26754, 1, 0, 0, 0, 0, 0, NULL),
(26755, 1, 0, 0, 0, 0, 0, NULL),
(26756, 1, 0, 0, 0, 0, 0, NULL),
(26757, 1, 0, 0, 0, 0, 0, NULL),
(26758, 1, 0, 0, 0, 0, 0, NULL),
(26759, 1, 0, 0, 0, 0, 0, NULL),
(26760, 1, 0, 0, 0, 0, 0, NULL),
(26761, 0, 1, 1, 0, 0, 0, NULL),
(26765, 1, 0, 0, 0, 0, 0, NULL),
(26773, 0, 0, 1, 0, 0, 0, NULL),
(26777, 0, 0, 1, 0, 0, 0, NULL),
(26778, 0, 0, 1, 0, 0, 0, NULL),
(26781, 1, 1, 0, 0, 0, 0, NULL),
(26813, 1, 1, 1, 0, 0, 0, NULL),
(26831, 0, 0, 1, 0, 0, 0, NULL),
(26832, 0, 1, 1, 0, 0, 0, NULL),
(26838, 0, 1, 1, 0, 0, 0, NULL),
(26841, 0, 1, 1, 0, 0, 0, NULL),
(26855, 0, 0, 1, 0, 0, 0, NULL),
(26856, 0, 0, 1, 0, 0, 0, NULL),
(26857, 0, 0, 1, 0, 0, 0, NULL),
(26858, 0, 1, 1, 0, 0, 0, NULL),
(26887, 0, 0, 1, 0, 0, 0, NULL),
(26889, 0, 0, 1, 0, 0, 0, NULL),
(26890, 0, 0, 1, 0, 0, 0, NULL),
(26892, 0, 0, 1, 0, 0, 0, NULL),
(26893, 0, 0, 1, 0, 0, 0, NULL),
(26925, 0, 0, 1, 0, 0, 0, NULL),
(26933, 0, 0, 1, 0, 0, 0, NULL),
(27064, 1, 1, 0, 1, 0, 0, NULL),
(27079, 0, 0, 1, 0, 0, 0, NULL),
(27101, 0, 0, 1, 0, 0, 0, NULL),
(27111, 0, 0, 1, 0, 0, 0, NULL),
(27112, 0, 0, 1, 0, 0, 0, NULL),
(27113, 1, 0, 0, 0, 0, 0, NULL),
(27114, 1, 0, 0, 0, 0, 0, NULL),
(27115, 1, 0, 0, 0, 0, 0, NULL),
(27135, 0, 0, 1, 0, 0, 0, NULL),
(27166, 1, 1, 1, 1, 0, 0, NULL),
(27169, 1, 1, 1, 1, 0, 0, NULL),
(27212, 1, 0, 1, 1, 0, 0, NULL),
(27220, 0, 1, 1, 0, 0, 0, NULL),
(27241, 1, 1, 1, 0, 0, 0, NULL),
(27244, 1, 1, 1, 0, 0, 0, NULL),
(27249, 0, 1, 1, 0, 0, 0, NULL),
(27253, 0, 0, 1, 0, 0, 0, NULL),
(27258, 1, 1, 1, 0, 0, 0, NULL),
(27273, 0, 0, 1, 0, 0, 0, NULL),
(27280, 0, 0, 1, 0, 0, 0, NULL),
(27285, 0, 1, 1, 0, 0, 0, NULL),
(27292, 0, 0, 1, 0, 0, 0, NULL),
(27296, 0, 0, 1, 0, 0, 0, NULL),
(27297, 0, 0, 1, 0, 0, 0, NULL),
(27303, 0, 0, 1, 0, 0, 0, NULL),
(27306, 0, 0, 1, 0, 0, 0, NULL),
(27307, 0, 0, 1, 0, 0, 0, NULL),
(27308, 0, 0, 1, 0, 0, 0, NULL),
(27309, 0, 0, 1, 0, 0, 0, NULL),
(27327, 0, 0, 1, 0, 0, 0, NULL),
(27331, 0, 0, 1, 0, 0, 0, NULL),
(27339, 0, 0, 1, 0, 0, 0, NULL),
(27352, 1, 1, 1, 0, 0, 2, NULL),
(27366, 0, 0, 1, 0, 0, 0, NULL),
(27375, 0, 0, 1, 0, 0, 0, NULL),
(27380, 0, 0, 1, 0, 0, 0, NULL),
(27394, 0, 0, 1, 0, 0, 0, NULL),
(27404, 0, 0, 1, 0, 0, 0, NULL),
(27419, 0, 0, 1, 0, 0, 0, NULL),
(27426, 0, 0, 1, 0, 0, 0, NULL),
(27427, 0, 0, 1, 0, 0, 0, NULL),
(27428, 0, 0, 1, 0, 0, 0, NULL),
(27429, 0, 0, 1, 0, 0, 0, NULL),
(27430, 1, 1, 0, 1, 0, 0, NULL),
(27438, 0, 1, 0, 0, 0, 0, NULL),
(27444, 0, 0, 1, 0, 0, 0, NULL),
(27445, 0, 0, 1, 0, 0, 0, NULL),
(27450, 0, 0, 1, 0, 0, 0, NULL),
(27452, 0, 0, 1, 0, 0, 0, NULL),
(27455, 1, 0, 0, 0, 0, 0, NULL),
(27480, 1, 0, 0, 0, 0, 0, NULL),
(27490, 0, 0, 1, 0, 0, 0, NULL),
(27521, 0, 1, 1, 0, 0, 0, NULL),
(27530, 0, 1, 1, 0, 0, 0, NULL),
(27542, 0, 1, 1, 0, 0, 0, NULL),
(27559, 1, 1, 0, 0, 0, 0, NULL),
(27593, 0, 1, 1, 0, 0, 0, NULL),
(27608, 0, 0, 1, 0, 0, 0, NULL),
(27617, 0, 1, 0, 0, 0, 0, NULL),
(27620, 0, 1, 1, 0, 0, 0, NULL),
(27622, 0, 0, 1, 0, 0, 0, NULL),
(27629, 0, 0, 1, 0, 0, 0, NULL),
(27638, 1, 1, 1, 0, 0, 0, NULL),
(27656, 1, 1, 1, 0, 0, 0, NULL),
(27663, 0, 0, 1, 0, 0, 0, NULL),
(27682, 0, 0, 1, 0, 0, 0, NULL),
(27688, 0, 0, 1, 0, 0, 0, NULL),
(27691, 1, 1, 1, 0, 0, 0, NULL),
(27692, 0, 1, 1, 0, 0, 0, NULL),
(27693, 0, 0, 1, 0, 0, 0, NULL),
(27698, 0, 0, 1, 0, 0, 0, NULL),
(27702, 0, 0, 1, 0, 0, 0, NULL),
(27709, 1, 0, 1, 0, 0, 0, NULL),
(27714, 0, 0, 0, 1, 0, 0, NULL),
(27721, 1, 1, 0, 0, 0, 0, NULL),
(27722, 1, 1, 0, 0, 0, 0, NULL),
(27724, 1, 0, 1, 0, 0, 0, NULL),
(27753, 1, 0, 1, 0, 0, 0, NULL),
(27754, 1, 0, 1, 0, 0, 0, NULL),
(27755, 0, 1, 1, 0, 0, 0, NULL),
(27756, 0, 1, 1, 0, 0, 0, NULL),
(27766, 0, 0, 1, 0, 0, 0, NULL),
(27767, 0, 0, 1, 0, 0, 0, NULL),
(27768, 0, 0, 1, 0, 0, 0, NULL),
(27769, 0, 0, 1, 0, 0, 0, NULL),
(27770, 0, 0, 1, 0, 0, 0, NULL),
(27771, 0, 0, 1, 0, 0, 0, NULL),
(27772, 0, 0, 1, 0, 0, 0, NULL),
(27773, 0, 0, 1, 0, 0, 0, NULL),
(27774, 0, 0, 1, 0, 0, 0, NULL),
(27775, 0, 0, 1, 0, 0, 0, NULL),
(27776, 0, 0, 1, 0, 0, 0, NULL),
(27777, 0, 0, 1, 0, 0, 0, NULL),
(27778, 0, 0, 1, 0, 0, 0, NULL),
(27779, 0, 0, 1, 0, 0, 0, NULL),
(27780, 0, 0, 1, 0, 0, 0, NULL),
(27781, 0, 0, 1, 0, 0, 0, NULL),
(27782, 0, 0, 1, 0, 0, 0, NULL),
(27832, 0, 0, 1, 0, 0, 0, NULL),
(27837, 0, 0, 1, 0, 0, 0, NULL),
(27852, 1, 1, 1, 0, 0, 0, NULL),
(27853, 0, 0, 1, 0, 0, 0, NULL),
(27886, 0, 1, 1, 0, 0, 0, NULL),
(27888, 0, 0, 1, 0, 0, 0, NULL),
(27894, 1, 1, 0, 1, 0, 0, NULL),
(27910, 0, 0, 1, 0, 0, 0, NULL),
(27923, 0, 0, 1, 0, 0, 0, NULL),
(27924, 0, 0, 1, 0, 0, 0, NULL),
(27925, 0, 0, 1, 0, 0, 0, NULL),
(27932, 0, 1, 1, 0, 0, 0, NULL),
(27939, 0, 1, 1, 0, 0, 0, NULL),
(27942, 0, 0, 1, 0, 0, 0, NULL),
(27987, 0, 1, 1, 0, 0, 0, NULL),
(27995, 0, 0, 1, 0, 0, 0, NULL),
(27996, 0, 0, 1, 0, 0, 0, NULL),
(27999, 0, 0, 1, 0, 0, 0, NULL),
(28010, 0, 1, 0, 0, 0, 0, NULL),
(28013, 0, 0, 1, 0, 0, 0, NULL),
(28015, 0, 0, 1, 0, 0, 0, NULL),
(28019, 0, 0, 1, 0, 0, 0, NULL),
(28055, 0, 0, 1, 0, 0, 0, NULL),
(28085, 1, 1, 1, 0, 0, 0, NULL),
(28086, 1, 1, 1, 0, 0, 0, NULL),
(28093, 0, 0, 1, 0, 0, 0, NULL),
(28130, 0, 0, 1, 0, 0, 0, NULL),
(28156, 1, 1, 0, 1, 0, 0, NULL),
(28170, 1, 1, 1, 0, 0, 0, NULL),
(28181, 0, 0, 1, 0, 0, 0, NULL),
(28183, 0, 1, 1, 0, 0, 0, NULL),
(28192, 0, 0, 1, 0, 0, 0, NULL),
(28229, 0, 0, 1, 0, 0, 0, NULL),
(28234, 0, 0, 1, 0, 0, 0, NULL),
(28235, 0, 0, 1, 0, 0, 0, NULL),
(28246, 1, 1, 1, 0, 0, 0, NULL),
(28253, 0, 0, 1, 0, 0, 0, NULL),
(28256, 0, 0, 1, 0, 0, 0, NULL),
(28274, 0, 0, 1, 0, 0, 0, NULL),
(28276, 1, 1, 1, 0, 0, 0, NULL),
(28279, 0, 0, 1, 0, 0, 0, NULL),
(28289, 0, 0, 1, 0, 0, 0, NULL),
(28307, 0, 0, 1, 0, 0, 0, NULL),
(28332, 0, 0, 1, 0, 0, 0, NULL),
(28351, 0, 0, 1, 0, 0, 0, NULL),
(28366, 1, 1, 0, 1, 0, 0, NULL),
(28367, 0, 0, 1, 0, 0, 0, NULL),
(28378, 1, 1, 1, 0, 0, 0, NULL),
(28408, 1, 1, 0, 1, 0, 0, NULL),
(28465, 0, 1, 1, 0, 0, 0, NULL),
(28466, 0, 0, 1, 0, 0, 0, NULL),
(28476, 0, 0, 1, 0, 0, 0, NULL),
(28481, 0, 0, 1, 0, 0, 0, NULL),
(28487, 0, 1, 1, 0, 0, 0, NULL),
(28492, 0, 0, 1, 0, 0, 0, NULL),
(28511, 1, 0, 1, 0, 0, 0, NULL),
(28534, 2, 1, 1, 0, 0, 0, NULL),
(28535, 0, 0, 1, 0, 0, 0, NULL),
(28536, 0, 0, 1, 0, 0, 0, NULL),
(28537, 0, 0, 1, 0, 0, 0, NULL),
(28539, 0, 0, 1, 0, 0, 0, NULL),
(28540, 0, 0, 1, 0, 0, 0, NULL),
(28563, 0, 0, 1, 0, 0, 0, NULL),
(28614, 1, 1, 1, 0, 0, 0, NULL),
(28622, 0, 0, 1, 0, 0, 0, NULL),
(28639, 0, 0, 1, 0, 0, 0, NULL),
(28640, 0, 0, 1, 0, 0, 0, NULL),
(28654, 1, 0, 0, 0, 2, 0, NULL),
(28669, 0, 0, 1, 0, 0, 0, NULL),
(28710, 1, 1, 1, 0, 0, 0, NULL),
(28713, 0, 0, 1, 0, 0, 0, NULL),
(28717, 0, 0, 1, 0, 0, 0, NULL),
(28724, 0, 0, 1, 0, 0, 0, NULL),
(28753, 0, 0, 1, 0, 0, 0, NULL),
(28755, 0, 0, 1, 0, 0, 0, NULL),
(28757, 0, 0, 1, 0, 0, 0, NULL),
(28758, 0, 0, 1, 0, 0, 0, NULL),
(28759, 0, 0, 1, 0, 0, 0, NULL),
(28804, 0, 0, 1, 0, 0, 0, NULL),
(28815, 0, 0, 1, 0, 0, 0, NULL),
(28823, 0, 0, 1, 1, NULL, NULL, NULL),
(28847, 0, 1, 0, 0, 0, 0, NULL),
(28859, 0, 1, 1, 0, 0, 0, NULL),
(28864, 1, 0, 1, 0, 0, 0, NULL),
(28875, 0, 0, 1, 0, 0, 0, NULL),
(28876, 0, 0, 1, 0, 0, 0, NULL),
(28904, 0, 0, 1, 0, 0, 0, NULL),
(28906, 0, 1, 1, 0, 0, 0, NULL),
(28932, 0, 0, 1, 0, 0, 0, NULL),
(28947, 0, 0, 1, 0, 0, 0, NULL),
(29038, 0, 0, 1, 0, 0, 0, NULL),
(29047, 2, 1, 1, 0, 0, 0, NULL),
(29048, 0, 0, 1, 0, 0, 0, NULL),
(29055, 0, 0, 1, 0, 0, 0, NULL),
(29056, 0, 0, 1, 0, 0, 0, NULL),
(29057, 0, 0, 1, 0, 0, 0, NULL),
(29058, 0, 0, 1, 0, 0, 0, NULL),
(29060, 0, 0, 1, 0, 0, 0, NULL),
(29066, 0, 0, 1, 0, 0, 0, NULL),
(29069, 0, 0, 1, 0, 0, 0, NULL),
(29229, 1, 0, 0, 0, 0, 0, NULL),
(29239, 1, 1, 1, 0, 0, 0, NULL),
(29241, 1, 1, 0, 0, 0, 0, NULL),
(29242, 1, 1, 0, 0, 0, 0, NULL),
(29243, 1, 1, 0, 0, 0, 0, NULL),
(29256, 1, 1, 0, 0, 0, 0, NULL),
(29259, 2, 1, 1, 0, 0, 0, NULL),
(29281, 1, 0, 1, 0, 0, 0, NULL),
(29310, 0, 1, 1, 0, 0, 0, NULL),
(29326, 0, 0, 1, 0, 0, 0, NULL),
(29332, 1, 1, 1, 0, 0, 0, NULL),
(29333, 0, 1, 1, 0, 0, 0, NULL),
(29349, 1, 1, 1, 0, 0, 0, NULL),
(29368, 0, 0, 1, 0, 0, 0, NULL),
(29379, 0, 0, 1, 0, 0, 0, NULL),
(29391, 0, 0, 1, 0, 0, 0, NULL),
(29392, 0, 1, 0, 0, 0, 0, NULL),
(29398, 0, 0, 1, 0, 0, 0, NULL),
(29403, 1, 0, 1, 0, 0, 0, NULL),
(29406, 0, 0, 1, 0, 0, 0, NULL),
(29414, 1, 0, 1, 0, 0, 0, NULL),
(29416, 0, 0, 1, 0, 0, 0, NULL),
(29424, 0, 0, 1, 0, 0, 0, NULL),
(29453, 0, 1, 1, 0, 0, 0, NULL),
(29455, 0, 1, 1, 0, 0, 0, NULL),
(29457, 1, 0, 0, 0, 0, 0, NULL),
(29458, 1, 1, 1, 0, 0, 0, NULL),
(29460, 1, 1, 1, 0, 0, 0, NULL),
(29475, 1, 1, 0, 1, 0, 0, NULL),
(29483, 1, 1, 0, 1, 0, 0, NULL),
(29488, 0, 1, 1, 0, 0, 0, NULL),
(29501, 0, 1, 1, 0, 0, 0, NULL),
(29550, 0, 0, 1, 0, 0, 0, NULL),
(29558, 0, 0, 1, 0, 0, 0, NULL),
(29570, 0, 1, 1, 0, 0, 0, NULL),
(29572, 0, 0, 1, 0, 0, 0, NULL),
(29577, 0, 0, 1, 0, 0, 0, NULL),
(29613, 1, 1, 0, 1, 0, 0, NULL),
(29625, 0, 0, 1, 0, 0, 0, NULL),
(29627, 0, 0, 1, 0, 0, 0, NULL),
(29646, 1, 1, 1, 0, 0, 0, NULL),
(29649, 0, 0, 0, 1, 0, 0, NULL),
(29679, 0, 0, 1, 0, 0, 0, NULL),
(29682, 0, 0, 1, 0, 0, 0, NULL),
(29692, 0, 1, 1, 0, 0, 0, NULL),
(29708, 0, 0, 1, 0, 0, 0, NULL),
(29709, 0, 0, 1, 0, 0, 0, NULL),
(29746, 0, 0, 1, 0, 0, 0, NULL),
(29747, 1, 0, 0, 1, 0, 0, NULL),
(29748, 0, 1, 1, 0, 0, 0, NULL),
(29753, 0, 1, 1, 0, 0, 0, NULL),
(29790, 0, 0, 0, 1, 0, 0, NULL),
(29792, 1, 1, 1, 0, 0, 0, NULL),
(29793, 1, 1, 1, 0, 0, 0, NULL),
(29794, 0, 1, 1, 0, 0, 0, NULL),
(29803, 0, 0, 1, 0, 0, 0, NULL),
(29805, 0, 0, 1, 0, 0, 0, NULL),
(29808, 1, 1, 1, 0, 0, 0, NULL),
(29834, 0, 1, 0, 0, 0, 0, NULL),
(29845, 0, 0, 1, 0, 0, 0, NULL),
(29846, 0, 0, 1, 0, 0, 0, NULL),
(29847, 0, 0, 1, 0, 0, 0, NULL),
(29849, 0, 0, 1, 0, 0, 0, NULL),
(29854, 0, 1, 1, 0, 0, 0, NULL),
(29863, 0, 0, 1, 0, 0, 0, NULL),
(29893, 1, 1, 1, 0, 0, 0, NULL),
(29894, 0, 1, 1, 0, 0, 0, NULL),
(29999, 0, 0, 1, 0, 0, 0, NULL),
(30000, 0, 0, 1, 0, 0, 0, NULL),
(30013, 0, 1, 1, 0, 0, 0, NULL),
(30038, 0, 0, 1, 0, 0, 0, NULL),
(30066, 1, 1, 0, 1, 0, 0, NULL),
(30078, 0, 0, 1, 0, 0, 0, NULL),
(30090, 0, 0, 1, 0, 0, 0, NULL),
(30106, 0, 0, 1, 0, 0, 0, NULL),
(30108, 0, 1, 1, 0, 0, 0, NULL),
(30118, 0, 0, 1, 0, 0, 0, NULL),
(30134, 0, 1, 1, 0, 0, 0, NULL),
(30156, 0, 1, 1, 0, 0, 0, NULL),
(30159, 1, 1, 1, 0, 0, 0, NULL),
(30161, 0, 0, 1, 0, 0, 0, NULL),
(30165, 1, 1, 1, 0, 0, 0, NULL),
(30181, 0, 0, 1, 0, 0, 0, NULL),
(30184, 1, 1, 1, 0, 0, 0, NULL),
(30215, 0, 0, 1, 0, 0, 0, NULL),
(30220, 0, 0, 1, 0, 0, 0, NULL),
(30228, 1, 0, 0, 0, 0, 0, NULL),
(30234, 0, 0, 1, 0, 0, 0, NULL),
(30236, 1, 1, 0, 1, 0, 0, NULL),
(30245, 0, 1, 1, 0, 0, 0, NULL),
(30246, 0, 0, 1, 0, 0, 0, NULL),
(30248, 0, 0, 1, 0, 0, 0, NULL),
(30249, 0, 1, 1, 0, 0, 0, NULL),
(30272, 0, 0, 1, 0, 0, 0, NULL),
(30275, 1, 1, 1, 0, 0, 0, NULL),
(30298, 0, 0, 1, 0, 0, 0, NULL),
(30302, 0, 0, 1, 0, 0, 0, NULL),
(30313, 0, 1, 1, 0, 0, 0, NULL),
(30315, 1, 1, 0, 0, 0, 0, NULL),
(30316, 1, 1, 0, 0, 0, 0, NULL),
(30317, 1, 1, 0, 0, 0, 0, NULL),
(30318, 1, 1, 0, 0, 0, 0, NULL),
(30330, 1, 1, 1, 0, 0, 0, NULL),
(30361, 0, 0, 1, 0, 0, 0, NULL),
(30391, 0, 0, 1, 1, 0, 0, NULL),
(30393, 0, 1, 1, 0, 0, 0, NULL),
(30412, 0, 0, 1, 0, 0, 0, NULL),
(30413, 0, 0, 1, 0, 0, 0, NULL),
(30420, 0, 1, 1, 0, 0, 0, NULL),
(30435, 0, 0, 1, 1, 0, 0, NULL),
(30442, 0, 0, 1, 0, 0, 0, NULL),
(30446, 1, 1, 1, 0, 0, 0, NULL),
(30449, 1, 1, 0, 0, 0, 0, NULL),
(30451, 1, 1, 0, 0, 0, 0, NULL),
(30452, 1, 1, 0, 0, 0, 0, NULL),
(30454, 0, 0, 1, 0, 0, 0, NULL),
(30461, 1, 1, 1, 0, 0, 0, NULL),
(30475, 1, 1, 0, 0, 0, 0, NULL),
(30477, 0, 0, 1, 0, 0, 0, NULL),
(30482, 1, 1, 1, 0, 0, 0, NULL),
(30487, 0, 0, 1, 0, 0, 0, NULL),
(30501, 0, 1, 1, 0, 0, 0, NULL),
(30518, 0, 1, 1, 0, 0, 0, NULL),
(30521, 0, 1, 1, 0, 0, 0, NULL),
(30527, 1, 0, 0, 1, 0, 0, NULL),
(30564, 0, 0, 1, 0, 0, 0, NULL),
(30575, 1, 1, 1, 0, 0, 0, NULL),
(30577, 0, 0, 1, 0, 0, 0, NULL),
(30592, 0, 0, 1, 0, 0, 0, NULL),
(30642, 0, 1, 0, 0, 0, 0, NULL),
(30644, 0, 0, 1, 0, 0, 0, NULL),
(30646, 0, 0, 1, 0, 0, 0, NULL),
(30651, 0, 0, 1, 0, 0, 0, NULL),
(30684, 0, 0, 1, 0, 0, 0, NULL),
(30700, 0, 0, 1, 0, 0, 0, NULL),
(30736, 0, 0, 1, 0, 0, 0, NULL),
(30749, 0, 0, 1, 0, 0, 0, NULL),
(30750, 0, 0, 1, 0, 0, 0, NULL),
(30771, 0, 0, 1, 0, 0, 0, NULL),
(30775, 0, 0, 1, 0, 0, 0, NULL),
(30777, 0, 0, 1, 0, 0, 0, NULL),
(30780, 0, 0, 1, 0, 0, 0, NULL),
(30782, 0, 0, 1, 0, 0, 0, NULL),
(30784, 0, 0, 1, 0, 0, 0, NULL),
(30786, 0, 0, 1, 0, 0, 0, NULL),
(30789, 1, 1, 0, 0, 0, 0, NULL),
(30794, 0, 0, 1, 0, 0, 0, NULL),
(30805, 0, 0, 1, 0, 0, 0, NULL),
(30808, 0, 0, 1, 0, 0, 0, NULL),
(30809, 1, 0, 1, 0, 0, 0, NULL),
(30810, 2, 0, 1, 0, 0, 0, NULL),
(30830, 1, 1, 0, 0, 0, 0, NULL),
(30832, 0, 0, 1, 0, 0, 0, NULL),
(30837, 0, 0, 1, 0, 0, 0, NULL),
(30879, 1, 0, 1, 0, 0, 0, NULL),
(30880, 0, 0, 1, 0, 0, 0, NULL),
(30889, 1, 1, 1, 0, 0, 0, NULL),
(30896, 0, 0, 1, 0, 0, 0, NULL),
(30897, 0, 0, 1, 0, 0, 0, NULL),
(30898, 0, 0, 1, 0, 0, 0, NULL),
(30903, 1, 1, 1, 0, 0, 0, NULL),
(30928, 0, 1, 0, 0, 0, 0, NULL),
(30945, 0, 1, 1, 0, 0, 0, NULL),
(30955, 1, 1, 1, 0, 0, 0, NULL),
(30959, 1, 1, 1, 0, 0, 0, NULL),
(30988, 1, 1, 1, 0, 0, 0, NULL),
(30991, 1, 1, 1, 0, 0, 0, NULL),
(30996, 0, 0, 1, 0, 0, 0, NULL),
(30997, 1, 1, 1, 0, 0, 0, NULL),
(31004, 0, 0, 1, 0, 0, 0, NULL),
(31029, 0, 1, 1, 0, 0, 0, NULL),
(31030, 0, 1, 1, 0, 0, 0, NULL),
(31040, 0, 1, 1, 0, 0, 0, NULL),
(31050, 0, 0, 1, 0, 0, 0, NULL),
(31087, 0, 1, 1, 0, 0, 0, NULL),
(31095, 1, 1, 1, 0, 0, 0, NULL),
(31103, 1, 1, 0, 1, 0, 0, NULL),
(31135, 0, 1, 1, 0, 0, 0, NULL),
(31137, 1, 1, 1, 0, 0, 0, NULL),
(31143, 1, 0, 0, 1, 0, 0, NULL),
(31144, 1, 0, 0, 1, 0, 0, NULL),
(31146, 1, 0, 0, 1, 0, 0, NULL),
(31157, 0, 0, 1, 0, 0, 0, NULL),
(31204, 1, 1, 0, 1, 0, 0, NULL),
(31238, 0, 1, 1, 0, 0, 0, NULL),
(31242, 1, 1, 0, 0, 0, 0, NULL),
(31245, 0, 0, 1, 0, 0, 0, NULL),
(31263, 0, 1, 1, 0, 0, 0, NULL),
(31269, 0, 0, 1, 0, 0, 0, NULL),
(31279, 1, 0, 0, 0, 0, 0, NULL),
(31280, 1, 1, 0, 1, 0, 0, NULL),
(31351, 0, 0, 1, 0, 0, 0, NULL),
(31353, 0, 0, 1, 0, 0, 0, NULL),
(31393, 0, 0, 1, 0, 0, 0, NULL),
(31396, 0, 1, 1, 0, 0, 0, NULL),
(31400, 0, 0, 1, 0, 0, 0, NULL),
(31406, 0, 0, 1, 0, 0, 0, NULL),
(31424, 1, 1, 0, 0, 0, 0, NULL),
(31432, 0, 0, 1, 0, 0, 0, NULL),
(31458, 0, 0, 1, 0, 0, 0, NULL),
(31461, 0, 0, 1, 1, 0, 0, NULL),
(31462, 0, 0, 1, 1, 0, 0, NULL),
(31465, 0, 1, 1, 0, 0, 0, NULL),
(31520, 1, 1, 0, 0, 0, 0, NULL),
(31534, 1, 1, 0, 0, 0, 0, NULL),
(31535, 1, 1, 0, 0, 0, 0, NULL),
(31547, 1, 1, 0, 1, 0, 0, NULL),
(31561, 1, 1, 1, 0, 0, 0, NULL),
(31583, 0, 1, 1, 0, 0, 0, NULL),
(31598, 1, 1, 1, 0, 0, 0, NULL),
(31630, 0, 0, 1, 0, 0, 0, NULL),
(31644, 0, 0, 1, 0, 0, 0, NULL),
(31645, 0, 0, 1, 0, 0, 0, NULL),
(31646, 0, 0, 1, 0, 0, 0, NULL),
(31655, 0, 0, 1, 0, 0, 0, NULL),
(31669, 1, 1, 1, 0, 0, 0, NULL),
(31676, 0, 1, 1, 0, 0, 0, NULL),
(31682, 0, 0, 1, 0, 0, 0, NULL),
(31686, 0, 0, 1, 0, 0, 0, NULL),
(31687, 0, 0, 1, 0, 0, 0, NULL),
(31689, 0, 1, 1, 0, 0, 0, NULL),
(31702, 1, 1, 1, 0, 0, 0, NULL),
(31719, 1, 1, 1, 0, 0, 0, NULL),
(31721, 1, 1, 1, 0, 0, 0, NULL),
(31734, 0, 1, 1, 0, 0, 0, NULL),
(31748, 0, 0, 1, 0, 0, 0, NULL),
(31749, 0, 0, 1, 0, 0, 0, NULL),
(31750, 0, 1, 1, 0, 0, 0, NULL),
(31751, 0, 1, 1, 0, 0, 0, NULL),
(31752, 0, 0, 1, 0, 0, 0, NULL),
(31766, 0, 0, 1, 0, 0, 0, NULL),
(31768, 0, 0, 1, 0, 0, 0, NULL),
(31769, 1, 1, 1, 0, 0, 0, NULL),
(31787, 0, 1, 1, 0, 0, 0, NULL),
(31814, 0, 0, 1, 0, 0, 0, NULL),
(31866, 0, 0, 1, 0, 0, 0, NULL),
(31874, 0, 0, 1, 0, 0, 0, NULL),
(31881, 0, 0, 1, 0, 0, 0, NULL),
(31975, 1, 0, 0, 0, 0, 0, NULL),
(31977, 1, 0, 0, 0, 0, 0, NULL),
(32011, 1, 1, 0, 0, 0, 0, NULL),
(32013, 1, 1, 0, 0, 0, 0, NULL),
(32113, 0, 0, 1, 0, 0, 0, NULL),
(32114, 0, 0, 1, 0, 0, 0, NULL),
(32115, 0, 0, 1, 0, 0, 0, NULL),
(32116, 0, 0, 1, 0, 0, 0, NULL),
(32117, 0, 0, 1, 0, 0, 0, NULL),
(32118, 0, 0, 1, 0, 0, 0, NULL),
(32180, 1, 0, 1, 0, 0, 0, NULL),
(32185, 1, 0, 1, 0, 0, 0, NULL),
(32186, 1, 1, 1, 0, 0, 0, NULL),
(32188, 1, 1, 1, 0, 0, 0, NULL),
(32189, 1, 1, 1, 0, 0, 0, NULL),
(32201, 1, 1, 1, 0, 0, 0, NULL),
(32225, 0, 0, 1, 0, 0, 0, NULL),
(32265, 0, 0, 1, 0, 0, 0, NULL),
(32292, 0, 0, 1, 0, 0, 0, NULL),
(32295, 0, 0, 1, 0, 0, 0, NULL),
(32323, 1, 1, 1, 0, 0, 0, NULL),
(32347, 1, 1, 0, 1, 0, 0, NULL),
(32358, 0, 1, 1, 0, 0, 0, NULL),
(32386, 1, 1, 0, 0, 0, 0, NULL),
(32406, 0, 0, 1, 0, 0, 0, NULL),
(32417, 1, 1, 0, 0, 0, 0, NULL),
(32446, 1, 1, 1, 0, 0, 0, NULL),
(32448, 0, 0, 1, 0, 0, 0, NULL),
(32472, 0, 1, 1, 0, 0, 0, NULL),
(32491, 1, 1, 1, 0, 0, 0, NULL),
(32492, 1, 1, 1, 0, 0, 0, NULL),
(32513, 0, 0, 1, 0, 0, 0, NULL),
(32520, 0, 0, 1, 0, 0, 0, NULL),
(32534, 0, 0, 1, 0, 0, 0, NULL),
(32535, 0, 0, 1, 0, 0, 0, NULL),
(32541, 1, 0, 0, 1, 0, 0, NULL),
(32542, 1, 0, 0, 1, 0, 0, NULL),
(32543, 1, 0, 0, 1, 0, 0, NULL),
(32545, 1, 0, 0, 1, 0, 0, NULL),
(32546, 1, 0, 0, 1, 0, 0, NULL),
(32547, 1, 0, 0, 1, 0, 0, NULL),
(32576, 1, 1, 0, 0, 0, 0, NULL),
(32577, 1, 1, 0, 0, 0, 0, NULL),
(32578, 1, 1, 0, 0, 0, 0, NULL),
(32579, 1, 1, 0, 0, 0, 0, NULL),
(32580, 1, 1, 0, 0, 0, 0, NULL),
(32596, 1, 1, 1, 0, 0, 0, NULL),
(32597, 1, 1, 1, 0, 0, 0, NULL),
(32602, 1, 1, 1, 0, 0, 0, NULL),
(32630, 0, 1, 1, 0, 0, 0, NULL),
(32664, 0, 1, 0, 0, 0, 0, NULL),
(32666, 1, 0, 0, 1, 0, 0, NULL),
(32667, 1, 0, 0, 1, 0, 0, NULL),
(32682, 0, 1, 1, 0, 0, 0, NULL),
(32767, 1, 1, 1, 0, 0, 0, NULL),
(32769, 0, 1, 1, 0, 0, 0, NULL),
(32780, 0, 0, 1, 0, 0, 0, NULL),
(32795, 1, 1, 0, 1, 0, 0, NULL),
(32821, 0, 0, 1, 0, 0, 0, NULL),
(32830, 0, 0, 1, 0, 0, 0, NULL),
(32840, 0, 0, 1, 0, 0, 0, NULL),
(32879, 0, 0, 1, 0, 0, 0, NULL),
(32892, 0, 0, 1, 0, 0, 0, NULL),
(32926, 1, 1, 0, 1, 0, 0, NULL),
(32930, 1, 1, 2, 0, 0, 0, NULL),
(32933, 0, 0, 1, 0, 0, 0, NULL),
(32934, 0, 0, 1, 0, 0, 0, NULL),
(32938, 1, 1, 0, 1, 0, 0, NULL),
(32953, 0, 1, 1, 0, 0, 0, NULL),
(33005, 0, 0, 1, 0, 0, 0, NULL),
(33006, 0, 0, 1, 0, 0, 0, NULL),
(33052, 0, 1, 1, 0, 0, 0, NULL),
(33068, 0, 1, 1, 0, 0, 0, NULL),
(33086, 0, 0, 1, 0, 0, 0, NULL),
(33089, 0, 1, 1, 0, 0, 0, NULL),
(33104, 0, 0, 1, 0, 0, 0, NULL),
(33105, 0, 0, 1, 0, 0, 0, NULL),
(33115, 0, 0, 1, 0, 0, 0, NULL),
(33116, 0, 1, 1, 0, 0, 0, NULL),
(33134, 0, 0, 1, 0, 0, 0, NULL),
(33140, 0, 0, 1, 0, 0, 0, NULL),
(33141, 0, 0, 1, 0, 0, 0, NULL),
(33174, 1, 1, 0, 1, 0, 0, NULL),
(33184, 1, 1, 0, 1, 0, 0, NULL),
(33186, 1, 1, 1, 0, 0, 0, NULL),
(33214, 0, 0, 1, 0, 0, 0, NULL),
(33218, 0, 1, 1, 0, 0, 0, NULL),
(33229, 1, 1, 0, 1, 0, 0, NULL),
(33243, 1, 1, 0, 1, 0, 0, NULL),
(33264, 1, 0, 0, 1, 0, 0, NULL),
(33272, 1, 1, 0, 1, 0, 0, NULL),
(33273, 0, 0, 1, 0, 0, 0, NULL),
(33280, 0, 0, 1, 0, 0, 0, NULL),
(33289, 1, 0, 0, 0, 0, 0, NULL),
(33291, 1, 0, 0, 0, 0, 0, NULL),
(33342, 1, 1, 0, 0, 0, 0, NULL),
(33350, 0, 0, 1, 0, 0, 0, NULL),
(33352, 1, 1, 0, 1, 0, 0, NULL),
(33353, 1, 1, 0, 1, 0, 0, NULL),
(33377, 0, 0, 1, 0, 0, 0, NULL),
(33378, 0, 0, 1, 0, 0, 0, NULL),
(33406, 0, 0, 1, 0, 0, 0, NULL),
(33497, 0, 1, 0, 0, 0, 0, NULL),
(33519, 1, 0, 1, 0, 0, 0, NULL),
(33528, 1, 1, 1, 0, 0, 0, NULL),
(33550, 1, 1, 1, 0, 0, 0, NULL),
(33552, 0, 0, 1, 0, 0, 0, NULL),
(33571, 0, 0, 1, 0, 0, 0, NULL),
(33575, 0, 0, 1, 0, 0, 0, NULL),
(33576, 1, 1, 1, 0, 0, 0, NULL),
(33632, 0, 0, 1, 0, 0, 0, NULL),
(33651, 1, 1, 0, 1, 0, 0, NULL),
(33670, 0, 1, 1, 0, 0, 0, NULL),
(33687, 0, 0, 1, 0, 0, 0, NULL),
(33721, 0, 1, 1, 0, 0, 0, NULL),
(33724, 1, 1, 1, 0, 0, 0, NULL),
(33733, 1, 1, 1, 0, 0, 0, NULL),
(33761, 0, 0, 1, 0, 0, 0, NULL),
(33778, 0, 1, 1, 0, 0, 0, NULL),
(33779, 0, 0, 1, 0, 0, 0, NULL),
(33780, 1, 1, 0, 0, 0, 0, NULL),
(33787, 1, 0, 0, 0, 0, 0, NULL),
(33802, 0, 0, 1, 0, 0, 0, NULL),
(33856, 0, 0, 1, 0, 0, 0, NULL),
(33861, 0, 0, 1, 0, 0, 0, NULL),
(33862, 0, 1, 1, 0, 0, 0, NULL),
(33874, 0, 0, 1, 0, 0, 0, NULL),
(33876, 0, 0, 1, 0, 0, 0, NULL),
(33877, 0, 0, 1, 0, 0, 0, NULL),
(33878, 0, 0, 1, 0, 0, 0, NULL),
(33879, 0, 0, 1, 0, 0, 0, NULL),
(33880, 0, 0, 1, 0, 0, 0, NULL),
(33882, 0, 0, 1, 0, 0, 0, NULL),
(33890, 0, 0, 1, 0, 0, 0, NULL),
(33906, 0, 0, 1, 0, 0, 0, NULL),
(33907, 0, 0, 1, 0, 0, 0, NULL),
(33909, 1, 1, 2, 0, 0, 0, NULL),
(33910, 0, 0, 1, 0, 0, 0, NULL),
(33911, 0, 0, 1, 0, 0, 0, NULL),
(33954, 0, 0, 1, 0, 0, 0, NULL),
(34011, 0, 0, 1, 0, 0, 0, NULL),
(34032, 0, 0, 1, 0, 0, 0, NULL),
(34047, 1, 1, 0, 1, 0, 0, NULL),
(34050, 1, 1, 0, 1, 0, 0, NULL),
(34055, 0, 0, 1, 0, 0, 0, NULL),
(34068, 1, 1, 0, 1, 0, 0, NULL),
(34070, 0, 0, 1, 0, 0, 0, NULL),
(34071, 1, 1, 0, 1, 0, 0, NULL),
(34096, 1, 1, 0, 1, 0, 0, NULL),
(34097, 0, 1, 1, 0, 0, 0, NULL),
(34099, 0, 1, 1, 0, 0, 0, NULL),
(34108, 1, 1, 0, 1, 0, 0, NULL),
(34109, 0, 1, 1, 0, 0, 0, NULL),
(34110, 1, 1, 0, 1, 0, 0, NULL),
(34116, 0, 1, 0, 0, 0, 0, NULL),
(34121, 1, 1, 0, 1, 0, 0, NULL),
(34131, 0, 0, 1, 0, 0, 0, NULL),
(34149, 1, 1, 0, 1, 0, 0, NULL),
(34159, 0, 0, 1, 0, 0, 0, NULL),
(34221, 0, 1, 1, 0, 0, 0, NULL),
(34222, 0, 1, 1, 0, 0, 0, NULL),
(34246, 0, 0, 1, 0, 0, 0, NULL),
(34281, 0, 0, 1, 0, 0, 0, NULL),
(34282, 0, 0, 1, 0, 0, 0, NULL),
(34286, 0, 0, 1, 0, 0, 0, NULL),
(34296, 0, 1, 1, 0, 0, 0, NULL),
(34297, 1, 1, 1, 0, 0, 0, NULL),
(34316, 0, 0, 1, 0, 0, 0, NULL),
(34332, 0, 0, 1, 0, 0, 0, NULL),
(34362, 1, 1, 0, 1, 0, 0, NULL),
(34363, 1, 1, 0, 1, 0, 0, NULL),
(34381, 0, 0, 1, 0, 0, 0, NULL),
(34496, 2, 1, 0, 0, 0, 0, NULL),
(34497, 2, 1, 0, 0, 0, 0, NULL),
(34527, 0, 0, 1, 0, 0, 0, NULL),
(34575, 0, 0, 1, 0, 0, 0, NULL),
(34606, 0, 1, 1, 0, 0, 0, NULL),
(34640, 0, 0, 1, 0, 0, 0, NULL),
(34649, 0, 1, 1, 0, 0, 0, NULL),
(34806, 0, 0, 1, 0, 0, 0, NULL),
(34810, 0, 0, 1, 0, 0, 0, NULL),
(34879, 0, 0, 1, 0, 0, 0, NULL),
(34888, 0, 0, 1, 0, 0, 0, NULL),
(34925, 0, 1, 0, 0, 0, 0, NULL),
(34984, 0, 0, 1, 0, 0, 0, NULL),
(35060, 0, 1, 0, 0, 0, 0, NULL),
(35061, 0, 1, 0, 0, 0, 0, NULL),
(35071, 0, 1, 0, 0, 0, 0, NULL),
(35146, 0, 0, 1, 0, 0, 0, NULL),
(35299, 1, 1, 1, 0, 0, 0, NULL),
(35347, 2, 1, 0, 0, 0, 0, NULL),
(35348, 2, 1, 0, 0, 0, 0, NULL),
(35349, 2, 1, 0, 0, 0, 0, NULL),
(35350, 2, 1, 0, 0, 0, 0, NULL),
(35351, 2, 1, 0, 0, 0, 0, NULL),
(35352, 2, 1, 0, 0, 0, 0, NULL),
(35373, 1, 1, 1, 0, 0, 0, NULL),
(35379, 0, 0, 1, 0, 0, 0, NULL),
(35380, 0, 0, 1, 0, 0, 0, NULL),
(35446, 0, 0, 1, 0, 0, 0, NULL),
(35474, 1, 1, 1, 0, 0, 0, NULL),
(35608, 0, 0, 1, 0, 0, 0, NULL),
(35845, 0, 0, 1, 0, 0, 0, NULL),
(36021, 1, 1, 1, 1, 0, 0, NULL),
(36024, 1, 1, 1, 1, 0, 0, NULL),
(36065, 1, 1, 1, 0, 0, 0, NULL),
(36066, 1, 1, 1, 0, 0, 0, NULL),
(36171, 0, 0, 1, 0, 0, 0, NULL),
(36209, 0, 0, 1, 0, 0, 0, NULL),
(36349, 0, 0, 1, 0, 0, 0, NULL),
(36350, 0, 0, 1, 0, 0, 0, NULL),
(36495, 0, 0, 1, 0, 0, 0, NULL),
(36508, 0, 0, 1, 0, 0, 0, NULL),
(36538, 1, 0, 0, 0, 0, 0, NULL),
(36551, 0, 0, 1, 0, 0, 0, NULL),
(36598, 1, 0, 1, 0, 0, 0, NULL),
(36609, 0, 0, 1, 0, 0, 0, NULL),
(36610, 0, 0, 1, 0, 0, 0, NULL),
(36640, 1, 1, 2, 0, 0, 0, NULL),
(36659, 0, 0, 1, 0, 0, 0, NULL),
(36661, 0, 0, 1, 0, 0, 0, NULL),
(36672, 1, 0, 0, 0, 0, 0, NULL),
(36715, 0, 0, 1, 0, 0, 0, NULL),
(36725, 1, 0, 0, 0, 0, 0, NULL),
(36731, 0, 0, 1, 0, 0, 0, NULL),
(36736, 0, 0, 1, 0, 0, 0, NULL),
(36790, 0, 0, 1, 0, 0, 0, NULL),
(36794, 0, 0, 1, 0, 0, 0, NULL),
(36812, 0, 1, 1, 0, 0, 0, NULL),
(36817, 0, 0, 1, 0, 0, 0, NULL),
(36838, 1, 1, 0, 0, 0, 0, NULL),
(36839, 1, 1, 0, 0, 0, 0, NULL),
(36848, 0, 0, 1, 0, 0, 0, NULL),
(36852, 1, 1, 2, 0, 0, 0, NULL),
(36853, 1, 1, 1, 0, 0, 0, NULL),
(36896, 1, 1, 1, 0, 0, 0, NULL),
(36913, 0, 0, 1, 0, 0, 0, NULL),
(36934, 0, 0, 1, 1, 0, 0, NULL),
(36939, 1, 1, 0, 0, 0, 0, NULL),
(36948, 1, 1, 0, 0, 0, 0, NULL),
(36950, 1, 1, 0, 0, 0, 0, NULL),
(36957, 1, 1, 0, 0, 0, 0, NULL),
(36960, 1, 1, 0, 0, 0, 0, NULL),
(36961, 1, 1, 0, 0, 0, 0, NULL),
(36967, 0, 0, 1, 0, 0, 0, NULL),
(36968, 1, 1, 0, 0, 0, 0, NULL),
(36969, 1, 1, 0, 0, 0, 0, NULL),
(36970, 1, 1, 0, 0, 0, 0, NULL),
(36971, 1, 1, 0, 0, 0, 0, NULL),
(36978, 1, 1, 0, 0, 0, 0, NULL),
(36982, 1, 1, 0, 0, 0, 0, NULL),
(37000, 0, 0, 1, 0, 0, 0, NULL),
(37006, 0, 0, 1, 0, 0, 0, NULL),
(37013, 0, 0, 1, 0, 0, 0, NULL),
(37014, 0, 0, 1, 0, 0, 0, NULL),
(37071, 0, 0, 1, 0, 0, 0, NULL),
(37098, 2, 1, 1, 0, 0, 0, NULL),
(37116, 1, 1, 0, 0, 0, 0, NULL),
(37117, 1, 1, 0, 0, 0, 0, NULL),
(37126, 0, 0, 1, 0, 0, 0, NULL),
(37158, 1, 1, 1, 0, 0, 0, NULL),
(37162, 0, 0, 1, 0, 0, 0, NULL),
(37169, 0, 0, 1, 0, 0, 0, NULL),
(37181, 0, 0, 1, 0, 0, 0, NULL),
(37182, 1, 1, 0, 0, 0, 0, NULL),
(37183, 0, 1, 1, 0, 0, 0, NULL),
(37186, 0, 0, 1, 0, 0, 0, NULL),
(37198, 0, 0, 1, 0, 0, 0, NULL),
(37215, 0, 0, 1, 0, 0, 0, NULL),
(37222, 0, 0, 1, 0, 0, 0, NULL),
(37227, 1, 0, 0, 0, 0, 0, NULL),
(37230, 0, 1, 1, 0, 0, 0, NULL),
(37294, 1, 0, 0, 0, 0, 0, NULL),
(37296, 1, 0, 0, 0, 0, 0, NULL),
(37330, 1, 1, 0, 0, 0, 0, NULL),
(37332, 1, 1, 0, 0, 0, 0, NULL),
(37436, 0, 0, 1, 0, 0, 0, NULL),
(37437, 0, 0, 1, 0, 0, 0, NULL),
(37438, 0, 0, 1, 0, 0, 0, NULL),
(37439, 0, 0, 1, 0, 0, 0, NULL),
(37440, 0, 0, 1, 0, 0, 0, NULL),
(37441, 0, 0, 1, 0, 0, 0, NULL),
(37488, 1, 0, 0, 0, 0, 0, NULL),
(37518, 0, 0, 1, 0, 0, 0, NULL),
(37519, 0, 0, 1, 0, 0, 0, NULL),
(37522, 0, 0, 1, 0, 0, 0, NULL),
(37528, 1, 1, 1, 0, 0, 0, NULL),
(37533, 1, 0, 1, 0, 0, 0, NULL),
(37534, 1, 0, 1, 0, 0, 0, NULL),
(37540, 0, 0, 1, 0, 0, 0, NULL),
(37543, 0, 0, 1, 0, 0, 0, NULL),
(37547, 0, 0, 1, 0, 0, 0, NULL),
(37558, 0, 0, 1, 0, 0, 0, NULL),
(37564, 0, 0, 1, 0, 0, 0, NULL),
(37574, 0, 0, 1, 0, 0, 0, NULL),
(37593, 0, 1, 1, 0, 0, 0, NULL),
(37601, 0, 0, 1, 0, 0, 0, NULL),
(37610, 0, 0, 1, 0, 0, 0, NULL),
(37611, 0, 0, 1, 0, 0, 0, NULL),
(37634, 0, 0, 1, 0, 0, 0, NULL),
(37636, 1, 1, 1, 0, 0, 0, NULL),
(37670, 0, 1, 1, 0, 0, 0, NULL),
(37690, 0, 0, 0, 0, 0, 0, NULL),
(37702, 0, 0, 1, 0, 0, 0, NULL),
(37704, 0, 0, 1, 0, 0, 0, NULL),
(37744, 0, 0, 1, 0, 0, 0, NULL),
(37746, 0, 0, 1, 0, 0, 0, NULL),
(37799, 0, 1, 1, 0, 0, 0, NULL),
(37824, 0, 0, 1, 0, 0, 0, NULL),
(37827, 0, 0, 1, 0, 0, 0, NULL),
(37832, 0, 0, 1, 0, 0, 0, NULL),
(37833, 1, 1, 0, 0, 0, 0, NULL),
(37852, 0, 0, 1, 0, 0, 0, NULL),
(37871, 0, 0, 1, 0, 0, 0, NULL),
(37878, 0, 0, 1, 0, 0, 0, NULL),
(37882, 0, 0, 1, 0, 0, 0, NULL),
(37894, 0, 0, 1, 0, 0, 0, NULL),
(37947, 0, 0, 1, 0, 0, 0, NULL),
(37948, 0, 0, 1, 0, 0, 0, NULL),
(37950, 0, 0, 1, 0, 0, 0, NULL),
(37952, 0, 0, 1, 0, 0, 0, NULL),
(37955, 1, 1, 0, 0, 0, 0, NULL),
(37964, 0, 0, 1, 1, 0, 0, NULL),
(37966, 0, 1, 1, 0, 0, 0, NULL),
(37968, 0, 0, 1, 0, 0, 0, NULL),
(37980, 0, 1, 1, 0, 0, 0, NULL),
(37981, 0, 0, 1, 0, 0, 0, NULL),
(37985, 0, 0, 1, 0, 0, 0, NULL),
(37986, 0, 0, 1, 0, 0, 0, NULL),
(38001, 0, 0, 1, 0, 0, 0, NULL),
(38003, 0, 0, 1, 0, 0, 0, NULL),
(38004, 1, 1, 1, 0, 0, 0, NULL),
(38035, 0, 0, 1, 0, 0, 0, NULL),
(38048, 0, 0, 0, 0, 0, 0, NULL),
(38058, 1, 0, 0, 0, 0, 0, NULL),
(38107, 0, 0, 1, 0, 0, 0, NULL),
(38110, 1, 1, 0, 0, 0, 0, NULL),
(38128, 0, 0, 1, 0, 0, 0, NULL),
(38129, 0, 0, 1, 0, 0, 0, NULL),
(38153, 0, 0, 1, 0, 0, 0, NULL),
(38156, 1, 1, 0, 0, 0, 0, NULL),
(38157, 1, 1, 0, 0, 0, 0, NULL),
(38159, 0, 0, 1, 0, 0, 0, NULL),
(38163, 0, 0, 1, 0, 0, 0, NULL),
(38219, 1, 0, 1, 0, 0, 0, NULL),
(38220, 1, 0, 1, 0, 0, 0, NULL),
(38223, 0, 0, 1, 0, 0, 0, NULL),
(38234, 0, 0, 1, 0, 0, 0, NULL),
(38256, 1, 1, 0, 0, 0, 0, NULL),
(38257, 1, 1, 0, 0, 0, 0, NULL),
(38258, 0, 0, 1, 0, 0, 0, NULL),
(38261, 1, 1, 0, 0, 0, 0, NULL),
(38262, 1, 1, 0, 0, 0, 0, NULL),
(38265, 1, 1, 1, 0, 0, 0, NULL),
(38266, 1, 1, 1, 0, 0, 0, NULL),
(38267, 1, 1, 1, 0, 0, 0, NULL),
(38288, 0, 0, 1, 0, 0, 0, NULL),
(38308, 0, 1, 1, 0, 0, 0, NULL),
(38309, 0, 1, 1, 0, 0, 0, NULL),
(38317, 0, 0, 1, 0, 0, 0, NULL),
(38319, 0, 0, 1, 0, 0, 0, NULL),
(38332, 0, 0, 1, 0, 0, 0, NULL),
(38340, 0, 0, 1, 0, 0, 0, NULL),
(38341, 0, 0, 1, 0, 0, 0, NULL),
(38342, 0, 0, 1, 0, 0, 0, NULL),
(38391, 2, 1, 1, 0, 0, 0, NULL),
(38392, 2, 1, 1, 0, 0, 0, NULL),
(38403, 1, 1, 0, 0, 0, 0, NULL),
(38404, 1, 1, 0, 0, 0, 0, NULL),
(38405, 1, 1, 0, 0, 0, 0, NULL),
(38406, 1, 1, 0, 0, 0, 0, NULL),
(38407, 1, 1, 0, 0, 0, 0, NULL),
(38408, 1, 1, 0, 0, 0, 0, NULL),
(38418, 2, 1, 1, 0, 0, 0, NULL),
(38421, 0, 0, 1, 0, 0, 0, NULL),
(38422, 0, 0, 1, 0, 0, 0, NULL),
(38434, 1, 1, 0, 0, 0, 0, NULL),
(38435, 1, 1, 0, 0, 0, 0, NULL),
(38436, 1, 1, 0, 0, 0, 0, NULL),
(38439, 0, 0, 1, 0, 0, 0, NULL),
(38444, 0, 1, 1, 0, 0, 0, NULL),
(38451, 0, 0, 1, 0, 0, 0, NULL),
(38454, 0, 0, 1, 0, 0, 0, NULL),
(38458, 0, 0, 1, 0, 0, 0, NULL),
(38461, 1, 1, 0, 1, 0, 0, NULL),
(38463, 0, 0, 1, 0, 0, 0, NULL),
(38556, 0, 0, 1, 0, 0, 0, NULL),
(38557, 0, 0, 1, 0, 0, 0, NULL),
(38558, 1, 1, 1, 0, 0, 0, NULL),
(38560, 0, 0, 1, 0, 0, 0, NULL),
(38569, 0, 0, 1, 0, 0, 0, NULL),
(38584, 0, 0, 1, 0, 0, 0, NULL),
(38637, 1, 1, 0, 0, 0, 0, NULL),
(38638, 1, 1, 0, 0, 0, 0, NULL),
(38639, 1, 1, 0, 0, 0, 0, NULL),
(38640, 1, 1, 0, 0, 0, 0, NULL),
(38667, 0, 0, 1, 0, 0, 0, NULL),
(38675, 1, 1, 0, 0, 0, 0, NULL),
(38676, 1, 1, 0, 0, 0, 0, NULL),
(38677, 1, 1, 0, 0, 0, 0, NULL),
(38678, 1, 1, 0, 0, 0, 0, NULL),
(38679, 1, 1, 0, 0, 0, 0, NULL),
(38680, 1, 1, 0, 0, 0, 0, NULL),
(38681, 1, 1, 0, 0, 0, 0, NULL),
(38682, 1, 1, 0, 0, 0, 0, NULL),
(38683, 1, 1, 0, 0, 0, 0, NULL),
(38684, 1, 1, 0, 0, 0, 0, NULL),
(38685, 1, 1, 0, 0, 0, 0, NULL),
(38686, 1, 1, 0, 0, 0, 0, NULL),
(38687, 1, 1, 0, 0, 0, 0, NULL),
(38688, 1, 1, 0, 0, 0, 0, NULL),
(38689, 1, 1, 0, 0, 0, 0, NULL),
(38690, 1, 1, 0, 0, 0, 0, NULL),
(38691, 1, 1, 0, 0, 0, 0, NULL),
(38692, 1, 1, 0, 0, 0, 0, NULL),
(38693, 1, 1, 0, 0, 0, 0, NULL),
(38694, 1, 1, 0, 0, 0, 0, NULL),
(38699, 0, 0, 1, 0, 0, 0, NULL),
(38700, 0, 0, 1, 0, 0, 0, NULL),
(38701, 0, 0, 1, 0, 0, 0, NULL),
(38702, 0, 0, 1, 0, 0, 0, NULL),
(38751, 1, 1, 1, 0, 0, 0, NULL),
(38752, 0, 0, 1, 0, 0, 0, NULL),
(38775, 0, 0, 1, 0, 0, 0, NULL),
(38776, 0, 0, 1, 0, 0, 0, NULL),
(38777, 0, 0, 1, 0, 0, 0, NULL),
(38806, 0, 0, 1, 0, 0, 0, NULL),
(38821, 0, 0, 1, 0, 0, 0, NULL),
(38879, 0, 0, 1, 0, 0, 0, NULL),
(38903, 0, 0, 1, 0, 0, 0, NULL),
(38917, 0, 0, 1, 0, 0, 0, NULL),
(38936, 0, 0, 1, 0, 0, 0, NULL),
(38980, 0, 0, 1, 0, 0, 0, NULL),
(38981, 0, 0, 1, 0, 0, 0, NULL),
(39010, 0, 0, 1, 0, 0, 0, NULL),
(39011, 0, 0, 1, 0, 0, 0, NULL),
(39012, 0, 0, 1, 0, 0, 0, NULL),
(39013, 0, 0, 1, 0, 0, 0, NULL),
(39048, 0, 0, 1, 0, 0, 0, NULL),
(39077, 1, 0, 0, 0, 0, 0, NULL),
(39093, 0, 0, 1, 0, 0, 0, NULL),
(39120, 0, 0, 1, 0, 0, 0, NULL),
(39121, 0, 0, 1, 0, 0, 0, NULL),
(39122, 0, 0, 1, 0, 0, 0, NULL),
(39140, 0, 0, 1, 0, 0, 0, NULL),
(39189, 0, 0, 1, 0, 0, 0, NULL),
(39190, 0, 1, 1, 0, 0, 0, NULL),
(39284, 0, 1, 1, 0, 0, 0, NULL),
(39285, 0, 1, 1, 0, 0, 0, NULL),
(39286, 0, 1, 1, 0, 0, 0, NULL),
(39287, 0, 1, 1, 0, 0, 0, NULL),
(39288, 0, 1, 1, 0, 0, 0, NULL),
(39289, 0, 1, 1, 0, 0, 0, NULL),
(39390, 0, 0, 1, 0, 0, 0, NULL),
(39420, 1, 0, 1, 0, 0, 0, NULL),
(39446, 1, 0, 0, 0, 0, 0, NULL),
(39600, 0, 0, 1, 0, 0, 0, NULL),
(39677, 0, 0, 1, 0, 0, 0, NULL),
(39747, 2, 1, 1, 0, 0, 0, NULL),
(39823, 2, 1, 1, 0, 0, 0, NULL),
(39842, 0, 0, 1, 0, 0, 0, NULL),
(39859, 0, 0, 1, 0, 0, 0, NULL),
(39867, 0, 0, 1, 0, 0, 0, NULL),
(40081, 0, 0, 1, 0, 0, 0, NULL),
(40083, 1, 1, 1, 0, 0, 0, NULL),
(40100, 1, 1, 1, 0, 0, 0, NULL),
(40146, 0, 0, 1, 0, 0, 0, NULL),
(40160, 0, 1, 1, 0, 0, 0, NULL),
(40164, 0, 0, 1, 0, 0, 0, NULL),
(40178, 0, 0, 1, 0, 0, 0, NULL),
(40183, 0, 0, 1, 0, 0, 0, NULL),
(40190, 0, 0, 1, 0, 0, 0, NULL),
(40197, 0, 0, 1, 0, 0, 0, NULL),
(40240, 0, 0, 1, 0, 0, 0, NULL),
(40246, 0, 0, 1, 0, 0, 0, NULL),
(40250, 0, 0, 1, 0, 0, 0, NULL),
(40343, 0, 0, 1, 0, 0, 0, NULL),
(40350, 0, 0, 1, 0, 0, 0, NULL),
(40363, 1, 0, 1, 0, 0, 0, NULL),
(40416, 1, 1, 0, 0, 0, 0, NULL),
(40435, 2, 0, 0, 0, 0, 0, NULL),
(40468, 1, 1, 1, 0, 0, 0, NULL),
(40469, 1, 1, 1, 0, 0, 0, NULL),
(40470, 0, 0, 1, 0, 0, 0, NULL),
(40471, 0, 0, 1, 0, 0, 0, NULL),
(40472, 0, 0, 1, 0, 0, 0, NULL),
(40650, 0, 0, 1, 0, 0, 0, NULL),
(40789, 0, 0, 1, 0, 0, 0, NULL),
(41095, 0, 0, 1, 0, 0, 0, NULL),
(41178, 0, 0, 1, 0, 0, 0, NULL),
(41194, 0, 0, 1, 0, 0, 0, NULL),
(41266, 0, 0, 1, 0, 0, 0, NULL),
(41505, 0, 0, 1, 0, 0, 0, NULL),
(41570, 0, 0, 1, 0, 0, 0, NULL),
(41761, 0, 0, 1, 0, 0, 0, NULL),
(41767, 0, 0, 1, 0, 0, 0, NULL),
(42098, 0, 0, 1, 0, 0, 0, NULL),
(42186, 0, 0, 1, 0, 0, 0, NULL),
(42333, 1, 1, 2, 0, 0, 0, NULL),
(42347, 0, 0, 1, 0, 0, 0, NULL),
(42452, 0, 0, 1, 0, 0, 0, NULL),
(42495, 0, 0, 1, 0, 0, 0, NULL),
(42505, 0, 0, 1, 0, 0, 0, NULL),
(42557, 0, 0, 1, 0, 0, 0, NULL),
(42608, 0, 0, 1, 0, 0, 0, NULL),
(42716, 0, 0, 1, 0, 0, 0, NULL),
(42728, 0, 0, 1, 1, 0, 0, NULL),
(42764, 0, 0, 1, 0, 0, 0, NULL),
(42767, 0, 0, 1, 0, 0, 0, NULL),
(42768, 0, 0, 1, 0, 0, 0, NULL),
(42894, 0, 0, 1, 0, 0, 0, NULL),
(43044, 0, 0, 1, 1, 0, 0, NULL),
(43046, 0, 0, 1, 0, 0, 0, NULL),
(43070, 0, 0, 1, 0, 0, 0, NULL),
(43150, 0, 0, 1, 0, 0, 0, NULL),
(43159, 1, 1, 2, 0, 0, 0, NULL),
(43161, 0, 0, 1, 0, 0, 0, NULL),
(43214, 1, 1, 2, 0, 0, 0, NULL),
(43235, 0, 0, 1, 0, 0, 0, NULL),
(43422, 0, 0, 1, 0, 0, 0, NULL),
(43583, 0, 0, 1, 0, 0, 0, NULL),
(43718, 0, 0, 1, 0, 0, 0, NULL),
(43999, 0, 0, 1, 1, 0, 0, NULL),
(44057, 0, 0, 1, 0, 0, 0, NULL),
(44586, 0, 0, 1, 0, NULL, NULL, NULL),
(44592, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(44593, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(44608, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(44609, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(44610, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(44687, 0, 0, 1, 0, 0, 0, NULL),
(44711, 0, 0, 1, 0, NULL, NULL, NULL),
(44712, 0, 0, 1, 1, NULL, NULL, NULL),
(44731, 1, 0, 0, 0, NULL, 0, NULL),
(44775, 0, 0, 1, 0, 0, 0, NULL),
(44796, 0, 0, 1, 0, NULL, NULL, NULL),
(44798, 0, 0, 1, 1, NULL, NULL, NULL),
(44821, 1, 0, 1, 0, NULL, 0, NULL),
(44840, 0, 0, 1, 1, NULL, NULL, NULL),
(44880, 0, 0, 1, 0, 0, 0, NULL),
(44908, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(44935, 1, 1, 0, 0, 0, 0, NULL),
(44951, 2, 1, 1, 0, 0, 0, NULL),
(45088, 0, 0, 1, 1, 0, 0, NULL),
(45213, 1, 1, 0, 1, 0, 0, NULL),
(45224, 0, 0, 1, 0, 0, 0, NULL),
(45256, 0, 0, 1, 0, 0, 0, NULL),
(45321, 1, 0, 0, 0, 0, 0, NULL),
(45365, 0, 0, 1, 0, 0, 0, NULL),
(45396, 0, 0, 1, 0, 0, 0, NULL),
(45406, 0, 0, 1, 0, 0, 0, NULL),
(45475, 0, 0, 1, 0, 0, 0, NULL),
(45492, 1, 1, 2, 0, 0, 0, NULL),
(45503, 0, 0, 1, 0, 0, 0, NULL),
(45561, 1, 1, 2, 0, 0, 0, NULL),
(45685, 0, 0, 1, 0, 0, 0, NULL),
(45748, 0, 0, 1, 0, 0, 0, NULL),
(45788, 0, 0, 1, 0, 0, 0, NULL),
(45979, 0, 0, 1, 0, 0, 0, NULL),
(46012, 0, 0, 1, 0, 0, 0, NULL),
(46023, 0, 0, 1, 0, 0, 0, NULL),
(46074, 0, 0, 1, 0, 0, 0, NULL),
(46085, 0, 0, 1, 0, 0, 0, NULL),
(46107, 0, 0, 1, 0, 0, 0, NULL),
(46134, 1, 0, 0, 0, 0, 0, NULL),
(46141, 0, 0, 1, 0, 0, 0, NULL),
(46162, 0, 0, 1, 0, 0, 0, NULL),
(46165, 0, 0, 1, 0, 0, 0, NULL),
(46265, 0, 0, 1, 0, 0, 0, NULL),
(46296, 0, 0, 1, 0, 0, 0, NULL),
(46315, 1, 0, 0, 0, 0, 0, NULL),
(46317, 1, 0, 0, 0, 0, 0, NULL),
(46325, 1, 0, 0, 0, 0, 0, NULL),
(46340, 1, 0, 0, 0, 0, 0, NULL),
(46402, 1, 0, 0, 0, 0, 0, NULL),
(46404, 0, 0, 1, 0, 0, 0, NULL),
(46416, 0, 0, 1, 0, 0, 0, NULL),
(46418, 0, 0, 1, 0, 0, 0, NULL),
(46419, 0, 0, 1, 0, 0, 0, NULL),
(46835, 0, 0, 1, 0, 0, 0, NULL),
(46985, 0, 0, 1, 0, 0, 0, NULL),
(47051, 1, 0, 0, 0, 0, 0, NULL),
(47186, 0, 0, 1, 0, 0, 0, NULL),
(47196, 0, 0, 1, 0, 0, 0, NULL),
(47308, 0, 0, 1, 0, 0, 0, NULL),
(47330, 0, 0, 1, 0, 0, 0, NULL),
(47380, 0, 0, 1, 0, 0, 0, NULL),
(47391, 0, 0, 1, 0, 0, 0, NULL),
(47476, 0, 0, 1, 0, 0, 0, NULL),
(47491, 0, 0, 1, 0, 0, 0, NULL),
(47539, 0, 0, 1, 0, 0, 0, NULL),
(47594, 1, 0, 0, 0, 0, 0, NULL),
(47606, 0, 0, 1, 0, 0, 0, NULL),
(47649, NULL, NULL, 1, 1, NULL, NULL, NULL),
(47680, 0, 0, 1, 0, 0, 0, NULL),
(47684, 1, 0, 0, 0, 0, 0, NULL),
(47780, 0, 0, 1, 0, 0, 0, NULL),
(47799, 0, 0, 1, 0, 0, 0, NULL),
(47906, 0, 0, 1, 0, 0, 0, NULL),
(47958, 0, 0, 1, 0, 0, 0, NULL),
(47988, 1, 0, 0, 0, 0, 0, NULL),
(48157, 0, 0, 1, 0, 0, 0, NULL),
(48239, 1, 0, 0, 0, 0, 0, NULL),
(48240, 0, 0, 1, 0, 0, 0, NULL),
(48310, 0, 0, 1, 0, 0, 0, NULL),
(48447, 0, 0, 1, 0, 0, 0, NULL),
(48450, 0, 0, 1, 0, 0, 0, NULL),
(48454, 0, 0, 1, 0, 0, 0, NULL),
(48476, 0, 0, 1, 0, 0, 0, NULL),
(48518, 1, 0, 0, 0, 0, 0, NULL),
(48523, 0, 0, 1, 0, 0, 0, NULL),
(48617, 1, 0, 0, 0, 0, 0, NULL),
(48623, 0, 0, 1, 0, 0, 0, NULL),
(48624, 0, 0, 1, 0, 0, 0, NULL),
(48681, 0, 0, 1, 0, 0, 0, NULL),
(48755, 0, 0, 1, 0, 0, 0, NULL),
(48781, 0, 0, 1, 0, 0, 0, NULL),
(48782, 0, 0, 1, 0, 0, 0, NULL),
(48839, 0, 0, 1, 0, 0, 0, NULL),
(48885, 1, 0, 0, 0, 0, 0, NULL),
(48887, 1, 0, 0, 0, 0, 0, NULL),
(48964, 0, 0, 1, 0, 0, 0, NULL),
(49060, 0, 0, 1, 0, 0, 0, NULL),
(49129, 0, 0, 1, 0, 0, 0, NULL),
(49144, 0, 0, 1, 0, 0, 0, NULL),
(49155, 1, 0, 2, 0, 0, 0, NULL),
(49160, 1, 0, 2, 0, 0, 0, NULL),
(49182, 0, 0, 1, 0, 0, 0, NULL),
(49198, 0, 0, 1, 0, 0, 0, NULL),
(49259, 0, 0, 1, 1, 0, 0, NULL),
(49284, 0, 0, 1, 0, 0, 0, NULL),
(49380, 0, 0, 1, 0, 0, 0, NULL),
(49381, 0, 0, 1, 0, 0, 0, NULL),
(49457, 0, 0, 1, 0, 0, 0, NULL),
(49538, 1, 1, 2, 0, 0, 0, NULL),
(49624, 1, 1, 2, 0, 0, 0, NULL),
(49628, 0, 0, 1, 0, 0, 0, NULL),
(49631, 0, 0, 1, 0, 0, 0, NULL),
(49683, 0, 0, 1, 0, 0, 0, NULL),
(49801, 0, 0, 1, 0, 0, 0, NULL),
(49842, 0, 0, 1, 0, 0, 0, NULL),
(49872, 0, 0, 1, 0, 0, 0, NULL),
(49873, 0, 0, 1, 0, 0, 0, NULL),
(50003, 0, 0, 1, 0, 0, 0, NULL),
(50063, 1, 0, 0, 0, 0, 0, NULL),
(50268, 0, 0, 1, 0, 0, 0, NULL),
(50282, 0, 0, 1, 0, 0, 0, NULL),
(50300, 0, 0, 1, 0, 0, 0, NULL),
(50328, 0, 0, 1, 0, 0, 0, NULL),
(50473, 0, 0, 1, 0, 0, 0, NULL),
(50803, 0, 0, 1, 0, 0, 0, NULL),
(51089, 0, 0, 1, 0, 0, 0, NULL),
(51101, 0, 0, 1, 0, 0, 0, NULL),
(51102, 0, 0, 1, 0, 0, 0, NULL),
(51103, 0, 0, 1, 0, 0, 0, NULL),
(51125, 0, 0, 1, 0, 0, 0, NULL),
(51248, 0, 0, 1, 0, 0, 0, NULL),
(51249, 0, 0, 1, 0, 0, 0, NULL),
(51250, 0, 0, 1, 0, 0, 0, NULL),
(51346, 0, 0, 1, 0, 0, 0, NULL),
(51348, 0, 0, 1, 0, 0, 0, NULL),
(51371, 0, 0, 1, 0, 0, 0, NULL),
(51372, 0, 0, 1, 0, 0, 0, NULL),
(51383, 0, 0, 1, 0, 0, 0, NULL),
(51684, 0, 0, 1, 1, 0, 0, NULL),
(51702, 0, 0, 1, 1, 0, 0, NULL),
(51703, 0, 0, 1, 1, 0, 0, NULL),
(51705, 0, 0, 1, 1, 0, 0, NULL),
(51710, 0, 0, 1, 0, 0, 0, NULL),
(51712, 0, 0, 1, 0, 0, 0, NULL),
(51720, 0, 0, 1, 1, 0, 0, NULL),
(51865, 0, 0, 1, 0, 0, 0, NULL),
(52054, 0, 0, 1, 0, 0, 0, NULL),
(52067, 0, 0, 1, 0, 0, 0, NULL),
(52150, 0, 0, 1, 0, 0, 0, NULL),
(52347, 0, 0, 1, 0, 0, 0, NULL),
(52377, 0, 0, 1, 0, 0, 0, NULL),
(52379, 0, 0, 1, 0, 0, 0, NULL),
(52419, 0, 0, 1, 0, 0, 0, NULL),
(52442, 0, 0, 1, 0, 0, 0, NULL),
(53102, 0, 0, 1, 0, 0, 0, NULL),
(53369, 0, 0, 1, 0, 0, 0, NULL),
(53396, 0, 0, 1, 0, 0, 0, NULL),
(53474, 0, 0, 1, 0, 0, 0, NULL),
(53488, 0, 0, 1, 0, 0, 0, NULL),
(53550, 0, 0, 1, 0, 0, 0, NULL),
(53648, 0, 0, 1, 0, 0, 0, NULL),
(53734, 0, 0, 1, 0, 0, 0, NULL),
(53761, 0, 0, 1, 0, 0, 0, NULL),
(53896, 0, 0, 1, 0, 0, 0, NULL),
(53900, 0, 0, 1, 0, 0, 0, NULL),
(53966, 0, 0, 1, 0, 0, 0, NULL),
(53986, 0, 0, 1, 0, 0, 0, NULL),
(54020, 0, 0, 1, 0, 0, 0, NULL),
(54043, 0, 0, 1, 0, 0, 0, NULL),
(54066, 0, 0, 1, 0, 0, 0, NULL),
(54068, 0, 0, 1, 0, 0, 0, NULL),
(54116, 0, 0, 1, 0, 0, 0, NULL),
(54432, 0, 0, 1, 0, 0, 0, NULL),
(54517, 0, 0, 1, 0, 0, 0, NULL),
(54519, 0, 0, 1, 1, 0, 0, NULL),
(54523, 0, 0, 1, 1, 0, 0, NULL),
(54567, NULL, NULL, 1, 1, NULL, NULL, NULL),
(54638, 1, 1, 1, 0, 0, 0, NULL),
(54927, 0, 0, 1, 0, 0, 0, NULL),
(54945, 0, 0, 1, 0, 0, 0, NULL),
(55078, 0, 0, 1, 0, 0, 0, NULL),
(55366, 0, 0, 1, 0, 0, 0, NULL),
(55390, 0, 0, 1, 0, 0, 0, NULL),
(55391, 0, 0, 1, 0, 0, 0, NULL),
(55700, 0, 0, 1, 0, 0, 0, NULL),
(55739, 0, 0, 1, 0, 0, 0, NULL),
(55911, 0, 0, 1, 0, 0, 0, NULL),
(55912, 0, 0, 1, 0, 0, 0, NULL),
(55967, 0, 0, 1, 0, 0, 0, NULL),
(55969, 0, 0, 1, 0, 0, 0, NULL),
(56087, 0, 0, 1, 0, 0, 0, NULL),
(56139, 0, 0, 1, 0, 0, 0, NULL),
(56140, 0, 0, 1, 0, 0, 0, NULL),
(56141, 0, 0, 1, 0, 0, 0, NULL),
(56143, 0, 0, 1, 0, 0, 0, NULL),
(56144, 0, 0, 1, 0, 0, 0, NULL),
(57458, 0, 0, 1, 0, 0, 0, NULL),
(57864, 0, 0, 1, 0, 0, 0, NULL),
(58664, 0, 0, 1, 1, 0, 0, NULL),
(58875, 0, 0, 1, 0, 0, 0, NULL),
(59200, 0, 0, 1, 1, 0, 0, NULL),
(59722, 0, 0, 0, 1, 0, 0, NULL),
(59738, 1, 1, 1, 1, 0, 0, NULL),
(60033, 1, 0, 1, 0, 0, 0, NULL),
(60197, 0, 0, 0, 1, 0, 0, NULL),
(61829, 0, 0, 1, 0, 0, 0, NULL),
(62242, 0, 0, 1, 0, 0, 0, NULL),
(62450, 0, 0, 1, 0, 0, 0, NULL),
(63078, 2, 0, 0, 0, 0, 0, NULL),
(63079, 2, 0, 0, 0, 0, 0, NULL),
(63084, 2, 0, 0, 0, 0, 0, NULL),
(65097, 0, 0, 1, 0, 0, 0, NULL),
(67329, 0, 0, 1, 0, 0, 0, NULL),
(67384, 0, 0, 1, 0, 0, 0, NULL),
(81406, 0, 1, 0, 0, 0, 0, NULL),
(81408, 0, 1, 0, 0, 0, 0, NULL),
(81695, 0, 0, 1, 0, 0, 0, NULL),
(81696, 0, 0, 1, 0, 0, 0, NULL),
(81907, 0, 0, 1, 0, 0, 0, NULL),
(82165, 0, 0, 1, 0, 0, 0, NULL),
(82175, 1, 0, 1, 0, 0, 0, NULL),
(82647, 0, 0, 1, 0, 0, 0, NULL),
(85525, 0, 0, 1, 0, 0, 0, NULL),
(85773, 0, 0, 1, 0, 0, 0, NULL),
(88206, 0, 0, 3, 1, 0, 0, NULL),
(90546, 0, 0, 1, 0, 0, 0, NULL),
(90578, 0, 0, 1, 0, 0, 0, NULL),
(91074, 1, 0, 1, 0, 0, 0, NULL),
(93105, 0, 0, 1, 0, 0, 0, NULL),
(93556, 0, 0, 1, 0, 0, 0, NULL),
(93819, 0, 1, 1, 0, 0, 0, NULL),
(93823, 0, 1, 1, 0, 0, 0, NULL),
(94654, 2, 0, 0, 0, 0, 0, NULL),
(95049, 0, 0, 1, 1, 0, 0, NULL),
(96568, 0, 0, 1, 0, 0, 0, NULL),
(96648, 0, 0, 1, 0, 0, 0, NULL),
(96765, 0, 0, 1, 1, 0, 0, NULL),
(97018, 0, 0, 1, 0, 0, 0, NULL),
(97068, 0, 0, 1, 0, 0, 0, NULL),
(97163, 0, 0, 1, 0, 0, 0, NULL),
(97229, 0, 0, 0, 0, 0, 0, NULL),
(98497, 0, 0, 1, 1, 0, 0, NULL),
(98905, 0, 0, 1, 0, 0, 0, NULL),
(98907, 0, 0, 1, 0, 0, 0, NULL),
(99160, 0, 0, 1, 1, 0, 0, NULL),
(99891, 0, 0, 1, 0, 0, 0, NULL),
(99914, 0, 0, 1, 1, 0, 0, NULL),
(101704, 0, 0, 1, 0, 0, 0, NULL),
(101748, 0, 0, 1, 1, 0, 0, NULL),
(102326, 0, 0, 1, 0, 0, 0, NULL),
(103115, 0, 0, 1, 0, 0, 0, NULL),
(103135, NULL, NULL, 1, NULL, NULL, NULL, NULL),
(104906, 0, 0, 1, 0, 0, 0, NULL),
(105316, 0, 0, 1, 1, 0, 0, NULL),
(105838, 1, 1, 0, 0, 0, 0, NULL);
